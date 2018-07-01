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
/*%%  $XConsortium: isamhooks.c /main/3 1995/10/23 11:34:48 rswiston $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isamhooks.c
 *
 * Description: 
 *	Entry and exit hooks that are called upon every entry (and exit)
 *	to any Access Module Function.
 *	
 *
 */

#include "isam_impl.h"
extern struct dlink  *pavail;
/*
 * _isam_entryhook()
 *
 */

void
_isam_entryhook(void)
{
    isdupl = 0;				     /* Reset duplicate indicator */
    _setiserrno2(0, '0', '0');
}

/*
 * _isam_exithook()
 *
 */

void
_isam_exithook(void)
{
    _isdisk_inval();			     /* Invalidate all buffers */
}
