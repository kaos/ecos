//=================================================================
//
//        serial_tx39.cxx
//
//        Drivers for the tx39 
//
//=================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     proven
// Contributors:    proven
// Date:          1998-05-25
// Description:   Class methods for the class Cyg_Device_Serial_tx39
// The tx39 on chip serial port only requries one interrupt handler
// as it only has one vector for the port. The handler must check 
// and multiples the read and write interrupts and then clear it.
// One unusual thing about the TX39 is that the transfer interrupt
// is not clearable if there is space to put characters in the 
// tfifo. This means that when nothing is to be written the 
// transfer interrupt must be disabled.
//####DESCRIPTIONEND####

#include <pkgconf/devs.h>		// To see if we need to bother
#if defined(CYG_HAL_TX39_JMR3904) && defined(CYGPKG_DEVICES_SERIAL_RS232_TX39)

#define CYG_DEVICE_INTERNAL
#include <cyg/devs/serial/rs232/tx39/serial_tx39.hxx>
#include <cyg/kernel/sema.hxx>		// Cyg_Binary_Semaphore

#ifdef CYG_DEVICE_SERIAL_RS232_TX39_NAME
#ifdef CYG_DEVICE_SERIAL_RS232_TX39_DECLARE
CYG_CLASS_DEVICE_SERIAL_RS232_TX39 CYG_DEVICE_SERIAL_RS232_TX39_NAME;
#endif
#endif

// ------------------------------------------------------------------------
// Constructor for serial device                 
//
CYG_CLASS_DEVICE_SERIAL_RS232_TX39::CYG_CLASS_DEVICE_SERIAL_RS232_TX39()
#ifdef CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT
    : both_interrupt(CYG_DEVICE_SERIAL_RS232_TX39_VEC, 4, (CYG_ADDRWORD)this, both_isr, both_dsr)
#endif
{
    CYG_REPORT_FUNCTION();

    read_buffer = NULL;

    // read_buffers = NULL; 
#ifdef CYG_DEVICE_SERIAL_RS232_READ_BUFFERS_LL
    read_buffers_ll_last = NULL;
    read_buffers_ll_first = NULL;
#endif

#ifdef CYG_DEVICE_SERIAL_RS232_READ_MODES
    // Default to ASCII mode if READ_MODES is configured.
    read_mode = 0xff;
//    read_mode_translate_char = 0;
//    read_mode_escape_next_char = 0;
#endif

    write_buffer = NULL;
    // write_buffers = NULL; 
#ifdef CYG_DEVICE_SERIAL_RS232_WRITE_BUFFERS_LL
    write_buffers_ll_last = NULL;
    write_buffers_ll_first = NULL;
#endif

#ifdef CYG_DEVICE_SERIAL_RS232_WRITE_MODES
    // Default to CR to CRLF translation
    write_mode = 1;
    write_char = 0;
#endif

    // Set the timers before enabling them or the serial device
    *CYG_DEVICE_SERIAL_RS232_TX39_SBRG = (CYG_DEVICE_SERIAL_RS232_T1_VALUE << 8) | CYG_DEVICE_SERIAL_RS232_T2_VALUE;
// (0x0100 | 10);

#ifdef CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT
    both_interrupt.attach();
    // Clear interrupts for now.
    *CYG_DEVICE_SERIAL_RS232_TX39_SDICR = 0x0000;
    /* Do not unmask the interrupt unless interrupt is the default mode */
#endif

    // Set the control registers, finallizing the settup.
    // Set up FIFOs rx and tx DMA transfer levels to 4
    *CYG_DEVICE_SERIAL_RS232_TX39_SFCR = 0x0000;

    // Source from baud rate generator, 8-1-no parity.
    *CYG_DEVICE_SERIAL_RS232_TX39_SLCR = 0x0020;
}

// ------------------------------------------------------------------------
// set_kmode()
// Set the kernel mode to (polled, interrrupt, ...)
// Only applicable if we have multiple modes.

#if defined (CYG_DEVICE_SERIAL_RS232_KMODE_POLLED) && \
    defined (CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT)

cyg_int32
CYG_CLASS_DEVICE_SERIAL_RS232_TX39::
  set_kmode(cyg_uint32 new_mode)
{

#ifdef CYG_DEVICE_SERIAL_RS232_MUTEX
    mutex.lock();
#endif

    /* Switching from polled mode to interrupt mode is easy */
    if (kmode == CYG_DEVICE_SERIAL_RS232_KMODE_POLLED) {
    	if (new_mode == CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT) {

	    /* Only enable read interrupts for now */
    	    *CYG_DEVICE_SERIAL_RS232_TX39_SDICR = 0x0001;
	    *CYG_DEVICE_SERIAL_RS232_TX39_SDISR = 0x0000;

	    /* Turn on interrupts by unmasking the vector */
	    both_interrupt.unmask_interrupt(both_interrupt.get_vector());
	}
    } 
    /* Switching to polled mode should flush the buffers */
    else {
	if (new_mode == CYG_DEVICE_SERIAL_RS232_KMODE_POLLED) {
  	    both_interrupt.mask_interrupt(both_interrupt.get_vector());
    	    *CYG_DEVICE_SERIAL_RS232_TX39_SDICR = 0x0000;
	    *CYG_DEVICE_SERIAL_RS232_TX39_SDISR = 0x0000;
	}
    }

    kmode = new_mode;

#ifdef CYG_DEVICE_SERIAL_RS232_MUTEX
    mutex.unlock();
#endif

    return 0;
}

