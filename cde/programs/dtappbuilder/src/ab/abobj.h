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
 *      $XConsortium: abobj.h /main/3 1995/11/06 17:14:34 rswiston $
 *
 * @(#)abobj.h	1.27 15 Feb 1994      cde_app_builder/src/ab
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

#ifndef _ABOBJ_H_
#define _ABOBJ_H_
/*
 * abobj.h - AB object manipulation functions
 */
#include <X11/Intrinsic.h>
#include <ab_private/objxm.h>
#include "dtb_utils.h"


/*
 * Flags to control ABObj Edit Menu state
 */
#define ABMenuNone	0x0000
#define ABMenuProps	0x0001	
#define ABMenuUndo	0x0002
#define ABMenuCut	0x0004
#define ABMenuCopy	0x0008
#define ABMenuPaste	0x0010
#define ABMenuDelete	0x0020
#define ABMenuAlign	0x0040
#define ABMenuDistribute 0x0080
#define ABMenuGroup	0x0100
#define ABMenuUngroup	0x0200
#define ABMenuNextLayer 0x0400
#define ABMenuBrowse	0x0800
#define ABMenuAttach	0x1000
#define ABMenuPane	0x2000
#define ABMenuUnpane	0x4000
#define ABMenuExpand	0x8000
#define ABMenuExpandAll	0x10000
#define ABMenuCollapse	0x20000
#define ABMenuTearOff	0x40000

/*
 * Two Types of Object Edit Menus: Browser & WindowObj
 */
typedef enum
{
 	WIN_EDIT_MENU,
 	BROWSER_EDIT_MENU,
        EDIT_MENU_TYPE_NUM_VALUES /* number of valid values - MUST BE LAST */
        /* ANSI: no comma after last enum item! */
} EDIT_MENU_TYPE;

/*
 * Selected Object storage
 */
typedef struct _AB_SELECTED_REC
{
    ABObj       *list;
    int         count;
} ABSelectedRec;



/*
 * EVENT-HANDLING for AB objects
 */

extern void    	abobj_register_build_actions(
            	    XtAppContext app
        	);

extern int    	abobj_tree_set_build_actions(
                    ABObj   	root
        	);

/*
 * Instantiate/Show/Hide/Propogate an AB object hierarchy 
 */
extern int	abobj_instantiate_tree(
		    ABObj	root,
		    BOOL	manage_last
		);

extern int	abobj_show_tree(
		    ABObj	root,
		    BOOL	manage_last
		);

extern int	abobj_hide_tree(
		    ABObj	root
		);

/*
 * SELECT AB object functions
 */
extern void     abobj_select(
                    ABObj obj
                );

extern void     abobj_deselect(
                    ABObj obj
                );

extern void     abobj_deselect_all(
                    ABObj root
                );

extern int	abobj_get_selected(  
	       /* NOTE: caller must free the memory for sel->list! */
                    ABObj root,
		    BOOL  include_root,
                    BOOL  include_items,
		    ABSelectedRec *sel
                );

extern void	abobj_sort_sel_list(
		    ABObj *sel_list, 
		    int sel_count, 
		    int sort);

extern void	abobj_group_selected_objects(
		);

extern void	abobj_ungroup_selected_objects(
		);

extern void	abobj_layout_group(
		    ABObj	obj,
		    BOOL	init
		);

extern void	abobj_register_group_expose_handler(
		    ABObj	obj
		);

extern void	abobj_make_panedwin(
		);

extern void 	abobj_unmake_panedwin(
		);


/*
 * MOVE AB object functions
 */
extern int      abobj_move(
                    ABObj       obj,
                    XEvent      *event
                );

extern void     abobj_move_selected(
                    ABObj      *sel_list,
                    int         sel_count,
                    XRectangle *start,
                    XRectangle *stop
                );

extern void	abobj_nudge_selected(
    		    ABObj       *sel_list,
    		    int         sel_count,
    		    short         x_delta,
    		    short         y_delta,
    		    BOOL        reselect
		);
extern BOOL	abobj_is_movable(
		    ABObj	obj
		);

/*
 * ALIGN AB object functions
 */

extern void	abobj_align(
		    unsigned long align_mask
		);

/*
 * RESIZE AB object functions
 */
  
extern int      abobj_resize(
                     ABObj    obj,
                     XEvent   *event
		);

extern BOOL	abobj_is_directly_resizable(
		     ABObj	obj
		);
extern BOOL	abobj_width_resizable(
                     ABObj      obj
                );
extern BOOL     abobj_height_resizable(
                     ABObj      obj
                );

/*
 * LAYOUT AB object functions
 */
extern int	abobj_layout_changed(
		    ABObj	obj
		);

extern int	abobj_calculate_new_layout(
    		    ABObj       obj,
    		    int         new_x,
    		    int         new_y,
    		    Dimension   new_width,
    		    Dimension   new_height
		);

extern void	abobj_sort_children(
		    ABObj obj, 
		    int sort);

extern void	abobj_clear_layout(
		    ABObj obj, 
		    BOOL  clear_children,
		    BOOL  init_attachments
		);

extern ABObj	abobj_handle_layered_pane(
		    ABObj	newobj,
		    ABObj	pane
		);

extern int	abobj_init_pane_position(
		    ABObj	pane
		);

extern int	abobj_layer_show_next(
		    ABObj	layer
		);

extern void	abobj_layer_set_size(
		    ABObj	layer,
		    int		new_width,
		    int		new_height
		);

extern int	abobj_layer_manage_visible(
		    ABObj	layer
		);

/*
 * Functions to duplicate an ABObj tree for the clipboard
 */
extern ABObj	abobj_dup(
		    ABObj obj
		);

extern ABObj	abobj_dup_tree(
		    ABObj obj
		);

/*
 * POPUP-MENU AB object functions
 */
		/* Handles Caching Menus */
extern void	abobj_popup_menu(
		    EDIT_MENU_TYPE mtype,
		    Widget        origin,
                    ABSelectedRec *target,
                    XButtonEvent  *event
                );
		/* Creates/Destroys Menu each time */
extern void	abobj_menu(
    		    Widget        origin,
		    ABSelectedRec *target,
    		    XButtonEvent  *event
		);

/*
 * Miscellaneous AB object utilities
 */
extern int	abobj_comp_rename(
		    ABObj	obj,
		    STRING	oldname,
		    STRING	newname
		);

extern int      abobj_get_value_x(
                    ABObj   obj
                );

extern int      abobj_get_actual_width(
                    ABObj    obj
                );

extern int      abobj_get_actual_height(
                    ABObj    obj
                );

extern int	abobj_get_comp_width(
		    ABObj	obj
		);

extern int	abobj_get_comp_height(
		    ABObj	obj
		);

extern int      abobj_get_x(
                    ABObj       obj
                );
 
extern int      abobj_get_y(
                    ABObj       obj
                );

extern int      abobj_get_actual_x(
                    ABObj       obj
                );
 
extern int      abobj_get_actual_y(
                    ABObj       obj
                );

extern int	abobj_get_comp_x(
		    ABObj	obj
		);

extern int      abobj_get_comp_y( 
                    ABObj       obj 
                ); 

extern int      abobj_get_label_width(
                    ABObj    obj
                );

extern void     abobj_get_rect_for_objects(
                    ABObj      *list,
		    int	        count,
		    XRectangle *j_rect
                );

extern void     abobj_get_greatest_size(
                    ABObj      *list,
		    int	        count,
		    int	       *width,
		    int	       *height,
		    ABObj      *tallest_obj,
		    ABObj      *widest_obj
                );
extern BOOL	abobj_has_attached_label(
		    ABObj		obj
		);

		/* Given an obj, return a module-format name:
		 * "modulename :: objname"
		 * NOTE: it is up to caller to free memory allocated
		 * by this function
		 */
extern STRING	abobj_get_moduled_name(
		    ABObj	obj
		);

		/* Give a module-name & an obj-name, return in format:
		 * "modulename :: objname"
		 * NOTE: the caller must free the memory allocated
		 * by this function
		 */
extern STRING	abobj_alloc_moduled_name(
		    STRING	mod_name,
		    STRING	obj_name
		);

		/* Given a module-format name "modulename :: objname",
		 * return the correct 'module' & 'obj' ABobjs
		 */
extern int	abobj_moduled_name_extract(
    		    STRING      m_o_name,
    		    ABObj       *module,
    		    ABObj       *obj
		);

extern int	abobj_reset_colors(
		    ABObj	obj,
		    BOOL	reset_bg,
		    BOOL	reset_fg
		);

extern void     abobj_force_dang_form_resize(
                    ABObj       obj
                );

extern int 	abobj_update_module_name(
		    ABObj       modobj
		);

extern int 	abobj_update_proj_name(
		    ABObj       proj_obj
		);

extern int 	abobj_update_palette_title(
		    ABObj       proj_obj
		);

extern void 	abobj_tree_realize(
		    ABObj       ab_interface,
		    Bool        manage
		);

extern int 	abobj_tree_set_centering_handler(
		    ABObj       root
		);

extern DTB_CENTERING_TYPES 	abobj_get_centering_type(
		    ABObj       obj
		);

extern int	abobj_build_menus_array(
    		    ABObj       root,
    		    ABObj       **menus_ptr,
    		    int         *menu_count_ptr
		);

extern void	abobj_build_layout_menu(
		    Widget	menu
		);

extern void	abobj_set_menu_item_state(
		    Widget	menu
		);

extern void	abobj_strings_init(
		);

extern int 	abobj_project_name_extract(
		    STRING      app_name,
		    ABObj       *obj
		);

/*
 * Data
 */

extern int		AB_grid_size;
extern const int	AB_selected_rect_size;

/*************************************************************************
 **									**
 ** 		INLINE IMPLEMENTATION					**
 **									**
 *************************************************************************/

#endif /* _ABOBJ_H_ */
