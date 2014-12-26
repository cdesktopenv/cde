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
/* $TOG: StringFuncs.c /main/15 1998/03/19 09:49:44 mgreess $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:     StringFuncs.c
 **
 **   Project:  Cde DtHelp
 **
 **   Description: Semi private string functions - can be platform dependent.
 **
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
/*
 * system includes
 */
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <locale.h>  /* getlocale(), LOCALE_STATUS, LC_xxx */

#if defined(_AIX) || defined(USL) || defined(__uxp__) || defined(CSRG_BASED)
#include <ctype.h>
#endif

#include "CvStringI.h"        /* for string functions used by Canvas Engine */
#include "StringFuncsI.h"     /* for _CEStrcollProc */

#if !defined(linux)
# include <iconv.h>
#else
# define iconv_t		int
# define iconv_open(a, b) 	((iconv_t) -1)
# define iconv(a, b, c, d, e)	((size_t) 0)
# define iconv_close(a)		(0)
#endif

#ifdef ICONV_INBUF_CONST
# define ICONV_INBUF_TYPE	const char **
#else
# define ICONV_INBUF_TYPE	char **
#endif

/**** Types ****/
/* Private structure of the IconvContext.  Public
   structure doesn't reveal implementation. */
typedef struct _DtHelpCeIconvContextRec
{
    iconv_t         cd;
    int             tableSize;
    unsigned char * table;        /* translation table */
    char *          fromCodeset;
    char *          toCodeset;
} _DtHelpCeIconvContextRec;


#ifndef True
#define True    1
#endif
#ifndef TRUE
#define TRUE    1
#endif
#ifndef False
#define False   0
#endif
#ifndef FALSE
#define FALSE   0
#endif

/******************************************************************************
 *
 * Private variables and defines.
 *
 *****************************************************************************/
#define EOS '\0'

/******************************************************************************
 *
 * Semi Public Functions
 *
 *****************************************************************************/
/******************************************************************************
 * Function: _DtHelpCeStrcspn (const char *s1, const char *s2, max_len, &ret_len)
 *
 *	Returns in 'ret_len' the length of the initial segment of string
 *	s1 which consists entirely of characters not found in string s2.
 *
 * Returns:
 *      -1  If found an invalid character.
 *       0  If found a character in string s2
 *       1  If found the null byte character.
 *****************************************************************************/
int
_DtHelpCeStrcspn (
    const char    *s1,
    const char    *s2,
    int            max_len,
    int           *ret_len )
{
    int    len;
    int    len2;
    const char  *p1;
    const char  *p2;

    if (!s1)
      {
	*ret_len = 0;
	return 0;
      }

    if (*s1 == '\0')
      {
	*ret_len = 0;
	return 1;
      }

    if (!s2 || *s2 == '\0')
      {
	*ret_len = strlen (s1);
	return 1;
      }

    if (max_len == 1)
      {
	/*
	 * no need to go through any hassle, just use the 3C function
	 */
	*ret_len = strcspn (s1, s2);
	if (s1[*ret_len] == '\0')
	    return 1;
	return 0;
      }

    p1 = s1;
    while (*p1 != '\0')
      {
	len = mblen (p1, max_len);
	if (len == -1)
	  {
	    /*
	     * we found an invalid character
	     * return the length found so far and the flag.
	     */
	    *ret_len = p1 - s1;
	    return -1;
	  }

	p2 = s2;
	while (*p2 != '\0')
	  {
	    len2 = mblen (p2, max_len);
	    if (len2 == -1)
		len2 = 1;

	    if (len2 == len && strncmp (p1, p2, len) == 0)
	      {
		*ret_len = p1 - s1;
		return 0;
	      }
	    p2 += len2;
	  }
	p1 += len;
      }

    *ret_len = p1 - s1;
    return 1;
}

