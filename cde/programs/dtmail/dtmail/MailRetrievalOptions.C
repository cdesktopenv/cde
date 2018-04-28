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
/* $TOG: MailRetrievalOptions.C /main/8 1998/07/23 18:13:00 mgreess $ */

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
#include <sys/param.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/XmAll.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/ToggleB.h>
#include <Dt/PrintOptionsP.h>

#include <DtMail/IO.hh>
#include "CheckBoxUiItem.hh"
#include "DtMail.hh"
#include "DtMailError.hh"
#include "InboxTextFieldUiItem.hh"
#include "IndexedOptionMenuUiItem.hh"
#include "MailMsg.h"
#include "MailRetrievalOptions.h"
#include "RoamApp.h"
#include "CheckForMailUiItem.hh"
#include "SpinBoxUiItem.hh"
#include "TextFieldUiItem.hh"
#include "dtmailopts.h"

#define MR_ARRAY_SIZE(ary)	(sizeof((ary))/sizeof((ary)[0]))

static MrPropVal serverprotocol_values[] =
{
    { DTMAS_PROTO_AUTO,
      27, 1, "Auto"
    },
    { DTMAS_PROTO_POP3,
      27, 2, "POP3"
    },
    { DTMAS_PROTO_APOP,
      27, 3, "APOP"
    },
    { DTMAS_PROTO_IMAP,
      27, 4, "IMAP"
    },
    { DTMAS_PROTO_POP2,
      27, 5, "POP2"
    },
};

#define MrGENPROPKEY(pk, fld, key) DTMAS_CONCAT_MAILRC_KEY(pk, fld, key)

