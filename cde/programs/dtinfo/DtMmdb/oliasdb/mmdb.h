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
/* $XConsortium: mmdb.h /main/5 1996/10/26 18:19:50 cde-hal $ */

#ifndef _x_mmdb_h
#define _x_mmdb_h

#include "api/info_lib.h"
#include "mgrs/managers.h"
#include "dstr/void_ptr_array.h"

#include "oliasdb/olias_consts.h"
#include "oliasdb/node_hd.h"
#include "oliasdb/graphic_hd.h"
#include "oliasdb/toc_hd.h"
#include "oliasdb/locator_hd.h"
#include "oliasdb/doc_hd.h"
#include "oliasdb/stylesheet_hd.h"
#include "oliasdb/dlp_hd.h"
#include "oliasdb/mark.h"


class OLIAS_DB {

protected:
   char** info_base_set_names;
   char** info_base_list_names;

   managers internal_managers; // this should be constructed before 
			       // the following objects!!!

   dlp                  dlp_template;
   doc                  doc_template;
   graphic              graphic_template;
   olias_locator        locator_template;
   olias_node           node_template;
   toc                  toc_template;
   umark                mark_template;
   stylesheet           stylesheet_template;

   void_ptr_array infolib_array;

protected:
   Boolean 
   real_destroy_info_base(const char* infoLibPath, const char* base_name);

   Boolean real_destroy(const char* infoLibPath, const char* infoLibName);
   info_lib* getInfoLib(const char* infoLibUid, int& i);

public:
   OLIAS_DB();
   virtual ~OLIAS_DB();

   info_lib* openInfoLib(const char* path = getenv("MMDB_PATH"), 
                         const char* selectedBookCaseName = 0,
			 const char* infoLibName = "InfoLibrary"
                        );
   // NOTE: default argument ("InfoLibrary") needs to be abandoned.
   void closeInfoLib(const char* infoLibUid = "InfoLibrary");
   info_lib* getInfoLib(int descriptor);

// complete remove file dir structure for the infolib
// The action will be taken when the infolib is in closed state
   Boolean destroy(const char* infoLibPath, const char* infoLibName = "InfoLibrary");

// return 1 if path is a valid dir path and the directory contains
// names.mmdb or bookcase.map file.
   int validInfoLibPath(const char* path);

   int major_code_version();
   int minor_code_version();
   mm_version code_version();

   int numItems() { return infolib_array.no_elmts(); }
   char* getInfoLibUid(char*);

};

#endif

