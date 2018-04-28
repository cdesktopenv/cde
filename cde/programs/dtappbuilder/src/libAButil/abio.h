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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */

/*
 *	$XConsortium: abio.h /main/5 1995/11/06 18:50:10 rswiston $
 *
 * @(#)abio.h	1.21 11 Feb 1994	cde_app_builder/src/libAButil
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */

/*
 * abio.h
 *
 * GUIDE Intermediate Language (GIL) file input / output interface.
 */

#ifndef _abio_h
#define _abio_h

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1		/* we want to be POSIX-compliant */
#endif

#include <stdio.h>
#include <ab_private/AB.h>
#include <ab_private/istr.h>
#include <ab/util_types.h>

/*
 * Public functions.
 */


/*
 * Indentation control
 */
int	abio_indent(FILE *text_file);
int	abio_outdent(FILE *text_file);
int	abio_set_indent(FILE *text_file, int level);
int	abio_get_indent(FILE *text_file);
int	abio_set_indent_char(FILE *text_file, int indent_char);
int	abio_get_indent_char(FILE *text_file);
int	abio_set_indent_chars_per_level(FILE *text_file, int chars_per_level);
int	abio_get_indent_chars_per_level(FILE *text_file);


int	abio_close_input(FILE *outFile);

int	abio_close_output(FILE *outFile);

int 	abio_access_file(
	    STRING 	name,
	    BOOL	*read_OK,
	    BOOL        *write_OK
	);

STRING	abio_comment_string(void);

STRING	abio_module_string(void);

STRING	abio_project_string(void);

int	abio_get_eof(FILE *inFile);

int	abio_get_file_begin(FILE *inFile);

int	abio_get_file_end(FILE *inFile);

int	abio_get_handler(
	    FILE	*inFile, 
	    ISTRING	*stringOut
	);
	
int	abio_get_integer(
	    FILE	*inFile,
	    int *i
	);

int	abio_get_keyword(
	    FILE	*inFile,
	    ISTRING	*stringOut
	);

int	abio_get_list(
	    FILE	*inFile,
	    ISTRING	*stringOut
	);

int	abio_get_list_begin(FILE *inFile);

int	abio_get_list_end(FILE *inFile);

int	abio_gil_get_full_name(
	    FILE	*inFile,
	    ISTRING	*interfaceOut,
	    ISTRING	*parentOut, 
	    ISTRING	*nameOut,
	    ISTRING	*itemOut
	);

int	abio_gil_get_object_begin(FILE *inFile);

int	abio_gil_get_object_end(FILE *inFile);

int	abio_get_string(
	    FILE	*inFile,
	    ISTRING	*stringOut
	);

int	abio_get_string_begin(FILE *inFile);

int	abio_get_string_end(FILE *inFile);

STRING	abio_integer_string(
	    int i
	);

STRING	abio_keyword_string(
	    STRING s
	);

STRING	abio_list_begin_string(void);

STRING	abio_list_end_string(void);

STRING	abio_name_string(
	    STRING s
	);

STRING	abio_gil_object_begin_string(void);

STRING	abio_gil_object_end_string(void);

STRING	abio_open_output(
	    STRING	name,
	    FILE	**pOutFile
	);

STRING	abio_open_gil_input(
	    STRING	name,
	    FILE	**pInFile
	);

STRING	abio_open_gil_output(
	    STRING 	outFileName,
	    FILE	**pOutFile
	);

int	abio_open_bil_output(
	    STRING 	outfile,
	    STRING	old_file,
	    FILE	**pOutFile
	);

STRING	abio_open_proj_input(
	    STRING	name,
	    FILE	**pInFile
	);

STRING	abio_open_proj_output(
	    STRING	name,
	    FILE	**pOutFile
	);

STRING	abio_open_resfile_input(
	    STRING	name,
	    FILE	**pInFile
	);

int
	abio_printf(
	    FILE * outFile, 
	    STRING fmt,...
	);

