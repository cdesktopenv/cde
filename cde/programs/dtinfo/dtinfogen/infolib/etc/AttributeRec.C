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
/* $XConsortium: AttributeRec.cc /main/4 1996/07/18 16:08:22 drk $ */
#include <string.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
#include <assert.h>

#include "SGMLDefn.h"
#include "SGMLName.h"
#include "AttributeRec.h"

#ifdef FISH_DEBUG
#include <dbug.h>
#endif

//---------------------------------------------------------------------
AttributeRec::AttributeRec( const char *name,
			    const char *value,
			    int type )
{
#ifdef FISH_DEBUG
  DBUG_PRINT("AttributeRec", ("name = %s\n"
			      "value = %s\n"
			      "type = %d", name, value, type));
#endif
  
  int len;
  next=NULL;
  attName = SGMLName::intern( name );

  switch(attType = type){
  case SGMLName::TOKEN:
  case SGMLName::NOTATION:
  case SGMLName::ENTITY:
    attValue = SGMLName::intern( value );
    attValueString = SGMLName::lookup(attValue);
    copy = NULL;
#ifdef FISH_DEBUG
    DBUG_PRINT("AttributeRec", ("attValue after the intern: %d", attValue));
#endif
    break;

  case SGMLName::CDATA:
    attValue = -1;
    len = strlen(value);
    attValueString = copy = new char[len + 1];
    *((char *) memcpy(copy, value, len) + len) = '\0';
    break;

  default:
    abort();
  }
  
}

//---------------------------------------------------------------------
AttributeRec::~AttributeRec()
{
  delete copy;
}

//---------------------------------------------------------------------
int
AttributeRec::getAttrName() const
{
  return(attName);
}

