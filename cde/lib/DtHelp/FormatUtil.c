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
/* $XConsortium: FormatUtil.c /main/9 1996/11/01 10:12:14 drk $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:	FormatUtil.c
 **
 **   Project:     Text Graphic Display Library
 **
 **  
 **   Description: Semi private format utility functions that do not
 **		   require the Display Area, Motif, Xt or X11.
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
 ****************************************************************************
 ************************************<+>*************************************/

/*
 * system includes
 */
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <X11/Xos.h>
#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

/*
 * Canvas Engine includes
 */
#include "CanvasP.h"
#include "CanvasSegP.h"

/*
 * private includes
 */
#include "CanvasError.h"
#include "bufioI.h"
#include "FormatUtilI.h"

#if defined(NLS16) || !defined(NO_MESSAGE_CATALOG)
#include <nl_types.h>
#endif

#ifndef NL_CAT_LOCALE
static const int NL_CAT_LOCALE = 0;
#endif

/********    Private Function Declarations    ********/
/********    End Private Function Declarations    ********/

/******************************************************************************
 *
 * Private variables and defines.
 *
 *****************************************************************************/
#define GROW_SIZE	 5

/******************************************************************************
 *
 * Private Functions
 *
 *****************************************************************************/
/******************************************************************************
 *
 * Semi Public Functions
 *
 *****************************************************************************/
/******************************************************************************
 * Function: int _DtHelpCeAddOctalToBuf (char *src, char **dst,
 *					int *dst_size,
 *					int *dst_max, int grow_size)
 *
 * Parameters:
 *		src		Specifies a pointer to a string.
 *		dst		Specifies a pointer to the buffer to
 *					to hold the information.
 *		dst_size	Specifies the current size of 'dst'.
 *				Returns the new size of 'dst'.
 *		dst_max		Specifies the current maximum size of 'dst'.
 *				Returns the new maximum size of 'dst'.
 *		grow_size	Specifies the minimum grow size of 'dst'
 *					when a malloc/realloc occurs.
 *				If this is less than one, 'dst' will
 *					grow only large enough to hold
 *					the new character.
 *
 * Returns:	0 if successful, -1 if errors.
 *
 * errno Values:
 *		EINVAL
 *		CEErrorMalloc
 *
 * Purpose:	(Re-)Allocates, if necessary, enough memory to hold the old
 *			information plus the byte.
 *		Coverts the 0xXX value pointed to by src to a 0-256 by value.
 *		Appends the character to the buffer pointed to 'dst'.
 *		Updates 'dst_size' to include the new character.
 *		Updates 'dst_max' to the new size of 'dst' if a
 *			malloc/realloc occurred.
 *
 *****************************************************************************/
int
_DtHelpCeAddOctalToBuf(
	char	 *src,
	char	**dst,
	int	 *dst_size,
	int	 *dst_max,
	int	  grow_size )
{
    char     tmp;
    char    *dstPtr;
    unsigned long value;

    if (src == NULL ||
		dst == NULL || dst_size == NULL || dst_max == NULL ||
				(*dst == NULL && (*dst_size || *dst_max)))
      {
	errno = EINVAL;
	return -1;
      }

    dstPtr = *dst;
    if ((*dst_size + 2) >= *dst_max)
      {
	if (grow_size > *dst_size + 3 - *dst_max)
	    *dst_max = *dst_max + grow_size;
	else
	    *dst_max = *dst_size + 3;

	if (dstPtr)
	    dstPtr = (char *) realloc ((void *) dstPtr, *dst_max);
	else
	  {
	    dstPtr = (char *) malloc (sizeof(char) * (*dst_max));
	    *dst_size = 0;
	  }
	*dst = dstPtr;
      }

    /*
     * check to see if we have good memory
     */
    if (!dstPtr)
      {
	errno = CEErrorMalloc;
	return -1;
      }

    tmp = src[4];
    src[4] = '\0';
    value = strtoul (src, NULL, 16);
    src[4] = tmp;

    if ((value == ULONG_MAX && errno == ERANGE) || value > 255 || value < 1)
      {
	errno = CEErrorFormattingValue;
	return -1;
      }

    /*
     * copy the source into the destination
     */
    dstPtr[*dst_size] = (char ) value;

    /*
     * adjust the pointers.
     */
    *dst_size = *dst_size + 1;

    /*
     * null the end of the buffer.
     */
    dstPtr[*dst_size] = '\0';

    return 0;
}

