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
 * $XConsortium: bucket.h /main/3 1996/06/11 17:19:00 cde-hal $
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


/***************************************************************/
//
//Implementation of the dynamic perfect hashing algorithm, based on:
//
//         "Dynamic Perfect Hashing: Upper and Lower Bounds"
//
//                                By
//
//        M. Dietzfelbinger, A. Karlin, K. Mehlhorn,
//        F. Meyer auf der Heider, H.  Rohnert and
//        R. E. Tarjan
//                            appearing in
//
//                             1988 FOCS.
//
/***************************************************************/


/***************************************************************/
// Programmer: QiFan Chen
// Date: June 14, 1992
// Language: C++ (2.0)
// Machine: SUN SPARCstation IPC
/***************************************************************/

#ifndef _bucket_h
#define _bucket_h

#include "utility/funcs.h"
#include "utility/prandom.h"
#include "dynhash/data.h"


/**************************************************************/
// define a data object that is shared by the first
// and the second level hash functions
/**************************************************************/

struct shared_t {
   prandom rand_generator;  // rand generator
   int p;                   // prime number p
   int sum;                 // the value of the lhs of condition 4
   int limit;               // the value of the rhs of condition 4
   data_t* internal_L;      // the list representation
};

/****************************/
// A bucket object forms a
// second level hash function
/****************************/
class bucket {

protected:
   static int upper_limit; // set to 2*Mj*Mj by first()
                           // for fast iteration 

   int k;                 // valye k used in H_{{2M_j}^2}()
   int Mj;                // size window 
   int wj;                // keys in the bucket
   int old_wj;            // value of wj after a memory allocation
   data_tPtr data_array;  // key array

   void rehash_all(data_t& dt, shared_t&); // rehash all keys
   void select_h_params(shared_t&);        // select the parameter k

public:
   bucket(int new_Mj, int old_wj);
   virtual ~bucket();

   int h(int key, shared_t&);    //  hash function H_{{2M_j}^2}()
   int M_size() { return Mj; };  //  the value of Mj
   int wj_size() { return wj; }; //  the value of wj

   Boolean insert(data_t& dt, shared_t&);  // insert a key  
   Boolean remove(data_t& dt, shared_t&);  // remove a key
   Boolean member(data_t& dt, shared_t&);  // member test

   int first();                 // iterate over all keys in 
   data_t& operator()(int ind); // the bucket
   void next(int& ind);         // terminate condition: -1

   friend ostream& operator<<(ostream&, bucket&);
};

typedef bucket* bucketPtr;

#endif