#else

cyg_int32
CYG_CLASS_DEVICE_SERIAL_RS232_TX39::
  set_kmode(cyg_uint32 new_mode)
{
#if defined(CYG_DEVICE_SERIAL_RS232_KMODE_POLLED)
    CYG_ASSERT (CYG_DEVICE_SERIAL_RS232_KMODE_POLLED == new_mode,
      "Can only select polled mode" );
#endif
#if defined(CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT)
    CYG_ASSERT (CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT == new_mode,
      "Can only select interrupt mode" );
#endif

    return 0;
}

#endif

// ------------------------------------------------------------------------
// Baud rate

static struct baud_rate {
    cyg_uint8	bclk;
    cyg_uint8   brd;
} baud_rate_table[] = {
    {  CYG_DEVICE_SERIAL_RS232_T1_VALUE_B0, 	CYG_DEVICE_SERIAL_RS232_T2_VALUE_B0 },	   // B0
    {  CYG_DEVICE_SERIAL_RS232_T1_VALUE_B0, 	CYG_DEVICE_SERIAL_RS232_T2_VALUE_B0 },	   // B50
    {  CYG_DEVICE_SERIAL_RS232_T1_VALUE_B0, 	CYG_DEVICE_SERIAL_RS232_T2_VALUE_B0 },	   // B75
    {  CYG_DEVICE_SERIAL_RS232_T1_VALUE_B0, 	CYG_DEVICE_SERIAL_RS232_T2_VALUE_B0 },	   // B110
 
    {  CYG_DEVICE_SERIAL_RS232_T1_VALUE_B0, 	CYG_DEVICE_SERIAL_RS232_T2_VALUE_B0 },	   // B134.5
    {  CYG_DEVICE_SERIAL_RS232_T1_VALUE_B0, 	CYG_DEVICE_SERIAL_RS232_T2_VALUE_B0 },	   // B150
    {  CYG_DEVICE_SERIAL_RS232_T1_VALUE_B0, 	CYG_DEVICE_SERIAL_RS232_T2_VALUE_B0 },	   // B200
    {  CYG_DEVICE_SERIAL_RS232_T1_VALUE_B0, 	CYG_DEVICE_SERIAL_RS232_T2_VALUE_B0 },	   // B300

    {  CYG_DEVICE_SERIAL_RS232_T1_VALUE_B600, 	CYG_DEVICE_SERIAL_RS232_T2_VALUE_B600 },   // B600
    {  CYG_DEVICE_SERIAL_RS232_T1_VALUE_B1200, 	CYG_DEVICE_SERIAL_RS232_T2_VALUE_B1200 },  // B1200
    {  CYG_DEVICE_SERIAL_RS232_T1_VALUE_B0, 	CYG_DEVICE_SERIAL_RS232_T2_VALUE_B0 },	   // B1800
    {  CYG_DEVICE_SERIAL_RS232_T1_VALUE_B2400, 	CYG_DEVICE_SERIAL_RS232_T2_VALUE_B2400 },  // B2400

    {  CYG_DEVICE_SERIAL_RS232_T1_VALUE_B4800, 	CYG_DEVICE_SERIAL_RS232_T2_VALUE_B4800 },  // B4800
    {  CYG_DEVICE_SERIAL_RS232_T1_VALUE_B9600, 	CYG_DEVICE_SERIAL_RS232_T2_VALUE_B9600 },  // B9600
    {  CYG_DEVICE_SERIAL_RS232_T1_VALUE_B19200, CYG_DEVICE_SERIAL_RS232_T2_VALUE_B19200 }, // B19200
    {  CYG_DEVICE_SERIAL_RS232_T1_VALUE_B38400, CYG_DEVICE_SERIAL_RS232_T2_VALUE_B38400 }  // B38400
};

cyg_int32
CYG_CLASS_DEVICE_SERIAL_RS232_TX39::
get_baud_rate() 
{
    cyg_ucount8 i;
    cyg_uint8 bclk, brd;

    brd = *CYG_DEVICE_SERIAL_RS232_TX39_SBRG & 0xff;
    bclk = (*CYG_DEVICE_SERIAL_RS232_TX39_SBRG >> 8) & 0xff;

    for (i = 0; i < (sizeof(baud_rate_table) / sizeof(struct baud_rate)); i++) {
	if ((bclk == baud_rate_table[i].bclk) &&
	  (brd == baud_rate_table[i].brd)) {
	    return(i);
	}
    }

    return(-1);
}

