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
/* $XConsortium: ArcEngine.h /main/1 1996/07/29 16:46:15 cde-hp $ */
// Copyright (c) 1996 James Clark
// See the file COPYING for copying permission.

#ifndef ArcEngine_INCLUDED
#define ArcEngine_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Event.h"
#include "Vector.h"
#include "SgmlParser.h"
#include <stddef.h>

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API ArcDirector {
public:
  virtual EventHandler *arcEventHandler(const Notation *,
					const Vector<StringC> &,
					const SubstTable<Char> *) = 0;
};

class SP_API SelectOneArcDirector : public ArcDirector, public Messenger {
public:
  SelectOneArcDirector(const Vector<StringC> &select, EventHandler &eh)
    : select_(select), eh_(&eh) { }
  EventHandler *arcEventHandler(const Notation *,
				const Vector<StringC> &,
				const SubstTable<Char> *);
  void dispatchMessage(const Message &);
  void dispatchMessage(Message &);
private:
  Vector<StringC> select_;
  EventHandler *eh_;
};

class SP_API ArcEngine {
public:
  static void parseAll(SgmlParser &,
		       Messenger &,
		       ArcDirector &,
		       SP_CONST SP_VOLATILE sig_atomic_t *cancelPtr = 0);
private:
  ArcEngine();
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not ArcEngine_INCLUDED */
