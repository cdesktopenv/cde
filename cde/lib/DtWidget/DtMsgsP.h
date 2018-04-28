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
/* $XConsortium: DtMsgsP.h /main/7 1996/06/19 11:27:42 cde-dec $ */
/*
 * DtWidget/DtMsgsP.h
 */
/*
 * DtMsgsP.h:
 *
 * Private header file for libDtWidget
 */
#ifndef _DtMessages_h
#define _DtMessages_h

/*** const causes the HP compiler to complain.  Remove reference until   ***
 *** the build and integration people can figure out how to get this to  ***
 *** work.                                                               ***/

/* #define CONST	const */
#define CONST

#ifdef I18N_MSG
#include <nl_types.h>
#define DTWIDGET_GETMESSAGE(set, number, string)\
    (char *) _DtWidgetGetMessage(set, number, string)

#else /* I18N_MSG */
#define DTWIDGET_GETMESSAGE(set, number, string) string
#endif /* I18N_MSG */

extern CONST char _DtMsgCommon_0000[];
extern CONST char _DtMsgCommon_0001[];
extern CONST char _DtMsgComboBox_0000[];
extern CONST char _DtMsgComboBox_0001[];
extern CONST char _DtMsgComboBox_0002[];
extern CONST char _DtMsgComboBox_0003[];
extern CONST char _DtMsgComboBox_0004[];
extern CONST char _DtMsgComboBox_0005[];
extern CONST char _DtMsgComboBox_0006[];
extern CONST char _DtMsgComboBox_0007[];
extern CONST char _DtMsgComboBox_0008[];
extern CONST char _DtMsgComboBox_0009[];
extern CONST char _DtMsgComboBox_0010[];
extern CONST char _DtMsgComboBox_0011[];
extern CONST char _DtMsgComboBox_0012[];
extern CONST char _DtMsgComboBox_0013[];
extern CONST char _DtMsgComboBox_0014[];
extern CONST char _DtMsgDialogBox_0000[];
extern CONST char _DtMsgDialogBox_0001[];
extern CONST char _DtMsgEditor_0000[];
extern CONST char _DtMsgEditor_0001[];
extern CONST char _DtMsgEditor_0002[];
extern CONST char _DtMsgEditor_0003[];
extern CONST char _DtMsgEditor_0004[];
extern CONST char _DtMsgEditor_0005[];
extern CONST char _DtMsgEditor_0006[];
extern CONST char _DtMsgEditor_0007[];
extern CONST char _DtMsgEditor_0008[];
extern CONST char _DtMsgEditor_0009[];
extern CONST char _DtMsgEditor_0010[];
extern CONST char _DtMsgEditor_0011[];
extern CONST char _DtMsgEditor_0012[];
extern CONST char _DtMsgEditor_0013[];
extern CONST char _DtMsgEditor_0014[];
extern CONST char _DtMsgEditor_0015[];
extern CONST char _DtMsgEditor_0016[];
extern CONST char _DtMsgEditor_0017[];
extern CONST char _DtMsgEditor_0018[];
extern CONST char _DtMsgEditor_0019[];
extern CONST char _DtMsgEditor_0020[];
extern CONST char _DtMsgEditor_0021[];
extern CONST char _DtMsgEditor_0022[];
extern CONST char _DtMsgEditor_0023[];
extern CONST char _DtMsgEditor_0024[];
extern CONST char _DtMsgEditor_0025[];
extern CONST char _DtMsgIcon_0000[];
extern CONST char _DtMsgIcon_0001[];
extern CONST char _DtMsgIcon_0002[];
extern CONST char _DtMsgIcon_0003[];
extern CONST char _DtMsgIcon_0004[];
extern CONST char _DtMsgIcon_0005[];
extern CONST char _DtMsgIndicator_0000[];
extern CONST char _DtMsgMenuButton_0000[];
extern CONST char _DtMsgMenuButton_0001[];
extern CONST char _DtMsgMenuButton_0002[];
extern CONST char _DtMsgSpinBox_0000[];
extern CONST char _DtMsgSpinBox_0001[];
extern CONST char _DtMsgSpinBox_0002[];
extern CONST char _DtMsgSpinBox_0003[];
extern CONST char _DtMsgSpinBox_0004[];
extern CONST char _DtMsgSpinBox_0005[];
extern CONST char _DtMsgSpinBox_0006[];
extern CONST char _DtMsgSpinBox_0007[];
extern CONST char _DtMsgSpinBox_0008[];
extern CONST char _DtMsgSpinBox_0009[];
extern CONST char _DtMsgSpinBox_0010[];
extern CONST char _DtMsgSpinBox_0011[];
extern CONST char _DtMsgSpinBox_0012[];
extern CONST char _DtMsgSpinBox_0013[];
extern CONST char _DtMsgSpinBox_0014[];
extern CONST char _DtMsgTitleBox_0000[];
extern CONST char _DtMsgTitleBox_0001[];
extern CONST char _DtMsgTitleBox_0002[];
extern CONST char _DtMsgTitleBox_0003[];
extern CONST char _DtMsgTitleBox_0004[];

