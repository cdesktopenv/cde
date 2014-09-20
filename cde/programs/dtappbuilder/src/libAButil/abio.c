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
 * $TOG: abio.c /main/6 1998/04/06 13:12:48 mgreess $
 * 
 * @(#)abio.c	1.87 04 May 1995
 * 
 * RESTRICTED CONFIDENTIAL INFORMATION:
 * 
 * The information in this document is subject to special restrictions in a
 * confidential disclosure agreement between HP, IBM, Sun, USL, SCO and
 * Univel.  Do not distribute this document outside HP, IBM, Sun, USL, SCO,
 * or Univel without Sun's specific written approval.  This document and all
 * copies and derivative works thereof must be returned or destroyed at Sun's
 * request.
 * 
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 * 
 */


/*
 * abio.c
 * 
 * Guide Intermediate Language (GIL) file input / output interface.
 * 
 * This file implements the functions required to read and write GIL files.  The
 * GIL syntax is based on Lisp so that they may be read directly into a Lisp
 * or Scheme interpreter.
 * 
 * Many of these functions return a string to indicate the begin or end of a
 * special sequence in the GIL syntax, for example, a list. Internally it is
 * common to only use the first character returned. I did it this way so the
 * functions are consistent and to allow for flexibility in the future.
 */

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif

#include <sys/types.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <ab_private/util.h>
#include <ab_private/abio.h>
#include <ab/util_types.h>
#include <ab_private/istr.h>
#include "utilP.h"

/*
 * Not defined in POSIX, but needed...
 */
#ifdef __cplusplus
extern "C" {
#endif
extern int ftruncate(int fd, off_t size);	/* truncates file */
#ifdef __cplusplus
} // extern "C" {
#endif

#define util_dassert(_debugLevel, _boolExpr) \
	    {if (debug_level() >= (_debugLevel)) {assert((_boolExpr));}}


/*
 * Delimiter for a full name.
 */
#define DELIMITER		':'
#define INDENT_ARRAY_SIZE	(132)
#define INDENT_MAX		((INDENT_ARRAY_SIZE-1)/Indent_chars_per_level)

/*
 * Internal state variables.
 */
static char     Indent[INDENT_ARRAY_SIZE]= "";	/* indent string */
static int      Indent_chars_per_level = 4;
static char	Indent_char = ' ';
static int      Indent_level = 0;
static int      Newline = TRUE;	/* TRUE if on new output line */

#ifndef MAXBSIZE
#  define MAXBSIZE 8192
#endif
static char     Buf[MAXBSIZE];	/* work buffer */

/*
 * System error message definitions.
 */

/*
 * Internal function declarations.
 */
static int	    debug_unbuffer_file(FILE *);	/*use on output files*/
static int          gil_version(FILE *);
static void         skip_comments(FILE *);
static STRING       get_token(FILE *);
static int	abioP_build_indent_string(
			FILE *out_file,
			int	indent_char,
			int	indent_chars_per_level,
			int	indent_level
		);

static UtilOutputHandler	output_handler = NULL;
static UtilOutputHandler	err_output_handler = NULL;
static STRING			help_text = NULL;
static STRING			help_volume = NULL;
static STRING			help_locID = NULL;
static STRING	gilVersionPrefix = ";GIL-";
static int	gilMajorVersion	= 4;
static int	gilMinorVersion	= 0;

/*
 * Return the string representation of a boolean.
 */
STRING
abio_bil_boolean_string(BOOL value)
{
    return value? abio_bil_true_string() : abio_bil_false_string();
}

/*
 * Close the input file.
 */
int
abio_close_input_impl(FILE ** pFile)
{
#define file (*pFile)
    if ((file != NULL) && (file != stdin))
    {
	util_fclose(file);
    }
    return 0;
#undef file
}

/*
 * Close the output file.
 */
int
abio_close_output_impl(FILE ** pOutFile)
{
#define outFile (*pOutFile)
    if ((outFile != NULL) && (outFile != stdout))
    {
	fflush(outFile);
	ftruncate(fileno(outFile), (size_t)ftell(outFile));
	util_fclose(outFile);
    }
    return 0;
#undef outFile
}

/*
 * Return the comment string.
 */
STRING
abio_comment_string(void)
{
    return "//";
}

/*
 * Return the boolean false string.
 */
STRING
abio_bil_false_string(void)
{
    return ":false";
}

/*
 * Return the file begin string.
 */
STRING
abio_file_begin_string(void)
{
    return ":bil-version";
}

/*
 * Return the file end string.
 */
STRING
abio_file_end_string(void)
{
    return ")";
}

/*
 * Get a boolean from the input file.  Returns TRUE if successful.
 */
int
abio_bil_get_boolean(FILE * inFile, BOOL *valueOut)
{
    STRING              p = get_token(inFile);

    if (strcmp(p, abio_bil_true_string()) == 0)
    {
	(*valueOut) = TRUE;
	return TRUE;
    }
    if (strcmp(p, abio_bil_false_string()) == 0)
    {
	(*valueOut) = FALSE;
	return TRUE;
    }
    return FALSE;
}

/*
 * Return whether we are at end-of-file.
 */
int
abio_get_eof(FILE * inFile)
{
    int                 ch;

    while (isspace(ch = fgetc(inFile)))
	;
    if (ch == EOF)
	return TRUE;
    ungetc(ch, inFile);
    return FALSE;
}

/*
 * Get the start of the file.  Returns TRUE if successful.
 */
int
abio_get_file_begin(FILE * inFile)
{
    int                 ch;

    while (isspace(ch = fgetc(inFile)))
	;
    if (ch == *abio_file_begin_string())
	return TRUE;
    ungetc(ch, inFile);
    return FALSE;
}

/*
 * Get the end of the file.  Returns TRUE if successful.
 */
int
abio_get_file_end(FILE * inFile)
{
    int                 ch;

    while (isspace(ch = fgetc(inFile)))
	;
    if (ch == *abio_file_end_string())
	return TRUE;
    ungetc(ch, inFile);
    return FALSE;
}

/*
 * Get a handler from the input file.  Returns TRUE if successful. Sets the
 * string pointer to a buffer allocated with malloc if the string is not
 * empty, otherwise NULL.  Handler could be a quoted string for special
 * language cases (PostScript, Lisp, etc...) or just a simple name for C.
 */
int
abio_get_handler(FILE * inFile, ISTRING * stringOutPtr)
{
#define stringOut (*stringOutPtr)
#define	INC		32
    int                 ch;
    int                 len;	/* string length */
    int                 c = 0;	/* string count */
    STRING		string = NULL;

    while (isspace(ch = fgetc(inFile)))
	;
    ungetc(ch, inFile);

    if (ch != *abio_string_begin_string())
    {
	return abio_gil_get_name(inFile, &stringOut);
    }
    else
    {
	string = (STRING) malloc(len = INC);
	string[c++] = fgetc(inFile);
	while (((ch = fgetc(inFile)) != EOF) &&
	       (ch != *abio_string_end_string()))
	{
	    if (c + 2 == len)
		string = (STRING) realloc(string, len += INC);
	    if (ch == '\\')
		ch = fgetc(inFile);
	    string[c++] = (char) ch;
	}
	string[c] = *abio_string_end_string();
	string[c + 1] = '\0';
	stringOut = istr_create(string);
	util_free(string);
	return TRUE;
    }

    return FALSE;
#undef INC
#undef stringOut
}


/*
 * Get a boolean from the input file.  Returns TRUE if successful.
 */
int
abio_gil_get_boolean(FILE * inFile, BOOL *valueOut)
{
    STRING              p = get_token(inFile);

    if (strcmp(p, abio_gil_true_string()) == 0)
    {
	(*valueOut) = TRUE;
	return TRUE;
    }
    if (strcmp(p, abio_gil_false_string()) == 0)
    {
	(*valueOut) = FALSE;
	return TRUE;
    }
    return FALSE;
}

/*
 * Return the boolean false string.
 */
STRING
abio_gil_false_string(void)
{
    return "nil";
}


/*
 * Return the boolean true string.
 */
STRING
abio_gil_true_string(void)
{
    return "t";
}


/*
 * Get an integer from the input file.  Returns TRUE if successful.
 */
int
abio_get_integer(FILE * inFile, int *i)
{
    STRING              p = get_token(inFile);
    STRING              q;
    long                l = strtol(p, &q, 10);

    if (p != q)
    {
	*i = (int) l;
	return TRUE;
    }
    return FALSE;
}

/*
 * Get a keyword from the input file.  Returns TRUE if successful. The
 * keyword is returned in a static buffer.
 */
int
abio_get_keyword(FILE * inFile, ISTRING *s)
{
    *s = istr_create(get_token(inFile));
    return TRUE;
}

/*
 * Get a list as a string from the input file.  Returns TRUE if successful.
 * Sets the string pointer to a buffer allocated with malloc if the string is
 * not empty, otherwise NULL.
 */
int
abio_get_list(FILE * inFile, ISTRING *stringOutPtr)
{
#define stringOut (*stringOutPtr)
#define	INC		32
    int                 ch;
    int                 len;	/* string length */
    int                 c;	/* string count */
    STRING		string = NULL;

    stringOut = NULL;
    while (isspace(ch = fgetc(inFile)))
	;
    if (ch != *abio_list_begin_string())
    {
	ungetc(ch, inFile);
	return FALSE;
    }

    if (abio_get_list_end(inFile))
    {
	return TRUE;
    }

    string = (STRING) malloc(len = INC);
    for (c = 0; (ch = fgetc(inFile)) != *abio_list_end_string(); c++)
    {
	if (c + 1 == len)
	    string = (STRING) realloc(string, len += INC);
	if (ch == '\\')
	    ch = fgetc(inFile);
	string[c] = (char) ch;
    }
    string[c] = '\0';
    stringOut = istr_create(string);
    return TRUE;
#undef INC
#undef stringOut
}

/*
 * Get the start of a list from the input file.  Returns TRUE if successful.
 */
int
abio_get_list_begin(FILE * inFile)
{
    int                 ch;

    while (isspace(ch = fgetc(inFile)))
	;
    if (ch == *abio_list_begin_string())
	return TRUE;
    ungetc(ch, inFile);
    return FALSE;
}

/*
 * Get the end of a list from the input file.  Returns TRUE if successful.
 */
int
abio_get_list_end(FILE * inFile)
{
    int                 ch;

    while (isspace(ch = fgetc(inFile)))
	;
    if (ch == *abio_list_end_string())
	return TRUE;
    ungetc(ch, inFile);
    return FALSE;
}

/*
 * Get a symbol from the input file.  Returns TRUE if successful. Sets the
 * string pointer to a buffer allocated with malloc if the string is not
 * empty, otherwise NULL.
 */
int
abio_gil_get_name(FILE * inFile, ISTRING *stringOutPtr)
{
#define stringOut (*stringOutPtr)
    STRING              p = get_token(inFile);
    stringOut = NULL;

    if (p == NULL)
    {
	return FALSE;
    }
    if (strcmp(p, abio_gil_false_string()) == 0)
    {
        stringOut = NULL;
    }
    else
    {
        stringOut = istr_create(p);
    }
    return TRUE;
#undef stringOut
}


#ifdef BOGUS
/*
 * Get the full name of an object in a bil file. The syntax of a full name
 * is: (name)          when the object is top-level; (parent name)   when the
 * object is not top-level; (parent name "item")    when the object is a
 * setting or menu item.
 */
int
abio_bil_get_full_name(
			FILE * inFile, 
			ISTRING *parentOutPtr, 
			ISTRING *nameOutPtr)
{
#define parentOut (*parentOutPtr)
#define nameOut (*nameOutPtr)
    int                 iReturn = 0;

    iReturn = ERR_NOT_IMPLEMENTED;
#ifdef BOGUS			/* this should NOT call anything in libABil */
    STRING              tmp1,
                        tmp2;
    STRING              ident = NULL;
    char                ch[4];
    ISTRING             string;
    BOOL                tmp1_done = FALSE;
    int                 type;
    int                 i,
                        y;

    *parent = NULL;
    *name = NULL;

    /** REMIND: this shouldn't oughtta be here
    if ((type = bil_load_get_value_type()) == AB_BIL_VALUE_IDENT)
        ident = bil_load_get_value();
    else
        util_error(abo_loadmsg(ERR_WANT_STRING));
    **/

    string = istr_create(ident);
    tmp1 = (STRING) malloc(strlen(istr_string(string)) + 1);
    for (i = 0, y = 0; i < ((int) strlen(istr_string(string))); i++, y++)
    {
	ch[0] = ident[i];
	ch[1] = '\0';
	if ((strcmp(".", ch)) == 0)
	{
	    tmp1[i] = '\0';
	    y = 0;
	    tmp1_done = TRUE;
	    tmp2 = (STRING) malloc(strlen(istr_string(string)) + 1);
	}
	else if (tmp1_done == FALSE)
	{
	    tmp1[i] = ident[i];
	}
	else if (tmp1_done == TRUE)
	{
	    tmp2[y - 1] = ident[i];
	}

    }
    if (tmp1_done == FALSE)
    {				/* only one */
	*name = (STRING) malloc(strlen(tmp1) + 1);
	strcpy(*name, tmp1);
	istr_destroy(string);
	free(tmp1);
	return TRUE;
    }
    else
    {				/* tmp1_done == TRUE */
	tmp2[y] = '\0';
	*parent = (STRING) malloc(strlen(tmp1) + 1);
	*name = (STRING) malloc(strlen(tmp2) + 1);
	strcpy(*parent, tmp1);
	strcpy(*name, tmp2);
	istr_destroy(string);
	free(tmp1);
	free(tmp2);
	return TRUE;
    }
#endif				/* BOGUS */

    return iReturn;
#undef parentOut
#undef nameOut
}
#endif /* BOGUS */


/*
 * Get the full name of an object in a gil file. The syntax of a full name
 * is one of: 
 *	(obj-name)    			when the object is top-level
 *	(toplevel-name obj-name)	when the object is not top-level
 *	(obj-name "label")		when the object is an item
 *	(file-name toplevel-name obj-name)	when obj is cross-interface
 *	(file-name toplevel-name "label")	when obj is cross-interface
 *
 * Returns >= 0 if successful, <0 on failure
 */
int
abio_gil_get_full_name(
			FILE	*inFile, 
			ISTRING	*interfaceStringOut,
			ISTRING	*parentStringOut,
			ISTRING	*objStringOut,
			ISTRING *itemStringOut
)
{
    int			return_value = 0;
    ISTRING		tmp1 = NULL;
    ISTRING		tmp2 = NULL;
    BOOL		tmp2IsString = FALSE;
    ISTRING		tmp3 = NULL;
    BOOL		tmp3IsString = FALSE;

    *interfaceStringOut = NULL;
    *parentStringOut = NULL;
    *objStringOut = NULL;
    *itemStringOut = NULL;

    if (!abio_get_list_begin(inFile))
    {
	return_value = -1;
	goto epilogue;
    }

    /*
     * Get first value - always name
     */
    if (!abio_gil_get_name(inFile, &tmp1))
    {
	return_value = -1;
	goto epilogue;
    }
    if (abio_get_list_end(inFile))
    {
	goto epilogue;
    }

    /*
     * Get second value - string or name
     */
    if (abio_gil_get_name(inFile, &tmp2))
    {
	tmp2IsString = FALSE;
    }
    else if (abio_get_string(inFile, &tmp2))
    {
	tmp2IsString = TRUE;
	/* we've seen: (name "string" */
	if (!abio_get_list_end(inFile))
	{
	    return_value = -1;
	    goto epilogue;
	}
	goto epilogue;
    }
    else
    {
	return_value = -1;
	goto epilogue;
    }
    if (abio_get_list_end(inFile))
    {
	goto epilogue;
    }

    /*
     * Get third value - string or name
     */
    if (abio_gil_get_name(inFile, &tmp3))
    {
	tmp3IsString = FALSE;
    }
    else if (abio_get_string(inFile, &tmp3))
    {
	tmp3IsString = TRUE;
    }
    else
    {
	return_value = -1;
	goto epilogue;
    }
    if (!abio_get_list_end(inFile))
    {
	/* 3 values seen - must be at end of list */
	return_value = -1;
	goto epilogue;
    }

epilogue:
    if (return_value >= 0)
    {
	if (tmp3 != NULL)
	{
	    /* 3 values */
	    if (tmp3IsString)
	    {
	        (*parentStringOut) = istr_dup(tmp1);
	        (*objStringOut) = istr_dup(tmp2);
	        (*itemStringOut) = istr_dup(tmp3);
	    }
	    else
	    {
		(*interfaceStringOut) = istr_dup(tmp1);
		(*parentStringOut) = istr_dup(tmp2);
		(*objStringOut) = istr_dup(tmp3);
	    }
	}
	else if (tmp2 != NULL)
	{
	    /* 2 strings */
	    if (tmp2IsString)
	    {
		(*objStringOut) = istr_dup(tmp1);
		(*itemStringOut) = istr_dup(tmp2);
	    }
	    else
	    {
	        (*parentStringOut) = istr_dup(tmp1);
	        (*objStringOut) = istr_dup(tmp2);
	    }
	}
	else if (tmp1 != NULL)
	{
	    /* 1 string */
	    (*objStringOut) = istr_dup(tmp1);
	}
    }
    istr_destroy(tmp1);
    istr_destroy(tmp2);
    istr_destroy(tmp3);

#ifdef DEBUG
    /*util_dprintf(3, "abio_gil_get_full_name() -> %d(%s %s %s %s)\n",
	return_value, 
	istr_string_safe(*interfaceStringOut),
	istr_string_safe(*parentStringOut),
	istr_string_safe(*objStringOut),
	istr_string_safe(*itemStringOut));*/
#endif
    return return_value;
}


#ifdef BOGUS /* obsoleted by libABil */
/*
 * Get the full name of an object in a project file. The syntax of a full
 * name is: interface:name		when the object is top-level;
 * interface:parent:name	when the object is not top-level;
 * interface:parent:name:"item"	when the object is a setting or menu item.
 */
int
abio_get_proj_full_name(FILE * inFile, STRING * interface, STRING * parent,
			STRING * name, STRING * item)
{
    STRING              tmp1,
                        tmp2;

    *interface = NULL;
    *parent = NULL;
    *name = NULL;
    *item = NULL;

    if (!abio_get_list_begin(inFile))
	return FALSE;

    if (!abio_gil_get_name(inFile, &tmp1))
	return FALSE;
    *interface = (STRING) malloc(strlen(tmp1) + 1);
    strcpy(*interface, tmp1);
    free(tmp1);

    if (!abio_gil_get_name(inFile, &tmp1))
	return FALSE;

    if (abio_get_list_end(inFile))
    {
	*name = (STRING) malloc(strlen(tmp1) + 1);
	strcpy(*name, tmp1);
	return TRUE;
    }

    if (abio_get_string(inFile, &tmp2))
    {
	*name = (STRING) malloc(strlen(tmp1) + 1);
	*item = (STRING) malloc(strlen(tmp2) + 1);
	strcpy(*name, tmp1);
	strcpy(*item, tmp2);
	if (!abio_get_list_end(inFile))
	    return FALSE;
	return TRUE;
    }

    *parent = (STRING) malloc(strlen(tmp1) + 1);
    strcpy(*parent, tmp1);
    free(tmp1);
    if (abio_gil_get_name(inFile, &tmp1))
    {
	*name = (STRING) malloc(strlen(tmp1) + 1);
	if (!abio_get_list_end(inFile))
	    if (abio_get_string(inFile, &tmp2))
	    {
		*item = (STRING) malloc(strlen(tmp2) + 1);
		strcpy(*name, tmp1);
		strcpy(*item, tmp2);
		if (abio_get_list_end(inFile))
		    return TRUE;
		return FALSE;
	    }
	strcpy(*name, tmp1);
	return TRUE;
    }
    return FALSE;

}
#endif /* BOGUS */


/*
 * Get the start of an object from the input file.  Returns TRUE if
 * successful.
 */
int
abio_gil_get_object_begin(FILE * inFile)
{
    int                 ch = 0;
    BOOL		objectFound = FALSE;

    util_dassert(1, (strcmp(abio_gil_object_begin_string(), "(") == 0));
							/* ) vi hack */

    while ((!objectFound) && ((ch = fgetc(inFile)) != EOF))
    {
	switch (ch)
	{
	    case ';':		/* GIL comment */
		while (   ((ch = fgetc(inFile)) != EOF)
		       && (ch != '\n') )
		{
		}
	    break;

	    case '(':		/* object begin string */
		objectFound = TRUE;
	    break;
	}
    }

    if (!objectFound)
    {
        if (ch != EOF)
        {
	    ungetc(ch, inFile);
	}
	return FALSE;
    }
    return TRUE;
}

/*
 * Get the end of an object from the input file.  Returns TRUE if successful.
 */
int
abio_gil_get_object_end(FILE * inFile)
{
    int                 ch;

    while (isspace(ch = fgetc(inFile)))
	;
    if (ch == *abio_gil_object_end_string())
	return TRUE;
    ungetc(ch, inFile);
    return FALSE;
}


/*
 * Get a string from the input file.  Returns TRUE if successful. 
 *
 * The ISTRING returned must be destroyed by the caller.
 */
int
abio_get_string(FILE * inFile, ISTRING *istringOutPtr)
{
#define istringOut (*istringOutPtr)
#define	INC		256			/* incremental allocation */
    BOOL		return_value = TRUE;
    int                 ch;
    int                 len;	/* string length */
    int                 c = 0;	/* string count */
    STRING		string = NULL;		/* string */

    istringOut = NULL;		/* necessary (see epilogue) */

    while (isspace(ch = fgetc(inFile)))
	;
    if (ch != *abio_string_begin_string())
    {
	ungetc(ch, inFile);
	return_value = FALSE;
	goto epilogue;
    }

    if (abio_get_string_end(inFile))
    {
	istringOut = istr_const("");
	goto epilogue;
    }

    string = (STRING)malloc(len = INC);
    while (((ch = fgetc(inFile)) != EOF) && (ch != *abio_string_end_string()))
    {
	if (c + 1 == len)
	    string = (STRING)realloc(string, len += INC);
	if (ch == '\\')
	{
	    ch = fgetc(inFile);
	    if (ch == 'n')	/* read in newline, '\n' */
		string[c++] = (char) '\n';
	    else
		string[c++] = (char) ch;
	}
	else
	{
	    string[c++] = (char) ch;
	}
    }
    string[c] = '\0';

epilogue:
    if (istringOut == NULL)
    {
        istringOut = istr_create_alloced(string);
    }
    return return_value;
#undef INC
#undef istringOutPtr
}


/*
 * Get the start of a string from the input file.  Returns TRUE if
 * successful.
 */
int
abio_get_string_begin(FILE * inFile)
{
    int                 ch;

    while (isspace(ch = fgetc(inFile)))
	;
    if (ch == *abio_string_begin_string())
	return TRUE;
    ungetc(ch, inFile);
    return FALSE;
}

/*
 * Get the end of a string from the input file.  Returns TRUE if successful.
 */
int
abio_get_string_end(FILE * inFile)
{
    int                 ch;

    if ((ch = fgetc(inFile)) == *abio_string_end_string())
	return TRUE;
    ungetc(ch, inFile);
    return FALSE;
}

/*
 * Write an integer to the output file.
 */
STRING
abio_integer_string(int i)
{
    sprintf(Buf, "%d", i);
    return Buf;
}

/*
 * Return the string representation of a keyword.
 */
STRING
abio_keyword_string(STRING s)
{
    return s;
}

/*
 * Return the list begin string.
 */
STRING
abio_list_begin_string(void)
{
    return "(";
}

/*
 * Return the list end string.
 */
STRING
abio_list_end_string(void)
{
    return ")";
}

/*
 * Return the string representation of a name.
 */
STRING
abio_name_string(STRING s)
{
    return s ? s : abio_bil_false_string();
}

/*
 * Return the object begin string.
 */
STRING
abio_gil_object_begin_string(void)
{
    return "(";
}

/*
 * Return the object end string.
 */
STRING
abio_gil_object_end_string(void)
{
    return ")";
}


/*
 * Open an input file.  Returns NULL if successful, otherwise an error
 * message.
 * 
 * pLinesRead is set to the number of lines read from the file.
 */
STRING
abio_open_bil_input(STRING name, int *pLinesRead, FILE ** pInFile)
{
#define inFile (*pInFile)
    STRING	errmsg = NULL;
    float	v = 0.0;

    inFile = NULL;
    /*
     * If the input file exists, can be read, and is
     * the correct version, then open it.
     */
    if ((inFile = util_fopen_locked(name, "r")) != NULL)
    {
	v = abio_get_bil_version(inFile, pLinesRead);
	errmsg = abio_check_bil_version(v, name);
    }
    else
    {
	sprintf(Buf, "%s: %s", name, strerror(errno));
	errmsg = Buf;
    }

    return errmsg;

#undef MAXVER
#undef inFile
}

STRING
abio_check_bil_version(
    float ver,
    STRING name
)
{
#define MAXVER 10
    STRING      errmsg = NULL;
    STRING      help_msg = NULL;
    STRING      help_buf = NULL;
    STRING	fileName = NULL;

    fileName = name? name : catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 35, "NoName");
    fileName = strdup(fileName);

    if (ver > Util_major_version)
    {
	errmsg = strdup(catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 32,
		"Incompatible BIL version"));

	help_msg = catgets(UTIL_MESSAGE_CATD,
                UTIL_MESSAGE_SET, 33,
                "The version of the BIL file %s (%3.2f)\nis not supported in this version of App Builder.");

	help_buf = (STRING)util_malloc(strlen(help_msg) + strlen(fileName) + MAXVER +1);
	sprintf(help_buf, help_msg, fileName, ver);
	util_set_help_data(help_buf, NULL, NULL);
    }
    else if (ver == 0.0)
    {
	errmsg = strdup(catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 30,
			"Unrecognized file format"));

	help_msg = catgets(UTIL_MESSAGE_CATD,
                UTIL_MESSAGE_SET, 31,
                "The file %s does not appear to be in BIL\nformat.  Either a BIL header (:bil-version) was\nnot found or the header is corrupt.");

	help_buf = (STRING)util_malloc(strlen(help_msg) + strlen(fileName) + 1);
	sprintf(help_buf, help_msg, fileName);
	util_set_help_data(help_buf, NULL, NULL);
    }
    
    if (errmsg)
    {
	sprintf(Buf, "%s: %s", fileName, errmsg);
	free(errmsg);
	errmsg = Buf;
    }

    if (help_buf)
	util_free(help_buf);

    if (fileName)
	free(fileName);

    return errmsg;
}

