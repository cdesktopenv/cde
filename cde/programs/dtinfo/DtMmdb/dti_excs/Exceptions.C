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
// $XConsortium: Exceptions.cc /main/3 1996/06/11 16:51:03 cde-hal $
#ifndef NATIVE_EXCEPTIONS
#include "Exceptions.hh"
//#include <new.h>
#include <memory.h>
#include <string.h>

#ifdef C_API
char* Exceptions::f_msg_internal_error =   0;
char* Exceptions::f_msg_application_error = 0;
char* Exceptions::f_msg_throw_message =  0;
char* Exceptions::f_msg_not_initialized = 0;
char* Exceptions::f_msg_initialized_twice = 0;
char* Exceptions::f_msg_not_caught = 0;
char* Exceptions::f_msg_no_current_exception = 0;
char* Exceptions::f_msg_throw_from_terminate = 0;
char* Exceptions::f_msg_throw_from_error_handler = 0;
char* Exceptions::f_msg_throw_from_destructor = 0;
char* Exceptions::f_msg_throw_ptr_to_stack = 0;
char* Exceptions::f_msg_out_of_exception_memory = 0;
char* Exceptions::f_msg_out_of_obj_stack_memory = 0;
char* Exceptions::f_msg_memory_already_freed = 0;
#else
char *Exceptions::f_msg_internal_error =
  (char*)"Internal exceptions error:";

char *Exceptions::f_msg_application_error =
  (char*)"Application exceptions error:";

char *Exceptions::f_msg_throw_message =
  (char*)"Application threw exception:";

char *Exceptions::f_msg_not_initialized =
  (char*)"Exceptions library not initialized with INIT_EXCEPTIONS().";

char *Exceptions::f_msg_initialized_twice =
  (char*)"Attept to call INIT_EXCEPTIONS() more than once.";
  
char *Exceptions::f_msg_not_caught =
  (char*)"Exception not caught.";

char *Exceptions::f_msg_no_current_exception =
  (char*)"There is no current exception (for catch or rethrow).";

char *Exceptions::f_msg_throw_from_terminate =
  (char*)"Exceptions may not be thrown from terminate.";

char *Exceptions::f_msg_throw_from_error_handler =
  (char*)"Exceptions may not be thrown from error handler.";

char *Exceptions::f_msg_throw_from_destructor =
  (char*)"Exited destructor with throw while handling an exception.";

char *Exceptions::f_msg_throw_ptr_to_stack =
  (char*)"Threw a pointer to an automatic (stack-based) exceptions object.";

char *Exceptions::f_msg_out_of_exception_memory =
  (char*)"Not enough memory to allocate an exception object.";

char *Exceptions::f_msg_out_of_obj_stack_memory =
  (char*)"Not enough memory to allocate object stack.";

char *Exceptions::f_msg_memory_already_freed =
  (char*)"Tried to alloc or realloc pool memory that was previously freed.";

#endif

// /////////////////////////////////////////////////////////////////
// initialize - initialize the exceptions library
// /////////////////////////////////////////////////////////////////

void
Exceptions::initialize (void *ptr)
{
  PRINTF (("Initializing exceptions library\n"));

  if (Destructable::g_stack_start != NULL)
    {
      error (f_msg_initialized_twice, APPLICATION_ERROR);
      terminate();
    }
  else
    {
      // These two values MUST be initialized before the static
      // Jump_Environment below, or the its Destructable base class
      // constructor will fail because it won't think the library
      // is initialized or it may think the Jump Environment is on
      // the stack because f_stack_start is 0.
      Destructable::g_stack_start = ptr;

      // The following is the global jump environment.  According to ARM
      // (6.7) this isn't initialized until the first call to this function,
      // NOT at a global level before main() is called.  This is important
      // because Jump_Environment is subclassed of Destructable, whose
      // constructor expects g_stack_start to be non-zero.
      static Jump_Environment __jump_env;

      if (setjmp (__jump_env.f_env) != 0)
	{
	  // Re-set base environment to allow for the creation of
	  // Destructable objects in error handler and terminate
	  // terminate function.
	  Jump_Environment::g_jump_env_stack = &__jump_env;

	  error (f_msg_not_caught, APPLICATION_ERROR);
	  terminate();
	}
    }
}


// Static variable declarations 

Exceptions::error_handler_t Exceptions::g_error_handler;

// Error message declarations
// The error messages are stored here because many appear in the
// header files and we don't want to duplicate them in every app
// that uses the header files. 


  
// /////////////////////////////////////////////////////////////////
// set_error_handler
// /////////////////////////////////////////////////////////////////

Exceptions::error_handler_t
Exceptions::set_error_handler (error_handler_t error_handler)
{
  error_handler_t previous = g_error_handler;
  g_error_handler = error_handler;
  return (previous);
}


// /////////////////////////////////////////////////////////////////
// error - print a useful error message
// /////////////////////////////////////////////////////////////////

void
Exceptions::error (const char *message, error_type_t error_type)
{
  static char buffer[3][100];
  static char *lines[3] = { buffer[0], buffer[1], buffer[2] };
  int count = 0;

  if (error_type == INTERNAL_ERROR)
    strcpy (buffer[count++], f_msg_internal_error);
  else if (error_type == APPLICATION_ERROR)
    strcpy (buffer[count++], f_msg_application_error);
  else
    strcpy (buffer[count++], f_msg_throw_message);
      
  // Don't use fprintf because it may try to allocate memory.
  if (Exception::g_current_exception != NULL)
    {
      sprintf (buffer[count++],
	       "   In exception thrown in file \"%s\", line %d,",
	       Exception::g_current_exception->f_file,
	       Exception::g_current_exception->f_line);
    }

  if (message != NULL)
    sprintf (buffer[count++], "   %s", message);

  // Call user print function if set, otherwise just dump lines.
  if (g_error_handler != NULL)
    {
      mtry
	{
	  // Reset global variable to NULL before calling to prevent
	  // the possibility of recursive calls. 
	  Exceptions::error_handler_t current = set_error_handler (NULL);
	  (*current) ((const char**)lines, count);
	  set_error_handler (current);
	}
      mcatch_any()
	{
	  // Error handler will be NULL at this point. 
	  Exceptions::error (Exceptions::f_msg_throw_from_error_handler,
			     Exceptions::APPLICATION_ERROR);
	  terminate();

	}
      end_try;
    }
  else
    {
      for (int i = 0; i < count; i++)
	{
	  fputs (buffer[i], stderr);
	  fputc ('\n', stderr);
	}
    }
}


// /////////////////////////////////////////////////////////////////
// check_initialized - exit with error if not initialized
// /////////////////////////////////////////////////////////////////

void
Exceptions::check_initialized()
{
  void terminate();
  if (Destructable::g_stack_start == NULL)
    {
      // Can't call user defined error handler unless we're initialized.
      // Also can't do lazy initialization because we need the stack
      // pointer from the start of main, not here.  From here is good
      // enough for terminating, however.
      INIT_EXCEPTIONS();
      error (f_msg_not_initialized, APPLICATION_ERROR);
      terminate();
    }
}

#endif /* NATIVE_EXCEPTIONS */
