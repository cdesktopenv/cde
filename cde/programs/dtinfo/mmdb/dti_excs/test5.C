// $XConsortium: test5.cc /main/3 1996/06/11 16:54:41 cde-hal $
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
terminate_handler()
{
  throw (Exception());
}

int
main()
{
  INIT_EXCEPTIONS();

#ifdef MUST_EXIT
  signal (SIGABRT, catch_abort);
#endif

  set_terminate (terminate_handler);
  
  try
    {
      puts ("Trying something");
      throw (Exception());
    }
  end_try;

}
