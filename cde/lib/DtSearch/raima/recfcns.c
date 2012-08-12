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
/* $XConsortium: recfcns.c /main/2 1996/05/09 04:13:59 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: r_chkfld
 *		r_clropt
 *		r_delrec
 *		r_gfld
 *		r_gmem
 *		r_gset
 *		r_pfld
 *		r_pmem
 *		r_pset
 *		r_setopt
 *		r_smem
 *		r_tstopt
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*---------------------------------------------------------------------------
   recfcns.c - db_VISTA Record Access/Manipulation Functions

   Copyright (C) 1984, 1985, 1986 by Raima Corporation.
---------------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  158 15-JUN-88 RSC passed new flag to key_bldcom.
  103 27-Jun-88 RSC Improve generation of single user version
      04-Aug-88 RTK MULTI_TASK changes
  310 10-Aug-88 RSC Cleanup function prototype.
  420 16-Aug-88 RTK Missing FAR pointer

*/
#include <stdio.h>
#include "vista.h"
#include "dbtype.h"

#define FALSE 0
#define TRUE 1

/* toggle for checking struct key modifications */
static int struct_key_chk = 1;

/* Check a field for permission to change it
*/
int
r_chkfld(field, fld_ptr, rec, data )
INT field;  /* field_table entry number */
FIELD_ENTRY FAR *fld_ptr; /* corresponds to field */
char FAR *rec;  /* pointer to record slot */
CONST char FAR *data; /* pointer to data area containing field contents */
{
   DB_ADDR dba;
   long fld;
   INT rn;
   char FAR *fptr, ckey[256];
   int i;
   FIELD_ENTRY FAR *sfld_ptr;
   RECORD_ENTRY FAR *rec_ptr;

   bytecpy(&rn, rec, sizeof(INT));
   rn &= ~RLBMASK; /* mask off rlb */
   if ( rn != NUM2EXT(fld_ptr->fd_rec, rt_offset) )
      return( dberr(S_INVFLD) );

   rec_ptr = &record_table[fld_ptr->fd_rec];
   fld = FLDMARK*rn + field - rec_ptr->rt_fields;

   if ( fld_ptr->fd_type == COMKEY ) {

      /* build compound key value. NOTE: cflag MUST be the same here as for
	 the call to key_bldcom in recwrite, which calls this function. */
      fptr = rec + rec_ptr->rt_data;
      key_bldcom(field, fptr, ckey, FALSE);
      fptr = ckey;
   }
   else
      fptr = rec + fld_ptr->fd_ptr;

   /* do nothing unless the new value is different */
   if (fldcmp(fld_ptr, data, fptr) == 0) 
      return( db_status = S_OKAY );

   /* if this is a unique key field, make sure the key does not already
      exist
   */
   if ( fld_ptr->fd_key == UNIQUE ) {
      dba = curr_rec;
      /* If the key field is not optional, or optional and stored */
      if ((!(fld_ptr->fd_flags & OPTKEYMASK) || r_tstopt(fld_ptr, rec)) &&
	  (d_keyfind(fld, data CURRTASK_PARM CURR_DB_PARM) == S_OKAY)) {
	 /* another record is already using this key value */
	 db_status = S_DUPLICATE;
      }
      curr_rec = dba;
      if ( db_status == S_DUPLICATE ) return( db_status );
   }
   /* if field is grouped, call r_chkfld for 1st entry of each sub-field */
   if ( fld_ptr->fd_type == GROUPED ) {
      for (i = field + 1, sfld_ptr = fld_ptr + 1;
	   (i < size_fd) && (sfld_ptr->fd_flags & STRUCTFLD);
	   ++i, ++sfld_ptr) {
	 fptr = (char *)data - (sfld_ptr->fd_ptr -
				       record_table[sfld_ptr->fd_rec].rt_data);
	 if (r_chkfld(i, sfld_ptr, rec, fptr) != S_OKAY)
	    return( db_status );
      }
   }
   return( db_status = S_OKAY );
}


