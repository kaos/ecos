/****************************************************************************

		THIS SOFTWARE IS NOT COPYRIGHTED

   HP offers the following for use in the public domain.  HP makes no
   warranty with regard to the software or it's performance and the
   user accepts the software "AS IS" with all faults.

   HP DISCLAIMS ANY WARRANTIES, EXPRESS OR IMPLIED, WITH REGARD
   TO THIS SOFTWARE INCLUDING BUT NOT LIMITED TO THE WARRANTIES
   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

****************************************************************************/

/****************************************************************************
 *  Header: remcom.c,v 1.34 91/03/09 12:29:49 glenne Exp $
 *
 *  Module name: remcom.c $
 *  Revision: 1.34 $
 *  Date: 91/03/09 12:29:49 $
 *  Contributor:     Lake Stevens Instrument Division$
 *
 *  Description:     low level support for gdb debugger. $
 *
 *  Considerations:  only works on target hardware $
 *
 *  Written by:      Glenn Engel $
 *  ModuleState:     Experimental $
 *
 *  NOTES:           See Below $
 *
 *  Modified for SPARC by Stu Grossman, Cygnus Support.
 *  Modified for generic CygMON stub support by Bob Manson, Cygnus Solutions.
 *
 *  To enable debugger support, two things need to happen.  One, a
 *  call to set_debug_traps() is necessary in order to allow any breakpoints
 *  or error conditions to be properly intercepted and reported to gdb.
 *  Two, a breakpoint needs to be generated to begin communication.  This
 *  is most easily accomplished by a call to breakpoint().  Breakpoint()
 *  simulates a breakpoint by executing a trap #1.
 *
 *************
 *
 *    The following gdb commands are supported:
 *
 * command          function                               Return value
 *
 *    g             return the value of the CPU registers  hex data or ENN
 *    G             set the value of the CPU registers     OK or ENN
 *
 *    mAA..AA,LLLL  Read LLLL bytes at address AA..AA      hex data or ENN
 *    MAA..AA,LLLL: Write LLLL bytes at address AA.AA      OK or ENN
 *
 *    c             Resume at current address              SNN   ( signal NN)
 *    cAA..AA       Continue at address AA..AA             SNN
 *
 *    s             Step one instruction                   SNN
 *    sAA..AA       Step one instruction from AA..AA       SNN
 *
 *    k             kill
 *
 *    ?             What was the last sigval ?             SNN   (signal NN)
 *
 *    bBB..BB	    Set baud rate to BB..BB		   OK or BNN, then sets
 *							   baud rate
 *
 * All commands and responses are sent with a packet which includes a
 * checksum.  A packet consists of
 *
 * $<packet info>#<checksum>.
 *
 * where
 * <packet info> :: <characters representing the command or response>
 * <checksum>    :: < two hex digits computed as modulo 256 sum of <packetinfo>>
 *
 * When a packet is received, it is first acknowledged with either '+' or '-'.
 * '+' indicates a successful transfer.  '-' indicates a failed transfer.
 *
 * Example:
 *
 * Host:                  Reply:
 * $m0,10#2a               +$00010203040506070809101112131415#42
 *
 ****************************************************************************/

#include <pkgconf/hal.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#include <cyg/hal/hal_stub.h>

#define	SIGHUP	1	/* hangup */
#define	SIGINT	2	/* interrupt */
#define	SIGQUIT	3	/* quit */
#define	SIGILL	4	/* illegal instruction (not reset when caught) */
#define	SIGTRAP	5	/* trace trap (not reset when caught) */
#define	SIGIOT	6	/* IOT instruction */
#define	SIGABRT 6	/* used by abort, replace SIGIOT in the future */
#define	SIGEMT	7	/* EMT instruction */
#define	SIGFPE	8	/* floating point exception */
#define	SIGKILL	9	/* kill (cannot be caught or ignored) */
#define	SIGBUS	10	/* bus error */
#define	SIGSEGV	11	/* segmentation violation */
#define	SIGSYS	12	/* bad argument to system call */
#define	SIGPIPE	13	/* write on a pipe with no one to read it */
#define	SIGALRM	14	/* alarm clock */
#define	SIGTERM	15	/* software termination signal from kill */

