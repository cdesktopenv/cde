// $XConsortium: test9.cc /main/3 1996/06/11 16:55:04 cde-hal $
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
  ~foo();
};

foo::~foo()
{
  puts ("Called foo destructor.");
  throw (Exception());
}


int
main()
{
  INIT_EXCEPTIONS();

#ifdef MUST_EXIT
  signal (SIGABRT, catch_abort);
#endif

  puts ("Testing illegal throw from destructor.");

  try
    {
      foo bar;

      throw (Exception());
    }
  catch (Exception &,e)
    {
      puts ("This should never be printed.");
    }
  end_try;
}
