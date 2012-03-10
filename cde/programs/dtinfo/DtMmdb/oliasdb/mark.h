/*
 * $XConsortium: mark.h /main/4 1996/06/11 17:29:36 cde-hal $
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



#ifndef _mark_h
#define _mark_h 1

#include "object/tuple.h"
#include "object/pstring.h"
#include "object/oid.h"
#include "oliasdb/olias_consts.h"
#include "oliasdb/mark_base.h"
#include "api/smart_ptr.h"

#define mark_smart_ptr user_mark_smart_ptr
#define umark user_mark

class umark : public tuple
{

public:
   umark();
   virtual ~umark() {};

   MMDB_SIGNATURES(umark);

   friend class mark_smart_ptr;

protected:
   cset_handler* mark_set_hd_ptr;

};

class mark_smart_ptr : public smart_ptr
{
public:
   mark_smart_ptr(mark_base*, const char* node_locator);
   mark_smart_ptr(mark_base*, const oid_t& mark_oid);
   virtual ~mark_smart_ptr();

   void update_usermark(const pstring& new_um);
   void update_usermark(const char* mark, int mark_sz);

   void remove_from_db();

   const char* node_locator();
   pstring* mark_value(); 

   friend ostream& operator <<(ostream&, mark_smart_ptr&);

protected:

protected:
   mark_base* mbase;
   cset_handler* mark_set_hd_ptr;

   pstring_handler* v_loct_hd;
   pstring_handler* v_mark_hd;
};

#endif
