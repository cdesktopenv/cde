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
 * $XConsortium: template_mgr.C /main/10 1996/10/03 18:50:20 drk $
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


#include "mgrs/template_mgr.h"

#include "object/root.h"
#include "object/oid.h"
#include "object/integer.h"
#include "object/pstring.h"
#include "object/compressed_pstring.h"
#include "object/tuple.h"
#include "object/short_list.h"
#include "object/cset.h"
#include "index/mphf_index.h"
#include "index/btree_index.h"
#include "index/dyn_memory_index.h"
#include "index/dyn_disk_index.h"
#include "object/dl_list_cell.h"
#include "object/dl_list.h"
#include "compression/zip.h"
#include "compression/huffman.h"
#include "compression/lzss.h"

#include "api/transaction.h"

extern memory_pool g_memory_pool;

#ifdef NMEMORY_MAPPED
#define CLASS_CODE_BYTES ll4(sizeof(c_code_t))
#else
#define CLASS_CODE_BYTES sizeof(c_code_t)
#endif

#define TEMP_OBJ_NUMS 18
static rootPtr template_obj_table[TEMP_OBJ_NUMS] ;


#ifdef C_API
buffer* cdr_io_buf_ptr = 0;
#define cdr_io_buf (*cdr_io_buf_ptr)
#else
static buffer cdr_io_buf(LBUFSIZ);
#endif


template_mgr_t::template_mgr_t() : v_template_objs(32801, 20)
{
#ifdef C_API
   if ( cdr_io_buf_ptr == 0 )
      cdr_io_buf_ptr = new buffer(LBUFSIZ); 
#endif

   template_obj_table[0] = ::new compressed_pstring;
   template_obj_table[1] = ::new oid;
   template_obj_table[2] = ::new integer;
   template_obj_table[3] = ::new pstring;
   template_obj_table[4] = ::new tuple;
   template_obj_table[5] = ::new oid_list;
   template_obj_table[6] = ::new fast_mphf;
   template_obj_table[7] = ::new inv_lists;
   template_obj_table[8] = ::new mphf_index;
   template_obj_table[9] = ::new btree_index;
   template_obj_table[10] = ::new dyn_memory_index;
   template_obj_table[11] = ::new dyn_disk_index;
   template_obj_table[12] = ::new cset;
   template_obj_table[13] = ::new short_list;
   template_obj_table[14] = ::new dl_list_cell;
   template_obj_table[15] = ::new dl_list;
   template_obj_table[16] = ::new huff;
   template_obj_table[17] = ::new lzss;

   for ( int i=0; i<TEMP_OBJ_NUMS; i++ ) {
      insert_template(template_obj_table[i]);
   }
}

template_mgr_t::~template_mgr_t()
{
   for ( int i=0; i<TEMP_OBJ_NUMS; i++ ) {
      ::delete template_obj_table[i];
   }

#ifdef C_API
   delete cdr_io_buf_ptr;
#endif
}


root* template_mgr_t::look_up(c_code_t c_code)
{
   data_t key_data(c_code, 0);
   if ( v_template_objs.member(key_data) == true &&
        long(key_data.dt) != -1 ) {
      return (root*)(key_data.dt);
   } else {
      return 0;
  }
}


c_code_t 
template_mgr_t::_peek_slot(abs_storage* store, mmdb_pos_t pos, 
                           char*& z, int& len)
{
//debug(cerr, pos);

   if ( store == 0 || store -> OK() == false ) {
      throw(stringException("template_mgr_t::_peek_obj() store in bad shape"));
   }
      
//first read in the oid_t string.

   if ( store -> get_str_ptr(pos, z, len) != 0 ) {
      throw(stringException("_peek_obj(): can't get oid_t string"));
   }

//debug(cerr, len);
/*cerr << "len=" << len << endl;
for (int i=0; i<len; i++)
   cerr << int(z[i]) << " ";
cerr << "\n";
*/


   if ( len < (int) CLASS_CODE_BYTES || z == 0 )
      throw(stringException("_peek_obj(): corrupted data"));

   c_code_t class_code; 
   memcpy((char*)&class_code, z, CLASS_CODE_BYTES);
//debug(cerr, class_code);
 
#ifdef PORTABLE_DB
   if ( store -> swap_order() == true ) {
//MESSAGE(cerr, "swap class code order");
      ORDER_SWAP_USHORT(class_code);
   }
#endif

//debug(cerr, class_code);

   return class_code;
}

