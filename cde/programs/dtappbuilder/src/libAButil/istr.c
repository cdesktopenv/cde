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
 *	$XConsortium: istr.c /main/4 1995/11/06 18:50:43 rswiston $
 *
 * @(#)istr.c	1.25 14 Feb 1994	cde_app_builder/src/libAButil
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


/*
 * istr.c
 * 
 * Implements ISTRING data type.
 *
 * the istr variable is private, but is made to be used within
 * the debugger.  To get the value of an ISTRING, use
 * print/display debug_istr[iString].
 *
 * Whenever setting the private variable int_array from within this 
 * ISTRING * implementation module, use int_array_set macro, as it
 * will keep the istr variable up-to-date for debugging clients.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ab_private/istr.h>
#include "utilP.h"

#define istrP_create_alloced_impl	istrP_create_alloced_impl9726039350PrivF
#define istrP_destroy_impl		istrP_destroy_impl518283652PrivF
#define istrP_get_string_verify		istrP_get_string_verify4521632085PrivF
#define int_array			istrP_int_array1809065681PrivD
#define num_count			istrP_num_count7608925912PrivD
#define STRN ISTR_PRIVT_STRN

/* #define NUMBUCKETS   211 */
#define NUMBUCKETS   1024
#define NUMBUCKETS_MASK	((unsigned)(NUMBUCKETS-1))
#define BUCKETSIZE 10
#define ARRAYSIZE  100 /* size of and freelist */

typedef struct BUCKET
{
    struct BUCKET	*next;
    int   		values[BUCKETSIZE];
} BucketRec, *Bucket;

/*
 * Public data
 */
STRING	Istr_null_string= "(nil)";
STRN 	*int_array= NULL;	/* unique number array  */
long	num_count  = 1;         /* unique number counter */

static BucketRec	hashtable[NUMBUCKETS];

#ifdef DEBUG
STRN *debug_istr= NULL;		/* debugging shortcut for clients (see */
				/* comment at top of this file) */
#endif /* DEBUG */

static long *freelist;           /* freelist for unused numbers */

static long freecount  = 0;      /* freelist count */
static long hash_count = 0;      /* total number that has been inserted */

static unsigned	hashing(
		    char *p
		);

static int	insert_array(
		    int flag, 
		    int val, 
		    char *string
		);

static int	hash_and_lookupstr(
		    char	*string,
		    int		*hash_val_out_ptr
		);

static int	hashtable_init(void);

static int	istrP_errmsg_noexist(int istr_value);

#define check_init()		((hash_count == 0) && (hashtable_init() >= 0))
#ifdef DEBUG
#define int_array_set(ptr)	(int_array = (ptr), debug_istr= int_array)
#else
#define int_array_set(ptr)	(int_array = (ptr))
#endif
#define istrP_int_to_client(intVal)	((ISTRING)(intVal))
#define istrP_client_to_int(istrVal)	((long)(istrVal))
#define return_istr(x)			return istrP_int_to_client(x)

/*****************************************************************************/
/*                        EXTERNAL FUNCTIONS                                 */
/*****************************************************************************/

/* 
 * Allocate a new int for a given string and return the value.
 * If the string already exists return the value for the existing int. 
 */
ISTRING 
istr_create (
    STRING string
)
{
    int hashval;
    long str_exists;
    Bucket	new_bucket;
    int i;
    Bucket	entry;
    check_init();

    if (string == NULL)
    {
        return NULL;
    }
    /* hashval = hashing(string); */
    if ((str_exists = hash_and_lookupstr(string,&hashval )) == -1)     /* new entry */  
    {
        entry = &hashtable[hashval];
        for (i=0; i< BUCKETSIZE; i++)
        { 
            if (entry->values[i] == -1)
            {
                
                if(freecount == 0)
                {
                    entry->values[i] = num_count;
                    if (insert_array(1,num_count, string) == -1)
                    {
                        fprintf(stderr,
			    catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 1,
                            "ISTR: error in allocating to int array\n") );
                        return NULL;
                    }
                    num_count++;
                    hash_count++;
                    return istrP_int_to_client(num_count-1);
                }
                else
                {   /* take int value from freelist */
                    if(int_array[freelist[freecount-1]].refcount != 0)
                    {
                        fprintf(stderr,
			    catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 2,
                            "ISTR: error in allocating space for string\n") );
                        return NULL;
                    }
                    entry->values[i] = freelist[freecount-1];
                    if (insert_array(1,freelist[freecount-1], string) == -1)
                    {
                        fprintf(stderr,
			    catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 1,
                            "ISTR: error in allocating to int array\n") );
                        return NULL; 
                    }
                    freecount--;
                    return istrP_int_to_client(freelist[freecount]);
                } 
            } 
        }
        while(entry->next != NULL)   /* check next bucket */
        {
            entry = entry->next;
            for (i=0; i< BUCKETSIZE; i++)
            { 
                if (entry->values[i] == -1)
                {
                    if (freecount == 0)
                    {
                        entry->values[i] = num_count;
                        if (insert_array(1, num_count, string) == -1)
                        {
                            fprintf(stderr,
			        catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 1,
                                "ISTR: error in allocating to int array\n") );
                            return NULL;
                        }
                        num_count++;
                        hash_count++;
                        return istrP_int_to_client(num_count-1);
                    }
                    else
                    {
                        if(int_array[freelist[freecount-1]].refcount != 0)
                        {
                            fprintf(stderr,
			        catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 2,
                                "ISTR: error in allocating space for string\n") );
                            return NULL;
                        }
                        entry->values[i] = freelist[freecount-1];
                        if (insert_array(1, 
                            freelist[freecount-1], string) == -1)
                        {
                            fprintf(stderr,
			        catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 1,
                                "ISTR: error in allocating to int array\n") );
                            return NULL;
                        }
                        freecount--;
                        return istrP_int_to_client(freelist[freecount]);
                    } 
                } 
            }
        }
