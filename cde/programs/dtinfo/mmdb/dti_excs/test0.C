// $XConsortium: test0.cc /main/3 1996/06/11 16:52:21 cde-hal $
#include <stdio.h>
#include "Exceptions.hh"

int
main()
{
  INIT_EXCEPTIONS();

  try
    {
      puts ("Trying something");
    }
  catch (Exception &,e)
    {
      puts ("Caught an exception");
    }
  end_try;
}
