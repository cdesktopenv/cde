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
/* $XConsortium: EditorP.h /main/4 1996/06/19 11:28:20 cde-dec $ */
/**********************************<+>*************************************
***************************************************************************
**
**  File:        EditorP.h
**
**  Project:     DtEditor widget 
**
**  Description:
**  -----------
**  Private include file for the DtEditor widget, text editor class.
**
*******************************************************************
*
* (c) Copyright 1993, 1994 Hewlett-Packard Company
* (c) Copyright 1993, 1994 International Business Machines Corp.
* (c) Copyright 1993, 1994 Sun Microsystems, Inc.
* (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of Novell, Inc.
*
********************************************************************
**
**
**************************************************************************
**********************************<+>*************************************/
#ifndef _DtEditorP_h
#define _DtEditorP_h

#include <Dt/Editor.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>

#include <Xm/Protocols.h>
#include <Xm/XmP.h>
#include <Xm/FormP.h>
#include <Xm/TextP.h>

#include "MacrosP.h"
#include <Dt/Dts.h>

#ifdef SENTINEL
#include <sentinel.h>
#endif  /* SENTINEL */


/****************************************************************
 *
 *  Defines
 *
 ****************************************************************/

#define READ_ACCESS     1
#define WRITE_ACCESS    2

#define REPLACE         0
#define FIND            1
#define SPELL           2

#define UNKNOWN_ACTION          -1
#define EXTEND_SELECTION        0
#define ICON_FOCUSED            1

#define NO_DELETION_IN_PROGRESS	-999 

#ifndef DtUNSPECIFIED
#define DtUNSPECIFIED          (~0)
#endif


#define CLOSE_BUTTON	DTWIDGET_GETMESSAGE( \
                          MS_Common, MSG_CLOSE, _DtMsgCommon_0000)

#define HELP_BUTTON	DTWIDGET_GETMESSAGE( \
                          MS_Common, MSG_HELP, _DtMsgCommon_0001)

#define FORMAT_SETTINGS	DTWIDGET_GETMESSAGE( \
                          MS_Editor, EDITOR_FORMAT_SETTINGS, _DtMsgEditor_0000)

#define RIGHT_MARGIN	DTWIDGET_GETMESSAGE( \
                          MS_Editor, EDITOR_RIGHT_MARGIN, _DtMsgEditor_0001)

#define LEFT_MARGIN	DTWIDGET_GETMESSAGE( \
                          MS_Editor, EDITOR_LEFT_MARGIN, _DtMsgEditor_0002)

#define LEFT_ALIGN	DTWIDGET_GETMESSAGE( \
                          MS_Editor, EDITOR_LEFT_ALIGN, _DtMsgEditor_0003)

#define RIGHT_ALIGN	DTWIDGET_GETMESSAGE( \
                          MS_Editor, EDITOR_RIGHT_ALIGN, _DtMsgEditor_0004)

#define JUSTIFY		DTWIDGET_GETMESSAGE( \
                          MS_Editor, EDITOR_JUSTIFY, _DtMsgEditor_0005)

#define CENTER		DTWIDGET_GETMESSAGE( \
                          MS_Editor, EDITOR_CENTER, _DtMsgEditor_0006)

#define PARAGRAPH	DTWIDGET_GETMESSAGE( \
                          MS_Editor, EDITOR_PARAGRAPH, _DtMsgEditor_0007)

#define ALL		DTWIDGET_GETMESSAGE( \
                          MS_Editor, EDITOR_ALL, _DtMsgEditor_0008)

#define SPELL_TITLE	DTWIDGET_GETMESSAGE( \
			  MS_Editor, EDITOR_SPELL_TITLE, _DtMsgEditor_0009)

#define FIND_TITLE	DTWIDGET_GETMESSAGE( \
			  MS_Editor, EDITOR_FIND_TITLE, _DtMsgEditor_0010)

#define MISSPELLED	DTWIDGET_GETMESSAGE( \
                          MS_Editor, EDITOR_MISSPELLED, _DtMsgEditor_0011)

#define FIND_LABEL	DTWIDGET_GETMESSAGE( \
                          MS_Editor, EDITOR_FIND_LABEL, _DtMsgEditor_0012)

#define CHANGE_LABEL	DTWIDGET_GETMESSAGE( \
                          MS_Editor, EDITOR_CHANGE_LABEL, _DtMsgEditor_0013)

