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
/* $XConsortium: v4ops.h /main/4 1995/11/09 12:55:26 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _V4OPS_H
#define _V4OPS_H

#include "ansi_c.h"
#include "rtable4.h"
#include "cm.h"
#include "log.h"
#include "cmscalendar.h"

#define is_appointment(p_appt)  	((p_appt)->period.period == single_4)
#define is_repeater(p_appt)     	((p_appt)->period.period != single_4)

typedef struct __DtCmsEntryId {
	long id;
	struct __DtCmsEntryId *next;
} _DtCmsEntryId;

extern CSA_return_code _DtCmsInsertAppt P((_DtCmsCalendar *cal, Appt_4 *appt4));

extern CSA_return_code _DtCmsDeleteAppt P((
			_DtCmsCalendar	*cal,
			char		*user,
			uint		access,
			Id_4		*p_key,
			Appt_4		**appt_r));

extern CSA_return_code _DtCmsDeleteApptAndLog P((
			_DtCmsCalendar	*cal,
			char		*user,
			uint		access,
			Id_4		*key,
			Appt_4		**oldappt));

extern CSA_return_code _DtCmsDeleteApptInstancesAndLog P((
			_DtCmsCalendar	*cal,
			char		*source,
			uint		access,
			Id_4		*key,
			Options_4	option,
			int		*remain,
			Appt_4		**oldappt));

extern CSA_return_code _DtCmsChangeAll P((
			_DtCmsCalendar	*cal,
			char		*source,
			uint		access,
			Id_4		*p_key,
			Appt_4		*newa,
			Appt_4		**oldappt));

extern CSA_return_code _DtCmsChangeSome P((
			_DtCmsCalendar	*cal,
			char		*source,
			uint		access,
			Id_4		*p_key,
			Appt_4		*p_appt,
			Options_4	option,
			Appt_4		**oldappt));

extern CSA_return_code _DtCmsInsertApptAndLog P((
			_DtCmsCalendar	*cal,
			Appt_4		*appt));

extern _DtCmsComparisonResult _DtCmsCompareAppt P((Id_4 *key, caddr_t data));

extern _DtCmsComparisonResult _DtCmsCompareRptAppt P((Id_4 *key, caddr_t data));

extern caddr_t _DtCmsGetApptKey P((caddr_t data));

extern CSA_return_code v4_transact_log P((
			char		*calendar,
			Appt_4		*a,
			_DtCmsLogOps	op));

extern CSA_return_code _DtCmsLookupRangeV4 P((
			_DtCmsCalendar	*cal,
			char		*user,
			uint		access,
			Range_4		*p_range,
			boolean_t	no_end_time_range,
			long		end1,
			long		end2,
			boolean_t	(*match_func)(),
			uint		num_attrs,
			cms_attribute	*attrs,
			CSA_enum	*ops,
			Appt_4		**appt_r,
			Abb_Appt_4	**abbr_r));

extern CSA_return_code _DtCmsLookupKeyrangeV4 P((
			_DtCmsCalendar	*cal,
			char		*user,
			uint		access,
			boolean_t	no_start_time_range,
			boolean_t	no_end_time_range,
			time_t		start1,
			time_t		start2,
			time_t		end1,
			time_t		end2,
			long		id,
			boolean_t	(*match_func)(),
			uint		num_attrs,
			cms_attribute	*attrs,
			CSA_enum	*ops,
			Appt_4		**appt_r,
			Abb_Appt_4	**abbr_r));

extern CSA_return_code _AddToLinkedAppts P((
			Appt_4	*p_appt,
			char	*user,
			uint	access,
			caddr_t	*ilp));

extern CSA_return_code _AddToLinkedAbbrAppts P((
			Appt_4	*p_appt,
			char	*user,
			uint	access,
			caddr_t	*ilp));

extern Privacy_Level_4 _GetAccessLevel P((
			char	*user,
			uint	access,
			Appt_4	*p_appt));

extern Appt_4 * _AddApptInOrder P((Appt_4 * head, Appt_4 * aptr));

extern Abb_Appt_4 * _AddAbbApptInOrder P((Abb_Appt_4 *head, Abb_Appt_4 *aptr));

extern CSA_return_code _DtCmsSetV4AccessListAndLog P((
			_DtCmsCalendar	*cal,
			Access_Entry_4	*alist));

extern CSA_return_code _DtCmsGetV4Reminders P((
			_DtCmsCalendar	*cal,
			long		tick,
			Reminder_4	**rem_r,
			_DtCmsEntryId	**ids_r));

extern void _DtCmsFreeEntryIds P((_DtCmsEntryId *ids));

extern CSA_return_code _DtCmsTruncateElist P((
			Appt_4		*parent_p,
			int		remain,
			Except_4	**excpt));

extern CSA_return_code _DtCmsSetV4AccessListInCal P((
			_DtCmsCalendar *cal,
			Access_Entry_4 *e));

#endif
