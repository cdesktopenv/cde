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
