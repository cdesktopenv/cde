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
/* $XConsortium: printSetup.c /main/14 1996/10/25 13:37:55 mgreess $ */
/**********************************<+>*************************************
***************************************************************************
**
**  File:        printSetup.c
**
**  Project:     HP DT dtpad, a memo maker type editor based on the
**               Dt Editor widget.
**
**  Description:  Routines which manipulate the print setup dialog
**
**************************************************************************
**********************************<+>*************************************/
/*
 *                   Common Desktop Environment
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *   (c) Copyright 1995 Digital Equipment Corp.
 *   (c) Copyright 1995 Fujitsu Limited
 *   (c) Copyright 1995 Hitachi, Ltd.
 *
 *
 *                     RESTRICTED RIGHTS LEGEND
 *
 *Use, duplication, or disclosure by the U.S. Government is subject to
 *restrictions as set forth in subparagraph (c)(1)(ii) of the Rights in
 *Technical Data and Computer Software clause in DFARS 252.227-7013.  Rights
 *for non-DOD U.S. Government Departments and Agencies are as set forth in
 *FAR 52.227-19(c)(1,2).

 *Hewlett-Packard Company, 3000 Hanover Street, Palo Alto, CA 94304 U.S.A.
 *International Business Machines Corp., Route 100, Somers, NY 10589 U.S.A.
 *Sun Microsystems, Inc., 2550 Garcia Avenue, Mountain View, CA 94043 U.S.A.
 *Novell, Inc., 190 River Road, Summit, NJ 07901 U.S.A.
 *Digital Equipment Corp., 111 Powdermill Road, Maynard, MA 01754, U.S.A.
 *Fujitsu Limited, 1015, Kamikodanaka Nakahara-Ku, Kawasaki 211, Japan
 *Hitachi, Ltd., 6, Kanda Surugadai 4-Chome, Chiyoda-ku, Tokyo 101, Japan
 */

#define REUSE_PRINT_SETUP_DIALOGS

#include <stdio.h>
#include <sys/param.h>

#include <X11/Intrinsic.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/MwmUtil.h>
#include <Xm/ToggleB.h>
#include <Dt/DtPStrings.h>
#include <Dt/HelpDialog.h>
#include <Dt/Print.h>
#include <Dt/Session.h>

#include "dtpad.h"

static char *_psGetResourceFileName(
		PrintSetup *pSetup);
static void _psSetupToSource(
		PrintSetup *pSetup);
static void _psSourceToSetup(
		PrintSetup *pSetup,
		Boolean wordWrap);
static void _psSetupToUi(
		PrintSetup *pSetup);
static void _psUiToSetup(
	    	PrintSetup *pSetup);
static Boolean _psUiSpecsAreValid(
	    	PrintSetup *pSetup);
static void _psAttachPrintSetupDialog(
		PrintSetup *pSetup,
		Editor *pPad);
static Widget _psCreatePrintSetupDialog(
		Widget parent,
		Editor *pPad);
static void _psPrintSetupCancelCB(
		Widget dtprintSetup,
		XtPointer client_data,
		XtPointer call_data);
static void _psPrintSetupDestroyCB(
		Widget dtprintSetup,
		XtPointer client_data,
		XtPointer call_data);
static void _psPrintSetupPrintCB(
		Widget dtprintSetup,
		XtPointer client_data,
		XtPointer call_data);
static void _psReleasePrintSetupDialog(
		PrintSetup *pSetup);

#define PS_ARRAY_SIZE(ary)	(sizeof((ary))/sizeof((ary)[0]))

#define PS_HOME_ENV_VARIABLE			"HOME"
#define PS_DEFAULT_RESOURCE_FILE		"dtpadrc"

#define PS_RESOURCE_HDRFTR_FORMAT_STRING	"Dtpad.%s:  %d\n"
#define PS_RESOURCE_MARGIN_FORMAT_STRING	"Dtpad.%s:  %s\n"

#define PS_RESOURCE_NAME_HEADER_LEFT		"headerLeftEnum"
#define PS_RESOURCE_NAME_HEADER_RIGHT		"headerRightEnum"
#define PS_RESOURCE_NAME_FOOTER_LEFT		"footerLeftEnum"
#define PS_RESOURCE_NAME_FOOTER_RIGHT		"footerRightEnum"

#define PS_RESOURCE_NAME_MARGIN_TOP		"marginTopSpec"
#define PS_RESOURCE_NAME_MARGIN_RIGHT		"marginRightSpec"
#define PS_RESOURCE_NAME_MARGIN_BOTTOM		"marginBottomSpec"
#define PS_RESOURCE_NAME_MARGIN_LEFT		"marginLeftSpec"

#define PS_RESOURCE_VALUE_NONE			"Empty"
#define PS_RESOURCE_VALUE_DATE			"Date"
#define PS_RESOURCE_VALUE_DOCNAME		"DocName"
#define PS_RESOURCE_VALUE_PAGE_NUMBER		"PageNumber"
#define PS_RESOURCE_VALUE_USER_NAME		"UserName"

#define PS_RESOURCE_VALUE_DFLT_MARGIN		"1.0 in"

typedef struct resource_value
{
        PrintStringTypeEnum	which;
        char			*resource_string;
        int			set_id;
        int			msg_id;
        char			*dflt_string;
} ResourceValue;

