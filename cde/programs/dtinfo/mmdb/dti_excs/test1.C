/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
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
