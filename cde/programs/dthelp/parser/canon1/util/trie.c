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
/* $XConsortium: trie.c /main/3 1995/11/08 09:55:10 rswiston $ */
/*
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Trie.c contains procedures for maintaining the tree structure
   used to store element names, delimiter strings, short reference strings,
   etc. */

#include <stdio.h>

#if defined(MSDOS)
#include <stdlib.h>
#endif

#include "basic.h"
#include "common.h"
#include "trie.h"

extern int m_line ;

extern FILE *m_errfile ;

extern M_CHARTYPE m_ctarray[M_CHARSETLEN] ;

void *m_malloc(
#if defined(M_PROTO)
  int size, char *msg
#endif
  ) ;

void m_entercharintrie(
#if defined(M_PROTO)
  M_TRIE **currentnode, M_WCHAR c
#endif
  ) ;

/* Enters the next character of a string into a trie */
#if defined(M_PROTO)
void m_entercharintrie(M_TRIE **currentnode, M_WCHAR c)
#else
void m_entercharintrie(currentnode, c)
  M_TRIE **currentnode ;
  M_WCHAR c ;
#endif
  {
    M_TRIE *newnode ;

    if (! (*currentnode)->data) {
      (*currentnode)->data = m_gettrienode() ;
      *currentnode = (*currentnode)->data ;
      (*currentnode)->next = M_NULLVAL ;
      (*currentnode)->symbol = c ;
      (*currentnode)->data = M_NULLVAL ;
      } /* end insert a son */
    else if ((*currentnode)->data->symbol > c) {
      newnode = m_gettrienode() ;
      newnode->next = (*currentnode)->data ;
      (*currentnode)->data = newnode ;
      *currentnode = (*currentnode)->data ;
      (*currentnode)->symbol = c ;
      (*currentnode)->data = M_NULLVAL ;
      } /* end insert before first son */
    else {
      for (*currentnode = (*currentnode)->data ;
           (*currentnode)->next &&(*currentnode)->next->symbol <= c;
           *currentnode = (*currentnode)->next ) ;
      if ((*currentnode)->symbol != c) {
        newnode = m_gettrienode() ;
        newnode->next = (*currentnode)->next ;
        (*currentnode)->next = newnode ;
        *currentnode = (*currentnode)->next ;
        (*currentnode)->symbol = c ;
        (*currentnode)->data = M_NULLVAL ;
        } /* end insert node in descendant list */
      } /* end check descendant list */
   } /* end entertree */

/* Gets a new node for a trie */
M_TRIE *m_gettrienode(M_NOPAR)
  {
    M_TRIE *new ;

    new = (M_TRIE *) m_malloc(sizeof(M_TRIE), "trie") ;
    new->symbol = 0 ;
    new->next = new->data = NULL ;
    return(new) ;
    } /*end m_gettrienode */


/* M_lookfortrie(p, xtrie) looks for string p in the specified trie,
   returning its data value if found and otherwise FALSE */
#if defined(M_PROTO)
void *m_lookfortrie( const M_WCHAR *p , const M_TRIE *xtrie )
#else
void *m_lookfortrie(p, xtrie)
  M_WCHAR *p ;
  M_TRIE *xtrie ;
#endif /* M_PROTO */
  {
    M_TRIE *currentnode ;

    currentnode = xtrie->data ;
    while (TRUE) {
      if (! currentnode) return(NULL) ;
      if (currentnode->symbol == m_ctupper(*p))
        if (! *p) return((void *) currentnode->data) ;
        else {
          p++ ;
          currentnode = currentnode->data ;
          continue ;
          }
      else if (currentnode->symbol < *p) {
        currentnode = currentnode->next ;
        continue ;
        }
      else return(NULL) ;
      }
    }

/* Enters a string and associated data value into a trie */
void *m_ntrtrie(p, xtrie, dataval)
  M_WCHAR *p ;
  M_TRIE *xtrie ;
  void *dataval ;
  {
    M_TRIE *currentnode ;
    void *n ;

    if ((n = m_lookfortrie(p, xtrie))) return(n) ;
    currentnode = xtrie ;
    for ( ; *p ; p++)
      m_entercharintrie(&currentnode, m_ctupper(*p)) ;
    m_entercharintrie(&currentnode, M_EOS) ;
    currentnode->data = (M_TRIE *) dataval ;
    return(NULL) ;
    }
