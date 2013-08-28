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
 * $XConsortium: base.h /main/4 1996/06/11 17:11:01 cde-hal $
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


#ifndef _base_h
#define _base_h 1

#include "object/cset.h"
#include "schema/object_dict.h"

#define MAP_FILE                "names.mmdb"
#define MAP_FILE_8_3            "bookcase.map"

#define DATA_FILE_SUFFIX	"dbd"
#define INDEX_FILE_SUFFIX	"dbi"

#define UIDSIZ 20

/*************************************/
// The base class
/*************************************/

class base : public primitive
{

public:
   base(object_dict* dict);
   base(object_dict* dict,
        char** set_nms, char** list_nms,
        const char* base_dir, const char* base_name, 
        const char* base_desc, const char* base_uid
       );
   virtual ~base();

   object_dict& get_obj_dict()  { return *f_obj_dict; };

   const char* get_base_name() const { return base_name; };
   const char* get_base_desc() const { return base_desc; };
   const char* get_base_path() const { return base_path; };

   const char* get_base_uid() const { return base_uid; };

protected:

   char base_path[PATHSIZ];
   char base_name[PATHSIZ];
   char base_desc[PATHSIZ];
   char base_uid[UIDSIZ];

   char** info_base_set_names;
   char** info_base_list_names;
   int num_cset_ptrs;
   int num_list_ptrs;

   object_dict* f_obj_dict;
};

typedef base* basePtr;

#endif