cyg_int32
CYG_CLASS_DEVICE_SERIAL_RS232_TX39::
set_baud_rate(cyg_uint32 baud_rate) 
{
    cyg_int32 old_baud_rate;

    if (baud_rate > (sizeof(baud_rate_table) / sizeof(struct baud_rate))) {
	return -1;
    }

#ifdef CYG_DEVICE_SERIAL_RS232_MUTEX
    mutex.lock();
#endif

    /*
     * Flush the write queue. 
     * Not necessary for the read queue
     */
    this->io_write_flush();

    /* Get the old baud rate */
    if ((old_baud_rate = this->get_baud_rate()) < 0)
	old_baud_rate = 0;

    /* Turn off read and write */
    // Cannot be done for the TX39 chip SIO

    /* Reenable timer and serial port for valid baud rates greater than 0 */
    if (baud_rate_table[baud_rate].bclk && baud_rate_table[baud_rate].brd) {
    	*CYG_DEVICE_SERIAL_RS232_TX39_SBRG = 
	  (baud_rate_table[baud_rate].bclk << 8) | baud_rate_table[baud_rate].brd;
    }

#ifdef CYG_DEVICE_SERIAL_RS232_MUTEX
    mutex.unlock();
#endif

    return(old_baud_rate);

}

// ------------------------------------------------------------------------
// Line mode

cyg_int32
CYG_CLASS_DEVICE_SERIAL_RS232_TX39::
get_line_mode() 
{
    cyg_uint16 slcr;
    cyg_int32 ret;

    slcr = *CYG_DEVICE_SERIAL_RS232_TX39_SLCR;

    /* bits per byte */
    if (slcr & 0x0001) {
	ret = CS7;
    } else {
	ret = CS8;
    }
    /* Stop bits */
    if (slcr & 0x0004) {
	ret |= CSTOPB;
    }
    /* Parity */
    if (slcr & 0x0008) {
	ret |= PARENB;
	if (!(slcr & 0x0010)) {
	    ret |= PARODD;
	}
    }

    return ret;
}

cyg_int32
CYG_CLASS_DEVICE_SERIAL_RS232_TX39::
set_line_mode(cyg_uint32 line_mode) 
{
    cyg_int32 old_line_mode;
    cyg_uint16 slcr;

#ifdef CYG_DEVICE_SERIAL_RS232_MUTEX
    mutex.lock();
#endif
    /*
     * Flush the write queue. 
     * Not necessary for the read queue
     */
    this->io_write_flush();

    slcr = *CYG_DEVICE_SERIAL_RS232_TX39_SLCR;

    old_line_mode = this->get_line_mode();

    switch(line_mode & CSIZE) {
    case CS8:
	slcr &= ~0x0001;
	break;
    case CS7:
	slcr |= 0x0001;
	break;
    default: 
	return -1;
	break;
    }
    if (line_mode & CSTOPB) {
	slcr |= 0x0004;
    } else {
	slcr &= ~0x0004;
    }
    if (line_mode & PARENB) {
	if (line_mode & PARODD) {
	    slcr &= ~0x0010;
	} else {
	    slcr |= 0x0010;
	}
    } else {
	slcr &= ~0x0008;
    }

    /* Set the new flags all in one write. */
    *CYG_DEVICE_SERIAL_RS232_TX39_SLCR = slcr;

#ifdef CYG_DEVICE_SERIAL_RS232_MUTEX
    mutex.unlock();
#endif

    return old_line_mode;

}

// ------------------------------------------------------------------------
// set_read_mode()
// Set the read mode (CRNL translation, EOL detection, escape characters)
// Currently there are only two modes (BINARY and ASCII)

#ifdef CYG_DEVICE_SERIAL_RS232_READ_MODES

cyg_int32
CYG_CLASS_DEVICE_SERIAL_RS232_TX39::
set_read_mode(cyg_uint32 new_mode)
{
#ifdef CYG_DEVICE_SERIAL_RS232_MUTEX
    mutex.lock();
#endif
    if (new_mode) {
     	read_mode = 0xff;
    } else {
	read_mode = 0;
    }
#ifdef CYG_DEVICE_SERIAL_RS232_MUTEX
    mutex.unlock();
#endif
    return 0;
}

#endif

static inline void
throttle(char ch) 
{
#ifdef CYG_DEVICE_SERIAL_RS232_FLOW_CONTROL
    cyg_uint8 tmp;

    tmp = (data->read_throttle_free + 1) % data->read_throttle_size;
    /* Check that there is space to put the character */
    if (tmp != data->read_throttle_queued) {
        data->read_throttle_buffer[tmp] = ch;
        data->read_throttle_free = tmp;
    }
    data->read_throttle = 1;
#endif
}

