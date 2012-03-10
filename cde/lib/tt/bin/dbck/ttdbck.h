/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: ttdbck.h /main/3 1995/10/20 16:34:12 rswiston $ 			 				 */
/*
 *
 * ttdbck.h
 *
 * ToolTalk 1.0 spec data base inspect and repair tool global declarations
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#ifndef _TTDBCK_H
#define _TTDBCK_H

#include "spec.h"
#include "options_tt.h"
#include "mp_typedb.h"

extern char *progname;			// from argv[0]
extern Spec_list_ptr specs_to_repair;
extern _Tt_typedb_ptr *tdb_ptr;
extern Dbck_specoptions *opts;

#endif /* _TTDBCK_H */
