/* $XConsortium: upstrcmp.c /main/3 1995/11/08 11:09:30 rswiston $ */
/*
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Compare uppercase versions of two strings */

#include "basic.h"

int m_upper(
#if defined(M_PROTO)
  int c
#endif
  ) ;

int m_upstrcmp(
#if defined(M_PROTO)
  char *p, char *q
#endif
  ) ;

int m_upstrcmp(p, q)
  char *p, *q ;
  {
    do {
      if (m_upper((int) *p) < m_upper((int) *q)) return(-1) ;
      if (m_upper((int) *p) > m_upper((int) *q++)) return(1) ;
      } while (*p++) ;
    return(0) ;
    }
