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
 * $XConsortium: info_lib.C /main/9 1996/12/02 12:47:19 cde-hal $
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


#include "unique_id.h"
#include "api/info_lib.h"
#include "utility/db_version.h"
#include "oliasdb/locator_hd.h"
#include "oliasdb/graphic_hd.h"

#ifdef DtinfoClient
#include <lib/DtSvc/DtUtil2/LocaleXlate.h>
#endif

/*
extern void report_total();
extern void reset_total();
*/

int g_mode_8_3 = 0;


typedef char* charPtr;

info_lib::info_lib(char** set_name_array, char** list_name_array,
                   char* info_lib_dir, char* selected_base_name, 
		   char* infoLibName, int des) :
set_nm_list(set_name_array), list_nm_list(list_name_array),
f_bad_base_array_size(0), f_bad_info_bases(0), 
f_bad_info_base_names(0), f_bad_info_base_paths(0), f_descriptor(des)

{
//debug(cerr, info_lib_dir);
//debug(cerr, infoLibName);

   int len;

   f_obj_dict = new object_dict;

   if ( info_lib_dir == 0 ) {
      throw(stringException("NULL info lib path"));
   }

   if ( check_and_create_dir(info_lib_dir) == false )
      throw(stringException(
        form("infolib %s does not exist or can't be created", info_lib_dir)
                           )
           );

   len = MIN(strlen(info_lib_dir), PATHSIZ -1);
   *((char *) memcpy (info_lib_path, info_lib_dir, len) + len) = '\0';
   len = MIN(strlen(infoLibName), PATHSIZ -1);
   *((char *) memcpy (info_lib_name, infoLibName, len) + len) = '\0';

   fstream *map_in = 0;

   if ( exist_file(MAP_FILE_8_3, info_lib_dir) == true ) {

      map_in = new fstream(form("%s/%s", info_lib_dir, MAP_FILE_8_3), ios::in);

      g_mode_8_3 = 1;

      if ( !map_in -> getline(info_lib_name, PATHSIZ, '\t') ||
           !map_in -> getline(info_lib_uid, UIDSIZ, '\n')
         )
      {
         throw(stringException(
           form("infolib %s does not have correct name-id entry", info_lib_dir)
                              )
              );
      }
   } else
   if ( exist_file(MAP_FILE, info_lib_dir) == true )
      map_in = new fstream(form("%s/%s", info_lib_dir, MAP_FILE), ios::in);
   else {
      return;
   }

   char base_name[PATHSIZ];
   char base_desc[PATHSIZ];
   char base_uid[UIDSIZ];
   char base_locale[PATHSIZ];
   char db_path_name[PATHSIZ];

   int major_mm_version = 0;
   int minor_mm_version = 0;


   while ( map_in -> getline(base_name, PATHSIZ, '\t') ) {

      map_in -> getline(base_desc, PATHSIZ, '\t');

      if ( g_mode_8_3 == 1 ) {
         map_in -> getline(base_uid, PATHSIZ, '\t');
	 map_in -> getline(base_locale, PATHSIZ, '\t');
      }
      else
         base_uid[0] = 0;

      (*map_in) >> major_mm_version >> minor_mm_version;

      map_in -> get();

      if ( base_name[0] != CommentChar ) {

//debug(cerr, base_name);
//debug(cerr, base_desc);
//fprintf(stderr, "base_name = %s\n", base_name);

	 char* mmdb_lang = getenv("MMDB_LANG");
#ifdef MMDB_LANG_DEBUG
	 cerr << "base_locale=" << base_locale << endl;
	 if (mmdb_lang)
	   cerr << "mmdb_lang=" << mmdb_lang << endl;
#endif

         if ((selected_base_name == 0 ||
	      strcmp(selected_base_name, base_name) == 0) &&
	     (mmdb_lang == NULL ||
	      strcmp(mmdb_lang, base_locale) == 0 ||
	      strcmp("C.ISO-8859-1", base_locale) == 0))
         {

            len = MIN(strlen(info_lib_dir) + strlen(base_name) +1, PATHSIZ -1);
            *((char *) memcpy (db_path_name,
			       form("%s/%s", info_lib_dir, base_name),
			       len) + len) = '\0';
 
            mm_version mmv_code(MAJOR, MINOR);
            mm_version mmv_base_data(2, 1);
            mm_version mmv_data(major_mm_version, minor_mm_version);

///////////////////////////////////////////////
// Hardcoded knowledge of discontinuation of 
// backward compatibility
///////////////////////////////////////////////
            if ( mmv_data < mmv_base_data ||
                 mmv_data == mmv_base_data || 
	         mmv_code < mmv_data
               ) 
	    {


               if ( f_bad_base_array_size == 0 ||
                    f_bad_base_array_size <= f_bad_info_bases 
                  ) 
               {
                 
                 if ( f_bad_base_array_size == 0 ) {
                    f_bad_base_array_size = 10;
                    f_bad_info_base_names = new charPtr[f_bad_base_array_size];
                    f_bad_info_base_paths = new charPtr[f_bad_base_array_size];

                    for (int i=0; i<f_bad_base_array_size; i++) {
                      f_bad_info_base_paths[i] = 0;
                      f_bad_info_base_names[i] = 0;
                    }

                 } else {
                    char** x = new charPtr[2*f_bad_base_array_size];
                    char** y = new charPtr[2*f_bad_base_array_size];

                    for (int i=0; i<2*f_bad_base_array_size; i++) {
                      x[i] = 0;
                      y[i] = 0;
                    }

                    memcpy(x, f_bad_info_base_names, sizeof(charPtr)*f_bad_base_array_size);
                    memcpy(y, f_bad_info_base_paths, sizeof(charPtr)*f_bad_base_array_size);
                    f_bad_base_array_size *= 2;

                    delete f_bad_info_base_names;
                    delete f_bad_info_base_paths;

                    f_bad_info_base_names = x;
                    f_bad_info_base_paths = y;
                     
                 }


               } 


               f_bad_info_base_paths[f_bad_info_bases] = strdup(info_lib_dir);
               f_bad_info_base_names[f_bad_info_bases] = strdup(base_name);

               f_bad_info_bases++;

               MESSAGE(cerr, "Data and code version mismatch");

               MESSAGE(cerr, form("Data version: v%d.%d",
                    major_mm_version, minor_mm_version
                           ));

               MESSAGE(cerr, form("Code version: v%d.%d", 
               		MAJOR, MINOR
    			   ));

               MESSAGE(cerr, form("infobase %s is not available.", base_name));

               continue;
            }

//reset_total();
            _init_info_base(db_path_name, base_name, base_desc, base_uid, base_locale,
		            mm_version(major_mm_version, minor_mm_version));
//report_total();
         }
      }

   }
   map_in -> close();
   delete map_in ;
}

