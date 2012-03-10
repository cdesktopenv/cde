/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_db_msg_q_lock_entry.h /main/3 1995/10/20 16:41:58 rswiston $ 			 				 */
/*
 * tt_db_msg_q_lock_entry.h - Defines a class for holding lock information in the
 *                            locks list in the tt_db_msg_q_lock class.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#ifndef _TT_DB_MSG_Q_LOCK_ENTRY_H
#define _TT_DB_MSG_Q_LOCK_ENTRY_H

#include "util/tt_object.h"
#include "util/tt_string.h"

class _Tt_db_msg_q_lock_entry : public _Tt_object {
public:
  _Tt_db_msg_q_lock_entry () {}
  ~_Tt_db_msg_q_lock_entry () {}

  _Tt_string clientID;
  _Tt_string fileKey;
};

#endif /* _TT_DB_MSG_Q_LOCK_ENTRY_H */
