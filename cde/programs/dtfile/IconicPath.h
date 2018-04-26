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
/* $XConsortium: IconicPath.h /main/4 1995/11/02 14:40:49 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           IconicPath.h
 *
 *   COMPONENT_NAME: Desktop File Manager
 *
 *   DESCRIPTION:    Definitions used in the IconicPath.c
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _DtIconicPath_h
#define _DtIconicPath_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Class record constants */

externalref WidgetClass dtIconicPathWidgetClass;

typedef struct _DtIconicPathClassRec * DtIconicPathWidgetClass;
typedef struct _DtIconicPathRec      * DtIconicPathWidget;


#ifndef XmIsIconicPath
#define XmIsIconicPath(w)  (XtIsSubclass (w, dtIconicPathWidgetClass))
#endif

/* New resources */
#define DtNsmallMinWidth	"smallMinWidth"
#define DtNlargeMinWidth	"largeMinWidth"
#define DtNforceSmallIcons	"forceSmallIcons"
#define DtCForceSmallIcons	"ForceSmallIcons"
#define DtNforceLargeIcons	"forceLargeIcons"
#define DtCForceLargeIcons	"ForceLargeIcons"
#define DtNfileMgrRec		"fileMgrRec"
#define DtCfileMgrRec		"FileMgrRec"
#define DtNcurrentDirectory	"currentDirectory"
#define DtCCurrentDirectory	"CurrentDirectory"
#define DtNlargeIcons		"largeIcons"
#define DtCLargeIcons		"LargeIcons"
#define DtNiconsChanged		"iconsChanged"
#define DtCIconsChanged		"IconsChanged"

/********    Public Function Declarations    ********/

extern Widget _DtCreateIconicPath(
                        Widget p,
                        String name,
                        ArgList args,
                        Cardinal n) ;

extern void DtUpdateIconicPath(
			FileMgrRec *file_mgr_rec,
			FileMgrData *file_mgr_data,
			Boolean icons_changed) ;
/********    End Public Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _DtIconicPath_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
