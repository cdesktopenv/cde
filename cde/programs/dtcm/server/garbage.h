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
