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
/*
 * $XConsortium: main.cc /main/5 1996/07/18 15:24:26 drk $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */


#include "api/utility.h"
#include "oliasdb/mmdb.h"
#include "oliasdb/olias_funcs.h"
#include "oliasdb/olias_test.h"

void usage(const char* command)
{
   cerr << "unknown operation type: " << command << "\n";
}

int select_and_process( int argc, char** argv )
{
   int ok = -1;

   OLIAS_DB mmdb;
   info_lib* infolib_ptr = 0;

   if ( argc < 2 ) {
      ok = -1;
   } else {
      if ( strcmp(argv[1], "define") == 0 ) {

         infolib_ptr = mmdb.openInfoLib(getenv("MMDB_PATH"));

         if ( infolib_ptr == 0 ||
              infolib_ptr->define_info_base(argv[3],argv[4],argv[2]) == false
            )
            ok = -1;
         else
            ok = 0;
         
      } else 
      if ( strcmp(argv[1], "build_dict") == 0 ) {

// args: build_dict $base $compress_agent_name

         infolib_ptr = mmdb.openInfoLib(getenv("MMDB_PATH"), argv[2]);  
         info_base* base_ptr = infolib_ptr -> get_info_base(argv[2]);

         if ( base_ptr == 0 )
            throw(stringException(form("unknown base: %s", argv[2])));

         base_ptr -> build_dict(argv[3]);

         ok = 0;

      } else 
   
      if ( strcmp(argv[1], "stdin_load") == 0 ) {
         if ( argc == 4 ) {
            infolib_ptr = 
		mmdb.openInfoLib(getenv("MMDB_PATH"), argv[2]);  
            info_base* base_ptr = infolib_ptr -> get_info_base(argv[2]);

            ok = load( base_ptr, argv[3] );
         } else {
            MESSAGE(cerr,
              "stdin_load args: stdin_load base_nm obj_name");
         }
      } else

      if ( strcmp(argv[1], "simple_mixed_load") == 0 ) {
         if ( argc != 3 ) {
            MESSAGE(cerr, "mixed_load args: mixed_load base_name");
         } else {
            infolib_ptr = 
		mmdb.openInfoLib(getenv("MMDB_PATH"), argv[2]);  
            info_base* base_ptr = infolib_ptr -> get_info_base(argv[2]);

            ok = _load_mixed_objects_from_cin(base_ptr);
         }
      } else

      if ( strcmp(argv[1], "mixed_load") == 0 ) {
         if ( argc != 3 ) {
            MESSAGE(cerr, "mixed_load args: mixed_load base_name");
         } else {
            infolib_ptr = 
		mmdb.openInfoLib(getenv("MMDB_PATH"), argv[2]);  
            info_base* base_ptr = infolib_ptr -> get_info_base(argv[2]);

            ok = load_mixed_objects_from_cin(base_ptr);
         }
      } else

      if ( (ok = select_debug_routine(argc, argv, mmdb)) == 2 )
      {
         usage(argv[1]);
      }
   }

   return (ok);
}

main( int argc, char** argv )
{
   //quantify_clear_data();
   INIT_EXCEPTIONS();

   int ok;

   mtry
   {
      ok = select_and_process( argc, argv );
   } 

   mcatch (mmdbException &,e)
   {
      cerr << "Exception msg: " << e << "\n";
#ifdef DEBUG
      //abort();
#else
      ok = -1;
#endif

   }
   end_try;

   exit(ok);
}

