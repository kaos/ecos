/*****************************************************************************
* record.c - PPP recording program file.
*
* Copyright (c) 2009 by Simon Kallweit, intefo AG
*
* The authors hereby grant permission to use, copy, modify, distribute,
* and license this software and its documentation for any purpose, provided
* that existing copyright notices are retained in all copies and that this
* notice and the following disclaimer are included verbatim in any
* distributions. No written agreement, license, or royalty fee is required
* for any of the authorized uses.
*
* THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS *AS IS* AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
* REVISION HISTORY
*
* 09-02-03 Simon Kallweit <simon.kallweit@intefo.ch>
*   First implementation.
*****************************************************************************/

#include "lwip/opt.h"

#if PPP_SUPPORT

#if RECORD_SUPPORT

#include "netif/ppp/ppp.h"
#include "record.h"

/***********************************/
/*** PUBLIC FUNCTION DEFINITIONS ***/
/***********************************/

static u32_t now;
static u8_t buf[1024];

extern void ppp_dump_record(void *data, int len);

/*
 * write_record - Writes a single record. 
 */
static void write_record(u8_t code, u8_t *data, int len)
{
    static u32_t last;
    u32_t diff;
    u8_t *b = buf;
    
    diff = now - last;
    if (diff > 0) {
        if (diff > 255) {
            /* Absolute timestamp */
            *b++ = 0x05;
            *b++ = (diff >> 24);
            *b++ = (diff >> 16);
            *b++ = (diff >> 8);
            *b++ = diff;
        } else {
            /* Relative timestamp */
            *b++ = 0x06;
            *b++ = diff;
        }
        last = now;
    }
    
    *b++ = code;
    *b++ = (len >> 8);
    *b++ = len;
    while (len-- && b < (buf + sizeof(buf)))
        *b++ = *data++;
    
    ppp_dump_record(buf, b - buf);
}

/*
 * record_init - Initialize the recording module.
 */
void record_init()
{
    u8_t *b = buf;

    /* Write start marker */
    *b++ = 0x07;
    *b++ = 0;
    *b++ = 0;
    *b++ = 0;
    *b++ = 0;
    ppp_dump_record(buf, b - buf);
}

/*
 * record_update - Updates the current time.
 */
void record_update(int ms)
{
    /* Recording timer resolution is 0.1 s */ 
    now += ms / 10;
}

/*
 * record_in - Dumps incoming data.
 */
void record_in(void *data, int len)
{
    write_record(2, data, len);
}

/*
 * record_out - Dumps outgoing data.
 */
void record_out(void *data, int len)
{
    write_record(1, data, len);
}

#endif /* RECORD_SUPPORT */

#endif /* PPP_SUPPORT */
