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
/* $XConsortium: cmfns.h /main/3 1995/11/03 10:37:28 rswiston $ */
/*	@(#)cmfns.h 1.3 94/11/07 SMI	*/

/*									*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */


#ifndef _CMFNS_H
#define _CMFNS_H

#ifdef FNS

#include "dtfns.h"

#define CMFNS_CALENDAR_SERVICE	"calendar"
#define CMFNS_CALENDAR_TYPE	"SUNW_fns_calendar"
#define CMFNS_CALENDAR_ADDR_TYPE "SUNW_cal_str"

#ifdef FNS_DEMO

#define FNS_FILE ".FNSName"
int cmfns_name_from_file(const char *path, char *name, int len);

#endif /* FNS_DEMO */

extern int cmfns_use_fns(Props *);
extern int cmfns_lookup_calendar(const char *name, char *addr_buf,
				 int addr_size);
extern int cmfns_description(const char	*, char *, int);
extern int cmfns_register_calendar(const char *name, const char *calendar);

#endif /* FNS */

#endif /* _CMFNS_H */

