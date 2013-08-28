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
 * $XConsortium: object_dict.C /main/7 1996/09/13 20:48:20 cde-hal $
 *
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
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


#include "schema/object_dict.h"
#include "utility/randomize.h"
#include "utility/db_version.h"
#include "misc/unique_id.h"

desc* desc_ptr = 0;
desc* last_desc_ptr = 0;

char replace_string[PATHSIZ];
int replace_string_len;

//extern int yyparse();
//extern int yyrestart(FILE*);
//extern FILE *yyin;

extern int schemaparse();
extern int schemarestart(FILE*);
extern FILE *schemain;

object_dict::object_dict() :
   v_dict(desc_name_eq, desc_name_ls), v_desc_ptr(0)
{
   v_db_path[0] = 0;
}

object_dict::~object_dict()
{
   quit_a_base(v_desc_ptr, 0, true);
}

void object_dict::quit_a_base(desc* b, desc* e, Boolean sync)
{
   _quit_stored_objects(b, e);
   _quit_stores(b, e, sync);
   _quit_descs(b, e);
}

void object_dict::_quit_descs(desc* begin_ptr, desc* end_ptr)
{
   desc *ptr = begin_ptr;
   desc *x_ptr = 0;

   while ( ptr != end_ptr ) {
      x_ptr = ptr;
      ptr = ptr -> next_desc;
      delete x_ptr;
   }
}

extern int g_mode_8_3;


desc* object_dict::init_a_base(char* db_path, char* db_name) 
{
//MESSAGE(cerr, "object_dict::init_a_base()");
//debug(cerr, db_path);

   int len = MIN(strlen(db_path), PATHSIZ - 1);
   *((char *) memcpy(v_db_path, db_path, len) + len) = '\0';

   desc* x = 0;

   char* schema_path = 0;

   if ( g_mode_8_3 )
      schema_path = form("%s/%s.%s", v_db_path, db_name, SCHEMA_FILE_SUFFIX);
   else
      schema_path = form("%s/%s", v_db_path, SCHEMA_FILE);

   fstream in(schema_path, ios::in);

   if ( !in )
      throw(streamException(in.rdstate()));

   char schema_header[LBUFSIZ];

   if ( !in.getline(schema_header, LBUFSIZ) ) {
       throw(streamException(in.rdstate()));
   }

   if ( strncmp(schema_header+1, "MMDB", 4) != 0 ) {
      in.close();
      x = parse(schema_path);
   } else {

      unsigned int sz = bytes(schema_path) - strlen(schema_header) - 1;
      char* buf = new char[sz];
   
      if ( !in.read(buf, sz) ) 
         throw(streamException(in.rdstate()));

      in.close();
   
      buffer orig(0);
      orig.set_chunk(buf, sz);
      orig.set_content_sz(sz);
   
      randomize rd(233);
      rd.restore(orig);

//fprintf(stderr, "buf=%s\n", buf);
//debug(cerr, buf);
   
   ////////////////
   // do the parse
   ////////////////
      x = parse(orig);

      delete buf;

   }

   _init(x);

   return x;
}