#define FIND_BUTTON	DTWIDGET_GETMESSAGE( \
                          MS_Editor, EDITOR_FIND_BUTTON, _DtMsgEditor_0014)

#define CHANGE_BUTTON	DTWIDGET_GETMESSAGE( \
                          MS_Editor, EDITOR_CHANGE_BUTTON, _DtMsgEditor_0015)

#define CHNG_ALL_BUTTON   DTWIDGET_GETMESSAGE( \
                          MS_Editor, EDITOR_CHNG_ALL_BUTTON, _DtMsgEditor_0016)

#define NO_FIND		DTWIDGET_GETMESSAGE( \
			  MS_Editor, EDITOR_NO_FIND, _DtMsgEditor_0017)

#define INFO_TITLE	DTWIDGET_GETMESSAGE( \
                          MS_Editor, EDITOR_INFO_TITLE, _DtMsgEditor_0018)

#define LINE		DTWIDGET_GETMESSAGE( \
                          MS_Editor, EDITOR_LINE, _DtMsgEditor_0019)

#define TOTAL		DTWIDGET_GETMESSAGE( \
                          MS_Editor, EDITOR_TOTAL, _DtMsgEditor_0020)

#define OVR		DTWIDGET_GETMESSAGE( \
                          MS_Editor, EDITOR_OVR, _DtMsgEditor_0021)

#define INS		DTWIDGET_GETMESSAGE( \
                          MS_Editor, EDITOR_INS, _DtMsgEditor_0022)

#define BAD_FILTER      DTWIDGET_GETMESSAGE( \
                          MS_Editor, EDITOR_FILTER_ERROR, _DtMsgEditor_0023)

#define BAD_FILTER2     DTWIDGET_GETMESSAGE( \
                          MS_Editor, EDITOR_FILTER_ERROR2, _DtMsgEditor_0024)

#define ERROR_TITLE     DTWIDGET_GETMESSAGE( \
                          MS_Editor, EDITOR_ERROR_TITLE, _DtMsgEditor_0025)


/****************************************************************
 *
 *  Structures
 *
 ****************************************************************/

typedef struct _AdjRec {
        int  bflag;     /* -b (blanks) option */
        int  cflag;     /* -c (center) option */
        int  jflag;     /* -j (justify) option  */
        int  rflag;     /* -r (right justify) */
        int  tabsize;   /* tab size in use  */
        long margin;
        FILE *infp;     /* file pointer for unformatted data */
        FILE *outfp;    /* file pointer for formatted data */
} AdjRec, *AdjRecPtr;


/****************************************************************
 *
 *  Deleted text structure definition
 *
 ****************************************************************/
typedef struct _DtEditorUndoRec {
    char                *deletedText;
    XmTextPosition      deletionStart;
    XmTextPosition      insertStart;
    int                 insertionLength;
} DtEditorUndoRec;

/****************************************************************
 *
 *  Data structures for miscellaneous edit information
 *
 ****************************************************************/
typedef struct _DtEditorEditStuff
{
    int			blinkRate,
			maxLength;
    short		columns, rows;
    XmTextPosition	cursorPos,
			topCharacter;
    Boolean		autoShowCursorPos,
    			cursorPosVisible,
    			editable,
    			overstrikeMode,
    			scrollHorizontal,
    			scrollLeft,
    			scrollTop,
    			scrollVertical,
    			wordWrap;
    Pixel		background,
			foreground;
    int			fontWidth,  /* widest font, used for formatting */
			fontHeight; /* font height; used for WM size hints */

    DtEditorUndoRec	undo;      /* holds deleted text for undo    */

    Boolean             loadingAllNewData,
                        unreadChanges; /* Data changed since read by app?   */

    SetSelectionProc    setSelection;
} DtEditorEditStuff;

/****************************************************************
 *
 *  Data structures for Search
 *
 ****************************************************************/
typedef struct _DtEditorSearchWidgets {
    Widget formDlg,
           listLbl,
           spellList,
           findLbl,
           findText,
           replaceLbl,
           replaceText,
           separator,
           findBtn,
           replaceBtn,
           replaceAllBtn,
           closeBtn,
           helpBtn;
} DtEditorSearchWidgets;

