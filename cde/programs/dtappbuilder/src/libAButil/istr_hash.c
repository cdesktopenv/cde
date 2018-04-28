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

/*
 *	$XConsortium: istr_hash.c /main/5 1996/10/02 17:09:13 drk $
 *
 * %W% %G%
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libAButil/istr_hash.h>

static int hash_count = 0;      /* total number that has been inserted */

unsigned static hashing(ISTRING val);

/*****************************************************************************/
/*                        EXTERNAL FUNCTIONS                                 */
/*****************************************************************************/

HASH * hash_create()
{
    HASH *table;
    int i,j;

    table = (HASH *)malloc(HASHSIZE*sizeof(HASH)); 
    if(table == NULL)
    { 
        fprintf(stderr,"Error in allocating memory\n");
        return NULL;
    }
    for(i=0; i< HASHSIZE; i++)
    {
        table[i].next = NULL;
        for (j=0; j < BUCKETSIZE; j++)
            table[i].value[j].value = -1;
    }
    return table;
}

int hash_destroy(HASH *table)
{
   free(table);
   table = NULL;
   return 1;
}

int  
insert(HASH *hashtable, ISTRING istring, void *str)
{
    int hashval;
    void *str_exists;
    HASH *new_bucket;
    int i,j;
    HASH *entry;
 
    if (istring == NULL)
    {
        fprintf(stderr,
            "ISTR: can't create empty string\n");
        return NULL;
    }
    hashval = hashing(istring);
    if ((str_exists = hash_lookup(hashtable, istring )) == NULL)
               /* new entry */  
    {     
        entry = &hashtable[hashval];
        for (i=0; i< BUCKETSIZE; i++)
        { 
            if (entry->value[i].value == -1)
            {
                    entry->value[i].value = istring;
                    entry->value[i].entry = str;
                    hash_count++;
                    return 1;
            } 
        }
        while(entry->next != NULL)   /* check next bucket */
        {
            entry = entry->next;
            for (i=0; i< BUCKETSIZE; i++)
            { 
                if (entry->value[i].value == -1)
                {
                    entry->value[i].value = istring;
                    entry->value[i].entry = str;
                    hash_count++;
                    return 1;
                } 
            }
        }
/* if get this far, then no space is available in existing bucket, 
   add new bucket */
        new_bucket = (HASH *)malloc (sizeof(HASH));
        if (new_bucket == NULL)
        {
            fprintf(stderr,"Error in allocating memory\n");
            return NULL ;
        }  
        for(i=1;i<BUCKETSIZE; i++)
            new_bucket->value[i].value = -1;
        new_bucket->next = NULL;
        new_bucket->value[0].value = istring;
        new_bucket->value[0].entry = str;
        hash_count++;
        entry->next = new_bucket;
        return 1;
    }
    else            /* duplicate entry */
    {
        return -1; 
    }
}

int 
delete(HASH *hashtable, ISTRING istring)
{
    int val;                 /* hashing value */
    int i;
    HASH *entry;

    if(istring == 0)  
    {
        fprintf(stderr,"the reference string %p doesn't exist\n",
            istring);
        return 0;
    }
    val = hashing(istring);
    entry = &hashtable[val];
    for (i =0; i< BUCKETSIZE; i++)
    {
        if (entry->value[i].value == istring)
        {
            entry->value[i].value = -1;
            entry->value[i].entry = NULL;
            return 1;                     
        } 
    } 
    while(entry->next != NULL)
    {
        entry = entry->next; 
        for (i =0; i< BUCKETSIZE; i++)
        {
            if (entry->value[i].value == istring)
            {
                entry->value[i].value = -1;
                entry->value[i].entry = NULL;
                return 1;                     
            } 
        } 
    }
    return -1;
}

/******************************************************************************/
/*                       PRIVATE FUNCTIONS                                    */
/******************************************************************************/

/* hash function */
unsigned 
static hashing(ISTRING val)
{
    val %= HASHSIZE;
    return val;
}

static
void  
*hash_lookup(HASH *hashtable, ISTRING istr)
{ /* search hash table for existing strings */
    HASH entry;
    int i;    
    unsigned val;

    val = hashing(istr);
    entry = hashtable[val];
    for (i =0; i< BUCKETSIZE; i++)
    {
        if (entry.value[i].value != -1)
        {
            if (entry.value[i].value == istr) 
            {
                return entry.value[i].entry;
            } 
        } 
    } 
    while(entry.next != NULL)
    {
        entry = *entry.next; 
        for (i =0; i< BUCKETSIZE; i++)
        {
            if (entry.value[i].value != -1)
            {
                if (entry.value[i].value == istr) 
                    return entry.value[i].entry;
            }
        } 
    }
    return NULL;
}

