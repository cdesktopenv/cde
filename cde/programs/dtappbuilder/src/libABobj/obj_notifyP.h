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
 *	$XConsortium: obj_notifyP.h /main/3 1995/11/06 18:38:04 rswiston $
 *
 *	@(#)obj_notifyP.h	1.24 02 Feb 1995	
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
 * obj_notifyP.h - object change notification
 *
  * Internal files - defines internal data structures, et cetera
 */
#ifndef _ABOBJ_NOTIFYP_H_
#define _ABOBJ_NOTIFYP_H_

#include "objP.h"		/* include before obj.h! */
#include <ab_private/obj.h>
#include <ab_private/obj_notify.h>

/*
 * return codes (supplement to util_err.h)
 */
#define OBJ_NOTIFY_BATCHED (100)	/* not an error! */


typedef enum
{
    OBJEV_UNDEF= 0,
    OBJEV_ALLOW_GEOMETRY_CHANGE,
    OBJEV_ATT_CHANGE,
    OBJEV_ALLOW_REPARENT,
    OBJEV_CREATE,
    OBJEV_DESTROY,
    OBJEV_REPARENT,
    OBJEV_UPDATE,
    OBJEV_UPDATE_WITH_DATA,
    OBJ_EVENT_TYPE_NUM_VALUES
} OBJ_EVENT_TYPE;


typedef BYTE OBJ_EVENT_MODE;
#define OBJEV_MODE_SEND_NOTIFY_EVS	((OBJ_EVENT_MODE)0x01)
#define OBJEV_MODE_SEND_ALLOW_EVS	((OBJ_EVENT_MODE)0x02)
#define OBJEV_MODE_BATCH_NOTIFY_EVS	((OBJ_EVENT_MODE)0x04)
#define OBJEV_MODE_DISALLOW_ALL_EVS	((OBJ_EVENT_MODE)0x08)
#define OBJEV_MODE_NORMAL \
		(OBJEV_MODE_SEND_NOTIFY_EVS | OBJEV_MODE_SEND_ALLOW_EVS)

typedef union
{
    OBJEV_ALLOW_GEOMETRY_CHANGE_INFO	allow_geometry_change;
    OBJEV_ALLOW_REPARENT_INFO		allow_reparent;
    OBJEV_ATT_CHANGE_INFO		att_change;
    OBJEV_CREATE_INFO			create;
    OBJEV_DESTROY_INFO			destroy_info;	/* destroy is C++ key*/
    OBJEV_REPARENT_INFO			reparent;
    OBJEV_UPDATE_INFO			update;
    OBJEV_UPDATE_WITH_DATA_INFO		update_with_data;
} OBJ_EVENT_EXTRA_INFO;


typedef struct
{
    OBJ_EVENT_TYPE		type;
    OBJ_EVENT_EXTRA_INFO	info;
} OBJ_EVENT, *ObjEvent;


int		objP_notify_push_mode(void);	/* push current set of modes */
int		objP_notify_set_mode(OBJ_EVENT_MODE);	/* set a mode */
int		objP_notify_clear_mode(OBJ_EVENT_MODE); /* clear a mode */
BOOL		objP_notify_mode_is_set(OBJ_EVENT_MODE); /* query mode */
int		objP_notify_pop_mode(void);	/*pop to most-recently pushed*/
OBJ_EVENT_MODE	objP_notify_get_mode(void);
int		objP_notify_get_num_queued(void);
BOOL		objP_notify_event_will_queue(void);

/*
 * These functions send or queue an event to be sent
 */
int		objP_notify_send_create(ABObj);
int		objP_notify_send_destroy(ABObj);
int		objP_notify_send_allow_geometry_change(ABObj,
			int new_x, int new_y, int new_width, int new_height);
int		objP_notify_send_allow_reparent(ABObj, ABObj new_parent);
int		objP_notify_send_att_change(ABObj, OBJEV_ATT_FLAGS atts);
int		objP_notify_send_geometry_change(ABObj,
			int old_x, int old_y, int old_width, int old_height);
int		objP_notify_send_rc_geometry_change(ABObj);
int		objP_notify_send_name_change(ABObj, ISTRING old_name);
int		objP_notify_send_reparent(ABObj, ABObj old_parent);
int             objP_notify_send_selected_change(ABObj);
int		objP_notify_send_update(ABObj, BOOL update_subtree);

int		objP_notify_send_update_with_data(
			ABObj	obj,
			BOOL	update_subtree,
			int	update_code,
			void	*update_data,
			UpdateDataFreeFunc update_data_free_func
			);


/*************************************************************************
**									**
**	Inline implementation						**
**									**
*************************************************************************/

#define objP_notify_mode_is_set(mode) ((objP_notify_get_mode() & (mode)) != 0)

#endif /* _ABOBJ_NOTIFYP_H_ */

