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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */

/*
 *	$XConsortium: prop.h /main/5 1996/08/08 17:58:47 mustafa $
 *
 * @(#)prop.h	1.75 95/04/20 Copyright 1993 Sun Microsystems
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
 * prop.h - defines structures/routines required for property sheets
 */
#ifndef _PROPS_H_
#define _PROPS_H_

#include <X11/Intrinsic.h>
#if !defined(CSRG_BASED)
#include <values.h>
#endif
#include <limits.h>
#include <ab_private/obj.h>
#include <ab_private/pal.h>
#include <ab_private/abobj_edit.h>
#include "revolv_ui.h"

#define SIZE_OF_CONTENTS_KEY    -1
#define SIZE_FIXED_KEY		0
#define SIZE_IN_CHARS_KEY	88
#define SIZE_IN_PIXELS_KEY	99
#define	EDIT_CHANGE		111
#define HELP_ITEM_KEY		222
#define DISMISS_ITEM_KEY 	333
#define ACTION1_ITEM_KEY	444
#define ACTION2_ITEM_KEY	555
#define ACTION3_ITEM_KEY	777
#define CANCEL_ITEM_KEY		888
#define SHOW_VALUE_KEY		999

/*
 * Load Flags - defines which attributes need to be loaded
 */
#define LoadClear		0x0000
#define LoadAll			0x0001
#define LoadName		0x0002
#define LoadPosition		0x0004
#define LoadSize		0x0008

/*
 * Enum values for Changebar State
 */
typedef enum
{
	PROP_CB_OFF,
	PROP_CB_ON,
	PROP_CB_NUM_VALUES
} PROP_CB_STATE;

#define	ERR_ATOI	MAXINT

/*
 * Item Editing Insert choices
 */
typedef enum
{
	INSERT_AFTER = 6, /* prevent conflict with AB_EDIT_ enums */
	INSERT_BEFORE,
	INSERT_TOP,
	INSERT_BOTTOM,
	INSERT_NUM_VALUES
} INSERT_TYPE;

/*
 * Geometry Keys
 */
typedef enum
{
	GEOM_X,
	GEOM_Y,
	GEOM_WIDTH,
	GEOM_HEIGHT,
	GEOM_NUM_VALUES
} GEOM_KEY;

extern char *LabelForString;
extern char *LabelForGraphic;
extern char *NoneItem;
extern char *XFieldStr;
extern char *YFieldStr;
extern char *WFieldStr;
extern char *HFieldStr;
extern char *OffsetFieldStr;
extern char *PercentageFieldStr;
extern char *RowColFieldStr;
extern char *VertSpacingFieldStr;
extern char *HorizSpacingFieldStr;

/*
 * Structures to store Prop Sheet Setting Widgets
 */
typedef struct _PROP_CHECKBOX_SETTING {
    Widget      label;
    Widget      checkbox;
    Widget      changebar;
} PropCheckboxSettingRec, *PropCheckboxSetting;

typedef struct _PROP_COLOR_SETTING {
    Widget      label;
    Widget      menubutton;
    Widget      swatch;
    Widget      field;
    Widget      changebar;
} PropColorSettingRec, *PropColorSetting;

typedef struct _PROP_OBJ_COMBOBOX_SETTING {
    Widget	label;
    Widget	combobox;
    Widget	list;
    Widget	changebar;
    ABObj	*current_obj_ptr;
} PropObjComboboxSettingRec, *PropObjComboboxSetting;

typedef struct _PROP_FIELD_SETTING {
    Widget	label;
    Widget	field;
    Widget	changebar;
} PropFieldSettingRec, *PropFieldSetting;

typedef struct _PROP_FOOTER_SETTING {
    Widget	label;
    Widget	checkbox;
    Widget	field;
    Widget	changebar;
} PropFooterSettingRec, *PropFooterSetting;

typedef struct _PROP_GEOMETRY_SETTING {
    Widget      label;
    Widget      x_label;
    Widget      x_field;
    Widget      y_label;
    Widget      y_field;
    Widget      w_label;
    Widget      w_field;
    Widget      h_label;
    Widget      h_field;
    Widget      changebar;
} PropGeometrySettingRec, *PropGeometrySetting;

typedef struct _PROP_MENUNAME_SETTING {
    AB_PROP_TYPE prop_type;
    Widget	label;
    Widget      menubutton;
    Widget      field;
    Widget      owner_name_field;
    Widget      changebar;
    PropFieldSetting menu_title_pfs;
    ABObj       *current_obj_ptr;
} PropMenunameSettingRec, *PropMenunameSetting;

