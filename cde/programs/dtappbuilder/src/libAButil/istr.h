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
 *	$XConsortium: istr.h /main/4 1995/11/06 18:50:56 rswiston $
 *
 * @(#)istr.h	1.27 11 Feb 1994	cde_app_builder/src/libAButil
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
 * istr.h - string allocater
 *
 * empty value for an ISTRING is NULL.
 */
#ifndef _ISTR_H
#define _ISTR_H

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1		/* we want to be POSIX-compliant */
#endif

#include <string.h>
#include <ab_private/AB.h>
#include <ab/util_types.h>
#include <ab_private/util.h>

typedef struct
{
    BYTE	you_dont_want_to_use_this_field;
} ISTRING_PUBLIC_REC;

typedef ISTRING_PUBLIC_REC *ISTRING;

/* 
 * Allocate a new istring for a given string and return the value,
 * or if the string already exists return the value
 * for an existing istring. If the string was previously allocated as
 * a read only string change its status to const and its string 
 * pointer.
 * Returns -1 if error.  
 * Note: This string is read only. The string sent in is already 
 *	 allocated and the destroy command will not deallocate the 
 *	 string 
 */
extern ISTRING	istr_create_const(
		    STRING string
		);

/* 
 * Identical to istr_create_const (shortcut) 
 */
extern ISTRING	istr_const(
		    STRING string
		);

/* 
 * Allocate a new istring for a given string and return the value, 
 * or if the string already exists return the value
 * for an existing istring. Returns -1 if error. 
 * Note: This string is read only. The string sent in is already 
 *	 allocated and the destroy command will deallocate the 
 *	 string 
 */
extern ISTRING	istr_create_alloced(
		    STRING string
		);

/* 
 * Allocate a new istring for a given string and return the value, 
 * or if the string already exists return the value for an 
 * existing istring. Returns -1 if error. 
 * Note: This string is read only. The string sent in has not 
 *	 been allocated and the destroy command will deallocate 
 *	 the string.
 */
extern ISTRING	istr_create(
		    STRING string
		);

/* 
 * Deallocate for the string if refcount=0 else decrement the refcount.
 * If the string type is const then don't deallocate.
 * returns -1 if error, else returns 1
 */
extern int	istr_destroy(
		    ISTRING istring
		);

/* 
 * Return the string value, and increment counter 
 * return -1 if error 
 */
extern ISTRING	istr_dup(
		    ISTRING istring
		);

/* 
 * Return (STRING) for istring, return NULL if error
 */
extern STRING	istr_string(
		    ISTRING istring
		);

/* 
 * Won't return NULL - returns string associated with istring,
 * or "(nil)" 
 * Note: printable string returned 
 */
extern STRING	istr_string_safe(
		    ISTRING istring
		);

/* 
 * Finds the existing ISTRING value for string, returns NULL if string
 * doesn't exist 
 *
 * Note that this dups the existing string, and istr_destroy() must
 * be called on the returned ISTRING when it is no longer needed.
 */
extern ISTRING	istr_dup_existing(
		    STRING s
		);

/* 
 * Moves the istring in "from" to the istring in "to" and 
 * sets "from" to NULL
 * always returns 0 
 */
extern int	istr_move(
		    ISTRING to, 
		    ISTRING from
		);

/* 
 * Returns the length of the string, returns NULL if the 
 * string is empty 
 */
extern int	istr_len(
		    ISTRING s
		);
 
/* 
 * Compare the two strings, returns the same as strcmp 
 */
extern int	istr_cmp(
		    ISTRING s1, 
		    ISTRING s2
		);

/* 
 * Compare the two istrings, returns TRUE if equal 
 */
extern BOOL	istr_equal(
		    ISTRING s1, 
		    ISTRING s2
		);

/* 
 * Compare the two strings, returns the same as strcmp 
 */
extern int	istr_cmpstr(
		    ISTRING s1, 
		    STRING s2
		);

/* 
 * Compare the two strings, returns TRUE if equal 
 */
extern BOOL	istr_equalstr(
		    ISTRING s1, 
		    STRING s2
		);

/*
 * Verifies that the given ISTRING is valid (e.g., that it exists, and that
 * it is not corrupted.
 *
 * Returns < 0 if an error is detected
 */
int istr_verify(ISTRING);

/*
 * Checks all the ISTRINGS and internal data structures to see if anything
 * has become corrupted.
 *
 * Returns < 0 if an error is detected
 */
int istr_verify_all(void);


/*************************************************************************
**									**
**		Internal functions - clients must not call these!	**
**									**
**************************************************************************/

typedef struct
{
    int   		refcount;
    STRING		str;
    unsigned int 	read_const : 1;
} ISTR_PRIVT_STRN;

extern STRING		Istr_null_string;
extern int		istrP_destroy_impl518283652PrivF(ISTRING*);
extern ISTRING		istrP_create_alloced_impl9726039350PrivF(STRING*);
extern int		istrP_notify_invalid2160413670PrivF(ISTRING);
extern STRING		istrP_get_string_fast3718930164PrivF(ISTRING);
extern STRING		istrP_get_string_verify4521632085PrivF(ISTRING);
extern int		istrP_num_count7608925912PrivD;
extern ISTR_PRIVT_STRN	*istrP_int_array1809065681PrivD;


/*************************************************************************
**									**
**		Inline implementation					**
**									**
**************************************************************************/

#define istr_equal(a,b)	((a) == (b))
#define istr_len(s)	((s)==NULL? 0:strlen(istr_string(s)))

#define istr_destroy(s)				\
    (istrP_destroy_impl518283652PrivF(&(s)))

#define istr_create_alloced(s)			\
    (istrP_create_alloced_impl9726039350PrivF(&(s)))

#define istr_move(a,b)  ((int)((a)=(b), (b)=NULL))

#define istr_cmp(a,b)   (util_strcmp(istr_string(a),istr_string(b)))

#define istr_cmpstr(a,b)   (util_strcmp(istr_string(a),(b)))

#define istr_equalstr(a,b)   (istr_cmpstr(a,b)==0)

#define istr_const(s)	(istr_create_const(s))

#define istr_string_safe(s) ((s) == NULL? Istr_null_string:istr_string(s))

#define istrP_get_string_fast3718930164PrivF(istring) \
    (istrP_int_array1809065681PrivD[((int)istring)].str)

/* istr_string - if debugging is turned on, checks ISTRINGS for validity */
#ifdef DEBUG
    #define istr_string(istring) \
        ((!debugging())? \
	    istrP_get_string_fast3718930164PrivF(istring) \
        : \
	    istrP_get_string_verify4521632085PrivF(istring) \
        )
#else
    #define istr_string(istring) \
	    (istrP_get_string_fast3718930164PrivF(istring))
#endif /* DEBUG */

#endif /* _ISTR_H */
