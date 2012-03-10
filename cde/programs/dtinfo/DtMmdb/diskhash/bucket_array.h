/*
 * $XConsortium: bucket_array.h /main/3 1996/06/11 17:15:56 cde-hal $
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



#ifndef _bucket_array_h
#define _bucket_array_h

#include "utility/funcs.h"
#include "diskhash/disk_bucket.h"

class bucket_array 
{
public:
   bucket_array(int buckets, page_storage* key_store);
   virtual ~bucket_array();

   void reset(); 
   Boolean expandWith(int extra_buckets);

   disk_bucket& get_bucket(int i);
   int count() { return v_buckets; };

   ostream& asciiOut(ostream& out, print_func_ptr_t print_f);

   friend ostream& operator<<(ostream&, bucket_array&);
   friend class disk_hash;

protected:
   int v_buckets;
   disk_bucket* v_cached_bucket_ptr;
   page_storage* v_key_store;
};

typedef bucket_array* bucket_arrayPtr;

#endif