static ResourceValue _psHdrFtrValues[] =
{
    { PRINT_NONE_STRING_TYPE_ENUM,
      PS_RESOURCE_VALUE_NONE,
      15, 1, "None"
    },
    { PRINT_DATE_STRING_TYPE_ENUM,
      PS_RESOURCE_VALUE_DATE,
      15, 2, "Date"
    },
    { PRINT_DOCNAME_STRING_TYPE_ENUM,
      PS_RESOURCE_VALUE_DOCNAME,
      15, 3, "Document Name"
    },
    { PRINT_PAGE_NUMBER_STRING_TYPE_ENUM,
      PS_RESOURCE_VALUE_PAGE_NUMBER,
      15, 4, "Page Number"
    },
    { PRINT_USER_NAME_STRING_TYPE_ENUM,
      PS_RESOURCE_VALUE_USER_NAME,
      15, 5, "User Name"
    }
};
				    

static Widget		_psDefaultPrintSetupDialog = (Widget) NULL;
static char		*_psDefaultMargin = PS_RESOURCE_VALUE_DFLT_MARGIN;

static XtResource _psResources[] = {
  {PS_RESOURCE_NAME_FOOTER_LEFT, "FooterLeftEnum", XmRInt, sizeof(int),
      XtOffset(PrintOptions*, footerLeft),
      XmRImmediate, (XtPointer) PRINT_NONE_STRING_TYPE_ENUM,
  },
  {PS_RESOURCE_NAME_FOOTER_RIGHT, "FooterRightEnum", XmRInt, sizeof(int),
      XtOffset(PrintOptions*, footerRight),
      XmRImmediate, (XtPointer) PRINT_PAGE_NUMBER_STRING_TYPE_ENUM,
  },
  {PS_RESOURCE_NAME_HEADER_LEFT, "HeaderLeftEnum", XmRInt, sizeof(int),
      XtOffset(PrintOptions*, headerLeft),
      XmRImmediate, (XtPointer) PRINT_NONE_STRING_TYPE_ENUM,
  },
  {PS_RESOURCE_NAME_HEADER_RIGHT, "HeaderRightEnum", XmRInt, sizeof(int),
      XtOffset(PrintOptions*, headerRight),
      XmRImmediate, (XtPointer) PRINT_DOCNAME_STRING_TYPE_ENUM,
  },
  {PS_RESOURCE_NAME_MARGIN_BOTTOM, "MarginBottomSpec", XmRString, sizeof(String),
      XtOffset(PrintOptions*, marginBottom),
      XmRString, (XtPointer) PS_RESOURCE_VALUE_DFLT_MARGIN,
  },
  {PS_RESOURCE_NAME_MARGIN_LEFT, "MarginLeftSpec", XmRString, sizeof(String),
      XtOffset(PrintOptions*, marginLeft),
      XmRString, (XtPointer) PS_RESOURCE_VALUE_DFLT_MARGIN,
  },
  {PS_RESOURCE_NAME_MARGIN_RIGHT, "MarginRightSpec", XmRString, sizeof(String),
      XtOffset(PrintOptions*, marginRight),
      XmRString, (XtPointer) PS_RESOURCE_VALUE_DFLT_MARGIN,
  },
  {PS_RESOURCE_NAME_MARGIN_TOP, "MarginTopSpec", XmRString, sizeof(String),
      XtOffset(PrintOptions*, marginTop),
      XmRString, (XtPointer) PS_RESOURCE_VALUE_DFLT_MARGIN,
  }
};


/************************************************************************
 * PrintSetupCreate
 *     Class constructor.
 *     Adds dtpad specific items to the passed print setup box.
 *     The document file name is presented in the bottom work area.
 ************************************************************************/
PrintSetup *
PrintSetupCreate(
		Widget	parent,
		char	*docName,
		Boolean	wordWrap,
		Editor	*pPad,
		XtCallbackProc cancelCB, XtPointer cancelClosure,
		XtCallbackProc closeDisplayCB, XtPointer closeDisplayClosure,
		XtCallbackProc printCB, XtPointer printClosure,
		XtCallbackProc setupCB, XtPointer setupClosure
		)
{
    Widget	w;
    XmString	xms;
    unsigned char is_set;
    PrintSetup	*pSetup;
    Widget	*menu_buttons;
    int		nitems;
    char	**strings;
    void	**data;
    int		i;

    pSetup = (PrintSetup *) XtMalloc( sizeof(PrintSetup) );
    pSetup->parent = parent;
    pSetup->docName = strdup(docName);
    pSetup->cancelCB = cancelCB;
    pSetup->closeDisplayCB = closeDisplayCB;
    pSetup->printCB = printCB;
    pSetup->setupCB = setupCB;
    pSetup->cancelClosure = cancelClosure;
    pSetup->closeDisplayClosure = closeDisplayClosure;
    pSetup->printClosure = printClosure;
    pSetup->setupClosure = setupClosure;
    pSetup->pPad = pPad;

    pSetup->dtprintSetup = (Widget) NULL;
    pSetup->widgets = (psWidgets *) NULL;

    /*
     * Get the current default values for the app-specific printing options.
     */
    _psSourceToSetup(pSetup, wordWrap);

    /*
     * Get a print setup dialog.
     */
    _psAttachPrintSetupDialog(pSetup, pPad);

    return pSetup;
}

