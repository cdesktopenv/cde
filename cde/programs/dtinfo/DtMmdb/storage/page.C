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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * $XConsortium: page.cc /main/6 1996/08/15 14:14:45 cde-hal $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */


#include "storage/page.h"


/////////////////////////////////////////////////////////////
// SLOT_HEADER_SIZE is defined for these fields:
//(sizeof(fwd_ptr) + sizeof(header.int_view) + sizeof(int))
//
// Note: the last int field is not used. It is here for
//       backward compatibility.  qfc 12-13-95
/////////////////////////////////////////////////////////////

#define SLOT_HEADER_SIZE (sizeof(mmdb_pos_t) + sizeof(unsigned int) + sizeof(int))

spointer_t::spointer_t(char* page_base, int slot): swapped(false)
{
   page_image = page_base + sizeof(count_t) + (slot-1)*SLOT_HEADER_SIZE;

   memcpy((char*)&fwd_ptr, page_image, sizeof(fwd_ptr));
   memcpy(
          (char*)&header.int_view, 
          page_image+sizeof(fwd_ptr), 
          sizeof(header.int_view)
         );
#ifdef PAGE_DEBUG
MESSAGE(cerr, "spointer_t::spointer_t() [1]");
debug(cerr, slot);
debug(cerr, (int)(void*)page_base);
debug(cerr, (int)(void*)page_image);
#endif

}

spointer_t::spointer_t(char* page_base, int slot_num, int ofst, int len) : 
   fwd_ptr(0), swapped(false)
{
   header.bit_view.spointer = ofst;
   header.bit_view.length = len;

   set_mode(DELETED, false);
   set_mode(IS_OBJECT, false);

   set_mode(UPDATED, true);

   page_image = page_base + sizeof(count_t) + (slot_num-1) * SLOT_HEADER_SIZE;
}

spointer_t::~spointer_t() 
{

//MESSAGE(cerr, "spointer_t::~spointer_t()");
//debug(cerr, header.bit_view.spointer);

   if ( swapped == true || get_mode(UPDATED) == true ) {
      memcpy(page_image, (char*)&fwd_ptr, sizeof(fwd_ptr));
      memcpy(page_image+sizeof(fwd_ptr), 
             (char*)&(header.int_view), 
             sizeof(header.int_view)
            );
   }
}

void spointer_t::set_mode(spointer_mode m, Boolean b)
{
   switch ( m ) {
     case IS_OBJECT:
        header.bit_view.is_object = ( b == true ) ? 1 : 0;
        header.bit_view.updated = 1;
        break;
     case FIRST_RECD:
        header.bit_view.first_recd = ( b == true ) ? 1 : 0;
        header.bit_view.updated = 1;
        break;
     case DELETED:
        header.bit_view.deleted = ( b == true ) ? 1 : 0;
        header.bit_view.updated = 1;
        break;
     case UPDATED:
        header.bit_view.updated = ( b == true ) ? 1 : 0;
        break;
     default:
        MESSAGE(cerr, "page::set_mode(): invalid mode");
        throw( intException((int)m) );
        break;
   }

}

Boolean spointer_t::get_mode(spointer_mode m)
{
   switch ( m ) {
     case IS_OBJECT:
        return ( header.bit_view.is_object == 1 ) ? true : false;
     case FIRST_RECD:
        return ( header.bit_view.first_recd == 1 ) ? true : false;
     case DELETED:
        return ( header.bit_view.deleted == 1 ) ? true : false;
     case UPDATED:
        return ( header.bit_view.updated == 1 ) ? true : false;
     default:
        MESSAGE(cerr, "page::get_mode(): invalid mode");
        throw( intException((int)m) );
        break;
   }
}

   
static int align_offset;

page::page(int buf_sz, int pid,  Boolean swap_order) : 
    buffer( buf_sz ),  pageid(pid), dirty(false), num_locks(0), 
    v_swap_order(swap_order)
{
   v_memalign_offset = align_offset;
   clean_all();
}

page::~page()
{
   align_offset = v_memalign_offset;
}


