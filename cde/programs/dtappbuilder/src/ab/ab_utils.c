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
 *	$XConsortium: ab_utils.c /main/3 1995/11/06 17:13:58 rswiston $
 *
 *	@(#)ab_utils.c	1.27 08 May 1995	
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
 * File: ab_utils.c - file containing ab utilities
 */


#include <sys/param.h>
#include <Xm/Xm.h>
#include <Xm/MessageB.h>
#include <X11/cursorfont.h>
#include <ab_private/ab.h>
#include <ab_private/abobj.h>
#include <ab_private/abobj_set.h>
#include <ab_private/ab_utils.h>
#include <ab_private/pal.h>
#include <ab_private/proj.h>
#include <ab_private/trav.h>
#include <ab_private/obj.h>
#include <ab_private/objxm.h>
#include <ab_private/cgen.h>
#include <ab_private/ui_util.h>
#include "palette_ui.h"
#include "dtb_utils.h"


typedef struct INIT_WIN_POS {
    Widget              ref_widget;
    AB_WPOS_TYPE        type;
} InitWinPos;

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/

static int 	format_dir_name_for_user(
		    STRING	ugly_dir, 
		    STRING	pretty_dir, 
		    int		pretty_dir_size
		);

static int 	format_dir_name_for_system(
		    STRING	user_dir_name,
		    STRING	system_dir,
		    int		system_dir_size
		);

static void 	set_cursor(
		    Widget      widget,
		    Cursor      cursor,
		    BOOL        on
		);

static void     track_win_iconify(
                    Widget      widget,
                    XtPointer   client_data,
                    XEvent      *event,
                    Boolean     *cont_disp
                );
static void     track_win_leader_iconify(
                    Widget      widget, 
                    XtPointer   client_data, 
                    XEvent      *event, 
                    Boolean     *cont_disp 
                );
static void	popdown_winCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );
static void	position_winCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );
static void	track_win_popupCB(
		    Widget	widget,
		    XtPointer	client_data,
		    XtPointer	call_data
		);
static void     track_win_popdownCB(
                    Widget      widget, 
                    XtPointer   client_data, 
                    XtPointer   call_data 
                );
static void     track_win_destroyCB(
                    Widget      widget, 
                    XtPointer   client_data, 
                    XtPointer   call_data 
                );

static void	set_windows_visibility(
		    BOOL	show 
		);
static void	set_window_objects_visibility(
		    BOOL	show 
		);

static void     exit_mboxCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );

static void     destroy_mboxCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );

static void     mwm_close_mboxCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );

/*************************************************************************
**                                                                      **
**       Data     	                                                **
**                                                                      **
**************************************************************************/

static ISTRING  current_dir = NULL;		/* formatted for user */
static ISTRING  raw_current_dir = NULL;		/* full path for system */
static STRING	home_env_var_name = "HOME";

static ABWindow	*AB_win_head = NULL;
static ABWindow	*AB_win_tail = NULL;
static ABWindow *AB_win_leader = NULL;

static int	AB_stack_offset = 50;
static int	AB_frame_title_height = 25;
static int	AB_frame_edge_width = 5;
static int	AB_min_dimension = 20;

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

