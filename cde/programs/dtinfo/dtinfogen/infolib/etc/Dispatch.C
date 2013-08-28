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
/* $XConsortium: Dispatch.cc /main/2 1996/06/11 16:49:03 cde-hal $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/* exported interfaces... */
#include "Dispatch.h"

/* imported interfaces... */
#include <assert.h>
#include <string.h>

#include "SGMLName.h"
#include "EntityScope.h"
#include "Token.h"
#include "FlexBuffer.h"
#include "SGMLDefn.h"
#include "SearchPath.h"

#include "OLAF.h"

Task *Dispatch::TaskObject = NULL;
Stack<int> *Dispatch::IgnoreStack = NULL;
int Dispatch::level = 0;
static EntityScope entity_stack;
Token*  Dispatch::tok = new Token();

char* Dispatch::f_file = NULL;
int   Dispatch::f_line = 0;

const char *Dispatch::tmpdir = NULL;
const char *Dispatch::srcdir = NULL;
SearchPath *Dispatch::search_path_table = NULL;

int Dispatch::tocgen_only = 0;

//---------------------------------------------------------------------
void
Dispatch::token(TOKEN_TYPE tokType, unsigned char *Name )
{

  tok->setFileLine(f_file, f_line);

  switch(tokType){
  case START:
    level++;
    tok->StoreStartTag( Name, level );

    if ( tok->LookupAttr( OLAF::OL_Ignore ) 
	 || tok->LookupAttr( OLAF::OL_ShortTitle ) )  {
      IgnoreStack->push ( level );
    }

    break;

  case END:
    tok->StoreEndTag ( Name, level );
    level--;
    break;
    
  case INTERNAL_ENTITY:
    tok->StoreEntity( Name , INTERNAL_ENTITY);
    break;
    
  case EXTERNAL_ENTITY:
    tok->StoreEntity( Name , EXTERNAL_ENTITY);

    SGMLDefn *sgml_rec;
    sgml_rec = entity_stack.LookupEntity( SGMLName::intern((char*)Name) );

    if(sgml_rec){
      tok->SetEntityValue( sgml_rec );
    }else{
      tok->reportError(Token::Internal, Token::Fatal,
		      "Unable to find entity definition for %.50s", Name);
    }
    break;
  default:
    break;
  }
  

  TaskObject->markup( *tok );

  if ( tokType == END ) {
    if ( !IgnoreStack->empty() ) {

      int topelement = IgnoreStack->top();
      if ( topelement == tok->level() ) {
	topelement = IgnoreStack->pop();
      }
    }
  }

  delete tok;
  tok = new Token();
}



//---------------------------------------------------------------------
void Dispatch::setRoot( Task *t, Stack<int> *istack)
{
  assert(TaskObject == NULL);
  TaskObject = t;

  assert(IgnoreStack == NULL);
  IgnoreStack = istack;
  
}

//---------------------------------------------------------------------
SGMLDefn *
Dispatch::entity_ref( const char *ent_name )
{
  int ent_num;
  ent_num = SGMLName::intern(ent_name);
  SGMLDefn *sgml_rec = entity_stack.LookupEntity( ent_num );
  return sgml_rec;
}

  
//---------------------------------------------------------------------
void
Dispatch::entity_decl( SGMLDefn *defn )
{
  EntityList *escope = entity_stack.GetTopEntities();

  assert(escope != NULL);

  /*
   * first clone up an SGMLDefn record for storage purposes
   */
  
  SGMLDefn *sgmlRec = new SGMLDefn();
  *sgmlRec = *defn;

  escope->insert( sgmlRec );
}

//---------------------------------------------------------------------
void
Dispatch::file(const char *f)
{
  int len = strlen(f);
  delete f_file;
  f_file = new char[len + 1];
  *((char *) memcpy(f_file, f, len) + len) = '\0';

  
  /*
   * put directory of f_file into the search path also 
   */
  if(search_path_table){
    // perform dirname first
    char *dirname = strdup( f_file );
    char *p = strrchr( dirname, '/' );
    if ( p ) {
      *p = '\0';
    }
    else {
      len = MIN(strlen(dirname), 1);
      *((char *) memcpy(dirname, ".", len) + len) = '\0';
    }
    
    search_path_table->replace_file_scope( dirname );
    delete dirname;
  }
}

//---------------------------------------------------------------------
void
Dispatch::subdoc_start()
{
  EntityList *newrec = new EntityList();
  entity_stack.push ( newrec );
}

//---------------------------------------------------------------------
void
Dispatch::subdoc_end()
{
  EntityList *rec = entity_stack.pop();
    
  if ( !rec ) {
    throw(Unexpected("SUBDOC end tag is not matched"));
  }

  delete rec;

}
