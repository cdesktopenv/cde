/* $XConsortium: c_api_infolib.cc /main/5 1996/07/18 16:01:28 drk $ */

#include "oliasdb/c_api_common.h"

//extern void_ptr_array* infolib_array;
extern void_ptr_array* bookcase_array;
extern OLIAS_DB* mmdb_ptr;

//extern olias_server* oserver_ptr;
      
void _assign_bookcase_index_id(info_lib* x)
{
  long ind = x -> first();
  info_base* base;
  while (ind) {
    base = (*x)(ind);

// no duplication check yet
    int i = bookcase_array -> no_elmts();
    bookcase_array -> insert(base, i);
    bookcase_array -> reset_elmts(i+1);

    base -> set_index_id(i);
    x -> next(ind);
  }
}

int 
DtMmdbOpenInfoLib(
	const char* infolib_path,
	const char* selected_base_name,
	DtMmdbBool delayed_infolib_init
               )
{
   try {

      Boolean option = (delayed_infolib_init == DtMmdbTrue) ? true : false;

      mmdb_ptr = new OLIAS_DB;
      info_lib* x = mmdb_ptr -> openInfoLib(infolib_path, selected_base_name);
   

      int i = x -> descriptor();
/*
      int i = infolib_array -> no_elmts();
      infolib_array -> insert(x, i);
      infolib_array -> reset_elmts(i+1);
*/

      _assign_bookcase_index_id(x);
   
      return i;
   }

   catch (mmdbException &,e)
   {
      return -1;
   } end_try;

   return -1;
}

void DtMmdbCloseInfoLib(int infolib_descriptor)
{
   delete mmdb_ptr;
   mmdb_ptr = 0;

   //infolib_array -> reset_vptr(0);
}


DtMmdbInfoLibInfo* 
DtMmdbInfoLibGetInfo(int infolib_descriptor)
{
   try {
        info_lib* x = mmdb_ptr -> getInfoLib(infolib_descriptor);
   
        if ( x == 0 ) return 0;
   
        DtMmdbInfoLibInfo *y = 
            (DtMmdbInfoLibInfo*)malloc(sizeof(DtMmdbInfoLibInfo)); 
   
        if ( y == 0 ) return 0;
   
        y -> path = x -> get_info_lib_path();
        y -> name = x -> get_info_lib_name();
        y -> num_bookcases = x -> num_of_bases();
   
        return y;
   
   }
   catch (mmdbException &,e)
   {
      return 0;
   } end_try;

   return 0;
}

void DtMmdbInfoLibFreeInfo(DtMmdbInfoLibInfo* x)
{
   free((void*)x);
}


