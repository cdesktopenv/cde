/*
 * $XConsortium: atoi_pearson.h /main/3 1996/06/11 17:35:50 cde-hal $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */


#ifndef _atoi_pearson_h
#define _atoi_pearson_h 1

#include "utility/funcs.h"
#include "utility/pm_random.h"
#include "utility/key.h"

// Based on Pearson's algorithm presented in CACM 90/6.
// rewritten to speed up atoi()

class atoi_pearson 
{


public:
   atoi_pearson(int _range, int _entries) ;
   atoi_pearson(int _range, int _entries, pm_random&) ;
   atoi_pearson(int _range, int _entries, char* v_tbl) ;
   virtual ~atoi_pearson() ;

   virtual int atoi(const key_type& k, int offset = 0) const ;
   int atoi(const char* str, int _offset = 0, int range = 0) const ;
   int atoi(const char* str, int sz, int _offset = 0, int range = 0) const ;

   int size() { return v_entries; } ;
   char* tblPtr() { return v_tbl; } ;

   friend ostream& operator<<(ostream&, atoi_pearson&);

private:
   void init(int _range, int _entries, pm_random&);

protected:
   char *v_tbl;
   Boolean v_shared;

   unsigned v_mask;
   unsigned int v_entries;
   unsigned int v_range;
   unsigned int v_no_bytes;
};


#endif
