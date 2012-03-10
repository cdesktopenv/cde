/* $XConsortium: Editor.h /main/3 1995/10/26 09:32:07 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 */

#ifndef _Dt_Editor_h
#define _Dt_Editor_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Constants
 */

/* Sources of Help requests */
/* Used with XmNhelpCallback */

#define DtEDITOR_HELP_EDIT_WINDOW		1
#define DtEDITOR_HELP_STATUS_LINE		2
#define DtEDITOR_HELP_STATUS_CURRENT_LINE	3
#define DtEDITOR_HELP_STATUS_TOTAL_LINES	4
#define DtEDITOR_HELP_STATUS_MESSAGE		5
#define DtEDITOR_HELP_STATUS_OVERSTRIKE		6
#define DtEDITOR_HELP_FORMAT_DIALOG		7
#define DtEDITOR_HELP_FORMAT_LEFT_MARGIN	8
#define DtEDITOR_HELP_FORMAT_RIGHT_MARGIN	9
#define DtEDITOR_HELP_FORMAT_ALIGNMENT		10
#define DtEDITOR_HELP_CHANGE_DIALOG		11
#define DtEDITOR_HELP_CHANGE_FIND		12
#define DtEDITOR_HELP_CHANGE_CHANGE		13
#define DtEDITOR_HELP_SPELL_DIALOG		14
#define DtEDITOR_HELP_SPELL_MISSPELLED_WORDS	15
#define DtEDITOR_HELP_SPELL_CHANGE		16

/* Specifies the last line in the edit area */
/* Used with DtEditorGoToLine */
#define DtEDITOR_LAST_LINE 		-999

/* Resource constants */

