//==========================================================================
//
//      net/net_io.c
//
//      Stand-alone network logical I/O support for RedBoot
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-07-14
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <redboot.h>
#include <eth_drv.h>            // Logical driver interfaces
#include <net/net.h>
#include <cyg/hal/hal_misc.h>   // Helper functions
#include <cyg/hal/hal_if.h>     // HAL I/O interfaces
#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_intr.h>

#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
#include <flash_config.h>

RedBoot_config_option("GDB connection port",
                      gdb_port,
                      ALWAYS_ENABLED, true,
                      CONFIG_INT,
                      CYGNUM_REDBOOT_NETWORKING_TCP_PORT
    );
RedBoot_config_option("Network debug at boot time", 
                      net_debug, 
                      ALWAYS_ENABLED, true,
                      CONFIG_BOOL,
                      false
    );
// Note: the following options are related.  If 'bootp' is false, then
// the other values are used in the configuration.  Because of the way
// that configuration tables are generated, they should have names which
// are related.  The configuration options will show up lexicographically
// ordered, thus the peculiar naming.  In this case, the 'use' option is
// negated (if false, the others apply) which makes the names even more
// confusing.
RedBoot_config_option("Use BOOTP for network configuration",
                      bootp, 
                      ALWAYS_ENABLED, true,
                      CONFIG_BOOL,
                      true
    );
RedBoot_config_option("Local IP address",
                      bootp_my_ip,
                      "bootp", false,
                      CONFIG_IP,
                      0
    );
RedBoot_config_option("Default server IP address",
                      bootp_server_ip,
                      "bootp", false,
                      CONFIG_IP,
                      0
    );
#endif

#define TCP_CHANNEL CYGNUM_HAL_VIRTUAL_VECTOR_COMM_CHANNELS

#ifdef DEBUG_TCP
int show_tcp = 0;
#endif 

static tcp_socket_t tcp_sock;
static int state;
static int _timeout = 500;
static int orig_console, orig_debug;

static int in_buflen = 0;
static unsigned char in_buf[64];
static unsigned char *in_bufp;
static int out_buflen = 0;
static unsigned char out_buf[64];
static unsigned char *out_bufp;

// Functions in this module
static void net_io_flush(void);
static void net_io_revert_console(void);
static void net_io_putc(void*, cyg_uint8);

// Special characters used by Telnet - must be interpretted here
#define TELNET_IAC    0xFF // Interpret as command (escape)
#define TELNET_IP     0xF4 // Interrupt process
#define TELNET_WONT   0xFC // I Won't do it
#define TELNET_DO     0xFD // Will you XXX
#define TELNET_TM     0x06 // Time marker (special DO/WONT after IP)

static cyg_bool
_net_io_getc_nonblock(void* __ch_data, cyg_uint8* ch)
{
    if (in_buflen == 0) {
        __tcp_poll();
        if (tcp_sock.state == _CLOSE_WAIT) {
            // This connection is breaking
            if (tcp_sock.data_bytes == 0) {
                __tcp_close(&tcp_sock);
            }
            return false;
        }
        if (tcp_sock.state == _CLOSED) {
            // The connection is gone
            net_io_revert_console();
            *ch = '\n';
            return true;
        }
        in_buflen = __tcp_read(&tcp_sock, in_buf, sizeof(in_buf));
        in_bufp = in_buf;
#ifdef DEBUG_TCP
        if (show_tcp && (in_buflen > 0)) {
            int old_console;
            old_console = start_console();  
            printf("%s:%d\n", __FUNCTION__, __LINE__);  
            dump_buf(in_buf, in_buflen);  
            end_console(old_console);
        }
#endif // DEBUG_TCP
    }
    if (in_buflen) {
        *ch = *in_bufp++;
        in_buflen--;
        return true;
    } else {
        return false;
    }
}

