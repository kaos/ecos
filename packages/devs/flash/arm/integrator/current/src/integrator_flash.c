//==========================================================================
//
//      integrator_flash.c
//
//      Flash programming
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
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: Philippe Robin
// Date:         November 7, 2000
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_cache.h>

#define  _FLASH_PRIVATE_
#include <cyg/io/flash.h>

#include "flash.h"

#define ARM_INTEGRATOR_FLASH

extern int strncmp(const char *s1, const char *s2, int len);

#define INTEGRATOR_NUM_REGIONS 256

int
flash_hwr_init(void)
{
    unsigned char data[96];
    extern char flash_query, flash_query_end;
    typedef int code_fun(unsigned char *);
    code_fun *_flash_query;
    int code_len, stat, num_regions, region_size;

    // Copy 'program' code to RAM for execution
    code_len = (unsigned long)&flash_query_end - (unsigned long)&flash_query;
    _flash_query = (code_fun *)flash_info.work_space;
    memcpy(_flash_query, &flash_query, code_len);
    HAL_DCACHE_SYNC();  // Should guarantee this code will run
    HAL_ICACHE_DISABLE(); // is also required to avoid old contents

    stat = (*_flash_query)(data);

    HAL_ICACHE_ENABLE();

    if ( data[4] == FLASH_28F320 ) {
        num_regions = INTEGRATOR_NUM_REGIONS;
        region_size = FLASH_BLOCK_SIZE;
        flash_info.block_size = region_size;
        flash_info.blocks = num_regions;
        flash_info.start = (void *)0x24000000;
        flash_info.end = (void *)(0x24000000+(num_regions*region_size));
        return FLASH_ERR_OK;
    } else {
        diag_printf("Can't identify FLASH, sorry\n");
        diag_dump_buf(data, sizeof(data));
        return FLASH_ERR_HWR;
    }
}

// Map a hardware status to a package error
int
flash_hwr_map_error(int err)
{
    if (err & 0x7E) {
        if (err & 0x10) {
            return FLASH_ERR_PROGRAM;
        } else 
        if (err & 0x20) {
            return FLASH_ERR_ERASE;
        } else 
        return FLASH_ERR_HWR;  // FIXME
    } else {
        return FLASH_ERR_OK;
    }
}

// See if a range of FLASH addresses overlaps currently running code
bool
flash_code_overlaps(void *start, void *end)
{
    extern char _stext, _etext;

    return ((((unsigned long)&_stext >= (unsigned long)start) &&
             ((unsigned long)&_stext < (unsigned long)end)) ||
            (((unsigned long)&_etext >= (unsigned long)start) &&
             ((unsigned long)&_etext < (unsigned long)end)));
}


#ifdef ARM_INTEGRATOR_FLASH
#define FLASH_BASE	   0x24000000
#define FLASH_SIZE	   0x02000000

/*     type information :-
 * 0xffff ffff     -       deleted (usual flash erased value)
 * 0x0000 xxxx     -       Reserved for ARM usage:
 * Bit 0           -       ARM Executable Image
 * Bit 1           -       System Information Block
 * Bit 2           -       File System Block
 * 0xyyyy 0000     -       Available for customers (y != 0)
 */
#define TYPE_DELETED       0xFFFFFFFF
#define TYPE_ARM_MASK      0x0000FFFF
#define TYPE_CUSTOM_MASK   0xFFFF0000
#define TYPE_ARM_EXEC      0x00000001
#define TYPE_ARM_SIB       0x00000002
#define TYPE_ARM_SYSBLOCK  0x00000004

/* Filetypes */

#define  UNKNOWN_FILE      0x00000000
#define  MOT_S_RECORD      0x00000001
#define  INTEL_HEX         0x00000002
#define  ELF               0x00000004
#define  DWARF             0x00000008
#define  ARM_AOF           0x00000010
#define  ARM_AIF           0x00000020
#define  PLAIN_BINARY      0x00000040
#define  ARM_AIF_BIN       0x00000080
#define  MCS_TYPE          0x00000100

