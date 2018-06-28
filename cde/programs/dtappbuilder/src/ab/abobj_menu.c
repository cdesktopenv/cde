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
 *      $XConsortium: abobj_menu.c /main/3 1995/11/06 17:16:46 rswiston $
 *
 * @(#)abobj_menu.c	1.23 15 Feb 1994      cde_app_builder/src/libABobj
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
 * abobj_menu.c - Implements popup Menu for a UI object
 *        
 ***********************************************************************
 */
#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/CascadeB.h>
#include <Xm/MenuShell.h>
#include <Xm/RowColumn.h>	/* XmMenuPosition() */
#include <Xm/Separator.h>	
#include <ab_private/objxm.h>
#include <ab_private/ui_util.h>
#include <ab_private/brws.h>
#include <ab_private/proj.h>
#include <ab_private/prop.h>
#include <ab_private/abobj.h>
#include <ab_private/abobj_edit.h>
#include <ab_private/attch_ed.h>
#include "dtbuilder.h"
#include "abobjP.h"
#include "brwsP.h"


/*
 * Bitmaps
 */
#include "bitmaps/align_left.xbm"
#include "bitmaps/align_vcenter.xbm"
#include "bitmaps/align_right.xbm"
#include "bitmaps/align_labels.xbm"
#include "bitmaps/align_top.xbm"
#include "bitmaps/align_hcenter.xbm" 
#include "bitmaps/align_bottom.xbm" 
#include "bitmaps/align_grid.xbm" 
#include "bitmaps/distribute_horizontal.xbm" 
#include "bitmaps/distribute_vertical.xbm" 
#include "bitmaps/distribute_hcenter.xbm" 
#include "bitmaps/distribute_vcenter.xbm"

static const int	EDIT_MENU_CACHE_BLOCK = 50;

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/

static int	add_win_menu_cache_entry(
		    ABObj	winobj,
		    Widget	menu
		);
static int	delete_win_menu_cache_entry(
		    ABObj	winobj
		);
static int	add_browser_menu_cache_entry(
		    Widget	brws_widget,
		    Widget	menu
		);
static int	delete_browser_menu_cache_entry(
		    Widget	brws_widget
		);


static Widget	get_edit_menu_from_cache(
		    EDIT_MENU_TYPE mtype,
		    Widget	origin
		);
static int	set_menu_state(
		    Widget	menu,
		    unsigned int flags
		);
static void	set_item_flags(
		    ABSelectedRec	*target,
		    unsigned int	*ret_item_flag
		);

/*
 * Callbacks
 */

static void	win_cache_menu_destroyCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );
static void	browser_cache_menu_destroyCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );
static void	rev_propCB(
		    Widget	widget,
		    XtPointer	client_data,
		    XtPointer   call_data
		);

static void	fix_propCB( 
                    Widget      widget, 
                    XtPointer   client_data, 
                    XtPointer   call_data 
                ); 

static void	alignCB( 
                    Widget      widget, 
                    XtPointer   client_data, 
                    XtPointer   call_data 
                ); 

static void     undoCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );

static void     cutCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );

static void     copyCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );

static void     pasteCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );

static void     deleteCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );

static void     browseCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );

static void	groupCB( 
                    Widget      widget, 
                    XtPointer   client_data, 
                    XtPointer   call_data 
                ); 

static void	ungroupCB( 
                    Widget      widget, 
                    XtPointer   client_data, 
                    XtPointer   call_data 
                ); 

static void	attachCB( 
                    Widget      widget, 
                    XtPointer   client_data, 
                    XtPointer   call_data 
                ); 

static void	next_layerCB( 
                    Widget      widget, 
                    XtPointer   client_data, 
                    XtPointer   call_data 
                ); 

static void     make_paneCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );

static void     unmake_paneCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );

static void     expandCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );

static void     expandAllCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );

static void     collapseCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );

static void     tearOffCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );

/*************************************************************************
**		    		    		    		        **
**       Private Data		    		    		        **
**	 IMPORTANT - the strings in the arrays are initialized in	**
**	 abobj_strings_init().						**
**		    		    		    		        **
**************************************************************************/
MenuItem	prop_menu[] =
{
	{ NULL, NULL/*"Revolving..."*/, AB_LABEL_STRING, &xmPushButtonWidgetClass, rev_propCB, 
	 NULL, (XtPointer)ABMenuProps, NULL, 0, TRUE },
	{ NULL, NULL/*"Fixed..."*/, AB_LABEL_STRING, &xmPushButtonWidgetClass, fix_propCB, 
	 NULL, (XtPointer)ABMenuProps, NULL, 0, TRUE },
	NULL
};

