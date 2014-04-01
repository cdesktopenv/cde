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
// $XConsortium: Exception.C /main/4 1996/10/04 09:40:25 drk $
#ifndef NATIVE_EXCEPTIONS
#include "Exceptions.hh"
#include <memory.h>

Exception *Exception::g_current_exception;

#ifdef C_API
char*  Exception::g_temp_space = 0;
char *Exception::g_next_avail = 0;
#else
char  Exception::g_temp_space[G_TEMP_SPACE_SIZE];
char *Exception::g_next_avail = Exception::g_temp_space;
#endif

// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

Exception::Exception()
: f_thrown(0), f_thrown_as_pointer(1), f_temporary(0), f_line(0)
{
  PRINTF (("Constructed Exception obj @ %p\n", this));
}


// /////////////////////////////////////////////////////////////////
// operator delete
// /////////////////////////////////////////////////////////////////

void
Exception::operator delete (void *place)
{
  // Move pointer back if it's in our temp space, else just free it. 
  if (place >= g_temp_space && place < g_temp_space + G_TEMP_SPACE_SIZE)
    {
      g_next_avail = (char *) place;
      PRINTF (("De-alloc EXC @ %p\n", place));
    }
  else
    {
      free ((char *) place);
    }
}


// /////////////////////////////////////////////////////////////////
// operator new
// /////////////////////////////////////////////////////////////////

// int arg is only used for type matching to insure that our version
//  of new gets called to make temporaries. 

void *
Exception::operator new (size_t size, int)
{
  if (g_next_avail + size > g_temp_space + G_TEMP_SPACE_SIZE)
    {
      Exceptions::error (Exceptions::f_msg_out_of_exception_memory,
			 Exceptions::INTERNAL_ERROR);
      terminate();
    }

  void *p = g_next_avail;
  g_next_avail += size;
  PRINTF (("Allocate EXC @ %p, size = %ld\n", p, (long)size));
  return (p);
}


// /////////////////////////////////////////////////////////////////
// throw_it
// /////////////////////////////////////////////////////////////////

// According to ARM, 15.2c temporary object thrown is of the static
// type of the operand to throw, so this method must be non-virtual.
// A non-virtual version of this function is created with macros in
// each subclass of the Exception object.  See Exception.hh. 

void
Exception::throw_it (unsigned int line, const char *file, int dbg)
{
  // NOTE: This is the only place we can detect a throw of a pointer
  // to a stack based object.

  Exception *temp;
  PRINTF (("Preparing to throw Exception object at %p:\n", this));

  // Only make a temporary if the exception object is on the stack.
  if (in_stack())
    {
      PRINTF (("  Exception is on the stack - copying it\n"));
      // Use the special new operator which allocates the exception
      // in reserved storage if there's no memory available.
      temp = new (0) Exception (*this);
      // We created it, so we should delete it. 
      temp->f_temporary = 1;
    }
  else
    {
      PRINTF (("  Exception is on the heap\n"));
      temp = this;
    }

  temp->do_throw (line, file, dbg);
}


// /////////////////////////////////////////////////////////////////
// throw - throw the current exception
// /////////////////////////////////////////////////////////////////

void
Exception::do_throw (unsigned int line, const char *file, int debugging)
{
#ifdef EXC_DEBUG
  if (f_line == 0)
    {
      PRINTF (("========== Throwing exception ==========\n"));
    }
  else // must be a rethrow 
    {
      PRINTF (("========== Re-throwing exception ==========\n"));
    }
#endif
  
  // Save line and file if we don't already have it.
  if (f_line == 0)
    {
      f_line = line;
      f_file = file;
    }

  f_thrown = 1;
  Jump_Environment::unwind_and_jump (this, debugging);
}


// /////////////////////////////////////////////////////////////////
// current_exception - return the current exception
// /////////////////////////////////////////////////////////////////

Exception &
Exception::current_exception()
{
  if (g_current_exception == NULL)
    {
      Exceptions::error (Exceptions::f_msg_no_current_exception,
			 Exceptions::APPLICATION_ERROR);
      terminate();
    }

  return (*g_current_exception);
}


// /////////////////////////////////////////////////////////////////
// (un)make_current - (un)make this exception the current one
// /////////////////////////////////////////////////////////////////

void
Exception::unmake_current()
{
  PRINTF (("Current Exc POP %p <-- %p\n",
	   g_current_exception, f_previous_exception));
  if (this != g_current_exception)
    {
      Exceptions::error ("Popped exception is the current one!",
			 Exceptions::INTERNAL_ERROR);
      terminate();
    }

  // Called when a catch clause is exited. 
  g_current_exception = f_previous_exception;
}


void
Exception::make_current()
{
  PRINTF (("Current Exc PUSH --> %p\n", this));
  f_previous_exception = g_current_exception;
  g_current_exception = this;
}


// /////////////////////////////////////////////////////////////////
// relocate - move this exception to the current top of stack
// /////////////////////////////////////////////////////////////////

void
Exception::relocate (Exception **exception, int length)
{
  PRINTF (("Moving %p --> %p (%d bytes)\n",
	   *exception, g_next_avail, length));
  // Slide the specified exception down to fill the hole below it.
  if (g_next_avail >= (char *) *exception)
    abort();
  memcpy (g_next_avail, (void*)*exception, length);
  *exception = (Exception *) g_next_avail;
  g_next_avail = ((char *) *exception) + length;
}


// /////////////////////////////////////////////////////////////////
// is - return true this class is of type TYPE
// /////////////////////////////////////////////////////////////////

int
Exception::is (const char *type, const char *this_class)
{
  PRINTF (("Type specified is <%s>\n", type));
  
  while (isalnum ((unsigned char) *type) &&
	 isalnum ((unsigned char) *this_class) &&
	 *type++ == *this_class++);
  if (isalnum ((unsigned char) *type) ||
      isalnum ((unsigned char) *this_class))
    return (0);

  // Check for pointer types
  while (isspace ((unsigned char) *type))
    {
      type++;
    }
  PRINTF (("  var part is <%s>\n", type));

  // See if one's a pointer and the other isn't. 
  if ((*type == '*' && !f_thrown_as_pointer) ||
      (*type != '*' &&  f_thrown_as_pointer))
    return (0);
  // Otherwise they are either both pointers or both objects/references.
  return (1);
}
#endif /* NATIVE_EXCEPTIONS */
