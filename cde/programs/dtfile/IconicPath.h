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
