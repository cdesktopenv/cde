/* $XConsortium: options.c /main/2 1996/05/09 04:13:11 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_off_opt
 *		d_on_opt
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   options.c -- db_VISTA options module.

   (C) Copyright 1987 by Raima Corporation.
-----------------------------------------------------------------------*/
#include <stdio.h>
#include "vista.h"
#include "dbtype.h"


/* Turn on db_VISTA runtime options
*/
d_on_opt(optflag TASK_PARM)
int optflag;
TASK_DECL
{
   int i;

   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(LOCK_ALL));
   dboptions |= optflag;

#ifndef NO_TRANS
   if ( optflag & ARCLOGGING ) d_tron();
#endif

#ifndef NO_COUNTRY
   if ( optflag & IGNORECASE ) {
      if ( !db_global.ctbl_activ) {
	 if (ctbl_alloc() != S_OKAY )
	    RETURN (db_status);
	 db_global.ctbl_activ = TRUE;
      }
      for ( i = 97; i < 123; i++)
	 /* map lower to upper */
	 db_global.country_tbl.ptr[i].sort_as1 = (char)(i-32);
   }
#endif

   RETURN( db_status = S_OKAY );
}


/* Turn off db_VISTA runtime options
*/
d_off_opt(optflag TASK_PARM)
int optflag;
TASK_DECL
{
   int i;

   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(LOCK_ALL));
   dboptions &= ~optflag;

#ifndef NO_TRANS
   if ( optflag & ARCLOGGING ) d_troff();
#endif

#ifndef NO_COUNTRY
   if ( optflag & IGNORECASE ) {
      if ( db_global.ctbl_activ) {
	 for ( i = 97; i < 123; i++)
	    /* restore lower to lower */
	    db_global.country_tbl.ptr[i].sort_as1 = (char)i;
      }
   }
#endif

   RETURN( db_status = S_OKAY );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin options.c */