MenuItem	align_menu[] =
{
	{ NULL, "bitmaps/align_left.xbm",  AB_LABEL_GLYPH, 
	  &xmPushButtonWidgetClass, alignCB, (XtPointer) LEFT_EDGES, 
	  (XtPointer)ABMenuAlign, NULL, 0, TRUE, align_left_width,
	  align_left_height, align_left_bits },
	{ NULL, "bitmaps/align_vcenter.xbm", AB_LABEL_GLYPH, 
	  &xmPushButtonWidgetClass, alignCB, (XtPointer) VCENTERS, 
	  (XtPointer)ABMenuAlign, NULL, 0, TRUE, align_vcenter_width,
	  align_vcenter_height, align_vcenter_bits },
	{ NULL, "bitmaps/align_right.xbm", AB_LABEL_GLYPH, 
	  &xmPushButtonWidgetClass, alignCB, (XtPointer) RIGHT_EDGES, 
	  (XtPointer)ABMenuAlign, NULL, 0, TRUE, align_right_width,
	  align_right_height, align_right_bits },
	{ NULL, "bitmaps/align_labels.xbm", AB_LABEL_GLYPH, 
	  &xmPushButtonWidgetClass, alignCB, (XtPointer) LABELS, 
	  (XtPointer)ABMenuAlign, NULL, 0, TRUE, align_labels_width,
	  align_labels_height, align_labels_bits },
	{ NULL, "bitmaps/align_top.xbm", AB_LABEL_GLYPH, 
	  &xmPushButtonWidgetClass, alignCB, (XtPointer) TOP_EDGES, 
	  (XtPointer)ABMenuAlign, NULL, 0, TRUE, align_top_width,
	  align_top_height, align_top_bits },
	{ NULL, "bitmaps/align_hcenter.xbm", AB_LABEL_GLYPH, 
	  &xmPushButtonWidgetClass, alignCB, (XtPointer) HCENTERS, 
	  (XtPointer)ABMenuAlign, NULL, 0, TRUE, align_hcenter_width,
	  align_hcenter_height, align_hcenter_bits },
	{ NULL, "bitmaps/align_bottom.xbm", AB_LABEL_GLYPH, 
	  &xmPushButtonWidgetClass, alignCB, (XtPointer) BOTTOM_EDGES, 
	  (XtPointer)ABMenuAlign, NULL, 0, TRUE, align_bottom_width,
	  align_bottom_height, align_bottom_bits },
	{ NULL, "bitmaps/align_grid.xbm", AB_LABEL_GLYPH, 
	  &xmPushButtonWidgetClass, alignCB, (XtPointer) TO_GRID, 
	  (XtPointer)ABMenuAlign, NULL, 0, TRUE, align_grid_width,
	  align_grid_height, align_grid_bits },
	NULL
};

MenuItem	distribute_menu[] =
{
	{ NULL, "bitmaps/distribute_horizontal.xbm",  AB_LABEL_GLYPH, 
	  &xmPushButtonWidgetClass, alignCB, (XtPointer) HSPACING, 
	  (XtPointer)ABMenuDistribute, NULL, 0, TRUE, 
	  distribute_horizontal_width, distribute_horizontal_height,
	  distribute_horizontal_bits },
	{ NULL, "bitmaps/distribute_vertical.xbm", AB_LABEL_GLYPH, 
	  &xmPushButtonWidgetClass, alignCB, (XtPointer) VSPACING, 
	  (XtPointer)ABMenuDistribute, NULL, 0, TRUE,
	  distribute_vertical_width, distribute_vertical_height,
	  distribute_vertical_bits },
	{ NULL, "bitmaps/distribute_hcenter.xbm", AB_LABEL_GLYPH, 
	  &xmPushButtonWidgetClass, alignCB, (XtPointer) HCENTERED, 
	  (XtPointer)ABMenuDistribute, NULL, 0, TRUE, 
	  distribute_hcenter_width, distribute_hcenter_height,
	  distribute_hcenter_bits },
	{ NULL, "bitmaps/distribute_vcenter.xbm", AB_LABEL_GLYPH, 
	  &xmPushButtonWidgetClass, alignCB, (XtPointer) VCENTERED, 
	  (XtPointer)ABMenuDistribute, NULL, 0, TRUE,
	  distribute_vcenter_width, distribute_vcenter_height,
	  distribute_vcenter_bits },
	NULL
};

