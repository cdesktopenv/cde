/* $XConsortium: I18nUtil.h /main/1 1996/03/25 00:52:10 pascale $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        I18nUtil.h
 **
 **
 **  This file contains function definitions for the corresponding .c
 **  file
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#ifndef _i18nUtil_h
#define _i18nUtil_h

#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>


	/* string */

#define  COMMENT_CHAR            '!'
#define  COMMENT_CHAR2           '#'

#define	is_comment_char(c)	((c) == COMMENT_CHAR || (c) == COMMENT_CHAR2)
#define	is_white(c)		((c) == ' ' || (c) == '\t' || (c) == '\n')
#define	skip_white(p)		while (is_white(*(p)))	(p)++
#define	cut_field(p)	\
	for ((p)++; *(p) && !is_white(*(p)); (p)++) ; *(p)++ = 0; skip_white(p)


#endif /* _i18nUtil_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
