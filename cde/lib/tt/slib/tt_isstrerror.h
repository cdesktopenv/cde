/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_isstrerror.h /main/3 1995/10/23 12:03:35 rswiston $ 			 				 */
/* 
 * tt_isstrerror.h
 * 
 * Copyright (c) 1994 by Sun Microsystems, Inc.
 * 
 * This file implements the _Tt_s_mp object which represents the global
 * information for the server MP component. There should only be one
 * instance of a _Tt_s_mp object in the server.
 */
#ifndef _TT_ISSTRERROR_H
#define _TT_ISSTRERROR_H

extern const char * _tt_isstrerror(int an_iserrno);

#endif				/* _TT_ISSSTRERROR_H */
