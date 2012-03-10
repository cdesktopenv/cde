/*
 * $XConsortium: abs_storage.cc /main/6 1996/07/18 14:52:12 drk $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
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


#include "storage/abs_storage.h"

abs_storage::abs_storage( char* file_path, char* file_name,
                          c_code_t c_id, rep_policy* p ) : 
policy(p), root(c_id), index_num(-1), v_swap_order(false)
{
   strcpy(path, file_path);
   strcpy(name, file_name);
}

abs_storage::~abs_storage()
{
}

int abs_storage::byte_order()
{
   unsigned int x;
   unsigned char *p;

   x = 0x01020304;
   p = (unsigned char *)&x;

   switch (*p) {
    case 1:
       return (mmdb_big_endian);
    case 4:
       return (mmdb_little_endian);
    default:
       throw(stringException("neither BIG_ENDIAN or LITTLE_ENDIAN machine"));
   }
}