/******************************************************************************
 * Function: _DtHelpCeStrspn (const char *s1, const char *s2, max_len, &ret_len)
 *
 *	Returns in 'ret_len' the length of the initial segment of string
 *	s1 which consists entirely of characters found in string s2.
 *
 * Returns:
 *      -1  If found an invalid character.
 *       0  If found a character not in string s2
 *       1  If found the null byte character.
 *****************************************************************************/
int
_DtHelpCeStrspn (
    char    *s1,
    char    *s2,
    int      max_len,
    int     *ret_len )
{
    int    len;
    int    len2;
    char  *p1;
    char  *p2;
    char   found;

    if (!s1 || !s2 || *s2 == '\0')
      {
	*ret_len = 0;
	return 0;
      }
    if (*s1 == '\0')
      {
	*ret_len = 0;
	return 1;
      }

    if (max_len == 1)
      {
	/*
	 * no need to go through any hassle, just use the 3C function
	 */
	*ret_len = strspn (s1, s2);
	if (s1[*ret_len] == '\0')
	    return 1;
	return 0;
      }

    p1 = s1;
    found = True;
    while (*p1 != '\0' && found)
      {
	len = mblen (p1, max_len);
	if (len == -1)
	  {
	    /*
	     * we found an invalid character
	     * return the length found so far and the flag.
	     */
	    *ret_len = p1 - s1;
	    return -1;
	  }

	p2 = s2;
	found = False;
	while (*p2 != '\0' && !found)
	  {
	    len2 = mblen (p2, max_len);
	    if (len2 == -1)
		len2 = 1;

	    if (len2 == len && strncmp (p1, p2, len) == 0)
		found = True;
	    p2 += len2;
	  }

	if (found)
	    p1 += len;
      }

    *ret_len = p1 - s1;

    if (found)
        return 1;

    return 0;
}

/******************************************************************************
 * Function: _DtHelpCeStrchr (char *s1, char *value, max_len, ret_ptr)
 *
 *	Returns in 'ret_ptr' the address of the first occurence of 'value'
 *	in string s1.
 *
 * Returns:
 *      -1  If found an invalid character.
 *       0  If found value in string s2
 *       1  If found the null byte character without finding 'value'.
 *	    'ret_ptr' will also be null in this case.
 *****************************************************************************/
int
_DtHelpCeStrchr (
    const char    *s1,
    const char    *value,
    int            max_len,
    char         **ret_ptr )
{
    int      len;
    int      valLen;
    const char *p1;

    *ret_ptr = NULL;

    if (s1 == NULL || *s1 == '\0')
	return 1;

    if (max_len == 1)
      {
	*ret_ptr = strchr (s1, ((int)*value));
	if (*ret_ptr)
	    return 0;
	return 1;
      }

    p1 = s1;
    valLen = mblen(value, max_len);
    if (valLen < 1)
	return -1;

    while (*p1 != '\0')
      {
	len = mblen (p1, max_len);
	if (len == -1)
	    return -1;
	if (len == valLen && strncmp(p1, value, len) == 0)
	  {
	    *ret_ptr = (char *)p1;
	    return 0;
	  }
	p1 += len;
      }

    return 1;
}

/******************************************************************************
 * Function: _DtHelpCeStrrchr (char *s1, char *value, max_len, ret_ptr)
 *
 *	Returns in 'ret_ptr' the address of the last occurence of 'value'
 *	in string s1.
 *
 * Returns:
 *      -1  If found an invalid character.
 *       0  If found value in string s2
 *       1  If found the null byte character without finding 'value'.
 *	    'ret_ptr' will also be null in this case.
 *****************************************************************************/
