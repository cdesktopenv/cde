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
/* $XConsortium: Token.C /main/6 1996/08/21 15:47:50 drk $ */

/* exported interfaces */
#include "Token.h"

/* imported interfaces */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "FlexBuffer.h"
#include "SGMLName.h"
#include "SGMLDefn.h"
#include "AttributeRec.h"
#include "AttributeList.h"
#include "OLAF.h"

#ifdef FISH_DEBUG
#include "dbug.h"
#endif

//---------------------------------------------------------------------
Token::Token()
{

#ifdef FISH_DEBUG
  DBUG_ENTER("Token::Token");
#endif

  GiValue=-1;
  entity_name = -1;
  entity_defn = NULL;
  tokType=TK_INVALID;
  f_olaf = -1;
  
#ifdef FISH_DEBUG
  DBUG_VOID_RETURN;
#endif
}

//---------------------------------------------------------------------
const char *
Token::giName() const
{
  if ( GiValue != -1 ) {
    return ( SGMLName::lookup( GiValue ) );
  }
  else { return (NULL); }
}

//---------------------------------------------------------------------
const AttributeRec *
Token::GetFirstAttr() const
{
  return ( aList.GetFirstAttr() );
}

//---------------------------------------------------------------------
const AttributeRec *
Token::GetNextAttr( const AttributeRec *aPtr) const
{
  return ( aList.GetNextAttr( aPtr ) );
}
  

//---------------------------------------------------------------------
void Token::StoreAttribute ( const char *name,
			     const char *value,
			     int type )
{
  AttributeRec *aRec = new AttributeRec ( name, value,
					  (SGMLName::DeclaredValue)type );

  aList.insert( aRec );

  if(aRec->getAttrName() == OLAF::OLIAS){
    f_olaf = aRec->getAttValue();
  }
}

//---------------------------------------------------------------------
void
Token::StoreStartTag( unsigned char *giName, int lvl)
{
  GiValue = SGMLName::intern ( ( char *)giName );
  tokType = START;
  f_level = lvl;
}

//---------------------------------------------------------------------
void
Token::StoreEndTag(unsigned char *giName, int lvl)
{
   GiValue = SGMLName::intern ( ( char *)giName );
   tokType = END;
   f_level = lvl;
}

//---------------------------------------------------------------------
void
Token::StoreEntity( unsigned char *ename, TOKEN_TYPE entity_type )
{
  const char *str = ( const char *)ename;

  entity_name = SGMLName::intern( str );
  tokType = entity_type;
}

//---------------------------------------------------------------------
void
Token::StoreSubDoc ( TOKEN_TYPE ttype )
{
  tokType = ttype;
}

//---------------------------------------------------------------------
void
Token::SetEntityValue( SGMLDefn *defn )
{
  entity_defn = defn;
}

//---------------------------------------------------------------------
const AttributeRec *
Token::LookupAttr( int Name ) const
{
  for ( const AttributeRec *aRec=GetFirstAttr();
	aRec;
	aRec=GetNextAttr(aRec) ) {
    if ( aRec->getAttrName() == Name ) {
      return(aRec);
    }
  }

  return(NULL);
}
      

      
//---------------------------------------------------------------------
int
Token::AttributeMatch( int aName, int aValue ) const
{
  
  for ( const AttributeRec *aPtr = GetFirstAttr();
	aPtr;
	aPtr=GetNextAttr( aPtr ) ) {
    if ( aPtr->getAttrName() == aName ) {
      return( aPtr->getAttValue() == aValue );
    }
  }

  return(0);
}


//---------------------------------------------------------------------

static void
signalErrorVA(int src, int sev, const char *file, int line,
	      const char *fmt, va_list ap)
{
  static const char *srcMsg[] = { "Error", "**Internal Error**" };

  if(file){
    fprintf(stderr, "DtInfo Toolkit: %s at %s:%d: ", srcMsg[src], file, line);
  }else{
    fprintf(stderr, "DtInfo Toolkit: %s: ", srcMsg[src]);
  }
  
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");

  if(sev == Token::Fatal){
    throw(ErrorReported(src, sev));
  }
}


void
Token::signalError(int src, int sev, const char *file, int line,
		   const char *fmt, ...)
{
  va_list ap;
  
  va_start(ap, fmt);

  signalErrorVA(src, sev, file, line, fmt, ap);

  va_end(ap);
}


void
Token::reportError(int src, int sev,
		   const char *fmt, ...)
{
  va_list ap;
  
  va_start(ap, fmt);

  signalErrorVA(src, sev, file(), line(), fmt, ap);

  va_end(ap);
}