/******************************************************************************
 * Function: int __CEAppendCharToInfo (char **src, char **dst, int *dst_size,
 *					int *dst_max, int grow_size)
 *
 * Parameters:
 *		src		Specifies a pointer to a string.
 *		dst		Specifies a pointer to the buffer to
 *					to hold the information.
 *		dst_size	Specifies the current size of 'dst'.
 *				Returns the new size of 'dst'.
 *		dst_max		Specifies the current maximum size of 'dst'.
 *				Returns the new maximum size of 'dst'.
 *		grow_size	Specifies the minimum grow size of 'dst'
 *					when a malloc/realloc occurs.
 *				If this is less than one, 'dst' will
 *					grow only large enough to hold
 *					the new character.
 *
 * Returns:	0 if successful, -1 if errors.
 *
 * errno Values:
 *		EINVAL
 *		CEErrorMalloc
 *
 * Purpose:	(Re-)Allocates, if necessary, enough memory to hold the old
 *			information plus the new.
 *		Appends the character pointed to by 'src' to the buffer
 *			pointed to 'dst'.
 *		Updates 'src' to point to the next character after the
 *			one appended to 'dst'.
 *		Updates 'dst_size' to include the new character.
 *		Updates 'dst_max' to the new size of 'dst' if a
 *			malloc/realloc occurred.
 *
 *****************************************************************************/
int
_DtHelpCeAddCharToBuf(
	char	**src,
	char	**dst,
	int	 *dst_size,
	int	 *dst_max,
	int	  grow_size )
{
    char    *srcPtr;
    char    *dstPtr;

    if (src == NULL || *src == NULL ||
		dst == NULL || dst_size == NULL || dst_max == NULL ||
				(*dst == NULL && (*dst_size || *dst_max)))
      {
	errno = EINVAL;
	return -1;
      }

    srcPtr = *src;
    dstPtr = *dst;
    if ((*dst_size + 2) >= *dst_max)
      {
	if (grow_size > *dst_size + 3 - *dst_max)
	    *dst_max = *dst_max + grow_size;
	else
	    *dst_max = *dst_size + 3;

	if (dstPtr)
	    dstPtr = (char *) realloc ((void *) dstPtr, *dst_max);
	else
	  {
	    dstPtr = (char *) malloc (*dst_max);
	    *dst_size = 0;
	  }
	*dst = dstPtr;
      }

    /*
     * check to see if we have good memory
     */
    if (!dstPtr)
      {
	errno = CEErrorMalloc;
	return -1;
      }

    /*
     * copy the source into the destination
     */
    dstPtr[*dst_size] = *srcPtr++;

    /*
     * adjust the pointers.
     */
    *src = srcPtr;
    *dst_size = *dst_size + 1;

    /*
     * null the end of the buffer.
     */
    dstPtr[*dst_size] = '\0';

    return 0;
}

/******************************************************************************
 * Function: int _DtHelpCeAddStrToBuf (char **src, char **dst, int *dst_size,
 *				int *dst_max, int copy_size, int grow_size)
 * 
 * Parameters:
 *		src		Specifies a pointer to a string.
 *		dst		Specifies a pointer to the buffer to
 *					to hold the information.
 *		dst_size	Specifies the current size of 'dst'.
 *				Returns the new size of 'dst'.
 *		dst_max		Specifies the current maximum size of 'dst'.
 *				Returns the new maximum size of 'dst'.
 *		copy_size	Specifies the number of characters to
 *					copy from 'src' to 'dst'.
 *		grow_size	Specifies the minimum grow size of 'dst'
 *					when a malloc/realloc occurs.
 *				If this is less than one, 'dst' will
 *					grow only large enough to hold
 *					the new character.
 *
 * Returns:	0 if successful, -1 if errors.
 *
 * errno Values:
 *		EINVAL
 *		CEErrorMalloc
 *
 * Purpose:	Copys 'copy_size' number of characters of 'src'
 *			to 'dst'.
 *		Updates 'src', to point after 'copy_size' number of
 *			characters.
 *		Updates the 'dst_size' to reflect the number of characters
 *			copied.
 *		If required, increments dst_max and (re)allocs memory
 *			to hold the extra 'copy_size' number of characters.
 *
 *****************************************************************************/