MenuItem	obj_menu_items[] = 
{
	{ "props_menu",  NULL/*"Props"*/,	AB_LABEL_STRING, 
	  &xmCascadeButtonWidgetClass, NULL, NULL, (XtPointer)(XtPointer)ABMenuProps, prop_menu, 1, TRUE, 0, 0, NULL, },
	{ NULL, NULL/*"Browse..."*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, browseCB, NULL, (XtPointer)ABMenuBrowse, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, "", AB_LABEL_SEPARATOR,
	  &xmSeparatorWidgetClass, NULL, NULL, NULL, NULL, 0, TRUE, 0, 0, NULL },
        { "undo_menu", NULL/*"Undo"*/, AB_LABEL_STRING,
          &xmPushButtonWidgetClass, undoCB, NULL, (XtPointer)ABMenuUndo, NULL, 0, TRUE, 0,
0, NULL },
	{ "cut_menu", NULL/*"Cut"*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, cutCB, NULL, (XtPointer)ABMenuCut, NULL, 0, TRUE, 0, 0, NULL },
	{ "copy_menu", NULL/*"Copy"*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, copyCB, NULL, (XtPointer)ABMenuCopy, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, NULL/*"Paste"*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, pasteCB, (XtPointer)AB_PASTE_INITIATOR_OBJ_MENU, (XtPointer)ABMenuPaste, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, NULL/*"Delete"*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, deleteCB, NULL, (XtPointer)ABMenuDelete, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, "", AB_LABEL_SEPARATOR,
	  &xmSeparatorWidgetClass, NULL, NULL, NULL, NULL, 0, TRUE, 0, 0, NULL },
	{ "align_menu", NULL/*"Align"*/, AB_LABEL_STRING, 
	  &xmCascadeButtonWidgetClass, NULL, NULL, (XtPointer)ABMenuAlign, align_menu, 2, TRUE, 0, 0, NULL },
	{ "distribute_menu", NULL/*"Distribute"*/, AB_LABEL_STRING, 
	  &xmCascadeButtonWidgetClass, NULL, NULL, (XtPointer)ABMenuDistribute, distribute_menu, 2, TRUE, 0, 0, NULL },
	{ NULL, NULL/*"Group"*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, groupCB, NULL, (XtPointer)ABMenuGroup, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, NULL/*"Ungroup"*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, ungroupCB, NULL, (XtPointer)ABMenuUngroup, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, NULL/*"Make Paned Window"*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, make_paneCB, NULL, (XtPointer)ABMenuPane, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, NULL/*"Unmake Paned Window"*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, unmake_paneCB, NULL, (XtPointer)ABMenuUnpane, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, NULL/*"Attachments..."*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, attachCB, NULL, (XtPointer)ABMenuAttach, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, "", AB_LABEL_SEPARATOR,
	  &xmSeparatorWidgetClass, NULL, NULL, NULL, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, NULL/*"NextLayer"*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, next_layerCB, NULL, (XtPointer)ABMenuNextLayer, NULL, 0, TRUE, 0, 0, NULL },
	NULL
};

MenuItem	browser_menu_items[] = 
{
	{ "props_menu",  NULL/*"Props"*/,	AB_LABEL_STRING, 
	  &xmCascadeButtonWidgetClass, NULL, NULL, (XtPointer)(XtPointer)ABMenuProps, prop_menu, 1, TRUE, 0, 0, NULL, },
	{ NULL, NULL/*"Tearoff Browser..."*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, tearOffCB, NULL, (XtPointer)ABMenuTearOff, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, "", AB_LABEL_SEPARATOR,
	  &xmSeparatorWidgetClass, NULL, NULL, NULL, NULL, 0, TRUE, 0, 0, NULL },
        { "undo_menu", NULL/*"Undo"*/, AB_LABEL_STRING,
          &xmPushButtonWidgetClass, undoCB, NULL, (XtPointer)ABMenuUndo, NULL, 0, TRUE, 0,
0, NULL },
	{ "cut_menu", NULL/*"Cut"*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, cutCB, NULL, (XtPointer)ABMenuCut, NULL, 0, TRUE, 0, 0, NULL },
	{ "copy_menu", NULL/*"Copy"*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, copyCB, NULL, (XtPointer)ABMenuCopy, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, NULL/*"Paste"*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, pasteCB, NULL, (XtPointer)ABMenuPaste, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, NULL/*"Delete"*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, deleteCB, NULL, (XtPointer)ABMenuDelete, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, "", AB_LABEL_SEPARATOR,
	  &xmSeparatorWidgetClass, NULL, NULL, NULL, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, NULL/*"Group"*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, groupCB, NULL, (XtPointer)ABMenuGroup, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, NULL/*"Ungroup"*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, ungroupCB, NULL, (XtPointer)ABMenuUngroup, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, NULL/*"Make Paned Window"*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, make_paneCB, NULL, (XtPointer)ABMenuPane, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, NULL/*"Unmake Paned Window"*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, unmake_paneCB, NULL, (XtPointer)ABMenuUnpane, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, NULL/*"Attachments..."*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, attachCB, NULL, (XtPointer)ABMenuAttach, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, "", AB_LABEL_SEPARATOR,
	  &xmSeparatorWidgetClass, NULL, NULL, NULL, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, NULL/*"Expand"*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, expandCB, NULL, (XtPointer)ABMenuExpand, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, NULL/*"Expand All"*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, expandAllCB, NULL, (XtPointer)ABMenuExpandAll, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, NULL/*"Collapse"*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, collapseCB, NULL, (XtPointer)ABMenuCollapse, NULL, 0, TRUE, 0, 0, NULL },
	NULL
};


MenuItem	layout_menu_items[] = 
{
	{ "align_menu", NULL/*"Align"*/, AB_LABEL_STRING, 
	  &xmCascadeButtonWidgetClass, NULL, NULL, (XtPointer)ABMenuAlign, align_menu, 2, TRUE, 0, 0, NULL },
	{ "distribute_menu", NULL/*"Distribute"*/, AB_LABEL_STRING, 
	  &xmCascadeButtonWidgetClass, NULL, NULL, (XtPointer)ABMenuDistribute, distribute_menu, 2, TRUE, 0, 0, NULL },
	{ NULL, NULL/*"Group"*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, groupCB, NULL, (XtPointer)ABMenuGroup, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, NULL/*"Ungroup"*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, ungroupCB, NULL, (XtPointer)ABMenuUngroup, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, NULL/*"Make Paned Window"*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, make_paneCB, NULL, (XtPointer)ABMenuPane, NULL, 0, TRUE, 0, 0, NULL },
	{ NULL, NULL/*"Unmake Paned Window"*/, AB_LABEL_STRING, 
	  &xmPushButtonWidgetClass, unmake_paneCB, NULL, (XtPointer)ABMenuUnpane, NULL, 0, TRUE, 0, 0, NULL },
	NULL
};

