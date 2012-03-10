/* $XConsortium: tree.c /main/3 1995/11/08 11:13:14 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Tree.c has procedures used by BUILD to construct the tree representation
   of a content model */

#include <malloc.h>
#include "build.h"
#include "context.h"

/* Called after a right parenthesis is read while scanning a content model.
   Resets curcon if it is the outermost submodel. */
void endmodel(M_NOPAR)
  {
    curtree = curtree->parent ;
    if (! curtree->parent) curcon = LASTM ;
    else if (curtree == curtree->parent->first) curcon = OCCUR ;
    }

/* Release storage used for the tree representation of a rule */
void freetree(treep)
  TREE *treep ;
  {
    TREE *child, *discard ;

    for (child = treep->first ; child ; ) {
      discard = child->right ;
      freetree(child) ;
      child = discard ;
      }
    m_free((M_POINTER) treep, "tree node") ;
    }

/* Gettreenode obtains a node to use in the tree representation of a rule */
TREE *gettreenode(M_NOPAR)
  {
    TREE *new ;

    new = (TREE *) m_malloc(sizeof(TREE), "tree node") ;
    new->first = new->right = new->parent = NULL ;
    new->connector = new->occurrence = M_NULLVAL ;
    new->value = NULL ;
    new->terminal = FALSE ;
    new->minim = FALSE ;
    new->eltid = eltsinrule++ ;
    return(new) ;
    }