MailRetrievalOptions::MailRetrievalOptions (
				Widget		parent,
				const char	*foldername
			 	) : UIComponent( "PrintOptions" )
{
    DtMail::Session	*d_session = theRoamApp.session()->session();
    PropUiItem		*pui = (PropUiItem  *) NULL;
    Widget		*menu_buttons, w;
    int			n, nitems;
    char		**strings;
    void		**data;
    XmString		xms;
    char		*propkey = new char[MAXPATHLEN];

    _password_pui = NULL;
    _inboxpath_label = NULL;
    _inboxpath_tf = NULL;
    _checkfornewmail_label = NULL;
    _checkfornewmail_sb = NULL;
    _system_tb = NULL;
    _server_frame = NULL;
    _server_tb = NULL;
    _serverframe_form = NULL;
    _serverprotocol_label = NULL;
    _serverprotocol_iom = NULL;
    _servername_label = NULL;
    _servername_tf = NULL;
    _username_label = NULL;
    _username_tf = NULL;
    _password_label = NULL;
    _password_tf = NULL;
    _rememberpassword_tb = NULL;
    _removeafterdelivery_tb = NULL;
    _retrieveold_tb = NULL;
    _custom_frame = NULL;
    _custom_tb = NULL;
    _customframe_form = NULL;
    _customcommand_label = NULL;
    _customcommand_tf = NULL;
    

    _propui_array = new DtVirtArray<PropUiItem *>(10);
    _propui_array_iterator = 0;
    _parent = parent;
    if (NULL != foldername)
      _foldername = strdup(foldername);
    else
      _foldername = strdup(DTMAS_INBOX);

    _retrieval_tbs = new DtVirtArray<Widget>(10);
    _custom_options = new DtVirtArray<Widget>(10);
    _server_options = new DtVirtArray<Widget>(10);

    //
    //  Create FORM to hold the printing options
    //
    _form = XtVaCreateWidget(
				"MailRetrievalOptions",
			  	xmFormWidgetClass,
			  	_parent,
			  	XmNbottomAttachment, XmATTACH_FORM,
			  	XmNrightAttachment, XmATTACH_FORM,
			  	XmNleftAttachment, XmATTACH_FORM,
			  	XmNtopAttachment, XmATTACH_FORM,
			  	XmNresizePolicy, XmRESIZE_ANY,
			  	XmNfractionBase, 3,
			  	NULL);
    if (_form == (Widget) NULL)
    {
	delete [] propkey;
	return;
    }
    _w = _form;
    installDestroyHandler();

    //
    //  Create GUI for the INBOX Name Option
    //
    _inboxpath_tf = XtVaCreateManagedWidget(
				"InboxNameText",
				xmTextFieldWidgetClass, _form,
    				XmNtopAttachment, XmATTACH_FORM,
				XmNtopOffset, 10,
				XmNleftAttachment, XmATTACH_POSITION,
				XmNleftPosition, 1,
    				XmNrightAttachment, XmATTACH_NONE,
				XmNcolumns, 30,
				XmNvalue, "",
				NULL);
    xms = XmStringCreateLocalized(GETMSG(DT_catd, 27, 18,"INBOX folder path:"));
    _inboxpath_label = XtVaCreateManagedWidget(
				"InboxNameLabel",
				xmLabelWidgetClass, _form,
    				XmNlabelString, xms,
    				XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
    				XmNtopOffset, 0,
    				XmNtopWidget, _inboxpath_tf,
    				XmNleftAttachment, XmATTACH_NONE,
				XmNrightAttachment, XmATTACH_WIDGET,
    				XmNrightOffset, 3,
				XmNrightWidget, _inboxpath_tf,
				NULL);
    XmStringFree(xms);

    //
    //  Create PropUiItem for the ServerName Option
    //
    DtMailObjectSpace	space;
    DtMailEnv		error;
    char		*dflt_inboxpath = NULL;
    d_session->queryImpl(
			error,
			d_session->getDefaultImpl(error),
			DtMailCapabilityInboxName,
			&space, &dflt_inboxpath);

    //MrGENPROPKEY(propkey, "", DTMAS_PROPKEY_INBOXPATH);
    strcpy(propkey, DTMAS_PROPKEY_INBOXPATH);
    PropSource::setDefaultValue(propkey, dflt_inboxpath);
    pui = (PropUiItem *) new InboxTextFieldUiItem(
					_inboxpath_tf,
					_FROM_MAILRC,
					propkey,
					MailRetrievalOptions::isValidInboxPath);
    _propui_array->append(pui);
    free(dflt_inboxpath);

    //
    //  Create GUI for the CheckForNewMail Option
    //
    _checkfornewmail_sb = XtVaCreateManagedWidget(
				"CheckForNewMailSB",
                		xmSimpleSpinBoxWidgetClass, _form,
    				XmNtopAttachment, XmATTACH_WIDGET,
    				XmNtopWidget, _inboxpath_tf,
				XmNtopOffset, 10,
				XmNleftAttachment, XmATTACH_POSITION,
				XmNleftPosition, 1,
    				XmNrightAttachment, XmATTACH_NONE,
        			XmNdecimalPoints, 0,
        			XmNincrementValue, 1,
        			XmNposition, 500,
        			XmNmaximumValue, 999999,
        			XmNminimumValue, 0,
        			XmNarrowLayout, XmARROWS_END,
        			XmNspinBoxChildType, XmNUMERIC,
        			XmNcolumns, 6,
				NULL);

    xms = XmStringCreateLocalized(
		GETMSG(Dtb_project_catd, 2, 13, "Check for new mail every:"));
    _checkfornewmail_label = XtVaCreateManagedWidget(
				"CheckForNewMailLabel",
                		xmLabelWidgetClass, _form,
                		XmNlabelType, XmSTRING,
                		XmNlabelString, xms,
    				XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
    				XmNtopOffset, 0,
    				XmNtopWidget, _checkfornewmail_sb,
    				XmNleftAttachment, XmATTACH_NONE,
				XmNrightAttachment, XmATTACH_WIDGET,
    				XmNrightOffset, 3,
				XmNrightWidget, _checkfornewmail_sb,
                		NULL);
    XmStringFree(xms);

    //
    //  Create PropUiItem for the CheckForNewMail Option
    //
    strcpy(propkey, "retrieveinterval");
    PropSource::setDefaultValue(propkey, "300");
    pui = (PropUiItem*) new CheckForMailUiItem(
					_checkfornewmail_sb,
					_FROM_MAILRC,
					propkey);
    _propui_array->append(pui);

    //
    //  Create separator
    //
    w = XtVaCreateManagedWidget(
				"MailRetrievalSeparator",
				xmSeparatorGadgetClass, _form,
			  	XmNrightAttachment, XmATTACH_FORM,
			  	XmNleftAttachment, XmATTACH_FORM,
			  	XmNtopAttachment, XmATTACH_WIDGET,
			  	XmNtopWidget, _checkfornewmail_sb,
				XmNtopOffset, 10,
				NULL);

    xms = XmStringCreateLocalized(
		GETMSG(DT_catd, 27, 6, "Automatic (system) delivery"));
    _system_tb = XtVaCreateManagedWidget(
				"SystemTB",
				xmToggleButtonGadgetClass, _form,
			  	XmNrightAttachment, XmATTACH_FORM,
			  	XmNleftAttachment, XmATTACH_FORM,
			  	XmNleftOffset, 18,
			  	XmNtopAttachment, XmATTACH_WIDGET,
			  	XmNtopWidget, w,
				XmNtopOffset, 10,
				XmNalignment, XmALIGNMENT_BEGINNING,
				XmNlabelString, xms,
				XmNset, True,
				NULL);
    XmStringFree(xms);

    XtAddCallback(
		_system_tb,
		XmNvalueChangedCallback,
		MailRetrievalOptions::retrievalTBSValueChangedCB,
		(XtPointer) this);

    //
    //  Create PropUiItem for the System Option
    //
    MrGENPROPKEY(propkey, _foldername, DTMAS_PROPKEY_GETMAILVIASYSTEM);
    PropSource::setDefaultValue(propkey, DTMAS_PROPDFLT_GETMAILVIASYSTEM);
    pui = (PropUiItem *) new CheckBoxUiItem(_system_tb, _FROM_MAILRC, propkey);
    _propui_array->append(pui);

    //
    //  Create FRAME to hold the server printing options
    //
    _server_frame = XtVaCreateManagedWidget(
				"ServerFrame", xmFrameWidgetClass, _form,
				XmNchildHorizontalAlignment,
							XmALIGNMENT_BEGINNING,
				XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
				XmNtopAttachment, XmATTACH_WIDGET,
				XmNtopWidget, _system_tb,
				XmNleftAttachment, XmATTACH_FORM,
				XmNleftOffset, 10,
				XmNrightAttachment, XmATTACH_FORM,
				XmNrightOffset, 10,
				XmNbottomAttachment, XmATTACH_NONE,
				NULL);

    xms = XmStringCreateLocalized(
		GETMSG(DT_catd, 27, 7, "Mail server retrieval"));
    _server_tb = XtVaCreateManagedWidget(
				"UseServerTB",
				xmToggleButtonGadgetClass, _server_frame,
				XmNchildType, XmFRAME_TITLE_CHILD,
				XmNchildHorizontalAlignment,
							XmALIGNMENT_BEGINNING,
				XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
				XmNlabelString, xms,
				XmNset, False,
				NULL);
    XmStringFree(xms);

    XtAddCallback(
		_server_tb,
		XmNvalueChangedCallback,
		MailRetrievalOptions::retrievalTBSValueChangedCB,
		(XtPointer) this);

    //
    //  Create PropUiItem for the Server Option
    //
    MrGENPROPKEY(propkey, _foldername, DTMAS_PROPKEY_GETMAILVIASERVER);
    PropSource::setDefaultValue(propkey, DTMAS_PROPDFLT_GETMAILVIASERVER);
    pui = (PropUiItem *) new CheckBoxUiItem(_server_tb, _FROM_MAILRC, propkey);
    _propui_array->append(pui);

    _serverframe_form = XtVaCreateManagedWidget(
				"ServerFrameForm",
			  	xmFormWidgetClass,
			  	_server_frame,
			  	//XmNallowOverlap, False,
			  	XmNfractionBase, 3,
			  	XmNresizePolicy, XmRESIZE_ANY,
			  	NULL);

    //
    //  Create GUI for the MailServerProtocol Option
    //
    nitems = MR_ARRAY_SIZE(serverprotocol_values);
    strings = (char**) XtMalloc(nitems*sizeof(char*));
    data = (void**) XtMalloc(nitems*sizeof(void*));
    for (int i=0; i<nitems; i++)
    {
        data[i] = (void*) serverprotocol_values[i].prop_value_string;
        strings[i] = GETMSG(
			DT_catd, 
			serverprotocol_values[i].set_id, 
			serverprotocol_values[i].msg_id,
			serverprotocol_values[i].dflt_gui_string);
    }

    _serverprotocol_iom =
      new IndexedOptionMenu(_serverframe_form, nitems, (char**) strings, data);
    XtVaSetValues(
		_serverprotocol_iom->baseWidget(),
    		XmNtopAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNleftPosition, 1,
    		XmNrightAttachment, XmATTACH_NONE,
		NULL);
    _serverprotocol_iom->manage();
    _serverprotocol_iom->setSpec((void*) DTMAS_PROPDFLT_PROTOCOL);
    XtFree((char*) strings);
    XtFree((char*) data);

    xms = XmStringCreateLocalized(
		GETMSG(DT_catd, 27, 8, "Server Protocol:"));
    _serverprotocol_label = XtVaCreateManagedWidget(
			"ServerProtocolLabel",
			xmLabelWidgetClass, _serverframe_form,
    			XmNlabelString, xms,
    			XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
    			XmNtopOffset, 0,
    			XmNtopWidget, _serverprotocol_iom->baseWidget(),
    			XmNleftAttachment, XmATTACH_NONE,
			XmNrightAttachment, XmATTACH_WIDGET,
    			XmNrightOffset, 3,
			XmNrightWidget, _serverprotocol_iom->baseWidget(),
			NULL);
    XmStringFree(xms);

    //
    //  Create PropUiItem for the Printed Headers option
    //
    MrGENPROPKEY(propkey, _foldername, DTMAS_PROPKEY_PROTOCOL);
    PropSource::setDefaultValue(propkey, DTMAS_PROPDFLT_PROTOCOL);
    pui = (PropUiItem *) new
	  IndexedOptionMenuUiItem(_serverprotocol_iom, _FROM_MAILRC, propkey);
    _propui_array->append(pui);

    //
    //  Create GUI for the ServerName Option
    //
    _servername_tf = XtVaCreateManagedWidget(
			"ServerNameText",
			xmTextFieldWidgetClass, _serverframe_form,
    			XmNtopAttachment, XmATTACH_WIDGET,
    			XmNtopWidget, _serverprotocol_iom->baseWidget(),
			XmNleftAttachment, XmATTACH_POSITION,
			XmNleftPosition, 1,
    			XmNrightAttachment, XmATTACH_NONE,
			XmNcolumns, 30,
			XmNvalue, "",
			NULL);
    xms = XmStringCreateLocalized(GETMSG(DT_catd, 27, 9, "Server Name:"));
    _servername_label = XtVaCreateManagedWidget(
			"ServerNameLabel",
			xmLabelWidgetClass, _serverframe_form,
    			XmNlabelString, xms,
    			XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
    			XmNtopOffset, 0,
    			XmNtopWidget, _servername_tf,
    			XmNleftAttachment, XmATTACH_NONE,
			XmNrightAttachment, XmATTACH_WIDGET,
    			XmNrightOffset, 3,
			XmNrightWidget, _servername_tf,
			NULL);
    XmStringFree(xms);

    //
    //  Create PropUiItem for the ServerName Option
    //
    MrGENPROPKEY(propkey, _foldername, DTMAS_PROPKEY_SERVERNAME);
    PropSource::setDefaultValue(propkey, DTMAS_PROPDFLT_SERVERNAME);
    pui = (PropUiItem *) new
	  TextFieldUiItem(_servername_tf, _FROM_MAILRC, propkey);
    _propui_array->append(pui);

    //
    //  Create GUI for the UserName Option
    //
    _username_tf = XtVaCreateManagedWidget(
			"UserNameText",
			xmTextFieldWidgetClass, _serverframe_form,
    			XmNtopAttachment, XmATTACH_WIDGET,
    			XmNtopWidget, _servername_tf,
			XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
			XmNleftWidget, _servername_tf,
    			XmNrightAttachment, XmATTACH_NONE,
			XmNcolumns, 30,
			XmNvalue, "",
			NULL);
    xms = XmStringCreateLocalized(GETMSG(DT_catd, 27, 10, "User Name:"));
    _username_label = XtVaCreateManagedWidget(
			"UserNameLabel",
			xmLabelWidgetClass, _serverframe_form,
    			XmNlabelString, xms,
    			XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
    			XmNtopWidget, _username_tf,
    			XmNleftAttachment, XmATTACH_NONE,
			XmNrightAttachment, XmATTACH_WIDGET,
			XmNrightWidget, _username_tf,
			NULL);
    XmStringFree(xms);
 
    //
    //  Create PropUiItem for the UserName Option
    //
    struct passwd pwd;
    GetPasswordEntry(pwd);

    MrGENPROPKEY(propkey, _foldername, DTMAS_PROPKEY_USERNAME);
    PropSource::setDefaultValue(propkey, pwd.pw_name);
    pui = (PropUiItem *) new
	  TextFieldUiItem(_username_tf, _FROM_MAILRC, propkey);
    _propui_array->append(pui);

    //
    //  Create GUI for the Password Option
    //
    _password_tf = XtVaCreateManagedWidget(
			"PasswordText",
			xmTextFieldWidgetClass, _serverframe_form,
    			XmNtopAttachment, XmATTACH_WIDGET,
    			XmNtopWidget, _username_tf,
			XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
			XmNleftWidget, _username_tf,
    			XmNrightAttachment, XmATTACH_NONE,
			XmNcolumns, 30,
			XmNvalue, "",
			NULL);
    xms = XmStringCreateLocalized(GETMSG(DT_catd, 27, 11, "Password:"));
    _password_label = XtVaCreateManagedWidget(
			"PasswordLabel",
			xmLabelWidgetClass, _serverframe_form,
    			XmNlabelString, xms,
    			XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
    			XmNtopWidget, _password_tf,
    			XmNleftAttachment, XmATTACH_NONE,
    			XmNrightAttachment, XmATTACH_WIDGET,
    			XmNrightWidget, _password_tf,
			NULL);
    XmStringFree(xms);
 

    //
    // Create a PropUiItem for the Password Option
    //
    MrGENPROPKEY(propkey, _foldername, DTMAS_PROPKEY_PASSWORD);
    PropSource::setDefaultValue(propkey, DTMAS_PROPDFLT_PASSWORD);
    _password_pui = new
	      EncryptedTextFieldUiItem(_password_tf, _FROM_MAILRC, propkey);
    _propui_array->append((PropUiItem*) _password_pui);

    //
    //  Create GUI for the Remember Password Option
    //
    xms = XmStringCreateLocalized(
		GETMSG(DT_catd, 27, 12, "Remember password"));
    _rememberpassword_tb = XtVaCreateManagedWidget(
			"RememberPasswordTB",
			xmToggleButtonWidgetClass, _serverframe_form,
    			XmNtopAttachment, XmATTACH_WIDGET,
    			XmNtopWidget, _password_tf,
    			XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
    			XmNleftWidget, _password_tf,
    			XmNrightAttachment, XmATTACH_NONE,
    			XmNalignment, XmALIGNMENT_END,
    			XmNlabelString, xms,
    			XmNset, False,
			NULL);
    XmStringFree(xms);

    XtAddCallback(
		_rememberpassword_tb,
		XmNvalueChangedCallback,
		MailRetrievalOptions::rememberPasswordChangedCB,
		(XtPointer) this);

    //
    //  Create PropUiItem for the RememberPassword Option
    //
    MrGENPROPKEY(propkey, _foldername, DTMAS_PROPKEY_REMEMBERPASSWORD);
    PropSource::setDefaultValue(propkey, DTMAS_PROPDFLT_REMEMBERPASSWORD);
    pui = (PropUiItem *) new
	  CheckBoxUiItem(_rememberpassword_tb, _FROM_MAILRC, propkey);
    _propui_array->append(pui);

    //
    //  Create GUI for the Delete From Server Option
    //
    xms = XmStringCreateLocalized(
		GETMSG(DT_catd, 27, 13,
		       "Delete from server after retrieval"));
    _removeafterdelivery_tb = XtVaCreateManagedWidget(
			"RemoveAfterDeliveryTB",
			xmToggleButtonWidgetClass, _serverframe_form,
    			XmNtopAttachment, XmATTACH_WIDGET,
    			XmNtopWidget, _rememberpassword_tb,
    			XmNleftAttachment, XmATTACH_POSITION,
    			XmNleftPosition, 1,
    			XmNrightAttachment, XmATTACH_NONE,
    			XmNalignment, XmALIGNMENT_BEGINNING,
    			XmNlabelString, xms,
    			XmNset, True,
			NULL);
    XmStringFree(xms);

    //
    //  Create PropUiItem for the RemoveAfterDelivery Option
    //
    MrGENPROPKEY(propkey, _foldername, DTMAS_PROPKEY_REMOVEAFTERDELIVERY);
    PropSource::setDefaultValue(propkey, DTMAS_PROPDFLT_REMOVEAFTERDELIVERY);
    pui = (PropUiItem *) new
	  CheckBoxUiItem(_removeafterdelivery_tb, _FROM_MAILRC, propkey);
    _propui_array->append(pui);

    //
    //  Create GUI for the RetrieveOld Option
    //
    xms = XmStringCreateLocalized(
		GETMSG(DT_catd, 27, 14,
		       "Retrieve old messages"));
    _retrieveold_tb = XtVaCreateManagedWidget(
			"RetrieveOldTB",
			xmToggleButtonWidgetClass, _serverframe_form,
    			XmNtopAttachment, XmATTACH_WIDGET,
    			XmNtopWidget, _removeafterdelivery_tb,
    			XmNleftAttachment, XmATTACH_POSITION,
    			XmNleftPosition, 1,
    			XmNrightAttachment, XmATTACH_NONE,
    			XmNalignment, XmALIGNMENT_BEGINNING,
    			XmNlabelString, xms,
    			XmNset, True,
			NULL);
    XmStringFree(xms);

    //
    //  Create PropUiItem for the RetrieveOld Option
    //
    MrGENPROPKEY(propkey, _foldername, DTMAS_PROPKEY_RETRIEVEOLD);
    PropSource::setDefaultValue(propkey, DTMAS_PROPDFLT_RETRIEVEOLD);
    pui = (PropUiItem *) new
	  CheckBoxUiItem(_retrieveold_tb, _FROM_MAILRC, propkey);
    _propui_array->append(pui);

    //
    //  Create FRAME to hold the user command printing options
    //
    _custom_frame = XtVaCreateManagedWidget(
				"CustomFrame", xmFrameWidgetClass, _form,
				XmNchildHorizontalAlignment,
							XmALIGNMENT_BEGINNING,
				XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
				XmNtopAttachment, XmATTACH_WIDGET,
				XmNtopWidget, _server_frame,
				XmNleftAttachment, XmATTACH_FORM,
				XmNleftOffset, 10,
				XmNrightAttachment, XmATTACH_FORM,
				XmNrightOffset, 10,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNbottomOffset, 10,
				NULL);

    xms = XmStringCreateLocalized(
		GETMSG(DT_catd, 27, 15, "Custom retrieval"));
    _custom_tb = XtVaCreateManagedWidget(
				"CustomTB",
				xmToggleButtonGadgetClass, _custom_frame,
				XmNchildType, XmFRAME_TITLE_CHILD,
				XmNchildHorizontalAlignment,
							XmALIGNMENT_BEGINNING,
				XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
				XmNlabelString, xms,
				XmNset, False,
				NULL);
    XmStringFree(xms);

    XtAddCallback(
		_custom_tb,
		XmNvalueChangedCallback,
		MailRetrievalOptions::retrievalTBSValueChangedCB,
		(XtPointer) this);

    //
    //  Create PropUiItem for the Custom Command Option
    //
    MrGENPROPKEY(propkey, _foldername, DTMAS_PROPKEY_GETMAILVIACOMMAND);
    PropSource::setDefaultValue(propkey, DTMAS_PROPDFLT_GETMAILVIACOMMAND);
    pui = (PropUiItem *) new CheckBoxUiItem(_custom_tb, _FROM_MAILRC, propkey);
    _propui_array->append(pui);

    _customframe_form = XtVaCreateManagedWidget(
				"CustomFrameForm",
			  	xmFormWidgetClass, _custom_frame,
			  	//XmNallowOverlap, False,
			  	XmNfractionBase, 3,
			  	XmNresizePolicy, XmRESIZE_ANY,
			  	NULL);

    //
    //  Create GUI for the User Command Option
    //
    _customcommand_tf = XtVaCreateManagedWidget(
			"CustomCommandText",
			xmTextFieldWidgetClass, _customframe_form,
    			XmNtopAttachment, XmATTACH_FORM,
			XmNleftAttachment, XmATTACH_POSITION,
			XmNleftPosition, 1,
    			XmNrightAttachment, XmATTACH_NONE,
			XmNcolumns, 45,
			XmNvalue, "",
			NULL);
    xms = XmStringCreateLocalized(
			GETMSG(DT_catd, 27, 16, "User Getmail command:"));
    _customcommand_label = XtVaCreateManagedWidget(
			"CustomCommandLabel",
			xmLabelWidgetClass, _customframe_form,
    			XmNlabelString, xms,
    			XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
    			XmNtopOffset, 0,
    			XmNtopWidget, _customcommand_tf,
    			XmNleftAttachment, XmATTACH_NONE,
			XmNrightAttachment, XmATTACH_WIDGET,
    			XmNrightOffset, 3,
			XmNrightWidget, _customcommand_tf,
			NULL);
    XmStringFree(xms);

    //
    //  Create PropUiItem for the ServerName Option
    //
    MrGENPROPKEY(propkey, _foldername, DTMAS_PROPKEY_GETMAILCOMMAND);
    PropSource::setDefaultValue(propkey, DTMAS_PROPDFLT_GETMAILCOMMAND);
    pui = (PropUiItem *) new
	  TextFieldUiItem(_customcommand_tf, _FROM_MAILRC, propkey);
    _propui_array->append(pui);

    //
    // Save widgets for manipulation.
    //
    _retrieval_tbs->append(_system_tb);
    _retrieval_tbs->append(_server_tb);
    _retrieval_tbs->append(_custom_tb);

    _server_options->append(_serverprotocol_label);
    _server_options->append(_serverprotocol_iom->baseWidget());
    _server_options->append(_servername_label);
    _server_options->append(_servername_tf);
    _server_options->append(_username_label);
    _server_options->append(_username_tf);
    _server_options->append(_password_label);
    _server_options->append(_password_tf);
    _server_options->append(_rememberpassword_tb);
    _server_options->append(_removeafterdelivery_tb);
    _server_options->append(_retrieveold_tb);

    _custom_options->append(_customcommand_label);
    _custom_options->append(_customcommand_tf);
    delete [] propkey;
}