info_lib::~info_lib()
{
   long ind = first();
   while ( ind ) {

     info_base* x = (*this)(ind);
     delete x;

     next(ind) ;
   }

   if ( f_bad_info_base_paths ) {
      for (int i=0; i<f_bad_base_array_size; i++) {
        delete f_bad_info_base_paths[i];
      }
      delete f_bad_info_base_paths;
   }


   if ( f_bad_info_base_names ) {
      for (int i=0; i<f_bad_base_array_size; i++) {
        delete f_bad_info_base_names[i];
      }
      delete f_bad_info_base_names;
   }

   delete f_obj_dict;
}


/* *********************************************************/
// init all bases. play the trick by changing the db_path 
// value to load all info bases (each has different db_path).
/* *********************************************************/
   
info_base *
info_lib::_init_info_base( const char* base_path, 
                           const char* base_name, 
                           const char* base_desc,
                           const char* base_uid,
                           const char* base_locale,
                           const mm_version& v
                         )
{

/*
debug(cerr, base_path);
debug(cerr, base_name);
*/

//fprintf(stderr, "init_base\n");
//fprintf(stderr, "base_path=%s\n", base_path);
//fprintf(stderr, "base_name=%s\n", base_name);

   info_base *x = 0;

   if ( ( x = get_info_base(base_name)) == 0 ) {

     if ( exist_dir(base_path) == false )
        return 0;

//fprintf(stderr, "try to init %s\n", base_name);

     mtry {
        f_obj_dict -> init_a_base((char*)base_path, (char*)base_name);

        x = new info_base(*f_obj_dict, set_nm_list, list_nm_list,
                       base_path, base_name, base_desc, base_uid, base_locale,
                       v
                      );

        info_base_list.insert_as_tail(new dlist_void_ptr_cell(x));
     }

     mcatch (mmdbException &,e)
     {
//fprintf(stderr, "in catch block\n");
       return 0;
     } end_try;

   }

   return x;
}


