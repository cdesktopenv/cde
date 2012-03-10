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
