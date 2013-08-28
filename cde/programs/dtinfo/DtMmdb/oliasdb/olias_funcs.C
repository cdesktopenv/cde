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
 * $XConsortium: olias_funcs.cc /main/5 1996/07/18 14:48:42 drk $
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


#include "oliasdb/olias_funcs.h"
#include "oliasdb/asciiIn_filters.h"
#include "oliasdb/olias_consts.h"
#include "oliasdb/node_hd.h"
#include "oliasdb/doc_hd.h"
#include "oliasdb/dlp_hd.h"


/*
#define BB_STRING "%%BoundingBox:"
int bb_string_leng = strlen(BB_STRING);
*/


/////////////////////////////////////////////////
// load sgml node data. This will cluster them. 
/////////////////////////////////////////////////

//extern char base_str[PATHSIZ];

struct map_record {
   char* col_name;
   c_code_t instance_class_code;
   handler* collection_hd;
};

struct map_record set_map[]=
{ 
   {(char*)"loc", LOCATOR_CODE, 0},
   {(char*)"toc", TOC_CODE, 0 },
   {(char*)"doc", DOC_CODE, 0 },
   {(char*)"graphic", GRAPHIC_CODE, 0 },
//   {(char*)"stylesheet", STYLESHEET_CODE, 0 },
}; 

struct map_record list_map[]=
{ 
   {(char*)"dlp", DLP_CODE, 0},
}; 

//#define SET_MAP_SZ 5
#define SET_MAP_SZ 4
#define LIST_MAP_SZ 1

/////////////////////////////////////////////////
// load mixed stream of infobase data from stdin.
/////////////////////////////////////////////////
void insert_to_collection(c_code_t ccode, istream& in)
{
    enum coll_type { VOID, SET, LIST };

    coll_type obj_type = VOID;

    abs_storage* store = 0;
//  handler* hd = 0;

    int i;
    for ( i = 0; i<SET_MAP_SZ ; i++ ) {
      if ( set_map[i].instance_class_code == ccode ) {
         obj_type = SET;
         store = set_map[i].collection_hd -> its_store();
//       hd = set_map[i].collection_hd;
         break;
      }
    }

    if ( obj_type == VOID ) 
    for ( i = 0; i<LIST_MAP_SZ ; i++ ) {
       if ( list_map[i].instance_class_code == ccode ) {
         obj_type = LIST;
          store = list_map[i].collection_hd -> its_store();
//        hd = set_map[i].collection_hd;
          break;
       }
    }

    if ( store == 0 ) {
       throw(stringException(form("unknown class code %d", ccode)));
    }

    handler* root_hd_ptr = new handler(ccode, store);

    (*root_hd_ptr) ->asciiIn(in);
    root_hd_ptr -> commit();

    switch ( obj_type ) {
       case SET:
         (*(cset_handlerPtr)(set_map[i].collection_hd)) ->
             insert_object(*root_hd_ptr);
         break;
       case LIST:
         (*(dl_list_handlerPtr)(list_map[i].collection_hd)) -> 
              insert_as_tail(*(dl_list_cell_handler*)root_hd_ptr);
         break;
       default:
         cerr << "bad ccode = " << ccode << "\n";
         throw(stringException("bad ccode"));
   }

   delete root_hd_ptr;

}

static char locator[LBUFSIZ];

void sgml_content_load(info_base* x, istream& in)
{
   in.getline(locator, LBUFSIZ);

   char* locator_string = strchr(locator, '\t') + 1;
   if ( locator_string == (char*)1 )
      throw(formatException("bad sgml data format"));

   node_smart_ptr y(x, locator_string);

   if ( y.update_data(in) == false ) 
      throw(formatException("can't update sgml data"));
   
}

void _connectNodeToDoc(info_base* x_base);

int _load_mixed_objects_from_cin(info_base* base_ptr)
{
   return _load_mixed_objects(base_ptr, cin);
}

int _load_mixed_objects(info_base* base_ptr, istream& in)
{
   int i;
   for ( i = 0; i<SET_MAP_SZ ; i++ ) {
      cset_handlerPtr x = base_ptr -> get_set(set_map[i].col_name);

      if (x==0)
         throw(stringException("null cset_handler pointer"));

      (*x) -> batch_index_begin();
      set_map[i].collection_hd = x;
   }

   for ( i = 0; i<LIST_MAP_SZ ; i++ ) {
      dl_list_handlerPtr x = base_ptr -> get_list(list_map[i].col_name);

      if (x==0)
         throw(stringException("null cset_handler pointer"));

      (*x) -> batch_index_begin();
      list_map[i].collection_hd = x;
   }

   
   char ccode_buf[LBUFSIZ];
   int ccode;

   while ( in.getline(ccode_buf, LBUFSIZ) ) {
      sscanf(ccode_buf, "%u", &ccode);
//debug(cerr, ccode);

      if ( ccode == SGML_CONTENT_CODE )
         sgml_content_load(base_ptr, in);
      else
         insert_to_collection(ccode, in);
   }

   for ( i = 0; i<SET_MAP_SZ ; i++ ) {
      cset_handlerPtr x = (cset_handlerPtr)(set_map[i].collection_hd);
      (*x) -> batch_index_end();
   }

   for ( i = 0; i<LIST_MAP_SZ ; i++ ) {
      dl_list_handlerPtr x = (dl_list_handlerPtr)(list_map[i].collection_hd);
      (*x) -> batch_index_end();
   }

   return 0;
}

int load_mixed_objects_from_cin(info_base* base_ptr)
{
   return load_mixed_objects(base_ptr, cin);
}

int load_mixed_objects(info_base* base_ptr, istream& in)
{
   _load_mixed_objects(base_ptr, in);
   _connectNodeToDoc(base_ptr);
   return 0;
}

void _connectNodeToDoc(info_base* x_base)
{
   int docs = x_base -> num_of_docs();

   doc_smart_ptr* doc = 0;

   node_smart_ptr* first_node = 0, *node = 0;
   dlp_smart_ptr* x = 0, *dlp_cell = 0;

   for ( int i=0; i<docs; i++ ) {
      doc = new doc_smart_ptr(x_base, i+1);
/*
debug(cerr, doc -> its_oid());
debug(cerr, doc -> long_title());
*/

      first_node = new node_smart_ptr(x_base, doc -> locator_id());

      dlp_cell = new dlp_smart_ptr(x_base, first_node -> its_oid());

      while ( dlp_cell ) {

         
//debug(cerr, dlp_cell -> node_id());

         node = new node_smart_ptr(x_base, dlp_cell -> node_id());
//debug(cerr, node -> doc_id());
         node -> update_doc_id(doc -> its_oid());
//debug(cerr, node -> doc_id());
         delete node;
   
         x = dlp_cell -> next();

         delete dlp_cell;

         dlp_cell = x;
      }

      delete doc;
      delete first_node;
   }
}


void connectNodeToDoc(info_lib* x_lib)
{
   if ( x_lib == 0 )
      throw(stringException("connectNodeToDoc: can't find infolib"));

   info_base* x_base = 0;

   long ind = x_lib -> first();

   while ( ind ) {
      x_base = (*x_lib)(ind);

      if ( x_base == 0 )
         throw(stringException("connectNodeToDoc: null base pointer"));

      _connectNodeToDoc(x_base);

      x_lib -> next(ind);
   };
}



