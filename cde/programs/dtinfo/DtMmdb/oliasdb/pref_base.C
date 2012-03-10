/*
 * $XConsortium: pref_base.cc /main/3 1996/06/11 17:30:37 cde-hal $
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



#include "oliasdb/pref_base.h"
#include "utility/debug.h"
#include "unique_id.h"

pref_base::pref_base(user_base::rw_flag_t rw) : 
	user_base(PREF_SPEC, rw)
{
}


pref_base::pref_base( const char* base_dir, 
                      const char* base_nm,
                      const char* base_ds,
		      user_base::rw_flag_t rw 
                    ) : 
	user_base(base_dir, base_nm, base_ds, PREF_SPEC, rw) 
{
   if ( checking_status != SUCC )
      return;

   desc* ptr = first_desc_ptr;

   if ( ptr == 0 )
      throw(stringException("empty pref base specification"));

   while ( ptr ) {
     if ( strcmp( ptr -> get_type(), "container	set") == 0 ) {
        pref_set_hd = (cset_handler*)
            (managers::obj_dict -> get_handler(ptr -> get_nm()));
        return;
     }
     ptr = ptr -> get_next_desc();
   }

   throw(stringException("can't find pref set description record"));
}

pref_base::~pref_base()
{
}