typedef struct _DtEditorSearchStuff {
    DtEditorSearchWidgets widgets;
    int search_dialog_mode;
    char *search_string,	/* last string searched for */
         *replace_string,	/* last replacement string */
         *misspelled_string,	/* last misspelled word selected */
	 *spellFilter;
    Boolean	misspelled_found;
    Dimension	buttonHeight;
    XmString spellTitle,
             fndChngTitle,
	     changeAllButtonLabel,
	     changeButtonLabel,
	     changeFieldLabel,
	     findButtonLabel,
	     findFieldLabel,
	     misspelledListLabel;
} DtEditorSearchStuff;

/****************************************************************
 *
 *  Data structures for Format
 *
 ****************************************************************/
typedef struct _DtEditorFormatWidgets {
    Widget format_dialog,
           right_label,
           left_label,
           rightMargField,           /* format right margin text field */
           leftMargField,	    /* format left margin text field */
           radio_box,
           left_just,               /* format type toggle button */
           right_just,              /* format type toggle button */
           both_just,               /* format type toggle button */
           center,                  /* format type toggle button */
           separator,
           paragraph,
           all,
           close,
           help;
} DtEditorFormatWidgets;

typedef struct _DtEditorFormatStuff {
    DtEditorFormatWidgets widgets;
    XmString	centerToggleLabel,
		formatAllButtonLabel,
		formatParaButtonLabel,
		formatDialogTitle,
		justifyToggleLabel,
		leftAlignToggleLabel,
		leftMarginFieldLabel,
		rightAlignToggleLabel,
		rightMarginFieldLabel;
} DtEditorFormatStuff;
 

/****************************************************************
 *
 *  Data structures for the Status Line
 *
 ****************************************************************/

typedef struct _status {
    Boolean	showStatusLine;
    int		currentLine,
    		lastLine;
    XmString	ovr,
		ins,
		currentLineLabel,
		totalLineLabel;
    Widget	statusArea,
          	lineLabel,
          	lineText,
          	totalLabel,
          	totalText,
		overstrikeLabel,
		messageText;
} DtEditorStatusStuff;

/****************************************************************
 *
 *  Data structures for holding warning dialogs
 *
 ****************************************************************/

typedef struct _textlibwarning {
    Widget 	warningDialog;
    XmString	infoDialogTitle;
} DtEditorWarningStuff;


/****************************************************************
 * 
 *  Class Part structure definition  
 * 
 ****************************************************************/
typedef struct _DtEditorClassPart 
{
	int	ignore;
} DtEditorClassPart;


/****************************************************************
 * 
 *  Full Class Record declaration
 * 
 ****************************************************************/
typedef struct _DtEditorClassRec 
{
        CoreClassPart                   core_class;
        CompositeClassPart              composite_class;
        ConstraintClassPart             constraint_class;
        XmManagerClassPart              manager_class;
	XmBulletinBoardClassPart	bulletin_board_class;
        XmFormClassPart		        form_class;
        DtEditorClassPart  		editor_class;
} DtEditorClassRec;

externalref	DtEditorClassRec	dtEditorClassRec;


/****************************************************************
 * 
 *  Resources added by the Editor widget
 * 
 ****************************************************************/
typedef struct _DtEditorPart
{
    Display                     *display;
    Widget                      topLevelShell,
                                /*
                                 * scrolled text widget
                                 */
    				text;
    Dimension			width;  /* the text widget width */

                                /* All data for Edit functions */
    DtEditorEditStuff		editStuff;
                                /* All data for Search functions */
    DtEditorSearchStuff		searchStuff;
                                /* All data for Format functions */
    DtEditorFormatStuff		formatStuff;
                                /* All data for warning dialogs functions */
    DtEditorWarningStuff	warningStuff;
                                /* All data for the status line */
    DtEditorStatusStuff		statusStuff;

    XtAppContext                app_context;

                                /*
                                 * The editor widget's callbacks
                                 */

    Boolean			textSelectCbCalled;
    XtCallbackList              textSelect;
    XtCallbackList              textDeselect;

} DtEditorPart; 


/****************************************************************
 * 
 *  Full Instance Record declaration
 * 
 ****************************************************************/
typedef struct _DtEditorRec
{
        CorePart		core;
        CompositePart		composite;
        ConstraintPart		constraint;
        XmManagerPart		manager;
	XmBulletinBoardPart	bulletin_board;
        XmFormPart		form; 
	DtEditorPart		editor;
} DtEditorRec; 