int
_DtHelpCeStrrchr (
    const char    *s1,
    const char    *value,
    int            max_len,
    char         **ret_ptr )
{
    int      len;
    int      valLen;
    const char *p1;

    *ret_ptr = NULL;

    if (s1 == NULL || *s1 == '\0')
	return 1;

    if (max_len == 1)
      {
	*ret_ptr = strrchr (s1, ((int)*value));
	if (*ret_ptr != NULL)
	    return 0;
	return 1;
      }

    p1 = s1;
    valLen = mblen(value, max_len);
    if (valLen < 1)
	return -1;

    while (*p1 != '\0')
      {
	len = mblen (p1, max_len);
	if (len == -1)
	    return -1;
	if (len == valLen && strncmp(p1, value, len) == 0)
	    *ret_ptr = (char *)p1;

	p1 += len;
      }

    if (*ret_ptr != NULL)
	return 0;

    return 1;
}

/******************************************************************************
 * Function: _DtHelpCeCountChars (char *s1, max_len, ret_len)
 *
 *	Returns in 'ret_len' the number of characters (not bytes)
 *	in string s1.
 *
 * Returns:
 *      -1  If found an invalid character. 'ret_len' contains the
 *	    number of 'good' characters found.
 *       0  If successful.
 *****************************************************************************/
int
_DtHelpCeCountChars (
    char    *s1,
    int      max_len,
    int     *ret_len )
{
    int    len;

    *ret_len = 0;

    if (s1)
      {
        if (max_len == 1)
	    *ret_len = strlen (s1);
	else
	  {
	    while (*s1)
	      {
		len = mblen (s1, max_len);
		if (len == -1)
		    return -1;
		*ret_len = *ret_len + 1;
		s1 += len;
	      }
	  }
      }
    return 0;
}

/*****************************************************************************
 * Function: _DtHelpCeUpperCase
 *
 * Parameters:  string          Specifies the string to change into
 *                              upper case.
 *
 * Return Value: Nothing
 *
 * Purpose:     To change all lower case characters into upper case.
 *
 *****************************************************************************/
void
_DtHelpCeUpperCase ( char *string )
{
    int   len;

    if (string)
      {
	while (*string != '\0')
	  {
	    len = mblen (string, MB_CUR_MAX);
	    if (len == 1)
	      {
		*string = (unsigned char) toupper (*(unsigned char *)string);
		string++;
	      }
	    else if (len > 0)
		string += len;
	    else
		return;
	  }
      }
}

/************************************************************************
 * Function: _DtHelpCeStrHashToKey
 *
 * Parameters:  C-format string to hash
 *
 * Purpose:
 *   Does a very simple hash operation on the string and returns the value
 *
 * Returns:  hash value
 *
 ************************************************************************/
int _DtHelpCeStrHashToKey(
        const char * str)
{
    register char c;
    register const char * tstr;
    register int key = 0;

    if (!str) return 0;                  /* RETURN */

    for (tstr = str; (c = *tstr++); )
        key = (key << 1) + c;

    return key;
}



/******************************************************************************
 * Function:    _DtHelpCeGetStrcollProc
 *
 * Parameters:  none
 *
 * Returns:	Ptr to the proper collation function to use
 *              If the codeset of the locale is "C", then it is 
 *              strcasecmp(). If it's not, then it is strcoll().
 *
 * Purpose:	When the codeset of the locale "C", strcoll() 
 *              performs collation identical to strcmp(), which is 
 *              strictly bitwise.
 *
 *              To get case-insensitive collation, you need to use
 *              strcasecmp() instead.  If codeset != "C", then 
 *              strcoll() collates according to the language
 *              setting.
 * 
 * Warning:     This code is not multi-thread safe.  The multi-thread
 *              safe setlocale must be used instead to make it so.
 *
 *****************************************************************************/
   _CEStrcollProc _DtHelpCeGetStrcollProc(void)
{
   int Clang = 0;
#if defined(__hpux)
   struct locale_data * li;
#else
   char * locale;
#endif

   extern int strcoll(const char *,const char *);
#if defined(_AIX) || defined(USL) || defined(__uxp__)
   extern int _DtHelpCeStrCaseCmp(const char *,const char *);
#else
   extern int strcasecmp(const char *,const char *);
#endif

#define C_LANG  "C"

   /* if locale is C, use the explicit case insensitive compare */
#if defined(__hpux)
   li = getlocale(LOCALE_STATUS);
   if ( NULL == li->LC_COLLATE_D || strcmp(C_LANG,li->LC_COLLATE_D) == 0 )
     Clang = 1;
#else
   locale = setlocale(LC_COLLATE,NULL); /* put locale in buf */
   if (strcmp(locale,C_LANG) == 0)
     Clang = 1;
#endif

   if (Clang)
#if defined(_AIX) || defined(USL) || defined(__uxp__)
     return _DtHelpCeStrCaseCmp;
#else
     return strcasecmp; 
#endif
   return strcoll;
}

