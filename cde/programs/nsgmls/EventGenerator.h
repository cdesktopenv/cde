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
/* $XConsortium: EventGenerator.h /main/1 1996/07/29 16:51:29 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef EventGenerator_INCLUDED
#define EventGenerator_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif

#include "SGMLApplication.h"

class SP_API EventGenerator {
public:
  virtual ~EventGenerator();
  // Can be called at most once for any object.
  // Returns number of errors.
  virtual unsigned run(SGMLApplication &) = 0;
  // may be called at any time
  virtual void inhibitMessages(bool);
  // may be called at any time, even from another thread
  virtual void halt() = 0;
  // called after run
  virtual EventGenerator *
    makeSubdocEventGenerator(const SGMLApplication::Char *systemId,
			     size_t systemIdLength);
};

#endif /* not EventGenerator_INCLUDED */
