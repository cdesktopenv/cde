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
/* $XConsortium: LpdEntityRef.h /main/1 1996/07/29 16:56:34 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef LpdEntityRef_INCLUDED
#define LpdEntityRef_INCLUDED 1

#include "Entity.h"
#include "Boolean.h"
#include "Ptr.h"

// Information about a reference to an entity that
// used a definition in an LPD.

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

struct LpdEntityRef {
  ConstPtr<Entity> entity;
  PackedBoolean lookedAtDefault;
  PackedBoolean foundInPass1Dtd;
  static inline const LpdEntityRef &key(const LpdEntityRef &r) { return r; }
  static inline unsigned long hash(const LpdEntityRef &r) {
    return Hash::hash(r.entity->name());
  }
};

inline
Boolean operator==(const LpdEntityRef &r1, const LpdEntityRef &r2)
{
  return (r1.entity == r2.entity
	  && r1.foundInPass1Dtd == r2.foundInPass1Dtd
	  && r1.lookedAtDefault == r2.lookedAtDefault);
}

inline
Boolean operator!=(const LpdEntityRef &r1, const LpdEntityRef &r2)
{
  return !(r1 == r2);
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not LpdEntityRef_INCLUDED */
