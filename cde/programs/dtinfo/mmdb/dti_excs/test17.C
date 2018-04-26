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
// $XConsortium: test17.C /main/5 1996/09/27 19:03:16 drk $
#include <stdio.h>
#include "Exceptions.hh"
#include <signal.h>

/* Throwing from constructor is not supported.  See test18.C.  */


void
catch_abort (int, ...)
{
  puts ("Abort (core not dumped)");
  // Exit normally so Purify can report.
  exit (1);
}

class MemberClass : public Destructable
{
public:
  char *name;
  MemberClass (char *s)
    : name(s)
    { printf ("Constructing MemberClass `%s' @ 0x%p\n", name, this);
      throw (Exception()); }
  ~MemberClass()
    { printf ("Destructing MemberClass\n", name); }
};

class BaseClass : public Destructable
{
public:
  MemberClass m1;
  BaseClass()
    : m1("m1")
    {
      puts ("Constructing BaseClass");
      throw (Exception());
    }
  ~BaseClass()
    { puts ("Destructing BaseClass"); }
};


class DerivedClass : public BaseClass
{
public:
  MemberClass m3;
  DerivedClass()
    : m3("m3")
    { puts ("Constructing DerivedClass"); }
  ~DerivedClass()
    { puts ("Destructing DerivedClass"); }
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
  printf ("Calling do_something()\n");
  DerivedClass derived_class;

  throw (ErrorString ("Tossed my cookies in do_something"));
}


int
main()
{
  INIT_EXCEPTIONS();

  printf ("Sizeof Destructable = %d\n", (int)sizeof (Destructable));

  puts ("Testing destructor calling sequence during throw.");

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
      puts ("Caught a vanilla exception.");
    }
  end_try;
}
