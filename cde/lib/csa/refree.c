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
/* $XConsortium: refree.c /main/1 1996/04/21 19:24:08 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <time.h>
#include <stdlib.h>
#include "rerule.h"

#define RE_DAILY(re)    (re->re_data.re_daily)
#define RE_WEEKLY(re)   (re->re_data.re_weekly)
#define RE_MONTHLY(re)  (re->re_data.re_monthly)
#define RE_YEARLY(re)   (re->re_data.re_yearly)

/* 
 * Deallocate a RepeatEvent structure.
 */
void
_DtCm_free_re(
	RepeatEvent	*re)
{
	int 		 i;

	if (!re) return;

	switch (re->re_type) {
        case RT_MINUTE:
                break;
        case RT_DAILY:
		if (RE_DAILY(re)->dd_ntime)
			free(RE_DAILY(re)->dd_time);
		if (RE_DAILY(re)) 
			free (RE_DAILY(re));
                break;
        case RT_WEEKLY:
		if (RE_WEEKLY(re)->wd_ndaytime) {
			for (i = 0; i < RE_WEEKLY(re)->wd_ndaytime; i++) {
				if (RE_WEEKLY(re)->wd_daytime[i].dt_ntime)
					free(
					  RE_WEEKLY(re)->wd_daytime[i].dt_time);
			}
			free(RE_WEEKLY(re)->wd_daytime);
		}
		if (RE_WEEKLY(re)) 
			free (RE_WEEKLY(re));
                break;
        case RT_MONTHLY_POSITION:
		if (RE_MONTHLY(re)->md_nitems) {
			for (i = 0; i < RE_MONTHLY(re)->md_nitems; i++) {
				if (RE_MONTHLY(re)->md_weektime[i].wdt_nday)
					free(RE_MONTHLY(re)->
						md_weektime[i].wdt_day);
				if (RE_MONTHLY(re)->md_weektime[i].wdt_ntime)
					free(RE_MONTHLY(re)->
						md_weektime[i].wdt_time);
				if (RE_MONTHLY(re)->md_weektime[i].wdt_nweek)
					free(RE_MONTHLY(re)->
						md_weektime[i].wdt_week);
			}
			free(RE_MONTHLY(re)->md_weektime);
		}
		if (RE_MONTHLY(re)) 
			free (RE_MONTHLY(re));
                break;
        case RT_MONTHLY_DAY:
		if (RE_MONTHLY(re)->md_nitems)
			free(RE_MONTHLY(re)->md_days);
		if (RE_MONTHLY(re)) 
			free (RE_MONTHLY(re));
                break;
        case RT_YEARLY_MONTH:
        case RT_YEARLY_DAY:
		if (RE_YEARLY(re)->yd_nitems)
			free(RE_YEARLY(re)->yd_items);
		if (RE_YEARLY(re)) 
			free(RE_YEARLY(re));
                break;
        }

	free(re);
}

/* 
 * Deallocate a RepeatEventState structure.
 */
void
_DtCm_free_re_state(
	RepeatEventState	*res)
{
	if (!res) return;

	free(res);
}
