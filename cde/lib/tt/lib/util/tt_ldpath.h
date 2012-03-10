/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_ldpath.h /main/3 1995/10/23 10:40:55 rswiston $ 			 				 */
/*
 *
 * tt_ldpath.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#ifndef _TT_LDPATH_H
#define _TT_LDPATH_H
#include <util/tt_string.h>

/* 
 * Attempts to find the best path to a dynamic library named by libname.
 * If succesful then 1 is returned and path will be set to the path to
 * the library. Otherwise 0 is returned. This function will use the
 * appropiate combination of LD_LIBRARY_PATH and the required hardcoded
 * paths to perform the search.
 */
int	tt_ldpath(_Tt_string libname, _Tt_string &path);

#endif				/* _TT_LDPATH_H */
