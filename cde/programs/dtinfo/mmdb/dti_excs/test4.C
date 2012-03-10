// $XConsortium: test4.cc /main/3 1996/06/11 16:54:36 cde-hal $
#include <stdio.h>
#include "Exceptions.hh"

int
main()
{
  INIT_EXCEPTIONS();
  int retry = 10000;

  printf ("Executing %d iterations of the try loop.\n", retry);
  while (retry)
    {
      //  printf ("Iter = %d\n", retry);
      try
	{
	  switch (retry-- % 3)
	    {
	    case 2:
	      // puts ("Throwing exception object");
	      throw (Exception());

	    case 1:
	      {
		Exception e;
		Exception *ep = &e;
		// puts ("Throwing exception pointer");
		throw (ep);
	      }

	    // case 0:
	      // puts ("Not throwing a damn thing.");
	    }
	}
      catch (Exception &,e)
	{
	   // puts ("Caught an exception reference");
	}
      catch (Exception *,e)
	{
	  // puts ("Caught an exception pointer");
	}
      end_try;
    }
}
