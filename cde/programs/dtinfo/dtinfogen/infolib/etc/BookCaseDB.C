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
/* $XConsortium: BookCaseDB.C /main/4 1996/10/26 18:17:13 cde-hal $ */

#include "BookCaseDB.h"

#include <assert.h>

/*--------------------------------------------------------------------
 *
 * The BookCase is represented as a database (a directory) containing
 * several tables (files).
 *
 * As the tables are shared by many tasks within the bookcase task,
 * they are accessed through the bookcase, and created here...
 *
 *--------------------------------------------------------------------*/

/* This will be placed in a global .h file eventually */
#define LINK_CODE 1333


BookCaseDB::BookCaseDB(const char *dir)
     : DB(dir)
{
  for(int i = 0; i < TableQty; i++){
    f_tables[i] = NULL;
  }
}


//--------------------------------------------------------------------

struct Schema{
  int         id;
  const char *name;
  int         code;
  int         cols;
};

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

//--------------------------------------------------------------------
DBTable*
BookCaseDB::table(int tid, int a)
{
  static Schema schema[] = {
    { BookMeta,  "BookMeta",     DOC_CODE,           6 },
    { NodeMeta,  "NodeMeta",     NODE_CODE,          8 },
    { NodeSGML,  "NodeSGML",     SGML_CONTENT_CODE,  3 },
    { Link,      "Link",         LINK_CODE,          3 },
    { Locator,   "Locator",      LOCATOR_CODE,       5 },
    { TOCTree,   "ContentsTree", TOC_CODE,           5 },
    { TOCPath,   "ContentsPath", DLP_CODE,           4 },
    { Graphics,  "Graphics",     GRAPHIC_CODE,       7 },
    { StyleSheet,  "StyleSheet", NODE_CODE,          3 },
    { XRef,      "XRef",         XREF_CODE,          4 },
 };

  assert(tid >= 0 && tid < TableQty);
  assert(schema[tid].id == tid); /* just be sure the code doesn't get out
				 * of sync.
				 */
  if(!f_tables[tid]){
    f_tables[tid] = DB::table(schema[tid].name,
			      schema[tid].code,
			      schema[tid].cols,
			      a);
  }

  return f_tables[tid];
}

BookCaseDB::~BookCaseDB()
{
  for( int i = 0; i < TableQty; i++ ) {
    if ( f_tables[i] ) {
      delete f_tables[i]; f_tables[i] = 0;
    }
  }
}


