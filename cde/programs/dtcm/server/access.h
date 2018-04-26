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
/* $XConsortium: access.h /main/4 1995/11/09 12:39:54 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _ACCESS_H
#define _ACCESS_H

#include "ansi_c.h"
#include "cm.h"
#include "rtable4.h"
#include "cmscalendar.h"

#define _DTCMS_HAS_VIEW_CALENDAR_ATTR_ACCESS(a) \
		((a) & (CSA_VIEW_CALENDAR_ATTRIBUTES | CSA_OWNER_RIGHTS))

#define _DTCMS_HAS_INSERT_CALENDAR_ATTR_ACCESS(a) \
		((a) & (CSA_INSERT_CALENDAR_ATTRIBUTES | CSA_OWNER_RIGHTS))

#define _DTCMS_HAS_CHANGE_CALENDAR_ATTR_ACCESS(a) \
		((a) & (CSA_CHANGE_CALENDAR_ATTRIBUTES | CSA_OWNER_RIGHTS))

#define _DTCMS_HAS_VIEW_ACCESS(a) ((a) & (CSA_VIEW_PUBLIC_ENTRIES | \
					    CSA_VIEW_CONFIDENTIAL_ENTRIES | \
					    CSA_VIEW_PRIVATE_ENTRIES | \
					    CSA_ORGANIZER_RIGHTS | \
					    CSA_SPONSOR_RIGHTS | \
					    CSA_OWNER_RIGHTS))

#define _DTCMS_HAS_INSERT_ACCESS(a) ((a) & (CSA_INSERT_PUBLIC_ENTRIES | \
					    CSA_INSERT_CONFIDENTIAL_ENTRIES | \
					    CSA_INSERT_PRIVATE_ENTRIES | \
					    CSA_OWNER_RIGHTS))

#define _DTCMS_HAS_CHANGE_ACCESS(a) ((a) & (CSA_CHANGE_PUBLIC_ENTRIES | \
					    CSA_CHANGE_CONFIDENTIAL_ENTRIES | \
					    CSA_CHANGE_PRIVATE_ENTRIES | \
					    CSA_ORGANIZER_RIGHTS | \
					    CSA_SPONSOR_RIGHTS | \
					    CSA_OWNER_RIGHTS))

#define _DTCMS_HAS_V4_BROWSE_ACCESS(a) \
		((a) & (access_read_4 | CSA_OWNER_RIGHTS))

#define _DTCMS_HAS_V4_WRITE_ACCESS(a) \
		((a) & (access_write_4 | CSA_OWNER_RIGHTS))

extern CSA_return_code _DtCmsV4LoadAndCheckAccess P((
			struct svc_req	*svcrq,
			char		*target,
			char		**sender,
			uint		*access,
			_DtCmsCalendar	**cal));

extern CSA_return_code _DtCmsV5LoadAndCheckAccess P((
			struct svc_req	*svcrq,
			char		*target,
			char		**sender,
			uint		*access,
			_DtCmsCalendar	**cal));

extern CSA_return_code _DtCmsGetClientInfo P((
			struct svc_req *svcrq,
			char **source));

extern boolean_t _DtCmsIsFileOwner P((char *owner, char *user, char *target));

extern void _DtCmsShowAccessList P((Access_Entry_4 *l));

extern Access_Entry_4 *_DtCmsCalendarAccessList P((_DtCmsCalendar *cal));

extern Privacy_Level_4 _DtCmCheckPrivacyLevel P((char **p_src, Appt_4 *p_appt));

extern CSA_return_code _DtCmsCheckViewAccess P((
			char *user,
			uint access,
			cms_entry *eptr));

extern CSA_return_code _DtCmsCheckChangeAccess P((
			char	*user,
			uint	access,
			cms_entry *eptr));

extern uint _DtCmsClassToInsertAccess P((cms_entry *entry));

extern uint _DtCmsClassToViewAccess P((cms_entry *entry));

extern uint _DtCmsClassToChangeAccess P((cms_entry *entry));

#endif
