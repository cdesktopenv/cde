/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
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
  int len = strlen(str);
  char *p = new char[len + 1];
  *((char *) memcpy(p, str, len) + len) = '\0';
  
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
