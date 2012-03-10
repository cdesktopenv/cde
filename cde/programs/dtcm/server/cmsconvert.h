/* $XConsortium: cmsconvert.h /main/4 1995/11/09 12:41:30 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _CMSCONVERT_H
#define _CMSCONVERT_H

#include <EUSCompat.h>
#include "ansi_c.h"
#include "cm.h"
#include "rtable4.h"
#include "v4ops.h"

extern CSA_return_code _DtCmsCmsentriesToAppt4ForClient P((
			cms_entry	*entries,
			Appt_4		**appt));

extern CSA_return_code _DtCmsCmsentriesToAbbrAppt4ForClient P((
			cms_entry	*entries,
			Abb_Appt_4	**abbr));

extern CSA_return_code _DtCmsCmsentryToAbbrAppt4 P((
			cms_entry	*entry,
			Abb_Appt_4	**abbr));

extern Access_Entry_4 *_DtCmsConvertV5AccessList P((
			cms_access_entry	*cmslist,
			boolean_t		strictmode));

extern cms_access_entry *_DtCmsConvertV4AccessList P((
			Access_Entry_4 *alist));

extern CSA_return_code _DtCmsReminderRefToReminder P((
			cms_reminder_ref	*rems,
			Reminder_4		**r4));

extern CSA_return_code _DtCmsV4ReminderToReminderRef P((
			char			*calname,
			Reminder_4		*r4,
			_DtCmsEntryId		*ids,
			cms_reminder_ref	**rems));

extern CSA_return_code _DtCmsCmsAccessToV4Access P((
				cms_access_entry *alist,
				Access_Entry_4 **a4));

extern CSA_return_code _DtCmsAppt4ToCmsentriesForClient P((
			char		*calname,
			Appt_4		*appt,
			cms_entry	**e_r));

extern CSA_return_code _DtCmsAppt4ToCmsentry P((
			char		*calname,
			Appt_4		*a4,
			cms_entry	**entry_r,
			boolean_t	rerule));

extern CSA_return_code _DtCmsAttrsToAppt4 P((
			uint		size,
			cms_attribute	*attrs,
			Appt_4		*appt,
			boolean_t	check));

#endif
