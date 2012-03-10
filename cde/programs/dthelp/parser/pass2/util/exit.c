/* $XConsortium: exit.c /main/3 1995/11/08 11:05:12 rswiston $ */
/*
Copyright 1988, 1989 Hewlett-Packard Co.
*/

#if defined(MSDOS)
#include <process.h>
#endif
#include "basic.h"

void m_exit(
#if defined(M_PROTO)
  int status
#endif
  ) ;

/* Exit function called by utilities used in PARSER and other programs in
   the MARKUP system */
void m_exit(status)
  int status ;
  {
    exit(status) ;
    }

