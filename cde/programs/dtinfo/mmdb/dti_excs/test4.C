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