// ------------------------------------------------------------------------
// read_isr()
// 
cyg_uint32
CYG_CLASS_DEVICE_SERIAL_RS232_TX39::
  read_isr(cyg_vector vector, CYG_ADDRWORD isr_data)
{
    CYG_CLASS_DEVICE_SERIAL_RS232_TX39 * data = (CYG_CLASS_DEVICE_SERIAL_RS232_TX39 *)isr_data;
    volatile cyg_uint16 * sdisr = CYG_DEVICE_SERIAL_RS232_TX39_SDISR;
    volatile cyg_uint8 * sfifo = CYG_DEVICE_SERIAL_RS232_TX39_SFIFO;
    cyg_uint32 off, ret;
    cyg_uint8 eob = 0;
    cyg_uint8 ch;

    // Default return value
    ret = Cyg_Interrupt::HANDLED;

#ifdef CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT
    // Clear interrupts regaurdless of kmode
    // data->read_interrupt.acknowledge_interrupt(vector);
#endif

    ch = *sfifo; 
    *sdisr = 0x0000;

#ifdef CYG_DEVICE_SERIAL_RS232_READ_MODES
    while (data->read_mode) {
	// On escape characters, skip mode checks; break
	// Deal with flow control characters; return

	/* Deal with ignored characters first */
	if (data->read_mode & CYG_DEVICE_SERIAL_RS232_READ_MODE_IGN) {
	    if (ch == 10) 
	        return ret;
	}
	/* Translate characters before checking on EOB conditions. */
	if (data->read_mode & CYG_DEVICE_SERIAL_RS232_READ_MODE_TRN) {
	    if (ch == 13) 
		ch = 10;
	}
	/* Return a not full IORB on an EOB condition. */
	if (data->read_mode & CYG_DEVICE_SERIAL_RS232_READ_MODE_EOB) {
	    if ((ch == 4) || (ch == 10) || (ch == 13) || (ch == 26)) {
		eob = 1;
	    }
	}
	break;
    }
#endif

#ifdef CYG_DEVICE_SERIAL_RS232_FLOW_CONTROL
    if (read_throttle == 0) {
#endif

#ifdef CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT
        if (data->read_buffer == NULL) {
	    while ((data->read_buffer = data->read_buffers.get_next_inuse(data->read_buffer))) {
	        if (data->read_buffer->xferred_length < data->read_buffer->buffer_length) {
	    	    ret = Cyg_Interrupt::CALL_DSR;
		    break;
		}
	    }
	    if (data->read_buffer == NULL) {
	        ret = Cyg_Interrupt::CALL_DSR;
		throttle(ch);
		return ret;
    	    }
        }
#endif

        off = data->read_buffer->xferred_length++;
        *((char *)data->read_buffer->buffer + off) = ch;

	// This is still ugly --proven 19980526
	if ((data->read_buffer->xferred_length == data->read_buffer->buffer_length) || eob) {
#ifdef CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT
    	    data->read_buffer = data->read_buffers.get_next_inuse(data->read_buffer);
#else
	    data->read_buffer = NULL;
#endif
	    ret = Cyg_Interrupt::CALL_DSR;
	}

#ifdef CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT
	while ((data->read_buffer != NULL) &&
          (data->read_buffer->xferred_length == data->read_buffer->buffer_length)) {
    	    data->read_buffer = data->read_buffers.get_next_inuse(data->read_buffer);
	    ret = Cyg_Interrupt::CALL_DSR;	// Probably not necessary
        }
#endif

#ifdef CYG_DEVICE_SERIAL_RS232_FLOW_CONTROL
    } else {
	throttle(ch);
    }
#endif

    return ret; 
    
}

void
CYG_CLASS_DEVICE_SERIAL_RS232_TX39::
  read_dsr(cyg_vector vector, cyg_ucount32 dsr_count, CYG_ADDRWORD dsr_data)
{
#ifdef CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT
    CYG_CLASS_DEVICE_SERIAL_RS232_TX39 * data = 
    (CYG_CLASS_DEVICE_SERIAL_RS232_TX39 *)dsr_data;
    cyg_ucount32 count, i;
    Cyg_IORB * iorb;
    
    /*
     * Note: Interrupts are not disabled but if the isr causes another
     * buffer to become done since the start of this call then 
     * dsr_read() will be called again to handle that buffer.
     */
 
    for (i = 0, count = data->read_buffers.min_done(); i < count; i++) {
	iorb = data->read_buffers.dequeue();
	if (iorb->callback)
	    iorb->callback(iorb);
    }

#ifdef CYG_DEVICE_SERIAL_RS232_WRITE_BUFFERS_LL
    // Enqueue as many iorbs as we dequeued
    for (i = 0; i < count; i++) { 
	if ((iorb = data->read_buffers_ll_first)) {
	    data->read_buffers_ll_first = iorb->next;
    	    data->read_buffers.enqueue (iorb);
	    iorb->next = NULL;
	} else {
	    break;
        }
    }
#endif
#endif
}

