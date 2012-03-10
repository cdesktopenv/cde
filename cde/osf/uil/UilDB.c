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
static char rcsid[] = "$XConsortium: UilDB.c /main/11 1996/11/21 20:03:11 drk $"
#endif
#endif

/*
*  (c) Copyright 1989, 1990, DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASS. */


/*
 *++
 *  FACILITY:
 *
 *      UIL Bindary Database :
 *
 *  ABSTRACT:
 *
 *--
 */

/*
 * This file contains routines which change the internal tables of UIL based on
 * a binary data base parameter in the command line 
 */


/*
 *
 *  INCLUDE FILES
 *
 */

#ifndef X_NOT_STDC_ENV
#include <stdlib.h>
#include <unistd.h>
#endif

#include <Mrm/MrmAppl.h>
#include <Mrm/Mrm.h>

#include "UilDefI.h"

#define X_INCLUDE_PWD_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>
#include <stdio.h>

/*
 *
 *  TABLE OF CONTENTS
 *
 *
 */


/*
 *
 *  DEFINE and MACRO DEFINITIONS
 *
 */
#define _check_read( __number_returned ) \
	if (( (__number_returned) != 1) || (feof(dbfile)) || (ferror(dbfile)) ) \
	{  diag_issue_diagnostic( d_bad_database, diag_k_no_source, diag_k_no_column ); } 




/*
 *
 *  EXTERNAL VARIABLE DECLARATIONS
 *
 */

/*
 *
 *  GLOBAL VARIABLE DECLARATIONS
 *
 */


/*
 *
 *  OWN VARIABLE DECLARATIONS
 *
 */
static FILE *dbfile;
static int  num_bits;

void db_incorporate()

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine incorporate the binary database passed in the command line.
 *
 *
 *  FORMAL PARAMETERS:
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

/*
 *  External Functions
 */

