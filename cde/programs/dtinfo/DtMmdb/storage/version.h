/*
 * $XConsortium: version.h /main/5 1996/08/21 15:54:03 drk $
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



#ifndef _mm_version_h
#define _mm_version_h 1

#include "utility/funcs.h"

/*************************************/
// The mm_version class
/*************************************/

class mm_version 
{
protected:
   short v_major;
   short v_minor;

public:
   mm_version(short ma =-1, short mi =-1) : v_major(ma), v_minor(mi) {};
   mm_version(char*, Boolean swap_order = false);
   virtual ~mm_version() {};

   Boolean operator<  (const mm_version&) const;
   Boolean operator== (const mm_version&) const;

   static int version_bytes() { return major_bytes() + minor_bytes(); };
   static int major_bytes() { return sizeof(short); };
   static int minor_bytes() { return sizeof(short); };

   void to_byte_string(char*, Boolean swap_order = false);

   short major_version() { return v_major; };
   short minor_version() { return v_minor; };

};

typedef mm_version* mm_versionPtr;

#endif
