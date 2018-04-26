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
/* $XConsortium: LcfTask.C /main/3 1996/10/26 18:17:58 cde-hal $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/* exported interfaces */
#include "LcfTask.h"

/* imported interfaces */
#include <assert.h>
#include "Token.h"
#include "NodeTask.h"
#include "DataBase.h"
#include "OL-Data.h"
#include "OLAF.h"
#include "BookTasks.h"
#include "BookCaseDB.h"

//--------------------------------------------------------------------
LcfTask::LcfTask( NodeTask *f_parent, const Token &t )
{
  assert( f_parent != NULL );
  f_node = f_parent;
  f_base = t.level();
}

//--------------------------------------------------------------------
void
LcfTask::markup( const Token &t )
{
  
  ComplexTask::markup(t);
  
  if ( t.type() == START ) {
    if ( t.LookupAttr( OLAF::OL_id ) ) {
      OL_Data *LocData = new OL_Data ( t, OLAF::OL_id, REMOVE_SPACES );
      
      /*
       * Add subtask only if I know Data will be available
       */
      
      if ( LocData->DataWillBeAvailable() ) {
	addSubTask( LocData );

	OL_Data *RefData = new OL_Data(t, OLAF::OL_XRefLabel, IGNORE_ON);
	addSubTask( RefData );
      }
      else { delete LocData; }
    }
  }
  else if ( t.type() == END ) {
    /* only write out all the locator if I am at the end f_node */
    if ( t.level() == f_base ) {
      write_record();
      reset();
    }
  }
}

//--------------------------------------------------------------------
void
LcfTask::reset()
{
  ComplexTask::removeAllSubTasks();
}
  
//--------------------------------------------------------------------
void
LcfTask::write_record()
{
  const char *NodeLocator = f_node->locator();
#if 0
  for ( int i = 0; i < ComplexTask::used; i++ ) {
    OL_Data *task = (OL_Data * )subtask(i);
    if ( !task->ContentIsEmpty() ) {
      DBTable *tbl = f_node->book()->bookcase()->table(BookCaseDB::Locator);
      tbl->insert( STRING_CODE, task->content(),
		   STRING_CODE, NodeLocator,
		   STRING_CODE, task->filename(),
		   INTEGER_CODE, task->line_no(),
		   NULL);
    }
  }
#else
  for ( int i = 0; i < ComplexTask::used; i+=2 ) {
    OL_Data* task    = (OL_Data * )subtask(i);
    OL_Data* reftask = (OL_Data * )subtask(i+1);

    if ( !task->ContentIsEmpty() ) {
      DBTable *tbl = f_node->book()->bookcase()->table(BookCaseDB::Locator);

      const char* reflabel = "";
      if ( !reftask->ContentIsEmpty() ) {
	const char* content = reftask->content();
	if (strlen(content) < 256)
	  reflabel = content;
      }

      tbl->insert( STRING_CODE, task->content(),
		   STRING_CODE, NodeLocator,
		   STRING_CODE, reflabel,
		   STRING_CODE, task->filename(),
		   INTEGER_CODE, task->line_no(),
		   NULL);
    }
  }
#endif
}
