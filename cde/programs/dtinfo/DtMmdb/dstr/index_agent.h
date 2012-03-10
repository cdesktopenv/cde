/*
 * $XConsortium: index_agent.h /main/3 1996/06/11 17:17:20 cde-hal $
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



#ifndef _index_agent_h
#define _index_agent_h 1

#include  "utility/funcs.h"
#include  "utility/ostring.h"
#include  "dynhash/data_t.h"

//**************************************************************
// A abstract index_agent class
//**************************************************************

class index_agent {

public:
   index_agent() ;
   virtual ~index_agent() ;

   virtual Boolean insert(data_t& v) = 0;  // insert a key
   virtual Boolean remove(data_t& v) = 0;  // remove a key
   virtual Boolean member(data_t& v) = 0; // member test
   virtual void clean() = 0; 		   // remove all keys

   virtual int no_keys() const { return n; }; // return key set size

   virtual ostream& asciiOut(ostream& out) = 0;
   virtual istream& asciiIn(istream& in) = 0;

protected:
   unsigned int n;          // current key set size

};

#endif