/************************************************************************
 * PrintSetupDestroy
 *     Class destructor.
 ************************************************************************/
void
PrintSetupDestroy(PrintSetup *pSetup)
{
    if (pSetup == NULL) return;

    if (pSetup->docName)
      free(pSetup->docName);
    if (pSetup->dtprintSetup);
      _psReleasePrintSetupDialog(pSetup);
    XtFree((char*) pSetup);
}

/************************************************************************
 * PrintSetupDisplay
 *     Displays the specified print setup GUI.
 ************************************************************************/
void
PrintSetupDisplay(PrintSetup *pSetup)
{
    if (pSetup == NULL) return;

    /*
     * Initialize the print setup Ui.
     */
    _psSetupToUi(pSetup);

    XtManageChild(pSetup->dtprintSetup);
    XtMapWidget(pSetup->dtprintSetup);
}

/************************************************************************
 * PrintSetupHide
 *     Hides the specified print setup GUI.
 ************************************************************************/
void
PrintSetupHide(PrintSetup *pSetup)
{
    if (pSetup == NULL) return;
    if (pSetup->dtprintSetup == NULL) return;
    XtUnmanageChild(pSetup->dtprintSetup);
    XtUnmapWidget(pSetup->dtprintSetup);
}


/************************************************************************
 * PrintSetupGetHdrFtrSpec
 *     Returns the current value of the specified header/footer print option.
 ************************************************************************/
PrintStringTypeEnum
PrintSetupGetHdrFtrSpec(PrintSetup *pSetup, _DtPrintHdrFtrEnum which)
{
    PrintStringTypeEnum	rtn;

    if (pSetup == NULL) return PRINT_NONE_STRING_TYPE_ENUM;
    switch (which)
    {
        case DTPRINT_OPTION_HEADER_LEFT:
	    rtn = pSetup->options.headerLeft;
	    break;
        case DTPRINT_OPTION_HEADER_RIGHT:
	    rtn = pSetup->options.headerRight;
	    break;
        case DTPRINT_OPTION_FOOTER_LEFT:
	    rtn = pSetup->options.footerLeft;
	    break;
        case DTPRINT_OPTION_FOOTER_RIGHT:
        default:
	    rtn = pSetup->options.footerRight;
	    break;
    }
    return rtn;
}

/************************************************************************
 * PrintSetupGetMarginSpec
 *     Returns the current value of the specified margin print option.
 ************************************************************************/
char *
PrintSetupGetMarginSpec(PrintSetup *pSetup, _DtPrintMarginEnum which)
{
    char	*rtn;

    if (pSetup == NULL) return _psDefaultMargin;
    switch (which)
    {
        case DTPRINT_OPTION_MARGIN_TOP:
	    rtn = pSetup->options.marginTop;
	    break;
        case DTPRINT_OPTION_MARGIN_RIGHT:
	    rtn = pSetup->options.marginRight;
	    break;
        case DTPRINT_OPTION_MARGIN_BOTTOM:
	    rtn = pSetup->options.marginBottom;
	    break;
        case DTPRINT_OPTION_MARGIN_LEFT:
        default:
	    rtn = pSetup->options.marginLeft;
	    break;
    }
    return rtn;
}

/************************************************************************
 * PrintSetupGetDefaultPrintData
 *     Returns the current value of the print data in the pData variable.
 ************************************************************************/
Boolean
PrintSetupGetDefaultPrintData(PrintSetup *pSetup, DtPrintSetupData *pData)
{
    if (pSetup == NULL || pData == NULL) return FALSE;
    return (DtPRINT_SUCCESS == DtPrintFillSetupData(
						pSetup->dtprintSetup,
						pData));
}

/************************************************************************
 * PrintSetupUseWordWrap
 *     Returns the current value of word wrap print option.
 ************************************************************************/
Boolean
PrintSetupUseWordWrap(PrintSetup *pSetup)
{
    if (pSetup == NULL) return TRUE;
    return pSetup->options.wordWrap;
}



/*******************************************************************************
 *******************************************************************************
 *
 *  Static Functions
 *
 *******************************************************************************
 ******************************************************************************/

/************************************************************************
 * _psGetResourceFileName
 ************************************************************************/
static char*
_psGetResourceFileName(PrintSetup *pSetup)
{
#define BUFFER_SIZE 256
    extern Widget topLevelWithWmCommand;
    static char	buffer[BUFFER_SIZE];
    static char	*dtpadrc = NULL;
    char	*savePath, *saveFile;

#ifdef hpV4
    return XtNewString("/tmp/dtpadrc");
#else
    if (dtpadrc != NULL) return XtNewString(dtpadrc);

    savePath = NULL;
    saveFile = PS_DEFAULT_RESOURCE_FILE;
    if (! DtSessionSavePath(topLevelWithWmCommand, &savePath, &saveFile))
    {
	sprintf(
		buffer, "%s/%s/%s",
		getenv(PS_HOME_ENV_VARIABLE),
		DtPERSONAL_TMP_DIRECTORY,
		PS_DEFAULT_RESOURCE_FILE);
	savePath = buffer;
    }
    dtpadrc = XtNewString(savePath);

    return XtNewString(dtpadrc);
#endif
}

/************************************************************************
 * _psSourceToSetup
 ************************************************************************/
