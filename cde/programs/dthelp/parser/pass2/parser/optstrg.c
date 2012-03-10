/* $XConsortium: optstrg.c /main/3 1995/11/08 10:55:04 rswiston $ */
/*
Copyright 1988, 1989 Hewlett-Packard Co.
*/

#include <stdio.h>
#include <string.h>
#include "basic.h"
#include "trie.h"
#include "dtdext.h"
#include "parser.h"

/* Get program options from a string */
void m_optstring(p)
  char *p ;
  {
    if (strchr(p, 'a')) m_malftrace = TRUE ;
    if (strchr(p, 'c')) m_chtrace = TRUE ;
    if (strchr(p, 'd')) m_cdtrace = TRUE ;
    if (strchr(p, 'e')) m_entdupchk = FALSE ;
    if (strchr(p, 'h')) m_heapchk = TRUE ;
    if (strchr(p, 'l')) m_explimit = FALSE ;
    if (strchr(p, 'm')) m_conform = TRUE ;
    if (strchr(p, 's')) m_scantrace = TRUE ;
    if (strchr(p, 't')) m_tagtrace = TRUE ;
    if (strchr(p, 'w')) m_wholetag = TRUE ;

    if (strchr(p, 'A')) m_malftrace = TRUE ;
    if (strchr(p, 'C')) m_chtrace = TRUE ;
    if (strchr(p, 'D')) m_cdtrace = TRUE ;
    if (strchr(p, 'E')) m_entdupchk = FALSE ;
    if (strchr(p, 'H')) m_heapchk = TRUE ;
    if (strchr(p, 'L')) m_explimit = FALSE ;
    if (strchr(p, 'M')) m_conform = TRUE ;
    if (strchr(p, 'S')) m_scantrace = TRUE ;
    if (strchr(p, 'T')) m_tagtrace = TRUE ;
    if (strchr(p, 'W')) m_wholetag = TRUE ;
    }