#ifdef I18N_MSG

/* 
 * Message set for shared messages 
 */
#define	MS_Common		2

/* Message IDs for shared messages */
#define	MSG_CLOSE		1
#define	MSG_HELP		2

/* 
 * Message set for Editor widget
 */
#define	MS_Editor		3

/* Message IDs for Editor */
#define	EDITOR_FORMAT_SETTINGS	1
#define	EDITOR_RIGHT_MARGIN	2
#define	EDITOR_LEFT_MARGIN	3
#define	EDITOR_LEFT_ALIGN	4
#define	EDITOR_RIGHT_ALIGN	5
#define	EDITOR_JUSTIFY		6
#define	EDITOR_CENTER		7
#define	EDITOR_PARAGRAPH	8
#define	EDITOR_ALL		9
#define	EDITOR_SPELL_TITLE	20
#define	EDITOR_FIND_TITLE	21
#define	EDITOR_MISSPELLED	22
#define	EDITOR_FIND_LABEL	23
#define	EDITOR_CHANGE_LABEL	24
#define	EDITOR_FIND_BUTTON	25
#define	EDITOR_CHANGE_BUTTON	26
#define	EDITOR_CHNG_ALL_BUTTON	27
#define	EDITOR_NO_FIND		30
#define	EDITOR_INFO_TITLE	31
#define	EDITOR_LINE		40
#define	EDITOR_TOTAL		41
#define	EDITOR_OVR		42
#define	EDITOR_INS		43
#define EDITOR_FILTER_ERROR     50 
#define EDITOR_FILTER_ERROR2    51 
#define EDITOR_ERROR_TITLE      52 

/* 
 * Message set for MenuButton widget
 */
#define	MS_MenuButton		4

/* Message IDs for MenuButton*/
#define	MENU_POST		1
#define	MENU_PARENT		2
#define	MENU_SUBMENU		3

/* 
 * Message set for ComboBox widget
 */
#define	MS_ComboBox 		5

/* Message IDs for ComboBox */
#define	COMBO_ALIGNMENT		1 
#define	COMBO_MARGIN_HEIGHT 	2
#define	COMBO_MARGIN_WIDTH	3 
#define	COMBO_HORIZONTAL_SPACING	4
#define	COMBO_VERTICAL_SPACING 	5
#define	COMBO_ORIENTATION 	6
#define	COMBO_ITEM_COUNT 	7
#define	COMBO_VISIBLE_ITEM	8 
#define	COMBO_TEXT 		9
#define	COMBO_SET_ITEM		10
#define	COMBO_SELECT_ITEM 	11
#define	COMBO_RESIZE		12
#define	COMBO_LABEL		13
#define	COMBO_CVTSTRING		14
#define	COMBO_DEL_POS		15

/* 
 * Message set for SpinBox widget
 */
#define	MS_SpinBox 		6

/* Message IDs for SpinBox */
#define	SPIN_ARROW_SENSITIVE	1
#define	SPIN_ALIGNMENT		2 
#define	SPIN_INIT_DELAY		3
#define SPIN_MARGIN_HEIGHT	4
#define SPIN_MARGIN_WIDTH	5
#define SPIN_ARROW_LAYOUT	6
#define SPIN_REPEAT_DELAY	7
#define SPIN_ITEM_COUNT		8
#define SPIN_POSITION_STRING	9
#define SPIN_POSITION_NUMERIC	10
#define SPIN_DECIMAL_POINTS	11
#define SPIN_MIN_MAX		12
#define SPIN_TEXT		13
#define SPIN_SET_ITEM		14	
#define SPIN_LABEL		15	

extern char *	_DtWidgetGetMessage(
			int set,
			int n,
			char *s );

#endif /* I18N_MSG */

#endif /* _DtMessageh */
