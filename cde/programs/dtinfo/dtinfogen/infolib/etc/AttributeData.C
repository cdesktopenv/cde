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
  if ( str = attribute_value( t, attr_name ) ) {
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
}

