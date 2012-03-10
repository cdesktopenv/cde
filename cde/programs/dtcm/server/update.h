/* $XConsortium: update.h /main/4 1995/11/09 12:54:08 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _UPDATE_H
#define _UPDATE_H

#include "ansi_c.h"
#include "cm.h"
#include "cmscalendar.h"

extern CSA_return_code _DtCmsUpdateCalAttrs P((
			_DtCmsCalendar	*cal,
			uint		num_attrs,
			cms_attribute	*attrs));

extern CSA_return_code _DtCmsUpdateEntry P((
			_DtCmsCalendar	*cal,
			char		*sender,
			uint		access,
			cms_key		*key,
			uint		num_attrs,
			cms_attribute	*attrs,
			cms_entry	**oldentry,
			cms_entry	**newentry));

extern CSA_return_code _DtCmsUpdateInstances P((
			_DtCmsCalendar	*cal,
			char		*sender,
			uint		access,
			cms_key		*key,
			int		scope,
			uint		num_attrs,
			cms_attribute	*attrs,
			cms_entry	**oldentry,
			cms_entry	**newentry));

#endif