typedef struct _PROP_OPTIONS_SETTING {
    Widget      label;
    Widget      optionbox;
    Widget      menu;
    Widget      changebar;
} PropOptionsSettingRec, *PropOptionsSetting;

typedef struct _PROP_OBJ_OPTIONS_SETTING {
    PropOptionsSettingRec	options;
    BOOL			display_module;
    ABObj       		*current_obj_ptr;
    ABObjTestFunc 		obj_test_func;
    ABObj			search_root;
} PropObjOptionsSettingRec, *PropObjOptionsSetting;

typedef struct _PROP_RADIO_SETTING {
    Widget	label;
    Widget	radiobox;
    Widget	changebar;
} PropRadioSettingRec, *PropRadioSetting;

typedef struct _PROP_ITEMS_SETTING {
    AB_ITEM_TYPE          item_type;
    Widget                item_list;
    Widget		  item_menu;
    PropFieldSetting      item_label_pfs;
    PropOptionsSetting    item_label_type_pos;
    PropFieldSetting      item_mnemonic_pfs;
    PropFieldSetting      item_accel_pfs;
    PropOptionsSetting    item_line_style_pos;
    PropCheckboxSetting   item_state_pcs;
    PropMenunameSetting   item_menuname_pms;
    Widget                changebar;
    ABObj                 current_item;
    ABObj                 clipboard_item;
    ABObj                 *current_obj_ptr;
} PropItemsSettingRec, *PropItemsSetting;

/*
 * Prop Dialog Management Functions
 */
		/* Prop module Initialization
		 */
extern void     prop_init(
                    Widget toplevel
                );

                /* Property Dialog "Popup" routines
                 */
extern void     prop_show_dialog(
                    Widget      w,
                    XtPointer   client_data,
                    XtPointer   call_data
                );
extern void     prop_show_fixed(
                    PalItemInfo *palitem
                );
extern void     prop_show_menu_props(
                    Widget      w,
                    XtPointer   client_data,
                    XtPointer   call_data
                );
extern void     prop_show_group_props(
                    Widget      w,
                    XtPointer   client_data,
                    XtPointer   call_data
                );
extern void     prop_load_obj(
                    ABObj        obj,
                    AB_PROP_TYPE ptype
                );
		/* Prop Dialog Initialization Functions
		 */
extern void	prop_activate_panel_init(
                    AB_PROP_TYPE type,
                    PalItemInfo *palitem,
		    Widget	ok_button,
    		    Widget      apply_button,
    		    Widget      reset_button,
    		    Widget      cancel_button,
		    Widget	help_button
		);
extern void	prop_editors_panel_init(
    		    AB_PROP_TYPE type,
    		    PalItemInfo *palitem,
    		    Widget	attach_ed_button,
    		    Widget	conn_ed_button,
    		    Widget	help_ed_button
		);
extern void     prop_fixed_dialog_init(
		    PalItemInfo		*palitem,
                    Widget              dialog,
                    Widget              list
                );

		/* Checkbox Setting Functions
	 	 */
extern void	prop_checkbox_init(
		    PropCheckboxSetting	pcs,
		    Widget		label,
		    Widget		checkbox,
		    int			num_items,
		    WidgetList		items,
		    int			*item_keys,
		    Widget		changebar
		);
extern BOOL	prop_checkbox_get_value(
		    PropCheckboxSetting pcs,
		    int			itemkey
		);
extern int	prop_checkbox_set_value(
		    PropCheckboxSetting pcs,
		    int			itemkey,
		    BOOL		value,
		    BOOL		trip_changebar
		);

		/* Color Field Functions
		 */
extern void	prop_colorfield_init(
		    PropColorSetting	pcs,
		    Widget		menubutton,
		    Widget		none_item,
		    Widget		chooser_item,
		    Widget		swatch,
		    Widget		field,
		    Widget		changebar
		);
extern int	prop_colorfield_set_value(
		    PropColorSetting    pcs,
		    STRING		colorname, /* NULL clears swatch */
		    BOOL		trip_changebar
		);
                /* Caller must free memory allocated for return String */
extern STRING	prop_colorfield_get_value(
		    PropColorSetting	pcs
		);

/* REMIND: ComboBox has too many bugs to use right now; if
 *  	   we never revert back to using it, delete these functions
 */
		/* Object Combobox Functions
		 */
extern void     prop_obj_combobox_init(
                    PropObjComboboxSetting 	pcs,
                    Widget              	label,
                    Widget              	combobox,
                    Widget              	changebar,
		    ABObj			*current_obj_ptr
                );

extern ABObj	prop_obj_combobox_get_value(
		    PropObjComboboxSetting	pcs
		);

