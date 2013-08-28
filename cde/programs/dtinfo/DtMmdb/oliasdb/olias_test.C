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
 * $XConsortium: olias_test.C /main/8 1996/10/26 18:20:03 cde-hal $
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


#include "oliasdb/olias_test.h"
#include "api/utility.h"
#include "utility/pm_random.h"
#include "misc/unique_id.h"
#include "storage/vm_storage.h"
#include "dstr/dstr_test.h"
#include "storage/store_test.h"
#include "oliasdb/olias_funcs.h"

#include "object/random_gen.h"

#ifdef REGRESSION_TEST

#include "oliasdb/graphic_test.h"
#include "oliasdb/loc_test.h"
#include "oliasdb/stylesheet_test.h"
#include "oliasdb/dlp_test.h"
#include "oliasdb/mark_test.h"
#include "oliasdb/toc_test.h"
#include "oliasdb/doc_test.h"
#include "oliasdb/node_test.h"

#endif

#include "oliasdb/collectionIterator.h"
#include "oliasdb/mmdb.h"

      
#ifdef NODEBUG
void test_collector_iterator(info_base* base)
{
   MESSAGE(cerr, "node locators:");
   nodeCollectionIterator x(base);

   while ( ++x ) {
     debug(cerr, x.get_locator());
   }

   MESSAGE(cerr, "stylesheet locators:");

   stylesheetCollectionIterator y(base);

   while ( ++y ) {
     debug(cerr, y.get_locator());
   }

   MESSAGE(cerr, "locator set locators:");

   locatorCollectionIterator z(base);

   while ( ++z ) {
     debug(cerr, z.get_locator());
   }
}

int dump_instances(info_base* base_ptr, char* col_nm, c_code_t ins_code)
{
   if ( base_ptr == 0 )
      throw(stringException("null base ptr"));

   Iterator *it = base_ptr -> first(col_nm, ins_code);
  
   if (it==0)
      throw(stringException("null iterator pointer"));

   int i = 0;

   while ( *it ) {
      oid_t id = base_ptr -> get_oid(*it);

      if ( strcmp(col_nm, NODE_SET_NAME) == 0 ) {
         i++;
         node_smart_ptr x(base_ptr, id);
         cout << x.data();
         //debug(cerr, x.long_title());
      } else 
      if ( strcmp(col_nm, LOCATOR_SET_NAME) == 0 ) {
         i++;
         locator_smart_ptr x(base_ptr, id);
         cout << x.inside_node_locator_str() << "\n";
      } 
      if ( strcmp(col_nm, GRAPHIC_SET_NAME) == 0 ) {
         i++;
         graphic_smart_ptr x(base_ptr, id);

         if ( x.type() == 3 ) {
            const char* ps_str = x.data();
            cout << "\n";
            cout << "\n";
            cout << x.data_size() << "\t";
            for ( int j=0; j<x.data_size(); j++ )
               cout << ps_str[j];

            cout << "\n";
         }
      } 

      base_ptr -> next(*it);
   }
   debug(cerr, i);

   delete it;

   return 0;
}

int cache_test( info_lib* infolib_ptr, const char* base_name, int argc, char** argv )
{
   locator_smart_ptrPtr* ptr_array = new locator_smart_ptrPtr[argc];
debug(cerr, argc);

   int i;
   for ( i=0; i<argc; i++ ) {
      ptr_array[i] = new locator_smart_ptr(infolib_ptr, base_name, argv[i]);
   
      cerr << form("node_id of locator %s:\n", argv[i]);
      ptr_array[i] -> node_id().asciiOut(cerr); cerr << "\n";
      //debug(cerr, ptr_array[i] -> vcc_num());
      cerr << "\n";
   }

   for ( i=0; i<argc; i++ ) {
      delete ptr_array[i];
   }
   delete ptr_array;

   return 0;
}

void dump_node_ids(info_base* x_base)
{
   if ( x_base == 0 )
     return;

   int docs = x_base -> num_of_docs();

   doc_smart_ptr* doc = 0;

   node_smart_ptr* first_node = 0, *node = 0;
   dlp_smart_ptr* x = 0, *dlp_cell = 0;

   for ( int i=0; i<docs; i++ ) {
      doc = new doc_smart_ptr(x_base, i+1);

      debug(cerr, doc -> long_title());

      first_node = new node_smart_ptr(x_base, doc -> locator_id());

      dlp_cell = new dlp_smart_ptr(x_base, first_node -> its_oid());

      while ( dlp_cell ) {

         debug(cerr, dlp_cell -> node_id());

         x = dlp_cell -> next();

         delete dlp_cell;

         dlp_cell = x;
      }

      delete doc;
      delete first_node;
   }
}


