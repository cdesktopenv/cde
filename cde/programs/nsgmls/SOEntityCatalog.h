/* $XConsortium: SOEntityCatalog.h /main/1 1996/07/29 17:03:35 cde-hp $ */
// Copyright (c) 1994, 1995 James Clark
// See the file COPYING for copying permission.

#ifndef SOEntityCatalog_INCLUDED
#define SOEntityCatalog_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif

#include "ExtendEntityManager.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API SOCatalogManager : public ExtendEntityManager::CatalogManager {
public:
  static ExtendEntityManager::CatalogManager *
    make(const Vector<StringC> &sysids,
	 size_t nSysidsMustExist,
	 const CharsetInfo &sysidCharset,
	 const CharsetInfo &catalogCharset);
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not SOEntityCatalog_INCLUDED */
