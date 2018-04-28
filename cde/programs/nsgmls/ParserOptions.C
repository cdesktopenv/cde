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
/* $XConsortium: ParserOptions.C /main/1 1996/07/29 17:01:10 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif
#include "splib.h"
#include "ParserOptions.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

ParserOptions::ParserOptions()
: datatag(0),
  omittag(1),
  rank(1),
  shorttag(1),
  linkSimple(1000),
  linkImplicit(1),
  linkExplicit(1),
  concur(0),
  subdoc(99999999),
  formal(1),
  shortref(1),
  errorIdref(1),
  errorSignificant(1),
  errorAfdr(1),
  errorLpdNotation(0),
  warnSgmlDecl(0),
  warnShould(0),
  warnDuplicateEntity(0),
  warnUndefinedElement(0),
  warnDefaultEntityReference(0),
  warnMixedContent(0),
  warnUnclosedTag(0),
  warnNet(0),
  warnEmptyTag(0),
  warnUnusedMap(0),
  warnUnusedParam(0),
  warnNotationSystemId(0)
{
  for (int i = 0; i < nQuantity; i++)
    quantity[i] = 99999999;
  quantity[BSEQLEN] = 960;
  quantity[NORMSEP] = 2;
  quantity[LITLEN] = 24000;
  quantity[PILEN] = 24000;
  quantity[DTEMPLEN] = 24000;
}

#ifdef SP_NAMESPACE
}
#endif
