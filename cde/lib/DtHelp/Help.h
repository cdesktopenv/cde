/* $XConsortium: Help.h /main/3 1995/10/26 12:23:24 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 */

#ifndef _Dt_Help_h
#define _Dt_Help_h

#include <X11/Intrinsic.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Constants
 */

/* Common DtHelp Resources  */

#ifndef DtNcloseCallback
#define DtNcloseCallback	   "closeCallback"
#endif
#ifndef DtNcolumns
#define DtNcolumns                 XmNcolumns
#endif
#ifndef DtNexecutionPolicy
#define DtNexecutionPolicy         "executionPolicy"
#endif
#ifndef DtNhelpFile
#define DtNhelpFile                "helpFile"
#endif
#ifndef DtNhelpOnHelpVolume
#define DtNhelpOnHelpVolume		"helpOnHelpVolume"
#endif
#ifndef DtNhelpPrint
#define DtNhelpPrint               "helpPrint"
#endif
#ifndef DtNhelpType
#define DtNhelpType                "helpType"
#endif
#ifndef DtNhelpVolume
#define DtNhelpVolume              "helpVolume"
#endif
#ifndef DtNhyperLinkCallback
#define DtNhyperLinkCallback       "hyperLinkCallback"
#endif
#ifndef DtNlocationId
#define DtNlocationId              "locationId"
#endif
#ifndef DtNmanPage
#define DtNmanPage                 "manPage"
#endif
#ifndef DtNminimizeButtons
#define DtNminimizeButtons         XmNminimizeButtons
#endif
#ifndef DtNpaperSize
#define DtNpaperSize               "paperSize"
#endif
#ifndef DtNprinter
#define DtNprinter                 "printer"
#endif
#ifndef DtNrows
#define DtNrows                    XmNrows
#endif
#ifndef DtNscrollBarPolicy
#define DtNscrollBarPolicy         "scrollBarPolicy"
#endif
#ifndef DtNstringData
#define DtNstringData              "stringData"
#endif

#ifndef DtCCallback
#define DtCCallback                XmCCallback
#endif

#ifndef DtCCloseCallback
#define DtCCloseCallback	   "CloseCallback"
#endif
#ifndef DtCColumns
#define DtCColumns                 XmCColumns
#endif
#ifndef DtCExecutionPolicy
#define DtCExecutionPolicy         "ExecutionPolicy"
#endif
#ifndef DtCHelpFile
#define DtCHelpFile                "HelpFile"
#endif
#ifndef DtCHelpOnHelpVolume
#define DtCHelpOnHelpVolume	   "HelpOnHelpVolume"
#endif
#ifndef DtCHelpPrint
#define DtCHelpPrint               "HelpPrint"
#endif
#ifndef DtCHelpType
#define DtCHelpType                "HelpType"
#endif
#ifndef DtCHelpVolume
#define DtCHelpVolume              "HelpVolume"
#endif
#ifndef DtCHyperLinkCallback
#define DtCHyperLinkCallback       "HyperLinkCallback"
#endif
#ifndef DtCLocationId
#define DtCLocationId              "LocationId"
#endif
#ifndef DtCManPage
#define DtCManPage                 "ManPage"
#endif
#ifndef DtCMinimizeButtons
#define DtCMinimizeButtons         XmCMinimizeButtons
#endif
#ifndef DtCPaperSize
#define DtCPaperSize               "PaperSize"
#endif
#ifndef DtCPrinter
#define DtCPrinter                 "Printer"
#endif
#ifndef DtCRows
#define DtCRows                    XmCRows
#endif
#ifndef DtCScrollBarPolicy
#define DtCScrollBarPolicy         "ScrollBarPolicy"
#endif
#ifndef DtCStringData
#define DtCStringData              "StringData"
#endif

/* Representation types */

#ifndef DtRDtExecutionPolicy
#define DtRDtExecutionPolicy         "DtExecutionPolicy"
#endif
#ifndef DtRDtHelpType
#define DtRDtHelpType                "DtHelpType"
#endif
#ifndef DtRDtPaperSize
#define DtRDtPaperSize               "DtPaperSize"
#endif
#ifndef DtRDtScrollBarPolicy
#define DtRDtScrollBarPolicy         "DtScrollBarPolicy"
#endif

/* DtHelpDialogCallbackStruct windowHint field */

#define DtHELP_POPUP_WINDOW       1
#define DtHELP_CURRENT_WINDOW     2
#define DtHELP_NEW_WINDOW         3

/* DtHelpDialogCallbackStruct hyperType field */

#define DtHELP_LINK_JUMP_NEW          1
#define DtHELP_LINK_TOPIC             1
#define DtHELP_LINK_MAN_PAGE          4
#define DtHELP_LINK_APP_DEFINE        5
#define DtHELP_LINK_TEXT_FILE         8

/* DtHelpDialogCallbackStruct reason field */

#define DtCR_HELP_LINK_ACTIVATE      1
#define DtCR_HELP_CLOSE              2
#define DtCR_HELP_HELP               3

/* Possible values for DtNscrollBarPolicy resource */

#define  DtHELP_NO_SCROLLBARS        0
#define  DtHELP_STATIC_SCROLLBARS    1
#define  DtHELP_AS_NEEDED_SCROLLBARS 2

/* Possible values for DtNexecutionPolicy resource */
#define  DtHELP_EXECUTE_NONE			0
#define  DtHELP_EXECUTE_QUERY_ALL		1
#define  DtHELP_EXECUTE_QUERY_UNALIASED		2
#define  DtHELP_EXECUTE_ALL			3

/* Possible values for DtNhelpType resource */

#define DtHELP_TYPE_TOPIC           0
#define DtHELP_TYPE_STRING          1
#define DtHELP_TYPE_MAN_PAGE        2
#define DtHELP_TYPE_FILE            3
#define DtHELP_TYPE_DYNAMIC_STRING  4

/* Possible values for DtNpaperSize resource */

#define DtHELP_PAPERSIZE_LETTER     0
#define DtHELP_PAPERSIZE_LEGAL      1
#define DtHELP_PAPERSIZE_EXECUTIVE  2
#define DtHELP_PAPERSIZE_A4         3
#define DtHELP_PAPERSIZE_B5         4

/* DtHelpQuickGetChildGetChild() function */

#define DtHELP_QUICK_CLOSE_BUTTON    1
#define DtHELP_QUICK_PRINT_BUTTON    2
#define DtHELP_QUICK_HELP_BUTTON     3
#define DtHELP_QUICK_SEPARATOR       4
#define DtHELP_QUICK_MORE_BUTTON     5
#define DtHELP_QUICK_BACK_BUTTON     6

/* Return values for DtHelpReturnSelectedWidgetId() */

#define DtHELP_SELECT_ERROR   -1 
#define DtHELP_SELECT_VALID    0
#define DtHELP_SELECT_ABORT    1
#define DtHELP_SELECT_INVALID  2 


/*
 * Types
 */

typedef struct {
	int	reason;
	XEvent	*event;
	char	*locationId;
	char	*helpVolume;
	char	*specification;
	int	hyperType;
	int	windowHint;
} DtHelpDialogCallbackStruct;


/*
 * Functions
 */

extern void DtHelpSetCatalogName(
		char		*catFile);

extern int DtHelpReturnSelectedWidgetId(
		Widget		parent,
		Cursor		cursor,
		Widget		*widget);

#ifdef __cplusplus
}
#endif

#endif /* _Dt_Help_h */
