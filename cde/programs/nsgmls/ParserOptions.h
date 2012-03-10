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
