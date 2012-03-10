/*
 * $XConsortium: storage.h /main/5 1996/08/21 15:53:54 drk $
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

#ifdef C_API
#include "utility/c_fstream.h"
#else
#include <fstream.h>
#endif

#include "utility/macro.h"
#include "storage/lru.h"
#include "storage/storage.h"

extern lru unixf_mgr;

class storage : public rep_cell, public root, private fstream 
{

protected:
   int mode;
   char name[PATHSIZ];
   rep_policy *policy;

   int _open();

public:
// mode: see ios::in etc. stuff in file iostream.h 
   unixf_storage(char* filenm, int init_buf_sz = LBUFSIZ, 
                 int md = ios::in | ios::out, rep_policy* = 0
                );
   virtual ~unixf_storage() ;

// IO functions
   virtual int readString (mmdb_pos_t loc, char*&, int len, int str_offset = 0);
   virtual int appendString(mmdb_pos_t& loc, char*, int len);
   virtual int updateString(mmdb_pos_t loc, char* base, int len, int string_ofst = 0);
   virtual int allocString (mmdb_pos_t&, int, char*&, int = 0) = 0;

// status function
   const char* my_nick_name() ;   // name of the unix file
   int bytes() ;
};

typedef storage* storagePtr;

#endif
