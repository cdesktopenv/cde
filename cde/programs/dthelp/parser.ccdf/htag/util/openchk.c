/* $XConsortium: openchk.c /main/3 1995/11/08 11:41:47 rswiston $ */
/*
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

#include <stdio.h>
#if defined(MSDOS)
#include <process.h>
#endif
#include "basic.h"

extern FILE *m_errfile ;

void m_exit(
#if defined(M_PROTO)
  int status
#endif
  ) ;

void m_openchk(
#if defined(M_PROTO)
  FILE **ptr, char *name, char *mode
#endif
  ) ;

/* Open a file and check that the open succeeded */
void m_openchk(ptr, name, mode)
  FILE **ptr ;
  char *name, *mode ;
  {
    *ptr = fopen(name, mode) ;
    if (*ptr) return ;
    fprintf(stderr, "Unable to open %s\n", name) ;
    if (m_errfile) fprintf(m_errfile, "Unable to open %s\n", name) ;
    m_exit(TRUE) ;
    }
