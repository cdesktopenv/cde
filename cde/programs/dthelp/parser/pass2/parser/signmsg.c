/* $XConsortium: signmsg.c /main/3 1995/11/08 10:58:59 rswiston $ */
/*
Copyright 1988, 1989 Hewlett-Packard Co.
*/

#include "userinc.h"
#include "globdec.h"

/* Process signon message text */
void m_signmsg(p)
M_WCHAR *p ;
{
char mb_p;

mb_p = MakeMByteString(p);
m_errline(mb_p) ;
m_free(mb_p,"multi-byte string");
}
