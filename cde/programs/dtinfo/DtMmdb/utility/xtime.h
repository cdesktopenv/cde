/*
 * $XConsortium: xtime.h /main/4 1996/06/11 17:40:11 cde-hal $
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



#ifndef _xtime_h
#define _xtime_h 1

#include <X11/Xos.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/time.h>
#include <sys/timeb.h>

#include "funcs.h"

/*******************************************/
// timer class.
/*******************************************/
class xtime 
{

public:
   xtime() ;
   virtual ~xtime() {};
   void start();  // reset clock
   void stop(float &cpu_time, long &elapsed_time); // record time

private:
   float  v_cpu_stamp;
   long v_elapsed_stamp;

   struct tms  v_time_regs;

   struct timeval v_tv;
   struct timezone v_tz;
};

#endif

