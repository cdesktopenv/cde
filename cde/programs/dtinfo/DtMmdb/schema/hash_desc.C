// $XConsortium: hash_desc.cc /main/3 1996/06/11 17:32:08 cde-hal $

#include "schema/hash_desc.h"
#include "handler/fast_mphf_handler.h"


mphf_desc::mphf_desc() : stored_object_desc(FAST_MPHF_CODE, "index_agent	mphf")
{
}

handler* mphf_desc::init_handler(object_dict& dict) 
{
   page_storage* store = (page_storage*)dict.get_store(store_nm);

   if ( v_oid.icode() == 0 ) {
      v_handler_ptr = new handler(FAST_MPHF_CODE, store);
      desc::set_oid(v_handler_ptr -> its_oid());
   } else
      v_handler_ptr = new fast_mphf_handler(v_oid, store);

   return v_handler_ptr;
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////

btree_desc::btree_desc() : stored_object_desc(BTREE_CODE, "index_agent	btree")
{
}

handler* btree_desc::init_handler(object_dict& dict) 
{
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////

smphf_desc::smphf_desc() : stored_object_desc(DYN_DISK_HASH_CODE, "index_agent	smphf")
{
}

handler* smphf_desc::init_handler(object_dict& dict) 
{
}