static ABSelectedRec sel;

static WinMenuCache  *win_menu_cache = NULL;
static int	     win_menu_cache_max = 0;
static int	     win_menu_cache_count = 0;
static BrowserMenuCache *browser_menu_cache = NULL;
static int	     browser_menu_cache_max = 0;
static int	     browser_menu_cache_count = 0;

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

static void
win_cache_menu_destroyCB(
    Widget	menu,
    XtPointer	client_data,
    XtPointer	call_data
)
{
    ABObj	winobj = (ABObj)client_data;

    delete_win_menu_cache_entry(winobj);

}
static void
browser_cache_menu_destroyCB(
    Widget	menu,
    XtPointer	client_data,
    XtPointer	call_data
)
{
    Widget	brws_widget = (Widget)client_data;

    delete_browser_menu_cache_entry(brws_widget);

}

static int
delete_win_menu_cache_entry(
    ABObj	winobj
)
{
    BOOL	shift = FALSE;
    int		i;

    for(i=0; i < win_menu_cache_count; i++)
    {
	if (win_menu_cache[i].winobj == winobj)
	    shift = TRUE;
	else if (shift)
	{
	    win_menu_cache[i-1].winobj = win_menu_cache[i].winobj;
	    win_menu_cache[i-1].menu   = win_menu_cache[i].menu;
	}
    }
    if (shift)
	win_menu_cache_count--;

    return 0;
}

static int
delete_browser_menu_cache_entry(
    Widget	brws_widget
)
{
    BOOL        shift = FALSE;
    int         i;

    for(i=0; i < browser_menu_cache_count; i++)
    {
        if (browser_menu_cache[i].brws_widget == brws_widget)
            shift = TRUE;
        else if (shift)
        {
            browser_menu_cache[i-1].brws_widget = browser_menu_cache[i].brws_widget;
            browser_menu_cache[i-1].menu        = browser_menu_cache[i].menu;
        }
    }    
    if (shift)
        browser_menu_cache_count--;

    return 0;
}
static int
add_win_menu_cache_entry(
    ABObj	winobj,
    Widget	menu
)
{
    WinMenuCache *old_cache;
    WinMenuCache *new_cache;
    int		 i;

    if (win_menu_cache_count == win_menu_cache_max) /* Need Cache Space*/
    {
	/* if win_menu_cache_count == 0, then it's our first time...*/
	old_cache = win_menu_cache;
	win_menu_cache_max+=EDIT_MENU_CACHE_BLOCK;
	new_cache = (WinMenuCache*)util_malloc(
		    win_menu_cache_max*sizeof(WinMenuCache));

	/* Copy old cache into new cache...
	 * NOTE: this would ONLY happen if the number of Window objects
	 *       instantiated exceeds WIN_MENU_CACHE_BLOCK - which should
	 *	 rarely happen!
	 */
	for(i=0; i < win_menu_cache_count; i++)
	{
	    new_cache[i].winobj = old_cache[i].winobj;
	    new_cache[i].menu   = old_cache[i].menu;
	}
	util_free(old_cache);
	win_menu_cache = new_cache;
    }
    win_menu_cache[win_menu_cache_count].winobj = winobj;
    win_menu_cache[win_menu_cache_count].menu   = menu;
    win_menu_cache_count++;

    return 0;
}
static int
add_browser_menu_cache_entry(
    Widget	brws_widget,
    Widget      menu
)
{
    BrowserMenuCache *old_cache;
    BrowserMenuCache *new_cache;
    int          i;

    if (browser_menu_cache_count == browser_menu_cache_max) /* Need Cache Space*/
    {
        /* if browser_menu_cache_count == 0, then it's our first time...*/
        old_cache = browser_menu_cache;
        browser_menu_cache_max+=EDIT_MENU_CACHE_BLOCK;
        new_cache = (BrowserMenuCache*)util_malloc(
                    browser_menu_cache_max*sizeof(BrowserMenuCache));

        /* Copy old cache into new cache...
         * NOTE: this would ONLY happen if the number of Browser Widgets 
         *       instantiated exceeds EDIT_MENU_CACHE_BLOCK - which should
         *       rarely happen!
         */
        for(i=0; i < browser_menu_cache_count; i++)
        {
            new_cache[i].brws_widget = old_cache[i].brws_widget;
            new_cache[i].menu        = old_cache[i].menu;
        }
        util_free(old_cache);
        browser_menu_cache = new_cache;
    }   
    browser_menu_cache[browser_menu_cache_count].brws_widget = brws_widget;
    browser_menu_cache[browser_menu_cache_count].menu   = menu;
    browser_menu_cache_count++;

    return 0;
}