/* Delete the current record
*/
int
r_delrec( rt, db_addr )
INT rt;
DB_ADDR db_addr;
{
   char FAR *rec;       /* ptr to record slot */
   char FAR *fptr;      /* field data ptr */
   char ckey[256];  /* compound key data */
#ifndef	 NO_TIMESTAMP
   ULONG timestamp;
#endif
   FILE_NO fno;
   F_ADDR rno;
   register int fld;
   RECORD_ENTRY FAR *rec_ptr;
   register FIELD_ENTRY FAR *fld_ptr;

   if ( dio_read( db_addr, (char FAR * FAR *)&rec, PGHOLD) != S_OKAY )
      return( db_status );

   rec_ptr = &record_table[rt];
   /* remove any key fields from the key files */
   for (fld = rec_ptr->rt_fields, fld_ptr = &field_table[fld];
	(fld < size_fd) && (fld_ptr->fd_rec == rt);
	++fld, ++fld_ptr) {
      if ( fld_ptr->fd_key != NOKEY ) {
	 if ( fld_ptr->fd_type == COMKEY ) {
	    key_bldcom(fld, rec + rec_ptr->rt_data, ckey, TRUE);
	    fptr = ckey;
	 }
	 else {
	    fptr = rec + fld_ptr->fd_ptr;
	 }
	 /* delete the key if it exists */
	 if ((!(fld_ptr->fd_flags & OPTKEYMASK) || r_tstopt(fld_ptr, rec)) &&
	     (key_delete(fld, fptr, db_addr) != S_OKAY))
	    return( db_status );
      }
   }
   fno = NUM2INT((FILE_NO)((db_addr >> FILESHIFT) & FILEMASK), ft_offset);
   rno = ADDRMASK & db_addr;
#ifndef	 NO_TIMESTAMP
   /* update timestamp, if necessary */
   if ( rec_ptr->rt_flags & TIMESTAMPED ) {
      timestamp = dio_pzgetts(fno);
      bytecpy( rec + RECCRTIME, &timestamp, sizeof(ULONG));
      bytecpy( rec + RECUPTIME, &timestamp, sizeof(ULONG));
   }
#endif
   dio_write(db_addr, NULL, PGFREE);

   /* place this record onto the delete chain */
   dio_pzdel(fno, rno);

   return( db_status );
}


/* Get data field from record
*/
int
r_gfld(fld_ptr, rec, data )
FIELD_ENTRY FAR *fld_ptr;
char FAR *rec;  /* pointer to record */
char FAR *data; /* pointer to data area to contain field contents */
{
   register int kt_lc;			/* loop control */
   INT rn;
   register FIELD_ENTRY FAR *kfld_ptr;
   register KEY_ENTRY FAR *key_ptr;

   bytecpy(&rn, rec, sizeof(INT));
   if ( rn < 0 )
      return( db_status = S_DELETED );

#ifndef SINGLE_USER
   if ( rn & RLBMASK ) {
      rn &= ~RLBMASK; /* mask off rlb */
      rlb_status = S_LOCKED;
   }
   else {
      rlb_status = S_UNLOCKED;
   }
#endif
#ifndef	 ONE_DB
   rn += curr_db_table->rt_offset;
#endif

   if ( fld_ptr->fd_rec != rn )
      return( dberr(S_INVFLD) );

   if ( fld_ptr->fd_type == KEY ) {
      /* clear compound key data area */
      byteset(data, '\0', fld_ptr->fd_len);

      /* copy each field of compound key to data area */
      for (kt_lc = size_kt - fld_ptr->fd_ptr,
					key_ptr = &key_table[fld_ptr->fd_ptr];
	   (--kt_lc >= 0) && (&field_table[key_ptr->kt_key] == fld_ptr);
	   ++key_ptr) {
	 kfld_ptr = &field_table[key_ptr->kt_field];
	 bytecpy(data + key_ptr->kt_ptr, rec + kfld_ptr->fd_ptr,
		 kfld_ptr->fd_len);
      }
   }
   else {
      bytecpy(data, rec + fld_ptr->fd_ptr, fld_ptr->fd_len);
   }
   return( db_status = S_OKAY );
}


/* Get member pointer from record
*/
int
r_gmem(set, rec, mem_addr )
int set;    /* set table entry number */
char FAR *rec;  /* pointer to record */
char FAR *mem_addr; /* pointer to member pointer */
{
   INT rt;
   register int mem, memtot;
   SET_ENTRY FAR *set_ptr;
   register MEMBER_ENTRY FAR *mem_ptr;

   /* search member list of set for record */
   set_ptr = &set_table[set];
   bytecpy(&rt, rec, sizeof(INT));
   rt &= ~RLBMASK;
   for (mem = set_ptr->st_members, memtot = mem + set_ptr->st_memtot,
						mem_ptr = &member_table[mem];
	mem < memtot;
	++mem, ++mem_ptr) {
      if (NUM2EXT(mem_ptr->mt_record, rt_offset) == rt) {
	 /* have found correct member record */
	 bytecpy(mem_addr, rec + mem_ptr->mt_mem_ptr, MEMPSIZE);
	 return( db_status = S_OKAY );
      }
   }
   /* this record is not member of set */
   return( dberr(S_INVMEM) );
}


