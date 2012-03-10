// $XConsortium: test25.cc /main/3 1996/06/11 16:54:03 cde-hal $
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
  puts ("Got into the error handler OK.");
  throw (Exception());
}

void terminator()
{
  puts ("I'll be back!");
  throw (Exception());
}

int
main()
{
  //  INIT_EXCEPTIONS();

  puts ("Test of error handler with no INIT.");
  
#ifdef MUST_EXIT
  signal (SIGABRT, catch_abort);
#endif

  Exceptions::set_error_handler (error_handler);
  set_terminate (terminator);

  try
    {
      puts ("Trying something");
      throw (Exception());
    }
  end_try;

}
