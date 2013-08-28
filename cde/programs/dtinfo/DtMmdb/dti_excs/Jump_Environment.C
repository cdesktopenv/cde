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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
// $XConsortium: Jump_Environment.C /main/6 1996/10/04 09:37:00 drk $
#ifndef NATIVE_EXCEPTIONS
#include "Exceptions.hh"

Jump_Environment *Jump_Environment::g_jump_env_stack;

#ifdef EXC_DEBUG
int Jump_Environment::g_level;
#endif

unsigned short Unwind_Stack::g_top ;

#ifdef C_API
Unwind_Record* Unwind_Stack::g_stack = 0; // inited in init.C
#else
Unwind_Record Unwind_Stack::g_stack[UNWIND_STACK_SIZE];
#endif


// /////////////////////////////////////////////////////////////////
// Jump_Environment - class constructor
// /////////////////////////////////////////////////////////////////

Jump_Environment::Jump_Environment()
: f_active_exception (NULL), f_unwinding (0)
{
  PRINTF (("<%d> New Jump_Environment @ %p\n", ++g_level, this));
  // Push this on to the top of the jump env stack. 
  f_next = g_jump_env_stack;
  g_jump_env_stack = this;
}

// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

Jump_Environment::~Jump_Environment()
{
  // If g_jump_env_stack == this an exception wasn't thrown.
  // (Because at throw time we set g_jump_env_stack to f_next.) 
  if (g_jump_env_stack == this)
    {
      g_jump_env_stack = f_next;
    }
  // An exception was thrown in our try block.
  // An exception may have been thrown in our catch block.
  // If one was, g_jump_env_stack->f_active_exception != NULL. 
  if (f_active_exception != NULL)
    {
      delete_active();
    }
  PRINTF (("<%d> Done with Jump_Environment @ %p\n",
	   g_level--, this));
}


// /////////////////////////////////////////////////////////////////
// unwind - call destructors for stack based objects
// /////////////////////////////////////////////////////////////////

void
Jump_Environment::do_unwind_and_jump (Exception *exception, int debugging)
{
  PRINTF (("----- <%d> Unwinding stack -----\n", g_level));

  // Remember the current exception so we can delete it later.
  f_active_exception = exception;

  // Check for a throw out of a destructor. 
  if (f_unwinding)
    {
      Exceptions::error (Exceptions::f_msg_throw_from_destructor,
			 Exceptions::APPLICATION_ERROR);
      terminate();
    }

  f_unwinding = 1;

  // Call the destructor of each objet on the stack in reverse.
  // Length is automatically decremented as each object unregisters itself.
  while (!f_unwind_stack.empty())
    {
      PRINTF (("* Calling dtor of %p\n", f_unwind_stack.top().f_object));
#if CC_VERSION < 30
      f_unwind_stack.top().f_object->destruct();
#else
      f_unwind_stack.top().f_object->~Destructable();
#endif
    }

  // This Jump Environment is no longer needed, so we need to pull it off
  // the global jump env stack.  Any new stack objects from this point
  // on must be registed in the enclosing Jump Environment scope for the
  // purposes of exceptions or otherwise.  The memory associated with the
  // unwind_stack is freed when we exit the scope or throw an exception. 
  g_jump_env_stack = f_next;
  PRINTF (("----- <%d> Unwind complete -----\n", g_level));

  // Make the current exception official (it may move in the process).
  f_active_exception->make_current();

  // Print a message about the throw point if debugging is on. 
  if (debugging)
    {
      Exceptions::error_handler_t saved = Exceptions::set_error_handler (NULL);
      Exceptions::error (NULL, Exceptions::THROW_MESSAGE);
      Exceptions::set_error_handler (saved);
    }

  // And away we go... 
  longjmp();
}


// /////////////////////////////////////////////////////////////////
// delete_active - delete active exception, maybe move pending
// /////////////////////////////////////////////////////////////////

void
Jump_Environment::delete_active()
{
  // For starters, it's no longer the current exception.
  f_active_exception->unmake_current();

  // We normally delete the current exception when it's enclosing
  // environment is destructed, UNLESS the same exception is being
  // rethrown to the handler above.  g_jump_env_stack points to
  // the Jump_Environment that is unwinding.

  if (f_active_exception->f_temporary &&
      (g_jump_env_stack == NULL || f_active_exception != pending_exception()))
    {
      // If there's one pending we created and a current one, we are going
      // to have to move the pending one down on our stack after
      // deleting the current one, so we need to remember where it ends.
      // If the pending exception == the current one, then we're just
      // returning to the scope of a previous exception. 
      
      if (g_jump_env_stack != NULL &&
	  pending_exception() != NULL &&
	  pending_exception()->f_temporary &&
	  pending_exception() != Exception::g_current_exception)
	{
	  // The length() method is only valid here! 
	  int length = pending_exception()->length();
	  delete f_active_exception;
	  Exception::relocate (&g_jump_env_stack->f_active_exception,
			       length);
	}
      else  // Just delete it -- no move necessary. 
	{
	  delete f_active_exception;
	}
    }
}
#endif /* NATIVE_EXCEPTIONS */