/* Get set pointer from record
*/
int
r_gset(set, rec, setptr )
int set;      /* set table entry number */
char FAR *rec;    /* pointer to record */
char FAR *setptr; /* pointer to set pointer */
{
   INT rt;
   int len;
   SET_ENTRY FAR *set_ptr;

   set_ptr = &set_table[set];
   bytecpy(&rt, rec, sizeof(INT));
   if (NUM2EXT(set_ptr->st_own_rt, rt_offset) == (rt & ~RLBMASK)) {
#ifndef NO_TIMESTAMP
      if ( set_ptr->st_flags & TIMESTAMPED )
	 len = SETPSIZE;
      else
#endif
	 len = SETPSIZE - sizeof(ULONG);
      bytecpy(setptr, rec + set_ptr->st_own_ptr, len);
      return( db_status = S_OKAY );
   }
   return( dberr(S_INVOWN) );
}


/* Put data field into record
*/
int
r_pfld(field, fld_ptr, rec, data, db_addr )
INT field;  /* field_table entry number */
FIELD_ENTRY FAR *fld_ptr; /* corresponds to field */
char FAR *rec;  /* pointer to existing record */
CONST char FAR *data; /* pointer to data area containing new field contents */
DB_ADDR FAR *db_addr;
{
   DB_ADDR mdba, odba, dba;
   int set, sn;
   char memp[MEMPSIZE];
   register char FAR *fptr;
   register CONST char FAR *tfptr;
   register int s, i, strfld;
   register FIELD_ENTRY FAR *sfld_ptr;
   register SORT_ENTRY FAR *srt_ptr;
   DB_ADDR FAR *co_ptr, FAR *cm_ptr;

   db_status = S_OKAY;
   fptr = rec + fld_ptr->fd_ptr;

   /* do nothing unless the new value is different */
   if (fldcmp(fld_ptr, fptr, data) == 0) 
      return( db_status );

   bytecpy(&dba, db_addr, DB_ADDR_SIZE);

   /* if this is a key field, change the key file also */
   if ((fld_ptr->fd_key != NOKEY) &&
       (!(fld_ptr->fd_flags & OPTKEYMASK) || r_tstopt(fld_ptr, rec))) {
      /* delete the old key and insert the new one */
      if ( key_delete(field, fptr, dba) == S_OKAY ) {
	 if ( key_insert( field, data, dba ) != S_OKAY )
	    return( db_status );
      }
      else 
	 return( db_status == S_NOTFOUND? dberr(S_KEYERR): db_status );
   }
   /* if subfield of struct field, check to see if struct is a key */
   if ( struct_key_chk && fld_ptr->fd_flags & STRUCTFLD ) {
      for (strfld = field - 1, sfld_ptr = &field_table[strfld];
	   sfld_ptr->fd_type != GROUPED;
	    --strfld, --sfld_ptr)
	 ; /* find struct field */
      if ((sfld_ptr->fd_key != NOKEY) &&
	 /* make sure it is stored */
	  (!(sfld_ptr->fd_flags & OPTKEYMASK) || r_tstopt(sfld_ptr, rec))) {
	 /* delete the old struct key */
	 if (key_delete(strfld, rec + sfld_ptr->fd_ptr, dba) != S_OKAY)
	    return( db_status );
      }
      else strfld = -1;
   }
   else strfld = -1;

   /* copy data into record area */
   switch ( fld_ptr->fd_type ) {
      case CHARACTER:
	 if ( fld_ptr->fd_dim[1] )
	    bytecpy(fptr, data, fld_ptr->fd_len);
	 else if ( fld_ptr->fd_dim[0] )
	    strncpy(fptr, data, fld_ptr->fd_len);
	 else
	    *fptr = *data;
	 break;
      case GROUPED:
	 if (  ! fld_ptr->fd_dim[0] ) {
	    /* non-arrayed structure */
	    struct_key_chk = 0;
	    for (i = field + 1, sfld_ptr = fld_ptr + 1;
		 (i < size_fd) && (sfld_ptr->fd_flags & STRUCTFLD);
		 ++i, ++sfld_ptr) {
	       tfptr = data + sfld_ptr->fd_ptr - fld_ptr->fd_ptr;
	       if ( r_pfld(i, sfld_ptr, rec, tfptr, &dba) != S_OKAY )
		  break;
	    }
	    struct_key_chk = 1;
	    if ( db_status != S_OKAY ) return( db_status );
	    break;
	 }
	 /* arrayed struct fall-thru to a full field copy */
      default:
	 bytecpy(fptr, data, fld_ptr->fd_len);
   }
   /* if this field is part of an ordered set, reconnect */
   if (fld_ptr->fd_flags & SORTFLD) {
      for (s = 0, srt_ptr = sort_table; s < size_srt; ++s, ++srt_ptr) {
	 if ( srt_ptr->se_fld == field ) {
	    sn = srt_ptr->se_set;
	    if ( r_gmem( sn, rec, memp ) != S_OKAY ) return( db_status );
	    if ( ! null_dba(memp+MP_OWNER) ) {
	       /* save currency */
	       odba = *(co_ptr = &curr_own[sn]);
	       mdba = *(cm_ptr = &curr_mem[sn]);

	       /* set current owner and member to sorted set */
	       bytecpy(co_ptr, memp+MP_OWNER, DB_ADDR_SIZE);
	       *cm_ptr = dba;

	       /* calculate set constant */
	       set = NUM2EXT(sn + SETMARK, st_offset);

	       /* disconnect from prior order set and reconnect in new order */
	       d_discon(set CURRTASK_PARM CURR_DB_PARM);
	       d_connect(set CURRTASK_PARM CURR_DB_PARM);

	       /* reset currency */
	       *co_ptr = odba;
	       *cm_ptr = mdba;
	    }
	 }
      }
   }
   if ( strfld >= 0 ) {
      /* insert the new struct key */
      if ( key_insert( strfld, rec + sfld_ptr->fd_ptr, dba ) != S_OKAY )
	 return( db_status );
   }
   return( db_status );
}


