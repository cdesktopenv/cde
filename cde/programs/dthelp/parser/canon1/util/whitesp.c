/* $XConsortium: whitesp.c /main/3 1995/11/08 09:57:49 rswiston $ */
/*
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

#include "basic.h"

LOGICAL m_whitespace(
#if defined(M_PROTO)
  M_WCHAR c
#endif
  ) ;

/* Test if character c is a blank, tab, or carriage return */
#if defined(M_PROTO)
LOGICAL m_whitespace(M_WCHAR c)
#else
LOGICAL m_whitespace(c)
  M_WCHAR c ;
#endif
{
char mb_c[32]; /* arbitrarily large */
int  length;

length = wctomb(mb_c, c);
if (length != 1) return FALSE;

if (*mb_c == ' ' || *mb_c == '\t' || *mb_c == '\n') return(TRUE) ;
else return(FALSE) ;
}
