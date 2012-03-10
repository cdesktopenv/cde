/*
 * $XConsortium: mark.cc /main/6 1996/07/18 14:47:10 drk $
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



#include "oliasdb/mark.h"

umark::umark() : tuple(2, USER_MARK_CODE) 
{
}


MMDB_BODIES(umark)

mark_smart_ptr::
mark_smart_ptr(mark_base* uptr, const char* node_locator) :
	mbase(uptr), 
        v_loct_hd(0), v_mark_hd(0)
{
#ifdef INTERCEPT_MARK_CALLS
MESSAGE(cerr, "mark_smart_ptr::cstr (1)");
debug(cerr, uptr -> get_base_name());
debug(cerr, node_locator);
#endif

   mark_set_hd_ptr = uptr->mark_set_hd;

   try {
      mbase -> trans().begin();
   
      abs_storage* marks_store = mark_set_hd_ptr -> its_store();

   /////////////////////
   // init tuple part
   /////////////////////

      tuple_handler* x = (tuple_handler*)
        (new handler(USER_MARK_CODE, marks_store));

   
      v_loct_hd = 
         new pstring_handler(node_locator, strlen(node_locator), marks_store);

//(v_loct_hd->operator->())->asciiOut(cerr);
   
      v_mark_hd = 
         new pstring_handler("", 0, marks_store);
/*
debug(cerr, v_loct_hd -> its_oid());
debug(cerr, *v_loct_hd);
debug(cerr, int(v_loct_hd));
debug(cerr, v_mark_hd -> its_oid());
debug(cerr, *v_mark_hd);
debug(cerr, int(v_mark_hd));
*/  

      (*x) -> pinned_insert(BASE_COMPONENT_INDEX, v_loct_hd -> its_oid());
      (*x) -> pinned_insert(BASE_COMPONENT_INDEX+1, v_mark_hd -> its_oid());
      
      x -> commit();
   /////////////////////
   // init smart_ptr
   /////////////////////

      smart_ptr::_init(x->its_oid(), marks_store);
   
   ////////////////////////
   // insert into mark set
   ////////////////////////
      v_loct_hd -> commit(); // commit it so that the locator
			     // can be seen inside insert_object(). 
      (*mark_set_hd_ptr) -> insert_object( *x );

      mark_set_hd_ptr -> commit();
   
   //debug(cerr, x -> its_oid());

     delete x;
   
     mbase -> trans().end();
   }

   catch (beginTransException&, e)
      {
         smart_ptr::_init(ground, 0); // mark the mark obsolete
         rethrow;
      }
   catch (commitTransException&, e)
      {
         smart_ptr::_init(ground, 0); // mark the mark obsolete
         rethrow;
      }
   catch (mmdbException&, e)
      {
         smart_ptr::_init(ground, 0); // mark the mark obsolete
         mbase -> trans().rollback();
         rethrow;
      }
   end_try;
}

mark_smart_ptr::
mark_smart_ptr(mark_base* uptr, const oid_t& mark_id) :
   smart_ptr(uptr->mark_set_hd->its_store(), mark_id), 
   mbase(uptr), mark_set_hd_ptr(uptr -> mark_set_hd)
{
#ifdef INTERCEPT_MARK_CALLS
MESSAGE(cerr, "mark_smart_ptr::cstr (2)");
debug(cerr, uptr -> get_base_name());
debug(cerr, mark_id);
#endif

   try 
   {
      v_loct_hd = (pstring_handler*)
               get_handler(BASE_COMPONENT_INDEX, STRING_CODE);
      v_mark_hd = (pstring_handler*)
               get_handler(BASE_COMPONENT_INDEX+1, STRING_CODE);
   }

   catch (mmdbException&, e)
      {
         smart_ptr::_init(ground, 0); // mark the mark obsolete
         rethrow;
      }
   end_try;

}

mark_smart_ptr::~mark_smart_ptr()
{
   delete v_loct_hd;
   delete v_mark_hd;
}

void mark_smart_ptr::update_usermark(const pstring& new_mark)
{
   update_usermark(((pstring&)new_mark).get(), ((pstring&)new_mark).size());
}

void mark_smart_ptr::update_usermark(const char* new_mark_str,
                              int new_mark_str_sz)
{
   try {
      mbase -> trans().begin();

      (*v_mark_hd) -> update(new_mark_str, new_mark_str_sz);
      v_mark_hd -> commit(); 

      mbase -> trans().end();
   }

   catch (beginTransException&, e)
      {
         rethrow;
      }
   catch (commitTransException&, e)
      {
         rethrow;
      }
   catch (mmdbException&, e)
      {
         mbase -> trans().rollback();
         rethrow;
      }
   end_try;
}

void mark_smart_ptr::remove_from_db()
{
   try {
      mbase -> trans().begin();

      (*mark_set_hd_ptr) -> remove_component(its_oid()); 
      mark_set_hd_ptr -> commit(); 

      mbase -> trans().end();
   }

   catch (beginTransException&, e)
      {
         rethrow;
      }
   catch (commitTransException&, e)
      {
         rethrow;
      }
   catch (mmdbException&, e)
      {
         mbase -> trans().rollback();
         rethrow;
      }
   end_try;
}

const char* mark_smart_ptr::node_locator()
{
   return (*v_loct_hd) -> get();
}

pstring* mark_smart_ptr::mark_value()
{
/*
pstring *z  = v_mark_hd -> operator->();
debug(cerr, z -> size());
*/

   return v_mark_hd -> operator->();
}

ostream& operator <<(ostream& out, mark_smart_ptr& umark)
{
   const char* x = umark.node_locator();
   if ( x )
      out << "node_locator: " <<  x << "\n";

   out << "mark_value: ";

   umark.mark_value() -> asciiOut(out);

//   out << "\n";

   return out;
}
