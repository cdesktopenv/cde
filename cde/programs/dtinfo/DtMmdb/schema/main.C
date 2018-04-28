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
// $XConsortium: main.cc /main/3 1996/06/11 17:32:36 cde-hal $

#include "utility/funcs.h"
#include "schema/object_dict.h"

int main( int argc, char** argv )
{
   INIT_EXCEPTIONS();

    try
   {
    if ( argc != 4 ) {
	MESSAGE(cerr, "usage: def_name db_path base_nm\n");
	exit(1);
    }

    object_dict od(argv[1], argv[2], argv[3]);
   }

   catch (mmdbException &,e)
   {
      cerr << "Exception msg: " << e << "\n";
#ifdef DEBUG
      abort();
#else
      ok = -1;
#endif

   }
   end_try;

    exit( 0 );
}

