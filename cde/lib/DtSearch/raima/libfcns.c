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
/* $XConsortium: libfcns.c /main/2 1996/05/09 04:11:16 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: ADDRcmp
 *		INTcmp
 *		Pi
 *		check_dba
 *		ctblcmp
 *		dbn_check
 *		fldcmp
 *		nfld_check
 *		nrec_check
 *		nset_check
 *		null_dba
 *		rec_okay
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*****************************************************************************
   Miscellaneous db_VISTA library functions
*****************************************************************************/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  255 30-Jun-88 RSC check_dba: don't assume page 0 read
  115 19-Jul-88 RSC Integrate VAX/VMS changes
      04-Aug-88 RTK MULTI_TASK changes
  310 10-Aug-88 RSC Cleanup function prototype.
      18-Aug-88 RSC Moved rn_type/dba to separate table
  424 21-Sep-88 RSC Integrate international character set (ESM)
  420 06-Dec-88 WLW Updated Curr_db_table when using setdb.
*/

#include <stdio.h>
#include "vista.h"
#include "dbtype.h"

/* Internal function prototypes */
static int rec_okay(P1(int) Pi(int *) 
				   Pi(RECORD_ENTRY FAR * FAR *));
static int ctblcmp(P1(CONST unsigned char FAR*)
                                  Pi(CONST unsigned char FAR*) Pi(int));

#ifndef	 ONE_DB
/* Check for valid db number and set curr_db, curr_db_table and curr_rn_table
*/
int
dbn_check(dbn)
int dbn;
{
   if ( ! dbopen ) 
      return( dberr(S_DBOPEN) );

   if ( no_of_dbs > 1 && ! setdb_on ) {
      if ( dbn < 0 || dbn >= no_of_dbs )
	 return( dberr(S_INVDB) );
      db_table[curr_db].curr_dbt_rec = curr_rec;
      curr_db_table = &db_table[curr_db = dbn];
      curr_rn_table = &rn_table[curr_db];	/* point to new rn_table */
      curr_rec = curr_db_table->curr_dbt_rec;
   }
   return( db_status = S_OKAY );
}
#endif


/* Check for valid (external) set number and return (internal) set number
   and set_table pointer.
*/
int
nset_check(nset, set, set_ptr )
register int nset;
int *set;
SET_ENTRY FAR * FAR *set_ptr;
{
   nset -= SETMARK;
   if ((nset < 0) || (nset >= TABLE_SIZE(Size_st)))
      return( dberr(S_INVSET) );

   *set_ptr = &set_table[*set = NUM2INT(nset, st_offset)];
   return( db_status = S_OKAY );
}


/* Check for valid (external) field number and return (internal) record
   and field numbers and pointers.
*/
int
nfld_check(nfld, rec, fld, rec_ptr, fld_ptr )
long nfld;
int *rec;
int *fld;
RECORD_ENTRY FAR * FAR *rec_ptr;
FIELD_ENTRY FAR * FAR *fld_ptr;
{
   int trec;
   int tfld;

   if (!rec_okay(trec = (int)(nfld/FLDMARK), rec, (RECORD_ENTRY FAR * FAR *)rec_ptr) ||
       ((tfld = (int)(nfld - trec*FLDMARK)) < 0) ||
       (tfld >= TABLE_SIZE(Size_fd)))
      return( dberr(S_INVFLD) );

   *fld_ptr = &field_table[*fld = tfld + (*rec_ptr)->rt_fields];
   return( db_status = S_OKAY );
}



/* Check for valid (external) record number and return (internal) record
   number and pointer.
*/
int
nrec_check(nrec, rec, rec_ptr)
int nrec;
int *rec;
RECORD_ENTRY FAR * FAR *rec_ptr;
{
   if (rec_okay(nrec - RECMARK, rec, (RECORD_ENTRY FAR * FAR *)rec_ptr))
      db_status = S_OKAY;
   else
      dberr(S_INVREC);
   return( db_status );
}