static cyg_bool
net_io_getc_nonblock(void* __ch_data, cyg_uint8* ch)
{
    cyg_uint8 esc;

    if (!_net_io_getc_nonblock(__ch_data, ch))
        return false;

    if (gdb_active || *ch != TELNET_IAC)
        return true;

    // Telnet escape - need to read/handle more
    while (!_net_io_getc_nonblock(__ch_data, &esc)) ;

    switch (esc) {
    case TELNET_IAC:
        // The other special case - escaped escape
        return true;
    case TELNET_IP:
        // Special case for ^C == Interrupt Process
        *ch = 0x03;  
        // Just in case the other end needs synchronizing
        net_io_putc(__ch_data, TELNET_IAC);
        net_io_putc(__ch_data, TELNET_WONT);
        net_io_putc(__ch_data, TELNET_TM);
        net_io_flush();
        return true;
    case TELNET_DO:
        // Telnet DO option
        while (!_net_io_getc_nonblock(__ch_data, &esc)) ;                
        // Respond with WONT option
        net_io_putc(__ch_data, TELNET_IAC);
        net_io_putc(__ch_data, TELNET_WONT);
        net_io_putc(__ch_data, esc);
        return false;  // Ignore this whole thing!
    default:
        return false;
    }
}

static cyg_uint8
net_io_getc(void* __ch_data)
{
    cyg_uint8 ch;
    int idle_timeout = 10;  // 10ms

    CYGARC_HAL_SAVE_GP();
    while (true) {
        if (net_io_getc_nonblock(__ch_data, &ch)) break;
        if (--idle_timeout == 0) {
            net_io_flush();
            idle_timeout = 10;
        } else {
            MS_TICKS_DELAY();
        }
    }
    CYGARC_HAL_RESTORE_GP();
    return ch;
}

static void
net_io_flush(void)
{
    int n;
    char *bp = out_buf;

    while (out_buflen) {
        if (tcp_sock.state == _CLOSE_WAIT) {
            // This connection is tring to close
            __tcp_close(&tcp_sock);
        }
        if (tcp_sock.state == _CLOSED) {
            // The connection is gone!
            net_io_revert_console();
            break;
        }
#ifdef DEBUG_TCP
        if (show_tcp) {
            int old_console;
            old_console = start_console();  
            printf("%s.%d\n", __FUNCTION__, __LINE__);
            dump_buf(out_buf, out_buflen);  
            end_console(old_console);
        }
#endif // SHOW_TCP
        n = __tcp_write(&tcp_sock, bp, out_buflen);
        if (n > 0) {
            out_buflen -= n;
            bp += n;
        }
        __tcp_poll();
    }
    out_bufp = out_buf;  out_buflen = 0;
    __tcp_drain(&tcp_sock);
    // Check interrupt flag
    if (CYGACC_CALL_IF_CONSOLE_INTERRUPT_FLAG()) {
        CYGACC_CALL_IF_CONSOLE_INTERRUPT_FLAG_SET(0);
        cyg_hal_user_break(0);
    }
}

static void
net_io_putc(void* __ch_data, cyg_uint8 c)
{
    static bool have_dollar, have_hash;
    static int hash_count;

    CYGARC_HAL_SAVE_GP();
    *out_bufp++ = c;
    if (c == '$') have_dollar = true;
    if (have_dollar && (c == '#')) {
        have_hash = true;
        hash_count = 0;
    }
    if ((++out_buflen == sizeof(out_buf)) || (c == '\n') ||
        (have_hash && (++hash_count == 3))) {
        net_io_flush();
        have_dollar = false;
    }
    CYGARC_HAL_RESTORE_GP();
}

static void
net_io_write(void* __ch_data, const cyg_uint8* __buf, cyg_uint32 __len)
{
    int old_console;

    old_console = start_console();
    printf("%s.%d\n", __FUNCTION__, __LINE__);
    end_console(old_console);
#if 0
    CYGARC_HAL_SAVE_GP();

    while(__len-- > 0)
        net_io_putc(__ch_data, *__buf++);

    CYGARC_HAL_RESTORE_GP();
#endif
}

static void
net_io_read(void* __ch_data, cyg_uint8* __buf, cyg_uint32 __len)
{
    int old_console;

    old_console = start_console();
    printf("%s.%d\n", __FUNCTION__, __LINE__);
    end_console(old_console);
#if 0
    CYGARC_HAL_SAVE_GP();

    while(__len-- > 0)
        *__buf++ = net_io_getc(__ch_data);

    CYGARC_HAL_RESTORE_GP();
#endif
}

