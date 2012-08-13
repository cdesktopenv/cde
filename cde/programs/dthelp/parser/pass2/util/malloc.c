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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: malloc.c /main/3 1995/11/08 11:06:50 rswiston $ */
/* Copyright (c) 1988, 1989 Hewlett-Packard Co. */

/* Interfaces to free and malloc with optional debugging traces */

/**/
#include <stdlib.h>
#include <stdio.h>
#if defined(MSDOS)
#include <process.h>
#endif
#include "basic.h"

extern LOGICAL m_heapchk ;
extern LOGICAL m_malftrace ;

void m_errline(
#if defined(M_PROTO)
  char *text
#endif
  ) ;

void m_exit(
#if defined(M_PROTO)
  int status
#endif
  ) ;

void m_free(
#if defined(M_PROTO)
  void *block, char *msg
#endif
  ) ;

void m_heapdump(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void *m_malloc(
#if defined(M_PROTO)
  int size, char *msg
#endif
  ) ;

void *m_realloc(
#if defined(M_PROTO)
  void *ptr, int size, char *msg
#endif
  ) ;

void *m_trace(
#if defined(M_PROTO)
  char *text
#endif
  ) ;

void *m_wctrace(
#if defined(M_PROTO)
  M_WCHAR *text
#endif
  ) ;

void m_free(block, msg)
  void *block ;
  char *msg ;
  {
    char buffer[32] ;

#if defined(MSDOS)
    if (m_heapchk) m_heapdump() ;
#endif
    free(block) ;
    if (m_malftrace) {
#if defined(hpux) || defined(_AIX) || defined(sun) || defined(USL) || defined(__uxp__)
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
#if defined(MSDOS)
    if (m_heapchk) m_heapdump() ;
#endif
    }

#if defined(MSDOS)
void m_heapdump(M_NOPAR)
  {
    struct _heapinfo hinfo ;
    int heapstatus ;

    heapstatus = _heapchk() ;
    if (heapstatus == _HEAPOK || heapstatus == _HEAPEMPTY) return ;
    printf("\nDumping heap:\n") ;
    hinfo._pentry = NULL ;
    while ((heapstatus = _heapwalk(&hinfo)) == _HEAPOK) 
      printf("%6s block at %p of size %4.4X\n",
             hinfo._useflag == _USEDENTRY ? "USED" : "FREE",
             hinfo._pentry, hinfo._size) ;
    switch(heapstatus) {
      case _HEAPEMPTY:
        printf("OK - empty heap\n\n") ;
        break ;
      case _HEAPEND:
        printf("OK - end of heap\n\n") ;
        break ;
      case _HEAPBADPTR:
        printf("Error - bad pointer to heap\n\n") ;
        break ;
      case _HEAPBADBEGIN:
        printf("Error - bad start of heap\n\n") ;
        break ;
      case _HEAPBADNODE:
        printf("Error - bad node in heap\n\n") ;
        break ;
      }
    m_exit(TRUE) ;
    }
#endif

void *m_malloc(size, msg)
  int size ;
  char *msg ;
  {
    char buffer[32] ;
    void *p ;

    size *= sizeof(M_WCHAR);
#if defined(MSDOS)
    if (m_heapchk) m_heapdump() ;
#endif
    if (! size) return(NULL) ;
    p = (void *) malloc(size) ;
#if defined(MSDOS)
    if (m_heapchk) m_heapdump() ;
#endif
    if (! p) {
      m_errline("Unable to allocate space for ") ;
      m_errline(msg) ;
      m_errline("\n") ;
      m_exit(TRUE) ;
      }
    if (m_malftrace) {
#if defined(hpux) || defined(_AIX) || defined(sun) || defined(USL) || defined(__uxp__)
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
#if defined(MSDOS)
    if (m_heapchk) m_heapdump() ;
#endif
    if (! size) return(NULL) ;
    p = (void *) realloc(ptr, size) ;
#if defined(MSDOS)
    if (m_heapchk) m_heapdump() ;
#endif
    if (! p) {
      m_errline("Unable to re-allocate space for ") ;
      m_errline(msg) ;
      m_errline("\n") ;
      m_exit(TRUE) ;
      }
    if (m_malftrace) {
#if defined(hpux) || defined(_AIX) || defined(sun) || defined(USL) || defined(__uxp__)
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