/***************************/
//
/***************************/
void page::clean_all()
{
   v_eptr = v_base + buf_sz();
   set_count(1);

//////////////////////////////////////////
// use spointer_t's dstr to write slot info to page.
//////////////////////////////////////////
   spointer_t x(v_base, 1, 
                buf_sz(), buf_sz()-sizeof(count_t)-2*SLOT_HEADER_SIZE
               );
}


/*************************************
* Get the ith string pointer from the 
* page.
**************************************/
spointer_t* page::get_spointer(int slot_num) const
{ 
   if ( !INRANGE(slot_num, 1, count()) ) {
      throw( boundaryException(1, count(), slot_num));
   }

   return new spointer_t(v_base, slot_num);
}


//////////////////////////////
// Get a string from the page.
//////////////////////////////
Boolean 
page::get( int slot_num, buffer& target, 
           int offset, int bytes_to_read
         ) const
{

/*
MESSAGE(cerr, "in page::get()");
debug(cerr, slot_num);
debug(cerr, offset);
debug(cerr, bytes_to_read);
*/

   if ( ! INRANGE(slot_num, 1, count()-1) ) 
      throw( boundaryException(1, count()-1, slot_num) );


   spointer_t slot_info(v_base, slot_num); 

   if ( slot_info.get_mode(spointer_t::DELETED) == true ) {
      MESSAGE(cerr, "page::get(): get a deleted slot");
      return false;
   }

   int string_len = slot_info.string_leng();
//debug(cerr, string_len);

   if ( bytes_to_read == 0 )
      bytes_to_read = string_len;

//debug(cerr, bytes_to_read);
      
   if ( bytes_to_read + offset > slot_info.string_leng() ) {
       MESSAGE(cerr, "page::get(): too many bytes to read");
       throw( boundaryException(0, slot_info.string_leng(), 
                                bytes_to_read + offset)
            );
   }

   offset += slot_info.string_ofst();


/*
MESSAGE(cerr, "page::get()");
debug(cerr, offset);
debug(cerr, long(v_base + offset));
debug(cerr, bytes_to_read);

for (int ki=0; ki<bytes_to_read; ki++)
   cerr << int(v_base[offset+ki]) << " ";
cerr << "\n";
*/


   target.put(v_base + offset, bytes_to_read);
   return true;
}

/*************************************/
// Put a string on the page.
// slot_num: slot number to be returned 
// buf: string 
/**************************************/
Boolean 
page::put(int& slot_num, buffer& source)
{
#ifdef PAGE_DEBUG
MESSAGE(cerr, "page::put()");
debug(cerr, free_bytes());
#endif

   int source_len = source.content_sz();

   char* target = 0;

   alloc_slot( slot_num, source_len, target );

#ifdef PAGE_DEBUG
MESSAGE(cerr, "AAA");
debug(cerr, slot_num);
debug(cerr, free_bytes());
#endif

   memcpy(target, source.get_base(), source_len);

#ifdef PAGE_DEBUG
debug(cerr, (int)(void*)target);
debug(cerr, source_len);
MESSAGE(cerr, "BBB");
debug(cerr, free_bytes());
#endif

   spointer_t slot_info(v_base, slot_num) ;
   slot_info.set_mode(spointer_t::DELETED, false);
   slot_info.set_string_leng(source_len);

#ifdef PAGE_DEBUG
MESSAGE(cerr, "CCC");
debug(cerr, free_bytes());
MESSAGE(cerr, "page::put() done");
#endif

   dirty = true;
   return true;
}

Boolean page::del_slot(int slot_num)
{
   if ( ! INRANGE(slot_num, 1, count()-1) ) 
      throw(boundaryException( 1, count()-1, slot_num));

   spointer_t deleted_slot_info(v_base, slot_num);

   deleted_slot_info.set_mode(spointer_t::DELETED, true);

   spointer_t manage_slot_info(v_base, count());

   deleted_slot_info.set_forward_ptr(
                          (int)(manage_slot_info.forward_ptr())
                                    );
         
   manage_slot_info.set_forward_ptr( slot_num );

   dirty = true;

   return true;
}