/* Internal record number check
*/
static int rec_okay(nrec, rec, rec_ptr)
register int nrec;
int *rec;
RECORD_ENTRY FAR * FAR *rec_ptr;
{
   if ((nrec < 0) || (nrec >= TABLE_SIZE(Size_rt)))
      return (FALSE);

   *rec_ptr = &record_table[*rec = NUM2INT(nrec, rt_offset)];
   return (TRUE);
}


/* Compare values of two db_VISTA data fields
*/
int fldcmp(fld_ptr, f1, f2)
FIELD_ENTRY FAR *fld_ptr;
CONST char FAR *f1;   /* pointer to field 1 */
CONST char FAR *f2;   /* pointer to field 2 */
/*
   returns < 0 if f1 < f2,
	   = 0 if f1 == f2,
	   > 0 if f1 > f2
*/
{
   register int kt_lc;			/* loop control */
   int i, k, elt, result, len, cur_len, sub_len, entries;
#ifdef DS
   int ui1, ui2;
   long ul1, ul2;
   short us1, us2;
#else
   unsigned int ui1, ui2;
   unsigned long ul1, ul2;
   unsigned short us1, us2;
#endif
   int i1, i2;
   long l1, l2;
   short s1, s2;
#ifndef	 NO_FLOAT
   float F1, F2;
   double d1, d2;
#endif
   FIELD_ENTRY FAR *fld_max;
   FIELD_ENTRY FAR *sfld_ptr;
   KEY_ENTRY FAR *key_ptr;
   INT FAR *dim_ptr;

   len = fld_ptr->fd_len;

   /* compute number of array elements */
   entries = 1;
   for (i = 0, dim_ptr = fld_ptr->fd_dim;
	(i < MAXDIMS) && *dim_ptr;
	++i, ++dim_ptr)
      entries *= *dim_ptr;

   switch ( fld_ptr->fd_type ) {
      case CHARACTER:
	 if ( fld_ptr->fd_dim[1] )
	    return ( bytecmp(f1, f2, len) );
	 else if ( fld_ptr->fd_dim[0] )
	 {
#ifdef NO_COUNTRY
	    return ( strncmp(f1, f2, len) );
#else
	    if ( db_global.ctbl_activ ) return ( ctblcmp(f1, f2, len) );
	    else return ( strncmp(f1, f2, len) );
#endif
	 }
	 else
	    return ( (int)(*f1) - (int)(*f2) );
      case REGINT:
	 for ( result = elt = 0; result == 0 && elt < entries; ++elt ) {
	    if ( fld_ptr->fd_flags & UNSIGNEDFLD ) {
	       bytecpy(&ui1, f1+(elt*sizeof(int)), sizeof(int));
	       bytecpy(&ui2, f2+(elt*sizeof(int)), sizeof(int));
	       if ( ui1 < ui2 ) result = -1;
	       else if ( ui1 > ui2 ) result =  1;
	    }
	    else {
	       bytecpy(&i1, f1+(elt*sizeof(int)), sizeof(int));
	       bytecpy(&i2, f2+(elt*sizeof(int)), sizeof(int));
	       if ( i1 < i2 ) result = -1;
	       else if ( i1 > i2 ) result =  1;
	    }
	 }
	 break;
      case LONGINT:
	 for ( result = elt = 0; result == 0 && elt < entries; ++elt ) {
	    if ( fld_ptr->fd_flags & UNSIGNEDFLD ) {
	       bytecpy(&ul1, f1+(elt*sizeof(long)), sizeof(long));
	       bytecpy(&ul2, f2+(elt*sizeof(long)), sizeof(long));
	       if ( ul1 < ul2 ) result = -1;
	       else if ( ul1 > ul2 ) result =  1;
	    }
	    else {
	       bytecpy(&l1, f1+(elt*sizeof(long)), sizeof(long));
	       bytecpy(&l2, f2+(elt*sizeof(long)), sizeof(long));
	       if ( l1 < l2 ) result = -1;
	       else if ( l1 > l2 ) result =  1;
	    }
	 }
	 break;
      case SHORTINT:
	 for ( result = elt = 0; result == 0 && elt < entries; ++elt ) {
	    if ( fld_ptr->fd_flags & UNSIGNEDFLD ) {
	       bytecpy(&us1, f1+(elt*sizeof(short)), sizeof(short));
	       bytecpy(&us2, f2+(elt*sizeof(short)), sizeof(short));
	       if ( us1 < us2 ) result = -1;
	       else if ( us1 > us2 ) result =  1;
	    }
	    else {
	       bytecpy(&s1, f1+(elt*sizeof(short)), sizeof(short));
	       bytecpy(&s2, f2+(elt*sizeof(short)), sizeof(short));
	       if ( s1 < s2 ) result = -1;
	       else if ( s1 > s2 ) result =  1;
	    }
	 }
	 break;
#ifndef	 NO_FLOAT
      case FLOAT:
	 for ( result = elt = 0; result == 0 && elt < entries; ++elt ) {
	    bytecpy(&F1, f1+(elt*sizeof(float)), sizeof(float));
	    bytecpy(&F2, f2+(elt*sizeof(float)), sizeof(float));
	    if ( F1 < F2 )  result = -1;
	    else if ( F1 > F2 )  result = 1;
	 }
	 break;
      case DOUBLE:
	 for ( result = elt = 0; result == 0 && elt < entries; ++elt ) {
	    bytecpy(&d1, f1+(elt*sizeof(double)), sizeof(double));
	    bytecpy(&d2, f2+(elt*sizeof(double)), sizeof(double));
	    if ( d1 < d2 )  result = -1;
	    else if ( d1 > d2 )  result = 1;
	 }
	 break;
#endif
      case DBADDR:
	 for ( result = elt = 0; result == 0 && elt < entries; ++elt ) {
	    result = ADDRcmp((DB_ADDR FAR *)(f1+(elt*sizeof(DB_ADDR))),
			     (DB_ADDR FAR *)(f2+(elt*sizeof(DB_ADDR))));
	 }
	 break;
      case GROUPED:
	 len /= entries; /* length of each entry */
	 fld_max = &field_table[size_fd];
	 for (i = 0, cur_len = 0; i < entries; ++i, cur_len += len) {
	    for (sfld_ptr = fld_ptr + 1;
		 (sfld_ptr < fld_max) && (sfld_ptr->fd_flags & STRUCTFLD);
		 ++sfld_ptr) {
	       sub_len = cur_len + sfld_ptr->fd_ptr - fld_ptr->fd_ptr;
	       if ((k = fldcmp(sfld_ptr, f1 + sub_len, f2 + sub_len)))
		  return ( k );
	    }
	 }
	 return ( 0 );
      case COMKEY:
	 for (kt_lc = size_kt - fld_ptr->fd_ptr,
					key_ptr = &key_table[fld_ptr->fd_ptr];
	      (--kt_lc >= 0) && (&field_table[key_ptr->kt_key] == fld_ptr);
	      ++key_ptr) {
	    i = key_ptr->kt_ptr;
	    if (( k = fldcmp(&field_table[key_ptr->kt_field], f1 + i, f2 + i) ))
	       return ( k );
	 }
	 return ( 0 );
   }
   return( result );
}


