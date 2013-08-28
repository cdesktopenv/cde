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
/* $XConsortium: AttributeData.cc /main/2 1996/07/18 16:07:35 drk $ */
/* exported interfaces */
#include "AttributeData.h"

/* imported interfaces */
#include "SGMLName.h"
#include "Token.h"
#include "Dispatch.h"

//-------------------------------------------------------------------------
AttributeData::AttributeData( const Token &t, 
			      int attr_name, 
			      ActionType mode ):BaseData(t, mode)
{

  const char *str;
  if ( (str = attribute_value( t, attr_name )) ) {
    data_complete = 1;
    ValueBuffer.writeStr( str );
  }
  else {
    data_avail = 0;
  }
}

//-------------------------------------------------------------------------
const char *
AttributeData::attribute_value( const Token &t, int attributeName )
{
  const AttributeRec *tmp;
  int att_type;
  
  tmp = t.LookupAttr( attributeName );
  if ( !tmp ) {
    return 0;
  }

  att_type = tmp->getAttrType();
  if ( att_type == SGMLName::ENTITY ) {
    SGMLDefn *sgml_defn = Dispatch::entity_ref( tmp->getAttrValueString() );
    if ( !sgml_defn ) {
      throw(Unexpected("no entity declaration for this entity"));
    }

    /*
     * Use the file name
     */

    return ( sgml_defn->getFileName() );
  }
  else if ( att_type == SGMLName::CDATA ||
	    att_type == SGMLName::TOKEN ){
    return ( tmp->getAttrValueString() );
  }
  else {
    return 0;
  }
}

