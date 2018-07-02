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
/* $XConsortium: renfile.c /main/2 1996/05/09 04:16:22 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_renfile
 *		renfiles
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   renfile - Database file rename functions

   This file contains functions which perform dynamic
   db_VISTA database file renaming.

   (C) Copyright 1986 by Raima Corp.
-----------------------------------------------------------------------*/
#include <stdio.h>
#include "vista.h"
#include "dbtype.h"


/* Rename database file
*/
int
d_renfile(
const char *dbn,  /* database containing file to be renamed */
FILE_NO fno,  /* file id number for file to be renamed */
const char *fnm   /* new file name */
)
{
   REN_ENTRY_P r;

   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(LOCK_ALL));

   if ( dbopen ) RETURN( dberr(S_DBOPEN) );

   r.ptr = (REN_ENTRY *)ALLOC(&r, sizeof(REN_ENTRY), "r");
   if ( r.ptr == NULL )
      RETURN( dberr(S_NOMEMORY) );
   ll_access(&ren_list);
   if ( ll_append(&ren_list, (CHAR_P *)&r) != S_OKAY ) {
      RETURN( db_status );
   }
   r.ptr->Ren_db_name.ptr = 
      ALLOC(&r.ptr->Ren_db_name, strlen(dbn)+1,"r.ptr->Ren_db_name");
   r.ptr->File_name.ptr = 
      ALLOC(&r.ptr->File_name, strlen(fnm)+1,"r.ptr->File_name");
   if ( ! r.ptr->Ren_db_name.ptr || ! r.ptr->File_name.ptr )
      RETURN( dberr(S_NOMEMORY) );

   strcpy(r.ptr->Ren_db_name.ptr, dbn);
   strcpy(r.ptr->File_name.ptr, fnm);
   r.ptr->file_no = fno;

   MEM_UNLOCK(&r.ptr->Ren_db_name);
   MEM_UNLOCK(&r.ptr->File_name);
   ll_deaccess(&ren_list);

   RETURN( db_status = S_OKAY );
}


/* Process renamed file table
*/
int
renfiles(void)
{
   int dbt_lc;			/* loop control */
   REN_ENTRY_P *rp;
   DB_ENTRY *db_ptr;

   if ( ll_access(&ren_list) ) {
      db_ptr = curr_db_table;		/* Have to save it because of macros */
      while ((rp = (REN_ENTRY_P *)ll_next(&ren_list)) != NULL) {
	 MEM_LOCK(&rp->ptr->Ren_db_name);
	 MEM_LOCK(&rp->ptr->File_name);
	 for (dbt_lc = no_of_dbs, curr_db_table = db_table;
	      --dbt_lc >= 0; ++curr_db_table) {
	    if (strcmp(rp->ptr->Ren_db_name.ptr, DB_REF(db_name)) == 0) {
	       if ( rp->ptr->file_no < 0 || rp->ptr->file_no >= DB_REF(Size_ft) ) {
		  MEM_UNLOCK(&rp->ptr->Ren_db_name);
		  MEM_UNLOCK(&rp->ptr->File_name);
		  FREE(&rp->ptr->Ren_db_name);
		  FREE(&rp->ptr->File_name);
		  ll_deaccess(&ren_list);
		  return( dberr(S_RENAME) );
	       }
	       snprintf(file_table[NUM2INT(rp->ptr->file_no, ft_offset)].ft_name, FILENMLEN, "%s",
		      rp->ptr->File_name.ptr);
	       break;
	    }
	 }
	 MEM_UNLOCK(&rp->ptr->Ren_db_name);
	 MEM_UNLOCK(&rp->ptr->File_name);
	 FREE(&rp->ptr->Ren_db_name);
	 FREE(&rp->ptr->File_name);
      }
      curr_db_table = db_ptr;
   }
   ll_deaccess(&ren_list);
   ll_free(&ren_list);
   return( db_status = S_OKAY );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin renfile.c */
