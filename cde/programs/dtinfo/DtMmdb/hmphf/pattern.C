/*
 * $XConsortium: pattern.cc /main/3 1996/06/11 17:20:37 cde-hal $
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



#include "pattern.h"

int_pattern::int_pattern(int i): v_rep_size(i), v_count(0)
{
   v_rep = new int[i];
}

int_pattern::~int_pattern()
{
   delete v_rep;
}

int int_pattern::duplicate()
{
   int i,j;
   for ( i=0; i<no_elmts()-1; i++ ) {
      for ( j=i+1; j<no_elmts(); j++ ) {
         if ( (*this)[i] == (*this)[j] )
            return -1;
      }
   }
   return 0;
}

