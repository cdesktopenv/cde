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
/* $XConsortium: InternalInputSource.h /main/1 1996/07/29 16:55:29 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef InternalInputSource_INCLUDED
#define InternalInputSource_INCLUDED 1
#ifdef __GNUG__
#pragma interface
#endif

#include <stddef.h>
#include "InputSource.h"
#include "Allocator.h"
#include "StringC.h"
#include "types.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class InputSourceOrigin;
class Messenger;
class NamedCharRef;

class InternalInputSource : public InputSource {
public:
  void *operator new(size_t sz, Allocator &alloc) { return alloc.alloc(sz); }
  void *operator new(size_t sz) { return Allocator::allocSimple(sz); }
  void operator delete(void *p) { Allocator::free(p); }
  InternalInputSource(const StringC &, InputSourceOrigin *);
  Xchar fill(Messenger &);
  void pushCharRef(Char ch, const NamedCharRef &);
  Boolean rewind(Messenger &);
  ~InternalInputSource();
private:
  InternalInputSource(const InternalInputSource &); // undefined
  void operator=(const InternalInputSource &);	    // undefined
  Char *buf_;
  const StringC *contents_;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not InternalInputSource_INCLUDED */