void
ab_position_window(
    Widget		widget,
    Widget		ref_widget,
    AB_WPOS_TYPE 	pos_type
)
{
    Widget	shell, ref_shell;
    Dimension	shell_w, shell_h, ref_w, ref_h;
    Position	shell_x, shell_y, ref_x, ref_y;
    int		screen_w, screen_h;
    int		delta_w, delta_h;

    if (widget == NULL || ref_widget == NULL ||
	!XtIsRealized(ref_widget))
	return;
    
    shell_w = shell_h = shell_x = shell_y = 0;
    ref_w = ref_h = ref_x = ref_y = 0;

    /* 
     * Get Shell widget-id and geometry of window and its
     * reference-window
     */
    shell = ui_get_ancestor_shell(widget);
    XtVaGetValues(shell,
		XmNwidth,	&shell_w,
		XmNheight,	&shell_h,
		NULL);

    ref_shell = ui_get_ancestor_shell(ref_widget);
    XtVaGetValues(ref_shell,
		XmNx, 		&ref_x,
		XmNy, 		&ref_y,
		XmNwidth, 	&ref_w,
		XmNheight, 	&ref_h,
		NULL);

    /* 
     * Adjust geometry values to accommodate window-frame
     *
     * Also, if this routine is called before a window's contents have
     * been managed, its current available dimensions may be misleadingly
     * small.  If so, print out a warning in debug mode...
     */
    if ((int)shell_w < AB_min_dimension || (int)shell_h < AB_min_dimension)
	util_dprintf(1, "ab_position_window: %s - %dx%d - size under minimum\n",
	    XtName(shell), shell_w, shell_h);

    shell_w += (2 * AB_frame_edge_width);
    shell_h += (AB_frame_title_height + AB_frame_edge_width);

    ref_y -= AB_frame_title_height;
    ref_x -= AB_frame_edge_width;
    ref_w += (2 * AB_frame_edge_width);
    ref_h += (AB_frame_title_height + AB_frame_edge_width);

    screen_w = WidthOfScreen(XtScreen(widget));
    screen_h = HeightOfScreen(XtScreen(widget));

    switch(pos_type)
    {
	case AB_WPOS_TILE_RIGHT:
            shell_y = ref_y;
	    shell_x = ref_x + ref_w;
	    break;

	case AB_WPOS_TILE_LEFT: 
            shell_y = ref_y; 
	    shell_x = ref_x - shell_w;
	    break;

	case AB_WPOS_TILE_HORIZONTAL:
	    shell_y = ref_y;
	    /* Determine side for best fit... */
	    if ((int)ref_x >= (int)(screen_w - (ref_x + ref_w)) ||
		(int)shell_w <= (int)ref_x)
		shell_x = ref_x - shell_w; /* TILE LEFT*/
	    else
		shell_x = ref_x + ref_w;   /* TILE RIGHT */
	    break;

	case AB_WPOS_TILE_ABOVE:
	    shell_x = ref_x;
	    shell_y = ref_y - shell_h;
	    break;

	case AB_WPOS_TILE_BELOW:
            shell_x = ref_x;
            shell_y = ref_y + ref_h; 
	    break;

	case AB_WPOS_TILE_VERTICAL:
	    shell_x = ref_x;
	    /* Determine side for best fit... */
            if ((int)ref_y >= (int)(screen_h - (ref_y + ref_h)) ||
                (int)shell_h <= (int)ref_y)
                shell_y = ref_y - shell_h; /* TILE ABOVE */
            else 
                shell_y = ref_y + ref_h;   /* TILE_BELOW */
            break;

	case AB_WPOS_STACK_DIAGONAL:
	    shell_x = ref_x + AB_stack_offset;
	    shell_y = ref_y + AB_stack_offset;
	    break;

	default:
	case AB_WPOS_STACK_CENTER:
	    delta_w = ref_w - shell_w;
	    shell_x = ref_x + (delta_w/2);
		
	    delta_h = ref_h - shell_h;
	    shell_y = ref_y + (delta_h/2);

	    break;
    }

    XtVaSetValues(shell, 
	XmNx,	shell_x,
	XmNy,	shell_y,
	NULL);
}

BOOL
ab_window_leader_iconified(void)
{
    return(AB_win_leader->state & WindowIconified);
}