/****************************************************************
 *
 * MACRO DEFINITIONS
 *
 ****************************************************************/

/*
 *  Class and Instance Macros
 */

/* 
 * Note: DtEditor utilizes the bulletin board's dialog_title and 
 * text_translations fields to hold its dialogTitle and textTranslations
 * resources
 */
#define E_dialogTitle(m)	(m -> bulletin_board.dialog_title)
#define E_textTranslations(m)	(m -> bulletin_board.text_translations)
#define E_buttonFontList(m)	(m -> bulletin_board.button_font_list)
#define E_labelFontList(m)	(m -> bulletin_board.label_font_list)
#define E_textFontList(m)	(m -> bulletin_board.text_font_list)

#define M_app_context(m)	(m -> editor.app_context)
#define M_display(m)		(m -> editor.display)
#define M_topLevelShell(m)	(m -> editor.topLevelShell)
#define M_text(m)		(m -> editor.text)
#define M_textWidth(m)		(m -> editor.width)

/* callbacks */
#define M_textSelect(m)		(m -> editor.textSelect)
#define M_textDeselect(m)	(m -> editor.textDeselect)
#define M_textSelectCbCalled(m)	(m -> editor.textSelectCbCalled)

/* editStuff - scrolled text resources */
#define M_autoShowCursorPos(m)	(m->editor.editStuff.autoShowCursorPos)
#define M_blinkRate(m) 		(m -> editor.editStuff.blinkRate)
#define M_columns(m) 		(m -> editor.editStuff.columns)
#define M_cursorPos(m)		(m -> editor.editStuff.cursorPos)
#define M_cursorPosVisible(m)	(m -> editor.editStuff.cursorPosVisible)
#define M_editable(m)		(m -> editor.editStuff.editable)
#define M_fontWidth(m) 		(m -> editor.editStuff.fontWidth)
#define M_fontHeight(m)		(m -> editor.editStuff.fontHeight)
#define M_maxLength(m) 		(m -> editor.editStuff.maxLength)
#define M_overstrikeMode(m)	(m -> editor.editStuff.overstrikeMode)
#define M_rows(m) 		(m -> editor.editStuff.rows)
#define M_scrollHorizontal(m)	(m -> editor.editStuff.scrollHorizontal)
#define M_scrollLeftSide(m)	(m -> editor.editStuff.scrollLeft)
#define M_scrollTopSide(m)	(m -> editor.editStuff.scrollTop)
#define M_scrollVertical(m)	(m -> editor.editStuff.scrollVertical)
#define M_topCharacter(m)	(m -> editor.editStuff.topCharacter)
#define M_wordWrap(m)		(m -> editor.editStuff.wordWrap)
#define M_textBackground(m)	(m -> editor.editStuff.background)
#define M_textForeground(m)	(m -> editor.editStuff.foreground)

/* editStuff - scrolled text related data */
#define M_deletionStart(m)	(m -> editor.editStuff.undo.deletionStart)
#define M_deletedText(m)	(m -> editor.editStuff.undo.deletedText)
#define M_insertStart(m)	(m -> editor.editStuff.undo.insertStart)
#define M_insertionLength(m)	(m -> editor.editStuff.undo.insertionLength)
#define M_loadingAllNewData(m)	(m -> editor.editStuff.loadingAllNewData)
#define M_unreadChanges(m)	(m -> editor.editStuff.unreadChanges)
#define M_setSelection(m)	(m -> editor.editStuff.setSelection)