/*****************************************************************************
 * Function:        String _DtHelpCeStripSpaces (string)
 *
 *
 * Parameters:    String to process
 *
 * Return Value:  Processed string
 *
 * Purpose:       Strip all leading and trailing spaces.
 *                Processing is in place
 *
 *****************************************************************************/
char * _DtHelpCeStripSpaces (
    char * string)
{
   int i;
   int multiLen = MB_CUR_MAX;
   char * space;

   if (string == NULL)
      return (string);

   /* Strip off leading spaces first */
   i = 0;
   while ((multiLen == 1 || (mblen(string + i, MB_LEN_MAX) == 1)) &&
          isspace((unsigned char) string[i]))
   {
      i++;
   }
   /* Copy over the leading spaces */
   strcpy(string, string + i);

   /* Drop out, if the string is now empty */
   if ((i = strlen(string) - 1) < 0)
      return(string);

   /* Strip off trailing spaces */
   if (multiLen == 1)
   {
      /* No multibyte; simply work back through the string */
      while ((i >= 0) && (isspace((unsigned char) string[i])))
         i--;
      string[i + 1] = '\0';
   }
   else
   {
      /* Work forward, looking for a trailing space of spaces */
      int len;

      i = 0;
      space = NULL;

      while (string[i])
      {
         if (    ((len =mblen(string + i, MB_LEN_MAX)) == 1) 
              && isspace((unsigned char) string[i]))
         {
            /* Found a space */
            if (space == NULL)
               space = string + i;
         }
         else if (space)
            space = NULL;

         /* if there is an invalid character, treat as a valid one-byte */
         if (len == -1)
           len = 1;
         i += len;

      }

      if (space)
         *space = '\0';
   }

   return (string);
}


/*****************************************************************************
 * Function:        void _DtHelpCeCompressSpace (string)
 *
 * Parameters:      string to process
 *
 * Return Value:    processed string
 *
 * Purpose:           This function strips all leading and trailing spaces
 *                    from the string; it also compresses any intervening
 *                    spaces into a single space.  This is useful when
 *                    comparing name strings.  For instance, the string:
 *                    "    First    Middle    Last   "
 *
 *                    would compress to:
 *
 *                    "First Middle Last"
 *
 *                     Processing is in place.
 *
 *****************************************************************************/
void _DtHelpCeCompressSpace (
    char * string)

{
   char * ptr;

   /* Strip leading and trailing spaces */
   _DtHelpCeStripSpaces(string);

   /* Compress intervening spaces */
   _DtHelpCeStrchr(string, " ", 1, &ptr);
   while (ptr)
   {
      /* Skip over the one space we plan to keep */
      ptr++;
      _DtHelpCeStripSpaces(ptr);
      _DtHelpCeStrchr(ptr, " ", 1, &ptr);
   }
}