MailRetrievalOptions::~MailRetrievalOptions (void)
{
    int i;
    PropUiItem		*pui;;
    IndexedOptionMenu	*iom;

    if (_propui_array)
    {
        for (i=0; i<_propui_array->length(); i++)
        {
            pui = (*_propui_array)[i];
	    delete pui;
        }
	delete _propui_array;
    }

    if (_retrieval_tbs) delete _retrieval_tbs;
    if (_server_options) delete _server_options;
    if (_custom_options) delete _custom_options;
    if (_serverprotocol_iom) delete _serverprotocol_iom;
    if (_foldername) free(_foldername);

    if (_w) XtDestroyWidget(_w);
}

char *
MailRetrievalOptions::isValidInboxPath(PropUiItem* pui, void* data)
{
    MailSession		*ses = theRoamApp.session();
    DtMail::Session	*d_session = ses->session();
    DtMailEnv		error;
    const char		*errmsg = NULL;
    char		*inbox_path = NULL;
    char		*s, *t, *tmp;
    char		*expanded_path = NULL;
    Widget		text;
    int			status;

    text = pui->getWidget();
    if (text) inbox_path = XmTextFieldGetString(text);
    if (NULL == inbox_path || 0 == strlen(inbox_path))
      return GETMSG(DT_catd, 27, 17, "Null INBOX path.");

    tmp = (char*) malloc(strlen(inbox_path)+1);
    for (s=inbox_path, t=tmp; *s; s++)
    {
	if (! isspace(*s))
	{
	    *t = *s;
	    t++;
	}
    }
    *t = '\0';
    free(inbox_path);
    inbox_path = tmp;

    if (NULL == inbox_path || 0 == strlen(inbox_path))
      return GETMSG(DT_catd, 27, 17, "Null INBOX path.");

    if (isalpha(inbox_path[0]))
    {
        tmp = (char*) malloc(strlen(inbox_path) + 3);
	sprintf(tmp, "./%s", inbox_path);
	free(inbox_path);
	inbox_path = tmp;
    }

    expanded_path = d_session->expandPath(error, inbox_path);
    if (expanded_path) XmTextFieldSetString(text, expanded_path);

    SafePathIsAccessible(error, expanded_path);
    if (error.isSet())
    {
        errmsg = (const char*) error;
        errmsg = strdup(errmsg);
    }
    else
      errmsg = NULL;

    if (inbox_path) free(inbox_path);
    free(expanded_path);
    return (char*) errmsg;
}

