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
/* $XConsortium: ParserOptions.h /main/1 1996/07/29 17:01:15 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef ParserOptions_INCLUDED
#define ParserOptions_INCLUDED 1
#ifdef __GNUG__
#pragma interface
#endif

#include "types.h"
#include "EventsWanted.h"
#include "Vector.h"
#include "StringC.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

struct SP_API ParserOptions {
  ParserOptions();
  EventsWanted eventsWanted;
  PackedBoolean datatag;
  PackedBoolean omittag;
  PackedBoolean rank;
  PackedBoolean shorttag;
  Number linkSimple;
  PackedBoolean linkImplicit;
  Number linkExplicit;
  Number concur;
  Number subdoc;
  PackedBoolean formal;
  PackedBoolean shortref;
  enum Quantity {
    ATTCNT,
    ATTSPLEN,
    BSEQLEN,
    DTAGLEN,
    DTEMPLEN,
    ENTLVL,
    GRPCNT,
    GRPGTCNT,
    GRPLVL,
    LITLEN,
    NAMELEN,
    NORMSEP,
    PILEN,
    TAGLEN,
    TAGLVL
  };
  enum { nQuantity = TAGLVL + 1 };
  Number quantity[nQuantity];
  PackedBoolean errorIdref;
  PackedBoolean errorSignificant;
  PackedBoolean errorAfdr;	// error if AFDR extensions are used
  PackedBoolean errorLpdNotation;
  PackedBoolean warnSgmlDecl;
  PackedBoolean warnDuplicateEntity;
  PackedBoolean warnShould;
  PackedBoolean warnUndefinedElement;
  PackedBoolean warnDefaultEntityReference;
  PackedBoolean warnMixedContent;
  PackedBoolean warnUnclosedTag;
  PackedBoolean warnNet;
  PackedBoolean warnEmptyTag;
  PackedBoolean warnUnusedMap;
  PackedBoolean warnUnusedParam;
  PackedBoolean warnNotationSystemId;
  Vector<StringC> includes;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not ParserOptions_INCLUDED */
