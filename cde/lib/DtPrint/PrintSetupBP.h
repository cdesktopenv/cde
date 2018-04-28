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
/* $XConsortium: PrintSetupBP.h /main/16 1996/11/01 15:00:02 cde-hp $ */
/*
 * DtPrint/PrintSetupBP.h
 */
/* (c) Copyright 1990, 1991, 1992, 1993, 1994 OPEN SOFTWARE FOUNDATION, INC.
 *     ALL RIGHTS RESERVED
 * (c) Copyright 1989, 1996 DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASS.
 * (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992, 1995, 1996,
 *     HEWLETT-PACKARD COMPANY
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
#ifndef _DtPrintSetupBP_h
#define _DtPrintSetupBP_h

#include <Xm/BulletinBP.h>
#include <Dt/Print.h>
#include <Dt/PrintMsgsP.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ------------------------------------------------------------------------
 * Constant Definitions
 *
 */
/*
 * Message Catalog Lookup
 */
#define WARN_WORK_AREA_LOCATION DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_WARN_WA_LOCATION, _DtPrMsgPrintSetupBox_0000)
#define SELECT_PRINTER_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_SELECT_PRINTER, _DtPrMsgPrintSetupBox_0001)
#define PRINT_TO_FILE_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_PRINT_TO_FILE, _DtPrMsgPrintSetupBox_0002)
#define SELECT_FILE_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_SELECT_FILE, _DtPrMsgPrintSetupBox_0003)
#define CANCEL_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_CANCEL, _DtPrMsgPrintSetupBox_0004)
#define COPIES_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_COPIES, _DtPrMsgPrintSetupBox_0005)
#define HELP_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_HELP, _DtPrMsgPrintSetupBox_0006)
#define PRINT_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_PRINT, _DtPrMsgPrintSetupBox_0007)
#define PRINTER_NAME_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_PRINTER_NAME, _DtPrMsgPrintSetupBox_0008)
#define SETUP_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_SETUP, _DtPrMsgPrintSetupBox_0009)
#define DESCRIPTION_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_DESCRIPTION, _DtPrMsgPrintSetupBox_0010)
#define WARN_COPY_COUNT DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_WARN_COPY_COUNT, _DtPrMsgPrintSetupBox_0011)
#define WARN_SETUP_MODE DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_WARN_SETUP_MODE, _DtPrMsgPrintSetupBox_0012)
#define WARN_SETUP_MODE_CHANGE DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_WARN_SETUP_MODE_CHANGE, _DtPrMsgPrintSetupBox_0013)
#define PRINTER_INFO_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_PRINTER_INFO, _DtPrMsgPrintSetupBox_0014)
#define INVALID_PRINTER_TITLE DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_INVALID_PRINTER_TITLE, _DtPrMsgPrintSetupBox_0015)
#define INVALID_PRINTER_MESSAGE DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_INVALID_PRINTER_MESSAGE, _DtPrMsgPrintSetupBox_0016)
#define INVALID_DISPLAY_MESSAGE DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_INVALID_DISPLAY_MESSAGE, _DtPrMsgPrintSetupBox_0017)
#define NOT_XP_DISPLAY_MESSAGE DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_NOT_XP_DISPLAY_MESSAGE, _DtPrMsgPrintSetupBox_0018)
#define NO_DEFAULT_MESSAGE DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_NO_DEFAULT_MESSAGE, _DtPrMsgPrintSetupBox_0019)
#define NO_DEFAULT_DISPLAY_MESSAGE DTPRINT_GETMESSAGE(MS_PrintSetupBox, \
    PSUB_NO_DEFAULT_DISPLAY_MESSAGE, _DtPrMsgPrintSetupBox_0020)
#define PRINTER_MISSING_MESSAGE DTPRINT_GETMESSAGE(MS_PrintSetupBox, \
    PSUB_PRINTER_MISSING_MESSAGE, _DtPrMsgPrintSetupBox_0021)
