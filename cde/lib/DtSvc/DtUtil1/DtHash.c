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
/* $XConsortium: DtHash.c /main/4 1996/05/09 04:21:56 drk $ */
/*

    Name:  hash.c

Synopsis:  

           #include "DtHash.h"

           void * _DtUtilMakeHash(int size)

	   void * _DtUtilMakeIHash(int size)

           void ** _DtUtilGetHash(void * tbl, unsigned char * key)

	   void ** _DtUtilFindHash(void * tbl,unsigned char * key)

	   void ** _DtUtilDelHash(void * tbl, unsigned char * key)

	   int _DtUtilOperateHash(void * tbl, void (*op_func)(), void * usr_arg)

	   void _DtUtilDestroyHash(void * tbl, int (*des_func)(), void * usr_arg)


 Description: 

   These routines provide a general purpose hash table facility that
supports multiple open hash tables.  Each entry in the table consists of a
key and a data ptr.  The key is a null terminated character string, while
the data ptr is opaque. Since all the entries are maintained in a doubly
linked lists, deletions and operations on entire table execute very quickly.
This make these routines suitable for use when the tables may be very ephemeral.

  DtUtilMakeHash returns a pointer to the created table.  The size argument
indicate the number of buckets the routine is to allocate.  This should be ~
the max number of items expected in the table for maximum performance....
but /2 or /3 should still be ok.  Note that for maximum efficiency the hash
table size should be a prime number (a side effect of the hash alorithm).

  DtUtilMakeIHash performs the same function as DtUtilMakeHash, except that the hash
routines will use the key arguments as arbitrary integers rather than strings.

  DtUtilGetHash searches the specified hash table tbl for an entry with the
specified key.  If the entry does not exist, it is created with a NULL data
ptr.  The routine returns a ptr to the area where the data ptr is (can be)
stored.

  DtUtilFindHash searchs the table for an entry with the specified key.  If the
entry is found, the address of the data pointer associated with the key is
returned.  If no such entry exists, the routine returns NULL.

  DtUtilDelHash deletes the specified table entry and returns the associated data
ptr.  If the entry did not exist ( or the data ptr was NULL), the routine
returns NULL.

  DtUtilOperateHash calls the routine pointed to by op_func once for each entry
in tbl, with three arguments: the data ptr, the usr_arg ptr and a ptr to the
key for that entry (which should NOT be altered).  This is useful for
transversing a hash table quickly and operating on the entries. Note that
the order of the traversal of the hash table is the reverse order of
insertion.

  DtUtilDestroyHash destroys the specified hash table after operating on it
with the specified des_func function as described for DtUtilOperateHash.  All storage
allocated by the hash routines is reclaimed.

Author:  Bart Smaalders 1/89


*/
#include <stdio.h> /* grab NULL define */
#include "DtHash.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

static int hash_string(const unsigned char * s, int modulo);

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


DtHashTbl _DtUtilMakeHash(int size)
{
  hash * ptr;

  ptr        = (hash *) malloc(sizeof(*ptr));
  ptr->size  =   size;
  ptr->table = (hash_entry **) malloc( (unsigned) (sizeof(hash_entry *) * size) );
  (void)memset((char *) ptr->table, (char) 0, sizeof(hash_entry *)*size);
  ptr->start = NULL;
  ptr->hash_type = String_Key;
  return((DtHashTbl)ptr);
}

