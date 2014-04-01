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
/* $TOG: SGMLName.C /main/3 1998/04/17 11:23:18 mgreess $ */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "SGMLName.h"

/****** private interface *******/

typedef struct {
  char *string;
  int   code;
}Item;


class StringToInt{

public:
  StringToInt();

  int          intern(const char* name);
  const char*  lookup(int);

protected:
  void         grow(size_t);
  
private:

  Item *items;

  size_t    used;
  size_t    total;
};


static int
whichString (const char  * target,
	     const Item  * table,
	     int qty);


/****** implementation *******/

static StringToInt *theTable = NULL;

int
SGMLName::intern(const char *name, int upcase)
{

  static char *buf = 0;
  static size_t buflen = 0;

  const char *n;

  if(!theTable) theTable = new StringToInt();
  
  if(upcase){
    size_t nlen = strlen(name);
    
    if(buflen < nlen + 1){
      delete buf;
      buf = new char[buflen = nlen * 3 / 2 + 10];
    }

    const char *src;
    char *dest;
    for(src = name, dest=buf; *src; src++, dest++){
      *dest = toupper((unsigned char) *src);
    }
    *dest = 0;
    
    n = buf;
  }else{
    n = name;
  }
    
  return theTable->intern(n);
}


const char*
SGMLName::lookup(int indx)
{
  return theTable->lookup(indx);
}


StringToInt::StringToInt()
{
  items = 0;
  total = used = 0;
}


int
StringToInt::intern(const char *name)
{
  int indx;

  if( (indx = whichString(name, items, used)) < 0){
    grow(used + 1);

    int len = strlen(name);
    char *p = new char[len + 1];
    *((char *) memcpy(p, name, len) + len) = '\0';

    indx = used;

    while(indx > 0 && strcmp(name, items[indx-1].string) < 0){
      items[indx] = items[indx-1];
      indx--;
    }
    
    items[indx].string = p;
    items[indx].code = used;
    used++;
  }

  return items[indx].code;
}


void StringToInt::grow(size_t needed)
{

  if(total < needed){
    Item *born = new Item[total = needed * 3 / 2 + 10];
    
    if(used){
      memcpy(born, items, sizeof(Item) * used);
      delete items;
    }

    items = born;
  }

}


const char* StringToInt::lookup(int indx)
{
  unsigned int i;
  
  for(i = 0; i < used; i++){
    if (items[i].code == indx) return items[i].string;
  }

  abort();

  return NULL; /* avoid compiler warning */
}

/*
** Binary search for an array of strings for a given string.
** return index into the table, or -1 if not found.
*/

static int
whichString (const char  * target,
	     const Item  * table,
	     int qty)
{
  int low = 0;
  int high = qty;
  int i, diff;

  while(high > 0){
    i = (high + low)/2;

    diff = strcmp(table[i].string, target);
    if(diff == 0) {                     /* success: found it */
      return i;
    }else if(low + 1 >= high) break;
    else if(diff > 0) high = i;
    else low = i;
  }
  
  return -1;
}



const int NAMECASE = 1; /* Only one SGML Decl. supported */


void SGMLName::init()
{
#define INTERN(n) intern(#n, NAMECASE);
  INTERN(IMPLIED);
  INTERN(CDATA);
  INTERN(NOTATION);
  INTERN(TOKEN);
  INTERN(ENTITY);
}


#ifdef TEST

#include <stdio.h>

int
main(int argc, char **)
{
  char buf[100];
  int upcase = 0;

  if(argc > 1) upcase = 1;
  
  while(fgets(buf, sizeof(buf)-1, stdin) != NULL){
    int indx;
    
    printf("intern: %d\n", indx = SGMLName::intern(buf, upcase));
    printf("lookup: %s\n", SGMLName::lookup(indx));
  }
}

#endif

    
