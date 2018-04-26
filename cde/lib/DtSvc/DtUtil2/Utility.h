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
 * File:         Utility.h $XConsortium: Utility.h /main/4 1995/10/26 15:31:55 rswiston $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#ifndef _Dt_Utility_h
#define _Dt_Utility_h

/******************************************************************************
 *
 * _DtVectorizeInPlace() takes a string which is made up a group of
 * components, separated by a common separator character, and breaks
 * the string up into the separate components.  To reduce the amount of
 * memory used (and to reduce memory fragmentation), the string is simply
 * searched for each occurrence of the separator, and the separator is then
 * replaced by a NULL character.  Pointers to the individual components are
 * returned as a NULL-terminated array of pointers.
 *
 * The passed-in string should be malloc'ed space, since the string will
 * eventually be freed when the application frees the returned array.  If
 * you don't want the original string modified, then a copy should be made,
 * before calling this function.
 *
 * The application is responsible for freeing up this memory, and should do
 * so by calling _DtFreeStringVector().
 *
 * Parameters:
 *
 *      string           A NULL-terminated string, which is to be vectorized.
 *
 *      separator        The character which separates the components within
 *                       the string.
 *
 *****************************************************************************/
extern char **  _DtVectorizeInPlace( char * string, 
                                     char separator );

/******************************************************************************
 *
 * _DtFreeStringVector will free up the vectorized string array returned by
 * a call to _DtVectorizeInPlace().  Both the array used to return the
 * vectorized strings, and the original string itself will be freed up.
 *
 * Parameters:
 *
 *    stringVector   The array to be freed; originally obtained by a call
 *                   to _DtVectorizeInPlace().
 *
 *****************************************************************************/
extern void     _DtFreeStringVector( char ** stringVector );


/*****************************************************************************
 * DtCmd String Utility routines.
 *
 *****************************************************************************
 *
 * _DtCmdStringToArrayOfStrings - takes a string and an array of pointers
 *   to strings and breaks the string into whitespace separated words.
 *
 *   A "word" is a sequence of characters that has no whitespace with
 *   the following exception:
 *
 *     -  A word may contain contain whitespace if it is delimited
 *        by a pair of matching single or double qotes.
 *
 *   "Whitespace" is a tab or blank character.
 *
 *
 * NOTES: 
 *
 *   -  The space for the "words" is malloc'd and must be free'd by
 *   the caller.
 *   -	_DtCmdFreeStringVector() should be used to free up string vectors
 *   created by _DtCmdStringToArrayOfStrings().
 *
 *   -  "theArray" is NULL terminated.
 *
 * PARAMETERS:
 *
 *   char theString[];		- The string to parse.
 *
 *   char *theArray[];		- MODIFIED: gets filled with pointers to
 *				  the words that are parsed.
 *
 *****************************************************************************/


/******************************************************************************
 *
 * _DtCmdFreeStringVector - takes an array of pointers to strings and
 *   frees the malloc'd space for the strings.
 *
 *   This does NOT free the string vector itself; It assumes that
 *   stringv is a static i.e. char *stringv[N].
 *
 * PARAMETERS:
 *
 *   char **stringv;    - MODIFIED:  Each string in the array is freed.
 *
 *****************************************************************************/

extern void _DtCmdStringToArrayOfStrings( 
                        char theString[],
                        char *theArray[]) ;
extern void _DtCmdFreeStringVector( 
                        char **stringv) ;

#endif /* _Dt_Utility_h */

/* DON'T ADD ANYTHING AFTER THIS #endif */
