/* $XConsortium: des.h /main/3 1995/10/27 16:19:08 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*
{ DES, USX/LIB/DESLIB, PJL, 04/05/82 }
{   DES routines }

{  CHANGES:
   12/21/88 gilbert   Modifications for C++.
   03/25/88 brian     Created ANSI C version
   01/13/86 nazgul    Created C version
   08/25/83 spf   Added us_des_ins_pas declaration
}
*/

#ifndef apollo_sys_des_h
#define apollo_sys_des_h


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

typedef  char  des_$key[8];

/*
  packed record                       { NOTE THIS WELL!!! }
        bits:   0..127;                 { 56 bits: LEFT ADJUSTED in 8 bytes }
        parity: 0..1                    { reserved for parity, but ignored }
        end;
*/

typedef  char  des_$text[8];          /* Actually a 64 bit set */

typedef  char  des_$key_sched[16][8];

std_$call void    des_$encrypt(
#ifdef _PROTOTYPES
        des_$text       & plain,          /* input 64 bits to encrypt */
        des_$text       * cipher,         /* ...get back result */
        des_$key_sched  & key_sched       /* made with des_$compute_key_sched */
#endif
);

std_$call void   des_$decrypt(
#ifdef _PROTOTYPES
        des_$text       & cipher,         /* input 64 bits to decrypt */
        des_$text       * plain,          /* ...get back result */
        des_$key_sched  & key_sched       /* made with des_$compute_key_sched */
#endif
);

std_$call void   des_$compute_key_sched(
#ifdef _PROTOTYPES
        des_$key        & key,            /* eight byte key */
        des_$key_sched  * key_sched       /* output key schedule */
#endif
);

#ifdef  __cplusplus
    }
#endif

#endif