#include <cyg/hal/hal_arch.h>           // HAL header

#ifdef CYGDBG_HAL_DEBUG_GDB_THREAD_SUPPORT
#define DEBUG_THREADS 1                 // FIXME: put in header file?
#else
#define DEBUG_THREADS 0                 // FIXME: put in header file?
#endif

#define uint32 cyg_uint32
#include "thread-pkts.h"

/************************************************************************/
/* BUFMAX defines the maximum number of characters in inbound/outbound buffers*/
/* at least NUMREGBYTES*2 are needed for register packets */
#define BUFMAX 2048

static int process_exception (int sigval);
static void do_nothing (void); /* and do it gracefully */
static int syscall_do_nothing (int);
static void kill_program (int sigval);
static void process_query (char *pkt);
static void process_set   (char *pkt);

volatile __PFI __process_exception_vec = process_exception;
volatile __PFV __process_exit_vec = do_nothing;
volatile __PFI __process_syscall_vec = syscall_do_nothing;
volatile __PFI __process_signal_vec = NULL;
volatile __PFI __interruptible_control = NULL;

static const char hexchars[] = "0123456789abcdef";

extern target_register_t registers[]; /* The current saved registers */
extern target_register_t * _registers ;
extern HAL_SavedRegisters *_hal_registers;

int __process_target_query(char * pkt, char * out, int maxOut)
{ return 0 ; }
int __process_target_set(char * pkt, char * out, int maxout)
{ return 0 ; }
int __process_target_packet(char * pkt, char * out, int maxout)
{ return 0 ; }


/* Return the value corresponding to register REG of the active thread. */
static target_register_t thread_get_register (regnames_t reg)
{
    return _registers[reg];
}


/* Store VALUE in the register corresponding to WHICH of the active thread. */
static void thread_put_register (regnames_t which, target_register_t value)
{
    _registers[which] = value;
}


char
__tohex (int c)
{
  return hexchars [c & 15];
}

target_register_t *__our_registers;

#ifndef NUMREGS_GDB
#define NUMREGS_GDB NUMREGS
#endif

static char *strcpy( char *s, const char *t)
{
    char *r = s;

    while( *t ) *s++ = *t++;

    return r;
}

static int strlen( const char *s )
{
    int r = 0;
    while( *s++ ) r++;
    return r;
}

// Use this function to disable serial interrupts whenever reply
// characters are expected from GDB.  The reason we want to control
// whether the target can be interrupted or not is simply that GDB on
// the host will be sending acknowledge characters/commands while the
// stub is running - if serial interrupts were still active, the
// characters would never reach the (polling) getDebugChar.
static void
interruptible (int state)
{
    static int current_state = 0;

    if (__interruptible_control) {
        if (state) {
            current_state--;
            if (0 >= current_state) {
                current_state = 0;
                __interruptible_control (1);
            }
        } else {
            current_state++;
            if (1 == current_state)
                __interruptible_control (0);
        }
    }
}


/* One pushback character. */
int ungot_char = -1;

static int
readDebugChar (void)
{
  if (ungot_char > 0)
    {
      int result = ungot_char;
      ungot_char = -1;
      return result;
    }
  else
    return getDebugChar ();
}

struct gdb_packet
{
  /* The checksum calculated so far. */
  int checksum;
  /* The checksum we've received from the remote side. */
  int xmitcsum;
  /* Contents of the accumulated packet. */
  char *contents;
  /* Number of characters received. */
  int length;
  /*
   * state is the current state:
   *  0 = looking for start of packet
   *  1 = got start of packet, looking for # terminator
   *  2 = looking for first byte of checksum
   *  3 = looking for second byte of checksum (indicating end of packet)
   */
  char state;
};

/* Convert ch from a hex digit to an int. */