/*****************************************************************************
 * Function:        void _DtHelpCeIconvStr1Step (string)
 *
 * Parameters:      
 *    fromCode;      codeset name
 *    fromStr;       string to convert
 *    toCode;        codeset name
 *    ret_toStr;     converted str; this string is malloc'd by this routine
 *                   and the CALLER MUST FREE it when no longer needed.
 *    dflt1;         1-byte default char
 *    dflt2;         2-byte default char
 *
 * Return Value:    
 *                  0: ok
 *                 -1: missing (NULL) argument
 *                 -2: no translation available from fromCode to toCode
 *                 -3: couldn't allocate memory 
 *                 -4: couldn't start conversion 
 *                 -5: incomplete multibyte char
 *                 -6: invalid char found
 *
 * Purpose:
 *                  Converts string from fromCode to toCode using iconv(3)
 *                  It expects the codeset strings to be iconv(3) compatible.
 *		    Generally, compatible strings can be retrieved using
 *		    the _DtHelpCeXlateStdToOpLocale() call.
 *
 * Comments:
 *                  iconv(3) is standardized in XPG4, which is just starting
 *                  to be supported.  Below are several different implementations 
 *                  of _DtHelpCeIconvStr, each using what is available on different 
 *                  platforms.  If no code is #ifdef'd, the XPG4 code defaults.
 *****************************************************************************/

int _DtHelpCeIconvStr1Step(
      const char * fromCode,   /* codeset name */
      const char * fromStr,    /* string to convert */
      const char * toCode,     /* codeset name */
      char * *     ret_toStr,  /* converted str */
      int          dflt1,      /* 1-byte default char */
      int          dflt2)      /* 2-byte default char */

{
   _DtHelpCeIconvContextRec * iconvContext;
   int ret;

   ret = _DtHelpCeIconvOpen(&iconvContext,fromCode,toCode,dflt1,dflt2);
   if (0 == ret) 
      ret = _DtHelpCeIconvStr(iconvContext,fromStr, ret_toStr,NULL,NULL,0);
   _DtHelpCeIconvClose(&iconvContext);
   return ret;
}


/*****************************************************************************
 * Function:        void _DtHelpCeIconvOpen (string)
 *
 * Parameters:      
 *    iconvContext   context 
 *    fromStr;       string to convert
 *    ret_toStr;     converted str; this string is malloc'd by this routine
 *                   and the CALLER MUST FREE it when no longer needed.
 *    dflt1;         1-byte default char
 *    dflt2;         2-byte default char
 *
 * Return Value:    
 *                  0: ok
 *                 -1: missing (NULL) argument
 *                 -2: no translation available from fromCode to toCode
 *                 -3: couldn't allocate memory 
 *                 -4: couldn't start conversion 
 *                 -5: incomplete multibyte char
 *                 -6: invalid char found
 *
 * Purpose:
 *              Opens an iconv table/algorithm to convert string from 
 *              fromCode to toCode using iconv(3)
 *              It expects the codeset strings to be iconv(3) compatible.
 *		Generally, compatible strings can be retrieved using
 *		the _DtHelpCeXlateStdToOpLocale() call.
 *
 * Comments:
 *              iconv(3) is standardized in XPG4, which is just starting
 *              to be supported.  Below are several different implementations 
 *              of _DtHelpCeIconvStr, each using what is available on different 
 *              platforms.  If no code is #ifdef'd, the XPG4 code defaults.
 *****************************************************************************/
int _DtHelpCeIconvOpen(
      _DtHelpCeIconvContext * ret_iconvContext,  /* iconv */
      const char * fromCode,   /* codeset name */
      const char * toCode,     /* codeset name */
      int          dflt1,      /* 1-byte default char */
      int          dflt2)      /* 2-byte default char */

{  /* XPG4-compliant code (args dflt1 & dflt2 are ignored in this code) */
   int              err;          /* error code of function */
   _DtHelpCeIconvContextRec * ic;

#define BAD (-1)
#define MEM_INC 20

   err = 0;  /* ok */

   /* check args */
   if (!ret_iconvContext || !fromCode || !toCode )
      return -1;                         /* RETURN error */

   /* init state */
   ic = *ret_iconvContext = calloc(1,sizeof(_DtHelpCeIconvContextRec));
   if (NULL == ic) return -3;   /* RETURN error */

   if ( strcmp(fromCode,toCode) == 0 )
   {
      ic->cd = (iconv_t) BAD;           /* BAD means use strdup() */
      goto success;
   }

   ic->cd = iconv_open(toCode,fromCode);
   if ( ic->cd == (iconv_t) BAD ) 
   {
      err = -4;                         /* error */
      goto success;
   }

success:
   ic->fromCodeset = strdup(fromCode);
   ic->toCodeset = strdup(toCode);
   return err;                             /* RETURN status */
}

