/* $XConsortium: parent.c /main/3 1995/11/08 10:56:02 rswiston $ */
/*
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

#include <stdio.h>
#include "basic.h"
#include "trie.h"
#include "dtdext.h"
#include "parser.h"

/* Procedure callable by interface designers.  Returns pointer to name
   of nth level parent of current element (0 is self, 1 is parent, 2
   is grandparent, etc.) */
M_WCHAR *m_parent(n)
  int n ;
  {
    M_PARSE *stackptr ;

    for (stackptr = m_stacktop ;
         stackptr->oldtop && n >= 0 ;
         stackptr = stackptr->oldtop, n--) {
      if (! n)
        return(m_nameofelt(stackptr->element)) ;
      }
    return(NULL) ;
    }
