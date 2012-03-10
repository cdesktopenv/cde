/* $XConsortium: Session.h /main/3 1995/10/26 15:10:56 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 */

#ifndef _Dt_Session_h
#define _Dt_Session_h

#include <X11/Intrinsic.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Functions
 */

extern Boolean DtSessionSavePath(
		Widget		widget,
		char		**save_path,
		char		**save_file);

extern Boolean DtSessionRestorePath(
		Widget		widget,
		char		**restore_path,
		char		*restore_file);

#ifdef __cplusplus
}
#endif

#endif /* _Dt_Session_h */
