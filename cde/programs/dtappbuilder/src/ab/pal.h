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
 *      $XConsortium: pal.h /main/3 1995/11/06 17:35:34 rswiston $
 *
 * @(#)pal.h	1.32 01 Feb 1995      cde_app_builder/src/ab
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
 * pal.h - implements the palette object mechanism
 */
#ifndef _PAL_H_
#define _PAL_H_

#include <X11/Intrinsic.h>
#include <ab_private/obj.h>
#include <ab_private/proj.h>

/*
 * AppBuilder Property sheet types: Revolving & fixed
 */
typedef enum
{
        AB_PROP_UNDEF = 0,
        AB_PROP_REVOLVING,
        AB_PROP_FIXED,
        AB_PROP_TYPE_NUM_VALUES /* number of valid values - MUST BE LAST */
        /* ANSI: no comma after last enum item! */
} AB_PROP_TYPE;

typedef struct _PAL_ITEM_INFO    	PalItemInfo;
typedef struct _PAL_EDITABLE_OBJ_INFO 	PalEditableObjInfo;

/*
 * define types for palette item "methods"
 */
typedef int    	(*PalItemInitProc) (
    ABObj
);

typedef Widget  (*PalItemPropInitProc) (
    Widget,
    AB_PROP_TYPE    
);

typedef int  (*PalItemPropClearProc) (
    AB_PROP_TYPE
);

typedef int	(*PalItemPropActiveProc) (
    AB_PROP_TYPE,
    BOOL
);

typedef int    	(*PalItemPropLoadProc) (
    ABObj,
    AB_PROP_TYPE,
    unsigned long
);

typedef int	(*PalItemPropApplyProc) (
    AB_PROP_TYPE
);

typedef BOOL	(*PalItemPropPendingFunc)(
    AB_PROP_TYPE
);

typedef BOOL    (*PalItemIsaFunc) (
    ABObj
);

typedef struct _PAL_SUBTYPE_INFO {
    int			subtype;
    String		subname;
    Pixmap		pixmap;
    unsigned int	pmwidth;
    unsigned int	pmheight;
} PalSubtypeInfo;
 
/*
 * Palette Item Info Structure - one for each type of object supported
 */
typedef struct _PAL_ITEM_INFO {

    /* Data...*/
    AB_OBJECT_TYPE  	   type;            /* AB type                */
    char    	    	   *name;           /* virtual name           */
    PalSubtypeInfo	   *subinfo;        /* specific subtype info  */
    int			   num_subinfo;     /* number of subtypes     */
    Widget          	   rev_prop_frame;  /* revolving prop sheet   */
    Widget          	   fix_prop_dialog; /* fixed prop sheet       */

    /* Methods...*/
    PalItemInitProc        initialize;      /* initializes instance     */
    PalItemIsaFunc         is_a_test;       /* "is a" test              */
    PalItemPropInitProc    prop_initialize; /* creates prop sheet       */
    PalItemPropActiveProc  prop_activate;   /* sets prop sheet active/inactive */
    PalItemPropClearProc   prop_clear;	    /* clears prop sheet	      */
    PalItemPropLoadProc    prop_load;       /* loads prop sheet         */
    PalItemPropApplyProc   prop_apply;	    /* applys properties to obj */
    PalItemPropPendingFunc prop_pending;    /* checks for pending changes */

} PAL_ITEM_INFO;

typedef struct _PAL_EDITABLE_OBJ_INFO {
    AB_OBJECT_TYPE	type;
    int			subtype;
    ISTRING		name;
    PalItemInfo		*palitem;
    PalEditableObjInfo	*next;
} PAL_EDITABLE_OBJ_INFO;

typedef BOOL    (*PalEditableObjTestFunc) (
    PalEditableObjInfo *edobj_info
);

extern PalItemInfo      *ab_button_palitem;
extern PalItemInfo      *ab_choice_palitem;
extern PalItemInfo      *ab_combo_box_palitem;
extern PalItemInfo      *ab_cpanel_palitem;
extern PalItemInfo      *ab_custdlg_palitem;
extern PalItemInfo      *ab_drawp_palitem;
extern PalItemInfo      *ab_label_palitem;
extern PalItemInfo      *ab_group_palitem;
extern PalItemInfo      *ab_list_palitem;
extern PalItemInfo      *ab_mainwin_palitem;
extern PalItemInfo      *ab_menu_palitem;
extern PalItemInfo      *ab_menubar_palitem;
extern PalItemInfo      *ab_scale_palitem;
extern PalItemInfo      *ab_separator_palitem;
extern PalItemInfo      *ab_spinbox_palitem;
extern PalItemInfo      *ab_termp_palitem;
extern PalItemInfo      *ab_textf_palitem;
extern PalItemInfo      *ab_textp_palitem;
extern PalItemInfo      *ab_fchooser_palitem;
extern PalItemInfo	*ab_panedwin_palitem;



extern void    	pal_register_item_info(
            	    Widget       pwidget,
            	    PalItemInfo  *palitem,
            	    int		 subtype,
		    String	 subname,
		    Pixmap	 pixmap
        	);

extern void	pal_enable_item_drag(
		    Widget	item_widget,
		    int		subtype
		);

extern PalItemInfo *pal_get_item_info(
            	    ABObj        obj
        	);

extern PalItemInfo* pal_get_type_item_info(
		    AB_OBJECT_TYPE	type,
		    int			subtype
		);

extern Pixmap	pal_get_item_pixmap(
		    ABObj	obj,
		    int		subtype,
		    int		*p_width,
		    int		*p_height
		);

extern STRING	pal_get_item_subname(
		    ABObj	obj,
		    int		subtype
		);

extern void     pal_register_editable_obj(
                    AB_OBJECT_TYPE      type,
                    int                 subtype,
                    STRING              name,
                    PalItemInfo         *palitem
                );

extern PalEditableObjInfo *
		pal_get_editable_obj_info(
		    ABObj		obj
		);

extern void     pal_add_editable_obj_menu_items(
                    Widget              menu_pane,
                    XtCallbackProc      item_cb,
                    PalEditableObjTestFunc test_func
                );

extern int    	pal_initialize_obj(
            	    ABObj        obj
        	);

extern int	pal_create_menu(
		    AB_PROP_TYPE  type,
		    ABObj	  module,
		    STRING	  menu_name,
		    STRING	  menu_title
		);

extern void 	pal_set_File_menu(
		    AB_CHOOSER_TYPE     chooser_type,
		    BOOL                active
		);

#endif /* _PAL_H_ */
