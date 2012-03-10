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
