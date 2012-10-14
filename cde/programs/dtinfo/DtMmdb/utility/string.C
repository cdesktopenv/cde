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
// $XConsortium: string.C /main/5 2012/09/18 22:56:32 xxx $

#include "utility/c_string.h"
#include "utility/c_charbuf.h"
#include <string.h>

string::string(char* str)
{
   sbuf = new charbuf(str, strlen(str), 1);
}

string::string(char* str, int size)
{
   sbuf = new charbuf(str, size);
}

string::~string()
{
}

char* string::c_str()
{
   char* x = sbuf -> get_buf();
// x[pcount()] = 0;
   return x;
}

int string::size()
{
   char* x = sbuf -> get_buf();
   return(strlen(x));
}
