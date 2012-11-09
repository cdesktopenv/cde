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
/* $XConsortium: CvString.c /main/4 1996/08/30 15:28:00 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:     CvString.c
 **
 **   Project:  Cde DtHelp
 **
 **   Description: Semi private string functions - can be platform dependent.
 **                These routines are straight C code. They do not require
 **                includes to know about the Canvas Engine, the Help
 **                dialogs or anything else.
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

/*
 * private includes
 */
#include "CvStringI.h"

/******************************************************************************
 *
 * Private variables and defines.
 *
 *****************************************************************************/
#define REALLOC_INCR    10

/******************************************************************************
 *
 * Semi Public Functions
 *
 *****************************************************************************/
/******************************************************************************
 * Function: _DtCvStrLen (const void *p1, int type)
 *
 * Paramenters:
 *		p1	Specifies the string or wide char array to process.
 *		type	Specifies the type p1 is. 0 indicaates a string.
 *			non-zero indicates a wide char array.
 * Returns:
 *		>= 0	If p1 is non-null.
 *		-1	If p1 is null.
 *
 * Purpose:
 *	Returns in number of characters in p1.
 *
 *****************************************************************************/
int
_DtCvStrLen (
    const void    *p1,
    int            type )
{
    int		   len = 0;
    const wchar_t *wcs;

    if (0 == type)
	return (strlen((const char *) p1));

    wcs = (wchar_t *) p1;
    while (0 != *wcs)
      {
	wcs++;
	len++;
      }

    return len;
}

/******************************************************************************
 * Function: _DtCvStrPtr (const void *p1, int type, int count)
 *
 * Paramenters:
 *		p1	Specifies the string or wide char array to process.
 *		type	Specifies the type p1 is. 0 indicaates a string.
 *			non-zero indicates a wide char array.
 *		count	Specifies an index into p1.
 * Returns:
 *		ptr	If everything works okay.
 *		NULL 	If problems.
 *
 * Purpose:
 *	Get to a point in the wide character or char string.
 *
 *****************************************************************************/
void *
_DtCvStrPtr (
    const void    *p1,
    int            type,
    int		   count)
{

    if (0 == type)
	return ((void *) (((char *) p1) + count));

    return ((void *) (((wchar_t *) p1) + count));
}

/******************************************************************************
 * Function: _DtCvChar (const void *p1, int type, int count)
 *
 * Paramenters:
 *		p1	Specifies the string or wide char array to process.
 *		type	Specifies the type p1 is. 0 indicaates a string.
 *			non-zero indicates a wide char array.
 *		count	Specifies an index into p1.
 * Returns:
 *		value		If everything works okay.
 *		(wchar_t) -1	If problems.
 *
 * Purpose:
 *	Get a character value.
 *
 *****************************************************************************/
wchar_t
_DtCvChar (
    const void    *p1,
    int            type,
    int		   count)
{
    wchar_t value = (wchar_t) -1;

    if (0 == type)
	value = *(((char *) p1) + count);
    else
	value = *(((wchar_t *) p1) + count);

    return (value);
}

/******************************************************************************
 * Function: _DtCvStrcspn (const void *s1, const char *s2, type, &ret_len)
 *
 *	Returns in 'ret_len' the length of the initial segment of string
 *	s1 which consists entirely of characters not found in string s2.
 *
 *	if type is 0, then s1 is a char* string.
 *	if type is not 0, then s1 is a wchar_t* string.
 *
 *      s2 cannot be more than 15 characters if looking a wide character
 *	string in s1.
 *
 * Returns:
 *      -1  If found an invalid character.
 *       0  If found a character in string s2
 *       1  If found the null byte character.
 *       2  s2 contained more than 15 characters, but found one of the
 *	    first fifteen characters in string s2.
 *	 3  s2 contained more than 15 characters, and didn't find any
 *	    of the first fifteen characters in string s2.
 *****************************************************************************/
int
_DtCvStrcspn (
    const void    *s1,
    const char    *s2,
    int            type,
    int           *ret_len )
{
    size_t         num;
    wchar_t	   widec[16];
    const wchar_t *wcp;
    const wchar_t *wcp2;

    if (NULL == s1)
      {
	*ret_len = 0;
	return 0;
      }

    if ((0 == type && '\0' == *((char *) s1)) ||
	(0 != type && 0 == *((wchar_t *) s1)))
      {
	*ret_len = 0;
	return 1;
      }

    if (NULL == s2 || '\0' == *s2)
      {
	*ret_len = _DtCvStrLen (s1, type);
	return 1;
      }

    if (0 == type)
      {
	/*
	 * no need to go through any hassle, just use the 3C function
	 */
	*ret_len = strcspn ((char *) s1, s2);
	if ('\0' == ((char *) s1)[*ret_len])
	    return 1;
	return 0;
      }

    /*
     * convert the test string into a wide char array
     */
    num = mbstowcs(widec, s2, 16);
    if ((size_t) -1 == num)
	return -1;

    /*
     * force a null termination of the array
     */
    widec[15] = 0;

    /*
     * if greater than 15 characters were converted, set the flag to 2
     */
    num = ((num > 15) ? 2 : 0);

    wcp = s1;
    while (0 != *wcp)
      {
	wcp2 = widec;
	while (0 != *wcp2)
	  {
	    if (*wcp2 == *wcp)
	      {
		*ret_len = wcp - (wchar_t *) s1;
		return 0 + num;
	      }
	    wcp2++;
	  }
	wcp++;
      }

    *ret_len = wcp - (wchar_t *) s1;
    return 1 + num;
}