/* searchStuff */
#define M_search_dialogMode(m)  (m ->editor.searchStuff.search_dialog_mode)
#define M_spellTitle(m) 	(m -> editor.searchStuff.spellTitle)
#define M_fndChngTitle(m) 	(m -> editor.searchStuff.fndChngTitle)
#define E_changeAllButtonLabel(m) (m->editor.searchStuff.changeAllButtonLabel)
#define E_changeButtonLabel(m)	(m->editor.searchStuff.changeButtonLabel)
#define E_changeFieldLabel(m)	(m->editor.searchStuff.changeFieldLabel)
#define E_findButtonLabel(m)	(m->editor.searchStuff.findButtonLabel)
#define E_findFieldLabel(m)	(m->editor.searchStuff.findFieldLabel)
#define E_misspelledListLabel(m) (m->editor.searchStuff.misspelledListLabel)
#define M_search_string(m) 	(m -> editor.searchStuff.search_string)
#define M_replace_string(m) 	(m -> editor.searchStuff.replace_string)
#define M_misspelled_string(m) 	(m -> editor.searchStuff.misspelled_string)
#define M_misspelled_found(m) 	(m -> editor.searchStuff.misspelled_found)
#define M_spellFilter(m) 	(m -> editor.searchStuff.spellFilter)
#define M_replaceText(m) 	(m -> editor.searchStuff.widgets.replaceText)
#define M_findText(m) 		(m -> editor.searchStuff.widgets.findText)
#define M_search_replaceLbl(m)	(m -> editor.searchStuff.widgets.replaceLbl)
#define M_search_buttonHeight(m) (m -> editor.searchStuff.buttonHeight)
#define M_search_dialog(m) 	(m -> editor.searchStuff.widgets.formDlg)
#define M_search_closeBtn(m) 	(m -> editor.searchStuff.widgets.closeBtn)
#define M_search_helpBtn(m) 	(m -> editor.searchStuff.widgets.helpBtn)
#define M_search_findBtn(m)	(m -> editor.searchStuff.widgets.findBtn)
#define M_search_replaceBtn(m) 	(m -> editor.searchStuff.widgets.replaceBtn)
#define M_search_replaceAllBtn(m)\
				(m -> editor.searchStuff.widgets.replaceAllBtn)
#define M_search_spellList(m) 	(m -> editor.searchStuff.widgets.spellList)
#define M_search_listLbl(m) 	(m -> editor.searchStuff.widgets.listLbl)
#define M_search_findLbl(m) 	(m -> editor.searchStuff.widgets.findLbl)
#define M_search_separator(m) 	(m -> editor.searchStuff.widgets.separator)

/* formatStuff */
#define E_format_centerToggleLabel(m) (m->editor.formatStuff.centerToggleLabel)
#define E_format_formatAllButtonLabel(m)\
			(m->editor.formatStuff.formatAllButtonLabel)
#define E_format_formatParagraphButtonLabel(m)\
			(m->editor.formatStuff.formatParaButtonLabel)
#define E_format_dialogTitle(m)	(m->editor.formatStuff.formatDialogTitle)
#define E_format_justifyToggleLabel(m)\
				(m->editor.formatStuff.justifyToggleLabel)
#define E_format_leftAlignToggleLabel(m)\
				(m->editor.formatStuff.leftAlignToggleLabel)
#define E_format_leftMarginFieldLabel(m)\
				(m->editor.formatStuff.leftMarginFieldLabel)
#define E_format_rightAlignToggleLabel(m)\
				(m->editor.formatStuff.rightAlignToggleLabel)
#define E_format_rightMarginFieldLabel(m)\
				(m->editor.formatStuff.rightMarginFieldLabel)
#define M_format_all(m)		(m -> editor.formatStuff.widgets.all)
#define M_format_bothJust(m)	(m -> editor.formatStuff.widgets.both_just)
#define M_format_center(m)	(m -> editor.formatStuff.widgets.center)
#define M_format_close(m)	(m -> editor.formatStuff.widgets.close)
#define M_format_dialog(m)	(m -> editor.formatStuff.widgets.format_dialog)
#define M_format_help(m)	(m -> editor.formatStuff.widgets.help)
#define M_format_leftJust(m)	(m -> editor.formatStuff.widgets.left_just)
#define M_format_leftLabel(m)	(m -> editor.formatStuff.widgets.left_label)
#define M_format_leftMarginField(m)  (m -> editor.formatStuff.widgets.leftMargField)
#define M_format_paragraph(m)	(m -> editor.formatStuff.widgets.paragraph)
#define M_format_radioBox(m)	(m -> editor.formatStuff.widgets.radio_box)
#define M_format_rightJust(m)	(m -> editor.formatStuff.widgets.right_just)
#define M_format_rightLabel(m)	(m -> editor.formatStuff.widgets.right_label)
#define M_format_rightMarginField(m) (m -> editor.formatStuff.widgets.rightMargField)
#define M_format_separator(m)	(m -> editor.formatStuff.widgets.separator)

