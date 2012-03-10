/*
 * $XConsortium: inv_lists.h /main/5 1996/07/18 14:38:10 drk $
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


#ifndef _inv_lists_h
#define _inv_lists_h 1

#include "storage/unixf_storage.h"
#include "object/oid_list.h"

class inv_lists : public oid_list
{

public:
   inv_lists(c_code_t = INV_LISTS_CODE) ;
   virtual ~inv_lists() ;

   MMDB_SIGNATURES(inv_lists);

// get the invlist
   virtual oid_list_handler* get_list(int index) ;
//   virtual oid_list_handler* get_list(mmdb_pos_t pod) ;

// update functions
   Boolean append_list(oid_list&);
   Boolean insert_list_pinned(int index, oid_list&);
   Boolean remove_list(int index);
   void insert_to_list(int index, oid_t& id);

// I/O function
   friend ostream& operator <<(ostream&, inv_lists&) ;

};

HANDLER_SIGNATURES(inv_lists)

#endif
