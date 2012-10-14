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
// $XConsortium: mmdb.C /main/7 1996/10/26 18:19:36 cde-hal $

#include "api/utility.h"
#include "utility/db_version.h"
#include "misc/unique_id.h"
#include "oliasdb/olias_funcs.h"
#include "oliasdb/collectionIterator.h"
#include "oliasdb/mmdb.h"
#include "mgrs/managers.h"
#include "api/base.h" // for UIDSIZ define

OLIAS_DB::OLIAS_DB() : infolib_array(20)
{
//fprintf(stderr, "OLIAS_DB::OLIAS_DB()");
   managers::template_mgr -> insert_template(&dlp_template);
   managers::template_mgr -> insert_template(&doc_template);
   managers::template_mgr -> insert_template(&graphic_template);
   managers::template_mgr -> insert_template(&locator_template); 
   managers::template_mgr -> insert_template(&node_template); 
   managers::template_mgr -> insert_template(&toc_template); 
   managers::template_mgr -> insert_template(&mark_template); 
   //managers::template_mgr -> insert_template(&pref_template); 
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
}

OLIAS_DB::~OLIAS_DB()
{
   info_lib* x = 0;

   for ( int i=0; i<infolib_array.no_elmts(); i++ ) {
      x = (info_lib*)(infolib_array)[i];
      delete x;
   }

   delete info_base_set_names;
   delete info_base_list_names;
}

int OLIAS_DB::validInfoLibPath(const char* path)
{
  mtry
   {
     if ( exist_dir(path) == false )
       return 0;
      
     if ( exist_file(MAP_FILE, path) == true )
       return 1;
      
     if ( exist_file(MAP_FILE_8_3, path) == true )
       return 1;
   
   }

   mcatch (mmdbException &,e)
   {
      return 0;
   }
   end_try;

   return 0;
}

info_lib* 
OLIAS_DB::openInfoLib(const char* infoLibPath, const char* selectedBaseName,
		  const char* infoLibName) 
{
   int i;
   for ( i=0; i<infolib_array.no_elmts(); i++ ) {
      if ( infolib_array[i] == 0 )
        break;
   }

   info_lib* x = new info_lib(
                       info_base_set_names, info_base_list_names,
                       (char*)infoLibPath, (char*)selectedBaseName,
	               (char*)infoLibName, i 
                      );

  
   // see if infolib already exists in mmdb, if it does, just
   // return a pointer to it.
   int idx = 0;
   info_lib* y;

   y = getInfoLib(x->get_info_lib_uid(), idx);
   if (y)
   {
     delete x;
     return y;
   }

   infolib_array.insert(x, i);
   infolib_array.reset_elmts(i+1);

   return x;
}

void OLIAS_DB::closeInfoLib(const char* infoLibUid)
{
   int i = 0;
   info_lib* x = getInfoLib(infoLibUid, i);

   if ( x ) {
      infolib_array.insert(0, i);
      delete x;
   }
}

info_lib* OLIAS_DB::getInfoLib(int i)
{
   if ( 0 <= i && i < infolib_array.no_elmts() )
   {
      return (info_lib*)infolib_array[i];
   } else
      return 0;
}

info_lib* OLIAS_DB::getInfoLib(const char* infoLibUid, int& i)
{
   info_lib* x = 0;
   for ( i=0; i<infolib_array.no_elmts(); i++ ) {
      x = (info_lib*)(infolib_array)[i];
      //if ( x && strcmp(x -> get_info_lib_name(), infoLibName) == 0 ) 
      if ( x && strcmp(x -> get_info_lib_uid(), infoLibUid) == 0 ) 
         return x;
   }
   return 0;
}

char* OLIAS_DB::getInfoLibUid(char* infoLibPath)
{
   static char info_lib_uid[UIDSIZ];
   char info_lib_name[PATHSIZ];

   if ( exist_file(MAP_FILE_8_3, infoLibPath) == true ) {
      fstream map_in(form("%s/%s", infoLibPath, MAP_FILE_8_3), ios::in);

      if ( !map_in.getline(info_lib_name, PATHSIZ, '\t') ||
           !map_in.getline(info_lib_uid, UIDSIZ, '\n') )
        return 0;

      return info_lib_uid;
   }
  return 0;
}

      
Boolean
OLIAS_DB::real_destroy_info_base(const char* info_lib_path, const char* base_name)
{
   if ( unlink(form("%s/%s/%s.%s", info_lib_path, base_name, base_name, DATA_FILE_SUFFIX)) != 0 )
      return false;

   if ( unlink(form("%s/%s/%s.%s", info_lib_path, base_name, base_name, INDEX_FILE_SUFFIX)) != 0 )
      return false;

   if ( unlink(form("%s/%s/%s.%s", info_lib_path, base_name, base_name, SCHEMA_FILE_SUFFIX)) != 0 )
      return false;

   if ( rmdir(form("%s/%s", info_lib_path, base_name)) != 0 )
      return false;

   return true;
}

Boolean OLIAS_DB::real_destroy(const char* infoLibPath, const char* infoLibName)
{
// remove all infobases in the dir

   char base_name[PATHSIZ];
   char base_desc[PATHSIZ];
   char buf[PATHSIZ];

   int major_mm_version = 0;
   int minor_mm_version = 0;

   sprintf(buf, "%s/%s", infoLibPath, MAP_FILE_8_3);
   fstream in(buf, ios::in);

   while ( in.getline(base_name, PATHSIZ, '\t') ) {
   
         in.getline(base_desc, PATHSIZ, '\t');
   
         in >> major_mm_version >> minor_mm_version;
   
         in.get();
   
         if ( real_destroy_info_base(infoLibPath, base_name) != true )
            return false;
   }
   
// remove map file
   if ( unlink(buf) != 0 )
      return false;
   else
      return true;
}

Boolean OLIAS_DB::destroy(const char* infoLibPath, const char* infoLibName)
{
   int i = 0;
   if ( getInfoLib(infoLibName, i) ) {
      if ( infolib_array[i] )
         return false;
   
      return real_destroy(infoLibPath, infoLibName);
   }

   return real_destroy(infoLibPath, infoLibName);
}

int OLIAS_DB::major_code_version()
{
   return MAJOR;
}

int OLIAS_DB::minor_code_version()
{
   return MINOR;
}

mm_version OLIAS_DB::code_version()
{
   return mm_version(MAJOR, MINOR);
}

