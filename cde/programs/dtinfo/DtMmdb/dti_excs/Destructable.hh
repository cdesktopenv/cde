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
// $XConsortium: Destructable.hh /main/3 1996/06/11 16:50:40 cde-hal $
#ifndef _Exceptions_hh_active
 #error "Include this file by including Exceptions.hh"
#endif

#ifdef C_API
#include "new_delete_simple.h"
#endif

class Destructable
{
#ifndef NATIVE_EXCEPTIONS
public:
  Destructable();
  // Copy and assignment constructors necessary to maintain proper
  // f_in_stack state variable.
  Destructable (const Destructable &);
  Destructable &operator = (const Destructable &);
  virtual ~Destructable();

  int in_stack();
  int in_stack_set_size();

#ifdef C_API
  NEW_AND_DELETE_SIGNATURES(Destructable);
#endif

private:
friend class Jump_Environment;
friend class Exceptions;
  static int stack_grows_down()
    { return (g_size == 0); }

  // Need this special method since Cfront 2.1 doesn't allow direct
  // call to destructor from another class. 
  void destruct();

  static void *g_stack_start;
  // We keep track of the size when the stack grows up.
  // This is copied over to the unwind stack for stack-based objects. 
  static unsigned short g_size;

#endif
};