int
stubhex(unsigned char ch)
{
  if (ch >= 'a' && ch <= 'f')
    return ch-'a'+10;
  if (ch >= '0' && ch <= '9')
    return ch-'0';
  if (ch >= 'A' && ch <= 'F')
    return ch-'A'+10;
  return -1;
}

static void
getpacket(char *buffer)
{
  struct gdb_packet packet;

  packet.state = 0;
  packet.contents = buffer;
  while (__add_char_to_packet (readDebugChar () & 0x7f, &packet) != 1)
    {
      /* Empty */
    }
}

int
__add_char_to_packet (int ch, struct gdb_packet *packet)
{
  if (packet->state == 0)
    {
      if (ch == '$')
	{
	  packet->state = 1;
	  packet->length = 0;
	  packet->checksum = 0;
	  packet->xmitcsum = -1;
	}
      return 0;
    }
  
  if (packet->state == 1)
    {
      if (ch == '#')
	{
	  packet->contents[packet->length] = 0;
	  packet->state = 2;
	}
      else
	{
	  if (packet->length == BUFMAX) {
	    packet->state = 0;
	    return -1;
	  }
	  packet->checksum += ch;
	  packet->contents[packet->length++] = ch;
	}
      return 0;
    }

  if (packet->state == 2)
    {
      packet->xmitcsum = stubhex (ch) << 4;
      packet->state = 3;
      return 0;
    }

  if (packet->state == 3)
    {
      packet->xmitcsum |= stubhex(ch);
      if ((packet->checksum & 255) != packet->xmitcsum)
	{
	  putDebugChar ('-');	/* failed checksum */
	  packet->state = 0;
	  return -1;
	}
      else
	{
	  putDebugChar('+'); /* successful transfer */
	  /* if a sequence char is present, reply the sequence ID */
	  if (packet->contents[2] == ':')
	    {
	      int count = packet->length;
	      int i;
	      putDebugChar (packet->contents[0]);
	      putDebugChar (packet->contents[1]);
	      /* remove sequence chars from buffer */
	      for (i=3; i <= count; i++)
		packet->contents[i-3] = packet->contents[i];
	    }
	  return 1;
	}
    }
  /* We should never get here. */
  packet->state = 0;
  return -1;
}

/* send the packet in buffer.  */

static void
putpacket(unsigned char *buffer)
{
  unsigned char checksum;
  int count;
  unsigned char ch;

  interruptible (0);

  /*  $<packet info>#<checksum>. */
  do
    {
      putDebugChar ('$');
      checksum = 0;
      count = 0;

      while ((ch = buffer[count]))
	{
	  putDebugChar (ch);
	  checksum += ch;
	  count += 1;
	}

      putDebugChar ('#');
      putDebugChar (hexchars[(checksum >> 4) & 0xf]);
      putDebugChar (hexchars[checksum & 0xf]);

    }
  while ((readDebugChar () & 0x7f) != '+');

  interruptible (1);
}

static char remcomInBuffer[BUFMAX];
static char remcomOutBuffer[BUFMAX];

/* Indicate to caller of mem2hex or hex2mem that there has been an
   error.  */
volatile int __mem_fault = 0;

static int
__copy_mem_safe (unsigned char *dst, unsigned char *src, int count)
{
  int res;

  __set_mem_fault_trap (1);
  while (count--)
    *(dst++) = *(src++);
  res = __mem_fault;
  __set_mem_fault_trap (0);
  return res ? 1 : 0;
}

int
__read_mem_safe (unsigned char *dst, target_register_t src, int count)
{
  return __copy_mem_safe (dst, (unsigned char*) src, count);
}

int
__write_mem_safe (unsigned char *src, target_register_t dst, int count)
{
  return __copy_mem_safe ((unsigned char*) dst, src, count);
}

/* Convert the memory pointed to by mem into hex, placing result in buf.
 * Return a pointer to the last char put in buf (null), in case of mem fault,
 * return 0.
 * If MAY_FAULT is non-zero, then we will handle memory faults by returning
 * a 0, else treat a fault like any other fault in the stub.
 */

