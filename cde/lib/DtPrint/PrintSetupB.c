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
/* $TOG: PrintSetupB.c /main/27 1998/08/06 17:28:56 mgreess $ */
/*
 * DtPrint/PrintSetupB.c
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
/*
 * ------------------------------------------------------------------------
 * Include Files
 *
 */
#include <Dt/DtNlUtils.h>
#ifdef MAXINT
#undef MAXINT
#endif

#include <ctype.h>
#include <stdlib.h>
#if defined(CSRG_BASED)
#define MAXINT INT_MAX
#else
#include <values.h>
#endif
#include <stdio.h>
#include <stdint.h>

#include <Xm/XmAll.h>
#include <Xm/ActivatableT.h>
#include <Xm/RowColumnP.h>
#include <Xm/TraitP.h>
#include <Xm/TakesDefT.h>
#include <Xm/XmPrivate.h>

#include <Dt/PrintSetupBP.h>
#include <Dt/PsubDefProcI.h>
#include <Dt/PsubUtilI.h>
#include <Dt/PrintI.h>


/*
 * ------------------------------------------------------------------------
 * Constant Definitions
 *
 */
#define IsButton(w) \
(((XtPointer) XmeTraitGet((XtPointer) XtClass((w)), XmQTactivatable) != NULL))

#define IsAutoButton(psub, w) ( \
			      w == PSUB_PrinterInfoButton(psub) || \
			      w == PSUB_SelectPrinterButton(psub) || \
			      w == PSUB_SelectFileButton(psub) || \
			      w == PSUB_PrintButton(psub) || \
			      w == PSUB_SetupButton(psub) || \
			      w == PSUB_CancelButton(psub) || \
			      w == PSUB_HelpButton(psub))

/*
 * value to indicate initially unspecified resources
 */
#define DtUNSPECIFIED (~0)

/*
 * limits
 */
#define MAX_COPIES MAXINT

/*
 * defaults
 */
#define WORK_AREA_DEFAULT DtWORK_AREA_BOTTOM
#define SETUP_MODE_DEFAULT DtPRINT_SETUP_XP

#define DESCRIPTION_COLUMNS 35
#define FILE_NAME_COLUMNS 27
#define PRINTER_NAME_COLUMNS 25

/*
 * Xp Attributes
 */
#define XpATTR_DESCRIPTOR "descriptor"
#define XpATTR_DOC_ATTRS_SUPPORTED "document-attributes-supported"
#define XpATTR_COPY_COUNT "copy-count"
#define XpATTR_SETUP_PROVISO "xp-setup-proviso"
#define XpATTR_SETUP_STATE "xp-setup-state"

/*
 * ------------------------------------------------------------------------
 * Type Definitions
 *
 */
typedef enum {
    DtPRINT_NULL_CHILD,
    DtPRINT_CANCEL_BUTTON,
    DtPRINT_FILES_BUTTON,
    DtPRINT_HELP_BUTTON,
    DtPRINT_INFO_BUTTON,
    DtPRINT_PRINT_BUTTON,
    DtPRINT_PRINTERS_BUTTON,
    DtPRINT_SETUP_BUTTON
} DtPrintSetupBoxChild;

/*
 * column oriented geometry definitions
 */
#define PSUB_GEO_COL_COUNT 3
#define PSUB_GEO_MAX_ROW 4

typedef enum {
    PSUB_GEO_ALIGN_LEFT = 0, /* default */
    PSUB_GEO_ALIGN_RIGHT,
    PSUB_GEO_ALIGN_EXPAND
} PSUB_GeoBoxAlignment;

typedef struct
{
    XmKidGeometry geo;
    PSUB_GeoBoxAlignment align;
    Dimension orig_width;
} PSUB_GeoBoxRec, *PSUB_GeoBox;

typedef struct
{
    int row_count;
    PSUB_GeoBoxRec pbox[PSUB_GEO_MAX_ROW][PSUB_GEO_COL_COUNT];
    XmGeoRowLayout layout_ptr[PSUB_GEO_MAX_ROW];
    Dimension col_width[PSUB_GEO_COL_COUNT];
} PSUB_GeoExtensionRec, *PSUB_GeoExtension ;

/*
 * ------------------------------------------------------------------------
 * External Function Declarations
 *
 */
/*
 * Geometry management functions defined in Xm/GeoUtils
 */
extern XmGeoMatrix _XmGeoMatrixAlloc(
				     unsigned int numRows,
				     unsigned int numBoxes,
				     unsigned int extSize);
extern Boolean _XmGeoSetupKid(
			      XmKidGeometry geo,
			      Widget kidWid);
/*
 * ------------------------------------------------------------------------
 * Static Function Declarations
 *
 */
static void ClassInitialize(
			    void);
static void ClassPartInitialize(
				WidgetClass w_class);
static void ClosePrintConnection(
				 DtPrintSetupBoxWidget psub,
				 String old_printer_name,
				 Boolean update_gui);
static void ClosePrintConnectionCallback(Widget w,
					 XtPointer client_data,
					 XtPointer call_data);
static void ColumnGeoFixUp(
			   XmGeoMatrix geoSpec,
			   int action,
			   XmGeoMajorLayout layoutPtr,
			   XmKidGeometry rowPtr);
static void ColumnGeoPreSet(
			    PSUB_GeoExtension ext,
			    int row);
static void ColumnGeoRestoreWidths(
				   PSUB_GeoExtension ext,
				   int row);
static void ColumnGeoSaveWidths(
				PSUB_GeoExtension ext,
				int row);
static void ColumnGeoSetPreferredWidths(
					PSUB_GeoExtension ext,
					int row);
static Boolean ConvertStringToEnum(
				   Display *dpy,
				   XrmValuePtr args,
				   Cardinal *num_args,
				   XrmValuePtr from,
				   XrmValuePtr to,
				   XtPointer *data);
static void CopiesTextValueChangedCallback(
				       Widget w,
				       XtPointer client_data,
				       XtPointer call_data);
static Widget CreateButton(Widget parent,
			   String label,
			   String name,
			   XmNavigationType nav_type);
static Widget CreateButtonGadget(
				 Widget parent,
			         String label,
				 String name,
				 XmNavigationType nav_type);
static Widget CreateLabelGadget(
				Widget parent,
				String label,
				String name);
static String CreateModalPrinterSpec(
				     XtEnum name_mode,
				     const String full_printer_name);
static void DeleteChild(
			Widget child);
static void DestinationChangedCallback(
				       Widget w,
				       XtPointer client_data,
				       XtPointer call_data);
static void Destroy(
		    Widget w);
static XtEnum EstablishPrinter(
			       DtPrintSetupBoxWidget psub,
			       DtPrintSetupData* psd);
static void GetPrintAttributes(
			       DtPrintSetupBoxWidget psub);
static void Initialize(
		       Widget rw,
		       Widget nw,
		       ArgList args,
		       Cardinal *num_args);
static void InsertChild(
			Widget child);
static Boolean IsSetupRequired(
			       DtPrintSetupBoxWidget psub);
static void MenuBarFixUp(
			 XmGeoMatrix geoSpec,
			 int action,
			 XmGeoMajorLayout layoutPtr,
			 XmKidGeometry rowPtr);
static void PrintSetupBoxCallback(
				  Widget w,
				  XtPointer client_data,
				  XtPointer call_data);
static void SeparatorFixUp(
			   XmGeoMatrix geoSpec,
			   int action,
			   XmGeoMajorLayout layoutPtr,
			   XmKidGeometry rowPtr);
static void SetNewPrinterName(
			      DtPrintSetupBoxWidget psub,
			      String new_printer_name,
			      String old_printer_name);
static void SetPrintAttributes(
			       DtPrintSetupBoxWidget psub);
static void SetPSDDestination(
			      DtPrintSetupBoxWidget psub,
			      DtPrintSetupData* psd);
static XmImportOperator SetSyntheticResForChild(
						Widget widget,
						int offset,
						XtArgVal *value);
static Boolean SetValues(
			 Widget cw,
			 Widget rw,
			 Widget nw,
			 ArgList args,
			 Cardinal *num_args) ;
static int SpanNonWhitespace(
			     const char* string);
static int SpanWhitespace(
			  const char* string);
static void SynthGetFileName(
			     DtPrintSetupBoxWidget psub);
static void SynthGetPrinterName(
				DtPrintSetupBoxWidget psub);
static void SynthSetCopies(
			   DtPrintSetupBoxWidget new_w,
			   DtPrintSetupBoxWidget current);
static void SynthSetFileName(
			     DtPrintSetupBoxWidget new_w,
			     DtPrintSetupBoxWidget current);
static void SynthSetPrintDestination(
				     DtPrintSetupBoxWidget new_w,
				     DtPrintSetupBoxWidget current);
static void SynthSetPrinterName(
				DtPrintSetupBoxWidget psub,
				String new_printer_name,
				String old_printer_name);
static void UpdatePrinterNameCallback(
				      Widget w,
				      XtPointer client_data,
				      XtPointer call_data);
static void UpdatePrinterNameTimeoutProc(XtPointer client_data,
					 XtIntervalId* id);
static void UpdateString(
			 Widget w,
			 XmString string,
                         XmStringDirection direction) ;
static void ValidatePrintSetupMode(
				   DtPrintSetupBoxWidget new_w,
				   DtPrintSetupBoxWidget current);
static void ValidateWorkAreaLocation(
				     DtPrintSetupBoxWidget new_w,
				     DtPrintSetupBoxWidget current);
/*
 * ------------------------------------------------------------------------
 * Static Global Variable Definitions
 *
 */

/*  
 * Synthetic Resource Definitions
 */
static XmSyntheticResource syn_resources[] = 
{
    {
	DtNcopies,
	sizeof(int), 
	XtOffsetOf(DtPrintSetupBoxRec,
		   print_setup_box.copies), 
	_DtPrintSetupBoxGetCopies,
	SetSyntheticResForChild
    },
    {
	DtNdescription,
	sizeof(XmString), 
	XtOffsetOf(DtPrintSetupBoxRec,
		   print_setup_box.description_string), 
	_DtPrintSetupBoxGetDescription,
	SetSyntheticResForChild
    },
    {
	DtNfileName,
	sizeof(String), 
	XtOffsetOf(DtPrintSetupBoxRec,
		   print_setup_box.file_name), 
	_DtPrintSetupBoxGetFileName,
	SetSyntheticResForChild
    },
    {
	DtNprinterName,
	sizeof(String), 
	XtOffsetOf(DtPrintSetupBoxRec,
		   print_setup_box.printer_name), 
	_DtPrintSetupBoxGetPrinterName,
	SetSyntheticResForChild
    }
};

/*
 * Resource Definitions
 */
static XtResource resources[] = 
{
    {
	XmNnoResize, XmNnoResize,
	XmRBoolean, sizeof(Boolean),
	XtOffsetOf(XmBulletinBoardRec, bulletin_board.no_resize),
	XmRImmediate, (XtPointer) True
    },
    {
	DtNcancelCallback, DtCCancelCallback,
	XmRCallback, sizeof(XtCallbackList), 
	XtOffsetOf(DtPrintSetupBoxRec,
		   print_setup_box.cancel_callback), 
	XmRImmediate, (XtPointer) NULL
    },
    {
	DtNclosePrintDisplayCallback, DtCClosePrintDisplayCallback,
	XmRCallback, sizeof(XtCallbackList), 
	XtOffsetOf(DtPrintSetupBoxRec,
		   print_setup_box.close_display_callback), 
	XmRImmediate, (XtPointer) NULL
    },
    {
	DtNcopies, DtCCopies,
	XmRInt, sizeof(int),
	XtOffsetOf(DtPrintSetupBoxRec,
		   print_setup_box.copies),
	XmRImmediate, (XtPointer) 1
    },
    {
	DtNdescription, DtCDescription,
	XmRXmString, sizeof(XmString), 
	XtOffsetOf(DtPrintSetupBoxRec,
		   print_setup_box.description_string), 
	XmRString, (XtPointer) NULL
    },
    {
	DtNfileName, DtCFileName,
	XmRString, sizeof(String), 
	XtOffsetOf(DtPrintSetupBoxRec,
		   print_setup_box.file_name), 
	XmRString, (XtPointer) NULL
    },
    {
	DtNminimizeButtons, DtCMinimizeButtons,
	XmRBoolean, sizeof(Boolean), 
	XtOffsetOf(DtPrintSetupBoxRec,
		   print_setup_box.minimize_buttons), 
	XmRImmediate, (XtPointer) False
    },
    {
	DtNprintCallback, DtCPrintCallback,
	XmRCallback, sizeof(XtCallbackList), 
	XtOffsetOf(DtPrintSetupBoxRec,
		   print_setup_box.print_callback), 
	XmRImmediate, (XtPointer) NULL
    },
    {
	DtNoptionCount, DtCOptionCount,
	XmRCardinal, sizeof(Cardinal),
	XtOffsetOf(DtPrintSetupBoxRec,
		   print_setup_box.option_count),
	XmRImmediate, (XtPointer) 0
    },
    {
	DtNoptions, DtCOptions,
	XmRPointer, sizeof(XtPointer), 
	XtOffsetOf(DtPrintSetupBoxRec,
		   print_setup_box.options),
	XmRImmediate, (XtPointer) NULL
    },
    {
	DtNprintDestination, DtCPrintDestination,
	XmREnum, sizeof(XtEnum), 
	XtOffsetOf(DtPrintSetupBoxRec,
		   print_setup_box.print_destination), 
	XmRImmediate, (XtPointer) DtPRINT_TO_PRINTER,
    },
    {
	DtNprinterInfoProc, DtCPrinterInfoProc,
	DtRPrintSetupProc, sizeof(DtPrintSetupProc), 
	XtOffsetOf(DtPrintSetupBoxRec,
		   print_setup_box.printer_info_proc), 
	XmRImmediate, (XtPointer)DtUNSPECIFIED
    },
    {
	DtNprinterName,	DtCPrinterName,
	XmRString, sizeof(String), 
	XtOffsetOf(DtPrintSetupBoxRec,
		   print_setup_box.printer_name), 
	XmRString, (XtPointer) NULL
    },
    {
	DtNprintSetupMode, DtCPrintSetupMode,
	XmREnum, sizeof(XtEnum), 
	XtOffsetOf(DtPrintSetupBoxRec,
		   print_setup_box.print_setup_mode),
	XmRImmediate, (XtPointer)SETUP_MODE_DEFAULT,
    },
    {
	DtNselectFileProc, DtCSelectFileProc,
	DtRPrintSetupProc, sizeof(DtPrintSetupProc), 
	XtOffsetOf(DtPrintSetupBoxRec,
		   print_setup_box.select_file_proc), 
	XmRImmediate, (XtPointer) _DtPrintSetupBoxSelectFileProc
    },
    {
	DtNselectPrinterProc, DtCSelectPrinterProc,
	DtRPrintSetupProc, sizeof(DtPrintSetupProc), 
	XtOffsetOf(DtPrintSetupBoxRec,
		   print_setup_box.select_printer_proc), 
	XmRImmediate, (XtPointer)DtUNSPECIFIED
    },
    {
	DtNsetupCallback, DtCSetupCallback,
        XmRCallback, sizeof(XtCallbackList), 
	XtOffsetOf(DtPrintSetupBoxRec,
		   print_setup_box.setup_callback), 
	XmRImmediate, (XtPointer) NULL
    },
    {
	DtNverifyPrinterProc, DtCVerifyPrinterProc,
	DtRPrintSetupProc, sizeof(DtPrintSetupProc), 
	XtOffsetOf(DtPrintSetupBoxRec,
		   print_setup_box.verify_printer_proc), 
	XmRImmediate, (XtPointer)DtUNSPECIFIED
    },
    {
	DtNworkAreaLocation, DtCWorkAreaLocation,
	XmREnum, sizeof(XtEnum), 
	XtOffsetOf(DtPrintSetupBoxRec,
		   print_setup_box.work_area_location), 
	XmRImmediate, (XtPointer)WORK_AREA_DEFAULT,
    },
};


