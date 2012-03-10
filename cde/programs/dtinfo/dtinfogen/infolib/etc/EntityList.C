/* $XConsortium: EntityList.cc /main/2 1996/07/18 16:11:54 drk $ */

/* exported interfaces */
#include "EntityList.h"

/* imported interfaces */
#include <stdio.h>
#include "SGMLName.h"
#include "SGMLDefn.h"

//---------------------------------------------------------
EntityList::EntityList()
{
  head = NULL;
  tail = NULL;
  next = NULL;
}

//---------------------------------------------------------
EntityList::~EntityList()
{
  SGMLDefn *pt = head;
  while ( pt ) {
    SGMLDefn *tmp = pt;
    pt = pt->next;
    delete tmp;
  }
}
//---------------------------------------------------------
// EntityList:lookup
SGMLDefn *
EntityList::lookup(int ename ) const
{

  SGMLDefn *pt = head;

  while ( pt ) {

    if ( pt->getName() == ename ) {
      return ( pt );
    }

    pt = pt->next;
  }

  return ( NULL );

}

//---------------------------------------------------------
// EntityList::add
void
EntityList::insert( SGMLDefn *entry )
{

  if ( !tail ) {
    head = tail = entry;
  }
  else {
    tail->next = entry;
    tail       = entry;
  }

}
