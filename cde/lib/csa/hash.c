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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: hash.c /main/1 1996/04/21 19:23:21 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

/*

Synopsis:  

           #include "hash.h"

           void * _DtCmMakeHash(int size)

	   void * _DtCmMakeIHash(int size)

           void ** _DtCmGetHash(void * tbl, unsigned char * key)

	   void ** _DtCmFindHash(void * tbl,unsigned char * key)

	   void ** _DtCmDelHash(void * tbl, unsigned char * key)

	   int _DtCmOperateHash(void * tbl, void (*op_func)(), void * usr_arg)

	   void _DtCmDestroyHash(void * tbl, int (*des_func)(), void * usr_arg)


 Description: 

   These routines provide a general purpose hash table facility that
supports multiple open hash tables.  Each entry in the table consists of a
key and a data ptr.  The key is a null terminated character string, while
the data ptr is opaque. Since all the entries are maintained in a doubly
linked lists, deletions and operations on entire table execute very quickly.
This make these routines suitable for use when the tables may be very ephemeral.

  _DtCmMakeHash returns a pointer to the created table.  The size argument
indicate the number of buckets the routine is to allocate.  This should be ~
the max number of items expected in the table for maximum performance....
but /2 or /3 should still be ok.  Note that for maximum efficiency the hash
table size should be a prime number (a side effect of the hash alorithm).

  _DtCmMakeIHash performs the same function as _DtCmMakeHash, except that the hash
routines will use the key arguments as arbitrary integers rather than strings.

  _DtCmGetHash searches the specified hash table tbl for an entry with the
specified key.  If the entry does not exist, it is created with a NULL data
ptr.  The routine returns a ptr to the area where the data ptr is (can be)
stored.

  _DtCmFindHash searchs the table for an entry with the specified key.  If the
entry is found, the address of the data pointer associated with the key is
returned.  If no such entry exists, the routine returns NULL.

  _DtCmDelHash deletes the specified table entry and returns the associated data
ptr.  If the entry did not exist ( or the data ptr was NULL), the routine
returns NULL.

  _DtCmOperateHash calls the routine pointed to by op_func once for each entry
in tbl, with three arguments: the data ptr, the usr_arg ptr and a ptr to the
key for that entry (which should NOT be altered).  This is useful for
transversing a hash table quickly and operating on the entries. Note that
the order of the traversal of the hash table is the reverse order of
insertion.

  _DtCmDestroyHash destroys the specified hash table after operating on it
with the specified des_func function as described for _DtCmOperateHash.  All storage
allocated by the hash routines is reclaimed.

Author:  Bart Smaalders 1/89


*/
#include <EUSCompat.h>
#include <stdio.h> /* grab NULL define */
#include <stdlib.h>
#include <string.h>
#include "hash.h"

static int    hash_string(const char *s, int modulo);

typedef struct hash_entry {
  struct hash_entry 
    * next_entry,
    * right_entry,
    * left_entry;
  unsigned char *       	key;
  void 		* 		data;
} hash_entry;

typedef struct hash {
  int 		size;
  hash_entry ** table;
  hash_entry * 	start;   
  enum hash_type { String_Key = 0 , Integer_Key = 1} hash_type;
} hash;


void * _DtCmMakeHash(int size)
{
  hash * ptr;

  ptr        = (hash *) malloc(sizeof(*ptr));
  ptr->size  =   size;
  ptr->table = (hash_entry **) malloc( (unsigned) (sizeof(hash_entry *) * size) );
  (void)memset((char *) ptr->table, (char) 0, sizeof(hash_entry *)*size);
  ptr->start = NULL;
  ptr->hash_type = String_Key;
  return((void*)ptr);
}

void ** _DtCmGetHash(void * t, const unsigned char * key)
{	
  hash * tbl = (hash *) t;
  int bucket;
  hash_entry * tmp;
  hash_entry * new;

  if(tbl->hash_type == String_Key)
    tmp = tbl->table[bucket = hash_string(key, tbl->size)];
  else
    tmp = tbl->table[bucket = abs((long)key) % tbl->size];

  if(tbl->hash_type == String_Key)
    while(tmp!=NULL)
      {	
	if(strcmp((char *)tmp->key, (char *)key)==0)
	  return(&tmp->data);
	tmp = tmp->next_entry;
      }
  else
    while(tmp!=NULL)
      {	
	if(tmp->key == key)
	  return(&tmp->data);
	tmp = tmp->next_entry;
      }
    
  /*
    not found.... 
    insert new entry into bucket...
    */

  new = (hash_entry *) malloc(sizeof(*new));
  new->key = (unsigned char *)((tbl->hash_type == String_Key)?(unsigned char *)strdup((char *)key):key);
  /*
    hook into chain from tbl...
    */
  new->right_entry = NULL;
  new->left_entry = tbl->start;
  tbl->start = new;
  /*
    hook into bucket chain
    */
  new->next_entry = tbl->table[bucket];
  tbl->table[bucket] = new;
  new->data = NULL;   /* so we know that it is new */
  return((void*)& new->data);
}

void ** _DtCmFindHash(void * t, const unsigned char * key)
{
  hash * tbl = (hash *) t;
  hash_entry * tmp;

  if(tbl->hash_type == String_Key)
    {
      tmp = tbl->table[hash_string(key, tbl->size)];
      for(;tmp!=NULL; tmp = tmp->next_entry)
	if(!strcmp((char *)tmp->key, (char *)key))
	  return((void *)&tmp->data);
    }
  else
    {
      tmp = tbl->table[abs((long)key) % tbl->size];
      for(;tmp!=NULL; tmp = tmp->next_entry)
	if(tmp->key == key)
	  return((void *)&tmp->data);
    }

  return(NULL);
}

void _DtCmDestroyHash(void * t, int (*ptr)(void *, void *, unsigned char *), void * usr_arg)
{
  hash * tbl = (hash *) t;
  hash_entry * tmp = tbl->start, * prev;

  while(tmp)
    {
      if(ptr)
	(*ptr)(tmp->data,usr_arg, tmp->key);

      if(tbl->hash_type == String_Key)
	free(tmp->key);
      prev = tmp;
      tmp = tmp->left_entry;
      free((char *)prev);
    }
  free((char *)tbl->table);
  free(tbl);
}

static int hash_string(const char *s, int modulo)
{
	unsigned result = 0;
	int i=1;

 	while(*s!=0)
	  result += (*s++ << i++);

 	return(result % modulo); 
}

