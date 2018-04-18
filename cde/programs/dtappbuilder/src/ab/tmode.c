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
 *	$XConsortium: tmode.c /main/3 1995/11/06 17:54:31 rswiston $
 *
 * @(#)tmode.c	1.2 17 Jan 1995 cde_app_builder/src/ab
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
** File: tmode.c - *New* Test Mode interface
*/

#include "tmode.h"
#include "tmodeP.h"
#include <Xm/DialogS.h>
#include <ab_private/objxm.h>
#include <ab_private/trav.h>
#include <ab_private/x_util.h>
#include <ab_private/conn.h>

#ifdef DEBUG	/* performance testing */
#include <sys/times.h>
typedef struct
{
    double	startSeconds;
    double	endSeconds;
    double	elapsedSeconds;
} ABTimedIntervalRec, *ABTimedInterval;

typedef struct
{
    ABTimedIntervalRec	realTime;
    ABTimedIntervalRec	CPUTime;
} ABProfiledIntervalRec, *ABProfiledInterval;

static int get_cur_times(double *realTimeOut, double *cpuTimeOut);
static int get_start_times(ABProfiledInterval);
static int get_end_times(ABProfiledInterval);
static int calc_elapsed_times(ABProfiledInterval);
static int print_startup_time(ABProfiledInterval totalTime);

static ABProfiledIntervalRec	startupTime;
#endif /* DEBUG/performance */

/*****************************************************************************
**                                                                          **
**       Private Function Declarations                                      **
**                                                                          **
*****************************************************************************/
static void     win_get_geometry(
                    ABObj       obj
                );
static void     track_win_focus(
                    Widget      widget, 
                    XtPointer   client_data, 
                    XEvent      *event, 
                    Boolean     *cont_disp 
                );
static void     hide_windows(
                    ABObj       obj
                );
static void     check_win_for_resize(
                    ABObj       obj
                );
static void     cleanup_window(
                    ABObj       obj
                );


/*****************************************************************************
**                                                                          **
**       Public Function Definitions                                        **
**                                                                          **
*****************************************************************************/

/*
** Enable Test Mode
**
** This is the entry point into Test Mode. It setups up the book keeping
** structures necessary, and prepairs for the interpretation of connections.
*/
void
tmode_enable(
    ABObj     project,
    BOOL      test_project
)
{
    if (!project || !obj_is_project(project))
	return;

    ab_set_busy_cursor(TRUE);
    util_dprintf(1, "\ntmode_enable\n");
#ifdef DEBUG	/* performance testing */
    get_start_times(&startupTime);
#endif /* DEBUG */

    /* store whether we are testing the whole project or not */
    if (test_project)
	tmodeP_obj_set_flags(project, TestModeTestProject);
    
    /* create a list of the windows within the project */
    if (tmodeP_window_list_create(project) != OK)
    {
	util_dprintf(1, "tmode_enable: Unable to create window list for Test Mode.\n");
	goto cret;
    }

    /* get the current dimensions of the windows */
    tmodeP_window_list_iterate(win_get_geometry);
    
    /*
    ** Attach a focus handler to each window in the project. When the window
    ** receives focus, it gets marked dirty.
    */
    if (tmodeP_window_list_add_handler(EnterWindowMask, False,
	    track_win_focus) != OK)
    {
	util_dprintf(1, "tmode_enable: Unable to add window focus handler.\n");
	goto cret;
    }

    /* initialize the old interface */
    conn_test_mode_initialize(project);
    conn_enable_action_interpret(project, test_project);

cret:
#ifdef DEBUG	/* performance testing */
    get_end_times(&startupTime);
    print_startup_time(&startupTime);
#endif /* DEBUG */
    ab_set_busy_cursor(FALSE);
}

/*
** Disable Test Mode
**
** This is the exit point for Test Mode. 
*/
void
tmode_disable(
    ABObj project
)
{
    ABObj		mod;
    AB_TRAVERSAL	trav;
    
    if (!project || !obj_is_project(project))
	return;

    ab_set_busy_cursor(TRUE);
    util_dprintf(1, "\ntmode_disable\n");
#ifdef DEBUG	/* performance testing */
    get_start_times(&startupTime);
#endif /* DEBUG */

    tmodeP_window_list_iterate(check_win_for_resize);	/* process for resize */
    tmodeP_window_list_iterate(hide_windows);		/* rm not needed or resized */

    conn_disable_action_interpret(project);
    conn_test_mode_cleanup(project);
    tmodeP_window_list_iterate(cleanup_window);		/* re-instantiate/set_ui_args */

    /* make sure windows are Mapped correctly */
    for (trav_open(&trav, project, AB_TRAV_MODULES);
	 (mod = trav_next(&trav)) != NULL; )
	if (obj_is_module(mod))
	{
	    if (obj_has_flag(mod, MappedFlag))
		objxm_tree_map(mod, TRUE);
	    else
		objxm_tree_map(mod, FALSE);
	}
    trav_close(&trav);
    
    /* detach the receive focus handler */
    if (tmodeP_window_list_remove_handler(EnterWindowMask, False,
	    track_win_focus) != OK)
    {
	util_dprintf(1, "tmode_disable: Unable to remove window focus handler.\n");
	goto cret;
    }

    /* destroy the window list */
    if (tmodeP_window_list_destroy() != OK)
	util_dprintf(1, "tmode_disable: Problems destroying window list.\n");

    tmodeP_obj_construct_flags(project);

cret:
#ifdef DEBUG	/* performance testing */
    get_end_times(&startupTime);
    print_startup_time(&startupTime);
#endif /* DEBUG */
    ab_set_busy_cursor(FALSE);
}

/*****************************************************************************
**                                                                          **
**       Private Function Definitions                                       **
**                                                                          **
*****************************************************************************/

#ifdef DEBUG	/* performance testing */
static int
get_start_times(ABProfiledInterval interval)
{
    return get_cur_times(&(interval->realTime.startSeconds),
		  	 &(interval->CPUTime.startSeconds));
}

static int
get_end_times(ABProfiledInterval interval)
{
    return get_cur_times(&(interval->realTime.endSeconds),
		  	 &(interval->CPUTime.endSeconds));
}

static int
calc_elapsed_times(ABProfiledInterval interval)
{
    interval->realTime.elapsedSeconds =
	interval->realTime.endSeconds - interval->realTime.startSeconds;
    interval->CPUTime.elapsedSeconds =
	interval->CPUTime.endSeconds - interval->CPUTime.startSeconds;
    return 0;
}

static int
print_startup_time(ABProfiledInterval totalTime)
{
    /*
     * Print out statistics about load
     */
    calc_elapsed_times(totalTime);

    fprintf(stderr, "Transition Time (Real/CPU) seconds: ");
    fprintf(stderr, "(%lg/%lg)\n", 
	totalTime->realTime.elapsedSeconds,
	totalTime->CPUTime.elapsedSeconds);

    return 0;
}

static int
get_cur_times(double *realTimeOut, double *cpuTimeOut)
{
    static BOOL	initialized = FALSE;
    static long	ticks_per_second = 1;
    struct tms  timeInfo;
    double      realTime;
    double      cpuTime;
    if (!initialized)
    {
	initialized = TRUE;
        ticks_per_second = sysconf(_SC_CLK_TCK);
    }

    realTime = times(&timeInfo);
    cpuTime = timeInfo.tms_utime + timeInfo.tms_stime
                        + timeInfo.tms_cutime + timeInfo.tms_cstime;
    *realTimeOut  = realTime / ticks_per_second;
    *cpuTimeOut = cpuTime / ticks_per_second;

    return 0;
}
#endif /* DEBUG/performance */

/*
** Record the window geometry
*/
static void
win_get_geometry(
    ABObj       obj
)
{
    XRectangle     rect;
    
    if (!obj || !obj->ui_handle)
	return;

    /* make sure there is a test mode data structure */
    if (!tmodeP_obj_has_data(obj))
    {
	if (tmodeP_obj_create_data(obj) != OK)
	{
	    util_dprintf(2, "win_get_geometry: Unable to create test mode data.\n");
	    return;
	}
    }

    x_get_widget_rect(objxm_get_widget(obj), &rect);
	
    tmodeP_obj_set_geometry(obj, rect);
}

/*
** Track when a window receives focus and mark the window dirty
*/
static void
track_win_focus(
    Widget      widget,
    XtPointer   client_data,
    XEvent      *event,
    Boolean     *cont_dispatch
)
{
    ABObj obj = (ABObj) client_data;
    
    if (event->type == EnterNotify)
    {
	/* mark it dirty */
	tmodeP_obj_set_flags(obj, TestModeFlagDirtyWindow);

	/* it's dirty, so no need to track it anymore */
	XtRemoveEventHandler(
	    widget,		   /* Widget */
	    EnterWindowMask,	   /* EventMask */
	    False,		   /* non-maskable events - Boolean */
	    track_win_focus,	   /* XtEventHandler */
	    (XtPointer) obj	   /* client_data */
	);
    }
}

/*
** If the module this window is associated with is hidden (in Build Mode)
** unmap it. Don't worry about unmapping the window's tree, that will be
** performed as one of the last steps in tmode_disable.
*/
static void
hide_windows(
    ABObj       obj
)
{
    Widget widget;
    BOOL hide = FALSE;
    
    if (!obj || !obj->ui_handle)
	return;

    /* if the module will be hidden, unmap the windows associated w/it */
    if (!obj_has_flag(obj_get_module(obj), MappedFlag))
	if (obj_has_flag(obj, MappedFlag)) 
	    hide = TRUE;
	    
    /* if the window is dirty or was resized, hide it to avoid uglyness */
    if (tmodeP_obj_has_flags(obj, TestModeFlagDirtyWindow | TestModeFlagResizedWindow))
	hide = TRUE;
    
    if (hide)
    {
	widget = (Widget)obj->ui_handle;
	
	util_dprintf(2,"UN-MAPPING widget: %s\n", 
		     util_strsafe(obj_get_name(obj)));

	if (XtIsSubclass(widget, applicationShellWidgetClass) ||
	    XtIsSubclass(widget, topLevelShellWidgetClass) ||
	    XtIsSubclass(widget, xmDialogShellWidgetClass))
	{
	    XtPopdown(widget); 
	    obj_clear_flag(obj, MappedFlag);
	}

	/*
	 * Since we need to unmap/remap the window in order to hide
	 * re-instantiations and set-args, we need to mark this window
	 * object so that when the iconify_tracker, object_track_iconify(),
	 * event handler is called, it doesn't interpret the Unmap event as
	 * an iconify. (this is an unfortunate hack to get around the
	 * mysterious problem of the Unmap event occurring AFTER we return
	 * to BUILD mode).
	 */
	obj_set_flag(obj, TestModeWinFlag);
    }
}

/*
** Check if the window has been resized. If so, mark dirty.
*/
static void
check_win_for_resize(
    ABObj       obj
)
{
    XRectangle     rect;
    Boolean        resized = False;
    
    if (!obj || !obj->ui_handle)
	return;

    /* compair current values with those stored on entry into Test Mode */
    if (tmodeP_obj_has_data(obj))
    {
	x_get_widget_rect(objxm_get_widget(obj), &rect);
	
	/* if width or height has changed, it's been resized */
	if (rect.width != tmodeP_obj_get_width(obj))
	    resized = True;
	else if (rect.height != tmodeP_obj_get_height(obj))
	    resized = True;
    
	/* if resized, mark dirty */
	if (resized)
	    tmodeP_obj_set_flags(obj, TestModeFlagResizedWindow);
    }
}

/*
** Cleanup the window -- If the window was resized, there is no easy way to
**     reconfigure it and make sure all of the widgets are proper; so,
**     destroy its widgets and re-instantiate. If the window was not
**     resized, but just fiddled with (Dirty); set all of the widgets
**     back to their original values (as defined in the obj structure).
*/
static void
cleanup_window(
    ABObj       obj
)
{
    if (!obj || !obj->ui_handle || !tmodeP_obj_flags(obj))
	return;

    if (tmodeP_obj_has_flags(obj, TestModeFlagResizedWindow))
    {
	/* destroy */
	objxm_tree_uninstantiate(obj, True);

	/* re-instantiate */
	abobj_instantiate_tree(obj, True);
    }
    else if (tmodeP_obj_has_flags(obj, TestModeFlagDirtyWindow))
    {
	BOOL new;
	
	/*
	** Reset the UI Args on the windows tree
	*/
	objxm_tree_remove_ui_args(obj, OBJXM_CONFIG_BUILD);
	objxm_tree_set_ui_args(obj, OBJXM_CONFIG_BUILD, TRUE);
	
	objxm_tree_instantiate_changes(obj, &new);

	objxm_tree_remove_ui_args(obj, OBJXM_CONFIG_BUILD);
    }
}

