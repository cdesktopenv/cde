// $XConsortium: init.cc /main/3 1996/06/11 16:51:58 cde-hal $

#ifndef NATIVE_EXCEPTIONS
#include "Exceptions.hh"

//#define G_TEMP_SPACE_SIZE 1024

void initialize_exception()
{

Exceptions::f_msg_internal_error =     "Internal exceptions error:";
Exceptions::f_msg_application_error =  "Application exceptions error:";
Exceptions::f_msg_throw_message =      "Application threw exception:";

Exceptions::f_msg_not_initialized =
  "Exceptions library not initialized with INIT_EXCEPTIONS().";

Exceptions::f_msg_initialized_twice =
  "Attept to call INIT_EXCEPTIONS() more than once.";
  
Exceptions::f_msg_not_caught =
  "Exception not caught.";

Exceptions::f_msg_no_current_exception =
  "There is no current exception (for catch or rethrow).";

Exceptions::f_msg_throw_from_terminate =
  "Exceptions may not be thrown from terminate.";

Exceptions::f_msg_throw_from_error_handler =
  "Exceptions may not be thrown from error handler.";

Exceptions::f_msg_throw_from_destructor =
  "Exited destructor with throw while handling an exception.";

Exceptions::f_msg_throw_ptr_to_stack =
  "Threw a pointer to an automatic (stack-based) exceptions object.";

Exceptions::f_msg_out_of_exception_memory =
  "Not enough memory to allocate an exception object.";

Exceptions::f_msg_out_of_obj_stack_memory =
  "Not enough memory to allocate object stack.";

Exceptions::f_msg_memory_already_freed =
  "Tried to alloc or realloc pool memory that was previously freed.";

   Unwind_Stack::g_stack = new Unwind_Record[UNWIND_STACK_SIZE];
   Exception::g_temp_space = new char[G_TEMP_SPACE_SIZE];
   Exception::g_next_avail = Exception::g_temp_space;
}

void quit_exception()
{
   delete Unwind_Stack::g_stack;
}

#endif