Boolean page::update_slot( int slot_num, buffer& source, int offset )
{
/*
MESSAGE(cerr, "page::update_slot()");
debug(cerr, slot_num);
*/

   if ( ! INRANGE(slot_num, 1, count()-1) ) 
      throw(boundaryException( 1, count()-1, slot_num));


   spointer_t slot_info(v_base, slot_num) ;

   int ofst = slot_info.string_ofst() + offset;
   int leng = MIN(slot_info.string_leng(), source.content_sz());
/*
debug(cerr, ofst);
debug(cerr, leng);
*/

   memcpy(v_base + ofst, source.get_base(), leng);
   dirty = true;

   return true;
}

///////////////////////////////
// Allocate a slot on the page 
///////////////////////////////
Boolean page::alloc_slot( int& slot_num, int size, char*& str_ptr )
{
#ifdef PAGE_DEBUG
debug(cerr, size);
#endif

   Boolean ok = false;
   slot_num = count();
   int prev_slot_num = 0;
   
   while ( slot_num != 0 && 
           ( ok = _alloc_slot( slot_num, size, str_ptr )) == false ) {
     prev_slot_num = slot_num;

     spointer_t slot_info(v_base, slot_num);
     slot_num = (int)slot_info.forward_ptr();
   }

   if ( prev_slot_num && slot_num ) {

       spointer_t slot_info(v_base, slot_num);
       
       int next_slot_num = (int)slot_info.forward_ptr();

       spointer_t prev_slot_info(v_base, prev_slot_num);

       prev_slot_info.set_forward_ptr(next_slot_num);
   }
#ifdef PAGE_DEBUG
debug(cerr, free_bytes());
MESSAGE(cerr, " page::alloc_slot done");
#endif

   return ok;
}

Boolean page::_alloc_slot( int slot_num, int size, char*& str_ptr )
{
//MESSAGE(cerr, "_alloc_slot");

    spointer_t* slot_info = new spointer_t(v_base, slot_num);

/*************************************************************/
// make sure that the new string starts at an aligned position
/*************************************************************/

    int end_ptr = slot_info -> string_ofst() - size;


#ifdef NO_COPY
    int left_shift_delta = end_ptr % 4;
    end_ptr -= left_shift_delta;
    size += left_shift_delta; 
#endif


    int new_blank_len = slot_info -> string_leng() ;

    if ( new_blank_len < size ) 
        return false;

    slot_info -> set_string_ofst( end_ptr );
    slot_info -> set_string_leng( size );
    slot_info -> set_mode(spointer_t::DELETED, false);
    slot_info -> set_forward_ptr(0);

   delete slot_info;

    if ( slot_num == count() ) {

//MESSAGE(cerr, "slot_num == count() case:");

       new_blank_len -= size;
//debug(cerr, new_blank_len);
   
       if ( new_blank_len < (int)(2*SLOT_HEADER_SIZE + 10) )
          new_blank_len = 0;
       else
          new_blank_len -= SLOT_HEADER_SIZE; // space allocated for new
					     // free space slot

//debug(cerr, new_blank_len);

   
   /*
   MESSAGE(cerr, "in page::alloc");
   debug(cerr, size);
   debug(cerr, i_th -> string_leng());
   debug(cerr, i_th -> string_ofst());
   debug(cerr, new_blank_len);
   */
   
///////////////////////////////
// append a new slot to the end
///////////////////////////////
       set_count(count()+1);
       spointer_t* z = new spointer_t(v_base, count(), end_ptr, new_blank_len);
       delete z;

//debug(cerr, free_bytes());
//MESSAGE(cerr, "page::alloc done");
   
   /*
   MESSAGE(cerr, "in page::alloc");
   debug(cerr, end_ptr);
   debug(cerr, size);
   */
   
    }

  str_ptr = v_base + end_ptr;

//debug(cerr, int(base));
//debug(cerr, i_th -> string_ofst());

//debug(cerr, slot_num);
//debug(cerr, long(str_ptr));
//debug(cerr, long(v_base));
//debug(cerr, end_ptr);

//if ( count() >= 1 ) {
//spointer_t first_slot(v_base, 1);
//debug(cerr, first_slot.string_ofst());
//debug(cerr, first_slot.string_leng());
//}

  dirty = true;

  return true;
}