void
ab_register_window(
    Widget		widget,
    AB_WIN_TYPE 	type,
    unsigned long 	init_state,
    Widget		init_pos_ref_widget,
    AB_WPOS_TYPE	init_pos_type,
    XtCallbackProc	close_callback,
    XtPointer		close_clientdata
)
{
    ABWindow 	*newwin;
    InitWinPos  *init_pos;
    Widget	shell = ui_get_ancestor_shell(widget);
  
    newwin = (ABWindow*)util_malloc(sizeof(ABWindow));
    newwin->widget = widget;
    newwin->type   = type;
    newwin->state  = init_state;
    newwin->next   = NULL;

    /*
     * Install a Window close callback and ensure the deleteResponse
     * gets set to XmDO_NOTHING (to prevent automatic unmanaging of the
     * shell's first child).
     * Use parameter callback if specified, else use default which just
     * pops the window down.
     */  
    ui_add_window_close_callback(shell,
        close_callback? close_callback : popdown_winCB,
        close_callback? close_clientdata : NULL,
        XmDO_NOTHING);

    if (type == AB_WIN_LEADER)
    {
	/* The Window leader is stored separately */
	if (AB_win_leader == NULL)
	{
	    AB_win_leader = newwin;
	    ui_add_window_iconify_handler(shell,
		track_win_leader_iconify, (XtPointer)newwin);
	    return;
	}
	else
	    util_dprintf(1, "ab_register_window: can only have 1 Window Leader\n");
    }

    /*
     * Append new Win Record onto linked-list 
     */
    if (AB_win_head == NULL) /* first one */
	AB_win_head = AB_win_tail = newwin;
    else
    {
	AB_win_tail->next = newwin;
	AB_win_tail = newwin;
    }
	
    /*
     * Set up handlers to track window behavior
     */
    if (type == AB_WIN_WINDOW)
        ui_add_window_iconify_handler(shell,
                track_win_iconify, (XtPointer)newwin);

    XtAddCallback(shell, XtNpopupCallback, track_win_popupCB,
        (XtPointer)newwin);
    XtAddCallback(shell, XtNpopdownCallback, track_win_popdownCB,
        (XtPointer)newwin);
    XtAddCallback(shell, XtNdestroyCallback, track_win_destroyCB, 
	(XtPointer)newwin);

    /* 
     * Set up general window resources to get popup/popdown to work correctly 
     * We will not be using Motif's method of Managing the first child of the
     * shell to get the window to popup!
     */
    XtSetMappedWhenManaged(shell, False);
    XtManageChild(widget);
    if (init_pos_type != AB_WPOS_UNSPECIFIED)
    {
	init_pos = (InitWinPos*)util_malloc(sizeof(InitWinPos));
	init_pos->ref_widget = init_pos_ref_widget;
	init_pos->type = init_pos_type;
	XtAddCallback(shell, XtNpopupCallback, position_winCB, 
	 	(XtPointer)init_pos);
    }
}

void
ab_show_window(
    Widget	widget
)
{
    ABWindow *win;
 
    for(win = AB_win_head; win != NULL; win = win->next)
    {
        if (win->widget == widget)
	{
	    if (win->state & WindowUp)
	    {
                if (win->state & WindowIconified &&
                    !(AB_win_leader->state & WindowIconified))
                {
		    /* Window is iconified, unmap it so we can change its
		     * initial state and remap it as Opened
		     */
		    ui_win_show(widget, False, XtGrabNone);
            	}
		else /* Window is already Open */
		{
		    ui_win_front(win->widget);
		}
	    }
	    /* Popup Window */
    	    XtSetMappedWhenManaged(ui_get_ancestor_shell(win->widget), True);
	    switch(win->type)
	    {
	    case AB_WIN_MODAL:
		ui_win_show(win->widget, True, XtGrabExclusive);
		break;
	    default:
		ui_win_show(win->widget, True, XtGrabNone);
		break;
	    }
	    return;
	}
    }

    util_dprintf(1, "ab_show_window: %s is not a registered window\n",
            XtName(widget));
}

BOOL
ab_window_is_open(
    Widget	widget
)
{
    ABWindow *win;

    for(win = AB_win_head; win != NULL; win = win->next)
    {
        if (win->widget == widget)
	    return((win->state & WindowUp) && 
		  !(win->state & WindowIconified));
    }
    util_dprintf(1, "ab_window_is_open: %s is not a registered window\n",
	    XtName(widget));

    return False;
}

static void
track_win_leader_iconify(
    Widget      widget,
    XtPointer   client_data,
    XEvent      *event,
    Boolean     *cont_dispatch
)
{
    switch (event->type)
    {
        case MapNotify: /* Leader Window Opened */
            AB_win_leader->state &= (~WindowIconified);
            if (InBuildMode)
	    {
		/*
		** Order is important. Objects need to be first. Modal
		** dialogs can be parented off of main windows. If the
		** parent is not mapped first, the dialog loses it's
		** modality.
		*/
		set_window_objects_visibility(True);
		set_windows_visibility(True);
	    }
            break;

        case UnmapNotify: /* Leader Window Iconified */
            AB_win_leader->state |= WindowIconified;
            if (InBuildMode)
	    {
		set_windows_visibility(False);
		set_window_objects_visibility(False);
	    }
            break;
    }
}

