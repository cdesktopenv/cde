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
/* $XConsortium: dtinfo_start.h /main/4 1996/09/14 14:52:13 drk $
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
#ifndef _dtinfo_start_h_
#define _dtinfo_start_h_

/*
 * macro to get message catalog strings
 */
#ifndef NO_MESSAGE_CATALOG
# define _CLIENT_CAT_NAME "dtinfo_start"

extern char *_DtGetMessage (char *filename, int set_num, int message_num, char *message);

# define GETMESSAGE(set, number, string)\
    (_DtGetMessage(_CLIENT_CAT_NAME, set, number, string))
#else /* NO_MESSAGE_CATALOG */
# define GETMESSAGE(set, number, string)\
    string
#endif /* NO_MESSAGE_CATALOG */

#endif /* _dtinfo_start_h_ */
