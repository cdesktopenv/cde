/* $XConsortium: opens.c /main/2 1996/05/09 04:12:57 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: open_b
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   opens.c -- db_VISTA system open() interface module

   (C) Copyright 1987 by Raima Corp.
-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  115 19-Jul-88 RSC Integrate VAX/VMS changes into code
  368 28-Jul-88 RSC Integrate BSD changes into code
  310 10-Aug-88 RSC Cleanup function prototype.
  571 27-Jan-89 RSC Removed extraneous include dbtype.h
  571 30-Jan-89 WLW Added required header info for Windows compile w/o dbtype.h
*/

#include <stdio.h>
#include <fcntl.h>
#include "vista.h"
#ifdef MSC
#include <share.h>
#include <io.h>
#include <string.h>
#endif


/* Open a binary file for shared access
*/
int open_b(filenm, flags)
CONST char FAR *filenm;
int flags;  
{
   int desc;

   desc = open(filenm, flags, 0666);
   if ( desc < 0 && (flags & O_CREAT) )
      desc = creat(filenm, 0666);
   if ((desc >= 3) && (fcntl(desc, F_SETFD, 1) < 0)) {
      close(desc);
      desc = -1;
   }
#ifdef MSC
   flags |= O_BINARY;
   desc = sopen((char *)filenm, flags, SH_DENYNO, 0666);
#endif
#ifdef TURBO
   flags |= O_BINARY | O_DENYNONE;
   desc = open(filenm, flags, 0666);
#endif
#ifdef LAT
   flags |= O_RAW | O_SDN;
   desc = open(filenm, flags, 0666);
#endif
#ifdef WIZ
   flags |= O_BINARY;
   desc = open(filenm, flags, 0666);
#endif
   return( desc );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin opens.c */