/* Put member pointer into record
*/
int
r_pmem(set, rec, mem_addr )
int set;    /* set table entry number */
char FAR *rec;  /* pointer to record */
char FAR *mem_addr; /* pointer to member pointer */
{
   INT rt;
   register int mem, memtot;
   SET_ENTRY FAR *set_ptr;
   register MEMBER_ENTRY FAR *mem_ptr;

   /* search member list of set for record */
   set_ptr = &set_table[set];
   bytecpy(&rt, rec, sizeof(INT));
   rt &= ~RLBMASK;
   for (mem = set_ptr->st_members, memtot = mem + set_ptr->st_memtot,
						mem_ptr = &member_table[mem];
	mem < memtot;
	++mem, ++mem_ptr) {
      if (NUM2EXT(mem_ptr->mt_record, rt_offset) == rt) {
	 /* have found correct member record */
	 bytecpy(rec + mem_ptr->mt_mem_ptr, mem_addr, MEMPSIZE);
	 return( db_status = S_OKAY );
      }
   }
   /* this record is not member of set */
   return( dberr(S_INVMEM) );
}


/* Put set pointer into record
*/
int
r_pset(set, rec, setptr )
int set;       /* set table entry number */
char FAR *rec;     /* pointer to record */
char FAR *setptr;  /* pointer to set pointer */
{
   INT rt;
   int len;
   SET_ENTRY FAR *set_ptr;

   set_ptr = &set_table[set];
   bytecpy(&rt, rec, sizeof(INT));
   if (NUM2EXT(set_ptr->st_own_rt, rt_offset) == (rt & ~RLBMASK)) {
#ifndef NO_TIMESTAMP
      if ( set_ptr->st_flags & TIMESTAMPED )
	 len = SETPSIZE;
      else
#endif
	 len = SETPSIZE - sizeof(ULONG);
      bytecpy(rec + set_ptr->st_own_ptr, setptr, len);
      return( db_status = S_OKAY );
   }
   else {
      return( dberr(S_INVOWN) );
   }
}


