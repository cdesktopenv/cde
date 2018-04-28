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
/* $XConsortium: Allocator.h /main/1 1996/07/29 16:46:04 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef Allocator_INCLUDED
#define Allocator_INCLUDED 1

#include <stddef.h>

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API Allocator {
public:
  Allocator(size_t maxSize, unsigned blocksPerSegment);
  ~Allocator();
  void *alloc(size_t);
  static void *allocSimple(size_t);
  static void free(void *);

  // It would be nice to make these private, but some compilers have problems.
  union ForceAlign {
    unsigned long n;
    struct SP_API {
      char c;
    } s;
    char *cp;
    long *lp;
  };
  struct SegmentHeader;
  union BlockHeader;
  friend union BlockHeader;
  union BlockHeader {
    SegmentHeader *seg;
    ForceAlign align;
  };
  struct Block;
  friend struct Block;
  struct SP_API Block {
    BlockHeader header;
    Block *next;
  };
  friend struct SegmentHeader;
  struct SP_API SegmentHeader {
    union {
      Block **freeList;
      ForceAlign align;
    };
    unsigned liveCount;
    SegmentHeader *next;
  };
private:
  Allocator(const Allocator &);	// undefined
  Allocator &operator=(const Allocator &); // undefined
  Block *freeList_;
  size_t objectSize_;
  unsigned blocksPerSegment_;
  SegmentHeader *segments_;
  void *alloc1();
  void tooBig(size_t);
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not Allocator_INCLUDED */
