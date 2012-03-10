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