c_code_t template_mgr_t::peek_slot(abs_storage* store, mmdb_pos_t pos)
{
   char* z = 0; int len = 0;
   c_code_t class_code = _peek_slot(store, pos, z, len);
   RESET_BIT(class_code, CDR_FLAG);
   return class_code;
}

/******************************************************/
// return an object in x. The object is fetched from
// store 'store' at location 'pos'.
// If the object has been previously deleted, an
// exception will be thrown. 
/******************************************************/
Boolean 
template_mgr_t::init_obj(abs_storage* store, mmdb_pos_t pos, root*& x)
{
//debug(cerr, store -> my_name());
//debug(cerr, pos);

   x = 0;

   char* z = 0; int len = 0;
   c_code_t class_code = _peek_slot(store, pos, z, len);
#ifdef DEBUG
   {
     char* dbg_char_ptr;
     int   dbg_int;
     store->get_str_ptr(pos, dbg_char_ptr, dbg_int);
     fprintf(stderr, "init_obj ptr=0x%lx len=%d\n", (long)dbg_char_ptr,dbg_int);
   }
#endif

   if ( class_code == 0 ) 
      throw(stringException("init_obj(): class code == 0 "));

   Boolean compacted_disk_rep = 
         ( BIT_TEST(class_code, CDR_FLAG) ) ? true : false;

//debug(cerr, int(compacted_disk_rep));

   RESET_BIT(class_code, CDR_FLAG);

//debug(cerr, class_code);
//if ( pos == 16385 ) {
//MESSAGE(cerr, "STOP:");
//}

   rootPtr object_template = look_up(class_code);

   if ( object_template == 0 ) {
      debug(cerr, class_code);
      debug(cerr, store -> my_name());
      throw(stringException("init_obj(): can't find object template"));
   }

   /*****************************************/
   // make sure that the string existing on 
   // the store is of sufficient length 
   /*****************************************/
   
   int obj_len = len - CLASS_CODE_BYTES;

   switch ( compacted_disk_rep ) {

     case true:
      {

////////////////////////////////////
// compute and cache the cdr size
////////////////////////////////////
       if ( object_template -> get_cdr_size() == 0 ) {
            cdr_io_buf.reset();
            object_template -> cdrOut(cdr_io_buf);
            object_template -> set_cdr_size(cdr_io_buf.content_sz());
       }


/////////////////
// safety check
/////////////////
#ifdef DEBUG
       int cdr_sz = object_template -> get_cdr_size();

       if ( obj_len != cdr_sz ) {
          debug(cerr, obj_len);
          debug(cerr, cdr_sz);
          debug(cerr, *(c_code_t*)z);
          throw(stringException("corrupted data"));
       }
#endif
       break;
      }

     case false:
       if ( obj_len != object_template -> mem_sizeof() ) {
          debug(cerr, obj_len);
          debug(cerr, object_template -> mem_sizeof());
          debug(cerr, *(c_code_t*)z);
          throw(stringException("corrupted data"));
       }
       break;
   }

    if ( g_transac ) {
        g_transac -> book(store);
    }

//debug(cerr, int(store));
//debug(cerr, pos);
//debug(cerr, class_code);

   persistent_info pinfo;

   pinfo.storage = store;
   pinfo.position = pos;
   pinfo.persistent = true;
   pinfo.old_object = true;
   pinfo.class_code = class_code;


#ifdef MEMORY_MAPPED
   x = object_template -> cast_to( z + CLASS_CODE_BYTES );
#else
   //char* p = g_memory_pool.alloc( object_template -> mem_sizeof());
   char* p = new char[object_template -> mem_sizeof()];

//MESSAGE(cerr, "template mgr: init case(), new char array");
//debug(cerr, (void*)p);

   switch ( compacted_disk_rep ) {

     case true:
       memset((char*)p, char(0), object_template -> mem_sizeof());
       break;
     case false:
       memcpy((char*)p, z + CLASS_CODE_BYTES, obj_len);
       break;
   }

   x = object_template -> cast_to( p );
#endif

   x -> init_persistent_info( &pinfo );

   switch ( compacted_disk_rep ) {
     case true:
       {
       buffer buf(0);
       buf.set_chunk(z+CLASS_CODE_BYTES, obj_len);
       buf.set_content_sz(obj_len);

#ifdef PORTABLE_DB
       buf.set_swap_order(store -> swap_order());
#endif

       x -> cdrIn(buf);
       }
       break;
     default:
       break;
   }

   x -> set_mode(UPDATE, false);

//x -> my_oid().asciiOut(cerr); cerr << "\n";
//MESSAGE(cerr, "template_mgr_t::init_obj() done");
   return true;

}

