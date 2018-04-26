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
/* $XConsortium: UilDef.h /main/2 1995/07/15 22:31:34 drk $ */
#ifndef _AB_UILDEF_H_
#define _AB_UILDEF_H_
/*
 * UilDef.h - include standard header files, adding symbols for AB
 * 		files.
 */

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1		/* we want to be POSIX-compliant */
#endif

#include <uil/UilDef.h>

/*
 * Make sure we don't redefine types defined in standard X include files
 */
#define _AB_BOOL_DEFINED_
#define _AB_BYTE_DEFINED_

#endif /* _AB_UILDEF_H_ */
