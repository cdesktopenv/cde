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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
// $XConsortium: tst_streambuf.C /main/4 1996/08/21 15:55:40 drk $

#include <iostream>
using namespace std;

#include "utility/debug.h"
#include "utility/c_charbuf.h"


streambuf_test1()
{
   MESSAGE(cerr, "TEST 1");
   char buf[5];
   charbuf sb(buf, 5);

   sb.put('a');
   sb.put('b');
   sb.put('c');

   MESSAGE(cerr, "examine char a:");
   debug(cerr, (char)sb.examine());

   MESSAGE(cerr, "get char a:");
   int c = sb.get();
   debug(cerr, (char)c);

   sb.put('d');
   sb.put('e');

   sb.putback(c);
   MESSAGE(cerr, "putback char a:");
   debug(cerr, (char)c);

   MESSAGE(cerr, "get char a - e:");
   debug(cerr, (char)sb.get());
   debug(cerr, (char)sb.get());
   debug(cerr, (char)sb.get());
   debug(cerr, (char)sb.get());
   debug(cerr, (char)sb.get());
}

streambuf_test2()
{
   MESSAGE(cerr, "TEST 2");
   char buf[5];
   charbuf sb(buf, 5);

   sb.put(0);
   sb.put(1);

   MESSAGE(cerr, "get 0:");
   debug(cerr, sb.get());
   MESSAGE(cerr, "get 1:");
   debug(cerr, sb.get());
   MESSAGE(cerr, "get -1:");
   debug(cerr, sb.get());

   sb.putback(2);
   MESSAGE(cerr, "get 2:");
   debug(cerr, sb.get());
}

streambuf_test3()
{
   MESSAGE(cerr, "TEST 3");
   char buf[5];
   charbuf sb(buf, 5);

   MESSAGE(cerr, "return 0:");
   debug(cerr, sb.put(0));
   MESSAGE(cerr, "return 0:");
   debug(cerr, sb.put(1));
   MESSAGE(cerr, "return 0:");
   debug(cerr, sb.put(2));
   MESSAGE(cerr, "return 0:");
   debug(cerr, sb.put(3));
   MESSAGE(cerr, "return 0:");
   debug(cerr, sb.put(4));
   MESSAGE(cerr, "return -1:");
   debug(cerr, sb.put(5));
   MESSAGE(cerr, "return -1:");
   debug(cerr, sb.put(6));
}

streambuf_test4()
{
   MESSAGE(cerr, "TEST 4");
   char buf[5];
   charbuf sb(buf, 5);

   debug(cerr, sb.putback(0));
   debug(cerr, sb.putback(1));
   debug(cerr, sb.putback(2));
   debug(cerr, sb.putback(3));
   debug(cerr, sb.putback(4));

   MESSAGE(cerr, "get 4:");
   debug(cerr, sb.get());
   MESSAGE(cerr, "get 3:");
   debug(cerr, sb.get());
   MESSAGE(cerr, "get 2:");
   debug(cerr, sb.get());
   MESSAGE(cerr, "get 1:");
   debug(cerr, sb.get());
   MESSAGE(cerr, "get 0:");
   debug(cerr, sb.get());
}

main()
{
   streambuf_test1();
   streambuf_test2();
   streambuf_test3();
   streambuf_test4();
}
