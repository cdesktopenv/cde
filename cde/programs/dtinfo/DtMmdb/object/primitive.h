/*
 * $XConsortium: primitive.h /main/4 1996/06/11 17:25:31 cde-hal $
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


#ifndef _primitive_h
#define _primitive_h 1

#include "object/root.h"
#include "storage/page_storage.h"
#include "object/handler.h"

/******************************************
* primitive object class
*******************************************/

class primitive : public root 
{
public:
   NEW_AND_DELETE_SIGNATURES(primitive);

   primitive(c_code_t c_id = PRIMITIVE_CODE);
   primitive(const primitive& x) ;
   virtual ~primitive() {};

   void set_mode(obj_mode_t, Boolean);

   Boolean swap_order() {
     return (storage_ptr) ? storage_ptr -> swap_order() : false;
   };

   MMDB_SIGNATURES(primitive);
};

#endif