extern int	prop_obj_combobox_set_value(
		    PropObjComboboxSetting	pcs,
		    ABObj			obj,
		    BOOL			trip_changebar
		);

extern void	prop_obj_combobox_load(
		    PropObjComboboxSetting      pcs,
		    ABObj			root,
		    ABObjTestFunc		obj_test_func
		);


		/* TextField Functions
		 */
extern void	prop_field_init(
		    PropFieldSetting	pfs,
		    Widget		label,
		    Widget		field,
		    Widget		changebar
		);
		/* Caller must free memory allocated for return String */
extern STRING	prop_field_get_value(
		    PropFieldSetting	pfs
		);
extern int	prop_field_get_numeric_value(
		    PropFieldSetting	pfs
		);
extern int	prop_field_set_value(
		    PropFieldSetting    pfs,
		    STRING		value,
		    BOOL		trip_changebar
		);
extern int	prop_field_set_numeric_value(
		    PropFieldSetting    pfs,
		    int			value,
		    BOOL		trip_changebar
		);

		/* Footer Setting Functions
		 */
extern void	prop_footer_init(
    		    PropFooterSetting    pfs,
    		    Widget              label,
    		    Widget              checkbox,
		    int		        num_items,
		    WidgetList		items,
		    int		       *item_keys,
		    Widget	        field,
		    Widget		changebar
		);

		/* Item Editor Functions
		 */
extern void	prop_item_editor_init(
		    PropItemsSetting    pis,
    		    AB_ITEM_TYPE        type,
    		    Widget              list,
    		    Widget              changebar,
		    Widget		add_button,
    		    int                 insert_count,
    		    WidgetList          insert_items,
    		    int                 *insert_enums,
    		    int                 edit_count,
    		    WidgetList          edit_items,
    		    int                 *edit_enums,
    		    PropFieldSetting    label_pfs,
    		    PropOptionsSetting  label_type_pos,
		    Widget		label_graphic_hint,
		    PropFieldSetting	mnemonic_pfs,
    		    PropFieldSetting    accel_pfs,
    		    PropOptionsSetting  line_style_pos,
    		    PropCheckboxSetting state_pcs,
		    PropMenunameSetting	menuname_pms,
    		    ABObj               *current_obj_ptr
		);
extern void	prop_item_editor_apply(
                    PropItemsSetting    pis
                );
extern void     prop_item_editor_clear(
                    PropItemsSetting    pis
                );
extern void	prop_item_editor_load(
		    PropItemsSetting	pis,
		    ABObj		obj
		);
extern void	prop_item_change(
		    PropItemsSetting    pis,
		    BOOL		reselect
                );
extern void	prop_item_edit(
		    PropItemsSetting	pis,
		    AB_EDIT_TYPE	etype
		);
extern STRING	prop_item_get_namebase(
		    ABObj		iobj
		);
extern int	prop_item_insert(
		    PropItemsSetting	pis,
		    INSERT_TYPE		itype,
		    ABObj		iobj
		);
extern void	prop_item_labeltypeCB(
		    Widget		w,
		    XtPointer		client_data,
		    XtPointer		call_data
		);

		/* Geometry Field Functions
		 */
extern void	prop_geomfield_init(
    		    PropGeometrySetting pgs,
    		    Widget              label,
    		    Widget              x_label,
    		    Widget              x_field,
    		    Widget              y_label,
    		    Widget              y_field,
    		    Widget              w_label,
    		    Widget              w_field,
    		    Widget              h_label,
    		    Widget              h_field,
    		    Widget              changebar
		);
extern int	prop_geomfield_get_value(
		    PropGeometrySetting	pgs,
		    GEOM_KEY		gkey
		);
extern int	prop_geomfield_set_value(
		    PropGeometrySetting pgs,
		    GEOM_KEY		gkey,
		    int			gvalue,
		    BOOL		trip_changebar
		);

extern int 	prop_geomfield_clear(
		    PropGeometrySetting pgs,
		    GEOM_KEY            gkey
		);

extern void	prop_size_policyCB(
		    Widget		w,
		    XtPointer		client_data,
		    XtPointer		call_data
		);

extern void	prop_load_obj_position(
		    ABObj		obj,
		    PropGeometrySetting	pgs
		);

extern void     prop_load_obj_size(
                    ABObj               obj,
                    PropGeometrySetting pgs
                );

		/* Menu Name Field Functions
		 */
