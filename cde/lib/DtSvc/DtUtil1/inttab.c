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
/* $XConsortium: inttab.c /main/5 1996/05/09 04:23:37 drk $ */
/*
  routines to implement a table of int, int value pairs.
  the data is relocatable to allow use at various addresses
  (eg shared memory)

  -1 is an illegal key

  Tables are limited to 64K entries

  */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include "DtHash.h"
#include "DtShmDb.h"

#define NOT_AN_INDEX 0

static int build_it(int * data, void * usr_arg, int key);

typedef struct inttab {
  int key;
  int data;
  unsigned short first;
  unsigned short next;
} inttab_t;

typedef struct intlist {
  int num_entries;
  void * tbl;
} intlist_t;

typedef struct builder {
  int 	      counter;
  intlist_t * intlist_ptr;
  inttab_t  * inttab_ptr;
} builder_t;


/*
  first entry is header block;
  contains key = size (not counting header)
  */

const int *
_DtShmFindIntTabEntry(DtShmInttab inttab, unsigned int key)
{
  register const inttab_t * ptr = (const inttab_t *) inttab;
  
  register int i;
  
  if ( !ptr->key)
	return(0);

  i = ptr[key % ptr->key + 1].first;
  
  while(i && key != ptr[i].key)
    i = ptr[i].next;
  
  return(i?(&ptr[i].data):(const int *)NULL);
}

DtShmProtoInttab
_DtShmProtoInitInttab(int sizeguess)
{
  intlist_t * ptr = (intlist_t *)malloc(sizeof(*ptr));
  
  ptr->tbl = _DtUtilMakeIHash(sizeguess);;
  ptr->num_entries = 0;

  return((void*)ptr);
}

_DtShmProtoAddInttab(DtShmProtoInttab intlist, unsigned int keyin, int datain)
{
  intlist_t * ptr = (intlist_t *) intlist;
  int ** data;

  data = (int**)_DtUtilGetHash(ptr->tbl, (unsigned char *)keyin);

  if(!*data) /* new */ {
    *data = (int *) malloc(sizeof(int));
    ptr->num_entries++;
  }

  **data = datain;
  return(0);
}

int
_DtShmProtoSizeInttab(DtShmProtoInttab intlist)
{
  intlist_t * ptr = (intlist_t * ) intlist;

  return(sizeof(inttab_t) * (ptr->num_entries +1));
}

DtShmInttab
_DtShmProtoCopyInttab(DtShmProtoInttab intlist, void * destination)
{
  builder_t build;

  build.counter = 1;
  build.intlist_ptr = (intlist_t * ) intlist;
  build.inttab_ptr = (inttab_t *) destination;
  
  memset(destination, 0, (build.intlist_ptr->num_entries+1)*sizeof(inttab_t));

  build.inttab_ptr->key = build.intlist_ptr->num_entries;

  _DtUtilOperateHash(build.intlist_ptr->tbl, (DtHashOperateFunc)build_it, &build);

  return(0);
}

int
_DtShmProtoDestroyInttab(DtShmProtoInttab intlist)
{
  intlist_t * ptr = (intlist_t *)intlist;
  _DtUtilDestroyHash(ptr->tbl,  (DtHashDestroyFunc)free, NULL);
  free(intlist);
  return(0);
}

static int build_it(int * data, void * usr_arg, int key)
{
  builder_t * ptr = (builder_t *) usr_arg;
  inttab_t * a;
  inttab_t * b;
  unsigned short * add_ptr;

  int bucket = key % (ptr->intlist_ptr->num_entries) + 1;
 
  a = ptr->inttab_ptr + ptr->counter;
  a->key 	= key;
  a->data	= *data;
  a->next	= NOT_AN_INDEX;

  b = ptr->inttab_ptr + bucket;

  if(b->first == NOT_AN_INDEX) {
    add_ptr = &b->first;
  } else {
    b = ptr->inttab_ptr + b->first;
    while(b->next != NOT_AN_INDEX)
      b = ptr->inttab_ptr + b->next;
    add_ptr = &b->next;	
  }

  *add_ptr = ptr->counter++;
  return(0);
}