/*
 * Open an output file.  Returns NULL if successful, otherwise an error
 * message.
 */
int
abio_open_bil_output(
    STRING	outfile,
    STRING	old_file, 
    FILE	**pOutFile
)
{
#define outFile (*pOutFile)
    int                 version_written = FALSE;
    char                backup[MAXPATHLEN];
    FILE               *bakp = NULL;
    STRING              file_begin = abio_file_begin_string();
    int                 fblen = strlen(file_begin);
    int			iRet = OK;
    BOOL		read_OK = FALSE,
			write_OK = FALSE;

    *backup = 0;

    /* Look for the specified output file.
     */
    if (!util_file_exists(outfile))
    {
	/*
	 * Output file does not exist.  Open a new one.
	 */
	if ((outFile = util_fopen_locked(outfile, "w")) != NULL)
	{
	    debug_unbuffer_file(outFile);
	    abio_set_indent_char(outFile, '\t');
	    abio_set_indent_chars_per_level(outFile, 1);
	}
	else 
	{
	    iRet = ERR_WRITE;
	    goto ERROR_EXIT;
	}
    }
    else
    {
	/* The output file exists.  Make sure it is
	 * writable before backing it up.
	 */
	abio_access_file(outfile, &read_OK, &write_OK);
	if (write_OK)
	{
	    /* Rename existing file to a backup and
	     * start a new output file.
	     */
	    sprintf(backup, "%s.BAK", outfile);
	    if ((rename(outfile, backup) != OK) ||
		!(bakp = util_fopen_locked(backup, "r")) ||
		!(outFile = util_fopen_locked(outfile, "w")))
	    {
		iRet = ERR_BACKUP;
		goto ERROR_EXIT;
	    }
	    debug_unbuffer_file(outFile);
	    abio_set_indent_char(outFile, '\t');
	    abio_set_indent_chars_per_level(outFile, 1);
	}
	else	/* The file is read-only */
	{
	    iRet = ERR_READ_ONLY;
	    goto ERROR_EXIT;	/* file access error */
	}
    }

    /* If the file has been saved before, then read
     * in any beginning comments from the old file
     * and transfer them to the new file.
     */
    if (old_file || bakp)
    {
	debug_unbuffer_file(outFile);
	abio_set_indent_char(outFile, '\t');
	abio_set_indent_chars_per_level(outFile, 1);

	if (bakp == NULL)
	{
	    bakp = util_fopen_locked(old_file, "r");
	    if (bakp == NULL)
	    {
		iRet = ERR_READ;
		goto ERROR_EXIT;	/* file access error */
	    }
	}

	/*
	 * Preserve leading text until :bil-version string.
	 */
	while (fgets(Buf, sizeof(Buf), bakp) &&
	       strncmp(Buf, file_begin, fblen) != 0)
	{
	    fputs(Buf, outFile);
	}
	fclose(bakp);
    }

    /*
     * Write the BIL version string
     */
    fprintf(outFile, "%s\t%d %d\n",
		AB_VERSION_PREFIX,
		Util_major_version,
		Util_minor_version);

ERROR_EXIT:
    return iRet;

#undef outFile
}

