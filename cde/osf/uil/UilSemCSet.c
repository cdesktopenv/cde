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
static char rcsid[] = "$TOG: UilSemCSet.c /main/10 1997/03/12 15:21:53 dbl $"
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
**      This file contains and routines related to the semantics of
**	character sets.
**	semantic validation.
**
**--
**/


/*
**
**  INCLUDE FILES
**
**/

#include <Xm/Xm.h>

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



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function returns a pointer to a character set name given
**	its sym_k_..._charset code and possibly a userdefined charset
**	value entry.
**
**  FORMAL PARAMETERS:
**
**	l_charset	charset of the string (token value)
**	az_charset_entry   charset of the string (symbol table value entry)
**
**  IMPLICIT INPUTS:
**
**      charset data tables
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      pointer to name (must NOT be freed)
**
**  SIDE EFFECTS:
**
**      error messages may be issued for objects that are still undefined
**	or of the wrong type
**
**--
**/

char *sem_charset_name (l_charset, az_charset_entry)
    int				l_charset;
    sym_value_entry_type	*az_charset_entry;

{

int		charset;		/* mapped character set */

charset = sem_map_subclass_to_charset (l_charset);
switch ( charset )
    {
    case sym_k_fontlist_default_tag:
      return XmFONTLIST_DEFAULT_TAG;      
    case sym_k_userdefined_charset:
        /*
	 ** If the charset is user-defined, then fetch info from the symbol 
	 ** table entry for it.						   
	 */
	_assert (az_charset_entry!=NULL, "null userdefined charset entry");
	return az_charset_entry->value.c_value;
    default:
	return charset_xmstring_names_table[charset];
    }

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function returns information about the character set specified
**	(writing direction and sixteen_bit properties).
**
**  FORMAL PARAMETERS:
**
**	l_charset	charset of the string (token value)
**	az_charset_entry   charset of the string (symbol table value entry)
**	direction	string writing direction
**	sixteen_bit	Boolean return value
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      none
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/

void sem_charset_info
    (l_charset, az_charset_entry, write_direction, parse_direction,  sixteen_bit)

int			l_charset;
sym_value_entry_type	*az_charset_entry;
int			*write_direction;
int			*parse_direction;
int			*sixteen_bit;

{

int		charset;	/* mapped character set */
    
charset = sem_map_subclass_to_charset (l_charset);
switch (charset) 
    {
    /*
     ** If the charset is user-defined, then fetch info from the symbol 
     ** table entry for it.						   
     */
    case sym_k_userdefined_charset:
        {
	*write_direction = az_charset_entry->b_direction;
	*parse_direction = az_charset_entry->b_direction;
	*sixteen_bit = (az_charset_entry->b_aux_flags &
			sym_m_sixteen_bit) != 0;
	break;
	}
    default:
	{
	*write_direction = charset_writing_direction_table[charset];
	*parse_direction = charset_parsing_direction_table[charset];
	if ( charset_character_size_table[charset] != sym_k_onebyte_charsize )
	    *sixteen_bit = TRUE;
	else
	    *sixteen_bit = FALSE;
	break;
	}
	}
}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function returns the charset id corresponding to the
**	charset specified in a $LANG variable. User-defined character
**	sets are not recognized - it must be one available in the
**	the compiler tables.
**
**	The name match is case-insensitive.
**
**  FORMAL PARAMETERS:
**
**	lang_charset	string naming a character set
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      0	no match - character set not found
**	>0	character set code from sym_k_..._charset.
**		sym_k_userdefined_charset is never returned.
**		
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/

int sem_charset_lang_name (lang_charset)
    char		*lang_charset;

{

char		uname[200];	/* upper-case character set name */
int		ndx;		/* loop index */


/*
 * Convert name to upper case, then search table (which is already in
 * upper case).
 */
strcpy (uname, lang_charset);
for ( ndx=0 ; ndx<(int)strlen(uname) ; ndx++ )
    uname[ndx] = _upper (uname[ndx]);

for ( ndx=0 ; ndx<(int)charset_lang_table_max ; ndx++ )
    if ( strcmp(uname,charset_lang_names_table[ndx]) == 0 )
	return (int)charset_lang_codes_table[ndx];
return 0;

}


