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
/* $XConsortium: InternalInputSource.C /main/1 1996/07/29 16:55:23 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif
#include "splib.h"
#include <string.h>
#include "InternalInputSource.h"
#include "macros.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

InternalInputSource::InternalInputSource(const StringC &str,
					 InputSourceOrigin *origin)
: InputSource(origin, str.data(), str.data() + str.size()), buf_(0),
  contents_(&str)
{
}

InternalInputSource::~InternalInputSource()
{
  if (buf_)
    delete [] buf_;
}
  
Xchar InternalInputSource::fill(Messenger &)
{
  return eE;
}

void InternalInputSource::pushCharRef(Char c, const NamedCharRef &ref)
{
  ASSERT(cur() == start());
  noteCharRef(startIndex() + (cur() - start()), ref);
  if (buf_ == 0) {
    buf_ = new Char[end() - start() + 1];
    memcpy(buf_ + 1, cur(), (end() - start())*sizeof(Char));
    changeBuffer(buf_ + 1, cur());
  }
  moveLeft();
  *(Char *)cur() = c;
}

Boolean InternalInputSource::rewind(Messenger &)
{
  reset(contents_->data(),
	contents_->data() + contents_->size());
  if (buf_) {
    delete [] buf_;
    buf_ = 0;
  }
  return 1;
}

#ifdef SP_NAMESPACE
}
#endif
