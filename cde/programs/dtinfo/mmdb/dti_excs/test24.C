// $XConsortium: test24.cc /main/3 1996/06/11 16:53:58 cde-hal $
#include <stdio.h>
#include "Exceptions.hh"
#include <signal.h>

void
catch_abort(int, ...)
{
  puts ("Abort (core not dumped)");
  // Exit normally so Purify can report.
  exit (1);
}

int
main()
{
  puts ("Test of something without initialize.");
  //  INIT_EXCEPTIONS();

#ifdef MUST_EXIT
  signal (SIGABRT, catch_abort);
#endif

  try
    {
      puts ("Trying something");
    }
  catch (Exception &,e)
    {
      puts ("Caught an exception");
    }
  end_try;
}