static Widget
get_edit_menu_from_cache(
    EDIT_MENU_TYPE mtype,
    Widget	origin
)
{
    int		i;
    Widget	menu = NULL;
    ABObj	obj;
    ABObj	winobj;
    ABObj	winchild;
    Widget	menup;

    if (mtype == WIN_EDIT_MENU)
    {
    	obj = objxm_get_obj_from_widget(origin);
    	if (obj_is_window(obj_get_root(obj)))
            winobj = obj_get_root(obj);
    	else
            winobj = obj_get_root(obj_get_window(obj));

    	/* Look for menu already stored for winobj */
    	for(i=0; i < win_menu_cache_count; i++)
	    if (win_menu_cache[i].winobj == winobj)
	    	return(win_menu_cache[i].menu);

	winchild = obj_get_child(winobj, 0);
	menup = (Widget)winchild->ui_handle;

        /* Menu not yet in Cache, must create new Menu */
        menu = ui_build_menu(menup, XmMENU_POPUP, 1, NULL, "obj_menu", obj_menu_items);
    }
    else /* BROWSER_EDIT_MENU */
    {
	ABBrowser	ab = NULL;

        /* Look for menu already stored for Browser widget */
        for(i=0; i < browser_menu_cache_count; i++)
            if (browser_menu_cache[i].brws_widget == origin)
                return(browser_menu_cache[i].menu);

	menup = origin;

        /* Menu not yet in Cache, must create new Menu */

	/*
	 * The browser routines need a handle to the relevant browser data
	 * structure (different for different browsers). A pointer to this
	 * structure is set as user data on the browser widget.
	 * Set this structure up as client data for the callbacks.
	 * (User data is used for the item flags so we can't use that here).
	 */
	XtVaGetValues(origin, XmNuserData, &ab, NULL);

	if (ab)
            for (i = 0; browser_menu_items[i].label != NULL; i++)
	    {
		browser_menu_items[i].client_data = (XtPointer)ab;
	    }

	/*
	 * Build menu
	 */
        menu = ui_build_menu(menup, XmMENU_POPUP, 1, NULL, "obj_menu", browser_menu_items);

	/*
	 * Unset the client data fields
	 */
	if (ab)
            for (i = 0; browser_menu_items[i].label != NULL; i++)
	    {
		browser_menu_items[i].client_data = (XtPointer)NULL;
	    }
    }


    if (mtype == WIN_EDIT_MENU)
    {
    	add_win_menu_cache_entry(winobj, menu);
    	XtAddCallback(menu, XtNdestroyCallback, win_cache_menu_destroyCB, 
			(XtPointer)winobj);
    }
    else /* BROWSER_EDIT_MENU */
    {
	add_browser_menu_cache_entry(origin, menu);
	XtAddCallback(menu, XtNdestroyCallback, browser_cache_menu_destroyCB,
			(XtPointer)origin);
    }
    
    return menu;

}

/*
 * Set state of ABObj Edit Menu
 */
static int 
set_menu_state(
    Widget	menu,
    unsigned int flags
)
{
    WidgetList	items;
    int		num_items;
    unsigned long item_flag;
    int		i;

    XtVaGetValues(menu,
		XmNnumChildren,	&num_items,
		XmNchildren,	&items,
		NULL);

    for (i=0; i < num_items; i++)
    {
	XtVaGetValues(items[i], XmNuserData, &item_flag, NULL);
	if (item_flag != ABMenuNone)
	{
	    if ((flags & (unsigned int)item_flag) != 0)
	    	ui_set_active(items[i], TRUE);
	    else
	        ui_set_active(items[i], FALSE);
	}
    }
    return 0;
}