/* if get this far, then no space is available in existing bucket, 
   add new bucket */
        new_bucket = (Bucket)malloc (sizeof(BucketRec));
        if (new_bucket == NULL)
        {
            fprintf(stderr,
		catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 3,
		"ISTR: error in allocating memory\n") );
            return NULL ;
        }  
        for(i=1;i<BUCKETSIZE; i++)
            new_bucket->values[i] = -1;
        new_bucket->next = NULL;
        if (freecount == 0)
        {
            new_bucket->values[0] = num_count;
            entry->next = new_bucket;
            if (insert_array(1, num_count, string) == -1)
            {
                fprintf(stderr,
		    catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 1,
		    "ISTR: error in allocating to int array\n") );
                return NULL;
            }
            num_count++;
            hash_count++;
            return istrP_int_to_client(num_count-1);
        }
        else
        {
            if(int_array[freelist[freecount-1]].refcount != 0)
            {
                fprintf(stderr,
		    catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 2,
		    "ISTR: error in allocating space for string\n") );
                return NULL;
            }
            new_bucket->values[0] = freelist[freecount-1];
            entry->next = new_bucket;
            if (insert_array(1, freelist[freecount-1], string) == -1)
            {
                fprintf(stderr,
		    catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 1,
		    "ISTR: error in allocating to int array\n") );
                return NULL;
            }
            freecount--;
            return_istr(freelist[freecount]);

        }
    }
    else            /* duplicate entry */
    {
        int_array[str_exists].refcount++;
        return istrP_int_to_client(str_exists);
    }
}

/* 
 * Assign a new int for a given allocated string and return the value.
 * If the string already exists return the value for the existing int. 
 */