#define WARN_PRINT_DESTINATION DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_WARN_PRINT_DESTINATION, _DtPrMsgPrintSetupBox_0022)
#define WARN_CT_CONVERSION DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_WARN_CT_CONVERSION, _DtPrMsgPrintSetupBox_0023)
#define WARN_CONVERSION_ARGS DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_WARN_CONVERSION_ARGS, _DtPrMsgPrintSetupBox_0024)
#define PRLIST_ERROR_TITLE DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_PRLIST_ERROR_TITLE, _DtPrMsgPrintSetupBox_0025)
#define NO_PRINTERS_MESSAGE DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_NO_PRINTERS_MESSAGE, _DtPrMsgPrintSetupBox_0026)
#define DESC_UNAVAILABLE DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_DESC_UNAVAILABLE, _DtPrMsgPrintSetupBox_0027)
#define SELECT_FILE_TITLE DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_SELECT_FILE_TITLE, _DtPrMsgPrintSetupBox_0028)
#define PRINTER_INFO_TITLE DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_PRINTER_INFO_TITLE, _DtPrMsgPrintSetupBox_0029)
#define FORMAT_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_FORMAT_LABEL, _DtPrMsgPrintSetupBox_0030)
#define MODEL_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_MODEL_LABEL, _DtPrMsgPrintSetupBox_0031)
#define PRINTER_LIST_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_PRINTER_LIST_LABEL, _DtPrMsgPrintSetupBox_0032)
#define MORE_PRINTERS_TITLE DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_MORE_PRINTERS_TITLE, _DtPrMsgPrintSetupBox_0033)
#define PRINT_TO_PRINTER_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_PRINT_TO_PRINTER, _DtPrMsgPrintSetupBox_0034)
#define FILE_NAME_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_FILE_NAME, _DtPrMsgPrintSetupBox_0035)
#define HELP_DLG_TITLE DTPRINT_GETMESSAGE( \
    MS_PrintSetupBox, PSUB_HELP_DLG_TITLE, _DtPrMsgPrintSetupBox_0036)
#
/*
 * Constants for Use in Allocation Geometry Matrix. 
 */
#define DtPSUB_MAX_WIDGETS_VERT	12

/*
 * ------------------------------------------------------------------------
 * Type Definitions
 *
 */
/*
 * data private to default resource procedures
 */
typedef struct
{
    String printer_name_ct;
    String printer_name;
    String description;
} DtPrintSelectPrinterRec, *DtPrintSelectPrinterList;
    
typedef struct _DtPrintDefaultProcData
{
    /*
     * common data
     */
    Widget error_message_box;
    XtEnum messages_hint;
    Widget help_dialog;
    /*
     * select file proc
     */
    Widget file_selection_box;
    /*
     * select printer proc
     */
    Widget printer_selection_box;
    Widget printer_list_box;
    String* xp_server_list;
    int xp_server_count;
    DtPrintSelectPrinterList* printer_lists;
    int* printer_counts;
    int selected_printer;
    Display* select_printer_info_display;
#if 0 && defined(PRINTING_SUPPORTED)
    XPContext select_printer_info_context;
#endif /* PRINTING_SUPPORT */
    /*
     * printer info proc
     */
    Widget printer_info_box;

} DtPrintDefaultProcData;

/*
 * verify state
 */
typedef enum {
    DtPRINT_VERIFIED,
    DtPRINT_IN_VERIFY,
    DtPRINT_NOT_VERIFIED
} DtPrintVerifyState;

/*
 * Constraint part record for PrintSetupBox widget 
 */
typedef struct _DtPrintSetupBoxConstraintPart
{
   char unused;
} DtPrintSetupBoxConstraintPart, * DtPrintSetupBoxConstraint;
/*
 * New fields for the PrintSetupBox widget class record  
 */
typedef struct
{
    XtCallbackProc	list_callback ;
    XtPointer		extension;      /* Pointer to extension record */
} DtPrintSetupBoxClassPart;
/*
 * Full class record declaration 
 */
typedef struct _DtPrintSetupBoxClassRec
{
    CoreClassPart		core_class;
    CompositeClassPart		composite_class;
    ConstraintClassPart		constraint_class;
    XmManagerClassPart		manager_class;
    XmBulletinBoardClassPart	bulletin_board_class;
    DtPrintSetupBoxClassPart	print_setup_box_class;
} DtPrintSetupBoxClassRec;

externalref DtPrintSetupBoxClassRec dtPrintSetupBoxClassRec;

/*
 * New fields for the PrintSetupBox widget record 
 */
typedef struct
{
    /*
     * synthetic resources
     */
    String file_name;
    String printer_name;
    XmString description_string;
    int copies;
    /*
     * flag resources
     */
    Boolean minimize_buttons;
    XtEnum print_destination;
    XtEnum print_setup_mode;
    XtEnum work_area_location;
    /*
     * procedure resources
     */
    DtPrintSetupProc printer_info_proc;
    DtPrintSetupProc select_file_proc;
    DtPrintSetupProc select_printer_proc;
    DtPrintSetupProc verify_printer_proc;
    /*
     * callback resources
     */
    XtCallbackList cancel_callback;
    XtCallbackList close_display_callback;
    XtCallbackList print_callback;
    XtCallbackList setup_callback;
    /*
     * other resources
     */
    XrmOptionDescList options;
    Cardinal option_count;
    /*
     * Xp standard resources
     */
    XtEnum xp_printer_name_mode;
    /*
     * internal flags
     */
    Boolean adding_sel_widgets;
    DtPrintVerifyState verify_printer_state;
    Boolean copy_count_supported;
    /*
     * internal data
     */
    String modal_printer_spec;
    Display* print_display;
#if 0 && defined(PRINTING_SUPPORTED)
    XPContext print_context;
#endif /* PRINTING_SUPPORTED */
    XtIntervalId timeout_id;
    /*
     * descendents
     */
    Widget bottom_separator;
    Widget bottom_work_area;
    Widget button_separator;
    Widget copies_control;
    Widget copies_spin_box;
    Widget description;
    Widget description_label;
    Widget destination_radio_box;
    Widget file_name_label;
    Widget file_name_text;
    Widget help_button;
    Widget print_button;
    Widget printer_info_button;
    Widget printer_name_label;
    Widget printer_name_text;
    Widget printer_name_combo;
    Widget select_file_button;
    Widget select_printer_button;
    Widget setup_button;
    Widget top_separator;
    Widget top_work_area;
    /*
     * data private to the default resource procedures
     */
    DtPrintDefaultProcData default_proc_data;

} DtPrintSetupBoxPart;