void
MailRetrievalOptions::initOptionInteractions(void)
{
    int			i;
    unsigned char	isset, isset_server;
    Widget		opt;

    XtVaGetValues(_server_tb, XmNset, &isset, NULL);
    if (XmUNSET == isset)
    {
        for (i=0; i<_server_options->length(); i++)
        {
            opt = (*_server_options)[i];
	    XtVaSetValues(opt, XmNsensitive, False, NULL);
        }
    }
    else
    {
        for (i=0; i<_server_options->length(); i++)
        {
            opt = (*_server_options)[i];
	    XtVaSetValues(opt, XmNsensitive, True, NULL);
        }
    }
    isset_server = isset;

    XtVaGetValues(_custom_tb, XmNset, &isset, NULL);
    if (XmUNSET == isset)
    {
        for (i=0; i<_custom_options->length(); i++)
        {
            opt = (*_custom_options)[i];
	    XtVaSetValues(opt, XmNsensitive, False, NULL);
        }
    }
    else
    {
        for (i=0; i<_custom_options->length(); i++)
        {
            opt = (*_custom_options)[i];
	    XtVaSetValues(opt, XmNsensitive, True, NULL);
        }
    }

    XtVaGetValues(_rememberpassword_tb, XmNset, &isset, NULL);
    if (XmSET == isset)
    {
        _password_pui->writeAllowed(DTM_TRUE);
	if (isset_server)
	  XtVaSetValues(_password_label, XmNsensitive, True, NULL);
    }
    else
    {
        _password_pui->writeAllowed(DTM_FALSE);
	XtVaSetValues(_password_label, XmNsensitive, False, NULL);
    }
}