ISTRING 
istrP_create_alloced_impl(
    STRING *string
)
{
    int hashval;
    long str_exists;
    Bucket	new_bucket;
    int i;
    Bucket	entry;
    check_init();

    if (*string == NULL)
    {
        return NULL;
    }

    /* hashval = hashing(*string); */
    if ((str_exists = hash_and_lookupstr(*string,&hashval)) == -1)     /* new entry */  
    {     
        entry = &hashtable[hashval];
        for (i=0; i< BUCKETSIZE; i++)
        { 
            if (entry->values[i] == -1)
            {
                
                if(freecount == 0)
                {
                    entry->values[i] = num_count;
                    if (insert_array(3,num_count, *string) == -1)
                    {
                        fprintf(stderr,
			    catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 1,
                            "ISTR: error in allocating to int array\n") );
                        return NULL;
                    }
                    num_count++;
                    hash_count++;
                    return_istr(num_count-1);
                }
                else
                {   /* take int value from freelist */
                    if(int_array[freelist[freecount-1]].refcount != 0)
                    {
                        fprintf(stderr,
			    catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 2,
                            "ISTR: error in allocating space for string\n") );
                        return NULL;
                    }
                    entry->values[i] = freelist[freecount-1];
                    if (insert_array(3,freelist[freecount-1], *string) == -1)
                    {
                        fprintf(stderr,
			    catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 1,
                            "ISTR: error in allocating to int array\n") );
                        return NULL;
                    }
                    freecount--;
                    return_istr(freelist[freecount]);
                } 
            } 
        }
        while(entry->next != NULL)  /* go to next bucket */
        {
            entry = entry->next;
            for (i=0; i< BUCKETSIZE; i++)
            { 
                if (entry->values[i] == -1)
                {
                    if (freecount == 0)
                    {
                        entry->values[i] = num_count;
                        if (insert_array(3, num_count, *string) == -1)
                        {
                            fprintf(stderr,
			        catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 1,
                                "ISTR: error in allocating to int array\n") );
                            return NULL;
                        }
                        num_count++;
                        hash_count++;
                        return_istr(num_count-1);
                    }
                    else
                    {
                        if(int_array[freelist[freecount-1]].refcount != 0)
                        {
                            fprintf(stderr,
			        catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 2,
                                "ISTR: error in allocating space for string\n") );
                            return NULL;
                        }
                        entry->values[i] = freelist[freecount-1];
                        if (insert_array(3, 
                            freelist[freecount-1], *string) == -1)
                        {
                            fprintf(stderr,
			        catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 1,
                                "ISTR: error in allocating to int array\n") );
                            return NULL;
                        }
                        freecount--;
                        return_istr(freelist[freecount]);
                    } 
                } 
            }
        }
/* if get this far, then no space is available in existing bucket, 
   add new bucket */
        new_bucket = (Bucket)malloc (sizeof(BucketRec));
        if (new_bucket == NULL)
        {
            fprintf(stderr,
		catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 3,
	        "ISTR: error in allocating memory\n") );
            return NULL ;
        }  
        for(i=1;i<BUCKETSIZE; i++)
            new_bucket->values[i] = -1;
        new_bucket->next = NULL;
        if (freecount == 0)
        {
            new_bucket->values[0] = num_count;
            entry->next = new_bucket;
            if (insert_array(3, num_count, *string) == -1)
            {
                fprintf(stderr,
		    catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 1,
		    "ISTR: error in allocating to int array\n") );
                return NULL;
            }
            num_count++;
            hash_count++;
            return_istr(num_count-1);
        }
        else
        {
            if(int_array[freelist[freecount-1]].refcount != 0)
            {
                fprintf(stderr,
		    catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 2,
		    "ISTR: error in allocating space for string\n") );
                return NULL;
            }
            new_bucket->values[0] = freelist[freecount-1];
            entry->next = new_bucket;
            if (insert_array(3, freelist[freecount-1], *string) == -1)
            {
                fprintf(stderr,
		    catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 1,
		    "ISTR: error in allocating to int array\n") );
                return NULL;
            }
            freecount--;
            return_istr(freelist[freecount]);

        }
    }
    else            /* duplicate entry */
    {
        int_array[str_exists].refcount++;
        if ((*string != int_array[str_exists].str) &&
           (*&string != &(int_array[str_exists].str)))
        {
            free(*string);
            *string = NULL;
        }
        return_istr(str_exists);
    }
}