static void
_psSourceToSetup(PrintSetup *pSetup, Boolean wordWrap)
{
    XrmDatabase		database;
    char		*dtpadrc;
    PrintOptions	pOption;

    if (pSetup == NULL) return;

    dtpadrc = _psGetResourceFileName(pSetup);
    database = XtDatabase(XtDisplay(pSetup->parent));
    XrmCombineFileDatabase(dtpadrc, &database, TRUE);
    XtFree(dtpadrc);

    /* Get the application resources */
    XtGetApplicationResources(
			pSetup->parent, &pOption,
			_psResources, XtNumber(_psResources),
			NULL, 0);

    pSetup->options.footerLeft = pOption.footerLeft;
    pSetup->options.footerRight = pOption.footerRight;
    pSetup->options.headerLeft = pOption.headerLeft;
    pSetup->options.headerRight = pOption.headerRight;
    pSetup->options.marginBottom = XtNewString(pOption.marginBottom);
    pSetup->options.marginLeft = XtNewString(pOption.marginLeft);
    pSetup->options.marginRight = XtNewString(pOption.marginRight);
    pSetup->options.marginTop = XtNewString(pOption.marginTop);
    pSetup->options.wordWrap = wordWrap;
}

/************************************************************************
 * _psSetupToSource
 ************************************************************************/
static void
_psSetupToSource(PrintSetup *pSetup)
{
    PrintOptions	*pOption;
    FILE		*fp;
    char		*dtpadrc;

    if (pSetup == NULL) return;
    pOption = &pSetup->options;

    dtpadrc = _psGetResourceFileName(pSetup);
    fp = fopen(dtpadrc, "wb");
    if (fp == NULL)
    {
	/* TBD:  Error message. */
        fprintf(
		stderr,
		"_psSetupToSource:  Cannot open resource file '%s'\n",
		dtpadrc);
	return;
    }
    XtFree(dtpadrc);

    fprintf(
	fp,
	PS_RESOURCE_HDRFTR_FORMAT_STRING,
	PS_RESOURCE_NAME_HEADER_LEFT,
	pOption->headerLeft
	);

    fprintf(
	fp,
	PS_RESOURCE_HDRFTR_FORMAT_STRING,
	PS_RESOURCE_NAME_HEADER_RIGHT,
	pOption->headerRight
	);

    fprintf(
	fp,
	PS_RESOURCE_HDRFTR_FORMAT_STRING,
	PS_RESOURCE_NAME_FOOTER_LEFT,
	pOption->footerLeft
	);

    fprintf(
	fp,
	PS_RESOURCE_HDRFTR_FORMAT_STRING,
	PS_RESOURCE_NAME_FOOTER_RIGHT,
	pOption->footerRight
	);

    fprintf(
	fp,
	PS_RESOURCE_MARGIN_FORMAT_STRING,
	PS_RESOURCE_NAME_MARGIN_TOP,
	pOption->marginTop
	);

    fprintf(
	fp,
	PS_RESOURCE_MARGIN_FORMAT_STRING,
	PS_RESOURCE_NAME_MARGIN_RIGHT,
	pOption->marginRight
	);

    fprintf(
	fp,
	PS_RESOURCE_MARGIN_FORMAT_STRING,
	PS_RESOURCE_NAME_MARGIN_BOTTOM,
	pOption->marginBottom
	);

    fprintf(
	fp,
	PS_RESOURCE_MARGIN_FORMAT_STRING,
	PS_RESOURCE_NAME_MARGIN_LEFT,
	pOption->marginLeft
	);
    
    fclose(fp);
}

/************************************************************************
 * _psSetupToUi
 ************************************************************************/
static void
_psSetupToUi(PrintSetup *pSetup)
{
    unsigned char	isSet;
    Widget		frame, w;
    PrintOptions	*pOption;

    if (pSetup == NULL ||
	pSetup->dtprintSetup == NULL ||
	pSetup->widgets == NULL)
      return;
    pOption = &pSetup->options;

    isSet = (pOption->wordWrap) ? XmSET : XmUNSET;
    XtVaSetValues(pSetup->widgets->wordWrapTB, XmNset, isSet, NULL);

    frame = pSetup->widgets->hdrftrFrame;
    w = _DtPrintHdrFtrFrameEnumToWidget(frame, DTPRINT_OPTION_HEADER_LEFT);
    _DtPrintSetHdrFtrByData(w, (void*) pOption->headerLeft);

    w = _DtPrintHdrFtrFrameEnumToWidget(frame, DTPRINT_OPTION_HEADER_RIGHT);
    _DtPrintSetHdrFtrByData(w, (void*) pOption->headerRight);

    w = _DtPrintHdrFtrFrameEnumToWidget(frame, DTPRINT_OPTION_FOOTER_LEFT);
    _DtPrintSetHdrFtrByData(w, (void*) pOption->footerLeft);

    w = _DtPrintHdrFtrFrameEnumToWidget(frame, DTPRINT_OPTION_FOOTER_RIGHT);
    _DtPrintSetHdrFtrByData(w, (void*) pOption->footerRight);

    frame = pSetup->widgets->marginFrame;
    w = _DtPrintMarginFrameEnumToWidget(frame, DTPRINT_OPTION_MARGIN_TOP);
    _DtPrintSetMarginSpec(w, pOption->marginTop);

    w = _DtPrintMarginFrameEnumToWidget(frame, DTPRINT_OPTION_MARGIN_RIGHT);
    _DtPrintSetMarginSpec(w, pOption->marginRight);

    w = _DtPrintMarginFrameEnumToWidget(frame, DTPRINT_OPTION_MARGIN_BOTTOM);
    _DtPrintSetMarginSpec(w, pOption->marginBottom);

    w = _DtPrintMarginFrameEnumToWidget(frame, DTPRINT_OPTION_MARGIN_LEFT);
    _DtPrintSetMarginSpec(w, pOption->marginLeft);
}

