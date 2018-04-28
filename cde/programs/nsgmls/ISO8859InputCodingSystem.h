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
/* $XConsortium: ISO8859InputCodingSystem.h /main/1 1996/07/29 16:54:04 cde-hp $ */
// Copyright (c) 1995 James Clark
// See the file COPYING for copying permission.

#ifndef ISO8859InputCodingSystem_INCLUDED
#define ISO8859InputCodingSystem_INCLUDED 1

#include "TranslateInputCodingSystem.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API ISO8859InputCodingSystem : public TranslateInputCodingSystem {
public:
  // part must be between 2 and 9
  ISO8859InputCodingSystem(int part);
private:
  const Char *partMap(int);
  static const Char maps[8][256];
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not ISO8859InputCodingSystem_INCLUDED */