externaldef(dtprintsetupboxclassrec) DtPrintSetupBoxClassRec
dtPrintSetupBoxClassRec =
{
    {
	/* superclass	      */	(WidgetClass)&xmBulletinBoardClassRec, 
	/* class_name	      */	"DtPrintSetupBox", 
	/* widget_size	      */	sizeof(DtPrintSetupBoxRec), 
	/* class_initialize   */	ClassInitialize, 
	/* chained class init */	ClassPartInitialize, 
	/* class_inited	      */	FALSE, 
	/* initialize	      */	Initialize, 
	/* initialize hook    */	NULL, 
	/* realize	      */	XtInheritRealize, 
	/* actions	      */	NULL, 
	/* num_actions	      */	0, 
	/* resources	      */	resources, 
	/* num_resources      */	XtNumber(resources), 
	/* xrm_class	      */	NULLQUARK, 
	/* compress_motion    */	TRUE, 
	/* compress_exposure  */	XtExposeCompressMaximal,
	/* compress enter/exit*/	TRUE, 
	/* visible_interest   */	FALSE, 
	/* destroy	      */	Destroy, 
	/* resize	      */	XtInheritResize,
	/* expose	      */	XtInheritExpose, 
	/* set_values	      */	SetValues, 
	/* set_values_hook    */	NULL,			 
	/* set_values_almost  */	XtInheritSetValuesAlmost,
	/* get_values_hook    */	NULL, 
	/* accept_focus	      */	NULL, 
	/* version	      */	XtVersion, 
	/* callback_offsets   */	NULL, 
	/* tm_table	      */	XtInheritTranslations, 
	/* query_geometry     */	XtInheritGeometryManager,
	/* display_accelerator*/	NULL, 
	/* extension	      */	NULL, 
    }, 

    {	/* composite class record */	

	/* childrens geo mgr proc   */	XtInheritGeometryManager,
	/* set changed proc	    */	XtInheritChangeManaged,
	/* insert_child		    */	InsertChild, 
	/* delete_child		    */	DeleteChild, 
	/* extension		    */	NULL, 
    }, 

    {	/* constraint class record */

	/* no additional resources  */	NULL, 
	/* num additional resources */	0, 
	/* size of constraint rec   */	0, 
	/* constraint_initialize    */	NULL, 
	/* constraint_destroy	    */	NULL, 
	/* constraint_setvalue	    */	NULL, 
	/* extension		    */	NULL, 
    }, 

    {	/* manager class record */
      XmInheritTranslations,			/* default translations	  */
      syn_resources,				/* syn_resources	  */
      XtNumber (syn_resources),			/* num_syn_resources	  */
      NULL,					/* syn_cont_resources	  */
      0,					/* num_syn_cont_resources */
      XmInheritParentProcess,			/* parent_process	  */
      NULL,					/* extension		  */
    }, 

    {	/* bulletin board class record */     
	TRUE,					/*always_install_accelerators*/
	_DtPrintSetupBoxGeoMatrixCreate,	 /* geo_matrix_create */
	XmInheritFocusMovedProc,		/* focus_moved_proc */
	NULL,					/* extension */
    },	

    {	/* print setup box class record */
	NULL,					/* extension  */
    }, 
};

externaldef(dtprintsetupboxwidgetclass) WidgetClass dtPrintSetupBoxWidgetClass = (WidgetClass) &dtPrintSetupBoxClassRec ;

/*
 * ------------------------------------------------------------------------
 * Name: ClassInitialize - Core class method
 *
 * Description:
 *
 * Return value:
 *
 *     None.
 *
 */
static void 
ClassInitialize(void)
{   
    /*
     * Register a type converter for String to Enum
     */
    XtSetTypeConverter("String", "Enum", ConvertStringToEnum,
		       NULL, 0, XtCacheAll, NULL);
}

/*
 * ------------------------------------------------------------------------
 * Name: ClassPartInitialize - Core class method
 *
 * Description:
 *
 * Return value:
 *
 *     None.
 *
 */
static void 
ClassPartInitialize(WidgetClass w_class)
{   
    DtPrintSetupBoxWidgetClass wc = (DtPrintSetupBoxWidgetClass) w_class;
    DtPrintSetupBoxWidgetClass super =
	(DtPrintSetupBoxWidgetClass) wc->core_class.superclass;

    if (wc->print_setup_box_class.list_callback == XmInheritCallbackProc)
	wc->print_setup_box_class.list_callback =
	    super->print_setup_box_class.list_callback;
}

	    
/*
 * ------------------------------------------------------------------------
 * Name: ClosePrintConnection
 *
 * Description:
 *
 *     Call the DtNclosePrintDisplayCallback list, and destroy the print
 *     context and X print display connection managed by the widget.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
ClosePrintConnection(
		     DtPrintSetupBoxWidget psub,
		     String old_printer_name,
		     Boolean update_gui)
{
    DtPrintSetupCallbackStruct cbs;
    DtPrintSetupData psd;
    XmString empty_label;
    
    memset(&cbs, 0, sizeof(DtPrintSetupCallbackStruct));
    memset(&psd, 0, sizeof(DtPrintSetupData));
    cbs.print_data = &psd;
    /*
     * call DtNclosePrintDisplayCallback before closing the
     * current print display
     */
    cbs.reason = DtPRINT_CR_CLOSE_PRINT_DISPLAY;
    psd.printer_name = old_printer_name;
    psd.print_display = PSUB_Display(psub);
    psd.print_context = PSUB_Context(psub);
    XtCallCallbackList((Widget)psub,
		       PSUB_CloseDisplayCallback(psub),
		       (XtPointer)&cbs);
    /*
     * unset the printer description
     */
    if(update_gui)
    {
	empty_label = XmStringGenerate((XtPointer)"", (XmStringTag)NULL,
				       XmMULTIBYTE_TEXT, (XmStringTag)NULL);
	UpdateString(PSUB_Description((Widget)psub), 
		     empty_label, PSUB_StringDirection((Widget)psub));
	XmStringFree(empty_label);
    }
    /*
     * reset copy_count_supported flag
     */
    if(!PSUB_CopyCountSupported(psub))
    {
	PSUB_CopyCountSupported(psub) = True;
	if(PSUB_PrintDestination(psub) == DtPRINT_TO_PRINTER && update_gui)
	{
	    if(PSUB_CopiesSpinBox(psub))
		XtSetSensitive(PSUB_CopiesSpinBox(psub), True);
	    if(PSUB_CopiesControl(psub))
	    {
		Widget copies_label =
		    XtNameToWidget(PSUB_CopiesControl(psub), "CopiesLabel");
		XtSetSensitive(copies_label, True);
	    }
	}
    }
    /*
     * destroy the print context
     */
    XpDestroyContext(PSUB_Display(psub), PSUB_Context(psub));
    PSUB_Context(psub) = (XPContext)NULL;
    /*
     * close the print display
     */
    XtCloseDisplay(PSUB_Display(psub));
    PSUB_Display(psub) = (Display*)NULL;
}

