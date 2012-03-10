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
static char rcsid[] = "$TOG: UilSymNam.c /main/13 1997/09/08 11:12:50 cshi $"
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
**      This module inserts names into the name table.
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



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**  This routine searches for a name entry of the same name as its parameters.
**  If the entry is found, a pointer to that name node is 
**  returned as the value of the function.  If no entry is found, a NULL
**  pointer is returned.
**
**  See sym_insert_name for a description of the name lookup alorithm.
**
**  FORMAL PARAMETERS:
**
**      l_length	    length of the name not including the null
**	c_text		    pointer to a null terminated string for name
**
**  IMPLICIT INPUTS:
**
**      sym_az_hash_table   the hash table
**
**  IMPLICIT OUTPUTS:
**
**      sym_az_hash_table   may be updated with an additional name
**
**  FUNCTION VALUE:
**
**      a pointer to a name entry
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/

sym_name_entry_type 
    *sym_find_name(l_length, c_text)

int	l_length;	/* length of name to find */
char	*c_text;	/* text of the name */

{
    sym_name_entry_type	*az_current_name;
    int			l_hash_code;
    int			l_compare_result;

    /* obtain the hash code of for the name */

    l_hash_code = hash_function( l_length, c_text );

    /*
    **  chain along hash chain looking for symbol - exit loop under 3 condition
    **        1) come to the end of the chain: name not found
    **        2) find symbol: return this symbol
    **        3) find node > symbol: name not found
    */

    for (az_current_name = sym_az_hash_table[ l_hash_code ];
	 az_current_name != NULL;
	 az_current_name = az_current_name->az_next_name_entry)
    {
	l_compare_result = _compare(c_text, az_current_name->c_text);

	if (l_compare_result == 0)	/* c_text = current name */
	{
	    /* found the name we are looking for */

	    return az_current_name;
	}

	if (l_compare_result > 0)	/* c_text > current name */
	{
	    /* return NULL - name should be before this spot in list */

	    return NULL;
	}

    }

    /* came to end of the list without finding the name */

    return NULL;
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**  This routine searches for a name entry of the same name as its parameters.
**  If the entry is found, a pointer to that name node is 
**  returned as the value of the function.  If no entry is found, one is 
**  inserted.  In this case the value of the function is a pointer to
**  the name entry created.
**
**  Name entries are linked off of a hash table.  Those
**  entries that have the same hash code, are sorted according to the
**  collating sequence.  Thus the algorithm involves hashing the symbol and
**  then following the chain for that hash code until one of the following
**  conditions is met.  1) the identifier is found, then return a pointer
**  to that name entry.  2) come to the end of the chain or a name
**  entry that comes later in the collating sequence than the symbol being
**  searched for.  In this case the name is inserted just prior to this
**  point in the chain.
**
**  FORMAL PARAMETERS:
**
**      l_length	    length of the name not including the null
**	c_text		    pointer to a null terminated string for name
**
**  IMPLICIT INPUTS:
**
**      sym_az_hash_table   the hash table
**
**  IMPLICIT OUTPUTS:
**
**      sym_az_hash_table   may be updated with an additional name
**
**  FUNCTION VALUE:
**
**      a pointer to a name entry
**
**  SIDE EFFECTS:
**
**      may create a name entry and update the hash table
**
**--
**/

sym_name_entry_type *sym_insert_name(l_length, c_text)

int	l_length;	/* length of name to insert */
char	*c_text;	/* text of the name */

