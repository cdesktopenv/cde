/* $XConsortium: dbacode.c /main/2 1996/05/09 04:02:05 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_decode_dba
 *		d_encode_dba
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   dbacode.c -- Module for encoding and decoding database addresses

   (C) Copyright 1987 by Raima Corporation.
-----------------------------------------------------------------------*/
#include <stdio.h>
#include "vista.h"
#include "dbtype.h"


/* Decode file and slot number from a database address
*/
d_decode_dba(dba, file, slot)
DB_ADDR dba;  /* database address */
int FAR *file;    /* returned file number */
long FAR *slot;   /* returned slot number */
{
   *file = (int)(FILEMASK & (dba >> FILESHIFT));
   *slot = ADDRMASK & dba;
   return ( db_status = S_OKAY );
}



/* Encode database address from file and slot number
*/
d_encode_dba(file, slot, dba)
int file;      /* returned file number */
long slot;     /* returned slot number */
DB_ADDR FAR *dba;  /* database address */
{
   *dba = ((FILEMASK & file) << FILESHIFT) | slot;
   return ( db_status = S_OKAY );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin dbacode.c */
