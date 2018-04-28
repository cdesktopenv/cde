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
 * $XConsortium: imp_bucket.h /main/4 1996/07/18 14:32:27 drk $
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


#ifndef _imp_bucket_h
#define _imp_bucket_h

#include "utility/funcs.h"
#include "utility/pm_random.h"
#include "dstr/dlist.h"
#include "dynhash/data_t.h"


/*******************************/
// An imp_bucket object forms 
// a second level hash function
/*******************************/
class imp_bucket : public dlist
{

protected:
   int k;                       // valye k used in H_{{2M_j}^2}()
   int rotate;                  // rotate value for this key subset

public:
   imp_bucket();
   virtual ~imp_bucket();

   data_t* operator()(long ind); 

   int h(int key, int prime, int M); //hash function H_{{2M_j}^2}()
   Boolean empty() ;                 //bucket empty?

   Boolean insert(data_t*);
   data_t* remove_all();

   ostream& asciiOut(ostream& out, print_func_ptr_t print_f);

   friend ostream& operator<<(ostream&, imp_bucket&);
   friend class imp_die;
};

typedef imp_bucket* imp_bucketPtr;

#endif
