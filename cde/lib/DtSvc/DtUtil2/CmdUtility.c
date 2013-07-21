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
 * File:         CmdUtility.c $XConsortium: CmdUtility.c /main/4 1995/10/26 15:18:41 rswiston $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>

#ifdef __apollo
#include "/sys5/usr/include/unistd.h"
#else
#include <unistd.h>
#endif
#include <limits.h>
#include <sys/stat.h>

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Dt/Utility.h>
#include <Dt/DtNlUtils.h>


#define _SINGLE  "\'"
#define _DOUBLE  "\""


/********    Static Function Declarations    ********/

static void SkipWhiteSpace( 
                        String string,
                        int *position) ;
static void FillWord( 
                        char *string,
                        char *word,
                        int *position) ;
static void GetWordWithQuotes( 
                        String string,
                        String word,
                        int *position) ;

/********    End Static Function Declarations    ********/


/*****************************************************************************
 *
 * SkipWhiteSpace - takes a string and in index ("position") into the 
 *   string and advances "position" until a non-whitespace character is 
 *   encountered.
 *
 *   A "whitespace" character is defined by "isspace". 
 *
 * PARAMETERS:
 *
 *   String string;	- The string to search.
 *
 *   int *position;	- MODIFIED: Set to the location of the first 
 *			  non-whitespace character.
 *
 *****************************************************************************/

static void 
SkipWhiteSpace(
        String string,
        int *position )
{
   string += (*position);

   while (
#ifdef NLS16
      (!is_multibyte || (mblen (string, MB_CUR_MAX) == 1)) &&
#endif
	 isspace ((u_char)*string)) {
	    string++;
	    (*position)++;
	 }
}

/*****************************************************************************
 *
 * FillWord - parses "string" for a complete argument and puts the
 * result in "word".
 *
 * The algorithm was derived by empirical observation and checking the
 * (poorly written) Bourne Shell tutorial.  A BNF for the shell meta
 * characters ", ', and \ was not availble.
 *
 * The algorithm - until the end of the word is found:
 *
 *  For each character "c":
 *     If c = \, remove the \ and pass on the next char. 
 *
 *     If c = ' or ", must save this in "qchar" and loop until the 
 *     ending quote is found: 
 *        c = the next char
 *        If c = qchar, found the ending quote, exit this loop
 *        If c = \
 *           If qchar = double quote and c2 = next char
 *              if c2 = \, ", $, or `, remove c and pass on c2
 *              otherwise, pass on both c and c2
 *           If qchar = single quote, and c2 = next char:
 *              if c2 = ', pass on c, remove the ' and exit this loop
 *                 (the ' cannot be escaped)
 *              otherwise, pass on both c and c2
 *        Othewise, pass on c
 *
 *     If c = white space, found the end of the word, return
 *     Otherwise, pass on the char
 *
 *
 * PARAMETERS:
 *
 *   char *string;	- The string to search.
 *
 *   char *word;	- MODIFIED: Points to the beginning of the word.
 *
 *   int *position	- MODIFIED: Starts at the beginning of the string 
 *			  and gets advanced past "word".
 *
 *****************************************************************************/