/*
 * Return the object begin string.
 */
STRING
abio_bil_object_begin_string(void)
{
    return "(";
}

/*
 * Return the object end string.
 */
STRING
abio_bil_object_end_string(void)
{
    return ")";
}

/*
 * Open an input file.  Returns NULL if successful, otherwise an error
 * message.
 */
STRING
abio_open_gil_input(STRING name, FILE ** pInFile)
{
#define inFile (*pInFile)
    int                 v = 0;
    STRING              errmsg = NULL;
    int			ch = -1;

    /*
     * If the input file exists and is the correct version, open it and skip
     * leading comments.
     */
    if (inFile = util_fopen_locked(name, "r"))
    {
	v = gil_version(inFile);
	if (v < 1)
	{
	    /* No dgettext() wrapper on purpose */
	    errmsg = "unrecognized file format";	
	}
	else if (v > gilMajorVersion)
	{
	    /* REMIND: should be "Bad GIL version" */
	    /* No dgettext() wrapper on purpose */
	    errmsg = "unrecognized file format";	
	}
    }
    else
    {
      errmsg = strerror(errno);
    }

    /*
     * Otherwise, return an error message.
     */
    if (errmsg != NULL)
    {
        sprintf(Buf, "%s: %s", name, errmsg);
	return Buf;
    }
    return NULL;
#undef inFile
}


