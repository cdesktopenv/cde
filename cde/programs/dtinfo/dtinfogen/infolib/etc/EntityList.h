/* $XConsortium: EntityList.h /main/2 1996/07/18 16:43:25 drk $ */
#ifndef ENT_LIST_HDR
#define ENT_LIST_HDR

#include "SGMLDefn.h"

class EntityList {

friend class EntityScope;
friend class Dispatch;
  
protected:
  SGMLDefn *head;
  SGMLDefn *tail;
  EntityList *next;

  SGMLDefn *lookup( int ) const;
  void      insert  ( SGMLDefn * );

  // SGMLDefn *GetFirstAttr() const;
  // SGMLDefn *GetNextAttr( const SGMLDefn *) const;

  EntityList();
  ~EntityList();
};

#endif