#endif

#ifdef REGRESSION_TEST

#define TEST_STY_FILE "test.sty"
#define TEST_SEC_FILE "test.sec"
#define TEST_MIX_FILE "test.mix"

int generate_stream(info_base* b_ptr, char* path, int num_sections, int min, int max)
{
   random_gen x;

   if ( check_and_create_dir(path) == false )
     return -1;

// generate stylesheet stream

   cerr << " stylesheets ..." << endl;
   int stylesheets = x.pick_a_value(5, 20); // assume [5, 20] is the range

   char buf[BUFSIZ];
   snprintf(buf, sizeof(buf), "%s/%s", path, TEST_STY_FILE);
   fstream stylesheet_stream(buf, ios::out);

   if ( !stylesheet_stream )
     return -1;

   int i;
   for (i=0; i<stylesheets; i++ ) {
// assume the stylesheet and section are of approx. length
     generate_stylesheet_instance(x, stylesheet_stream, min, max);
   }

// generate section stream

   cerr << " sections ..." << endl;

   snprintf(buf, sizeof(buf), "%s/%s", path, TEST_SEC_FILE);
   fstream section_stream(buf, ios::out);

   if ( !section_stream ) return -1;


// prepare mixed stream
   snprintf(buf, sizeof(buf), "%s/%s", path, TEST_MIX_FILE);
   fstream mix_stream(buf, ios::out);

   if ( !mix_stream ) return -1;

   for (i=0; i<num_sections; i++ )
     generate_node_instance(b_ptr, x, section_stream, mix_stream, min, max);

// generate mixed stream
   cerr << " docs ..." << endl;

// doc
   int docs = x.pick_a_value(num_sections/4, num_sections/2);
   for (i=0; i<docs; i++ ) {
// [docs/10, docs] is for the number of tab sections
      //generate_doc_instance(x, mix_stream, i+1, docs/10, docs);
      generate_doc_instance(x, mix_stream, i+1, 1, 10);
   }

// toc
   cerr << " tocs ..." << endl;
   int tocs = docs;
   for (i=0; i<tocs; i++ ) {
// [0, num_sections/10] is for the branch factor
     generate_toc_instance(x, mix_stream, i, 0, num_sections/10);
   }

// sgml 

// graphic
   cerr << " graphics ..." << endl;
   int graphics = x.pick_a_value(num_sections/2, 2*num_sections);
   for (i=0; i<graphics; i++ )
     generate_graphic_instance(b_ptr, x, mix_stream, min, max);

// locators 
   cerr << " locators ..." << endl;
   int locs = x.pick_a_value(num_sections, num_sections*5);
   for (i=0; i<locs; i++ )
     generate_loc_instance(x, mix_stream);

   return 0;
}

int destroy_stream(char* path)
{
   mtry {
      del_file(TEST_STY_FILE, path);
      del_file(TEST_SEC_FILE, path);
      del_file(TEST_MIX_FILE, path);
   }
   mcatch_any()
   {
      return -1;
   }
   end_try;

   return 0;
}

int extract_and_compare_objects(istream& in, info_base* base_ptr, int code)
{
   int ok = 0;

   handler* root_hd_ptr = 0;

   vm_storage st((char*)"", (char*)"");

   char ccode_buf[LBUFSIZ];
   int c;
   while ( (c = in.get()) != EOF ) {
      in.putback(c);

      in.getline(ccode_buf, LBUFSIZ);

      sscanf(ccode_buf, "%u", &c);
//debug(cerr, c);
//fprintf(stderr, "code = %d\n", c);

      if ( c == SGML_CONTENT_CODE ) {
        if ( code == c || code == 0 )
           ok |= compare_SGML_content(in, base_ptr, (Boolean)true);
        else
           compare_SGML_content(in, base_ptr, (Boolean)false);
      } else {

         root_hd_ptr = new handler(c, &st);
   
         (*root_hd_ptr) -> asciiIn(in);
   
         if ( code == c || code == 0 ) {
            switch ( c ) {
              case OLIAS_NODE_CODE :
                 ok |= compare_node(*(node_smart_ptr*)root_hd_ptr, base_ptr);
                 break;
              case STYLESHEET_CODE :
                 ok |= compare_stylesheet(*(stylesheet_smart_ptr*)root_hd_ptr, base_ptr);
                 break;
              case DOC_CODE :
                 ok |= compare_doc(*(doc_smart_ptr*)root_hd_ptr, base_ptr);
                 break;
              case GRAPHIC_CODE :
                 ok |= compare_graphic(*(graphic_smart_ptr*)root_hd_ptr, base_ptr);
                 break;
              case TOC_CODE :
                 ok |= compare_toc(*(toc_smart_ptr*)root_hd_ptr, base_ptr);
                 break;
              case LOCATOR_CODE :
                 ok |= compare_locator(*(locator_smart_ptr*)root_hd_ptr, base_ptr);
                 break;
              default:
                 break;
            }
         }
   
         delete root_hd_ptr;
      }
      st.remove();
   }
   return ok;
}

