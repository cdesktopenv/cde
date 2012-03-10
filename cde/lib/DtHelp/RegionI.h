/* $XConsortium: RegionI.h /main/3 1996/05/09 03:44:34 drk $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        RegionI.h
 **
 **   Project:     Cde Help System
 **
 **   Description: Defines the Region structure.
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef	_DtHelpRegionI_h
#define	_DtHelpRegionI_h

/********    Public Enum Declarations    ********/

enum	_dtHelpDARegType
    {
      _DtHelpDAGraphic,
      _DtHelpDASpc
    };

typedef enum _dtHelpDARegType _DtHelpDARegType;

/********    Public Structure Declarations    ********/

typedef	struct _dtHelpDARegion {
	short			inited;
	_DtHelpDARegType	type;
	_DtCvPointer		handle;
} _DtHelpDARegion;

typedef	struct _dtHelpDASpcInfo {
	char			*name;
	_DtHelpFontHints	 spc_fonts;
} _DtHelpDASpcInfo;

#endif /* _DtHelpRegionI_h */
