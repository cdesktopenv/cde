// $XConsortium: test21.cc /main/3 1996/06/11 16:53:41 cde-hal $
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

class ErrorString : public Exception
{
public:
  DECLARE_EXCEPTION (ErrorString, Exception);

  ErrorString (char *s)
    { msg = s; }
  char *msg;
};


void
do_something()
{
  try
    {
      throw (ErrorString ("<error message from do_something>"));
    }
  catch (ErrorString &,e)
    {
      puts ("Caught ErrorString exception.");
      throw (Exception());
    }
  end_try;
}


int
main()
{
  INIT_EXCEPTIONS();

  puts ("Testing mult-depth throws of different exceptions.");

#ifdef MUST_EXIT
  signal (SIGABRT, catch_abort);
#endif

  try
    {
      do_something();
    }
  // Do it right in this section. 
  catch (ErrorString &,e)
    {
      puts ("This is not printed.");
      puts (e.msg);
    }
  catch (Exception &,e)
    {
      puts ("This will be printed.");
    }
  end_try;
}
