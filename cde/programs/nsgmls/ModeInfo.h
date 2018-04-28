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
/* $XConsortium: ModeInfo.h /main/1 1996/07/29 16:58:04 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef ModeInfo_INCLUDED
#define ModeInfo_INCLUDED 1
#ifdef __GNUG__
#pragma interface
#endif

#include <stdlib.h>
#include "Boolean.h"
#include "Syntax.h"
#include "Mode.h"
#include "Priority.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

struct TokenInfo {
  enum Type {
    delimType,
    setType,
    functionType,
    delimDelimType,
    delimSetType
    };
  Type type;
  Priority::Type priority;
  Token token;
  Syntax::DelimGeneral delim1;
  union {
    Syntax::DelimGeneral delim2;
    Syntax::Set set;
    Syntax::StandardFunction function;
  };
};

class Sd;
struct PackedTokenInfo;

class ModeInfo {
public:
  ModeInfo(Mode mode, const Sd &sd);
  Boolean nextToken(TokenInfo *);
  Boolean includesShortref() const;
private:
  Mode mode_;
  const PackedTokenInfo *p_;		// points to next
  size_t count_;
  unsigned missingRequirements_;
};

inline Boolean ModeInfo::includesShortref() const
{
  return mode_ >= minShortrefMode;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not ModeInfo_INCLUDED */