#define  CONVERT_TYPE      (MOT_S_RECORD | INTEL_HEX | MCS_TYPE | ELF)

#define SIB_OWNER_STRING_SIZE   32
#define MAX_SIB_SIZE            512
#define MAX_SIB_INDEX		64

#define NoOfElements(array)	(sizeof(array) / sizeof(array[0]))
#define AddressToBlock(address) ((((cyg_uint32)(address)) - FLASH_BASE) / FLASH_BLOCK_SIZE)
#define BlockToAddress(block)   (((block) * FLASH_BLOCK_SIZE) + FLASH_BASE)
#define BlockBaseAddress(address) (((cyg_uint32)(address)) & ~(FLASH_BLOCK_SIZE - 1))
#define FooterAddress(block)    ((tFooter *)((BlockToAddress(block) + FLASH_BLOCK_SIZE) \
						- sizeof (tFooter)))
#define InFlash(address)        ((((cyg_uint32)(address)) >= FLASH_BASE) && (((cyg_uint32)(address))\
					 < (FLASH_BASE + FLASH_SIZE)))

#define NO_OF_BLOCKS    (FLASH_SIZE / FLASH_BLOCK_SIZE)

#define SIB_HEADER_SIGNATURE    0xA00FFF9F /* This is an invalid instruction - MULGE pc,pc,pc */
#define FLASH_FOOTER_SIGNATURE  0xA0FFFF9F /* This is an invalid instruction - SMULALGES pc,pc,pc */


typedef struct SIBType {
    cyg_uint32  signature;
    cyg_uint32  size;
    char        owner[SIB_OWNER_STRING_SIZE];
    cyg_uint32  index;
    cyg_uint32  revision;
    cyg_uint32  checksum;
} tSIB; 

typedef struct SIBInfoType {
    cyg_uint32 SIB_number;      /* Unique number of SIB Block            */
    cyg_uint32 SIB_Extension;   /* Base of SIB Flash Block               */
    char Label[16];             /* String space for ownership string     */
    cyg_uint32 checksum;        /* SIB Image checksum                    */
} tSIBInfo;

typedef struct FooterType {
    void        *infoBase;      /* Address of first word of ImageFooter  */
    char        *blockBase;     /* Start of area reserved by this footer */
    cyg_uint32  signature;      /* 'Magic' number proves it's a footer   */
    cyg_uint32  type;           /* Area type: ARM Image, SIB, customer   */
    cyg_uint32  checksum;       /* Just this structure                   */
} tFooter ;

typedef struct ImageInfoType {
    cyg_uint32 bootFlags;       /* Boot flags, compression etc.          */
    cyg_uint32 imageNumber;     /* Unique number, selects for boot etc.  */
    char *loadAddress;          /* Address program should be loaded to   */
    cyg_uint32 length;          /* Actual size of image                  */
    char *address;                /* Image is executed from here           */
    char name[16];              /* Null terminated                       */
    char *headerBase;           /* Flash Address of any stripped header  */
    cyg_uint32 header_length;   /* Length of header in memory            */
    cyg_uint32 headerType;      /* AIF, RLF, s-record etc.               */
    cyg_uint32 checksum;        /* Image checksum (inc. this struct)     */
} tImageInfo;

/* globals */

static tFooter 		*active_flash_footers[NO_OF_BLOCKS];
static tSIB    		*active_sibs[MAX_SIB_INDEX];
static char 		active_id_string[SIB_OWNER_STRING_SIZE];
static cyg_uint32 	buffer[MAX_SIB_SIZE / sizeof(cyg_uint32)];
static cyg_uint32 	*free_ptr;
static cyg_uint32 	free_space;

