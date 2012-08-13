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
 *      $XConsortium: connP.h /main/3 1995/11/06 17:25:39 rswiston $
 *
 * @(#)connP.h	1.12 17 Feb 1994      cde_app_builder/src/libABobj
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

#ifndef _CONN_P_H_
#define _CONN_P_H_
/*
 * connP.h -
 */
#include <X11/Intrinsic.h>
#include <ab_private/obj.h>
#include <ab_private/obj_notify.h>
#include <ab_private/conn.h>
#include "conn_ui.h"

#define FUNCTION_LABEL	"Function:"
#define ARGUMENT_LABEL	"Argument:"
#define CODE_LABEL	"Code:"

extern Widget	ConnP_dialog;

typedef enum {
    ACTION_TYPE_PREDEFINED = 0,
    ACTION_TYPE_CALLFUNC,
    ACTION_TYPE_EXECUTE_CODE,
    ACTION_TYPE_ON_ITEM_HELP,
    ACTION_TYPE_HELP_VOLUME,
    ACTION_TYPE_NUM_VALUES
} ACTION_TYPE;

typedef struct _ConnWhenRec
{
    AB_WHEN		when_type;
    char		*label;
} ConnWhen;

typedef struct _ConnActRec
{
    AB_BUILTIN_ACTION	act_type;
    char		*label;
} ConnAct;

typedef struct _ConnObjRec
{
    AB_OBJECT_TYPE	obj_type;
    int			obj_subtype;
    char		*label;
    AB_WHEN		*when_list;
    Cardinal		num_whens;
    AB_WHEN		default_when;
    AB_BUILTIN_ACTION	*action_list;
    Cardinal		num_actions;
    AB_BUILTIN_ACTION	default_act;
} ConnObj;


extern ConnObj		ConnP_conn_objs[];
extern int		ConnP_num_conn_objs;

extern ConnWhen		*ConnP_conn_whens;
extern int		ConnP_num_conn_whens;

extern ConnAct		*ConnP_conn_acts;
extern int		ConnP_num_conn_acts;


extern char **		connP_get_when_labels(
			    int	*num_labels
			);
extern char **		connP_get_act_labels(
			    int	*num_labels
			);
extern long		connP_get_obj_type_index(
			    AB_OBJECT_TYPE	ab_type,
			    int			ab_subtype
			);
extern STRING		connP_get_moduled_name(
			    ABObj	obj
			);
extern char		*connP_make_conn_string(
			    ABObj	action_obj
			);
extern ABObj		connP_get_source(
			    void
			);
extern ABObj		connP_get_target(
			    void
			);
extern ABObj		connP_get_connection(
			    void
			);
extern void		connP_set_connection(
			    ABObj	action
			);
extern void		connP_set_source_type(
			    AB_OBJECT_TYPE	obj_type,
			    int			obj_subtype
			);
extern void		connP_set_target_type(
			    AB_OBJECT_TYPE	obj_type,
			    int			obj_subtype
			);
extern AB_OBJECT_TYPE	connP_get_source_type(
			    void
			);
extern int		connP_get_source_subtype(
			    void
			);
extern AB_OBJECT_TYPE	connP_get_target_type(
			    void
			);
extern int		connP_get_target_subtype(
			    void
			);
extern void		connP_set_action_type(
			    AB_FUNC_TYPE	action_type
			);
extern AB_FUNC_TYPE	connP_get_action_type(
			    void
			);
extern void		connP_ui_source_type_update(
			    AB_OBJECT_TYPE	new_type,
			    int			new_subtype
			);
extern void		connP_ui_target_type_update(
			    AB_OBJECT_TYPE	new_type,
			    int			new_subtype
			);
extern AB_ARG_TYPE	connP_action_needs_arg(
			    AB_BUILTIN_ACTION	act_type
			);

extern void		connP_guess_when_action(
			    AB_OBJECT_TYPE	source_type,
			    int			source_subtype,
			    AB_OBJECT_TYPE	target_type,
			    int			target_subtype,
			    AB_WHEN		*when_ret,
			    AB_FUNC_TYPE	*func_type_ret,
			    AB_BUILTIN_ACTION	*act_ret
			);
extern int		connP_objtype_needs_subtype(
			    AB_OBJECT_TYPE	obj_type,
			    int			obj_subtype
			);
extern long		connP_obj_enabled(
			    AB_OBJECT_TYPE	obj_type,
			    int			obj_subtype
			);
extern void		connP_set_conn_arg(
			    ABObj		ab_action,
			    AB_ARG_TYPE		arg_type,
			    char		*str_value
			);
extern void		connP_make_builtin_conn(
			    ABObj		ab_action,
			    ABObj		c_source,
			    ABObj		c_target,
			    AB_WHEN		c_when,
			    AB_BUILTIN_ACTION	c_act
			);
extern void		connP_make_user_def_conn(
			    ABObj		ab_action,
			    ABObj		c_source,
			    AB_WHEN		c_when,
			    char		*function_name
			);
extern void		connP_make_code_frag_conn(
			    ABObj		ab_action,
			    ABObj		c_source,
			    AB_WHEN		c_when,
			    char		*code_frag
			);
extern void 		connP_make_on_item_help_conn(
			    ABObj               ab_action,
			    ABObj               c_source,
			    AB_WHEN             c_when
			);
extern void 		connP_make_help_vol_conn(
			    ABObj               ab_action,
			    ABObj               c_source,
			    AB_WHEN             c_when,
			    STRING              volume,
			    STRING              location
			);
extern int		connP_update_on_obj_destroy(
			    ObjEvDestroyInfo destroyInfo
			);
extern int		connP_update_on_obj_rename(
			    ObjEvAttChangeInfo    info
			);
extern int		connP_update_on_obj_reparent(
			    ObjEvReparentInfo    info
			);
extern int		connP_update_on_show_status(
			    ObjEvUpdateInfo     info
			);

extern BOOL		connP_change_in_builtin(
    			    ABObj	ab_action,
    			    ABObj	c_source,
    			    ABObj	c_target,
    			    AB_WHEN	c_when,
    			    AB_BUILTIN_ACTION	c_act,
    			    char	*arg_str
			);

extern void		connP_builtin_remove_arg(
    			    ABObj	ab_action
			);

extern void		connP_destroy_connections_for(
			    ABObj	obj
			);

extern BOOL		connP_conn_is_possible(
			    void
			);


extern BOOL 		allow_show_help_connection(
			    ABObj 	src_obj, 
			    ABObj 	target_obj
			);

extern void 		connP_update_when_menu(
			    Widget	widget,
			    XtPointer 	clientData,
			    XtPointer 	callData
			);

extern void 		connP_update_action_menu(
			    Widget 	widget,
			    XtPointer 	clientData,
			    XtPointer 	callData
			);

extern BOOL 		connP_obj_part_of_conn(
			    ABObj       obj,
			    ABObj       conn_obj
			);

#endif /* _CONN_P_H_ */
