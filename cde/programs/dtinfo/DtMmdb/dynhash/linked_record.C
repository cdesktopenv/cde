// $XConsortium: linked_record.cc /main/3 1996/06/11 17:19:38 cde-hal $
   
#include "dynhash/linked_record.h"

linked_record::linked_record() 
{ 
   next = 0; 
}

linked_record::linked_record(data_t& v) : data_t(v) 
{ 
   next = 0; 
}
   
linked_record& linked_record::operator = (linked_record& d) 
{  
   data_t::operator =(d) ;
   next = d.next ;  
   return *this;
}
   
linked_record::~linked_record() 
{
}
