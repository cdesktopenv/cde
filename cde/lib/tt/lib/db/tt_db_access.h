/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_db_access.h /main/3 1995/10/23 10:00:40 rswiston $ 			 				 */
/*
 * tt_db_access.h - Define the TT DB server access class.  This class is used
 *                  for specifying the access privilege of a particular object
 *                  or file in the database.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#ifndef _TT_DB_ACCESS_H
#define _TT_DB_ACCESS_H

#include <sys/types.h>

#include "util/tt_object.h"

class _Tt_db_access : public _Tt_object {
public:
  _Tt_db_access ();
  virtual ~_Tt_db_access ();
  uid_t  user;
  gid_t  group;
  mode_t mode;
};

#endif // _TT_DB_ACCESS_H