/*
 *  Local variables
 */
{
    int			return_num_items;
    _db_header		header;
    _db_globals		globals;

    db_open_file();

    return_num_items = fread (&globals, sizeof(_db_globals), 1, dbfile);
    _check_read (return_num_items);

    /*
     * Some heuristics to see if this is a reasonable database.
     * The magic numbers are about 10 times as big as the DXm database
     * for DECWindows V3. The diagnostic does a fatal exit.
     */
    if ( globals.uil_max_arg>5000 ||
	 globals.uil_max_charset>200 ||
	 globals.charset_lang_table_max>1000 ||
	 globals.uil_max_object>500 ||
	 globals.uil_max_reason>1000 ||
	 globals.uil_max_enumval>3000 ||
	 globals.uil_max_enumset>1000 ||
	 globals.key_k_keyword_count>10000 ||
	 globals.key_k_keyword_max_length>200 ||
	 globals.uil_max_child>250)
	diag_issue_diagnostic (d_bad_database,
			       diag_k_no_source,
			       diag_k_no_column);

    uil_max_arg = globals.uil_max_arg ;
    uil_max_charset = globals.uil_max_charset ;
    charset_lang_table_max = globals.charset_lang_table_max ;
    uil_max_object = globals.uil_max_object ;
    uil_max_reason = globals.uil_max_reason ;
    uil_max_enumval = globals.uil_max_enumval ;
    uil_max_enumset = globals.uil_max_enumset ;
    key_k_keyword_count = globals.key_k_keyword_count ;
    key_k_keyword_max_length = globals.key_k_keyword_max_length ;
    uil_max_child = globals.uil_max_child;
    num_bits = (uil_max_object +7) / 8;

    if (globals.version > DB_Compiled_Version) 
	diag_issue_diagnostic( d_future_version, diag_k_no_source, diag_k_no_column );

    for (;;) 
	{
	return_num_items = fread (&header, sizeof(_db_header), 1, dbfile);
	if (feof(dbfile)) break;
	_check_read (return_num_items);
	switch (header.table_id)
	    {
	    case Constraint_Tab:
		constraint_tab = (unsigned char *) XtMalloc (header.table_size);
		return_num_items = fread (constraint_tab, 
					     sizeof(unsigned char) * header.num_items,
					     1, dbfile);
		_check_read (return_num_items);
		break;
	    case Argument_Type_Table_Value:
		/*
		 * NOTE: The first entry is not used but we copy it anyway
		 */
		argument_type_table = (unsigned char *) XtMalloc (header.table_size);
		return_num_items = fread (argument_type_table,
					     sizeof(unsigned char) * header.num_items,
					     1, dbfile);
		_check_read (return_num_items);
		break;
	    case Child_Class_Table:
		/*
		 * NOTE: The first entry is not used but we copy it anyway
		 */
		child_class_table = 
		  (unsigned char *) XtMalloc (header.table_size);
		return_num_items = 
		  fread (child_class_table,
			 sizeof(unsigned char) * header.num_items, 1, dbfile);
		_check_read (return_num_items);
		break;
	    case Charset_Wrdirection_Table:
		charset_writing_direction_table = (unsigned char *) XtMalloc (header.table_size);
		return_num_items = fread (charset_writing_direction_table, 
					     sizeof(unsigned char) * header.num_items,
					     1, dbfile);
		_check_read (return_num_items);
		break;
	    case Charset_Parsdirection_Table:
		charset_parsing_direction_table = (unsigned char *) XtMalloc (header.table_size);
		return_num_items = fread (charset_parsing_direction_table, 
					     sizeof(unsigned char) * header.num_items,
					     1, dbfile);
		_check_read (return_num_items);
		break;
	    case Charset_Charsize_Table:
		charset_character_size_table = (unsigned char *) XtMalloc (header.table_size);
		return_num_items = fread (charset_character_size_table, 
					     sizeof(unsigned char) * header.num_items,
					     1, dbfile);
		_check_read (return_num_items);
		break;
	    case Key_Table:
	    case Key_Table_Case_Ins:
		db_read_ints_and_string (&header);
		break;
	    case Allowed_Argument_Table:
	    case Allowed_Child_Table:
	    case Allowed_Control_Table:
	    case Allowed_Reason_Table:
		db_read_char_table (&header);
		break;
	    case Charset_Xmstring_Names_Table:
	    case Charset_Lang_Names_Table:
	    case Uil_Widget_Names:
	    case Uil_Children_Names:
	    case Uil_Argument_Names:
	    case Uil_Reason_Names:
	    case Uil_Enumval_names:
	    case Uil_Charset_Names:
	    case Uil_Widget_Funcs:
	    case Uil_Argument_Toolkit_Names:
	    case Uil_Reason_Toolkit_Names:
		db_read_length_and_string (&header);
		break;
	    case Charset_Lang_Codes_Table:
		charset_lang_codes_table = (unsigned short int *) XtMalloc (header.table_size);
		return_num_items = fread (charset_lang_codes_table, 
					     header.table_size,
					     1, dbfile);
		_check_read (return_num_items);
		break;
	    case Argument_Enum_Set_Table:
		argument_enumset_table = (unsigned short int *) XtMalloc (header.table_size);
		return_num_items = fread (argument_enumset_table, 
					     header.table_size,
					     1, dbfile);
		_check_read (return_num_items);
		break;
	    case Related_Argument_Table:
		related_argument_table = (unsigned short int *) XtMalloc (header.table_size);
		return_num_items = fread (related_argument_table, 
					     header.table_size,
					     1, dbfile);
		_check_read (return_num_items);
		break;
	    case Uil_Gadget_Funcs:
		uil_gadget_variants = (unsigned short int *) XtMalloc (header.table_size);
		return_num_items = fread (uil_gadget_variants, 
					     header.table_size,
					     1, dbfile);
		_check_read (return_num_items);
		break;
	    case Uil_Urm_Nondialog_Class:
		uil_urm_nondialog_class = (unsigned short int *) XtMalloc (header.table_size);
		return_num_items = fread (uil_urm_nondialog_class, 
					     header.table_size,
					     1, dbfile);
		_check_read (return_num_items);
		break;
	    case Uil_Urm_Subtree_Resource:
		uil_urm_subtree_resource = (unsigned short int *) XtMalloc (header.table_size);
		return_num_items = fread (uil_urm_subtree_resource, 
					     header.table_size,
					     1, dbfile);
		_check_read (return_num_items);
		break;
	    case Enum_Set_Table:
		db_read_int_and_shorts(&header);
		break;
	    case Enumval_Values_Table:
		enumval_values_table = (int *) XtMalloc (header.table_size);
		return_num_items = fread (enumval_values_table, 
					     header.table_size,
					     1, dbfile);
		_check_read (return_num_items);
		break;
	    default:
		diag_issue_diagnostic( d_bad_database, diag_k_no_source, diag_k_no_column );
	    } /* end switch */
	} /* end for */
    fclose (dbfile);
    return;
}



