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
 *      $XConsortium: pal_create.c /main/4 1996/07/25 09:19:49 mustafa $
 *
 *      @(#)pal_create.c	1.102 19 May 1995
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
 * pal_create.c - Implements object creation (dragging & dropping items
 *		 off the palette)
 *
 ***********************************************************************
 */
#include <stdio.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/BulletinB.h>
#include <Xm/CascadeB.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/SelectioB.h>
#include <Xm/Scale.h>
#include <Dt/SpinBox.h>
#include <Dt/ComboBox.h>
#include <ab_private/trav.h>
#include <ab_private/objxm.h>
#include <ab_private/brws.h>
#include <ab_private/proj.h>
#include <ab_private/abobj.h>
#include <ab_private/abobj_set.h>
#include <ab_private/abobj_edit.h>
#include <ab_private/ab.h>
#include <ab_private/pal.h>
#include <ab_private/prop.h>
#include <ab_private/ui_util.h>
#include <ab_private/x_util.h>
#include "dtbuilder.h"
#include "palette_ui.h"

const int AB_max_item_height = 64;
const int AB_max_item_width  = 256;

/*
 * Default (x,y) positions of objects when dragged out
 * from palette. This is only used when the objects are not
 * dropped onto the interface created, but dropped instead 
 * onto the browser.
 */
#define AB_DEFAULT_X	0
#define AB_DEFAULT_Y	0

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
static void     palitem_update_type(
                    Widget w,
                    XtPointer cd,
                    XEvent *ev,
                    Boolean *cont
                );
static void	palitem_select_event( 
		    Widget w, 
		    XtPointer cd, 
		    XEvent *ev, 
		    Boolean *cont
		);
static  void    palitem_drag_action( 
		    Widget w, 
		    int    subtype,
		    XEvent *event 
		);
static void     palitem_release_event(
		    Widget w, 
		    XtPointer cd, 
		    XEvent *ev, 
		    Boolean *cont
		);
static void     create_obj_action(
		    Widget w, 
		    int    subtype,
		    XEvent *event
		);
static char 	*locate_obj_parent(
		    ABObj obj, 
		    Widget widget, 
		    int x, 
		    int y,
		    ABObj *parentptr,
		    BOOL  *ModuleCreated
		);
static Boolean	initiate_drag(
		    Widget w, 
		    int    subtype,
		    int *x, 
		    int *y, 
		    int *xhot, 
		    int *yhot
		);
static XImage   *build_item_image(
		    Widget w
		);
static void     image_copy(
		    Widget w, 
		    XImage *from, 
		    XImage *to, 
		    int width, 
		    int gx, 
		    int px
		);
ABObj		locate_obj_at_rootxy(
        	    int     x,
        	    int     y,
        	    int     *p_wx,
        	    int     *p_wy
		);


/*************************************************************************
**                                                                      **
**       Private Data                                                   **
**                                                                      **
**************************************************************************/
static Widget 	palette_widget = NULL; 
static Cursor	drag_cursor;
static int	drag_cursor_xhot;
static int	drag_cursor_yhot;
static Boolean	drag_on = FALSE;

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/
/*
 * Set up palette item(widget) to handle select-drag operation
 */
void
pal_enable_item_drag(
    Widget     	widget,
    int		subtype
)
{
    static int st_subtype;

    st_subtype = subtype;
    XtInsertEventHandler(widget, ButtonPressMask, FALSE,
            palitem_select_event, (XtPointer)&st_subtype, XtListHead);
    XtInsertEventHandler(widget, ButtonReleaseMask, FALSE,
            palitem_release_event, (XtPointer)&st_subtype, XtListHead);
    XtAddEventHandler(widget, EnterWindowMask|LeaveWindowMask, FALSE,
	    palitem_update_type, (XtPointer)&st_subtype);

    /* We need to keep track of the main palette widget
     * for drag-behavior purposes
     */
    if (!palette_widget)
    {
	if ((palette_widget = dtb_palette_ab_palette_main.palette_cpanel) == NULL)
	{
	    if (util_get_verbosity() > 2)
		fprintf(stderr,"pal_enable_item_drag: couldn't find \"%s\"\n",
			XtName(dtb_palette_ab_palette_main.palette_cpanel));
	    palette_widget = XtParent(XtParent(widget));
	}
    }
}
static void
palitem_update_type(
    Widget       w,
    XtPointer clientData,
    XEvent       *event,
    Boolean   *cont_dispatch
)
{
    PalItemInfo   *palitem;
    int		  subtype = *((int*)clientData);
    String	  objname;
    int		  i;

    XtVaGetValues(w, XmNuserData, &palitem, NULL);

    if (event->type == EnterNotify)
    {
	objname = palitem->name;  

	for(i=0; i < palitem->num_subinfo; i++)
	    if (palitem->subinfo[i].subtype == subtype)
	    {
		objname = palitem->subinfo[i].subname;
		break;
	    }
		
	ab_update_stat_region(AB_STATUS_OBJ_TYPE, objname);
    }
    else /* LeaveNotify */
	ab_update_stat_region(AB_STATUS_OBJ_TYPE, "     ");

}

/*
 * EventHandler: ButtonPress activated on palette item
 */
static void
palitem_select_event(
    Widget       w, 
    XtPointer clientData, 
    XEvent       *event, 
    Boolean   *cont_dispatch
)
{
    XButtonEvent *bevent;
    int 	 subtype = *((int*)clientData);

    if (event->type != ButtonPress)
	return;

    bevent = (XButtonEvent*)event;
    *cont_dispatch = FALSE;

    if (AB_builder_mode != MODE_BUILD)
	return;

    if (bevent->button == 1 || 
	(AB_btn1_transfer != True && bevent->button == 2))
        palitem_drag_action(w, subtype, event);

}

/*
 * Action Proc: Palette item has been selected - begin drag
 */
static void
palitem_drag_action(
    Widget     widget, 
    int	       subtype,
    XEvent     *event 
)
{
    int x, y;
    Display *dpy = XtDisplay(widget);

    if (event->type == ButtonPress)
    {
        XButtonEvent *bevent = (XButtonEvent*)event;
 
        x = bevent->x;
        y = bevent->y;
 
        if (!initiate_drag(widget, subtype, &x, &y, &drag_cursor_xhot, &drag_cursor_yhot))
        {
            drag_on = FALSE;
            if (util_get_verbosity() > 2)
                fprintf(stderr, "palitem_drag_action: couldn't begin drag\n");
        }
        else
            drag_on = TRUE;
    }

}

/*
 * EventHandler: ButtonRelease activated on palette item
 */
static void 
palitem_release_event(
    Widget       w, 
    XtPointer clientData, 
    XEvent       *event, 
    Boolean      *cont_dispatch
) 
{
    XButtonEvent *bevent;
    int 	 subtype = *((int*)clientData);

    if (event->type != ButtonRelease)
	return;

    bevent = (XButtonEvent*)event;
    *cont_dispatch = FALSE;

    if (AB_builder_mode != MODE_BUILD)
	return;

    if (bevent->button == 1 || 
	(AB_btn1_transfer != True && bevent->button == 2))
        create_obj_action(w, subtype, event);
 
} 


/*
 * Action Proc: Palette item has been released; if drag was taking
 *    place, release the pointer grab and create object
 */
static void
create_obj_action(
    Widget     widget, 
    int	       subtype,
    XEvent     *event
) 
{ 
    ABObj	  project = proj_get_project();
    ABObj         obj = NULL;
    ABObj	  obj_parent = NULL;
    PalItemInfo	  *palitem;
    Display       *dpy;
    char          *errmsg = NULL;
    BOOL	  ModuleCreated = FALSE;
    XmString	  xm_buf = (XmString) NULL;

    dpy = XtDisplay(widget);

    if (drag_on)
    {
        XUngrabPointer(dpy, CurrentTime);
        XFreeCursor(dpy, drag_cursor);
        drag_on = FALSE;
    }

    if (event->type == ButtonRelease)
    {
        Window	     xy_win;
	int	     wx, wy;
        XButtonEvent *bevent = (XButtonEvent *)event;
        XtVaGetValues(widget, XmNuserData, &palitem, NULL);

   	/* Creation may take awhile, so set busy cursor */
        xy_win = x_xwin_at_rootxy(AB_toplevel, bevent->x_root, 
			bevent->y_root, &wx, &wy);
	ab_set_busy_cursor(TRUE);
	ui_set_busy_cursor(xy_win, TRUE);

        obj = obj_create(palitem->type, NULL);

	if (palitem->type == AB_TYPE_SCALE)
	    obj_set_read_only(obj, subtype);

	if (subtype != AB_NO_SUBTYPE)
	    obj_set_subtype(obj, subtype);

        if (errmsg = locate_obj_parent(obj, widget, bevent->x_root, 
		bevent->y_root, &obj_parent, &ModuleCreated))
        {
            obj_destroy(obj);
            if (!util_streq(errmsg, ""))
	    {
		xm_buf = XmStringCreateLocalized(errmsg);
		dtb_palette_error_msg_initialize(&dtb_palette_error_msg);
		(void)dtb_show_modal_message(dtb_get_toplevel_widget(), 
			&dtb_palette_error_msg, xm_buf, NULL, NULL);
		XmStringFree(xm_buf);
	    }
        }
        else
        {
            if (pal_initialize_obj(obj) == ERROR)
                fprintf(stderr, "create_obj_action: couldn't initialize object\n");
	    else if (abobj_show_tree(obj, TRUE) == -1)
                fprintf(stderr,"create_obj_action: couldn't show object\n");
	    else
	    {
		/* set the initial visiblity of new layers to false */
		if (obj_is_layers(obj_parent))
		    obj_set_is_initially_visible(obj, FALSE);
		
		/* Deselect any objects that happen to be selected */
		abobj_deselect_all(project);
                aob_deselect_all_objects(project);

		/* Make the new obj selected */
		abobj_select(obj);

		/* Set the dirty bit on the module */
		abobj_set_save_needed(obj_get_module(obj), TRUE);

		/* If the window is the first one dragged out and 
         	 * its parent (the module) has not been named yet, 
		 * popup the module name dialog.  The only time a
		 * module will have not been named by the user is
		 * when it is created at the time the first window
		 * is dragged from the palette.
         	 */
        	if( obj_is_window(obj) && ModuleCreated )
        	{
		    /* Pop up the name dialog */
		    proj_show_name_dlg(obj_parent, objxm_get_widget(obj));
		}
		
		/*
		 * Once an object is successfully dragged out from the
		 * palette, disable undo. Object creation is not undo-able.
		 */
		abobj_cancel_undo();
	    }
        }
	/* Restore to original cursor */
	ab_set_busy_cursor(FALSE);
        ui_set_busy_cursor(xy_win, FALSE);
    }
}

/*
 * Make a cursor from the palette item's image & cursor
 * and grab the cursor...
 */
static Boolean
initiate_drag(
    Widget     w, 
    int	    subtype,
    int     *x, 
    int     *y, 
    int     *xhot, 
    int     *yhot
)
{

    static unsigned int	  max_c_width = 0; /* The maximum cursor size */
    static unsigned int	  max_c_height = 0;
    static Pixmap 	  item_pixmap = 0;/* pixmap for cursor */
    static GC     	  p_gc = NULL;       /* GC used for pixmap creation */
    char		  *server_vendor = NULL;
    PalItemInfo   	  *palitem;            /* palette obj */
    PalSubtypeInfo 	  *palitem_subptr = NULL;
    Widget        	  item_widget = w;   /* widget used for cursor image */
    XRectangle    	  w_rect;            /* widget width,height,x,y */
    int           	  c_width, c_height; /* cursor width & height */
    int            	  cc_width, cc_height;/* copycursor width & height */
    Display       	  *dpy;
    Window        	  win;
    int		  	  i;

    if (max_c_width == 0) /* Need to Query server's Cursor constraints */
    {
	server_vendor = XServerVendor(XtDisplay(w));

	if (strcmp(server_vendor, "Sun Microsystems, Inc.") == 0)
	{
	    /* 
	     * On a Sun X server, a call to XQueryBestSize() will return a limit
	     * based on a *hardware* cursor;  this is misleading because the 
	     * server actually supports a much larger software cursor (only limited
	     * by the screen size); therefore set the limit based on screen size.
	     */
    	    max_c_width = WidthOfScreen(XtScreen(w));
    	    max_c_height = HeightOfScreen(XtScreen(w));
	}
	else
	    /* Ask for a big size to get the maximum */
	    XQueryBestSize(XtDisplay(w), CursorShape, XtWindow(w), 1000, 1000, 
		&max_c_width, &max_c_height);

	util_dprintf(0, "For Server(%s) Max Cursor size = %dx%d\n", 
		server_vendor, max_c_width, max_c_height);
	/* 
	 * Need to leave room for the outlining of the object's image
	 * which is taken care of in x_create_stencil_cursor().
 	 */
	max_c_width -= 2;
	max_c_height -= 2;
    }

    XtVaGetValues(w, 
        		XmNuserData,	&palitem, 
            		XtNwidth,       &(w_rect.width),
            		XtNheight,      &(w_rect.height), 
        		XtNx,    	&(w_rect.x),
            		XtNy,       	&(w_rect.y), 
        		NULL);
    /*
     * Create Drag Cursor containing item's image
     */

    /*
     * If item is a choice setting or textfield, need to
     * use the parent container's image for the cursor;
     * also must translate the cursor x,y position to
     * the parent's rectangle coordinates
     *
     * NOTE:
     * For SpinBox, this is done if the widget passed
     * is not a subclass of spinbox. (We cannot assume
     * what these widgets are composed of).
     */
    if ((palitem->type == AB_TYPE_CHOICE &&
            XtIsSubclass(w, xmToggleButtonWidgetClass)) || 
	(palitem->type == AB_TYPE_CONTAINER  &&
	    XtIsSubclass(w, xmCascadeButtonWidgetClass)) ||
        (palitem->type == AB_TYPE_TEXT_FIELD && 
            (XtIsSubclass(w, xmLabelWidgetClass) ||
                        XtIsSubclass(w, xmTextFieldWidgetClass))) ||
	(palitem->type == AB_TYPE_SPIN_BOX && 
	    !XtIsSubclass(w, dtSpinBoxWidgetClass)) ||
	(palitem->type == AB_TYPE_COMBO_BOX && 
	    !XtIsSubclass(w, dtComboBoxWidgetClass)) ||
	(palitem->type == AB_TYPE_SCALE &&
            !XtIsSubclass(w, xmScaleWidgetClass)))
    {
        item_widget = XtParent(w);

        *x +=  w_rect.x;
        *y +=  w_rect.y;

        XtVaGetValues(item_widget,
            		XmNuserData,    (XtArgVal)&palitem,
                        XtNwidth,       (XtArgVal)&(w_rect.width), 
                        XtNheight,      (XtArgVal)&(w_rect.height),  
            		NULL);
    }
        
    dpy = (Display *)XtDisplay(item_widget);
    win = (Window)   XtWindow(item_widget);

    cc_width  = AB_cp_cursor_width;
    cc_height = AB_cp_cursor_height;

    if (dtb_app_resource_rec.use_small_drag_cursor == FALSE)
    {
    	c_width   = AB_max_item_width  + cc_width  + 4;
    	c_height  = AB_max_item_height + cc_height + 4;
    }
    else /* Need Drag Cursor <= 32x32 */
    {
	c_width   = 28;
	c_height  = 28;
    }

    /* 
     * Ensure cursor is within server limits
     * Note that on some servers (HP, IBM) the cursor limitation
     * will cause some object drag-cursors to be cropped.
     * Unfortunately, for 1.0, there is nothing we can do about
     * this.
     */
    if (c_width > max_c_width)
	c_width = max_c_width;
    if (c_height > max_c_height)
	c_height = max_c_height;

    if (!p_gc) /* First-time thru: Create Cursor pixmaps & GC */
    {
	XGCValues	   values;
	Pixel	   fg_pixel, bg_pixel;

	item_pixmap = XCreatePixmap(dpy, win, c_width, c_height, 1);
 
	fg_pixel = BlackPixelOfScreen(XtScreen(item_widget));
	bg_pixel = WhitePixelOfScreen(XtScreen(item_widget));
 
	values.foreground = fg_pixel;
	values.background = bg_pixel;
	p_gc = XCreateGC(dpy, item_pixmap, GCForeground|GCBackground, &values);
    }

    /* Find the appropriate palette pixmap for the object subtype */
    for (i=0; i < palitem->num_subinfo; i++)
	if (palitem->subinfo[i].subtype == subtype)
	{
	    palitem_subptr = &(palitem->subinfo[i]);
	    break;
	}

    /* This should never happen! */
    if (palitem_subptr == NULL)
    {
	util_dprintf(0,"initiate_drag: could not find palette object subtype info\n");
	return FALSE;
    }

    if (!palitem_subptr->pixmap)
    {
	XImage    *item_image;
	    
	item_image = build_item_image(item_widget);
	/* no error recovery here. Do we care? */

	palitem_subptr->pmwidth  = w_rect.width  + 4;
	palitem_subptr->pmheight = w_rect.height + 4;
	palitem_subptr->pixmap = XCreatePixmap(dpy, win, palitem_subptr->pmwidth, 
	        palitem_subptr->pmheight, 1);

	XSetFunction(dpy, p_gc, GXclear);
	XFillRectangle(dpy, palitem_subptr->pixmap, p_gc, 0, 0, 
	        w_rect.width + 4, w_rect.height + 4);

	XSetFunction(dpy, p_gc, GXcopy);
	XPutImage(dpy, palitem_subptr->pixmap, p_gc, item_image, 0, 0,
	    2, 2, item_image->width, item_image->height);
    
	XDestroyImage(item_image);
    }

    XSetFunction(dpy, p_gc, GXclear);
    XFillRectangle(dpy, item_pixmap, p_gc, 0, 0, c_width, c_height);

    XSetFunction(dpy, p_gc, GXcopy);
    if (dtb_app_resource_rec.use_small_drag_cursor == FALSE)
    {
	/* Copy the item's image pixmap onto the cursor_pixmap */
    	XCopyArea(dpy, palitem_subptr->pixmap, item_pixmap, p_gc, 0, 0,
	    c_width, c_height, 0, 0);

    	*xhot = *x + 2;
    	*yhot = *y + 2;
    }
    else  /* dtb_app_resource_rec.use_small_drag_cursor == TRUE */
    {
	int rw, rh;
	BOOL is_win = FALSE;

	/* Render a simple shape describing basic type of 
	 * object is being dragged....
	 */
	switch(palitem->type)
	{
	    case AB_TYPE_BASE_WINDOW:
	    case AB_TYPE_DIALOG:
		rw = 25;
		rh = 16;
		is_win = TRUE;
		*xhot = 6;
		*yhot = 6;
		break;
	    case AB_TYPE_CONTAINER:
	    case AB_TYPE_DRAWING_AREA:
	    case AB_TYPE_TEXT_PANE:
	    case AB_TYPE_TERM_PANE:
		rw = 24;
		rh = 15;
		*xhot = 6;
		*yhot = 6;
		break;
	    default: /* Controls */
		rw = 24;
		rh = 10;
		*xhot = 5;
		*yhot = 5;
	}
	XDrawRectangle(dpy, item_pixmap, p_gc, 0, 0, rw, rh);
	if (is_win)
	    XDrawLine(dpy, item_pixmap, p_gc, 0, 4, rw, 4);

    }

    /* overlay image of copycursor on item image */
    XSetFunction(dpy, p_gc, GXor);
    x_graphics_op(dpy, item_pixmap, p_gc, *xhot, *yhot,
                        cc_width, cc_height, AB_cp_cursor_pixmap);

    /*
     * Ensure hot-spot is within server cursor size
     */
    if (*xhot > c_width)
	*xhot = c_width - 4;
    if (*yhot > c_height)
	*yhot = c_height - 4;

    drag_cursor = x_create_stencil_cursor(item_widget, item_pixmap,
            c_width, c_height, *xhot, *yhot);

    if (XGrabPointer(dpy, win, FALSE, 
            ButtonReleaseMask | PointerMotionMask,
            GrabModeAsync, GrabModeAsync, None, drag_cursor,
            CurrentTime) == GrabSuccess)
        return TRUE;
    else
        XFreeCursor(dpy, drag_cursor);

    return FALSE;

}

/*
 * Get a 1-bit image of the palette item
 */
static XImage *
build_item_image(
    Widget widget
)
{
    XRectangle   	w_rect;
    Window            	win;
    Display    		*dpy;
    XImage        	*get_image, 
			*new_image;
    XWindowAttributes   attr;
    char            	*new_image_data;

    /*
     * Get the item's bounding rect and other associated information.
     */
    x_get_widget_rect(widget, &w_rect);

    win   = (Window)XtWindow(widget);
    dpy   = (Display *)XtDisplay(widget);

    /*
     * Get the item's image and it's attributes (including visual).
     */
    get_image = XGetImage(dpy,
                  win,
                  0,
                  0,
                  w_rect.width,
                  w_rect.height,
                  AllPlanes,
                  XYPixmap);

    XGetWindowAttributes(dpy, win, &attr);

    /*
     * Create a new blank image.
     */
    new_image_data = (char *)XtMalloc((w_rect.width * w_rect.height) * sizeof(char));
    new_image = XCreateImage(dpy,
                 attr.visual,
                 1,
                 XYBitmap,
                 0,
                 new_image_data,
                 w_rect.width,
                 w_rect.height,
                 8,
                 0);

    /*
     * Copy the specified planes from the "get" image to the new image.
     */
    image_copy(widget, get_image, new_image, w_rect.width, 0, 0);

    XDestroyImage(get_image);
    return(new_image);
}


/*
 * Copy item's outline image to a 1 bit image
 */
static void
image_copy(
    Widget     widget, 
    XImage     *from_image, 
    XImage     *to_image, 
    int     image_width, 
    int     get_x, 
    int     put_x
)
{
    int    i, j, k, pix;
    Pixel    	    background;
    Pixel    	    foreground;
    Pixel           bshadow, tshadow;
    Pixel	    black, white;
 
    /*
     * Only draw bits that are not part of the background and, if
     * color, the depression color (BG2).
     */
    XtVaGetValues(widget, 
        	XtNbackground, 		&background, 
        	XtNforeground,    	&foreground,
        	XmNbottomShadowColor, 	&bshadow,
        	XmNtopShadowColor, 	&tshadow,
        	NULL);

    black = BlackPixelOfScreen(XtScreen(widget));
    white = WhitePixelOfScreen(XtScreen(widget));
 
    for (i = 0; i < from_image->height; i++)
    {
        for (j = get_x, k = put_x; j < image_width; j++, k++)
        {
            pix = XGetPixel(from_image, j, i);
            if ((pix == foreground || 
        	pix == bshadow ||
                pix == tshadow ||
		pix == black || 
		pix == white) &&
		pix != background)
		XPutPixel(to_image, k, i, black);
	    else
		XPutPixel(to_image, k, i, white);
         }
    }
}

/*
 * Figure out which ui object the palette item was dropped on
 */
static char *
locate_obj_parent(
    ABObj   obj, 
    Widget  widget, 
    int     x, 
    int     y,
    ABObj   *parentptr,
    BOOL    *ModuleCreated
)
{
    ABObj		project = proj_get_project();
    ABObj		module  = proj_get_cur_module();
    ABObj		obj_parent = (ABObj) NULL;
    Display     	*dpy = (Display *)XtDisplay(widget);
    int         	w_x, w_y;
    DTB_MODAL_ANSWER	answer = DTB_ANSWER_NONE;
    STRING		errmsg = (STRING) NULL;
    STRING		i18n_msg = (STRING) NULL;

    *ModuleCreated = FALSE;

    if (obj_is_window(obj))
    {
    	if (x_rootxy_inside_widget(palette_widget, x, y))
            return "";

	if ( module == NULL )
	{
	    obj_parent = obj_create(AB_TYPE_MODULE, project);
	    obj_set_name(obj_parent, "module");
	    abobj_show_tree(obj_parent, FALSE);
	    proj_set_cur_module(obj_parent);
	    *ModuleCreated = TRUE;
	}
	else
	{
	    obj_parent = module;
	}
	obj_append_child(obj_parent, obj);

        obj->x = x - drag_cursor_xhot;
        obj->y = y - drag_cursor_yhot;
    }
    else 
    {
	obj_parent = locate_obj_at_rootxy(x, y, &w_x, &w_y);

        obj->x = w_x - drag_cursor_xhot;
        obj->y = w_y - drag_cursor_yhot;

	/* If not dropped on an object, but is still within
	 * palette, just ignore the drag...
	 */
        if (obj_parent == NULL &&
	    x_rootxy_inside_widget(palette_widget, x, y))
	    return "";

	if (obj_is_menubar(obj))
	{
 	    AB_TRAVERSAL	trav;
	    ABObj 		winobj;
	    ABObj		nobj;

	    if (obj_parent != NULL)
	    {
	    	winobj = obj_parent;
	    	while(!(obj_is_window(winobj) && !obj_is_sub(winobj)))
		    winobj = obj_get_parent(winobj);

		if (!obj_is_base_win(winobj))
		    obj_parent = NULL;
		else /* Check to make sure there isn't already a Menubar for
		      * this window
		      */
		{
		    for (trav_open(&trav, winobj, AB_TRAV_UI);
		        (nobj = trav_next(&trav)) != NULL; )
			if (obj_is_menubar(nobj))
			{
			    i18n_msg = catgets(Dtb_project_catd, 100, 11,
				"There is already a Menubar for this window.");

			    /* If we have an old buffer lying around, free it */
			    if (errmsg != (STRING) NULL) 
				util_free(errmsg);

			    errmsg = (STRING) util_malloc(strlen(i18n_msg) + 1);
			    strcpy(errmsg, i18n_msg); 
			    return (errmsg);
			}

		    obj_parent = objxm_comp_get_subobj(winobj, AB_CFG_MENU_PARENT_OBJ);
		}
	    }
	    if (obj_parent == NULL)
	    {
                i18n_msg = catgets(Dtb_project_catd, 100, 15,
			"Menubars must be dropped on a Main Window.");

                /* If we have an old buffer lying around, free it */
                if (errmsg != (STRING) NULL)
                    util_free(errmsg);

                errmsg = (STRING) util_malloc(strlen(i18n_msg) + 1);
                strcpy(errmsg, i18n_msg);
                return (errmsg);
	    }

	}
        else if (obj_is_container(obj) && !obj_is_control_panel(obj))
        {
            if (obj_parent == NULL ||
		(!(obj_is_container(obj_parent) &&
                obj_is_sub(obj_parent) &&
                obj_is_window(obj_get_root(obj_parent)) &&
		!obj_is_file_chooser(obj_parent))))
	    {
		i18n_msg = catgets(Dtb_project_catd, 100, 12,
			"Containers must be dropped on\na Main Window or Custom Dialog."); 
		/* If we have an old buffer lying around, free it */     
		if (errmsg != (STRING) NULL)
		    util_free(errmsg);
 
		errmsg = (STRING) util_malloc(strlen(i18n_msg) + 1);
		strcpy(errmsg, i18n_msg);
                return (errmsg);
	    }
        }
        else if (obj_is_pane(obj))
        {
	    if (obj_parent != NULL && 
		(obj_is_pane(obj_parent) &&
		 !obj_is_window(obj_get_root(obj_parent))))
	    {
		ABObj	grandparent = NULL;
		ABObj	great_grandparent = NULL;
		grandparent = obj_get_parent(obj_get_root(obj_parent));
		great_grandparent = obj_get_parent(grandparent);

                /* The child is a textpane, termpane, or drawing area
                 * and the parent (object being dropped upon) is a   
                 * control pane that is not part of a panedWindow.
                 */ 
                if (!obj_is_control_panel(obj) && 
                    obj_is_control_panel(obj_parent) &&
                    !obj_is_paned_win(grandparent))
		{
		    /* If the parent of obj_parent is a layered pane
		     * then we have to check if the layered pane is
		     * a child of a panedwindow. If so, then we should
		     * ask the user whether he wants to make the pane
		     * a child of the control pane, a new layer, or
		     * a new pane in the paned window.
		     */
		    if (obj_is_layers(grandparent) && 
			obj_is_paned_win(great_grandparent))
		    {
			dtb_palette_chld_layr_pw_msg_initialize(
				&dtb_palette_chld_layr_pw_msg);
			answer = dtb_show_modal_message(
				(Widget)obj_parent->ui_handle,
				&dtb_palette_chld_layr_pw_msg,
				NULL, NULL, NULL);
			switch(answer)
			{
			    /* Create as a child of the control pane */
                            case DTB_ANSWER_ACTION1:
                                break;

			    /* Create as a new layer */
                            case DTB_ANSWER_ACTION2:
                                obj_parent = abobj_handle_layered_pane(obj, obj_parent);
                                break;

			    /* Create as a new pane in the paned window */
                            case DTB_ANSWER_ACTION3:
                                obj_parent = great_grandparent;
                                break;

                            case DTB_ANSWER_CANCEL:
			        return "";
			}
		    }
		    else
		    {
    		        /* Popup Modal Message and wait for answer */
		        dtb_palette_chld_or_layr_msg_initialize(
				&dtb_palette_chld_or_layr_msg);
                        answer = dtb_show_modal_message(
				(Widget)obj_parent->ui_handle,
                                &dtb_palette_chld_or_layr_msg, 
				NULL, NULL, NULL);
    		        switch(answer)
    		        {
			    case DTB_ANSWER_ACTION1: /* As Child */
			    break;

        	    	    case DTB_ANSWER_ACTION2: /* As Layered Pane */
			    obj_parent = abobj_handle_layered_pane(obj, obj_parent);
			    break;

			    case DTB_ANSWER_CANCEL:
			    return "";
    		        }
		    }
		}
		/* The child is a control pane or the parent is
		 * not a control pane (i.e. a textpane child of
		 * a control pane). The parent is not part of a
		 * panedWindow.
		 */
		else if (!obj_is_paned_win(grandparent))
		{
		    /* If Pane was dropped on a Pane which is a NORMAL child of
		     * a Control Pane, then use the Control Pane as the actual
		     * parent instead of the pane.
		     */
		    if (obj_is_control_panel(obj_get_root(grandparent)))
		    {
			obj_parent = grandparent;
		    }
 
		    /* Check if the grandparent is a layered pane.
		     * If so, then we have to check it is part of
	  	     * a panedwindow. If it is, then we need to
		     * ask the user whether to create the pane as
		     * a layered pane or a new pane in the paned
		     * window.
		     */
		    if (obj_is_layers(grandparent) &&
			obj_is_paned_win(great_grandparent))
		    {
                        dtb_palette_layr_pw_msg_initialize(
                                &dtb_palette_layr_pw_msg);
                        answer = dtb_show_modal_message(
                                (Widget)obj_parent->ui_handle,
                                &dtb_palette_layr_pw_msg,
                                NULL, NULL, NULL);
                        switch(answer)
                        {
                            case DTB_ANSWER_ACTION1: /* Layered Pane */
                                obj_parent = abobj_handle_layered_pane(obj, obj_parent);
                                break;
 
                            case DTB_ANSWER_ACTION2: /* PanedWindow Pane */
                                obj_parent = grandparent;
				break;

                            case DTB_ANSWER_CANCEL:
                                return "";
 
                            case DTB_ANSWER_HELP:
                                break;
                        }
		    }
	 	    else
		    {
                        /* Popup Modal Message and wait for answer */
		        dtb_palette_layered_pane_msg_initialize(
				&dtb_palette_layered_pane_msg);
                        answer = dtb_show_modal_message(
				(Widget)obj_parent->ui_handle,
                                &dtb_palette_layered_pane_msg, 
				NULL, NULL, NULL);
                        switch(answer)
                        {
                    	    case DTB_ANSWER_ACTION2: /* Layered Pane */
                                obj_parent = abobj_handle_layered_pane(obj, obj_parent);
                                break;

                            case DTB_ANSWER_CANCEL: /* Cancel */
                                return "";
		        }
		    }
                }
		/* A termpane, textpane, or draw area is being dropped
		 * on another pane (not control pane) and that pane
		 * is inside a paned window.
		 *
		 * OR
		 *
		 * The object being dragged is a control pane and
		 * the parent is part of a panedWindow.
		 */
		else if (!obj_is_control_panel(obj_parent) ||
			 obj_is_control_panel(obj))
		{
		    dtb_palette_layr_pw_msg_initialize(
				&dtb_palette_layr_pw_msg);
		    answer = dtb_show_modal_message(
				(Widget)obj_parent->ui_handle,
				&dtb_palette_layr_pw_msg, 
				NULL, NULL, NULL);
                    switch(answer)
                    {
                        case DTB_ANSWER_ACTION1: /* Layered Pane */ 
			    obj_parent = abobj_handle_layered_pane(obj, obj_parent);
                            break;
 
                        case DTB_ANSWER_ACTION2: /* PanedWindow Pane */
                            obj_parent = grandparent;
			    break;

                        case DTB_ANSWER_CANCEL: 
			    return "";

                        case DTB_ANSWER_HELP:
                            break;
                    }
		}
		/* The obj is a textpane, termpane, or draw area
		 * and the parent of obj_parent is a panedWindow.
		 */
		else
		{
		    dtb_palette_chld_layr_pw_msg_initialize(
				&dtb_palette_chld_layr_pw_msg);
		    answer = dtb_show_modal_message(
				(Widget)obj_parent->ui_handle,
				&dtb_palette_chld_layr_pw_msg, 
				NULL, NULL, NULL);
                    switch(answer)
                    {
			/* Create as a child of the control pane */
                        case DTB_ANSWER_ACTION1:
                            break;

			/* Create as a new layer */
                        case DTB_ANSWER_ACTION2:
                            obj_parent = abobj_handle_layered_pane(obj, obj_parent);
                            break;

			/* Create as a new pane in the paned window */
                        case DTB_ANSWER_ACTION3:
                            obj_parent = obj_get_parent(obj_parent);
                            break;

                        case DTB_ANSWER_CANCEL:
			    return "";
                    }
                }
	    }
	    /*
	     * Dropping panes onto a group
	     */
	    else if (obj_parent != NULL && obj_is_group(obj_parent))
	    {
		if (obj_is_control_panel(obj))
		{
                    i18n_msg = catgets(Dtb_project_catd, 100, 59,
			"Control Panes must be dropped on a Main Window,\nCustom Dialog, or another pane.");

                    /* If we have an old buffer lying around, free it */
 
                    if (errmsg != (STRING) NULL)
                        util_free(errmsg);
  
                    errmsg = (STRING) util_malloc(strlen(i18n_msg) + 1);
                    strcpy(errmsg, i18n_msg); 
                    return (errmsg); 
		}
		else
		{
		    dtb_palette_child_of_group_msg_initialize(
				&dtb_palette_child_of_group_msg);
		    answer = dtb_show_modal_message(
				(Widget)obj_parent->ui_handle,
				&dtb_palette_child_of_group_msg, 
				NULL, NULL, NULL);
                    switch(answer)
                    {
			/* OK - Create as a child of group */
                        case DTB_ANSWER_ACTION1:
                            break;

                        case DTB_ANSWER_CANCEL:
			    return "";
                    }
		}
	    }
            else if (obj_parent == NULL ||
		(!(obj_is_container(obj_parent) &&
                !obj_is_menubar(obj_parent))))
	    {
                i18n_msg = catgets(Dtb_project_catd, 100, 13,
			"Panes must be dropped on a Main Window,\nCustom Dialog, or another pane.");

                /* If we have an old buffer lying around, free it */
 
                if (errmsg != (STRING) NULL)
                    util_free(errmsg);
  
                errmsg = (STRING) util_malloc(strlen(i18n_msg) + 1);
                strcpy(errmsg, i18n_msg); 
                return (errmsg); 
	    }

        }
        else if (obj_is_control(obj))
        {
	    if (obj_parent == NULL ||
		(!obj_is_control_panel(obj_get_root(obj_parent)) && 
		!obj_is_group(obj_get_root(obj_parent))))
	    {
                i18n_msg = catgets(Dtb_project_catd, 100, 14, 
			"Controls must be dropped on\na Control Pane or Group."); 
                /* If we have an old buffer lying around, free it */
                if (errmsg != (STRING) NULL) 
                    util_free(errmsg); 
  
                errmsg = (STRING) util_malloc(strlen(i18n_msg) + 1); 
                strcpy(errmsg, i18n_msg);  
                return (errmsg);
	    }

        }
        else
 	{
            i18n_msg = catgets(Dtb_project_catd, 100, 16,
			"Unknown object type.");

            /* If we have an old buffer lying around, free it */
            if (errmsg != (STRING) NULL)
                    util_free(errmsg);

            errmsg = (STRING) util_malloc(strlen(i18n_msg) + 1);
            strcpy(errmsg, i18n_msg);
            return (errmsg);
	}

        obj_append_child(obj_parent, obj);
    }

    *parentptr = obj_parent;
    return NULL;

}

ABObj
locate_obj_at_rootxy(
    int     x,
    int     y,
    int     *p_wx,
    int     *p_wy
)
{
    ABObj	    project = proj_get_project();
    ABObj           obj     = project;
    AB_TRAVERSAL    trav;
    Window     	    window;
    Widget          widget;
 
    if (obj == NULL)
        return NULL;

    window = x_xwin_at_rootxy(AB_toplevel, x, y, p_wx, p_wy);
    if (window == 0)
    	return NULL;

    for (trav_open(&trav, obj, AB_TRAV_UI);
        (obj = trav_next(&trav)) != NULL; )
    {
        widget = (Widget)obj->ui_handle;
 
        if (widget != NULL &&
            XtIsRealized(widget) &&
            XtWindow(widget) == window)
        {
            return obj;
        }
    }

    /*
     * Are we in the draw area of any browser window ?
     */
    if (aob_is_browser_win(project, window))
    {
	ABObj	browser_obj;

	/*
	 * Get the ABObj that the ptr is currently above
	 */
        browser_obj = aob_get_object_from_xy(project, window, *p_wx, *p_wy);

	if (browser_obj)
	{
	    /*
	     * Return the config parent of the ABObj.
	     * This needs to be done because the ABObj shown on the 
	     * browser and the ABObj of the interface objects as 
	     * seen by the user are different things.
	     *
	     * Also, since we are dealing with browser coordinates
	     * here (and not precise positions on the parent obj), 
	     * set the position (x,y) to some default.
	     *
	     * drag_cursor_[x,y]hot is added here because it is 
	     * subtracted later to determine the exact position
	     * of the object.
	     */
	    *p_wx = AB_DEFAULT_X + drag_cursor_xhot;
	    *p_wy = AB_DEFAULT_Y + drag_cursor_yhot;
            return (objxm_comp_get_subobj(browser_obj, AB_CFG_PARENT_OBJ));
	}
    }

    return NULL;

}