/************************************************************************
 * _psUiToSetup
 ************************************************************************/
static void
_psUiToSetup(PrintSetup *pSetup)
{
    XtArgVal		isSet;
    Widget		frame, w;
    PrintOptions	*pOption;
    char		*marginSpec;
    Boolean		error = FALSE;

    if (pSetup == NULL ||
	pSetup->dtprintSetup == NULL ||
	pSetup->widgets == NULL)
      return;
    pOption = &pSetup->options;

    XtVaGetValues(pSetup->widgets->wordWrapTB, XmNset, &isSet, NULL);
    pOption->wordWrap = (isSet == XmUNSET) ? FALSE : TRUE;

    frame = pSetup->widgets->hdrftrFrame;

    w = _DtPrintHdrFtrFrameEnumToWidget(frame, DTPRINT_OPTION_HEADER_LEFT);
    pOption->headerLeft = (PrintStringTypeEnum) _DtPrintGetHdrFtrData(w);

    w = _DtPrintHdrFtrFrameEnumToWidget(frame, DTPRINT_OPTION_HEADER_RIGHT);
    pOption->headerRight = (PrintStringTypeEnum) _DtPrintGetHdrFtrData(w);

    w = _DtPrintHdrFtrFrameEnumToWidget(frame, DTPRINT_OPTION_FOOTER_LEFT);
    pOption->footerLeft = (PrintStringTypeEnum) _DtPrintGetHdrFtrData(w);

    w = _DtPrintHdrFtrFrameEnumToWidget(frame, DTPRINT_OPTION_FOOTER_RIGHT);
    pOption->footerRight = (PrintStringTypeEnum) _DtPrintGetHdrFtrData(w);

    frame = pSetup->widgets->marginFrame;

    w = _DtPrintMarginFrameEnumToWidget(frame, DTPRINT_OPTION_MARGIN_TOP);
    marginSpec = _DtPrintGetMarginSpec(w);
    if (pOption->marginTop != NULL)
      XtFree(pOption->marginTop);
    if (marginSpec == NULL || strcmp("", marginSpec) == 0)
      pOption->marginTop = XtNewString(_psDefaultMargin);
    else
      pOption->marginTop = marginSpec;

    w = _DtPrintMarginFrameEnumToWidget(frame, DTPRINT_OPTION_MARGIN_RIGHT);
    marginSpec = _DtPrintGetMarginSpec(w);
    if (pOption->marginRight != NULL)
      XtFree(pOption->marginRight);
    if (marginSpec == NULL || strcmp("", marginSpec) == 0)
      pOption->marginRight = XtNewString(_psDefaultMargin);
    else
      pOption->marginRight = marginSpec;

    w = _DtPrintMarginFrameEnumToWidget(frame, DTPRINT_OPTION_MARGIN_BOTTOM);
    marginSpec = _DtPrintGetMarginSpec(w);
    if (pOption->marginBottom != NULL)
      XtFree(pOption->marginBottom);
    if (marginSpec == NULL || strcmp("", marginSpec) == 0)
      pOption->marginBottom = XtNewString(_psDefaultMargin);
    else
      pOption->marginBottom = marginSpec;

    w = _DtPrintMarginFrameEnumToWidget(frame, DTPRINT_OPTION_MARGIN_LEFT);
    marginSpec = _DtPrintGetMarginSpec(w);
    if (pOption->marginLeft != NULL)
      XtFree(pOption->marginLeft);
    if (marginSpec == NULL || strcmp("", marginSpec) == 0)
      pOption->marginLeft = XtNewString(_psDefaultMargin);
    else
      pOption->marginLeft = marginSpec;
}

/************************************************************************
 * _psUiSpecsAreValid
 ************************************************************************/
