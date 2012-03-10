/* $XConsortium: db_oid.h /main/3 1996/06/11 17:11:06 cde-hal $ */


#ifndef _db_oid_h
#define _db_oid_h 1

#include "object/oid_t.h"

/*************************************/
// The db_oid class
/*************************************/

class db_oid : public oid_t
{

public:
   db_oid() {};
   db_oid(oid_t& x) : oid_t(x) {};
   virtual ~db_oid() {};

   int index_num;
};

#endif
