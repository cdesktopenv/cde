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
/* $XConsortium: Options.h /main/1 1996/07/29 16:59:30 cde-hp $ */
// Copyright (c) 1996 James Clark
// See the file COPYING for copying permission.

#ifndef Options_INCLUDED
#define Options_INCLUDED 1

#include "Boolean.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

// This is a mildly C++ified version of getopt().
// It never prints any message.

template<class T>
class Options {
public:
  Options(int argc, T *const *, const T *);
  // Returns false if there are no more options.
  bool get(T &);
  T *arg() const { return arg_; } // optarg
  T opt() const { return opt_; }  // optopt
  int ind() const { return ind_; } // optind
private:
  const T *search(T) const;
  const T *opts_;
  T *const *argv_;
  int argc_;
  int ind_;
  T opt_;
  T *arg_;
  int sp_;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not Options_INCLUDED */

#ifdef SP_DEFINE_TEMPLATES
#include "Options.C"
#endif
