/* $XConsortium: letter.c /main/3 1995/11/08 10:33:31 rswiston $ */
/* Copyright 1987, 1988, 1989 Hewlett-Packard Co. */

/* Tests whether a character is a letter */

#include "basic.h"

LOGICAL m_letter(
#if defined(M_PROTO)
  M_WCHAR c
#endif
  ) ;

#if defined(M_PROTO)
LOGICAL m_letter(M_WCHAR c)
#else
LOGICAL m_letter(c)
  M_WCHAR c ;
#endif
{
if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return(TRUE) ;
return(FALSE) ;
}
