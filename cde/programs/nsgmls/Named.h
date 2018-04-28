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
/* $XConsortium: Named.h /main/1 1996/07/29 16:58:23 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef Named_INCLUDED
#define Named_INCLUDED 1

#include "StringC.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

#ifndef SP_API
#define SP_API /* as nothing */
#endif

class SP_API Named {
public:
  Named(const StringC &name) : name_(name) { }
  virtual ~Named() { }
  const StringC &name() const { return name_; }
  const StringC *namePointer() const { return &name_; }
  void setName(const StringC &name) { name_ = name; }
  void swap(Named &to) { name_.swap(to.name_); }
private:
  StringC name_;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not Named_INCLUDED */