Boolean page::get_str_ptr( int slot_num, char*& str, int& len )
{
   if ( ! INRANGE(slot_num, 1, count()-1) ) 
      throw(boundaryException( 1, count()-1, slot_num));

   spointer_t slot_info(v_base, slot_num);

   str = slot_info.string_ofst() + v_base;
   len = slot_info.string_leng();

/*
MESSAGE(cerr, "page::get_str_ptr(), result:");
debug(cerr, slot_info.string_ofst());
debug(cerr, slot_info.string_leng());
*/

   return true;
}

// return number of free bytes on the page
int page::free_bytes() const  
{
   spointer_t slot_info(v_base, count());
   return slot_info.string_leng() - SLOT_HEADER_SIZE - 10;
}

int page::free_slots() const  // return number of free slots on the page
{
   spointer_t *x;
   int sum = 0;
   int ind = first();
   while ( ind ) {

      x = new spointer_t(v_base, ind);

      if ( x -> get_mode(spointer_t::DELETED) == true )
         sum++;

      delete x;

      next(ind);
   }
   return sum;
}

void page::next(int& ind) const // return next index relative to 'ind'
{
   if ( ind >= count()-1 )
      ind = 0;
   else
      ind ++;
}

void page::prev(int& ind) const // return prev index relative to 'ind'
{
   if ( ind == 1 )
      ind = 0;
   else
      ind --;
}

void page::_swap_order(Boolean swap_count_field_first)
{
   if ( v_swap_order == true ) {
/*
MESSAGE(cerr, "page::_swap_order()");
debug(cerr, pageid);
*/

      int& x = *((count_t*)v_base);  // use this version to point at
                                     // the count field

//debug(cerr, x);
      if ( swap_count_field_first == true ) {
         ORDER_SWAP_UINT(x); // swap the count field first
//MESSAGE(cerr, "swap earlier");
//debug(cerr, x);
      }

      for ( int i=1; i<=x; i++ ) {
//debug(cerr, i);
           spointer_t slot_info(v_base, i);
//debug(cerr, hex(slot_info.header.int_view));
//debug(cerr, slot_info.string_ofst());
           slot_info.swap_order();
//debug(cerr, hex(slot_info.header.int_view));
//debug(cerr, slot_info.string_ofst());
      }
   
      if ( swap_count_field_first == false ) {
         ORDER_SWAP_UINT(x); // swap the count field second
//MESSAGE(cerr, "swap later");
//debug(cerr, x);
      }
      
   }
}

ostream& operator<<(ostream&s, page& p)
{
   buffer lbuf;
   debug(s, int(p.dirty));
   debug(s, p.count());
   debug(s, p.pageid);
   debug(s, p.free_bytes());

   debug(s, long(p.v_base));
   debug(s, long(p.v_eptr));

   int ind = p.first();
   while ( ind ) {
      debug(s, int(ind));
      lbuf.reset();
      if ( p.get(ind, lbuf, 0) == true ) {
         s << lbuf;
         s << "\n";
      }
      p.next(ind);
   } 
   return s;
}

void* page::operator new( size_t sz )
{
   int alignment_overhead = sizeof(char*);

   char* p = ::new char[alignment_overhead+sz];

   if ( p ) {
       align_offset = int(alignment_overhead - long(p) % alignment_overhead);
       return (void*)(p + align_offset);
   } else {
      MESSAGE(cerr, "page::new() memalign call failed");
      throw(systemException(errno));
   }
}

void page::operator delete( void* p, size_t )
{
   if ( p ) 
      free((char*)((char*)p - align_offset));
}

/*
int page::cdr_sizeof()
{
   return sizeof(pageid) + v_bufsz;
}

io_status page::cdrIn(buffer& buf)
{
   buf.put(pageid);
   buffer::cdrIn(buf);
   return done;
}

io_status page::cdrOut(buffer& buf)
{
   buf.get(pageid);
   buffer::cdrOut(buf);
   return done;
}
*/
