/* $XConsortium: StringList.h /main/2 1996/07/18 15:19:39 drk $ -*- c++ -*- */

#ifndef __StringList_h
#define __StringList_h

#include <stddef.h> /* for size_t */

/***********************************
 *
 * StringList
 *
 ***********************************/

class StringList{
  
public:
  StringList(void);
  ~StringList();

  const char * append(const char*); /* returns a copy, owned by this obj. */

  void add(char*); /* caller relinquishes ownership */

  size_t qty() { return used; };

  const char *item(size_t indx) { return items[indx]; };
  const char **array() { return (const char **)items; };

  void reset();

protected:
  char **items;
  size_t used;

private:
  void grow(size_t n);

  size_t alloc;
};

#endif /* __StringList_h */
