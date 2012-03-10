/*
 * $XConsortium: short_list.h /main/5 1996/07/18 14:46:30 drk $
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


#ifndef _short_list_h
#define _short_list_h 1

#include "object/oid_list.h"

/***************************************
* short_list class.
****************************************/

class short_list: public oid_list
{

public:

   short_list(rootPtr tempt = 0, c_code_t = SHORT_LIST_CODE);
   short_list(short_list&);
   virtual ~short_list();

   MMDB_SIGNATURES(short_list);

/*
// value comparison functions
   virtual Boolean value_EQ(root&, Boolean safe = true) const ;
   virtual Boolean value_LS(root&, Boolean safe = true) const ;
*/

// insert component function
   Boolean pinned_insert(int index, oid_t&);

// iteration over components
   virtual handler* get_component(int index) ;
   //virtual handler* get_component(oid_t& comp_oid) ;

// get template
   virtual rootPtr get_template();

// print function
   virtual io_status asciiOut(ostream&) ;
   virtual io_status asciiIn(istream&) ;

protected:
   rootPtr v_tempt;

};
   
   
HANDLER_SIGNATURES(short_list)

#endif