static Boolean
_psUiSpecsAreValid(PrintSetup *pSetup)
{
    unsigned char	isSet;
    Widget		frame, w;
    PrintOptions	*pOption;
    char		*marginSpec;

    if (pSetup == NULL ||
	pSetup->dtprintSetup == NULL ||
	pSetup->widgets == NULL)
      return;
    pOption = &pSetup->options;

    frame = pSetup->widgets->marginFrame;

    w = _DtPrintMarginFrameEnumToWidget(frame, DTPRINT_OPTION_MARGIN_TOP);
    marginSpec = _DtPrintGetMarginSpec(w);
    if (marginSpec != NULL && strcmp("", marginSpec) != 0)
    {
	XtEnum	parseError = FALSE;

        (void) XmConvertStringToUnits(
				XtScreenOfObject(pSetup->parent),
				marginSpec,
				XmVERTICAL,
				XmPIXELS,
				&parseError);

        if (parseError)
	{
	    char	*i18nMsg;
	    char	*errMsg;
            
	    i18nMsg = GETMESSAGE(15, 6, "Top Margin specifier is invalid:  ");
	    errMsg = (char*) XtMalloc(strlen(i18nMsg) + strlen(marginSpec) + 1);
	    sprintf(errMsg, "%s%s", i18nMsg, marginSpec);
	    Warning(pSetup->pPad, (char *) errMsg, XmDIALOG_WARNING);
	    XtFree(errMsg);
	    return FALSE;
        }
    }

    w = _DtPrintMarginFrameEnumToWidget(frame, DTPRINT_OPTION_MARGIN_RIGHT);
    marginSpec = _DtPrintGetMarginSpec(w);
    if (marginSpec != NULL && strcmp("", marginSpec) != 0)
    {
	XtEnum	parseError = FALSE;

        (void) XmConvertStringToUnits(
				XtScreenOfObject(pSetup->parent),
				marginSpec,
				XmHORIZONTAL,
				XmPIXELS,
				&parseError);

        if (parseError)
	{
	    char	*i18nMsg;
	    char	*errMsg;
            
	    i18nMsg =
	      GETMESSAGE(15, 7, "Right Margin specifier is invalid:  ");
	    errMsg = (char*) XtMalloc(strlen(i18nMsg) + strlen(marginSpec) + 1);
	    sprintf(errMsg, "%s%s", i18nMsg, marginSpec);
	    Warning(pSetup->pPad, (char *) errMsg, XmDIALOG_WARNING);
	    XtFree(errMsg);
	    return FALSE;
        }
    }

    w = _DtPrintMarginFrameEnumToWidget(frame, DTPRINT_OPTION_MARGIN_BOTTOM);
    marginSpec = _DtPrintGetMarginSpec(w);
    if (marginSpec != NULL && strcmp("", marginSpec) != 0)
    {
	XtEnum	parseError = FALSE;

        (void) XmConvertStringToUnits(
				XtScreenOfObject(pSetup->parent),
				marginSpec,
				XmVERTICAL,
				XmPIXELS,
				&parseError);

        if (parseError)
	{
	    char	*i18nMsg;
	    char	*errMsg;
            
	    i18nMsg =
	      GETMESSAGE(15, 8, "Bottom Margin specifier is invalid:  ");
	    errMsg = (char*) XtMalloc(strlen(i18nMsg) + strlen(marginSpec) + 1);
	    sprintf(errMsg, "%s%s", i18nMsg, marginSpec);
	    Warning(pSetup->pPad, (char *) errMsg, XmDIALOG_WARNING);
	    XtFree(errMsg);
	    return FALSE;
        }
    }

    w = _DtPrintMarginFrameEnumToWidget(frame, DTPRINT_OPTION_MARGIN_LEFT);
    marginSpec = _DtPrintGetMarginSpec(w);
    if (marginSpec != NULL && strcmp("", marginSpec) != 0)
    {
	XtEnum	parseError = FALSE;

        (void) XmConvertStringToUnits(
				XtScreenOfObject(pSetup->parent),
				marginSpec,
				XmHORIZONTAL,
				XmPIXELS,
				&parseError);

        if (parseError)
	{
	    char	*i18nMsg;
	    char	*errMsg;
            
	    i18nMsg = GETMESSAGE(15, 9, "Left Margin specifier is invalid:  ");
	    errMsg = (char*) XtMalloc(strlen(i18nMsg) + strlen(marginSpec) + 1);
	    sprintf(errMsg, "%s%s", i18nMsg, marginSpec);
	    Warning(pSetup->pPad, (char *) errMsg, XmDIALOG_WARNING);
	    XtFree(errMsg);
	    return FALSE;
        }
    }

    return TRUE;
}


/*
 * _psAttachPrintSetupDialog
 *	Creates the widgets for the Setup Dialog.
 */