static void
set_item_flags(
    ABSelectedRec	*target,
    unsigned int	*ret_item_flags
)
{
    ABObj	selobj;
    ABObj	selparent;
    ABObj	pane_parent;
    Widget	popup_menu;
    unsigned int item_flags = ABMenuNone;
    int		i, j;
    BOOL	all_groups = TRUE;
    BOOL	all_panes = TRUE;
    BOOL	all_panedwins = TRUE;
    BOOL	same_parent = TRUE;
    BOOL	already_in_panedwin = FALSE;

    /* Store Select info */
    sel.count = target->count;
    util_free(sel.list);
    sel.list  = (ABObj*)util_malloc(sel.count*sizeof(ABObj));
    for(i = 0; i < sel.count; i++)
        sel.list[i] = target->list[i];

    item_flags |= ABMenuTearOff;
    item_flags |= ABMenuExpandAll;

    /* 
     * Configure the Item's state (active/inactive) depending
     * on the current objects selected
     */
    if (target->count == 1)
    {
	item_flags |= ABMenuProps;

	if (obj_is_layers(obj_get_parent(target->list[0])))
	    item_flags |= ABMenuNextLayer;
    }

    if (target->count >= 1)
    {
	item_flags |= ABMenuCut;
	item_flags |= ABMenuCopy;
	item_flags |= ABMenuDelete;
	item_flags |= ABMenuBrowse;
	item_flags |= ABMenuAlign;
	item_flags |= ABMenuDistribute;
	item_flags |= ABMenuAttach;

	item_flags |= ABMenuExpand;
	item_flags |= ABMenuCollapse;

	/* Loop1: look at selected objects to TURN-ON menu items */
	for (j = 0; j < target->count; j++)
	{
	    selobj = target->list[j];
	    selparent = obj_get_root(obj_get_parent(obj_get_root(selobj)));

            if ((obj_is_pane(selobj) && obj_is_control_panel(selparent) &&
			!obj_is_group_member(selobj)) ||
		(obj_is_control(selobj) && !obj_is_group_member(selobj)) ||
                (obj_is_group(selobj) && !obj_is_group_member(selobj)))
                item_flags |= ABMenuGroup;

            if (!obj_is_group(selobj))
		all_groups = FALSE;

	    /* Check if obj is a textpane, termpane, draw area, or
	     * a control panel.  These are all valid children for
	     * a panedWindow, as long as they are not children of
	     * a control panel.
	     */
	    if (!obj_is_pane(selobj) ||
		obj_is_control_panel(selparent))
	    {
		all_panes = FALSE;
	    }
	    else
	    {
	   	/* Save a handle to the first pane's parent */
	 	if (j == 0)
		   pane_parent = selparent;

		/* Compare the selected obj's parent to the
		 * saved parent.  If any two parents differ,
		 * then don't allow the selected objects to
		 * become part of a panedWindow.
		 */
		if (pane_parent != selparent)
		   same_parent = FALSE;

		if (obj_is_paned_win(selparent))
		   already_in_panedwin = TRUE;
	    }

	    if (!obj_is_paned_win(selobj))
		all_panedwins = FALSE;
	}

	if (all_groups)
	    item_flags |= ABMenuUngroup;
	if (all_panes && same_parent && !already_in_panedwin)
	    item_flags |= ABMenuPane;
	if (all_panedwins)
	    item_flags |= ABMenuUnpane;

	/* Loop2: look at selected objects to TURN-OFF menu items */
        for (j = 0; j < target->count; j++)
        {
            selobj = target->list[j];
            selparent = obj_get_root(obj_get_parent(obj_get_root(selobj)));
 
            if (!obj_is_control(selobj) && !obj_is_group(selobj) &&
		!(obj_is_pane(selobj) && obj_is_control_panel(selparent)))
	    {
		item_flags &= (~ABMenuAlign);
		item_flags &= (~ABMenuDistribute);
		item_flags &= (~ABMenuGroup);
	    }
            if (obj_is_window(selobj)) 
                item_flags &= (~ABMenuAttach); 

	    if (obj_is_control(selobj) || obj_is_window(selobj))
	    {
		item_flags &= (~ABMenuPane);
		item_flags &= (~ABMenuUnpane);
	    }
	}
    	if (!abobj_clipboard_is_empty())
            item_flags |= ABMenuPaste;
    }

    if (abobj_undo_active())
	item_flags |= ABMenuUndo;
    else
	item_flags &= (~ABMenuUndo);

    *ret_item_flags = item_flags;
}


/*
 * Popup Menu for Object
 */
void
abobj_popup_menu(
    EDIT_MENU_TYPE mtype,
    Widget	origin,
    ABSelectedRec *target,
    XButtonEvent  *event
)
{
    ABObj	selobj;
    ABObj	selparent;
    ABObj	pane_parent;
    Widget	popup_menu;
    unsigned int item_flags = ABMenuNone;
    int		i, j;
    BOOL	all_groups = TRUE;
    BOOL	all_panes = TRUE;
    BOOL	all_panedwins = TRUE;
    BOOL	same_parent = TRUE;
    BOOL	already_in_panedwin = FALSE;

    popup_menu = get_edit_menu_from_cache(mtype, origin);

    set_item_flags(target, &item_flags);

    set_menu_state(popup_menu, item_flags);

    XmMenuPosition(popup_menu, event);
    XtManageChild(popup_menu);

}

    
/*******************************************************************
 * Menu Callbacks
 *******************************************************************/

