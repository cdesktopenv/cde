// $XConsortium: test27.C /main/4 1996/09/27 19:03:26 drk $
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

class foo : virtual public Destructable
{
public:
  foo();
  ~foo()  { puts ("Destructing foo."); }
};

foo::foo()
{
  printf ("Construct foo @ 0x%p\n", this);
}


int
main()
{
  INIT_EXCEPTIONS();

#ifdef MUST_EXIT
  signal (SIGABRT, catch_abort);
#endif

  puts ("Testing virtual inheritance of Destructable.");

  try
    {
      foo bar;

      throw (Exception());
    }
  catch (Exception &,e)
    {
      puts ("Got it.");
    }
  end_try;
}
