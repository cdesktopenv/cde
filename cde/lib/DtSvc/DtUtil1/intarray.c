/* $XConsortium: intarray.c /main/4 1996/05/09 04:23:20 drk $ */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <malloc.h>
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

