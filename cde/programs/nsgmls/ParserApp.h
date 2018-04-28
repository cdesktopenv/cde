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
/* $XConsortium: ParserApp.h /main/1 1996/07/29 17:00:38 cde-hp $ */
// Copyright (c) 1996 James Clark
// See the file COPYING for copying permission.

#ifndef ParserApp_INCLUDED
#define ParserApp_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif

#include "SgmlParser.h"
#include "ParserOptions.h"
#include "EntityApp.h"
#include "StringC.h"
#include "ErrorCountEventHandler.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API ParserApp : public EntityApp {
public:
  ParserApp();
  void processOption(AppChar opt, const AppChar *arg);
  int processSysid(const StringC &);
  virtual ErrorCountEventHandler *makeEventHandler() = 0;
  Boolean enableWarning(const AppChar *s);
  void initParser(const StringC &sysid);
  SgmlParser &parser();
  // This calls the ArcEngine if the options have enabled that.
  void parseAll(SgmlParser &, EventHandler &,
		SP_CONST SP_VOLATILE sig_atomic_t *cancelPtr);
  virtual void allLinkTypesActivated();
protected:
  virtual int generateEvents(ErrorCountEventHandler *);
  ParserOptions options_;
  SgmlParser parser_;
  unsigned errorLimit_;
  Vector<StringC> arcNames_;
  Vector<const AppChar *> activeLinkTypes_;
};

inline
SgmlParser &ParserApp::parser()
{
  return parser_;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not ParserApp_INCLUDED */
