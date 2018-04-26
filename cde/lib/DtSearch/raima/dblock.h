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
/* $XConsortium: dblock.h /main/2 1996/05/09 04:02:57 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: FCNINIT
 *
 *   ORIGINS: 157
 *
 */
/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  368 28-Jul-88 RSC Integrate BSD changes into code
  115 16-Aug-88 RSC Integrate VAX/VMS changes into source code
  423 09-Sep-88 RSC Change variables to be compatible with MULTI_TASK
  420 07-Oct-88 RSC Unoptimized use of fl_list (full of bugs)
  441 09-Dec-88 RSC Modified defn of FCNPID for general lockmgr
*/

/* Lock Manager/Runtime function interaction */

/* Network Bios Status codes */
#define N_BUSY		0xff
#define N_OKAY		0x00
#define N_TIMEOUT	0x05
#define N_SESCLOSED	0x0a
#define N_DUPNAME	0x0d
#define N_TABFULL	0x11
#define N_OPENREJ	0x12
#define N_CALLNAME	0x14
#define N_NAMEUSED	0x16
#define N_NAMEDEL	0x17
#define N_SESABORT	0x18
#define N_INTFBUSY	0x21
#define N_COMMANDS	0x22

/* Function/Status codes */
#define L_RECOVER	-5
#define L_QUEUEFULL	-4
#define L_TIMEOUT	-3
#define L_UNAVAIL	-2
#define L_SYSERR	-1

#define L_OKAY		0

#define L_DBOPEN	1
#define L_DBCLOSE	2
#define L_LOCK		3
#define L_FREE		4
#define L_TRCOMMIT	5
#define L_TREND		6
#define L_SETTIME	7
#define L_RECDONE   	8
#define L_LOGIN		9
#define L_DELETE	10

#ifndef LOGFILELEN
#define LOGFILELEN 48
#endif

/* Macros for VMS mailbox usage */


typedef struct LM_LOCKREQ_S {
    INT  fref;
    INT  type;
} LM_LOCKREQ;

/* Message packets, Runtime to Lock Manager */

/* Some macro / typdefs to make this file much more readable */

#define PERMISSION 0666

#ifdef GENERAL

/*  Defines for GENERAL LOCKMGR */

#define FCNPID	INT fcn;
#define FCNDEF  INT fcn;
#define FCNINIT()  0
typedef char LM_FILEID;

#else					/* GENERAL */


/*  Defines for UNIX SYS-V */

#define FCNPID  LONG fcn;\
		int pid;
#define FCNDEF  LONG fcn;
#define FCNINIT()  0

/*  Defines for both UNIX SYS-V and BSD */

typedef struct LM_FILEID_S {
    ino_t  inode;
    dev_t  device;
} LM_FILEID;

#endif					/* GENERAL */

typedef struct LM_LOGIN_S {
    FCNPID
    char dbusrid[80];
} LM_LOGIN;

typedef struct LM_DBOPEN_S {
    FCNPID
    INT  nfiles;
    INT  type;
    LM_FILEID fnames[1];
} LM_DBOPEN;

typedef struct LM_DBCLOSE_S {
    FCNPID
    INT  nfiles;
    INT  frefs[1];
} LM_DBCLOSE;

typedef struct LM_LOCK_S {
    FCNPID
    INT  nfiles;
    LM_LOCKREQ locks[1];
} LM_LOCK;

typedef struct LM_RECOVERED_S {
    FCNPID
} LM_RECOVERED;

typedef struct LM_FREE_S {
    FCNPID
    INT  nfiles;
    INT  frefs[1];
} LM_FREE;

typedef struct LM_TRCOMMIT_S {
    FCNPID
    char logfile[LOGFILELEN];
} LM_TRCOMMIT;

typedef struct LM_TREND_S {
    FCNPID
} LM_TREND;

typedef struct LM_SETTIME_S {
    FCNPID
    INT  secs;
} LM_SETTIME;


/* Message packets, Lock Manager to Runtime */


typedef struct LR_DBOPEN_S {
    FCNDEF
    INT  status;
    char logfile[LOGFILELEN];
    INT  nusers;
    INT  nfiles;
    INT  frefs[1];
} LR_DBOPEN;

typedef struct LR_LOCK_S {
    FCNDEF
    INT  status;
    char logfile[LOGFILELEN];
} LR_LOCK;

#define DBLOCK_H
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin dblock.h */