//
// find_free_space()
//
// This routine searches backwards from the end of flash, this is to ensure 
// that any SIB's are at the end of flash and therefore less likely to be 
// overwritten by the boot monitor loader.  
//
// RETURNS: NULL or pointer on area of free space

static cyg_uint32 *
find_free_space(int size)
{
    tFooter *footer;
    int     required_blocks;
    int     free_blocks;

    //Calculate the number of blocks required.
    required_blocks = (size / FLASH_BLOCK_SIZE) + 1;

    // Get a pointer to the last possible footer in flash.
    footer = (tFooter *)(FLASH_BASE + FLASH_SIZE - sizeof(tFooter));

    free_blocks = 0;

    while (free_blocks < required_blocks) {
        // Skip pass any allocated blocks
        while (footer->signature == FLASH_FOOTER_SIGNATURE) {
            footer = (tFooter *)(footer->blockBase - sizeof (tFooter));

            if ((cyg_uint32)footer < FLASH_BASE)
                return NULL;
	}

        // Count free blocks
        free_blocks = 0;

        while (footer->signature != FLASH_FOOTER_SIGNATURE) {
            free_blocks++;

            // Have we found enough.
            if (free_blocks >= required_blocks)
                break;

            footer = (tFooter *)((cyg_uint32)footer - FLASH_BLOCK_SIZE);

            if ((cyg_uint32)footer < FLASH_BASE)
                return NULL;
	}
    }

    // Return the address of the first location of the free space.
    return (cyg_uint32 *)BlockBaseAddress(footer);
}

//
// arm_fis_find_free()
//

bool
arm_fis_find_free(cyg_uint32 *addr, cyg_uint32 size)
{
    tFooter *footer;
    int     required_blocks;
    int     free_blocks;

    //Calculate the number of blocks required.
    required_blocks = (size / FLASH_BLOCK_SIZE) + 1;

    footer = FooterAddress(AddressToBlock(*addr));
    free_blocks = 0;

    while (free_blocks < required_blocks) {
        // Skip pass any allocated blocks
        while (footer->signature == FLASH_FOOTER_SIGNATURE) {
            footer = (tFooter *)((cyg_uint32)footer + FLASH_BLOCK_SIZE);

            if ((cyg_uint32)footer > (FLASH_BASE + FLASH_SIZE))
                return 0;
	}

        // Count free blocks
        free_blocks = 0;
	*addr = (tFooter *)((cyg_uint32)footer + sizeof(tFooter) - FLASH_BLOCK_SIZE);

        while (footer->signature != FLASH_FOOTER_SIGNATURE) {
            free_blocks++;

            // Have we found enough.
            if (free_blocks >= required_blocks)
                break;

            footer = (tFooter *)((cyg_uint32)footer + FLASH_BLOCK_SIZE);

            if ((cyg_uint32)footer > (FLASH_BASE + FLASH_SIZE))
                return 0;
	}
    }

    // Return the address of the first location of the free space.
    return 1;
}

//
//  sib_checksum() - calculate SIB checksum.
//

static cyg_uint32 
sib_checksum(tSIB *sib)
{
    cyg_uint32	checksum;
    cyg_uint32  *ptr;
    int         size;

    checksum    = 0;
    size        = sib->size;
    ptr         = (cyg_uint32 *)sib;

    while (size > 0) {
        cyg_uint32 word;

        word = *ptr++;

        if (word > ~checksum) checksum++;

        checksum += word;
        size -= sizeof (word);
    }

    return ~checksum;
}

// footer_checksum() - calculate the footer checksum.
static cyg_uint32 
footer_checksum(tFooter *footer)
{
    cyg_uint32	checksum;
    cyg_uint32	*ptr;
    int         size;

    checksum    = 0;
    size        = sizeof (tFooter);
    ptr         = (cyg_uint32 *)footer;

    while (size > 0) {
        cyg_uint32 word;

        word = *ptr++;

        if (word > ~checksum) checksum++;

        checksum += word;
        size -= sizeof (word);
    }

    return ~checksum;
}