#ifndef DtNautoShowCursorPosition
#define DtNautoShowCursorPosition	XmNautoShowCursorPosition
#endif
#ifndef DtCAutoShowCursorPosition
#define DtCAutoShowCursorPosition	XmCAutoShowCursorPosition
#endif
#ifndef DtNblinkRate
#define DtNblinkRate            	XmNblinkRate
#endif
#ifndef DtCBlinkRate
#define DtCBlinkRate            	XmCBlinkRate
#endif
#ifndef DtNbuttonFontList  
#define DtNbuttonFontList             	XmNbuttonFontList
#endif
#ifndef DtCFontList 
#define DtCFontList             	XmCFontList
#endif
#ifndef DtNcenterToggleLabel
#define DtNcenterToggleLabel       	"centerToggleLabel"
#endif
#ifndef DtCCenterToggleLabel
#define DtCCenterToggleLabel       	"CenterToggleLabel"
#endif
#ifndef DtNchangeAllButtonLabel
#define DtNchangeAllButtonLabel       	"changeAllButtonLabel"
#endif
#ifndef DtCChangeAllButtonLabel
#define DtCChangeAllButtonLabel       	"ChangeAllButtonLabel"
#endif
#ifndef DtNchangeButtonLabel
#define DtNchangeButtonLabel       	"changeButtonLabel"
#endif
#ifndef DtCChangeButtonLabel
#define DtCChangeButtonLabel       	"ChangeButtonLabel"
#endif
#ifndef DtNchangeFieldLabel
#define DtNchangeFieldLabel       	"changeFieldLabel"
#endif
#ifndef DtCChangeFieldLabel
#define DtCChangeFieldLabel       	"ChangeFieldLabel"
#endif
#ifndef DtNcolumns
#define DtNcolumns		        XmNcolumns
#endif
#ifndef DtCColumns
#define DtCColumns		        XmCColumns
#endif
#ifndef DtNcurrentLineLabel
#define DtNcurrentLineLabel       	"currentLineLabel"
#endif
#ifndef DtCCurrentLineLabel
#define DtCCurrentLineLabel       	"CurrentLineLabel"
#endif
#ifndef DtNcursorPosition
#define DtNcursorPosition       	XmNcursorPosition
#endif
#ifndef DtCCursorPosition
#define DtCCursorPosition       	XmCCursorPosition
#endif
#ifndef DtNcursorPositionVisible
#define DtNcursorPositionVisible      	XmNcursorPositionVisible
#endif
#ifndef DtCCursorPositionVisible
#define DtCCursorPositionVisible       	XmCCursorPositionVisible
#endif
#ifndef DtNdialogTitle
#define DtNdialogTitle          	"dialogTitle"
#endif
#ifndef DtCDialogTitle
#define DtCDialogTitle          	XmCDialogTitle
#endif
#ifndef DtNeditable  
#define DtNeditable             	XmNeditable
#endif
#ifndef DtCEditable 
#define DtCEditable             	XmCEditable
#endif
#ifndef DtNfindButtonLabel
#define DtNfindButtonLabel       	"findButtonLabel"
#endif
#ifndef DtCFindButtonLabel
#define DtCFindButtonLabel       	"FindButtonLabel"
#endif
#ifndef DtNfindChangeDialogTitle
#define DtNfindChangeDialogTitle       	"findChangeDialogTitle"
#endif
#ifndef DtCFindChangeDialogTitle
#define DtCFindChangeDialogTitle       	"FindChangeDialogTitle"
#endif
#ifndef DtNfindFieldLabel
#define DtNfindFieldLabel       	"findFieldLabel"
#endif
#ifndef DtCFindFieldLabel
#define DtCFindFieldLabel       	"FindFieldLabel"
#endif
#ifndef DtNformatAllButtonLabel
#define DtNformatAllButtonLabel       	"formatAllButtonLabel"
#endif
#ifndef DtCFormatAllButtonLabel
#define DtCFormatAllButtonLabel       	"FormatAllButtonLabel"
#endif
#ifndef DtNformatParagraphButtonLabel
#define DtNformatParagraphButtonLabel	"formatParagraphButtonLabel"
#endif
#ifndef DtCFormatParagraphButtonLabel
#define DtCFormatParagraphButtonLabel	"FormatParagraphButtonLabel"
#endif
#ifndef DtNformatSettingsDialogTitle
#define DtNformatSettingsDialogTitle	"formatSettingsDialogTitle"
#endif
#ifndef DtCFormatSettingsDialogTitle
#define DtCFormatSettingsDialogTitle	"FormatSettingsDialogTitle"
#endif
#ifndef DtNinformationDialogTitle
#define DtNinformationDialogTitle	"informationDialogTitle"
#endif
#ifndef DtCInformationDialogTitle
#define DtCInformationDialogTitle	"InformationDialogTitle"
#endif
#ifndef DtNinsertLabel
#define DtNinsertLabel		       	"insertLabel"
#endif
#ifndef DtCInsertLabel 
#define DtCInsertLabel       		"InsertLabel"
#endif
#ifndef DtNjustifyToggleLabel
#define DtNjustifyToggleLabel       	"justifyToggleLabel"
#endif
#ifndef DtCJustifyToggleLabel
#define DtCJustifyToggleLabel       	"JustifyToggleLabel"
#endif
#ifndef DtNlabelFontList  
#define DtNlabelFontList             	XmNlabelFontList
#endif
#ifndef DtNleftAlignToggleLabel
#define DtNleftAlignToggleLabel       	"leftAlignToggleLabel"
#endif
#ifndef DtCLeftAlignToggleLabel 
#define DtCLeftAlignToggleLabel       	"LeftAlignToggleLabel"
#endif
#ifndef DtNleftMarginFieldLabel
#define DtNleftMarginFieldLabel       	"leftMarginFieldLabel"
#endif
#ifndef DtCLeftMarginFieldLabel 
#define DtCLeftMarginFieldLabel       	"LeftMarginFieldLabel"
#endif
#ifndef DtNmaxLength  
#define DtNmaxLength             	XmNmaxLength
#endif
#ifndef DtCMaxLength  
#define DtCMaxLength             	XmCMaxLength
#endif
#ifndef DtNmisspelledListLabel
#define DtNmisspelledListLabel       	"misspelledListLabel"
#endif
#ifndef DtCMisspelledListLabel 
#define DtCMisspelledListLabel       	"MisspelledListLabel"
#endif
#ifndef DtNoverstrike 
#define DtNoverstrike			"overstrike"
#endif
#ifndef DtCOverstrike
#define DtCOverstrike          		"Overstrike"
#endif
#ifndef DtNoverstrikeLabel
#define DtNoverstrikeLabel       	"overstrikeLabel"
#endif
#ifndef DtCOverstrikeLabel 
#define DtCOverstrikeLabel       	"OverstrikeLabel"
#endif
#ifndef DtNrightAlignToggleLabel
#define DtNrightAlignToggleLabel       	"rightAlignToggleLabel"
#endif
#ifndef DtCRightAlignToggleLabel 
#define DtCRightAlignToggleLabel       	"RightAlignToggleLabel"
#endif
#ifndef DtNrightMarginFieldLabel
#define DtNrightMarginFieldLabel       	"rightMarginFieldLabel"
#endif
#ifndef DtCRightMarginFieldLabel 
#define DtCRightMarginFieldLabel       	"RightMarginFieldLabel"
#endif
#ifndef DtNrows 
#define DtNrows            		XmNrows
#endif
#ifndef DtCRows
#define DtCRows  		        XmCRows
#endif
#ifndef DtNscrollHorizontal 
#define DtNscrollHorizontal		XmNscrollHorizontal
#endif
#ifndef DtCScroll
#define DtCScroll          		XmCScroll
#endif
#ifndef DtNscrollLeftSide 
#define DtNscrollLeftSide		XmNscrollLeftSide
#endif
#ifndef DtCScrollSide
#define DtCScrollSide          		XmCScrollSide
#endif
#ifndef DtNscrollTopSide 
#define DtNscrollTopSide		XmNscrollTopSide
#endif
#ifndef DtNscrollVertical 
#define DtNscrollVertical		XmNscrollVertical
#endif
#ifndef DtNshowStatusLine
#define DtNshowStatusLine		"showStatusLine"
#endif
#ifndef DtCShowStatusLine
#define DtCShowStatusLine		"ShowStatusLine"
#endif
#ifndef DtNspellDialogTitle
#define DtNspellDialogTitle		"spellDialogTitle"
#endif
#ifndef DtCSpellDialogTitle
#define DtCSpellDialogTitle		"SpellDialogTitle"
#endif
#ifndef DtNspellFilter
#define DtNspellFilter			"spellFilter"
#endif
#ifndef DtCSpellFilter
#define DtCSpellFilter			"SpellFilter"
#endif
#ifndef DtNtextBackground 
#define DtNtextBackground   		"textBackground"
#endif
#ifndef DtCBackground 
#define DtCBackground   		XmCBackground
#endif
#ifndef DtNtextDeselectCallback 
#define DtNtextDeselectCallback 	"textDeselectCallback"
#endif
#ifndef DtNtextFontList  
#define DtNtextFontList             	XmNtextFontList
#endif
#ifndef DtNtextForeground 
#define DtNtextForeground   		"textForeground"
#endif
#ifndef DtCForeground 
#define DtCForeground   		XmCForeground
#endif
#ifndef DtNtextSelectCallback 
#define DtNtextSelectCallback   	"textSelectCallback"
#endif
#ifndef DtNtextTranslations 
#define DtNtextTranslations   		XmNtextTranslations
#endif
#ifndef DtCTranslations 
#define DtCTranslations   		XmCTranslations
#endif
#ifndef DtCCallback 
#define DtCCallback   			XmCCallback
#endif
#ifndef DtNtopCharacter 
#define DtNtopCharacter			XmNtopCharacter
#endif
#ifndef DtCTopCharacter  
#define DtCTopCharacter			XmCTopCharacter
#endif
#ifndef DtNtotalLineCountLabel
#define DtNtotalLineCountLabel       	"totalLineCountLabel"
#endif
#ifndef DtCTotalLineCountLabel 
#define DtCTotalLineCountLabel       	"TotalLineCountLabel"
#endif
#ifndef DtNwordWrap 
#define DtNwordWrap			XmNwordWrap
#endif
#ifndef DtCWordWrap  
#define DtCWordWrap			XmCWordWrap
#endif

