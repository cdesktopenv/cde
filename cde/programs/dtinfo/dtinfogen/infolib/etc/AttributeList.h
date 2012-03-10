/* $XConsortium: AttributeList.h /main/3 1996/08/21 15:46:01 drk $ */
//---------------------------------------------------------
// AttributeList.h

#ifndef ATT_LIST_HDR
#define ATT_LIST_HDR

#include <stdio.h>
#include "AttributeRec.h"


class OL_AttributeList {

friend class Token;
  
protected:
  AttributeRec *head;
  AttributeRec *tail;

  AttributeRec *lookup ( int ) const;
  void  insert(AttributeRec * );

  const AttributeRec *GetFirstAttr() const;
  const AttributeRec *GetNextAttr( const AttributeRec *) const;
  OL_AttributeList();
  ~OL_AttributeList();
};

#endif


  