struct {
   char* name;
   c_code_t code;
} auto_test_spec[] = 
{
  {(char*)"auto_node_test", OLIAS_NODE_CODE},
  {(char*)"auto_SGML_content_test", SGML_CONTENT_CODE},
  {(char*)"auto_stylesheet_test", STYLESHEET_CODE},
  {(char*)"auto_graphic_test", GRAPHIC_CODE},
  {(char*)"auto_doc_test", DOC_CODE},
  {(char*)"auto_toc_test", TOC_CODE},
  {(char*)"auto_loc_test", LOCATOR_CODE},
};

#define NUM_OF_AUTO_TESTS 7

int auto_test(int argc, char** argv, OLIAS_DB& db)
{
   int ok = 0;

   info_lib* infolib_ptr = db.openInfoLib(argv[2]);

// define
   if ( infolib_ptr == 0 ||
        infolib_ptr->define_info_base(argv[4],argv[4],argv[3]) == false
      ) {
      STDERR_MESSAGE("creating test database failed\n");
      return -1;
   } else
      STDERR_MESSAGE("creating test database done\n");

   db.closeInfoLib(infolib_ptr->get_info_lib_uid());

// randomly generate raw stream
   infolib_ptr = db.openInfoLib(argv[2]);
   info_base* base_ptr = infolib_ptr -> get_info_base(argv[4]);
   ok = generate_stream(base_ptr, argv[2],
		   atoi(argv[5]), atoi(argv[6]), atoi(argv[7])
		  );
   if ( ok != 0 ) {
      STDERR_MESSAGE("generating test data failed\n");
      return -1;
   } else
      STDERR_MESSAGE("generating test data done\n");

   db.closeInfoLib(infolib_ptr->get_info_lib_uid());

// load style sheet
   cerr << " loading stylesheets ..." << endl;
   char buf[256];
   char file_name[256];
   snprintf(buf, sizeof(buf), "%s.stylesheet", argv[4]);
   snprintf(file_name, sizeof(file_name), "%s/%s", argv[2], TEST_STY_FILE);

   infolib_ptr = db.openInfoLib(argv[2]);
   base_ptr = infolib_ptr -> get_info_base(argv[4]);
   ok = load( base_ptr, buf, file_name );

   if ( ok != 0 ) {
      STDERR_MESSAGE("loading test stylesheet data failed\n");
      return -1;
   } else
      STDERR_MESSAGE("loading test stylesheet data done\n");

   db.closeInfoLib(infolib_ptr->get_info_lib_uid());


// load sections (small component)
   cerr << " loading sections ..." << endl;
   snprintf(buf, sizeof(buf), "%s.node", argv[4]);
   snprintf(file_name, sizeof(file_name), "%s/%s", argv[2], TEST_SEC_FILE);

   infolib_ptr = db.openInfoLib(argv[2]);
   base_ptr = infolib_ptr -> get_info_base(argv[4]);
   ok = load( base_ptr, buf, file_name );

   if ( ok != 0 ) {
      STDERR_MESSAGE("loading test section data failed\n");
      return -1;
   } else
      STDERR_MESSAGE("loading test section data done\n");

   db.closeInfoLib(infolib_ptr->get_info_lib_uid());

// load all other objects
   cerr << " loading all other objects ..." << endl;
   snprintf(file_name, sizeof(file_name), "%s/%s", argv[2], TEST_MIX_FILE);
   fstream in(file_name, ios::in);

   if ( !in ) return -1;

   infolib_ptr = db.openInfoLib(argv[2]);
   base_ptr = infolib_ptr -> get_info_base(argv[4]);
   ok = _load_mixed_objects(base_ptr, in);

   if ( ok != 0 ) {
      STDERR_MESSAGE("loading all other test data failed\n");
      return -1;
   } else
      STDERR_MESSAGE("loading all other test data done\n");

   db.closeInfoLib(infolib_ptr->get_info_lib_uid());

// test
           
   infolib_ptr = db.openInfoLib(argv[2]);
   base_ptr = infolib_ptr -> get_info_base(argv[4]);

// stylesheet
   snprintf(file_name, sizeof(file_name), "%s/%s", argv[2], TEST_STY_FILE);
   fstream ss_in(file_name, ios::in);

   if ( !ss_in ) return -1;

   ok = extract_and_compare_objects(ss_in, base_ptr, 0);

   if ( ok != 0 ) {
      STDERR_MESSAGE("testing stylesheet data failed\n");
      return -1;
   } else
      STDERR_MESSAGE("testing stylesheet data done\n");

// node 
   snprintf(file_name, sizeof(file_name), "%s/%s", argv[2], TEST_SEC_FILE);
   fstream nd_in(file_name, ios::in);
   if ( !nd_in ) return -1;

   ok = extract_and_compare_objects(nd_in, base_ptr, 0);

   if ( ok != 0 ) {
      STDERR_MESSAGE("testing section failed\n");
      return -1;
   } else
      STDERR_MESSAGE("testing section done\n");

// all other 
   snprintf(file_name, sizeof(file_name), "%s/%s", argv[2], TEST_MIX_FILE);
   fstream all_in(file_name, ios::in);
   if ( !all_in ) return -1;

   ok = extract_and_compare_objects(all_in, base_ptr, 0);

   if ( ok != 0 ) {
      STDERR_MESSAGE("testing all other data failed\n");
      return -1;
   } else
      STDERR_MESSAGE("testing all other data done\n");

   db.closeInfoLib(infolib_ptr->get_info_lib_uid());

   if ( argc == 8 ||
        ( argc == 9 && strcmp(argv[8], "true") == 0 ) )
   {
      db.destroy(argv[2]);
      destroy_stream(argv[2]);
   }

   return ok;
}