static void
track_win_iconify(
    Widget      widget,
    XtPointer   client_data,
    XEvent      *event,
    Boolean     *cont_disp
)
{
    ABWindow *win = (ABWindow*)client_data;

    switch (event->type)
    {
        case MapNotify:
            if (InBuildMode &&
		win->state & WindowIconified &&
	    	!(AB_win_leader->state & WindowIconified))
            {
                win->state &= (~WindowIconified);
	    	XtVaSetValues(widget, XtNinitialState, NormalState, NULL);
            }
            break;
        case UnmapNotify:
	    /* 
	     * We can detect that the window has been iconified 
	     * (not popped-down)  because the track_win_popdownCB
	     * was not called (so WindowUp is still set).
	     *
	     * Note: XtNiconic is not being used because it doesn't
	     * reflect the current state of the window (Xt bug??)
	     */
            if (InBuildMode &&
		win->state & WindowUp &&
		!(win->state & WindowIconified) &&
                !(AB_win_leader->state & WindowIconified))
            {
                win->state |= WindowIconified;
	    	XtVaSetValues(widget, XtNinitialState, IconicState, NULL);
            }
            break;
    }
}

static void
popdown_winCB(
    Widget      shell,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    ui_win_show(shell, False, XtGrabNone);
}

static void
position_winCB(
    Widget      shell,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    InitWinPos	*init_pos = (InitWinPos*)client_data;
    
    ab_position_window(shell, init_pos->ref_widget, init_pos->type);

    /* We only position the window the first time it is invoked; after
     * this, the user is in control, so remove this callback and destroy
     * the init data.
     */
    XtRemoveCallback(shell, XtNpopupCallback, position_winCB, client_data);
    util_free(init_pos);
}

static void
track_win_popupCB(
    Widget      shell,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    ABWindow *win = (ABWindow*)client_data;
    win->state |= WindowUp;
}
 
static void
track_win_popdownCB(
    Widget      shell,
    XtPointer   client_data, 
    XtPointer   call_data 
) 
{ 
    ABWindow *win = (ABWindow*)client_data; 

    if (InBuildMode &&
        !(AB_win_leader->state & WindowIconified))
    {
        win->state &= (~WindowUp);
	if (win->state & WindowIconified)
	{
	    XtVaSetValues(shell, XtNinitialState, NormalState, NULL);
	    win->state &= (~WindowIconified);
	}
    }
}

static void
track_win_destroyCB(
    Widget      shell,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    ABWindow *win = (ABWindow*)client_data;
    ABWindow *prev = NULL;
    ABWindow *p;

    /* 
     * Adjust linked-list for removal of Window 
     */
    if (AB_win_head == win)
	AB_win_head = win->next;
    else
    {
    	/* Find previous window in linked-list */
    	p = AB_win_head;
    	while (p != win)
    	{
	    prev = p;
	    p = p->next;
    	}
	prev->next = win->next;

        if (AB_win_tail == win)
	    AB_win_tail = prev;
    }

    /*
     * Remove all tracker routines, and free Win record 
     */
    if (win->type == AB_WIN_WINDOW)
	ui_remove_window_iconify_handler(shell, track_win_iconify, 
           (XtPointer)win);
    XtRemoveCallback(shell, XtNpopupCallback, track_win_popupCB, 
	(XtPointer)win);
    XtRemoveCallback(shell, XtNpopdownCallback, track_win_popdownCB, 
	(XtPointer)win);

    util_free(win);

}

static void
set_windows_visibility(
    BOOL	show
)
{
    ABWindow *win;
 
    for(win = AB_win_head; win != NULL; win = win->next)
    {
        if (win->state & WindowUp)
	{
	    switch(win->type)
	    {
	    case AB_WIN_MODAL:
		ui_win_show(win->widget, show, XtGrabExclusive);
		break;

	    default:
		ui_win_show(win->widget, show, XtGrabNone);
		break;
	    }
	}
    }
}

void
ab_takedown_windows(void)
{
    set_windows_visibility(False);
}

void
ab_putback_windows(void)
{
    set_windows_visibility(True);
}

static void
set_window_objects_visibility(
    BOOL	show
)
{
    AB_TRAVERSAL trav;
    ABObj       project = proj_get_project();
    ABObj       winobj;
 
    for (trav_open(&trav, project, AB_TRAV_WINDOWS);
        (winobj = trav_next(&trav)) != NULL; )
    {   
        if (obj_is_base_win(winobj) &&
            obj_has_flag(winobj, MappedFlag))
		ui_win_show(objxm_get_widget(winobj), show, XtGrabNone);
    }
    trav_close(&trav);
}


