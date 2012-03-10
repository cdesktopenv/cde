/* $XConsortium: StringList.cc /main/2 1996/07/18 15:19:21 drk $ */

/* exported interfaces... */
#include "StringList.h"

/* imported interfaces... */
#include <string.h>

StringList::StringList()
{
  used = alloc = 0;
  items = NULL;
}

StringList::~StringList()
{
  reset();
  delete items;
}

void StringList::grow(size_t total)
{
  if(total + 1 > alloc){
    char **born = new char*[alloc = total * 3 / 2 + 1];
    
    if(used > 0){
      memcpy(born, items, used *sizeof(items[0]));
      delete items;
    }

    items = born;
  }
}


const char * StringList::append(const char *str)
{
  char *p = new char[strlen(str)+1];
  strcpy(p, str);
  
  grow(used);
  
  items[used++] = p;

  return p;
}

void StringList::add(char *str)
{
  grow(used);
  
  items[used++] = str;
}

void StringList::reset()
{
  for(size_t i = 0; i < used; i++){
    delete items[i]; 
    items[i] = 0;     /* This is to prevent the item[i] from being deleted
                       * again, ie free memory freed.
                       */
  }

  used = 0;
}