void db_read_ints_and_string(header)
    _db_header_ptr   header;

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine reads in tables of integers and one string unsigned chars and places them into 
 *	memory. It will Malloc new space for the table. The tables supported
 *	this routine are:
 *
 *	    Key_Table:
 *	    Key_Table_Case_Ins:
 *
 *
 *  FORMAL PARAMETERS:
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

{

/*
 *  External Functions
 */

/*
 *  Local variables
 */
	int			return_num_items, i, string_size=0;
	key_keytable_entry_type	*table;
	char			*string_table;
	 
	switch (header->table_id)
	    {
	    /*
	     * NOTE: Calloc is used here to protect against bad
	     *	     pointers.
	     */
	    case Key_Table:
		key_table = (key_keytable_entry_type *) XtCalloc (1, header->table_size);
		table = key_table;
		break;
	    case Key_Table_Case_Ins:
		key_table_case_ins = (key_keytable_entry_type *) XtCalloc (1, header->table_size);
		table = key_table_case_ins;
		break;
	    default:
		diag_issue_internal_error ("Bad table_id in db_read_ints_and_string");
	    }

	/* 
	 * Get the entire table with one read. 
	 * Then loop through the table and up the length of the strings.
	 * Get all the strings with one read.
	 * Reassign the addresses
	 */
	return_num_items = fread(table, header->table_size, 1, dbfile);
	_check_read (return_num_items);

	for ( i=0 ; i<header->num_items; i++)
	    {
	    /*
	     * Add one for the null character on the string
	     */
	    string_size += table[i].b_length + 1;
	    };

	string_table = XtMalloc (sizeof (char) * string_size);
	return_num_items = fread(string_table, 
				    sizeof(unsigned char) * string_size,
				    1, dbfile);
	_check_read (return_num_items);

	for ( i=0 ; i<header->num_items; i++)
	    {
	    table[i].at_name = string_table;
	    string_table +=  table[i].b_length + 1; 
	    };

	return;
}



void db_read_char_table(header)
    _db_header_ptr   header;

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine reads in tables of unsigned chars and places them into 
 *	memory. It will Malloc new space for the table. The tables supported
 *	this routine are:
 *
 *	    Allowed_Argument_Table:
 *	    Allowed_Child_Table:
 *	    Allowed_Control_Table:
 *	    Allowed_Reason_Table:
 *
 *
 *  FORMAL PARAMETERS:
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

{

/*
 *  External Functions
 */

/*
 *  Local variables
 */
	unsigned char	**ptr;
	int		return_num_items, i;
	unsigned char	*table;
	 
	switch (header->table_id)
	    {
	    /*
	     * NOTE: Calloc is used here to protect against bad
	     *	     pointers.
	     */
	    case Allowed_Argument_Table:
		allowed_argument_table = (unsigned char **) XtCalloc (1, header->table_size);
		ptr = allowed_argument_table;
		break;
	    case Allowed_Child_Table:
		allowed_child_table = 
		  (unsigned char **) XtCalloc (1, header->table_size);
		ptr = allowed_child_table;
		break;
	    case Allowed_Control_Table:
		allowed_control_table = (unsigned char **) XtCalloc (1, header->table_size);
		ptr = allowed_control_table;
		break;
	    case Allowed_Reason_Table:
		allowed_reason_table = (unsigned char **) XtCalloc (1, header->table_size);
		ptr = allowed_reason_table;
		break;
	    default:
		diag_issue_internal_error ("Bad table_id in db_read_char_table");
	}

	/*
	 * Read in the entire table contents in one whack.
	 * Then go through the table and set the addresses
	 */
	table = (unsigned char *) XtMalloc (sizeof (unsigned char) * header->num_items * num_bits);
	return_num_items = fread(table, 
				    sizeof(char) * num_bits * header->num_items,
				    1, dbfile);
	_check_read (return_num_items);
	for ( i=1 ; i<=header->num_items; i++ )
	    {
	    ptr[i] = table;
	    table += num_bits;
	    };

	return;
}



