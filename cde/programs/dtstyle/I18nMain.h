/* $XConsortium: I18nMain.h /main/1 1996/03/25 00:51:57 pascale $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        I18nMain.h
 **
 **
 **  This file contains function definitions for the corresponding .c
 **  file
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

#ifndef _i18nMain_h
#define _i18nMain_h


/* External Interface */

extern void popup_i18nBB( Widget shell) ;
extern void restoreI18n( Widget shell, XrmDatabase db) ;
extern void saveI18n( int fd) ;


#endif /* _i18nMain_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
