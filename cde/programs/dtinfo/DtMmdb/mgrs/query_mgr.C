/*
 * $XConsortium: query_mgr.cc /main/3 1996/06/11 17:23:04 cde-hal $
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


#include "mgrs/query_mgr.h"

query_mgr_t::query_mgr_t() : 
   v_pstring(new pstring(v_char_buffer, 256)), 
   v_integer(new integer), v_oid(new oid)
{
}

query_mgr_t::~query_mgr_t()
{
   v_handler.set(0, 0);
   delete v_pstring;
   delete v_integer;
   delete v_oid;
}


handler& query_mgr_t::form_pstring_handler(const char* q_str)
{
   v_pstring -> update(q_str, strlen(q_str));
   v_handler.set(v_pstring, 0);
   return v_handler;
}

handler& query_mgr_t::form_integer_handler(int q_int)
{
   v_integer -> set(q_int);
   v_handler.set(v_integer, 0);
   return v_handler;
}

handler& query_mgr_t::form_oid_handler(const oid_t& q_oid)
{
   v_oid -> set(q_oid);
   v_handler.set(v_oid, 0);
   return v_handler;
}

