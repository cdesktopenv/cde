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
/* $XConsortium: TypeId.h /main/1 1996/07/29 17:06:57 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef TypeId_INCLUDED
#define TypeId_INCLUDED 1

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

#ifndef SP_API
#define SP_API /* as nothing */
#endif

class SP_API TypeId {
public:
  TypeId(const void *const *bases) : bases_(bases) { }
  // Is this object of type ti?
  int isA(TypeId ti) const;
  // Can an object with this dynamic type be cast from a static type FROM
  // to a static type TO?
  int canCast(TypeId to, TypeId from) const;
  int operator==(TypeId ti) const { return bases_ == ti.bases_; }
  int operator!=(TypeId ti) const { return bases_ != ti.bases_; }
private:
  const void *const *bases_;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not TypeId_INCLUDED */
