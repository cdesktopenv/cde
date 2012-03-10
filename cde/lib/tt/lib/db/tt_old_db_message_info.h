/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_old_db_message_info.h /main/3 1995/10/23 10:06:18 rswiston $ 			 				 */
/*
 *
 * @(#)tt_old_db_message_info.h	1.6 07 Sep 1993
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */
/*
 * tt_old_db_message_info.h - Used to create a database record that stores
 *                        queued message info in a way that is reasonable
 *		          for the new DB server and compatible with the
 *		          old DB server.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 */

#ifndef _TT_OLD_DB_MESSAGE_INFO_H
#define _TT_OLD_DB_MESSAGE_INFO_H

#include "util/tt_object.h"
#include "util/tt_string.h"
#include "util/tt_xdr_version.h"

class _Tt_old_db_message_info : public _Tt_object {
public:
  _Tt_old_db_message_info ();
  ~_Tt_old_db_message_info ();

  int                 messageID;
  int                 numParts;
  int                 messageSize;
  _Tt_string_list_ptr ptypes;

  bool_t xdr (XDR *xdrs);

};

#endif /* _TT_OLD_DB_MESSAGE_INFO_H */
