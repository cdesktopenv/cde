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
// $XConsortium: init.cc /main/3 1996/06/11 16:51:58 cde-hal $

#ifndef NATIVE_EXCEPTIONS
#include "Exceptions.hh"

//#define G_TEMP_SPACE_SIZE 1024

void initialize_exception()
{

Exceptions::f_msg_internal_error =     (char*)"Internal exceptions error:";
Exceptions::f_msg_application_error =  (char*)"Application exceptions error:";
Exceptions::f_msg_throw_message =      (char*)"Application threw exception:";

Exceptions::f_msg_not_initialized =
  (char*)"Exceptions library not initialized with INIT_EXCEPTIONS().";

Exceptions::f_msg_initialized_twice =
  (char*)"Attept to call INIT_EXCEPTIONS() more than once.";
  
Exceptions::f_msg_not_caught =
  (char*)"Exception not caught.";

Exceptions::f_msg_no_current_exception =
  (char*)"There is no current exception (for catch or rethrow).";

Exceptions::f_msg_throw_from_terminate =
  (char*)"Exceptions may not be thrown from terminate.";

Exceptions::f_msg_throw_from_error_handler =
  (char*)"Exceptions may not be thrown from error handler.";

Exceptions::f_msg_throw_from_destructor =
  (char*)"Exited destructor with throw while handling an exception.";

Exceptions::f_msg_throw_ptr_to_stack =
  (char*)"Threw a pointer to an automatic (stack-based) exceptions object.";

Exceptions::f_msg_out_of_exception_memory =
  (char*)"Not enough memory to allocate an exception object.";

Exceptions::f_msg_out_of_obj_stack_memory =
  (char*)"Not enough memory to allocate object stack.";

Exceptions::f_msg_memory_already_freed =
  (char*)"Tried to alloc or realloc pool memory that was previously freed.";

   Unwind_Stack::g_stack = new Unwind_Record[UNWIND_STACK_SIZE];
   Exception::g_temp_space = new char[G_TEMP_SPACE_SIZE];
   Exception::g_next_avail = Exception::g_temp_space;
}

void quit_exception()
{
   delete Unwind_Stack::g_stack;
}

#endif
