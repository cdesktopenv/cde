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
// $XConsortium: test18.C /main/4 1996/09/27 19:03:20 drk $
#include <stdio.h>
#include "Exceptions.hh"
#include <signal.h>

/*

  This test case illustrates a problem that isn't handled by the current
  exceptions library.  BaseClass contains two Destructable members whose
  constructor throws an exception.  When this happens, the exceptions
  library calls the destructors of all members, constructed or not.  This
  means that the Destructable destructor of m2 is called even though it
  wasn't constructed.  This is probably just dumb luck, because nothing
  says that the virtual table of m2 should already have the pointer to
  the destructor filled in.  For some reason it does.  It seems like the
  compiler should be smarter about this and maybe not call it, but we're
  kind of stuck.

  One way to handle this would be to remember all Destructable objects
  that are constructed on the Jump_Environment stack, but only destroy
  those that don't have the f_registered (or f_destroy) flag set.  Then
  we can tell if an object has been constructed by comparing it's address
  to the object on the top of the stack.  Actually, we'd need two separate
  stacks because with one stack the members would have been removed before
  we get to the object that contains them.  We're not going to implement
  this because throwing an exception from a constructor is too dangerous
  with the current language language definition anyhow.

  16:56 22-May-93 DJB

 */

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
  MemberClass m1, m2;
  BaseClass()
    : m1("m1"), m2("m2")
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

  puts ("Test of throw from multiple member constructor.");

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
