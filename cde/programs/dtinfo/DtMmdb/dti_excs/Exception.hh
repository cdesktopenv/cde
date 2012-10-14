
#include <ctype.h>

#define G_TEMP_SPACE_SIZE 1024

#define CASTEXCEPT

class Exception : public Destructable
{
public:
#ifdef NATIVE_EXCEPTIONS

#if defined(hpux)
  Exception() { }
#endif

#else
  Exception();

#if defined(linux) && defined(EXC_DEBUG)
~Exception()
{
    PRINTF (("Destroying Exception @ %p\n", this));
}
#elif defined(EXC_DEBUG)
~Exception()
{
    PRINTF (("Destroying Exception @ %p\n", this));
}
#endif

  // For printing debugging info. Can be overloaded by derived classes.
  virtual void print_exception()
    { }
  
  virtual const char *class_name()
    { return ("Exception"); }

  Exception *prepare_to_throw();

  // This operator invoked if this is thrown as an object/reference
  Exception *operator ->()
    { if (!f_thrown) f_thrown_as_pointer = 0;
      return (this); }

  operator Exception *()
    { return (this); }

  virtual int isa (const char *type)
    { return (is (type, "Exception")); }

  int is (const char *type, const char *this_class);

  static Exception &current_exception();

  const char *file()
    { return (f_file); }
  unsigned short line()
    { return (f_line); }

  // Need to provide a plain operator new definition because we
  // have a definition of the other one below.  (See the ARM.) 

#ifndef USL    
  static void *operator new (size_t size)
    { return (::operator new (size)); }
#endif

  static void operator delete (void *place);

  // For initial throw.  Redefined by decendents.
  // Makes temporaries for stack based objects and calls do_throw. 
  void throw_it (unsigned int line, const char *file, int debugging);

  // Actual throw code.  Also used for rethrows. 
  void do_throw (unsigned int line = 0,
		 const char *file = "(unknown)",
		 int debugging = 0);

  // Length is only valid for exceptions at the top of the stack! 
  int length() { return (g_next_avail - (char *) this); }
  void unmake_current();
  void make_current();
  static void relocate (Exception **exception, int length);

protected:
friend class Jump_Environment;
friend class Exceptions;

#if defined(hpux)||defined(__uxp__)
#define MakeOperatorNewPublic
#endif

#ifdef MakeOperatorNewPublic
// A problem with the HP-UX 3.65 compiler requires us to make this public.
// And, since we gen the Templates.nd.c file on a non-HPUX platform, we need
// a second way to trigger the change.
public:
#endif
  static void *operator new (size_t size, int);
#ifdef MakeOperatorNewPublic
protected:
#endif

// f_thrown flag is a safty feature to make sure that the f_throw_as_pointer
// flag can't be reset accidently by the user calling operator -> 
  
#ifdef PURIFY
  unsigned char f_thrown;
  unsigned char f_thrown_as_pointer;
  unsigned char f_temporary;
#else
  unsigned char f_thrown : 1;
  unsigned char f_thrown_as_pointer : 1;
  unsigned char f_temporary : 1;
#endif

private:
  // Line and file where exception was originally thrown from. 
  const char     *f_file;
  unsigned short  f_line;
  Exception      *f_previous_exception;

  static Exception *g_current_exception;

#ifdef C_API
  static char      *g_temp_space;
#else
  static char       g_temp_space[G_TEMP_SPACE_SIZE];
#endif

  static char      *g_next_avail;

#ifdef C_API
  friend void initialize_exception();
  friend void quit_exception();
#endif

#endif /* NATIVE_EXCEPTIONS */
};


// /////////////////////////////////////////////////////////////////
// DECLARE_EXCEPTION - macros for derived classes
// /////////////////////////////////////////////////////////////////

#ifndef NATIVE_EXCEPTIONS
// NOTE: class_name, is, and isa should be renamed to _exc_... 

#define _DECL_START(NAME) \
  const char *class_name() \
    { return (STRINGIFY(NAME)); } \
  void throw_it (unsigned int line, const char *file, int dbg) \
    { Exception *temp; \
      if (in_stack()) { \
	temp = new (0) NAME (*this); \
	((NAME *)temp)->f_temporary = 1; \
      } else \
        temp = this; \
      temp->do_throw (line, file, dbg); } \
  NAME *operator ->() { if (!f_thrown) f_thrown_as_pointer = 0; \
			return (this); } \
  operator NAME *() { return (this); } \
  int isa (const char *type) \
    { return (is (type, STRINGIFY(NAME)) ||

#define _DECL_END \
	      ); } \

// MUST be the first thing in a subclass or protections get screwed up.

#define DECLARE_EXCEPTION DECLARE_EXCEPTION1

#define DECLARE_EXCEPTION1(NAME,SUPER1) \
  _DECL_START (NAME) \
  SUPER1::isa (type) \
  _DECL_END

#define DECLARE_EXCEPTION2(NAME,SUPER1,SUPER2) \
  _DECL_START (NAME) \
  SUPER1::isa (type) || \
  SUPER2::isa (type) \
  _DECL_END

#define DECLARE_EXCEPTION3(NAME,SUPER1,SUPER2,SUPER3) \
  _DECL_START (NAME) \
  SUPER1::isa (type) || \
  SUPER2::isa (type) || \
  SUPER3::isa (type) \
  _DECL_END

#else

#define DECLARE_EXCEPTION(NAME,SUPER1)
#define DECLARE_EXCEPTION1(NAME,SUPER1)
#define DECLARE_EXCEPTION2(NAME,SUPER1,SUPER2)
#define DECLARE_EXCEPTION3(NAME,SUPER1,SUPER2)

#endif /* NATIVE_EXCEPTIONS */

// Users can define more if they need to... 
