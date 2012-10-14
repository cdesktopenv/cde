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
 * $XConsortium: info_base.C /main/5 1996/09/04 01:37:05 cde-hal $
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


#ifndef C_API
#include <sstream>
using namespace std;
#endif
#include "api/info_base.h"
#include "compression/abs_agent.h"

info_base::info_base(object_dict& obj_dict, 
		     char** set_name_list, char** list_name_list,
                     const char* base_dir, const char* base_nm,
                     const char* base_ds, const char* base_uid, 
		     const char* base_locale,
		     const mm_version& v
		    )
: base(&obj_dict, 
       set_name_list, list_name_list, 
       base_dir, base_nm, base_ds, base_uid
      ), f_v(v)
{
/*
debug(cerr, base_dir);
debug(cerr, base_nm);
debug(cerr, base_ds);
*/
   if (base_locale)
     strcpy(info_base_locale, base_locale);
   else
     *info_base_locale = 0;

   char* nm ;
   int i;
 
//////////////////////
// sets
//////////////////////
   info_base_set_ptrs = new cset_handlerPtr[num_cset_ptrs];

   for ( i=0; i<num_cset_ptrs; i++ ) {

      nm = form("%s.%s", base_nm, info_base_set_names[i]);

      mtry {
         info_base_set_ptrs[i] = (cset_handler*)
              (f_obj_dict -> get_handler(nm));
      }
      mcatch_any()
          {
         info_base_set_ptrs[i] = 0;
          }
      end_try;
   }

//////////////////////
// lists
//////////////////////
   info_base_list_ptrs = new dl_list_handlerPtr[num_list_ptrs];

   for ( i=0; i<num_list_ptrs; i++ ) {

      nm = form("%s.%s", base_nm, info_base_list_names[i]);

      mtry {
         info_base_list_ptrs[i] = (dl_list_handler*)
             obj_dict.get_handler(nm);
      }
      mcatch_any()
          {
         info_base_list_ptrs[i] = 0;
          }
      end_try;
   }

   MESSAGE(cerr, form("info base %s in %s available.", base_name, base_dir));
}

info_base::~info_base()
{
   delete info_base_list_ptrs;
   delete info_base_set_ptrs;
}

int info_base::get_set_pos(const char* set_nm)
{
   char* nm = 0;
   for ( int i=0; i<num_cset_ptrs; i++ ) {
      if ( strcmp(set_nm,  info_base_set_names[i]) == 0 ) 
         return i;
   }

   return -1;
}

cset_handlerPtr info_base::get_set(const char* set_nm)
{
//MESSAGE(cerr, "in info_base::get_set()");
   return get_set(get_set_pos(set_nm));
}

cset_handlerPtr info_base::get_set(int i)
{
   if ( !INRANGE(i, 0, num_cset_ptrs-1) )
      throw (boundaryException(0, num_cset_ptrs-1, i));

   return info_base_set_ptrs[i];
}


///////////////////
// funcs about list
///////////////////
int info_base::get_list_pos(const char* list_nm)
{
   for ( int i=0; i<num_list_ptrs; i++ ) {
      if ( strcmp(list_nm,  info_base_list_names[i]) == 0 ) 
         return i;
   }

   return -1;
}

dl_list_handlerPtr info_base::get_list(const char* list_nm)
{
   return ( get_list(get_list_pos(list_nm)) );
}

dl_list_handlerPtr info_base::get_list(int i)
{
   if ( !INRANGE(i, 0, num_list_ptrs-1) ) {
      throw(boundaryException(0, num_list_ptrs-1, i));
   }

   return info_base_list_ptrs[i];
}

int info_base::num_of_docs()
{
// the implementation is temp. It should take
// a hint as where to obtain the information.
// like from a set named so and so.

   cset_handler* x = get_set("doc");
   if (x) {
      return (*x) -> count();
   } else {
      return 0;
   }
}

////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////

Iterator::Iterator( handler* x, c_code_t y) :
   collection_hd(x), instance_c_code(y), ind(0)
{
}