int
ab_update_stat_region(
    AB_STATUS_INFO type,
    String         value
)
{
    static Widget       obj_type = NULL;
    static Widget       obj_name = NULL;
    static Widget       obj_pos  = NULL;
    static Widget       obj_size = NULL;
    static Widget       curs_pos = NULL;
    static Widget       cur_module = NULL;
    Widget              widget = NULL;
    String              nullstr = "        ";

    switch(type)
    {
        case AB_STATUS_OBJ_TYPE:
            if (obj_type == NULL)
                obj_type = dtb_palette_ab_palette_main.object_type_field;
            widget = obj_type;
            break;
        case AB_STATUS_OBJ_NAME:
            if (obj_name == NULL)
                obj_name = dtb_palette_ab_palette_main.object_name_field;
            widget = obj_name;
            break;
        case AB_STATUS_OBJ_POS:
            if (obj_pos == NULL)
                obj_pos = dtb_palette_ab_palette_main.position_field;
            widget = obj_pos;
            break;
        case AB_STATUS_OBJ_SIZE:
            if (obj_size == NULL)
                obj_size = dtb_palette_ab_palette_main.size_field;
            widget = obj_size;
            break;
        case AB_STATUS_CURS_POS:
            if (curs_pos == NULL)
                curs_pos = dtb_palette_ab_palette_main.cursor_position_field;
            widget = curs_pos;
            break;
        case AB_STATUS_CUR_MODULE:
            if (cur_module == NULL)
                cur_module = dtb_palette_ab_palette_main.editing_module_field;

            widget = cur_module;
	    if (value == NULL)
		value = "( None )";
    }
    if (value == NULL)
        value = nullstr;
 
    /* Update the footer to display the current object-type */
    if (widget != NULL)
        XtVaSetValues(widget,
                XtVaTypedArg, XmNlabelString, XtRString,
                value, strlen(value)+1,
                NULL);
 
    return 0;
}

/*
 * Set the busy-cursor ON or OFF on all AB Windows
 */
void
ab_set_busy_cursor(
    BOOL        on
)
{
    AB_TRAVERSAL trav;
    ABObj       project = proj_get_project();
    ABWindow	*win;
    ABObj       module;
    ABObj       winobj;
    static      Cursor busy_cursor = 0;
    Cursor      new_cursor = 0;
    int         num_wins;
    int         i;

    if (busy_cursor == 0)
        busy_cursor = XCreateFontCursor(XtDisplay(AB_toplevel), XC_watch);

    if (on)
        new_cursor = busy_cursor;

    set_cursor(AB_toplevel, new_cursor, on);

    /*
     * Loop through Windows and set cursor on any that
     * are currently UP
     */  
    for(win = AB_win_head; win != NULL; win = win->next)
    {
        if (win->state & WindowUp)
	    set_cursor(win->widget, new_cursor, on);
    }    
 
    /* Set busy cursor for any AB windows currently up */
    for (trav_open(&trav, project, AB_TRAV_MODULES);
        (module = trav_next(&trav)) != NULL; )
    {
        num_wins = obj_get_num_children(module);
        for (i = 0; i < num_wins; i++)
        {
            winobj = obj_get_child(module, i);
            if (obj_is_window(winobj) && winobj->ui_handle != NULL)
                set_cursor((Widget)winobj->ui_handle, new_cursor, on);
        }
    }
    trav_close(&trav);
}


/*
 * This function is used as a callback for libABobjXm. Currently, it
 * only calls a dtb_ function, but if the dtb_functions change, or
 * we need to do more, here, we won't have to modify libABobjXm.
 */
int	
ab_cvt_image_file_to_pixmap(
			Widget	widget,
			STRING	file_name,
			Pixmap	*pixmap_out
)
{
    return dtb_cvt_image_file_to_pixmap(widget, file_name, pixmap_out);
}

/*
 * This function is the post instantiate callback for libABobjXm. Currently, it
 * only calls the group layout function, but if we need to do more here, 
 * we won't have to modify libABobjXm.
 */