unsigned char *
__mem2hex(mem, buf, count, may_fault)
     unsigned char *mem;
     unsigned char *buf;
     int count;
     int may_fault;
{
  unsigned char ch;

  __set_mem_fault_trap(may_fault);

  while (count-- > 0)
    {
      ch = *mem++;
      if (__mem_fault)
	return 0;
      *buf++ = hexchars[(ch >> 4) & 0xf];
      *buf++ = hexchars[ch & 0xf];
    }

  *buf = 0;

  __set_mem_fault_trap(0);

  return buf;
}

/* convert the hex array pointed to by buf into binary to be placed in mem
 * return a pointer to the character AFTER the last byte written */

static char *
hex2mem(unsigned char *buf, unsigned char *mem, int count, int may_fault)
{
  int i;
  unsigned char ch;

  __set_mem_fault_trap (may_fault);

  for (i=0; i<count; i++)
    {
      ch = stubhex(*buf++) << 4;
      ch |= stubhex(*buf++);
      *mem++ = ch;
      if (__mem_fault)
	return 0;
    }

  __set_mem_fault_trap(0);

  return mem;
}

/*
 * While we find nice hex chars, build an int.
 * Return number of chars processed.
 */

static unsigned int
__hexToInt(char **ptr, target_register_t *intValue)
{
  int numChars = 0;
  int hexValue;

  *intValue = 0;

  while (**ptr)
    {
      hexValue = stubhex(**ptr);
      if (hexValue < 0)
	break;

      *intValue = (*intValue << 4) | hexValue;
      numChars ++;

      (*ptr)++;
    }

  return (numChars);
}

/* 
 * Complement of __hexToInt: take an int of size "numBits", 
 * convert it to a hex string.  Return length of (unterminated) output.
 */

unsigned int
__intToHex (char *ptr, target_register_t intValue, int numBits)
{
  int numChars = 0;

  if (intValue == 0)
    {
      *(ptr++) = '0';
      *(ptr++) = '0';
      return 2;
    }

  numBits = (numBits + 7) / 8;
  while (numBits)
    {
      int v = (intValue >> ((numBits - 1) * 8));
      if (v || (numBits == 1))
	{
	  v = v & 255;
	  *(ptr++) = __tohex ((v / 16) & 15);
	  *(ptr++) = __tohex (v & 15);
	  numChars += 2;
	}
      numBits--;
    }

  return (numChars);
}

void
__handle_exception (void)
{
  void breakinst(void);
  int sigval;

  interruptible (0);

  // Expand the HAL_SavedRegisters structure into the GDB register
  // array format.
  HAL_GET_GDB_REGISTERS (&registers[0], _hal_registers);
  _registers = &registers[0];

  /* reply to host that an exception has occurred */
  sigval = __computeSignal (__get_trap_number ());

  if (__is_breakpoint_function ())
    __skipinst ();

#if 0 //def SIGSYSCALL
  if(sigval == SIGSYSCALL)
    {
      int val;
      /* Do the skipinst FIRST. */
      __skipinst ();
      val =  __process_syscall_vec (__get_syscall_num ());
      if (val == 0)
	return;
      else if (val < 0)
	sigval = -val;
    }

#endif

  while (__process_exception_vec (sigval));


  // Compact register array again.
  HAL_SET_GDB_REGISTERS (_hal_registers, &registers[0]);

  interruptible (1);
}

