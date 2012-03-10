/* $XConsortium: openent.c /main/3 1995/11/08 09:39:07 rswiston $ */
/* Copyright 1988, 1989 Hewlett-Packard Co. */

/* Default open SYSTEM entity procedure */

#include "userinc.h"
#include "globdec.h"

void *m_openent(entcontent)
  M_WCHAR *entcontent ;
  {
    return((void *) fopen(entcontent, "r")) ;
    }

