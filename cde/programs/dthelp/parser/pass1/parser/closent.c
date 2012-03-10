/* $XConsortium: closent.c /main/3 1995/11/08 10:15:34 rswiston $ */
/* Copyright 1988, 1989 Hewlett-Packard Co. */

/* Default close SYSTEM entity procedure */

#include "userinc.h"
#include "globdec.h"

void m_closent(m_ptr)
  void *m_ptr ;
  {
    fclose((FILE *) m_ptr) ;
    }