/*
 * Open an output file.  Returns NULL if successful, otherwise an error
 * message.
 */
STRING
abio_open_gil_output(STRING outfile, FILE ** pOutFile)
{
#define outFile (*pOutFile)
    struct stat         statbuf;
    int                 version_written = FALSE;

    /*
     * Look for the specified output file.
     */
    if (stat(outfile, &statbuf) != OK)
    {

	/*
	 * outFileut file does not exits.  Open a new one.
	 */
	if (outFile = util_fopen_locked(outfile, "w"))
	{
	    debug_unbuffer_file(outFile);

	    /*
	     * First line is the version number.
	     */
	    fprintf(outFile, "%s%d\n", gilVersionPrefix, gilMajorVersion);
	    return NULL;
	}
	else
	    goto ERROR_EXIT;
    }

    /*
     * The output file exists.  Make sure we can successfully open it before
     * backing it up.
     */
    if (outFile = util_fopen_locked(outfile, "a"))
    {
	char                backup[MAXPATHLEN];
	FILE               *bakp = NULL;
	STRING              comment = abio_comment_string();
	int                 len = strlen(comment);

	debug_unbuffer_file(outFile);

	/*
	 * Rename existing file to a backup and start a new output file.
	 */
	fclose(outFile);
	sprintf(backup, "%s.BAK", outfile);
	if ((rename(outfile, backup) != OK) ||
	    !(bakp = util_fopen_locked(backup, "r")) ||
	    !(outFile = util_fopen_locked(outfile, "w")))
	    goto ERROR_EXIT;	/* file access error */

	debug_unbuffer_file(outFile);

	/*
	 * Ignore any unrelated leading text until first comment (eg. garbage
	 * from mailtool).
	 */
	while (fgets(Buf, sizeof(Buf), bakp) &&
	       strncmp(Buf, comment, len) != 0)
	    ;

	/*
	 * Preserve any comments.
	 */
	do
	{
	    if (!version_written)
	    {

		/*
		 * First comment is always version number.
		 */
		fprintf(outFile, "%s%d\n",
			gilVersionPrefix,
			Util_major_version);
		version_written = TRUE;
	    }
	    else
		fputs(Buf, outFile);
	}
	while (fgets(Buf, sizeof(Buf), bakp) &&
	       strncmp(Buf, comment, len) == 0);

	fclose(bakp);
	return NULL;
    }

ERROR_EXIT:

    /*
     * Return a message if unsuccessful.
     */
    sprintf(Buf, "%s: %s", outfile, strerror(errno));
    return Buf;
#undef outFile
}


