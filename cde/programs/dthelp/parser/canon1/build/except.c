/* $XConsortium: except.c /main/3 1995/11/08 09:25:25 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Except.c contains procedures related to exceptions for program BUILD */

#include "build.h"

/* Add an exception (inclusion or exclusion) for the current element */
void addex(M_NOPAR)
  {
    excount++ ;
    *nextex = (EXCEPTION *) m_malloc(sizeof(EXCEPTION), "exception") ;
    if (exlist) exlist->next = *nextex ;
    exlist = *nextex ;
    (*nextex)->next = NULL ;
    (*nextex)->nextptr = NULL ;
    (*nextex)->element = ntrelt(name)->eltno ;
    nextex = &(*nextex)->nextptr ;
    }