// write_sib() - internal routine to write a SIB to flash.
//
// RETURN: -1 if error 0 otherwise

static int 
write_sib(tSIB *sib)
{
    cyg_uint32  *ptr;

    // Is there space available for this SIB.
    if (sib->size > free_space) {
        int required_space;
        int count;
        int block;
        tFooter footer;
        tFooter *footer_ptr;

        // If not then we need to allocate a new area in flash.
        //
        // First calculate the size required for the new area by
        // adding up the size of all the active SIB's.
        //
        required_space = sib->size;

        for (count = 0; count < MAX_SIB_INDEX; count++) {
            tSIB *sib2;

            sib2 = active_sibs[count];

            if (sib2 != NULL) required_space += sib2->size;
	}

        // Get a pointer to the free space.
        free_ptr = find_free_space (required_space);

        // Build a flash footer to describe this area.
        footer.blockBase    = (char *)BlockBaseAddress(free_ptr);
        footer.infoBase     = NULL;
        footer.type         = TYPE_ARM_SIB;
        footer.signature    = FLASH_FOOTER_SIGNATURE;
        footer.checksum     = 0;
        footer.checksum     = footer_checksum (&footer);

        // Calculate the address where the footer will live in flash
        // and write it to flash.
        block = AddressToBlock((cyg_uint32)free_ptr + required_space + sizeof (tFooter));
        
        footer_ptr = FooterAddress(block);

#ifdef TO_CHECK
	if (sysFlashWrite(FLASH_CAST(&footer), sizeof(tFooter), 
			  ((cyg_uint32)footer_ptr - FLASH_BASE),
			  flashType, 0) != OK)
	    return -1;
#endif
	if (flash_program_buf(footer_ptr, (unsigned long *)&footer, sizeof(tFooter)) < 0 )
	    return -1;

        // Update the free space global varible.
        free_space = ((AddressToBlock(footer_ptr) - block + 1) * FLASH_BLOCK_SIZE)
			- sizeof (tFooter);
    }

    // If the SIB that we are writing is already in flash then it need to
    // be copied into memory as it may become unreadable when we start
    // programming the flash (if it is in the same flash part).
    if (InFlash(sib)) {
        cyg_uint32  *src;
        cyg_uint32  *dest;
        int         size;
        tSIB        *tmp_sib;

        size    = sib->size;
        src     = (cyg_uint32 *)sib;
        dest    = buffer;

        while (size > 0) {
            *dest++ = *src++;
            size -= sizeof (cyg_uint32);
	}

        // Increment the revision count.
        tmp_sib = (tSIB *)buffer;
        tmp_sib->revision = sib->revision + 1;

        ptr = buffer;
    } else
        ptr = (cyg_uint32 *)sib;

    // Write the SIB to flash.
#ifdef TO_CHECK
    if (sysFlashWrite(FLASH_CAST(ptr), sib->size, (int)((cyg_uint32)free_ptr - FLASH_BASE), 
		      flashType, 0) != OK)
        return -1;
#endif

    if (flash_program_buf((cyg_uint32)free_ptr, ptr, sib->size) < 0 ) 
        return -1;

    // Update the free space, free pointer and active SIB's varibles
    // to reflect this write.
    free_space  -= sib->size;
    free_ptr    = (cyg_uint32 *)((cyg_uint32)free_ptr - sib->size);

    active_sibs[sib->index] = sib;

    return 0;
}

// delete_sib() - delete a SIB.
//
// This will delete at least a whole flash block and therefore any active
// SIB's will need to be rewritten.