/*****************************************************************************
 * Function:        void _DtHelpCeIconvStr (string)
 *
 * Parameters:      
 *    iconvContext   context for the conversion
 *    fromStr;       string to convert
 *    ret_toStr;     converted str; this string is malloc'd by this routine
 *                   and the CALLER MUST FREE it when no longer needed.
 *    toStrBuf;      for efficiency, can pass in a buf 
 *    toStrBufLen;   length of buf
 *
 * Return Value:    
 *                  0: ok
 *                 -1: missing (NULL) argument
 *                 -2: no translation available from fromCode to toCode
 *                 -3: couldn't allocate memory 
 *                 -4: couldn't start conversion 
 *                 -5: incomplete multibyte char
 *                 -6: invalid char found
 *
 * Purpose:
 *              Converts string from fromCode to toCode using iconv(3)
 *              If toStrBuf is NULL, memory for the converted string
 *                will be malloced as needed.
 *              If toStrBuf is not NULL, the conversion will use up 
 *                to toStrBufLen bytes of the buffer and then realloc
 *                more memory if needed.
 *              If toStrBuf is not NULL, the size of the buf is
 *                returned in ret_toStrLen; otherwise, the value is
 *                not set.
 *              ret_toStr receives the pointer to the buf, which may
 *                be different from toStrBuf if memory was allocated
 *                or NULL if an error occurred.  If toStrBuf is
 *                not NULL and memory must be allocated, a realloc()
 *                call is used, possibly invalidating the toStrBuf ptr.
 *              ret_toStrLen receives the length of the buffer if
 *                toStrBuf is not NULL.  If it is NULL, the length
 *                is not returned.
 * Comments:
 *              iconv(3) is standardized in XPG4, which is just starting
 *              to be supported.  Below are several different implementations 
 *              of _DtHelpCeIconvStr, each using what is available on different 
 *              platforms.  If no code is #ifdef'd, the XPG4 code defaults.
 *****************************************************************************/