int
	abio_print_line(
	    FILE * outFile, 
	    STRING fmt,...
	);

int	abio_putc(
	    FILE *outFile,
	    char c
	);

int	abio_puts(
	    FILE *outFile,
	    STRING s
	);

int	abio_put_float(
	    FILE *outFile,
	    double d
	);

int	abio_put_integer(
	    FILE *outFile,
	    int i
	);

int	abio_put_keyword(
	    FILE *outFile,
	    AB_OBJECT_TYPE abo_type
	);

int	abio_put_keyword_name(
	    FILE *outFile,
	    STRING name
	);
int	abio_put_handler(
	    FILE *outFile,
	    STRING s
	);

int	abio_put_full_name(
	    FILE *outFile,
	    STRING parent, 
	    STRING name, 
	    STRING item
	);

int	abio_put_proj_full_name(
	    FILE *outFile,
	    STRING itf, 
	    STRING parent, 
	    STRING name, 
	    STRING item
	);

int	abio_put_string(
	    FILE *outFile,
	    STRING s
	);

int	abio_put_string_to_file(
	    FILE *outFile,
	    STRING s
	);

int 	abio_put_string_to_buffer(
	    STRING 	string,
	    STRING 	outBuf, 
	    int		size
	);

STRING	abio_string_begin_string(void);

STRING	abio_string_end_string(void);

STRING	abio_string_string(
	    STRING
	);

int	abio_open_block(FILE *outFile);

int	abio_close_block(FILE *outFile);

int	abio_expand_path(
	    STRING path
	);

int	abio_expand_file(
    	    char    *file,
    	    char    *exp_file 
	);

/*
 * BIL functions
 */
int	abio_expand_bil_proj_path(
	    STRING path
	);

int	abio_expand_bil_module_path(
	    STRING path
	);

int	abio_expand_bil_encapsulated_path(
    	    STRING path
	);

STRING	abio_open_bil_input(
	    STRING	fileName, 
	    int		*linesRead,
	    FILE	**pInFile
	);

float   abio_get_bil_version(
	    FILE *	fp,
	    int 	*pLinesRead
	);

STRING	abio_bil_object_begin_string(void);

STRING	abio_bil_object_end_string(void);

STRING	abio_bil_boolean_string(
	    BOOL	value
	);

STRING	abio_bil_true_string(void);

STRING	abio_bil_false_string(void);

int	abio_bil_get_boolean(
	    FILE	*inFile,
	    BOOL	*valueOut
	);

int	abio_bil_put_boolean(
	    FILE	*outFile,
	    BOOL	value
	);

STRING	abio_check_bil_version(
	    float ver,
	    STRING name
	);



/*
 * GIL functions
 */
int	abio_is_gil_path(
	    STRING path
	);

int	abio_gil_expand_path(
	    STRING path
	);

int	abio_gil_expand_proj_path(
	    STRING path
	);

int	abio_is_gil_proj_path(
	    STRING path
	);

int	abio_gil_put_name(
	    FILE *outFile,
	    STRING s
	);

int	abio_gil_get_name(
	    FILE	*inFile,
	    ISTRING	*stringOut
	);

STRING	abio_gil_boolean_string(
	    BOOL	value
	);

STRING	abio_gil_true_string(void);

STRING	abio_gil_false_string(void);

int	abio_gil_get_boolean(
	    FILE	*inFile,
	    BOOL	*valueOut
	);

int	abio_gil_put_boolean(
	    FILE	*outFile,
	    BOOL	value
	);


/*****************************************************************
**								**
**	Inline implementation					**
**								**
*****************************************************************/


#define abio_close_input(fp)  (abio_close_input_impl(&(fp)))
int abio_close_input_impl(FILE **pFp);

#define abio_close_output(fp)  (abio_close_output_impl(&(fp)))
int abio_close_output_impl(FILE **pFp);


#endif /* _abio_h */
