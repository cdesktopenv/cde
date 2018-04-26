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
