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
/* $XConsortium: malloc.c /main/3 1995/11/08 11:41:28 rswiston $ */
/* Copyright (c) 1988, 1989 Hewlett-Packard Co. */

/* Interfaces to free and malloc with optional debugging traces */

/**/
#include <stdlib.h>
#include <stdio.h>
#include "basic.h"

extern LOGICAL m_heapchk ;
extern LOGICAL m_malftrace ;

void m_errline(char *text);

void m_exit(int status);

void m_free(void *block, char *msg);

void m_heapdump(void);

void *m_malloc(int size, char *msg);

void *m_realloc(void *ptr, int size, char *msg);

void *m_trace(char *text);

void *m_wctrace(M_WCHAR *text);

void m_free(block, msg)
  void *block ;
  char *msg ;
  {
    char buffer[32] ;

    if (m_malftrace) {
#if defined(hpux) || defined(_AIX) || defined(sun)
      sprintf(buffer, "%5x:%5x",
        (unsigned int) ((unsigned long) block >> 16),
        (unsigned int) block, msg) ;
#else
      sprintf(buffer, "  %9p", block, msg) ;
#endif
      m_trace(buffer) ;
      m_trace("- Freed                      ") ;
      m_trace(msg) ;
      m_trace("\n") ;
      }      
    free(block) ;
    }

void *m_malloc(size, msg)
  int size ;
  char *msg ;
  {
    char buffer[32] ;
    void *p ;

    size *= sizeof(M_WCHAR);
    if (! size) return(NULL) ;
    p = (void *) malloc(size) ;
    if (! p) {
      m_errline("Unable to allocate space for ") ;
      m_errline(msg) ;
      m_errline("\n") ;
      m_exit(TRUE) ;
      }
    if (m_malftrace) {
#if defined(hpux) || defined(_AIX) || defined(sun)
      sprintf(buffer, "%5x:%5x",
        (unsigned int) ((unsigned long) p >> 16), (unsigned int) p) ;
#else
      sprintf(buffer, "  %9p", p) ;
#endif
      m_trace(buffer) ;
      m_trace("- Allocated ") ;
      sprintf(buffer, "%6d", size) ;
      m_trace(buffer) ;
      m_trace(" bytes for ") ;
      m_trace(msg) ;
      m_trace("\n") ;
      }      
    return(p) ;
    }

void *m_realloc(ptr, size, msg)
  void *ptr ;
  int size ;
  char *msg ;
  {
    char buffer[32] ;
    void *p ;

    size *= sizeof(M_WCHAR);
    if (! size) return(NULL) ;
    p = (void *) realloc(ptr, size) ;
    if (! p) {
      m_errline("Unable to re-allocate space for ") ;
      m_errline(msg) ;
      m_errline("\n") ;
      m_exit(TRUE) ;
      }
    if (m_malftrace) {
#if defined(hpux) || defined(_AIX) || defined(sun)
      sprintf(buffer, "%5x:%5x",
        (unsigned int) ((unsigned long) p >> 16), (unsigned int) p) ;
#else
      sprintf(buffer, "  %9p", p) ;
#endif
      m_trace(buffer) ;
      m_trace("- Re-allocated ") ;
      sprintf(buffer, "%6d", size) ;
      m_trace(buffer) ;
      m_trace(" bytes for ") ;
      m_trace(msg) ;
      m_trace("\n") ;
      }      
    return(p) ;
    }