static int
process_packet (char *packet)
{
//diag_printf("pkt %s\n",packet);    
  remcomOutBuffer[0] = 0;

  switch (packet[0])
    {
    case '?':
      {
	int sigval = __computeSignal (__get_trap_number ());
	remcomOutBuffer[0] = 'S';
	remcomOutBuffer[1] = hexchars[(sigval >> 4) & 0xf];
	remcomOutBuffer[2] = hexchars[sigval & 0xf];
	remcomOutBuffer[3] = 0;
	break;
      }

    case 'd':
      /* toggle debug flag */
      break;

    case 'q':
      /* general query packet */
      process_query (&packet[1]);
      break;

    case 'Q':
      /* general set packet */
      process_set (&packet[1]);
      break;

    case 'g':		/* return the value of the CPU registers */
      {
	char *ptr = remcomOutBuffer;
	int x;

	for (x = 0; x < NUMREGS_GDB; x++)
	  {
	    /* We need to compensate for the value offset within the
	       register. */
	    target_register_t addr = thread_get_register (x);
	    char *vptr = ((char *)&addr) + sizeof(addr) - REGSIZE(x);

	    ptr = __mem2hex(vptr, ptr, REGSIZE(x), 0);
	  }
	break;
      }

    case 'P':
    case 'G':	   /* set the value of the CPU registers - return OK */
      {
	int x;
	int sr = 0, er = NUMREGS_GDB;
	char *ptr = &packet[1];

	if (packet[0] == 'P')
	  {
	    target_register_t regno;

	    if (__hexToInt (&ptr, &regno)
		&& (*ptr++ == '='))
	      {
		sr = regno;
		er = regno + 1;
	      }
	    else
	      {
		strcpy (remcomOutBuffer, "P01");
		break;
	      }
	  }

	for (x = sr; x < er; x++)
	  {
	    target_register_t value = 0;

	    char *vptr = ((char *)&value) + sizeof(value) - REGSIZE(x);
	    hex2mem(ptr, vptr, REGSIZE (x), 0);
	    thread_put_register (x, value);
	    ptr += REGSIZE(x) * 2;
	  }

	strcpy (remcomOutBuffer, "OK");
	break;
      }

    case 'm':	  /* mAA..AA,LLLL  Read LLLL bytes at address AA..AA */
      /* Try to read %x,%x.  */
      {
	target_register_t addr, length;
	char *ptr = &packet[1];

	if (__hexToInt(&ptr, &addr)
	    && *ptr++ == ','
	    && __hexToInt(&ptr, &length))
	  {
	    if (__mem2hex ((char *)addr, remcomOutBuffer, length, 1))
	      break;

	    strcpy (remcomOutBuffer, "E03");
	  }
	else
	  strcpy(remcomOutBuffer,"E01");
	break;
      }

    case 'M': /* MAA..AA,LLLL: Write LLLL bytes at address AA.AA return OK */
      /* Try to read '%x,%x:'.  */
      {
	target_register_t addr, length;
	char *ptr = &packet[1];

	if (__hexToInt(&ptr, &addr)
	    && *ptr++ == ','
	    && __hexToInt(&ptr, &length)
	    && *ptr++ == ':')
	  {
	    if (hex2mem(ptr, (char *)addr, length, 1))
	      strcpy(remcomOutBuffer, "OK");
	    else
	      strcpy(remcomOutBuffer, "E03");
	  }
	else
	  strcpy(remcomOutBuffer, "E02");
	break;
      }

    case 'S':
    case 's':	 /* sAA..AA    Step from address AA..AA(optional) */
    case 'C':
    case 'c':    /* cAA..AA    Continue at address AA..AA(optional) */
      /* try to read optional parameter, pc unchanged if no parm */

      {
	char *ptr = &packet[1];
	target_register_t addr;
	target_register_t foo = 99999;
	target_register_t sigval;

//diag_printf("stepping\n");        
	sigval = foo;
	if (packet[0] == 'C' || packet[0] == 'S')
	  {
	    __hexToInt (&ptr, &sigval);
	    if (*ptr == ';')
	      ptr++;
	  }

	if (__hexToInt(&ptr, &addr))
	  set_pc (addr);
	
      /* Need to flush the instruction cache here, as we may have deposited a
	 breakpoint, and the icache probably has no way of knowing that a data ref to
	 some location may have changed something that is in the instruction cache.
	 */

	flush_i_cache ();

	if (sigval != foo)
	  {
	    if (__process_signal_vec != NULL)
	      {
		if (! __process_signal_vec (sigval))
		  return -1;
	      }
	    sigval = SIGKILL; /* Always nuke the program */
	    kill_program (sigval);
	    return 0;
	  }

	/* Set machine state to force a single step.  */
	if (packet[0] == 's' || packet[0] == 'S')
	  single_step ();

	return -1;
      }

      /* kill the program */
    case 'k' :
      __process_exit_vec ();
      return 1;
#if 0
    case 't':		/* Test feature */
      __asm__ (" std %f30,[%sp]");
      break;
#endif
    case 'r':		/* Reset */
      __reset ();
      break;
    case 'H':
      STUB_PKT_CHANGETHREAD (packet+1, remcomOutBuffer, 300) ;
      break ;
    case 'T' :
      STUB_PKT_THREAD_ALIVE (packet+1, remcomOutBuffer, 300) ;
      break ;

    case 'b':	  /* bBB...  Set baud rate to BB... */
      {
	target_register_t baudrate;

	char *ptr = &packet[1];
	if (!__hexToInt(&ptr, &baudrate))
	  {
	    strcpy(remcomOutBuffer,"B01");
	    break;
	  }

	putpacket ("OK");	/* Ack before changing speed */
	__set_baud_rate (baudrate);
	break;
      }
    }

  /* reply to the request */
  putpacket (remcomOutBuffer);
  return 0;
}