cyg_int32
CYG_CLASS_DEVICE_SERIAL_RS232_TX39::io_read(Cyg_IORB * iorb)
{
    volatile cyg_uint16 * sdisr = CYG_DEVICE_SERIAL_RS232_TX39_SDISR;
    const cyg_vector vector = CYG_DEVICE_SERIAL_RS232_TX39_VEC;

    iorb->xferred_length = 0;
    iorb->next = NULL;

#ifdef CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT
#ifdef CYG_DEVICE_SERIAL_RS232_READ_BUFFERS_LL
    /*
     * If more iorbs are be queued than the queuet can handle then
     * a separate link list needs to hold the extra. Since the DSR
     * does the dequeueing from the link list we have to enqueue
     * with the scheduler disabled.
     *
     * Note: To prevent lots of scheduler locks and unlocks we check
     * the queue while the scheduler is unlocked, and only if it is
     * full do we lock the scheduler and verify that the queue is
     * still full. It is possible that between the check and the lock
     * that the DSR could run and make some space in the queue.
     *
     * Note: We cannot have any iorbs on the ll if the queue is not full.
     *
     * Note: We cannot have a full queue and also be in polled mode.
     * This is why we return after placing the iorb on the ll if the
     * queue is full, as there is nothing else to do.
     */
    if (read_buffers.min_free() == 0) {

	Cyg_Scheduler::lock();

	// Check again with the scheduler locked
	if (read_buffers.min_free() == 0) {
	    if (read_buffers_ll_first) {
	    	read_buffers_ll_last->next = iorb;
	    } else {
		read_buffers_ll_first = iorb;
	    }
	    read_buffers_ll_last = iorb;
	    Cyg_Scheduler::unlock();
	    return 0;
	}
	Cyg_Scheduler::unlock();
    }

#endif

    read_buffers.enqueue(iorb);

    if (kmode == CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT) {
	/* We may have to enable CTR here to tell the other side to start
	 * sending characters --proven 19980513 */
	return 0;
    } 
#else 
    read_buffer = iorb;
#endif
  
#ifndef CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT
    /* 
     * Special nonblocking mode for polled only drivers. 
     * This is so BSPs and other non kernel things can poll for each char.
     * individually. Use only if read modes don't otherwise solve the problem.
     */
    if (CYG_IORB_NOBLOCK == iorb->opcode) {
	if ((*sdisr & 0x0001) != 0x0001) {
	    read_isr(vector, (CYG_ADDRWORD)this);
	}
        return 0;
    }
#endif

    do {
    	do {
	    while ((*sdisr & 0x0001) != 0x0001) continue;
    	} while (read_isr(vector, (CYG_ADDRWORD)this) == Cyg_Interrupt::HANDLED);
#ifdef CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT
	read_dsr(vector, 0, (CYG_ADDRWORD)this);
#else

        // Make a callback, even in poll-mode.

        // This should really be done in read_dsr, but this requires
        // the iorb to be passed along as an argument (or in
        // read_buffer) which a) isn't pretty, b) would require other
        // callers of read_dsr to know that fact. The read_dsr would
        // extract the iorb and do the below.  This fix seemed cleaner
        // given that it is a short term solution anyway.  -jskov

        if (iorb->callback)
            iorb->callback(iorb);
        
#endif
    } while (read_buffer);
    return 0;
}

// ------------------------------------------------------------------------
// set_write_mode()
// Set the write mode (CRNL translation)
// Currently there are only two modes (BINARY and ASCII)

#ifdef CYG_DEVICE_SERIAL_RS232_WRITE_MODES

cyg_int32
CYG_CLASS_DEVICE_SERIAL_RS232_TX39::
set_write_mode(cyg_uint32 new_mode)
{
#ifdef CYG_DEVICE_SERIAL_RS232_MUTEX
    mutex.lock();
#endif
    write_mode = new_mode;
#ifdef CYG_DEVICE_SERIAL_RS232_MUTEX
    mutex.unlock();
#endif
    return 0;
}

#endif

// ------------------------------------------------------------------------
// write_isr()
//

