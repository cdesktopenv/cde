// $XConsortium: test10.cc /main/3 1996/06/11 16:52:32 cde-hal $
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
  foo (char *s);
  ~foo();
  char *name;
};

foo::foo (char *s)
{
  name = s;
  printf ("constructing <%s>\n", name);
}

foo::~foo()
{
  printf ("destructing <%s>\n", name);
}


int
main()
{
  INIT_EXCEPTIONS();

  puts ("Testing stack unwinding and illegal rethrow out of main.");

  foo foo_main ("Main");

#ifdef MUST_EXIT
  signal (SIGABRT, catch_abort);
#endif

  try
    {
      foo foo_try ("Try");

      throw (Exception());
    }
  catch (Exception &,e)
    {
      puts ("Caught exception.");
      foo foo_catch ("Catch");
      rethrow;
    }
  end_try;
}
