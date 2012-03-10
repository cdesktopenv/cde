/*
 * $XConsortium: unixf_storage.h /main/5 1996/07/18 14:56:35 drk $
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


#ifndef _unixf_storage_h
#define _unixf_storage_h 1

#include <sys/types.h>
#include <sys/stat.h>

#include "utility/funcs.h"
#include "storage/lru.h"
#include "storage/abs_storage.h"

extern lru unixf_mgr;

//class unixf_storage : public rep_cell, public abs_storage, public fstream 
class unixf_storage : public abs_storage, public fstream 
{

protected:
   int mode;
   int total_bytes;
   int _open(int mode);
   Boolean v_file_exist;

public:
// mode: see ios::in etc. stuff in file iostream.h 
   unixf_storage( char* file_path, 
                  char* file_nm, 
                  rep_policy*,
                  int md = ios::in | ios::out 
                );
   virtual ~unixf_storage() ;

   virtual void remove() ;

// IO functions
   int readString (mmdb_pos_t loc, char*, int len, int str_offset = 0);
   int appendString(mmdb_pos_t loc, const char*, int len, Boolean flush_opt = false);
   int updateString(mmdb_pos_t loc, const char* base, int len, int string_ofst = 0, Boolean flush = false);


// non-applicable funcs
   int insertString(mmdb_pos_t& , const char* , int, Boolean = false)
       { return -1; };
   int get_str_ptr(mmdb_pos_t, char*&, int& ) 
       { return -1; };

   int deleteString (mmdb_pos_t, Boolean = false ) 
       { return -1; };
   int allocString (mmdb_pos_t& , int, char*&, int = 0)
       { return -1; };
   virtual int truncate(int target_length_in_bytes);

   Boolean io_mode(int mode) ;
   int bytes() ;

   friend class storage_mgr_t;
   friend class ld_dyn_hash;
   friend class handler;
};

typedef unixf_storage* unixf_storagePtr;

#endif
