/* $XConsortium: level.c /main/3 1995/11/08 10:53:50 rswiston $ */
/*
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

#include <stdio.h>
#include "basic.h"
#include "trie.h"
#include "dtdext.h"
#include "parser.h"

/* Procedure callable by interface designer.  Returns number of occurrences
   of element on parse stack */
int m_level(elt)
  M_WCHAR *elt ;
  {
    int i = 0 ;
    M_PARSE *stackptr ;

    for (stackptr = m_stacktop ;
         stackptr->oldtop ;
         stackptr = stackptr->oldtop) {
      if (! m_wcupstrcmp(elt, m_nameofelt(stackptr->element))) i++ ;
      }
    return(i) ;
    }


int m_mblevel(elt)
  char *elt ;
  {
  int     retval;
  M_WCHAR *wc_elt;

  wc_elt = MakeWideCharString(elt);
  retval = m_level(wc_elt);
  m_free(wc_elt,"wide character string");
  return(retval);
  }
