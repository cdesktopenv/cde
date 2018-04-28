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
/* $XConsortium: OffsetOrderedList.h /main/1 1996/07/29 16:59:10 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef OffsetOrderedList_INCLUDED
#define OffsetOrderedList_INCLUDED 1

#include "types.h"
#include "Owner.h"
#include "NCVector.h"
#include "Boolean.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

struct OffsetOrderedListBlock {
  Offset offset;		// next Offset
  size_t nextIndex;		// index of first item in next block
  enum { size = 200 };
  unsigned char bytes[size];
};

// This is an ordered list of Offsets with no duplicates.

class OffsetOrderedList {
public:
  OffsetOrderedList();
  // off must be > the last offset added.
  void append(Offset off);
  // Find the last offset in the list <= off.
  Boolean findPreceding(Offset off, size_t &foundIndex, Offset &foundOffset)
    const;
  size_t size() const;
private:
  OffsetOrderedList(const OffsetOrderedList &);	// undefined
  void operator=(const OffsetOrderedList &);	// undefined
  void addByte(unsigned char b);
  // bytes used in current block
  int blockUsed_;
  NCVector<Owner<OffsetOrderedListBlock> > blocks_;
};

inline
size_t OffsetOrderedList::size() const
{
  return blocks_.size() == 0 ? 0 : blocks_.back()->nextIndex;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not OffsetOrderedList_INCLUDED */
