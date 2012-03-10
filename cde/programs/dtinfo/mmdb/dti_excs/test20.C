// $XConsortium: test20.cc /main/3 1996/06/11 16:53:34 cde-hal $
#include <stdio.h>
#include "Exceptions.hh"

int
main()
{
  INIT_EXCEPTIONS();

  printf ("Throw of new'ed exception.\n");
  Exception *z = new Exception();

  try
    {
      puts ("Trying something");
      throw ((*z));
    }
  catch (Exception &,e)
    {
      puts ("Caught an exception");
    }
  end_try;

  delete z;
}
