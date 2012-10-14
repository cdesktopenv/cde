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
// $XConsortium: prolatex.C /main/3 1996/06/11 17:38:26 cde-hal $

#include <stdlib.h> 
#include <string.h> 
#include <iostream>
using namespace std;

#define BUFSIZ 1000

main()
{
   char buf[BUFSIZ];
   while ( cin.getline(buf, BUFSIZ) ) {
       int loc = strlen("\\epsffile{") ; 
       char c = buf[loc];
       buf[loc] = '\0';
       if ( strcmp(buf, "\\epsffile{") == 0 ) {
         cout << buf;
         cout << getenv("PWD");
         cout << "/";
         buf[loc] = c;
         cout << buf + loc << "\n";
       } else {
         buf[loc] = c;
         cout << buf << "\n";
       }
   }
   return 0;
}
