/*
 * $XConsortium: inv_desc.cc /main/4 1996/06/11 17:32:27 cde-hal $
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


#include "schema/inv_desc.h"
#include "index/inv_lists.h"

inv_desc::inv_desc() : stored_object_desc(INV_LISTS_CODE, "inv	inv")
{
}

handler* inv_desc::init_handler(object_dict& dict)
{
   page_storage* store = (page_storage*)dict.get_store(get_store_nm());

   if ( v_oid.icode() == 0 ) {
      v_handler_ptr = new handler(INV_LISTS_CODE, store);
      desc::set_oid(v_handler_ptr -> its_oid());
   } else
      v_handler_ptr = new inv_lists_handler(v_oid, store);

   return v_handler_ptr;
}


