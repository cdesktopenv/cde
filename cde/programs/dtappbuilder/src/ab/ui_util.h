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
 *      $XConsortium: ui_util.h /main/3 1995/11/06 17:56:54 rswiston $
 *
 * @(#)ui_util.h	1.29 14 Feb 1994      cde_app_builder/src/libABobj
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
 * ui_util.h 
 */
#ifndef _UI_UTIL_H_
#define _UI_UTIL_H_

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <ab_private/obj.h>
#include <ab_private/util.h>
#include "dtb_utils.h"


/*
 * Return "answer" values for Modal Messages
 */
typedef enum
{
	UI_ANSWER_NONE,
	UI_ANSWER_OK,
	UI_ANSWER_CANCEL,
	UI_ANSWER_HELP	
} UI_MODAL_ANSWER;



typedef struct _MENU_ITEM {
    char		*name;
    char                *label;
    AB_LABEL_TYPE	label_type;
    WidgetClass         *wclass;
    XtCallbackProc	callback;
    XtPointer           client_data;
    XtPointer		user_data;
    struct _MENU_ITEM   *subitems;
    int			num_columns;
    BOOL		active;
    int			pixwidth;
    int			pixheight;
    unsigned char	*bits;
} MenuItem;

/*
 * Data structure for object pixmaps
 */
typedef struct _UiObjPixmap
{
    AB_OBJECT_TYPE	type;
    int			subtype;
    char		*filename;
    Pixmap		pixmap;
    unsigned int	width;
    unsigned int	height;
} UiObjPixmap;


typedef Boolean (*ModalPromptVerifyProc)(Widget, XtPointer);

/* Data structure for userData on the modal prompt dialog created
 * by ui_popup_modal_prompt().
 */
typedef struct _Prompt_Info_Rec {
        ModalPromptVerifyProc   verify_proc; /* Hook to supply custom 
					      * verification routine of text 
					      * entered in the prompt dialog */
        XtPointer               client_data; /* Data to pass onto verification 
					      * routine */
        STRING                  *seln_str;  /* Stores text from prompt dialog */
} Prompt_Info_Rec;

/*
 * Function type for rubber band func
 */
typedef void    (*UiRubberBandFunc) (
    Widget,
    XEvent *,
    XRectangle *,
    XtPointer
);

		/*
		 * Move a Window to the front of stack
		 */ 
extern void     ui_win_front(
          	    Widget	widget
		);

		/*
		 * Show or Hide a Window
		 */
extern void	ui_win_show(
		    Widget	widget,
		    BOOL	show,
		    XtGrabKind  grab_kind
		);

extern void	ui_win_set_resizable(
		    Widget	widget,
		    BOOL	resizable,
		    BOOL	remap
		);

		/*
		 * Popup a Simple InfoMessage Dialog in front
		 * of a window (parent)
		 */
extern void	ui_popup_info_message(
		    Widget	parent,
		    STRING	title,
		    STRING	message
		);

		/*
		 * Popup up a MessageDialog of "msg_type"
		 * Any widget can be passed in as the "parent";
		 * the routine will use this widget's shell ancestor
		 * as the actual parent of the Dialog.
		 *
		 * If a button_label parameter is NULL, the 
		 * corresponding button will not appear in the
		 * dialog.  'callback' parameters can also be NULL.
		 *
		 * This routine takes care of dynamic creation/destruction
		 * of the MessageDialog.
		 */
extern void	ui_popup_message(
		    Widget	   parent,
		    unsigned char  msg_type,
		    STRING	   title,
		    STRING	   message,
		    STRING	   ok_label,
		    XtCallbackProc ok_callback,
		    STRING	   cancel_label,
		    XtCallbackProc cancel_callback,
		    STRING	   help_label,
		    XtCallbackProc help_callback,
		    unsigned char  default_button_type
		);

		/* Popup an Application-MODAL MessageDialog of "msg_type".
		 * Any widget can be passed in as the "parent";
                 * the routine will use this widget's shell ancestor
                 * as the actual parent of the Dialog.
                 *
                 * If a button_label parameter is NULL, the
                 * corresponding button will not appear in the dialog.
	 	 *
		 * This routine returns the "answer" corresponding to
		 * the button the user pressed on the dialog:
		 *    UI_ANSWER_OK, UI_ANSWER_CANCEL, UI_ANSWER_HELP
		 */
extern UI_MODAL_ANSWER
		ui_popup_modal_message(
                    Widget         parent,
                    unsigned char  msg_type,
		    STRING	   title,
                    STRING         message,
                    STRING         ok_label,
                    STRING         cancel_label,
                    STRING         help_label,
                    unsigned char  default_button_type,
		    Widget	   *modal_dlg
                );

		/*
		 * Set the LabelString resource on a widget
		 * (converts it to XmString internally)
		 */
extern void	ui_set_label_string(
		    Widget	widget,
		    STRING	label
		);

		/*
		 * Set the XmNlabel resource on a widget from the image file 
		 */
extern void	ui_set_label_glyph(
		    Widget	widget,
		    STRING	fileName
		);

		/* Set a widget (and it's immediate children)
		 * sensitive or insensitive
		 */
extern void	ui_set_active(
		    Widget widget,
		    BOOL   state
		);

		/* 
		 * Set a widget to be visible or invisible
		 * (unmanages, so space will collapse)
		 */
extern void	ui_set_visible(
		    Widget	widget,
		    BOOL	viz
		);

		/* Set the value-string in a TextField
		 */
extern void	ui_field_set_string(
    		    Widget   field,
    		    STRING   valuestr
		);

		/* Get/alloc the value-string of a TextField
		 * NOTE: caller must free memory allocated
		 * by this call
		 */
extern STRING	ui_field_get_string(
    		    Widget   field
		);

		/* Make the string inside a TextField selected
		 */
extern void	ui_field_select_string(
		    Widget	field,
		    BOOL	assign_focus
		);

extern void	ui_field_set_editable(
		    Widget	field,
		    BOOL	editable
		);

extern Widget	ui_get_ancestor_shell(
		    Widget	widget
		);

extern Widget	ui_get_ancestor_dialog(
		    Widget	widget
		);

extern Widget	ui_build_menu(
		    Widget	parent,
		    int		menu_type,
		    int		num_columns,
		    char	*menu_title,
		    char	*menu_name,
		    MenuItem	*menu_items
		);
extern void	ui_populate_pulldown_menu(
		    Widget	menu,
		    int		num_columns,
		    MenuItem	*menu_items
);
extern void	ui_size_to_row_col(
    		    Widget         text,
    		    unsigned short width,
    		    unsigned short height,
    		    int         *row_ptr,
    		    int         *col_ptr
		);

extern int	ui_set_busy_cursor(
		    Window	window,
		    BOOL	on
		);

extern void	ui_get_obj_pixmap(
		    AB_OBJ		*obj,
		    Pixmap		*pixmap,/* RETURN */
		    unsigned int	*width,	/* RETURN */
		    unsigned int	*height	/* RETURN */
		);

/*
 * ComboBox Widget Manipulation utilities
 */
extern int	ui_combobox_add_item(
		    Widget	combobox,
		    STRING	item,
		    int		pos,
  		    BOOL	unique
		);

extern int	ui_combobox_clear(
                    Widget      combobox
                );

extern Widget   ui_combobox_get_list_widget(
                    Widget      combobox
                );
 
extern Widget   ui_combobox_get_field_widget(
                    Widget      combobox
                );

extern int	ui_combobox_select_item(
                    Widget      combobox,
                    STRING      item
		);

/*
 * ScrolledList Widget Manipulation utilities
 */
extern int	ui_list_add_item(
		    Widget	list,
		    STRING	item_str,
		    int		pos
		);

extern int	ui_list_find_item(
		    Widget	list,
		    STRING	item_str,
		    int		*pos_out
		);

extern int	ui_list_get_info(
		    Widget	list,
		    int		*num_items_out,
		    int		*first_visible_out,
		    int		*last_visible_out
		);

extern int	ui_list_delete_item(
		    Widget	list,
		    STRING	item_str
		);

extern int	ui_list_get_selected_pos(
		    Widget	list
		);

extern int	ui_list_replace_item(
		    Widget	list,
		    STRING	old_item_str,
		    STRING	new_item_str
		);

extern int	ui_list_replace_item_prefix(
		    Widget	list,
		    STRING	old_prefix_str,
		    STRING	new_prefix_str
		);

extern int	ui_list_select_item(
		    Widget	list,
		    STRING	item_str,
		    BOOL	invoke_callback
		);

extern int	ui_list_select_pos(
		    Widget	list,
		    int		item_pos,
                    BOOL        invoke_callback
                );

extern void	ui_popup_prompt(
		    Widget              parent,
		    STRING              title,
		    STRING              label,
		    STRING              initial_text,
		    STRING              ok_label,
		    XtCallbackProc      ok_callback,
		    XtPointer           ok_clientData,
		    STRING              cancel_label,
		    XtCallbackProc      cancel_callback,
		    XtPointer           cancel_clientData,
		    STRING              help_label,
		    XtCallbackProc      help_callback,
		    XtPointer           help_clientData,
		    unsigned char       default_button_type
		);
extern void	ui_add_window_close_callback(
		    Widget		shell,
		    XtCallbackProc 	callback,
		    XtPointer		client_data,
                    unsigned char       delete_response
		);
extern void	ui_remove_window_close_callback(
		    Widget	shell,
		    XtCallbackProc callback,
		    XtPointer	client_data
		);

extern void     ui_add_window_iconify_handler(
                    Widget      shell,
                    XtEventHandler iconify_proc,
                    XtPointer   client_data
                );

extern void     ui_remove_window_iconify_handler(
                    Widget      shell,
                    XtEventHandler iconify_proc,
                    XtPointer   client_data
                );

extern UI_MODAL_ANSWER 
		ui_popup_modal_prompt(
		    Widget              parent,
		    STRING              title,
		    STRING              label,
		    STRING              initial_text,
		    STRING              ok_label,
		    STRING              cancel_label,
		    STRING              help_label,
		    DtbObjectHelpData	help_data,
		    unsigned char       default_button_type,
		    STRING              *seln_str,
		    ModalPromptVerifyProc verify_proc,
		    XtPointer		client_data
		);

extern void	ui_refresh_widget_tree(
		    Widget		widget
		);

extern int	ui_sync_display_of_widget(
		    Widget widget
		);

extern int	ui_initiate_rubberband(
		    Widget      	widget,
		    Boolean     	confine_to_window,
		    UiRubberBandFunc	rb_func,
		    XtPointer   	client_data
		);

extern void	ui_button_drag(
		    Widget      widget,
		    XEvent      *event,
		    XtPointer   client_data
		);

extern void     ui_rubberband_finish(
		    Widget	widget,
                    XEvent	*event,
                    XtPointer	client_data
                );

extern Widget 	ui_optionmenu_add_item(
		    Widget      opmenu,
		    STRING      item_str
		);

extern int 	ui_optionmenu_delete_item(
		    Widget      opmenu,
		    STRING      item_str
		);

extern Widget 	ui_optionmenu_replace_item(
		    Widget      opmenu,
		    STRING      old_item_str,
		    STRING      new_item_str
		);

extern Widget 	ui_optionmenu_find_item(
		    Widget      opmenu,
		    STRING      item_str
		);

extern int 	ui_optionmenu_num_items(
		    Widget      opmenu
		);

extern void 	ui_optionmenu_change_label(
		    Widget      opmenu,
		    STRING      new_str
		);

extern void 	ui_optionmenu_change_label_pixmap(
		    Widget      opmenu,
		    Pixmap	pixmap
		);

		/*
		** Set the label string on an object
		** (converts it to XmString internally, if needed)
		*/
extern void	ui_obj_set_label_string(
                    ABObj       obj,
		    STRING	label
		);

		/*
		** Set the label glyph (ie graphic) on an object
		*/
extern void	ui_obj_set_label_glyph(
                    ABObj       obj,
		    STRING	fileName
		);

		/*
		** Set the label on an object
		**     string and glyphs are supported
		*/
extern void	ui_obj_set_label(
                    ABObj       obj,
		    STRING	label
		);

#endif /* _UI_UTIL_H_ */
