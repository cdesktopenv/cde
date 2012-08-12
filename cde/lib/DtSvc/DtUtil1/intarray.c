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
/* $XConsortium: intarray.c /main/4 1996/05/09 04:23:20 drk $ */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include "DtHash.h"
#include "DtShmDb.h"

typedef struct intent {
  struct intent * next;
  int	size;
  int   data[1];
} intent_t;

typedef struct intarray {
  intent_t * first;
  intent_t ** next;
  int    num_ints;
} intarray_t;

/* ARGSUSED */
void * 
_DtShmProtoInitIntLst(int estimated_entries)
{
  intarray_t * ptr = (intarray_t *) malloc(sizeof(*ptr));
  ptr->first = (intent_t*)malloc(sizeof(intent_t));
  ptr->next = &ptr->first->next;
  ptr->first->size = 1;
  ptr->first->data[0] = 0;
  ptr->num_ints = 2;
  return((void*)ptr);
}

int * 
_DtShmProtoAddIntLst(DtShmProtoIntList handle, int size, int * index)
{
  intarray_t * ptr = (intarray_t *) handle;
  intent_t * e = (intent_t *) malloc(sizeof(intent_t) + sizeof(int) * (size-1));
  e->size = size;
  e->next = NULL;
  *index = ptr->num_ints + 1;
  ptr->num_ints += size + 1;
  *(ptr->next) = e;
  ptr->next = &e->next;
  return(e->data);
}

int 
_DtShmProtoSizeIntLst(DtShmProtoIntList handle)
{
  intarray_t * ptr = (intarray_t *) handle;
  return(sizeof(int) * ptr->num_ints);
}  


DtShmIntList
_DtShmProtoCopyIntLst(DtShmProtoIntList handle, void * destination)
{
  intarray_t * ptr = (intarray_t *) handle;
  intent_t * a = ptr->first;
  int * d = (int*) destination;

  while(a) {
    int size = a->size;
    *d++ = size;
    memcpy(d, a->data, sizeof(int) * size);
    d+=size;
    a = a->next;
  }
  return((DtShmIntList) destination);
}

int 
_DtShmProtoDestroyIntLst(DtShmProtoIntList handle)
{
  intarray_t * ptr = (intarray_t *) handle;
  intent_t * a= ptr->first;
  intent_t * b;

  while(a) {
    b = a->next;
    free(a);
    a = b;
  }
  free(ptr);
  return(0);  
}

