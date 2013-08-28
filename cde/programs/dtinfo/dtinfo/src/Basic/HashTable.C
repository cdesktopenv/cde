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
/*
 * $XConsortium: HashTable.cc /main/3 1996/06/11 16:18:45 cde-hal $
 *
 * Copyright (c) 1991 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */

#define C_StringHash
#define C_HashTable
#define C_List
#define L_Basic
#include <Prelude.h>

#include <string.h>

class HashObject
{
  public:
    HashObject(const char *key, FolioObject *object);
    ~HashObject();

    FolioObject *object();

    const char *key();

private:
    char       	*f_key ;
    FolioObject	*f_object ;
};


inline FolioObject *
HashObject::object()
{ 
    return f_object; 
}

HashObject::HashObject(const char *key, FolioObject *object) 
: f_object(object)
{
    assert(key != NULL);
    assert(object != NULL);

    int len = strlen(key);
    f_key = new char[len + 1] ;
    *((char *) memcpy(f_key, key, len) + len) = '\0';
}

HashObject::~HashObject()
{
    // NOTE: this is virtual, don't make it inline
    assert(f_key != NULL);
    delete f_key ;
    delete f_object ;		// delete the object 
}

inline const char *
HashObject::key()
{
    return f_key ;
}

HashTable::~HashTable()
{
  ON_DEBUG(cerr << "~HashTable" << endl);
  for (int i = 0 ; i < HASH_TABLE_SIZE; i++)
    if (f_table[i] != NULL)
      {
	List_Iterator<HashObject *> cursor(f_table[i]);
	while (cursor)
	  {
	    delete cursor.item();
	    f_table[i]->remove(cursor);
	  }
	delete f_table[i] ;
      }
}

HashTable::HashTable()
{
  for (int i = 0 ; i < HASH_TABLE_SIZE; i++)
    f_table[i] = 0 ;
}

void 
HashTable::add(const char *key, FolioObject *object)
{
    int position = hash(key) ;

//    ON_DEBUG(cerr << "HashTable add " << key << " ==> " << position << endl;);

    HashObject *hash_object = new HashObject(key, object);

    assert(hash_object != NULL);

    if (f_table[position] == NULL)
      {
	f_table[position] = new xList<HashObject *> ;
      }

    f_table[position]->insert(hash_object);

}


void *
HashTable::find(const char *key)
{
    unsigned int position = hash(key);

//    ON_DEBUG(cerr << "Hash Table find " << key << " ==> " << position << endl);

    void *rvalue = NULL ;
    
    if (f_table[position] != NULL)
      {
	List_Iterator<HashObject *> cursor(f_table[position]);
	
	for (; cursor; cursor++)
	  if (!strcmp(cursor.item()->key(), key))
	    {
	      rvalue =  cursor.item()->object();
	      break;
	    }
      }
    return rvalue ;
}

unsigned int
HashTable::hash (const char *key)
{
    return string_hash(key) % HASH_TABLE_SIZE;
}