cyg_uint32
CYG_CLASS_DEVICE_SERIAL_RS232_TX39::
  write_isr(cyg_vector vector, CYG_ADDRWORD isr_data)
{
    CYG_CLASS_DEVICE_SERIAL_RS232_TX39 * data = (CYG_CLASS_DEVICE_SERIAL_RS232_TX39 *)isr_data;
    volatile cyg_uint16 * sdisr  = CYG_DEVICE_SERIAL_RS232_TX39_SDISR;
    volatile cyg_uint8 * tfifo = CYG_DEVICE_SERIAL_RS232_TX39_TFIFO;
    cyg_uint32 off, ret;
    cyg_uint8 ch;

    // Default return value
    ret = Cyg_Interrupt::HANDLED;

#ifdef CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT
    // Clear interrupts regaurdless of kmode
    // data->write_interrupt.acknowledge_interrupt(vector);

    while ((data->write_buffer == NULL) ||
      (data->write_buffer->xferred_length == data->write_buffer->buffer_length)) {
	data->write_buffer = data->write_buffers.get_next_inuse(data->write_buffer);
        ret = Cyg_Interrupt::CALL_DSR;
	if (data->write_buffer == NULL) {
            volatile cyg_uint16 sdicr_v;
            volatile cyg_uint16* sdicr  = CYG_DEVICE_SERIAL_RS232_TX39_SDICR;

	    /* Turn off write interrupts */
    	    sdicr_v = *sdicr & ~0x0002;
	    *sdicr = sdicr_v;
    	    *sdisr = 0x0000;
	    return ret;
	}
    }
#else 
    if (data->write_buffer->xferred_length == data->write_buffer->buffer_length) {
	ret = Cyg_Interrupt::CALL_DSR;
	data->write_buffer = NULL;
	return ret;
    }
#endif

    off = data->write_buffer->xferred_length++;

#ifdef CYG_DEVICE_SERIAL_RS232_WRITE_MODES
    if (data->write_mode) {
        // This is for cr to cr/lf conversion
        if ((ch = data->write_char)) {
	    data->write_char = '\0';
        } else {
	    if ((*((char *)data->write_buffer->buffer + off)) != '\n') {
	        ch = *((char *)data->write_buffer->buffer + off);
	    } else {
	        // Decrement so next pass will DTRT
    	        data->write_buffer->xferred_length--;
	        data->write_char = '\n';
	        ch = '\r';
	    }
        }
    } else
#endif
	ch = *((char *)data->write_buffer->buffer + off);

    *sdisr = 0x0000;
    *tfifo = ch;

    // Do not modify any data structure after the data is written because 
    // it is possible an interrupt will call this routine before this
    // invocation of this routine executes any code beyond this point. 
    return ret;
}

void
CYG_CLASS_DEVICE_SERIAL_RS232_TX39::
  write_dsr(cyg_vector vector, cyg_ucount32 dsr_count, CYG_ADDRWORD dsr_data)
{
#ifdef CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT
    CYG_CLASS_DEVICE_SERIAL_RS232_TX39 * data = 
    (CYG_CLASS_DEVICE_SERIAL_RS232_TX39 *)dsr_data;
    cyg_ucount32 count, i;
    Cyg_IORB * iorb;
    
    /*
     * Note: Interrupts are not disabled but if the isr causes another
     * buffer to become done since the start of this call then 
     * dsr_write() will be called again to handle that buffer.
     */
 
    for (i = 0, count = data->write_buffers.min_done(); i < count; i++) {
	iorb = data->write_buffers.dequeue();
	if (iorb->callback)
	    iorb->callback(iorb);
    }

#ifdef CYG_DEVICE_SERIAL_RS232_WRITE_BUFFERS_LL
    // Enqueue as many iorbs as we dequeued
    for (i = 0; i < count; i++) { 
	if ((iorb = data->write_buffers_ll_first)) {
	    data->write_buffers_ll_first = iorb->next;
    	    data->write_buffers.enqueue (iorb);
	    iorb->next = NULL;
	} else {
	    break;
        }
    }
#endif
#endif

}

