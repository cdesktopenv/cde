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
/* $XConsortium: ParserEventGeneratorKit.h /main/1 1996/07/29 17:00:54 cde-hp $ */
// Copyright (c) 1995 James Clark
// See the file COPYING for copying permission.

#ifndef ParserEventGeneratorKit_INCLUDED
#define ParserEventGeneratorKit_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif

#include "EventGenerator.h"

class ParserEventGeneratorKitImpl;

class SP_API ParserEventGeneratorKit {
public:
  ParserEventGeneratorKit();
  ~ParserEventGeneratorKit();
  enum Option {
    showOpenEntities,
    showOpenElements,
    outputCommentDecls,
    outputMarkedSections,
    outputGeneralEntities,
    mapCatalogDocument
  };
  enum OptionWithArg {
    addCatalog,
    includeParam,
    enableWarning,
    addSearchDir,
    activateLink,
    architecture		// not implemented
    };
  void setOption(Option);
#ifdef SP_WIDE_SYSTEM
  void setProgramName(const wchar_t *);
  void setOption(OptionWithArg, const wchar_t *);
  EventGenerator *makeEventGenerator(int nFiles, wchar_t *const *files);
#else
  void setProgramName(const char *);
  void setOption(OptionWithArg, const char *);
  EventGenerator *makeEventGenerator(int nFiles, char *const *files);
#endif
private:
  ParserEventGeneratorKit(const ParserEventGeneratorKit &); // undefined
  void operator=(const ParserEventGeneratorKit &);   // undefined

  ParserEventGeneratorKitImpl *impl_;
};

#endif /* not ParserEventGeneratorKit_INCLUDED */