void db_read_length_and_string(header)
    _db_header_ptr   header;

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine reads in length and strings of unsigned chars and places them into 
 *	memory. It will Malloc new space for the table. The tables supported
 *	this routine are:
 *
 *	    Charset_Xmstring_Names_Table:
 *	    Charset_Lang_Names_Table:
 *	    Uil_Widget_Names:
 *	    Uil_Children_Names:
 *	    Uil_Argument_Names:
 *	    Uil_Reason_Names:
 *	    Uil_Enumval_names:
 *	    Uil_Charset_Names:
 *	    Uil_Widget_Funcs:
 *	    Uil_Argument_Toolkit_Names:
 *	    Uil_Reason_Toolkit_Names:
 *
 *
 *  FORMAL PARAMETERS:
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

{

/*
 *  External Functions
 */

/*
 *  Local variables
 */
	int		return_num_items, i, string_size=0;
	int		*lengths;
	char		*string_table;
	char		**table;
	 
	switch (header->table_id)
	    {
	    /*
	     * NOTE: Calloc is used here because it might be possible to
	     *	     have a string of zero length, particularly for the
	     *	     first record. Ergo we Calloc to protect against bad
	     *	     pointers.
	     */
	    case Charset_Xmstring_Names_Table:
		charset_xmstring_names_table = (char **) XtCalloc (1, header->table_size);
		table = charset_xmstring_names_table;
		break;
	    case Charset_Lang_Names_Table:
		charset_lang_names_table = (char **) XtCalloc (1, header->table_size);
		table = charset_lang_names_table;
		break;
	    case Uil_Widget_Names:
		uil_widget_names = (char **) XtCalloc (1, header->table_size);
		table = uil_widget_names ;
		break;
	    case Uil_Children_Names:
		uil_child_names = (char **) XtCalloc (1, header->table_size);
		table = uil_child_names ;
		break;
	    case Uil_Argument_Names:
		uil_argument_names = (char **) XtCalloc (1, header->table_size);
		table = uil_argument_names;
		break;
	    case Uil_Reason_Names:
		uil_reason_names = (char **) XtCalloc (1, header->table_size);
		table = uil_reason_names;
		break;
	    case Uil_Enumval_names:
		uil_enumval_names = (char **) XtCalloc (1, header->table_size);
		table = uil_enumval_names;
		break;
	    case Uil_Charset_Names:
		uil_charset_names = (char **) XtCalloc (1, header->table_size);
		table = uil_charset_names;
		break;
	    case Uil_Widget_Funcs:
		uil_widget_funcs = (char **) XtCalloc (1, header->table_size);
		table = uil_widget_funcs;
		break;
	    case Uil_Argument_Toolkit_Names:
		uil_argument_toolkit_names = (char **) XtCalloc (1, header->table_size);
		table = uil_argument_toolkit_names;
		break;
	    case Uil_Reason_Toolkit_Names:
		uil_reason_toolkit_names = (char **) XtCalloc (1, header->table_size);
		table = uil_reason_toolkit_names;
		break;
	    default:
		diag_issue_internal_error ("Bad table_id in db_read_length_and_string");
	    }

	/* 
	 * Get the lengths of all the strings with one read.  
	 * Then loop through the table and up the length of the strings.
	 * Get all the strings with one read. 
	 * Reassign the addresses using the length table and string table.
	 * Cleanup by Freeing length table.
	 * 
	 * NOTE: In some tables the counting starts at 1 not 0 so you
	 *	 have to be carefull.
	 */

	lengths = (int *) XtMalloc (sizeof (int) * (header->num_items + 1));
	return_num_items = fread(lengths, 
				    sizeof(int) * (header->num_items + 1),
				    1, dbfile);
	_check_read (return_num_items);
	for ( i=0 ; i<=header->num_items; i++)
	    {
	    /*
	     * Add one for the null terminator
	     */
	    if (lengths[i])
		{
		string_size += lengths[i] + 1;
		}
	    }

	string_table = XtMalloc (sizeof (unsigned char) * string_size);
	return_num_items = fread(string_table, 
				    sizeof(unsigned char) * string_size,
				    1, dbfile);
	_check_read (return_num_items);
	for ( i=0 ; i<=header->num_items; i++)
	    {
	    if (lengths[i])
		{
		table[i] = string_table;
/* BEGIN HaL Fix CR 5618 */
		  if ((header->table_id == Uil_Widget_Names) &&
		      (strcmp(table[i], "user_defined") == 0))
		    uil_sym_user_defined_object = i;
/* END HaL Fix CR 5618 */
		string_table +=  lengths[i] + 1;
		}
	    }

	XtFree ((char *)lengths);

	return;
}



