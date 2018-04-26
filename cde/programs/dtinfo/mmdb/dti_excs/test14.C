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
// $XConsortium: test14.cc /main/3 1996/06/11 16:52:54 cde-hal $
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


class BaseClass : public Destructable
{
public:
  BaseClass()
    { puts ("Constructing BaseClass"); }
  ~BaseClass()
    { puts ("Destructing BaseClass"); }
};

class BaseClass2 : public Destructable
{
public:
  BaseClass2()
    { puts ("Constructing BaseClass2"); }
  ~BaseClass2()
    { puts ("Destructing BaseClass2"); }
};

class DerivedClass : public BaseClass
{
public:
  DerivedClass()
    { puts ("Constructing DerivedClass"); }
  ~DerivedClass()
    { puts ("Destructing DerivedClass"); }
};

class DerivedClass2 : public BaseClass, public BaseClass2
{
public:
  DerivedClass2()
    { puts ("Constructing DerivedClass2"); }
  ~DerivedClass2()
    { puts ("Destructing DerivedClass2"); }
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
  DerivedClass derived_class;
  DerivedClass2 derived_class2;

  throw (ErrorString ("Tossed my cookies in do_something"));
}


int
main()
{
  INIT_EXCEPTIONS();

  puts ("Testing destruction with single and multiple inheritance.");

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
