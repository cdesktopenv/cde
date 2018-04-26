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
/* $XConsortium: xmcvt.h /main/4 1995/11/01 15:58:38 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        xmcvt.h
 **
 **   Project:     CDE
 **
 **   Description: Public include file for xmcvt.c
 **
 **
 **   (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992
 **       by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _Dtksh_xmcvt_h
#define _Dtksh_xmcvt_h

#include <Tt/tttk.h>

extern struct named_integer NI_TT_STATUS[];



extern char * XmStringToString( 
                        XmString string) ;
extern void DtkshCvtXmStringToString( 
                        Display *dpy,
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval,
                        XtPointer data) ;
extern void DtkshCvtKeySymToString( 
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval) ;
extern void DtkshCvtListItemsToString( 
                        Display *dpy,
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval,
                        XtPointer data) ;
extern void DtkshCvtNamedValueToString( 
                        XrmValue *args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval) ;
extern void DtkshCvtStringToNamedValue( 
                        XrmValue *args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval) ;
extern void DtkshRegisterNamedIntConverters( void ) ;
extern void DtkshCvtXEModifierStateToString( 
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval) ;
extern void DtkshCvtXEMotionHintToString( 
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval) ;
extern void DtkshCvtTtStatusToString(
			Tt_status ttStatus,
			XrmValue *pToVal);
extern void DtkshCvtStringToTtStatus(
			char *pString,
			XrmValue *pToVal);
extern void DtkshCvtStringToBool(
			char *pString,
			XrmValue *pToVal);
extern void DtkshCvtStringToTtOp(
			char *pString,
			XrmValue *pToVal);
extern void DtkshCvtTtOpToString(
			Tttk_op op,
			XrmValue *pToVal);
extern void DtkshCvtStringToTtScope(
			char *pString,
			XrmValue *pToVal);


/********    Private Function Declarations    ********/

extern char * _CvtXmStringTableToString( 
                        XmStringTable stringtable,
                        int itemCount) ;



#endif /* _Dtksh_xmcvt_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