/*
 * Open an input proj file.  Returns NULL if successful, otherwise an error
 * message.
 */
STRING
abio_open_proj_input(STRING name, FILE ** pInFile)
{
    return abio_open_gil_input(name, pInFile);
}


/*
 * Open an output proj file.  Returns NULL if successful, otherwise an error
 * message.
 */
STRING
abio_open_proj_output(STRING name, FILE ** pOutFile)
{
    return abio_open_gil_output(name, pOutFile);
}


/*
 * Open an input resource file.	Returns NULL is successful, otherwise an
 * error message.
 */
STRING
abio_open_resfile_input(STRING name, FILE ** pInFile)
{
    return abio_open_gil_input(name, pInFile);
}

/*
 * Open an output file.  Returns NULL if successful, otherwise an error
 * message.
 */
STRING
abio_open_output(STRING name, FILE ** pOutFile)
{
#define outFile (*pOutFile)
    if (name == NULL)
    {
	outFile = tmpfile();
    }
    else
    {
        outFile = util_fopen_locked(name, "w");
    }
    if (outFile != NULL)
    {
        debug_unbuffer_file(outFile);
	abio_set_indent_char(outFile, ' ');
	abio_set_indent_chars_per_level(outFile, 4);
	return NULL;
    }

    sprintf(Buf, "%s: %s", name, strerror(errno));
    return Buf;
#undef outFile
}

/*
 * fprintf to the current output file.
 */
int
abio_printf(FILE *outFile, STRING fmt, ...)
{
    va_list             args;

    if (Newline)
	fputs(Indent, outFile);

    va_start(args, fmt);
    vfprintf(outFile, fmt, args);
    va_end(args);

    Newline = fmt[strlen(fmt) - 1] == '\n';
    return 0;
}

int
abio_print_line(FILE *outFile, STRING fmt,...)
{
    int                 iReturn = 0;
    va_list             args;

    va_start(args, fmt);
    if (fmt == NULL)
    {
	fputs("\n", outFile);
	Newline = TRUE;
    }
    else
    {
	if (Newline)
	    fputs(Indent, outFile);

        iReturn = vfprintf(outFile, fmt, args);
	Newline = (fmt[strlen(fmt)-1] == '\n');
    }
    va_end(args);
    return iReturn;
}

int
abio_open_block(FILE * outFile)
{
    fprintf(outFile, "\n%s{", Indent);
    abio_indent(outFile);
    return 0;
}