static void 
FillWord(
        char *string,
        char *word,
        int *position )
{
   char *qchar;
   int len, i;
   Boolean found = False;
   Boolean done  = False;
   int j = 0;
   char *pbeg = string;

   while ((*string != '\0') && (!found)) {
      /* 
       * Check for multibyte chars. The assumption here is that if
       * is_multibyte is true and "string" points to a multi-byte char,
       * then that entire char should be copied to "word".
       */
#ifdef NLS16
      if (is_multibyte && ((len = mblen (string, MB_CUR_MAX)) > 1))
	 for (i=0; i < len; i++, j++, string++)
	    word[j] = *string;
      else 
#endif
       {
	 switch (*string) {
	    case '\\':
	       /* Remove the slash and add the following character. */
	       string++;
#ifdef NLS16
	       if (is_multibyte && ((len = mblen (string, MB_CUR_MAX)) > 1))
		  for (i=0; i < len; i++, j++, string++)
		     word[j] = *string;
	       else 
#endif
		  word[j++] = *(string)++;
	       break;
	    case '\'':
	    case '\"':
	       qchar = _DOUBLE;
	       if (*string == '\'') qchar = _SINGLE;
	       string++;
	       /* Search for the ending quote. */ 
	       done = False;
	       while ((!done) && (*string != '\0')) {
#ifdef NLS16
		  if (is_multibyte && ((len = mblen (string, MB_CUR_MAX)) > 1))
		     for (i=0; i < len; i++, j++, string++)
			word[j] = *string;
		  else 
#endif
                  {
		     if (*string == *qchar) {
			done = True;
			string++;
			break;
		     }
		     if (*string == '\\') {
			/* Must follow the rules of the single or double
			 * quote - which ever "qchar" points to.
			 */
			if (!strcmp (qchar, _DOUBLE)) {
			   if ((DtStrcspn (string+1, "\"\\$`")) == 0) {
			      /* Skip past the '\', but fill in the 
			       * following character. 
			       */
			      string++;
			   }
			   else
			      /* Want to pass on both the '\' and the 
			       * following char. 
			       */
			      word[j++] = *(string)++;
			   /* The '\' is skipped.  Fill in the next char. */
#ifdef NLS16
			   if (is_multibyte && ((len = mblen (string, MB_CUR_MAX)) > 1))
			      for (i=0; i < len; i++, j++, string++)
				 word[j] = *string;
			   else
#endif
			      word[j++] = *(string)++;
			   /* The \ and following char are now skipped. */
			}
			else if (!strcmp (qchar, _SINGLE)) {
			   /* Must be working on a _SINGLE quoted word. */
			   if ((DtStrcspn (string+1, "\'")) == 0) {
			      /* 
			       * Have \', which passes on the \, skips 
			       * the single quote and ends the word. An 
			       * assumption here is that the char following 
			       * the '\' was a single byte single quote 
			       * and there is no need for checking multi-byte. 
			       */
			      word[j++] = *(string)++;
			      /* Now skip the quote. */
			      string++;
			      done = True;
			      break;
			   }
			   else {
			      /* 
			       * Need to pass on both chars.  Pass on the 
			       * first char here.  
			       */
			      word[j++] = *(string)++;

			      /* 
			       * The '\' is skipped if present.  Fill in the
			       * next char.
			       */
#ifdef NLS16
			      if (is_multibyte && ((len = mblen (string, MB_CUR_MAX)) > 1))
				 for (i=0; i < len; i++, j++, string++)
				    word[j] = *string;
			      else
#endif
				 /* Pass on what ever char is there. */
				 word[j++] = *(string)++;
			   }
			}
		     }
		     else
			/* This char was not escaped, just add it. */
			word[j++] = *(string)++;
                  }
	       }
	       break;
	    case ' ':
	    case '\t':
	       /* Found the end of the word. */
	       found = True;
	       string++;
	       break;
	    default: {
	       word[j++] = *(string)++;
	    }
	 }
      }
   }
   word [j] = '\0';
   *position = *position + (string - pbeg);
}

/*****************************************************************************
 *
 * GetWordWithQuotes - takes the strings "string" and "word" and an index
 * into "string" and fills "word" with one word from "string".
 *
 * A word is defined in the function "FillWord".
 *
 * Note that if an ending quote is not found, "position" will be advanced to
 * the end of the string.  
 *
 * PARAMETERS:
 *
 *   String string;	- String containing the word to be extracted.
 *
 *   String word;	- MODIFIED - contains the next word in "string".
 *
 *   int *position;	- MODIFIED - starts at beginning of word, ends 
 *			  at end of word.
 *
 *****************************************************************************/

static void 
GetWordWithQuotes(
        String string,
        String word,
        int *position )
{
   int len = strlen(string);
   SkipWhiteSpace (string, position);

   if ((*position) >= len) {
      word[0] = '\0';
      return;
   }

   string += (*position);

   FillWord (string, word, position);
}

/*****************************************************************************
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

void 
_DtCmdStringToArrayOfStrings(
        char theString[],
        char *theArray[] )
{
   int len, i, position;
   char *tmp;
   tmp = (char *) XtMalloc (1 + strlen (theString));

   len=strlen(theString);
   for (position=0, i=0; (position <= len) && 
        (theString[position] != '\0'); ) {
      (void) strcpy (tmp, "");
      GetWordWithQuotes (theString, tmp, &position);
      /* Check word to see if it contains only trailing blanks. */
      if (tmp[0] == '\0') 
      {
	 if (position < len)
	 {
	    /*
	     * This parameter is empty, such as "" or '' but we are
	     * not at the end of the line.  Consequently, put an
	     * empty string in "theArray".
	     */
	    theArray[i] = XtNewString ("");
	 }
	 else
	 {
	    /*
	     * Must be at the end of the line.
	     */
	    theArray[i] = (char *) NULL;
	    break;
	 }
      }
      else
	 theArray[i] = XtNewString (tmp);
      i++;
   }

   /* Null terminate the array of string pointers. */
   theArray[i]=NULL;

   XtFree ((char *) tmp);
}

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
 *   char **stringv;	- MODIFIED:  Each string in the array is freed.
 *
 *****************************************************************************/

void 
_DtCmdFreeStringVector(
        char **stringv )
{
   char **pch;

   for (pch = stringv; *pch != NULL; pch++) {
      XtFree (*pch);
      *pch = NULL;
   }
}