Boolean
template_mgr_t::commit_obj(abs_storage* store, root* obj_ptr)
{
   if ( store && obj_ptr && 
        obj_ptr -> get_mode(UPDATE) == true
   ) {

/*
MESSAGE(cerr, "COMMIT OBJECT");
debug(cerr, obj_ptr -> my_oid());
*/

////////////////////////////////
// update the object disk copy.
////////////////////////////////

      switch ( obj_ptr -> get_mode(CDR) ) {
     
         case true:

#ifdef PORTABLE_DB
             cdr_io_buf.set_swap_order(store -> swap_order());
#endif

            cdr_io_buf.reset();
            obj_ptr -> cdrOut(cdr_io_buf);

//debug(cerr, cdr_io_buf);
//debug(cerr, cdr_io_buf.content_sz());

            store -> 
                updateString(mmdb_pos_t(obj_ptr -> my_oid().icode()), 
                             cdr_io_buf.get_base(),
                             cdr_io_buf.content_sz(),
                             CLASS_CODE_BYTES 
                            );
   
            break;
   
        case false:

            store -> 
                updateString(mmdb_pos_t(obj_ptr -> my_oid().icode()), (char*)obj_ptr,
                             obj_ptr -> mem_sizeof(), 
                             CLASS_CODE_BYTES
                            );
             break;
      }

      obj_ptr -> set_mode(UPDATE, false);
     
   }

   return true;
}

Boolean
template_mgr_t::quit_obj(abs_storage* store, root* obj_ptr)
{
//MESSAGE(cerr, "template_mgr_t::quit_obj()");
//debug(cerr, (void*)obj_ptr);
   commit_obj(store, obj_ptr);

   if ( obj_ptr ) 
      obj_ptr -> f_oid.become(ground);

   delete obj_ptr;

   return true;
}

/******************************************************/
// return an object in x. The object is
// saved on the store 'store'. Its location is the
// i_code of the object.
/******************************************************/

