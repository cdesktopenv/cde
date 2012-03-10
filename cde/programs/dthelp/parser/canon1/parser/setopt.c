/* $XConsortium: setopt.c /main/3 1995/11/08 09:42:58 rswiston $ */
/*
Copyright 1988, 1989 Hewlett-Packard Co.
*/

#include "userinc.h"
#include "globdec.h"


/* Set program options */
void m_setoptions(M_NOPAR)
  {
    if (m_argc > 1) m_optstring(m_argv[1]) ;
    }