PropUiItem *
MailRetrievalOptions::getFirstProp(void)
{
    _propui_array_iterator = 0;
    return getNextProp();
}

PropUiItem *
MailRetrievalOptions::getNextProp(void)
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
MailRetrievalOptions::getNumProps(void)
{
    return _propui_array->length();
}

void
MailRetrievalOptions::rememberPasswordChangedCB(
				Widget w,
				XtPointer client_data,
				XtPointer call_data)
{
    MailRetrievalOptions
		*mro = (MailRetrievalOptions*) client_data;
    XmToggleButtonCallbackStruct
		*cbs = (XmToggleButtonCallbackStruct*) call_data;

#if 0
    if (XmSET == cbs->set)
    {
	XtVaSetValues(mro->_password_label, XmNsensitive, True, NULL);
        mro->_password_pui->writeAllowed(DTM_TRUE);
    }
    else
    {
	XtVaSetValues(mro->_password_label, XmNsensitive, False, NULL);
        mro->_password_pui->writeAllowed(DTM_FALSE);
    }
#else
    mro->initOptionInteractions();
#endif
}

void
MailRetrievalOptions::retrievalTBSValueChangedCB(
					Widget w,
					XtPointer client_data,
					XtPointer call_data)
{
    MailRetrievalOptions
		*mro = (MailRetrievalOptions*) client_data;
    XmToggleButtonCallbackStruct
		*cbs = (XmToggleButtonCallbackStruct*) call_data;

    int		i;
    Widget	opt, tb;

    if (XmUNSET == cbs->set)
    {
        XtVaSetValues(w, XmNset, XmSET, NULL);
	return;
    }

    for (i=0; i<mro->_retrieval_tbs->length(); i++)
    {
        tb = (*mro->_retrieval_tbs)[i];
	if (tb != w) XtVaSetValues(tb, XmNset, XmUNSET, NULL);
    }

#if 0
    if (w == mro->_system_tb)
    {
        for (i=0; i<mro->_server_options->length(); i++)
        {
            opt = (*mro->_server_options)[i];
	    XtVaSetValues(opt, XmNsensitive, False, NULL);
        }
        for (i=0; i<mro->_custom_options->length(); i++)
        {
            opt = (*mro->_custom_options)[i];
	    XtVaSetValues(opt, XmNsensitive, False, NULL);
        }
    }
    else if (w == mro->_server_tb)
    {
        for (i=0; i<mro->_server_options->length(); i++)
        {
            opt = (*mro->_server_options)[i];
	    XtVaSetValues(opt, XmNsensitive, True, NULL);
        }
        for (i=0; i<mro->_custom_options->length(); i++)
        {
            opt = (*mro->_custom_options)[i];
	    XtVaSetValues(opt, XmNsensitive, False, NULL);
        }
    }
    else if (w == mro->_custom_tb)
    {
        for (i=0; i<mro->_server_options->length(); i++)
        {
            opt = (*mro->_server_options)[i];
	    XtVaSetValues(opt, XmNsensitive, False, NULL);
        }
        for (i=0; i<mro->_custom_options->length(); i++)
        {
            opt = (*mro->_custom_options)[i];
	    XtVaSetValues(opt, XmNsensitive, True, NULL);
        }
    }
#else
    mro->initOptionInteractions();
#endif
}

char *
MailRetrievalOptions::getPassword(char *foldername)
{
    DtMail::Session	*d_session = NULL;
    DtMail::MailRc	*m_rc = NULL;
    DtMailEnv		error;
    const char		*string = NULL;
    char		propkey[MAXPATHLEN];

    d_session = theRoamApp.session()->session();
    m_rc = d_session->mailRc(error);

    MrGENPROPKEY(propkey, foldername, DTMAS_PROPKEY_PASSWORD);

    m_rc->getValue(error, propkey , &string, DTM_TRUE);
    if (string == NULL || error.isSet()) return NULL;

    return (char*) string;
}
