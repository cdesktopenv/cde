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
// $XConsortium: tst_strstream.C /main/4 1996/08/21 15:55:44 drk $


#include <stdio.h>
#include "utility/c_stringstream.h"

tst_ostringstream()
{
   char buf[1024];
   ostringstream os(buf, 1024);
   os << "istream : virtual public ios.";
   fprintf(stderr, "buf=%s\n", buf);
}

tst_istringstream()
{
   char* buf = "303.0";
   istringstream is(buf, strlen(buf));

   int c = is.get() ;
   fprintf(stderr, "c=%c\n", c);
   is.putback(c) ;

   int x; char ch;
   is >> x; fprintf(stderr, "x=%d\n", x);
   is >> ch; fprintf(stderr, "ch=%c\n", ch);
   is >> x; fprintf(stderr, "x=%d\n", x);
}

main()
{
   tst_ostringstream();
   tst_istringstream();
}
