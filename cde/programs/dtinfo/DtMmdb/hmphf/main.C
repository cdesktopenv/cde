/*
 * $XConsortium: main.cc /main/3 1996/06/11 17:20:02 cde-hal $
 *
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */


#include "utility/xtime.h"
#include "hmphf/mphf_funcs.h"
#include "hmphf/sorter.h"

#define NO_BITS_INCREASES 5

main(int argc, char* argv[])
{
   int ok;
 
   if ( argc != 2 ) {
      fprintf(stderr, form("usage: %s keyfile", argv[0]));
      return(-1);
   }


/*
xtime tmer;
tmer.start();
*/

   sorter stor(argv[1]);

/*
float f1 = 0, f2 = 0;
tmer.stop(f1, f2);
debug(cerr, f1);
debug(cerr, f2);
*/

   params pms;

   pms.v_n = stor.no_unique_keys();


   pms.select_value();
//debug(cerr, pms);

   buffer mphf_spec(LBUFSIZ);

   for ( int i=0; i<NO_BITS_INCREASES; i++ ) {

      ok = compute_a_mphf(stor.unique_keys(), pms, mphf_spec) ;

      switch (ok) {

       case 0:
          MESSAGE(cerr, "Hashing done");
          exit(0);

       case 1:
          pms.re_select_value();
          break;

       case -1:
          exit(-1);
      }
   }

   MESSAGE(cerr, "finding a mphf failed");
   exit(-1);
}
   