int _DtHelpCeIconvStr(
      _DtHelpCeIconvContext iconvContext, /* iconv */
      const char * fromStr,      /* string to convert */
      char * *     ret_toStr,    /* converted str */
      size_t *     ret_toStrLen, /* converted str */
      char *       toStrBuf,     /* for efficiency, can pass in a buf */
      size_t       toStrBufLen)  /* length of buf */
{  /* XPG4-compliant code (args dflt1 & dflt2 are ignored in this code) */
   char *           toStr;        /* ptr to tostr memory */
   int              toStrSize;    /* size of mallocd string */
   size_t           inBytesLeft;  /* bytes left to use from input buf */
   const char *     inChar;       /* ptr into fromstr */
   char *           outChar;      /* ptr into tostr  */
   size_t           outBytesLeft; /* bytes left in the output str */
   int              err;          /* error code of function */

#define BAD (-1)
#define MEM_INC 20

   /* init ret values; allows function to be called nicely in a loop.  */
   if (ret_toStr) *ret_toStr = toStrBuf;
   if (ret_toStrLen) *ret_toStrLen = toStrBufLen;

   /* check args */
   if (   !iconvContext || !fromStr || !ret_toStr
       || (!ret_toStrLen && toStrBuf))
      return -1;                         /* RETURN error */

   /* just do a straight copy if codesets the same or invalid context */
   if ( iconvContext->cd == (iconv_t) BAD ) 
   {
      if (NULL == toStrBuf)
      {
         *ret_toStr = strdup(fromStr);
      }
      else  /* reuse the buffer */
      {
         int len = strlen(fromStr) + 1;
         if (len > toStrBufLen)
            *ret_toStr = realloc(toStrBuf,len);
         else 
         {
            *ret_toStr = toStrBuf;
            len = toStrBufLen;
         }
         /* set return values */
         strcpy(*ret_toStr,fromStr);
         *ret_toStrLen = len;
      }
      return (NULL != *ret_toStr ? 0 : -3);    /* RETURN result */
   }


   /* init ptrs */
   toStr = toStrBuf;
   toStrSize = (NULL == toStrBuf ? 0 : toStrBufLen);
   inChar = fromStr;
   inBytesLeft = strlen(fromStr);
   outChar = toStr;
   outBytesLeft = toStrSize;

   /* translate the string */
   err = -3;
   while ( inBytesLeft > 0 )
   {
      /* convert a character */
      if(iconv(iconvContext->cd, (ICONV_INBUF_TYPE)&inChar,
	       &inBytesLeft,&outChar,&outBytesLeft) == -1)
      {
         switch(errno)
         {
         case 0: /* no error */
            continue;
         case EINVAL: /* inChar pts to incomplete multibyte char */
            inBytesLeft = 0;       /* end the translation */
            err = -5;
            break;
         case EILSEQ: /* invalid char at inChar */
            inBytesLeft = 0;       /* end the translation */
            err = -6;
            break;
         case E2BIG: /* no room in toStr */
            /* provide enough mem in the toStr */
            if (outBytesLeft < sizeof(wchar_t))
            {
               size_t offset = outChar - (char *) toStr;
   
               outBytesLeft += MEM_INC;
               toStrSize += MEM_INC;
               toStr = realloc(toStr,toStrSize * sizeof(char));
               if (NULL == toStr)
               {
                  inBytesLeft = 0;       /* end the translation */
                  err = -3;
                  break;                 /* BREAK */
               }
               outChar = (char *) (toStr + offset); /* recalc ptr */
            }
            break;
         default:
            inBytesLeft = 0;             /* breakout of loop */
            break;
         }  /* switch on convertsion result */
      } /* if an error */
   } /* while chars left to translate */

   /* set the EOS */
   if(outChar) *outChar = EOS;

   /* set return values */
   *ret_toStr = (char *) toStr;
   if (toStrBuf) *ret_toStrLen = toStrSize;

   return (NULL != toStr ? 0 : err);    /* RETURN result */
}


/*****************************************************************************
 * Function:        void _DtHelpCeIconvClose()
 *
 * Parameters:      
 *    io_iconvContext;      context
 *
 * Return Value:    none
 *
 * Purpose:
 *              Closes an iconv context used to convert
 *              fromCode to toCode using iconv(3)
 *
 * Comments:
 *              iconv(3) is standardized in XPG4, which is just starting
 *              to be supported.  Below are several different implementations 
 *              of _DtHelpCeIconvStr, each using what is available on different 
 *              platforms.  If no code is #ifdef'd, the XPG4 code defaults.
 *****************************************************************************/
void _DtHelpCeIconvClose(
         _DtHelpCeIconvContext * io_iconvContext)
{  /* XPG4-compliant code (args dflt1 & dflt2 are ignored in this code) */
   _DtHelpCeIconvContextRec * ic;

   if (!io_iconvContext || NULL == *io_iconvContext) return;
   ic = *io_iconvContext;

   if ( ic->cd != (iconv_t) BAD ) 
        iconv_close(ic->cd);
   if (ic->fromCodeset) free(ic->fromCodeset);
   if (ic->toCodeset) free(ic->toCodeset);
   free(ic);
   *io_iconvContext = NULL;
}


/*****************************************************************************
 * Function:        void _DtHelpCeIconvContextSuitable()
 *
 * Parameters:      
 *    iconvContext:   context
 *    fromCode:       proposed fromCodeset
 *    toCode:         proposed toCodeset
 *
 * Return Value:    True: proposed conversion compatible with this context
 *                  False: proposed conversion is not compatible
 *
 * Purpose:
 *              Checks whether the proposed conversion from
 *              fromCodeset to toCodeset can be handled by
 *              the iconv context that already exists.
 *
 * Comments:
 *              This function is designed to allow a context
 *              to stay open as long as possible and avoid
 *              closing and then reopening the contexts for
 *              the same conversion.
 *****************************************************************************/
