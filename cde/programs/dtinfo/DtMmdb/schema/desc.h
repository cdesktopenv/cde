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
 * $XConsortium: desc.h /main/5 1996/07/18 14:50:09 drk $
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


#ifndef _desc_h
#define _desc_h 1

#include "object/oid_t.h"
#include "object/c_codes.h"
#include "storage/abs_storage.h"
#include "object/handler.h"

#define desc_print3(s, n, x)  s << "	" << n << "=" << (x) << ":\\" << endl
#define desc_print_end3(s, n, x)  s << "	" << n << "=" << (x) << endl

#if !defined ( __STDC__) && !defined (hpux)
#define desc_print(s, x)  s << "	" << "x" << "=" << (x) << ":\\\n"
#define desc_print_end(s, x)  s << "	" << "x" << "=" << (x) << "\n"
#else
#define desc_print(s, x)  s << "	" << #x << "=" << (x) << ":\\\n"
#define desc_print_end(s, x)  s << "	" << #x << "=" << (x) << "\n"
#endif

Boolean desc_name_ls(const void* o1, const void* o2);
Boolean desc_name_eq(const void* o1, const void* o2);

class object_dict;

class desc : public Destructable {

public:
   desc(const char* name);
   desc(int class_code, const char* type);
   virtual ~desc();

   virtual handler* init_handler(object_dict&) { return 0; };
   virtual abs_storage* init_store(char*) { return 0; };

   virtual handler* get_handler() { return 0; };
   virtual abs_storage* get_store() { return 0; };

   virtual void quit_handler() {};
   virtual void sync_store() {};
   virtual void quit_store() {};

   void set_nm(const char*);
   void set_oid(const char*);
   void set_oid(const oid_t&);

   void set_next_desc(desc* x) { next_desc = x; };

   const char* get_nm() { return nm; };
   const char* get_type() { return type; };
   desc* get_next_desc() { return next_desc; };

   friend ostream& operator<<(ostream&, const desc&);
   virtual ostream& asciiOut(ostream& out, Boolean last = true);

   ostream& asciiOutList(ostream& out);

   friend Boolean desc_name_ls(const void* o1, const void* o2);
   friend Boolean desc_name_eq(const void* o1, const void* o2);

   friend class object_dict;

protected:
   oid_t v_oid;
   desc* next_desc;

private:
   char* nm;
   char* type;
};


#endif
