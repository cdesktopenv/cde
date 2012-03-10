/* $XConsortium: _sffcvt.c /main/3 1995/11/01 18:24:27 rswiston $ */
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
static char* __sffcvt(reg double dval, int n_digit, int* decpt, int* sign)
#else
static char* __sffcvt(dval,n_digit,decpt,sign)
reg double	dval;		/* value to convert */
int		n_digit;	/* number of digits wanted */
int		*decpt;		/* to return decimal point */
int		*sign;		/* to return sign */
#endif
{
	return sffcvt(dval,n_digit,decpt,sign);
}

#undef sffcvt

#if __STD_C
char *sffcvt(reg double dval, int n_digit, int* decpt, int* sign)
#else
char *sffcvt(dval,n_digit,decpt,sign)
reg double	dval;		/* value to convert */
int		n_digit;	/* number of digits wanted */
int		*decpt;		/* to return decimal point */
int		*sign;		/* to return sign */
#endif
{
	return __sffcvt(dval,n_digit,decpt,sign);
}
