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
/* $XConsortium: _sfecvt.c /main/3 1995/11/01 18:23:48 rswiston $ */
/***************************************************************
*                                                              *
*                      AT&T - PROPRIETARY                      *
*                                                              *
*        THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF        *
*                    AT&T BELL LABORATORIES                    *
*         AND IS NOT TO BE DISCLOSED OR USED EXCEPT IN         *
*            ACCORDANCE WITH APPLICABLE AGREEMENTS             *
*                                                              *
*          Copyright (c) 1994 AT&T Bell Laboratories           *
*              Unpublished & Not for Publication               *
*                     All Rights Reserved                      *
*                                                              *
*       The copyright notice above does not evidence any       *
*      actual or intended publication of such source code      *
*                                                              *
*               This software was created by the               *
*           Software Engineering Research Department           *
*                    AT&T Bell Laboratories                    *
*                                                              *
*               For further information contact                *
*                   advsoft@research.att.com                   *
*                 Randy Hackbarth 908-582-5245                 *
*                  Dave Belanger 908-582-7427                  *
*                                                              *
***************************************************************/

#line 1
#include	"sfhdr.h"

#if __STD_C
static char *__sfecvt(reg double dval, int n_digit, int* decpt, int* sign)
#else
static char *__sfecvt(dval,n_digit,decpt,sign)
reg double	dval;		/* value to convert */
int		n_digit;	/* number of digits wanted */
int		*decpt;		/* to return decimal point */
int		*sign;		/* to return sign */
#endif
{
	return sfecvt(dval,n_digit,decpt,sign);
}

#undef sfecvt

#if __STD_C
char *sfecvt(reg double dval, int n_digit, int* decpt, int* sign)
#else
char *sfecvt(dval,n_digit,decpt,sign)
reg double	dval;		/* value to convert */
int		n_digit;	/* number of digits wanted */
int		*decpt;		/* to return decimal point */
int		*sign;		/* to return sign */
#endif
{
	return __sfecvt(dval,n_digit,decpt,sign);
}
