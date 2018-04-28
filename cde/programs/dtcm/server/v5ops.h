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
/* $XConsortium: v5ops.h /main/4 1995/11/09 12:56:05 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _V5OPS_H
#define _V5OPS_H

#include "ansi_c.h"
#include "cm.h"
#include "cmscalendar.h"
#include "rerule.h"
#include "repeat.h"

extern _DtCmsComparisonResult _DtCmsCompareEntry P((
				cms_key *key,
				caddr_t data));

extern _DtCmsComparisonResult _DtCmsCompareRptEntry P((
				cms_key *key,
				caddr_t data));

extern caddr_t _DtCmsGetEntryKey P((caddr_t data));

extern CSA_return_code _DtCmsSetLastUpdate P((cms_entry *entry));

extern void _DtCmsConvertToOnetime P((cms_entry *entry, RepeatEvent *re));

extern int _DtCmsGetDuration P((cms_entry *eptr));

extern CSA_return_code _DtCmsCheckInitialAttributes P((cms_entry *entry));

extern CSA_return_code _DtCmsCheckStartEndTime P((cms_entry *entry));

extern void _DtCmsCleanupExceptionDates P((cms_entry *newe, long ftick));

extern int _DtCmsNumberExceptionDates P((cms_entry *entry));

extern CSA_return_code _DtCmsUpdateDurationInRule P((
				cms_entry	*entry,
				uint		remain));

extern CSA_return_code _DtCmsAddEndDateToRule P((
				cms_attribute	*attr,
				RepeatEvent	*re,
				long		time));
#endif
