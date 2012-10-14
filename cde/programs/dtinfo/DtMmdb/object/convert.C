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
// $XConsortium: convert.cc /main/3 1996/06/11 17:23:53 cde-hal $

#include <iostream>
using namespace std;

/*****************************************/
//Note: convert node.dat file to new format
/*****************************************/

#define BUFSIZ 100

main()
{
   int sz;
   int buf_sz = BUFSIZ;
   char *buf = new char[BUFSIZ];

   while ( cin.getline(buf, BUFSIZ) ) { // read in '100L'
       cout << "102	0\n";
       cout << "4\n";

       for ( int i=0; i<4; i++ ) {
          cin >> sz;
/*
cout << "***";
cout << sz;
cout << "***\n";
*/
          cin.get();
   
          if ( buf_sz < sz ) {
             buf_sz = sz;
             buf = new char[buf_sz];
          }
   
          cin.read(buf, sz); 

          cin.get(); // skip '\n'
          cout << "5\t0" << "\n";
          cout << sz << "\t";
          cout.write(buf, sz); 

          cout << "\n";
       }
   }

   delete buf;
}