/*
 * Full instance record declaration
 */
typedef struct _DtPrintSetupBoxRec
{
	CorePart		core;
	CompositePart		composite;
	ConstraintPart		constraint;
	XmManagerPart		manager;
	XmBulletinBoardPart	bulletin_board;
	DtPrintSetupBoxPart	print_setup_box;
} DtPrintSetupBoxRec;

/*
 * ------------------------------------------------------------------------
 * Macro Definitions
 *
 */
/*
 * Instance Record Access
 */
#define PSUB_AddingSelWidgets(w) \
    (((DtPrintSetupBoxWidget) w)->print_setup_box.adding_sel_widgets)
#define PSUB_AutoUnmanage(w) \
    (((DtPrintSetupBoxWidget) (w))->bulletin_board.auto_unmanage)
#define PSUB_BottomSeparator(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.bottom_separator)
#define PSUB_BottomWorkArea(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.bottom_work_area)
#define PSUB_ButtonSeparator(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.button_separator)
#define PSUB_ButtonFontList(w) \
    (((DtPrintSetupBoxWidget) (w))->bulletin_board.button_font_list)
#define PSUB_CancelButton(w) \
    (((DtPrintSetupBoxWidget) (w))->bulletin_board.cancel_button)
#define PSUB_CancelCallback(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.cancel_callback)
#define PSUB_CloseDisplayCallback(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.close_display_callback)
#if 0 && defined(PRINTING_SUPPORTED)
#define PSUB_Context(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.print_context)
#endif /* PRINTING_SUPPORTED */
#define PSUB_Copies(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.copies)
#define PSUB_CopiesControl(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.copies_control)
#define PSUB_CopiesSpinBox(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.copies_spin_box)
#define PSUB_CopyCountSupported(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.copy_count_supported)
#define PSUB_DefaultButton(w) \
    (((DtPrintSetupBoxWidget) (w))->bulletin_board.default_button)
#define PSUB_DefaultProcData(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.default_proc_data)
#define PSUB_Description(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.description)
#define PSUB_DescriptionLabel(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.description_label)
#define PSUB_DescriptionString(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.description_string)
#define PSUB_DestinationRadioBox(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.destination_radio_box)
#define PSUB_Display(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.print_display)
#define PSUB_FileName(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.file_name)
#define PSUB_FileNameLabel(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.file_name_label)
#define PSUB_FileNameText(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.file_name_text)
#define PSUB_HelpButton(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.help_button)
#define PSUB_LabelFontList(w) \
    (((DtPrintSetupBoxWidget) (w))->bulletin_board.label_font_list)
#define PSUB_MarginHeight(w) \
    (((DtPrintSetupBoxWidget) (w))->bulletin_board.margin_height)
#define PSUB_MarginWidth(w) \
    (((DtPrintSetupBoxWidget) (w))->bulletin_board.margin_width)
#define PSUB_MinimizeButtons(w) \
    (((DtPrintSetupBoxWidget) w)->print_setup_box.minimize_buttons)
#define PSUB_ModalPrinterSpec(w) \
    (((DtPrintSetupBoxWidget) w)->print_setup_box.modal_printer_spec)
#define PSUB_MustMatch(w) \
    (((DtPrintSetupBoxWidget) w)->print_setup_box.must_match)
