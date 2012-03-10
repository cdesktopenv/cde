/* $XConsortium: AttributeRec.h /main/3 1996/08/21 15:46:05 drk $ */

//----------------------------------------------
// AttributeRec.h

#ifndef ATT_REC_HDR
#define ATT_REC_HDR

#include "SGMLName.h"

class SGMLDefn;

class AttributeRec {

friend class OL_AttributeList;
friend class Token;
  
private:
  
  int attName;
  int attType;
  const char *attValueString;
  char  *copy;
  int  attValue;
  AttributeRec *next;
  
protected:
  AttributeRec( const char * name, const char *value,
		int type );
  ~AttributeRec();

public:  
  int getAttrName() const;

  /* use this for CDATA attributes */
  const char *getAttrValueString() const { return(attValueString); }

  /* use this for NAME, NOTATION, ENTITY attributes */
  int   getAttValue()        const    { return(attValue); } 

  int getAttrType() const    { return(attType); }

  int operator== ( AttributeRec & s ) {
    return ( attName == s.attName );
  }
};

#endif