cyg_int32
CYG_CLASS_DEVICE_SERIAL_RS232_TX39::io_write(Cyg_IORB * iorb)
{
    volatile cyg_uint16 * sdisr = CYG_DEVICE_SERIAL_RS232_TX39_SDISR;
    const cyg_vector vector = CYG_DEVICE_SERIAL_RS232_TX39_VEC;

    iorb->xferred_length = 0;
    iorb->next = NULL;
    
#ifdef CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT
#ifdef CYG_DEVICE_SERIAL_RS232_WRITE_BUFFERS_LL
    /*
     * If more iorbs are be queued than the queuet can handle then
     * a separate link list needs to hold the extra. Since the DSR
     * does the dequeueing from the link list we have to enqueue
     * with the scheduler disabled.
     *
     * Note: To prevent lots of scheduler locks and unlocks we check
     * the queue while the scheduler is unlocked, and only if it is
     * full do we lock the scheduler and verify that the queue is
     * still full. It is possible that between the check and the lock
     * that the DSR could run and make some space in the queue.
     *
     * Note: We cannot have any iorbs on the ll if the queue is not full.
     *
     * Note: We cannot have a full queue and also be in polled mode.
     * This is why we return after placing the iorb on the ll if the
     * queue is full, as there is nothing else to do.
     */
    if (write_buffers.min_free() == 0) {

	Cyg_Scheduler::lock();

	// Check again with the scheduler locked
	if (write_buffers.min_free() == 0) {
	    if (write_buffers_ll_first) {
	    	write_buffers_ll_last->next = iorb;
	    } else {
		write_buffers_ll_first = iorb;
	    }
	    write_buffers_ll_last = iorb;
	    Cyg_Scheduler::unlock();
	    return 0;
	}
	Cyg_Scheduler::unlock();
    }

#endif

    write_buffers.enqueue (iorb);
    if (kmode == CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT) {
	if (write_buffer == NULL) {
            volatile cyg_uint16 sdicr_v;
            volatile cyg_uint16* sdicr = CYG_DEVICE_SERIAL_RS232_TX39_SDICR;


// Must check that iorb isn't already done --proven 19980516
// if (iorb->length && (iorb->ret_length == 0))
	    /* Prime serial write interrupts with first byte */
//    	    while ((*sdisr & 0x0002) != 0x0002) continue;
//          write_isr (vector, (CYG_ADDRWORD)this);
	    sdicr_v = *sdicr | 0x0002;
	    *sdicr = sdicr_v; 

        }
	return 0;
    }
#else
    write_buffer = iorb;
#endif
  
    do {
    	do {
    	    while ((*sdisr & 0x0002) != 0x0002) continue;
        } while (write_isr(vector, (CYG_ADDRWORD)this) == Cyg_Interrupt::HANDLED);
#ifdef CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT
	write_dsr (vector, 0, (CYG_ADDRWORD)this);
#else

        // Make a callback, even in poll-mode.

        // This should really be done in write_dsr, but this requires
        // the iorb to be passed along as an argument (or in
        // read_buffer) which a) isn't pretty, b) would require other
        // callers of write_dsr to know that fact. The write_dsr would
        // extract the iorb and do the below.  This fix seemed cleaner
        // given that it is a short term solution anyway.  -jskov

        if (iorb->callback)
            iorb->callback(iorb);
        
#endif
    } while (write_buffer);
    return 0;
}

// ------------------------------------------------------------------------
// Multiplexor ISR and DSR routines.

#if defined(CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT) \
    && defined (CYG_DEVICE_SERIAL_RS232_ONE_VECTOR)
cyg_uint32
CYG_CLASS_DEVICE_SERIAL_RS232_TX39::
  both_isr(cyg_vector vector, CYG_ADDRWORD isr_data)
{
    volatile cyg_uint16 sdisr_v, * sdisr  = CYG_DEVICE_SERIAL_RS232_TX39_SDISR;
    cyg_uint32 read_ret, write_ret, ret;

    // Read status
    sdisr_v = *sdisr;

    // Default return value
    ret = Cyg_Interrupt::HANDLED;
    read_ret = Cyg_Interrupt::HANDLED;
    write_ret = Cyg_Interrupt::HANDLED;

    // Clear interrupts regaurdless of kmode
    // data->both_interrupt.acknowledge_interrupt(vector);

    if (sdisr_v & 0x0001) {
	read_ret = CYG_CLASS_DEVICE_SERIAL_RS232_TX39::read_isr(vector, isr_data);
    }
    if (sdisr_v & 0x0002) {
	write_ret = CYG_CLASS_DEVICE_SERIAL_RS232_TX39::write_isr(vector, isr_data);
    }

    if ((read_ret == Cyg_Interrupt::CALL_DSR) || 
      (write_ret == Cyg_Interrupt::CALL_DSR)) 
	ret = Cyg_Interrupt::CALL_DSR;
    *sdisr = 0x0000;
    return ret;
}

void
CYG_CLASS_DEVICE_SERIAL_RS232_TX39::
  both_dsr(cyg_vector vector, cyg_ucount32 dsr_count, CYG_ADDRWORD dsr_data)
{
    CYG_CLASS_DEVICE_SERIAL_RS232_TX39::read_dsr(vector, 0, dsr_data);
    CYG_CLASS_DEVICE_SERIAL_RS232_TX39::write_dsr(vector, 0, dsr_data);
}
#endif

// ------------------------------------------------------------------------
// Asynchronous versions are only configured with the 
// interrupt version is configured 
//
#ifdef CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT

cyg_int32
CYG_CLASS_DEVICE_SERIAL_RS232_TX39::io_read_asynchronous(Cyg_IORB * iorb)
{
    // Do we want to configure an error condition for this? --proven 19980506
    CYG_ASSERT (kmode == CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT, 
      "Cannot do an io_read_asynchronous while in polled mode" );

#ifdef CYG_DEVICE_SERIAL_RS232_MUTEX
    mutex.lock();
#endif
    this->io_read (iorb);
#ifdef CYG_DEVICE_SERIAL_RS232_MUTEX
    mutex.unlock();
#endif
    return 0;

}

