// $XConsortium: test2.cc /main/3 1996/06/11 16:53:29 cde-hal $
#include <stdio.h>
#include "Exceptions.hh"
#include <signal.h>

void
catch_abort (int, ...)
{
  puts ("Abort (core not dumped)");
  // Exit normally so Purify can report.
  exit (0);
}

int
main()
{
  INIT_EXCEPTIONS();

#ifdef MUST_EXIT
  signal (SIGABRT, catch_abort);
#endif
  
  try
    {
      puts ("Trying something");
      throw (Exception());
    }
  end_try;

}