int
abio_close_block(FILE * outFile)
{
    abio_outdent(outFile);
    fprintf(outFile, "\n%s}", Indent);
    return 0;
}

/*
 * Write a character to the output file, preceeded by the indent string if we
 * are on a new line.
 */
int
abio_putc(FILE * outFile, char c)
{
    if (Newline)
	fputs(Indent, outFile);
    fputc(c, outFile);
    Newline = (c == '\n');
    return 0;
}

/*
 * Write characters to the output file, preceeded by the indent string if we
 * are on a new line.
 */
int
abio_puts(FILE * outFile, STRING s)
{
    if (Newline)
	fputs(Indent, outFile);
    fputs(s, outFile);
    Newline = s[strlen(s) - 1] == '\n';
    return 0;
}

/*
 * Write a boolean to the output file.
 */
int
abio_bil_put_boolean(FILE * outFile, BOOL value)
{
    return abio_puts(outFile, abio_bil_boolean_string(value));
}

/*
 * Write an integer to the output file.
 */
int
abio_put_float(FILE * outFile, double d)
{
    sprintf(Buf, "%f", d);
    return abio_puts(outFile, Buf);
}

/*
 * Write an integer to the output file.
 */
int
abio_put_integer(FILE * outFile, int i)
{
    sprintf(Buf, "%d", i);
    return abio_puts(outFile, Buf);
}

int
abio_put_keyword_name(FILE * outFile, STRING name)
{
    return abio_puts(outFile, name);
}

/*
 * Write a keyword to the output file.
 */
int
abio_put_keyword(FILE * outFile, AB_OBJECT_TYPE abo_type)
{
    switch (abo_type)
    {
	case AB_TYPE_PROJECT:
	abio_puts(outFile, ":project");
	break;

    case AB_TYPE_MODULE:
	abio_puts(outFile, ":module");
	break;

    case AB_TYPE_ACTION:
	abio_puts(outFile, ":connection");
	break;

    case AB_TYPE_BASE_WINDOW:
	abio_puts(outFile, ":element");
	break;

    case AB_TYPE_BUTTON:
	abio_puts(outFile, ":element");
	break;

    case AB_TYPE_CHOICE:
	abio_puts(outFile, ":element");
	break;

    case AB_TYPE_COMBO_BOX:
	abio_puts(outFile, ":element");
	break;

    case AB_TYPE_FILE_CHOOSER:
	abio_puts(outFile, ":element");
	break;

    case AB_TYPE_MESSAGE:
	abio_puts(outFile, ":element");
	break;

    case AB_TYPE_CONTAINER:
	abio_puts(outFile, ":element");
	break;

    case AB_TYPE_DIALOG:
	abio_puts(outFile, ":element");
	break;

    case AB_TYPE_DRAWING_AREA:
	abio_puts(outFile, ":element");
	break;

    case AB_TYPE_ITEM:
	abio_puts(outFile, ":element");
	break;

    case AB_TYPE_LABEL:
	abio_puts(outFile, ":element");
	break;

    case AB_TYPE_LAYERS:
	abio_puts(outFile, ":element");
	break;

    case AB_TYPE_LIST:
	abio_puts(outFile, ":element");
	break;

    case AB_TYPE_MENU:
	abio_puts(outFile, ":element");
	break;

    case AB_TYPE_SEPARATOR:
	abio_puts(outFile, ":element");
	break;

    case AB_TYPE_SCALE:
	abio_puts(outFile, ":element");
	break;

    case AB_TYPE_SPIN_BOX:
	abio_puts(outFile, ":element");
	break;

    case AB_TYPE_TERM_PANE:
	abio_puts(outFile, ":element");
	break;

    case AB_TYPE_TEXT_FIELD:
	abio_puts(outFile, ":element");
	break;

    case AB_TYPE_TEXT_PANE:
	abio_puts(outFile, ":element");
	break;

    case AB_TYPE_UNKNOWN:
	abio_puts(outFile, ":element");
	break;

    case AB_OBJECT_TYPE_NUM_VALUES:
	abio_puts(outFile, ":element");
	break;


    default:
	break;
    }
    return 0;
}

/*
 * Write a handler to the output file.
 */
int
abio_put_handler(FILE * outFile, STRING s)
{
    STRING              newstr;
    STRING              p;

    if (s && (s[0] == *abio_string_begin_string()) &&
	(s[strlen(s) - 1] == *abio_string_end_string()))
    {
	newstr = (STRING) malloc(strlen(s) + 1);
	strcpy(newstr, s);
	newstr[strlen(s) - 1] = '\0';

	fputc(*abio_string_begin_string(), outFile);
	for (p = newstr + 1; *p; p++)
	{
	    if ((*p == *abio_string_end_string()) || (*p == '\\'))
		fputc('\\', outFile);
	    fputc(*p, outFile);
	}
	fputc(*abio_string_end_string(), outFile);
	free(newstr);
    }
    else
    {
	abio_gil_put_name(outFile, s);
    }
    return 0;
}

/*
 * Write a name list to the output file.
 */
int
abio_gil_put_name(FILE * outFile, STRING s)
{
    return abio_puts(outFile, s ? s : abio_gil_false_string());
}

/*
 * Write a full name to the output file.
 */
int
abio_put_full_name(FILE * outFile, STRING parent, STRING name, STRING item)
{
    abio_puts(outFile, abio_list_begin_string());
    if (parent)
    {
	abio_puts(outFile, parent);
	abio_putc(outFile, ' ');
    }
    if (name)
    {
	abio_puts(outFile, name);
    }
    if (item)
    {
	abio_putc(outFile, ' ');
	abio_put_string(outFile, item);
    }
    abio_puts(outFile, abio_list_end_string());
    abio_putc(outFile, '\n');
    return 0;
}

/*
 * Write a full name (the project form) to the output file.
 */
int
abio_put_proj_full_name(
			FILE * outFile,
			STRING interface,
			STRING parent,
			STRING name,
			STRING item
)
{
    abio_puts(outFile, abio_list_begin_string());

    if (interface)
    {
	abio_puts(outFile, interface);
	abio_putc(outFile, ' ');
    }
    if (parent)
    {
	abio_puts(outFile, parent);
	abio_putc(outFile, ' ');
    }
    if (name)
    {
	abio_puts(outFile, name);
    }
    if (item)
    {
	abio_putc(outFile, ' ');
	abio_put_string(outFile, item);
    }

    abio_puts(outFile, abio_list_end_string());
    abio_putc(outFile, '\n');
    return 0;
}


/*
 * Write a string to the output file.
 *
 * Quotes special characters in C format 
 * (e.g., "\"hi\"", "newline:\n")
 */
int
abio_put_string(FILE * outFile, STRING s)
{
    abio_puts(outFile, abio_string_begin_string());
    abio_put_string_to_file(outFile, s);
    abio_puts(outFile, abio_string_end_string());
    return 0;
}

/*
 * Write a string to the output file.
 */
int
abio_put_string_to_file(FILE * outFile, STRING string)
{
    register STRING     ptr;
    char		strEndChar = *(abio_string_end_string());
    char		curChar = 0;

    if (string != NULL)
    {
	for (ptr = string; (*ptr != 0); ++ptr)
	{
	    curChar = *ptr;
	    if (   (curChar == strEndChar) 
		|| (curChar == '\\') )
	    {
		fputc('\\', outFile);
	        fputc(curChar, outFile);
	    }
	    else if (curChar == '\n')
	    {
		fputs("\\n", outFile);
	    }
	    else
	    {
		fputc(curChar, outFile);
	    }
	}
    }
    return 0;
}


