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
/* $XConsortium: PosixStorage.h /main/1 1996/07/29 17:01:53 cde-hp $ */
// Copyright (c) 1994, 1995 James Clark
// See the file COPYING for copying permission.

#ifndef PosixStorage_INCLUDED
#define PosixStorage_INCLUDED 1
#ifdef __GNUG__
#pragma interface
#endif

#include "StorageManager.h"
#include "DescriptorManager.h"
#include "Vector.h"
#include "StringC.h"
#include "CharsetInfo.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class Messenger;
class CharsetInfo;
class UnivCharsetDesc;
class Filename;
class OutputCodingSystem;

class SP_API PosixStorageManager : public IdStorageManager {
public:
  PosixStorageManager(const char *type,
		      const UnivCharsetDesc &filenameCharset,
#ifndef SP_WIDE_SYSTEM
		      const OutputCodingSystem *filenameCodingSystem,
#endif
		      int maxFDs);
  StorageObject *makeStorageObject(const StringC &id,
				   const StringC &baseId,
				   Boolean search,
				   Boolean mayRewind,
				   Messenger &,
				   StringC &foundId);
  Boolean resolveRelative(const StringC &, StringC &, Boolean syntactic = 0)
    const;
  const char *type() const;
  void addSearchDir(const StringC &);
  Boolean transformNeutral(StringC &, Boolean fold, Messenger &) const;
private:
  Boolean isAbsolute(const StringC &) const;
  StringC extractDir(const StringC &) const;
  StringC combineDir(const StringC &, const StringC &) const;
  PosixStorageManager(const PosixStorageManager &); // undefined
  void operator=(const PosixStorageManager &);	    // undefined
  DescriptorManager descriptorManager_;
#ifndef SP_WIDE_SYSTEM
  const OutputCodingSystem *filenameCodingSystem_;
#endif
  const char *type_;
  Vector<StringC> searchDirs_;
};

class SP_API PosixFdStorageManager : public IdStorageManager {
public:
  PosixFdStorageManager(const char *type,
			const UnivCharsetDesc &filenameCharset);
  StorageObject *makeStorageObject(const StringC &id,
				   const StringC &baseId,
				   Boolean,
				   Boolean mayRewind,
				   Messenger &mgr,
				   StringC &foundId);
  const char *type() const;
  Boolean inheritable() const;
private:
  PosixFdStorageManager(const PosixFdStorageManager &); // undefined
  void operator=(const PosixFdStorageManager &);	    // undefined
  const char *type_;

};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not PosixStorage_INCLUDED */
