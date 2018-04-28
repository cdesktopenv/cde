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
/* $XConsortium: RewindStorageObject.h /main/1 1996/07/29 17:03:03 cde-hp $ */
#ifndef RewindStorageObject_INCLUDED
#define RewindStorageObject_INCLUDED 1

#include "StorageManager.h"
#include "Boolean.h"
#include "StringOf.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class Messenger;

class SP_API RewindStorageObject : public StorageObject {
public:
  RewindStorageObject(Boolean mayRewind, Boolean canSeek);
protected:
  PackedBoolean mayRewind_;

  void saveBytes(const char *, size_t);
  Boolean readSaved(char *, size_t, size_t &);
  Boolean rewind(Messenger &);
  void willNotRewind();
  void unread(const char *s, size_t n);
  virtual Boolean seekToStart(Messenger &) = 0;
private:
  PackedBoolean savingBytes_;
  PackedBoolean readingSaved_;
  PackedBoolean canSeek_;
  String<char> savedBytes_;
  size_t nBytesRead_;
};

inline
void RewindStorageObject::saveBytes(const char *s, size_t n)
{
  if (savingBytes_)
    savedBytes_.append(s, n);
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not RewindStorageObject_INCLUDED */
