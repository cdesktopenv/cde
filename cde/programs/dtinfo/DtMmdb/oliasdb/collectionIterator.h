/* $XConsortium: collectionIterator.h /main/3 1996/06/11 17:28:33 cde-hal $ */

#ifndef _collection_iterator_h
#define _collection_iterator_h 1

#include "api/info_base.h"
#include "object/cset.h"

// collectionIterator iterates over MMDB's cset objects through
// index.

class collectionIterator
{
public:
   collectionIterator(info_base*, int set_position);
   ~collectionIterator();

// advance the iterator to the next position.
// A newly constructed iterator's position is
// undefined. This operator must be called to
// advance to the 1st position.

// return 1 if advance successful and 0 otherwize.
   virtual unsigned int operator++() ;

protected:
   oid_t get_oid(int index);

protected:
   int f_index;
   cset_handler* f_set_ptr;
   info_base* f_base;
};

// iterator over node set
class nodeCollectionIterator : public collectionIterator
{

public:
   nodeCollectionIterator(info_base*);
   ~nodeCollectionIterator();

   const char* get_locator();
};

// iterator over graphic set
class graphicCollectionIterator : public collectionIterator
{

public:
   graphicCollectionIterator(info_base*);
   ~graphicCollectionIterator();

   const char* get_locator();
};

// iterator over stylesheet set
class stylesheetCollectionIterator : public collectionIterator
{

public:
   stylesheetCollectionIterator(info_base*);
   ~stylesheetCollectionIterator();

   const char* get_locator();
};

// iterator over locator set
class locatorCollectionIterator : public collectionIterator
{

public:
   locatorCollectionIterator(info_base*);
   ~locatorCollectionIterator();

   const char* get_locator();
};

#endif
