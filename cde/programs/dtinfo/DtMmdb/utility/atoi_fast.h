/*
 * $XConsortium: atoi_fast.h /main/3 1996/06/11 17:35:30 cde-hal $
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


#ifndef _atoi_fast_h
#define _atoi_fast_h 1

#include "utility/funcs.h"
#include "utility/pm_random.h"
#include "utility/key.h"

class atoi_fast 
{

public:
   atoi_fast(int _range, int _entries) ;
   atoi_fast(int _range, int _entries, pm_random&) ;
   virtual ~atoi_fast() ;

   virtual int atoi(const key_type& k, int offset = 0) const ;
   int atoi(const char* str, int _offset = 0, int range = 0) const ;
   int atoi(const char* str, int sz, int _offset = 0, int range = 0) const ;

   int size() { return v_entries; } ;

   friend ostream& operator<<(ostream&, atoi_fast&);

private:
   void init(int _range, int _entries, pm_random&);

protected:
   char *v_tbl;
   unsigned v_mask;
   unsigned int v_entries;
   unsigned int v_range;
   unsigned int v_no_bytes;
};


#endif
