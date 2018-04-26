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
/* $XConsortium: StdioStorage.h /main/1 1996/07/29 17:04:47 cde-hp $ */
// Copyright (c) 1994, 1995 James Clark
// See the file COPYING for copying permission.

#ifndef StdioStorage_INCLUDED
#define StdioStorage_INCLUDED 1
#ifdef __GNUG__
#pragma interface
#endif

#include "StorageManager.h"
#include "DescriptorManager.h"
#include "StringC.h"
#include "CharsetInfo.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class Messenger;
class CharsetInfo;
class UnivCharsetDesc;
class OutputCodingSystem;

class SP_API StdioStorageManager : public IdStorageManager {
public:
  StdioStorageManager(const char *type,
		      const UnivCharsetDesc &filenameCharset,
		      const OutputCodingSystem *filenameCodingSystem);
  StorageObject *makeStorageObject(const StringC &id,
				   const StringC &baseId,
				   Boolean,
				   Boolean mayRewind,
				   Messenger &,
				   StringC &foundId);
  const char *type() const;
private:
  StdioStorageManager(const StdioStorageManager &); // undefined
  void operator=(const StdioStorageManager &);	    // undefined
  const OutputCodingSystem *filenameCodingSystem_;
  const char *type_;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not StdioStorage_INCLUDED */
