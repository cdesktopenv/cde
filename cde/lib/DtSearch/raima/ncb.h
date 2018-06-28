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
/* $XConsortium: ncb.h /main/2 1996/05/09 04:12:28 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: none
 *
 *   ORIGINS: 157
 *
 */
#define NETNAME_LEN 16
#ifndef NULL
#define NULL (char *)0
#endif

#define NETBIOS_INT 0x5c

#define ADDNAME 0x30
#define DELNAME 0x31
#define RESET   0x32
#define STATUS  0x33
#define SESSTAT 0x34
#define CALL    0x10
#define LISTEN  0x11
#define HANGUP  0x12
#define SEND    0x14
#define RECEIVE 0x15
#define ADDNAIT 0xB0
#define DELNAIT 0xB1
#define CALNAIT 0x90
#define LISNAIT 0x91
#define HANNAIT 0x92
#define SENNAIT 0x94
#define RECNAIT 0x95
#define ARENAIT 0x96
#define ANY     0xFF
#define PENDING 0xFFFF
#define NCBSIZE 64

#ifdef TURBO
#ifdef __SMALL__
#define SMALL_DATA
#endif
#ifdef __MEDIUM__
#define SMALL_DATA
#endif
#endif

#ifdef MSC
#ifdef    M_I86SM
#define SMALL_DATA
#endif
#ifdef    M_I86MM
#define SMALL_DATA
#endif
#endif

#ifdef LAT
#ifdef SPTR
#define SMALL_DATA
#endif
#endif

/***** NCB Commands parameter block *****/
typedef struct {
	char  command;	/* NETBIOS Command */
	char  retcode;	/* Return value (some enumerated below) */
	char  lsn;	/* Local session number */
	char  num;	/* Name number */
	char *buffer_off; /* Message buffer offset */
#ifdef SMALL_DATA
	int   buffer_seg; /* Message buffer segment */
#endif
	int   length;	/* Length of message buffer */
	char  callname[NETNAME_LEN]; /* Name of user being called */
	char  name[NETNAME_LEN];     /* My name */
	char  rto;	/* Receive timeout (500ms units) */
	char  sto;	/* Send timeout (500ms units) */
	char *post_off; /* Post function offset */
#ifdef SMALL_DATA
	int   post_seg; /* Post function segment */
#endif
	char  lana_num; /* Local area network adapter number (use 0) */
	char  cmd_cplt; /* Command complete return value, used when posting */
	char  reserve[14]; /* reserved for Net BIOS */
} NCB;

typedef struct {
    char rep_sess;
    char name_sess;
    char gram;
    char rcvany;
    struct {
	char lsn;
	char state_sess;
	char loc_name[16];
	char rem_name[16];
	char rcvs;
	char sends;
    } s[32];
} SESSION;

#define NCB_H
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin ncb.h */