/* compare the INT variables
*/
int INTcmp( i1, i2 )
CONST char FAR *i1, FAR *i2;
{
   INT I1, I2;

   bytecpy( &I1, i1, sizeof(INT) );
   bytecpy( &I2, i2, sizeof(INT) );
   return( (int)( I1-I2 ) );
}


/* compare two DB_ADDR variables 
*/
int ADDRcmp( d1, d2 )
CONST DB_ADDR FAR *d1, FAR *d2;
{
   DB_ADDR a1, a2;
   FILE_NO f1, f2;
   F_ADDR r1, r2;

   bytecpy(&a1, d1, DB_ADDR_SIZE);
   bytecpy(&a2, d2, DB_ADDR_SIZE);

   f1 = (FILE_NO)(FILEMASK & (a1 >> FILESHIFT));
   f2 = (FILE_NO)(FILEMASK & (a2 >> FILESHIFT));
   r1 = ADDRMASK & a1;
   r2 = ADDRMASK & a2;
   
   if ( f1 == f2 ) {
      if ( r1 < r2 ) return( -1 );
      if ( r1 > r2 ) return( 1 );
      return( 0 );
   }
   else 
      return(f1 - f2);
}




/* check for empty DB_ADDR
*/
int
null_dba( db_addr )
CONST char FAR *db_addr;
{
   DB_ADDR dba;

   bytecpy( &dba, db_addr, DB_ADDR_SIZE );
   return( dba == NULL_DBA );
}