static void
send_t_packet (int sigval)
{
  __build_t_packet (sigval, remcomOutBuffer);
  putpacket(remcomOutBuffer);
}

/*
 * This function does all command procesing for interfacing to gdb.
 */

static int
process_exception (int sigval)
{
  int status;

  /* Nasty. */
  if (ungot_char < 0)
    send_t_packet (sigval);

  /* Undo effect of previous single step.  */
  clear_single_step ();

  do {
    getpacket (remcomInBuffer);
    status = process_packet (remcomInBuffer);
  } while (status == 0);


  /* Install breakpoints that may have been set to perform single stepping.  */
  install_breakpoints ();

  if (status < 0)
    return 0;
  else
    return 1;
}

void
__send_exit_status (int status)
{
  remcomOutBuffer[0] = 'W';
  remcomOutBuffer[1] = hexchars[(status >> 4) & 0xf];
  remcomOutBuffer[2] = hexchars[status & 0xf];
  remcomOutBuffer[3] = 0;
  putpacket (remcomOutBuffer);
}

int
__get_gdb_input (char *dest, int maxlen, int block)
{
  char buf[4];
  int len, i;

  buf[0] = 'I';
  buf[1] = '0';
  buf[2] = block ? '0' : '1';
  buf[3] = 0;
  putpacket (buf);
  getpacket (remcomInBuffer);
  if (remcomInBuffer[0] != 'I')
    return -1;
  len = stubhex (remcomInBuffer[1]) * 16 + stubhex (remcomInBuffer[2]);
  for (i = 0; i < len; i++)
    {
      dest[i] = stubhex (remcomInBuffer[3 + i * 2]) * 16;
      dest[i] |=  stubhex (remcomInBuffer[3 + i * 2 + 1]);
    }
  dest[i] = '\0';
  return len;
}

void
__output_hex_value (target_register_t i)
{
  char buf[32], *ptr=buf+31;
  unsigned int x;

  *ptr = 0;
  for (x = 0; x < (sizeof(i) * 2); x++)
    {
      *(--ptr) = hexchars[i & 15];
      i = i >> 4;
    }
  while (*ptr)
    {
      putDebugChar (*(ptr++));
    }
}

int
__output_gdb_string(const char *str, int string_len)
{
  static char buf[1024];
  int x;
  int len;

  if (string_len == 0)
    string_len = strlen (str);

  len = string_len * 2 + 1;

  buf[0] = 'O';
  for (x = 0; x < string_len; x++) 
    {
      char c = str[x];
      buf[x*2+1] = hexchars[(c >> 4) & 0xf];
      buf[x*2+2] = hexchars[c % 16];
    }
  buf[x*2+1] = 0;
  putpacket (buf);
  return string_len;
}