Iterator::~Iterator()
{
}

Iterator::operator void*()
{ 
   return ( ind == 0 ) ? 0 : (void*)1; 
}



Iterator* info_base::first(char* col_nm, c_code_t code)
{
   handler* x = get_set(col_nm);

   if ( x == 0 ) {
      handler* x = get_list(col_nm);
   }

   if ( x == 0 ) {
      throw(stringException("unknown collection name"));
   }

   page_storage *s = (page_storage*)(x -> its_store());

   if ( s == 0 ) {
      throw(stringException("collection has no store"));
   }

   if ( s -> my_oid().ccode() != PAGE_STORAGE_CODE ) {
      throw(stringException("non page store no supported"));
   }

   Iterator* it = new Iterator(x, code);

   it -> ind = s -> first_loc();

   if ( managers::template_mgr -> peek_slot(s, it->ind) != code ) {
      this -> next(*it);
   }

   return it;
}

oid_t info_base::get_oid(const Iterator& it)
{
   page_storage *s = (page_storage*)( it.collection_hd -> its_store() );

   root *r = 0;
   managers::template_mgr -> init_obj(s, it.ind, r);
   if (r == 0)
       throw(stringException("null root pointer"));

   return r -> my_oid();
}

void info_base::next(Iterator& it)
{
   page_storage *s = (page_storage*)( it.collection_hd -> its_store());

   while ( s -> seek_loc( it.ind, positive, spointer_t::IS_OBJECT ) == true ) {
     if ( managers::template_mgr -> peek_slot(s, it.ind) == it.instance_c_code ) {
        return;
     }
   }

   it.ind = 0;
}

int stdin_sgml_data_getchar(unsigned char* buf, int max_sz)
{
   static int remain_chars = 0;
   static int chars_to_read;
   static char loc_buf[LBUFSIZ];

   if ( remain_chars == 0 ) {
      if ( fgets(loc_buf, LBUFSIZ, stdin) == NULL )
         return 0;

      if ( fgets(loc_buf, LBUFSIZ, stdin) == NULL )
         throw(stringException("no locator line"));

      if ( fscanf(stdin, "%d", &remain_chars) != 1 )
         throw(stringException("sgml_data_getchar(): fscanf() failed"));

      if ( getc(stdin) != '\t' ) {
         debug(cerr, remain_chars);
         throw(stringException("'\\t' expected"));
      }

   }

   chars_to_read = MIN(max_sz, remain_chars);

   if ( fread((char*)buf, 1, chars_to_read, stdin) != chars_to_read ) {
      throw(stringException("sgml_data_getchar(): fread() failed"));
   }

   remain_chars -= chars_to_read;

   if ( remain_chars == 0 ) {
      if ( getc(stdin) != '\n' )
         throw(stringException("'\\n' expected"));
   }

/*
for ( int i=0; i<chars_to_read; i++ ) {
   cerr << buf[i];
}
MESSAGE(cerr, "\n");
debug(cerr, max_sz);
debug(cerr, chars_to_read);
*/

   return chars_to_read;
}

int stdin_ps_data_getchar(unsigned char* buf, int max_sz)
{
   return fread((char*)buf, 1, max_sz, stdin);
}

extern int sgmllex();
extern int pslex();

void info_base::build_dict(char* agent_name)
{
   compress_agent_handler* x = (compress_agent_handler*)
                  (f_obj_dict -> get_handler(agent_name));

   if ( x == 0 )
      throw(stringException("info_base::build_dict(): unknown compress agent"));

   if ( strstr(agent_name, ".sgml.") != NULL ) {
      (*x) -> build_dict(sgmllex, stdin_sgml_data_getchar);
   } else
   if ( strstr(agent_name, ".ps.") != NULL ) {
      (*x) -> build_dict(pslex, stdin_ps_data_getchar);
   } else
      throw(stringException("info_base::build_dict(): unknown compress target"));
}

