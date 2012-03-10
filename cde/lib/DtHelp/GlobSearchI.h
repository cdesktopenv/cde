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
