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
 *	$XConsortium: gilP.h /main/3 1995/11/06 18:27:37 rswiston $
 *
 *	@(#)gilP.h	1.14 30 Jan 1995	cde_app_builder/src/libABil
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
 * gil.h - gil file defines, et cetera.
 */
#ifndef _ABIL_GIL_P_H_
#define _ABIL_GIL_P_H_

#include <ab_private/obj.h>

/*
 * GIL file attributes
 */
typedef enum
{
	AB_GIL_UNDEF,
	AB_GIL_ABBREVIATED,
	AB_GIL_ACTION,				/* actions - one action "type"*/
	AB_GIL_ACTIONS,				/* actions - list of actions */
	AB_GIL_ANCHOR_OBJECT,
	AB_GIL_ANCHOR_POINT,
	AB_GIL_ARG_TYPE,			/* actions */
	AB_GIL_BACKGROUND_COLOR,
	AB_GIL_BUSY_DROP_GLYPH,
	AB_GIL_BUTTON_TYPE,
	AB_GIL_CHOICES,
	AB_GIL_CHOICE_DEFAULTS,
	AB_GIL_CHOICE_LABEL_TYPES,
	AB_GIL_CHOICE_COLORS,
	AB_GIL_COLUMNS,
	AB_GIL_COL_ALIGNMENT,
	AB_GIL_CONNECTIONS,
	AB_GIL_CONSTANT_WIDTH,
	AB_GIL_DEFAULT_DROP_SITE,
	AB_GIL_DND_ACCEPT_CURSOR,
	AB_GIL_DND_ACCEPT_CURSOR_XHOT,
	AB_GIL_DND_ACCEPT_CURSOR_YHOT,
	AB_GIL_DND_CURSOR,
	AB_GIL_DND_CURSOR_XHOT,
	AB_GIL_DND_CURSOR_YHOT,
	AB_GIL_DRAGGABLE,
	AB_GIL_DONE_HANDLER,
	AB_GIL_DROPPABLE,
	AB_GIL_DROP_TARGET_WIDTH,
	AB_GIL_DRAWING_MODEL,
	AB_GIL_EVENTS,
	AB_GIL_EVENT_HANDLER,
	AB_GIL_FILE_CHOOSER_FILTER_PATTERN,
	AB_GIL_FILE_CHOOSER_MATCH_GLYPH,
	AB_GIL_FILE_CHOOSER_MATCH_GLYPH_MASK,
	AB_GIL_FILE_CHOOSER_TYPE,
	AB_GIL_FOREGROUND_COLOR,
	AB_GIL_FROM,			/* actions */
	AB_GIL_FUNCTION_TYPE,		/* actions */
	AB_GIL_GROUP_TYPE,
	AB_GIL_HEIGHT,
	AB_GIL_HELP,
	AB_GIL_HOFFSET,
	AB_GIL_HSCROLL,
	AB_GIL_HSPACING,
	AB_GIL_ICON_FILE,
	AB_GIL_ICON_LABEL,
	AB_GIL_ICON_MASK_FILE,
	AB_GIL_INITIAL_LIST_GLYPHS,
	AB_GIL_INITIAL_LIST_VALUES,
	AB_GIL_INITIAL_SELECTIONS,
	AB_GIL_INITIAL_STATE,
	AB_GIL_INITIAL_VALUE,
	AB_GIL_INTERFACES,
	AB_GIL_INTERNATIONAL_DB_BEGIN,
	AB_GIL_INTERNATIONAL_DB_END,
	AB_GIL_LABEL,
	AB_GIL_LABEL_TYPE,
	AB_GIL_LABEL_BOLD,
	AB_GIL_LAYOUT_TYPE,
	AB_GIL_MAPPED,
	AB_GIL_MAX_TICK_STRING,
	AB_GIL_MAX_VALUE,
	AB_GIL_MAX_VALUE_STRING,
	AB_GIL_MEMBERS,
	AB_GIL_MENU,
	AB_GIL_MENU_HANDLER,
	AB_GIL_MENU_ITEM_ACCELERATORS,
	AB_GIL_MENU_ITEM_COLORS,
	AB_GIL_MENU_ITEM_DEFAULTS,
	AB_GIL_MENU_ITEM_HANDLERS,
	AB_GIL_MENU_ITEM_LABELS,
	AB_GIL_MENU_ITEM_LABEL_TYPES,
	AB_GIL_MENU_ITEM_MENUS,
	AB_GIL_MENU_ITEM_STATES,
	AB_GIL_MENU_TITLE,
	AB_GIL_MENU_TYPE,
	AB_GIL_MIN_TICK_STRING,
	AB_GIL_MIN_VALUE,
	AB_GIL_MIN_VALUE_STRING,
	AB_GIL_MULTIPLE_SELECTIONS,
	AB_GIL_NAME,
	AB_GIL_NORMAL,
	AB_GIL_NORMAL_DROP_GLYPH,
	AB_GIL_ACTIVATE_HANDLER,
	AB_GIL_ORIENTATION,
	AB_GIL_OWNER,
	AB_GIL_PINNABLE,
	AB_GIL_PINNED,
	AB_GIL_READ_ONLY,
	AB_GIL_REFERENCE_POINT,
	AB_GIL_REPAINT_PROC,
	AB_GIL_RESIZABLE,
	AB_GIL_ATT_ROOT_WINDOW,
	AB_GIL_ROWS,
	AB_GIL_ROW_ALIGNMENT,
	AB_GIL_SCROLLABLE_HEIGHT,
	AB_GIL_SCROLLABLE_WIDTH,
	AB_GIL_SELECTION_REQUIRED,
	AB_GIL_SETTING_TYPE,
	AB_GIL_SHOW_BORDER,
	AB_GIL_SHOW_ENDBOXES,
	AB_GIL_SHOW_FOOTER,
	AB_GIL_SHOW_RANGE,
	AB_GIL_SHOW_VALUE,
	AB_GIL_SCALE_WIDTH,
	AB_GIL_STORED_LENGTH,
	AB_GIL_MAX_LENGTH,
	AB_GIL_TEXT_TYPE,
	AB_GIL_TICKS,
	AB_GIL_TITLE,
	AB_GIL_TO,			/* actions */
	AB_GIL_TYPE,
	AB_GIL_USER_DATA,
	AB_GIL_VALUE_LENGTH,
	AB_GIL_VALUE_UNDERLINED,
	AB_GIL_VALUE_X,
	AB_GIL_VALUE_Y,
	AB_GIL_VOFFSET,
	AB_GIL_VSCROLL,
	AB_GIL_VSPACING,
	AB_GIL_WHEN,			/* actions */
	AB_GIL_WIDTH,
	AB_GIL_X,
	AB_GIL_Y,
	AB_GIL_ATTRIBUTE_NUM_VALUES	/* must be last */
} AB_GIL_ATTRIBUTE;

/* gil.c */
AB_ALIGNMENT		gilP_string_to_alignment(STRING string);
AB_ARG_TYPE		gilP_string_to_arg_type(STRING string);
AB_GIL_ATTRIBUTE	gilP_string_to_att(STRING att);
AB_BUILTIN_ACTION	gilP_string_to_builtin_action(STRING string);
AB_BUTTON_TYPE		gilP_string_to_button_type(STRING string);
AB_CHOICE_TYPE		gilP_string_to_choice_type(STRING string);
AB_COMPASS_POINT	gilP_string_to_compass_point(STRING string);
int			gilP_string_to_file_chooser_type(STRING string);
AB_GROUP_TYPE		gilP_string_to_group_type(STRING string);
AB_LABEL_TYPE		gilP_string_to_label_type(STRING string);
AB_MENU_TYPE		gilP_string_to_menu_type(STRING string);
AB_OBJECT_STATE		gilP_string_to_object_state(STRING string);
AB_OBJECT_TYPE		gilP_string_to_object_type(STRING string);
AB_ORIENTATION		gilP_string_to_orientation(STRING string);
AB_TEXT_TYPE		gilP_string_to_text_type(STRING string);
AB_WHEN			gilP_string_to_when(STRING string);


/* gil_loadact.c */
int	gilP_load_att_done_handler(FILE *inFile, ABObj obj, ABObj interface);
int	gilP_load_att_event_handler(FILE *inFile, ABObj obj, ABObj interface);
int	gilP_load_att_menu_handler(FILE *inFile, ABObj obj, ABObj interface);
int	gilP_load_att_menu_item_handlers(FILE *inFile, ABObj obj, ABObj interface);
int	gilP_load_att_notify_handler(FILE *inFile, ABObj obj, ABObj interface);
int	gilP_load_att_repaint_proc(FILE *inFile, ABObj obj, ABObj interface);
int	gilP_load_att_actions(FILE *inFile, ABObj obj, ABObj interface);

/* gil_loadobj */
int	gilP_load_object(FILE *inFile, ABObj parent);
int	gilP_load_object2(FILE *inFile, ABObj obj, ABObj parent);

#endif /* _ABIL_GIL_P_H_ */
