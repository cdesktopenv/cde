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
/* $XConsortium: EntityApp.C /main/1 1996/07/29 16:49:57 cde-hp $ */
// Copyright (c) 1996 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif

#include "splib.h"
#include "EntityApp.h"
#include "sptchar.h"

#include <stdlib.h>

#ifdef SP_MULTI_BYTE
#include "ISO8859InputCodingSystem.h"
#endif

#include "PosixStorage.h"
#include "URLStorage.h"
#include "LiteralStorage.h"
#include "ExtendEntityManager.h"
#include "SOEntityCatalog.h"
#include "CodingSystem.h"
#include "macros.h"

#ifndef SGML_SEARCH_PATH_DEFAULT
#define SGML_SEARCH_PATH_DEFAULT SP_T("")
#endif

#ifndef SGML_CATALOG_FILES_DEFAULT
#define SGML_CATALOG_FILES_DEFAULT SP_T("")
#endif /* not SGML_CATALOG_FILES_DEFAULT */

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

#ifdef MSDOS_FILENAMES
const Char FILE_SEP = ';';
#else
const Char FILE_SEP = ':';
#endif

#ifdef SP_MULTI_BYTE
static ISO8859InputCodingSystem iso8859_2InputCodingSystem(2);
static ISO8859InputCodingSystem iso8859_3InputCodingSystem(3);
static ISO8859InputCodingSystem iso8859_4InputCodingSystem(4);
static ISO8859InputCodingSystem iso8859_5InputCodingSystem(5);
static ISO8859InputCodingSystem iso8859_6InputCodingSystem(6);
static ISO8859InputCodingSystem iso8859_7InputCodingSystem(7);
static ISO8859InputCodingSystem iso8859_8InputCodingSystem(8);
static ISO8859InputCodingSystem iso8859_9InputCodingSystem(9);

static struct {
  const char *name;
  const InputCodingSystem *cs;
} inputCodingSystems[] = {
 { "IS8859-2", &iso8859_2InputCodingSystem },
 { "IS8859-3", &iso8859_3InputCodingSystem },
 { "IS8859-4", &iso8859_4InputCodingSystem },
 { "IS8859-5", &iso8859_5InputCodingSystem },
 { "IS8859-6", &iso8859_6InputCodingSystem },
 { "IS8859-7", &iso8859_7InputCodingSystem },
 { "IS8859-8", &iso8859_8InputCodingSystem },
 { "IS8859-9", &iso8859_9InputCodingSystem },
};

#endif /* SP_MULTI_BYTE */

#ifdef SP_MULTI_BYTE
static UnivCharsetDesc::Range range = { 0, 65536, 0 };
#else
static UnivCharsetDesc::Range range = { 0, 256, 0 };
#endif

EntityApp::EntityApp()
: mapCatalogDocument_(0),
  systemCharset_(UnivCharsetDesc(&range, 1))
{
  registerOption('c', SP_T("catalog_sysid"));
  registerOption('C');
  registerOption('D', SP_T("dir"));
}

void EntityApp::processOption(AppChar opt, const AppChar *arg)
{
  switch (opt) {
  case 'c':
    catalogSysids_.push_back(arg);
    break;
  case 'C':
    mapCatalogDocument_ = 1;
    break;
  case 'D':
    searchDirs_.push_back(arg);
    break;
  default:
    CmdLineApp::processOption(opt, arg);
    break;
  }
}

int EntityApp::processArguments(int argc, AppChar **argv)
{
  StringC sysid;
  if (!makeSystemId(argc, argv, sysid))
    return 1;
  return processSysid(sysid);
}

Boolean EntityApp::makeSystemId(int nFiles, AppChar *const *files,
					StringC &result)
{
  Vector<StringC> filenames(nFiles == 0 ? 1 : nFiles);
  int i;
  for (i = 0; i < nFiles; i++)
    filenames[i] = convertInput(tcscmp(files[i], SP_T("-")) == 0
				? SP_T("<OSFD>0")
				: files[i]);
  if (nFiles == 0)
    filenames[0] = convertInput(SP_T("<OSFD>0"));
  return entityManager()->mergeSystemIds(filenames,
					 mapCatalogDocument_,
					 systemCharset_,
					 *this,
					 result);
}


Ptr<ExtendEntityManager> &EntityApp::entityManager()
{
  if (!entityManager_.isNull())
    return entityManager_;
  PosixStorageManager *sm
    = new PosixStorageManager("OSFILE",
			      systemCharset_.desc(),
#ifndef SP_WIDE_SYSTEM
			      codingSystem(),
#endif
			      5);
  size_t i;
  for (i = 0; i < searchDirs_.size(); i++)
    sm->addSearchDir(convertInput(searchDirs_[i]));
  {
    const AppChar *e = tgetenv(SP_T("SGML_SEARCH_PATH"));
    if (!e)
      e = SGML_SEARCH_PATH_DEFAULT;
    if (*e) {
      StringC str(convertInput(e));
      size_t i = 0;
      size_t start = 0;
      for (;;) {
	if (i == str.size() || str[i] == FILE_SEP) {
	  sm->addSearchDir(StringC(str.data() + start,
				   i - start));
	  if (i == str.size())
	    break;
	  start = ++i;
	}
	else
	  i++;
      }
    }
  }

  entityManager_ = ExtendEntityManager::make(sm, codingSystem());
  entityManager_
  ->registerStorageManager(new PosixFdStorageManager("OSFD",
						     systemCharset_.desc()));
  entityManager_->registerStorageManager(new URLStorageManager("URL"));
  entityManager_->registerStorageManager(new LiteralStorageManager("LITERAL"));
  for (i = 0;; i++) {
    const char *s;
    const CodingSystem *p = codingSystem(i, s);
    if (!p)
      break;
    entityManager_->registerCodingSystem(s, p);
  }
#ifdef SP_MULTI_BYTE
  for (i = 0; i < SIZEOF(inputCodingSystems); i++)
    entityManager_->registerCodingSystem(inputCodingSystems[i].name,
					 inputCodingSystems[i].cs);
#endif
  Vector<StringC> v;
  for (i = 0; i < catalogSysids_.size(); i++)
    // filenames specified on command-line must exist
    v.push_back(convertInput(catalogSysids_[i]));
  {
    const AppChar *e = tgetenv(SP_T("SGML_CATALOG_FILES"));
    if (!e)
      e = SGML_CATALOG_FILES_DEFAULT;
    if (*e) {
      StringC str(convertInput(e));
      size_t i = 0;
      size_t start = 0;
      for (;;) {
	if (i == str.size() || str[i] == FILE_SEP) {
	  v.push_back(StringC(str.data() + start,
			      i - start));
	  if (i == str.size())
	    break;
	  start = ++i;
	}
	else
	  i++;
      }
    }
  }
  entityManager_->setCatalogManager(SOCatalogManager::make(v,
							   catalogSysids_.size(),
							   systemCharset_,
							   systemCharset_));
  return entityManager_;
}


#ifdef SP_NAMESPACE
}
#endif
