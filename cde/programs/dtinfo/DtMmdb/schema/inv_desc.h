/* $XConsortium: inv_desc.h /main/3 1996/06/11 17:32:31 cde-hal $ */

#ifndef _inv_desc_h
#define _inv_desc_h 1

#include "schema/stored_object_desc.h"
#include "schema/object_dict.h"

class inv_desc : public stored_object_desc {

public:
   inv_desc();
   virtual ~inv_desc() {};

   handler* init_handler(object_dict&) ;
 
protected:
};


#endif
