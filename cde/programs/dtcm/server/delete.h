/* $XConsortium: delete.h /main/4 1995/11/09 12:43:40 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _DELETE_H
#define _DELETE_H

#include "ansi_c.h"
#include "cm.h"
#include "cmscalendar.h"

extern CSA_return_code _DtCmsDeleteEntry P((
			_DtCmsCalendar	*cal,
			char		*sender,
			uint		access,
			cms_key		*key,
			cms_entry	**entry_r));

extern CSA_return_code _DtCmsDeleteEntryAndLog P((
			_DtCmsCalendar	*cal,
			char		*sender,
			uint		access,
			cms_key		*key,
			cms_entry	**entry_r));

extern CSA_return_code _DtCmsDeleteInstancesAndLog P((
			_DtCmsCalendar	*cal,
			char		*sender,
			uint		access,
			cms_key		*key,
			int		scope,
			cms_entry	**newe,
			cms_entry	**olde));

#endif
