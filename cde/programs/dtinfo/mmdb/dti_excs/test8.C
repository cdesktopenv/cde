// $XConsortium: test8.cc /main/3 1996/06/11 16:54:58 cde-hal $
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

int
main()
{
  INIT_EXCEPTIONS();

#ifdef MUST_EXIT
  signal (SIGABRT, catch_abort);
#endif

  puts ("Testing catch with no try.");
  if (0) {
    // remember that catch expands to an "else if"
  catch (Exception &,e)
    {
      puts ("The should never be printed");
    }
  }
}
