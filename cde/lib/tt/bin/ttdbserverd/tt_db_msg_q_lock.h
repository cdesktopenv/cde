/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_db_msg_q_lock.h /main/3 1995/10/20 16:41:50 rswiston $ 			 				 */
/*
 * tt_db_msg_q_lock.h - Defines a class for managing the locking and unlocking
 *                      the  message queue for a particular file.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#ifndef _TT_DB_MSG_Q_LOCK_H
#define _TT_DB_MSG_Q_LOCK_H

#include "util/tt_new.h"
#include "util/tt_string.h"
#include "tt_db_msg_q_lock_entry_utils.h"

class _Tt_db_msg_q_lock : public _Tt_allocated {
public:
  _Tt_db_msg_q_lock ();
  ~_Tt_db_msg_q_lock ();

  bool_t testAndSetLock (const _Tt_string &client_id,
			 const _Tt_string &file_key);
  void unsetLock (const _Tt_string &client_id,
		  const _Tt_string &file_key);
  bool_t checkLock (const _Tt_string &client_id,
		    const _Tt_string &file_key);

  void unsetAllLocks ()
    {
      (*locks)->flush();
    }

private:
  static _Tt_db_msg_q_lock_entry_list_ptr *locks;
};

#endif /* _TT_DB_MSG_Q_LOCK_H */
