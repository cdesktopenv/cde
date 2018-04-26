/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * $XConsortium: root.cc /main/5 1996/07/18 14:45:50 drk $
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

   
#include "object/root.h"
#include "api/transaction.h"

persistent_info::persistent_info(abs_storage* s, c_code_t c,
       mmdb_pos_t p, Boolean per, Boolean d)
{
   cdr = false;
   storage = s;
   class_code = c;
   position = p;
   persistent = per;
   old_object = d;
}

persistent_info transient_info;

root::root(c_code_t c_id) : f_oid(c_id, 0)
{
   init_persistent_info();
}

root::root(const oid_t& x ) : f_oid(x)
{
   init_persistent_info();
}

root::root(const root& x) : f_oid(x.f_oid), status(x.status)
{
   init_persistent_info();
}

void root::init_persistent_info(persistent_info* pinfo)
{
/*
MESSAGE(cerr, "STATUS before set");
debug(cerr, int(get_mode(PERSISTENT)));
debug(cerr, int(get_mode(BASE_DATA_INITED)));
debug(cerr, int(storage_ptr));
*/

   status.ok = TOBIT(true);
   status.ref_count = 0;

   storage_ptr = pinfo -> storage;
   set_mode(CDR, pinfo -> cdr);
   set_mode(PERSISTENT, pinfo -> persistent);
   set_mode(SWAP_ALLOWED, true);

   switch ( pinfo -> persistent ) {

     case true: 		// disk object case
      f_oid.v_i_code = pinfo -> position;
      set_mode(OLD_OBJECT, pinfo -> old_object);

      break;

     case false: 		// vm object case
      f_oid.v_i_code = 0;
      set_mode(OLD_OBJECT, false);
      break;
   }
}

root::~root() 
{
}

void root::set_c_code(c_code_t x)
{
   f_oid.v_c_code = x;
}

void root::set_mode(obj_mode_t mode, Boolean v)
{
   switch ( mode ) {
     case  HEALTH:
        status.ok = TOBIT(v);
        break;
     case  PERSISTENT:
        status.persistent = TOBIT(v);
        break;
     case  UPDATE:

        if ( v == true && g_transac && storage_ptr ) {
           g_transac -> book(f_oid, storage_ptr);
        }

        status.update= TOBIT(v);
        break;
     case  OLD_OBJECT:
        status.old_object= TOBIT(v);
        break;
     case  CDR:
        status.cdr= TOBIT(v);
        break;
     case  SWAP_ALLOWED:
        status.swap = TOBIT(v);
        break;
   }
}

Boolean root::get_mode(obj_mode_t mode) const
{
   Boolean ok = false;
   switch ( mode ) {
     case  HEALTH:
        ok = TOBOOLEAN(status.ok);
        break;
     case  PERSISTENT:
        ok = TOBOOLEAN(status.persistent);
        break;
     case  UPDATE:
        ok = TOBOOLEAN(status.update);
        break;
     case  OLD_OBJECT:
        ok = TOBOOLEAN(status.old_object);
        break;
     case  CDR:
        ok = TOBOOLEAN(status.cdr);
        break;
     case  SWAP_ALLOWED:
        ok = TOBOOLEAN(status.swap);
        break;
   }
   return ok;
}

void root::reset_ref_count()
{
   status.ref_count = 0;
}

void root::set_ref_count(int delta)
{
   status.ref_count += delta;
}

int root::get_ref_count()
{
   return status.ref_count;
}

Boolean root::OK() const
{
   return get_mode(HEALTH);
}

const oid_t& root::my_oid() const
{
   return f_oid;
}

io_status root::asciiOut(ostream& out) 
{
   return f_oid.asciiOut(out);
}

ostream& operator<<(ostream& out, const root& rt) 
{
   (*(root*)&rt).asciiOut(out);
   return out;
}

io_status root::asciiIn(istream& in) 
{
   return f_oid.asciiIn(in);
}

ostream& root::memory_layout(root* rt, ostream& out)
{
   MESSAGE(cerr, "In memory_layout");
   debug(cerr, long(rt));
   debug(cerr, (int)sizeof(*rt));

   long* p = (long*)rt;
   int ptrs = sizeof(*rt)/sizeof(long);
  
   for ( int i=0; i<ptrs; i++ )
     out << long(p[i]) << " "; 

   out << "\n";

   return out;
}

void* root::heap_alloc( size_t sz )
{
   return (void*) new char[sz];
}


int root::cdr_sizeof()
{
   return sizeof(char);
}

   
io_status root::cdrOut(buffer& buf)
{
   unsigned int status_rep = 0;

   lsb_putbits(status_rep, 3, 1, status.cdr);
   lsb_putbits(status_rep, 2, 1, status.ok);
   lsb_putbits(status_rep, 1, 1, status.persistent);
   lsb_putbits(status_rep, 0, 1, status.update);

   buf.put((char)status_rep);

   return done;
}
   
io_status root::cdrIn(buffer& buf)
{
   char char_status_rep = 0;
   buf.get(char_status_rep);

   unsigned int status_rep = char_status_rep;

   status.cdr         = lsb_getbits((unsigned)status_rep, 3, 1);
   status.ok          = lsb_getbits((unsigned)status_rep, 2, 1);
   status.persistent  = lsb_getbits((unsigned)status_rep, 1, 1);
   status.update      = lsb_getbits((unsigned)status_rep, 0, 1);

   return done;
}

MMDB_BODIES(root)

#ifdef C_API
NEW_AND_DELETE_BODIES_SIMPLE(root)
#endif
