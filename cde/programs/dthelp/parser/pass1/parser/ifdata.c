/* $XConsortium: ifdata.c /main/3 1995/11/08 10:19:44 rswiston $ */
/* Copyright 1988, 1989 Hewlett-Packard Co. */

/* Ifdata.c contains functions used by the interface to access the
   single item of interface-determined data stored on the parse stack. */

#include <stdio.h>
#include "basic.h"
#include "trie.h"
#include "dtdext.h"
#include "parser.h"

/* Retrieve the interface data stored with the current element's nth parent */
void *m_getdata(n, flag)
  int n ;
  LOGICAL *flag ;
  {
    M_PARSE *stackptr ;

    for (stackptr = m_stacktop ;
         stackptr->oldtop && n >= 0 ;
         stackptr = stackptr->oldtop, n--) {
      if (! n) {
        *flag = TRUE ;
        return(stackptr->ifdata) ;
        }
      }
    *flag = FALSE ;
    return(NULL) ;
    }

/* Store interface data for the current element's nth parent */
LOGICAL m_putdata(data, n)
  void *data ;
  int n ;
  {
    M_PARSE *stackptr ;

    for (stackptr = m_stacktop ;
         stackptr->oldtop && n >= 0 ;
         stackptr = stackptr->oldtop, n--) {
      if (! n) {
        stackptr->ifdata = data ;
        return(TRUE) ;
        }
      }
    return(FALSE) ;
    }

