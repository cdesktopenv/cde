/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: optstrg.c /main/3 1995/11/08 09:39:33 rswiston $ */
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
void m_optstring(char *p)
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