void db_read_int_and_shorts(header)
    _db_header_ptr   header;

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine reads in a structure consisting of one integer and a 
 *	pointer to a table of integer and places them into 
 *	memory. It will Malloc new space for the table. The tables supported
 *	this routine are:
 *
 *	    Enum_Set_Table:
 *
 *
 *  FORMAL PARAMETERS:
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

{

/*
 *  External Functions
 */

/*
 *  Local variables
 */
	int			return_num_items, i, int_table_size=0;
	UilEnumSetDescDef	*table;
	unsigned short int 	*int_table;
	 
	switch (header->table_id)
	    {
	    case Enum_Set_Table:
		enum_set_table = (UilEnumSetDescDef *) XtCalloc (1, header->table_size);
		table = enum_set_table;
		break;
	    default:
		diag_issue_internal_error ("Bad table_id in db_read_int_shorts");
	    }

	/* 
	 * Get the entire table with one read. 
	 * Then loop through the table and add up the number of ints in each int table.
	 * Get all the integer tables with one read.
	 * Reassign the addresses of the tables.
	 */
	return_num_items = fread(table, header->table_size, 1, dbfile);
	_check_read (return_num_items);
	for ( i=0 ; i<=header->num_items; i++)
	    {
	    int_table_size += table[i].values_cnt;
	    }

	int_table = (unsigned short int *) XtCalloc (1, sizeof (short) * int_table_size);
	return_num_items = fread(int_table, 
				    sizeof(short) * int_table_size,
				    1, dbfile);
	_check_read (return_num_items);
	for ( i=0 ; i<=header->num_items; i++)
	    {
	    if (table[i].values_cnt)
		{
		table[i].values = int_table;
		int_table += table[i].values_cnt;
		}
	    }

	return;
}



void db_open_file ()

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine opens the binary database file in a platform-dependent way,
 *	performing i18n language switching in order to do so.
 *
 *	Per the latest agreement on semantics, this routine does:
 *		- first, try to open in the local directory (that is, with
 *		  no switching).
 *		- second, try language switching and open
 *
 *  FORMAL PARAMETERS:
 *
 *	name		A system-dependent string specifying the IDB file
 *			to be opened.
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

{

/*
 *  External Functions
 */

/*
 *  Local variables
 */
	char			*resolvedname;		/* current resolved name */
	SubstitutionRec		subs[3];
	char			*wmdPath;

	/*
	 * Use XtFindFile instead of XtResolvePathName. XtResolvePathName requires a
	 * display which UIL doesn't have. At the current time there is no support for
	 * $LANG in the path string. If such support was deamed necessary, the %L, %l,
	 * %t, %c values would be set up as subs here using globals from the fetch of
	 * LANG variable used to determine the default codeset (or vice versa depending
	 * on which is called first) 
	 *
	 * If the last 4 characters of the file name are not .bdb
	 * then pass in the suffix of .bdb. If a file isn't found with the suffix passed
	 * in then try without the suffix.
	 */

	/*
	 * Make sure 'S' is the last one so we can remove the suffix for the first pass.
	 */
	subs[0].match = 'N';
	subs[0].substitution = Uil_cmd_z_command.ac_database;
	subs[1].match = 'T';
	subs[1].substitution = "wmd";
	subs[2].match = 'S';
	subs[2].substitution = ".wmd";

	wmdPath = init_wmd_path(Uil_cmd_z_command.ac_database);

	resolvedname = 0;

	/*
	 * Check and see if the .wmd suffix is already on the file. If not then try to
	 * resolve the pathname with .wmd suffix first. If that fails or the suffix is
	 * already on the file then just try to resolve the pathname.
	 */
	if ( strcmp (&Uil_cmd_z_command.ac_database[strlen(Uil_cmd_z_command.ac_database)-4],".wmd") != 0 ) 
		resolvedname = XtFindFile(wmdPath,
					      subs,
					      XtNumber(subs),
					      (XtFilePredicate)NULL);

	/*
	 * No .wmd suffix or a failure to resolve the pathname with the .wmd suffix
	 * Try without the suffix.
	 */
	subs[2].substitution = "";
	if (resolvedname == 0) 
		resolvedname = XtFindFile(wmdPath,
					      subs,
					      XtNumber(subs),
					      (XtFilePredicate)NULL);

	if (resolvedname == 0)
	    {
	    diag_issue_diagnostic( d_wmd_open,
				   diag_k_no_source, diag_k_no_column,
				   Uil_cmd_z_command.ac_database);
	    }

	dbfile = fopen (resolvedname, "r");

	/* If the file is not found, a fatal error is generated.	*/
	if (dbfile == NULL)
	    {
	    diag_issue_diagnostic( d_src_open,
				   diag_k_no_source, diag_k_no_column,
				   resolvedname);
	    }

	return;
}