/* check for valid DB_ADDR
*/
int
check_dba( dba )
DB_ADDR dba;
{
   FILE_NO fno;
   F_ADDR  rno, last;

   fno = (FILE_NO)(FILEMASK & (dba >> FILESHIFT));
   rno = ADDRMASK & dba;

   /* Make sure page 0 has been read */
   if ( (last = dio_pznext(NUM2INT(fno, ft_offset))) <= 0 )
      return( db_status );

   if (((fno < 0) || (fno >= TABLE_SIZE(Size_ft))) ||
       ((rno <= 0L) || (rno >= last)))
      dberr(S_INVADDR);
   else
      db_status = S_OKAY;

   return( db_status );
}

#ifndef NO_COUNTRY
/* Compare two strings with sorting according to char-table
*/
static int ctblcmp(s, t, n)
CONST unsigned char FAR *s;  /* String 1 */
CONST unsigned char FAR *t;  /* String 2 */
int    n;   /* Max. String length */
{
   int x;
   unsigned char   f1, f2, x1, x2;

   /* Always return immediately if first difference found */
   for (; (n && *s && *t); n--) {
      if ( db_global.country_tbl.ptr[*s].sort_as1 )
	 f1 = db_global.country_tbl.ptr[*s].sort_as1;
      else f1 = *s;
      if ( db_global.country_tbl.ptr[*t].sort_as1 )
	 f2 = db_global.country_tbl.ptr[*t].sort_as1;
      else f2 = *t;

      if ((x = f1 - f2)) return(x);
 
      /* Check sort_as2-values if sort_as1-values are equal */
      /*----------------------------------------------------*/
      x1 = db_global.country_tbl.ptr[*s].sort_as2;
      x2 = db_global.country_tbl.ptr[*t].sort_as2;
      if ( x1 && x2 ) {  /* We have an entry for char. of both strings */
	 if (( x = x1 - x2 )) return(x);
      }
      else {
	 if ( x1 || x2 ) { /* Only sort_as2 value for one string */
	    if ( x1 ) {
               *t++;    /* Compare with next character in string 2 */
	       if ( db_global.country_tbl.ptr[*t].sort_as1 )
		  f2 = db_global.country_tbl.ptr[*t].sort_as1;
               else f2 = *t;
	       if (( x = x1 - f2 )) return(x);
	    }
	    if ( x2 ) {
               *s++;    /* Compare with next character in string 1 */
	       if ( db_global.country_tbl.ptr[*s].sort_as1 )
		  f1 = db_global.country_tbl.ptr[*s].sort_as1;
               else f1 = *s;
	       if (( x = f1 - x2 )) return(x);
	    }
	 }

         /* if both are equal compare sub_sort values */
         /*-------------------------------------------*/
	 if ((x = db_global.country_tbl.ptr[*s].sub_sort -
		db_global.country_tbl.ptr[*t].sub_sort))
            return(x);  
      }
      *s++;
      *t++;
   }
   if (n) {
      if (*s) return(1);
      if (*t) return(-1);
   }
   return(0);
}
#endif
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin libfcns.c */
