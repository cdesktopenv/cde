/* $XConsortium: cmsentry.h /main/4 1995/11/09 12:41:58 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _CMSENTRY_H
#define _CMSENTRY_H

#include <EUSCompat.h>
#include "ansi_c.h"
#include "nametbl.h"
#include "cm.h"
#include "csa.h"
#include "cmscalendar.h"

extern CSA_return_code _DtCmsMakeHashedEntry P((
			_DtCmsCalendar	*cal,
			uint		num,
			cms_attribute	*attrs,
			cms_entry	**entry));

extern void _DtCmsFreeEntryAttrResItem P((cms_get_entry_attr_res_item *elist));

extern CSA_return_code _DtCmsGetCmsEntryForClient P((
			cms_entry	*e,
			cms_entry	**e_r,
			boolean_t	time_only));

#endif
