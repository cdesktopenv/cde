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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: passwd.h /main/3 1995/10/27 16:19:23 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/* passwd.h us/unix/include/apollo/sys, brian
   Password definitions


     --------------------------------------------------------------------------
    |   THE FOLLOWING PROGRAMS ARE THE SOLE PROPERTY OF APOLLO COMPUTER INC.   |
    |         AND CONTAIN ITS PROPRIETARY AND CONFIDENTIAL INFORMATION.        |
     --------------------------------------------------------------------------

Changes:
    12/27/88    gilbert Modifications for C++.
    03/25/88    brian   ANSI version (from .ins.c)
    11/05/87    betsy   need to include des.ins.c 
    08/31/87    betsy   add support for unix encryption, remove passwd_$create
                          for sr10 registry
    07/07/87    pato    drop the (wrong) explicit values for passwd_$encrypt_t
    04/02/86    lwa     Change enum to short enum.
    01/13/86    nazgul  Created C version
    04/05/82    PJL     changes for DES encryption
*/

#ifndef apollo_sys_passwd_ins_c
#define apollo_sys_passwd_ins_c


/*  Enable function prototypes for ANSI C and C++  */
#if defined(__STDC__) || defined(c_plusplus) || defined(__cplusplus)
#    define _PROTOTYPES
#endif

/*  Required for C++ V2.0  */
#ifdef  __cplusplus
    extern "C" {
#endif

#ifdef _PROTOTYPES
#define std_$call	extern
#endif

#include "apollo/des.h"		/* copy from <apollo/sys/des.h>  */

#define  passwd_$mod     3
#define  passwd_$maxlen 8

typedef  short  enum { passwd_$none, passwd_$des,
                       passwd_$unix, passwd_$unix_bits }   passwd_$encrypt_t;
typedef  struct {
    passwd_$encrypt_t  ptype;
    union {
        char  plain[8];                 /* passwd_$none */
        struct {                        /* passwd_$des, passwd_$unix_bits   */
            short       rand;
            des_$text   bits;
        } des; 
        struct {                        /* passwd_$unix */
            char        salt[2];
            char        cipher[12]; 
        } unix_encrypted;
    } pw_union;
} passwd_$rec_t;

typedef  char  passwd_$str_t[passwd_$maxlen];            

std_$call boolean   passwd_$check(
#ifdef _PROTOTYPES
		passwd_$str_t		& pass_str,
		short			& pass_len,
		passwd_$rec_t		& enc_passwd,
		status_$t		* status
#endif
);

#ifdef  __cplusplus
    }
#endif

#endif
