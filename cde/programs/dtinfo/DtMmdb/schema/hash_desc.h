/* $XConsortium: hash_desc.h /main/3 1996/06/11 17:32:12 cde-hal $ */

#ifndef _hash_desc_h
#define _hash_desc_h 1

#include "schema/stored_object_desc.h"
#include "schema/object_dict.h"

class mphf_desc : public stored_object_desc {

public:
   mphf_desc();
   ~mphf_desc() {};

   handler* init_handler(object_dict&) ;

protected:
};

class btree_desc : public stored_object_desc {

public:
   btree_desc();
   ~btree_desc() {};

   handler* init_handler(object_dict&) ;
protected:
};

class smphf_desc : public stored_object_desc {

public:
   smphf_desc();
   ~smphf_desc() {};

   handler* init_handler(object_dict&) ;
protected:
};


#endif
