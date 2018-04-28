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

#ifndef SINGLE_USER
# include <X11/Xos.h>
# include <errno.h>
# ifdef X_NOT_STDC_ENV
extern int errno;
# endif

static char tokfile[80];
static int inqid;
static int outqid;
static int semid;
static int mypid;
static char tmpdir[] = "/tmp/";



/* Check for the existence of NetBIOS on this machine
*/
int netbios_chk()
{
    /* On UNIX, indicate that there is no problem */
    return( 1 );
}

/* Issue ADD NAME command - wait 
*/
/*ARGSUSED*/
int nw_addnm(name, ncb_num)
char *name;
int *ncb_num;
{
    return( net_status = N_OKAY );
}



/* Issue DELETE NAME command - wait 
*/
/*ARGSUSED*/
int nw_delnm(name)
char *name;
{
    return( net_status = N_OKAY );
}




/* Cleanup any dirty sessions
*/
/*ARGSUSED*/
int nw_cleanup(dbusrid)
   char *dbusrid;
{
    return( net_status = N_OKAY );
}


/* Issue a session status call
*/
int nw_sestat()
{
    return( net_status = N_OKAY );
}



/* Issue CALL command - wait 
*/
int nw_call(them, me, ncb_lsn)
char *them;
char *me;
int *ncb_lsn;
{
    key_t key;
    FILE *fp;
    struct sembuf sops[1];
    LM_LOGIN login;
    int tokpos;

    if ( ! db_lockmgr )
	RETURN( net_status = N_OKAY );

    /* create the token file for the lock manager */
    strcpy(tokfile, tmpdir);
    tokpos = sizeof(tmpdir) - 1;
    strcpy(&tokfile[tokpos], them);

    if ( ( key = ftok( tokfile, 1 ) ) == -1 )
	/* /tmp/lockmgr file non-existant */
	RETURN( net_status = N_CALLNAME );
    
    /* obtain the id to the lock manager's input queue and check the
       value of the semaphore
    */
    if (((outqid = msgget(key, PERMISSION | IPC_CREAT)) == -1) ||
	((semid = semget(key, 1, PERMISSION)) == -1))
	RETURN( net_status = N_TIMEOUT );

    if ( semctl( semid, 0, GETVAL, NULL ) != 1 ) {
	/* the lockmgr has terminated, but has not been cleared */
	RETURN( net_status = N_CALLNAME );
    }

    /* create the token file for this dbuserid */
    strcpy(&tokfile[tokpos], me);

    /* get a key that is unique for the token file */
    if ( ( key = ftok( tokfile, 1 ) ) != -1 ) {
	/* The token file exists.  Is it because there is another active
	   process which is using the id?
	*/
	/* check the value of the semaphore */
	if ( ( semid = semget( key, 1, PERMISSION | IPC_CREAT ) ) == -1 )
	    RETURN( net_status = N_TIMEOUT );
	if ( semctl( semid, 0, GETVAL, NULL ) == 1 ) {
	    /* this dbuserid is already active */
	    RETURN( net_status = N_NAMEUSED );
	}
	/* just in case this process formerly aborted with message remaining
	   in its input queue, delete the queue
	*/
	if (((inqid = msgget(key, PERMISSION | IPC_CREAT)) == -1) ||
	    (msgctl(inqid, IPC_RMID, (struct msqid_ds *)NULL) == -1))
	    RETURN( net_status = N_TIMEOUT );
    }
    else {
	/* create the token file */
	if ( ( fp = fopen( tokfile, "w" ) ) == NULL )
	    RETURN( net_status = N_TIMEOUT );
	fclose( fp );

	/* get the key value for the token file */
	if ( ( key = ftok( tokfile, 1 ) ) == -1 ) {
	    RETURN( net_status = N_TIMEOUT );
	}
    }

    /* create and set the value of a semaphore */
    if ( ( semid = semget( key, 1, PERMISSION | IPC_CREAT ) ) == -1 )
	RETURN( net_status = N_TIMEOUT );
    sops[0].sem_num = 0;
    sops[0].sem_op = 1;
    sops[0].sem_flg = SEM_UNDO;
    while ( semop( semid, sops, 1 ) == -1 ) {
	if ( errno == EINTR ) continue;
	RETURN( net_status = N_TIMEOUT );
    }

    /* create my incoming message queue */
    if ( ( inqid = msgget( key, PERMISSION | IPC_CREAT ) ) == -1 )
	RETURN( net_status = N_TIMEOUT );

    /* send the message to the lock manager */
    login.fcn = L_LOGIN;
    strcpy( login.dbusrid, me );
    login.pid = getpid();
    mypid = login.pid;
    while ( msgsnd(outqid, (struct msgbuf *)&login,
		   sizeof(LM_LOGIN)-sizeof(LONG), 0) == -1 ) {
	if ( errno == EINTR ) continue;
	RETURN( net_status = N_TIMEOUT );
    }
    *ncb_lsn = 0;
    RETURN( net_status = N_OKAY );
}


/* Issue HANGUP command - wait 
*/
/*ARGSUSED*/
int nw_hangup(lsn)
int lsn;
{
    if ( ! db_lockmgr )
	RETURN( net_status = N_OKAY );

    /* delete the incoming message queue */
    msgctl( inqid, IPC_RMID, (struct msqid_ds *)NULL );

    /* delete the semaphore */
    semctl( semid, 0, IPC_RMID, NULL );

    /* delete the token file */
    unlink( tokfile );
    RETURN( net_status = N_OKAY );
}



/* Issue SEND command - wait 
*/
/*ARGSUSED*/
int nw_send(lsn, msgtxt, msglen)
int lsn;
MESSAGE *msgtxt;
int msglen;
{

    if ( ! db_lockmgr ) {
       last_mtype = msgtxt->mtype;
       RETURN( net_status = N_OKAY );
    }
    msgtxt->pid = mypid;

    /* everything should be ready, just send it */
    while ( msgsnd(outqid, (struct msgbuf *)msgtxt, msglen-sizeof(LONG),
		   0) == -1 ) {
	if ( errno == EINTR ) continue;
	RETURN( net_status = N_TIMEOUT );
    }

    RETURN( net_status = N_OKAY );
}



/* Issue RECEIVE command - wait 
*/
/*ARGSUSED*/
int nw_rcvmsg(lsn, msgtxt, msglen, ncb_len)
int lsn;
MESSAGE *msgtxt;
int msglen;
int *ncb_len;
{
    LR_DBOPEN *dp;
    LR_LOCK *lp;

    if ( ! db_lockmgr ) {
       if ( last_mtype == L_LOCK ) {
	  lp = (LR_LOCK *)msgtxt;
	  lp->fcn = L_LOCK;
	  lp->status = L_OKAY;
       }
       else {
	  dp = (LR_DBOPEN *)msgtxt;
	  dp->fcn = L_DBOPEN;
	  dp->status = L_OKAY;
	  dp->nusers = 1;
       }
       RETURN( net_status = N_OKAY );
    }
    while ( ( *ncb_len = msgrcv(inqid, (struct msgbuf *)msgtxt,
				msglen-sizeof(LONG), (LONG)0, 0) ) == -1 ) {
	if ( errno == EINTR ) continue;
	RETURN( net_status = N_TIMEOUT );
    }

    RETURN( net_status = N_OKAY );
}

/* The remaining functions are BSD only */

#endif
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin netunix.c */