int _DtHelpCeIconvContextSuitable(
         _DtHelpCeIconvContext iconvContext,
         const char *          fromCode,
         const char *          toCode)
{
   if (   !iconvContext 
       || !iconvContext->fromCodeset 
       || !iconvContext->toCodeset
       || !fromCode 
       || !toCode) 
       return False;

   if (   strcmp(iconvContext->fromCodeset,fromCode) == 0
       && strcmp(iconvContext->toCodeset,toCode) == 0)
       return True;

   return False;
}


#if defined(_AIX) || defined (USL) || defined(__uxp__)
/*****************************************************************************
 * Function: _DtHelpCeStrCaseCmp
 *
 * Parameters:
 *
 * Return Value:
 *
 * Purpose: IBM and USL do not support the 'strcasecmp' routine. This takes it's
 *          place.
 *****************************************************************************/
int
_DtHelpCeStrCaseCmp (
    const char	*s1,
    const char	*s2)
{
    return (_DtCvStrCaseCmp(s1, s2));
}
#endif /* _AIX or USL */

/******************************************************************************
 * Function:     int _DtHelpCeFreeStringArray (char **array)
 *
 * Parameters:   array           A pointer to the NULL-terminated
 *                               string array which is to be freed.
 *
 * Return Value: 0 if successful, -1 if a failure occurs
 *
 * Purpose:      Free the memory used for a NULL-terminated string array.
 *
 ******************************************************************************/
int
_DtHelpCeFreeStringArray (char **array)
{
    char        **next;

    if (array == NULL)
        return -1;

    for (next = array; *next != NULL; next++)
        free (*next);

    free (array);
    return (0);
}

/****************************************************************************
 * Function:    void **_DtHelpCeAddPtrToArray (void **array, void *ptr)
 *
 * Parameters:  array           A pointer to a NULL-terminated array
 *                              of pointers.
 *              ptr             The pointer which is to be added to
 *                              the end of the array.
 *
 * Returns:     A pointer to the NULL-terminated array created
 *              by adding 'ptr' to the end of 'array'.
 *
 * Purpose:     Add a new element to a NULL-terminated array of pointers.
 *              These are typed as "void *" so that they can be used with
 *              pointers to any type of data.
 *
 ****************************************************************************/
void **
_DtHelpCeAddPtrToArray (
       void  **array,
       void   *ptr)
{
  return (_DtCvAddPtrToArray(array, ptr));
}

/*****************************************************************************
 * Function: _DtHelpCeStrCaseCmpLatin1
 *
 * Parameters:
 *
 * Return Value:
 *
 * Purpose: Use a version of CaseCmp that does not go through tolower().
 *          This routine should be used only for compares that now the
 *          strings are in English (iso8859-1) and do not want setlocale
 *          to screw it up.
 *****************************************************************************/
int
_DtHelpCeStrCaseCmpLatin1 (
    const char	*s1,
    const char	*s2)
{
    return (_DtCvStrCaseCmpLatin1(s1, s2));
}

/*****************************************************************************
 * Function: _DtHelpCeStrNCaseCmpLatin1
 *
 * Parameters:
 *
 * Return Value:
 *
 * Purpose: Use a version of CaseCmp that does not go through tolower().
 *          This routine should be used only for compares that now the
 *          strings are in English (iso8859-1) and do not want setlocale
 *          to screw it up.
 *****************************************************************************/
int
_DtHelpCeStrNCaseCmpLatin1 (
    const char	*s1,
    const char	*s2,
    size_t        n)
{
    return (_DtCvStrNCaseCmpLatin1(s1, s2, n));
}

