// $TOG: terminate.C /main/4 1997/09/05 11:30:17 samborn $
#if !defined(NATIVE_EXCEPTIONS) && !defined(HAS_TERMINATE)
#include "Exceptions.hh"

typedef void (*PFV)();

static PFV _terminate_handler = NULL;

PFV
set_terminate (PFV handler)
{
  PFV old_handler = _terminate_handler;

  _terminate_handler = handler != NULL ? handler : NULL;

  return (old_handler);
}

void
terminate()
{
  if (_terminate_handler != NULL)
    {
      // Reset terminate handler to NULL before handling to
      // avoid potential recursive calls due to exceptions
      // abuse in the terminate handler. 
      PFV handler = _terminate_handler;
      _terminate_handler = NULL;
      try
	{
	  (*handler)();
	}
      catch_any()
	{
	  Exceptions::error (Exceptions::f_msg_throw_from_terminate,
			     Exceptions::APPLICATION_ERROR);
	}
      end_try;
    }

  abort();
}
#else

#ifdef hpux
// HPUX doesn't define the set_terminate function from the ARM. 
typedef void (*PFV)();
PFV set_terminate (PFV) { return (0); }
#endif

#endif /* NATIVE_EXCEPTIONS */