/******************************************/
// 
// def_strings array:
//
//   def_strings[0] : infobase name
//   def_strings[1] : infobase textual description
//   def_strings[2] : define spec file name (full path)
//
/******************************************/

Boolean 
info_lib::define_info_base( char* base_name, char* base_desc, 
                            char* spec_file_path
                          )
{
//MESSAGE(cerr, "define_info_base()");
//debug(cerr, base_name);
//debug(cerr, base_desc);
//debug(cerr, spec_file_path);

   char new_db_path[PATHSIZ]; 
   char f_name[PATHSIZ]; 
   char base_uid[UIDSIZ]; 
   int len;
   const char* uid;

   len = MIN(strlen(info_lib_path) + strlen(base_name) + 1, PATHSIZ -1);
   *((char *) memcpy (new_db_path,
		      form("%s/%s", info_lib_path, base_name),
		      len) + len) = '\0';

   uid = unique_id();
   len = MIN(strlen(uid), UIDSIZ -1);
   *((char *) memcpy(base_uid, uid, len) + len) = '\0';

   g_mode_8_3 = 1;

   info_base* base = get_info_base(base_name) ;

/* no checking here. DDK assures unique base name case
   if ( base == 0 ) {
*/
      
//////////////////////////
// check info base path
//////////////////////////
      if ( check_and_create_dir(new_db_path) == false ) {
         throw(stringException(form("bad base bath %s", new_db_path)));
      }

//////////////////////////
// remove any old files 
//////////////////////////

      len = MIN(strlen(base_name) + strlen(DATA_FILE_SUFFIX) +1, PATHSIZ -1);
      *((char *) memcpy(f_name,
			form("%s.%s", base_name, DATA_FILE_SUFFIX),
			len) + len) = '\0';
      if ( exist_file(f_name, new_db_path) == true )
         del_file(f_name, new_db_path);

      len = MIN(strlen(base_name) + strlen(INDEX_FILE_SUFFIX) + 1, PATHSIZ -1);
      *((char *) memcpy(f_name,
			form("%s.%s", base_name, INDEX_FILE_SUFFIX),
			len) + len) = '\0';
      if ( exist_file(f_name, new_db_path) == true )
         del_file(f_name, new_db_path);

      len = MIN(strlen(base_name) + strlen(SCHEMA_FILE_SUFFIX) +1, PATHSIZ -1);
      *((char *) memcpy(f_name,
			form("%s.%s", base_name, SCHEMA_FILE_SUFFIX),
			len) + len) = '\0';
      if ( exist_file(f_name, new_db_path) == true )
         del_file(f_name, new_db_path);


      f_obj_dict -> init_a_base(spec_file_path, new_db_path, base_name);

      const char* lang;
      if ((lang = getenv("LC_ALL")) == NULL)
	if ((lang = getenv("LC_CTYPE")) == NULL)
	  if ((lang = getenv("LANG")) == NULL)
	    lang = "C";

#ifdef DtinfoClient
      _DtXlateDb db    = NULL;
      char* std_locale = NULL;

      if (_DtLcxOpenAllDbs(&db) == 0)
      {
	char platform[_DtPLATFORM_MAX_LEN + 1];
	int execver, compver;

	if (_DtXlateGetXlateEnv(db, platform, &execver, &compver) == 0)
	{
	  _DtLcxXlateOpToStd(db, platform, compver, DtLCX_OPER_SETLOCALE,
					lang, &std_locale, NULL, NULL, NULL);
	}
	_DtLcxCloseDb(&db);
	db = NULL;
      }
#endif

      base = new info_base(*f_obj_dict, set_nm_list, list_nm_list,
                           new_db_path, base_name, base_desc, base_uid,
#ifdef DtinfoClient
			   std_locale ? std_locale : lang,
#else
			   lang,
#endif
                           mm_version(MAJOR, MINOR)
                          );

      info_base_list.insert_as_tail(new dlist_void_ptr_cell(base));

/*************************************/
// add the base name and description
// to the names file
/*************************************/
      char* lib_nm = form("%s/%s", info_lib_path, MAP_FILE_8_3);

      fstream nm_out(lib_nm, ios::out | ios::app);
//    fstream nm_out(lib_nm, ios::app, open_file_prot());

      if ( !nm_out ) {
         MESSAGE(cerr, form("can't open %s/%s for append", 
                            info_lib_path, MAP_FILE_8_3)
                );
         throw(streamException(nm_out.rdstate()));
      }

      if ( bytes(lib_nm) == 0 ) {
         char* lib_entry = form("%s\t%s\n", info_lib_name, unique_id());

         if ( !(nm_out << lib_entry) ) {
            MESSAGE(cerr, 
	       form("write %s.%s failed", info_lib_path, MAP_FILE_8_3));
            throw(streamException(nm_out.rdstate()));
         }
      }

      char* base_entry = form("%s\t%s\t%s\t%s\t%d\t%d\n", 
                              base_name, base_desc, base_uid,
#ifdef DtinfoClient
			      std_locale ? std_locale: lang,
#else
			      lang,
#endif
			      MAJOR, MINOR
                             );
#ifdef DtinfoClient
      if (std_locale)
	free(std_locale);
#endif

      if ( !(nm_out << base_entry) ) {
         MESSAGE(cerr, form("write %s.%s failed", info_lib_path, MAP_FILE_8_3));
         throw(streamException(nm_out.rdstate()));
      }

      nm_out.close();

      if ( nm_out.fail() ) {
         MESSAGE(cerr, form("close %s.%s failed", info_lib_path, MAP_FILE_8_3));
         throw(streamException(nm_out.rdstate()));
      }

   //}


//MESSAGE(cerr, "define() done");
   return true;
}