String get_root_dir_name()
{
	int uid;
	_Xgetpwparams pwd_buf;
	struct passwd *pwd_value;
	static char *ptr = NULL;
	char *outptr;

	if (ptr == NULL)
	{
	if((ptr = (char *)getenv("HOME")) == NULL) 
	    {
	    if((ptr = (char *)getenv(USER_VAR)) != NULL) 
		{
		pwd_value = _XGetpwnam(ptr, pwd_buf);
		}
	    else 
		{
		uid = getuid();
		pwd_value = _XGetpwuid(uid, pwd_buf);
		}
	    if (pwd_value != NULL)
		{
		ptr = pwd_value->pw_dir;
		}
	    else 
		{
		 ptr = "";
		}
	    }
	}

	outptr = XtMalloc (strlen(ptr) + 2);
	strcpy (outptr, ptr);
	strcat (outptr, "/");
	return outptr; 
}

/*
 * XAPPLRES_DEFAULT and UIDPATH_DEFAULT are intentionally split to support
 * SCCS. DO NOT reformat the lines else %-N-%-S could be converted by SCCS into
 * something totally bizarre causing MrmOpenHierarchy failures.
 */

/* The following are usually defined in the Makefile */

#ifndef LIBDIR
#define LIBDIR "/usr/lib/X11"
#endif
#ifndef INCDIR
#define INCDIR "/usr/include/X11"
#endif

static char libdir[] = LIBDIR;
static char incdir[] = INCDIR;

static char XAPPLRES_DEFAULT[] = "\
%%N\
%%S:\
%s/%%T/%%N\
%%S:\
%s%%T/%%N\
%%S:\
%s%%N\
%%S:\
%s/%%T/%%N\
%%S:\
%s/%%T/%%N\
%%S";

static char WMDPATH_DEFAULT[] = "\
%%N\
%%S:\
%s%%T/%%N\
%%S:\
%s%%N\
%%S:\
%s/%%L/%%T/%%N\
%%S:\
%s/%%T/%%N\
%%S";

static char ABSOLUTE_PATH[] = "\
%N\
%S";

String init_wmd_path(filename)
    String	filename ;
{
    String path;
    String old_path;
    String homedir;
    String wmd_path;


    if (filename[0] == '/')
	{
	wmd_path = XtMalloc(strlen(ABSOLUTE_PATH));
	strcpy (wmd_path, ABSOLUTE_PATH);
	}
    else
	{
	path = (char *)getenv ("WMDPATH");
	if (path  == NULL) 
	    {
	    homedir = get_root_dir_name();
	    old_path = (char *)getenv ("XAPPLRESDIR");
	    if (old_path == NULL) 
		{
		wmd_path = XtCalloc(1, 2*strlen(homedir) +
				 strlen(libdir) + strlen(incdir) +
				 strlen(WMDPATH_DEFAULT));
		sprintf( wmd_path, WMDPATH_DEFAULT,
			 homedir, homedir, libdir, incdir);
		} 
	    else 
		{
		wmd_path = XtCalloc(1, 1*strlen(old_path) + 2*strlen(homedir) +
				 strlen(libdir) + strlen(incdir) +
				 strlen(XAPPLRES_DEFAULT));
		sprintf(wmd_path, XAPPLRES_DEFAULT, 
			old_path,
			homedir, homedir, libdir, incdir);
		}
	    XtFree (homedir);
	    }
	else
	    {
	    wmd_path = XtMalloc(strlen(path) + 1);
	    strcpy (wmd_path, path);
	    free (path);
	    }
	}
    return (wmd_path);
}
