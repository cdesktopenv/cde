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
// $XConsortium: test25.cc /main/3 1996/06/11 16:54:03 cde-hal $
#include <stdio.h>
#include "Exceptions.hh"
#include <signal.h>
#include <string.h>

void
catch_abort(int, ...)
{
  puts ("Abort (core not dumped)");
  // Exit normally so Purify can report.
  exit (1);
}

// Test of throw in error handler. 

void
error_handler (const char *[], int)
{
  puts ("Got into the error handler OK.");
  throw (Exception());
}

void terminator()
{
  puts ("I'll be back!");
  throw (Exception());
}

int
main()
{
  //  INIT_EXCEPTIONS();

  puts ("Test of error handler with no INIT.");
  
#ifdef MUST_EXIT
  signal (SIGABRT, catch_abort);
#endif

  Exceptions::set_error_handler (error_handler);
  set_terminate (terminator);

  try
    {
      puts ("Trying something");
      throw (Exception());
    }
  end_try;

}
