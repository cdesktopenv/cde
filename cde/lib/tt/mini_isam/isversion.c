/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: isversion.c /main/3 1995/10/23 11:46:03 rswiston $ 			 				 */
#ifndef lint
static char sccsid[] = "@(#)isversion.c 1.5 89/07/17 Copyr 1988 Sun Micro";
#endif
/*
 * Copyright (c) 1988, 1992 by Sun Microsystems, Inc.
 */

/*
 * isversion.c
 *
 * Description:
 *	Keep version for Customer Support benefit
 *      Since this version is considered part of ToolTalk, use the
 *	ToolTalk version number.
 *
 */

char	_isam_version[] = TT_VERSION_STRING;
