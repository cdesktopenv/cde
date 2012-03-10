/* $XConsortium: getc.c /main/3 1995/11/08 09:37:33 rswiston $ */
/* Copyright 1988, 1989 Hewlett-Packard Co. */

/* Default get-char procedure */

#include "userinc.h"
#include "globdec.h"

int m_getc(m_ptr)
  void *m_ptr ;
  {
    return(getc((FILE*) m_ptr)) ;
    }
