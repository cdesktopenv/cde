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
/* $XConsortium: entity.c /main/3 1995/11/08 10:05:17 rswiston $ */
/* Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co. */

/* Entity.c has ELTDEF procedures relevant to entities */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "basic.h"
#include "trie.h"
#include "dtdext.h"
#include "eltdef.h"
#include "entext.h"

/* Add an entity */
void addent(M_NOPAR)
  {
    M_ENTITY *new ;

    new = (M_ENTITY *) m_malloc(sizeof(M_ENTITY), "entity") ;
    if (entity = (M_ENTITY *) m_ntrtrie(name, m_enttrie, (M_TRIE *) new)) {
      m_free(new, "entity") ;
      if (! entity->wheredef) return ;
      if (entity->wheredef == M_DBUILD) {
        warning1("Redefining %s: entity defined in BUILD", entity->name) ;
        entity->type = M_GENERAL ;
        entity->wheredef = FALSE ;
        entity->content = NULL ;
        return ;
        }
      else
        m_err1("Attempt to redefine %s", entity->name) ;
      return ;
      }
    entity = new ;
    if (lastent) lastent->next = entity ;
    else firstent = entity ;
    lastent = entity ;
    entity->type = M_GENERAL ;
    entity->wheredef = FALSE ;
    entity->content = NULL ;
    entity->name =
      (M_WCHAR *) m_malloc(w_strlen(name) + 1, "entity name") ;
    entity->index = ++m_entcnt ;
    entity->codeindex = M_NULLVAL ;
    entity->next = NULL ;
    w_strcpy(entity->name, name) ;
    return ;
    }

#include "entout.c"
