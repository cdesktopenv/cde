/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_old_db_results.h /main/3 1995/10/23 10:06:56 rswiston $ 			 				 */
/* @(#)tt_old_db_results.h	1.3 93/07/30
 *
 * tt_old_db_results.h - Defines the result values that may be
 *                       returned by an old TT DB server operation.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#ifndef _TT_OLD_DB_RESULTS_H
#define _TT_OLD_DB_RESULTS_H

// Status values that may be returned by an old DB server.
enum _Tt_old_db_results {
  TT_OLD_DB_OK = TT_OK,
  TT_OLD_DB_ERROR = TT_STATUS_LAST+1,
  TT_OLD_DB_INIT_FAILED,
  TT_OLD_DB_DBDESC_EXISTS,
  TT_OLD_DB_DB_EXISTS,
  TT_OLD_DB_CREATE_FAILED,
  TT_OLD_DB_OPEN_FAILED,
  TT_OLD_DB_NO_RECORD,
  TT_OLD_DB_READ_FAILED,
  TT_OLD_DB_WRITE_FAILED,
  TT_OLD_DB_DELETE_FAILED,
  TT_OLD_DB_CLOSE_FAILED,
  TT_OLD_DB_UNKNOWN_DBTABLE,
  TT_OLD_DB_UNKNOWN_INDEX,
  TT_OLD_DB_INVALID_VERSION_NUMBER,
  TT_OLD_DB_PATHMAP_FAILED,
  TT_OLD_DB_UPDATE_MFS_INFO_FAILED,
  TT_OLD_DB_CLEAR_LOCKS_FAILED,
  TT_OLD_DB_RECORD_LOCKED,
  TT_OLD_DB_NO_MFS,
  TT_OLD_DB_UNKNOWN_FS,
  TT_OLD_DB_CONVERSION_ERROR,
  TT_OLD_DB_RECORD_SET,
  TT_OLD_DB_ACCESS_DENIED
};

#endif /* _TT_OLD_DB_RESULTS_H */
