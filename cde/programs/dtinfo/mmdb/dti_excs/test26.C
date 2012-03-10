// $XConsortium: test26.cc /main/3 1996/06/11 16:54:09 cde-hal $
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

class foo : public Destructable
{
public:
  ~foo()  {   puts ("Destructing foo."); }
};


void
cleanup()
{
  try
    {
      throw (Exception());
    }
  catch_any()
    {
      puts ("clean up caught");
    }
  end_try;
}

int
main()
{
  INIT_EXCEPTIONS();

#ifdef MUST_EXIT
  signal (SIGABRT, catch_abort);
#endif

  puts ("Testing nested try in catch.");

  try
    {
      foo bar;

      throw (Exception());
    }
  catch (Exception &,e)
    {
      puts ("Cleaning up");
      cleanup();
    }
  end_try;
}
