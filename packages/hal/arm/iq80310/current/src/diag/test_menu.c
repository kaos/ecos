//=============================================================================
//
//      test_menu.c - Cyclone Diagnostics
//
//=============================================================================
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
// Copyright (C) 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   Scott Coulter, Jeff Frazier, Eric Breeden
// Contributors:
// Date:        2001-01-25
// Purpose:     
// Description: 
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

/*****************************************************************************
* test_menu.c - Menu dispatching routine 
* 
* modification history
* --------------------
* 30aug00 ejb Ported to IQ80310 Cygmon
*/

/*
DESCRIPTION:

A table-driven menu dispatcher
*/

#include "test_menu.h"
#include "iq80310.h"

#define QUIT			-1
#define MAX_INPUT_LINE_SIZE	80

extern long decIn(void);

/*
 * Internal routines
 */
static int menuGetChoice (MENU_ITEM	menuTable[],
			  int		numMenuItems,
			  char		*title,
			  unsigned long	options);
static void printMenu (MENU_ITEM	menuTable[],
		       int		numMenuItems,
		       char		*title);


/***************************************************************************
*
* menu - a table-driven menu dispatcher
*
* RETURNS:
*
*	The menu item argument, or NULL if the item chosen is QUIT.
*/
MENU_ARG menu (
      MENU_ITEM	menuTable[],
      int		numMenuItems,
      char		*title,
      unsigned long	options
      )
{
int	item;		/* User's menu item choice */

    /*
     * Get the user's first choice.  Always display the menu the first time.
     */
    item = menuGetChoice (menuTable, numMenuItems, title, MENU_OPT_NONE);
    if (item == QUIT)
		return (NULL);

    /*
     * If the user just wants a value returned, return the argument.  If the
     * argument is null, return the item number itself.
     */
    if (options & MENU_OPT_VALUE)
    {
		if (menuTable[item].arg == NULL)
			return ((void *)item);
		else
			return (menuTable[item].arg);
    }

    /*
     * Process menu items until the user selects QUIT
     */
    while (TRUE)
    {
		/*
		 * Call the action routine for the chosen item.  If the argument is
		 * NULL, pass the item number itself.
		 */
		if (menuTable[item].actionRoutine != NULL)
		{
			if (menuTable[item].arg == NULL)
			{
				printf("\n");
				(*menuTable[item].actionRoutine) ((void *)item);
			}
			else
			{
				printf("\n");
				(*menuTable[item].actionRoutine) (menuTable[item].arg);
			}
		}

		/*
		 * Get the next choice, using any display options the user specified.
		 */
		item = menuGetChoice (menuTable, numMenuItems, title, options);
		if (item == QUIT)
			return (NULL);
    }

} /* end menu () */


/***************************************************************************
*
* menuGetChoice - Get the user's menu choice.
*
* If display is not suppressed, display the menu, then prompt the user for
* a choice.  If the choice is out of range or invalid, display the menu and
* prompt again.  Continue to display and prompt until a valid choice is made.
*
* RETURNS:
*	The item number of the user's menu choice. (-1 if they chose QUIT)
*/

static int
menuGetChoice (
	       MENU_ITEM	menuTable[],
	       int		numMenuItems,
	       char		*title,
	       unsigned long	options
	       )
{
/*char	inputLine[MAX_INPUT_LINE_SIZE];*/

int	choice;

    /*
     * Suppress display of the menu the first time if we're asked
     */
    if (!(options & MENU_OPT_SUPPRESS_DISP))
		printMenu (menuTable, numMenuItems, title);

    /*
     * Prompt for a selection.  Redisplay the menu and prompt again
     * if there's an error in the selection.
     */
    choice = -1;
	
    while (choice < 0 || choice > numMenuItems)
    {

		printf ("\nEnter the menu item number (0 to quit): ");

		choice = decIn ();

		if (choice < 0 || choice > numMenuItems)

			printMenu (menuTable, numMenuItems, title);

    }

    if (choice == 0)
		
		return (QUIT);

    return (choice - 1);

} /* end menuGetChoice () */


/***************************************************************************
*
* printMenu - Print the menu
*
*
*/

static void
printMenu (
	   MENU_ITEM	menuTable[],
	   int		numMenuItems,
	   char		*title
	   )
{
    int		i;


    printf("\n%s\n\n", title);

    for (i = 0; i < numMenuItems; i++)
    {
		printf ("%2d - %s\n", i+1, menuTable[i].itemName);
    }

    printf(" 0 - quit");

} /* end printMenu () */
