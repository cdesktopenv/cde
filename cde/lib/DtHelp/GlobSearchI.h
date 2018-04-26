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
/* $XConsortium: GlobSearchI.h /main/4 1995/10/26 12:22:45 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        GlobSearchI.h
 **
 **   Project:     DtHelp Project
 **
 **   Description: Builds and displays an instance of a DtHelp GlobSearch
 **                Dialog.
 ** 
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
#ifndef _GlobSearchI_h
#define _GlobSearchI_h

/* global var */
extern char _DtHelpDefaultSrchHitPrefixFont[];

void _DtHelpGlobSrchDisplayDialog(
    Widget owner,
    char * searchWord,
    char * curVolume);
void _DtHelpGlobSrchUpdateCurVol(
    Widget widget);
void  _DtHelpGlobSrchCleanAndClose(
       _DtHelpGlobSearchStuff * srch,
       Boolean               destroy);
void  _DtHelpGlobSrchInitVars(
       _DtHelpGlobSearchStuff * srch);

#endif /* _GlobSearchI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
