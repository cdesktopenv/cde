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
// $XConsortium: testsym.cc /main/3 1996/06/11 17:10:33 cde-hal $
#include "SymTab.h"


SymbolTable *gSymTab = new SymbolTable;
SymbolTable *gElemSymTab = 0;

main()
{


  Symbol a = gSymTab->intern("a");
  Symbol b = gSymTab->intern("b");
  Symbol a2 = gSymTab->intern("a");

  cout << (a == a) << endl;
  cout << (a == b) << endl;
  cout << (a == a2) << endl;

  cout << "ids = " << endl;
  cout << a.name() << "\t" << a.id() << endl;
  cout << b.name() << "\t" << b.id() << endl;
  cout << a2.name() << "\t" << a2.id() << endl;


}