static void
_psAttachPrintSetupDialog(PrintSetup *pSetup, Editor *pPad)
{
    char	*title, *titlePrefix, *titleParent;
    XmString	xms;
    Widget	dialog;
    unsigned char is_set;

#ifdef REUSE_PRINT_SETUP_DIALOGS
    if (_psDefaultPrintSetupDialog != NULL)
    {
	/*
	 * Attempt to reuse the last print setup and print display connection.
	 */
        pSetup->dtprintSetup = _psDefaultPrintSetupDialog;
	_psDefaultPrintSetupDialog = NULL;
    }
    else
#endif
    {
        char    *dirname;

        pSetup->dtprintSetup = _psCreatePrintSetupDialog(pSetup->parent, pPad);
	dirname = getenv("HOME");
        if (dirname)
        {
            char        path[MAXPATHLEN+1];
            char        *filename;

	    filename = strrchr(pSetup->docName, '/');
	    if (NULL == filename)
	      filename = pSetup->docName;
	    else
	      filename++;
            sprintf(path, "%s/%s.ps", dirname, filename);
            XtVaSetValues(pSetup->dtprintSetup, DtNfileName, path, NULL);
        }
    }

    /*
     * Copy the information about the widgets into the setup.
     */
    XtVaGetValues(pSetup->dtprintSetup, XmNuserData, &pSetup->widgets, NULL);

    /*
     * Add the setup-specific callbacks.
     */
    XtAddCallback(
		pSetup->dtprintSetup,
		DtNcancelCallback,
		_psPrintSetupCancelCB,
		(XtPointer) pSetup);
    XtAddCallback(
		pSetup->dtprintSetup,
		DtNclosePrintDisplayCallback,
		pSetup->closeDisplayCB,
		(XtPointer) pSetup->closeDisplayClosure);
    XtAddCallback(
		pSetup->dtprintSetup,
		DtNprintCallback,
		_psPrintSetupPrintCB,
		(XtPointer) pSetup);
    XtAddCallback(
		pSetup->dtprintSetup,
		DtNsetupCallback,
		pSetup->setupCB,
		(XtPointer) pSetup->setupClosure);

    xms = XmStringCreateLocalized(pSetup->docName);
    XtVaSetValues(pSetup->widgets->docNameLabel, XmNlabelString, xms, NULL);
    XmStringFree(xms);

    dialog = XtParent(pSetup->widgets->dtprintSetup);
    if (NULL != dialog && XtIsShell(dialog))
    {
        titleParent = (char*) GETMESSAGE(7, 2, "Text Editor - ");
        titlePrefix = (char*) GETMESSAGE(14, 23, "Print Setup - ");
        title =
          (char*) XtMalloc(strlen(titleParent) + strlen(titlePrefix) + strlen(pSetup->docName) + 1);
        sprintf(title, "%s%s%s", titleParent, titlePrefix, pSetup->docName);
        XtVaSetValues(dialog, XmNtitle, title, NULL);
        XtFree(title);
    }
    else
    {
	fprintf(stderr, "Print Setup box is not parented to a shell\n");
    }

    is_set = (pSetup->options.wordWrap) ? XmSET : XmUNSET;
    XtVaSetValues(pSetup->widgets->wordWrapTB, XmNset, is_set, NULL);
}


/*
 * _psCreatePrintSetupDialog
 *	Creates the widgets for the Setup Dialog.
 */
static Widget
_psCreatePrintSetupDialog(Widget parent, Editor *pPad)
{
    Arg		args[2];
    int		nargs;
    psWidgets	*widgets;
    Widget	w;
    XmString	xms;
    unsigned char is_set;
    int		nitems;
    char	**strings;
    void	**data;
    int		i;

    /*
     * Create the widgets for the Setup Dialog.
     */
    widgets = (psWidgets *) XtMalloc(sizeof(psWidgets));

    /*
     * Create the DtPrintSetupDialog and specify that the application
     * specific area be located below the generic area.
     */
    nargs=0;
    XtSetArg(args[nargs], XmNautoUnmanage, FALSE); nargs++;
    widgets->dtprintSetup =
      DtCreatePrintSetupDialog(parent, "DtPrintSetup", args, nargs);

    XtVaSetValues(
	widgets->dtprintSetup,
	DtNworkAreaLocation, DtWORK_AREA_BOTTOM,
	XmNuserData, widgets,
	NULL);

    XtAddCallback(
		widgets->dtprintSetup,
		XmNhelpCallback,
		HelpPrintSetupDialogCB,
		(XtPointer) pPad);

    XtAddCallback(
		widgets->dtprintSetup,
		XmNdestroyCallback,
		_psPrintSetupDestroyCB,
		(XtPointer) widgets);
    /*
     * Create the dtpad-specific setup dialog in the bottom work area
     */
    widgets->form = XtVaCreateManagedWidget(
				"SetupForm",
				xmFormWidgetClass,
				widgets->dtprintSetup,
				NULL);
		 
    XtAddCallback(
		widgets->form,
		XmNhelpCallback,
		HelpPrintSetupAppSpecificCB,
		(XtPointer) pPad);

    /*
     * Create the document name title label
     */
    xms = XmStringCreateLocalized( GETMESSAGE(14, 12, "Document Name: ") );
    widgets->docLabel = XtVaCreateManagedWidget(
				"DocumentLable",
				xmLabelGadgetClass,
				widgets->form,
				XmNleftAttachment, XmATTACH_FORM,
				XmNtopAttachment, XmATTACH_FORM,
				XmNlabelString, xms,
			 	NULL);
    XmStringFree(xms);

    /*
     * Create the document name label
     */
    widgets->docNameLabel = XtVaCreateManagedWidget(
				"DocumentName",
			 	xmLabelGadgetClass,
			 	widgets->form,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, widgets->docLabel,
#if 0
				XmNrightAttachment, XmATTACH_FORM,
#endif
				XmNtopAttachment, XmATTACH_FORM,
			 	NULL);

    xms = XmStringCreateLocalized(GETMESSAGE(14, 13, "Wrap To Fit"));
    widgets->wordWrapTB = XtVaCreateManagedWidget(
				"WrapToFit",
				xmToggleButtonWidgetClass,
				widgets->form,
				XmNalignment, XmALIGNMENT_BEGINNING,
				XmNlabelString, xms,
				XmNleftAttachment, XmATTACH_FORM,
				XmNrightAttachment, XmATTACH_FORM,
				XmNtopAttachment, XmATTACH_WIDGET,
				XmNtopWidget, widgets->docLabel,
				NULL);
    XmStringFree(xms);

    /*
     *  Create GUI for the Header/Footer options
     */
    nitems = PS_ARRAY_SIZE(_psHdrFtrValues);
    strings = (char **) XtMalloc( nitems * sizeof(char*) );
    data = (void **) XtMalloc( nitems * sizeof(void*) );
    for (i=0; i<nitems; i++)
    {
        data[i] = (void*) _psHdrFtrValues[i].which;
        strings[i] = GETMESSAGE(
			_psHdrFtrValues[i].set_id, 
			_psHdrFtrValues[i].msg_id,
			_psHdrFtrValues[i].dflt_string
			);
    }

#if 0
    /*
     *  Create GUI for the Header/Footer options
     */
    widgets->hdrftrFrame =
      _DtPrintCreateHdrFtrFrame(widgets->form, nitems, strings, data);
    XtVaSetValues(
		widgets->hdrftrFrame,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, widgets->wordWrapTB,
		XmNleftAttachment, XmATTACH_FORM,
		NULL);
    XtManageChild(widgets->hdrftrFrame);

    /*
     *  Create GUI for the Margin options
     */
    widgets->marginFrame = _DtPrintCreateMarginFrame(widgets->form);
    XtVaSetValues(
		widgets->marginFrame,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, widgets->hdrftrFrame,
		XmNleftAttachment, XmATTACH_FORM,
		NULL);
    XtManageChild(widgets->marginFrame);
#else
    /*
     *  Create GUI for the Margin options
     */
    widgets->marginFrame = _DtPrintCreateMarginFrame(widgets->form);
    XtVaSetValues(
		widgets->marginFrame,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, widgets->wordWrapTB,
		XmNleftAttachment, XmATTACH_FORM,
		NULL);
    XtManageChild(widgets->marginFrame);

    /*
     *  Create GUI for the Header/Footer options
     */
    widgets->hdrftrFrame =
      _DtPrintCreateHdrFtrFrame(widgets->form, nitems, strings, data);
    XtVaSetValues(
		widgets->hdrftrFrame,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, widgets->wordWrapTB,
		XmNtopWidget, widgets->marginFrame,
		XmNleftAttachment, XmATTACH_FORM,
		NULL);
    XtManageChild(widgets->hdrftrFrame);
#endif

    XtAddCallback(
		widgets->hdrftrFrame,
		XmNhelpCallback,
		HelpPrintSetupPageHeadersFootersCB,
		(XtPointer) pPad);

    XtAddCallback(
		widgets->marginFrame,
		XmNhelpCallback,
		HelpPrintSetupPageMargins,
		(XtPointer) pPad);

    XtRealizeWidget(widgets->dtprintSetup);
    return widgets->dtprintSetup;
}


