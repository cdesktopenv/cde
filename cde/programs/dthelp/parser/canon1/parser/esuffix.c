/* $XConsortium: esuffix.c /main/3 1995/11/08 09:36:52 rswiston $ */
/*
Copyright 1988, 1989 Hewlett-Packard Co.
*/

#include "userinc.h"
#include "globdec.h"
#if defined(MSDOS)
#include <process.h>
#endif

/* Write error message suffix */
void m_esuffix(M_NOPAR)
  {
    m_errline("\n") ;
    m_dumpline(m_thisfile(), m_thisline()) ;
    m_errline(":\n") ;
    m_lastchars() ;
    if (++m_errcnt == m_errlim) {
      m_error("Too many errors, processing stopped") ;
      m_exit(TRUE) ;
      }
    }

