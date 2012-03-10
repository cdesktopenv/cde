/* $XConsortium: upper.c /main/3 1995/11/08 11:09:21 rswiston $ */
/*
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Force a letter to uppercase */

#include "basic.h"

int m_upper(
#if defined(M_PROTO)
  int c
#endif
  ) ;

int m_upper(c)
  int c ;
  {
    if (c >= 'a' && c <= 'z') c += 'A' - 'a' ;
    return(c) ;
    }
