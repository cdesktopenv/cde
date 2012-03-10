/* $XConsortium: vm_storage.h /main/4 1996/07/18 16:35:41 drk $ */

#ifndef _vm_storage_h
#define _vm_storage_h 1

#include "dstr/void_ptr_array.h"
#include "storage/abs_storage.h"

class vm_storage : public abs_storage
{
protected:
   void_ptr_array f_array;

public:
   vm_storage(char* path, char* name, rep_policy* rep_p = 0);
   ~vm_storage() ;

   void remove();

   // i/o functions
   int readString (mmdb_pos_t loc, char* base, int len, 
                  int str_offset = 0) ;
   int insertString(mmdb_pos_t& loc, const char* base, int len, Boolean flush = false) ;
   int get_str_ptr(mmdb_pos_t loc, char*&, int& len);

   int updateString(mmdb_pos_t loc, const char* base, int len, 
                    int string_ofst = 0, Boolean flush = false) ;
   int deleteString (mmdb_pos_t loc, Boolean flush = false) ;
   int allocString (mmdb_pos_t& loc, int len, char*&, int mode = 0);

   int appendString(mmdb_pos_t loc, const char*, int len,
                            Boolean flush_opt = false) ;

// status functions
   Boolean io_mode(int mode) ;

   friend class storage_mgr_t;

protected:
};

#endif
