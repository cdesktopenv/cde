/*
 * $XConsortium: mphf_hash_table.h /main/3 1996/06/11 17:20:23 cde-hal $
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


#ifndef _mphf_hash_table_h
#define _mphf_hash_table_h 1

#include "pattern.h"
#include "utility/funcs.h"
#include "utility/pm_random.h"
#include "hmphf/params.h"

class mphf_hash_table {

public:
   mphf_hash_table(params& params_ptr);
   ~mphf_hash_table();

   void clear();
   int fast_fit(int_pattern& pat);
   int fit_hash_table(int_pattern& pat);

   int num_filled_slots() { return v_num_filled_slots; };
   int no_slots() { return v_no_slots; };

protected:
   int* v_map_table;
   int* v_random_table;
   char* v_rep;
   int v_no_slots;
   int v_num_filled_slots;

};

#endif
