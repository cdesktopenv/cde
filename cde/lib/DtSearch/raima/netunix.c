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
/* $XConsortium: netunix.c /main/4 1996/11/01 10:13:29 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: netbios_chk
 *		nw_addnm
 *		nw_call
 *		nw_cleanup
 *		nw_delnm
 *		nw_hangup
 *		nw_rcvmsg
 *		nw_send
 *		nw_sestat
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*------------------------------------------------------------------------
   netunix.c -- db_VISTA System-V/BSD Interface Module

   The "nw_" prefix specifies that the function will not return
   until the command has completed. For example, nw_send will return
   when the sent message has been received.

   NOTES FOR UNIX SYS V/BSD ARE EMBEDDED BELOW.  RUNTIME FUNCTIONS WILL USE
   THESE FUNCTIONS IN THE SAME WAY ON DOS OR UNIX.

   These functions are used by the db_VISTA runtime.

   nw_addnm      -- Adds user id name to network.
		    DO NOTHING.
   nw_delnm      -- Deletes user id name from network.
		    DO NOTHING.
   nw_call       -- Calls a user id to establish communication session.
                    SYS V:
		       CREATE THE TOKEN FILE, AND MAKE SURE THAT THE LOCK
		       MANAGER'S EXISTS.
		       CREATE AND SET VALUE OF SEMAPHORE.
		       CREATE AN INCOMING MESSAGE QUEUE.
                    BSD:
                       MAKE SURE THAT THE LOCK MANAGER EXISTS.
                       OPEN A BIDIRECTIONAL COMMUNICATION SOCKET TO IT.
		    SEND FIRST MESSAGE TO LOCK MANAGER.  MESSAGE WILL INCLUDE
		    DBUSERID (RECEIVED BY nw_addnm()), AND PROCESS ID.
   nw_hangup     -- Disconnects session.
		    SYS V:
		       REMOVE THE TOKEN FILE.
		       DELETE THE SEMAPHORE.
		       DELETE THE INCOMING MESSAGE QUEUE.
		    BSD:
		       CLOSE THE SOCKET.
   nw_send       -- Sends message to specific user.
		    SEND THE MESSAGE, AS IS, TO LOCK MANAGER.
   nw_rcvmsg     -- Receives message from specific user.
		    RECEIVE THE MESSAGE, AS IS, FROM LOCK MANAGER.

------------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  310 10-Aug-88 RSC Cleanup function prototype (put MESSAGE in dbtype.h)
  368 25-Aug-88 RSC Fix BSD integration
  423 08-Sep-88 RSC Change for compatibility with Multi-tasking
  423 10-Sep-88 RSC Change dbpush to use new calling convention
      08-Feb-90 WLW In nw_hangup, unlink file after deleting queue (SYSV)
*/

#include <sys/errno.h>
#include <stdio.h>
#include "vista.h"
#include "dbtype.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>

/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin netunix.c */
