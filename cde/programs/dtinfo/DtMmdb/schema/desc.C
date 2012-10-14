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
 * $XConsortium: desc.cc /main/3 1996/06/11 17:31:57 cde-hal $
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


#include "schema/desc.h"

desc::desc(const char* name) : v_oid(ground), next_desc(0)
{
   nm = strdup(name);
   type = strdup("");
}

desc::desc(int tp, const char* type_str) : v_oid(ground), next_desc(0)
{
   nm = strdup("");

   v_oid.set_c_code(tp);
   type = strdup(type_str);
}

desc::~desc()
{
   delete nm; 
   delete type;
}

void desc::set_nm(const char* name)
{
   delete nm;
   nm = strdup(name);
}

void desc::set_oid(const char* source)
{
   istringstream in((char*)source);
   v_oid._asciiIn(in);
}

void desc::set_oid(const oid_t& source)
{
   v_oid.become(source);
}

ostream& desc::asciiOut(ostream& out, Boolean last)
{
   out << type << ":\\\n";

   desc_print(out, nm); 

   if ( last == true )
      desc_print_end(out, v_oid); 
   else
      desc_print(out, v_oid); 

   if ( !out )
      throw(stringException("desc::asciiOut() failed"));

   return out;
}

ostream& desc::asciiOutList(ostream& out)
{
   desc *ptr = this;

   out << "#=====\n";
   out << "#\n";

   if ( !out )
      throw(stringException("desc::asciiOutList() failed"));

   while ( ptr ) {
      out << "#=====\n";
      ptr -> asciiOut(out);
      out << "#\n";

      if ( !out )
         throw(stringException("desc::asciiOutList() failed"));

      ptr = ptr -> next_desc;
   }

   return out;
}

ostream& operator<<(ostream& out, const desc& ds)
{
   (*(desc*)&ds).asciiOut(out);
   return out;
}

Boolean desc_name_ls(const void* o1, const void* o2)
{
   desc *x = (desc*)o1;
   desc *y = (desc*)o2;
   return (strcmp(x->nm, y->nm) < 0) ? true : false;
}

Boolean desc_name_eq(const void* o1, const void* o2)
{
   desc *x = (desc*)o1;
   desc *y = (desc*)o2;
   return (strcmp(x->nm, y->nm) == 0) ? true : false;
}

