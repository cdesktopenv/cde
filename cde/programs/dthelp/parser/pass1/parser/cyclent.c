/* $XConsortium: cyclent.c /main/3 1995/11/08 10:15:57 rswiston $ */
/*
                   Copyright 1988, 1989 Hewlett-Packard Co.
*/

/* Cyclent.c contains procedure m_cyclent(), callable by interface
   designers, to cycle through all defined entities, returning information
   about them */

#include <stdio.h>
#include "basic.h"
#include "trie.h"
#include "dtdext.h"
#include "parser.h"
#include "entext.h"

#if defined(M_PROTO)
M_WCHAR *m_cyclent(LOGICAL init, unsigned char *type, M_WCHAR **content, unsigned char *wheredef)
#else
M_WCHAR *m_cyclent(init, type, content, wheredef)
  LOGICAL init ;
  unsigned char *type ;
  M_WCHAR **content ;
  unsigned char *wheredef ;
#endif
{
    static M_TRIE *current ;
    static M_TRIE *ancestor[M_NAMELEN + 1] ;
    static length = 0 ;
    static M_WCHAR name[M_NAMELEN + 1] ;

    if (init) {
      current = m_enttrie->data ;
      length = 0 ;
      }
    if (length < 0) return(NULL) ;
    while (current->symbol) {
      ancestor[length] = current ;
      name[length++] = current->symbol ;
      current = current->data ;
      }
    name[length] = M_EOS ;
    *type = ((M_ENTITY *) current->data)->type ;
    *content = ((M_ENTITY *) current->data)->content ;
    *wheredef = ((M_ENTITY *) current->data)->wheredef ;
    while (length >= 0) {
      if (current->next) {
        current = current->next ;
        break ;
        }
      length-- ;
      if (length < 0) break ;
      current = ancestor[length] ;
      }
    return(name) ;
    }

