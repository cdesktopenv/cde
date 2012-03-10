/* $XConsortium: errline.c /main/3 1995/11/08 11:22:46 rswiston $ */
/*
Copyright 1988, 1989 Hewlett-Packard Co.
*/

#include "userinc.h"
#include "globdec.h"

/* Process error message text */
void m_errline(p)
M_WCHAR *p ;
{
char mb_p;

mb_p = MakeMByteString(p);
fputs(mb_p, m_errfile) ;
fputs(mb_p, stderr) ;
m_free(mb_p,"multi-byte string");
}

