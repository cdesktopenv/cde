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
/* $XConsortium: IListBase.C /main/1 1996/07/29 16:53:20 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#include "splib.h"
#include "IListBase.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

void IListBase::append(Link *p)
{
  Link **pp;
  for (pp = &head_; *pp; pp = &(*pp)->next_)
    ;
  *pp = p;
}

void IListBase::remove(Link *p)
{
  for (Link **pp = &head_; *pp; pp = &(*pp)->next_)
    if (*pp == p) {
      *pp = p->next_;
      break;
    }
}

void IListBase::clear()
{
  while (!empty())
    delete get();
}

#ifdef SP_NAMESPACE
}
#endif
