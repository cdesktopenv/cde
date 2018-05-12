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
// $XConsortium: Jump_Environment.hh /main/6 1996/10/04 09:35:47 drk $
#include <setjmp.h>

#include "Unwind_Stack.hh"

class Jump_Environment : public Destructable
{
public:
  Jump_Environment();

  // Only unregister the environment if it hasn't been unregistered.
  // It is unregistered when an exception is thrown and stack unwound. 
  ~Jump_Environment();

  static void register_object (Destructable *object);
  static void unregister_object (Destructable *object);

  static void unwind_and_jump (Exception *current, int debugging)
    { g_jump_env_stack->do_unwind_and_jump (current, debugging); }

  // Must be public so setjmp can be called on it.  Can't use a
  // method because of the way setjmp works. 
  jmp_buf            f_env;
#ifdef SVR4
  // NOTE: this MUST follow f_env...mask bug in setjmp that overwrites our data
  // 13:59 08/03/93 - jbm 
  int		     f_filler[8] ;
#endif

private:
  void longjmp()
    { ::longjmp (f_env, 1); }
  void do_register (Destructable *);
  void do_unregister (Destructable *);
  void do_unwind_and_jump (Exception *, int debugging);
  void delete_active();
  // The pending exception is the exception that is about to become
  // the current exception.  It may be NULL if none has been thrown. 
  static Exception *pending_exception()
    { return (g_jump_env_stack->f_active_exception); }

private: // variables
  Exception         *f_active_exception;
  Jump_Environment  *f_next;           // For Jump_Enviroment stacking.
  unsigned char      f_unwinding;      // True when unwind in progress. 
  Unwind_Stack       f_unwind_stack;   // Stack of objects to unwind.

friend class Exceptions;
  static Jump_Environment *g_jump_env_stack;
  static Jump_Environment *g_used_jump_env_stack;
#ifdef EXC_DEBUG
  static int g_level;
#endif
};


// /////////////////////////////////////////////////////////////////
// do_register
// /////////////////////////////////////////////////////////////////

// Register the object if it isn't a member of another Destructable
// object.  If it is a member of another, that object (or the object
// that it's a member of) must be the last thing we registered.

// First check for downward growing stack.  In this case the object
// is part of another if it's address is greater than the start of
// that other object.  If it wasn't a member of that object, it's
// address would have to be lower than that object, having been 
// created after that object.

// Second check for upward growing stack.  In this case the object
// is part of another if it's address is less than the end of that
// other object.

// If the stack is empty, it's the first object and can't be in another.

inline void
Jump_Environment::do_register (Destructable *object)
{
  PRINTF (("  Considering object %p: ", object));
    
  if (f_unwind_stack.empty() ||
      (Destructable::stack_grows_down() ?
       (unsigned long) object < f_unwind_stack.top().object_start() :
       (unsigned long) object > f_unwind_stack.top().object_end()))
    {
      PRINTF ((" -- registered\n"));
      f_unwind_stack.push (object, Destructable::g_size);
    }
#ifdef EXC_DEBUG
  else
    {
      PRINTF ((" -- not registered\n"));
    }
#endif 
}


// /////////////////////////////////////////////////////////////////
// unregister_object
// /////////////////////////////////////////////////////////////////

inline void
Jump_Environment::do_unregister (Destructable *object)
{
  PRINTF (("  Unregister object @ %p:  ", object));
  // Don't do anything it it wasn't on the stack.  It must have been
  // part of another object or was never really constructed. 
  if (!f_unwind_stack.empty() && f_unwind_stack.top().f_object == object)
    {
      PRINTF ((" -- removed\n"));
      f_unwind_stack.pop();
    }
#ifdef EXC_DEBUG
  else
    {
      PRINTF ((" -- never registered\n"));
    }
#endif
}



// /////////////////////////////////////////////////////////////////
// register/unregister
// /////////////////////////////////////////////////////////////////

inline void
Jump_Environment::register_object (Destructable *object)
{
  g_jump_env_stack->do_register (object);
}

inline void
Jump_Environment::unregister_object (Destructable *object)
{
  g_jump_env_stack->do_unregister (object);
}

