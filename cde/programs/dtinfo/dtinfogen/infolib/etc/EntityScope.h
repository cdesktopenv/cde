/* $XConsortium: EntityScope.h /main/2 1996/07/18 16:43:47 drk $ */
#ifndef ENT_SCOPE_HDR
#define ENT_SCOPE_HDR

#include "EntityList.h"

class EntityScope {
private:
  EntityList *currentEntityScope;

public:
  void push ( EntityList *escope ) {
    escope->next = currentEntityScope;
    currentEntityScope = escope;
  }

  EntityList *pop();
  EntityList *GetTopEntities() const { return(currentEntityScope); }

  SGMLDefn *LookupEntity( int ename );

  /*
   * Constructor EntityScope creates a topmost entitylist
   * so the stack will contain an element after it is constructed
   */
  
  EntityScope() { currentEntityScope = new EntityList(); }
  ~EntityScope();

};

//----------------------------------------------------------------
inline
SGMLDefn *
EntityScope::LookupEntity( int ename )
{
  return ( currentEntityScope->lookup( ename ) );
}

//-----------------------------------------------------------------
inline
EntityList *
EntityScope::pop()
{
  EntityList *ptr;
  
  ptr = currentEntityScope;
  currentEntityScope = ptr->next;
  return ( ptr );
}

//-----------------------------------------------------------------
inline
EntityScope::~EntityScope()
{
  EntityList *pt = currentEntityScope;
  while( pt ) {
    EntityList *tmp = pt;
    pt = pt->next;
    delete tmp;
  }
}

#endif

  
