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
 *	$XConsortium: obj_notify.h /main/3 1995/11/06 18:37:49 rswiston $
 *
 *	@(#)obj_notify.h	1.14 02 Feb 1995	
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
 * obj_notify.h - object change notification
 */
#ifndef _ABOBJ_NOTIFY_H_
#define _ABOBJ_NOTIFY_H_

#include <ab_private/obj.h>

typedef unsigned char OBJEV_ATT_FLAGS;
#define OBJEV_ATT_UNDEF		((OBJEV_ATT_FLAGS)0x00)
#define OBJEV_ATT_NONE		((OBJEV_ATT_FLAGS)0x00)
#define OBJEV_ATT_GEOMETRY	((OBJEV_ATT_FLAGS)0x01)
#define OBJEV_ATT_HSCROLL	((OBJEV_ATT_FLAGS)0x02)
#define OBJEV_ATT_LABEL		((OBJEV_ATT_FLAGS)0x04)
#define OBJEV_ATT_NAME		((OBJEV_ATT_FLAGS)0x08)
#define OBJEV_ATT_POSITION	((OBJEV_ATT_FLAGS)0x10)
#define OBJEV_ATT_SELECTED	((OBJEV_ATT_FLAGS)0x20)
#define OBJEV_ATT_SIZE		((OBJEV_ATT_FLAGS)0x40)
#define OBJEV_ATT_VSCROLL       ((OBJEV_ATT_FLAGS)0x80)

typedef struct
{
    ABObj		obj;
} OBJEV_STANDARD_INFO;

typedef struct
{
    ABObj	obj;
    int		new_x, new_y, new_width, new_height;
} OBJEV_ALLOW_GEOMETRY_CHANGE_INFO;

typedef struct
{
    ABObj	obj;
    ABObj	new_parent;
} OBJEV_ALLOW_REPARENT_INFO;

typedef struct
{
    ABObj		obj;
    OBJEV_ATT_FLAGS	atts;
    ISTRING		old_name;
} OBJEV_ATT_CHANGE_INFO;

typedef struct
{
    ABObj	obj;
    ABObj	old_parent;
} OBJEV_REPARENT_INFO;

typedef struct
{
    ABObj	obj;
    BOOL	update_subtree;
} OBJEV_UPDATE_INFO;

typedef struct
{
    ABObj		obj;
    BOOL		update_subtree;
    int			update_code;
    void		*update_data;
    UpdateDataFreeFunc	update_data_free_func;
} OBJEV_UPDATE_WITH_DATA_INFO;

typedef OBJEV_STANDARD_INFO	OBJEV_CREATE_INFO;
typedef OBJEV_STANDARD_INFO	OBJEV_DESTROY_INFO;

typedef OBJEV_ALLOW_GEOMETRY_CHANGE_INFO *ObjEvAllowGeometryChangeInfo;
typedef OBJEV_ALLOW_REPARENT_INFO	*ObjEvAllowReparentInfo;
typedef OBJEV_ATT_CHANGE_INFO		*ObjEvAttChangeInfo;
typedef OBJEV_CREATE_INFO		*ObjEvCreateInfo;
typedef OBJEV_DESTROY_INFO		*ObjEvDestroyInfo;
typedef OBJEV_REPARENT_INFO		*ObjEvReparentInfo;
typedef OBJEV_UPDATE_INFO		*ObjEvUpdateInfo;
typedef OBJEV_UPDATE_WITH_DATA_INFO	*ObjEvUpdateWithDataInfo;


/*
 * Callback function types.
 * Must return >= 0 for success, or value from util_err.h
 */
typedef int (*ObjAllowGeometryChangeCallback)	(ObjEvAllowGeometryChangeInfo);
typedef int (*ObjAllowReparentCallback)		(ObjEvAllowReparentInfo);
typedef int (*ObjAttChangeCallback)		(ObjEvAttChangeInfo);
typedef int (*ObjCreateCallback)		(ObjEvCreateInfo);
typedef int (*ObjDestroyCallback)		(ObjEvDestroyInfo);
typedef int (*ObjReparentCallback)		(ObjEvReparentInfo);
typedef int (*ObjUpdateCallback)		(ObjEvUpdateInfo);
typedef int (*ObjUpdateWithDataCallback)	(ObjEvUpdateWithDataInfo);


/*
 * The debug_name parameter is used for error reporting, and may
 * be NULL.
 */
int obj_add_allow_geometry_change_callback(
			ObjAllowGeometryChangeCallback,
			STRING debug_name
			);
int obj_add_allow_reparent_callback(
			ObjAllowReparentCallback,
			STRING debug_name
			);
int obj_add_create_callback(
			ObjCreateCallback,
			STRING debug_name
			);
int obj_add_destroy_callback(
			ObjDestroyCallback,
			STRING debug_name
			);
int obj_add_geometry_change_callback(
			ObjAttChangeCallback,
			STRING debug_name
			);
int obj_add_rename_callback(
			ObjAttChangeCallback,
			STRING debug_name
			);
int obj_add_reparent_callback(
			ObjReparentCallback,
			STRING debug_name
			);
int obj_add_selected_change_callback(
			ObjAttChangeCallback,
			STRING debug_name
			);
int obj_add_update_callback(
			ObjUpdateCallback,
			STRING debug_name
			);
int obj_add_update_with_data_callback(
			ObjUpdateWithDataCallback,
			STRING debug_name
			);

#endif /* _ABOBJ_NOTIFY_H_ */

