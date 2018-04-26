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
/* $XConsortium: garbage.h /main/4 1995/11/09 12:44:07 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _GARBAGE_H
#define _GARBAGE_H

#include "ansi_c.h"

/*
**  Housekeeping mechanism to clean up the calendar log file.  It copies the
**  log file to a backup file, dumps the red/black tree to a temp file, copies
**  the temp file back to the original log file, and deletes the temp and
**  backup files.  Any errors encountered along the way abort the process.
**  The garbage collector runs at midnight every.
*/

extern void _DtCmsCollectOne P((_DtCmsCalendar *cal));
extern CSA_return_code _DtCmsDumpDataV1 P((char *file, _DtCmsCalendar *cal));
extern CSA_return_code _DtCmsDumpDataV2 P((char *file, _DtCmsCalendar *cal));

#endif
