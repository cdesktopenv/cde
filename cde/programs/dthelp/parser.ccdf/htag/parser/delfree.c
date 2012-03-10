/* $XConsortium: delfree.c /main/3 1995/11/08 11:21:55 rswiston $ */
/* Copyright (c) 1988 Hewlett-Packard Co. */

/* Versions of m_free and m_malloc for debugging. */

/**/
#include <malloc.h>
#include <stdio.h>
#include "basic.h"
#include "trie.h"
#include "dtd.h"
#include "parser.h"

m_free(block, msg)
  char *block ;
  char *msg ;
  {
    free(block) ;
    fprintf(m_outfile, "%5u:%5u- Freed     %s\n",
      (unsigned int) ((unsigned long) block >> 16),
      (unsigned int) block, msg) ;
    }

char *m_mallocx(size, msg)
  int size ;
  char *msg ;
  {
    char *p ;

    if (! size) return(NULL) ;
    p = malloc(size) ;
    if (! p) {
      m_err1("Unable to allocate space for %s", msg) ;
      exit(TRUE) ;
      }
    fprintf(m_outfile, "%5u:%5u- Allocated %s\n",
      (unsigned int) ((unsigned long) p >> 16), (unsigned int) p, msg) ;
    return(p) ;
    }
