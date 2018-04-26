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
/* $TOG: InboxTextFieldUiItem.C /main/1 1998/02/17 12:35:07 mgreess $ */
/*
 *+SNOTICE
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#include <DtMail/options_util.h>
#include <DtMail/PropUi.hh>
#include <DtMail/InboxTextFieldUiItem.hh>
#include <DtMail/DtMail.hh>
#include <DtMail/DtMailError.hh>
#include "MailSession.hh"
#include "RoamApp.h"

////////////////////////////////////////////////////////////////////
InboxTextFieldUiItem::InboxTextFieldUiItem(
				Widget w,
				int source,
				char *search_key,
				PropUiCallback validator,
				void * validator_data)
: PropUiItem(w, source, search_key, validator, validator_data)
{
    options_field_init(w, &(this->dirty_bit));
}

// Takes values in the UI and puts them into the source DB
///////////////////////////////////////////////////////////////////
void InboxTextFieldUiItem::writeFromUiToSource()
{
    char 		*mailspool_file = NULL;
    char 		*value;
    Widget		w = this->getWidget();

    MailSession		*ses = theRoamApp.session();
    DtMail::Session	*d_session = ses->session();
    DtMailObjectSpace	 space;
    DtMailEnv		 error;
    DtMailEnv		 mail_error;

    d_session->queryImpl(error,
			 d_session->getDefaultImpl(mail_error),
			 DtMailCapabilityMailspoolName,
			 &space,
			 &mailspool_file);
    value = options_field_get_value(w);

    if (0 == strcmp(value, mailspool_file))
      prop_source->setValue("MAILSPOOL_FILE");
    else
      prop_source->setValue(value);

    free((void*) mailspool_file);
}

// Takes values in the UI and puts them into the source DB
///////////////////////////////////////////////////////////////////
void InboxTextFieldUiItem::writeFromSourceToUi()
{
    char 	*mailspool_file = NULL;
    char	*value;
    Widget	w = this->getWidget();

    MailSession		*ses = theRoamApp.session();
    DtMail::Session	*d_session = ses->session();
    DtMailObjectSpace	 space;
    DtMailEnv		 error;
    DtMailEnv		 mail_error;

    d_session->queryImpl(error,
			 d_session->getDefaultImpl(mail_error),
			 DtMailCapabilityMailspoolName,
			 &space,
			 &mailspool_file);
    value = (char *)prop_source->getValue();

    if (0 == strcmp(value, "MAILSPOOL_FILE"))
      options_field_set_value(w, mailspool_file, this->dirty_bit);
    else
      options_field_set_value(w, value, this->dirty_bit);

    free((void*) value);
    if (NULL != mailspool_file) free((void*) mailspool_file);
}
