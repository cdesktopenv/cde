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
/*
 *+SNOTICE
 *
 *	$TOG: MailSession.hh /main/8 1999/03/26 16:52:44 mgreess $
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
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#ifndef _MAILSESSION_HH
#define _MAILSESSION_HH

#include <sys/stat.h>
#include <sys/types.h>

#include <DtMail/DtMail.hh>
#include "RoamMenuWindow.h"

class MailSession
{
  public:
    MailSession(DtMailEnv &, XtAppContext);
    ~MailSession(void);

    // raises ME_NotBento, ME_AlreadyLocked, ME_OtherAssumedRole, ME_NoSuchFile
    DtMail::MailBox	*open(DtMailEnv&, const char*, DtMailCallback, void*,
			      DtMailBoolean auto_create = DTM_TRUE,
			      DtMailBoolean request_lock = DTM_TRUE,
			      DtMailBoolean auto_parse = DTM_TRUE);

    void		 close(DtMailEnv&, const DtMail::MailBox*);

    // raises ME_UnknownFormat
    void		 convert(DtMailEnv&, const char*, const char*,
				 DtMailStatusCallback cb_func = NULL,
				 void *client_data = NULL);

    // raises ME_NotBento, ME_RequestDenied
    void		 copy(DtMailEnv&, DtMail::Message&, const char*);

    int		 	 isActiveRMW(RoamMenuWindow*);
    void		 activateRMW(RoamMenuWindow*);
    void		 deactivateRMW(RoamMenuWindow*);
    RoamMenuWindow	*getRMW(DtMail::MailBox* mbox);
    RoamMenuWindow	*getRMW(const char* path);
    Boolean		 isMboxOpen(const char* path);
    int			 numDeactivatedRMW() { return _num_deactivated; }
    DtMail::Session	*session(void) { return _app_session; }

  private:
    struct MailBoxItem
    {
	DtMail::MailBox		*handle;
	int			 stated;
	struct stat		 statbuf;
	int			 is_active;
	int			 open_ref_count;
	char			*path;
	RoamMenuWindow		*rmw;
    };
    DtVirtArray<MailBoxItem *>	 _open_mailboxes;
    static DtMail::Session	*_app_session;
    static int			 _num_deactivated;

    void	addToList(DtMail::MailBox*, const char*, RoamMenuWindow*);
    int		locate(const char * path);
    int		locate(const DtMail::MailBox *);
    int		locate(const RoamMenuWindow *);
    static Tt_status
		lockCB(Tt_message, Tttk_op, const char*, uid_t, int, void*);
};

#endif