int	
ab_post_instantiate(
			ABObj	obj
)
{
    if (obj && obj_is_group(obj) && (obj_get_group_type(obj) != AB_GROUP_IGNORE))
    {
	Widget		group_w;

        abobj_disable_save_needed();

        abobj_layout_group(obj, TRUE);

	group_w = objxm_get_widget(obj);

	/*
	 * Register expose handler
	 * Some group objects depend on it's members' sizes for their layout.
	 * Unfortunately, some group members have invalid sizes prior to
	 * XtRealize(), so the group layout has to be recalculated after the
	 * group is realized or exposed in this case, since there is no realize 
	 * callback.
	 */
	if (group_w)
	{
	    ABObj	parent = NULL;
	    Widget	parent_w = NULL;

	    /*
	     * Get (root) parent ABObj
	     */
	    parent = obj_get_parent(obj);

	    if (parent)
	        parent = obj_get_root(parent);

	    /*
	     * Get parent widget
	     */
	    if (parent)
		parent_w = objxm_get_widget(parent);

	    if (parent_w)
	    {
		/*
		 * If parent widget is not realized, register expose handler
		 */
		if (!XtIsRealized(parent_w))
		    abobj_register_group_expose_handler(obj);
	    }
	}

        abobj_enable_save_needed();
    }

    return (0);
}


/*
 * Accepts NULL and "" as valid directories ( == "." )
 */
int
ab_change_dir(STRING request_dir)
{
    char        pretty_dir[MAXPATHLEN+1];
    BOOL        dir_changed = TRUE;		/* if path string changed */
    BOOL	force_update = FALSE;
    *pretty_dir = 0;

    /*
     * Determine the new directory
     */  
    if (current_dir == NULL)
    {
        ab_get_cur_dir();               /* initialize it! */
    }
    if (request_dir == NULL)
    {
        dir_changed = FALSE;
	force_update = TRUE;
        request_dir = istr_string(raw_current_dir);
    }
    else
    {   
	/* see if the path we present to the user will change because of
	 * this.
	 */
        if (format_dir_name_for_user(request_dir, pretty_dir, MAXPATHLEN) >= 0)
	{
            dir_changed = !(istr_equalstr(current_dir, pretty_dir));
	}
    }
 
    /*
     * See if any work actually needs to be done
     */
    if ((!dir_changed) && (!force_update))
    {  
        /* no action necessary */
        return 0;
    }
 
    /*
     * Actually change directories and update info
     */
    if (dir_changed)
    {  
	{
	    char	sys_dir[MAXPATHLEN+1];
	    if (format_dir_name_for_system(
				request_dir, sys_dir, MAXPATHLEN+1) >= 0)
	    {
		/*
		 * We may have changed paths but still be in the same actual
		 * directory. Only chdir if we've actually moved.
		 */
		if (!util_paths_are_same_file(".", sys_dir))
		{
                    if (chdir(sys_dir) != 0)
                    {
                        return -1;
		    }
		}
	    }
        }
	if ((*pretty_dir) == 0)		/* we may have done this above */
	{
            format_dir_name_for_user(request_dir, pretty_dir, MAXPATHLEN);
	}
        istr_destroy(raw_current_dir);
        raw_current_dir = istr_create(request_dir);
        istr_destroy(current_dir);
        current_dir = istr_create(pretty_dir);
    }
 
    /*
     * Update other subsystems.
     */
    cgen_notify_new_directory(istr_string(current_dir));
       
    return 0;
}

BOOL
ab_is_cur_dir(STRING dir)
{
    char        pretty_dir[MAXPATHLEN];
    BOOL        is_cur_dir= FALSE;
    *pretty_dir = 0;
         
    if (current_dir == NULL)
    {
        ab_get_cur_dir();       /* initialize it! */
    }
 
    if ((dir == NULL) || (*dir == 0) || (*dir == '.'))
    {
        return TRUE;
    }
 
    format_dir_name_for_user(dir, pretty_dir, MAXPATHLEN);
    is_cur_dir = istr_equalstr(current_dir, pretty_dir);
 
    return is_cur_dir;
}
 
 
STRING
ab_get_cur_dir(void)
{
    static BOOL initialized = FALSE;
    char        formatted_cur_dir[MAXPATHLEN];
    char        cur_dir[MAXPATHLEN];
    STRING      new_raw_dir = NULL;
    STRING      new_dir = NULL;
    *formatted_cur_dir = 0;
    *cur_dir = 0;
 
    if ( initialized &&
         (raw_current_dir != NULL) &&
         util_paths_are_same_file(istr_string(raw_current_dir), ".") )
    {
        goto epilogue;  /* no change */
    }
 
    /*
     * Get the raw current directory
     */
    new_raw_dir = NULL;
    if (!initialized)
    {
        initialized = TRUE;
        new_raw_dir = getenv("PWD");
    }
    if (new_raw_dir == NULL)
    {
        if (getcwd(cur_dir, MAXPATHLEN) != NULL)
        {
            new_raw_dir = cur_dir;
        }
    }
 
    /*
     * Get the pretty version of the current directory
     */
    if (new_raw_dir == NULL)
    {  
        new_raw_dir = ".";
        new_dir = "* Indeterminable *";
    }
    else
    {
        format_dir_name_for_user(new_raw_dir, formatted_cur_dir, MAXPATHLEN);
        new_dir = formatted_cur_dir;
    }
 
    istr_destroy(current_dir);
    istr_destroy(raw_current_dir);
 
    current_dir = istr_create(new_dir);
    raw_current_dir = istr_create(new_raw_dir);
 
epilogue:
    return istr_string(current_dir);
}

