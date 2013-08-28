// $XConsortium: Exceptions.hh /main/4 1996/07/05 15:16:42 rws $
#ifndef _Exceptions_hh
#define _Exceptions_hh

#define _Exceptions_hh_active

#undef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#ifndef C_API
#ifndef NATIVE_EXCEPTIONS
#define NATIVE_EXCEPTIONS
#endif
#define Exception mException
#endif

#ifndef NATIVE_EXCEPTIONS
extern "C" {
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
}
#else
extern "C" {
#include <stdio.h>
#include <stdlib.h>
}
#endif

#ifndef NATIVE_EXCEPTIONS
#ifdef EXC_DEBUG
#define PRINTF(MSG) printf MSG
#ifndef DEBUG
#define DEBUG
#endif
#ifndef DEBUG_THROW
#define DEBUG_THROW
#endif
#ifndef DEBUG_CATCH
#define DEBUG_CATCH
#endif
#else
#define PRINTF(MSG)
#endif

#ifndef STRINGIFY
#if !defined(__STDC__) && !defined(hpux)
#define STRINGIFY(S) "S"
#else
#define STRINGIFY(S) #S
#endif
#endif

#ifndef UNUSED_VARIABLE
# if defined(__GNUC__)
#  define UNUSED_VARIABLE(x) x __attribute__((unused))
# elif defined(__LCLINT__)
#  define UNUSED_VARIABLE(x) /*@unused@*/ x
# else
#  define UNUSED_VARIABLE(x) x
# endif
#endif

#endif /* NATIVE_EXCEPTIONS */

#include "terminate.hh"
#include "Destructable.hh"
#include "Exception.hh"

#ifdef NATIVE_EXCEPTIONS

#define INIT_EXCEPTIONS()

#define mthrow(OBJ) throw OBJ
#define rethrow throw

#define mtry try

#define mcatch_any() catch(...)
#define mcatch_noarg(OBJ) catch(OBJ)
#define mcatch(TYPE,OBJ) catch(TYPE OBJ)

#define end_try

#else

// This macro, which should be the first thing in main, establishes a jump
// environment for the context of the entire program. 

#define INIT_EXCEPTIONS() \
  { int __stack_start; Exceptions::initialize (&__stack_start); }

// TRY MACRO 

#define mtry \
  { \
    Jump_Environment __jump_env; \
    if (setjmp (__jump_env.f_env) == 0) {

      
// THROW MACROS 

#ifdef DEBUG_THROW
#define DEBUG_THROW_FLAG 1
#else
#define DEBUG_THROW_FLAG 0
#endif

// This works if OBJ is an object or a pointer since Exception objects
// overload operator ->.
#if !defined(hpux) && !defined(USL)
#define mthrow(OBJ) \
  (OBJ)->throw_it (__LINE__, __FILE__, DEBUG_THROW_FLAG)
#else
#define mthrow(OBJ) \
  OBJ->throw_it (__LINE__, __FILE__, DEBUG_THROW_FLAG)
#endif

#define rethrow \
  Exception::current_exception().do_throw (__LINE__, __FILE__)

// CATCH MACROS 
      
#ifdef DEBUG_CATCH
#define PRINT_CATCH \
	fprintf (stderr, "Application caught exception:\n"); \
	fprintf (stderr, "   Thrown from file \"%s\", line %d\n", \
		 Exception::current_exception().file(), \
		 Exception::current_exception().line()); \
	fprintf (stderr, "   Caught in file \"%s\", line %d.\n", \
		 __FILE__, __LINE__);
#else
#define PRINT_CATCH
#endif

#define mcatch_any() \
    } else if (1) { \
      PRINT_CATCH

#define mcatch_noarg(OBJ) \
    } else if (Exception::current_exception().isa (STRINGIFY(OBJ))) { \
      PRINT_CATCH

#define mcatch(TYPE,OBJ) \
    mcatch_noarg (TYPE) \
    TYPE UNUSED_VARIABLE(OBJ) = (TYPE) Exception::current_exception();

#define end_try \
    } else { \
      rethrow; \
    } \
  }

class Exceptions
{
public:
  typedef void (*error_handler_t) (const char *[], int);

  static void initialize (void *ptr);

  // Error handling stuff (message appear below).
  enum error_type_t { INTERNAL_ERROR, APPLICATION_ERROR, THROW_MESSAGE };
  static void error (const char *, error_type_t);
  static error_handler_t set_error_handler (error_handler_t);

private:
friend class Destructable;
friend class Jump_Environment;
friend class Unwind_Stack;
friend class Exception;
friend void terminate();

  static void check_initialized();

protected: // variables
  // function pointer to error message handler
  static error_handler_t     g_error_handler;

  // Error types 
  static char *f_msg_internal_error;
  static char *f_msg_application_error;
  static char *f_msg_throw_message;

  // Usage errors. 
  static char *f_msg_not_initialized;
  static char *f_msg_initialized_twice;
  static char *f_msg_not_caught;
  static char *f_msg_no_current_exception;
  static char *f_msg_throw_from_terminate;
  static char *f_msg_throw_from_error_handler;
  static char *f_msg_throw_from_destructor;
  static char *f_msg_throw_ptr_to_stack;

  // Internal memory errors. 
  static char *f_msg_out_of_exception_memory;
  static char *f_msg_out_of_obj_stack_memory;
  static char *f_msg_memory_already_freed;

#ifdef C_API
  friend void initialize_exception();
  friend void quit_exception();
#endif
};

// includes for inline functions 

#include "Jump_Environment.hh"
#include "Destructable_il.hh"

#endif /* NATIVE_EXCEPTIONS */

#undef _Exceptions_hh_active

#endif /* _Exceptions_hh */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
