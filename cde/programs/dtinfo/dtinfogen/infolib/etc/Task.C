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
/* $XConsortium: Task.cc /main/2 1996/07/18 15:22:57 drk $ */
/* $XConsortium: Task.cc /main/2 1996/07/18 15:22:57 drk $ */

/* exported interfaces... */
#include "Task.h"

/* imported interfaces... */
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "SGMLName.h"
#include "Token.h"
#include "AttributeRec.h"

ComplexTask::ComplexTask()
{
  used = alloc = 0;
  subtasks = NULL;
}

ComplexTask::~ComplexTask()
{
  int i;

  for(i = 0; i < used; i++){
    delete subtasks[i];
  }

  if ( subtasks ) delete [] subtasks;
}

void ComplexTask::removeAllSubTasks()
{
  for ( int i = 0; i < used; i++ ) {
    delete subtasks[i];
  }

  if ( subtasks ) { delete [] subtasks; subtasks = NULL; }
  used = alloc = 0;
}

void ComplexTask::addSubTask(Task *t)
{
  grow(used + 1);
  subtasks[used++] = t;
}


void ComplexTask::stopSubTask(Task *t)
{
  for(int i = 0; i < used; i++){

    if(subtasks[i] == t){

      while(i + 1 < used){
	subtasks[i] = subtasks[i+1];
	i++;
      }

      used--;

      return;
    }
  }

  fprintf(stderr, "Internal errnor: stop unknown task.");
  abort();
}


void
ComplexTask::grow(int needed)
{
  if(needed + 1 > alloc){
    Task **born = new Task*[alloc = needed * 3 / 2 + 10];

    if(used){
      memcpy(born, subtasks, sizeof(Task*) * used);
      delete [] subtasks; subtasks = NULL;
    }

    subtasks = born;
  }
}

void ComplexTask::markup(const Token& t)
{
  int i;

  for(i = 0; i < used; i++){
    subtasks[i]->markup(t);
  }
}

void ComplexTask::data(const char *d, size_t len)
{
  int i;

  for(i = 0; i < used; i++){
    subtasks[i]->data(d, len);
  }
}


#if TEST_TASK

#include <stdio.h>

void TestTask::markup(const Token &t)
{
  switch(t.type()){
  case START:
    printf("\nStart Element: %s\n", t.giName());

    const AttributeRec *a;

    for(a = t.GetFirstAttr(); a ; a = t.GetNextAttr(a)){
      const char *ty = SGMLName::lookup(a->getAttrType());
      printf("Attribute: %s = [%s]`%s'\n",
	     SGMLName::lookup(a->getAttrName()), ty, a->getAttrValueString());
    }

    break;

  case END:
    printf("End Element: %s\n", t.giName());
    break;

  default:
    printf("Unknown Token Type: %d\n", t.type());
    abort();
    break;
    
  }
}


void TestTask::data(const char *data, size_t)
{
  printf("data: `%s'\n", data);
}

/*
 * TestTask 2 is for OL-Data testing
 */

#include <stdio.h>
#include "OLAF.h"
#include "OL-Data.h"
TestTask2::TestTask2()
{
  f_base = -1;
}

void TestTask2::markup(const Token &t)
{

  ComplexTask::markup( t );
  
  switch(t.type()){
  case START:

    /*
     * See if any OLIAS Architecture Form exists
     */

    if ( t.AttributeMatch( OLAF::OLIAS, OLAF::Graphic ) ) {
      f_base = t.level();

      if ( t.LookupAttr( OLAF::OL_data ) ) 
	ComplexTask::addSubTask( new OL_Data ( t, OLAF::OL_data ) );
    }
    break;

  case END:
    if ( f_base == t.level() ) {
      for ( int i=0; i < ComplexTask::used; i++ ) {
	OL_Data *t = ( OL_Data *) subtask(i);
	cout << "OL_Data found = " << t->content() << endl;
      }
    }
    break;

  default :
    break;

  }
}


void TestTask2::data(const char *data, size_t t)
{
  ComplexTask::data( data, t );
}

#endif