#ifndef DtEditorHelpCallbackStruct
#define DtEditorHelpCallbackStruct XmAnyCallbackStruct 
#endif
#ifndef DtEditorSelectCallbackStruct 
#define DtEditorSelectCallbackStruct	XmAnyCallbackStruct
#endif
#ifndef DtEditorDeselectCallbackStruct 
#define DtEditorDeselectCallbackStruct	XmAnyCallbackStruct
#endif


/* Used with DtEditorChange() */

enum {
	DtEDITOR_CURRENT_SELECTION,
	DtEDITOR_NEXT_OCCURRENCE,
	DtEDITOR_ALL_OCCURRENCES 
};

/* Used with DtEditorFormat() */

enum {
	DtEDITOR_FORMAT_ALL,
	DtEDITOR_FORMAT_PARAGRAPH
};

enum {
	DtEDITOR_ALIGN_CENTER,
	DtEDITOR_ALIGN_JUSTIFY,
	DtEDITOR_ALIGN_LEFT,
	DtEDITOR_ALIGN_RIGHT
};

/* Used with DtNtextSelectCallback */
enum {
        DtEDITOR_TEXT_SELECT,
        DtEDITOR_TEXT_DESELECT
};

/*
 * Types
 */

typedef enum _DtEditorErrorCode{
        DtEDITOR_NO_ERRORS,
        DtEDITOR_INVALID_TYPE,
        DtEDITOR_INVALID_RANGE,
        DtEDITOR_NULL_ITEM,
        DtEDITOR_ILLEGAL_SIZE,
        DtEDITOR_SPELL_FILTER_FAILED,
        DtEDITOR_NO_TMP_FILE,
        DtEDITOR_INVALID_FILENAME,
        DtEDITOR_NONEXISTENT_FILE,
        DtEDITOR_UNREADABLE_FILE,
        DtEDITOR_READ_ONLY_FILE,
        DtEDITOR_NO_FILE_ACCESS,
        DtEDITOR_DIRECTORY,
        DtEDITOR_CHAR_SPECIAL_FILE,
        DtEDITOR_BLOCK_MODE_FILE,
        DtEDITOR_UNWRITABLE_FILE,
        DtEDITOR_WRITABLE_FILE,
        DtEDITOR_SAVE_FAILED,
	DtEDITOR_INSUFFICIENT_MEMORY,
	DtEDITOR_NULLS_REMOVED
} DtEditorErrorCode;


