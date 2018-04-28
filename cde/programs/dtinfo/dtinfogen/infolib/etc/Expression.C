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
/* $XConsortium: Expression.C /main/3 1996/08/21 15:46:40 drk $ */
/* imported interfaces */

#include "Expression.h"
#include "ExprList.h"

OL_Expression::~OL_Expression()
{
  /* This is a hack right now to eliminate the memory leak
   */
  if ( data_type == CONCAT || data_type == FIRSTOF ) {
    ExprList *vlist = ( ExprList * )value_list;
    delete vlist;
  }
  else if ( data_type == LITERAL ) {
    char *vlist = (char *)value_list;
    delete vlist;
  }

}