static void
rev_propCB(
    Widget      menu,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    ABObj	obj = sel.list[0];

    if (obj != NULL)
    	prop_load_obj(obj, AB_PROP_REVOLVING);
}

static void
fix_propCB(
    Widget      menu,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    ABObj       obj = sel.list[0];
 
    if (obj != NULL)
    {
        prop_load_obj(obj, AB_PROP_FIXED);
    }
}

static void
alignCB(
    Widget      menu,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    abobj_align((unsigned long) client_data);
}

static void
undoCB(
    Widget      menu,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    (void)abobj_undo();
}

static void
cutCB(
    Widget      menu,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    (void)abobj_cut();
}

static void
copyCB(
    Widget      menu,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    (void)abobj_copy();
}

static void
pasteCB(
    Widget      menu,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    AB_PASTE_INITIATOR_TYPE initiator = (AB_PASTE_INITIATOR_TYPE)client_data;
    
    /*
    ** REMIND bfreeman (03/03/95): Hack Alert!
    **
    ** The browser popup menu, browser_menu_items, uses client_data to store
    ** a handle to the relevant browser data structure. So, client_data can
    ** not be used to store the paste initiator.
    **
    ** But, *only* the obj popup and browser popup menus need to pass the
    ** paste initiator. If the initiator does not match
    ** AB_PASTE_INITIATOR_OBJ_MENU, force the initiator to
    ** AB_PASTE_INITIATOR_BRWS_MENU.
    **
    ** Of course, this will break if paste is added to another menu :-(
    ** Try to safeguard by only setting the initiator if it is unknown.
    */
    switch(initiator)
    {
    case AB_PASTE_INITIATOR_OBJ_MENU:
	break;
    case AB_PASTE_INITIATOR_BRWS_MENU:
	break;
    case AB_PASTE_INITIATOR_BRWS_EDIT_MENU:
	break;
    case AB_PASTE_INITIATOR_PAL_EDIT_MENU:
	break;
    default:
	initiator = AB_PASTE_INITIATOR_BRWS_MENU;
	break;
    }
    (void)abobj_paste(initiator);
}

static void
deleteCB(
    Widget      menu,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    abobj_delete();
}

static void
browseCB(
    Widget      menu,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    /*
     * This code assumes that the objects in sel.list
     * are all in one window. See object_popup_menu()
     * in abobj_events.c. It gets the selected objects
     * in *one* particular window.
     *
     * Because of this, it is unnecessary to traverse
     * sel.list to search for and map different browsers
     * since all the objects on sel.list will be on the same
     * module/window/browser.
     */
    if (sel.count > 0)
    {
        ABBrowser	cur_b;

	/*
	 * Find/create browser for first selected object on
	 * sel.list
	 */
	cur_b = brws_get_browser_for_obj(sel.list[0]);

	/*
	 * Center the browser's detailed view on the selected object
	 */
	brws_center_on_obj(cur_b, sel.list[0]);

	/*
	 * Map browser
	 */
	brws_popup(cur_b);
    }
}

static void
groupCB(
    Widget      menu,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    abobj_group_selected_objects();
}
static void

ungroupCB(
    Widget      menu,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    abobj_ungroup_selected_objects();
}

static void
next_layerCB(
    Widget	widget,
    XtPointer	client_data,
    XtPointer	call_data
)
{
    ABObj	obj = sel.list[0];

    abobj_layer_show_next(obj_get_parent(obj));
}

static void
attachCB(
    Widget	widget,
    XtPointer	client_data,
    XtPointer	call_data
)
{
    ABObj	obj = sel.list[0];

    attch_ed_show_dialog(obj);
}

static void
make_paneCB(
    Widget      widget, 
    XtPointer   client_data, 
    XtPointer   call_data 
) 
{
    abobj_make_panedwin();
}

static void
unmake_paneCB(
    Widget      widget, 
    XtPointer   client_data, 
    XtPointer   call_data 
) 
{
    abobj_unmake_panedwin();
}

static void
expandCB(
    Widget      menu,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    ABBrowser	ab = (ABBrowser)client_data;

    brwsP_expand_selected(ab);
}

static void
expandAllCB(
    Widget      menu,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    ABBrowser	ab = (ABBrowser)client_data;

    brwsP_expand_collapsed(ab);
}

static void
collapseCB(
    Widget      menu,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    ABBrowser	ab = (ABBrowser)client_data;

    brwsP_collapse_selected(ab);
}

static void
tearOffCB(
    Widget      menu,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    ABBrowser	ab = (ABBrowser)client_data;

    brwsP_tear_off_selected(ab);
}