desc* object_dict::init_a_base(char* define_desc_path, char* db_path, 
                         char* db_name) 
{
//MESSAGE(cerr, "object_dict::init a base (define case)");
//debug(cerr, define_desc_path);
//debug(cerr, db_path);
//debug(cerr, db_name);

   int len;

   len = MIN(strlen(db_path), PATHSIZ - 1);
   *((char *) memcpy(v_db_path, db_path, len) + len) = '\0';

   if ( db_name ) {
      len = MIN(strlen(db_name), PATHSIZ - 1);
      *((char *) memcpy(replace_string, db_name, len) + len) = '\0';
      replace_string_len = strlen(replace_string);
   } else {
      replace_string[0] = 0;
      replace_string_len = 0;
   } 


   fstream in_test(define_desc_path, ios::in);

   if ( ! in_test ) {
      throw(stringException(form("%s does not exist.", define_desc_path)));
   }

   unsigned long llen = bytes(define_desc_path)*4;

   in_test.close();

   if ( disk_space(v_db_path) < llen ) {
       throw(stringException(form("no enough space on %s", v_db_path)));
   }

///////////////////////////////////
// define and init the dictionary
///////////////////////////////////
   desc* x = parse(define_desc_path);
   _init(x);

////////////////////////////
// randomize the schema.mmdb
////////////////////////////

   fstream in(define_desc_path, ios::in);

   if ( !in) {
       debug(cerr, define_desc_path);
       throw(streamException(in.rdstate()));
   } 

   unsigned int sz = bytes(define_desc_path);
   in.close();

   char* schema_buf = new char[sz*3];

   ostringstream* string_out = new ostringstream(schema_buf);
   if ( !(*string_out) ) {
       throw(streamException(string_out -> rdstate()));
   } 

   x -> asciiOutList(*string_out);

   len = MIN((unsigned int) string_out->str().size(), sz*3 - 1);
   *((char *) memcpy(schema_buf, string_out->str().c_str(), len) + len) = '\0';
   delete string_out;

   sz = strlen(schema_buf);

   buffer orig(0);
   orig.set_chunk(schema_buf, sz);
   orig.set_content_sz(sz);

   randomize rd(233);
   rd.scramble(orig);

/////////////////////////////
// save the output to db_path
/////////////////////////////
   fstream out(form("%s/%s.%s", db_path, db_name, SCHEMA_FILE_SUFFIX), ios::out);
// fstream out(form("%s/%s.%s", db_path, db_name, SCHEMA_FILE_SUFFIX), ios::out, open_file_prot());

   if ( !out ) {
       MESSAGE(cerr, form("bad file name: %s", db_path));
       throw(streamException(out.rdstate()));
   } 

   char* desc_header = form("#MMDB\t%d\t%d\n", MAJOR, MINOR);
   if ( !out.write(desc_header, strlen(desc_header)) ) {
       MESSAGE(cerr, form("write descirption head in %s failed", db_path));
       throw(streamException(out.rdstate()));
   }

   if ( !out.write(schema_buf, strlen(schema_buf)) ) {
       MESSAGE(cerr, form("write descirption in %s failed", db_path));
       throw(streamException(out.rdstate()));
   }

   out.close();

   delete schema_buf;


   return x;
}

desc* object_dict::parse(buffer& desc_buf)
{
    char unique_nm[PATHSIZ];

    Boolean ok = writeToTmpFile (
		unique_nm, 
		desc_buf.get_base(), 
		desc_buf.content_sz()
				); 

//debug(cerr, desc_buf.get_base());
//fprintf(stderr, "get_base=%s\n", desc_buf.get_base());
//fprintf(stderr, "sz=%d\n", desc_buf.content_sz());
//fprintf(stderr, "tmpnm=%s\n", unique_nm);

    if ( ok == false ) {
//fprintf(stderr, "write to temp failed\n");
       throw(stringException("can't prepare the object dictionary."));
    }

    desc* x = 0;

    mtry {
       x = parse(unique_nm);
    }

    mcatch (mmdbException &,e) {
       del_file(unique_nm);
       rethrow;
    } end_try;

    del_file(unique_nm);

    return x;
}

desc* object_dict::parse(char* define_desc_path)
{
    schemain = fopen(define_desc_path, "r");

    if ( schemain == NULL )
       throw(stringException("open desc file failed"));

    static int ct = 0;

    if ( ct > 0 )
       schemarestart(schemain);
    else 
       ct = 1;

    mtry {
       if ( schemaparse() != 0 ) {
          fclose(schemain);
          throw(stringException("Parsing input failed"));
       }
    }

    mcatch (mmdbException &,e) {
      fclose(schemain);
      rethrow;
    } end_try;

    fclose(schemain);

    return desc_ptr;
}


