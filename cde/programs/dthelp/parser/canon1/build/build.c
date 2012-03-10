/* $XConsortium: build.c /main/3 1995/11/08 09:24:44 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Build.c contains the main procedure for program BUILD */

#include <string.h>
#include <malloc.h>
#if defined(MSDOS)
#include <process.h>
#endif
#include "context.h"
#define BUILDEF
#include "build.h"
#include "delim.h"
#include "entdef.h"

/* Main procedure */
void main(argc, argv)
int argc ;
char **argv ;
{
int m_prevcon ;

if (argc > 1)
    {
    if (strchr(argv[1], 'a')) m_malftrace = TRUE ;
    if (strchr(argv[1], 'A')) m_malftrace = TRUE ;
    if (strchr(argv[1], 'a')) m_malftrace = TRUE ;
    if (strchr(argv[1], 'H')) m_heapchk = TRUE ;
    if (strchr(argv[1], 'm')) standard = TRUE ;
    if (strchr(argv[1], 'M')) standard = TRUE ;
    if (strchr(argv[1], 's')) scantrace = TRUE ;
    if (strchr(argv[1], 'S')) scantrace = TRUE ;
    }
initialize() ;
while (TRUE)
    {
    m_token = scan() ;
    m_prevcon = curcon ;
    curcon = m_newcon(m_prevcon - 1, m_token - 1) ;
    if (scantrace)
    printf(
    "m_prevcon=%d, m_token=%d, curcon=%d, scanval = %c (%d), line=%d\n",
    m_prevcon, m_token, curcon, scanval, scanval, m_line) ;
    #include "case.c"
    if (m_token == ENDFILE) exit(FALSE) ;
    }
}
