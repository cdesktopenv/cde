/* $XConsortium: sffcvt.c /main/2 1996/05/08 19:57:51 drk $ */
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
#include	"sfhdr.h"

#if __STD_C
char *sffcvt(double dval, int n_digit, int* decpt, int* sign)
#else
char *sffcvt(dval,n_digit,decpt,sign)
double	dval;		/* value to convert */
int	n_digit;	/* number of digits wanted */
int*	decpt;		/* to return decimal point */
int*	sign;		/* to return sign */
#endif
{
	return _sfcvt((Double_t)dval,n_digit,decpt,sign,0);
}