extern void	prop_menuname_init(
    		    PropMenunameSetting pms,
		    AB_PROP_TYPE	ptype,
		    Widget		label,
    		    Widget              menubutton,
    		    Widget              field,
    		    Widget              owner_name_field,
    		    Widget              changebar,
		    PropFieldSetting	menu_title_pfs,
                    ABObj               *current_obj_ptr,
		    BOOL		new_item_ok
		);
                /* Caller must free memory allocated for return String */
extern STRING	prop_menuname_get_value(
		    PropMenunameSetting	pms
		);
extern int	prop_menuname_set_value(
		    PropMenunameSetting pms,
		    STRING		value,
		    BOOL		trip_changebar
		);

		/* Option Menu Functions
		 */
extern void     prop_options_init(
                    PropOptionsSetting 	pos,
                    Widget              label,
                    Widget              optionbox,
		    Widget		menu,
                    int                 num_items,
                    WidgetList          items,
                    XtPointer           *item_vals,
                    Widget              changebar
                );
extern XtPointer    prop_options_get_value(
		    PropOptionsSetting  pos
		);
extern int	prop_options_set_value(
		    PropOptionsSetting  pos,
		    XtPointer           value,
		    BOOL		trip_changebar
		);
extern int	prop_options_remove_value(
		    PropOptionsSetting	pos,
		    XtPointer         	value,
		    BOOL		trip_changebar
		);

                /* Object OptionMenu Functions
                 */
extern void     prop_obj_options_init(
                    PropObjOptionsSetting       pos,
                    Widget                      label,
                    Widget                      optionbox,
		    Widget			menu,
                    int                 	num_items,
                    WidgetList          	items,
                    XtPointer                 	*item_vals,
                    Widget                      changebar,
		    BOOL			display_module,
                    ABObj                       *current_obj_ptr,
		    ABObjTestFunc		obj_test_func
                );

extern ABObj    prop_obj_options_get_value(
                    PropObjOptionsSetting       pos
                );

extern int      prop_obj_options_set_value(
                    PropObjOptionsSetting       pos,
                    ABObj                       obj,
                    BOOL                        trip_changebar
                );

extern void     prop_obj_options_load(
                    PropObjOptionsSetting       pos,
                    ABObj                       search_root
                );

		/* Radiobox Functions
		 */
extern void     prop_radiobox_init(
                    PropRadioSetting 	prs,
                    Widget              label,
                    Widget              radiobox,
                    int                 num_items,
                    WidgetList          items,
                    XtPointer           *item_vals,
                    Widget              changebar
		);
extern int	prop_radiobox_get_value(
                    PropRadioSetting    prs
                );
extern int	prop_radiobox_set_value(
                    PropRadioSetting    prs,
		    XtPointer		value,
		    BOOL		trip_changebar
		);

extern void	prop_label_field_init(
		    PropFieldSetting	label_pfs,
		    Widget		graphic_hint,
		    WidgetList		labeltype_items,
		    int			num_items
		);
extern void	prop_setup_label_field(
		    PropFieldSetting	label_pfs,
		    PropOptionsSetting	label_line_style_pos,
		    AB_LABEL_TYPE 	ltype,
		    STRING		label,
		    AB_LINE_TYPE	line_style
		);

		/*
		 * Changebar Management Routines
		 */
extern BOOL     prop_changed(
                    Widget      changebar
                );
extern BOOL	prop_changebars_pending(
                    Widget      prop_form
                );
extern void	prop_changebars_cleared(
		    Widget	prop_form
		);
extern void     prop_set_changebar(
                    Widget 	changebar,
		    int 	state
                );

		/*
		 * Prop Setting Verification Routines
		 */
extern int	prop_str_to_int(
		    STRING	str
		);

extern BOOL	prop_string_ok(
		    Widget	field,
		    const char *chars,
		    BOOL	display_notice
		);

extern BOOL	prop_identifier_ok(
		    Widget	field
		);

extern BOOL	prop_name_ok(
		    ABObj	obj,
		    Widget	field
		);

extern BOOL	prop_graphic_filename_ok(
		    Widget	field,
		    BOOL	empty_ok
		);

extern BOOL	prop_obj_name_ok(
		    Widget	field,
		    ABObj	root,
		    AB_OBJECT_TYPE objtype,
		    STRING	objtype_name
		);

extern BOOL	prop_submenu_name_ok(
		    Widget	field,
		    ABObj	owner
		);

extern BOOL	prop_color_ok(
		    Widget	field
		);

extern BOOL	prop_number_ok(
		    Widget	field,
		    STRING	field_name,
		    int		min,
		    int		max
		);

extern BOOL	prop_help_item_ok(
		    Widget	list,
		    ABObj	item_obj
		);


#endif /* _PROP_H_ */