#define PSUB_OptionCount(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.option_count)
#define PSUB_Options(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.options)
#define PSUB_PrintButton(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.print_button)
#define PSUB_PrintCallback(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.print_callback)
#define PSUB_PrintDestination(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.print_destination)
#define PSUB_PrinterInfoButton(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.printer_info_button)
#define PSUB_PrinterInfoProc(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.printer_info_proc)
#define PSUB_PrinterName(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.printer_name)
#define PSUB_PrinterNameText(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.printer_name_text)
#define PSUB_PrinterNameCombo(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.printer_name_combo)
#define PSUB_PrinterNameLabel(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.printer_name_label)
#define PSUB_PrintSetupMode(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.print_setup_mode)
#define PSUB_SelectFileButton(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.select_file_button)
#define PSUB_SelectFileProc(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.select_file_proc)
#define PSUB_SelectPrinterButton(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.select_printer_button)
#define PSUB_SelectPrinterProc(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.select_printer_proc)
#define PSUB_SetupButton(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.setup_button)
#define PSUB_SetupCallback(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.setup_callback)
#define PSUB_Shell(w) \
    (((DtPrintSetupBoxWidget) (w))->bulletin_board.shell)
#define PSUB_StringDirection(w) \
    (((DtPrintSetupBoxWidget)(w))->manager.string_direction)
#define PSUB_Text(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.text)
#define PSUB_TextColumns(w) \
    (((DtPrintSetupBoxWidget) w)->print_setup_box.text_columns)
#define PSUB_TextFontList(w) \
    (((DtPrintSetupBoxWidget) (w))->bulletin_board.text_font_list)
#define PSUB_TimeoutId(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.timeout_id)
#define PSUB_TopSeparator(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.top_separator)
#define PSUB_TopWorkArea(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.top_work_area)
#define PSUB_VerifyPrinterProc(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.verify_printer_proc)
#define PSUB_VerifyPrinterState(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.verify_printer_state)
#define PSUB_WorkAreaLocation(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.work_area_location)
#define PSUB_XpPrinterNameMode(w) \
    (((DtPrintSetupBoxWidget) (w))->print_setup_box.xp_printer_name_mode)

/*
 * ------------------------------------------------------------------------
 * Private Function Declarations
 *
 */
/*
 * Child Widget Creation Functions
 */
extern void _DtPrintSetupBoxCreateButtonSeparator(
						  DtPrintSetupBoxWidget psub);
extern void _DtPrintSetupBoxCreateBottomSeparator(
						  DtPrintSetupBoxWidget psub);
extern void _DtPrintSetupBoxCreateCancelButton(
					       DtPrintSetupBoxWidget psub);
extern void _DtPrintSetupBoxCreateCopiesControl(
						DtPrintSetupBoxWidget psub);
extern void _DtPrintSetupBoxCreateDescription(
					      DtPrintSetupBoxWidget psub);
extern void _DtPrintSetupBoxCreateDescriptionLabel(
						   DtPrintSetupBoxWidget psub);
extern void _DtPrintSetupBoxCreateDestinationRadioBox(
						      DtPrintSetupBoxWidget psub);
extern void _DtPrintSetupBoxCreateFileNameLabel(
					       DtPrintSetupBoxWidget psub);
extern void _DtPrintSetupBoxCreateFileNameText(
					       DtPrintSetupBoxWidget psub);
extern void _DtPrintSetupBoxCreateHelpButton(
					     DtPrintSetupBoxWidget psub);
extern void _DtPrintSetupBoxCreatePrintButton(
					      DtPrintSetupBoxWidget psub);
extern void _DtPrintSetupBoxCreatePrinterInfoButton(
						    DtPrintSetupBoxWidget psub);
extern void _DtPrintSetupBoxCreatePrinterNameLabel(
						   DtPrintSetupBoxWidget psub);
extern void _DtPrintSetupBoxCreatePrinterNameCombo(
						   DtPrintSetupBoxWidget psub);
extern void _DtPrintSetupBoxCreateSelectFileButton(
						   DtPrintSetupBoxWidget psub);
extern void _DtPrintSetupBoxCreateSelectPrinterButton(
						      DtPrintSetupBoxWidget psub);
extern void _DtPrintSetupBoxCreateSetupButton(
					      DtPrintSetupBoxWidget psub);
extern void _DtPrintSetupBoxCreateTopSeparator(
					       DtPrintSetupBoxWidget psub);
/*
 * Synthetic resource internal->external conversion functions
 */
extern void _DtPrintSetupBoxGetCopies(
				      Widget wid,
				      int resource_offset,
				      XtArgVal *value);
extern void _DtPrintSetupBoxGetDescription(
					   Widget wid,
					   int resource_offset,
					   XtArgVal *value);
extern void _DtPrintSetupBoxGetFileName(
					Widget wid,
					int resource_offset,
					XtArgVal *value);
extern void _DtPrintSetupBoxGetPrinterName(
					   Widget wid,
					   int resource_offset,
					   XtArgVal *value);
/*
 * Geometry Management
 */
extern XmGeoMatrix _DtPrintSetupBoxGeoMatrixCreate(
						   Widget wid,
						   Widget instigator,
						   XtWidgetGeometry *desired);
extern Boolean _DtPrintSetupBoxNoGeoRequest(
					    XmGeoMatrix geoSpec);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _DtPrintSetupBP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
