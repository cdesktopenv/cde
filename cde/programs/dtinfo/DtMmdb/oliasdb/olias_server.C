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
// $XConsortium: olias_server.cc /main/3 1996/06/11 17:30:16 cde-hal $
/*	Copyright (c) 1994 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

/*
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

#include "oliasdb/olias_server.h"

#ifdef DEMO_VERSION
void demo_version_check(info_lib* infolibptr)
{
#if 1
   return;
#else
   static char* demo_key = "DBzjcy94aK";
   static char* demo_short_title = "02192ec285a9349ae85e3cc7";
   static char* demo_long_title  = "1e87fe1d9c927e9e24be85e3cc733598faeda88101ecb253";

   if ( infolibptr == 0 ) 
     return;

   info_base* x = 0;
   int i = infolibptr -> first();

   while ( i != 0 ) {
      x = (*infolibptr)(i);

      if ( x && (*(x  -> get_set(NODE_SET_POS))) -> count() > 0 ) 
      {

         node_smart_ptr n(x, demo_key);
   
         if ( strcmp(n.locator(), demo_key) != 0 ||
              strcmp(n.short_title(), demo_short_title) != 0 ||
              strcmp(n.long_title(), demo_long_title) != 0 
         ) {
              debug(cerr, n.locator());
              debug(cerr, n.short_title());
              debug(cerr, n.long_title());
              throw(demoException(x -> get_base_path(), x -> get_base_name()));
         }
      }

      infolibptr -> next(i);
   }
#endif
}
#endif

olias_server::olias_server(
            Boolean delayed_infolib_init,
            char* selected_base_name,
            char* x_info_lib_path
                          ) :
server(x_info_lib_path)
{
   managers::template_mgr -> insert_template(&dlp_template);
   managers::template_mgr -> insert_template(&doc_template);
   managers::template_mgr -> insert_template(&graphic_template);
   managers::template_mgr -> insert_template(&locator_template); 
   managers::template_mgr -> insert_template(&node_template); 
   managers::template_mgr -> insert_template(&toc_template); 
   managers::template_mgr -> insert_template(&mark_template); 
   managers::template_mgr -> insert_template(&pref_template); 
   managers::template_mgr -> insert_template(&stylesheet_template); 

///////////////////////////////
//
///////////////////////////////

   info_base_set_names = new charPtr[7];

   info_base_set_names[0] = NODE_SET_NAME;
   info_base_set_names[1] = TOC_SET_NAME;
   info_base_set_names[2] = LOCATOR_SET_NAME;
   info_base_set_names[3] = GRAPHIC_SET_NAME,
   info_base_set_names[4] = DOC_SET_NAME;
   info_base_set_names[5] = STYLESHEET_SET_NAME;
   info_base_set_names[6] = 0;

   info_base_list_names = new charPtr[2];
   info_base_list_names[0] = DLP_LIST_NAME;
   info_base_list_names[1] = 0;

   if ( delayed_infolib_init == false ) {
      infolibptr = new info_lib(
                       info_base_set_names, info_base_list_names,
                       info_lib_dir, selected_base_name   
                    );

#ifdef DEMO_VERSION
      demo_version_check(infolibptr);
#endif

   } else
      infolibptr = 0;

}

olias_server::~olias_server()
{
}

void olias_server::init_infolib()
{
   if ( infolibptr == 0 ) {
      infolibptr = new info_lib(
                       info_base_set_names, info_base_list_names,
                       info_lib_dir, 0);

#ifdef DEMO_VERSION
      demo_version_check(infolibptr);
#endif
   }
}

info_base* olias_server::get_infobase(const char *locator_string, enum TestSelector sel)
{

   if ( locator_string == 0 )
      return 0;


   info_base* ib = 0;

   int ind = infolibptr -> first();

   while ( ind ) {

      ib =  (*infolibptr)(ind);

      if (ib==0)
         throw(stringException("null info_base ptr"));

      try { // since an infobase may not have any graphics, we catch
            // any exceptions there and try next infobase.

         switch (sel) {
          case LOC:
   	   {
            locator_smart_ptr loc(ib, locator_string);
   
//fprintf(stderr, "inside-loc-string=%s\n", loc.inside_node_locator_str());
//fprintf(stderr, "loc-string=%s\n", locator_string);
            if ( strcmp( loc.inside_node_locator_str(), locator_string) == 0 ) {
               return ib;
            }
   
           }
          case GRA:
   	   {
            graphic_smart_ptr graphic(ib, locator_string);
   
            if ( strcmp( graphic.locator(), locator_string) == 0 ) {
               return ib;
            }
           }
         }
      }

      catch (mmdbException &,e)
      {
      } end_try;


      infolibptr -> next(ind);
   }

   return 0;
}

info_base** olias_server::get_infobases(char **locator_strings, int count, enum TestSelector sel)
{
   info_base** ibs = new info_basePtr[count];
   for ( int i=0; i<count; ibs[i++] = 0 );

   info_base* ib = 0;

   int ind = infolibptr -> first();

   while ( ind ) {

      ib =  (*infolibptr)(ind);

      if (ib == 0)
         throw(stringException("null info_base ptr"));

      for ( i=0; i<count; i++ ) {

         try {
   
            if ( locator_strings[i] && ibs[i] == 0 ) { 	
              switch (sel) {
               case LOC:
   	      {
                  locator_smart_ptr loc(ib, locator_strings[i]);
   
                  if ( strcmp( loc.inside_node_locator_str(), 
   			    locator_strings[i]) == 0 
   		  )
                     ibs[i] = ib;
                 }
   	      break;
   
               case GRA:
   	      {
                  graphic_smart_ptr graphic(ib, locator_strings[i]);
   
                  if ( strcmp( graphic.locator(), 
   			    locator_strings[i]) == 0 
   		  )
                     ibs[i] = ib;
                 }
   	      break;
              }
            }
         }

         catch (mmdbException &,e)
         {
         } end_try;

      }

      infolibptr -> next(ind);
   }

   return ibs;
}