static int 
delete_sib (tSIB *sib_to_delete)
{
    int         block;
    tFooter     *footer;
    tSIB        *sib;    
    int         delete_size;

    // Calculate the first address of this flash block and the
    // first possible footer location.
    //
    block   = AddressToBlock(sib_to_delete);
    footer  = FooterAddress(block);

    // Search for the footer for this block.
    while (footer->signature != FLASH_FOOTER_SIGNATURE)
        footer = (tFooter *)((cyg_uint32)footer + FLASH_BLOCK_SIZE);

    if (footer_checksum (footer) != 0)
        return -1;

    // Now get the a pointer to the first SIB.
    sib = (tSIB *)footer->blockBase;

    // Now search through all the SIB's.
    while (sib->signature == SIB_HEADER_SIGNATURE) {
        if (sib_checksum (sib) != 0)
            break;
  
	// Any valid and active SIB will need to be rewritten.
        if (sib->index < MAX_SIB_INDEX) {
            if ((active_sibs[sib->index] == NULL) || 
                (sib->revision > active_sibs[sib->index]->revision)) {
                write_sib (sib);
	    }
	}

        // Move onto next SIB.
        sib = (tSIB *)((cyg_uint32)sib + sib->size);
    }

    // Calculate how much space needs to be deleted, it may be multiple
    // blocks, and delete it.
    delete_size = (AddressToBlock(footer) - block + 1) * FLASH_BLOCK_SIZE;

    // !! must add code for cases where (delete_size > block_size)
#ifdef TO_CHECK
    return sysFlashEraseBlock((cyg_uint32 *)footer->blockBase, sysFlashTypeGet());
#endif
    return flash_erase_block((cyg_uint32 *)footer->blockBase);
    }

// SIB_Open() -  Open all SIB's with the owner by 'id_string'.
//
// RETURN: -1 if error 0 otherwise

int 
SIB_Open(char *id_string, int *sib_count, int private_flag)
{
    int     count;
    tFooter *footer;
    int     no_of_sibs;

    // Initialize various globals.
    memset(active_id_string, 0, SIB_OWNER_STRING_SIZE);
    free_ptr    = NULL;
    free_space  = 0;
    *sib_count  = 0;

    // Find all the active footers.
    fLib_FindFooter((cyg_uint32 *)FLASH_BASE, FLASH_SIZE, active_flash_footers);

    // Initialise the active SIB array.
    for (count = 0; count < MAX_SIB_INDEX; count++)
        active_sibs[count] = NULL;

    // Populate the active SIB array with all the active SIB found in flash.
    count       = 0;
    no_of_sibs  = 0;

    while ((footer = active_flash_footers[count++]) != NULL) {
        tSIB    *sib;

        if (footer->type != TYPE_ARM_SIB)
            continue;

        sib = (tSIB *)footer->blockBase;

        while (sib->signature == SIB_HEADER_SIGNATURE) {
            char    c;
            char    *ptr1;
            char    *ptr2;

            if (sib_checksum (sib) != 0)
                return -1;

            ptr1 = id_string;
            ptr2 = sib->owner;

            while ((c = *ptr1++) != '\0') {
                if (c != *ptr2++)
                    break;
	    }

            if (c == '\0') {
	        // If two SIB's have the same name and index the one with the
	        // highest revision is considered to be the active one.
                if (sib->index < MAX_SIB_INDEX) {
                    tSIB *current_sib;

                    current_sib = active_sibs[sib->index];

                    if (current_sib == NULL) {
                        active_sibs[sib->index] = sib;
		    } else if (sib->revision > current_sib->revision) {
                        delete_sib(current_sib);
                        active_sibs[sib->index] = sib;
		    } else {
                        tSIB *old_sib;

                        old_sib = sib;
                        sib = (tSIB *)((cyg_uint32)sib + sib->size); 
                        
                        delete_sib(old_sib);

                        continue;
		    }
		} else
                    return -1;

                no_of_sibs++;
	    }

            sib = (tSIB *)((cyg_uint32)sib + sib->size);
	}
    }

    memcpy(active_id_string, id_string, SIB_OWNER_STRING_SIZE);

    *sib_count = no_of_sibs;

    return 0;
}

// SIB_Close() - close access to SIBs