#endif

int select_debug_routine(int argc, char** argv, OLIAS_DB& db)
{
   int ok = 2;

#ifdef REGRESSION_TEST
   if ( strcmp(argv[1], "auto_test") == 0 ) {

      if (argc != 8 && argc != 9 ) {
//                      1            2           3            4             5
        cerr << "usage: auto_test info_lib_path info_base_spec base_name number_of_sections max_section_size, min_section_size [true|false]\n"; 
        return 1;
      } else
        return auto_test(argc, argv, db);

   } 

   ok =store_test(argc, argv);

   if ( ok != 2 ) {
      return ok;
   } 

   ok =dstr_test(argc, argv);

   if ( ok != 2 ) {
      return ok;
   } 

   ok =mark_test(argc, argv);
#ifdef NODEBUG
   if (ok < 0) {
     cerr << "mark_test failed." << endl;
     return ok;
   }
   else
     cerr << "mark_test done." << endl;
#else
   if ( ok != 2 ) {
      return ok;
   } 
#endif

   for ( int i=0; i<NUM_OF_AUTO_TESTS; i++ )
   
      if ( strcmp(argv[1], auto_test_spec[i].name) == 0 ) {
        
        if ( argc != 4 && argc != 3 ) {
           cerr << " args: " << auto_test_spec[i].name ;
           cerr << " base_name [stream_file_name]";
           return 1;
        }

        info_base* base_ptr = db.openInfoLib() -> get_info_base(argv[2]);

        if ( argc == 4 ) {
           fstream in(argv[3], ios::in);
           return extract_and_compare_objects(in, base_ptr, auto_test_spec[i].code);
        } else {
           return extract_and_compare_objects(cin, base_ptr, auto_test_spec[i].code);
        }
      }
#endif

#ifdef NODEBUG
   if ( strcmp(argv[1], "random_gen") == 0 ) {
        if ( argc != 7 ) {
           cerr << "random_gen args: " << "random_gen base_name path num_sections min_section_len max_sec_len\n";
           return 1;
        }
       info_base* base_ptr = db.openInfoLib() -> get_info_base(argv[2]);

       return generate_stream(base_ptr, argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
   } else

   if ( strcmp(argv[1], "test_iterator") == 0 ) {
         info_base* base_ptr = db.openInfoLib() -> get_info_base(argv[2]);
        test_collector_iterator(base_ptr);
        ok = 0;
   } else 

   if ( strcmp(argv[1], "dump_node_ids") == 0 ) {
      if ( argc != 3 ) {
         MESSAGE(cerr, "dump_node_ids args: dump_node_ids base_nm");
      } else {
         info_base* base_ptr = db.openInfoLib() -> get_info_base(argv[2]);
         dump_node_ids(base_ptr);
         ok = 0;
      }
   } else 

   if ( strcmp(argv[1], "dump_instances") == 0 ) {
      if ( argc != 4 ) {
         MESSAGE(cerr, "load args: dump_all_instance base_nm type");
      } else {
         info_base* base_ptr = db.openInfoLib() -> get_info_base(argv[2]);

         if ( strcmp(argv[3], "nodes") == 0 )
            ok = dump_instances(base_ptr, NODE_SET_NAME, OLIAS_NODE_CODE);
         else
         if ( strcmp(argv[3], "locators") == 0 )
            ok = dump_instances(base_ptr, LOCATOR_SET_NAME, LOCATOR_CODE);
         else
         if ( strcmp(argv[3], "graphics") == 0 )
            ok = dump_instances(base_ptr, GRAPHIC_SET_NAME, GRAPHIC_CODE);
      }
   } else

   if ( strcmp(argv[1], "file_load") == 0 ) {
      if ( (argc-2) % 3 != 0 ) {
         MESSAGE(cerr,
           "load args: file_load [obj_type obj_name data_file]+");
      } else {
         info_base* base_ptr = db.openInfoLib() -> get_info_base(argv[2]);
         ok = load( base_ptr, argv+2, argc-2 );
      }

   } else

   if ( strcmp(argv[1], "loc_test") == 0 ) {
      if ( argc != 4 ) {
         MESSAGE(cerr,
           "loc_test args: loc_test base_name loc_value");
      } else {
         ok = loc_test( db.openInfoLib(), argv[2], argv[3] );
      }
   
   } else 
   if ( strcmp(argv[1], "toc_test") == 0 ) {
      if ( argc != 4 ) {
         MESSAGE(cerr,
           "toc_test args: toc_test base_name oid_str");
      } else {
         ok = toc_test( db.openInfoLib(), argv[2], argv[3] );
      }
   
   } else 
   if ( strcmp(argv[1], "graphic_test") == 0 ) {
      if ( argc != 4 ) {
         MESSAGE(cerr,
           "graphic_test args: graphic_test base_name oid_str");
      } else {
         ok = graphic_test( db.openInfoLib(), argv[2], argv[3] );
      }
   
   } else 
   if ( strcmp(argv[1], "stylesheet_test_loc") == 0 ) {
      if ( argc != 4 ) {
         MESSAGE(cerr,
           "stylesheet_test_loc args: stylesheet_test_loc base_name loc");
      } else {
         ok = stylesheet_test_loc( db.openInfoLib(), argv[2], argv[3] );
      }
   } else 
   if ( strcmp(argv[1], "stylesheet_test_oid") == 0 ) {
      if ( argc != 4 ) {
         MESSAGE(cerr,
           "stylesheet_test_oid args: stylesheet_test_oid base_name oid");
      } else {
         ok = stylesheet_test_oid( db.openInfoLib(), argv[2], argv[3] );
      }
   } else 
   if ( strcmp(argv[1], "node_test_loc") == 0 ) {
      if ( argc != 4 ) {
         MESSAGE(cerr,
           "node_test_loc args: node_test_loc base_name loc");
      } else {
         ok = node_test_loc( db.openInfoLib(), argv[2], argv[3] );
      }
   } else 

   if ( strcmp(argv[1], "node_test_oid") == 0 ) {
      if ( argc != 4 ) {
         MESSAGE(cerr,
           "node_test_oid args: node_test_oid base_name oid_str");
      } else {
         ok = node_test_oid( db.openInfoLib(), argv[2], argv[3] );
      }
   
   } else 
   if ( strcmp(argv[1], "dlp_test") == 0 ) {
      if ( argc != 4 ) {
         MESSAGE(cerr,
           "dlp_test args: dlp_test base_name oid_str");
      } else {
         ok = dlp_test( db.openInfoLib(), argv[2], argv[3] );
      }
   
   } else 
   if ( strcmp(argv[1], "doc_test_oid") == 0 ) {
      if ( argc != 4 ) {
         MESSAGE(cerr,
           "doc_test_oid args: doc_test_oid base_name oid_str");
      } else {
         ok = doc_test_oid( db.openInfoLib(), argv[2], argv[3] );
      }
   
   } else 
   if ( strcmp(argv[1], "doc_test_int") == 0 ) {
      if ( argc != 4 ) {
         MESSAGE(cerr,
           "doc_test_int args: doc_test_int base_name int_str");
      } else {
         ok = doc_test_int( db.openInfoLib(), argv[2], argv[3] );
      }
   } 

#endif
      
   return ok;
}
