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
/* $XConsortium: tree.c /main/3 1995/11/08 10:44:17 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Tree.c has procedures used by BUILD to construct the tree representation
   of a content model */

#include <stdlib.h>
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