/* Set the current set member from record
*/
int
r_smem( db_addr, set )
DB_ADDR FAR *db_addr;
INT set;
{
#ifndef	 NO_TIMESTAMP
   int nset;
#endif
   char mem[MEMPSIZE], FAR *ptr;
   DB_ADDR dba;

   bytecpy(&dba, db_addr, DB_ADDR_SIZE);

   /* make sure record is owned */
   if ((dio_read(dba, (char FAR * FAR *)&ptr, NOPGHOLD) != S_OKAY) ||
       (r_gmem(set, ptr, mem) != S_OKAY))
      return( db_status );

   if ( null_dba( mem+MP_OWNER ) ) return( dberr( S_NOTCON ) );

   bytecpy( &curr_own[set], mem+MP_OWNER, DB_ADDR_SIZE );

   /* ownership okay, set the member */
   curr_mem[set] = dba;
#ifndef	 NO_TIMESTAMP
   nset = NUM2EXT(set + SETMARK, st_offset);
   /* set timestamps */
   if ( db_tsrecs ) {
      d_utsco( nset, &co_time[set] CURRTASK_PARM CURR_DB_PARM );
      d_utscm( nset, &cm_time[set] CURRTASK_PARM CURR_DB_PARM );
   }
   if ( db_tssets )
      d_utscs( nset, &cs_time[set] CURRTASK_PARM CURR_DB_PARM );
#endif
   return( db_status = S_OKAY );
}

/* Set the optional key field "stored" bit */
int
r_setopt( fld_ptr, rec )
FIELD_ENTRY FAR *fld_ptr; /* field table entry of optional key */
char FAR *rec;	/* Pointer to record */
{
   int offset;	/* offset to the bit map */
   int keyndx;	/* index into bit map of this key */
   int byteno, bitno;	/* position within bit map of this key */

   /* calculate the position to the bit map */
   offset = (record_table[fld_ptr->fd_rec].rt_flags & TIMESTAMPED) ?
	       (RECHDRSIZE + 2*sizeof(LONG)) : RECHDRSIZE;

   /* extract the index into the bit map of this key */
   keyndx = (((fld_ptr->fd_flags & OPTKEYMASK) >> OPTKEYSHIFT) & OPTKEYNDX) - 1;
   if ( keyndx < 0 ) return( dberr(S_SYSERR) );

   /* determine which byte, and which bit within the byte */
   byteno = keyndx/BITS_PER_BYTE;
   bitno = keyndx - byteno*BITS_PER_BYTE;

   /* set the bit */
   rec[byteno + offset] |= 1 << (BITS_PER_BYTE - bitno - 1);

   return( db_status = S_OKAY );
}

/* Clear the optional key field "stored" bit */
int
r_clropt( fld_ptr, rec )
FIELD_ENTRY FAR *fld_ptr;	/* Field table entry of optional key */
char FAR *rec;	/* Pointer to record */
{
   int offset;	/* offset to the bit map */
   int keyndx;	/* index into bit map of this key */
   int byteno, bitno;	/* position within bit map of this key */

   /* calculate the position to the bit map */
   offset = (record_table[fld_ptr->fd_rec].rt_flags & TIMESTAMPED) ?
	       (RECHDRSIZE + 2*sizeof(LONG)) : RECHDRSIZE;

   /* extract the index into the bit map of this key */
   keyndx = (((fld_ptr->fd_flags & OPTKEYMASK) >> OPTKEYSHIFT) & OPTKEYNDX) - 1;
   if ( keyndx < 0 ) return( dberr(S_SYSERR) );

   /* determine which byte, and which bit within the byte */
   byteno = keyndx / BITS_PER_BYTE;
   bitno = keyndx - byteno*BITS_PER_BYTE;

   /* clear the bit */
   rec[byteno + offset] &= ~(1 << (BITS_PER_BYTE - bitno - 1));

   return( S_OKAY );
}

/* Test the optional key field "stored" bit */
int
r_tstopt( fld_ptr, rec )
FIELD_ENTRY FAR *fld_ptr;	/* Field table entry of optional key */
char FAR *rec;	/* Pointer to record */
{
   int offset;	/* offset to the bit map */
   int keyndx;	/* index into bit map of this key */
   int byteno, bitno;	/* position within bit map of this key */

   /* calculate the position to the bit map */
   offset = (record_table[fld_ptr->fd_rec].rt_flags & TIMESTAMPED) ?
	       (RECHDRSIZE + 2*sizeof(LONG)) : RECHDRSIZE;

   /* extract the index into the bit map of this key */
   keyndx = (((fld_ptr->fd_flags & OPTKEYMASK) >> OPTKEYSHIFT) & OPTKEYNDX) - 1;
   if ( keyndx < 0 ) return( dberr(S_SYSERR) );

   /* determine which byte, and which bit within the byte */
   byteno = keyndx / BITS_PER_BYTE;
   bitno = keyndx - byteno*BITS_PER_BYTE;

   /* extract the bit */
   if (rec[byteno + offset] & (1 << (BITS_PER_BYTE - bitno - 1)))
      return( db_status = S_DUPLICATE );
   return( db_status = S_OKAY );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin recfcns.c */
