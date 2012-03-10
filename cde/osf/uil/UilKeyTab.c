/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* 
 *  @OSF_COPYRIGHT@
 *  COPYRIGHT NOTICE
 *  Copyright (c) 1990, 1991, 1992, 1993 Open Software Foundation, Inc.
 *  ALL RIGHTS RESERVED (MOTIF). See the file named COPYRIGHT.MOTIF for
 *  the full copyright text.
*/ 
/* 
 * HISTORY
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: UilKeyTab.c /main/11 1995/07/14 09:34:29 drk $"
#endif
#endif

/*
*  (c) Copyright 1989, 1990, DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASS. */

/*
**++
**  FACILITY:
**
**      User Interface Language Compiler (UIL)
**
**  ABSTRACT:
**
**      This module contains the keyword table used by the lexical analyzer
**	to look up the keywords in the UIL.
**
**--
**/


/*
**
**  INCLUDE FILES
**
**/

#include "UilDefI.h"


/*
**
**  DEFINE and MACRO DEFINITIONS
**
**/


/*
**
**  EXTERNAL VARIABLE DECLARATIONS
**
**/


/*
**
**  GLOBAL VARIABLE DECLARATIONS
**
**/


/*
**
**  OWN VARIABLE DECLARATIONS
**
**/

/*    Keyword table pointer.    */

static key_keytable_entry_type * key_keytable_ptr;


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine searches for a symbol in the compiler's keyword table.
**	There are two arguments to the routine, the length of the symbol and
**      the address of the start of the symbol.  The routine returns the
**	address of the keyword entry found, or a NULL pointer if the
**	symbol is not found in the table.
**
**	The search for the symbol is case sensitive depending upon the
**	keytable binding that was established by the key_initialize routine.
**
**	The require file UilKeyTab.h defines and initializes the keyword
**	tables.  It is built automatically from other files, thus it should
**	not be hand editted.
**
**  FORMAL PARAMETERS:
**
**	symbol_length.rl.v : 	length of symbol to look up
**	symbol_ptr.ra.v : 	address of symbol to look up
**
**  IMPLICIT INPUTS:
**
**      key_keytable_ptr		: current keyword table
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**	NULL		: if the symbol is not in the keyword table
**	otherwise	: the address of the keyword table entry for
**			  the specified symbol.
**
** SIDE EFFECTS:
**
**	none
**
**--
**/
key_keytable_entry_type *
	key_find_keyword (symbol_length, symbol_ptr)

unsigned int	symbol_length;
char		* symbol_ptr;

{
    
    int
	lower_limit,
	upper_limit;
    
/*    Check the arguments.    */

    if (symbol_length > key_k_keyword_max_length)
	return NULL;

/*    Initialize region to search.    */
    
    lower_limit = 0;
    upper_limit = key_k_keyword_count-1;
    
/*    Perform binary search on keyword index.    */
    
    do {
	int		mid_point, result;

	key_keytable_entry_type * keyword_entry_ptr;

	mid_point = (lower_limit + upper_limit) >> 1;	/* divide by 2 */

	keyword_entry_ptr = & key_keytable_ptr [mid_point];

	result = strcmp (symbol_ptr, keyword_entry_ptr -> at_name);

	if (result == 0) {
	    return keyword_entry_ptr;		/*    Found keyword.    */
	} else if (result < 0) {
	    upper_limit = mid_point - 1;	/*    Search lower half.    */
	} else {
	    lower_limit = mid_point + 1;	/*    Search upper half.    */
	}

    } while (lower_limit <= upper_limit);

/*    If we fall out of the bottom of the loop, symbol was not found.    */

    return NULL;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine initializes the keyword lookup facility.  It can be
**	called multiple times during a single compilation.  It must be called
**	at least once before the keyword table is accessed.
**
**  FORMAL PARAMETERS:
**
**	none
**
**  IMPLICIT INPUTS:
**
**      uil_v_case_sensitive	: case sensitive switch, determines which
**				: keyword table to use.
**
**  IMPLICIT OUTPUTS:
**
**      key_keytable_ptr	: pointer to the keyword table to
**				  use for keyword lookups.
**
**  FUNCTION VALUE:
**
**	none
**
** SIDE EFFECTS:
**
**	none
**
**--
**/
void
	key_initialize ()

{

/*    Use the correct keyword table based on the global case
      sensitivity.   */

    if (uil_v_case_sensitive) {
	key_keytable_ptr = key_table;
    } else {
	key_keytable_ptr = key_table_case_ins;
    }

}    

