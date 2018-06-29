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
/* $XConsortium: mapchar.c /main/3 1996/10/01 16:39:53 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: bgets
 *		ctb_init
 *		ctbl_alloc
 *		ctbl_free
 *		d_mapchar
 *		nextc
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------

   mapchar.c -- db_VISTA character map module.

   detailed description

   AUTHOR: Guido Weischedel
   DATE:   August, 1988
   PROJECT: International Character sets

   Copyright (C) 1988 by Raima Corporation

-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
      20-Sep-88 WLW MULTI_TASK changes

*/


/* ********************** INCLUDE FILES ****************************** */
#include <stdio.h>
#include <fcntl.h>
#include "vista.h"
#include "dbtype.h"

/* ********************** LOCAL VARIABLE DECLARATIONS **************** */
static char buf[30];
static int cptr = -1;
static int buflen;

/* ********************** LOCAL FUNCTION DECLARATIONS **************** */
static void bgets(P1(char *) Pi(int) Pi(int));
static int nextc(P1(int));

/* Map ASCII-Characters for output and sorting
*/
int d_mapchar(
unsigned char	inchar,     /* value of character to be mapped */
unsigned char	outchar,    /* output character as    ... */
const char *sort_str,       /* sort string (max. len = 2) */
unsigned char	subsort     /* subsort value, to distinguish between two */
			    /* equal values (e.g. 'a' and 'A', if necessary) */
)
{
   int indx;

   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(RECORD_IO));

   if ( strlen(sort_str) > 2 )
      RETURN( dberr(S_INVSORT) );

   /* Is character-set table already installed? */
   if ( !db_global.ctbl_activ ) {
      if ( ctbl_alloc() != S_OKAY )
         RETURN( db_status );
      db_global.ctbl_activ = TRUE;
   }

   /* Modify table for inchar specifications */
   indx = inchar;
   db_global.country_tbl.ptr[indx].out_chr = outchar;
   db_global.country_tbl.ptr[indx].sort_as1 = sort_str[0];
   db_global.country_tbl.ptr[indx].sort_as2 = sort_str[1];
   db_global.country_tbl.ptr[indx].sub_sort = subsort;

   RETURN( db_status=S_OKAY );
}

/* read MAP_FILE and make appropriate d_mapchar-calls
*/
int ctb_init(void)
{
   int map_fd;
   unsigned char inchar, outchar, subsort;
   char loc_buf[21], sortas[3];
   short subs_i;
   char ctb_name[FILENMLEN*2];

   strcpy( ctb_name, db_global.ctbpath );
   strcat( ctb_name, CTBNAME );

   if ( (map_fd = open_b(ctb_name,O_RDONLY)) != -1 ) {
      while(bgets(loc_buf,20,map_fd), *loc_buf) {
	 if ( strcmp( loc_buf, "ignorecase" ) == 0 ) {
	    if ( d_on_opt( IGNORECASE CURRTASK_PARM ) != S_OKAY )
	       break;
	 }
	 else {
	    sscanf(loc_buf,"%c,%c,%hd,%2s",&inchar,&outchar,&subs_i,&sortas[0]);
	    subsort = (unsigned char) subs_i;
	    if (d_mapchar(inchar,outchar,sortas,subsort CURRTASK_PARM) != S_OKAY )
	       break;
	    }
      }
      close(map_fd);
      return( db_status );
   }
   return(db_status = S_OKAY);
}

/* do an fgets from a binary file */
static void bgets(char *s, int len, int fd)
{
    int c;

    len--;
    while ( len-- ) {
	do {
	    c = nextc( fd );
	    if ( c == -1 ) goto eof;
	    c &= 0xff;
	} while ( c == '\r' );
	if ( c == '\n' ) break;
	*s++ = (char)c;
    }
eof:
    *s = '\0';
}

/* get one character from the file */
static int nextc(int fd)
{
    int n;

    if ( cptr < 0 || cptr >= buflen ) {
	n = read( fd, buf, 30 );
	if ( n == 0 ) {
	    cptr = -1;
	    return( -1 );
	}
	cptr = 0;
	buflen = n;
    }
    return( (int)buf[cptr++] );
}


/* Allocate and initialize country_table
*/
int ctbl_alloc(void)
{
   if ((db_global.country_tbl.ptr = (CNTRY_TBL *)
	   ALLOC(&db_global.country_tbl,256*sizeof(CNTRY_TBL)+1,"country_tbl")) 
	   == NULL ) return( dberr(S_NOMEMORY) );

   /* fill table with standard values */
   byteset(db_global.country_tbl.ptr, '\0', 256*sizeof(CNTRY_TBL)+1);

   return( db_status = S_OKAY );
}

/* Free country table
*/
void ctbl_free(void)
{
   MEM_UNLOCK( &db_global.country_tbl );
   FREE( &db_global.country_tbl );
   db_global.ctbl_activ = FALSE;
}

/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin mapchar.c */
