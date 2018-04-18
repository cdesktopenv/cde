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
 *      $XConsortium: abobj_events.c /main/3 1995/11/06 17:15:49 rswiston $
 *
 * @(#)abobj_events.c	1.50 15 Feb 1994      cde_app_builder/src/ab
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
 ***********************************************************************
 * abobj_events.c - implements UI object 'behavior' for created
 *         UI objects
 *         
 *
 ***********************************************************************
 */
#include <stdio.h>
#include <X11/keysym.h>
#include <Xm/Xm.h>
#include <Xm/XmStrDefs.h>
#include <Xm/DrawingA.h>
#include <Xm/MainW.h>
#include <Xm/ScrolledW.h>
#include <Xm/Scale.h>
#include <Xm/Text.h>
#include <Xm/FileSB.h>
#include <Dt/ComboBox.h>
#include <Dt/MenuButton.h>
#include <Dt/SpinBox.h>
#include <ab_private/trav.h>
#include <ab_private/objxm.h>
#include <ab_private/ab.h>
#include <ab_private/pal.h>
#include "abobjP.h"
#include <ab_private/x_util.h>
#include <ab_private/ui_util.h>
#include <ab_private/conn.h>
#include <ab_private/brws.h>
#include <ab_private/prop.h>
#include "palette_ui.h"


#define AB_MAX_TRANSTBLS    70

/*
 * Structure to store info related to implementing programmatic
 * double-click on widgets where the translation does not work
 * (DtMenuButton)
 */
typedef struct DOUBLE_CLICK_INFO {
    Widget		widget;
    XEvent		*event;
    unsigned int 	interval;
    int			start_x;
    int			start_y;
    BOOL                waiting;
} DoubleClickInfo;

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
/*
 * Widget Action Procs
 */
static void     object_button_press(
                    Widget w,
                    XEvent *ev,
                    String *params,
                    int nparams
                );
static void     object_mouse_enter(
                    Widget w,
                    XEvent *ev,
                    String *params,
                    int nparams
                );
static void     object_mouse_leave( 
                    Widget w, 
                    XEvent *ev, 
                    String *params, 
                    int nparams 
                ); 
static void     object_mouse_motion( 
                    Widget w,  
                    XEvent *ev,  
                    String *params,  
                    int nparams  
                );  
static void    	object_select(
		    Widget w, 
		    XEvent *ev, 
		    String *params, 
		    int nparams
        	);
static void    	object_toggle_select(
		    Widget w, 
		    XEvent *ev, 
		    String *params, 
		    int nparams
        	);
static void    	object_invoke_props(
		    Widget w, 
		    XEvent *ev, 
		    String *params, 
		    int nparams
        	);
static void    	object_popup_menu(
		    Widget w, 
		    XEvent *ev, 
		    String *params, 
		    int nparams
        	);
static void	objects_pixel_move(
                    Widget w,
                    XEvent *ev,
                    String *params,
                    int nparams
                );

/*
 * Widget Event Handlers
 */
static void	interpose_button_event(
                    Widget w,
                    XtPointer cd,
                    XEvent *ev,
                    Boolean *cont
                );
static void     object_button_drag(
                    Widget w,
                    XtPointer cd,
                    XEvent *ev,
                    Boolean *cont
                );
static void    	object_move_release(
		    Widget w, 
		    XtPointer cd, 
		    XEvent *ev, 
		    Boolean *cont
        	);
static void    	object_resize_release(
		    Widget w, 
		    XtPointer cd, 
		    XEvent *ev, 
		    Boolean *cont
        	);
static void	object_mselect(
		    Widget      widget,
		    XEvent      *event,
		    XRectangle  *rb_rect,
		    XtPointer   client_data
		);
static void     object_track_move(
                    Widget      widget,
                    XtPointer 	cd,
                    XEvent 	*ev,
                    Boolean 	*cont
                );
static void	object_track_iconify(
    		    Widget      widget,
                    XtPointer 	cd,
                    XEvent 	*ev,
                    Boolean 	*cont
                );
static void	prevent_closeCB(
		    Widget	w,
		    XtPointer	client_data,
		    XtPointer	call_data
		);
static void	wait_for_double(
    		    XtPointer    client_data,
    		    XtIntervalId timer_id
		);

/*
 * Private Internal Procs
 */
static int     	initiate_move(
		    Widget 	widget, 
		    ABObj 	obj 
        	);

static int    	initiate_resize(
		    Widget     	widget,
		    ABObj      	obj,
		    RESIZE_DIR 	dir
        	);

static int      initiate_mselect(
                    Widget     	widget,
                    ABObj      	obj
                );

/*************************************************************************
**                                                                      **
**    Private Data                                                    **
**                                                                      **
**************************************************************************/

/*
 * Build-Mode Actions
 */
XtActionsRec build_actions[] = {
    {"ObjectMouseEnter",    (XtActionProc)object_mouse_enter },
    {"ObjectMouseLeave",    (XtActionProc)object_mouse_leave },
    {"ObjectMouseMotion",   (XtActionProc)object_mouse_motion },
    {"ObjectSelect",        (XtActionProc)object_select },
    {"ObjectButtonPress",   (XtActionProc)object_button_press },
    {"ObjectToggleSelect",  (XtActionProc)object_toggle_select },
    {"ObjectInvokeProps",   (XtActionProc)object_invoke_props },
    {"ObjectPopupMenu",     (XtActionProc)object_popup_menu },
    {"ObjectDragChord",     (XtActionProc)conn_drag_chord },
    {"ObjectsPixelMove",    (XtActionProc)objects_pixel_move }
};

/* 
 * Build-Mode Translations
 */
String base_translations =
    "<Enter>:			ObjectMouseEnter() \n\
     <Leave>:			ObjectMouseLeave() \n\
     <Motion>:			ObjectMouseMotion() \n\
      Ctrl <Btn1Down>:		ObjectDragChord() \n\
      Shift <Btn1Down>: 	ObjectButtonPress() \n\
      Shift <Btn1Down>,<Btn1Up>:ObjectToggleSelect() \n\
     <Btn1Down>:		ObjectButtonPress() \n\
     <Btn1Down>,<Btn1Up>:	ObjectSelect() \n\
     <Btn1Up>(2):     		ObjectInvokeProps() \n\
     <Key>Left:			ObjectsPixelMove() \n\
     <Key>Right:		ObjectsPixelMove() \n\
     <Key>Down:			ObjectsPixelMove() \n\
     <Key>Up:			ObjectsPixelMove() \n"; /* will be appended-to */

String btn2_adjust_translations = 
    "<Btn2Down>:                ObjectButtonPress() \n\
     <Btn2Down>,<Btn2Up>:       ObjectToggleSelect() \n"; /* will be appended-to */

String btn3_menu_translations =
     "<Btn3Down>:               ObjectPopupMenu()";

String btn2_menu_translations =
     "<Btn2Down>:		ObjectPopupMenu()";

static XtTranslations build_transtbl = NULL;



extern const int AB_drag_threshold;

static RESIZE_DIR resize_dir      = NONE;
static Boolean just_moved         = False;
static Boolean just_mselected	  = False;
static Boolean potential_move	  = False;
static Boolean move_in_progress   = False;
static Boolean resize_in_progress = False;
static Boolean mselect_in_progress= False;
static Boolean mselect_adjust	  = False;
static Boolean actions_init	  = False;

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

/*
 * Traverse tree and set the mode of behavior ("build" or "test")
 * for each object with a valid ui_handle (instantiated widget ID)
 */
int
abobj_tree_set_build_actions(
    ABObj    	root
)
{
    AB_TRAVERSAL    	trav;
    ABObj       	obj;
 
    if (root == NULL)
	return ERROR;

    /* set the appropriate build actions */
    for (trav_open(&trav, root, AB_TRAV_UI);
        (obj = trav_next(&trav)) != NULL; )
    {
	if (objxm_get_widget(obj) != NULL)
	{
	    if (AB_builder_mode == MODE_BUILD)
	    	abobjP_enable_build_actions(obj, objxm_get_widget(obj));
	    else /* TEST MODE */
        	abobjP_disable_build_actions(obj, objxm_get_widget(obj));
       	}
    }
    trav_close(&trav);

    return OK;
}

/*
 * Register build actions with Xt
 */
void
abobj_register_build_actions(
    XtAppContext app
)
{

    /* Register Build Actions */
    XtAppAddActions(app, build_actions, XtNumber(build_actions));
    actions_init = True;

}

static void
enable_on_internal_widgets(
    Widget	widget,
    ABObj	obj,
    ABObj	root
)
{
    WidgetList		children;
    int			num_children = 0;
    int			i;

    XtVaGetValues(widget,
                   XtNnumChildren,    &num_children,
                   XtNchildren,       &children,
                   NULL);

    for (i = 0; i < num_children; ++i)
    {
        if (!XtIsWidget(children[i]) || 
            XtIsSubclass(children[i], shellWidgetClass) ||
            XtIsSubclass(children[i], xmTextWidgetClass) ||
            XtIsSubclass(children[i], xmDrawingAreaWidgetClass))
            continue;

        objxm_store_obj_and_actions(children[i], obj);

        XtVaSetValues(children[i],
                XtNtranslations,	build_transtbl,
                NULL);

        XtAddEventHandler(children[i], Button1MotionMask | Button2MotionMask,
                False, object_button_drag, (XtPointer)root);

	/* Recursion...*/
	if (XtIsSubclass(children[i], compositeWidgetClass))
	    enable_on_internal_widgets(children[i], obj, root);
    }
}

static void
disable_on_internal_widgets(
    Widget	widget,
    ABObj	obj,
    ABObj	root
)
{
    WidgetList  	children;
    int         	num_children = 0;
    XtTranslations 	orig_trans;
    int         	i;

    XtVaGetValues(widget,
                   XtNnumChildren,    &num_children,
                   XtNchildren,       &children,
                   NULL);

    for (i = 0; i < num_children; ++i)
    {
        if (!XtIsWidget(children[i]) || 
	    XtIsSubclass(children[i], shellWidgetClass) ||
	    XtIsSubclass(children[i], xmTextWidgetClass) ||
	    XtIsSubclass(children[i], xmDrawingAreaWidgetClass))
            continue;

        XtRemoveEventHandler(children[i],
                        Button1MotionMask | Button2MotionMask,
                        False,
                        object_button_drag, (XtPointer)root);
 
        orig_trans = objxm_get_actions_from_widget(children[i]);
 
        if (orig_trans)
            XtVaSetValues(children[i],
                        XtNtranslations, orig_trans,
                        NULL);
        else
            XtUninstallTranslations(children[i]);

	/* Recursion...*/
        if (XtIsSubclass(children[i], compositeWidgetClass)) 
	    disable_on_internal_widgets(children[i], obj, root);
    }
}


/*
 * Enable build-mode behavior for the widget
 */
void
abobjP_enable_build_actions(
    ABObj 	obj, 
    Widget 	widget
)
{
    ABObj 	 evObj;
    ABObj 	 rootObj = obj_get_root(obj);

    if (obj_has_flag(obj, BuildActionsFlag))
	return; /* Already has actions */

    if (objxm_is_menu_widget(widget) || obj_is_menu_item(obj))
	return;

    /* Window objects require specialized event tracking */
    if (obj_is_window(obj) && XtIsSubclass(widget, shellWidgetClass))
    {
 	/* Prevent the user from "Quitting" a window in build-mode*/
	ui_add_window_close_callback(widget, prevent_closeCB, NULL, XmDO_NOTHING);

   	/* Track when a window is iconified */
	if (obj_is_base_win(obj))
	    ui_add_window_iconify_handler(widget, object_track_iconify, (XtPointer)obj);

	/* Track when the user moves the window so that if it is re-instantiated,
	 * it will appear in the position the user placed it in.
	 * NOTE: this does not set a permanent position attribute for a window!
	 */
        XtAddEventHandler(widget, StructureNotifyMask, False,
                (XtEventHandler)object_track_move, (XtPointer)rootObj);
        return;
    }

    if (build_transtbl == NULL)
    {
	int        len;
  	String	   build_translations, menu_translations;

	/* First time only.
         * Build up translation table based on the number of mouse buttons
         * and the value of the display resource, "enableBtn1Transfer".
	 */
	menu_translations = (AB_BMenu == Button3? btn3_menu_translations : btn2_menu_translations);

	len = strlen(base_translations) + strlen(menu_translations) + 1;

	if (AB_BMenu == Button3 && AB_btn1_transfer == True) 
	    /* Button2 can be used for ADJUST */
	    len += strlen(btn2_adjust_translations);

	build_translations = (String)util_malloc(len*sizeof(char));
	if (build_translations == NULL)
	    return; /* ouch */

	/* Build Translation table */
	strcpy(build_translations, base_translations);
	if (AB_BMenu == Button3 && AB_btn1_transfer == True)
	   strcat(build_translations, btn2_adjust_translations);
	strcat(build_translations, menu_translations);

	build_transtbl = XtParseTranslationTable(build_translations);

	util_free(build_translations);
    }

    /* Enable build mode behavior */
    XtVaSetValues(widget, 
		XtNtranslations, build_transtbl,
        	NULL);

    if (obj_is_item(obj))
	evObj = obj_get_parent(obj);
    else
	evObj = obj;

    /* The DtMenuButton widget uses an internal event-handler to post its menu,
     * so our standard translations will not get called. Therefore,  we use
     * an event-handler to get these buttons events before the widget does.
     */  
    if (XtIsSubclass(widget, dtMenuButtonWidgetClass))
        XtInsertEventHandler(widget, 
		ButtonMotionMask | ButtonPressMask | ButtonReleaseMask,
                False, interpose_button_event, (XtPointer)rootObj, XtListHead);

    /* Track drag events on all other widgets */
    else if (!(obj_is_sub(obj) && obj_is_window(rootObj)) &&
        !obj_is_menubar(rootObj))
        XtAddEventHandler(widget, Button1MotionMask | Button2MotionMask, False,
                object_button_drag, (XtPointer)rootObj);

    /* Track size changes caused internally & externally */
    if (objxm_comp_get_subobj(rootObj, AB_CFG_SIZE_OBJ) == obj)
	XtAddEventHandler(widget, StructureNotifyMask, False,
		(XtEventHandler)abobjP_track_external_resizes, (XtPointer)rootObj);

    /* Ensure all this stuff gets registered on *internal* widgets as well */
    if ((XtIsSubclass(widget, xmScrolledWindowWidgetClass) &&
	!XtIsSubclass(widget, xmMainWindowWidgetClass)) ||
	XtIsSubclass(widget, dtComboBoxWidgetClass) ||
	XtIsSubclass(widget, xmScaleWidgetClass) ||
	XtIsSubclass(widget, xmFileSelectionBoxWidgetClass) ||
	XtIsSubclass(widget, dtSpinBoxWidgetClass))
    	enable_on_internal_widgets(widget, obj, rootObj);

    obj_set_flag(obj, BuildActionsFlag);

}

/*
 * Disable build-mode behavior for the widget
 */
void
abobjP_disable_build_actions(
    ABObj   	obj,
    Widget  	widget
)
{
    ABObj	   evObj;
    ABObj          rootObj = obj_get_root(obj);
    XtTranslations orig_trans;

    if (obj_is_window(obj) && XtIsSubclass(widget, shellWidgetClass))
    {
	ui_remove_window_close_callback(widget, prevent_closeCB, NULL);

        if (obj_is_base_win(obj))
            ui_remove_window_iconify_handler(widget, object_track_iconify, (XtPointer)obj); 

        XtRemoveEventHandler(widget, StructureNotifyMask, False, 
                (XtEventHandler)object_track_move, (XtPointer)rootObj);
    }

    /* Don't bother for Shells or menu-related widgets */
    if (XtIsSubclass(widget, shellWidgetClass) ||
	objxm_is_menu_widget(widget) || obj_is_menu_item(obj))
        return;

    if (!obj_has_flag(obj, BuildActionsFlag))
	return;  /* No actions to disable */

    if (obj_is_item(obj))
        evObj = obj_get_parent(obj);
    else
        evObj = obj;

    abobj_deselect(obj);
 
    if (XtIsSubclass(widget, dtMenuButtonWidgetClass))
        XtRemoveEventHandler(widget, 
		ButtonMotionMask | ButtonPressMask | ButtonReleaseMask,
                False, interpose_button_event, (XtPointer)rootObj);

    else if (!(obj_is_sub(obj) && obj_is_window(rootObj)) &&
        !(obj_is_menubar(rootObj))) 
        XtRemoveEventHandler(widget, Button1MotionMask | Button2MotionMask, False,
                object_button_drag, (XtPointer)rootObj);

    if (objxm_comp_get_subobj(rootObj, AB_CFG_SIZE_OBJ) == obj)
        XtRemoveEventHandler(widget, StructureNotifyMask, False,
                (XtEventHandler)abobjP_track_external_resizes, (XtPointer)rootObj);

    orig_trans = objxm_get_actions_from_widget(widget);

    /*
     * if no original translations, uninstall instead of setting
     * XtNtranslations to NULL because this may not have any
     * effect
     */
    if (orig_trans)
        XtVaSetValues(widget, XtNtranslations, orig_trans, NULL);
    else
	XtUninstallTranslations(widget);

    if ((XtIsSubclass(widget, xmScrolledWindowWidgetClass) &&
        !XtIsSubclass(widget, xmMainWindowWidgetClass)) ||
        XtIsSubclass(widget, dtComboBoxWidgetClass) ||
        XtIsSubclass(widget, xmScaleWidgetClass) ||
	XtIsSubclass(widget, xmFileSelectionBoxWidgetClass) ||
        XtIsSubclass(widget, dtSpinBoxWidgetClass))
    	disable_on_internal_widgets(widget, obj, rootObj);

    obj_clear_flag(obj, BuildActionsFlag);
}

/*
 * Action: pointer has 'entered' widget
 */
static void
object_mouse_enter(
    Widget 	widget,
    XEvent 	*event,
    String 	*params,
    int 	num_params
)
{
    ABObj obj = NULL;
    ABObj rootObj;
    char  pos_str[12];
    char  sz_str[16];

    obj = objxm_get_obj_from_widget(widget);

    if (obj != NULL)
    {
	rootObj = obj_is_item(obj)? obj_get_parent(obj) : obj;

	rootObj = obj_get_root(rootObj);

	sprintf(pos_str, "%3d,%3d", rootObj->x, rootObj->y);
	sprintf(sz_str,  "%3d X %3d", 
		abobj_get_actual_width(rootObj), abobj_get_actual_height(rootObj));
	ab_update_stat_region(AB_STATUS_OBJ_TYPE, pal_get_item_subname(rootObj,obj_get_subtype(rootObj)));
	ab_update_stat_region(AB_STATUS_OBJ_NAME, obj_get_name(rootObj));
	ab_update_stat_region(AB_STATUS_OBJ_POS,  pos_str);
	ab_update_stat_region(AB_STATUS_OBJ_SIZE,  sz_str);
    }
}

/*
 * Action: pointer has 'left' widget
 */
static void 
object_mouse_leave(
    Widget 	widget, 
    XEvent 	*event, 
    String 	*params,
    int 	num_params 
)
{ 

    ab_update_stat_region(AB_STATUS_OBJ_TYPE, "      ");
    ab_update_stat_region(AB_STATUS_OBJ_NAME, "      "); 
    ab_update_stat_region(AB_STATUS_CURS_POS, "      ");
    ab_update_stat_region(AB_STATUS_OBJ_POS,  "      ");
    ab_update_stat_region(AB_STATUS_OBJ_SIZE, "      ");
} 


/* 
 * Action: pointer has 'left' widget 
 */ 
static void  
object_mouse_motion(
    Widget 	widget, 
    XEvent 	*event,  
    String 	*params, 
    int 	num_params  
) 
{  
    ABObj	 obj = NULL, rootObj;
    XMotionEvent *mevent = (XMotionEvent*)event;
    char         motionstr[16];
    int		 rx, ry;

    obj = objxm_get_obj_from_widget(widget);

    if (obj != NULL)
    {
	/* Make sure the x,y position is relative to the ROOT obj
	 * of a given obj or CompositeObj
	 */
        rootObj = obj_is_item(obj)? obj_get_parent(obj) : obj;
        rootObj = obj_get_root(rootObj);
	if (rootObj != obj)
    	    x_widget_translate_xy(objxm_get_widget(obj), (XtPointer)objxm_get_widget(rootObj), 
                mevent->x, mevent->y, &rx, &ry); 
	else
	{
	    rx = mevent->x;
	    ry = mevent->y;
  	}
  
        sprintf(motionstr, "%3d,%3d", rx, ry);
        ab_update_stat_region(AB_STATUS_CURS_POS, motionstr);  
    }
}  
    
/*
 * Action: caused the object to become "selected"
 */
static void
object_select(
    Widget 	widget, 
    XEvent 	*event, 
    String 	*params, 
    int 	num_params
)
{
    ABObj obj = NULL;

    obj = objxm_get_obj_from_widget(widget);

    /* This action is called directly after a Move (on the Mouse
     * button Release), therefore, IF a Move just occurred, DO NOT
     * deselect all other selected objects...
     */
    if (!just_moved)
    	abobj_deselect_all(obj_get_project(obj));
    else
	just_moved = False;

    abobj_select(obj);

}

/*
 * Action: toggles the select-state of the object
 */
static void
object_toggle_select(
    Widget 	widget, 
    XEvent 	*event, 
    String 	*params, 
    int 	num_params
)
{
    ABObj 	  obj, rootObj;
    ABSelectedRec sel;
    BOOL	  allow = True;
    int		  i;

    if (just_moved) /* BUTTON2 TRANSFER - don't toggle select! */
    {
        just_moved = False;
	return;
    }

    obj = objxm_get_obj_from_widget(widget);
    rootObj = obj_get_root(obj);
    if (obj_is_item(rootObj))
	rootObj = obj_get_root(obj_get_parent(rootObj));

    if (obj_is_selected(rootObj))
        abobj_deselect(rootObj); 
    else
    {
	/* Only objects on the same level (siblings) may be
	 * selected at one time.  If a non-sibling object is
	 * already selected, do not allow the new object to
	 * become selected...
	 */
	abobj_get_selected(obj_get_window(rootObj), True, False, &sel);

	for(i = 0; i < sel.count; i++)
	{
	    if (!obj_is_sibling(rootObj, sel.list[i]))
		allow = False;
	}
	if (allow)
    	    abobj_select(rootObj);

	util_free(sel.list);
    }

}

/*
 * Action: invokes the property sheet for the object
 */
static void
object_invoke_props(
    Widget 	widget, 
    XEvent 	*event, 
    String 	*params, 
    int 	num_params
)
{
    ABObj         obj = NULL;

    obj = objxm_get_obj_from_widget(widget);

    prop_load_obj(obj, AB_PROP_REVOLVING);
 
}

/*
 * Action: popup the build menu for the object
 */
static void
object_popup_menu(
    Widget 	widget, 
    XEvent 	*event, 
    String 	*params, 
    int 	num_params
)
{
    ABObj	  obj = NULL;
    ABSelectedRec sel;

    obj = objxm_get_obj_from_widget(widget); 

    abobj_get_selected(obj_get_window(obj), True, False, &sel);

    if (event->type == ButtonPress)
    	abobj_popup_menu(WIN_EDIT_MENU, widget, &sel, (XButtonEvent *)event);

    util_free(sel.list);

}

/*
 * Action: move selected objects by 1 pixel
 */
static void
objects_pixel_move(
    Widget 	widget,
    XEvent 	*event,
    String 	*params,
    int 	num_params
)
{
    XKeyEvent	  *kevent;
    ABObj         obj = NULL;
    ABSelectedRec sel;
    KeySym	  keysym;
    Modifiers	  mod;

    if (event->type == KeyPress)
    {
	kevent = (XKeyEvent*)event;

    	obj = objxm_get_obj_from_widget(widget);
 
    	abobj_get_selected(obj_get_window(obj), False, False, &sel);

	if (sel.count == 0)
	    return;

    	XtTranslateKeycode(kevent->display, kevent->keycode, kevent->state,
		&mod, &keysym);

    	switch(keysym)
	{
	    case XK_Up:
		abobj_nudge_selected(sel.list, sel.count, 0, -1, True);
		break;
	    case XK_Down:
                abobj_nudge_selected(sel.list, sel.count, 0, 1, True); 
                break; 
	    case XK_Right:
                abobj_nudge_selected(sel.list, sel.count, 1, 0, True);  
                break;  
	    case XK_Left:
                abobj_nudge_selected(sel.list, sel.count, -1, 0, True);   
                break;   
	    default:
		break;
	}

    	util_free(sel.list);
    }
}

/*
 * grab the pointer to track the movement of the object
 */
static int 
initiate_move(
    Widget 	widget, 
    ABObj 	obj 
)
{
    ABObj		rootObj;
    ABObj        	geomObj;
    Widget        	geomWidget;
    Widget        	parent;
    static Cursor     	move_cursor = 0;

    rootObj = obj_get_root(obj);

    geomObj    = objxm_comp_get_subobj(rootObj, AB_CFG_POSITION_OBJ);
    geomWidget = objxm_get_widget(geomObj);

    if (geomWidget == NULL)
    {
        util_dprintf(0,"initiate_move: no geometry widget\n");
        return ERROR;
    }
    parent = XtParent(geomWidget);

    if (!move_cursor)
	move_cursor = abobjP_get_resize_cursor(widget, MOVE);

    XtAddEventHandler(widget, ButtonReleaseMask,  False,
                object_move_release, (XtPointer)rootObj);

    if (XtGrabPointer(widget, False, 
        	ButtonReleaseMask | ButtonMotionMask | PointerMotionMask, 
                GrabModeAsync, GrabModeAsync,  XtWindow(parent), 
                        move_cursor, CurrentTime) == GrabSuccess)
 	return OK;

    return ERROR;

}

/*
 * Grab the pointer to interpret the resize action
 */
static int 
initiate_resize(
    Widget 	widget,
    ABObj 	obj,
    RESIZE_DIR 	dir
)
{
    ABObj     rootObj;
    ABObj     geomObj;
    Widget    geomWidget;
    Cursor    resize_cursor;

    rootObj = obj_get_root(obj);
    geomObj    = objxm_comp_get_subobj(rootObj, AB_CFG_SIZE_OBJ);
    geomWidget = objxm_get_widget(geomObj);

    if (geomWidget == NULL)
    {
        util_dprintf(0,"initiate_resize: no geometry widget\n");
        return ERROR;
    }

    resize_cursor = abobjP_get_resize_cursor(widget, dir);

    XtAddEventHandler(widget, ButtonReleaseMask,  False,
                object_resize_release, (XtPointer)obj);
 
    if (XtGrabPointer(widget, False,
        ButtonReleaseMask | ButtonMotionMask | PointerMotionMask,
        GrabModeAsync, GrabModeAsync,  0,
            resize_cursor, CurrentTime) == GrabSuccess)
 
        return OK;
 
    return ERROR;
 
}

/*
 * Action: mouse button down...
 */
static void
object_button_press(
    Widget 	widget,
    XEvent 	*event,
    String 	*params,
    int 	num_params
)
{
    XButtonEvent 	*bevent;
    ABObj       	obj = NULL;
    ABObj		rootObj;

    potential_move = False;
    just_moved = False;

    obj = objxm_get_obj_from_widget(widget); 
    rootObj = obj_get_root(obj);

    if (obj_is_window(rootObj))
	return;
 
    if (event->type == ButtonPress) 
    {
	bevent = (XButtonEvent*)event;

	if (bevent->state == 0 &&
	    (bevent->button == 1 ||
	    (AB_btn1_transfer != True && bevent->button == 2)))
				/* MOVE, RESIZE or RUBBERBAND-SELECT */
        {
	    resize_dir = abobjP_find_resize_direction(obj, widget, event);

	    if (resize_dir == MOVE) /* MOVE (pending subsequent drag event) */
		potential_move = True;

	    else if (bevent->button == 1 && resize_dir == NONE && obj_is_pane(rootObj))
	    {
		if (obj_is_control_panel(rootObj))
		{
		    /* RUBBERBAND-SELECT */
                    if (ui_initiate_rubberband(widget, False,
                        object_mselect, (XtPointer)obj) == ERROR)
                    {
			mselect_in_progress = False;
                        util_dprintf(0,"object_button_press: couldn't begin rubberband\n");
                    }
		    else
		    {
		    	mselect_in_progress = True;
		    	mselect_adjust = False;
		    }
		}
	    }
	    else if (bevent->button == 1) /* RESIZE */
	    {
	        if (obj_is_selected(rootObj))
	        {
            	    if (initiate_resize(widget, obj, resize_dir) == ERROR)
            	    {
                    	resize_in_progress = False;
                        util_dprintf(0, "object_button_press: couldn't begin resize\n");
            	    }
            	    else
                    	resize_in_progress = True;
	        }
            }
	}
        else if (bevent->button == 2 || 
		 (bevent->button == 1 && (bevent->state & ShiftMask))) /* RUBBERBAND-ADJUST */
	{
	    if (obj_is_control_panel(rootObj))
	    {
	        if (ui_initiate_rubberband(widget, False,
                        object_mselect, (XtPointer)obj) == ERROR)
                {
                    mselect_in_progress = False;
                    util_dprintf(0,"object_button_press: couldn't begin rubberband\n");
                }
                else
                    mselect_in_progress = mselect_adjust = True;
	    }
	}
    }
}

/*
 * Xt Timer proc : used to detect a double-click event for widgets
 * 		   where the translation doesn't work (DtMenuButton).
 */
static void 
wait_for_double(
    XtPointer	 client_data,
    XtIntervalId timer_id
)
{
    DoubleClickInfo *d_click = (DoubleClickInfo*)client_data;

    /* Waiting interval for double-click expired.
     * Go ahead and process the single-click...
     */
    d_click->waiting = False;
    object_button_press(d_click->widget, d_click->event, NULL, 0);

}

/*
 * Event Handler: For widgets which use internal event-handlers for
 * 		  button events, we must use an event-handler (instead
 *		  of translations) to grab these events.  We simply
 *		  call the appropriate action procedure ourselves.
 */
static void
interpose_button_event(
    Widget      widget,
    XtPointer   client_data,
    XEvent      *event,
    Boolean     *cont_dispatch
)
{
    XButtonEvent 		*bevent;
    XMotionEvent        	*mevent;
    static XEvent		event_cpy;
    static DoubleClickInfo	*d_click = NULL;
    static XtIntervalId		timer_id = 0;
    ABObj               	obj = (ABObj)client_data;
    ABObj               	moveObj;
    
    if (event->type == ButtonPress || event->type == ButtonRelease)
    {
	bevent = (XButtonEvent*)event;

        /* absorb all button events */
        *cont_dispatch = False;

	switch(bevent->button)
	{
	    case 1:
	    	if (event->type == ButtonPress)
		{
		    if (bevent->state & ControlMask) /* Connections Accelerator */
		    {
			conn_drag_chord(widget, event, NULL, 0);
			return;
		    }

    		    if (d_click == NULL) /* Init DoubleClick structure */
    		    {
        		d_click = (DoubleClickInfo*)util_malloc(sizeof(DoubleClickInfo));
        		if (d_click == NULL)
            		    return; /* yikes! */
        		d_click->widget = NULL;
        		d_click->event = NULL;
			d_click->interval = XtGetMultiClickTime(XtDisplay(widget));
        		d_click->waiting = False;
    		    }   

		    if (d_click->waiting) /* double-click occurred */
		    {
			XtRemoveTimeOut(timer_id);
			d_click->waiting = False;
			object_invoke_props(widget, event, NULL, 0);
		    }
		    else if (bevent->state == 0) /* Start timing for double-click */
		    {
			event_cpy = *event;
			d_click->waiting = True;
			d_click->widget = widget;
			d_click->event = &event_cpy;
			d_click->start_x = bevent->x;
			d_click->start_y = bevent->y;
		    	timer_id = XtAppAddTimeOut(
				XtWidgetToApplicationContext(widget), 
				d_click->interval,
				(XtTimerCallbackProc)wait_for_double, 
				(XtPointer)d_click);
		    }

		}
		/* ButtonRelease */

		else if (resize_in_progress)
		    object_resize_release(widget, client_data, event, cont_dispatch);

	    	else if (move_in_progress) /* ButtonRelease */
		    object_move_release(widget, client_data, event, cont_dispatch);

                else if (bevent->state & ShiftMask)
                    object_toggle_select(widget, event, NULL, 0);

	    	else
	    	    object_select(widget, event, NULL, 0);
		break;

	    case 2:
		if (event->type == ButtonPress && AB_btn1_transfer != True) /* BUTTON2 TRANSFER */
		    object_button_press(widget, event, NULL, 0);

	    	else if (event->type == ButtonRelease)
		{
		    if (AB_btn1_transfer != True && move_in_progress) /* BUTTON2 TRANSFER */
			object_move_release(widget, client_data, event, cont_dispatch);
		    else
	    	        object_toggle_select(widget, event, NULL, 0);
		}
		break;

	    case 3:
		if (event->type == ButtonPress)
		    object_popup_menu(widget, event, NULL, 0);
		break;
	}
    }
    else if (event->type == MotionNotify)
    {
	mevent = (XMotionEvent*)event;
        if (mevent->state & Button1Mask)
	{
	    /* Check to see if the drag occurred while we are waiting for hte
	     * double-click interval to expire. If so, then if the drag is
	     * outside the threshold, cancel the double-click and initiate
	     * a move or resize.
	     */
	    if (d_click->waiting)
	    {
		if (abs(mevent->x - d_click->start_x) > AB_drag_threshold ||
		    abs(mevent->y - d_click->start_y) > AB_drag_threshold)
	    	{
                     XtRemoveTimeOut(timer_id);
                     d_click->waiting = False;
    	             object_button_press(d_click->widget, d_click->event, NULL, 0);
		}
	    }
	    else /* call the generic drag event-handler to process the drag */
		object_button_drag(widget, client_data, event, cont_dispatch);
	}
	else if (mevent->state & Button2Mask && AB_btn1_transfer != True)
	    object_button_drag(widget, client_data, event, cont_dispatch);

	/* absorb all motion events */
	*cont_dispatch = False;
    }
}

/*
 * EventHandler: drag action ...
 */
static void
object_button_drag(
    Widget 	widget,
    XtPointer 	client_data,
    XEvent 	*event,
    Boolean 	*cont_dispatch
)
{
    XMotionEvent	*mevent;
    ABObj             	obj = (ABObj)client_data;
    ABObj		moveObj;
 
    if (event->type == MotionNotify)
    {
	mevent = (XMotionEvent*)event;

	if (obj_is_item(obj))
	    obj = obj_get_root(obj_get_parent(obj));

	if (mevent->state & Button1Mask ||
	    (AB_btn1_transfer != True && mevent->state & Button2Mask)) 
						/* RESIZE, MOVE or RUBBERBAND-SELECT */
        {
            if (resize_in_progress)
            {
                abobjP_resize_object_outline(obj, event, resize_dir);
            }
	    else if (potential_move)
            {
		/* If moving a Control within a Group, move the whole
		 * Group instead.
	     	 */
                moveObj = obj_get_root(obj);
                while(obj_is_group(obj_get_root(obj_get_parent(moveObj))))
                    moveObj = obj_get_root(obj_get_parent(moveObj));
 
                if (moveObj != obj_get_root(obj)) /* moveObj is translated to Group */
                {
		    Widget orig_w;
		    int    trans_x, trans_y;
		    Window win;

		    /* Translate obj/widget to be the Group */
                    obj = objxm_comp_get_subobj(moveObj, AB_CFG_PARENT_OBJ);
		    orig_w = widget;
                    widget = objxm_get_widget(obj);

		    /* Translate Control's x,y position relative to Group 0,0 */
        	    XTranslateCoordinates(XtDisplay(orig_w), 
			XtWindow(orig_w), XtWindow(widget),
            		mevent->x, mevent->y, &trans_x, &trans_y, &win);

		    mevent->x = trans_x;
		    mevent->y = trans_y;
                }
		if (!move_in_progress) /* Initialize Move operation */
		{
                    if (!obj_is_selected(moveObj))
                        object_select(widget, NULL, NULL, 0);
 
                    if (initiate_move(widget, obj) == ERROR)
                    {
                    	move_in_progress = False;
			potential_move   = False;
                        util_dprintf(0, "object_select_drag: couldn't begin move\n");
                    }
                    else
                        move_in_progress = True;
		}
                abobjP_move_object_outline(obj, mevent);
                just_moved = True;
            }
	    else if (mselect_in_progress) /* RUBBERBAND-SELECT */
            	ui_button_drag(widget, event, (XtPointer)obj);
        }
        else if (mevent->state & Button2Mask) /* RUBBERBAND-ADJUST */
	    ui_button_drag(widget, event, (XtPointer)obj);
    }
}


/*
 * EventHandler: object move action is completed...
 *         move the object!
 */
static void
object_move_release(
    Widget       widget, 
    XtPointer    client_data, 
    XEvent       *event, 
    Boolean      *cont_dispatch
)
{
    ABObj obj         = (ABObj)client_data;

    if (event->type != ButtonRelease)
        return;

    if (move_in_progress)
    {
        XtUngrabPointer(widget, CurrentTime);
        XtRemoveEventHandler(widget, ButtonReleaseMask, False, 
            object_move_release, (XtPointer)obj);
        move_in_progress = False;
        if (just_moved)
        {
            abobj_move(obj, event);
        }
    }
 
}

/*
 * EventHandler: object resize action is completed...
 *          resize the object!
 */
static void
object_resize_release(
    Widget 	widget,
    XtPointer 	client_data,
    XEvent 	*event,
    Boolean 	*cont_dispatch
)
{
    ABObj obj = (ABObj)client_data;

    if (!resize_in_progress || event->type != ButtonRelease)
        return; 

    XtUngrabPointer(widget, CurrentTime);
    XtRemoveEventHandler(widget, ButtonReleaseMask, False,
            object_resize_release, (XtPointer)obj);
    resize_in_progress = False;
    abobj_resize(obj, event);
    *cont_dispatch = False;
 
}

static void
object_mselect(
    Widget      widget,
    XEvent      *event,
    XRectangle  *rb_rect,
    XtPointer   client_data
)
{
    ABObj      	control,
		pobj,
		xyobj,
		obj = NULL;
    ABSelectedRec old_sel, new_sel;
    int   	num_controls; 
    XRectangle 	c_rect;
    XRectangle 	tmp_rect;
    int		i, j;

    if (!mselect_in_progress)
	return;

    obj = (ABObj)client_data;

    pobj = objxm_comp_get_subobj(obj, AB_CFG_PARENT_OBJ);
    num_controls = obj_get_num_children(pobj);
    new_sel.list = (ABObj*)util_malloc(num_controls*sizeof(ABObj));
    new_sel.count = 0;

    abobj_get_selected(obj_get_window(pobj), True, False, &old_sel);

    /* If rubberband was drawn from lower-right to upper-left,
     * translate rect so that x,y is upper-left point in rectangle.
     */
    if (rect_right(rb_rect) < rb_rect->x ||
	rect_bottom(rb_rect) < rb_rect->y)
    {
	tmp_rect.x = rect_right(rb_rect);
	tmp_rect.y = rect_bottom(rb_rect);
	tmp_rect.width = rb_rect->x - tmp_rect.x;
	tmp_rect.height = rb_rect->y - tmp_rect.y;
	rb_rect = &tmp_rect;
    }

    for (i = 0; i < num_controls; i++)
    {
        control = obj_get_child(pobj, i);

        if (obj_is_salient(control))
        {
            xyobj = objxm_comp_get_subobj(control, AB_CFG_POSITION_OBJ);

            if (objxm_get_widget(xyobj) != NULL)
            {
                x_get_widget_rect(objxm_get_widget(xyobj), &c_rect);

                if (rect_includesrect(rb_rect, &c_rect))
		{
		    /* 
		     * If RUBBERBAND-SELECT, deselect other currently selected 
		     * objects.
		     * If RUBBERBAND-ADJUST, deselect only currently selected
		     * NON-sibling objects
		     */
		    if (old_sel.count > 0)
		    {
        	    	for(j = 0; j < old_sel.count; j++) 
        	    	{ 
            		    if (control != old_sel.list[j] &&
				(!mselect_adjust ||
				!obj_is_sibling(control, old_sel.list[j])))
                	    	abobj_deselect(old_sel.list[j]);
        	    	} 
			old_sel.count = 0;
		    }
		    new_sel.list[new_sel.count] = control;
		    new_sel.count++;
		}
            }
        }
    }
    for (i=0; i < new_sel.count; i++)
        abobj_select(new_sel.list[i]);

    util_free(old_sel.list);
    util_free(new_sel.list);

    mselect_in_progress = mselect_adjust = False;
}


static void
prevent_closeCB(
    Widget	widget,
    XtPointer   client_data,
    XtPointer   call_data
)
{
   dtb_palette_prevent_close_msg_initialize(
		&dtb_palette_prevent_close_msg); 
    dtb_show_message(widget, &dtb_palette_prevent_close_msg, NULL, NULL);
}

static void
object_track_move(
    Widget      widget,
    XtPointer   client_data,
    XEvent      *event,
    Boolean     *cont_disp
)
{
    ABObj       	winobj = (ABObj)client_data;
    XConfigureEvent	*cevent;

    if (event->type == ConfigureNotify)
    {
	cevent = (XConfigureEvent*)event;
	winobj->x = cevent->x;
	winobj->y = cevent->y;
    }
}

static void
object_track_iconify(
    Widget	widget,
    XtPointer	client_data,
    XEvent	*event,
    Boolean	*cont_disp
)
{
    ABObj	winobj = (ABObj)client_data;

    switch (event->type)
    {
        case MapNotify: 
	    /* 
	     * If this event is a result of the remap that occurs when
	     * window decorations are changed going to/from Test mode, then
	     * do not interpret it as a de-iconification.
	     */
	    if (obj_has_flag(winobj, DecorChangedFlag))
		obj_clear_flag(winobj, DecorChangedFlag);

	    /* 
	     * If this event is a result of the remap that occurs when
	     * windows are maped exiting from Test mode, then do not
	     * interpret it as a de-iconification.
	     */
	    else if (obj_has_flag(winobj, TestModeWinFlag))
		obj_clear_flag(winobj, TestModeWinFlag);
	    
	    else if (InBuildMode && 
		obj_has_flag(winobj, IconifiedFlag) &&
		!ab_window_leader_iconified())
	    {
	    	obj_clear_flag(winobj, IconifiedFlag);
		XtVaSetValues(widget, XtNinitialState, NormalState, NULL);
	    }
            break;
 
        case UnmapNotify:
            /* 
             * If this event is a result of the unmap that occurs when
             * window decorations are changed going to/from Test mode, then 
             * do not interpret it as an iconification. 
             */ 
	    if (obj_has_flag(winobj, DecorChangedFlag))
		obj_clear_flag(winobj, DecorChangedFlag);

	    /* 
	     * If this event is a result of the unmap that occurs when
	     * windows are unmaped exiting from Test mode, then do not
	     * interpret it as an iconification.
	     */
	    else if (obj_has_flag(winobj, TestModeWinFlag))
		obj_clear_flag(winobj, TestModeWinFlag);
	    
	    else if (InBuildMode && 
		!obj_has_flag(winobj, IconifiedFlag) &&
		 obj_has_flag(winobj, MappedFlag) &&
		!ab_window_leader_iconified())
	    {
	    	obj_set_flag(winobj, IconifiedFlag);
		XtVaSetValues(widget, XtNinitialState, IconicState, NULL);
	    }
            break;
    }
}