/****************************************************************************
 * Function:    void **_DtCvAddPtrToArray (void **array, void *ptr)
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
_DtCvAddPtrToArray (
       void  **array,
       void   *ptr)
{

    void **nextP = NULL;
    int numElements;

    /* If this is the first item for the array, malloc the array and set
       nextP to point to the first element. */
    if (array == NULL || *array == NULL) {
        array = (void **) malloc (REALLOC_INCR * sizeof (void *));

        nextP = array;
    }

    else {

        /* Find the NULL pointer at the end of the array. */
        numElements = 0;
        for (nextP = array; *nextP != NULL; nextP++)
                numElements++;

        /* The array always grows by chunks of size REALLOC_INCR.  So see if
           it currently is an exact multiple of REALLOC_INCR size (remember to
           count the NULL pointer).  If it is then it must be full, so realloc
           another chunk.  Also remember to move 'nextP' because the array
           will probably move in memory. */
        if ((numElements + 1) % REALLOC_INCR == 0) {
            array = (void **) realloc (array,
                        (numElements + 1 + REALLOC_INCR) * sizeof (void *));
            if (array)
                nextP = array + numElements;
            else
                nextP = NULL;
        }
    }

    if (nextP)
      {
        *nextP++ = ptr;
        *nextP = NULL;
      }

    return (array);
}

/******************************************************************************
 * Function:     int _DtCvFreeArray (void **array)
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
_DtCvFreeArray (void **array)
{
    void        **next;

    if (array == NULL)
        return -1;

    for (next = array; *next != NULL; next++)
        free (*next);

    free (array);
    return (0);
}

#if defined(_AIX) || defined (USL) || defined(__uxp__)
/*****************************************************************************
 * Function: _DtCvStrCaseCmp
 *
 * Parameters:
 *
 * Return Value:
 *
 * Purpose: IBM and USL do not support the 'strcasecmp' routine. This takes it's
 *          place.
 *****************************************************************************/
int
_DtCvStrCaseCmp (
    const char	*s1,
    const char	*s2)
{
    int   c1;
    int   c2;
    int   result = 0;

    if (s1 == s2)    return  0;
    if (NULL == s1)  return -1;
    if (NULL == s2)  return  1;


    while (result == 0 && *s1 != '\0' && *s2 != '\0')
      {
	c1 = (unsigned char) *s1;
	c2 = (unsigned char) *s2;

	if (isupper(c1))
	    c1 = _tolower(c1);
	if (isupper(c2))
	    c2 = _tolower(c2);

	result = c1 - c2;
	s1++;
	s2++;
      }

    if (result == 0 && (*s1 != '\0' || *s2 != '\0'))
      {
	c1 = (unsigned char) *s1;
	c2 = (unsigned char) *s2;

	if (isupper(c1))
	    c1 = _tolower(c1);
	if (isupper(c2))
	    c2 = _tolower(c2);

	result = c1 - c2;
      }

    return result;
}
#endif /* _AIX or USL */

/*****************************************************************************
 * Function: _DtCvStrNCaseCmpLatin1()
 *
 * Purpose: Does not use the tolower() functions to determine the lower
 *	    case of a character.  On some platforms, using tolower() on
 *	    the upper case of a typical Latin1 character does not match
 *	    it's Latin1 one lower case.
 *
 *	    This routine is to be used only when we want to map strict
 *	    Latin1 characters to it's lower case.  I.e.  when we -know-
 *	    the data is in English.
 *****************************************************************************/
int
_DtCvStrNCaseCmpLatin1 (
    const char	*s1,
    const char	*s2,
    size_t	 n)
{
    int   c1;
    int   c2;
    int   result = 0;

    if (s1 == s2 || n < 1) return  0;
    if (NULL == s1)        return -1;
    if (NULL == s2)        return  1;


    while (result == 0 && *s1 != '\0' && *s2 != '\0' && n > 0)
      {
	c1 = (unsigned char) *s1;
	c2 = (unsigned char) *s2;

	c1 = _DtCvToLower(c1);
	c2 = _DtCvToLower(c2);

	result = c1 - c2;
	s1++;
	s2++;
	n--;
      }

    if (result == 0 && n > 0 && (*s1 != '\0' || *s2 != '\0'))
      {
	c1 = (unsigned char) *s1;
	c2 = (unsigned char) *s2;

	c1 = _DtCvToLower(c1);
	c2 = _DtCvToLower(c2);

	result = c1 - c2;
      }

    return result;
}

/*****************************************************************************
 * Function: _DtCvStrCaseCmpLatin1
 *
 * Purpose: Does not use the tolower() functions to determine the lower
 *	    case of a character.  On some platforms, using tolower() on
 *	    the upper case of a typical Latin1 character does not match
 *	    it's Latin1 one lower case.
 *
 *	    This routine is to be used only when we want to map strict
 *	    Latin1 characters to it's lower case.  I.e.  when we -know-
 *	    the data is in English.
 *****************************************************************************/
int
_DtCvStrCaseCmpLatin1 (
    const char	*s1,
    const char	*s2)
{
    int   c1;
    int   c2;
    int   result = 0;

    if (s1 == s2)    return  0;
    if (NULL == s1)  return -1;
    if (NULL == s2)  return  1;


    while (result == 0 && *s1 != '\0' && *s2 != '\0')
      {
	c1 = (unsigned char) *s1;
	c2 = (unsigned char) *s2;

	c1 = _DtCvToLower(c1);
	c2 = _DtCvToLower(c2);

	result = c1 - c2;
	s1++;
	s2++;
      }

    if (result == 0 && (*s1 != '\0' || *s2 != '\0'))
      {
	c1 = (unsigned char) *s1;
	c2 = (unsigned char) *s2;

	c1 = _DtCvToLower(c1);
	c2 = _DtCvToLower(c2);

	result = c1 - c2;
      }

    return result;
}

