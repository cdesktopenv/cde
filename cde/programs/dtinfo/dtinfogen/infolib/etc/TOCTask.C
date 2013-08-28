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
/* $XConsortium: TOCTask.cc /main/2 1996/07/18 15:21:32 drk $ */

/* exported interfaces... */
#include "TOCTask.h"

/* imported interfaces... */
#include "Token.h"
#include "AttributeRec.h"
#include "OLAF.h"
#include "SGMLName.h"
#include "OL-Data.h"
#include "BookTasks.h"
#include "DataBase.h"
#include "BookCaseDB.h"

/*
 * TOCTask
 */

TOCTask::TOCTask(const Token& t, BookTask *book, TOCTask *parent, int sibindx)
{
  f_book = book;
  f_parent = parent;

  f_sibling_index = sibindx;
  
  f_base = t.level();

  f_locator = NULL;
  
  markup(t);
}


int toc_depth(const Token& t)
{
  int ret = -1;
  const AttributeRec *a;

  if( t.LookupAttr( OLAF::OL_ToC ) ){
    ret = 0;
  }
  else if((a = t.LookupAttr( OLAF::OL_ToCEntry ))){
    const char *val;

    if((val = a->getAttrValueString())){
      ret = atoi(val);
    }
  }

  return ret;
}


void TOCTask::markup(const Token& t)
{
  if(f_base >= 0){
    if(f_locator) f_locator->markup(t);
    if(used > 0) subtasks[used-1]->markup(t);
    
    if(t.type() == START){
      
      if(f_locator == NULL){
	/* locator for root node, TOC, is an OL_id.
	 * others are OL_idref
	 */
	if(f_parent == NULL && t.LookupAttr(OLAF::OL_id)){
	  f_locator = new OL_Data(t, OLAF::OL_id, REMOVE_SPACES);
	}
	else if(f_parent != NULL && t.LookupAttr(OLAF::OL_idref)){
	  f_locator = new OL_Data(t, OLAF::OL_idref, REMOVE_SPACES);
	}
      }

      if (t.level() == f_base + 1
	  && t.LookupAttr( OLAF::OL_ToCEntry ) ){
	TOCTask *child = new TOCTask(t, f_book, this, used + 1);
	addSubTask(child);
      }

    }
    
    else if(t.type() == END){
      
      if(t.level() == f_base){
	if(f_parent == 0){ /* is this the root node? */
	  /* tree is now complete. write it out. */
	  BookCaseTask *bc = f_book->bookcase();
	    
	  write_tree(bc->table(BookCaseDB::TOCTree));

	  /* write out path too */
	  write_path(bc->table(BookCaseDB::TOCPath));
	}

	f_base = -1;
      }

    }

  }
}


void TOCTask::data(const char *chars, size_t len)
{
  if(f_locator) f_locator->data(chars, len);
  if(used) subtasks[used-1]->data(chars,len);
}


const char *TOCTask::locator()
{
  if (!f_locator){
    throw(Unexpected("No locator for TOC entry"));
  }

  return f_locator->content();
}


void TOCTask::write_path(DBTable *tbl)
{
  tbl->insert(STRING_CODE, f_book->locator(),
	      STRING_CODE, locator(),
	      INTEGER_CODE, f_locator->line_no(),
	      STRING_CODE,  f_locator->filename(),
	      NULL);

  for(int i = 0; i < used; i++){
    ((TOCTask*)subtasks[i])->write_path(tbl);
  }
}


int TOCTask::write_tree(DBTable *tbl)
{
  const char **children = NULL;
  int qty = 1;
  
  if(used){
    children = (const char **)new char*[used];

    for(int i = 0; i < used; i++){
      TOCTask *ch = (TOCTask*)subtasks[i];
      children[i] = ch->locator();
      qty += ((TOCTask*)subtasks[i])->write_tree(tbl);
    }
  }
  
  tbl->insert(STRING_CODE, f_book->locator(),
	      STRING_CODE, locator(),
	      STRING_CODE, f_parent ? f_parent->locator() : "",
	      SHORT_LIST_CODE,
	      used, STRING_CODE, children,
	      INTEGER_CODE, qty,
	      NULL);

  delete children;
  return qty;
}


