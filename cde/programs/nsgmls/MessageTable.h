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
/* $XConsortium: MessageTable.h /main/1 1996/07/29 16:57:49 cde-hp $ */
// Copyright (c) 1996 James Clark
// See the file COPYING for copying permission.

#ifndef MessageTable_INCLUDED
#define MessageTable_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Message.h"
#include "Boolean.h"
#include "StringC.h"
#include "sptchar.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API MessageTable {
public:
  static const MessageTable *instance();
  virtual Boolean getText(const MessageFragment &,
			  String<SP_TCHAR> &) const = 0;
private:
  static MessageTable *instance_;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not MessageTable_INCLUDED */
