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
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: none
 *
 *   ORIGINS: 27,157
 *
 *   This module contains IBM CONFIDENTIAL code. -- (IBM
 *   Confidential Restricted when combined with the aggregated
 *   modules for this product)
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 *   (C) COPYRIGHT International Business Machines Corp. 1995, 1996
 *   All Rights Reserved
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/* $XConsortium: oflag.c /main/3 1996/05/07 13:46:59 drk $
 * Author: Mike Russell, October 1995.
 * Global db_oflag replaces hardcoded O_RDWR in calls
 * to open_b() in runtime and open_u() in utilities
 * to permit read-only database opens.
 * Placed in its own module because there are no common
 * modules or headers in both runtime and utility source sets.
 * This module is duplicated exactly in both directories.
 *
 * $Log$
 * Revision 1.1  1995/10/17  19:57:02  miker
 * Initial revision
 *
 */
#include <fcntl.h>

int     db_oflag =     O_RDWR;