/* 
 * Assign a new int for a given string and return the value.
 * If the string already exists deallocate the existing string
 * and assign the new string to the int return the value for 
 * the existing int. 
*/
ISTRING 
istr_create_const(
    STRING string
)
{
    int hashval;
    long str_exists;
    Bucket	new_bucket;
    int i;
    Bucket	entry;
    check_init();
 
    if (string == NULL)
    {
        return NULL;
    }

    /* hashval = hashing(string); */
    if ((str_exists = hash_and_lookupstr(string,&hashval)) == -1)     /* new entry */  
    {     
        entry = &hashtable[hashval];
        for (i=0; i< BUCKETSIZE; i++)
        { 
            if (entry->values[i] == -1)
            {
                
                if(freecount == 0)
                {         /* no numbers on freelist */
                    entry->values[i] = num_count;
                    if (insert_array(2, num_count, string) == -1)
                    {
                        fprintf(stderr,
			    catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 1,
                            "ISTR: error in allocating to int array\n") );
                        return NULL;
                    }
                    num_count++;
                    hash_count++;
                    return_istr(num_count-1);
                }
                else
                {   /* take int value from freelist */
                    if(int_array[freelist[freecount-1]].refcount != 0)
                    {
                        fprintf(stderr,
			    catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 2,
                            "ISTR: error in allocating space for string\n") );
                        return NULL;
                    }
                    entry->values[i] = freelist[freecount-1];
                    if (insert_array(2, freelist[freecount-1], string) == -1)
                    {
                        fprintf(stderr,
			    catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 1,
                            "ISTR: error in allocating to int array\n") );
                        return NULL;
                    }
                    freecount--;
                    return_istr(freelist[freecount]);
                } 
            } 
        }
        while(entry->next != NULL)
        {                            /* check next bucket */
            entry = entry->next;
            for (i=0; i< BUCKETSIZE; i++)
            { 
                if (entry->values[i] == -1)
                {
                    if (freecount == 0)
                    {
                        entry->values[i] = num_count;
                        if (insert_array(2, num_count, string) == -1)
                        {
                            fprintf(stderr,
			        catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 1,
                                "ISTR: error in allocating to int array\n") );
                            return NULL;
                        }
                        num_count++;
                        hash_count++;
                        return_istr(num_count-1);
                    }
                    else
                    {
                        if(int_array[freelist[freecount-1]].refcount != 0)
                        {
                            fprintf(stderr,
			        catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 2,
                                "ISTR: error in allocating space for string\n") );
                            return NULL;
                        }
                        entry->values[i] = freelist[freecount-1];
                        if (insert_array(2, 
                            freelist[freecount-1], string) == -1)
                        {
                            fprintf(stderr,
			        catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 1,
                                "ISTR: error in allocating to int array\n") );
                            return NULL;
                        }
                        freecount--;
                        return_istr(freelist[freecount]);
                    } 
                } 
            }
        }
/* if get this far, then no space is available in existing bucket, 
   add new bucket */
        new_bucket = (Bucket)malloc (sizeof(BucketRec));
        if (new_bucket == NULL)
        {
            fprintf(stderr,
		catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 3,
		"ISTR: error in allocating memory\n") );
            return NULL ;
        }  
	for(i=1;i<BUCKETSIZE; i++)
	{
            new_bucket->values[i] = -1;
	}
        new_bucket->next = NULL;
        if (freecount == 0)
        {
            new_bucket->values[0] = num_count;
            entry->next = new_bucket;
            if (insert_array(2, num_count, string) == -1)
            {
                fprintf(stderr,
		    catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 1,
		    "ISTR: error in allocating to int array\n") );
                return NULL;
            }
            num_count++;
            hash_count++;
            return_istr(num_count-1);
        }
        else
        {
            if(int_array[freelist[freecount-1]].refcount != 0)
            {
                fprintf(stderr,
		    catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 2,
		    "ISTR: error in allocating space for string\n") );
                return NULL;
            }
            new_bucket->values[0] = freelist[freecount-1];
            entry->next = new_bucket;
            if (insert_array(2, freelist[freecount-1], string) == -1)
            {
                fprintf(stderr,
		    catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 1,
		    "ISTR: error in allocating to int array\n") );
                return NULL;
            }
            freecount--;
            return_istr(freelist[freecount]);

        }
    }
    else            /* duplicate entry */
    {
        if(int_array[str_exists].read_const == 0)
        {      /* change read only string to const string */
            if ((string != int_array[str_exists].str) &&
               (&string != &(int_array[str_exists].str)))
            {
                free(int_array[str_exists].str);
                int_array[str_exists].str = string;
            }
            int_array[str_exists].read_const = 1;
        } 
        int_array[str_exists].refcount++;
        return_istr(str_exists);
    }
}

/* 
 * Lookup string if find it duplicate the entry and return int value,
 * else return NULL.
 */
ISTRING
istr_dup_existing(
    STRING string
)
{
    int hashval;
    long str_exists;

    if (string == NULL)
    {
        return NULL;
    }
    /* hashval = hashing(string); */
    if ((str_exists = hash_and_lookupstr(string,&hashval)) == -1)    
    {                                          /* string doesn't exist */
        return NULL;
    }
    else 
    {
        int_array[str_exists].refcount++;
        return_istr(str_exists);
    }
}

/* 
 * Deallocate for the string if refcount = 0, else decrement refcount.
 * If read_const flag is set then don't deallocate.
 * Return -1 if error, else return 1 
 */
