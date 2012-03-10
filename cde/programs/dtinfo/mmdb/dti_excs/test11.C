// $XConsortium: test11.cc /main/3 1996/06/11 16:52:38 cde-hal $
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

void
do_something()
{
  try
    {
      throw (Exception());
    }
  catch (Exception &,e)
    {
      puts ("Caught in do_something, rethrowing.");
      rethrow;
    }
  end_try;
}


int
main()
{
  INIT_EXCEPTIONS();

  puts ("Testing mult-depth rethrow.");

#ifdef MUST_EXIT
  signal (SIGABRT, catch_abort);
#endif

  try
    {
      do_something();
    }
  catch (Exception &,e)
    {
      puts ("Caught exception back in main.");
    }
  end_try;
}
