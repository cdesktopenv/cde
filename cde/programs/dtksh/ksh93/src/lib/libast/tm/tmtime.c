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
/* $XConsortium: tmtime.c /main/3 1995/11/01 18:55:48 rswiston $ */
/***************************************************************
*                                                              *
*                      AT&T - PROPRIETARY                      *
*                                                              *
*         THIS IS PROPRIETARY SOURCE CODE LICENSED BY          *
*                          AT&T CORP.                          *
*                                                              *
*                Copyright (c) 1995 AT&T Corp.                 *
*                     All Rights Reserved                      *
*                                                              *
*           This software is licensed by AT&T Corp.            *
*       under the terms and conditions of the license in       *
*       http://www.research.att.com/orgs/ssr/book/reuse        *
*                                                              *
*               This software was created by the               *
*           Software Engineering Research Department           *
*                    AT&T Bell Laboratories                    *
*                                                              *
*               For further information contact                *
*                     gsf@research.att.com                     *
*                                                              *
***************************************************************/

/* : : generated by proto : : */

#if !defined(__PROTO__)
#if defined(__STDC__) || defined(__cplusplus) || defined(_proto) || defined(c_plusplus)
#if defined(__cplusplus)
#define __MANGLE__	"C"
#else
#define __MANGLE__
#endif
#define __STDARG__
#define __PROTO__(x)	x
#define __OTORP__(x)
#define __PARAM__(n,o)	n
#if !defined(__STDC__) && !defined(__cplusplus)
#if !defined(c_plusplus)
#define const
#endif
#define signed
#define void		int
#define volatile
#define __V_		char
#else
#define __V_		void
#endif
#else
#define __PROTO__(x)	()
#define __OTORP__(x)	x
#define __PARAM__(n,o)	o
#define __MANGLE__
#define __V_		char
#define const
#define signed
#define void		int
#define volatile
#endif
#if defined(__cplusplus) || defined(c_plusplus)
#define __VARARG__	...
#else
#define __VARARG__
#endif
#if defined(__STDARG__)
#define __VA_START__(p,a)	va_start(p,a)
#else
#define __VA_START__(p,a)	va_start(p)
#endif
#endif
#include <ast.h>
#include <tm.h>

/*
 * convert Tm_t to time_t
 *
 * if west==TM_LOCALZONE then the local timezone is used
 * otherwise west is the number of minutes west
 * of GMT with DST taken into account
 */

time_t
tmtime __PARAM__((register Tm_t* tp, int west), (tp, west)) __OTORP__(register Tm_t* tp; int west;){
	register time_t		clock;
	register Tm_leap_t*	lp;
	int			n;
	int			sec;
	time_t			now;

	tmset(tm_info.zone);
	clock = (tp->tm_year * (4 * 365 + 1) - 69) / 4 - 70 * 365;
	if ((n = tp->tm_mon) > 11) n = 11;
	if (n > 1 && !(tp->tm_year % 4)) clock++;
	clock += tm_data.sum[n] + tp->tm_mday - 1;
	clock *= 24;
	clock += tp->tm_hour;
	clock *= 60;
	clock += tp->tm_min;
	clock *= 60;
	clock += sec = tp->tm_sec;
	if (!(tm_info.flags & TM_UTC))
	{
		/*
		 * time zone adjustments
		 */

		if (west == TM_LOCALZONE)
		{
			clock += tm_info.zone->west * 60;
			now = clock;
			if (tmmake(&now)->tm_isdst) clock += tm_info.zone->dst * 60;
		}
		else clock += west * 60;
	}
	if (tm_info.flags & TM_LEAP)
	{
		/*
		 * leap second adjustments
		 */

		if (clock > 0)
		{
			for (lp = &tm_data.leap[0]; clock < lp->time - (lp+1)->total; lp++);
			clock += lp->total;
			n = lp->total - (lp+1)->total;
			if (clock <= (lp->time + n) && (n > 0 && sec > 59 || n < 0 && sec > (59 + n) && sec <= 59)) clock -= n;
		}
	}
	return(clock);
}
