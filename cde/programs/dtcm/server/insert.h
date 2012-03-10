/* $XConsortium: insert.h /main/4 1995/11/09 12:44:48 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _INSERT_H
#define _INSERT_H

#include "ansi_c.h"
#include "cm.h"
#include "cmscalendar.h"

extern CSA_return_code _DtCmsInsertEntry P((
			_DtCmsCalendar	*cal,
			cms_entry	*entry));

extern CSA_return_code _DtCmsInsertEntryAndLog P((
			_DtCmsCalendar	*cal,
			cms_entry	*entry));

#endif
