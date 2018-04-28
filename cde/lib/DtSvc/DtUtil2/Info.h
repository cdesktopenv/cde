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
/* $XConsortium: Info.h /main/2 1996/03/26 15:05:44 barstow $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 *
 */

#ifndef _dt_info_h_
#define _dt_info_h_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	DtINFO_SHOW_OK,
	DtINFO_SHOW_BAD_LOCATOR,	/* the locator argument is NULL */
	DtINFO_SHOW_TT_OPEN_FAIL,	/* tt_open() failed */
	DtINFO_SHOW_MSG_CREATE_FAIL,	/* tt_message_create() failed */
	DtINFO_SHOW_MSG_SEND_FAIL	/* tt_message_send() failed */
} DtInfoShowStatus;

extern DtInfoShowStatus DtInfoShowTopic (
	const char	* info_lib,	/* The InfoLib to browse */
	const char	* locator);	/* The locator in Generalized Locator 
					   Format */

#ifdef __cplusplus
}
#endif

#endif /* _dt_info_h_ */
