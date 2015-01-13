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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: rerule.c /main/4 1995/11/09 12:50:23 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "rerule.h"

typedef enum {
        NUM_NUM         = 0,    /* Used in NumsToBuf to tell the proc what  */
        NUM_DAY,                /* type of list we are handing it.          */
        NUM_WEEK
} NumType;

static void NumsToBuf(unsigned int *, unsigned int, NumType, char *,
                        unsigned int);
static void WeekNumberToString(WeekNumber, char *);
static void WeekDayToString(WeekDay, char *);
static void ConvertDaily(RepeatEvent *, char *, unsigned int);
static void ConvertWeekly(RepeatEvent *, char *, unsigned int);
static void ConvertMonthly(RepeatEvent *, char *, unsigned int);
static void ConvertYearly(RepeatEvent *, char *, unsigned int);

#define RE_DAILY(re) (re->re_data.re_daily)
#define RE_WEEKLY(re) (re->re_data.re_weekly)
#define RE_MONTHLY(re) (re->re_data.re_monthly)
#define RE_YEARLY(re) (re->re_data.re_yearly)

char *
ReToString(
	RepeatEvent *re)
{
	char	*cmd_buf = NULL;
	char	*cmd_buf_tmp = NULL;
	int	 cmd_buf_size = 0;
	int	 subcommand_size = 0;
	char     subcommand[1024];  /* XXX: fixed size */

	if (!re) return (char *)NULL;

	while (re) {
		switch (re->re_type) {

		case RT_MINUTE:
			sprintf(subcommand, "M%d #%d",
					re->re_interval, re->re_duration);
			break;
		case RT_DAILY:
			ConvertDaily(re, subcommand, 1024);
			break;
		case RT_WEEKLY:
			ConvertWeekly(re, subcommand, 1024);
			break;
		case RT_MONTHLY_POSITION:
		case RT_MONTHLY_DAY:
			ConvertMonthly(re, subcommand, 1024);
			break;
		case RT_YEARLY_MONTH:
		case RT_YEARLY_DAY:
			ConvertYearly(re, subcommand, 1024);
			break;
		}

		cmd_buf_tmp = cmd_buf;

		if (cmd_buf) cmd_buf_size = strlen(cmd_buf);
		if (subcommand) subcommand_size = strlen(subcommand);

		cmd_buf = (char *)calloc(1, cmd_buf_size + subcommand_size + 2);

		if (cmd_buf_tmp)
			strcat (cmd_buf, cmd_buf_tmp);
			
		if (subcommand) {
			if (cmd_buf_tmp)
				strcat (cmd_buf, " ");
			strcat (cmd_buf, subcommand);
		}

		memset (subcommand, 0, 1024);

		re = re->re_next;
	}

	return cmd_buf;
}

/*
 * Takes an array of numbers, converts them back into their string
 * type (e.g. SU 1W etc) and puts them into a string buffer with end
 * marks as necessary, seperated by spaces.
 */
static void
NumsToBuf(
	unsigned int	*array,
	unsigned int	 array_size,
	NumType		 type,
	char		*buffer,
	unsigned int	 buf_size)
{
	int 	 i,
		 size = 0; 
	char	 tmp_buf[32],
		 tmp_buf2[32];

	for (i = 0; i < array_size; i++) {
		if (type == NUM_NUM)
			sprintf(tmp_buf2, " %d", RE_MASK_STOP(array[i]));
		else if (type == NUM_DAY)
			WeekDayToString(RE_MASK_STOP(array[i]), tmp_buf2);
		else if (type == NUM_WEEK)
			WeekNumberToString(RE_MASK_STOP(array[i]), tmp_buf2);
			
			/* Add end mark if needed */
		if (RE_STOP_IS_SET(array[i])) {
			sprintf(tmp_buf, "%s$", tmp_buf2);
			strcat (buffer, tmp_buf);
			size += strlen(tmp_buf);
		} else {
			strcat (buffer, tmp_buf2);
			size += strlen(tmp_buf2);
		}

			/* Make sure the size of our buffer does not overflow */
		if (size > buf_size) {
			printf ("Error: Internal buffer size exceeded\n");
			return;
		}
	}
}

