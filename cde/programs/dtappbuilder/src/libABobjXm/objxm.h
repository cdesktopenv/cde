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
 *      $XConsortium: objxm.h /main/3 1995/11/06 18:43:58 rswiston $
 *
 *	@(#)objxm.h	1.73 24 Apr 1995	
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
 * objxm.h - Handle Motif-specific mechanism for obj structure
 *
 */
#ifndef _OBJXM_H_
#define _OBJXM_H_

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <ab_private/obj.h>
#include <ab_private/istr.h>


/* 
 * Define Flags for Color Args 
 */
#define ObjxmNone	0x00
#define ObjxmForeground 0x01
#define ObjxmBackground	0x02

/*
 * OBJXM Configuration Types
 * The difference between the 2 types of XmConfiguring is in
 * how the Obj's Resource ArgLists are constructed:
 *
 *	For Build-mode, resource values are converted to the
 *	correct 'type' for a real "SetValues" or "Create" call.
 *	(e.g. For XmNbackground, the value is converted to a Pixel)
 *
 * 	For Codegen-mode, resource values are converted to what
 *	should be *written-out* in code.
 *	(e.g. For XmNbackground, the value is the color-name string,
 *	NOT a Pixel)
 *	
 */
typedef enum
{
        OBJXM_CONFIG_BUILD,
        OBJXM_CONFIG_CODEGEN,
        OBJXM_CONFIG_TYPE_NUM_VALUES /* number of valid values - MUST BE LAST */
        /* ANSI: no comma after last enum item! */
} OBJXM_CONFIG_TYPE;

/*
 * Type of "objects" in an XmConfigured hierarchy
 */
typedef enum
{
        /* Required by all XmConfigured objs */
        AB_CFG_UNDEF = 0,
        AB_CFG_OBJECT_OBJ,      /* Object SubObj */
        AB_CFG_LABEL_OBJ,       /* Label SubObj  */
        AB_CFG_SIZE_OBJ,        /* SubObj responsible for Sizing */
        AB_CFG_POSITION_OBJ,    /* SubObj responsible for Positioning*/
        AB_CFG_PARENT_OBJ,      /* SubObj for parenting children */
        AB_CFG_SELECT_OBJ,      /* SubObj for displaying Select feedback */

        /* Optional for XmConfigured objs */
        AB_CFG_BORDER_OBJ,      /* SubObj for displaying border */
	AB_CFG_DND_OBJ,		/* drag from/drop on this object */
	AB_CFG_HELP_OBJ,	/* where we attach help */
        AB_CFG_STATUS_OBJ,      /* SubObj for status display */
        AB_CFG_MENU_PARENT_OBJ, /* SubObj for parenting menus */
        AB_CFG_SCROLLBAR_OBJ,   /* Scrollbar SubObj */
        AB_CFG_SCROLLING_OBJ,   /* Scrolling container SubObj */
        AB_CFG_MENU_OBJ,    	/* Menu (reference) SubObj */
	AB_CFG_WINDOW_BB_OBJ,	/* Dialog internal BulletinBoard SubObj */
	AB_CFG_WINDOW_PW_OBJ,	/* Window internal PanedWindow SubObj */
	AB_CFG_WINDOW_MW_OBJ,	/* Window internal MainWindow SubObj */
        AB_CFG_OBJ_TYPE_NUM_VALUES /* number of valid values - MUST BE LAST */
} AB_CFG_OBJ_TYPE;


typedef int	ObjxmFilenameToPixmapFuncRec(
			Widget	widget,
			STRING	filename,
			Pixmap	*pixmap_out
		);
typedef ObjxmFilenameToPixmapFuncRec *ObjxmFilenameToPixmapFunc;

typedef int	ObjxmPostInstantiateFuncRec(
			ABObj	obj
		);
typedef ObjxmPostInstantiateFuncRec *ObjxmPostInstantiateFunc;

/*
 * define types for Xm-Configure object "methods"
 */
typedef int     (*XmConfigProc) (
    ABObj       obj
);

typedef int	(*XmUnconfigProc) (
    ABObj	obj
);

typedef BOOL    (*XmConfigIsaFunc) (
    ABObj       obj
);

typedef int     (*XmConfigSetArgProc) (
    ABObj	obj,
    OBJXM_CONFIG_TYPE ctype
);

typedef int	(*XmConfigPostInstProc) (
    ABObj	obj,
    OBJXM_CONFIG_TYPE ctype
);

typedef ABObj   (*XmConfigGetObjProc) (
    ABObj       obj,
    AB_CFG_OBJ_TYPE type
);

/*
 * XmConfigInfo Structure - one for each type of object supported
 */
typedef struct _XM_CONFIG_INFO_REC {
    /* Methods...*/
    XmConfigProc         xmconfig;      /* configures obj hierarchy */
    XmUnconfigProc	 xmunconfig;	/* unconfigures obj 	    */
    XmConfigGetObjProc   get_config_obj;/* returns config obj       */
    XmConfigSetArgProc   set_args;      /* sets appropriate resources */
    XmConfigPostInstProc post_instantiate; /* does any post-instantiation stuff*/
    XmConfigIsaFunc      is_a_test;     /* "is a" test              */
} XM_CONFIG_INFO_REC, XmConfigInfoRec, *XmConfigInfo;

/*
 * Xm String Cache Data structures & defines
 */
/*
 * ABXm XmCreateFunc structure
 */
typedef  Widget (*XmCreateFunc) (
    Widget   parent,
    String   name,
    ArgList  args,
    Cardinal arg_count
);

/*
 * Widget Classes
 */
extern char * const _applicationShell;
extern char * const _dtComboBox;
extern char * const _dtMenuButton;
extern char * const _dtSpinBox;
extern char * const _dtTerm;
extern char * const _group; 
extern char * const _topLevelShell;
extern char * const _xmArrowButton;
extern char * const _xmBulletinBoard;
extern char * const _xmCascadeButton;
extern char * const _xmCommand;
extern char * const _xmDialogShell;
extern char * const _xmDrawingArea;
extern char * const _xmDrawnButton;
extern char * const _xmFileSelectionBox;
extern char * const _xmForm;
extern char * const _xmFrame;
extern char * const _xmLabel;
extern char * const _xmLabelGadget;
extern char * const _xmList;
extern char * const _xmMainWindow;
extern char * const _xmManager;
extern char * const _xmMenuShell;
extern char * const _xmMessageBox;
extern char * const _xmPanedWindow;
extern char * const _xmPopupMenu;
extern char * const _xmPrimitive;
extern char * const _xmPulldownMenu;
extern char * const _xmPushButton;
extern char * const _xmRowColumn;
extern char * const _xmScale;
extern char * const _xmScrollBar;
extern char * const _xmScrolledList;
extern char * const _xmScrolledWindow;
extern char * const _xmSelectionBox;
extern char * const _xmSeparator;
extern char * const _xmSeparatorGadget;
extern char * const _xmText;
extern char * const _xmTextField;
extern char * const _xmToggleButton;
extern char * const _xmToggleButtonGadget;

/*
 * XmConfigure Routines
 */
		/* Function to initialize objxm 
 		 */
extern int      objxm_init(
		    Widget	toplevel
                );


		/*
		 * Hook to add routine to convert filenames to callbacks.
		 * NULL callback reinstates default conversion method.
		 */
extern int	objxm_set_cvt_filename_to_pixmap_callback(
		    ObjxmFilenameToPixmapFunc	filename_to_pixmap_cb
		);

		/*
		 * Hook to add routine to do additional post instantiate
		 * stuff
		 * by default, this callback is NULL, so nothing is called
		 */
extern int	objxm_set_post_instantiate_callback(
		    ObjxmPostInstantiateFunc	post_instantiate_cb
		);


		/* Function to register XmConfigInfo 
		 * for a single object type
 		 */
extern void     objxm_register_config_info(
                    XmConfigInfo cfginfo
                );

		/* Function which XmConfigures an object
		 * into it's appropriate Composite object
		 * hierarchy
 		 */
extern int      objxm_obj_configure(
                    ABObj        obj,
		    OBJXM_CONFIG_TYPE ctype,
		    BOOL	 set_args
                );
 
		/* Function which XmConfigures an entire tree
 		 */
extern int      objxm_tree_configure(
                    ABObj       root,
                    OBJXM_CONFIG_TYPE ctype 
                );

		/* Function which UnConfigures an object
		 * NOTE: does NOT destroy associated widgets!
		 */
extern int	objxm_obj_unconfigure(
		    ABObj	obj
		);

                /* Function which UnConfigures an entire tree
		 * NOTE: does NOT destroy associated widgets! 
                 */ 
extern int      objxm_tree_unconfigure( 
                    ABObj      root 
                ); 

		/* Function which returns the correct 
		 * Composite SubObj corresponding to 'type'
 		 */
extern ABObj    objxm_comp_get_subobj(
                    ABObj       obj,
                    AB_CFG_OBJ_TYPE type
                );

		/* Function which configures an object with the
		 * the standard labeled-structure (container
		 * managing Label & Main-object)
		 */
extern int	objxm_comp_config_labeled_obj(
		    ABObj	obj,
		    AB_OBJECT_TYPE obj_type,
		    STRING	obj_suffix,
		    STRING	obj_classname
		);

		/* Function which collapses a labeled object
		 * back down into a single Main-object
		 */
extern int	objxm_comp_unconfig_labeled_obj(
		    ABObj	obj
		);

                /* Function which builds a Menu-reference object
                 * if "menu_name" is non-NULL
                 */
extern ABObj    objxm_comp_config_menu_ref(
                    ABObj       obj
                );
 
                /* Function which finds & destroys a Menu-reference
                 * object if one exists
                 */
extern int      objxm_comp_unconfig_menu_ref(
                    ABObj       obj
                );

                /* Function which makes the RootObj a "Frame" and
                 * creates a new SubObj directly underneath to
                 * represent the actual Obj.
                 */
extern ABObj    objxm_comp_config_border_frame(
                    ABObj       obj
                );
 
                /* Function that collapses a Composite Obj configured
                 * with a Frame as the Root, back down to an Obj
                 * without a Frame.
                 */
extern int      objxm_comp_unconfig_border_frame(
                    ABObj       obj
		);

/*
 * Xm Instantiation/Change/Destruction routines
 */
		/* Instantiate widgets for an entire tree
		 * NOTE: for instantiating large trees at once
		 * (like loading BIL or UIL, set "manage_last" to
		 * TRUE! (if instantiating a single object or small
		 * group of objects, set it to FALSE).
		 */
extern int      objxm_tree_instantiate(
                    ABObj    root,
		    BOOL     manage_last
                );

		/* Destroy Widgets in a tree and set ui_handles=NULL
		 * NOTE: this does NOT destroy the Objs
		 */
extern int	objxm_tree_uninstantiate(
		    ABObj 	root,
		    BOOL	top_most
		);

                /* Destroy Widgets AND Objs for an entire tree
                 */
extern int      objxm_tree_destroy(
                    ABObj       root
                );

                /* Instantiate any changes made to the
                 * Composite-obj
                 * Return whether or not New widgets were
                 * created as a result of the changes
                 */
extern int      objxm_comp_instantiate_changes(
                    ABObj       obj,
                    BOOL        *new_widgets
                );
extern int	objxm_tree_instantiate_changes(
		    ABObj	obj,
		    BOOL	*new_widgets
		);

extern int	objxm_comp_set_item_selection(
		    ABObj	obj
		);

/*
 * Routines to Temporarily Instantiate/Destroy Menus
 */
extern int	objxm_instantiate_menus(
		    ABObj	root
		);

extern int	objxm_destroy_menus(
		    ABObj	root
		);

extern int	objxm_create_popup_menu_title(
		    ABObj	menu_ref,
		    OBJXM_CONFIG_TYPE	ctype,
		    STRING	title
		);

/*
 * Routines to Store default Translations & Obj pointer
 * inside widget's userData
 */
extern int	objxm_store_obj_and_actions(
		    Widget	w,
		    ABObj	obj
		);

extern int      objxm_free_obj_and_actions(
                    Widget      w
                );

extern ABObj	objxm_get_obj_from_widget(
		    Widget	w
		);

extern XtTranslations
		objxm_get_actions_from_widget(
		    Widget	w
		);

/*
 * Xm Map/Unmap routine
 */
extern int	objxm_tree_map(
		    ABObj	root,
		    BOOL	map
		);

/*
 * Object-specific methods
 */
extern ABObj	objxm_comp_mainwin_get_area(
		    ABObj		obj,
		    AB_CONTAINER_TYPE	area_type
		);

extern ABObj	objxm_comp_custdlg_get_area(
		    ABObj		obj,
                    AB_CONTAINER_TYPE   area_type
                ); 

/*
 * Xm Resource-setting routines
 */

		/* Set Xm resource lists for entire tree
		 */
extern int    	objxm_tree_set_ui_args(
            	    ABObj root, 
                    OBJXM_CONFIG_TYPE ctype,
            	    BOOL replace
        	);

		/* Remove Xm resource lists for entire tree */
extern int    	objxm_tree_remove_ui_args(
            	    ABObj root, 
                    OBJXM_CONFIG_TYPE ctype
        	);

		/* Set Xm resource list for a Composite object
		 * hierarchy
		 */
extern int    	objxm_comp_set_ui_args(
            	    ABObj    comproot,
                    OBJXM_CONFIG_TYPE ctype, 
            	    BOOL replace
        	);
		/* Set Xm resources that can only be set AFTER
		 * ALL WIDGETS have been instantiated
		 */
extern int	objxm_tree_set_post_inst_args(
		    ABObj	obj,
		    OBJXM_CONFIG_TYPE	ctype
		);
extern int	objxm_obj_set_post_inst_args(
		    ABObj	obj,
                    OBJXM_CONFIG_TYPE ctype
		);

		/* Set Xm resource list corresponding to
		 * attachment properties for a Composite object
		 * hierarchy
		 */
extern int	objxm_comp_set_attachment_args(
		    ABObj	comproot,
                    OBJXM_CONFIG_TYPE ctype
		);

		/* Set Xm resource list corresponding to
		 * attachment properties for a single object
		 */
extern int	objxm_obj_set_attachment_args(
	            ABObj       obj,
                    OBJXM_CONFIG_TYPE ctype
                );

		/* Set Xm resource list corresponding to
		 * color properties for a Composite object
		 * hierarchy
		 */
extern int      objxm_comp_set_color_args(
                    ABObj    		comproot,
		    OBJXM_CONFIG_TYPE 	ctype,
		    unsigned int	flag
                );

		/* Set Xm resources for "default button" within a
		 * dialog's button-panel
		 */
extern int	objxm_comp_set_default_button_args(
		    ABObj	comproot,
		    OBJXM_CONFIG_TYPE	ctype
		);

		/* Set the Label type/value resources
		 * for a Composite object hierarchy
		 */
extern int	objxm_comp_set_label_args(
		    ABObj	comproot,
                    OBJXM_CONFIG_TYPE ctype 
		);

		/* Set Xm resource list corresponding to
		 * label alignment properties for a 
		 * Composite object hierarchy
		 */
extern int	objxm_comp_set_lbl_align_args(
		    ABObj	comproot,
                    OBJXM_CONFIG_TYPE ctype
		);

		/* Set Xm resource list correspondin gto
		 * label positioning for a Composite object
		 * hierarchy
		 */
extern int	objxm_comp_set_lbl_pos_args(
		    ABObj	comproot,
                    OBJXM_CONFIG_TYPE ctype
		);
extern int	objxm_comp_set_icon_args(
		    ABObj	comproot,
		   OBJXM_CONFIG_TYPE ctype
                );

		/* Set Xm resource list corresponding to
		 * Border-frame style for a Composite object
		 * hierarchy
		 */
extern int	objxm_comp_set_border_args(
		    ABObj	comproot,
                    OBJXM_CONFIG_TYPE ctype
		);
extern int      objxm_comp_set_read_only_args(
                    ABObj       comproot,
                    OBJXM_CONFIG_TYPE ctype
                );
                /* Set Xm resource list corresponding to
                 * the MessageBox object's buttons.
                 */
extern int      objxm_comp_set_msgbox_button_arg(
                    ABObj               comproot,
                    OBJXM_CONFIG_TYPE   ctype,
                    unsigned char       which_btn
                );

extern int      objxm_obj_set_arrow_style_arg( 
                    ABObj       obj, 
                    OBJXM_CONFIG_TYPE ctype 
                ); 
extern int	objxm_obj_set_direction_arg(
		    ABObj	obj,
		    OBJXM_CONFIG_TYPE ctype
		);
extern int      objxm_obj_set_line_style_arg(
                    ABObj       obj,
                    OBJXM_CONFIG_TYPE ctype
                );
extern int      objxm_obj_set_orientation_arg(
                    ABObj       obj,
                    OBJXM_CONFIG_TYPE ctype
                );
extern int      objxm_obj_set_selection_arg(
                    ABObj       obj,
                    OBJXM_CONFIG_TYPE ctype
                );

extern Widget	objxm_set_widget(
		    ABObj	obj,
		    Widget	widget
		);

extern Widget	objxm_get_widget(
		    ABObj	obj
		);

extern WidgetClass
		objxm_get_class_ptr(
		    STRING	class_name
		);

/*
 * Connections stuff
 */
extern ABObj	objxm_comp_get_source_for_when(
		    ABObj		sourceObj, 
		    AB_WHEN		when
		);
extern ABObj	objxm_comp_get_target_for_builtin_action(
		    ABObj 		targetObj, 
		    AB_BUILTIN_ACTION	builtin_action
		);
extern STRING	objxm_get_resource_for_when(
  		    ABObj	sourceObj,
		    AB_WHEN	when
		);
extern STRING	objxm_get_resource_for_builtin_action(
		    ABObj		targetObj, 
		    AB_BUILTIN_ACTION	builtin_action
		);


		/* Return number of resource args presently
		 * set in ArgList
		 */
extern int      objxm_get_num_args(
                    ArgList     arglist
                );

		/* Return number of resource args presently in object's
		*  ui_arg_list
		*/
extern int	objxm_obj_get_num_ui_args(
  		    ABObj	obj
		);

		/* Return default motif widget-class name
		 * for a given object's type
		 */
extern STRING   objxm_obj_get_default_motif_class(
            	    ABObj    obj
        	);
		/* Return to Motif Create function for
		 * the Obj
		 */
extern XmCreateFunc
                objxm_obj_get_xmcreate_func(
                    ABObj       obj
                );

		/* Set resource (name/value) in ArgList
		 */
extern int      objxm_set_arg( 
		    ArgList	*p_arglist,
                    AB_ARG_TYPE type,
                    String      name,
                    XtArgVal    value
                );
		/* Set a Literal type (name/value) in ArgList
		 */
extern int	objxm_set_literal_arg(
		    OBJXM_CONFIG_TYPE 	ctype,
		    ArgList		*p_arglist,
		    String		name,
		    unsigned char 	literal
		);
                /* Set a Widget type (name/value) in ArgList 
                 */ 
extern int      objxm_set_widget_arg( 
                    OBJXM_CONFIG_TYPE 	ctype, 
                    ArgList     	*p_arglist, 
                    String      	name, 
		    ABObj		w_obj
                ); 

                /* Set resource (name/value) in ArgList
		 * only if not already present
                 */
extern int      objxm_set_arg_if_new(
                    ArgList     *p_arglist,
                    AB_ARG_TYPE type,
                    String      name,
                    XtArgVal    value
                );
 
		/* Remove resource (name/value) from ArgList
		 */
extern int      objxm_remove_arg(
                    ArgList     *p_arglist,   
                    String      name
                );
 
		/* Remove all resources from ArgList
		 */
extern void     objxm_remove_all_args(
                    ArgList     *p_arglist,
		    OBJXM_CONFIG_TYPE	ctype    
                );

extern Arg      *objxm_get_arg(
                    ArgList     arglist,
                    char        *name
                );

		/* FOR DEBUG PURPOSES ONLY */
extern int	objxm_dump_arglist(
    		    ABObj       obj,
    		    ArgList     args,
    		    int         num_args
		);

extern int	objxm_dump_arglist_indented(
    		    ABObj       obj,
    		    ArgList     args,
    		    int         num_args,
		    int		spaces
		);
		/* FOR DEBUG PURPOSES ONLY */
extern int	objxm_dump_widget_geometry(
		    Widget	widget
		);


/*
 * Xm String cache routines
 */
		/* Load internal resource tables with 
		 * required Xt/Xm resources/enums/createfuncs
		 */
extern void     objxm_strs_init(
                );

		/* return "strname" corresponding to xmname 
		 */
extern ISTRING  objxm_get_res_strname(
                    String    xmname
                );
 
		/* return "xmname" corresponding to strname 
		 */
extern String   objxm_get_res_xmname(
                    ISTRING    strname
                );
		/* return type corresponding to resource strname 
		 */
extern AB_ARG_TYPE objxm_get_istr_res_type(
                    ISTRING    strname
                );
		/* return type corresponding to resource xmname 
		 */
extern AB_ARG_TYPE objxm_get_res_type(
                    String      xmname
                );

		/* return arg class corresponding to resource strname 
		 */
extern AB_ARG_CLASS objxm_get_istr_res_class(
                    ISTRING    strname
                );
		/* return arg class corresponding to resource xmname 
		 */
extern AB_ARG_CLASS objxm_get_res_class(
                    String      xmname
                );
		/* return whether or not the value is defined as the
		 * "default" for that resource
		 */
extern BOOL	objxm_res_value_is_default(
		    String	xmname,
		    WidgetClass	wclass,
		    XtArgVal	value
		);

		/* return "strdef" corresponding to xmdef 
		 */
extern ISTRING  objxm_get_enum_strdef(
		    String	  xmname,
                    unsigned char xmdef
                );
 
		/* return "xmdef" corresponding to strdef 
		 */
extern unsigned char
                objxm_get_enum_xmdef(
		    String	xmname,
                    ISTRING     strdef
                );
		/* return "ftnstr" corresponding to ftn 
		 */
extern ISTRING  objxm_get_xmcreate_ftnstr(
                    XmCreateFunc    ftn
                );
 
		/* return "ftn" corresponding to ftnstr 
		 */
XmCreateFunc    objxm_get_xmcreate_xmdef(
                    ISTRING ftnstr
                );
 

/*
 * Xm Utility routines
 */

extern BOOL	objxm_color_exists(
		    String	colorname
		);

extern int	objxm_name_to_pixel(
		    Widget	widget,
                    String      colorname,
		    Pixel	*pixel_p
                );

extern int	objxm_filebase_to_pixmap(
		    Widget      widget,
                    String      filebase,
		    Pixmap	*pixmap_p
                );

extern STRING	objxm_pixmap_conversion_error_msg(
		    ABObj	obj,
		    STRING	filebase,
		    int		status
		);

extern XFontStruct *
                objxm_fontlist_to_font(
                    XmFontList  fontlist
                );

extern XmString objxm_str_to_xmstr(
		    Widget	widget,
		    String	string
		);

extern STRING   objxm_xmstr_to_str(
                    XmString    xmstr
                );

extern String	objxm_accel_to_acceltext(
    		    String      accelstr
		);


extern unsigned char
		objxm_linestyle_to_enum(
    		    AB_LINE_TYPE        linestyle
		);

extern BOOL	objxm_is_menu_widget(
		    Widget	widget
		);


/*
 * Debugging functions
 */
int 		objxm_print(ABObj obj);
int 		objxm_print_indented(ABObj obj, int spaces, int verbosity);
int 		objxm_tree_print(ABObj obj);
int 		objxm_tree_print_indented(ABObj obj, int spaces, int verbosity);


/*
 * Xm Resource Setting macros
 */
#define         objxm_obj_set_ui_arg(obj, type, name, val) \
                        objxm_set_arg((ArgList*)&((obj)->ui_args), type, name, (XtArgVal)val)

#define		objxm_obj_set_literal_ui_arg(obj, ctype, name, enum) \
			objxm_set_literal_arg(ctype, (ArgList*)&((obj)->ui_args), name, enum) 

#define		objxm_obj_set_widget_ui_arg(obj, ctype, name, w_obj) \
			objxm_set_widget_arg(ctype, (ArgList*)&((obj)->ui_args), name, w_obj) 

#define		objxm_obj_remove_ui_arg(obj, name) \
			objxm_remove_arg((ArgList*)&((obj)->ui_args), name)

#define		objxm_obj_remove_all_ui_args(obj, t) objxm_remove_all_args((ArgList*)&((obj)->ui_args), t)

#define		objxm_obj_get_ui_args(obj) ((ArgList)((obj)->ui_args))

#define		objxm_obj_get_num_ui_args(obj) \
		    (objxm_get_num_args(objxm_obj_get_ui_args(obj)))

#define		objxm_obj_get_ui_arg(obj,name) \
		    (objxm_get_arg(objxm_obj_get_ui_args(obj),(name)))


/*************************************************************************
 **									**
 ** 		INLINE IMPLEMENTATION					**
 **									**
 *************************************************************************/

#define objxm_set_widget(obj,w)	((obj)->ui_handle = ((void *)(w)))
#define objxm_get_widget(obj)	((Widget)((obj)->ui_handle))

#endif /* _OBJXM_H_ */
