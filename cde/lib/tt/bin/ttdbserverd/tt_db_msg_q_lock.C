//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: tt_db_msg_q_lock.C /main/3 1995/10/20 16:41:41 rswiston $ 			 				
/*
 * tt_db_msg_q_lock.cc - Defines a class for managing the locking and unlocking
 *                       the  message queue for a particular file.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#include "tt_db_msg_q_lock.h"
#include "tt_db_msg_q_lock_entry_utils.h"

_Tt_db_msg_q_lock_entry_list_ptr*
_Tt_db_msg_q_lock::locks = (_Tt_db_msg_q_lock_entry_list_ptr *)NULL;

_Tt_db_msg_q_lock::_Tt_db_msg_q_lock ()
{
  if (!locks) {
    locks = new _Tt_db_msg_q_lock_entry_list_ptr;
    *locks = new _Tt_db_msg_q_lock_entry_list;
  }
}

_Tt_db_msg_q_lock::~_Tt_db_msg_q_lock ()
{
}

bool_t _Tt_db_msg_q_lock::testAndSetLock (const _Tt_string &client_id,
				          const _Tt_string &file_key)
{
  bool_t found = FALSE;
  bool_t results = FALSE;

  _Tt_db_msg_q_lock_entry_list_cursor locks_cursor(*locks);
  while (locks_cursor.next()) {
    if ((locks_cursor->clientID == client_id) &&
	(locks_cursor->fileKey == file_key)) {
      found = TRUE;
      results = TRUE;
      break;
    }
    else if (locks_cursor->fileKey == file_key) {
      found = TRUE;
      results = FALSE;
      break;
    }
  }

  if (!found) {
    _Tt_db_msg_q_lock_entry_ptr lock = new _Tt_db_msg_q_lock_entry;
    lock->clientID = (char *)client_id;
    lock->fileKey = (char *)file_key;
    (*locks)->append(lock);

    results = TRUE;
  }

  return results;
}

void _Tt_db_msg_q_lock::unsetLock (const _Tt_string &client_id,
				   const _Tt_string &file_key)
{
  _Tt_db_msg_q_lock_entry_list_cursor locks_cursor(*locks);
  while (locks_cursor.next()) {
    if ((locks_cursor->clientID == client_id) &&
	(locks_cursor->fileKey == file_key)) {
      locks_cursor.remove();
      break;
    }
  }
}

bool_t _Tt_db_msg_q_lock::checkLock (const _Tt_string &client_id,
				     const _Tt_string &file_key)
{
  bool_t found = FALSE;

  _Tt_db_msg_q_lock_entry_list_cursor locks_cursor(*locks);
  while (locks_cursor.next()) {
    if ((locks_cursor->clientID == client_id) &&
	(locks_cursor->fileKey == file_key)) {
      found = TRUE;
      break;
    }
  }

  return found;
}
