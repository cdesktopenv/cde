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
/* $XConsortium: StorageManager.C /main/1 1996/07/29 17:04:57 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif
#include "splib.h"
#include "StorageManager.h"
#define DEFAULT_BLOCK_SIZE 1024

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

StorageObject::StorageObject()
{
}

StorageObject::~StorageObject()
{
}

void StorageObject::willNotRewind()
{
}

size_t StorageObject::getBlockSize() const
{
  return DEFAULT_BLOCK_SIZE;
}

StorageManager::StorageManager()
{
}

StorageManager::~StorageManager()
{
}

Boolean StorageManager::inheritable() const
{
  return 1;
}

Boolean StorageManager::resolveRelative(const StringC &, StringC &,
					Boolean) const
{
  return 1;
}

Boolean StorageManager::guessIsId(const StringC &, const CharsetInfo &) const
{
  return 0;
}

Boolean StorageManager::transformNeutral(StringC &, Boolean, Messenger &) const
{
  return 0;
}

const InputCodingSystem *StorageManager::requiredCodingSystem() const
{
  return 0;
}

Boolean StorageManager::requiresCr() const
{
  return 0;
}

const CharsetInfo *StorageManager::idCharset() const
{
  return 0;
}

const StringC *StorageManager::reString() const
{
  return 0;
}

IdStorageManager::IdStorageManager(const UnivCharsetDesc &idCharset)
: idCharset_(idCharset)
{
}

const CharsetInfo *IdStorageManager::idCharset() const
{
  return &idCharset_;
}

const StringC *IdStorageManager::reString() const
{
  return &reString_;
}


#ifdef SP_NAMESPACE
}
#endif
