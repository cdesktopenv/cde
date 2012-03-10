/* $XConsortium: string.c /main/3 1995/11/08 11:32:47 rswiston $ */
/* Copyright 1988, 1989 Hewlett-Packard Co. */

/* String.c executes string-code specified in the interface. */

#include "userinc.h"
#include "globdec.h"

/* Include generated code */
#include "stfile.c"

#if defined(M_PROTO)
void m_stcaction(M_WCHAR *m_string, LOGICAL M_START, LOGICAL M_END)
#else
void m_stcaction(m_string, M_START, M_END)
  M_WCHAR *m_string ;
  LOGICAL M_START ;
  LOGICAL M_END ;
#endif
{
    m_stackpar = m_stacktop->stparam ;
    (*m_sttable[m_stacktop->stccase])(m_string, M_START, M_END) ;
    }
