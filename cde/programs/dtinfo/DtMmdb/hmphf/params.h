/*
 * $XConsortium: params.h /main/3 1996/06/11 17:20:32 cde-hal $
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


#ifndef _params_h
#define _params_h 1

#include <values.h>
#include "utility/funcs.h"

class params 
{

public:
   float v_bits;
   unsigned int v_p1;
   unsigned int v_p2;
   unsigned int v_b; /* no. of buckets */
   unsigned int v_n; /* no. of keys */
   unsigned int v_r; /* offset into the random number table */
   unsigned int v_seed; /* seed to the random number table generator */

   params() {};
   ~params() {};

   void select_value(float bts = 0.0);
   void re_select_value();

   friend ostream& operator<<(ostream&, params&);
};

#endif