int 
SIB_Close(void)
{
    int count;

    memset(active_id_string, 0, SIB_OWNER_STRING_SIZE);

    // Initialise the active SIB array.
    for (count = 0; count < MAX_SIB_INDEX; count++)
        active_sibs[count] = NULL;

    return 0;
}


// SIB_GetSize() - Get the size of the user data in a SIB.

int 
SIB_GetSize(int sib_index, int *data_size)
{
    tSIB    *sib;

    sib = active_sibs[sib_index];

    if (sib == NULL) {
        *data_size = 0;
        return -1;
    }

    *data_size = sib->size = sizeof(tSIB);

    return 0;
}

// SIB_Program() - create or update a SIB with new user data.
//
// Return: 0 if OK -1 otherwise

int 
SIB_Program(int sib_index, void *data_block, int data_size)
{
    tSIB    *old_sib;
    tSIB    *new_sib;
    int     sib_size;
    cyg_uint32  *src;
    cyg_uint32  *dest;

    // Calculate the size of the SIB as a whole.
    sib_size = data_size + sizeof(tSIB);

    if (sib_size > MAX_SIB_SIZE)
        return -1;

    // Create the new SIB in local memory
    new_sib = (tSIB *)&buffer;

    new_sib->signature   = SIB_HEADER_SIGNATURE;
    new_sib->size        = sib_size;
    new_sib->index       = sib_index;

    memcpy(new_sib->owner, active_id_string, SIB_OWNER_STRING_SIZE);

    // Increment the revision number if this is updating a SIB.
    old_sib = active_sibs[sib_index];

    if (old_sib == NULL)
        new_sib->revision = 0;
    else
        new_sib->revision = old_sib->revision + 1;

    // Copy the user data into the SIB.
    src     = (cyg_uint32 *)data_block;
    dest    = (cyg_uint32 *)((cyg_uint32)new_sib + sizeof(tSIB));

    while (data_size > 0) {
        *dest++ = *src++;
        data_size -= sizeof (cyg_uint32);
    }

    // Finally calculate the checksum and call write_sib.
    new_sib->checksum = 0;
    new_sib->checksum = sib_checksum (new_sib);

    if (write_sib (new_sib) != 0)
        return -1;
 
    // Delete the SIB that this one replaces if there is one.
    if (old_sib != NULL)
        delete_sib (old_sib);

    return 0;
}

// SIB_Erase() - erase a SIB
//
// Return: -1 if error 0 otherwise

int 
SIB_Erase(int sib_index)
{
    tSIB *sib;

    sib = active_sibs[sib_index];

    active_sibs[sib_index] = NULL;
    
    return delete_sib (sib);
}

// fLib_FindFooter() - find footer in flash memory
//
// Scan the flash memory from start for size bytes, returning a list of pointers to the
// image footers. Pointer list should point to an area of ram supplied by the application
// and should be large enough to contain a pointer to each logical block of flash in the 
// specified area.
// If the size is defined as zero, only the address of the next footer found is returned.
// Parameters:
//   cyg_uint32 *start
//     pointer to the address of the flash memory to be scanned
//   cyg_uint32 size
//     size of the flash memory in bytes 
//   tFooter **list                     
//     pointer to a list of pointers to footers
//
//   Returns: number of flash footers found.                                      
//

