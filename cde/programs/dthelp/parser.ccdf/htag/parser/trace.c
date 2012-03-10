/* $XConsortium: trace.c /main/3 1995/11/08 11:33:58 rswiston $ */
/*
Copyright 1988, 1989 Hewlett-Packard Co.
*/

#include "userinc.h"
#include "globdec.h"

/* Write debugging trace information */
void m_wctrace(p)
M_WCHAR *p ;
{
char *mb_p;

mb_p = MakeMByteString(p);
fputs(mb_p, m_outfile) ;
m_free(mb_p,"multi-byte string");
}

void m_trace(p)
char *p ;
{
fputs(p, m_outfile);
}
