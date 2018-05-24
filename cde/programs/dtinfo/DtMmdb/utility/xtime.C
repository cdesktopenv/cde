/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * $XConsortium: xtime.cc /main/13 1996/07/18 15:00:24 drk $
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



#include "utility/xtime.h"

#ifdef SVR4
#include <sys/time.h>

#if !defined (_IBMR2) && !defined(sun)
extern "C" { extern int gettimeofday(struct timeval *tp); }
#endif

#endif


xtime::xtime() : 

v_cpu_stamp(0), v_elapsed_stamp(0)
{
   memset(&v_time_regs, 0, sizeof(struct tms));
   memset(&v_tv, 0, sizeof(struct timeval));
}

void xtime::stop(float &cpu_time, long &elp_time)
{
   cpu_time = -v_cpu_stamp;
   elp_time = v_elapsed_stamp;

   start();

   cpu_time += v_cpu_stamp;
   elp_time = v_elapsed_stamp - elp_time;
}

// add a mark

void xtime::start()
{
#if defined (SVR4) && !defined (_IBMR2) && !defined(sun)
   if ( gettimeofday(&v_tv) != 0 ) {
#else
   if ( gettimeofday(&v_tv, &v_tz) != 0 ) {
#endif
      MESSAGE(cerr, "xtime::start(): gettimeofday() failed");
      throw(systemException(errno));
   } 

   v_elapsed_stamp = v_tv.tv_sec;

#if defined(SVR4)
   if ( times(&v_time_regs) == -1 ) 
#else
   if ( times(&v_time_regs) != 0 ) 
#endif
   {
      MESSAGE(cerr, "xtime::start(): times() failed");
      throw(systemException(errno));
   } 

   v_cpu_stamp = 
      float(v_time_regs.tms_utime + v_time_regs.tms_stime +
            v_time_regs.tms_cutime + v_time_regs.tms_cstime 
           ) / 60.0;
}
