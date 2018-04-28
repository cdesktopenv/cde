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
// $XConsortium: test13.C /main/5 1996/10/04 15:39:41 drk $
#include <stdio.h>
#include "Exceptions.hh"
#include <signal.h>

void
catch_abort (int, ...)
{
  puts ("Abort (core not dumped)");
  // Exit normally so Purify can report.
  exit (1);
}


class StackFixer
{
public:
  StackFixer()
    { printf ("I'm at address %p\n", this); }
};


class SomeClass : public Destructable
{
public:
  SomeClass()
    { puts ("Constructing SomeClass"); }
  ~SomeClass()
    { puts ("Destructing SomeClass"); }
};


class SomeOtherClass : public Destructable
{
public:
  SomeClass some_class;

  SomeOtherClass()
    { puts ("Constructing SomeOtherClass"); }
  ~SomeOtherClass()
    { puts ("Destructing SomeOtherClass"); }
};


class ErrorString : public Exception
{
public:
  DECLARE_EXCEPTION (ErrorString, Exception);

  ErrorString (char *s)
    { msg = s; }
  char *msg;
};


void
do_something()
{
  SomeOtherClass some_other_class;
  //  SomeClass some_class;

  throw (ErrorString ("Tossed my cookies in do_something"));
}


int
main()
{
  INIT_EXCEPTIONS();

  puts ("Testing cleanup of nested destructable objects.");

#ifdef MUST_EXIT
  signal (SIGABRT, catch_abort);
#endif

  try
    {
      do_something();
    }
  // Do it right in this section. 
  catch (ErrorString &,e)
    {
      puts ("Caught exception back in main.");
      puts (e.msg);
    }
  catch (Exception &,e)
    {
      puts ("This will never be printed.");
    }
  end_try;
}
