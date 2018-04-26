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
/* $XConsortium: random_gen.h /main/3 1996/06/11 17:42:42 cde-hal $ */

#ifndef _random_gen_h
#define _random_gen_h

#include "utility/pm_random.h"
#include "object/oid_t.h"

class random_gen
{
protected:
   pm_random rand_gen;

protected:
   void _random_string(ostream& out, int low, int high);

public:
   random_gen();
   ~random_gen();

   pm_random& random_generator() { return rand_gen; };
   int pick_a_value(int low, int high);

   const char* get_random_string(int low, int high);

   void random_string(ostream& out, int len);
   void random_string(ostream& out, int low, int high, 
		      Boolean out_string_code = true);
   void random_string_to_be_compressed(ostream&, int l, int h, const oid_t& = ground);
   void random_oid(ostream&, int class_code = 0, int instance_code = 0);
   void random_integer(ostream&, int value = 0);
};

#endif
