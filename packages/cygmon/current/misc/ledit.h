//==========================================================================
//
//      ledit.h
//
//      Header for the utterly simple line editor
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
// Author(s):    
// Contributors: gthomas
// Date:         1999-10-20
// Purpose:      Header for the udderly simple line editor
// Description:  
//               
//
//####DESCRIPTIONEND####
//
//=========================================================================

#ifndef LEDIT_H
#define LEDIT_H

#define LF '\n'
#define CR '\r'
#define BOLCMD '\001'
#define EOLCMD '\005'
#define FORWCMD '\006'
#define BACKCMD '\002'
#define DELBACK '\010'
#define DELEOL '\013'
#define YANKCH '\031'
#define DELCURRCH '\004'
#define PREVCMD '\020'
#define ERASELINE '\025'
#define NEXTCMD '\016'

/* Prompt for one line of input using PROMPT. The input from the user
   (up to MAXINPLEN characters) will be stored in BUFFER. The number
   of characters read will be returned. */
extern int lineedit(char *prompt, char *buffer, int maxInpLen);

/* Add CMD to the user's command history. */
extern void addHistoryCmd (char *cmd);

/* Configure the editor to use the specified terminal. */
extern void set_term_name (char *name);

/* Beep the terminal. */
extern void beep (void);

/* Print a history list. */
extern void printHistoryList(void);

#endif
