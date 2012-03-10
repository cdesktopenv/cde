/*
 * $XConsortium: transaction.h /main/4 1996/06/11 17:11:50 cde-hal $
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


#ifndef _transaction_h
#define _transaction_h 1

#include <stdarg.h>
#include "dstr/bset.h"
#include "object/oid_t.h"
#include "storage/page_storage.h"
#include "mgrs/misc.h"

class transaction 
{

public:
   transaction();
   ~transaction(); 

   void book(oid_t&, abs_storage*);
   void book(abs_storage*);

   void begin();
   void end();

   void sync(); // checkpoint in odmg93
   void rollback();
   void abort();


protected:
   bset v_store_array;
   bset v_updated_objects;    
};

extern transaction* g_transac;

#endif