static cyg_bool
net_io_getc_timeout(void* __ch_data, cyg_uint8* ch)
{
    int delay_count;
    cyg_bool res;

    CYGARC_HAL_SAVE_GP();
    net_io_flush();  // Make sure any output has been sent
    delay_count = _timeout;

    for(;;) {
        res = net_io_getc_nonblock(__ch_data, ch);
        if (res || 0 == delay_count--)
            break;
        MS_TICKS_DELAY();
    }

    CYGARC_HAL_RESTORE_GP();

    return res;
}

static int
net_io_control(void *__ch_data, __comm_control_cmd_t __func, ...)
{
    static int vector = 0;
    int ret = 0;
    static int irq_state = 0;

    CYGARC_HAL_SAVE_GP();

    switch (__func) {
    case __COMMCTL_IRQ_ENABLE:
        irq_state = 1;
        if (vector == 0) {
            vector = eth_drv_int_vector();
        }
        HAL_INTERRUPT_UNMASK(vector); 
        break;
    case __COMMCTL_IRQ_DISABLE:
        ret = irq_state;
        irq_state = 0;
        if (vector == 0) {
            vector = eth_drv_int_vector();
        }
        HAL_INTERRUPT_MASK(vector);
        break;
    case __COMMCTL_DBG_ISR_VECTOR:
        ret = vector;
        break;
    case __COMMCTL_SET_TIMEOUT:
    {
        va_list ap;

        va_start(ap, __func);

        ret = _timeout;
        _timeout = va_arg(ap, cyg_uint32);

        va_end(ap);
	break;
    }
    case __COMMCTL_FLUSH_OUTPUT:
        net_io_flush();
	break;
    default:
        break;
    }
    CYGARC_HAL_RESTORE_GP();
    return ret;
}

static int
net_io_isr(void *__ch_data, int* __ctrlc, 
           CYG_ADDRWORD __vector, CYG_ADDRWORD __data)
{
    char ch;

    CYGARC_HAL_SAVE_GP();
    *__ctrlc = 0;
    if (net_io_getc_nonblock(__ch_data, &ch)) {
        if (ch == 0x03) {
            *__ctrlc = 1;
        }
    }
    CYGARC_HAL_RESTORE_GP();
    return CYG_ISR_HANDLED;
}

// TEMP

int 
start_console(void)
{
    int cur_console;
    cur_console = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);
    CYGACC_CALL_IF_SET_CONSOLE_COMM(0);
    return cur_console;
}

void
end_console(int old_console)
{
    // Restore original console
    CYGACC_CALL_IF_SET_CONSOLE_COMM(old_console);
}
// TEMP

static void
net_io_revert_console(void)
{
#ifdef CYGPKG_REDBOOT_ANY_CONSOLE
    console_selected = false;
#endif
    CYGACC_CALL_IF_SET_CONSOLE_COMM(orig_console);
    CYGACC_CALL_IF_SET_DEBUG_COMM(orig_debug);
    console_echo = true;
}

static void
net_io_assume_console(void)
{
#ifdef CYGPKG_REDBOOT_ANY_CONSOLE
    console_selected = true;
#endif
    console_echo = false;
    orig_console = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);
    CYGACC_CALL_IF_SET_CONSOLE_COMM(TCP_CHANNEL);
    orig_debug = CYGACC_CALL_IF_SET_DEBUG_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);
    CYGACC_CALL_IF_SET_DEBUG_COMM(TCP_CHANNEL);
}

static void
net_io_init(void)
{
    static int init = 0;
    if (!init) {
        hal_virtual_comm_table_t* comm;
        int cur = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);

        // Setup procs in the vector table
        CYGACC_CALL_IF_SET_CONSOLE_COMM(TCP_CHANNEL);
        comm = CYGACC_CALL_IF_CONSOLE_PROCS();
        //CYGACC_COMM_IF_CH_DATA_SET(*comm, chan);
        CYGACC_COMM_IF_WRITE_SET(*comm, net_io_write);
        CYGACC_COMM_IF_READ_SET(*comm, net_io_read);
        CYGACC_COMM_IF_PUTC_SET(*comm, net_io_putc);
        CYGACC_COMM_IF_GETC_SET(*comm, net_io_getc);
        CYGACC_COMM_IF_CONTROL_SET(*comm, net_io_control);
        CYGACC_COMM_IF_DBG_ISR_SET(*comm, net_io_isr);
        CYGACC_COMM_IF_GETC_TIMEOUT_SET(*comm, net_io_getc_timeout);

        // Disable interrupts via this interface to set static
        // state into correct state.
        net_io_control( comm, __COMMCTL_IRQ_DISABLE );
        
        // Restore original console
        CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);

        init = 1;
        gdb_active = false;
    }
    __tcp_listen(&tcp_sock, gdb_port);
    state = tcp_sock.state; 
