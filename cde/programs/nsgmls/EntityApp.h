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
/* $XConsortium: EntityApp.h /main/1 1996/07/29 16:50:06 cde-hp $ */
// Copyright (c) 1996 James Clark
// See the file COPYING for copying permission.

#ifndef EntityApp_INCLUDED
#define EntityApp_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif

#include "CmdLineApp.h"
#include "CharsetInfo.h"
#include "Boolean.h"
#include "ExtendEntityManager.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API EntityApp : public CmdLineApp {
public:
  EntityApp();
  void processOption(AppChar opt, const AppChar *arg);
  virtual int processSysid(const StringC &) = 0;
  int processArguments(int argc, AppChar **files);
  Boolean makeSystemId(int nFiles, AppChar *const *files, StringC &result);
  Ptr<ExtendEntityManager> &entityManager();
protected:
  void clearEntityManager();
  CharsetInfo systemCharset_;
private:
  Vector<const AppChar *> searchDirs_;
  Vector<const AppChar *> catalogSysids_;
  Boolean mapCatalogDocument_;
  Ptr<ExtendEntityManager> entityManager_;
};

inline
void EntityApp::clearEntityManager()
{
  entityManager_.clear();
}
    
#ifdef SP_NAMESPACE
}
#endif

#endif /* not EntityApp_INCLUDED */
