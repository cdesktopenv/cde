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
/* $XConsortium: strtab.c /main/4 1996/05/09 04:23:54 drk $ */
/*
  routines to implement a string -> unique ptr table & access functions
  suitable for use with shared memory
  */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "DtHash.h"
#include "DtShmDb.h"

typedef struct strtab {
  unsigned int st_size;		/* size in bytes						*/
  unsigned int st_stroffset;	/* offset (bytes) from this structure to contained strings 	*/
  unsigned int st_taboffset;	/* offset (bytes) from this structure to table offset           */
  unsigned int st_count;	/* number of elements in this string table			*/
} strtab_t;

typedef struct strtab_entry {
  unsigned int key;		/* offset into contained strings				*/
  unsigned short first;		/* offset in table to first string to hash to this spot		*/
  unsigned short next;		/* offset in table to next item to hash to this spot		*/
} strtab_entry_t;

typedef struct strlist {
  DtHashTbl 		sl_hash;	/* hash table to hold strings during definition		*/
  DtHashTbl		sl_bosons;	/* reverse hash table for Nolan's lookup function	*/		
  unsigned int    	sl_charcount;	/* count of current bytes of strings			*/
} strlist_t;

struct strtab_build {
  unsigned int	   	index;			/* iteration 			*/
  unsigned char *  	strstart;		/* start of strings		*/
  strtab_entry_t * 	tabstart;		/* start of table		*/	
  strtab_t * 		strtab;			/* pointer to head        	*/
};

#define NOT_AN_INDEX ((unsigned short) 0xffff)

static void inc_it  (int * a, int * b, unsigned char * key);
static void build_it(int a, struct strtab_build * ptr, unsigned char * key);
#if !defined(linux)
extern char * strdup(const char *);
#endif
typedef	int (*des_func)(void *);

/*
  client routine
  
  returns offset in string table where string is found,
  else -1 if string is not found.
  */

const char *    _DtShmBosonToString(DtShmStrtab strtab, int boson)
{
  const char * s = (const char *) strtab + ((strtab_t *) strtab)->st_stroffset;
  return(s+boson);
}


DtShmBoson _DtShmStringToBoson(DtShmStrtab strtab, const char * string)
{
  register unsigned int i,j;
  register unsigned const char * s;
  register strtab_entry_t * ptr = (strtab_entry_t *) ((unsigned char *) strtab  + 
						      ((strtab_t *) strtab) -> st_taboffset);
 
  /*
    hash the input string
    */
  i = 1;
  j = 0;
  s = (unsigned const char *) string;

  while(*s!=0)
    j += (*s++ << i++); 
  
  i = j % ((strtab_t *) strtab)->st_count;
  
  /*
    check bucket first pointer....
    */

  if((i=(ptr+i)->first) == NOT_AN_INDEX)	
    return(-1);
  
  /*
    loop through other buckets on hash table, looking for
    our string.  Note that we reused s.
    */

  s = (unsigned const char *) strtab + ((strtab_t *) strtab)->st_stroffset;
  
  while(strcmp( (char *) (s +  ptr[i].key), string))
    if((i= ptr[i].next) == NOT_AN_INDEX)
      return(-1);

  return(ptr[i].key);
}



DtShmProtoStrtab
_DtShmProtoInitStrtab(int sizeguess)
{
  strlist_t * ptr = (strlist_t *) malloc(sizeof(*ptr));
  if(!ptr)
    return(NULL);

  if(!(ptr->sl_hash =  _DtUtilMakeHash(sizeguess))) {
    free(ptr);
    return(NULL);
  }

  if(!(ptr->sl_bosons =  _DtUtilMakeIHash(sizeguess))) {
    (void)_DtUtilDestroyHash(ptr->sl_hash, NULL, NULL);
    free(ptr);
    return(NULL);
  }

  ptr->sl_charcount = 1;
  return((DtShmProtoStrtab) ptr);
}

int 
_DtShmProtoDestroyStrtab(DtShmProtoStrtab strlist)
{
  strlist_t * ptr = (strlist_t *) strlist;
  
  _DtUtilDestroyHash(ptr->sl_hash, NULL, NULL);
  _DtUtilDestroyHash(ptr->sl_bosons, (des_func)free, NULL);
  free(ptr);
  return(0);
}