/*
 * _psReleasePrintSetupDialog
 *	Removes setup-specific callbacks.
 */
static void
_psReleasePrintSetupDialog(PrintSetup *pSetup)
{
    /*
     * Remove the setup-specific callbacks.
     */
    XtRemoveCallback(
		pSetup->dtprintSetup,
		DtNcancelCallback,
		_psPrintSetupCancelCB,
		(XtPointer) pSetup);
    XtRemoveCallback(
		pSetup->dtprintSetup,
		DtNclosePrintDisplayCallback,
		pSetup->closeDisplayCB,
		(XtPointer) pSetup->closeDisplayClosure);
    XtRemoveCallback(
		pSetup->dtprintSetup,
		DtNprintCallback,
		_psPrintSetupPrintCB,
		(XtPointer) pSetup);
    XtRemoveCallback(
		pSetup->dtprintSetup,
		DtNsetupCallback,
		pSetup->setupCB,
		(XtPointer) pSetup->setupClosure);

#ifdef REUSE_PRINT_SETUP_DIALOGS
    if (_psDefaultPrintSetupDialog != NULL)
    {
	/*
	 * Attempt to reuse the last print setup and print display connection.
	 */
        XtDestroyWidget(_psDefaultPrintSetupDialog);
	_psDefaultPrintSetupDialog = NULL;
    }
    _psDefaultPrintSetupDialog = pSetup->dtprintSetup;
#else
    XtDestroyWidget(pSetup->dtprintSetup);
#endif
}


/*
 * _psPrintSetupCancelCB
 */
static void
_psPrintSetupCancelCB(
		Widget dtprintSetup,
		XtPointer client_data,
		XtPointer call_data)
{
    PrintSetup *pSetup = (PrintSetup*) client_data;

    PrintSetupHide(pSetup);
    pSetup->cancelCB(dtprintSetup, pSetup->cancelClosure, call_data);
}


/*
 * _psPrintSetupDestroyCB
 */
static void
_psPrintSetupDestroyCB(
		Widget dtprintSetup,
		XtPointer client_data,
		XtPointer call_data)
{
    psWidgets	*widgets;

    XtVaGetValues(dtprintSetup, XmNuserData, &widgets, NULL);
    XtFree((XtPointer) widgets);
}


/*
 * _psPrintSetupPrintCB
 */
static void
_psPrintSetupPrintCB(
		Widget dtprintSetup,
		XtPointer client_data,
		XtPointer call_data)
{
    PrintSetup	*pSetup = (PrintSetup*) client_data;
    Boolean	error = FALSE;

    if (! _psUiSpecsAreValid(pSetup)) return;

    PrintSetupHide(pSetup);
    _psUiToSetup(pSetup);
    _psSetupToSource(pSetup);
    pSetup->printCB(dtprintSetup, pSetup->printClosure, call_data);
}