/*
 * Maintain indent state for output file.
 */
int
abio_indent(FILE *out_file)
{
    return abioP_build_indent_string(out_file,
		Indent_char, Indent_chars_per_level, Indent_level + 1);

}

int
abio_outdent(FILE *out_file)
{
    return abioP_build_indent_string(out_file,
		Indent_char, Indent_chars_per_level, Indent_level - 1);
}

/*
 * Get the current indent level of the output file.
 */
int
abio_get_indent(FILE *out_file)
{
    out_file = out_file;	/* avoid warning */
    return Indent_level;
}


int
abio_set_indent(FILE *out_file, int indent)
{
    return abioP_build_indent_string(out_file, 
		Indent_char, Indent_chars_per_level, indent);
}


/*
 * Set indent char
 */
int
abio_set_indent_char(FILE *out_file, int indent_char)
{
    return abioP_build_indent_string(out_file, 
		indent_char, Indent_chars_per_level, Indent_level);
}


int
abio_get_indent_char(FILE *out_file)
{
    out_file = out_file;	/* avoid warning */
    return Indent_char;
}


int
abio_set_indent_chars_per_level(FILE *out_file, int chars_per_level)
{
    return abioP_build_indent_string(out_file, 
		Indent_char, chars_per_level, Indent_level);
}


int
abio_get_indent_chars_per_level(FILE *out_file)
{
    out_file = out_file;	/* avoid warning */
    return Indent_chars_per_level;
}


/*
 * Set the current indent level of the output file.
 */
static int
abioP_build_indent_string(
			FILE	*out_file,
			int	new_indent_char,
			int	new_indent_chars_per_level,
			int	new_indent_level
)
{
    register int        i;
    int			indent_length = 0;
    out_file = out_file;	/* avoid warning */

    if (   (new_indent_char == Indent_char)
	&& (new_indent_chars_per_level == Indent_chars_per_level)
	&& (new_indent_level == Indent_level) )
    {
	return Indent_level;
    }

    Indent_char = new_indent_char;
    Indent_chars_per_level = new_indent_chars_per_level;
    Indent_level = new_indent_level;
    Indent_level = util_max(Indent_level, 0);
    Indent_level = util_min(Indent_level, INDENT_MAX);
    indent_length = Indent_level * Indent_chars_per_level;

    for (i = 0; i < indent_length; ++i)
	Indent[i] = Indent_char;
    Indent[i] = 0;

    return Indent_level;
}


/*
 * Return the string begin string.
 */
STRING
abio_string_begin_string(void)
{
    return "\"";
}

/*
 * Return the string end string.
 */
STRING
abio_string_end_string(void)
{
    return "\"";
}

/*
 * Return the string representation of a string.  Truncates the string if it
 * is too long.
 */
STRING
abio_string_string(STRING s)
{
    char               *p;
    int                 i = 0;

    Buf[i++] = *abio_string_begin_string();
    if (s)
	for (p = s; *p && i < MAXPATHLEN - 4; p++)
	{
	    if (*p == *abio_string_end_string())
		Buf[i++] = '\\';
	    Buf[i++] = *p;
	}
    Buf[i++] = *abio_string_end_string();
    Buf[i] = '\0';
    return Buf;
}

/*
 * Return the boolean true string.
 */
STRING
abio_bil_true_string(void)
{
    return ":true";
}

/*
 * Internal utility functions.
 */

/*
 * Get the next token from the input stream.  It is assumed that we are not
 * at end-of-file.
 */
static              STRING
get_token(FILE * inFile)
{
    int                 ch = 0;
    int                 count = 0;	/* character count */

    while (isspace(ch = fgetc(inFile)))
	;
    if (ch != EOF)
    {
        ungetc(ch, inFile);
    }

    /* vi hack ( */
    while (! (   ((ch = fgetc(inFile)) == EOF)
	      || (isspace(ch))
	      || (ch == ')')
	      || (ch == '"') ))
    {
	if (count < (sizeof(Buf) - 1))
	    Buf[count++] = (char)ch;
    }

    if (ch != EOF)
    {
        ungetc(ch, inFile);
    }
    Buf[count] = '\0';
    return (count > 0? Buf:NULL);
}


/*
 * Return whether a file contains a GIL file.  Returns non-0 version number
 * if true, otherwise FALSE.  Leaves file positioned at the beginning of
 * first comment (should be version number).
 */
static int
gil_version(FILE * fp)
{
    STRING              ascii_version_num;
    STRING              tmp;
    long                int_version_num;
    long                first_char;
    int                 version = 0;
    int                 len = strlen(gilVersionPrefix);

    rewind(fp);
    while (fgets(Buf, sizeof(Buf), fp))
    {

	/*
	 * Ignore lines until a GIL prefix is found.
	 */
	if (strncmp(Buf, gilVersionPrefix, len) == 0)
	{

	    /*
	     * Prefix matched.  Point to the version number and convert it to
	     * an integer.
	     */
	    first_char = ftell(fp);
	    Buf[strlen(Buf) - 1] = '\0';
	    ascii_version_num = Buf + len;
	    int_version_num = strtol(ascii_version_num, &tmp, 10);

	    if (ascii_version_num != tmp)
		version = (int) int_version_num;
	    fseek(fp, first_char, 0);
	    break;
	}
    }
    return version;
}

/*
 * Skip leading comments in a GIL file.
 */
static void
skip_comments(FILE * inFile)
{
    long                pos;

    for (;;)
    {
	pos = ftell(inFile);
	if (!fgets(Buf, sizeof(Buf), inFile) ||
	    Buf[0] != *abio_comment_string())
	    break;
    }
    fseek(inFile, pos, 0);
}

/*
 *  If the file is not NULL, and verbosity >= 3, then util_unbuffer_file
 *  is called on the file. This keeps files flushed during debugging, so
 *  it is easier to see what's going on while the file is being written.
 */
static int
debug_unbuffer_file(FILE *file)
{
    if ((file == NULL) || (!debugging()))
    {
	return 0;
    }

    if (   ((debug_level() >= 1) && ((file == stdin) || (file == stdout)) )
	|| (debug_level() >= 5) )
    {
	util_unbuffer_file(file);
    }
    return 0;
}


/*************************************************************************
 **									**
 **	OUTPUT								**
 **									**
 *************************************************************************/

int
util_set_output_handler(UtilOutputHandler new_output_handler)
{
    output_handler = new_output_handler;
    return 0;
}


int
util_set_err_output_handler(UtilOutputHandler new_err_output_handler)
{
    err_output_handler = new_err_output_handler;
    return 0;
}


int
util_puts(STRING msg)
{
    if (output_handler == NULL)
    {
        if (util_get_program_name() != NULL)
        {
            printf("%s: ", util_get_program_name());
        }
        printf("%s", msg);
    }
    else
    {
	output_handler(msg);
    }
    util_set_help_data(NULL, NULL, NULL);
    return 0;
}


