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
 * $XConsortium: xclock.cc /main/3 1996/06/11 17:39:56 cde-hal $
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




#include "xclock.h"

#if defined (SVR4) && !defined (_IBMR2) && !defined(SC3)
extern "C" { extern int gettimeofday(struct timeval*); }
#endif

struct timeval xclock::v_tp;

xclock::xclock() 
{
   v_tp.tv_sec = 0;
   v_tp.tv_usec = 0;
}

char* xclock::unique_time_stamp()
{
   long sec, usec;
   unique_time_stamp(sec, usec);
   return form("%ld.%ld", sec, usec);
}

void xclock::unique_time_stamp(long& sec, long& usec)
{
   struct timeval tpx;

#if !defined (SVR4) || defined (_IBMR2) || defined(SC3)
  struct timezone tzpx;
#endif

   do {
       if (
#if defined (SVR4) && !defined (_IBMR2) && !defined(SC3)
        gettimeofday(&tpx)
#else
        gettimeofday(&tpx, &tzpx)
#endif
       == -1 ) {
         MESSAGE(cerr, "xclock(): gettimeofday() failed");
         throw(systemException(errno));
       }
    } while (tpx.tv_sec == v_tp.tv_sec && tpx.tv_usec == v_tp.tv_usec);

   v_tp.tv_sec=  tpx.tv_sec;
   v_tp.tv_usec = tpx.tv_usec;

   sec = v_tp.tv_sec;
   usec = v_tp.tv_usec;
}

//xclock mmdb_xclock;