int 
istrP_destroy_impl(
    ISTRING *istringClientPtr
)
{
    long istring = (long)(*istringClientPtr);
    int val;                 /* hashing value */
    int i;
    Bucket	entry;
    static int free_num = 0;   /* allocated space in freelist = 
                                  free_num * ARRAYSIZE */

    if(istring == 0)  
    {
        return 0;
    }
    if((istring < 0) || (istring >= num_count) || 
       ((int_array[istring].refcount == 0) && 
        (int_array[istring].read_const == 0)))
    {
	istrP_errmsg_noexist(istring);
        return 0;
    }
    if (int_array[istring].read_const == 1)  /* const entry */
    {
	/* never let refs to const entries drop below 1 */
        if (int_array[istring].refcount > 1)
	{
            int_array[istring].refcount--;
	}
        (*istringClientPtr) = NULL;
        return 0;
    }
    int_array[istring].refcount--;
    if(int_array[istring].refcount == 0) 
    {                                     /* remove from hash table */
        val = hashing(int_array[istring].str);
        entry = &hashtable[val];
        for (i =0; i< BUCKETSIZE; i++)
        {
            if (entry->values[i] == istring)
            {
                entry->values[i] = -1;
            } 
        } 
        while(entry->next != NULL)
        {
            entry = entry->next; 
            for (i =0; i< BUCKETSIZE; i++)
            {
                if (entry->values[i] == istring)
                {
                    entry->values[i] = -1;
                }
            } 
        }
        free(int_array[istring].str);
	int_array[istring].str = NULL;
/* put unused int_array location of the freelist */
        if(free_num == 0)
        {
            freelist = (long *)malloc(ARRAYSIZE * sizeof(long));
            if (freelist == NULL)
            {
                fprintf(stderr,
		    catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 3,
		    "ISTR: error in allocating memory\n") );
                return 0 ;
            }  
            free_num = 1;
        }
        if(freecount < (ARRAYSIZE * free_num))  /* put free int value on 
                                                   freelist  */
        {
            freelist[freecount] = istring;
            freecount++;
        }
        else  /* need more freelist space */
        { 
            free_num++;
            freelist = (long *)realloc(freelist,
                (ARRAYSIZE * free_num) * sizeof(long));
	    assert(freelist != NULL);
            if (freelist == NULL)
            {
                fprintf(stderr,
		    catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 3,
		    "ISTR: error in allocating memory\n") );
                return 0 ;
            }  
            freelist[freecount] = istring;
            freecount++;
        }
    }

    /*
     * Set the client's variable to NULL
     */
    istring = 0;
    (*istringClientPtr) = istrP_int_to_client(istring);
    return 1;
}


/* 
 * Increment the string refcounter, and return the string value.
 * Return -1 if error 
 */
ISTRING 
istr_dup(
    ISTRING istringClientVal
)
{
    long	istring= istrP_client_to_int(istringClientVal);

    if(istring == 0) 
    {
        return 0;
    }
    else if((istring < 0) || (istring >= num_count) || 
       ((int_array[istring].refcount == 0) && 
        (int_array[istring].read_const == 0)))
    {
	istrP_errmsg_noexist(istring);
        return NULL;
    }
    int_array[istring].refcount++;
    return_istr(istring);
}

/* 
 * Return the string name for the given int, return NULL if error 
 *
 * Verifies that the string is valid
 */
STRING 
istrP_get_string_verify(
    ISTRING istringClientVal
)
{
    long	istring = istrP_client_to_int(istringClientVal);

    if(istring == 0) 
    {
        return NULL;
    }
    if((istring < 0) || (istring >= num_count) || 
       ((int_array[istring].refcount == 0) &&
        (int_array[istring].read_const == 0)))
    {
	istrP_errmsg_noexist(istring);
        return NULL;
    }
    return int_array[istring].str;
}

/******************************************************************************/
/*                       PRIVATE FUNCTIONS                                    */
/******************************************************************************/

/* 
 * hash function 
 */
static unsigned 
hashing(
	STRING	string
)
{
    register unsigned hash_val = 0;

    for (hash_val = 0; *string; ++string)
        hash_val = hash_val * 65599 + *string;
    return hash_val % NUMBUCKETS;
}

/* 
 * search hash table for existing strings 
 *
 * Returns int value for string, or -1 if not found
 */