DtShmBoson
_DtShmProtoAddStrtab(DtShmProtoStrtab strlist, const char * string, int * isnew)
{
  strlist_t * ptr = (strlist_t *) strlist;

  int * bucket = (int *) _DtUtilGetHash(ptr->sl_hash, (const unsigned char *)string);
  
  int ret = *bucket;

  if(*bucket == 0) /* new */ {
    unsigned char ** sptr;
    *isnew = 1;
    *bucket = ret = ptr->sl_charcount;
    sptr = (unsigned char**)_DtUtilGetHash(ptr->sl_bosons, (const unsigned char *)ret);
    *sptr = (unsigned char*)strdup(string);
    ptr->sl_charcount += strlen(string) + 1;
  } 

  else {
    *isnew = 0;
  }
    

  return((DtShmBoson)ret);
}

const char *		
_DtShmProtoLookUpStrtab (DtShmProtoStrtab prototab, DtShmBoson boson)
{
  strlist_t * ptr = (strlist_t *) prototab;
  unsigned char ** sptr;
  
  sptr = (unsigned char **) _DtUtilFindHash(ptr->sl_bosons, (const unsigned char *) boson);

  return(sptr?((const char *)*sptr):NULL);
}


int 
_DtShmProtoSizeStrtab(DtShmProtoStrtab strlist)
{
  int foo[2];
  int size;
  strlist_t * ptr = (strlist_t * ) strlist;

  foo[1] = foo[0] = 0;

  _DtUtilOperateHash(ptr->sl_hash, inc_it, &foo);

  size = sizeof(strtab_t) + (foo[0]) * sizeof(strtab_entry_t) + foo[0] + foo[1] + 3 ;
         /* header */      /* table */  /* for string + terminator */	     /* padding */

  /*
    fix size so that it is always a multiple of 4 for ease of programming
    */
  size = (~3) & (size+3);

  return(size);
  
}

DtShmStrtab 
_DtShmProtoCopyStrtab(DtShmProtoStrtab in, void * destination)
{
  strlist_t * 	strlist = (strlist_t *) in;
  strtab_t * 	ptr = (strtab_t *) destination;

  int foo[2];
  int size;
  struct strtab_build building;

  
  foo[0] = foo[1] = 0;

  _DtUtilOperateHash(strlist->sl_hash, inc_it, &foo);

  size = sizeof(strtab_t) + (foo[0]) * sizeof(strtab_entry_t) + (foo[0] + foo[1] + 3) & ~3 ;
         /* header */      /* table */  /* for string + terminator */

  memset((char *) ptr, 255, size);
  

  ptr-> st_size= size;
  ptr-> st_stroffset = sizeof(*ptr);
  ptr-> st_taboffset = sizeof(*ptr) + (foo[0] + foo[1] + 3) & ~3;
  ptr-> st_count = foo[0];			/* patch alignment */

  building.index = 0;
  building.strstart = (unsigned char *) ptr  + ptr->st_stroffset;
  building.tabstart = (strtab_entry_t *) ((unsigned char *) ptr  + ptr->st_taboffset);
  building.strtab = ptr;

  _DtUtilOperateHash(strlist->sl_hash, build_it, & building);
  return((DtShmStrtab) destination);
}

static void build_it(int a, struct strtab_build * ptr, unsigned char * key)
{
  register unsigned int i,j;
  register unsigned char * s;
  register strtab_entry_t * e;
  register unsigned short * add_ptr;

  strcpy((char *) ptr->strstart + a, (const char *)key);

  i = 1;
  j = 0;
  s = key;

  while(*s!=0)
    j += (*s++ << i++); 
  
  i = j % ((strtab_t *) ptr->strtab)->st_count;
  
  e = ptr->tabstart + ptr->index;

  e -> key = a; /* save key value in our block */
  e -> next = NOT_AN_INDEX;

  e = ptr->tabstart + i;

  if(e->first == NOT_AN_INDEX)
    add_ptr = & e->first;
  else {
    e = ptr->tabstart + e->first;
    while( e->next != NOT_AN_INDEX)
      e = ptr->tabstart + e->next;
    add_ptr = & e->next;
  }

  *add_ptr = ptr->index++;
  
}

/* ARGSUSED */
static void inc_it(int * a, int * b, unsigned char * key)
{
  b[0]++;
  b[1] += strlen((const char *)key) + 1;
}