int
_DtHelpCeAddStrToBuf (
	char	**src,
	char	**dst,
	int	 *dst_size,
	int	 *dst_max,
	int	  copy_size,
	int	  grow_size )
{
    char *srcPtr;
    char *dstPtr;

    /*
     * check the input
     */
    if (src == NULL || *src == NULL || (((int)strlen(*src)) < copy_size)
		|| dst == NULL || dst_size == NULL || dst_max == NULL
		|| (*dst == NULL && (*dst_size || *dst_max)))
      {
	errno = EINVAL;
	return -1;
      }

    srcPtr = *src;
    dstPtr = *dst;

    if ((*dst_size + copy_size + 1) >= *dst_max)
      {
	if (grow_size > (*dst_size + copy_size + 2 - *dst_max))
	    *dst_max = *dst_max + grow_size;
	else
	    *dst_max = *dst_size + copy_size + 2;

	if (dstPtr)
	    dstPtr = (char *) realloc ((void *) dstPtr, *dst_max);
	else
	  {
	    dstPtr = (char *) malloc (*dst_max);
	    *dst_size = 0;
	  }
	*dst = dstPtr;
      }

    if (!dstPtr)
      {
	errno = CEErrorMalloc;
	return -1;
      }

    /*
     * make sure there is a null byte to append to.
     */
    dstPtr[*dst_size] = '\0';

    /*
     * copy the source into the destination
     */
    strncat (dstPtr, srcPtr, copy_size);

    /*
     * adjust the pointers
     */
    *src = srcPtr + copy_size;
    *dst_size = *dst_size + copy_size;

    return 0;
}

/******************************************************************************
 * Function: int _DtHelpCeGetNxtBuf (FILE *file, char *dst, char **src,
 *					int max_size)
 * 
 * Parameters:
 *		file		Specifies a stream to read from.
 *		dst		Specifies the buffer where new information
 *					is placed.
 *		src		Specifies a pointer into 'dst'. If there
 *					is information left over, it
 *					is moved to the begining of 'dst'.
 *				Returns 'src' pointing to 'dst'.
 *		max_size	Specifies the maximum size of 'dst'.
 *
 * Returns:	 0 if this is the last buffer that can be read for the topic.
 *		-1 if errors.
 *		>0 if more to be read.
 *
 * errno Values:
 *		read (2)	Errors set via a read call.
 *		EINVAL
 *		CEErrorReadEmpty
 *
 * Purpose:	Reads the next buffer of information.
 *
 *****************************************************************************/
int
_DtHelpCeGetNxtBuf(
    BufFilePtr	  file,
    char	 *dst,
    char	**src,
    int		  max_size)
{
    int	leftOver;
    int	result;

    if (file == NULL)
      {
	errno = EINVAL;
	return -1;
      }

    (void ) strcpy (dst, (*src));
    leftOver = strlen (dst);

    result = _DtHelpCeReadBuf (file, &(dst[leftOver]), (max_size - leftOver));

    /*
     * check to see if we ran into trouble reading this buffer
     * of information. If not reset the pointer to the beginning
     * of the buffer.
     */
    if (result != -1)
	*src = dst;

    return result;
}


/******************************************************************************
 * Function: 	int  _DtHelpCeReadBuf (FILE *file, char *buffer, int size)
 *
 * Parameters:	FILE		Specifies the stream to read from.
 *		buffer		Specifies a buffer to read information
 *					into.
 *		size	 	Specifies the maximum number of bytes
 *				'buffer' can contain. It should never be
 *				larger than 'buffer' can hold, but it can
 *					be smaller.
 *
 * Returns:	 0 if this is the last buffer that can be read for the topic.
 *		-1 if errors.
 *		>0 if more to be read.
 *
 * errno Values:
 *		read (2)	Errors set via a read call.
 *
 * Purpose:	Get size-1 number of bytes into a buffer and possibly
 *		check for page markers imbedded within the text.
 *
 *****************************************************************************/
int
_DtHelpCeReadBuf(
    BufFilePtr	 file,
    char	*buffer,
    int		 size)
{
    int    flag;

    /*
     * take into account the last byte must be an end of string marker.
     */
    size--;

    flag = _DtHelpCeBufFileRd(file, buffer, size);

    if (flag != -1)
	buffer[flag] = '\0';

    return flag;

} /* End _DtHelpCeReadBuf */

