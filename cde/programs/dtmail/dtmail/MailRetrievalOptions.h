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
/* $TOG: MailRetrievalOptions.h /main/3 1998/02/17 12:35:42 mgreess $ */

#ifndef _MAIL_RETRIEVAL_OPTIONS_H
#define _MAIL_RETRIEVAL_OPTIONS_H

/*
 *+SNOTICE
 *
 *	$:$
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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>

#include <DtMail/EncryptedTextFieldUiItem.hh>
#include <DtMail/PropUi.hh>
#include <DtMail/DtMailServer.hh>
#include <DtMail/DtVirtArray.hh>
#include <DtMail/PropUi.hh>
#include "UIComponent.h"
#include "IndexedOptionMenu.h"

struct MrPropVal
{
        char		*prop_value_string;
        int		set_id;
        int		msg_id;
        char		*dflt_gui_string;
};

class MailRetrievalOptions : public UIComponent
{

private:
    char		*_foldername;
    DtVirtArray<PropUiItem *>
			*_propui_array;
    int			 _propui_array_iterator;
    EncryptedTextFieldUiItem
			*_password_pui;
    DtVirtArray<Widget> *_retrieval_tbs;

    Widget		 _parent;
    Widget		 _form;

    Widget		 _inboxpath_label;
    Widget		 _inboxpath_tf;
    Widget		 _checkfornewmail_label;
    Widget		 _checkfornewmail_sb;
 
    Widget		 _system_tb;

    DtVirtArray<Widget> *_server_options;
    Widget		 _server_frame;
    Widget		 _server_tb;
    Widget		 _serverframe_form;
    Widget		 _serverprotocol_label;
    IndexedOptionMenu	*_serverprotocol_iom;
    Widget		 _servername_label;
    Widget		 _servername_tf;
    Widget		 _username_label;
    Widget		 _username_tf;
    Widget		 _password_label;
    Widget		 _password_tf;
    Widget		 _rememberpassword_tb;
    Widget		 _removeafterdelivery_tb;
    Widget		 _retrieveold_tb;

    DtVirtArray<Widget> *_custom_options;
    Widget		 _custom_frame;
    Widget		 _custom_tb;
    Widget		 _customframe_form;
    Widget		 _customcommand_label;
    Widget		 _customcommand_tf;

    static void		rememberPasswordChangedCB(Widget,XtPointer,XtPointer);
    static void		retrievalTBSValueChangedCB(Widget,XtPointer,XtPointer);

public:

    MailRetrievalOptions(Widget, const char *foldername = NULL);
    ~MailRetrievalOptions(void);

    PropUiItem		*getFirstProp(void);
    PropUiItem		*getNextProp(void);
    int			 getNumProps(void);
    void		 initOptionInteractions(void);
    static char		*isValidInboxPath(PropUiItem* pui, void* data);
    static char		*getPassword(char *foldername);
};

#endif // _MAIL_RETRIEVAL_OPTIONS_H