static void
WeekDayToString(
	WeekDay		 day,
	char		*buffer)
{
	switch (RE_MASK_STOP(day)) {
	case WD_SUN:
		sprintf (buffer, " SU");
		break;
	case WD_MON:
		sprintf (buffer, " MO");
		break;
	case WD_TUE:
		sprintf (buffer, " TU");
		break;
	case WD_WED:
		sprintf (buffer, " WE");
		break;
	case WD_THU:
		sprintf (buffer, " TH");
		break;
	case WD_FRI:
		sprintf (buffer, " FR");
		break;
	case WD_SAT:
		sprintf (buffer, " SA");
		break;
	}

	if (RE_STOP_IS_SET(day))
		strcat (buffer, "$");
}

static void
WeekNumberToString(
	WeekNumber	 week,
	char		*buffer)
{
	switch (RE_MASK_STOP(week)) {
	case WK_F1:
		sprintf (buffer, " 1+");
		break;
	case WK_F2:
		sprintf (buffer, " 2+");
		break;
	case WK_F3:
		sprintf (buffer, " 3+");
		break;
	case WK_F4:
		sprintf (buffer, " 4+");
		break;
	case WK_F5:
		sprintf (buffer, " 5+");
		break;
	case WK_L1:
		sprintf (buffer, " 1-");
		break;
	case WK_L2:
		sprintf (buffer, " 2-");
		break;
	case WK_L3:
		sprintf (buffer, " 3-");
		break;
	case WK_L4:
		sprintf (buffer, " 4-");
		break;
	case WK_L5:
		sprintf (buffer, " 5-");
		break;
	}

	if (RE_STOP_IS_SET(week))
		strcat (buffer, "$");
}

static void
ConvertDaily(
	RepeatEvent	*re,
	char		*subcommand,
	unsigned int	 buf_size)
{
	char	 	tmp_buf[32];
	unsigned int	size = 0,
		 	num_time,
		 	i;

	num_time = RE_DAILY(re)->dd_ntime;

	sprintf(subcommand, "D%d", re->re_interval);
	size += strlen(subcommand);

	NumsToBuf((unsigned int *)RE_DAILY(re)->dd_time, num_time, NUM_NUM,
		  subcommand, buf_size - size);

		/* Tack on the duration information */
	sprintf(tmp_buf, " #%d", re->re_duration);

	size += strlen(tmp_buf);
	if (size > buf_size) {
		printf ("Error: Internal buffer size exceeded\n");
		return;
	}
	strcat (subcommand, tmp_buf);
}

static void
ConvertWeekly(
	RepeatEvent	*re,
	char		*subcommand,
	unsigned int	 subcommand_size)
{
	char	 tmp_buf[32];
	int	 size = 0,
		 num_items,
		 i;

	num_items = RE_WEEKLY(re)->wd_ndaytime;

	sprintf(subcommand, "W%d", re->re_interval);
	size += strlen(subcommand);

		/* walk through Day/time data (e.g. TU 1200 Th 2000)	*/
	for (i = 0; i < num_items; i++) {

			/* The day: MO TU TH etc. */
		WeekDayToString(RE_WEEKLY(re)->wd_daytime[i].dt_day, tmp_buf);
		size += strlen(tmp_buf);

			/* Make sure the size of our buffer does not overflow */
		if (size > subcommand_size) {
			printf ("Error: Internal buffer size exceeded\n");
			return;
		}

		strcat (subcommand, tmp_buf);

			/* The hours: 1000 1400 etc. */
		NumsToBuf((unsigned int *)RE_WEEKLY(re)->wd_daytime[i].dt_time,
			  RE_WEEKLY(re)->wd_daytime[i].dt_ntime, NUM_NUM,
			  subcommand, subcommand_size - size);

		size = strlen(subcommand);
	}
		/* Tack on the duration information */
	sprintf(tmp_buf, " #%d", re->re_duration);

	size += strlen(tmp_buf);
	if (size > subcommand_size) {
		printf ("Error: Internal buffer size exceeded\n");
		return;
	}
	strcat (subcommand, tmp_buf);
}

