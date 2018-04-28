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
 *	$XConsortium: tmodeP.h /main/3 1995/11/06 17:55:14 rswiston $
 *
 * @(#)tmodeP.h	1.1 15 Jan 1995
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

/* 
** File: tmodep.h - Common Test Mode functionality
*/

#ifndef _AB_TMODEP_H
#define _AB_TMODEP_H

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <ab_private/AB.h>
#include <ab_private/obj.h>
#include <ab_private/obj_list.h>

/*
 * Constants
 */
#define TestModeFlagsNone		((TestModeFlags)0x0000)
#define	TestModeTestProject		((TestModeFlags)0x0001)	/* stored on project obj */
#define	TestModeFlagDirtyWindow		((TestModeFlags)0x0002)	/* per window */
#define	TestModeFlagResizedWindow	((TestModeFlags)0x0004)	/* per window */

/*
 * Data Structures
 */
typedef struct TEST_MODE_ANY_DATA_REC	/* name must match obj.h! */
{
    XRectangle rect;		/* win dimension. saved on entry, compaired with on exit */
} TestModeAnyDataRec;

typedef TestModeAnyDataRec	TestModeDataRec;
typedef TestModeAnyData		TestModeData;


/*
 * Test Mode Flags manipulation
 */
#define tmodeP_obj_flags(obj) ((obj)->test_mode_flags)

#define tmodeP_obj_has_flags(obj, _flags)  \
		((tmodeP_obj_flags(obj) & ((TestModeFlags)(_flags))) != 0)

#define tmodeP_obj_set_flags(obj, _flags)  \
		((obj)->test_mode_flags |= ((TestModeFlags)(_flags)))

#define tmodeP_obj_clear_flags(obj, _flags) \
		(tmodeP_obj_flags(obj) &= ~((TestModeFlags)(_flags)))

#define tmodeP_obj_construct_flags(obj)  \
		((obj)->test_mode_flags = TestModeFlagsNone)


/*
 * Test Mode Data manipulation
 */
#define tmodeP_obj_data(obj) \
	        (obj->test_mode_data)

#define tmodeP_obj_has_data(obj) \
	        ((obj)->test_mode_data != NULL)

extern int      tmodeP_obj_create_data(
                    ABObj             obj
                );
extern int      tmodeP_obj_destroy_data(
                    ABObj              obj
                );
extern int      tmodeP_obj_construct_data(
                    ABObj              obj
                );

/*
 * Geometry
 */
#define tmodeP_obj_geometry(obj) \
                (tmodeP_obj_data(obj)->rect)

#define tmodeP_obj_set_geometry(obj, _rect) \
                (tmodeP_obj_geometry(obj) = _rect)

#define tmodeP_obj_get_width(obj) \
                (tmodeP_obj_data(obj)->rect.width)

#define tmodeP_obj_set_width(obj, _width) \
                (tmodeP_obj_data(obj)->rect.width = _width)

#define tmodeP_obj_get_height(obj) \
                (tmodeP_obj_data(obj)->rect.height)

#define tmodeP_obj_set_height(obj, _height) \
                (tmodeP_obj_data(obj)->rect.height = _height)

#define tmodeP_obj_get_x(obj) \
                (tmodeP_obj_data(obj)->rect.x)

#define tmodeP_obj_set_x(obj, _x) \
                (tmodeP_obj_data(obj)->rect.x = _x)

#define tmodeP_obj_get_y(obj) \
                (tmodeP_obj_data(obj)->rect.y)
    
#define tmodeP_obj_set_y(obj, _y) \
                (tmodeP_obj_data(obj)->rect.y = _y)

/*
 * Window List manipulation
 */
extern int      tmodeP_window_list_create(
                    ABObj              project
                );
extern int      tmodeP_window_list_construct(
                    ABObj              project
                );
extern int      tmodeP_window_list_destruct(
                    void
                );
extern int      tmodeP_window_list_destroy(
                    void
                );
extern void    tmodeP_window_list_iterate(
                    ABObjListIterFn    fn
                );
extern int      tmodeP_window_list_add_handler(
                    EventMask          event_mask,
                    Boolean            nonmaskable,
                    XtEventHandler     event_handler
                );
extern int      tmodeP_window_list_remove_handler(
                    EventMask          event_mask,
                    Boolean            nonmaskable,
                    XtEventHandler     event_handler
                );

#endif /* _AB_TMODEP_H */