static void
do_nothing (void)
{
  /* mmmm */
}

static int
syscall_do_nothing (int junk)
{
  return 0;
}

/* Start the stub running. */
void
__switch_to_stub ()
{
  __process_exception_vec = process_exception;
}

void
ungetDebugChar (int c)
{
  ungot_char = c;
}

static void
kill_program (int sigval)
{
  remcomOutBuffer[0] = 'X';
  remcomOutBuffer[1] = hexchars[(sigval >> 4) & 15];
  remcomOutBuffer[2] = hexchars[sigval & 15];
  remcomOutBuffer[3] = 0;
  putpacket (remcomOutBuffer);
}

/* Table used by the crc32 function to calcuate the checksum. */
static uint32 crc32_table[256];
static int tableInit = 0;

/* 
   Calculate a CRC-32 using LEN bytes of PTR. CRC is the initial CRC
   value.
   PTR is assumed to be a pointer in the user program. */

static uint32
crc32 (unsigned char *ptr, int len, uint32 crc)
{
  if (! tableInit)
    {
      /* Initialize the CRC table and the decoding table. */
      uint32 i, j;
      uint32 c;

      tableInit = 1;
      for (i = 0; i < 256; i++)
	{
	  for (c = i << 24, j = 8; j > 0; --j)
	    c = c & 0x80000000 ? (c << 1) ^ 0x04c11db7 : (c << 1);
	  crc32_table[i] = c;
	}
    }

  __mem_fault = 0;
  while (len--)
    {
      unsigned char ch;

      __read_mem_safe (&ch, (target_register_t) ptr, 1);
      if (__mem_fault)
	{
	  break;
	}
      crc = (crc << 8) ^ crc32_table[((crc >> 24) ^ ch) & 255];
      ptr++;
    }
  return crc;
}

/* Handle the 'q' request */

static void
process_query (char *pkt)
{
  remcomOutBuffer[0] = '\0';
  if ('C' == pkt[0] &&
      'R' == pkt[1] &&
      'C' == pkt[2] &&
      ':' == pkt[3])
    {
      target_register_t startmem;
      target_register_t length;
      uint32 our_crc;

      pkt += 4;
      if (__hexToInt (&pkt, &startmem)
	  && *(pkt++) == ','
	  && __hexToInt (&pkt, &length))
	{
	  our_crc = crc32 ((unsigned char *) startmem, length, 0xffffffff);
	  if (__mem_fault)
	    {
	      strcpy (remcomOutBuffer, "E01");
	    }
	  else
	    {
	      int numb = __intToHex (remcomOutBuffer + 1, our_crc, 32);
	      remcomOutBuffer[0] = 'C';
	      remcomOutBuffer[numb + 1] = 0;
	    }
	}
      return;
    }
  else
    {
      char ch ;
      char * subpkt ;
      ch = *pkt ;
      subpkt = pkt + 1 ;
      switch (ch)
	{
	case 'L' : /* threadlistquery */
	  STUB_PKT_GETTHREADLIST (subpkt, remcomOutBuffer, 300);
	  break ;
	case 'P' : /* Thread or process information request */
	  STUB_PKT_GETTHREADINFO (subpkt, remcomOutBuffer, 300);
	  break ;
	default:
	  __process_target_query (pkt, remcomOutBuffer, 300);
	  break ;
	}
    }
}

/* Handle the 'Q' request */

static void
process_set (char *pkt)
{
  char ch ;
  ch = *pkt ;
  
  switch (ch)
    {
    case 'p' : /* Set current process or thread */
      /* reserve the packet id even if support is not present */
      /* Dont strip the 'p' off the header, there are several variations of
	 this packet */
      STUB_PKT_CHANGETHREAD (pkt, remcomOutBuffer, 300) ;
      break ;
    default:
      __process_target_set (pkt, remcomOutBuffer, 300);
      break ;
    }
}

#endif // CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
