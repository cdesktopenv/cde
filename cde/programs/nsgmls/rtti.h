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
/* $XConsortium: rtti.h /main/1 1996/07/29 17:09:50 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef rtti_INCLUDED
#define rtti_INCLUDED 1

#ifdef SP_HAVE_RTTI

#define DYNAMIC_CAST_PTR(T, p) dynamic_cast<T *>(p)
#define DYNAMIC_CAST_CONST_PTR(T, p) dynamic_cast<const T *>(p)

#define RTTI_CLASS

#define RTTI_DEF0(T)
#define RTTI_DEF1(T, B1)
#define RTTI_DEF2(T, B1, B2)
#define RTTI_DEF3(T, B1, B2, B3)

#else /* not SP_HAVE_RTTI */

#include "TypeId.h"

#define RTTI_CLASS \
public: \
  virtual TypeId dynamicType() const; \
  static inline TypeId staticType() { return TypeId(RTTI_bases_); } \
protected: \
  static const void *RTTI_bases_[]; \
private:

#define RTTI_DEF0(T) \
  const void *T::RTTI_bases_[] = { 0 }; \
  TypeId T::dynamicType() const { return staticType(); }

#define RTTI_DEF1(T, B1) \
  const void *T::RTTI_bases_[] = { B1::RTTI_bases_, 0 }; \
  TypeId T::dynamicType() const { return staticType(); }

#define RTTI_DEF2(T, B1, B2) \
  const void *T::RTTI_bases_[] = { B1::RTTI_bases_, B2::RTTI_bases_, 0 }; \
  TypeId T::dynamicType() const { return staticType(); }

#define RTTI_DEF3(T, B1, B2, B3) \
  const void *T::RTTI_bases_[] = { \
    B1::RTTI_bases_, B2::RTTI_bases_, B3::RTTI_bases_, 0 }; \
  TypeId T::dynamicType() const { return staticType(); }

#define DYNAMIC_CAST_PTR(T, p) \
((p) && (p)->dynamicType().canCast(T::staticType(), (p)->staticType()) \
 ? (T *)(p) \
 : 0)
#define DYNAMIC_CAST_CONST_PTR(T, p) \
((p) && (p)->dynamicType().canCast(T::staticType(), (p)->staticType()) \
 ? (const T *)p \
 : 0)

#endif /* not SP_HAVE_RTTI */

#endif /* not rtti_INCLUDED */