main()
{
int a,b,c,d;
int *x,*y,*z;
int i,j;
int *w;
HASH entry;
HASH *table, *tb;
printf("\n\n\n");
table = hash_create();
x = (int *)malloc(sizeof(int));
y = (int *)malloc(sizeof(int));
z = (int *)malloc(sizeof(int));
*x = 10;
*y = 20; 
*z = 30;
printf("%d x value \n",*x);
a = insert(table,5,x);
printf("%d  return value\n",a);
a = insert(table,5,x);
printf("%d  return value\n",a);
for (i=0; i<HASHSIZE; i++)
{
    entry = table[i];
    for (j=0; j<BUCKETSIZE; j++)
        if (entry.value[j].value != -1)
            printf("%p  value %p ptr \n",entry.value[j].value,
                   entry.value[j].entry);
    while(entry.next != NULL)
    {
      entry = *entry.next;
      for (j=0; j<BUCKETSIZE; j++)
        if (entry.value[j].value != -1)
            printf("%p  value %p ptr \n",entry.value[j].value,
                   entry.value[j].entry);
     }
}
printf("\n\n\n");
b= insert(table,2,y);
printf("%d  return value\n",b);
for (i=0; i<HASHSIZE; i++)
{
    entry = table[i];
    for (j=0; j<BUCKETSIZE; j++)
        if (entry.value[j].value != -1)
            printf("%p  value %p ptr \n",entry.value[j].value,
                   entry.value[j].entry);
    while(entry.next != NULL)
    {
      entry = *entry.next;
      for (j=0; j<BUCKETSIZE; j++)
        if (entry.value[j].value != -1)
            printf("%p  value %p ptr \n",entry.value[j].value,
                   entry.value[j].entry);
     }
}

   printf("\n\n\n");
d = delete(table,5);
printf("%d  return value\n",d);
d = delete(table,1);
printf("%d  return value\n",d);
for (i=0; i<HASHSIZE; i++)
{
    entry = table[i];
    for (j=0; j<BUCKETSIZE; j++)
        if (entry.value[j].value != -1)
            printf("%p  value %p ptr \n",entry.value[j].value,
                   entry.value[j].entry);
    while(entry.next != NULL)
    {
      entry = *entry.next;
      for (j=0; j<BUCKETSIZE; j++)
        if (entry.value[j].value != -1)
            printf("%p  value %p ptr \n",entry.value[j].value,
                   entry.value[j].entry);
     }
}
   printf("\n\n\n");

b= insert(table,7,z);
printf("%d  return value\n",b);
b= insert(table,3,x);
printf("%d  return value\n",b);
b= insert(table,9,y);
printf("%d  return value\n",b);
b= insert(table,11,y);
printf("%d  return value\n",b);
for (i=0; i<HASHSIZE; i++)
{
    entry = table[i];
    for (j=0; j<BUCKETSIZE; j++)
        if (entry.value[j].value != -1)
            printf("%p  value %p ptr \n",entry.value[j].value,
                   entry.value[j].entry);
    while(entry.next != NULL)
    {
      entry = *entry.next;
      for (j=0; j<BUCKETSIZE; j++)
        if (entry.value[j].value != -1)
            printf("%p  value %p ptr \n",entry.value[j].value,
                   entry.value[j].entry);
     }
}
printf("\n\n\n");
w= hash_lookup(table,3);
printf("%p  return value\n",w);
printf("\n\n\n");

d = delete(table,3);
printf("%d  return value\n",d);
for (i=0; i<HASHSIZE; i++)
{
    entry = table[i];
    for (j=0; j<BUCKETSIZE; j++)
        if (entry.value[j].value != -1)
            printf("%p  value %p ptr \n",entry.value[j].value,
                   entry.value[j].entry);
    while(entry.next != NULL)
    {
      entry = *entry.next;
      for (j=0; j<BUCKETSIZE; j++)
        if (entry.value[j].value != -1)
            printf("%p  value %p ptr \n",entry.value[j].value,
                   entry.value[j].entry);
     }
}
printf("\n\n\n");
b= insert(table,4,y);
printf("%d  return value\n",b);

for (i=0; i<HASHSIZE; i++)
{
    entry = table[i];
    for (j=0; j<BUCKETSIZE; j++)
        if (entry.value[j].value != -1)
            printf("%p  value %p ptr \n",entry.value[j].value,
                   entry.value[j].entry);
    while(entry.next != NULL)
    {
      entry = *entry.next;
      for (j=0; j<BUCKETSIZE; j++)
        if (entry.value[j].value != -1)
            printf("%p  value %p ptr \n",entry.value[j].value,
                   entry.value[j].entry);
     }
}
tb = hash_create();
b= insert(tb,4,z);
printf("%d  return value\n",b);
b= insert(tb,9,x);
printf("%d  return value\n",b);
b= insert(tb,2,y);
printf("%d  return value\n",b);
b= insert(tb,22,y);
printf("%d  return value\n",b);
for (i=0; i<HASHSIZE; i++)
{
    entry = tb[i];
    for (j=0; j<BUCKETSIZE; j++)
        if (entry.value[j].value != -1)
            printf("%p  value %p ptr \n",entry.value[j].value,
                   entry.value[j].entry);
    while(entry.next != NULL)
    {
      entry = *entry.next;
      for (j=0; j<BUCKETSIZE; j++)
        if (entry.value[j].value != -1)
            printf("%p  value %p ptr \n",entry.value[j].value,
                   entry.value[j].entry);
     }
}
printf("\n\n\n");
for (i=0; i<HASHSIZE; i++)
{
    entry = table[i];
    for (j=0; j<BUCKETSIZE; j++)
        if (entry.value[j].value != -1)
            printf("%p  value %p ptr \n",entry.value[j].value,
                   entry.value[j].entry);
    while(entry.next != NULL)
    {
      entry = *entry.next;
      for (j=0; j<BUCKETSIZE; j++)
        if (entry.value[j].value != -1)
            printf("%p  value %p ptr \n",entry.value[j].value,
                   entry.value[j].entry);
     }
}
hash_destroy(table);
}