{
    sym_name_entry_type	*az_previous_name;
    sym_name_entry_type	*az_current_name;
    sym_name_entry_type	*az_new_name;
    int			l_hash_code;
    int			l_compare_result;

    /*
    **  algorithm keeps 2 pointers, one for the previous name and one
    **  for the current name.  This permits easy insertion of a new name 
    */


    /* obtain the hash code of for the name */

    l_hash_code = hash_function( l_length, c_text );

    /*
    **  chain along hash chain looking for symbol - exit loop under 3 condition
    **        1) come to the end of the chain: insert new node on end
    **        2) find symbol: return this symbol
    **        3) find node > symbol: insert new node prior to current node
    */

    for (az_current_name = sym_az_hash_table[ l_hash_code ],
	 az_previous_name = NULL;

	 az_current_name != NULL;

	 az_previous_name = az_current_name,
	 az_current_name = az_current_name->az_next_name_entry)
    {
	l_compare_result = _compare(c_text, az_current_name->c_text);

	if (l_compare_result == 0)	/* c_text = current name */
	{
	    /* found the name we are looking for */

	    return az_current_name;
	}

	if (l_compare_result > 0)	/* c_text > current name */
	{
	    /* exit the loop to insert just prior to current name */

	    goto insert_name;
	}

    }

insert_name:

    /*
    **	name is not in the table so it must be inserted between the
    **  az_previous_name and az_current_name entries.
    */

    /* allocate and initialize the name entry */

    az_new_name = (sym_name_entry_type *)
	sem_allocate_node (sym_k_name_entry, 
			   sym_k_name_entry_size + l_length + 1);

    az_new_name->header.b_type = l_length;	/* b_type holds length */
    az_new_name->az_object = NULL;
    az_new_name->az_cycle_id = 0;

    _move( az_new_name->c_text, c_text, l_length+1 );

    /*
    **  link the name entry into the hash table
    */

    az_new_name->az_next_name_entry = az_current_name;

    if (az_previous_name == NULL)
	sym_az_hash_table[ l_hash_code ] = az_new_name;
    else
	az_previous_name->az_next_name_entry =
			(sym_name_entry_type *) az_new_name;

    return az_new_name;
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This procedure is a hashing function.  It takes a length and a
**	pointer to a value.  Using this value as a string, the function
**	returns an integer in the range of 0 to sym_k_hash_table_limit-1.
**
**  FORMAL PARAMETERS:
**
**      l_length	    length of the value in bytes not including null
**	c_value		    a null terminated string 
**
**  IMPLICIT INPUTS:
**
**      sym_k_hash_table_limit
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      integer (the hash code) in range 0 to sym_k_hash_table_limit-1
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/

int	hash_function(l_length, c_value)

int	l_length;
char	*c_value;
{
#ifdef WORD64
#define _shift 3
    static unsigned int XmConst    mask[ 8 ] =
                { 0x00000000000000FF, 0x000000000000FFFF,
                  0x0000000000FFFFFF, 0x00000000FFFFFFFF,
                  0x00000000FFFFFFFF, 0x0000FFFFFFFFFFFF,
                  0x00FFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, };
#elif defined (LONG64)
    #define _shift 3
    static long XmConst    mask[ 8 ] =
                { 0x00000000000000FF, 0x000000000000FFFF,
                  0x0000000000FFFFFF, 0x00000000FFFFFFFF,
                  0x00000000FFFFFFFF, 0x0000FFFFFFFFFFFF,
                  0x00FFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, };
#else
#define _shift 2
    static unsigned int XmConst	mask[ 4 ] = 
		{ 0x000000FF, 0x0000FFFF, 0x00FFFFFF, 0xFFFFFFFF };
#endif

#ifdef LONG64
    long                l_hash_code;
    long                al_value[20];
#else
    int                 l_hash_code;
    int                 al_value[20];
#endif
    int	 	   	l_limit;
    int	  	  	l_extra;
    int	   	 	i;

    l_limit = (l_length-1) >> _shift;	/* divide by wordsize */
    l_extra = (l_length-1) & _slm;	/* remainder from divide by wordsize */

#ifdef LONG64
    bzero((char *)al_value, sizeof(long) * 20);
#else
    bzero((char *)al_value, sizeof(int) * 20);
#endif
    strncpy((char *)al_value, c_value, l_length);
    l_hash_code = 0;

    for (i = 0; i < l_limit; i++)
    {
	l_hash_code = l_hash_code ^ al_value[ i ];
    }

    l_hash_code = l_hash_code ^ (al_value[ i ] & mask[ l_extra ]);

    return (int)(l_hash_code % sym_k_hash_table_limit);
}


#if debug_version

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This debugging routine will dump out the name entries linked
**	from the hash table.
**
**  FORMAL PARAMETERS:
**
**      none
**
**  IMPLICIT INPUTS:
**
**      sym_az_hash_table
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      prints out the hash table
**
**--
**/

void	sym_dump_hash_table()
{
    int		i;
    int		total_count;
    int		max_length;
    int		empty_count;

    total_count = 0;
    empty_count = 0;
    max_length = 0;
	
    for (i=0;  i<sym_k_hash_table_limit;  i++)
    {
	int		    bucket_count;
	sym_name_entry_type  *az_name;

	bucket_count = 0;

	for (az_name = sym_az_hash_table[ i ];  
	     az_name != NULL;  
	     az_name = az_name->az_next_name_entry)
	{
	    bucket_count++;

	    _debug_output("\t %s \n", az_name->c_text);
	}

	total_count += bucket_count;
	if (bucket_count == 0)
	    empty_count++;
	max_length = ( max_length > bucket_count )? max_length : bucket_count;

	_debug_output("bucket: %d length: %d\n", i, bucket_count);
    }

    _debug_output("name count: %d \n empty count: %d \n max length: %d",
		   total_count, empty_count, max_length );
}
#endif
