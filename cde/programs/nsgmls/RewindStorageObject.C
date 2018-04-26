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
/* $XConsortium: RewindStorageObject.C /main/1 1996/07/29 17:02:59 cde-hp $ */
// Copyright (c) 1994, 1995 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif

#include "splib.h"
#include "RewindStorageObject.h"
#include "macros.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

RewindStorageObject::RewindStorageObject(Boolean mayRewind, Boolean canSeek)
: mayRewind_(mayRewind), canSeek_(canSeek),
  savingBytes_(mayRewind && canSeek), readingSaved_(0),
  nBytesRead_(0)
{
}

Boolean RewindStorageObject::rewind(Messenger &mgr)
{
  ASSERT(mayRewind_);
  if (canSeek_)
    return seekToStart(mgr);
  else {
    readingSaved_ = 1;
    nBytesRead_ = 0;
    return 1;
  }
}

void RewindStorageObject::unread(const char *s, size_t n)
{
  savedBytes_.append(s, n);
  if (!readingSaved_) {
    readingSaved_ = 1;
    nBytesRead_ = 0;
  }
}

void RewindStorageObject::willNotRewind()
{
  mayRewind_ = 0;
  savingBytes_ = 0;
  if (!readingSaved_) {
    // Ensure that memory is released now.
    String<char> tem;
    tem.swap(savedBytes_);
  }
}

Boolean RewindStorageObject::readSaved(char *buf, size_t bufSize,
				       size_t &nread)
{
  if (!readingSaved_)
    return 0;
  if (nBytesRead_ >= savedBytes_.size()) {
    if (!mayRewind_) {
      // Ensure that memory is released now.
      String<char> tem;
      tem.swap(savedBytes_);
    }
    readingSaved_ = 0;
    return 0;
  }
  nread = savedBytes_.size() - nBytesRead_;
  if (nread > bufSize)
    nread = bufSize;
  memcpy(buf, savedBytes_.data() + nBytesRead_, nread);
  nBytesRead_ += nread;
  return 1;
}

#ifdef SP_NAMESPACE
}
#endif
