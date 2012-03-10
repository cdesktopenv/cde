/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_generic.h /main/3 1995/10/23 10:38:52 rswiston $ 			 				 */
/*-*-C++-*-
 * @(#)tt_generic.h	1.3 @(#)
 * tt_generic.h
 *
 * Copyright (c) 1993 by Sun Microsystems, Inc.
 *
 */

#if !defined(_TT_GENERIC_H)
#define _TT_GENERIC_H

#if defined(__GNUG__) || defined(__GNUC__)
#define name2(a,b)      _name2_aux(a,b)
#define _name2_aux(a,b)      a##b
#define name3(a,b,c)    _name3_aux(a,b,c)
#define _name3_aux(a,b,c)    a##b##c
#define name4(a,b,c,d)  _name4_aux(a,b,c,d)
#define _name4_aux(a,b,c,d)  a##b##c##d
#else
#	include <generic.h>
#endif

#endif