/* Used with DtEditorChange() to specify Find and Change To strings */

typedef struct _DtEditorChangeValues {
        char *find;
        char *changeTo;
} DtEditorChangeValues;

/* Used with DtEditorFormat() to specify left margin, right margin *
 * and justification styles.					   */

typedef struct _DtEditorFormatSettings {
	int		leftMargin;
	int		rightMargin;
	unsigned int	alignment;
} DtEditorFormatSettings;

/* Types used to tag data to/from the editor widget */

typedef enum _DtEditorDataFormat{
        DtEDITOR_TEXT,
        DtEDITOR_WCHAR,
        DtEDITOR_DATA
} DtEditorDataFormat;


/* Types used to pass data to/from the editor widget */

typedef struct _DtEditor_DataObj {
        unsigned int    length;
        void            *buf;
} DtEditor_DataObj;

typedef struct _DtEditorContentRec {
        DtEditorDataFormat type;
        union   {
                 char     *string;
                 wchar_t  *wchar;
		 DtEditor_DataObj	data;
        } value;
} DtEditorContentRec;


/* Widget class and instance */

typedef struct	_DtEditorClassRec	*DtEditorClass;
typedef struct	_DtEditorRec		*DtEditorWidget;


/*
 * Data
 */

/* Widget class */

externalref	WidgetClass		dtEditorWidgetClass;


