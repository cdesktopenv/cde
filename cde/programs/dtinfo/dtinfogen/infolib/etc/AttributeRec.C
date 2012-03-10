/* $XConsortium: AttributeRec.cc /main/4 1996/07/18 16:08:22 drk $ */
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
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
    attValueString = copy = new char[ strlen(value) + 1 ];
    strcpy( copy, value );
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

