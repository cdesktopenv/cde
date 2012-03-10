/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_audit.h /main/3 1995/10/23 10:37:05 rswiston $ 			 				 */
/* %W 93/07/20
 *
 * tt_audit.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#ifndef TT_AUDIT_H
#define TT_AUDIT_H
#include <stdio.h>
#include <stdarg.h>
#include <util/tt_trace.h>
#include <util/tt_object.h>

class _Tt_audit: public _Tt_trace {
	public:
		Tt_status entry(char *argskey, _Tt_entry_pt func, ...);
};
#endif	/* TT_AUDIT_H */
