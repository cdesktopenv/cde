// $XConsortium: test19.cc /main/3 1996/06/11 16:53:23 cde-hal $
#include <stdio.h>
#include "Exceptions.hh"

void
catch_abort (int, ...)
{
  puts ("Abort (core not dumped)");
  // Exit normally so Purify can report.
  exit (1);
}

class BigException : public Exception
{
public:
  DECLARE_EXCEPTION (BigException, Exception);
  BigException() { }

  int waste_space[300];
};


int
main()
{
  INIT_EXCEPTIONS();

#ifdef MUST_EXIT
  signal (SIGABRT, catch_abort);
#endif

  printf ("Sucking up a lot of memory to test internal allocator.\n");
  try
    {
      // Should run out of memory trying to allocate this one. 
      throw (BigException());
    }
  catch (Exception &,e)
    {
      puts ("Caught an exception reference");
    }
  catch (Exception *,e)
    {
      puts ("Caught an exception pointer");
    }
  end_try;
}
