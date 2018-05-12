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
 *
 *	$TOG: FileShare.hh /main/5 1997/04/29 16:45:06 mgreess $
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

#ifndef _FILESHARE_HH
#define _FILESHARE_HH

#include <DtMail/DtMail.hh>
#include <Tt/tt_c.h>

class FileShare : public DtCPlusPlusAllocator {
  public:
    FileShare(DtMailEnv & error,
	      DtMail::Session * session,
	      const char * path,
	      DtMailCallback cb_func,
	      void * clientData);
    virtual ~FileShare(void);
    
    void lockFile(DtMailEnv & error);
    DtMailBoolean readOnly(DtMailEnv & error);
    DtMailBoolean readWriteOverride(DtMailEnv & error);

#ifdef DEAD_WOOD
    DtMailBoolean locked(void);
#endif /* DEAD_WOOD */

  private:
    struct TTHandle : public DtCPlusPlusAllocator {
	DtMail::Session		*session;
	DtMailObjectKey		key;
	FileShare *		self;
    };

    DtMailObjectKey	_key;
    DtMail::Session	*_session;
    char		*_path;
    TTHandle		*_tt_handle;
    Tt_pattern		*_file_pats;
    DtMailBoolean	_have_write_access;
    DtMailBoolean	_other_modified;
    DtMailCallback	_cb_func;
    void		*_cb_data;

    enum PendingOp {
	PENDING_LOCK = 1,
	PENDING_SAVE = 2,
	PENDING_DESTROY = 3
    };

    PendingOp		_pending;
    DtMailBoolean	_outstanding;

    DtMailBoolean	isModified(DtMailEnv & error);

    static Tt_message fileCB(Tt_message msg, Tttk_op op,
			     char * path, void *clientData, int same_euid,
			     int same_proc);

    static Tt_callback_action mt_lock_cb(Tt_message m, Tt_pattern p);

    static Tt_message fileRequestCB(Tt_message, Tttk_op,
				    char *, void *, int, int);

  /* _mt_pattern is used to handle tlock and rulock messages from mailtool */
    Tt_pattern	_mt_pattern;

  /* _mt_lock is DTM_TRUE is we use the mailtool locking protocol */
    int	_mt_lock;
};

#endif
