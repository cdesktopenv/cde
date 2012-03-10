// $XConsortium: test1.C /main/4 1996/10/04 15:40:56 drk $
#include <stdio.h>
#include "Exceptions.hh"

int
main()
{
  INIT_EXCEPTIONS();

#ifndef NATIVE_EXCEPTIONS
  printf ("Unwind_Record size = %ld\n", (long)sizeof (Unwind_Record));
#endif
  printf ("Exception size = %ld\n", (long)sizeof (Exception));
  
  puts ("CODE Exception e");
  Exception e;
  // Test Destructable copy constructor. 
  puts ("CODE Exception b = e");
  Exception b = e;
  puts ("CODE Exception *z = new Exception()");
  Exception *z = new Exception();
  // Test Destructable assignment operator. 
  puts ("CODE Exception *z = b");
  *z = b;
  puts ("CODE delete z");
  delete z;

  try
    {
      puts ("Trying something");
      throw (Exception());
    }
  catch (Exception &,e)
    {
      puts ("Caught an exception");
    }
  end_try;
}