cyg_int32
CYG_CLASS_DEVICE_SERIAL_RS232_TX39::io_write_asynchronous(Cyg_IORB * iorb)
{
    // Do we want to configure an error condition for this? --proven 19980506
    CYG_ASSERT (kmode == CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT, 
      "Cannot do an io_write_asynchronous while in polled mode" );

#ifdef CYG_DEVICE_SERIAL_RS232_MUTEX
    mutex.lock();
#endif
    this->io_write (iorb);
#ifdef CYG_DEVICE_SERIAL_RS232_MUTEX
    mutex.unlock();
#endif
    return 0;
}
#endif

// ------------------------------------------------------------------------
// Blocking versions just call the internal version with a callback.
//

#ifdef CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT
static void
callback(Cyg_IORB *iorb)
{
    Cyg_Binary_Semaphore * data = (Cyg_Binary_Semaphore *)iorb->callback_data;
    data->post();
}
#endif

cyg_int32
CYG_CLASS_DEVICE_SERIAL_RS232_TX39::io_read_blocking(Cyg_IORB * iorb)
{
#ifdef CYG_DEVICE_SERIAL_RS232_MUTEX
    mutex.lock();
#endif
#ifdef CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT
    if (kmode == CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT) {
    	Cyg_Binary_Semaphore read(0);

	// We need a callback routine to wake us up
	iorb->callback_data = (CYG_ADDRESS)&read;
	iorb->callback = callback;
    	this->io_read(iorb);
#ifdef CYG_DEVICE_SERIAL_RS232_MUTEX
   	 mutex.unlock();
#endif
	read.wait();
	return 0;
    } else 
#endif
    {
	iorb->callback = NULL;
        this->io_read(iorb);
    }
#ifdef CYG_DEVICE_SERIAL_RS232_MUTEX
    mutex.unlock();
#endif
    return 0;
}

cyg_int32
CYG_CLASS_DEVICE_SERIAL_RS232_TX39::io_write_blocking(Cyg_IORB * iorb)
{
#ifdef CYG_DEVICE_SERIAL_RS232_MUTEX
    mutex.lock();
#endif
#ifdef CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT
    if (kmode == CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT) {
    	Cyg_Binary_Semaphore write(0);

	// We need a callback routine to wake us up
	iorb->callback_data = (CYG_ADDRESS)&write;
	iorb->callback = callback;
    	this->io_write(iorb);
#ifdef CYG_DEVICE_SERIAL_RS232_MUTEX
   	 mutex.unlock();
#endif
	write.wait();
	return 0;
    } else 
#endif
    {
	iorb->callback = NULL;
    	this->io_write(iorb);
    }
#ifdef CYG_DEVICE_SERIAL_RS232_MUTEX
    mutex.unlock();
#endif
    return 0;
}

// Internal routine needed to flush writes in a blocking manner.
void
CYG_CLASS_DEVICE_SERIAL_RS232_TX39::io_write_flush(void)
{
    Cyg_IORB iorb;

    iorb.buffer_length = 0;
#ifdef CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT
    if (kmode == CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT) {
    	Cyg_Binary_Semaphore write(0);

	// We need a callback routine to wake us up
	iorb.callback_data = (CYG_ADDRESS)&write;
	iorb.callback = callback;
    	this->io_write(&iorb);
	write.wait();
    } else 
#endif
    {
	iorb.callback = NULL;
    	this->io_write(&iorb);
    }
}

// ------------------------------------------------------------------------
// Assert versions. These versions bypass most of the driver code.
// These are to be used by the BSP or asserts. Only use them once
// start_sync() is called and when done normal operations is 
// restarted with the end_sync() mode. 
//
// Note DO NOT DO LOCKING IN THIS ROUTINE!!!
//

#ifdef CYG_DEVICE_SERIAL_RS232_TX39_KMODE_ASSERT
cyg_int32
CYG_CLASS_DEVICE_SERIAL_RS232_TX39::io_write_assert(Cyg_IORB * iorb)
{
    volatile cyg_uint16 sdisr_v, * sdisr = CYG_DEVICE_SERIAL_RS232_TX39_SDISR;
    volatile cyg_uint8 ch, * tfifo = CYG_DEVICE_SERIAL_RS232_TX39_TFIFO;

    iorb->xferred_length = 0;
    while (iorb->xferred_length < iorb->buffer_length) {
	if ((ch = (*((char*)iorb->buffer + iorb->xferred_length++))) == '\n') {
    	    while (((sdisr_v = *sdisr) & 0x0002) != 0x0002) continue;
	    *sdisr = (sdisr_v & ~0x0002);
	    *tfifo = '\r';
	}
    	while (((sdisr_v = *sdisr) & 0x0002) != 0x0002) continue;
	*sdisr = (sdisr_v & ~0x0002);
	*tfifo = ch;
    }
    return 0;
}

#endif // #ifdef CYG_DEVICE_SERIAL_RS232_TX39_KMODE_ASSERT

#endif // #ifdef CYGPKG_DEVICES_SERIAL_RS232_TX39
// EOF serial_tx39.cxx
