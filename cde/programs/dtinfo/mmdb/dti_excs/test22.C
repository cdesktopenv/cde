// $XConsortium: test22.cc /main/3 1996/06/11 16:53:47 cde-hal $
#include <stdio.h>
#include "Exceptions.hh"
#include <signal.h>
#include <string.h>

void
catch_abort(int, ...)
{
  puts ("Abort (core not dumped)");
  // Exit normally so Purify can report.
  exit (1);
}

// Test of throw in error handler. 

void
error_handler (const char *[], int)
{
  puts ("Throwing an exception from the error handler.");
  throw (Exception());
}

int
main()
{
  INIT_EXCEPTIONS();

  puts ("Testing throw from error handler.");
  
#ifdef MUST_EXIT
  signal (SIGABRT, catch_abort);
#endif

  Exceptions::set_error_handler (error_handler);

  try
    {
      puts ("Trying something");
      throw (Exception());
    }
  end_try;

}