static void
ConvertMonthly(
	RepeatEvent	*re,
	char		*subcommand,
	unsigned int	 subcommand_size)
{
	char	 tmp_buf[32];
	int	 size = 0,
		 num_items,
		 i;

	num_items = RE_MONTHLY(re)->md_nitems;

	if (re->re_type == RT_MONTHLY_POSITION)
		sprintf(subcommand, "MP%d", re->re_interval);
	else
		sprintf(subcommand, "MD%d", re->re_interval);

	size += strlen(subcommand);

	if (re->re_type == RT_MONTHLY_POSITION) {

		/* walk through Day/time data (e.g. TU 1200 Th 2000)	*/
	    for (i = 0; i < num_items; i++) {

			/* The week: 1+ 3- etc. */
		NumsToBuf(
			 (unsigned int*)RE_MONTHLY(re)->md_weektime[i].wdt_week,
			  RE_MONTHLY(re)->md_weektime[i].wdt_nweek, NUM_WEEK,
			  subcommand, subcommand_size - size);

		size = strlen(subcommand);

			/* Make sure the size of our buffer does not overflow */
		if (size > subcommand_size) {
			printf ("Error: Internal buffer size exceeded\n");
			return;
		}

			/* The day: SU MO TU etc. */
		NumsToBuf(
			 (unsigned int *)RE_MONTHLY(re)->md_weektime[i].wdt_day,
			  RE_MONTHLY(re)->md_weektime[i].wdt_nday, NUM_DAY,
			  subcommand, subcommand_size - size);

		size = strlen(subcommand);

			/* Make sure the size of our buffer does not overflow */
		if (size > subcommand_size) {
			printf ("Error: Internal buffer size exceeded\n");
			return;
		}

			/* The hours: 1000 1400 etc. */
		NumsToBuf(
			 (unsigned int *)
				RE_MONTHLY(re)->md_weektime[i].wdt_time,
			  RE_MONTHLY(re)->md_weektime[i].wdt_ntime, NUM_NUM,
			  subcommand, subcommand_size - size);

		size = strlen(subcommand);
	    }
	} else {  /* RT_MONTHLY_DAY */
			/* The days: 1 15 31 etc. */
		NumsToBuf((unsigned int *)RE_MONTHLY(re)->md_days,
			  num_items, NUM_NUM, subcommand,
			  subcommand_size - size);
	}
		/* Tack on the duration information */
	sprintf(tmp_buf, " #%d", re->re_duration);

	size += strlen(tmp_buf);
	if (size > subcommand_size) {
		printf ("Error: Internal buffer size exceeded\n");
		return;
	}
	strcat (subcommand, tmp_buf);
}

static void
ConvertYearly(
	RepeatEvent	*re,
	char		*subcommand,
	unsigned int	 subcommand_size)
{
	char	 tmp_buf[32];
	int	 size = 0,
		 num_items,
		 i;

	num_items = RE_YEARLY(re)->yd_nitems;

	if (re->re_type == RT_YEARLY_MONTH)
		sprintf(subcommand, "YM%d", re->re_interval);
	else
		sprintf(subcommand, "YD%d", re->re_interval);

	size += strlen(subcommand);

		/* An array of days or months */
	NumsToBuf(RE_YEARLY(re)->yd_items,
		  RE_YEARLY(re)->yd_nitems, NUM_NUM,
		  subcommand, subcommand_size - size);

		/* Tack on the duration information */
	sprintf(tmp_buf, " #%d", re->re_duration);

	size += strlen(tmp_buf);
	if (size > subcommand_size) {
		printf ("Error: Internal buffer size exceeded\n");
		return;
	}
	strcat (subcommand, tmp_buf);
}
