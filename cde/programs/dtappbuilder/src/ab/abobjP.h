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
 *      $XConsortium: abobjP.h /main/3 1995/11/06 17:14:46 rswiston $
 *
 * @(#)abobjP.h	1.17 15 Feb 1994      cde_app_builder/src/ab
 *
 *      RESTRICTED CONFIDENTIAL INFORMATION:
 *
 *      The information in this document is subject to special
 *      restrictions in a confidential disclosure agreement between
 *      HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *      document outside HP, IBM, Sun, USL, SCO, or Univel without
 *      Sun's specific written approval.  This document and all copies
 *      and derivative works thereof must be returned or destroyed at
 *      Sun's request.
 *
 *      Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */

/*
 * abobjP.h -  
 */
#ifndef _ABOBJ_P_H_
#define _ABOBJ_P_H_


#if !defined(CSRG_BASED)
#include <values.h>
#endif

#include <ab_private/abobj.h>
#include <ab_private/abobj_set.h>

#ifndef MAXINT
#define MAXINT          (0x7FFFFFFF)
#endif /* MAXINT */
#define XSORT           (1)
#define YSORT           (2)

/*
 * Resize directions.
 */
typedef enum {
    NONE,
    MOVE,
    NORTH,
    SOUTH,
    EAST,
    WEST,
    NORTH_EAST,
    NORTH_WEST,
    SOUTH_EAST,
    SOUTH_WEST,
    RESIZE_DIR_NUM_VALUES
    /* ANSI: no comma after last enum item! */
} RESIZE_DIR;

typedef enum {
        LEFT_EDGES      = (1L << 0),
        VCENTERS        = (1L << 1),
        RIGHT_EDGES     = (1L << 2),
        LABELS          = (1L << 3),
        TOP_EDGES       = (1L << 4),
        HCENTERS        = (1L << 5),
        BOTTOM_EDGES    = (1L << 6),
        TO_GRID         = (1L << 7),
        VSPACING        = (1L << 8),
        HSPACING        = (1L << 9),
        FIXEDROWS       = (1L << 10),
        FIXEDCOLS       = (1L << 12),
        ROWFIRST        = (1L << 13),
        COLFIRST        = (1L << 14),
        HCENTERED       = (1L << 15),
        VCENTERED       = (1L << 16)
        /* ANSI: no comma after last enum item! */
} AB_ALIGN_TYPES;

/*
 * Translation Table storage
 */
typedef struct _AB_TRANS_TBL
{
    WidgetClass    wclass;
    XtTranslations translations;

} ABTransTbl;

/*
 * Object Menu Cache structure
 */
typedef struct _WIN_MENU_CACHE
{
    ABObj	winobj;
    Widget	menu;
} WinMenuCache;

/*
 * Browser Menu Cache structure
 */
typedef struct _BROWSER_MENU_CACHE
{
    Widget	brws_widget;
    Widget	menu;
} BrowserMenuCache;


/*
 * EVENT-HANDLING for AB objects
 */
extern void    	abobjP_enable_build_actions(
                    ABObj obj,
                    Widget widget
        	);

extern void    	abobjP_disable_build_actions(
                    ABObj obj,
                    Widget widget
        	);

/*
 * SELECT AB object functions
 */

/*
 * MOVE AB object functions
 */
extern int      abobjP_move_object_outline(
                    ABObj        obj,
                    XMotionEvent *mevent
                );

/*
 * RESIZE AB object functions
 */
extern Cursor   abobjP_get_resize_cursor(
                     Widget      w,
                     RESIZE_DIR  dir
                );   

extern RESIZE_DIR  abobjP_find_resize_direction(
                     ABObj           obj,
                     Widget          w,
                     XEvent          *event
                );   
 
extern int      abobjP_resize_object_outline(
                     ABObj        obj,
                     XEvent          *event,
                     RESIZE_DIR   dir
                );   

extern void     abobjP_track_external_resizes(
                     Widget      widget,
                     XtPointer   client_data,
                     XEvent *event,
                     Boolean     cont_to_dispatch
                );


#endif /* _ABOBJ_P_H_ */

