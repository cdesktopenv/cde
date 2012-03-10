/*
 * $XConsortium: oid.h /main/5 1996/07/18 14:42:55 drk $
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


#ifndef _oid_h
#define _oid_h 1

#include "object/primitive.h"

/*************************************/
// The oid class
// containing an oid of some object.
/*************************************/

class oid : public primitive
{

public:
   oid(c_code_t c_id = OID_CODE);
   oid(const oid_t&);
   oid(const oid&) ;
   virtual ~oid() {};

   void set(const oid_t& x) { v_oid.become(x); } ;

   MMDB_SIGNATURES(oid);

/*
// comparison functions: value comparison 
   virtual Boolean value_LS(root&, Boolean safe = true) const ;
   virtual Boolean value_EQ(root&, Boolean safe = true) const ;
*/

// export functions 
   virtual io_status asciiOut(ostream&) ;
   virtual io_status asciiIn(istream&) ;

// compacted disk representation In and Out functions
   virtual int cdr_sizeof();
   virtual io_status cdrOut(buffer&);
   virtual io_status cdrIn(buffer&);


// get the oid
   oid_t& my_coid() { return v_oid; };

protected:
   oid_t v_oid;
};

typedef oid* oidPtr;

   
HANDLER_SIGNATURES(oid)

#endif
