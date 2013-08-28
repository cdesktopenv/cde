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
 * $XConsortium: bset.cc /main/5 1996/07/18 14:28:37 drk $
 *
 * Copyv_right (c) 1993 HAL Computer Systems International, Ltd.
 * All v_rights reserved.  Unpublished -- v_rights reserved under
 * the Copyv_right Laws of the United States.  USE OF A COPYRIGHT
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


#include  "dstr/bset.h"

Boolean void_ls(const void* o1, const void* o2)
{
   return (long(o1) < long(o2) ) ? true : false;
}

Boolean void_eq(const void* o1, const void* o2)
{
   return (long(o1) == long(o2) ) ? true : false;
}

//**************************************************************
//
// a set implemented based on binary search tree data structure.
//
//**************************************************************


bset::bset(cmp_func_ptr_t eq, cmp_func_ptr_t ls): set(eq, ls), v_setroot(0)
{
   //assert ( eq && ls );
}

bset::~bset()
{
   delete v_setroot;
}

Boolean bset::_insert(void* x, bsetnode *rt)
{
/*
debug(cerr, "in bsert _insert");
debug(cerr, int(x));
debug(cerr, int(rt));
*/

/*
debug(cerr, int(f_cmp_func_eq));
debug(cerr, int(f_cmp_func_ls));
*/

   if ( (*f_cmp_func_eq)(x, rt -> v_element) == true )
      return false;

   if ( (*f_cmp_func_ls)(x, rt -> v_element) == true ) 
      if ( rt -> v_left == 0 ) {
         rt -> v_left = new bsetnode(x);
         return true;
      } else
         return _insert(x, rt -> v_left);
   else
      if ( rt -> v_right == 0 ) {
         rt -> v_right = new bsetnode(x);
         return true;
      } else
         return _insert(x, rt -> v_right);
}

Boolean bset::insert(void* x)
{
/*
debug(cerr, "in bsert insert");
debug(cerr, int(f_cmp_func_eq));
debug(cerr, int(f_cmp_func_ls));
debug(cerr, int(this));
debug(cerr, int(x));
debug(cerr, int(v_setroot));
*/

   if ( v_setroot == 0 ) {
     v_setroot = new bsetnode(x);
     return true;
   } else {

//debug(cerr, "to bsert _insert");

     return _insert(x, v_setroot); 
   }
}

/////////////////////////////////////////////////////////////
// delete the min node from true rooted at rt and return its 
// v_element pointer 
/////////////////////////////////////////////////////////////
void* bset::_remove_min(void* x, bsetnode*& rt)
{
   if ( rt -> v_left == 0 ) {
      if ( rt -> v_right )
         return _move(rt, rt -> v_right);
      else {
         void* y = rt -> v_element;
         delete rt;
         rt = 0;
         return y;
      }
   } else
      return _remove_min(x, rt -> v_left);
}

//**********************************************************************
// move the content and pointers of node2 to node1 and remove node2
//**********************************************************************
void* bset::_move(bsetnode* node1, bsetnode* node2)
{
   void* y = node1 -> v_element ;

   node1 -> v_element = node2 -> v_element ;
   node2 -> v_element = 0;
   node1 -> v_left = node2 -> v_left ;
   node2 -> v_left = 0;
   node1 -> v_right = node2 -> v_right ;
   node2 -> v_right = 0;
   delete node2 ;

   return y;
}

void* bset::_remove(void* x, bsetnode*& rt)
{
   if ( (*f_cmp_func_ls)(x, rt -> v_element) == true )  // v_left branch
      return _remove(x, rt -> v_left);
   else
   if ( !((*f_cmp_func_eq)(x, rt -> v_element) == true ) ) // v_right branch
      return _remove(x, rt -> v_right);
   else
   if ( rt -> v_left == 0 && rt -> v_right == 0 )  { // find the node.
      void* y = rt -> v_element ;
      delete rt;
      rt = 0;
      return y;
   } else 
   if ( rt -> v_left == 0 ) {
      return _move(rt, rt -> v_right);
   } else 
   if ( rt -> v_right == 0 ) {
      return _move(rt, rt -> v_left);
   } else {
      void* y = rt -> v_element ;
      rt -> v_element = _remove_min(x, rt -> v_right );
      return y;
   } 
}

void* bset::remove(void* x)
{
   if ( v_setroot == 0 )
      return 0;
   else
      return _remove(x, v_setroot);
}

void* bset::_member(const void* x, bsetnode *rt)
{
   if ( rt == 0 ) {
//debug(cerr, "member void zeor");
      return 0;
   } else if ( (*f_cmp_func_eq)(x, rt -> v_element) == true )
      return rt -> v_element;
   else if ( (*f_cmp_func_ls)(x, rt -> v_element) == true )
      return _member(x, rt -> v_left);
   else
      return _member(x, rt -> v_right);
}

void* bset::member(const void* x)
{
/*
debug(cerr, "member");
debug(cerr, int(f_cmp_func_eq));
debug(cerr, int(f_cmp_func_ls));
debug(cerr, int(this));
*/
   return _member(x, v_setroot);
}

void bset::apply(app_func_ptr_t f)
{
   _apply(v_setroot, f);
}

void bset::_apply(bsetnode* rt, app_func_ptr_t f)
{
   if ( rt ) {

      f(rt -> v_element);

      _apply( rt -> v_left, f );
      _apply( rt -> v_right, f );

   }
}

void bset::del_elements(app_func_ptr_t func)
{
   _del_elements(v_setroot, func);
   v_setroot = 0;
}

