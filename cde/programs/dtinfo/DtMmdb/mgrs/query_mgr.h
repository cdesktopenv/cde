/*
 * $XConsortium: query_mgr.h /main/4 1996/06/11 17:23:09 cde-hal $
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


#ifndef _query_mgr
#define _query_mgr 1

#include "object/handler.h"
#include "object/pstring.h"
#include "object/integer.h"
#include "object/oid.h"

class query_mgr_t
{

public:
   query_mgr_t();
   virtual ~query_mgr_t();

   handler& form_pstring_handler(const char* q_string);
   handler& form_integer_handler(int q_int);
   handler& form_oid_handler(const oid_t& q_oid);

protected:
   char v_char_buffer[LBUFSIZ];

   pstring* v_pstring;
   integer* v_integer;
   oid* v_oid;

   handler v_handler;
};

extern query_mgr_t* query_mgr;


#endif