void object_dict::_init(desc* x)
{

   desc *ptr = x;

   mtry { // init all stores
      while ( ptr ) {
         ptr -> init_store(this -> v_db_path);
         ptr = ptr -> next_desc;
      }
   }

   mcatch (mmdbException &,e) {
      _quit_stores(x, ptr);
      _quit_descs(x, ptr);
      rethrow;
   } end_try;

   ptr = x;

   while ( ptr ) { // add to the dict
      if ( ptr -> get_store() )
         v_dict.insert(ptr);     
      ptr = ptr -> next_desc;
   }

//////////////////////////////
// init stored objects
//////////////////////////////

//MESSAGE(cerr, "init stored obj:");
   ptr = x;

   while ( ptr ) {
      ptr -> init_handler(*this);
      v_dict.insert(ptr);     
      ptr = ptr -> next_desc;
   }

   if ( v_desc_ptr == 0 ) {
      v_desc_ptr = x;
   } else {
      v_last_desc_ptr -> next_desc = x;
   }
   v_last_desc_ptr = last_desc_ptr;
}

void object_dict::_quit_stores(desc* start_ptr, desc* end_ptr, Boolean sync)
{
   desc *ptr = start_ptr;

   if ( sync == true ) {
      while ( ptr != end_ptr ) {
   
         mtry {
            ptr -> sync_store();
         }
         mcatch (mmdbException &,e)
         {
#ifdef DEBUG
	    fprintf(stderr, "mmdbException caught @ %s line:%d.\n",
						__FILE__, __LINE__);
	    rethrow;
#endif
         }
         end_try;
   
         ptr = ptr -> next_desc;
      }
   }

   ptr = start_ptr;

   while ( ptr != end_ptr ) {
      mtry {
         ptr -> quit_store();
      }

      mcatch (mmdbException &,e)
      {
#ifdef DEBUG
	    fprintf(stderr, "mmdbException caught @ %s line:%d.\n",
						__FILE__, __LINE__);
	    rethrow;
#endif
      }
      end_try;

      ptr = ptr -> next_desc;
   }
}

void object_dict::_quit_stored_objects(desc* start_ptr, desc* end_ptr)
{
   desc *ptr = start_ptr;

   while ( ptr != end_ptr ) {

      mtry {
         ptr -> quit_handler();
//debug(cerr, *ptr);
      }

      mcatch (mmdbException &,e)
      {
#ifdef DEBUG
	 fprintf(stderr, "mmdbException caught @ %s line:%d.\n",
						__FILE__, __LINE__);
	 rethrow;
#endif
      }
      end_try;

      ptr = ptr -> next_desc;
   }
}

handler* object_dict::get_handler(const char* obj_name)
{
   stored_object_desc key((char*)obj_name);

   stored_object_desc* x = (stored_object_desc*)v_dict.member(&key);

   if ( x ) {
      return x -> get_handler();
   } else {
      throw(stringException(form("handler %s not in dict", obj_name)));
      return 0;
   }
}
   
abs_storage* object_dict::get_store(const char* obj_name)
{
   store_desc key((char*)obj_name);

   store_desc* x = (store_desc*)v_dict.member(&key);

   if ( x ) {
      return x -> get_store();
   } else {
      throw(stringException(form("store %s not in dict", obj_name)));
      return 0;
   }
}
   
void schemaerror( char* errorstr )
{
    extern int linecount;
    extern char schematext[];    

    if ( strlen( schematext ) > 0 )
	throw(stringException(form("line %d %s at or before \"%s\"",
		linecount, errorstr, schematext
                        )
             ));

    else
	throw(stringException(form("line %d %s illegal-symbol", 
                         linecount, errorstr 
                        )
             ));
	
    return;
}

extern "C" {
int schemawrap()
{
    return( 1 );
}
}

//void yyerror( char* errorstr )
//{
//    extern int linecount;
//    extern char yytext[];    
//
//    if ( strlen( yytext ) > 0 )
//	MESSAGE(cerr, form("line %d %s at or before \"%s\"",
//		linecount, errorstr, yytext
//                        )
//             );
//
//    else
//	MESSAGE(cerr, form("line %d %s illegal-symbol", 
//                         linecount, errorstr 
//                        )
//             );
//	
//    return;
//}
//
//int yywrap()
//{
//    extern int linecount;
//    extern char *pname;
//
//#ifdef DEBUG
//    MESSAGE(cerr, form("Number of input lines %6d", linecount));
//#endif
//    
//cerr << "Calling yywrap()\n";
//    //return( 1 );
//    return( 0 );
//}