static int 
hash_and_lookupstr(
    char	*string,
    int		*hash_val_out_ptr
)
{
#define fast_streq(s1,s2) (((s1)[0] == (s2)[0]) && (strcmp(s1,s2) == 0))

    register unsigned hash_val = 0;
    register char	*stringPtr;
    Bucket	entry;
    int		*valuePtr;
    int		*valuePtrEnd;
    int		value;

    /*
     * Get bucket
     */
    for (hash_val = 0, stringPtr = string; *stringPtr; ++stringPtr)
        hash_val = hash_val * 65599 + *stringPtr;
    (*hash_val_out_ptr) = (hash_val &= NUMBUCKETS_MASK);

    /*
     * Find entry in bucket
     */
    for (entry = &hashtable[hash_val]; entry != NULL; entry = entry->next)
    {
	valuePtr = entry->values;
	valuePtrEnd = valuePtr + BUCKETSIZE;
        for (; valuePtr != valuePtrEnd; ++valuePtr)
        {
	    value = *valuePtr;
            if (   (value >= 0) 
		&& (fast_streq(int_array[value].str,string)) )
            {
                return value;
	    }
        } 
    } 

    return -1;
#undef fast_streq
}

/* 
 * Insert the given string into the given array location.
 * Return 1 if successful, else return NULL if error.
 * Note: int_array[0] unused 
 */
static int 
insert_array(
    int flag, 
    int val, 
    char *string
)
{
    char *str;
    static int array_num =0;

    if(hash_count == 0)
    {
        int_array_set((STRN *)calloc(ARRAYSIZE, sizeof(STRN)));
        if (int_array == NULL)
        {
            fprintf(stderr,
		catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 3,
		"ISTR: error in allocating memory\n") );
            return 0 ;
        }  
        array_num = 1;
    }
    if(val < (ARRAYSIZE * array_num)) /* insert into array */
    {
        int_array[val].refcount =1;
        if (flag == 1)
        {                   /* read only */
           str = strdup(string);
           int_array[val].read_const = 0;
           int_array[val].str = str; 
        }
        else if (flag == 2)
        {                  /* const */
           int_array[val].read_const = 1;
           int_array[val].str = string; 
        }
        else               /* flag == 3 */
        {                  /* allocated */
           int_array[val].read_const = 0;
           int_array[val].str = string; 
        }
    }
    else  /* need more array space */
    { 
        array_num++;
        int_array_set((STRN *)realloc(int_array,
            (ARRAYSIZE * array_num) * sizeof(STRN)));
	assert(int_array != NULL);
        if (int_array == NULL)
        {
            fprintf(stderr,
		catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 3,
		"ISTR: error in allocating memory\n") );
            return 0 ;
        }  
        int_array[val].refcount =1;
        if (flag == 1)
        {                   /* read only */
           str = strdup(string);
           int_array[val].read_const = 0;
           int_array[val].str = str; 
        }
        else if(flag == 2)   /* const */
        {
           int_array[val].read_const = 1;
           int_array[val].str = string; 
        }
        else                /*flag == 3*/  
        {                   /* allocated */
           int_array[val].read_const = 0;
           int_array[val].str = string; 
        }
    }
    return 1;
}


static int
hashtable_init(void)
{
    int		i = 0, j = 0;
    
    for(i=0; i< NUMBUCKETS; i++)
    {
        hashtable[i].next = NULL;
        for (j=0; j < BUCKETSIZE; j++)
	{
            hashtable[i].values[j] = -1;
	}
    }
    return 0;
}


/*
 * Verifies the string exists and is not corrupted.
 */
int
istr_verify(ISTRING istringClientVal)
{
    int		istring= istrP_client_to_int(istringClientVal);
    int		return_value = 0;

    if (istringClientVal == (ISTRING)NULL)
    {
	return 0;	/* OK */
    }
    if((istring < 0) || (istring >= num_count) || 
       ((int_array[istring].refcount == 0) && 
        (int_array[istring].read_const == 0)))
    {
	return_value = -1;
    }
    return return_value;
}

/*
 * This doesn't do anything right now, although it really should.
 */
int
istr_verify_all(void)
{
    return 0;
}


static int
istrP_errmsg_noexist(int istr_value)
{
#ifdef DEBUG
    fprintf(stderr,"ISTR: the reference string %d doesn't exist\n",
        istr_value);

    if (util_get_verbosity() >= 3)
    {
	util_error("Aborting (dumping core).");
	abort();
    }
#endif /* DEBUG */
    return 0;
}

