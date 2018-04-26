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
/* $XConsortium: DescriptorManager.C /main/1 1996/07/29 16:48:58 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#include "splib.h"
#include "DescriptorManager.h"
#include "ListIter.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

DescriptorUser::DescriptorUser(DescriptorManager *manager)
: manager_(manager)
{
  if (manager_)
    manager_->addUser(this);
}

DescriptorUser::~DescriptorUser()
{
  if (manager_)
    manager_->removeUser(this);
}

void DescriptorUser::managerDeleted()
{
  manager_ = 0;
}

Boolean DescriptorUser::suspend()
{
  return 0;
}

void DescriptorUser::acquireD()
{
  if (manager_)
    manager_->acquireD();
}

void DescriptorUser::releaseD()
{
  if (manager_)
    manager_->releaseD();
}

DescriptorManager::DescriptorManager(int maxD)
: maxD_(maxD), usedD_(0)
{
}

DescriptorManager::~DescriptorManager()
{
  for (ListIter<DescriptorUser *> iter(users_);
       !iter.done();
       iter.next())
    iter.cur()->managerDeleted();
}

void DescriptorManager::addUser(DescriptorUser *p)
{
  users_.insert(p);
}

void DescriptorManager::removeUser(DescriptorUser *p)
{
  users_.remove(p);
}

void DescriptorManager::acquireD()
{
  if (usedD_ >= maxD_) {
    for (ListIter<DescriptorUser *> iter(users_);
	 !iter.done();
	 iter.next()) {
      if (iter.cur()->suspend())
	break;
    }
  }
  usedD_++;
}

void DescriptorManager::releaseD()
{
  usedD_--;
}

#ifdef SP_NAMESPACE
}
#endif