/*
 * Takes a directory name and formats it so that it will look good
 * to the user.
 * Guarantees: no trailing slashes (except for / directory)
 */
static int
format_dir_name_for_user(
			STRING	ugly_dir, 
			STRING	pretty_dir, 
			int	pretty_dir_size
)
{
    STRING      home= getenv(home_env_var_name);
    int         home_name_len = 0;
    int         i= 0;
 
    if (home != NULL)
    {
        char    home_relative[MAXPATHLEN];
        home_name_len = strlen(home);
	*home_relative = 0;

        util_cvt_path_to_relative(ugly_dir, home, home_relative, MAXPATHLEN);
        *pretty_dir = 0;
        if (home_relative[0] == '/')
        {
            strncpy(pretty_dir, home_relative, pretty_dir_size);
            pretty_dir[pretty_dir_size-1]= 0;
        }
        else
        {
            /* relative to home (but avoid "~/." if actually in home dir)*/
	    strcpy(pretty_dir, "~");
            if (!util_streq(home_relative, "."))
            {
                strcat(pretty_dir, "/");
                strncat(pretty_dir, home_relative, pretty_dir_size-2);
            }
            pretty_dir[pretty_dir_size-1]= 0;
        }    
    }    
    else
    {
        strncpy(pretty_dir, ugly_dir, pretty_dir_size);
        pretty_dir[pretty_dir_size-1]= 0;
    }
 
    /*
     * strip trailing slashes
     */
    for (i= strlen(pretty_dir)-1;  i > 0; --i)
    {  
        if (pretty_dir[i] == '/')
        {
            pretty_dir[i]= 0;
        }
        else
        {
            break;
        }
    }
    util_dprintf(2,"user format dir '%s' -> '%s'\n", ugly_dir, pretty_dir);
    return 0;
}


/*
 * Takes a directory name that may or may not be formatted to look
 * good to the user, and converts it to a path that will work with
 * chdir().
 */
static int 	
format_dir_name_for_system(
		    STRING	user_dir_name,
		    STRING	system_dir,
		    int		system_dir_size
)
{
    int		return_value = 0;
    char	*user_dir_ptr = user_dir_name;
    STRING	homeVar = NULL;
    *system_dir = 0;
    if (util_strempty(user_dir_name))
    {
	goto epilogue;
    }

    if (user_dir_name[0] == '~')
    {
	if ((homeVar = getenv(home_env_var_name)) != NULL)
	{
	    util_strncpy(system_dir, homeVar, system_dir_size);
	    ++user_dir_ptr;
	}
	if ((*user_dir_ptr) == '/')
	{
	    strcat(system_dir, "/");
	    ++user_dir_ptr;
	}
    }

    strcat(system_dir, user_dir_ptr);

epilogue:
    util_dprintf(2,"sys format dir '%s' -> '%s'\n", user_dir_name, system_dir);
    return return_value;
}


static void
set_cursor(
    Widget      widget,
    Cursor      cursor,
    BOOL        on
)
{
    Window      win;
    Display     *dpy;

    if (widget && XtIsRealized(widget))
    {
        dpy = XtDisplay(widget);
        win = XtWindow(widget);
        if (on)
            XDefineCursor(dpy, win, cursor);
        else
            XUndefineCursor(dpy, win);
    }
}

void
ab_palette_set_active(
    BOOL        active
)
{
    static Widget       palette_cpanel = NULL;
    static Widget       palette_footer = NULL;
    /* REMIND andy: take out when working */

    if (palette_cpanel == NULL)
    {
        palette_cpanel = dtb_palette_ab_palette_main.palette_cpanel;
        palette_footer = dtb_palette_ab_palette_main.palette_footer;
    }

    ui_set_active(palette_cpanel, active);
    ui_set_active(palette_footer, active);
}

