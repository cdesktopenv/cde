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
/* $XConsortium: DescriptorManager.h /main/1 1996/07/29 16:49:04 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef DescriptorManager_INCLUDED
#define DescriptorManager_INCLUDED 1

#include "Boolean.h"
#include "List.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class DescriptorManager;

class SP_API DescriptorUser {
public:
  DescriptorUser(DescriptorManager *);
  virtual ~DescriptorUser();
  virtual Boolean suspend();
  void managerDeleted();
  void acquireD();
  void releaseD();
  DescriptorManager *manager() const;
private:
  DescriptorManager *manager_;
};

class SP_API DescriptorManager {
public:
  DescriptorManager(int maxD);
  ~DescriptorManager();
  void acquireD();
  void releaseD();
  void addUser(DescriptorUser *);
  void removeUser(DescriptorUser *);
private:
  DescriptorManager(const DescriptorManager &);	// undefined
  void operator=(const DescriptorManager &);	// undefined

  int usedD_;
  int maxD_;
  List<DescriptorUser *> users_;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not DescriptorManager_INCLUDED */
