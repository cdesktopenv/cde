/*
 * $XConsortium: xclock.h /main/4 1996/08/21 15:55:52 drk $
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




#ifndef _xclock_h
#define _xclock_h 1

#include <time.h>
#include <sys/time.h>

#ifdef C_API
#include "utility/c_stream.h"
#else
#include <stream.h>
#endif

#include "utility/funcs.h"


/*******************************************/
// xclock class.
/*******************************************/
class xclock {

public:
   xclock() ;
   virtual ~xclock() {};
   char* unique_time_stamp();      
   void unique_time_stamp(long& sec, long& usec);      

protected:
    static struct timeval v_tp;
    static struct timezone v_tzp;
};

#endif

