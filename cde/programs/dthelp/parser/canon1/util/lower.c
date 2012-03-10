/* $XConsortium: lower.c /main/3 1995/11/08 09:52:41 rswiston $ */
/*
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

#include "basic.h"

int m_lower(
#if defined(M_PROTO)
  int c
#endif
  ) ;



/* Force a letter to lowercase */
int m_lower(c)
  int c ;
  {
    if (c >= 'A' && c <= 'Z') c += 'a' - 'A' ;
    return(c) ;
    }