cyg_uint32 
fLib_FindFooter(cyg_uint32 *start, cyg_uint32 size, tFooter *list[])
{                                                                                           
    cyg_uint32    *end ;
    cyg_uint32    count = 0;
    cyg_uint32    logical_block = FLASH_BLOCK_SIZE;
    cyg_uint32    block_mask;
    tFooter   *test;

    end = flash_info.end;
   
    // Start looking at each block boundary to see if there is a 
    // structured footer there if so that is a footer to put in the footer list
    //
    // Initially we must get the nearest logical block boundary to the 
    // initial address
    block_mask = logical_block - 1;
  
    start = (cyg_uint32*)((int)start | block_mask);
  
    test = (tFooter*)(start - (sizeof(tFooter) >> 2) );
    test = (tFooter*) ((char*)test + 1);
  
    // Now test should be at the first available footer location
  
    for ( ; (cyg_uint32*)test < end; test = (tFooter*)((char*)test + logical_block)) {
        // initial check is to see if the type is deleted (ie empty block)
	if (TYPE_DELETED != test->type) {
	   // next check for a signature in the footer
         
	   if (FLASH_FOOTER_SIGNATURE == test->signature) {
	      cyg_uint32 check = 0;
	      cyg_uint32 *check_ptr = (cyg_uint32 *)test;
	      count = (sizeof(tFooter) - 4) >> 2;	// sizeof will return bytes
            
	      // We're pretty certain by now but lets check the checksum to make fully sure
	      for ( ; count > 0; count--) {
		  if (*check_ptr > ~check)
            	      check++;
            	  check += *check_ptr++;
	      }
            
	      if (test->checksum == ~check) {
		  if (size != 0 )
		      *list++ = test;
		  else {
		      // If the size was set at 0 - return only the first found footer
		      *list = test;
		      return 1;
		  }
	      }
	   }
	}
	// This should carry on through checking each logical block boundary to end */
    }

    // tidy up the array/list
    *list = NULL;
    return count;
}

//
// fLib_DeleteImage(tFooter *foot, tFlash *flash)
//
// Delete the image in flash as detailed by the specified image footer. We will have to
// define the flash device to access the relevant flash delete routines
// Parameters
//   tFooter *foot
//      pointer to the footer which defines the image to be deleted
//
// Returns: 0 if successful, otherwise -1.
//

static int
fLib_DeleteImage(tFooter *foot)
{                                                                                                 
    cyg_uint32 delete_size , block_mask = flash_info.block_size;
    cyg_uint8 *erase_addr = foot->blockBase;
    cyg_uint8  complete_flag = 0;
  
     // presume that the erase size is chosen to cover the boundary size
    block_mask -= 1;
 
    delete_size = ((tImageInfo*)foot->infoBase)->length +
      			((tImageInfo*)foot->infoBase)->header_length +
      			sizeof (tImageInfo) + sizeof (tFooter);
    delete_size |= block_mask;
    delete_size += 1; /* for the boundary */

    while (!complete_flag ) {
      if (  delete_size > flash_info.block_size) {
         delete_size -= flash_info.block_size;
      } else 
         complete_flag = 1;

      flash_erase_block(erase_addr);
      erase_addr += flash_info.block_size;
    }
    return 0;
}

//
// fLib_FindImage(tFooter **list, cyg_uint32 imageNo, tFooter *foot)
//
// Scan the list of flash footers looking for a footer with an imageNumber which
// matches the specified number.
// If the specified footer pointer is not NULL, the footer is copied from flash.
// Parameters:
//   tFooter **list
//      pointer to a list of pointers to footers
//   unsigned32 imageNo
//      number of image to be located
//   tFooter *foot
//      pointer to where the found footer should be copied
//
// Returns: 1 if successful, otherwise 0
//

static int
fLib_FindImage(tFooter **list, char *name, tFooter *foot)
{
  // Initially, if there are no images in the flash we can just return so check 
  if (*list == NULL)
        return 0;
  
  // Now we can start checking for the images that are in flash
  while (*list != NULL) {
      tImageInfo *im = (tImageInfo *)(*list)->infoBase;
      if (strncmp(name, im->name, 16) == 0 ) {
         *foot = **list;
         return 1;
      }
      list++;      
  }

  return 0;
}

#define FLASH_ADRS 0x23000000                                                      

