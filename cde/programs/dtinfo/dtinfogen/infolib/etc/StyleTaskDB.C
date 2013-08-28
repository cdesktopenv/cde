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
/* $XConsortium: StyleTaskDB.cc /main/2 1996/07/18 15:20:49 drk $ */
/* export... */
#include "StyleTask.h"

/* import... */
#include <assert.h>

#include "BookCaseDB.h"
#include "BookTasks.h"
#include "DataBase.h"

//---------------------------------------------------------------------

StyleTaskDB::StyleTaskDB(BookCaseTask *bc)
: StyleTask()
{
   f_bookcase = bc;
}


void StyleTaskDB::done(const char * name,
		  const char * online, int online_len,
		  const char * print, int print_len)
{
  /*
   * Use -STRING_CODE instead of STRING_CODE to handle 8-bit clean
   * data
   */
  
  DBTable *tbl = f_bookcase->table(BookCaseDB::StyleSheet);
  
  tbl->insert(STRING_CODE, name,
	      -STRING_CODE, online, (size_t)online_len,
	      -STRING_CODE, print,  (size_t)print_len,
	      NULL);

  reset();
}
