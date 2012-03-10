/*
 * $XConsortium: version.C /main/4 1996/08/21 15:53:59 drk $
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


#include "version.h"

mm_version::mm_version(char* str, Boolean swap_order)
{
   short x ; 
   short y ;

   memcpy((char*)&x, str, sizeof(x));
   memcpy((char*)&y, str+major_bytes(), sizeof(y));

   if ( swap_order == true ) {
       ORDER_SWAP_USHORT(x);
       ORDER_SWAP_USHORT(y);
   }

   v_major = x; 
   v_minor = y;
}

Boolean mm_version::operator<(const mm_version& v2) const
{
   if ( v_major < v2.v_major )
      return true;

   if ( v_major == v2.v_major && v_minor < v2.v_minor )
      return true;
   else
      return false;
}

Boolean mm_version::operator==(const mm_version& v2) const
{
   if ( v_major == v2.v_major && v_minor == v2.v_minor )
      return true;
   else
      return false;
}

void mm_version::to_byte_string(char* str, Boolean swap_order)
{
   short x = v_major; 
   short y = v_minor;

   if ( swap_order == true ) {
       ORDER_SWAP_USHORT(x);
       ORDER_SWAP_USHORT(y);
   }

   memcpy(str, &x, sizeof(v_major));
   memcpy(str+major_bytes(), &y, sizeof(v_minor));

}
