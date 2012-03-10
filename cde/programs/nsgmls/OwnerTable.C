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
/* $XConsortium: OwnerTable.C /main/1 1996/07/29 17:00:02 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef OwnerTable_DEF_INCLUDED
#define OwnerTable_DEF_INCLUDED 1

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

template<class T, class K, class HF, class KF>
OwnerTable<T, K, HF, KF>::~OwnerTable()
{
  for (size_t i = 0; i < vec_.size(); i++)
    delete vec_[i];
}

template<class T, class K, class HF, class KF>
void OwnerTable<T, K, HF, KF>::clear()
{
  for (size_t i = 0; i < vec_.size(); i++)
    delete vec_[i];
  PointerTable<T *, K, HF, KF>::clear();
}

template<class T, class K, class HF, class KF>
void
CopyOwnerTable<T, K, HF, KF>::operator=(const CopyOwnerTable<T, K, HF, KF> &t)
{
  clear();
  //PointerTable<T *, K, HF, KF>::operator=(t);
  // FIXME This isn't exception safe.
  for (size_t i = 0; i < vec_.size(); i++)
    if (vec_[i])
      vec_[i] = vec_[i]->copy();
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not OwnerTable_DEF_INCLUDED */
