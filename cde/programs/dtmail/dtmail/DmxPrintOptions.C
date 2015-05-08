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
/* $TOG: DmxPrintOptions.C /main/8 1997/04/30 09:44:12 mgreess $ */

/*
 *+SNOTICE
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1994 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */
/*
 *		     Common Desktop Environment
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


#include <stdio.h>
#include <stdint.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/ToggleB.h>
#include <Dt/PrintOptionsP.h>

#include "Dmx.h"
#include "DmxPrintOptions.h"
#include "DtMail.hh"
#include "DtMailError.hh"
#include "dtmailopts.h"
#include "IndexedOptionMenuUiItem.hh"
#include "MailMsg.h"
#include "RoamApp.h"
#include "TextFieldUiItem.hh"

#define DMX_ARRAY_SIZE(ary)	(sizeof((ary))/sizeof((ary)[0]))

static DmxpoPropKey hdrftr_keys[] =
{
    { (int) DTPRINT_OPTION_HEADER_LEFT,
      DMX_PROPKEY_HEADER_LEFT,
      DMX_PROPVAL_SUBJECT_HEADER
    },
    { (int) DTPRINT_OPTION_HEADER_RIGHT,
      DMX_PROPKEY_HEADER_RIGHT,
      DMX_PROPVAL_EMPTY
    },
    { (int) DTPRINT_OPTION_FOOTER_LEFT,
      DMX_PROPKEY_FOOTER_LEFT,
      DMX_PROPVAL_USER_NAME
    },
    { (int) DTPRINT_OPTION_FOOTER_RIGHT,
      DMX_PROPKEY_FOOTER_RIGHT,
      DMX_PROPVAL_PAGE_NUMBER
    }
};

static DmxpoPropKey margin_keys[] =
{
    { (int) DTPRINT_OPTION_MARGIN_TOP,
      DMX_PROPKEY_MARGIN_TOP,
      DMX_PROPVAL_DFLT_MARGIN
    },
    { (int) DTPRINT_OPTION_MARGIN_LEFT,
      DMX_PROPKEY_MARGIN_LEFT,
      DMX_PROPVAL_DFLT_MARGIN
    },
    { (int) DTPRINT_OPTION_MARGIN_BOTTOM,
      DMX_PROPKEY_MARGIN_BOTTOM,
      DMX_PROPVAL_DFLT_MARGIN
    },
    { (int) DTPRINT_OPTION_MARGIN_RIGHT,
      DMX_PROPKEY_MARGIN_RIGHT,
      DMX_PROPVAL_DFLT_MARGIN
    }
};

static DmxpoPropValue hdrftr_values[] =
{
    { (int) DMX_NONE_STRING,
      DMX_PROPVAL_EMPTY,
      22, 1, "Empty"
    },
    { (int) DMX_CC_HEADER_STRING,
      DMX_PROPVAL_CC_HEADER,
      22, 2, "CC Header"
    },
    { (int) DMX_DATE_HEADER_STRING,
      DMX_PROPVAL_DATE_HEADER,
      22, 3, "Date Header"
    },
    { (int) DMX_FROM_HEADER_STRING,
      DMX_PROPVAL_FROM_HEADER,
      22, 4, "From Header"
    },
    { (int) DMX_SUBJECT_HEADER_STRING,
      DMX_PROPVAL_SUBJECT_HEADER,
      22, 5, "Subject Header"
    },
    { (int) DMX_TO_HEADER_STRING,
      DMX_PROPVAL_TO_HEADER,
      22, 6, "To Header"
    },
    { (int) DMX_PAGE_NUMBER_STRING,
      DMX_PROPVAL_PAGE_NUMBER,
      22, 7, "Page Number"
    },
    { (int) DMX_USER_NAME_STRING,
      DMX_PROPVAL_USER_NAME,
      22, 8, "User Name"
    }
};
				    
static DmxpoPropValue prthdr_values[] =
{
    { (int) DMX_PRINT_HEADERS_NONE,
      DMX_PROPVAL_NONE,
      23, 1, "None" },
    { (int) DMX_PRINT_HEADERS_STANDARD,
      DMX_PROPVAL_STANDARD,
      23, 2, "Standard" },
    { (int) DMX_PRINT_HEADERS_ABBREV,
      DMX_PROPVAL_ABBREVIATED,
      23, 3, "Abbreviated" },
    { (int) DMX_PRINT_HEADERS_ALL,
      DMX_PROPVAL_ALL,
      23, 4, "All" }
};
				    
static DmxpoPropValue msgsep_values[] =
{
    { (int) DMX_SEPARATOR_NEW_LINE,
      DMX_PROPVAL_NEW_LINE,
      24, 1, "New Line" },
    { (int) DMX_SEPARATOR_BLANK_LINE,
      DMX_PROPVAL_BLANK_LINE,
      24, 2, "Blank Line" },
    { (int) DMX_SEPARATOR_CHARACTER_LINE,
      DMX_PROPVAL_CHARACTER_LINE,
      24, 3, "Character Line" },
    { (int) DMX_SEPARATOR_PAGE_BREAK,
      DMX_PROPVAL_PAGE_BREAK,
      24, 4, "New Page" }
};
				    


DmxPrintOptions::DmxPrintOptions (
				Widget parent
			 	) : UIComponent( "PrintOptions" )
{
    IndexedOptionMenu	*iom = (IndexedOptionMenu *) NULL;
    PropUiItem		*pui = (PropUiItem  *) NULL;
    Widget		*menu_buttons, w;
    int			nitems;
    char		**strings;
    void		**data;
    XmString		xms;
    int			i, j, nhdrftrs;

    _iom_array = new DtVirtArray<IndexedOptionMenu *>(10);
    _propui_array = new DtVirtArray<PropUiItem *>(10);
    _propui_array_iterator = 0;
    _parent = parent;

    //
    //  Create form to hold the printing options
    //
    _form = XtVaCreateWidget(
				"PrintingOptionsPane",
			  	xmFormWidgetClass,
			  	_parent,
			  	XmNbottomAttachment, XmATTACH_FORM,
			  	XmNrightAttachment, XmATTACH_FORM,
			  	XmNleftAttachment, XmATTACH_FORM,
			  	XmNtopAttachment, XmATTACH_FORM,
			  	XmNresizePolicy, XmRESIZE_ANY,
			  	NULL
				);
    if (_form == (Widget) NULL) return;
    _w = _form;
    installDestroyHandler();

    //
    //  Create GUI for the Header/Footer options
    //
    nitems = DMX_ARRAY_SIZE(hdrftr_values);;
    strings = (char **) XtMalloc( nitems * sizeof(char*) );
    data = (void **) XtMalloc( nitems * sizeof(void*) );
    for (i=0; i<nitems; i++)
    {
        data[i] = (void*) hdrftr_values[i].prop_string;
        strings[i] = GETMSG(
			DT_catd, 
			hdrftr_values[i].set_id, 
			hdrftr_values[i].msg_id,
			hdrftr_values[i].dflt_string
			);
    }

    _hdrftr_frame = _DtPrintCreateHdrFtrFrame( _form, nitems, strings, data);
    XtVaSetValues(
		_hdrftr_frame,
		XmNtopAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		NULL
		);
    XtManageChild(_hdrftr_frame);

    //
    //  Create PropUiItem's for the Header/Footer options
    //
    menu_buttons = NULL;
    _DtPrintHdrFtrFrameMenuWidgets( _hdrftr_frame, NULL, NULL, &menu_buttons);
    for (j=0, nhdrftrs=DMX_ARRAY_SIZE(hdrftr_keys); j<nhdrftrs; j++)
    {
        w = _DtPrintHdrFtrFrameEnumToWidget(
				_hdrftr_frame,
				(_DtPrintHdrFtrEnum) hdrftr_keys[j].which
				);
        iom =
	  new IndexedOptionMenu(w, nitems, (char**)strings, data, menu_buttons);
	iom->manage();
        _iom_array->append(iom);

        pui = (PropUiItem *) new IndexedOptionMenuUiItem(
							iom,
							_FROM_MAILRC,
							hdrftr_keys[j].key
							);
        _propui_array->append(pui);
    }
    XtFree((char*) menu_buttons);
    XtFree((char*) data);
    XtFree((char*) strings);


    //
    //  Create GUI for the Margin options
    //
    _margin_frame = _DtPrintCreateMarginFrame(_form);
    XtVaSetValues(
		_margin_frame,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, _hdrftr_frame,
		XmNleftAttachment, XmATTACH_FORM,
		NULL
		);
    XtManageChild(_margin_frame);

    //
    //  Create PropUiItem's for the Margin options
    //
    nitems = DMX_ARRAY_SIZE(margin_keys);
    for (j=0; j<nitems; j++)
    {
        w = _DtPrintMarginFrameEnumToWidget(
				_margin_frame,
				(_DtPrintMarginEnum) margin_keys[j].which
				);
        pui = (PropUiItem *) new TextFieldUiItem(
					w,
					_FROM_MAILRC,
					margin_keys[j].key,
					DmxPrintOptions::isValidMarginSpec,
					(void*) (intptr_t) margin_keys[j].which);
        _propui_array->append(pui);
    }


    //
    //  Create GUI for the Printed Headers option
    //
    nitems = DMX_ARRAY_SIZE(prthdr_values);;
    strings = (char **) XtMalloc( nitems * sizeof(char*) );
    data = (void **) XtMalloc( nitems * sizeof(void*) );
    for (i=0; i<nitems; i++)
    {
        data[i] = (void*) prthdr_values[i].prop_string;
        strings[i] = GETMSG(
			DT_catd, 
			prthdr_values[i].set_id, 
			prthdr_values[i].msg_id,
			prthdr_values[i].dflt_string
			);
    }

    iom = new IndexedOptionMenu(_form, nitems, (char**) strings, data);
    xms = XmStringCreateLocalized(
		GETMSG(DT_catd, 25, 1, "Printed Message Headers:  ")
		);
    XtVaSetValues(
		iom->baseWidget(),
		XmNlabelString, xms,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, _margin_frame,
		XmNrightAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		NULL
		);
    iom->manage();
    _prthdr_iom = iom;
    _iom_array->append(iom);

    XmStringFree(xms);
    XtFree((char*) strings);
    XtFree((char*) data);

    //
    //  Create PropUiItem for the Printed Headers option
    //
    pui = (PropUiItem *) new IndexedOptionMenuUiItem(
						iom,
						_FROM_MAILRC,
						DMX_PROPKEY_PRINT_HEADERS
						);
    _propui_array->append(pui);


    //
    //  Create GUI for the Message Separator option
    //
    nitems = DMX_ARRAY_SIZE(msgsep_values);;
    strings = (char **) XtMalloc( nitems * sizeof(char*) );
    data = (void **) XtMalloc( nitems * sizeof(void*) );
    for (i=0; i<nitems; i++)
    {
        data[i] = (void*) msgsep_values[i].prop_string;
        strings[i] = GETMSG(
			DT_catd, 
			msgsep_values[i].set_id, 
			msgsep_values[i].msg_id,
			msgsep_values[i].dflt_string
			);
    }

    iom = new IndexedOptionMenu(_form, nitems, (char**) strings, data);
    xms = XmStringCreateLocalized(
		GETMSG( DT_catd, 25, 2, "Separate Multiple Messages With:  ")
		);
    XtVaSetValues(
		iom->baseWidget(),
		XmNlabelString, xms,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, _prthdr_iom->baseWidget(),
		XmNrightAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		XmNbottomAttachment, XmATTACH_FORM,
		NULL
		);
    iom->manage();
    _iom_array->append(iom);
    _msgsep_iom = iom;

    XmStringFree(xms);
    XtFree((char*) strings);
    XtFree((char*) data);

    //
    //  Create PropUiItem for the Message Separator option
    //
    pui = (PropUiItem *) new IndexedOptionMenuUiItem(
						iom,
						_FROM_MAILRC,
						DMX_PROPKEY_MESSAGE_SEPARATOR);
    _propui_array->append(pui);

    //XtRealizeWidget(_w);
    //XtManageChild(_w);
}


DmxPrintOptions::~DmxPrintOptions (void)
{
    int i;
    PropUiItem		*pui;;
    IndexedOptionMenu	*iom;

    if (_propui_array)
      for (i=0; i<_propui_array->length(); i++)
      {
          pui = (*_propui_array)[i];
	  delete pui;
      }

    if (_iom_array)
      for (i=0; i<_iom_array->length(); i++)
      {
          iom = (*_iom_array)[i];
	  delete iom;
      }

    if (_w)
      XtDestroyWidget(_w);
}

PropUiItem *
DmxPrintOptions::getFirstProp(void)
{
    _propui_array_iterator = 0;
    return getNextProp();
}

PropUiItem *
DmxPrintOptions::getNextProp(void)
{
    PropUiItem	*pui = (PropUiItem  *) NULL;

    if (_propui_array_iterator < _propui_array->length())
    {
	pui = (*_propui_array)[_propui_array_iterator];
	_propui_array_iterator++;
    }
    return pui;
}

int
DmxPrintOptions::getNumProps(void)
{
    return _propui_array->length();
}

const char *
DmxPrintOptions::getSeparatorString(void)
{
    DtMail::Session	*d_session = NULL;
    DtMail::MailRc	*m_rc = NULL;
    DtMailEnv		error;
    const char		*string = NULL;
    char		*dflt = "-";

    d_session = theRoamApp.session()->session();
    m_rc = d_session->mailRc(error);

    m_rc->getValue(error, DMX_PROPKEY_SEPARATOR_STRING, &string);
    if (string == NULL || error.isSet())
      return strdup(dflt);

    return string;
}

DmxStringTypeEnum
DmxPrintOptions::getHdrFtrSpec(_DtPrintHdrFtrEnum which)
{
    DtMail::Session	*d_session = NULL;
    DtMail::MailRc	*m_rc = NULL;
    DtMailEnv		error;
    DmxpoPropKey	*key;
    const char		*string = NULL;
    DmxpoPropValue	*value = NULL;
    DmxStringTypeEnum	rtn = DMX_NONE_STRING;

    d_session = theRoamApp.session()->session();
    m_rc = d_session->mailRc(error);

    key = DmxPrintOptions::enumToPropKey(
				which,
				DMX_ARRAY_SIZE(hdrftr_keys),
				hdrftr_keys
				);

    if (key != NULL)
    {
        m_rc->getValue(error, key->key , &string);
        if (string == NULL || error.isSet())
          string = strdup(key->dflt_prop_string);
    }
    else
      return rtn;

    value = stringToPropValue(
			string,
			DMX_ARRAY_SIZE(hdrftr_values),
			hdrftr_values);
    if (value != NULL)
      rtn = (DmxStringTypeEnum) value->which;

    if (NULL != string)
      free((void*) string);

    return rtn;
}

const char *
DmxPrintOptions::getMarginSpec(_DtPrintMarginEnum which)
{
    DtMail::Session	*d_session = NULL;
    DtMail::MailRc	*m_rc = NULL;
    DtMailEnv		error;
    DmxpoPropKey	*key;
    const char		*string = NULL;

    d_session = theRoamApp.session()->session();
    m_rc = d_session->mailRc(error);

    key = DmxPrintOptions::enumToPropKey(
				which,
				DMX_ARRAY_SIZE(margin_keys),
				margin_keys
				);

    if (key == NULL)
      return DMX_PROPVAL_DFLT_MARGIN;

    m_rc->getValue(error, key->key , &string);
    if (string == NULL || error.isSet())
      string = strdup(key->dflt_prop_string);

    return string;
}

DmxMsgSeparatorEnum
DmxPrintOptions::getMessageSeparator(void)
{
    DtMail::Session	*d_session = NULL;
    DtMail::MailRc	*m_rc = NULL;
    DtMailEnv		error;
    const char		*string = NULL;
    DmxpoPropValue	*value = NULL;
    DmxMsgSeparatorEnum	rtn = DMX_SEPARATOR_PAGE_BREAK;

    d_session = theRoamApp.session()->session();
    m_rc = d_session->mailRc(error);

    m_rc->getValue(error, DMX_PROPKEY_MESSAGE_SEPARATOR , &string);
    if (string == NULL || error.isSet())
      return rtn;

    value = stringToPropValue(
			string,
			DMX_ARRAY_SIZE(msgsep_values),
			msgsep_values);
    if (value != NULL)
      rtn = (DmxMsgSeparatorEnum) value->which;

    if (NULL != string)
      free((void*) string);

    return rtn;
}

DmxPrintHeadersEnum
DmxPrintOptions::getPrintedHeaders(void)
{
    DtMail::Session	*d_session = NULL;
    DtMail::MailRc	*m_rc = NULL;
    DtMailEnv		error;
    const char		*string = NULL;
    DmxpoPropValue	*value = NULL;
    DmxPrintHeadersEnum	rtn = DMX_PRINT_HEADERS_STANDARD;

    d_session = theRoamApp.session()->session();
    m_rc = d_session->mailRc(error);

    m_rc->getValue(error, DMX_PROPKEY_PRINT_HEADERS , &string);
    if (string == NULL || error.isSet())
      return rtn;

    value = stringToPropValue(
			string,
			DMX_ARRAY_SIZE(prthdr_values),
			prthdr_values);
    if (value != NULL)
      rtn = (DmxPrintHeadersEnum) value->which;

    if (NULL != string)
      free((void*) string);

    return rtn;
}

DmxpoPropKey *
DmxPrintOptions::enumToPropKey(int which, int nkeys, DmxpoPropKey *keys)
{
    for (int i=0; i<nkeys; i++)
      if (keys[i].which == which)
	return &(keys[i]);
    
    return NULL;
}

PropUiItem *
DmxPrintOptions::propKeyToPropItem(char *key)
{
    for (int i=0; i<_propui_array->length(); i++)
    {
	PropUiItem	*pui;
	char		*puikey;

	pui = (*_propui_array)[i];
	puikey = pui->getKey();
        if (strcmp(puikey, key) == 0)
	  return pui;
    }
    
    return NULL;
}

DmxpoPropValue *
DmxPrintOptions::stringToPropValue(
				const char *string,
				int nvalues,
				DmxpoPropValue *values
				)
{
    for (int i=0; i<nvalues; i++)
      if (strcmp(values[i].prop_string, string) == 0)
	return &(values[i]);
    
    return NULL;
}

char *
DmxPrintOptions::isValidMarginSpec(PropUiItem* pui, void* data)
{
    char        *i18nMsg;
    char	*errMsg = NULL;
    char	*marginSpec = NULL;
    XtEnum	parseError;
    Widget	text;

    _DtPrintMarginEnum which = (_DtPrintMarginEnum)(long)data;

    text = pui->getWidget();
    if (text)
      marginSpec = _DtPrintGetMarginSpec(text);

    parseError = FALSE;
    if (marginSpec == NULL || strcmp("", marginSpec) == 0)
      parseError = TRUE;
    else 
      (void) XmConvertStringToUnits(
                                XtScreenOfObject(text),
                                marginSpec,
                                XmVERTICAL,
                                XmPIXELS,
                                &parseError);
 
    if (! parseError) return NULL;
 
    switch (which)
    {
      case DTPRINT_OPTION_MARGIN_TOP:
        i18nMsg =
	  GETMSG(DT_catd, 26, 1, "Top Margin specifier is invalid:  ");
	break;
      case DTPRINT_OPTION_MARGIN_RIGHT:
        i18nMsg =
	  GETMSG(DT_catd, 26, 2, "Right Margin specifier is invalid:  ");
	break;
      case DTPRINT_OPTION_MARGIN_BOTTOM:
        i18nMsg =
	  GETMSG(DT_catd, 26, 3, "Bottom Margin specifier is invalid:  ");
	break;
      case DTPRINT_OPTION_MARGIN_LEFT:
        i18nMsg =
	  GETMSG(DT_catd, 26, 4, "Left Margin specifier is invalid:  ");
	break;
    }
    errMsg = (char*) XtMalloc(strlen(i18nMsg) + strlen(marginSpec) + 1);
    sprintf(errMsg, "%s%s", i18nMsg, marginSpec);
    
    return errMsg;
}