info_base* info_lib::get_info_base(const char* info_base_nm)
{
   long ind = first();
//debug(cerr, ind);

   while ( ind ) {
     info_base* x = (*this)(ind);  

/*
debug(cerr, int(x));
debug(cerr, x -> base_name);
debug(cerr, info_base_nm);
*/

     if ( strcmp ( x -> base_name, info_base_nm) == 0 )
        return x;
     next(ind) ;
   }
   return 0;
}

/* inline */
/*
int info_lib::num_of_bases()
{
   return info_base_list.count();
}
*/

/*************************/
// iteration funcstions
/*************************/
/* inline */
/*
int info_lib::first()
{
   return info_base_list.first();
}

info_base* info_lib::operator()(int ind)
{
   return (info_base*)(((dlist_void_ptr_cell*)ind)->void_ptr());  
}

void info_lib::next(int& ind)
{
   info_base_list.next(ind) ;
}
*/


int info_lib::bad_infobases()
{
   return f_bad_info_bases;

}
   
const char* info_lib::get_bad_infobase_path(int x)
{
   if ( x <= 0 || x > f_bad_info_bases )
      return 0;

   return f_bad_info_base_paths[x-1];
}
   
const char* info_lib::get_bad_infobase_name(int x)
{
   if ( x <= 0 || x > f_bad_info_bases )
      return 0;

   return f_bad_info_base_names[x-1];
}


info_base* 
info_lib::getInfobaseByComponent(const char *locator_string, enum TestSelector sel)
{

   if ( locator_string == 0 )
      return 0;

   info_base* ib = 0;

   long ind = first();

   while ( ind ) {

      ib =  (*this)(ind);

      if (ib==0)
         throw(stringException("null info_base ptr"));

      mtry { // since an infobase may not have any graphics, we catch
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

      mcatch (mmdbException &,e)
      {
      } end_try;


      next(ind);
   }

   return 0;
}

info_base** 
info_lib::getInfobasesByComponent(char **locator_strings, int count, enum TestSelector sel)
{
   info_base** ibs = new info_basePtr[count];
   int i;
   for ( i=0; i<count; ibs[i++] = 0 );

   info_base* ib = 0;

   long ind = first();

   while ( ind ) {

      ib =  (*this)(ind);

      if (ib == 0)
         throw(stringException("null info_base ptr"));

      for ( i=0; i<count; i++ ) {

         mtry {
   
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

         mcatch (mmdbException &,e)
         {
         } end_try;

      }

      next(ind);
   }

   return ibs;
}