int 
markBlock()
{
        tFooter footer;
	tFooter *current;
	cyg_uint32  writeOffset;

        // Build a flash footer to describe this area.
        footer.blockBase    = (char *)FLASH_ADRS;
        footer.infoBase     = NULL;
        footer.type         = 0x10000; //TYPE_WRS_SIB;
        footer.signature    = FLASH_FOOTER_SIGNATURE;
        footer.checksum     = 0;
        footer.checksum     = footer_checksum (&footer);

	current = (tFooter *)(FLASH_ADRS + FLASH_BLOCK_SIZE - sizeof (tFooter));

	if ( current->type == TYPE_ARM_SIB)
	    return 1;

	writeOffset = FLASH_ADRS + FLASH_BLOCK_SIZE - sizeof (tFooter);

	if (flash_program_buf(writeOffset, (unsigned long *)&footer, sizeof(tFooter)) < 0 ) 
	    return 0;

#ifdef TO_CHECK
	if (sysFlashWrite(FLASH_CAST(&footer), sizeof(tFooter), 
				  writeOffset,
				  flashType, 0) != OK)
		    return 0;
#endif
}

void 
arm_fis_list(void)
{
    int count = 0;
    tFooter *footer;

    fLib_FindFooter((cyg_uint32 *)FLASH_BASE, FLASH_SIZE, active_flash_footers);

    diag_printf("Name              FLASH addr   Mem addr    Length    Entry point\n");
    while ((footer = active_flash_footers[count++]) != NULL) {
	if (footer->type == TYPE_ARM_EXEC) {
	    diag_printf("%-16s  0x%08lX   0x%08lX  0x%06lX  0x%08lX\n",
		   ((tImageInfo *)footer->infoBase)->name, 
		   FLASH_BASE, footer->blockBase, 
		   ((tImageInfo *)footer->infoBase)->length,
		   ((tImageInfo *)footer->infoBase)->loadAddress);
	} 
    }
}

void 
arm_fis_delete(char *name)
{
    tFooter footer;

    if ( name == NULL ) return;

    fLib_FindFooter((cyg_uint32 *)FLASH_BASE, FLASH_SIZE, active_flash_footers);

    if ( fLib_FindImage(active_flash_footers, name, &footer) ) {
        diag_printf("Found Image %s, base 0x%x\n", name, footer.blockBase);
	fLib_DeleteImage(&footer);
    }
}

static void 
SIB_test(void)
{
    int count;
    tFooter *footer;

    fLib_FindFooter((cyg_uint32 *)FLASH_BASE, FLASH_SIZE, active_flash_footers);


    // Find everything that looks like a SIB.
    count       = 0;

    diag_printf("\n");
    while ((footer = active_flash_footers[count++]) != NULL) {
	if (footer->type == TYPE_ARM_EXEC) {
	    diag_printf("Blk: %d Image Nb: %3d Name: %s Load address: 0x%x Length 0x%x\n\n",
		   AddressToBlock(footer->blockBase),
		   ((tImageInfo *) footer->infoBase)->imageNumber,
		   ((tImageInfo *)footer->infoBase)->name, 
		   ((tImageInfo *)footer->infoBase)->loadAddress,
		   ((tImageInfo *)footer->infoBase)->length);
	} else if (footer->type == TYPE_ARM_SIB) {
	    tSIB    *sib;

	    sib = (tSIB *)footer->blockBase;
	    diag_printf("SIB Address     Owner                        Size  Idx  Rev\n");
	    diag_printf("-----------     -----                        ----  ---  ---\n");
	    while (sib->signature == SIB_HEADER_SIGNATURE) {
                if (sib_checksum (sib) == 0) {
                    diag_printf("0x%08X  %-32s %4d %4d %4d\n\n", 
			   (int)sib, sib->owner, sib->size, sib->index, sib->revision);
		    sib = (tSIB *)((cyg_uint32)sib + sib->size);
		} else {
		    diag_printf("0x%08X  *** Bad Checksum ***\n", (int)sib);
                break;
		}
	    }

	} 
    }
}
#endif // ARM_INTEGRATOR_FLASH
