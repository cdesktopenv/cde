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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: cyclent.c /main/3 1995/11/08 11:21:36 rswiston $ */
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

M_WCHAR *m_cyclent(LOGICAL init, unsigned char *type, M_WCHAR **content, unsigned char *wheredef)
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

