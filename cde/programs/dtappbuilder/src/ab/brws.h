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
 *	$XConsortium: brws.h /main/3 1995/11/06 17:20:37 rswiston $
 *
 * @(#)brws.h	1.40 29 Mar 1995
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
 * ab_browser.h
 * Declarations for the App Builder browser
 *
 * Currently, this is a direct port of the Devguide browser. It will change
 * for the CDE APP Builder soon.
 */
#ifndef _brws_h
#define _brws_h

#include <ab/util_types.h>
#include <ab_private/obj.h>
#include <ab_private/vwr.h>
#include <X11/Intrinsic.h>


#ifndef BIT_FIELD
#define BIT_FIELD(field)        unsigned field : 1
#endif


/*
 * Indices to the node element array
 * e.g. index 2 points to the data that
 * represents the AB_OBJ type.
 *
 * This is also used to determine which bit in the 
 * 'elements_shown' bit vector corresponds to
 * the particular node element.
 * e.g. The AB_OBJ name is shown only if bit # 1
 * in the vector is set.
 */
#define BRWS_GLYPH_POS		0
#define BRWS_NAME_POS		1
#define BRWS_TYPE_POS		2
#define BRWS_WCLASS_POS		3
#define BRWS_NUM_ELM		BRWS_WCLASS_POS + 1

#define BRWS_SHOW_GLYPH		(1L << BRWS_GLYPH_POS)	/* Node elements shown, */
#define BRWS_SHOW_NAME		(1L << BRWS_NAME_POS)	/* max = sizeof(int) */
#define BRWS_SHOW_TYPE		(1L << BRWS_TYPE_POS)
#define BRWS_SHOW_WIDGET_CLASS	(1L << BRWS_WCLASS_POS)

/*
 * Flags for browser properties
 */
#define BRWS_MATRIX_MODE	0		/* Draw mode */
#define BRWS_TREE_MODE		1
#define BRWS_VERTICAL		0		/* Orientation */
#define BRWS_HORIZONTAL		1

/*
 * Masks for browser node state
 */
#define BRWS_NODE_SELECTED	(1L << 0)
#define BRWS_BNODE_PREVIEWED	(1L << 1)
#define BRWS_NODE_EXPANDED	(1L << 2)
#define BRWS_NODE_VISIBLE	(1L << 3)
#define BRWS_NODE_STATE_IS_SET(bnode, mask)	(bnode->state & mask)
#define BRWS_NODE_SET_STATE(bnode, mask)	(bnode->state |= mask)
#define BRWS_NODE_UNSET_STATE(bnode, mask)	(bnode->state &= ~mask)

/*
 * Dimensions of borders, margins, lines
 */
#define BRWS_INTER_ELM_DISTANCE	1
#define BRWS_ELM_BBOX_MARGIN	3
#define BRWS_ELM_BORDER_WIDTH	2
#define BRWS_LINE_WIDTH		0

#define BRWS_NODE_LINK_GAP	4
#define BRWS_NODE_SUBTREE_GAP	5

/* 
 * The gap between two nodes 
 */
#define	BRWS_NODE_X_GAP		10 + BRWS_NODE_LINK_GAP + BRWS_NODE_SUBTREE_GAP
#define BRWS_NODE_Y_GAP		10 + BRWS_NODE_LINK_GAP + BRWS_NODE_SUBTREE_GAP

/* 
 * The origin where the graphics starts 
 */
#define BRWS_X_ORIGIN	10
#define BRWS_Y_ORIGIN	10

typedef struct _AB_BROWSER		*ABBrowser;
typedef struct _BrowserUiObjects	*BrowserUiObj;
typedef struct _BrowserProperties	*BrowserProps;

/*
 * The browser window consists of 2 windows:
 * - the project window, where the modules for the project is displayed
 * - the module window, where the relevant modules are 'expanded'
 *   into the tree structured diagram.
 */
typedef struct _AB_BROWSER
{
    Vwr		project;
    Vwr		module;
    ABBrowser	previous;
    ABBrowser	next;
} AB_BROWSER;

/*
 * Description of the Xt specific data in the browser
 */
typedef struct _BrowserUiObjects
{
    void	*ip;
    Widget	shell;

    Widget	textf;
    Widget	find_box;
    GC		normal_gc;
    GC		select_gc;
    GC		line_gc;
    XFontStruct	*sm_font;
    XFontStruct	*bg_font;
    unsigned long    fg_color;
    unsigned long    bg_color;
}BrowserUiObjects;

/*
 * Description of browser properties
 */
typedef struct _BrowserProperties
{
    unsigned long	elements_shown;
    int			initial_state;	/* of browser nodes */
    Dimension		min_width;	/* of drawing area */
    Dimension		min_height;
    BIT_FIELD(orientation);	/* values: VERTICAL, HORIZONTAL */
    BIT_FIELD(show_mult_trees);	/* values: TRUE, FALSE */
    BIT_FIELD(active);		/* values: TRUE, FALSE */
}BrowserProperties;

/*
 * API for manipulating App Builder browser
 */
extern void		brws_register_actions(
                            XtAppContext app
                        );

extern ABBrowser	brws_create (
                        );

extern void		brws_destroy (
			    ABBrowser	b
                        );

extern void		brws_add_objects(
    			    AB_OBJ	*obj
			);

extern void		brws_add_objects_to_browser(
			    ABBrowser	ab,
    			    AB_OBJ	*obj
			);

extern void		brws_delete_objects(
    			    AB_OBJ	*obj
			);

extern int aob_populate_tree(
			    Viewer *browser, 
			    AB_OBJ *root
			);

extern BOOL		aob_is_browser_win(
			    AB_OBJ *project, 
			    Window	w
			);

extern AB_OBJ		*aob_get_object_from_xy(
			    AB_OBJ *project, 
			    Window	w,
			    int x, 
			    int y
			);

extern void		brws_select(
			    AB_OBJ *obj
			);

extern void		brws_deselect(
			    AB_OBJ *obj
			);

extern void		brws_toggle_select(
			    AB_OBJ *obj
			);

extern void		aob_redraw(
                            Viewer *b
                        );

extern ViewerNode	*aob_object_insert(
                            Viewer *browser, 
                            AB_OBJ *obj
                        );

extern void		brws_popup(
                            ABBrowser	b
                        );

extern void		brws_popdown(
                            ABBrowser	b
                        );

extern void		aob_set_mode(
                            AB_OBJ	*project
                        );

extern void		aob_free_graphics(
                            Viewer	*b
                        );

extern int		browser_get_num_siblings(
                            ViewerNode	*bnode
                        );

extern int		browser_num_elm_shown(
    		  	    Viewer	*b
			);

extern ViewerNode	*aob_find_bnode(
                            AB_OBJ	*obj,
                            Viewer	*b
                        );

extern AB_OBJ		*aob_project_from_browser(
                            Viewer	*b
                        );

extern BrowserProps 	aob_browser_properties(
                            Viewer	*b
                        );

extern void		aob_copy_props(
    			    Viewer	*from,
    			    Viewer	*to
			);

extern BrowserUiObjects *aob_ui_from_browser(
                            Viewer	*b
                        );

extern Widget		aob_ui_shell(
                            Viewer	*b
                        );

extern ViewerNodeElm *aob_bnode_elements(
                            ViewerNode	*bnode
                        );

extern void		aob_str_elm_render (
                            ViewerNode	*node,
                            ViewerNodeElm	*elm
                        );

extern unsigned long	browser_get_elm_shown(
    			    Viewer	*b
			);

extern Vwr		aob_proj_or_module(
    			    ABBrowser	b,
    			    Widget	w
			);

extern void		brws_set_module_name(
    			    Vwr		b
			);

extern void		aob_preview(
			    ABObj	obj,
		    	    Window	browser_window
			);

extern void		aob_deselect_all_nodes(
		    	    Vwr		b,
		    	    int		flag
			);

extern void		aob_deselect_all_objects(
    			    AB_OBJ	*project
			);

extern int		number_of_selected(
			    VNode	tree
			);

extern VNode		node_selected(
			    VNode	tree
			);

extern void		draw_viewer(
			    Vwr		v
			);

extern void		erase_viewer(
			    Vwr		v
			);

extern void		setup_vwr_graphics(
			    Vwr		v
			);

extern BrowserUiObj	aob_create_ui_obj();

extern void		brws_switch_module(
			    Widget	widget,
			    XtPointer	client_data,
			    XtPointer	call_data
			);

extern void		brws_update_node(
			    ABObj	obj
			);

extern ABBrowser	brws_get_browser_for_obj(
			    ABObj	obj
			);

extern Widget		brws_get_browser_shell_for_obj(
			    ABObj	obj
			);

extern void		brws_edit_cascadeCB(
			    Widget	widget,
			    XtPointer	client_data,
			    XtPointer	call_data
			);

extern void		brws_build_module_menu(
			    Widget              pulldown,
			    XtCallbackProc 	callback
			); 

extern void		brws_show_browser(
			    Widget		widget,
			    XtPointer		client_data,
			    XtPointer		call_data
			);

extern void		brws_center_on_obj(
			    ABBrowser	ab,
			    ABObj	obj
			);

extern void		brws_init(
			);
#endif /* _brws_h */