/******************************************************************************
 * Function:    char *_DtHelpGetNxtToken (char *str, char **retToken)
 *
 * Parameters:
 *		str             The string (in memory) which is being
 *					parsed.
 *              retToken        Returns the next token from the input.
 *				Valid tokens are strings of non-whitespace
 *                              characters, newline ("\n"), and
 *                              end-of-data (indicated by a zero length
 *                              string).
 *
 *				A NULL value indicates an error.
 *
 *                              Newline or zero length strings are
 *                              not owned by the caller.
 *
 *                              Otherwise, the memory for the returned
 *                              token is owned by the caller.
 *
 * Return Value:	Returns the pointer to the next unparsed character in
 *			the input string. A NULL value indicates an error.
 *
 * errno Values:
 *		EINVAL
 *		CEErrorMalloc
 *
 * Purpose:     Parse tokens in resource string values.
 *
 *****************************************************************************/
char *
_DtHelpGetNxtToken (
    char        *str,
    char        **retToken)
{
    int          len = 1;
    char        *start;
    char        *token;
    short	 quote = False;
    short	 done = False;

    if (retToken) *retToken = NULL;  /* tested in caller code */

    if (str == NULL || *str == '\0' || retToken == NULL)
      {
	errno = EINVAL;
	return NULL;
      }

    /* Find the next token in the string.  The parsing rules are:

         - Whitespace (except for \n) separates tokens.
         - \n is a token itself.
         - The \0 at the end of the string is a token.
     */

    /* Skip all of the whitespace (except for \n). */
    while (*str && (*str != '\n') && isspace (*str))
        str++;

    /* Str is pointing at the start of the next token.  Depending on the
       type of token, malloc the memory and copy the token value. */
    if (*str == '\0')
        token = str;

    else if (*str == '\n') {
        token = str;
        str++;
    }

    else {
        /* We have some non-whitespace characters.  Find the end of */
        /* them and copy them into new memory. */
	if ((MB_CUR_MAX == 1 || mblen (str, MB_CUR_MAX) == 1) && *str == '\"')
	  {
	    /*
	     * found a quoted token - skip the quote.
	     */
	    quote = True;
	    str++;
	  }

        start = str;
        while (*str && !done)
	  {
	    /*
	     * get the length of the item.
	     */
	    len = 1;
	    if (MB_CUR_MAX != 1)
	      {
	        len = mblen (str, MB_CUR_MAX);
	        if (len < 0)
		    len = 1;
	      }
	    if (len == 1)
	      {
		/*
		 * check for the token terminator
		 */
	        if ((quote && *str == '\"') ||
				(!quote && (isspace (*str) || *str == '\n')))
		    done = True;
		else
		    str++;
	      }
	    else
	        str += len;
	  }

	/*
	 * determine the length of the token.
	 */
        token = (char *) malloc ((str - start + 1) * sizeof (char));
	if (token)
	  {
            strncpy (token, start, str - start);
            *(token + (str - start)) = '\0';
	  }
	else
	    errno = CEErrorMalloc;

	/*
	 * skip the quote terminator
	 */
	if (quote && len == 1 && *str == '\"')
	    str++;
    }

    *retToken = token;
    return (str);
}

/******************************************************************************
 * Function:    _DtCvSegment *_DtHelpAllocateSegments(int malloc_size)
 *
 * Parameters:
 *              malloc_size     Specifies the number of segments to
 *				allocate if 'alloc_size' is NULL or less
 *				than 1 or if '*next_seg' is NULL.  The
 *				first one is returned to the caller with
 *				the rest in 'next_seg' if 'next_seg' is
 *				non-NULL.
 *
 * Return Value:
 *		non-null   If succeeds.
 *              Null       If failure.
 *
 * Purpose:  Allocate a block of segments, zeros the structures, and
 *	     sets the link_idx of each structure to -1.
 *
 *****************************************************************************/