void bset::_del_elements(bsetnode* rt, app_func_ptr_t func)
{
   if ( rt ) {

      bsetnode *lt = rt -> v_left;
      bsetnode *rg = rt -> v_right;

      rt -> v_left = 0;
      rt -> v_right = 0;

      func(rt -> v_element);
      delete rt;

      _del_elements( lt, func );
      _del_elements( rg, func );
   }
}

void bset::unlink_elements()
{
   _unlink_elements(v_setroot);
   v_setroot = 0;
}

void bset::_unlink_elements(bsetnode* rt)
{
   if ( rt ) {

      bsetnode *lt = rt -> v_left;
      bsetnode *rg = rt -> v_right;

      rt -> v_left = 0;
      rt -> v_right = 0;
      delete rt;

      _unlink_elements( lt );
      _unlink_elements( rg );
   }
}

void bset::_smaller_member(const void* x, bsetnode* rt, void*& answer)
{
   if ( rt == 0 ) return;

   if ( (*f_cmp_func_eq)(x, rt -> v_element) == true ) {
      answer = rt -> v_element;
      return;
   }

   if ( (*f_cmp_func_ls)(rt -> v_element, x) == true ) {
      answer = rt -> v_element;
      _smaller_member(x, rt -> v_right, answer);
      return;
   } else {
      _smaller_member(x, rt -> v_left, answer);
      return;
   }

/*
   if ( (*f_cmp_func_ls)(x, rt -> v_element) == true ) {
      if ( rt -> v_left )
         return _smaller_member(x, rt -> v_left);
      else
         return 0;
   } 
   
   if ( (*f_cmp_func_eq)(x, rt -> v_element) == true || 
        rt -> v_right == 0 
      ) 
      return rt -> v_element;

   return _smaller_member(x, rt -> v_right);
*/
}

void* bset::_larger_member(const void* x, bsetnode* rt)
{
   if ( gt(x, rt -> v_element) == true ) {
      if ( rt -> v_right )
         return _larger_member(x, rt -> v_right);
      else
         return 0;
   } 
   
   if ( (*f_cmp_func_eq)(x, rt -> v_element) == true || 
        rt -> v_left == 0 
      ) 
      return rt -> v_element;

   return _larger_member(x, rt -> v_left);
}


void* bset::smaller_member(const void* x)
{
   void* answer = 0;
   _smaller_member(x, v_setroot, answer);
   return answer;
}

void* bset::larger_member(const void* x)
{
   return _larger_member(x, v_setroot);
}

Boolean bset::gt(const void* x, const void* y)
{
   if ( (*f_cmp_func_eq)(x, y) == false &&
        (*f_cmp_func_ls)(x, y) == false 
      )
     return true;
   else
     return false;
}

ostream& bset::asciiOut(ostream& out)
{
   _asciiOut(out, v_setroot);
   return out;
}

void bset::_asciiOut(ostream& out, bsetnode* rt)
{
   if ( rt ) {
      out << long(rt -> v_element);
      out << "\n";
      _asciiOut( out, rt -> v_left );
      _asciiOut( out, rt -> v_right );
   }
}

#ifdef REGRESSION_TEST

#include <sys/time.h>
#include "utility/pm_random.h"

int
in_bset_test(bset& bst, int* ins, unsigned int& in_cts, int* outs, unsigned int& out_cts, pm_random& rand_gen)
{
   if ( in_cts == 0 ) return 0;

   int k = rand_gen.rand() % in_cts;

   int_swap(ins[k], ins[in_cts-1]);

//cerr << "<-------------- removing " << ins[in_cts-1] << "\n";
   long j = (long)bst.remove((void*)ins[in_cts-1]);

   if ( j != ins[in_cts-1] ) {
      cerr << "can't correctly remove " << ins[in_cts-1] << "\n";
      return -1;
   }

   if ( bst.member((void*)ins[in_cts-1]) != 0 ) {
      cerr << "element " << ins[in_cts-1] << " still in the set\n";
      return -1;
   }

   outs[out_cts++] = ins[in_cts-1];
   in_cts--;

   return 0;
}

int
out_bset_test(bset& bst, int* ins, unsigned int& in_cts, int* outs, unsigned int& out_cts, 
	      pm_random& rand_gen)
{
   if ( out_cts == 0 ) return 0;

   int k = rand_gen.rand() % out_cts;

   int_swap(outs[k], outs[out_cts-1]);

   if ( bst.member((void*)outs[out_cts-1]) != 0 ) {
      cerr << "can still find " << outs[out_cts-1] << "\n";
      return -1;
   }

//cerr << "--------------> inserting " << outs[out_cts-1] << "\n";

   bst.insert((void*)outs[out_cts-1]);

   if ( bst.member((void*)outs[out_cts-1]) == 0 ) {
      cerr << "can't find " << outs[out_cts-1] << "\n";
      return -1;
   }

   ins[in_cts++] = outs[out_cts-1];
   out_cts--;

   return 0;
}


int
bset_test(unsigned int in_cts, unsigned int out_cts, pm_random& rand_gen, unsigned int cycles)
{
   int ok = 0;

   int* ins = new int[in_cts+out_cts];
   int* outs = new int[in_cts+out_cts];

   bset bst(void_eq, void_ls);

   for ( int i=0; i<in_cts; i++ ) {
      ins[i] = i + 1;
      bst.insert((void*)ins[i]);
   }

   for ( i=0; i<out_cts; i++ ) {
      outs[i] = i + in_cts + 1;
   }

   for ( i=0; i<cycles; i++ ) {
      if ( rand_gen.rand_01() > 0.5 )
         ok |= in_bset_test(bst, ins, in_cts, outs, out_cts, rand_gen);
      else
         ok |= out_bset_test(bst, ins, in_cts, outs, out_cts, rand_gen);
   }

   delete ins; 
   delete outs; 

   return ok;
}

#endif