#ifdef DEBUG_TCP
    printf("show tcp = %p\n", (void *)&show_tcp);
#endif
}

// Check for incoming TCP debug connection
void
net_io_test(bool is_idle)
{
    if (!is_idle) return;  // Only care about idle case
    if (!have_net) return;
    __tcp_poll();
    if (state != tcp_sock.state) {
        // Something has changed
        if (tcp_sock.state == _ESTABLISHED) {
            // A new connection has arrived
            net_io_assume_console();
            in_bufp = in_buf;  in_buflen = 1;  *in_bufp = '\r';
            out_bufp = out_buf;  out_buflen = 0;
        }
        if (tcp_sock.state == _CLOSED) {
            net_io_init();  // Get ready for another connection
        }
    }
    state = tcp_sock.state;
}

// This schedules the 'net_io_test()' function to be run by RedBoot's
// main command loop when idle (i.e. when no input arrives after some
// period of time).
RedBoot_idle(net_io_test, RedBoot_IDLE_NETIO);

//
// Network initialization
//
#include <eth_drv.h>
#include <netdev.h>
#include <cyg/hal/hal_tables.h>

// Define table boundaries
CYG_HAL_TABLE_BEGIN( __NETDEVTAB__, netdev );
CYG_HAL_TABLE_END( __NETDEVTAB_END__, netdev );

RedBoot_init(net_init, RedBoot_INIT_LAST);

void
net_init(void)
{
    cyg_netdevtab_entry_t *t;

    // Set defaults as appropriate
    use_bootp = true;
    net_debug = false;
    gdb_port = CYGNUM_REDBOOT_NETWORKING_TCP_PORT;
#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
    // Fetch values from saved config data, if available
    flash_get_config("net_debug", &net_debug, CONFIG_BOOL);
    flash_get_config("gdb_port", &gdb_port, CONFIG_INT);
    flash_get_config("bootp", &use_bootp, CONFIG_BOOL);
    {
        ip_addr_t bootp_my_ip;
        char i;
        flash_get_config("bootp_my_ip", &bootp_my_ip, CONFIG_IP);
        if (bootp_my_ip[0] != 0 || bootp_my_ip[1] != 0 ||
            bootp_my_ip[2] != 0 || bootp_my_ip[3] != 0) {
            // bootp_my_ip is set to something so let it override any static IP
            for (i=0; i<4; i++)
                __local_ip_addr[i] = bootp_my_ip[i];
        }        
    }
    flash_get_config("bootp_server_ip", &my_bootp_info.bp_siaddr, CONFIG_IP);
#endif
    have_net = false;
    // Make sure the recv buffers are set up
    eth_drv_buffers_init();
    __pktbuf_init();
    // Initialize all network devices
    for (t = &__NETDEVTAB__[0]; t != &__NETDEVTAB_END__; t++) {
        if (t->init(t)) {
            t->status = CYG_NETDEVTAB_STATUS_AVAIL;
        } else {
            // What to do if device init fails?
            t->status = 0;  // Device not [currently] available
        }
    }
    if (!__local_enet_sc) {
        printf("No network interfaces found\n");
        return;
    }    
    // Initialize the network [if present]
    if (use_bootp) {
        if (__bootp_find_local_ip(&my_bootp_info) == 0) {
            have_net = true;
        } else {
            // Is it an unset address, or has it been set to a static addr
            if (__local_ip_addr[0] == 0 && __local_ip_addr[1] == 0 &&
                __local_ip_addr[2] == 0 && __local_ip_addr[3] == 0) {
                printf("Can't get BOOTP info - network disabled!\n");
            } else {
                printf("Can't get BOOTP info, using default IP address\n");
                have_net = true;
            }
        }
    } else {
        have_net = true;  // Assume values in FLASH were OK
    }
    if (have_net) {
        printf("IP: %s", inet_ntoa((in_addr_t *)&__local_ip_addr));
        printf(", Default server: %s\n", inet_ntoa(&my_bootp_info.bp_siaddr));
        net_io_init();
    }
}