_DtCvSegment *
_DtHelpAllocateSegments (
     int	  malloc_size)
{
    int			 i;
    _DtCvSegment	*newSeg;
    FrmtPrivateInfo	*p;

    /*
     * don't allow zero or negative allocations
     */
    if (malloc_size < 1)
	malloc_size = 1;

    /*
     * allocate the block of segments
     */
    newSeg = (_DtCvSegment *) calloc (sizeof(_DtCvSegment), malloc_size);
    if (NULL != newSeg)
      {
	/*
	 * now allocate the same number of private information structures
	 */
	p = (FrmtPrivateInfo *) calloc (sizeof(FrmtPrivateInfo), malloc_size);
	if (NULL != p)
	  {
	    /*
	     * mark the first item as the top block.  Since it will be
	     * attached to the first segment, it marks both the segment
	     * and the private information blocks for later frees.
	     */
	    p->top_block = True;
	    for (i = 0; i < malloc_size; i++)
	      {
		/*
		 * invalidate the link index and attach a private
		 * information structure to each segment
		 */
	        newSeg[i].link_idx   = -1;
		newSeg[i].client_use = p++;
	      }
	  }
	else
	  {
	    /*
	     * had trouble allocating the private information.
	     * free the new segment list and return NULL as an error.
	     */
	    free(newSeg);
	    newSeg = NULL;
	  }
      }

    return newSeg;
}

/******************************************************************************
 * Function:    int _DtHelpFmtFindBreak (char *ptr, int mb_len, int *num_chars)
 *
 * Parameters:
 *              ptr             Specifies the string to check.
 *              mb_len          Specifies if the sequence should be single
 *                              byte or multi-byte.
 *              num_chars       Returns the character count.
 *
 * Returns      number of bytes in the sequence.
 *
 * errno Values:
 *
 * Purpose:     Find a length of 'ptr' comprised of multi or single byte
 *              characters.
 *
 *****************************************************************************/
int
_DtHelpFmtFindBreak (
    char *ptr,
    int   mb_len,
    int  *num_chars)
{
    int   len = 0;
    int   numChars = 0;
    int   mySize;
    short done = 0;

    while (0 == done && '\0' != *ptr)
      {
        mySize = mblen(ptr, MB_CUR_MAX);
        done   = 1;
        if (0 < mySize &&
		((1 != mb_len && 1 != mySize) || (1 == mb_len && 1 == mySize)))
          {
            numChars++;
            ptr  += mySize;
            len  += mySize;
            done  = 0;
          }
      }

    *num_chars = numChars;
    return len;
}

/******************************************************************************
 * Function:	_DtHelpLoadMultiInfo
 *
 * Returns:	Loads the multi-byte formatting table for the current locale.
 *
 *****************************************************************************/
void
_DtHelpLoadMultiInfo (
    wchar_t  **cant_begin_chars,
    wchar_t  **cant_end_chars,
    short     *nl_to_space)
{
#ifndef NO_MESSAGE_CATALOG
    int       len;
    char     *ptr;
    nl_catd   cat_fd;

    cat_fd = catopen ("fmt_tbl", NL_CAT_LOCALE);
    if (cat_fd != ((nl_catd) -1))
      {
	/*
	 * Get the list of characters that can't begin a line.
	 */
	ptr = catgets (cat_fd, 1, 1, "");
	len = strlen (ptr) + 1;
	*cant_begin_chars = (wchar_t *) malloc (len * sizeof (wchar_t));
	if (NULL != *cant_begin_chars &&
				mbstowcs(*cant_begin_chars, ptr, len) == -1)
	  {
	    free (*cant_begin_chars);
	    *cant_begin_chars = NULL;
	  }

	/*
	 * Get the list of characters that can't end a line.
	 */
	ptr = catgets (cat_fd, 1, 2, "");
	len = strlen (ptr) + 1;
	*cant_end_chars = (wchar_t *) malloc (len * sizeof (wchar_t));
	if (*cant_end_chars != NULL &&
				mbstowcs(*cant_end_chars, ptr, len) == -1)
	  {
	    free (*cant_end_chars);
	    *cant_end_chars = NULL;
	  }

	/*
	 * Get the spacing flag. I.E. when does a internal newline
	 * get turned into a space.
	 *      1 means all the time.
	 *      0 means only between a multibyte string and
	 *        a singlebyte string.
	 */
	ptr = catgets (cat_fd, 1, 3, "1");
	*nl_to_space = atoi(ptr);

	catclose (cat_fd);
      }
    else
#endif
      {
	*cant_begin_chars = NULL;
	*cant_end_chars   = NULL;
	*nl_to_space      = 1;
      }
}