/* statusStuff */
#define M_status_showStatusLine(m)  (m -> editor.statusStuff.showStatusLine) 
#define M_status_currentLine(m)	    (m -> editor.statusStuff.currentLine) 
#define M_status_lastLine(m)	    (m -> editor.statusStuff.lastLine) 
#define M_status_overstrikeLabel(m) (m -> editor.statusStuff.ovr)
#define M_status_insertLabel(m)	    (m -> editor.statusStuff.ins)
#define E_status_currentLineLabel(m) (m -> editor.statusStuff.currentLineLabel)
#define E_status_totalLineCountLabel(m) (m->editor.statusStuff.totalLineLabel)
#define M_status_statusArea(m)	    (m -> editor.statusStuff.statusArea)
#define M_status_lineLabel(m)	    (m -> editor.statusStuff.lineLabel)
#define M_status_lineText(m)	    (m -> editor.statusStuff.lineText)
#define M_status_totalLabel(m)	    (m -> editor.statusStuff.totalLabel)
#define M_status_totalText(m)	    (m -> editor.statusStuff.totalText)
#define M_status_overstrikeWidget(m) (m->editor.statusStuff.overstrikeLabel)
#define M_status_messageText(m)    (m -> editor.statusStuff.messageText)

/* warningStuff */
#define M_gen_warning(m)	(m ->editor.warningStuff.warningDialog)
#define E_infoDialogTitle(m)	(m ->editor.warningStuff.infoDialogTitle)

/* 
 * Misc macros
 */ 
#define M_editor(m)	((DtEditorWidget) m ->core.parent -> core.parent) 


/****************************************************************
 *
 *  Private library functions
 *
 ****************************************************************/

/*** library-private functions in Editor.c ***/
extern void	_DtEditorResetUndo(
			DtEditorWidget editor);
extern void	_DtEditorUpdateLineDisplay(
			DtEditorWidget	editor,
			int		currentLine,
			Boolean		forceUpdate );
extern int	_DtEditorGetLineIndex(
			XmTextWidget tw,
			XmTextPosition pos);
extern void 	_DtEditorWarning(
			DtEditorWidget pPriv,
			char *mess,
			unsigned char dialogType);
extern DtEditorErrorCode 	_DtEditorValidateFileAccess(
			char 			*fileName,
			int     		accessType );
extern void	_DtEditorHelpSearchCB (
			Widget	w,
			caddr_t client_data ,
			caddr_t call_data );
extern void	_DtEditorHelpSearchSpellCB (
			Widget	w,
			caddr_t client_data ,
			caddr_t call_data );
extern void	_DtEditorHelpSearchFindCB (
			Widget	w,
			caddr_t client_data ,
			caddr_t call_data );
extern void	_DtEditorHelpSearchChangeCB (
			Widget	w,
			caddr_t client_data ,
			caddr_t call_data );

/*** library-private functions in EditAreaData.c ***/

extern char *_DtEditorGetPointer(
        char *pString,
        int startChar);

/*** library-private functions in EditorCalls.c ***/

extern void _DtEditorModifyVerifyCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data );

/*** library-private functions in SearchCalls.c ***/

extern void _DtEditorSearchMapCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data );
extern int _DtEditor_CountCharacters(
        char *str,
        int num_count_bytes);
extern void _DtEditorDialogSearchCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data );
extern void _DtEditorDialogReplaceCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data );
extern void _DtEditorDialogReplaceAllCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data );
extern void _DtEditorDialogFindCancelCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data );
extern void _DtEditorMisspelledSelectCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data );
extern void _DtEditorMisspelledDblClickCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data );
extern void _DtEditorFindTextChangedCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data );
extern void _DtEditorReplaceTextChangedCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data );
extern void _DtEditorSetFindSensitivity(
        DtEditorWidget pPriv,
	Boolean sensitivity);
extern void _DtEditorSetReplaceSensitivity(
        DtEditorWidget pPriv,
	Boolean sensitivity);
extern void _DtEditorSetReplaceAllSensitivity(
        DtEditorWidget pPriv,
	Boolean sensitivity);


/*** library-private functions in SearchDlg.c ***/
extern void _DtEditorSearch(
	DtEditorWidget pPriv,
	Boolean spell,
	Boolean createonly );

#endif /* _DtEditorP_h */