/*
 * Functions
 */

extern Widget DtCreateEditor(
		Widget		parent,
		char		*name,
		ArgList		arglist,
		Cardinal	argcount);

extern DtEditorErrorCode DtEditorAppend(
		Widget		widget,
		DtEditorContentRec *data);

extern DtEditorErrorCode DtEditorAppendFromFile(
		Widget		widget,
		char		*fileName);

extern Boolean DtEditorChange(
		Widget			widget,
		DtEditorChangeValues	*findChangeStrings,
		unsigned int		instanceToChange);

extern Boolean DtEditorCheckForUnsavedChanges(
		Widget		widget);

extern Boolean DtEditorClearSelection(
		Widget		widget);

extern Boolean DtEditorCopyToClipboard(
		Widget		widget);

extern Boolean DtEditorCutToClipboard(
		Widget		widget);

extern Boolean DtEditorDeleteSelection(
		Widget		widget);

extern Boolean DtEditorDeselect(
		Widget		widget);

extern void DtEditorDisableRedisplay(
		Widget		widget);

extern void DtEditorEnableRedisplay(
		Widget		widget);

extern Boolean DtEditorFind(
		Widget		widget,
		char * 	find);

extern DtEditorErrorCode DtEditorFormat(
		Widget		widget,
		DtEditorFormatSettings	*formatSettings,
		unsigned int	amountToFormat); 

extern DtEditorErrorCode DtEditorGetContents(
		Widget		widget,
		DtEditorContentRec *data,
		Boolean		hardCarriageReturns,
		Boolean		markContentsAsSaved);

extern XmTextPosition DtEditorGetInsertionPosition(
		Widget		widget);

extern XmTextPosition DtEditorGetLastPosition(
		Widget		widget);

extern Widget DtEditorGetMessageTextFieldID(
		Widget		widget);

extern void DtEditorGetSizeHints(
		Widget		widget,
		XSizeHints	*pHints);

extern void DtEditorGoToLine(
		Widget		widget,
		int	lineNumber);

extern DtEditorErrorCode DtEditorInsert(
		Widget		widget,
		DtEditorContentRec *data);

extern DtEditorErrorCode DtEditorInsertFromFile(
		Widget		widget,
		char		*fileName);

extern void DtEditorInvokeFindChangeDialog(
		Widget		widget);

extern void DtEditorInvokeFormatDialog(
		Widget		widget);

extern DtEditorErrorCode DtEditorInvokeSpellDialog(
		Widget		widget);

extern Boolean DtEditorPasteFromClipboard(
		Widget		widget);

extern DtEditorErrorCode DtEditorReplace(
		Widget			widget,
		XmTextPosition		startPos,
		XmTextPosition		endPos,
		DtEditorContentRec	*data);

extern DtEditorErrorCode DtEditorReplaceFromFile(
		Widget			widget,
		XmTextPosition		startPos,
		XmTextPosition		endPos,
		char			*fileName);

extern void DtEditorReset(
		Widget		widget);

extern DtEditorErrorCode DtEditorSaveContentsToFile(
		Widget		widget,
		char		*fileName,
		Boolean		overwriteIfExists,
		Boolean		hardCarriageReturns,
		Boolean		markContentsAsSaved);

extern Boolean DtEditorSelectAll(
		Widget		widget);

extern DtEditorErrorCode DtEditorSetContents(
		Widget		widget,
		DtEditorContentRec *data);

extern DtEditorErrorCode DtEditorSetContentsFromFile(
		Widget		widget,
		char		*fileName);

extern void DtEditorSetInsertionPosition(
		Widget		widget,
		XmTextPosition	position);

extern void DtEditorTraverseToEditor(
		Widget		widget);

extern Boolean DtEditorUndoEdit(
		Widget		widget);


#ifdef __cplusplus
}
#endif

#endif /* _Dt_Editor_h */
