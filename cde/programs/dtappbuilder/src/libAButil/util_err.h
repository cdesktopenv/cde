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
 *	$XConsortium: util_err.h /main/4 1995/11/06 18:53:55 rswiston $
 *
 * @(#)util_err.h	1.14 11 Feb 1994	cde_app_builder/src/libAButil
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

#ifndef _ABUTIL_UTIL_ERR_H_
#define _ABUTIL_UTIL_ERR_H_
/*
 * util_err.h - error codes
 *
 * All error codes are negative.  They are numbered in 
 * groups (general, file i/o, memory, ...).
 *
 * When returning an error, use the most specific error code.
 */
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1		/* we want to be POSIX-compliant */
#endif

#include <ab_private/AB.h>
#include <ab/util_types.h>

#define OK			(0)	/* smooth sailing */
#define ERR			(-1)	/* generic error */

#define ERR_INTERNAL		(-100)	/* internal errors */
#define ERR_RECURSION		(-101)	/* recursion not allowed! */
#define ERR_DATA_SPACE_FULL	(-102)
#define ERR_BUFFER_TOO_SMALL	(-103)
#define ERR_NOT_IMPLEMENTED	(-104)
#define ERR_NOT_INITIALIZED	(-105)

#define ERR_CLIENT		(-200)	/* errors on the part of the caller */
#define ERR_NOT_ALLOWED		(-201)
#define ERR_NOT_FOUND		(-202)	/* search failed */
#define ERR_DUPLICATE_KEY	(-203)  /* insert of duplicate key */
#define ERR_BAD_PARAM		(-220)	/* bad parameter */
#define ERR_BAD_PARAM1		(-221)	/* parameter #1 bad */
#define ERR_BAD_PARAM2		(-222)
#define ERR_BAD_PARAM3		(-223)
#define ERR_BAD_PARAM4		(-224)
#define ERR_BAD_PARAM5		(-225)
#define ERR_BAD_PARAM6		(-226)
#define ERR_BAD_PARAM7		(-227)
#define ERR_BAD_PARAM8		(-228)
#define ERR_BAD_PARAM9		(-229)

#define ERR_IO			(-300)	/* I/O errors */
#define ERR_FILE_NOT_FOUND	(-301)
#define ERR_OPEN		(-302)
#define ERR_WRITE		(-303)
#define ERR_READ		(-304)
#define ERR_EOF			(-305)
#define ERR_BAD_FILE_FORMAT	(-306)
#define ERR_RENAME		(-307)
#define ERR_READ_ONLY		(-308)
#define ERR_BACKUP		(-309)

#define ERR_MEMORY		(-400)	/* memory errors */
#define ERR_NO_MEMORY		(-401)
#define ERR_MEMORY_CORRUPT	(-402)
#define ERR_MULTIPLE_FREE	(-403)	/* multiple frees of same object/buf */

/*************************************************************************
**									**
**	Functions							**
**									**
*************************************************************************/

STRING	util_error_string(int errCode);
int	util_print_error(int errCode, STRING string); /* NULL OK for string*/

#endif /* _ABUTIL_UTIL_ERR_H_ */