/*
 * ------------------------------------------------------------------------
 * Name: ClosePrintConnectionCallback
 *
 * Description:
 *
 *     Calls ClosePrintConnection if there is an existing connection.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
ClosePrintConnectionCallback(Widget w,
			     XtPointer client_data,
			     XtPointer call_data)
{
    DtPrintSetupBoxWidget psub = (DtPrintSetupBoxWidget)client_data;
    /*
     * close the Xp connection if open
     */
    if(PSUB_Display(psub) != (Display*)NULL)
    {
	ClosePrintConnection(psub, PSUB_PrinterName(psub), True);
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: ColumnGeoFixUp
 *
 * Description:
 *
 *     This routine is a fixup routine which can be used for groups of
 *     rows containing entries that can be organized into columns.
 *
 * Return value:
 *
 *     None.
 *
 */
static void 
ColumnGeoFixUp(
	       XmGeoMatrix geo_spec,
	       int action,
	       XmGeoMajorLayout layout_ptr,
	       XmKidGeometry row_ptr)
{
    PSUB_GeoExtension ext = geo_spec->extension;
    int row;
    /*
     * determine which row we're on
     */
    for(row = 0; row < ext->row_count; row++)
	if((ext->layout_ptr)[row] == &(layout_ptr->row))
	    break;
    if(row == ext->row_count)
	/*
	 * shouldn't happen
	 */
	return;

    switch(action)
    {
    case XmGET_PREFERRED_SIZE:
	ColumnGeoSaveWidths(ext, row);
	ColumnGeoSetPreferredWidths(ext, row);
	break;
	
    case XmGEO_PRE_SET:
	ColumnGeoPreSet(ext, row);
	break;

    case XmGEO_POST_SET:
	ColumnGeoRestoreWidths(ext, row);
	break;
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: ColumnGeoPreSet
 *
 * Description:
 *
 *     Update origins and widths based on alignment.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
ColumnGeoPreSet(
		PSUB_GeoExtension ext,
		int row)
{
    Dimension delta = 0;
    Dimension slack = 0;
    int col;
    
    for(col = 0; col < PSUB_GEO_COL_COUNT; col++)
    {
	PSUB_GeoBox pbox = &(ext->pbox)[row][col];
	if(pbox->geo)
	{
	    pbox->geo->box.x += slack;
	    
	    switch(pbox->align)
	    {
	    case PSUB_GEO_ALIGN_RIGHT:
		delta += (ext->col_width)[col] - pbox->geo->box.width;
		pbox->geo->box.x += (ext->col_width)[col] - pbox->orig_width;
		pbox->geo->box.width = pbox->orig_width;
		break;
		
	    case PSUB_GEO_ALIGN_LEFT:
		pbox->geo->box.x += delta;
		delta += (ext->col_width)[col] - pbox->geo->box.width;
		pbox->geo->box.width = pbox->orig_width;
		break;
		
	    case PSUB_GEO_ALIGN_EXPAND:
		pbox->geo->box.x += delta;
		pbox->geo->box.width = (ext->col_width)[col];
		break;
	    }
	}
	else
	    slack += (ext->col_width)[col];
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: ColumnGeoRestoreWidths
 *
 * Description:
 *
 *     Restore the original widths for items in the current row.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
ColumnGeoRestoreWidths(
		       PSUB_GeoExtension ext,
		       int row)
{
    int col;
    
    for(col = 0; col < PSUB_GEO_COL_COUNT; col++)
    {
	PSUB_GeoBox pbox = &(ext->pbox)[row][col];
	if(pbox->geo)
	{
	    /*
	     * restore the original box width
	     */
	    pbox->geo->box.width = pbox->orig_width;
	}
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: ColumnGeoSaveWidths
 *
 * Description:
 *
 *     Save the original widths for items in the current row, and
 *     increase a given column width if an item in a corresponding column
 *     in this row is wider.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
ColumnGeoSaveWidths(
		    PSUB_GeoExtension ext,
		    int row)
{
    int col;
    
    for(col = 0; col < PSUB_GEO_COL_COUNT; col++)
    {
	PSUB_GeoBox pbox = &(ext->pbox)[row][col];
	if(pbox->geo)
	{
	    /*
	     * save the original box width
	     */
	    pbox->orig_width = pbox->geo->box.width;
	    /*
	     * update the overall column width
	     */
	    if(pbox->geo->box.width > (ext->col_width)[col])
		(ext->col_width)[col] = pbox->geo->box.width;
	}
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: ColumnGeoSetPreferredWidths
 *
 * Description:
 *
 *     Ensure the overall width of the current row is equal to the
 *     overall width of all of the columns.
 *
 *     Assumes at least one box is defined in the row.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
ColumnGeoSetPreferredWidths(
			    PSUB_GeoExtension ext,
			    int row)
{
    Dimension slack = 0;
    int last_non_empty_col;
    int col;
    
    for(col = 0; col < PSUB_GEO_COL_COUNT; col++)
    {
	PSUB_GeoBox pbox = &(ext->pbox)[row][col];
	if(pbox->geo)
	{
	    pbox->geo->box.width = (ext->col_width)[col] + slack;
	    slack = 0;
	    last_non_empty_col = col;
	}
	else
	{
	    slack += (ext->col_width)[col];
	}
    }
    if(slack)
	(ext->pbox)[row][last_non_empty_col].geo->box.width += slack;
}

/*
 * ------------------------------------------------------------------------
 * Name: ConvertStringToEnum
 *
 * Description:
 *
 *     This type converter converts from type String to Enum.  It is
 *     used for the DtNprintDestination, DtNworkAreaLocation, and
 *     DtNprintSetupMode resources.
 *
 */
static Boolean
ConvertStringToEnum(
		    Display *dpy,
		    XrmValuePtr args,
		    Cardinal *num_args,
		    XrmValuePtr from, 
		    XrmValuePtr to,
		    XtPointer *data)
{
    char *from_str;
    static unsigned char to_value;

    if(*num_args !=0 )
    {
	XtError(WARN_CONVERSION_ARGS);
    }

    from_str = (char *)from->addr;
    if(strcmp(from_str, "PRINT_TO_FILE")==0 ||
       strcmp(from_str, "DtPRINT_TO_FILE")==0)
	to_value = DtPRINT_TO_FILE;
    else if(strcmp(from_str, "PRINT_TO_PRINTER")==0 ||
	    strcmp(from_str, "DtPRINT_TO_PRINTER")==0)
	to_value = DtPRINT_TO_PRINTER;
    else if(strcmp(from_str, "PRINT_SETUP_PLAIN")==0 ||
	    strcmp(from_str, "DtPRINT_SETUP_PLAIN")==0)
	to_value = DtPRINT_SETUP_PLAIN;
    else if(strcmp(from_str, "PRINT_SETUP_XP")==0 ||
	    strcmp(from_str, "DtPRINT_SETUP_XP")==0)
	to_value = DtPRINT_SETUP_XP;
    else if(strcmp(from_str, "WORK_AREA_BOTTOM")==0 ||
	    strcmp(from_str, "DtWORK_AREA_BOTTOM")==0)
	to_value = DtWORK_AREA_BOTTOM;
    else if(strcmp(from_str, "WORK_AREA_TOP")==0 ||
	    strcmp(from_str, "DtWORK_AREA_TOP")==0)
	to_value = DtWORK_AREA_TOP;
    else if(strcmp(from_str, "WORK_AREA_TOP_AND_BOTTOM")==0 ||
	    strcmp(from_str, "DtWORK_AREA_TOP_AND_BOTTOM")==0)
	to_value = DtWORK_AREA_TOP_AND_BOTTOM;
    else if(strcmp(from_str, "SHORT_NAME")==0 ||
	    strcmp(from_str, "DtSHORT_NAME")==0)
	to_value = DtSHORT_NAME;
    else if(strcmp(from_str, "MEDIUM_NAME")==0 ||
	    strcmp(from_str, "DtMEDIUM_NAME")==0)
	to_value = DtMEDIUM_NAME;
    else if(strcmp(from_str, "LONG_NAME")==0 ||
	    strcmp(from_str, "DtLONG_NAME")==0)
	to_value = DtLONG_NAME;
    else
    {
	XtDisplayStringConversionWarning(dpy, from->addr, "Enum");
	return False;
    }

    if(to->addr == NULL)
	to->addr = (caddr_t) &to_value;
    else if(to->size <sizeof(unsigned char))
    {
	XtDisplayStringConversionWarning(dpy, from->addr, "Enum");
	return False;
    }
    else
	*(unsigned char *) to->addr = to_value;

    to->size = sizeof(unsigned char);

    return True;
}

/*
 * ------------------------------------------------------------------------
 * Name: CopiesTextValueChangedCallback
 *
 * Description:
 *
 *     Process a change in the textfield of the copies simple spin box.
 *
 * Return value:
 *
 *     None.
 *
 */
static void 
CopiesTextValueChangedCallback(
			   Widget w,
			   XtPointer client_data,
			   XtPointer call_data)
{
    DtPrintSetupBoxWidget psub = (DtPrintSetupBoxWidget)client_data;
    XmAnyCallbackStruct* cbs = (XmAnyCallbackStruct*)call_data;

    if (NULL == psub || NULL == cbs) return;

    if(XmCR_VALUE_CHANGED == cbs->reason)
    {
	char *value = NULL;
	int value_int = 0;
	
	XtVaGetValues(w, XmNvalue, &value, NULL);
	if (value) value_int = strtol(value, (char**)NULL, 10);

	if (1 <= value_int || value_int <= MAX_COPIES)
	{
	    PSUB_Copies(psub) = value_int;
	    XtVaSetValues(
			PSUB_CopiesSpinBox(psub),
			XmNposition, value_int,
			NULL);
	}
	else
	{
	    XmeWarning((Widget)w, WARN_COPY_COUNT);
	    XtVaGetValues(
			PSUB_CopiesSpinBox(psub),
			XmNposition, &value_int,
			NULL);

	    value = (char*) XtMalloc(64); 
	    if (value)
	    {
	        sprintf(value, "%d", value_int);
	        XtVaSetValues(w, XmNvalue, value, NULL);
	        XtFree(value);
	    }
	}

	if (value) XtVaSetValues(w, XmNcursorPosition, strlen(value), NULL);
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: CreateButton
 *
 * Description:
 *
 * Return value:
 *
 *     Widget ID of the new button widget.
 *
 */
static Widget 
CreateButton(Widget parent,
	     String label,
	     String name,
	     XmNavigationType nav_type)
{
    XmString label_string;
    Widget w;
    XmTakesDefaultTrait trait_default ;
    
    label_string = XmStringGenerate((XtPointer)label, (XmStringTag)NULL,
				    XmMULTIBYTE_TEXT, (XmStringTag)NULL);
    w = XtVaCreateWidget(name,
			 xmPushButtonWidgetClass,
			 parent,
			 XmNlabelString, label_string,
			 XmNnavigationType, nav_type,
			 NULL);

    if(label_string) XmStringFree(label_string);

    trait_default = (XmTakesDefaultTrait)
	XmeTraitGet((XtPointer)XtClass(w), XmQTtakesDefault) ;
    if(trait_default) 
	trait_default->showAsDefault(w, XmDEFAULT_READY);

    return w;
}

/*
 * ------------------------------------------------------------------------
 * Name: CreateButtonGadget
 *
 * Description:
 *
 * Return value:
 *
 *     Widget ID of the new button gadget.
 *
 */
static Widget 
CreateButtonGadget(Widget parent,
		   String label,
		   String name,
		   XmNavigationType nav_type)
{
    XmString label_string;
    Widget w;
    XmTakesDefaultTrait trait_default ;
    
    label_string = XmStringGenerate((XtPointer)label, (XmStringTag)NULL,
				    XmMULTIBYTE_TEXT, (XmStringTag)NULL);
    w = XtVaCreateWidget(name,
			 xmPushButtonGadgetClass,
			 parent,
			 XmNlabelString, label_string,
			 XmNnavigationType, nav_type,
			 NULL);

    if(label_string) XmStringFree(label_string);

    trait_default = (XmTakesDefaultTrait)
	XmeTraitGet((XtPointer)XtClass(w), XmQTtakesDefault) ;
    if(trait_default) 
	trait_default->showAsDefault(w, XmDEFAULT_READY);

    return w;
}

/*
 * ------------------------------------------------------------------------
 * Name: CreateLabelGadget
 *
 * Description:
 *
 * Return value:
 *
 *     Widget ID of the new label gadget.
 *
 */
static Widget 
CreateLabelGadget(
		  Widget parent,
		  String label,
		  String name)
{
    XmString label_string;
    Widget w;
    
    label_string = XmStringGenerate((XtPointer)label, (XmStringTag)NULL,
				    XmMULTIBYTE_TEXT, (XmStringTag)NULL);
    w = XtVaCreateWidget(name,
			 xmLabelGadgetClass,
			 parent,
			 XmNlabelString, label_string,
			 NULL);

    if(label_string) XmStringFree(label_string);

    return w;
}

/*
 * ------------------------------------------------------------------------
 * Name: CreateModalPrinterSpec
 *
 * Arguments:
 *
 *     name_mode
 *         A valid X Printer Specifier Display Mode (see the
 *         XpPrinterNameMode resource definition).
 *
 *     full_printer_name
 *         The fully qualified printer name
 *
 * Description:
 *
 *     Creates an X Printer Specifier appropriate to an X Printer
 *     Specifier Display Mode, given a full printer name
 *
 * Return value:
 *
 *     A newly allocated string containing the resulting X Printer
 *     specifier. It is the resposibility of the caller to free the
 *     memory using XtFree(1).
 *
 *     NULL is returned if:
 * 	* the passed printer_name is NULL
 * 	* if memory allocation fails.
 *
 */
static String
CreateModalPrinterSpec(
		       XtEnum name_mode,
		       const String full_printer_name)
{
    String modal_printer_spec;
    String printer_name;
    String display_spec;
    /*
     * check for NULL passed parms
     */
    if(full_printer_name == (String)NULL)
    {
	return (String)NULL;
    }
    /*
     * break the fully qualified printer name into printer and display
     * name components
     */
    _DtPrintParseXPrinterSpecifier(full_printer_name,
				   &printer_name, &display_spec);
    /*
     * if display mode is SHORT, just the printer name will do
     */
    if(name_mode == DtSHORT_NAME)
    {
	modal_printer_spec = XtNewString(printer_name);
    }
    else
    {
	String host_name;
	DtPrintSpecNet spec_net;
	int display_num;
	int screen_num;
	/*
	 * break up the display specifier
	 */
	_DtPrintParseXDisplaySpecifier(display_spec, &host_name,
				       &spec_net, &display_num, &screen_num);
	/*
	 * create the modal specifier
	 */
	switch(name_mode)
	{
	case DtMEDIUM_NAME:
	    modal_printer_spec =
		_DtPrintCreateXPrinterSpecifier(printer_name, host_name,
						DtPRINT_NET_UNSPECIFIED,
						-1, -1);
	    break;

	case DtLONG_NAME:
	    modal_printer_spec =
		_DtPrintCreateXPrinterSpecifier(printer_name, host_name,
						spec_net,
						display_num, screen_num);
	    break;
	    
	default:
	    /*
	     * invalid name mode; default to short name
	     */
	    modal_printer_spec = XtNewString(printer_name);
	    break;
	}
	XtFree(host_name);
    }
    /*
     * clean up and return
     */
    XtFree(printer_name);
    XtFree(display_spec);
    return modal_printer_spec;
}

/*
 * ------------------------------------------------------------------------
 * Name: DeleteChild - Composite class method
 *
 * Description:
 *
 *     Remove child from setup box widget
 *
 * Return value:
 *
 *     None.
 *
 */
static void 
DeleteChild(
	    Widget child)
{   
    DtPrintSetupBoxWidget psub ;
    XtWidgetProc delete_child;

    if(XtIsRectObj(child))
    {	
	psub = (DtPrintSetupBoxWidget) XtParent(child);
	/*
	 * Clear widget fields (BulletinBoard does default and cancel).
	 */
	if(child == PSUB_BottomSeparator(psub))
	{
	    PSUB_BottomSeparator(psub) = NULL;
	} 
	else if(child == PSUB_ButtonSeparator(psub))
	{
	    PSUB_ButtonSeparator(psub) = NULL;
	} 
	else if(child == PSUB_DescriptionLabel(psub))
	{
	    PSUB_DescriptionLabel(psub) = NULL;
	} 
	else if(child == PSUB_Description(psub))
	{
	    PSUB_Description(psub) = NULL;
	} 
	else if(child == PSUB_PrinterNameLabel(psub))
	{
	    PSUB_PrinterNameLabel(psub) = NULL;
	} 
	else if(child == PSUB_CopiesControl(psub))
	{
	    PSUB_CopiesControl(psub) = NULL;
	    PSUB_CopiesSpinBox(psub) = NULL;
	} 
	else if(child == PSUB_TopWorkArea(psub))
	{
	    PSUB_TopWorkArea(psub) = NULL;
	} 
	else if(child == PSUB_BottomWorkArea(psub))
	{
	    PSUB_BottomWorkArea(psub) = NULL;
	} 
	else if(child == PSUB_PrintButton(psub))
	{
	    PSUB_PrintButton(psub) = NULL;
	} 
	else if(child == PSUB_PrinterInfoButton(psub))
	{
	    PSUB_PrinterInfoButton(psub) = NULL;
	}
	else if(child == PSUB_PrinterNameCombo(psub))
	{
	    PSUB_PrinterNameCombo(psub) = NULL;
	    PSUB_PrinterNameText(psub) = NULL;
	}
	else if(child == PSUB_SetupButton(psub))
	{
	    PSUB_SetupButton(psub) = NULL;
	} 
	else if(child == PSUB_HelpButton(psub))
	{
	    PSUB_HelpButton(psub) = NULL;
	}
	else if(child == PSUB_TopSeparator(psub))
	{
	    PSUB_TopSeparator(psub) = NULL;
	}
	else if(child == PSUB_DestinationRadioBox(psub))
	{
	    PSUB_DestinationRadioBox(psub) = NULL;
	}
	else if(child == PSUB_FileNameLabel(psub))
	{
	    PSUB_FileNameLabel(psub) = NULL;
	}
	else if(child == PSUB_FileNameText(psub))
	{
	    PSUB_FileNameText(psub) = NULL;
	}
	else if(child == PSUB_SelectFileButton(psub))
	{
	    PSUB_SelectFileButton(psub) = NULL;
	}
	else if(child == PSUB_SelectPrinterButton(psub))
	{
	    PSUB_SelectPrinterButton(psub) = NULL;
	}
    }
    _DtPrintProcessLock();
    delete_child = ((XmBulletinBoardWidgetClass)xmBulletinBoardWidgetClass)
      ->composite_class.delete_child;
    _DtPrintProcessUnlock();
    (*delete_child)(child) ;
}

/*
 * ------------------------------------------------------------------------
 * Name: DestinationChangedCallback
 *
 * Description:
 *
 *     Process a change in the print destination generated by the GUI user.
 *
 * Return value:
 *
 *     None.
 *
 */
static void 
DestinationChangedCallback(
			   Widget w,
			   XtPointer client_data,
			   XtPointer call_data)
{
    DtPrintSetupBoxWidget psub = (DtPrintSetupBoxWidget)client_data;
    XmToggleButtonCallbackStruct* cbs =
	(XmToggleButtonCallbackStruct*)call_data;

    if(cbs->set)
    {
	XtPointer user_data;
	/*
	 * set the print destination according to the selected radio
	 * button
	 */
	XtVaGetValues(w, XmNuserData, &user_data, NULL);
	PSUB_PrintDestination(psub) = (XtEnum) (intptr_t) user_data;
	/*
	 * Update the print setup box controls sensitivity accordingly
	 */
	if(PSUB_SelectFileButton(psub))
	    XtSetSensitive(PSUB_SelectFileButton(psub),
			   PSUB_PrintDestination(psub) == DtPRINT_TO_FILE);
	if(PSUB_FileNameLabel(psub))
	    XtSetSensitive(PSUB_FileNameLabel(psub),
			   PSUB_PrintDestination(psub) == DtPRINT_TO_FILE);
	if(PSUB_FileNameText(psub))
	    XtSetSensitive(PSUB_FileNameText(psub),
			   PSUB_PrintDestination(psub) == DtPRINT_TO_FILE);
	if(PSUB_CopyCountSupported(psub))
	{
	    if(PSUB_CopiesSpinBox(psub))
		XtSetSensitive(PSUB_CopiesSpinBox(psub),
			       PSUB_PrintDestination(psub) == DtPRINT_TO_PRINTER);
	    if(PSUB_CopiesControl(psub))
	    {
		Widget copies_label =
		    XtNameToWidget(PSUB_CopiesControl(psub), "CopiesLabel");
		XtSetSensitive(copies_label,
			       PSUB_PrintDestination(psub) == DtPRINT_TO_PRINTER);
	    }
	}
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: Destroy - Core class method
 *
 * Description:
 *
 *     Clean up resources allocated by the PrintSetupBox widget.
 *
 * Return Value:
 *
 *     None.
 */
static void
Destroy(
	Widget w)
{
    DtPrintSetupBoxWidget psub = (DtPrintSetupBoxWidget)w;
    /*
     * close the Xp connection
     */
    if(PSUB_PrintSetupMode(psub) == DtPRINT_SETUP_XP
       &&
       PSUB_Display(psub) != (Display*)NULL)
    {
	ClosePrintConnection(psub, PSUB_PrinterName(psub), False);
    }
    /*
     * synthetic resources
     */
    XtFree(PSUB_FileName(psub));
    XtFree(PSUB_PrinterName(psub));
    /*
     * internal data
     */
    XtFree(PSUB_ModalPrinterSpec(psub));
}

/*
 * ------------------------------------------------------------------------
 * Name: EstablishPrinter
 *
 * Description:
 *
 *
 * Return Value:
 *
 *     DtPRINT_SUCCESS
 *         if a valid printer has been established
 *         (i.e. for Xp, a connection has been set up)
 *
 *     DtPRINT_FAILURE
 *         if the printer could not be established
 *
 *     DtPRINT_BAD_PARM
 *         if any passed parameter is invalid
 *
 */
static XtEnum
EstablishPrinter(
		 DtPrintSetupBoxWidget psub,
		 DtPrintSetupData* psd)
{
    switch(PSUB_VerifyPrinterState(psub))
    {
    case DtPRINT_NOT_VERIFIED:
	/*
	 * if there's a printer connection timer waiting, remove it
	 * to avoid conflict
	 */
	if(PSUB_TimeoutId(psub) != (XtIntervalId)NULL)
	{
	    XtRemoveTimeOut(PSUB_TimeoutId(psub));
	    PSUB_TimeoutId(psub) = (XtIntervalId)NULL;
	}
	/*
	 * call the verify printer proc for the new printer
	 */
	if(PSUB_VerifyPrinterProc(psub) != NULL)
	{
	    int status;
	    
	    psd->printer_name = PSUB_PrinterName(psub);
	    PSUB_VerifyPrinterState(psub) = DtPRINT_IN_VERIFY;
	    status = (*PSUB_VerifyPrinterProc(psub))((Widget)psub, psd);
	    if(status != DtPRINT_SUCCESS)
	    {
		/*
		 * the printer isn't valid - set the verify printer state
		 * back to "not verified" and return
		 */
		PSUB_VerifyPrinterState(psub) = DtPRINT_NOT_VERIFIED;
		return status;
	    }
	    PSUB_VerifyPrinterState(psub) = DtPRINT_VERIFIED;
	    if(PSUB_PrintSetupMode(psub) == DtPRINT_SETUP_XP)
	    {
		/*
		 * set the new display and context in the widget
		 */
		PSUB_Display(psub) = psd->print_display;
		PSUB_Context(psub) = psd->print_context;
		/*
		 * initialize the display for use with Xt
		 */
		{
		    String *argv = NULL, *argv_copy;
		    int argc = 0, i;
		    String video_name, video_class;
		    Widget video_widget = (Widget)psub;
		    XtAppContext app_context;
		    /*
		     * get to the video app root shell 
		     */
		    while((Widget)NULL != video_widget)
		    {
			if(XtIsApplicationShell(video_widget))
			    break;
			else
			    video_widget = XtParent(video_widget);
		    }
		    if((Widget)NULL == video_widget)
			video_widget = (Widget)psub;
		    /*
		     * if not an applicationShell, argc/argv will stay
		     * NULL in the GetValues call, no need to check
		     * explicitly
		     */
		    XtVaGetValues(video_widget,
				  XmNargc, &argc, XmNargv, &argv,
				  NULL);

		    /* copy argv, modified by XtDisplayInitialize below */
		    argv_copy =
			(String *) XtMalloc(sizeof(String) * (argc + 1));
		    for (i = 0; i < argc; i++) {
			argv_copy[i] = argv[i];
		    }			
		    argv_copy[argc] = NULL;

		    /*
		     * now add the print display in our app context, so
		     * that event dispatching works
		     */
		    XtGetApplicationNameAndClass(XtDisplay(video_widget),
						 &video_name, &video_class);
		    app_context = XtWidgetToApplicationContext(video_widget);
		    XtDisplayInitialize(app_context,
					PSUB_Display(psub),
					video_name, video_class,
					PSUB_Options(psub),
					PSUB_OptionCount(psub),
					&argc, argv_copy);

		    XtFree((char *) argv_copy);
		}
		/*
		 * pick up attributes for the new printer
		 */
		GetPrintAttributes(psub);
	    }
	}
	else
	{
	    PSUB_VerifyPrinterState(psub) = DtPRINT_VERIFIED;
	}
	/*
	 * no break
	 */
	
    case DtPRINT_VERIFIED:
	/*
	 * fill out current printer info in setup data
	 */
	psd->printer_name = PSUB_PrinterName(psub);
	if(PSUB_PrintSetupMode(psub) == DtPRINT_SETUP_XP)
	{
	    psd->print_display = PSUB_Display(psub);
	    psd->print_context = PSUB_Context(psub);
	}
	break;

    case DtPRINT_IN_VERIFY:
	/*
	 * EstablishPrinter should not be called recursively
	 */
	return DtPRINT_BAD_PARM;
	break;
    }
    
    return DtPRINT_SUCCESS;
}

/*
 * ------------------------------------------------------------------------
 * Name: GetPrintAttributes
 *
 * Description:
 *
 *     Get printing attributes relevant to the PrintSetupBox from the
 *     current print context.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
GetPrintAttributes(
		   DtPrintSetupBoxWidget psub)
{
    char* attr_value;
    XTextProperty text_prop;
    char** list;
    int count;
    XmString description = (XmString)NULL;
    char* ptr;
    /*
     * get the printer description, and set the description field
     */
    attr_value = XpGetOneAttribute(PSUB_Display(psub),
				   PSUB_Context(psub),
				   XPPrinterAttr,
				   XpATTR_DESCRIPTOR);
    if(attr_value != (char*)NULL)
    {
	/*
	 * convert the description from COMPOUND_TEXT into the
	 * codeset of the current locale
	 */
	text_prop.value = (unsigned char*)attr_value;
	text_prop.encoding =
	    XInternAtom(XtDisplay((Widget)psub), "COMPOUND_TEXT", False);
	text_prop.format = 8;
	text_prop.nitems = strlen((char*)text_prop.value);
	if(Success ==
	   XmbTextPropertyToTextList(XtDisplay((Widget)psub),
				     &text_prop, &list, &count))
	{
	    if(count > 0)
	    {
		if((String)NULL != list[0] && '\0' != *(list[0]))
		{
		    /*
		     * chop the description after the 1st line
		     */
		    char* ptr = Dt_strchr(list[0], '\n');
		    if((char*)NULL != ptr)
			*ptr = '\0';
		}
		description =
		    XmStringGenerate((XtPointer)list[0], (XmStringTag)NULL,
				     XmMULTIBYTE_TEXT, (XmStringTag)NULL);
	    }
	    XFreeStringList(list);    
	}
	else
	{
	    XmeWarning((Widget)psub, WARN_CT_CONVERSION);
	}
	XFree(attr_value);
    }
    /*
     * update the printer description
     */
    if((XmString)NULL == description)
	description = XmStringGenerate((XtPointer)"", (XmStringTag)NULL,
				       XmMULTIBYTE_TEXT, (XmStringTag)NULL);
    UpdateString(PSUB_Description(psub), 
		 description,
		 PSUB_StringDirection(psub));
    XmStringFree(description);
    /*
     * determine if copy-count is supported
     */
    PSUB_CopyCountSupported(psub) = False;
    attr_value = XpGetOneAttribute(PSUB_Display(psub), PSUB_Context(psub),
				   XPPrinterAttr, XpATTR_DOC_ATTRS_SUPPORTED);
    if(attr_value != (char*)NULL)
    {
	int token_len;
	for(ptr = attr_value + SpanWhitespace(attr_value);
	    *ptr != '\0';
	    ptr += SpanWhitespace(ptr+=token_len))
	{
	    token_len = SpanNonWhitespace(ptr);
	    if(0 == strncmp(ptr, XpATTR_COPY_COUNT, token_len))
	    {
		PSUB_CopyCountSupported(psub) = True;
		break;
	    }
	}
	XFree(attr_value);
    }
    if(!PSUB_CopyCountSupported(psub))
    {
	/*
	 * copy-count not supported; disable the copies control
	 */
	if(PSUB_CopiesSpinBox(psub))
	    XtSetSensitive(PSUB_CopiesSpinBox(psub), False);
	if(PSUB_CopiesControl(psub))
	{
	    Widget copies_label =
		XtNameToWidget(PSUB_CopiesControl(psub), "CopiesLabel");
	    XtSetSensitive(copies_label, False);
	}
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: Initialize - Core class method
 *
 * Description:
 *
 *     Create a PrintSetupBox widget instance.
 *
 * Return value:
 *
 *     None.
 *
 */
static void 
Initialize(
	   Widget rw,
	   Widget nw,
	   ArgList args,
	   Cardinal *num_args)
{
    DtPrintSetupBoxWidget new_w = (DtPrintSetupBoxWidget) nw ;

    /*
     * Validate the Print Setup Mode
     */
    ValidatePrintSetupMode(new_w, (DtPrintSetupBoxWidget)NULL);
    /*
     * set the Xp-mode default resource procedure if the resource is
     * unspecified
     */
    if(PSUB_PrintSetupMode(new_w) == DtPRINT_SETUP_XP)
    {
	if(PSUB_PrinterInfoProc(new_w) == (DtPrintSetupProc)DtUNSPECIFIED)
	    PSUB_PrinterInfoProc(new_w) = _DtPrintSetupBoxXPrinterInfoProc;
	if(PSUB_SelectPrinterProc(new_w) == (DtPrintSetupProc)DtUNSPECIFIED)
	    PSUB_SelectPrinterProc(new_w) = _DtPrintSetupBoxSelectXPrinterProc;
	if(PSUB_VerifyPrinterProc(new_w) == (DtPrintSetupProc)DtUNSPECIFIED)
	    PSUB_VerifyPrinterProc(new_w) = _DtPrintSetupBoxVerifyXPrinterProc;
    }
    else
    {
	/*
	 * for the plain setup mode, unspecified procedure resources
	 * default to NULL
	 */
	if(PSUB_PrinterInfoProc(new_w) == (DtPrintSetupProc)DtUNSPECIFIED)
	    PSUB_PrinterInfoProc(new_w) = (DtPrintSetupProc)NULL;
	if(PSUB_SelectPrinterProc(new_w) == (DtPrintSetupProc)DtUNSPECIFIED)
	    PSUB_SelectPrinterProc(new_w) = (DtPrintSetupProc)NULL;
	if(PSUB_VerifyPrinterProc(new_w) == (DtPrintSetupProc)DtUNSPECIFIED)
	    PSUB_VerifyPrinterProc(new_w) = (DtPrintSetupProc)NULL;
    }
    /*
     * initialize internal flags
     */
    PSUB_VerifyPrinterState(new_w) = DtPRINT_NOT_VERIFIED;
    PSUB_CopyCountSupported(new_w) = True;
    /*
     * initialize internal data
     */
    PSUB_ModalPrinterSpec(new_w) = (String)NULL;
    PSUB_Display(new_w) = (Display*)NULL;
    PSUB_Context(new_w) = (XPContext)NULL;
    PSUB_TimeoutId(new_w) = (XtIntervalId)NULL;
    /*
     * retrieve the XpPrinterNameMode application resource for this
     * widget
     */
    {
	XtResource res_struct;
	res_struct.resource_name = "xpPrinterNameMode";
	res_struct.resource_class = "XpPrinterNameMode";
	res_struct.resource_type =  XmREnum;
	res_struct.resource_size = sizeof(XtEnum);
	res_struct.resource_offset = 0;
	res_struct.default_type = XmRImmediate;
	res_struct.default_addr = (XtPointer)DtSHORT_NAME;
	XtGetApplicationResources(nw,
				  (XtPointer)&PSUB_XpPrinterNameMode(new_w),
				  &res_struct, 1, (ArgList)NULL, 0);
    }
    /*
     * Initialize work area children variables
     */
    PSUB_TopWorkArea(new_w) = (Widget)NULL;
    PSUB_BottomWorkArea(new_w) = (Widget)NULL;
    ValidateWorkAreaLocation(new_w, (DtPrintSetupBoxWidget)NULL);
    /*
     * Initialize default resource procedures private data
     */
    _DtPrintDefProcInitialize((Widget)new_w);
    /*
     * Create child widgets.
     */
    PSUB_AddingSelWidgets(new_w) = True;

    _DtPrintSetupBoxCreateTopSeparator(new_w);

    _DtPrintSetupBoxCreateDescriptionLabel(new_w);
    _DtPrintSetupBoxCreateDescription(new_w);
    _DtPrintSetupBoxCreatePrinterInfoButton (new_w);
    
    _DtPrintSetupBoxCreatePrinterNameLabel(new_w);
    _DtPrintSetupBoxCreatePrinterNameCombo(new_w);
    _DtPrintSetupBoxCreateSelectPrinterButton(new_w);

    _DtPrintSetupBoxCreateFileNameLabel(new_w);
    _DtPrintSetupBoxCreateFileNameText(new_w);
    _DtPrintSetupBoxCreateSelectFileButton(new_w);

    _DtPrintSetupBoxCreateDestinationRadioBox(new_w);
    _DtPrintSetupBoxCreateCopiesControl(new_w);

    _DtPrintSetupBoxCreateBottomSeparator(new_w);

    _DtPrintSetupBoxCreateButtonSeparator (new_w);
    _DtPrintSetupBoxCreatePrintButton (new_w);
    _DtPrintSetupBoxCreateSetupButton (new_w);
    _DtPrintSetupBoxCreateCancelButton (new_w);
    _DtPrintSetupBoxCreateHelpButton (new_w);
    /*
     * Validate/update copies spin box
     */
    SynthSetCopies(new_w, (DtPrintSetupBoxWidget)NULL);
    /*
     * Update Printer Description
     */
    if((XmString)NULL != PSUB_DescriptionString(new_w))
    {
	UpdateString(PSUB_Description(new_w), 
		     PSUB_DescriptionString(new_w),
		     PSUB_StringDirection(new_w));
	PSUB_DescriptionString(new_w) = NULL ;
    }
    /*
     * Initialize printer information
     */
    SynthSetPrinterName(new_w,
			XtNewString(PSUB_PrinterName(new_w)),
			(String)NULL);
    /*
     * Validate print destination / update print to file checkbox
     */
    SynthSetPrintDestination(new_w, (DtPrintSetupBoxWidget)NULL);
    /*
     * Update the file name text box
     */
    SynthSetFileName(new_w, (DtPrintSetupBoxWidget)NULL);
    /*
     * specify the Print button as the default button
     */
    BB_DefaultButton(new_w) = PSUB_PrintButton(new_w) ;
    _XmBulletinBoardSetDynDefaultButton((Widget) new_w,
					BB_DefaultButton(new_w)) ;

    PSUB_AddingSelWidgets(new_w) = False;

    XtManageChildren (new_w->composite.children, 
		      new_w->composite.num_children) ;

}

/*
 * ------------------------------------------------------------------------
 * Name: InsertChild - Composite class method
 *
 * Description:
 *
 *     The Print Setup widget supports up to TWO work area children. This
 *     routine handles adding a child to the Print Setup widget.
 *
 * Return value:
 *
 *     None.
 *
 */
static void 
InsertChild(
	    Widget child)
{   
    DtPrintSetupBoxWidget psub ;
    XtWidgetProc insert_child;
    /*
     * Use the dialog class insert proc to do all the dirty work
     */
    _DtPrintProcessLock();
    insert_child = ((XmBulletinBoardWidgetClass)xmBulletinBoardWidgetClass)
      ->composite_class.insert_child;
    _DtPrintProcessUnlock();
    (*insert_child)(child) ;

    if(!XtIsRectObj(child))
    {
	return ;
    } 
    psub = (DtPrintSetupBoxWidget) XtParent(child) ;
    /*
     * check if this child is to be the one of the work areas
     */
    if(!PSUB_AddingSelWidgets(psub)
       && !(XmIsRowColumn(child)
	    && ((XmRowColumnWidget)child)->row_column.type == XmMENU_BAR
	    )
       && !XtIsShell(child)
       && !IsButton(child))
    {
	if(PSUB_TopWorkArea(psub) == (Widget)NULL
	   &&
	   (PSUB_WorkAreaLocation(psub) == DtWORK_AREA_TOP
	    ||
	    PSUB_WorkAreaLocation(psub) == DtWORK_AREA_TOP_AND_BOTTOM)
	   )
	{
	    PSUB_TopWorkArea(psub) = child;
	}
	else if(PSUB_BottomWorkArea(psub) == (Widget)NULL
		&&
		(PSUB_WorkAreaLocation(psub) == DtWORK_AREA_BOTTOM
		 ||
		 PSUB_WorkAreaLocation(psub) == DtWORK_AREA_TOP_AND_BOTTOM)
		)
	{
	    PSUB_BottomWorkArea(psub) = child;
	}
    } 
}

static Boolean
IsSetupRequired(
		DtPrintSetupBoxWidget psub)
{
    char* setup_proviso;
    Boolean required = False;

    setup_proviso = XpGetOneAttribute(PSUB_Display(psub),
				      PSUB_Context(psub),
				      XPPrinterAttr,
				      XpATTR_SETUP_PROVISO);
    if((char*)NULL != setup_proviso)
    {
	char* ptr;
	int token_len;

	ptr = setup_proviso + SpanWhitespace(setup_proviso);
	token_len = SpanNonWhitespace(ptr);
	if(token_len && 0 == strncmp(ptr, "xp-setup-mandatory", token_len))
	{
	    char* setup_state;
	    /*
	     * setup is mandatory; check to see if it has been performed
	     */
	    setup_state = XpGetOneAttribute(PSUB_Display(psub),
					    PSUB_Context(psub),
					    XPJobAttr,
					    XpATTR_SETUP_STATE);
	    if((char*)NULL == setup_state)
		required = True;
	    else
	    {
		ptr = setup_state + SpanWhitespace(setup_state);
		token_len = SpanNonWhitespace(ptr);
		if(token_len)
		{
		    /*
		     * if the value of the setup state attribute is
		     * xp-setup-ok then setup is not required
		     */
		    if(0 != strncmp(ptr, "xp-setup-ok", token_len))
			required = True;
		}
		else
		    required = True;
		XFree(setup_state);
	    }
	}
	XFree(setup_proviso);
    }
    return required;
}

/*
 * ------------------------------------------------------------------------
 * Name: MenuBarFixUp
 *
 * Description:
 *
 *     This routine is a fixup routine which can be used for rows which
 *     consist of a single MenuBar RowColumn.  The effect of this routine
 *     is to have the RowColumn ignore the margin width and height.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
MenuBarFixUp(
	     XmGeoMatrix geoSpec,
	     int action,
	     XmGeoMajorLayout layoutPtr,
	     XmKidGeometry rowPtr)
{
    register Dimension	     marginW ;
    register Dimension	     marginH ;
    register Dimension	     twoMarginW ;

    marginW = geoSpec->margin_w ;
    twoMarginW = (marginW << 1) ;
    marginH = geoSpec->margin_h ;

    switch(    action	 )
    {
	case XmGEO_PRE_SET:
	{
	    rowPtr->box.x -= marginW ;
	    rowPtr->box.width += twoMarginW ;
	    rowPtr->box.y -= marginH ;
	    break ;
	}
	default:
	{
	    if(	   rowPtr->box.width > twoMarginW    )
	    {
		/*
		 * Avoid subtracting a margin from box width which would
		 * result in underflow.
		 */
		rowPtr->box.x += marginW ;
		rowPtr->box.width -= twoMarginW ;
	    }
	    if(	   action == XmGET_PREFERRED_SIZE    )
	    {
		/*
		 * Set width to some small value so it does not effect
		 * total width of matrix.
		 */
		rowPtr->box.width = 1 ;
	    }
	    break ;
	}
    }
} 

/*
 * ------------------------------------------------------------------------
 * Name: PrintSetupBoxCallback
 *
 * Description:
 *
 *     Call the callbacks for a PrintSetupBox button.
 *
 * Return value:
 *
 *     None.
 *
 */
static void 
PrintSetupBoxCallback(
		      Widget w,
		      XtPointer client_data,
		      XtPointer call_data)
{
    DtPrintSetupBoxChild which_button = (DtPrintSetupBoxChild)client_data;
    DtPrintSetupBoxWidget psub = (DtPrintSetupBoxWidget)XtParent(w);
    XmAnyCallbackStruct* callback = (XmAnyCallbackStruct*)call_data;
    DtPrintSetupCallbackStruct cbs;
    DtPrintSetupData psd;

    memset(&cbs, 0, sizeof(DtPrintSetupCallbackStruct));
    memset(&psd, 0, sizeof(DtPrintSetupData));
    cbs.print_data = &psd;
    cbs.event = callback->event;

    /*
     * we want to see any error messages that may pop up when
     * establishing the printer connection
     */
    psd.messages_hint = DtPRINT_HINT_MESSAGES_OK;
    
    switch(which_button)
    {
    case DtPRINT_CANCEL_BUTTON:
	cbs.reason = DtPRINT_CR_CANCEL;
	XtCallCallbackList((Widget)psub,
			   PSUB_CancelCallback(psub),
			   (XtPointer)&cbs);
	break;

    case DtPRINT_FILES_BUTTON:
	if(PSUB_SelectFileProc(psub))
	{
	    /*
	     * setup the destination info
	     */
	    SetPSDDestination(psub, &psd);
	    /*
	     * call select file proc
	     */
	    (*PSUB_SelectFileProc(psub))((Widget)psub, &psd);
	}
	break;

    case DtPRINT_HELP_BUTTON:
	/*
	 * Invoke the help system. 
	 */
	_XmManagerHelp((Widget)psub, callback->event, NULL, NULL) ;
	break;

    case DtPRINT_INFO_BUTTON:
	if(PSUB_PrinterInfoProc(psub))
	{
	    /*
	     * pick up the printer name from the widget's Printer Name text
	     * field
	     */
	    SynthGetPrinterName(psub);
	    /*
	     * establish a connection to the X printer
	     */
	    if(EstablishPrinter(psub, &psd) == DtPRINT_SUCCESS)
	    {
		/*
		 * For X printing, set printing attributes
		 */
		if(PSUB_PrintSetupMode(psub) == DtPRINT_SETUP_XP)
		{
		    SetPrintAttributes(psub);
		}
		/*
		 * call printer info proc
		 */
		(*PSUB_PrinterInfoProc(psub))((Widget)psub, &psd);
	    }
	}
	break;
	
    case DtPRINT_PRINT_BUTTON:
	/*
	 * pick up the printer name from the widget's Printer Name text
	 * field
	 */
	SynthGetPrinterName(psub);
	/*
	 * establish a connection to the X printer
	 */
	if(EstablishPrinter(psub, &psd) == DtPRINT_SUCCESS)
	{
	    /*
	     * For X printing, set printing attributes
	     */
	    if(PSUB_PrintSetupMode(psub) == DtPRINT_SETUP_XP)
	    {
		SetPrintAttributes(psub);
	    }
	    /*
	     * setup the destination info
	     */
	    SetPSDDestination(psub, &psd);

	    if(PSUB_PrintSetupMode(psub) == DtPRINT_SETUP_XP
	       &&
	       IsSetupRequired(psub))
	    {
		/*
		 * setup is needed; call the setup callback instead
		 */
		cbs.reason = DtPRINT_CR_SETUP;
		XtCallCallbackList((Widget)psub,
				   PSUB_SetupCallback(psub),
				   (XtPointer)&cbs);
	    }
	    else
	    {
		/*
		 * The printer name is OK. Go ahead and unmanage the print
		 * setup box if the auto-unmanage resource is set.
		 */
		if(PSUB_AutoUnmanage(psub) && PSUB_Shell(psub))
		    XtUnmanageChild((Widget)psub);
		/*
		 * call the Print callback list
		 */
		cbs.reason = DtPRINT_CR_PRINT;
		XtCallCallbackList((Widget)psub,
				   PSUB_PrintCallback(psub),
				   (XtPointer)&cbs);
	    }
	}
	break;

    case DtPRINT_PRINTERS_BUTTON:
	if(PSUB_SelectPrinterProc(psub))
	{
	    /*
	     * call the printer selection proc
	     */
	    psd.printer_name = PSUB_PrinterName(psub);
	    (*PSUB_SelectPrinterProc(psub))((Widget)psub, &psd);
	}
	break;

    case DtPRINT_SETUP_BUTTON:
	/*
	 * pick up the printer name from the widget's Printer Name text
	 * field
	 */
	SynthGetPrinterName(psub);
	/*
	 * establish a connection to the X printer
	 */
	if(EstablishPrinter(psub, &psd) == DtPRINT_SUCCESS)
	{
	    /*
	     * For X printing, set printing attributes
	     */
	    if(PSUB_PrintSetupMode(psub) == DtPRINT_SETUP_XP)
	    {
		SetPrintAttributes(psub);
	    }
	    /*
	     * setup the destination info
	     */
	    SetPSDDestination(psub, &psd);
	    /*
	     * Call setup callback
	     */
	    cbs.reason = DtPRINT_CR_SETUP;
	    XtCallCallbackList((Widget)psub,
			       PSUB_SetupCallback(psub),
			       (XtPointer)&cbs);
	}
	break;
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: SeparatorFixUp
 *
 * Description:
 *
 *     This routine is a fixup routine which can be used for rows which
 *     consist of a single separator widget.  The effect of this routine
 *     is to have the separator ignore the margin width.
 *
 * Return value:
 *
 *     None.
 *
 */
static void 
SeparatorFixUp(
	     XmGeoMatrix geoSpec,
	     int action,
	     XmGeoMajorLayout layoutPtr,
	     XmKidGeometry rowPtr )
{
    register Dimension	     marginW ;
    register Dimension	     twoMarginW ;

    marginW = geoSpec->margin_w ;
    twoMarginW = (marginW << 1) ;

    switch(    action	 )
    {	
	case XmGEO_PRE_SET:
	{
	    rowPtr->box.x -= marginW ;
	    rowPtr->box.width += twoMarginW ;
	    break ;
	} 
	default:
	{
	    if(	   rowPtr->box.width > twoMarginW    )
	    {	
		/*
		 * Avoid subtracting a margin from box width which would
		 * result in underflow.
		 */
		rowPtr->box.x += marginW ;
		rowPtr->box.width -= twoMarginW ;
	    } 
	    if(	   action == XmGET_PREFERRED_SIZE    )
	    {	
		/*
		 * Set width to some small value so it does not 
		 * effect total width of matrix.
		 */
		rowPtr->box.width = 1 ;
	    } 
	    break ;
	} 
    } 
}

/*
 * ------------------------------------------------------------------------
 * Name: SetNewPrinterName
 *
 * Description:
 *
 *     For Xp, if there is an existing open Xp connection, the
 *     connection is closed, and the DtNclosePrintDisplayCallback list is
 *     called using old_printer_name.
 *
 *     old_printer_name is then freed using XtFree.
 *
 *     The printer_name instance variable is set to new_printer_name
 *     which must point to memory previously allocated using one of the
 *     Xt memory allocation functions.
 *
 *     This function unsets the printer name verified flag.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
SetNewPrinterName(
		  DtPrintSetupBoxWidget psub,
		  String new_printer_name,
		  String old_printer_name)
{
    /*
     * close the existing Xp connection
     */
    if(PSUB_PrintSetupMode(psub) == DtPRINT_SETUP_XP
       &&
       PSUB_Display(psub) != (Display*)NULL)
    {
	ClosePrintConnection(psub, old_printer_name, True);
    }
    /*
     * free the old printer name
     */
    XtFree(old_printer_name);
    /*
     * update the printer name instance data
     */
    PSUB_PrinterName(psub) = new_printer_name;
    /*
     * ensure the new printer name will be verified
     */
    if(PSUB_VerifyPrinterState(psub) == DtPRINT_VERIFIED)
	PSUB_VerifyPrinterState(psub) = DtPRINT_NOT_VERIFIED;
}

/*
 * ------------------------------------------------------------------------
 * Name: SetPrintAttributes
 *
 * Description:
 *
 *     Set printing attributes relevant to the PrintSetupBox into the
 *     current print context.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
SetPrintAttributes(
		   DtPrintSetupBoxWidget psub)
{
    /*
     * Set the copy-count document attribute in the current context
     */
    if((Widget)NULL != PSUB_CopiesSpinBox(psub))
    {
	char str[48];
	
	XtVaGetValues(PSUB_CopiesSpinBox(psub),
		      XmNposition, &PSUB_Copies(psub),
		      NULL);
	sprintf(str, "*%s: %d\n", XpATTR_COPY_COUNT, PSUB_Copies(psub));
	XpSetAttributes(PSUB_Display(psub), PSUB_Context(psub),
			XPDocAttr, str, XPAttrMerge);
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: SetPSDDestination
 *
 * Description:
 *
 *     Set print destination information in the passed setup data
 *     struct for use in callbacks.
 *
 * Return value:
 *
 *     None.
 *
 */
static void SetPSDDestination(
			      DtPrintSetupBoxWidget psub,
			      DtPrintSetupData* psd)
{
    psd->destination = PSUB_PrintDestination(psub);
    switch(PSUB_PrintDestination(psub))
    {
    case DtPRINT_TO_PRINTER:
	psd->dest_info = PSUB_ModalPrinterSpec(psub);
	break;
		
    case DtPRINT_TO_FILE:
	SynthGetFileName(psub);
	psd->dest_info = PSUB_FileName(psub);
	break;
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: SetSyntheticResForChild
 *
 * Description:
 *
 *     Indicate to the synthetic resource mechanism that it should copy
 *     the resource value.
 *
 * Return value:
 *
 *     Always XmSYNTHETIC_LOAD.
 *
 */
static XmImportOperator
SetSyntheticResForChild(
			Widget widget,
			int offset,
			XtArgVal *value)
{ 
    return XmSYNTHETIC_LOAD;
}

/*
 * ------------------------------------------------------------------------
 * Name: SetValues - Core class method
 *
 * Description:
 *
 *     Update widget when values change.
 *
 * Return value:
 *
 *     Always False, because this widget will redraw itself.
 *
 */
static Boolean 
SetValues(
	  Widget cw,
	  Widget rw,
	  Widget nw,
	  ArgList args,
	  Cardinal *num_args)
{
    DtPrintSetupBoxWidget current = (DtPrintSetupBoxWidget) cw ;
    DtPrintSetupBoxWidget new_w = (DtPrintSetupBoxWidget) nw ;
    Boolean need_layout = False;

    BB_InSetValues(new_w) = True;
    /*
     * Validate the Print Setup Mode
     */
    if(PSUB_PrintSetupMode(new_w) != PSUB_PrintSetupMode(current))
	ValidatePrintSetupMode(new_w, current);
    /*
     * Validate work area location
     */
    if(PSUB_WorkAreaLocation(new_w) != PSUB_WorkAreaLocation(current))
	ValidateWorkAreaLocation(new_w, current);
    /*
     * Update Printer Description
     */
    if(PSUB_DescriptionString(new_w) != PSUB_DescriptionString(current))
    {
	UpdateString(PSUB_Description(new_w), 
		     PSUB_DescriptionString(new_w),
		     PSUB_StringDirection(new_w));
	PSUB_DescriptionString(new_w) = NULL ;
    }
    /*
     * Validate print destination / update print to file checkbox
     */
    if(PSUB_PrintDestination(new_w) != PSUB_PrintDestination(current))
	SynthSetPrintDestination(new_w, current);
    /*
     * Update the file name
     */
    if(PSUB_FileName(new_w) != PSUB_FileName(current))
	SynthSetFileName(new_w, current);
    /*
     * Validate/update copies spin box
     */
    if(PSUB_Copies(new_w) != PSUB_Copies(current))
	SynthSetCopies(new_w, current);
    /*
     * Update the Printer Name text field
     */
    if(PSUB_PrinterName(new_w) != PSUB_PrinterName(current))
    {
	if((PSUB_PrinterName(new_w) == NULL
	    ||
	    PSUB_PrinterName(current) == NULL)
	   ? True
	   : strcmp(PSUB_PrinterName(new_w), PSUB_PrinterName(current)) != 0)
	{
	    /*
	     * the printer names are different; set the new printer name
	     * into the instance structure and the printer name text
	     * field
	     */
	    SynthSetPrinterName(new_w,
				XtNewString(PSUB_PrinterName(new_w)),
				PSUB_PrinterName(current));
	}
	else
	{
	    /*
	     * printer names are identical; keep current
	     */
	    PSUB_PrinterName(new_w) = PSUB_PrinterName(current);
	}
    }
    if(PSUB_MinimizeButtons(new_w) != PSUB_MinimizeButtons(current))
    {
	need_layout = True;
    }
    /*
     * finished setting values
     */
    BB_InSetValues (new_w) = False;
    /*
     * If this is the instantiated class then do layout.
     */
    if(need_layout && XtClass(new_w) == dtPrintSetupBoxWidgetClass)
    {
	_XmBulletinBoardSizeUpdate((Widget) new_w) ;
    }
    return False;
}

/*
 * ------------------------------------------------------------------------
 * Name: SpanNonWhitespace
 *
 * Description:
 *
 *     Returns the length of the initial segment of the passed string
 *     that consists entirely of non-whitespace characters.
 *
 *
 */
static int
SpanNonWhitespace(const char* string)
{
    const char* ptr;
    for(ptr = string;
	*ptr != '\0' && !DtIsspace((char*)ptr);
	ptr = DtNextChar((char*)ptr));
    return ptr - string;
}

/*
 * ------------------------------------------------------------------------
 * Name: SpanWhitespace
 *
 * Description:
 *
 *     Returns the length of the initial segment of the passed string
 *     that consists entirely of whitespace characters.
 *
 *
 */
static int
SpanWhitespace(const char* string)
{
    const char* ptr;
    for(ptr = string;
	*ptr != '\0' && DtIsspace((char*)ptr);
	ptr = DtNextChar((char*)ptr));
    return ptr - string;
}

/*
 * ------------------------------------------------------------------------
 * Name: SynthGetFileName
 *
 * Description:
 *
 *     Updates the file_name element of the PrintSetupBox instance
 *     structure based on the current state of the File Name text field.
 *
 * Return Value:
 *
 *     None.
 *
 */
static void SynthGetFileName(
			     DtPrintSetupBoxWidget psub)
{
    /*
     * free the existing file name
     */
    XtFree(PSUB_FileName(psub));
    /*
     * set the file name widget instance value based on the File Name
     * text field value
     */
    if(PSUB_FileNameText(psub) != (Widget)NULL)
	XtVaGetValues((Widget)PSUB_FileNameText(psub),
		      XmNvalue, &PSUB_FileName(psub),
		      NULL);
    else
	PSUB_FileName(psub) = (String)NULL;
}

/*
 * ------------------------------------------------------------------------
 * Name: SynthGetPrinterName
 *
 * Description:
 *
 *     Updates the modal_printer_spec and the printer_name elements of
 *     the PrintSetupBox instance structure based on the current state of
 *     the Printer Name text field. The verify printer state will be set
 *     to "not verified" if the printer_name is updated.
 *
 * Return Value:
 *
 *     None.
 *
 */
static void
SynthGetPrinterName(
		    DtPrintSetupBoxWidget psub)
{
    Boolean printer_spec_changed = False;
    String previous_modal_spec;
    
    if(PSUB_PrinterNameText(psub) != (Widget)NULL)
    {	
	/*
	 * get the modal Printer Specifier from the Printer Name text
	 * field
	 */
	previous_modal_spec = PSUB_ModalPrinterSpec(psub);
 	XtVaGetValues((Widget)PSUB_PrinterNameText(psub),
		      XmNvalue, &PSUB_ModalPrinterSpec(psub),
		      NULL);
	/*
	 * determine if we have a new printer name
	 */
	switch(PSUB_PrintSetupMode(psub))
	{
	case DtPRINT_SETUP_PLAIN:
	    if((PSUB_ModalPrinterSpec(psub) == (String)NULL
		||
		PSUB_PrinterName(psub) == (String)NULL))
	    {
		printer_spec_changed = True;
	    }
	    else if(strcmp(PSUB_ModalPrinterSpec(psub), PSUB_PrinterName(psub))
		    != 0)
	    {
		printer_spec_changed = True;
	    }
	    break;

	case DtPRINT_SETUP_XP:
	    if(PSUB_PrinterName(psub) == (String)NULL
	       ||
	       PSUB_ModalPrinterSpec(psub) == (String)NULL
	       ||
	       (Display*)NULL == PSUB_Display(psub)
	       ||
	       strcmp(previous_modal_spec, PSUB_ModalPrinterSpec(psub)) != 0)
	    {
		printer_spec_changed = True;
	    }
	    break;
	}
	XtFree(previous_modal_spec);
    }
    if(printer_spec_changed)
    {
	SetNewPrinterName(psub,
			  XtNewString(PSUB_ModalPrinterSpec(psub)),/*new name*/
			  PSUB_PrinterName(psub));	         /* old name */
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: SynthSetCopies
 *
 * Description:
 *
 *     Validate the DtNcopies resource, and use it to update the Copies
 *     spin box.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
SynthSetCopies(
	       DtPrintSetupBoxWidget new_w,
	       DtPrintSetupBoxWidget current)
{
    if(PSUB_Copies(new_w) < 1 || PSUB_Copies(new_w) > MAX_COPIES)
    {
	XmeWarning((Widget)new_w, WARN_COPY_COUNT);
	if(current == (DtPrintSetupBoxWidget)NULL)
	    PSUB_Copies(new_w) = 1;
	else
	    PSUB_Copies(new_w) = PSUB_Copies(current);
    }
    if(PSUB_CopiesSpinBox(new_w) != (Widget)NULL)
	XtVaSetValues(PSUB_CopiesSpinBox(new_w),
		      XmNposition, PSUB_Copies(new_w),
		      NULL);
}

/*
 * ------------------------------------------------------------------------
 * Name: SynthSetFileName
 *
 * Description:
 *
 *     Update the file name text field.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
SynthSetFileName(
		 DtPrintSetupBoxWidget new_w,
		 DtPrintSetupBoxWidget current)
{
    /*
     * free the old file name
     */
    if(current != (DtPrintSetupBoxWidget)NULL)
    {
	XtFree(PSUB_FileName(current));
    }
    /*
     * make a copy of the new file name
     */
    PSUB_FileName(new_w) = XtNewString(PSUB_FileName(new_w));
    /*
     * update the file name text box
     */
    if(PSUB_FileNameText(new_w) != (Widget)NULL)
    {
	XmTextPosition last_position;
	XmTextFieldSetString(PSUB_FileNameText(new_w), PSUB_FileName(new_w));
	last_position = XmTextFieldGetLastPosition(PSUB_FileNameText(new_w));
	XmTextFieldSetInsertionPosition(PSUB_FileNameText(new_w),
					last_position);
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: SynthSetPrintDestination
 *
 * Description:
 *
 *     Validate the DtNprintDestination resource, and use it to update
 *     the print destination radio buttons.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
SynthSetPrintDestination(
			 DtPrintSetupBoxWidget new_w,
			 DtPrintSetupBoxWidget current)
{
    if(PSUB_PrintDestination(new_w) != DtPRINT_TO_PRINTER
       &&
       PSUB_PrintDestination(new_w) != DtPRINT_TO_FILE)
    {
	XmeWarning((Widget)new_w, WARN_PRINT_DESTINATION);
	if(current == (DtPrintSetupBoxWidget)NULL)
	    PSUB_PrintDestination(new_w) = DtPRINT_TO_PRINTER;
	else
	    PSUB_PrintDestination(new_w) = PSUB_PrintDestination(current);
    }
    if((Widget)NULL != PSUB_DestinationRadioBox(new_w))
    {
	Widget button;
	
	switch(PSUB_PrintDestination(new_w))
	{
	case DtPRINT_TO_PRINTER:
	    button =
		XtNameToWidget(PSUB_DestinationRadioBox(new_w), "button_0");
	    XmToggleButtonSetState(button, True, True);
	    break;
	    
	case DtPRINT_TO_FILE:
	    button =
		XtNameToWidget(PSUB_DestinationRadioBox(new_w), "button_1");
	    XmToggleButtonSetState(button, True, True);
	    break;
	}
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: SynthSetPrinterName
 *
 * Description:
 *
 *     Set the passed new_printer_name into the print setup box by
 *     updating the Printer Name text field and the modal_printer_name
 *     and printer_name instance variables. The verify printer state will
 *     be set to DtPRINT_NOT_VERIFIED if the printer_name was updated.
 *
 *     new_printer_name must have been previously allocated using one of
 *     the Xt memory allocation functions. If the passed new_printer_name
 *     is NULL, a default printer name will be used.
 *
 *     The old_printer_name is passed to the DtNclosePrintDisplayCallback
 *     list, if it is called. The old_printer_name is freed using XtFree.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
SynthSetPrinterName(
		    DtPrintSetupBoxWidget psub,
		    String new_printer_name,
		    String old_printer_name)
{
    /*
     * update the printer name in the widget instance structure
     */
    SetNewPrinterName(psub, new_printer_name, old_printer_name);
    /*
     * create a new modal printer name
     */
    XtFree(PSUB_ModalPrinterSpec(psub));
    switch(PSUB_PrintSetupMode(psub))
    {
    case DtPRINT_SETUP_PLAIN:
	/*
	 * Ignore the name mode
	 */
	PSUB_ModalPrinterSpec(psub) = XtNewString(PSUB_PrinterName(psub));
	break;

    case DtPRINT_SETUP_XP:
	/*
	 * Create a modal X Printer Specifier
	 */
	PSUB_ModalPrinterSpec(psub) =
	    CreateModalPrinterSpec(PSUB_XpPrinterNameMode(psub),
				   PSUB_PrinterName(psub));
	break;
    }
    /*
     * Set the modal printer specifier in the Printer Name text field
     */
    if(PSUB_PrinterNameText(psub))
    {
	XtVaSetValues(PSUB_PrinterNameText(psub),
		      XmNvalue, PSUB_ModalPrinterSpec(psub),
		      NULL);
	XmComboBoxUpdate(PSUB_PrinterNameCombo(psub));
    }
    /*
     * verify the printer; get intial attributes for a new X printer
     * connection
     */
    if(PSUB_VerifyPrinterState(psub) == DtPRINT_NOT_VERIFIED)
    {
	DtPrintSetupData psd;
	Window window;
	
	memset(&psd, 0, sizeof(DtPrintSetupData));
	/*
	 * don't show any messages if this widget isn't mapped
	 */
	psd.messages_hint = DtPRINT_HINT_NO_MESSAGES;
	window = XtWindow((Widget)psub);
	if(window)
	{
	    XWindowAttributes attr;
	    Status status = XGetWindowAttributes(XtDisplay((Widget)psub),
						 window, &attr);
	    if(status != 0 && attr.map_state == IsViewable)
		psd.messages_hint = DtPRINT_HINT_MESSAGES_OK;
	}
	/*
	 * establish a connection to the X printer
	 */
	EstablishPrinter(psub, &psd);
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: UpdatePrinterNameCallback
 *
 * Description:
 *
 *     Kick off a timer proc to establish the selected printer in the
 *     widget. This allows the hourglass to be visible while the
 *     connection is being established. The hourglass can't be set inside
 *     the callback because the combo box list is still popped up with
 *     the pointer grabbed... Of course if the user is traversing through
 *     the list with the cursor keys, ie. without popping it down,
 *     there's nothing we can do to change the cursor...
 *
 * Return value:
 *
 *     None.
 *
 */
static void
UpdatePrinterNameCallback(
			  Widget w,
			  XtPointer client_data,
			  XtPointer call_data)
{
    DtPrintSetupBoxWidget psub = (DtPrintSetupBoxWidget)client_data;

    if(PSUB_TimeoutId(psub) != (XtIntervalId)NULL)
	XtRemoveTimeOut(PSUB_TimeoutId(psub));

    PSUB_TimeoutId(psub) =
	XtAppAddTimeOut(XtWidgetToApplicationContext(w),
			(unsigned long)0,
			UpdatePrinterNameTimeoutProc,
			(XtPointer)psub);
}

/*
 * ------------------------------------------------------------------------
 * Name: UpdatePrinterNameCallback
 *
 * Description:
 *
 *     Timeout proc that establishes a printer connection in response to
 *     a user selection in the printer name combo box.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
UpdatePrinterNameTimeoutProc(XtPointer client_data,
			     XtIntervalId* id)
{
    DtPrintSetupBoxWidget psub = (DtPrintSetupBoxWidget)client_data;
    DtPrintSetupData psd;
    /*
     * the timeout seems to happen before expose events from the
     * popdown
     */
    XmUpdateDisplay((Widget)psub);
    /*
     * the timeout proc is only called once per XtAppAddTimeOut
     */
    PSUB_TimeoutId(psub) = (XtIntervalId)NULL;
    /*
     * check to see if the printer name has changed (if so,
     * SynthGetPrinterName will close an existing print connection)
     */
    SynthGetPrinterName(psub);
    /*
     * try to establish a connection to a new printer name
     * (the message hint is set so that we don't see error messages from
     * here)
     */
    memset(&psd, 0, sizeof(DtPrintSetupData));
    psd.messages_hint = DtPRINT_HINT_NO_MESSAGES;
    EstablishPrinter(psub, &psd);
}

/*
 * ------------------------------------------------------------------------
 * Name: UpdateString
 *
 * Description:
 *
 *     Set the label string of a label or button
 *
 * Return value:
 *
 *     None.
 *
 */
static void 
UpdateString(
	     Widget w,
	     XmString string,
	     XmStringDirection direction)
{
    Arg		al[3];
    register int	ac = 0;

    if (w)
    {
	XtSetArg (al[ac], XmNstringDirection, direction);  ac++;
	XtSetArg (al[ac], XmNlabelString, string);  ac++;
	XtSetValues (w, al, ac);
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: ValidatePrintSetupMode
 *
 * Description:
 *
 *     Validate the DtNprintSetupMode resource.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
ValidatePrintSetupMode(
		       DtPrintSetupBoxWidget new_w,
		       DtPrintSetupBoxWidget current)
{
    if(current == (DtPrintSetupBoxWidget)NULL)
    {
	switch(PSUB_PrintSetupMode(new_w))
	{
	case DtPRINT_SETUP_PLAIN:
	case DtPRINT_SETUP_XP:
	    break;
	default:
	    PSUB_PrintSetupMode(new_w) = SETUP_MODE_DEFAULT;
	    XmeWarning((Widget)new_w, WARN_SETUP_MODE);
	    break;
	}
    }
    else
    {
	/*
	 * prevent changes to the mode
	 */
	PSUB_PrintSetupMode(new_w) = PSUB_PrintSetupMode(current);
	XmeWarning((Widget)new_w, WARN_SETUP_MODE_CHANGE);
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: ValidateWorkAreaLocation
 *
 * Description:
 *
 *     Validate the DtNworkAreaLocation resource.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
ValidateWorkAreaLocation(
			 DtPrintSetupBoxWidget new_w,
			 DtPrintSetupBoxWidget current)
{
    switch(PSUB_WorkAreaLocation(new_w))
    {
    case DtWORK_AREA_NONE:
    case DtWORK_AREA_TOP:
    case DtWORK_AREA_TOP_AND_BOTTOM:
    case DtWORK_AREA_BOTTOM:
	break;
	
    default:
	XmeWarning((Widget)new_w, WARN_WORK_AREA_LOCATION);
	if(current == (DtPrintSetupBoxWidget)NULL)
	    PSUB_WorkAreaLocation(new_w) = WORK_AREA_DEFAULT;
	else
	    PSUB_WorkAreaLocation(new_w) = PSUB_WorkAreaLocation(current);
	break;
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxCreateBottomSeparator
 *
 * Description:
 *
 *     Create the Separator displayed above the bottom work area
 *
 * Return value:
 *
 *     None.
 *
 */
void 
_DtPrintSetupBoxCreateBottomSeparator(DtPrintSetupBoxWidget psub)
{
    Arg al[10];
    register int ac = 0;

    XtSetArg(al[ac], XmNhighlightThickness, 0);	 ac++;
    PSUB_BottomSeparator(psub) =
	XmCreateSeparatorGadget((Widget) psub,
				"BottomWorkAreaSeparator",
				al, ac);
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxCreateButtonSeparator
 *
 * Description:
 *
 *     Create the Separator displayed above the buttons.
 *
 * Return value:
 *
 *     None.
 *
 */
void 
_DtPrintSetupBoxCreateButtonSeparator(DtPrintSetupBoxWidget sel)
{
    Arg al[10];
    register int ac = 0;

    XtSetArg(al[ac], XmNhighlightThickness, 0);	 ac++;
    PSUB_ButtonSeparator(sel) =
	XmCreateSeparatorGadget((Widget) sel, "ButtonSeparator", al, ac);
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxCreateCancelButton
 *
 * Description:
 *
 *     Create the "Cancel" PushButton.
 *
 * Return value:
 *
 *     None.
 *
 */
void 
_DtPrintSetupBoxCreateCancelButton(
				   DtPrintSetupBoxWidget psub)
{
    PSUB_CancelButton(psub) =
	CreateButtonGadget((Widget)psub, CANCEL_LABEL, "Cancel", XmNONE);

    XtAddCallback(PSUB_CancelButton(psub), XmNactivateCallback, 
		  PrintSetupBoxCallback, (XtPointer) DtPRINT_CANCEL_BUTTON) ;
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxCreateCopiesControl
 *
 * Description:
 *
 *     Create the copy count label and spin box.
 *
 * Return value:
 *
 *     None.
 *
 */
void 
_DtPrintSetupBoxCreateCopiesControl(DtPrintSetupBoxWidget psub)
{
    Widget copies_label;
    Widget copies_text;
    Boolean copies_text_editable;

    /*
     * create a row column to contain the copy count label and spin box
     */
    PSUB_CopiesControl(psub) =
	XtVaCreateWidget("CopiesControl",
			 xmRowColumnWidgetClass,
			 (Widget)psub,
			 XmNorientation, XmHORIZONTAL,
			 NULL);
    /*
     * create the label
     */
    copies_label = CreateLabelGadget(PSUB_CopiesControl(psub),
				     COPIES_LABEL, "CopiesLabel");
    XtManageChild(copies_label);
    /*
     * spin box
     */
    PSUB_CopiesSpinBox(psub) =
	XtVaCreateManagedWidget("Copies", 
				xmSimpleSpinBoxWidgetClass,
				PSUB_CopiesControl(psub) ,
				XmNspinBoxChildType, XmNUMERIC,
				XmNminimumValue, 1,
				XmNmaximumValue, MAX_COPIES,
				XmNposition, 1,
				XmNpositionType, XmPOSITION_VALUE,
				XmNcolumns, 5,
				XmNwrap, False,
				NULL);

    copies_text = NULL;
    copies_text_editable = FALSE;
    XtVaGetValues(
		PSUB_CopiesSpinBox(psub),
		XmNtextField, &copies_text,
		XmNeditable, &copies_text_editable,
		NULL);

    if (copies_text && copies_text_editable)
      XtAddCallback(
		copies_text, XmNvalueChangedCallback,
		CopiesTextValueChangedCallback, (XtPointer) psub);
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxCreateDescription
 *
 * Description:
 *
 *     Create the printer description.
 *
 * Return value:
 *
 *     None.
 *
 */
void 
_DtPrintSetupBoxCreateDescription(
				  DtPrintSetupBoxWidget psub)
{
    XmRenderTable render_table;
    XFontStruct* font;
    unsigned long char_width;
    XmString empty_label;
    /*
     * create the description label gadget
     */
    empty_label = XmStringCreateLocalized(" ");
    PSUB_Description(psub) =
	XtVaCreateWidget("Description",
			 xmLabelWidgetClass,
			 (Widget)psub,
			 XmNalignment, XmALIGNMENT_BEGINNING,
			 XmNlabelString, empty_label,
			 NULL);
    XmStringFree(empty_label);
    /*
     * get the maximum character width for the default font of the gadget
     */
    XtVaGetValues(PSUB_Description(psub), XmNrenderTable, &render_table, NULL);
    if(XmeRenderTableGetDefaultFont(render_table, &font))
    {
	Bool success;

	success = XGetFontProperty(font, XA_QUAD_WIDTH, &char_width);
	if(!success || char_width == 0)
	{
	    if(font->per_char
	       && font->min_char_or_byte2 <= '0'
	       && font->max_char_or_byte2 >= '0'
	       )
		char_width =
		    font->per_char['0' - font->min_char_or_byte2].width;
	    else
		char_width = font->max_bounds.width;
	}
	/*
	 * set and lock the width of description gadget
	 */
	XtVaSetValues(PSUB_Description(psub),
		      XmNwidth, (Dimension)(DESCRIPTION_COLUMNS*char_width),
		      XmNrecomputeSize, False,
		      NULL);
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxCreateDescriptionLabel
 *
 * Description:
 *
 *     Create the Label for the printer description.
 *
 * Return value:
 *
 *     None.
 *
 */
void 
_DtPrintSetupBoxCreateDescriptionLabel(DtPrintSetupBoxWidget psub)
{
    PSUB_DescriptionLabel(psub) =
	CreateLabelGadget((Widget) psub,
			  DESCRIPTION_LABEL,
			  "DescriptionLabel") ;
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxCreateDestinationRadioBox
 *
 * Description:
 *
 *     Create the "Print to Printer" and "Print to File" radio buttons.
 *
 * Return value:
 *
 *     None.
 *
 */
void
_DtPrintSetupBoxCreateDestinationRadioBox(DtPrintSetupBoxWidget psub)
{
    Widget button;
    XmString label;
    
    PSUB_DestinationRadioBox(psub) =
	XtVaCreateWidget(
			 "DestRadioBox",
			 xmRowColumnWidgetClass,
			 (Widget)psub,
			 XmNradioBehavior, True,
			 XmNorientation, XmHORIZONTAL,
			 XmNpacking, XmPACK_TIGHT,
			 NULL);

    label =
	XmStringGenerate((XtPointer)PRINT_TO_PRINTER_LABEL, (XmStringTag)NULL,
			 XmMULTIBYTE_TEXT, (XmStringTag)NULL);
    button =
	XtVaCreateManagedWidget("button_0",
				xmToggleButtonWidgetClass,
				PSUB_DestinationRadioBox(psub),
				XmNlabelString, label,
				XmNuserData, (XtPointer)DtPRINT_TO_PRINTER,
				NULL);
    XmStringFree(label);
    XtAddCallback(button, XmNvalueChangedCallback,
		  DestinationChangedCallback, (XtPointer)psub);

    label = XmStringGenerate((XtPointer)PRINT_TO_FILE_LABEL, (XmStringTag)NULL,
			     XmMULTIBYTE_TEXT, (XmStringTag)NULL);
    button =
	XtVaCreateManagedWidget("button_1",
				xmToggleButtonWidgetClass,
				PSUB_DestinationRadioBox(psub),
				XmNlabelString, label,
				XmNuserData, (XtPointer)DtPRINT_TO_FILE,
				NULL);
    XmStringFree(label);
    XtAddCallback(button, XmNvalueChangedCallback,
		  DestinationChangedCallback, (XtPointer)psub);
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxCreateFileNameLabel
 *
 * Description:
 *
 *     Create the "Print to File" check box.
 *
 * Return value:
 *
 *     None.
 *
 */
void 
_DtPrintSetupBoxCreateFileNameLabel(DtPrintSetupBoxWidget psub)
{
    PSUB_FileNameLabel(psub) =
	CreateLabelGadget((Widget)psub, FILE_NAME_LABEL, "FileNameLabel") ;
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxCreateFileNameText
 *
 * Description:
 *
 *     Create the print to file name text field.
 *
 * Return value:
 *
 *     None.
 *
 */
void 
_DtPrintSetupBoxCreateFileNameText(DtPrintSetupBoxWidget psub)
{
    PSUB_FileNameText(psub) =
	XtVaCreateWidget("FileName", 
			 xmTextFieldWidgetClass,
			 (Widget)psub,
			 XmNcolumns, FILE_NAME_COLUMNS,
			 NULL);
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxCreateHelpButton
 *
 * Description:
 *     Create the "Help" PushButton.
 *
 * Return value:
 *
 *     None.
 *
 */
void 
_DtPrintSetupBoxCreateHelpButton(DtPrintSetupBoxWidget psub)
{
    PSUB_HelpButton(psub) =
	CreateButtonGadget((Widget)psub, HELP_LABEL, "Help", XmNONE) ;
    /*
     * Remove BulletinBoard Unmanage callback
     */
    XtRemoveAllCallbacks(PSUB_HelpButton(psub), XmNactivateCallback) ;

    XtAddCallback(PSUB_HelpButton (psub), XmNactivateCallback, 
		  PrintSetupBoxCallback, (XtPointer)DtPRINT_HELP_BUTTON) ;
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxCreatePrintButton
 *
 * Description:
 *     Create the "Print" PushButton.
 *
 * Return value:
 *
 *     None.
 *
 */
void 
_DtPrintSetupBoxCreatePrintButton(DtPrintSetupBoxWidget psub)
{
    PSUB_PrintButton(psub) =
	CreateButton((Widget)psub, PRINT_LABEL, "Print", XmNONE) ;
    /*
     * Remove the BulletinBoard unmanage callback. The
     * PrintSetupBoxCallback routine will unmanage the widget if the
     * AutoUnmanage resource is set, but only if the printer name is
     * successfully verified.
     */
    XtRemoveAllCallbacks(PSUB_PrintButton(psub), XmNactivateCallback);
    XtAddCallback(PSUB_PrintButton(psub), XmNactivateCallback, 
		  PrintSetupBoxCallback, (XtPointer)DtPRINT_PRINT_BUTTON) ;
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxCreatePrinterInfoButton
 *
 * Description:
 *
 *     Create the "Info..." PushButton
 *
 * Return value:
 *
 *     None.
 *
 */
void 
_DtPrintSetupBoxCreatePrinterInfoButton(DtPrintSetupBoxWidget psub)
{
    PSUB_PrinterInfoButton(psub) =
	CreateButton((Widget)psub, PRINTER_INFO_LABEL, "Info", XmTAB_GROUP);

    XtRemoveAllCallbacks(PSUB_PrinterInfoButton(psub), XmNactivateCallback);
    XtAddCallback(PSUB_PrinterInfoButton(psub), XmNactivateCallback, 
		  PrintSetupBoxCallback, (XtPointer) DtPRINT_INFO_BUTTON) ;
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxCreatePrinterNameCombo
 *
 * Description:
 *
 *     Create the combo box containing the printer name text field and
 *     the short printer list.
 *
 * Return value:
 *
 *     None.
 *
 */
void 
_DtPrintSetupBoxCreatePrinterNameCombo(DtPrintSetupBoxWidget psub)
{
    PSUB_PrinterNameCombo(psub) =
	XtVaCreateWidget("Name", xmComboBoxWidgetClass, (Widget)psub,
			 XmNcomboBoxType, XmDROP_DOWN_COMBO_BOX,
			 XmNcolumns, PRINTER_NAME_COLUMNS,
			 XmNpositionMode, XmZERO_BASED,
			 NULL);
    PSUB_PrinterNameText(psub) =
	XtNameToWidget(PSUB_PrinterNameCombo(psub), "*Text");
    /*
     * add a selection callback to the combo box that will cause the
     * selected printer to be set in the PSUB (verified, etc).
     */
    XtAddCallback(PSUB_PrinterNameCombo(psub), XmNselectionCallback,
		  UpdatePrinterNameCallback, (XtPointer)psub);
    /*
     * Xp Setup Mode specific initialization
     */
    if(PSUB_PrintSetupMode(psub) == DtPRINT_SETUP_XP)
    {
	String* printer_list;
	Boolean hide_arrow = True;
	/*
	 * add a callback to close an existing connection whenever the
	 * user modifies the text field.
	 */
	XtAddCallback(PSUB_PrinterNameText(psub), XmNvalueChangedCallback,
		      ClosePrintConnectionCallback, (XtPointer)psub);
	/*
	 * initialize the combo box list
	 */
	printer_list = _DtPrintGetXpPrinterList((Widget)psub);
	if(printer_list != (String*)NULL)
	{
	    int count;
	    /*
	     * count the number of printer names in the returned list,
	     */
	    for(count = 0; printer_list[count] != (String)NULL; count++);
	    if(count > 0)
	    {
		XmString* printer_xmstr_list;
		int i;
		Widget combo_list;
		/*
		 * build a XmString version of the printer list
		 */
		printer_xmstr_list =
		    (XmString*)XtCalloc(count, sizeof(XmString));
		for(i = 0; i < count; i++)
		    printer_xmstr_list[i] =
			XmStringGenerate((XtPointer)printer_list[i],
					 (XmStringTag)NULL,
					 XmMULTIBYTE_TEXT, (XmStringTag)NULL);
		/*
		 * get the widget ID of the combo box list
		 */
		combo_list =
		    XtNameToWidget(PSUB_PrinterNameCombo(psub), "*List");
		if(combo_list)
		{
		    int visible_item_count;
		    /*
		     * get the initial visible item count
		     */
		    XtVaGetValues(combo_list,
				  XmNvisibleItemCount, &visible_item_count,
				  NULL);
		    /*
		     * reduce the visible item count if needed
		     */
		    if(count < visible_item_count)
			visible_item_count = count;
		    /*
		     * set the XmString printer list in the combo box list
		     */
		    XtVaSetValues(combo_list,
				  XmNitemCount, count,
				  XmNitems, printer_xmstr_list,
				  XmNvisibleItemCount, visible_item_count,
				  NULL);
		    hide_arrow = False;
		}
		for(i = 0; i < count; i++)
		    XmStringFree(printer_xmstr_list[i]);
		XtFree((char*)printer_xmstr_list);
	    }
	    _DtPrintFreeStringList(printer_list);
	}
	if(hide_arrow)
	{
	    Dimension text_shadow_thickness;
	    XtVaGetValues(PSUB_PrinterNameText(psub),
			  XmNshadowThickness, &text_shadow_thickness,
			  NULL);
	    XtVaSetValues(PSUB_PrinterNameCombo(psub),
			  XmNarrowSize, 0,
			  XmNarrowSpacing, 0,
			  XmNmarginWidth, 0,
			  XmNmarginHeight, 0,
			  XmNshadowThickness, 0,
			  NULL);
	    XtVaSetValues(PSUB_PrinterNameText(psub),
			  XmNshadowThickness, text_shadow_thickness,
			  NULL);
	}

    }
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxCreatePrinterNameLabel
 *
 * Description:
 *
 *     Create the Label for the printer name.
 *
 * Return value:
 *
 *     None.
 *
 */
void 
_DtPrintSetupBoxCreatePrinterNameLabel(DtPrintSetupBoxWidget psub)
{
    PSUB_PrinterNameLabel(psub) =
	CreateLabelGadget((Widget) psub,
			  PRINTER_NAME_LABEL,
			  "NameLabel") ;
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxCreateSelectFileButton
 *
 * Description:
 *
 *     Create the "Select File..." PushButton
 *
 * Return value:
 *
 *     None.
 *
 */
void 
_DtPrintSetupBoxCreateSelectFileButton(DtPrintSetupBoxWidget psub)
{
    PSUB_SelectFileButton(psub) =
	CreateButtonGadget((Widget)psub, SELECT_FILE_LABEL,
			   "SelectFile", XmTAB_GROUP);

    XtRemoveAllCallbacks(PSUB_SelectFileButton(psub), XmNactivateCallback) ;
    XtAddCallback(PSUB_SelectFileButton(psub), XmNactivateCallback, 
		  PrintSetupBoxCallback, (XtPointer) DtPRINT_FILES_BUTTON) ;
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxCreateSelectPrinterButton
 *
 * Description:
 *
 *     Create the "Select Printer..." PushButton
 *
 * Return value:
 *
 *     None.
 *
 */
void 
_DtPrintSetupBoxCreateSelectPrinterButton(DtPrintSetupBoxWidget psub)
{
    PSUB_SelectPrinterButton(psub) =
	CreateButtonGadget((Widget)psub, SELECT_PRINTER_LABEL,
			   "SelectPrinter", XmTAB_GROUP);

    XtRemoveAllCallbacks(PSUB_SelectPrinterButton(psub), XmNactivateCallback);
    XtAddCallback(PSUB_SelectPrinterButton(psub), XmNactivateCallback, 
		  PrintSetupBoxCallback, (XtPointer) DtPRINT_PRINTERS_BUTTON) ;
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxCreateSetupButton
 *
 * Description:
 *
 *     Create the "Setup" PushButton.
 *
 * Return value:
 *
 *     None.
 *
 */
void 
_DtPrintSetupBoxCreateSetupButton(DtPrintSetupBoxWidget psub)
{
    PSUB_SetupButton(psub) =
	CreateButton((Widget) psub, SETUP_LABEL, "Setup", XmNONE);
    /*
     * Remove BulletinBoard Unmanage callback from Setup button
     */
    XtRemoveAllCallbacks(PSUB_SetupButton(psub), XmNactivateCallback) ;

    XtAddCallback(PSUB_SetupButton (psub), XmNactivateCallback, 
		  PrintSetupBoxCallback, (XtPointer) DtPRINT_SETUP_BUTTON) ;
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxCreateTopSeparator
 *
 * Description:
 *
 *     Create the Separator displayed below the top work area
 *
 * Return value:
 *
 *     None.
 *
 */
void 
_DtPrintSetupBoxCreateTopSeparator(DtPrintSetupBoxWidget psub)
{
    Arg al[10];
    register int ac = 0;

    XtSetArg(al[ac], XmNhighlightThickness, 0);	 ac++;
    PSUB_TopSeparator(psub) =
	XmCreateSeparatorGadget((Widget) psub,
				"TopWorkAreaSeparator",
				al, ac);
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxGeoMatrixCreate - BulletinBoard class method
 *
 * Description:
 *
 * Return value:
 *
 */
XmGeoMatrix 
_DtPrintSetupBoxGeoMatrixCreate(
				Widget wid,
				Widget instigator,
				XtWidgetGeometry *desired)
{
    DtPrintSetupBoxWidget psub = (DtPrintSetupBoxWidget) wid ;
    XmGeoMatrix	    geoSpec ;
    register XmGeoRowLayout  layoutPtr ;
    register XmKidGeometry   boxPtr ;
    XmKidGeometry   firstButtonBox ;
    XmKidGeometry   boxMark;
    Dimension	    vspace = BB_MarginHeight(psub);
    int		    i;
    PSUB_GeoExtension ext;
    int psub_geo_fill = XmGEO_PACK;
    int psub_geo_fit = XmGEO_PROPORTIONAL;
    /*
     * Layout PrintSetupBox XmGeoMatrix.
     * Each row is terminated by leaving an empty XmKidGeometry and
     * moving to the next XmGeoRowLayout.
     */
    geoSpec = _XmGeoMatrixAlloc(DtPSUB_MAX_WIDGETS_VERT,
				psub->composite.num_children,
				sizeof(PSUB_GeoExtensionRec));
    ext = geoSpec->extension;

    geoSpec->composite = (Widget) psub ;
    geoSpec->instigator = (Widget) instigator ;
    if(desired)
    {
	geoSpec->instig_request = *desired ;
    } 
    geoSpec->margin_w = BB_MarginWidth(psub) + psub->manager.shadow_thickness;
    geoSpec->margin_h = BB_MarginHeight(psub) + psub->manager.shadow_thickness;
    geoSpec->no_geo_request = _DtPrintSetupBoxNoGeoRequest ;

    layoutPtr = &(geoSpec->layouts->row) ;
    boxPtr = geoSpec->boxes ;
    /*
     * menu bar 
     */
    for (i = 0; i < psub->composite.num_children; i++)
    {
	Widget w = psub->composite.children[i];

	if(XmIsRowColumn(w)
	    && ((XmRowColumnWidget)w)->row_column.type == XmMENU_BAR
	    && w != PSUB_TopWorkArea(psub)
	    && w != PSUB_BottomWorkArea(psub)
	    && _XmGeoSetupKid(boxPtr, w))
	{
	    layoutPtr->fix_up = MenuBarFixUp;
	    boxPtr += 2;
	    ++layoutPtr;
	    vspace = 0;		/* fixup space_above of next row. */
	    break;
	}
    }
    /*
     * top work area + separator
     */
    if(PSUB_TopWorkArea(psub) != (Widget)NULL)
    {
	if(_XmGeoSetupKid(boxPtr, PSUB_TopWorkArea(psub)))
	{
	    layoutPtr->space_above = vspace;
	    vspace = BB_MarginHeight(psub);
	    boxPtr += 2;
	    ++layoutPtr;
	}
	if(_XmGeoSetupKid(boxPtr, PSUB_TopSeparator(psub)))
	{
	    layoutPtr->fix_up = SeparatorFixUp;
	    layoutPtr->space_above = vspace;
	    vspace = BB_MarginHeight(psub);
	    boxPtr += 2 ;
	    ++layoutPtr ;
	} 
    }
    /*
     * printer description label, printer description and printer info
     * button
     */
    boxMark = boxPtr;
    if(_XmGeoSetupKid(boxPtr, PSUB_DescriptionLabel(psub)))
    {	
	(ext->pbox)[ext->row_count][0].geo = boxPtr;
	(ext->pbox)[ext->row_count][0].align = PSUB_GEO_ALIGN_RIGHT;
	++boxPtr;
    } 
    if(_XmGeoSetupKid(boxPtr, PSUB_Description(psub)))
    {	
	(ext->pbox)[ext->row_count][1].geo = boxPtr;
	(ext->pbox)[ext->row_count][1].align = PSUB_GEO_ALIGN_LEFT;
	++boxPtr;
    }
    if(_XmGeoSetupKid(boxPtr, PSUB_PrinterInfoButton(psub)))
    {	
	(ext->pbox)[ext->row_count][2].geo = boxPtr;
	(ext->pbox)[ext->row_count][2].align = PSUB_GEO_ALIGN_EXPAND;
	++boxPtr;
    }
    if(boxPtr != boxMark)
    {
	(ext->layout_ptr)[ext->row_count++] = layoutPtr;
	layoutPtr->fix_up = ColumnGeoFixUp;
	layoutPtr->even_width = 0;
	layoutPtr->even_height = 0;
	layoutPtr->fill_mode = psub_geo_fill ;
	layoutPtr->fit_mode = psub_geo_fit ;
	layoutPtr->space_above = vspace;
	vspace = BB_MarginHeight(psub);
	++boxPtr;
	++layoutPtr ;
    }
    /*
     * printer name label, printer name, select printer button
     */
    boxMark = boxPtr;
    if(_XmGeoSetupKid(boxPtr, PSUB_PrinterNameLabel(psub)))
    {	
	(ext->pbox)[ext->row_count][0].geo = boxPtr;
	(ext->pbox)[ext->row_count][0].align = PSUB_GEO_ALIGN_RIGHT;
	++boxPtr;
    } 
    if(_XmGeoSetupKid(boxPtr, PSUB_PrinterNameCombo(psub)))
    {	
	(ext->pbox)[ext->row_count][1].geo = boxPtr;
	(ext->pbox)[ext->row_count][1].align = PSUB_GEO_ALIGN_LEFT;
	++boxPtr;
    }
    if(_XmGeoSetupKid(boxPtr, PSUB_SelectPrinterButton(psub)))
    {	
	(ext->pbox)[ext->row_count][2].geo = boxPtr;
	(ext->pbox)[ext->row_count][2].align = PSUB_GEO_ALIGN_EXPAND;
	++boxPtr;
    }
    if(boxPtr != boxMark)
    {
	(ext->layout_ptr)[ext->row_count++] = layoutPtr;
	layoutPtr->fix_up = ColumnGeoFixUp;
	layoutPtr->even_width = 0;
	layoutPtr->even_height = 0;
	layoutPtr->fill_mode = psub_geo_fill ;
	layoutPtr->fit_mode = psub_geo_fit ;
	layoutPtr->space_above = vspace;
	vspace = BB_MarginHeight(psub);
	++boxPtr;
	++layoutPtr ;
    }
    /*
     * file name label, file name text, select file button
     */
    boxMark = boxPtr;
    if(_XmGeoSetupKid(boxPtr, PSUB_FileNameLabel(psub)))
    {	
	(ext->pbox)[ext->row_count][0].geo = boxPtr;
	(ext->pbox)[ext->row_count][0].align = PSUB_GEO_ALIGN_RIGHT;
	++boxPtr;
    } 
    if(_XmGeoSetupKid(boxPtr, PSUB_FileNameText(psub)))
    {	
	(ext->pbox)[ext->row_count][1].geo = boxPtr;
	(ext->pbox)[ext->row_count][1].align = PSUB_GEO_ALIGN_LEFT;
	++boxPtr;
    } 
    if(_XmGeoSetupKid(boxPtr, PSUB_SelectFileButton(psub)))
    {	
	(ext->pbox)[ext->row_count][2].geo = boxPtr;
	(ext->pbox)[ext->row_count][2].align = PSUB_GEO_ALIGN_EXPAND;
	++boxPtr;
    } 
    if(boxPtr != boxMark)
    {
	(ext->layout_ptr)[ext->row_count++] = layoutPtr;
	layoutPtr->fix_up = ColumnGeoFixUp;
	layoutPtr->even_width = 0;
	layoutPtr->even_height = 0;
	layoutPtr->fill_mode = psub_geo_fill ;
	layoutPtr->fit_mode = psub_geo_fit ;
	layoutPtr->space_above = vspace;
	vspace = BB_MarginHeight(psub);
	++boxPtr;
	++layoutPtr ;
    }
    /*
     * copies label and copies field
     */
    boxMark = boxPtr;
    if(_XmGeoSetupKid(boxPtr, PSUB_DestinationRadioBox(psub)))
    {	
	++boxPtr;
    } 
    if(_XmGeoSetupKid(boxPtr, PSUB_CopiesControl(psub)))
    {	
	++boxPtr;
    } 
    if(boxPtr != boxMark)
    {
	layoutPtr->fill_mode = XmGEO_CENTER ;
	layoutPtr->fit_mode = XmGEO_WRAP ;
	layoutPtr->even_width = 0;
	layoutPtr->even_height = 0;
	layoutPtr->space_between = 10;
	layoutPtr->space_above = vspace;
	vspace = BB_MarginHeight(psub);
	++boxPtr;
	++layoutPtr ;
    }
    /*
     * bottom work area + separator
     */
    if(PSUB_BottomWorkArea(psub) != (Widget)NULL)
    {
	if(_XmGeoSetupKid(boxPtr, PSUB_BottomSeparator(psub)))
	{
	    layoutPtr->fix_up = SeparatorFixUp;
	    layoutPtr->space_above = vspace;
	    vspace = BB_MarginHeight(psub);
	    boxPtr += 2 ;
	    ++layoutPtr ;
	} 
	if(_XmGeoSetupKid(boxPtr, PSUB_BottomWorkArea(psub)))
	{
	    layoutPtr->space_above = vspace;
	    vspace = BB_MarginHeight(psub);
	    boxPtr += 2;
	    ++layoutPtr;
	}
    }
    /*
     * button separator 
     */
    if(_XmGeoSetupKid(boxPtr, PSUB_ButtonSeparator(psub)))
    {
	layoutPtr->fix_up = SeparatorFixUp;
	layoutPtr->space_above = vspace;
	vspace = BB_MarginHeight(psub);
	boxPtr += 2 ;
	++layoutPtr ;
    } 
    /*
     * button row 
     */
    firstButtonBox = boxPtr ;
    if(_XmGeoSetupKid(boxPtr, PSUB_PrintButton(psub)))
    {
	++boxPtr ;
    } 
    for(i = 0; i < psub->composite.num_children; i++)
    {
	Widget w = psub->composite.children[i];
	if(IsButton(w)
	   &&
	   !IsAutoButton(psub,w)
	   &&
	   w != PSUB_TopWorkArea(psub)
	   &&
	   w != PSUB_BottomWorkArea(psub)
	   )
	{
	    if(_XmGeoSetupKid(boxPtr, w))
	    {
		++boxPtr ;
	    } 
	}
    }
    if(_XmGeoSetupKid(boxPtr, PSUB_SetupButton(psub)))
    {
	++boxPtr ;
    } 
    if(_XmGeoSetupKid(boxPtr, PSUB_CancelButton(psub)))
    {
	++boxPtr ;
    } 
    if(_XmGeoSetupKid(boxPtr, PSUB_HelpButton(psub)))
    {
	++boxPtr ;
    } 
    if(boxPtr != firstButtonBox)
    {
	layoutPtr->fill_mode = XmGEO_CENTER ;
	layoutPtr->fit_mode = XmGEO_WRAP ;
	layoutPtr->space_above = vspace;
	vspace = BB_MarginHeight(psub);
	if(!(psub->print_setup_box.minimize_buttons))
	{
	    layoutPtr->even_width = 1 ;
	} 
	layoutPtr->even_height = 1 ;
	++layoutPtr ;
    }
    /*
     * the end. 
     */
    layoutPtr->space_above = vspace;
    layoutPtr->end = TRUE ;
    return(geoSpec) ;
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxNoGeoRequest
 *
 * Description:
 *
 * Return value:
 *
 */
Boolean 
_DtPrintSetupBoxNoGeoRequest(
			     XmGeoMatrix geoSpec)
{
    if(BB_InSetValues(geoSpec->composite)
       && (XtClass(geoSpec->composite) == dtPrintSetupBoxWidgetClass))
    {	
	return(TRUE) ;
    } 
    return(FALSE) ;
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxGetCopies
 *
 * Description:
 *
 *     Update the location pointed to by "value" with the current value
 *     of the Copies spin box.
 *
 * Return value:
 *
 *     None.
 *
 */
void 
_DtPrintSetupBoxGetCopies(
			  Widget wid,
			  int resource_offset,
			  XtArgVal *value)
{
    DtPrintSetupBoxWidget psub = (DtPrintSetupBoxWidget) wid ;
    int copies;

    if(PSUB_CopiesSpinBox(psub) != (Widget)NULL)
    {	
	XtVaGetValues(PSUB_CopiesSpinBox(psub),
		      XmNposition, &copies,
		      NULL);
	*value = (XtArgVal)copies;
    }
    else
    {
	*value = (XtArgVal)PSUB_Copies(psub);
    } 
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxGetDescription
 *
 * Description:
 *
 *     Update the description instance variable based on the current
 *     value of the Printer Description label gadget.
 *
 * Return value:
 *
 *     None.
 *
 */
void 
_DtPrintSetupBoxGetDescription(
			       Widget wid,
			       int resource_offset,
			       XtArgVal *value)
{
    DtPrintSetupBoxWidget psub = (DtPrintSetupBoxWidget) wid ;
    XmString data ;

    if(PSUB_Description(psub) != (Widget)NULL)
    {	
	XtVaGetValues((Widget)PSUB_Description(psub),
		      XmNlabelString, &data,
		      NULL);
	*value = (XtArgVal)data;
    }
    else
    {
	*value = (XtArgVal)NULL;
    } 
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxGetFileName
 *
 * Description:
 *
 * Return value:
 *
 *     None.
 *
 */
void 
_DtPrintSetupBoxGetFileName(
			    Widget w,
			    int resource_offset,
			    XtArgVal *value)
{
    DtPrintSetupBoxWidget psub = (DtPrintSetupBoxWidget)w;

    SynthGetFileName(psub);
    *value = (XtArgVal)PSUB_FileName(psub);
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxGetPrinterName
 *
 * Description:
 *
 * Return value:
 *
 *     None.
 *
 */
void 
_DtPrintSetupBoxGetPrinterName(
			       Widget w,
			       int resource_offset,
			       XtArgVal *value)
{
    DtPrintSetupBoxWidget psub = (DtPrintSetupBoxWidget)w;

    SynthGetPrinterName(psub);
    *value = (XtArgVal)PSUB_PrinterName(psub);
}

/*
 * ------------------------------------------------------------------------
 * Name: DtCreatePrintSetupBox
 *
 * Description:
 *
 *     DtCreatePrintSetupBox is a convenience function that creates an
 *     unmanaged instance of a DtPrintSetupBox widget, and returns its
 *     widget ID.
 *
 * Arguments:
 *
 *     parent
 *         Specifies the parent widget ID.
 *
 *     name
 *         Specifies the name of the created widget.
 *
 *     args
 *         Specifies the argument list.
 *
 *     n
 *         Specifies the number of attribute/value pairs in arglist.
 *
 * Return value:
 *
 *     Returns the DtPrintSetupBox widget ID.
 *
 */
Widget 
DtCreatePrintSetupBox(
		      Widget p,
		      String name,
		      ArgList args,
		      Cardinal n)
{
    return XtCreateWidget(name, dtPrintSetupBoxWidgetClass, p, args, n);
}

/*
 * ------------------------------------------------------------------------
 * Name: DtCreatePrintSetupDialog
 *
 * Description:
 *
 *     DtCreatePrintSetupDialog is a convenience function that creates an
 *     instance of a dialog containing a DtPrintSetupBox widget, and
 *     returns the DtPrintSetupBox widget ID.
 *
 * Arguments:
 *
 *     parent
 *         Specifies the parent widget ID.
 *
 *     name
 *         Specifies the name of the created widget.
 *
 *     args
 *         Specifies the argument list.
 *
 *     n
 *         Specifies the number of attribute/value pairs in arglist.
 *
 * Return value:
 *
 *     Returns the DtPrintSetupBox widget ID.
 *
 */
Widget 
DtCreatePrintSetupDialog(
			 Widget parent,
			 String name,
			 ArgList args,
			 Cardinal n)
{
    return XmeCreateClassDialog(dtPrintSetupBoxWidgetClass,
				parent, name, args, n);
}

/*
 * ------------------------------------------------------------------------
 * Name: DtPrintCopySetupData
 *
 * Description:
 *
 *     DtPrintCopySetupData is used to copy the DtPrintSetupData
 *     structure pointed to by 'source' to the DtPrintSetupData structure
 *     pointed to by 'target'. Elements in 'target' are updated only if
 *     different than the corresponding element in 'source'. For elements
 *     that point to allocated memory, DtPrintCopySetupData allocates new
 *     memory for those elements updated in 'target'. Existing elements
 *     in 'target' are freed using XtFree. All elements in a
 *     DtPrintSetupData structure can be freed by calling
 *     DtPrintFreeSetupData.
 *
 *     If 'source' or 'target' is NULL the copy will not be performed.
 *
 * Arguments:
 *
 *     target
 *         A pointer to the DtPrintSetupData structure to copy to.
 *
 *     source
 *         A pointer to the DtPrintSetupData structure to copy from.
 *
 * Return value:
 *
 *     The target pointer.
 *
 */
DtPrintSetupData*
DtPrintCopySetupData(
		     DtPrintSetupData* target,
		     const DtPrintSetupData* source)
{
    if(source != (DtPrintSetupData*)NULL
       &&
       target != (DtPrintSetupData*)NULL)
    {
	if((source->printer_name == (String)NULL
	    ||
	    target->printer_name == (String)NULL)
	   ? True
	   : strcmp(source->printer_name, target->printer_name) != 0)
	{
	    XtFree(target->printer_name);
	    target->printer_name = XtNewString(source->printer_name);
	}

	target->print_display = source->print_display;
	target->print_context = source->print_context;
	target->destination = source->destination;
	target->messages_hint = source->messages_hint;

	if((source->dest_info == (String)NULL
	    ||
	    target->dest_info == (String)NULL)
	   ? True
	   : strcmp(source->dest_info, target->dest_info) != 0)
	{
	    XtFree(target->dest_info);
	    target->dest_info = XtNewString(source->dest_info);
	}
    }
    return target;
}

/*
 * ------------------------------------------------------------------------
 * Name: DtPrintFillSetupData
 *
 * Description:
 *
 *     DtPrintFillSetupData is used to obtain an X printer connection in
 *     order to initiate an X printing job in situations other than
 *     direct interaction with a DtPrintSetupBox (e.g. a "quick print"
 *     button on a toolbar). This printer connection information can be
 *     obtained from an existing DtPrintSetupBox widget instance, or if a
 *     DtPrintSetupBox widget instance is unavailable,
 *     DtPrintFillSetupData will provide a new X printer connection.
 *
 * Arguments:
 *
 *     psub
 *         The widget ID of a DtPrintSetupBox, or NULL if no
 *         DtPrintSetupBox is available.
 *
 *     print_data
 *         A pointer to an existing DtPrintSetupData structure that
 *         DtPrintFillSetupData will update with valid X printer
 *         connection information.
 *
 * Return value:
 *
 *     DtPRINT_SUCCESS
 *         The X printer connection was successfully obtained.
 *
 *     DtPRINT_FAILURE
 *         The X printer connection could not be established. The
 *         specific reason has been reported by the print setup box to
 *         the user.
 *
 *     DtPRINT_INVALID_DISPLAY
 *         The indicated X print server could not be found.
 *
 *     DtPRINT_NOT_XP_DISPLAY
 *         The indicated X server does not support the X Printing
 *         Extension.
 *
 *     DtPRINT_NO_PRINTER
 *         The indicated printer could not be found on the X print
 *         server.
 *
 *     DtPRINT_NO_DEFAULT
 *         A default printer could not be determined.
 *
 *     DtPRINT_BAD_PARM
 *         The value passed for print_data is NULL.
 *
 */
XtEnum DtPrintFillSetupData(
			    Widget w,
			    DtPrintSetupData* print_data)
{
    DtPrintSetupBoxWidget psub = (DtPrintSetupBoxWidget)w;
    
    if(print_data == (DtPrintSetupData*)NULL)
	return DtPRINT_BAD_PARM;
    
    if(psub == (DtPrintSetupBoxWidget)NULL)
    {
	XtEnum status;
	String new_printer_spec;
	Display* new_display;
	XPContext new_context;
	/*
	 * GUI-less printing; verify the printer name and establish the
	 * print connection without involving the print setup box.
	 */
	status = _DtPrintVerifyXPrinter(NULL,
					print_data->printer_name,
					&new_printer_spec,
					&new_display,
					&new_context);
	if(status == DtPRINT_SUCCESS)
	{
	    print_data->print_display = new_display;
	    print_data->print_context = new_context;
	}
	if(new_printer_spec != (String)NULL)
	{
	    XtFree(print_data->printer_name);
	    print_data->printer_name = new_printer_spec;
	}
	return status;
    }
    else
    {
	DtPrintSetupData psd;
	XtEnum status;
	_DtPrintWidgetToAppContext((Widget)psub);
	/*
	 * return if the passed widget's setup mode is not Xp
	 */
	if(PSUB_PrintSetupMode(psub) != DtPRINT_SETUP_XP)
	    return DtPRINT_BAD_PARM;
	/*
	 * intialize an internal version of the print setup data struct
	 */
	memset(&psd, 0, sizeof(DtPrintSetupData));
	/*
	 * get the latest printer name from the widget
	 */
	_DtPrintAppLock(app);
	SynthGetPrinterName(psub);
	/*
	 * check to see if the passed printer name is different than
	 * the psub printer name
	 */
	if(print_data->printer_name != (String)NULL)
	{
	    if((PSUB_PrinterName(psub) == NULL)
	       ? True
	       : strcmp(print_data->printer_name, PSUB_PrinterName(psub)) != 0)
	    {
		/*
		 * if different, set the passed printer name into the
		 * passed print setup box widget
		 */
		SynthSetPrinterName(psub,
				    XtNewString(print_data->printer_name),
				    PSUB_PrinterName(psub));
	    }
	}
	/*
	 * establish a connection to the X printer
	 */
	psd.messages_hint = DtPRINT_HINT_NO_MESSAGES;
	status = EstablishPrinter(psub, &psd);
	if(status == DtPRINT_SUCCESS)
	{
	    /*
	     * set printing attributes
	     */
	    SetPrintAttributes(psub);
	    /*
	     * setup the destination info
	     */
	    SetPSDDestination(psub, &psd);
	    /*
	     * update the passed print setup data struct based on the
	     * internal version of the print setup data struct
	     */
	    DtPrintCopySetupData(print_data, &psd);
	}
	else
	{
	    /*
	     * Unable to establish the printer connection; manage the
	     * print setup box in order to present the error box created
	     * by EstablishPrinter() to the user, allowing the user to
	     * select a new printer or cancel.
	     */
	    XtManageChild((Widget)psub);
	    /*
	     * if the default verifyPrintProc created an error box,
	     * display it. If the app installed its own verify proc, the
	     * app will need to check the return status of
	     * DtPrintFillSetupData, and manage its error dialog itself.
	     */
	    _DtPrintDefProcManageErrorBox(&PSUB_DefaultProcData(psub));
	}
	_DtPrintAppUnlock(app);
	return status;
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: DtPrintFreeSetupData
 *
 * Description:
 *
 *     DtPrintFreeSetupData calls XtFree to deallocate memory pointed to
 *     by elements of the DtPrintSetupData structure indicated by
 *     'target'. 'target' is then re-initialized to zeros.
 *
 * Arguments:
 *
 *     target
 *         points to the DtPrintSetupData structure whose elements are to
 *         be freed.
 *
 * Return value:
 *
 *     None.
 *
 */
void DtPrintFreeSetupData(
			  DtPrintSetupData* target)
{
    XtFree(target->printer_name);
    XtFree(target->dest_info);
    memset(target, 0, sizeof(DtPrintSetupData));
}

/*
 * ------------------------------------------------------------------------
 * Name: DtPrintResetConnection
 *
 * Arguments:
 *
 *     psub
 *         The DtPrintSetupBox widget ID.
 *
 *     mode
 *         Indicates whether DtPrintResetConnection should close the X
 *         print server connection, or simply cause the DtPrintSetupBox
 *         to cease managing the connection.
 *
 * Description:
 *
 *     DtPrintResetConnection is intended to be used by applications that
 *     fork a child process to perform the print rendering
 *     operation. After the fork is performed, the parent process will
 *     close its X print server connection, and maintain its connection
 *     to the video X server. The forked child on the other hand will
 *     close its video X server connection and perform the rendering
 *     operation on the X print server connection.
 *
 *     Valid values for the 'mode' parameter are:
 *
 *         DtPRINT_CLOSE_CONNECTION
 *             Set by the parent process when the application forks a
 *             child to perform the print rendering. This will cause the
 *             DtNclosePrintDisplayCallback list set for the passed
 *             DtPrintSetupBox to be called.
 *
 *         DtPRINT_RELEASE_CONNECTION
 *             Set when the application wishes to destroy the
 *             DtPrintSetupBox widget instance and still perform print
 *             rendering using the X print server connection initiated by
 *             the widget. For example, the child process of an
 *             application that forks to perform print rendering will
 *             close the video display connection (thereby destroying the
 *             DtPrintSetupBox widget) prior to print rendering.
 *
 * Return value:
 *
 *     DtPRINT_SUCCESS
 *         DtPrintResetConnection was successful.
 *
 *     DtPRINT_NO_CONNECTION
 *         An open X print server connection is not currently being
 *         managed by the DtPrintSetupBox.
 *
 *     DtPRINT_BAD_PARM
 *         The value passed for wid is NULL, or an invalid mode was
 *         passed.
 *
 */
XtEnum
DtPrintResetConnection(
		       Widget w,
		       DtPrintResetConnectionMode mode)
{
    DtPrintSetupBoxWidget psub = (DtPrintSetupBoxWidget)w;
    _DtPrintWidgetToAppContext((Widget)psub);

    if(psub == (DtPrintSetupBoxWidget)NULL)
	return DtPRINT_BAD_PARM;

    _DtPrintAppLock(app);

    if(PSUB_PrintSetupMode(psub) != DtPRINT_SETUP_XP)
    {
        _DtPrintAppUnlock(app);
	return DtPRINT_BAD_PARM;
    }

    if(PSUB_Display(psub) == (Display*)NULL)
    {
        _DtPrintAppUnlock(app);
	return DtPRINT_NO_CONNECTION;
    }
    
    switch(mode)
    {
    case DtPRINT_RELEASE_CONNECTION:
	/*
	 * simply disavow knowledge of the X print connection
	 */
	PSUB_Display(psub) = (Display*)NULL;
	PSUB_Context(psub) = (XPContext)NULL;
	break;
	
    case DtPRINT_CLOSE_CONNECTION:
	/*
	 * call the widget's close print connection routine
	 */
	ClosePrintConnection(psub, PSUB_PrinterName(psub), True);
	break;

    default:
        _DtPrintAppUnlock(app);
	return DtPRINT_BAD_PARM;
    }
    /*
     * cause the printer to be re-verified if new connection is
     * is required later, and return
     */
    PSUB_VerifyPrinterState(psub) = DtPRINT_NOT_VERIFIED;
    _DtPrintAppUnlock(app);
    return DtPRINT_SUCCESS;
}