DtHashTbl _DtUtilMakeIHash(int size)
{
  hash * ptr;

  ptr        = (hash *) malloc(sizeof(*ptr));
  ptr->size  =   size;
  ptr->table = (hash_entry **) malloc( (unsigned) (sizeof(hash_entry *) * size) );
  (void)memset((char *) ptr->table, (char) 0, sizeof(hash_entry *)*size);
  ptr->start = NULL;
  ptr->hash_type = Integer_Key;
  return((DtHashTbl) ptr);
}

  
void ** _DtUtilGetHash(DtHashTbl t, const unsigned char * key)
{	
  hash * tbl = (hash *) t;
  register int bucket;
  register hash_entry * tmp;
  hash_entry * new;

  if(tbl->hash_type == String_Key)
    tmp = tbl->table[bucket = hash_string(key, tbl->size)];
  else
    tmp = tbl->table[bucket = abs((int)(intptr_t)key) % tbl->size];

  if(tbl->hash_type == String_Key)
    while(tmp!=NULL)
      {	
	if(strcmp((char *)tmp->key, (char*)key)==0)
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
  new->key = (unsigned char *)((tbl->hash_type == String_Key)?(unsigned char *)strdup((char*)key):key);
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

void ** _DtUtilFindHash(DtHashTbl t, const unsigned char * key)
{
  register hash * tbl = (hash *) t;
  register hash_entry * tmp;

  if(tbl->hash_type == String_Key)
    {
      tmp = tbl->table[hash_string(key, tbl->size)];
      for(;tmp!=NULL; tmp = tmp->next_entry)
	if(!strcmp((char*)tmp->key, (char*)key))
	  return((void *)&tmp->data);
    }
  else
    {
      tmp = tbl->table[abs((int)(intptr_t)key) % tbl->size];
      for(;tmp!=NULL; tmp = tmp->next_entry)
	if(tmp->key == key)
	  return((void *)&tmp->data);
    }

  return(NULL);
}

void * _DtUtilDelHash(DtHashTbl t, const unsigned char * key)
{
  register hash * tbl = (hash *) t;

  register int bucket;
  register hash_entry * tmp, * prev = NULL;

  if(tbl->hash_type == String_Key)
    bucket = hash_string(key, tbl->size);
  else
    bucket = abs((int)(intptr_t)key) % tbl->size;

  if((tmp = tbl->table[bucket])==NULL)
    return(NULL);

  else
    {
      if(tbl->hash_type == String_Key)
	while(tmp!=NULL)
	  {	
	    if(!strcmp((char*)tmp->key, (char*)key))
	      break;  /* found item to delete ! */
	    prev = tmp;
	    tmp  = tmp->next_entry;
	  }
      else
	while(tmp!=NULL)
	  {
	    if(tmp->key == key)
	      break;
	    prev = tmp;
	    tmp  = tmp->next_entry;
	  }
      if(tmp == NULL)
	return(NULL); /* not found */
    }
      /* tmp now points to entry marked for deletion, prev to 
         item preceeding in bucket chain or NULL if tmp is first.
	 
	 remove from bucket chain first....

	 */
  if(tbl->hash_type == String_Key)  
    free(tmp->key);
  if(prev!=NULL)
    prev->next_entry = tmp->next_entry;
  else
    tbl->table[bucket] = tmp->next_entry;
  /*
    now remove from tbl chain....
    */
  if(tmp->right_entry !=NULL) /* not first in chain.... */
    tmp->right_entry->left_entry = (tmp->left_entry ? 
				    tmp->left_entry->right_entry: NULL);
      else	
	tbl->start = (tmp->left_entry ?tmp->left_entry->right_entry: NULL);
  return(tmp->data);	
}

int _DtUtilOperateHash(DtHashTbl t, void (*ptr)(), void * usr_arg)
{
  hash * tbl = (hash *) t;
  register hash_entry * tmp = tbl->start;
  int c = 0;

  while(tmp)
    {
      (*ptr)(tmp->data,usr_arg, tmp->key);
      tmp = tmp->left_entry;
      c++;
    }
  return(c);
}

void _DtUtilDestroyHash(DtHashTbl t, int (*ptr)(), void * usr_arg)
{
  hash * tbl = (hash *) t;
  register hash_entry * tmp = tbl->start, * prev;

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

static int hash_string(register const unsigned char * s, int modulo)
{
	register unsigned result = 0;
	register int i=1;

 	while(*s!=0)
	  result += (*s++ << i++);

 	return(result % modulo); 
}

