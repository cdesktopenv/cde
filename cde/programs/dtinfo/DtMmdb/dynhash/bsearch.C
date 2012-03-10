/*
 * $XConsortium: bsearch.C /main/4 1996/10/04 09:33:19 drk $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
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


#include <sys/time.h>
#include <math.h>
#include <fstream.h>
#include <search.h>
#include "dynhash/bucket.h"

/*
extern "C" {
   int bsearch(char*, char*, int, int, int(*)());
};
*/

int compare(const void* vd1, const void* vd2)
{
   data_t* d1 = (data_t*)vd1;
   data_t* d2 = (data_t*)vd2;

   if ( d1 -> key < d2 -> key )
      return -1;
   else
   if ( d1 -> key == d2 -> key )
      return 0;
   else
      return 1;
}

#define TBSIZE 1000

main( int argc, char** argv )
{
   data_t *x = new data_t[TBSIZE];

   srand(314159);
   for ( int i=0; i<TBSIZE; i++ ) {
      x[i].key = rand() % TBSIZE;
      //x[i].key = i;
      x[i].dt  = voidPtr(i); 
   }


/*
   for ( i=0; i<TBSIZE; i++ ) 
     debug(cerr, x[i]);
*/

   struct timeval sec1, sec2;
   struct timezone tz1, tz2;

gettimeofday(&sec1, &tz1);
//debug(cerr, sec1.tv_sec);
//debug(cerr, sec1.tv_usec);

   qsort((char*)x, TBSIZE, sizeof(data_t), compare);

   for ( i=0; i<TBSIZE; i++ ) {
      data_t key = x[i];
      void * ok = bsearch((char*)(&key), 
                   (char*)x, TBSIZE, sizeof(data_t), compare);
      if ( ok == 0 ) {
          debug(cerr, "member test failed");
          exit(-2);
      }
   }
gettimeofday(&sec2, &tz2);
//debug(cerr, sec2.tv_sec);
//debug(cerr, sec2.tv_usec);

   if ( sec1.tv_usec > sec2.tv_usec ) {
      printf("difference = %ld   %6ld\n", 
	     (long)sec2.tv_sec-sec1.tv_sec-1, 
	     (long)1000000+sec2.tv_usec-sec1.tv_usec
            );
   } else {
      printf("difference = %ld   %6ld\n", 
	     (long)sec2.tv_sec-sec1.tv_sec, 
	     (long)sec2.tv_usec-sec1.tv_usec
            );
   }
}
