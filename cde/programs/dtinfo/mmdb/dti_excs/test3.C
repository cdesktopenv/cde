// $XConsortium: test3.cc /main/3 1996/06/11 16:54:31 cde-hal $
#define DEBUG_THROW
#define DEBUG_CATCH
#include <stdio.h>
#include "Exceptions.hh"

int
main()
{
  INIT_EXCEPTIONS();
  int retry = 3;

  puts ("Testing throw and catch debugging.");
  while (retry)
    {
      try
	{
	  switch (retry--)
	    {
	    case 3:
	      puts ("Throwing exception object");
	      throw (Exception());

	    case 2:
	      {
		Exception e;
		Exception *ep = &e;
		puts ("Throwing exception pointer");
		// NOTE: This isn't normally a good idea, since ep
		// points to a stack based value. 
		throw (ep);
	      }

	    case 1:
	      puts ("Not throwing a damn thing.");
	    }
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
}
