// $XConsortium: test12.cc /main/3 1996/06/11 16:52:43 cde-hal $
#include <stdio.h>
#include "Exceptions.hh"
#include <signal.h>

void
catch_abort (int, ...)
{
  puts ("Abort (core not dumped)");
  // Exit normally so Purify can report.
  exit (1);
}

class ErrorString : public Exception
{
public:
  DECLARE_EXCEPTION (ErrorString, Exception);

  ErrorString (char *s)
    { msg = s; }
  char *msg;
};


void
do_something()
{
  try
    {
      throw (ErrorString ("<error message from do_something>"));
    }
  // Catch the more general exception first -- a definite mistake. 
  catch (Exception &,e)
    {
      puts ("Caught in do_something, rethrowing.");
      rethrow;
    }
  catch (ErrorString &,e)
    {
      puts ("This will never be printed.");
    }
  end_try;
}


int
main()
{
  INIT_EXCEPTIONS();

  puts ("Testing mult-depth throws of different exceptions.");

#ifdef MUST_EXIT
  signal (SIGABRT, catch_abort);
#endif

  try
    {
      do_something();
    }
  // Do it right in this section. 
  catch (ErrorString &,e)
    {
      puts ("Caught exception back in main.");
      puts (e.msg);
    }
  catch (Exception &,e)
    {
      puts ("This will never be printed.");
    }
  end_try;
}
