/* $XConsortium: eltree.c /main/3 1995/11/08 10:42:15 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Eltree.c contains procedures that manipulate element names */

#include <string.h>
#include <malloc.h>
#include "build.h"

/* Enters an element name into the element name tree */
ELTSTRUCT *ntrelt(p)
  M_WCHAR *p ;
  {
    ELTSTRUCT *new ;
    ELTSTRUCT *old ;
    int length ;

    new = (ELTSTRUCT *) m_malloc(sizeof(ELTSTRUCT), "element structure") ;
    if (old = (ELTSTRUCT *) m_ntrtrie(p, &eltree, (M_TRIE *) new)) {
      m_free((M_POINTER) new, "element structure") ;
      return(old) ;
      }
    *nextelt = new ;
    nextelt = &new->next ;
    new->eltno = ++ecount ;
    length = w_strlen(p) + 1 ;
    enamelen += length ;
    new->enptr = (M_WCHAR *) m_malloc(length, "element name") ;
    w_strcpy(new->enptr, p) ;
    new->model = M_NULLVAL ;
    new->content = M_NULLVAL ;
    new->inptr = new->exptr = NULL ;
    new->parptr = NULL ;
    new->parindex = M_NULLVAL ;
    new->paramcount = M_NULLVAL ;
    new->stmin = new->etmin = FALSE ;
    new->srefptr = M_NULLVAL ;
    new->useoradd = TRUE ; 
    new->next = NULL ;
    return(new) ;
    }
