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
/* $XConsortium: OLAF.C /main/4 1996/09/24 16:55:40 cde-hal $
 *
 * OLAF -- OLIAS Architectural Forms
 */

#include <assert.h>

#include "OLAF.h"
#include "SGMLName.h"

static int init = OLAF::init();


const int NAMECASE = 1; /* Only one SGML Decl. supported */

int
OLAF::init()
{
  static int done = 0;
  if(done) return 0;
  done = 1;

#define INTERN(n) SGMLName::intern(#n, NAMECASE);

  SGMLName::init();
  
  /* The order of calls to SGMLName::intern _MUST_ match the
   * enumeration in OLAF.h
   */
  INTERN(OLIAS);
  INTERN(OLIAS.Value);
  INTERN(OLIAS.ID);
  INTERN(OLIAS.IDREF);
  INTERN(OLIAS.Scope);
  INTERN(OLIAS.Style);
  INTERN(OLIAS.Choice);

  INTERN(OLIAS.TOC);
  INTERN(OLIAS.TOCEntry);
  INTERN(OLIAS.TOClevel);
  INTERN(OLIAS.Section);
  INTERN(OLIAS.Title);
  INTERN(OLIAS.ShortTitle);
  INTERN(OLIAS.Ignore);
  INTERN(OLIAS.Graphic);
  INTERN(OLIAS.Table);
  INTERN(OLIAS.XRefLabel);
  INTERN(OLIAS.XRef);
  
  INTERN(Example);
  INTERN(Graphic);
  INTERN(Index);
  INTERN(Table);
  INTERN(Title);

  INTERN(BookcaseDesc);
  INTERN(BookcaseName);
  INTERN(BookShortTitle);
  INTERN(BookTitle);
  INTERN(Book);
  INTERN(Bookcase);

  INTERN(Style);
  INTERN(Stylesheet);
  INTERN(BookTab);
  
  INTERN(Feature);
  INTERN(Online);
  INTERN(Print);
  INTERN(Path);
  INTERN(Select);
  INTERN(FeatureSet);
  INTERN(FeatureText);
  INTERN(AutoNumber);
  INTERN(AutoRef);
  INTERN(BookAccess);

  assert(SGMLName::intern("Feature", NAMECASE) == OLAF::Feature);

  SGMLName::intern("lnfeed");
  SGMLName::intern("nbsp");
  SGMLName::intern("amp");
  SGMLName::intern("lt");

  assert(SGMLName::intern("lnfeed") == OLAF::lnfeed);

  INTERN(VenCode);
  INTERN(Version);
  INTERN(Grouping);
  INTERN(DemoTerms);
  INTERN(DefaultSection);

  assert( SGMLName::intern("Grouping", NAMECASE) == OLAF::Grouping );

  return 0;
}
