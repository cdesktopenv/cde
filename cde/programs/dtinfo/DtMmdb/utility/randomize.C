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
// $XConsortium: randomize.cc /main/3 1996/06/11 17:38:40 cde-hal $

#include "utility/randomize.h"

randomize::randomize(int sd) : rdn(sd)
{
}

void randomize::scramble(buffer& original)
{
   int bytes = original.content_sz();
   char* buf = original.get_base();

   for ( int i=0; i<bytes-1; i++ ) 
      char_swap(buf[i], buf[rdn.rand() % ( bytes - i ) + i]);
}   

void randomize::restore(buffer& scrambled)
{
   int bytes = scrambled.content_sz();
   char* buf = scrambled.get_base();

   int *pos = new int[bytes-1];

   int i;
   for ( i=0; i<bytes-1; i++ ) {
      pos[i] = rdn.rand() % ( bytes - i ) + i;
   }

   for ( i=bytes-2; i>=0; i-- ) {
      char_swap(buf[i], buf[pos[i]]);
   }

   delete pos;
}