void
ab_exit_dtbuilder(void)
{
    static DTB_MODAL_ANSWER     answer;
    XtAppContext                app;
    static BOOL                 Up = False;
    Widget                      dlg = (Widget) NULL;
    Widget                      shell = (Widget) NULL;
    Widget                      mbox = (Widget) NULL;
    Arg                         args[10];
    int                         n = 0;

    answer = DTB_ANSWER_NONE;
    if ( !Up )
    {
        if (proj_check_unsaved_edits(proj_get_project()))
        {
            Up = True;

	    dtb_palette_exit_msg_initialize(&dtb_palette_exit_msg);

            if (ab_window_leader_iconified())
            {
                shell = XtCreatePopupShell("exit_dtbuilder",
                        topLevelShellWidgetClass,
                        dtb_get_toplevel_widget(),
                        (ArgList) NULL, (Cardinal) 0);
 
                n = 0;
                XtSetArg(args[n], XmNdialogType,
                        dtb_palette_exit_msg.type); n++;
                XtSetArg(args[n], XmNmessageString,
                        dtb_palette_exit_msg.message); n++;
                XtSetArg(args[n], XmNdialogTitle,
                        dtb_palette_exit_msg.title); n++;
                XtSetArg(args[n], XmNokLabelString,
                        dtb_palette_exit_msg.action1_label); n++;
                XtSetArg(args[n], XmNdefaultButtonType,
                        XmDIALOG_CANCEL_BUTTON); n++;
                XtSetArg(args[n], XmNuserData, &answer); n++;
                mbox = XmCreateMessageBox(shell, "exit_mbox", args, n);
                XtManageChild(mbox);
 
                XtAddCallback(mbox, XmNokCallback, exit_mboxCB,
                        (XtPointer) DTB_ANSWER_ACTION1);
                XtAddCallback(mbox, XmNcancelCallback, exit_mboxCB,
                        (XtPointer) DTB_ANSWER_CANCEL);
                XtAddCallback(shell, XmNpopdownCallback, destroy_mboxCB,
                        (XtPointer) NULL);
 
                ui_add_window_close_callback(shell, mwm_close_mboxCB,
                        (XtPointer) &answer, XmDO_NOTHING);
         
                XtRealizeWidget(shell);
                XtPopup(shell, XtGrabExclusive);
 
                ui_win_front(shell);
                app = XtDisplayToApplicationContext(XtDisplay(mbox));
                while (answer == DTB_ANSWER_NONE)
                {
                    XtAppProcessEvent(app, XtIMAll);
                }
            }
            else if (shell == NULL)
            {
                /* Display the "unsaved edits" message DIALOG if the
                 * "unsaved edits" message topLevelShell is not already
                 * up.
                 */
                answer = dtb_show_modal_message(dtb_get_toplevel_widget(),
                        &dtb_palette_exit_msg, NULL, NULL, &dlg);
            }
 
            switch (answer)
            {
                case DTB_ANSWER_ACTION1:
                    exit(0);
                case DTB_ANSWER_CANCEL:
                    break;
            }
            Up = False;
        }    
        else
        {
            exit(0);
        }
    }
    else
    {
        if (dlg != NULL)
	    ui_win_front(dlg);
        else if (shell != NULL)
	    ui_win_front(shell);
    }
}

static void
exit_mboxCB(
    Widget      widget,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    DTB_MODAL_ANSWER     op = (DTB_MODAL_ANSWER) client_data;
    DTB_MODAL_ANSWER     *answerp = NULL;

    XtVaGetValues(widget, XmNuserData, &answerp, NULL);

    /* Dismiss the "unsaved edits" message. */
    XtPopdown(ui_get_ancestor_shell(widget));

    *answerp = op;
}

/*
 * popdownCallback for Exit MessageBox.
 */
static void
destroy_mboxCB(
    Widget      widget,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    XtDestroyWidget(widget);
}

static void
mwm_close_mboxCB(
    Widget      widget,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    DTB_MODAL_ANSWER     *op = (DTB_MODAL_ANSWER *) client_data;

    /* Dismiss the "unsaved edits" message. */
    XtPopdown(widget);

    *op = DTB_ANSWER_CANCEL;
}
