//=================================================================
//
//        gdb_1.cxx
//
//        Pseudo Driver for GDB
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
// Date:          1998-05-30
// Description:   Class methods for class Cyg_Device_GDB_1
//####DESCRIPTIONEND####

#include <pkgconf/devs.h>		// To see if we need to bother
#ifdef CYG_DEVICE_GDB_1 		// Do we need to build this file

#define CYG_DEVICE_INTERNAL
#include <cyg/devs/gdb/gdb_1.hxx>
#include <cyg/kernel/sema.hxx>          // Cyg_Binary_Semaphore
#include <stddef.h>			// NULL

#ifdef CYG_DEVICE_GDB_1_NAME // Only construct object if there is a name
CYG_CLASS_DEVICE_GDB_1 CYG_DEVICE_GDB_1_NAME;
#endif

// ------------------------------------------------------------------------
// Constructor for GDB device                 
//
CYG_CLASS_DEVICE_GDB_1::CYG_CLASS_DEVICE_GDB_1()
    : CYG_DEVICE_GDB_1_CLASS()
{
    CYG_REPORT_FUNCTION();

    // Initialize any data

    // write_buffers = NULL; 

}

// ------------------------------------------------------------------------
// Routines to deal with the new iorb
//

static const char hexchars[] = "0123456789abcdef";

static cyg_uint32 
iorb_compute(char * in_data, cyg_uint32 length, Cyg_IORB * out)
{
    cyg_uint8 csum = 0;
    cyg_ucount8 i;
    char * data;

    data = (char *)out->buffer;

    if (length > CYGNUM_DEVICE_GDB_BUFFER_SIZE) {
	length = CYGNUM_DEVICE_GDB_BUFFER_SIZE;
    }

    data[0] = '$';
    data[1] = 'O';
    csum += data[1];
    for (i = 0; i < length; i++) {
	data[(i*2)+2] = hexchars[(in_data[i] >> 4) & 0x0f];
	data[(i*2)+3] = hexchars[in_data[i] & 0x0f];
        csum += data[(i*2)+2];
        csum += data[(i*2)+3];
    }
    data[(i*2)+2] = '#';
    data[(i*2)+3] = hexchars[csum >> 4];
    data[(i*2)+4] = hexchars[csum & 0x0f];

    out->buffer_length = (i * 2) + 5;
    return (length);
}

void
CYG_CLASS_DEVICE_GDB_1::iorb_setup(CYG_CLASS_DEVICE_GDB_1 * data)
{
    Cyg_IORB * iorb;

    iorb = data->gdb_write_buffer_first;

    /* set up gdb_write_iorb */
    data->gdb_write_iorb.buffer = data->gdb_write_buf;
    data->gdb_write_iorb.callback = iorb_callback;
    data->gdb_write_iorb.callback_data = (CYG_ADDRWORD)data;
    iorb->xferred_length = iorb_compute((char *)iorb->buffer, iorb->buffer_length, &data->gdb_write_iorb);

    /* Save the length of the original data that still needs to be sent */
    data->gdb_write_length = iorb->buffer_length - iorb->xferred_length;

    /* Start the write process */
    data->io_write(&data->gdb_write_iorb);
}

void
CYG_CLASS_DEVICE_GDB_1::iorb_callback(Cyg_IORB *iorb)
{
    CYG_CLASS_DEVICE_GDB_1 * foo = (CYG_CLASS_DEVICE_GDB_1 *)iorb->callback_data;
    cyg_uint32 length;

    if (foo->gdb_write_length) {
        /* Compute the offset into the original data */
        length = iorb_compute ((char *)foo->gdb_write_buffer_first->buffer + 
          foo->gdb_write_buffer_first->xferred_length, foo->gdb_write_length, 
          &foo->gdb_write_iorb);

        foo->gdb_write_buffer_first->xferred_length += length;
        foo->gdb_write_length -= length;

        foo->io_write(&foo->gdb_write_iorb);
        
    } else {
	Cyg_IORB * done;

	/* Get the next iorb */
	done = foo->gdb_write_buffer_first;
	foo->gdb_write_buffer_first = foo->gdb_write_buffer_first->next;
	if (foo->gdb_write_buffer_first == NULL) {
            foo->gdb_write_buffer_last = NULL;
	} else {
            foo->iorb_setup(foo);
        }
	if (done->callback) {
            done->callback(done);
	}
    }
}

// ------------------------------------------------------------------------
// Asynchronous write
//

cyg_int32
CYG_CLASS_DEVICE_GDB_1::io_write_asynchronous(Cyg_IORB * iorb)
{
    iorb->next = NULL;
    Cyg_Scheduler::lock();
    if (gdb_write_buffer_first == NULL) {
        gdb_write_buffer_first = iorb;
        gdb_write_buffer_last = iorb;
        Cyg_Scheduler::unlock();
        iorb_setup(this);
    } else {
        gdb_write_buffer_last->next = iorb;
        gdb_write_buffer_last = iorb;
        Cyg_Scheduler::unlock();
    }
    return 0;    
}

// ------------------------------------------------------------------------
// Synchronous write
//

static void
callback(Cyg_IORB *iorb)
{
    Cyg_Binary_Semaphore * data = (Cyg_Binary_Semaphore *)iorb->callback_data;
    data->post();
}

cyg_int32
CYG_CLASS_DEVICE_GDB_1::io_write_blocking(Cyg_IORB * iorb)
{
    Cyg_Binary_Semaphore write(0);

    // We need a callback routine to wake us up
    iorb->callback_data = (CYG_ADDRESS)&write;
    iorb->callback = callback;
    iorb->next = NULL;

    Cyg_Scheduler::lock();
    if (gdb_write_buffer_first == NULL) {
        gdb_write_buffer_last = iorb;
        gdb_write_buffer_first = iorb;
    } else {
        gdb_write_buffer_last->next = iorb;
        gdb_write_buffer_last = iorb;
        Cyg_Scheduler::unlock();
        write.wait();
        return 0;
    }

    Cyg_Scheduler::unlock();
    iorb_setup(this);
    write.wait();
    return 0;
}

// ------------------------------------------------------------------------
// Assert versions. These versions bypass most of the driver code.
// These are to be used by the BSP or asserts. Only use them once
// start_sync() is called and when done normal operations is
// restarted with the end_sync() mode.
//
// Note DO NOT DO LOCKING IN THIS ROUTINE!!!
//

cyg_int32
CYG_CLASS_DEVICE_GDB_1::io_write_assert(Cyg_IORB * iorb)
{
    Cyg_IORB assert_iorb;
    char assert_buffer[256];
    cyg_uint32 length, length_left;

    assert_iorb.callback = NULL;
    assert_iorb.callback_data = 0;
    assert_iorb.buffer = assert_buffer;
    for (length_left = iorb->buffer_length; length_left; length_left -= length) {
        length = iorb_compute((char *)iorb->buffer + iorb->xferred_length,
          length_left, &assert_iorb);
	iorb->xferred_length += length;
        (CYG_DEVICE_GDB_1_CLASS *)this->io_write_assert(&assert_iorb);
    }

    return 0;
}

#endif // #ifdef CYG_DEVICE_GDB_1
// EOF gdb_1.cxx
