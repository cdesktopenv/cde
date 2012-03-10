// $XConsortium: test29.cc /main/4 1996/08/08 19:55:36 cde-hal $
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

  virtual void name()
    { puts ("ErrorString"); }

  ErrorString (char *s)
    { msg = s; }
  char *msg;
};

class ErrorThing : public ErrorString
{
public:
  DECLARE_EXCEPTION (ErrorThing,ErrorString);

  ErrorThing (char *s)
    : ErrorString (s) { }

  virtual void name()
    { puts ("ErrorThing"); }
};


void
do_something()
{
  try
    {
      throw (ErrorThing ("<error message from do_something>"));
    }
  // Catch the more general exception first -- a definite mistake. 
  catch (Exception &,e)
    {
      puts ("Caught in do_something, rethrowing.");
      rethrow;
    }
  catch (ErrorString &,e)
    {
      puts ("This will never be printed.");
    }
  end_try;
}


int
main()
{
  puts ("Testing mult-depth throws of different exceptions.");

  INIT_EXCEPTIONS();

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
      puts ("Caught exception back in main.");
      e.name();
      puts (e.msg);
    }
  catch (Exception &,e)
    {
      puts ("This will never be printed.");
    }
  end_try;
}