Boolean 
template_mgr_t::create_obj(abs_storage* store, c_code_t class_code, root*& new_obj)
{
//MESSAGE(cerr, "Create obj case");
//debug(cerr, class_code);

#ifdef DEBUG
    fprintf(stderr, "create_obj class_code=%d\n", class_code);
#endif

    /*****************************************/
    // Can we find a matched template object?
    // The match is based on the class code.
    /*****************************************/
    root* object_template = look_up(class_code);

    if ( object_template == 0 ) {
        debug(cerr, class_code);
        throw(stringException("get_obj(): unknown object instance"));
    }

    /*****************************************/
    // ask the template to create a new object
    // on store 'store'.
    /*****************************************/

    char* store_str_ptr = 0;

#ifdef COMPACTED_DISK_REP
	 
    if ( object_template -> get_cdr_size() == 0 ) {
//////////////////////////////////
// compute and cache the cdr_size  
//////////////////////////////////
      cdr_io_buf.reset();
      object_template -> cdrOut(cdr_io_buf);
      object_template -> set_cdr_size(cdr_io_buf.content_sz());
    }

    int disk_obj_len = object_template -> get_cdr_size();

#else
    int disk_obj_len = object_template -> mem_sizeof();
#endif

/*
MESSAGE(cerr, "to allocate string");
debug(cerr, disk_obj_len);
debug(cerr, CLASS_CODE_BYTES);
*/

    if ( g_transac ) {
        g_transac -> book(store);
    }

    persistent_info pinfo;

#ifdef C_API
    store -> allocString (pinfo.position,
                                disk_obj_len + CLASS_CODE_BYTES,
                                store_str_ptr, spointer_t::IS_OBJECT
                         );
#else
    store -> allocString (pinfo.position,
                                disk_obj_len + CLASS_CODE_BYTES,
                                store_str_ptr, spointer_t::IS_OBJECT
                         );
#ifdef DEBUG
    {
      char* dbg_char_ptr;
      int   dbg_int;
      store->get_str_ptr(pinfo.position, dbg_char_ptr, dbg_int);
      fprintf(stderr, "create_obj ptr=0x%lx len=%d\n",
					(long)dbg_char_ptr, dbg_int);
    }
#endif
#endif

#ifdef COMPACTED_DISK_REP
    SET_BIT(class_code, CDR_FLAG); 
#endif

//debug(cerr, pinfo.position);

#ifdef PORTABLE_DB
   if ( store -> swap_order() == true ) {
//MESSAGE(cerr, "switch class code in create_obj");
       c_code_t x = class_code;
//debug(cerr, x);
       ORDER_SWAP_USHORT(x);
//debug(cerr, x);
       memcpy(store_str_ptr, (char*)&x, sizeof(x));
   } else {
//MESSAGE(cerr, "do not switch class code in create_obj");
//debug(cerr, class_code);
       memcpy(store_str_ptr, (char*)&class_code, sizeof(class_code));
   }
#else
   memcpy(store_str_ptr, (char*)&class_code, sizeof(class_code));
#endif

//debug(cerr, class_code);
//debug(cerr, long(store_str_ptr));
//for ( int i=0; i<sizeof(class_code); i++ )
//   cerr << int(store_str_ptr[i]) << " ";
//cerr << endl;

#ifdef COMPACTED_DISK_REP
    RESET_BIT(class_code, CDR_FLAG); 
#endif

    pinfo.storage = store;
    pinfo.old_object = false;
    pinfo.persistent = true;
    pinfo.class_code = class_code;

#ifdef MEMORY_MAPPED
    new_obj = object_template -> cast_to( store_str_ptr + CLASS_CODE_BYTES);
#else

//MESSAGE(cerr, "to clean mem allocated");

    int mem_obj_len = object_template -> mem_sizeof();

    //char* p = g_memory_pool.alloc(mem_obj_len);
    char* p = new char[mem_obj_len];

//MESSAGE(cerr, "template mgr: create case(), new char array");
//debug(cerr, (void*)p);

    memset(p, char(0), mem_obj_len);
    new_obj = object_template -> cast_to( p );

#endif

   new_obj -> init_persistent_info( &pinfo );

#ifdef COMPACTED_DISK_REP
    new_obj -> set_mode(CDR, true);
#else
    new_obj -> set_mode(CDR, false);
#endif

    new_obj -> set_mode(OLD_OBJECT, true);
    new_obj -> set_mode(UPDATE, true);
    new_obj -> set_mode(HEALTH, true);

//debug(cerr, int(new_obj -> get_mode(BASE_DATA_INITED)));
//debug(cerr, int(new_obj -> get_mode(PERSISTENT)));

/*
MESSAGE(cerr, "oid of the new created obj:");
debug(cerr, class_code);
cerr << "class_code=" << class_code ;
cerr <<  ", location=";
cerr << PAGE_ID((new_obj -> my_oid()).icode(), 8192) << ".";
cerr << PAGE_IDX((new_obj -> my_oid()).icode(), 8192) << "\n";
*/


//MESSAGE(cerr, "create_obj done()");

    return true;
}

Boolean template_mgr_t::destroy_obj(abs_storage* store, root* x)
{
   if ( store && x ) {
//MESSAGE(cerr, "destroy_obj()");
//debug(cerr, x -> my_oid());
      store -> deleteString( mmdb_pos_t(x -> my_oid().icode()) );
      x -> f_oid.become(ground);
//MESSAGE(cerr, "destroy_obj() done");
   }

   return true;
}

Boolean template_mgr_t::insert_template(root* tmt)
{
    if ( tmt == 0 ) return true;

    c_code_t class_code = tmt -> my_oid().ccode();

    if ( look_up(class_code) == 0 ) {
       data_t key_data(class_code, tmt);
       v_template_objs.insert(key_data);
    } 

    return true;
}

