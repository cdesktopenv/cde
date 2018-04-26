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
/* $XConsortium: CopyOwner.C /main/1 1996/07/29 16:48:46 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef CopyOwner_DEF_INCLUDED
#define CopyOwner_DEF_INCLUDED 1

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

template<class T>
CopyOwner<T>::CopyOwner(const CopyOwner<T> &o)
: Owner<T>(o.pointer() ? o.pointer()->copy() : 0)
{
}
 
template<class T>
void CopyOwner<T>::operator=(const CopyOwner<T> &o)
{
  Owner<T>::operator=(o.pointer() ? o.pointer()->copy() : 0);
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not CopyOwner_DEF_INCLUDED */