void
abobj_build_layout_menu(
    Widget	menu
)
{
    if (menu)
        ui_populate_pulldown_menu(menu, 1, layout_menu_items);
}

void
abobj_set_menu_item_state(
    Widget	menu
)
{
    ABSelectedRec	sel;
    unsigned int	item_flags;

    if (!menu)
	return;

    abobj_get_selected(proj_get_project(), TRUE, FALSE, &sel);

    set_item_flags(&sel, &item_flags);

    set_menu_state(menu, item_flags);

    if (sel.list)
        XtFree((char *)sel.list);
}

void
abobj_strings_init(void)
{
    char 	*props_str;
    char 	*undo_str;
    char 	*cut_str;
    char 	*copy_str;
    char 	*paste_str;
    char 	*delete_str;
    char 	*align_str;
    char 	*distr_str;
    char 	*group_str;
    char 	*ungroup_str;
    char 	*mk_paned_win_str;
    char 	*unmk_paned_win_str;
    char 	*attach_str;

    props_str = 
	XtNewString(catgets(Dtb_project_catd, 100, 137, "Props"));
    undo_str = 
	XtNewString(catgets(Dtb_project_catd, 100, 138, "Undo"));
    cut_str = 
	XtNewString(catgets(Dtb_project_catd, 100, 139, "Cut"));
    copy_str = 
	XtNewString(catgets(Dtb_project_catd, 100, 140, "Copy"));
    paste_str = 
    	XtNewString(catgets(Dtb_project_catd, 100, 141, "Paste"));
    delete_str = 
	XtNewString(catgets(Dtb_project_catd, 100, 142, "Delete"));
    align_str = 
	XtNewString(catgets(Dtb_project_catd, 100, 143, "Align"));
    distr_str = 
	XtNewString(catgets(Dtb_project_catd, 100, 144, "Distribute"));
    group_str = 
	XtNewString(catgets(Dtb_project_catd, 100, 145, "Group"));
    ungroup_str = 
	XtNewString(catgets(Dtb_project_catd, 100, 146, "Ungroup"));
    mk_paned_win_str = 
	XtNewString(catgets(Dtb_project_catd, 100, 147, "Make Paned Window"));
    unmk_paned_win_str = 
	XtNewString(catgets(Dtb_project_catd, 100, 148, "Unmake Paned Window"));
    attach_str = 
	XtNewString(catgets(Dtb_project_catd, 100, 149, "Attachments..."));

    prop_menu[0].label = 
	XtNewString(catgets(Dtb_project_catd, 100, 150, "Revolving..."));
    prop_menu[1].label = 
	XtNewString(catgets(Dtb_project_catd, 100, 151, "Fixed..."));

    obj_menu_items[0].label = props_str;
    obj_menu_items[1].label = 
	XtNewString(catgets(Dtb_project_catd, 100, 152, "Browse..."));
    obj_menu_items[3].label = undo_str;
    obj_menu_items[4].label = cut_str;
    obj_menu_items[5].label = copy_str;
    obj_menu_items[6].label = paste_str;
    obj_menu_items[7].label = delete_str;
    obj_menu_items[9].label = align_str;
    obj_menu_items[10].label = distr_str;
    obj_menu_items[11].label = group_str;
    obj_menu_items[12].label = ungroup_str;
    obj_menu_items[13].label = mk_paned_win_str;
    obj_menu_items[14].label = unmk_paned_win_str;
    obj_menu_items[15].label = attach_str;
    obj_menu_items[17].label = 
	XtNewString(catgets(Dtb_project_catd, 100, 153, "NextLayer"));

    browser_menu_items[0].label = props_str;
    browser_menu_items[1].label = 
	XtNewString(catgets(Dtb_project_catd, 100, 154, "Tearoff Browser..."));
    browser_menu_items[3].label = undo_str;
    browser_menu_items[4].label = cut_str;
    browser_menu_items[5].label = copy_str;
    browser_menu_items[6].label = paste_str;
    browser_menu_items[7].label = delete_str;
    browser_menu_items[9].label = group_str;
    browser_menu_items[10].label = ungroup_str;
    browser_menu_items[11].label = mk_paned_win_str;
    browser_menu_items[12].label = unmk_paned_win_str;
    browser_menu_items[13].label = attach_str;
    browser_menu_items[15].label = 
	XtNewString(catgets(Dtb_project_catd, 100, 155, "Expand"));
    browser_menu_items[16].label = 
	XtNewString(catgets(Dtb_project_catd, 100, 156, "Expand All"));
    browser_menu_items[17].label = 
	XtNewString(catgets(Dtb_project_catd, 100, 157, "Collapse"));

    layout_menu_items[0].label = align_str;
    layout_menu_items[1].label = distr_str;
    layout_menu_items[2].label = group_str;
    layout_menu_items[3].label = ungroup_str;
    layout_menu_items[4].label = mk_paned_win_str;
    layout_menu_items[5].label = unmk_paned_win_str;
}
