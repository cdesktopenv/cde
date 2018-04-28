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
// $XConsortium: test19.cc /main/3 1996/06/11 16:53:23 cde-hal $
#include <stdio.h>
#include "Exceptions.hh"

void
catch_abort (int, ...)
{
  puts ("Abort (core not dumped)");
  // Exit normally so Purify can report.
  exit (1);
}

class BigException : public Exception
{
public:
  DECLARE_EXCEPTION (BigException, Exception);
  BigException() { }

  int waste_space[300];
};


int
main()
{
  INIT_EXCEPTIONS();

#ifdef MUST_EXIT
  signal (SIGABRT, catch_abort);
#endif

  printf ("Sucking up a lot of memory to test internal allocator.\n");
  try
    {
      // Should run out of memory trying to allocate this one. 
      throw (BigException());
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