int
util_dputs(int debug_level, STRING msg)
{
    if ((debugging()) && (debug_level() >= debug_level))
    {
        fprintf(stderr, "%s", msg);
    }
    return 0;
}


int
util_puts_err(STRING msg)
{
    if (err_output_handler == NULL)
    {
        if (util_get_program_name() != NULL)
        {
            fprintf(stderr, "%s: ", util_get_program_name());
        }
        fprintf(stderr, "%s", msg);
    }
    else
    {
	/* 
	** Because the message passed in might be from catgets(), 
	** we need to make a local copy before calling the output
	** handler.  Then we'll free it.
	*/
	int	msg_len = strlen(msg)+1;
	STRING	msg_buf = NULL;

	if( (msg_buf = (STRING)util_malloc(msg_len)) == NULL) {
		fprintf(stderr,"%s: could not allocate memory!\n",
			util_get_program_name());
		return 0;
	}
	strcpy(msg_buf,msg);
	err_output_handler(msg_buf);
	free(msg_buf);
    }
    util_set_help_data(NULL, NULL, NULL);
    return 0;
}


int
util_printf(STRING fmt, ...)
{
    va_list             args;
    va_start(args, fmt);

    if (output_handler == NULL)
    {
        if (util_get_program_name() != NULL)
        {
	    printf("%s: ", util_get_program_name());
        }
        vprintf(fmt, args);
    }
    else
    {
	char	msg[8192] = "";
	int	num_chars = 0;

	num_chars = vsprintf(msg, fmt, args);
	util_dassert(1, (num_chars < 8192));
	output_handler(msg);
    }

    va_end(args);
    util_set_help_data(NULL, NULL, NULL);
    return 0;
}


int
util_dprintf(int debug_level, STRING fmt, ...)
{
    va_list	args;
    BOOL	do_debug = debugging();
    int		the_level = debug_level();

    va_start(args, fmt);
    if ((debugging()) && (debug_level() >= debug_level))
    {
	vfprintf(stderr, fmt, args);
    }
    va_end(args);
    return 0;
}


int
util_printf_err(STRING fmt, ...)
{
    va_list	args;
    va_start(args, fmt);

    if (err_output_handler != NULL)
    {
        char	errmsg[8192] = "";
	int	num_chars = 0;

	num_chars = vsprintf(errmsg, fmt, args);
	util_dassert(1, (num_chars < 8192));
	err_output_handler(errmsg);
    }
    else
    {
        if (util_get_program_name() != NULL)
        {
	    fprintf(stderr, "%s: ", util_get_program_name());
        }
        vfprintf(stderr, fmt, args);
    }

    va_end(args);
    util_set_help_data(NULL, NULL, NULL);
    return 0;
}

/*
 * Check if we have read/write permission for the file.
 */
int
abio_access_file(
    STRING 	name,
    BOOL	*read_OK,
    BOOL	*write_OK
)
{
    struct stat 	statbuf;
    uid_t       	userid;
    gid_t       	groupid;
    int			ret = 0;

    *read_OK = FALSE;
    *write_OK = FALSE;

    /*
     * Check if it can be read. If so, check that it really is
     * a bil file.
     */  

    userid = geteuid();         /* Get the effective user id */
    groupid = getegid();        /* Get the effective group id */

    if ((ret = stat(name, &statbuf)) == -1)
    {
	return ERR;
    }

    /* If the user is the file owner then the file has to
     * have the owner read/write permission bits set.
     */
    if (statbuf.st_uid == userid)
    {   
        if ((statbuf.st_mode & S_IWUSR) != 0)
            *write_OK = TRUE;
        if ((statbuf.st_mode & S_IRUSR) != 0)
            *read_OK = TRUE;
    }   
    /* If the user is not the file owner, but is in the group, then
     * the file has to have the group read/write permission bits set.
     */ 
    else if (statbuf.st_gid == groupid)
    {   
        if ((statbuf.st_mode & S_IWGRP) != 0)
            *write_OK = TRUE;
        if ((statbuf.st_mode & S_IRGRP) != 0)
            *read_OK = TRUE;
    }
    /* If the user is not the file owner and is not in the group, then
     * the file has to have the other read/write permission bits set.
     */
    else
    {  
        if ((statbuf.st_mode & S_IWOTH) != 0)
            *write_OK = TRUE;
        if ((statbuf.st_mode & S_IROTH) != 0)
            *read_OK = TRUE;
    }

    return 0;
}

/*
 * Return the module keyword.
 */
STRING
abio_module_string(void)
{
    return ":module";
}

/*
 * Return the project keyword.
 */
STRING
abio_project_string(void)
{
    return ":project";
}

/*
 * Write an escaped string to a buffer.
 * Assumes space has been allocated for outBuf.
 * size is the size allocated for the buffer.
 */
int
abio_put_string_to_buffer(STRING string, STRING outBuf, int size)
{
#define INC             32
    register STRING     ptr;
    char		strEndChar = *(abio_string_end_string());
    char		curChar = 0;
    int			i = 0;

    *outBuf = 0;
    if (string != NULL)
    {
	for (ptr = string, i = 0; (*ptr != 0); ++ptr, ++i)
	{
	    /* If we've run out of space, allocate more.
	     */
	    if (i == (size - 1))
	    {
		outBuf = (STRING) realloc(outBuf, size += INC);
	    }

	    curChar = *ptr;

	    /* Escape backslashes, double-quotes, 
	     * newlines, and tabs. 
	     */
	    if (   (curChar == strEndChar) 
		|| (curChar == '\\') )
	    {
		outBuf[i] = '\\'; i++;
		outBuf[i] = curChar;
	    }
	    else if (curChar == '\n')
	    {
		outBuf[i] = '\\'; i++;
		outBuf[i] = 'n';
	    }
	    else if (curChar == '\t')
	    {
		outBuf[i] = '\\'; i++;
		outBuf[i] = 't';
	    }
	    else
	    {
		outBuf[i] = curChar;
	    }
	}
	outBuf[i] = '\0';
    }
    return 0;
}

/* This function is called to set the help text that will
 * be displayed for a message that is posted via a call
 * to any of the print utilities, such as util_printf_err,
 * util_puts_err, or util_print_error.
 */
void
util_set_help_data(
    STRING	msg,
    STRING	vol,
    STRING	locID
)
{
    if (help_text)
    {
	util_free(help_text); 
	help_text = NULL;
    }
    if (msg)
    {
	help_text = (STRING)util_malloc(strlen(msg)+1);
	strcpy(help_text, msg);
    }

    if (help_volume)
    {
	util_free(help_volume); 
	help_volume = NULL;
    }
    if (vol)
    {
	help_volume = (STRING)util_malloc(strlen(vol)+1);
	strcpy(help_volume, vol);
    }

    if (help_locID)
    {
	util_free(help_locID); 
  	help_locID = NULL;
    }
    if (locID)
    {
	help_locID = (STRING)util_malloc(strlen(locID)+1);
	strcpy(help_locID, locID);
    } 
}

/* This function is called by (err_)output_handler() in
 * dtbuilder.c. It retrieves the help text for the message
 * that is going to be displayed. 
 */
int
util_get_help_data(
    STRING	*msg,
    STRING	*vol,
    STRING	*locID
)
{
    if (msg)
	*msg = help_text;
    if (vol)
	*vol = help_volume;
    if (locID)
	*locID = help_locID;
    return 0;
}
