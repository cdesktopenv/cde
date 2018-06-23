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
/* $XConsortium: lookent.c /main/3 1995/11/08 10:54:08 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Lookent.c contains procedure m_lookent(), callable by interface
   designers, to return entity type and content.  Useful for entity
   parameters */

#include <stdio.h>
#include "basic.h"
#include "trie.h"
#include "dtdext.h"
#include "parser.h"
#include "entext.h"

LOGICAL m_lookent(M_WCHAR *name, unsigned char *type, M_WCHAR **content,
                  unsigned char *wheredef)
  {
    M_ENTITY *entity ;

    if (entity = (M_ENTITY *) m_lookfortrie(name, m_enttrie)) {
      *type = entity->type ;
      *content = entity->content ;
      *wheredef = entity->wheredef ; 
      return(TRUE) ;
      }
    else return(FALSE) ;
    }
