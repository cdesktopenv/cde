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
/* $XConsortium: keyfcns.c /main/3 1996/08/12 12:34:31 cde-ibm $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: NODE_WIDTH
 *		Pi
 *		close_slots
 *		d_keyread
 *		delete
 *		expand
 *		key_bldcom
 *		key_boundary
 *		key_close
 *		key_cmpcpy
 *		key_delete
 *		key_found
 *		key_init
 *		key_insert
 *		key_locpos
 *		key_open
 *		key_reset
 *		key_scan
 *		keycmp
 *		node_search
 *		open_slots
 *		split_node
 *		split_root
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*---------------------------------------------------------------------------
   db_VISTA Key File/Field Manipulation Functions
   ----------------------------------------------
   An implementation of the B-tree indexing method  described in 
   "Sorting and Searching: The Art of Computer Programming, Vol III",
   Knuth, Donald E., Addison-Wesley, 1975. pp 473-480.

   A more detailed description of the generic algorithms can be found
   in "Fundamentals of Data Structures in Pascal", Horowitz & Sahni,
   Computer Science Press, 1984. pp 491-512.

   A tutorial survey of B-tree methods can be found in "The Ubiquitous
   B-Tree", Comer, D., ACM Computing Surveys, Vol 11, No. 2, June 1979.

   (C) Copyright 1985, 1986, 1987 by Raima Corp.

---------------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  158 15-JUN-88 RSC added compliment flag to key_bldcom
   42 15-JUN-88 RSC make keynext 4X faster!!
      04-Aug-88 RTK MULTI_TASK changes
  310 10-Aug-88 RSC cleanup function prototypes
  420 16-Sep-88 RTK Several missing FAR pointers
  420 16-Sep-88 RTK np_ptr not initialized in d_prkeys
  536 06-Jan-89 RSC pointer can go negative in node_search if 1st slot matches
      21-Feb-89 RSC add consistency check to key_locpos
      16-Mar-89 WLW reset table pointers in d_keyread

*/
#include <stdio.h>
#include "vista.h"
#include "dbtype.h"

/* Data definitions used for the B-tree algorithms */

/* node number of root */
#define ROOT_ADDR 1L

/* null node pointer */
#define NULL_NODE -1L

/* index file node structure */
typedef struct {
   LONG     last_chgd;    /* date/time of last change of this node */
   INT      used_slots;   /* current # of used slots in node */
   char     slots[1];     /* start of slot area */
} NODE;
/* Number of used slots plus orphan */
#define	 NODE_WIDTH(node)     ((node)->used_slots*slot_len + sizeof(F_ADDR))

/* last status value */
#define KEYEOF -1
#define KEYINIT 0
#define KEYFOUND 1
#define KEYNOTFOUND 2
#define KEYREPOS 3

/* Internal function prototypes */
static int node_search(P1(CONST char FAR *) Pi(DB_ADDR FAR *) 
				      Pi(NODE FAR *) Pi(int *) Pi(int *) 
				      Pi(F_ADDR *));
static int keycmp(P1(CONST char FAR *) Pi(KEY_SLOT FAR *) 
				    Pi(DB_ADDR FAR *));
static int expand(P1(CONST char FAR *) Pi(DB_ADDR) Pi(F_ADDR));
static int split_root(P1(NODE FAR *));
static int split_node(P1(F_ADDR) Pi(NODE FAR *));
static int delete(P0);
static void open_slots(P1(NODE FAR *) Pi(int) Pi(int));
static void close_slots(P1(NODE FAR *) Pi(int) Pi(int));
static void key_found(P1(DB_ADDR *));

#ifdef	 ONE_DB
#define	 prefix	  keyno
#endif

static KEY_INFO FAR *curkey;
static int key_len;
static int key_data;
static int slot_len;
static int max_slots;
static int mid_slot;
static int keyfile;
static INT fldno;
static FIELD_ENTRY FAR *cfld_ptr;
static INT keyno;
#ifndef	 ONE_DB
static INT prefix;
#endif
static int unique;


/* Open B-tree key field index processing
*/
int
key_open()
{
   register int fd_lc;			/* loop control */
   long t;               /* total keys thru level l */
   int  l;               /* level number */
   register int i;       /* field subscript */
   register FIELD_ENTRY FAR *fld_ptr;
   register KEY_INFO FAR *ki_ptr;
   FILE_ENTRY FAR *file_ptr;

   /*           child ptr      key number   */
   key_data = sizeof(F_ADDR) + sizeof(INT);

   /* count total number of key fields */
   no_of_keys = 0;
   for (fd_lc = size_fd - old_size_fd, fld_ptr = &field_table[old_size_fd];
	--fd_lc >= 0; ++fld_ptr) {
      if (fld_ptr->fd_key != NOKEY )
	 ++no_of_keys;
   }
   if ( no_of_keys ) {
      key_info =
	/* Macro references must be on one line for some compilers */ 
	(KEY_INFO FAR *)
	ALLOC(&db_global.Key_info, no_of_keys*sizeof(KEY_INFO), "key_info");
      if ( ! key_info )
	 return( dberr(S_NOMEMORY) );
      for (i = 0, fld_ptr = &field_table[old_size_fd]; 
	   i < size_fd; ++i, ++fld_ptr) {
	 if ( fld_ptr->fd_key != NOKEY ) {
	    ki_ptr = &key_info[fld_ptr->fd_keyno];
	    ki_ptr->level = 0;
	    ki_ptr->lstat = KEYINIT;
	    ki_ptr->fldno = i;
	    ki_ptr->keyfile = fld_ptr->fd_keyfile;
	    ki_ptr->dba = NULL_DBA;
	    file_ptr = &file_table[fld_ptr->fd_keyfile];
	    ki_ptr->keyval =
		/* Macro references must be on one line for some compilers */ 
		ALLOC(&ki_ptr->Keyval, file_ptr->ft_slsize, db_avname);
	    if ( ! ki_ptr->keyval )
	       return( dberr(S_NOMEMORY) );
	    MEM_UNLOCK(&ki_ptr->Keyval);
	    /* compute maximum possible levels */
	    for (t = file_ptr->ft_slots, l = 1; t < MAXRECORDS; ++l)
	       t *= file_ptr->ft_slots;
	    ki_ptr->max_lvls = ++l;
	    ki_ptr->node_path =
		(NODE_PATH FAR *)
		ALLOC(&ki_ptr->Node_path, l*sizeof(NODE_PATH), db_avname);
	    if ( ! ki_ptr->node_path )
	       return( dberr(S_NOMEMORY) );
	    byteset(ki_ptr->node_path, 0, l*sizeof(NODE_PATH));
	    MEM_UNLOCK(&ki_ptr->Node_path);
	 }
      }
   }
   return( db_status = S_OKAY );
}



/* Close key field processing
*/
void key_close()
{
   register int k;
   KEY_INFO FAR *ki_ptr;

   if ( key_info ) {
      /* free memory allocated for key functions */
      for (k = 0, ki_ptr = key_info; k < no_of_keys; ++k, ++ki_ptr) {
	 MEM_UNLOCK(&ki_ptr->Node_path);
	 FREE(&ki_ptr->Node_path);
	 MEM_UNLOCK(&ki_ptr->Keyval);
	 FREE(&ki_ptr->Keyval);
      }
      MEM_UNLOCK(&db_global.Key_info);
      FREE(&db_global.Key_info);
   }
}


/* Initialize key function operation
*/
int
key_init(field )
int field;  /* field number to be processed */
{
   FIELD_ENTRY FAR *fld_ptr;
   FILE_ENTRY FAR *file_ptr;

   fld_ptr = &field_table[field];
   if ( fld_ptr->fd_key == NOKEY )
      return( dberr(S_NOTKEY) );

   fldno     = field;
   cfld_ptr  = fld_ptr;
   keyno     = fld_ptr->fd_keyno;
#ifndef	 ONE_DB
   prefix    = keyno - curr_db_table->key_offset;
#endif
   key_len   = fld_ptr->fd_len;
   keyfile   = fld_ptr->fd_keyfile;
   file_ptr  = &file_table[keyfile];
   slot_len  = file_ptr->ft_slsize;
   max_slots = file_ptr->ft_slots;
   mid_slot  = max_slots/2;
   curkey    = &key_info[keyno];
   unique    = (fld_ptr->fd_key == UNIQUE);
   dio_setdef( keyfile );

   return( db_status = S_OKAY );
}



/* Reset key_info last status to reposition keys on file "fno" 
*/
int
key_reset(fno )
FILE_NO fno;
{
   register int i;
   register KEY_INFO FAR *ki_ptr;

   for (i = 0, ki_ptr = key_info; i < no_of_keys; ++i, ++ki_ptr) {
      if (((fno == size_ft) || (ki_ptr->keyfile == fno)) && 
	  ((ki_ptr->lstat == KEYFOUND) || (ki_ptr->lstat == KEYNOTFOUND)))
	 ki_ptr->lstat = KEYREPOS;
   }
   return( db_status = S_OKAY );
}



/* Locate proper key position on B-tree
*/
int
key_locpos(key_val, dba)
CONST char FAR *key_val; /* key search value */
DB_ADDR FAR *dba;        /* database address of located key */
{
   NODE FAR *node;   /* pointer to current node */
   F_ADDR child;     /* page number of child node */
   F_ADDR pg;        /* page number of current node */
   int stat;         /* saves node search status */
   int slot, slot_pos;
   int match_lvl;    /* lowest level with duplicate key */
   NODE_PATH FAR *np_ptr;
   char FAR *node_slot_ptr;

   match_lvl = -1;
   MEM_LOCK(&curkey->Node_path);
   for (curkey->level = 0, np_ptr = curkey->node_path, pg = ROOT_ADDR;
	TRUE;
	++curkey->level, ++np_ptr, pg = child) {
      /* read in next node */
      if ( dio_get(pg, (char FAR * FAR *)&node, NOPGHOLD) != S_OKAY ) {
         MEM_UNLOCK(&curkey->Node_path);
	 return( db_status );
      }

      np_ptr->node = pg;
      if ( curkey->level == 0 && node->used_slots == 0 ) {
	 np_ptr->slot = 0;
	 curkey->lstat = KEYEOF;
         MEM_UNLOCK(&curkey->Node_path);
	 return( db_status = S_NOTFOUND );
      }
      else if (node->used_slots == 0)
	 return( dberr( S_SYSERR ) );	/* non-root nodes can't be empty */

      /* search node for key */
      stat = node_search(key_val, ((*dba == NULL_DBA) ? NULL : dba), node,
			 &slot, &slot_pos, &child);
      np_ptr->slot = slot;

      node_slot_ptr = &node->slots[slot_pos];
      if ( stat == S_OKAY ) {
	 if ( unique || *dba != NULL_DBA )
	    break;

	 /* mark level as having matching key */
	 match_lvl = curkey->level;

	 /* save the key value */
	 bytecpy(&key_type, node_slot_ptr, slot_len);
      }
      /* check for end of search */
      if ( child == NULL_NODE )
	 break;
   }
   if ( match_lvl >= 0 ) {
      /* set to lowest duplicate key */
      curkey->level = match_lvl;
      db_status = S_OKAY;
      curkey->lstat = KEYFOUND;
   }
   else if ( stat == S_OKAY ) {
      bytecpy(&key_type, node_slot_ptr, slot_len);
      db_status = S_OKAY;
      curkey->lstat = KEYFOUND;
   }
   else {
      /* key not found - save key at positioned slot */
      if ( np_ptr->slot > 0 )
	 node_slot_ptr -= slot_len;
      bytecpy(&key_type, node_slot_ptr, slot_len);
      curkey->lstat = KEYNOTFOUND;
      db_status = S_NOTFOUND;
   }
   MEM_UNLOCK(&curkey->Node_path);
   MEM_LOCK(&curkey->Keyval);
   /* save key value and database address for possible repositioning */
   bytecpy(curkey->keyval, key_type.ks.data, key_len);
   MEM_UNLOCK(&curkey->Keyval);
   bytecpy(&curkey->dba, &key_type.ks.data[key_len], sizeof(DB_ADDR));

   /* return database address for d_keyfind */
   if ( *dba == NULL_DBA )
      bytecpy(dba, &curkey->dba, sizeof(DB_ADDR));
   
   return( db_status );
}



/* Search node for key value
*/
static int node_search(key_val, dba, node, slotno, slot_offset, 
					 child)
CONST char FAR *key_val; /* key being searched */
DB_ADDR FAR *dba;    /* database address included in comparison if not null */
NODE    FAR *node;   /* node being searched */
int     *slotno; /* slot number of key position in node */
int     *slot_offset; /* slot position offset */
F_ADDR  *child;  /* child ptr of located key */
{
   register int cmp, i, l, u, slot_pos;
   char FAR *node_slot_ptr;

   /* perform binary search on node */
   l = 0;
   u = node->used_slots - 1;
   while ( u >= l ) {
      i = (l + u)/2;
      node_slot_ptr = &node->slots[slot_pos = i*slot_len];
      if ( (cmp = keycmp(key_val, (KEY_SLOT FAR *)node_slot_ptr, dba)) < 0 )
	 u = i - 1;
      else if ( cmp > 0 )
	 l = i + 1;
      else if ( i && !unique && !dba ) {
	 /* backup to lowest duplicate in node */
	 while (keycmp(key_val, (KEY_SLOT FAR *)(node_slot_ptr -= slot_len),
		       dba) == 0) {
	    slot_pos -= slot_len;
	    if (--i == 0) goto have_slot;
	 }
	 node_slot_ptr += slot_len;
	 goto have_slot;
      }
      else
	 goto have_slot;
   }
have_slot:
   if ( cmp > 0 ) { /* always return next highest position */
      ++i;
      node_slot_ptr += slot_len;
      slot_pos += slot_len;
   }
   /* return child pointer from located slot */
   bytecpy(child, node_slot_ptr, sizeof(F_ADDR));

   /* return slot number */
   *slotno = i;
   *slot_offset = slot_pos;

   return( db_status = (cmp == 0 ? S_OKAY : S_NOTFOUND) );
}



/* Compare key value
*/
static int keycmp(key_val, slot, dba)
CONST char FAR *key_val;  /* key value */
KEY_SLOT FAR *slot;    /* pointer to key slot to be compared */
DB_ADDR FAR *dba;     /* database address included in comparison if not null */
{
/* 
   returns < 0 if key_val < slot
	   > 0 if key_val > slot
	   = 0 if key_val == slot
*/
   register int cmp;

   if (((cmp = INTcmp((char FAR *)&prefix, (char FAR *)&slot->keyno)) == 0) &&
       ((cmp = fldcmp(cfld_ptr, key_val, slot->data)) == 0) &&
       dba)
      cmp = ADDRcmp(dba, (DB_ADDR FAR *)&slot->data[key_len]);

   return( cmp );
}


/* Scan thru key field
*/
int
key_scan(fcn, dba )
int fcn;       /* next or prev */
DB_ADDR *dba;  /* db address of scanned record */
{
   F_ADDR child;
   NODE FAR *node;
   NODE_PATH FAR *np_ptr;
   char FAR *node_slot_ptr;

   /* locate next key on file */
   switch ( curkey->lstat ) {
      case KEYINIT:
      case KEYEOF:
	 return( key_boundary(((fcn == KEYNEXT) ? KEYFRST : KEYLAST), dba) );
      case KEYREPOS:
         MEM_LOCK(&curkey->Keyval);
	 key_locpos(curkey->keyval, &curkey->dba);
         MEM_UNLOCK(&curkey->Keyval);
	 if (db_status != S_OKAY)
	    break;
	 /* PASS THROUGH */
      case KEYFOUND:
         MEM_LOCK(&curkey->Node_path);
	 if (fcn == KEYNEXT)
	    ++curkey->node_path[curkey->level].slot;
	 MEM_UNLOCK(&curkey->Node_path);
   }
   MEM_LOCK(&curkey->Node_path);
   np_ptr = &curkey->node_path[curkey->level];
   if (dio_get(np_ptr->node, (char FAR * FAR *)&node, NOPGHOLD) != S_OKAY) {
      MEM_UNLOCK(&curkey->Node_path);
      return( db_status );
   }
   node_slot_ptr = &node->slots[np_ptr->slot*slot_len];
   bytecpy(&child, node_slot_ptr, sizeof(F_ADDR));
   if (child == NULL_NODE) {
      if (fcn == KEYPREV) {
	 --np_ptr->slot;
	 node_slot_ptr -= slot_len;
      }
      while (((fcn == KEYNEXT) && (np_ptr->slot >= node->used_slots)) ||
	     ((fcn == KEYPREV) && (np_ptr->slot < 0))) {
	 if (curkey->level <= 0) {
	    /* return end of file */
	    curkey->lstat = KEYEOF;
	    MEM_UNLOCK(&curkey->Node_path);
	    return( db_status = S_NOTFOUND );
	 }
	 --curkey->level;
	 node_slot_ptr = NULL;
	 if (dio_get((--np_ptr)->node, (char FAR * FAR *)&node,
		     NOPGHOLD) != S_OKAY) {
	    MEM_UNLOCK(&curkey->Node_path);
	    return( db_status );
	 }
	 if (fcn == KEYPREV)
	    --np_ptr->slot;
      }
      if (node_slot_ptr == NULL)
	 node_slot_ptr = &node->slots[np_ptr->slot*slot_len];
   }
   else do { /* move down to leaf node */
      if ( dio_get(child, (char FAR * FAR *)&node, NOPGHOLD) != S_OKAY ) {
	 MEM_UNLOCK(&curkey->Node_path);
	 return( db_status );
      }
      ++curkey->level;
      (++np_ptr)->node = child;
      if (fcn == KEYNEXT) {
	 np_ptr->slot = 0;
	 node_slot_ptr = node->slots;
      }
      else {
	 np_ptr->slot = node->used_slots;
	 node_slot_ptr = &node->slots[np_ptr->slot*slot_len];
      }
      bytecpy(&child, node_slot_ptr, sizeof(F_ADDR));
   } while ( child != NULL_NODE ); 

   if (np_ptr->slot == node->used_slots) {
      --np_ptr->slot;
      node_slot_ptr -= slot_len;
   }

   bytecpy(&key_type, node_slot_ptr, slot_len);
   if (key_type.ks.keyno == prefix)
      key_found(dba);
   else {
      curkey->lstat = KEYEOF;
      db_status = S_NOTFOUND;
   }
   MEM_UNLOCK(&curkey->Node_path);
   return( db_status );
}


/* Key has been found.  Save appropriate information
*/
static void key_found(dba)
DB_ADDR *dba;
{
   MEM_LOCK(&curkey->Keyval);
   /* save key value and database address for possible repositioning */
   bytecpy(curkey->keyval, key_type.ks.data, key_len);
   MEM_UNLOCK(&curkey->Keyval);
   bytecpy(&curkey->dba, &key_type.ks.data[key_len], sizeof(DB_ADDR));

   /* return found db addr */
   *dba = curkey->dba;

   curkey->lstat = KEYFOUND;		/*[42] FOUND a match */
   db_status = S_OKAY;
}


/* Find key boundary
*/
int
key_boundary(fcn, dba )
int  fcn;     /* KEYFRST or KEYLAST */
DB_ADDR *dba; /* to get dba of first or last key */
{
   F_ADDR pg;         /* node number */
   NODE FAR *node;        /* pointer to node contents in cache */
   int cmp;           /* keycmp result */
   int match_lvl;     /* lowest level containing matched key */
   register int lvl;  /* node_path level variable */
   register int slot; /* slot position in node */
   register NODE_PATH FAR *np_ptr;
   register char FAR *node_slot_ptr;

   if ( fcn == KEYFIND ) {
      curkey->lstat = KEYINIT;
      return( db_status = S_OKAY );
   }
   curkey->lstat = KEYNOTFOUND;

   /* traverse B-tree for first or last key with specified prefix */
   match_lvl = -1;
   pg = ROOT_ADDR;
   MEM_LOCK(&curkey->Node_path);
   for (lvl = 0; TRUE; ++lvl) {
      /* read next node */
      if ( dio_get(pg, (char FAR * FAR *)&node, NOPGHOLD) != S_OKAY ) {
	 MEM_UNLOCK(&curkey->Node_path);
	 return( db_status );
      }
      if ( node->used_slots == 0 ) {
	 /* must not be anything on file */
	 curkey->lstat = KEYEOF;
	 MEM_UNLOCK(&curkey->Node_path);
	 return( db_status = S_NOTFOUND );
      }
      if ( fcn == KEYFRST ) {
	 for (slot = 0, node_slot_ptr = node->slots;
	      slot < node->used_slots;
	      ++slot, node_slot_ptr += slot_len) {
	    if ((cmp = INTcmp((char FAR *)&prefix,
			   (char FAR *)(node_slot_ptr + sizeof(F_ADDR)))) <= 0)
	       break;
	 }
      }
      else { /* KEYLAST */
	 for (slot = node->used_slots - 1,
				 node_slot_ptr = &node->slots[slot*slot_len];
	      slot >= 0;
	      --slot, node_slot_ptr -= slot_len) {
	    if ((cmp = INTcmp((char FAR *)&prefix,
			   (char FAR *)(node_slot_ptr + sizeof(F_ADDR)))) >= 0)
	       break;
	 }
      }
      /* save node path position */
      np_ptr = &curkey->node_path[lvl];
      np_ptr->node = pg;
      np_ptr->slot = slot;

      if ( cmp == 0 ) {
	 /* save matched level & key value */
	 match_lvl = lvl;
	 bytecpy(&key_type, node_slot_ptr, slot_len);
      }
      /* fetch appropriate child pointer */
      if (fcn == KEYLAST)
	 node_slot_ptr += slot_len;
      bytecpy(&pg, node_slot_ptr, sizeof(F_ADDR));

      if ( pg == NULL_NODE ) break;
   }
   if ( match_lvl >= 0 ) {
      curkey->level = match_lvl;
      key_found(dba);
      curkey->lstat = KEYREPOS;		/*[42] Need to reposition */
   }
   else {
      /* no keys on file with requested prefix */
      curkey->level = 0;
      curkey->lstat = KEYEOF;
      db_status = S_NOTFOUND;
   }
   MEM_UNLOCK(&curkey->Node_path);
   return( db_status );
}


/* Insert key field into B-tree
*/
int
key_insert(fld, key_val, dba )
int fld;      /* key field number */
CONST char FAR *key_val; /* key value */
DB_ADDR dba;  /* record's database address */
{
   int stat;

   /* initialize key function operation */
   if ( key_init(fld) != S_OKAY ) return( db_status );

   /* locate insertion point */
   if ( key_locpos(key_val, &dba) == S_NOTFOUND ) {
      /* expand node to include key */
      if ( (stat = expand(key_val, dba, NULL_NODE)) == S_OKAY ) {
	 /* save key value and database address for possible repositioning */
         MEM_LOCK(&curkey->Keyval);
	 bytecpy(curkey->keyval, key_val, key_len);
         MEM_UNLOCK(&curkey->Keyval);
	 curkey->dba = dba;

	 /* reset key position */
	 key_reset(curkey->keyfile);
      }
      db_status = stat;
   }
   else if ( db_status == S_OKAY ) 
      dberr(S_SYSERR);
   
   return( db_status );
}



/* Expand node for new key
*/
static int expand(key_val, dba, brother )
CONST char FAR *key_val;    /* key value */
DB_ADDR dba;     /* record's database address */
F_ADDR brother;  /* page number of brother node */
{
   F_ADDR pg;
   NODE FAR *node;
   NODE_PATH FAR *np_ptr;
   int slot_pos;
   register char FAR *node_slot_ptr;

   MEM_LOCK(&curkey->Node_path);
   np_ptr = &curkey->node_path[curkey->level];

   if (dio_get(pg = np_ptr->node, (char FAR * FAR *)&node, PGHOLD) != S_OKAY) {
      MEM_UNLOCK(&curkey->Node_path);
      return( db_status );
   }
   if ( node->used_slots >= max_slots ) {
      MEM_UNLOCK(&curkey->Node_path);
      return( dberr(S_KEYERR) );
   }
   node_slot_ptr = &node->slots[slot_pos = np_ptr->slot*slot_len];
   open_slots(node, slot_pos, 1);

   /* copy brother into opened slot's child pointer */
   bytecpy(node_slot_ptr + slot_len, &brother, sizeof(F_ADDR));
   /* copy keyno into current slot */
   bytecpy(node_slot_ptr + sizeof(F_ADDR), &prefix, sizeof(INT));

   node_slot_ptr += key_data;
   /* clear slot data area to zeros */
   byteset(node_slot_ptr, 0, slot_len - key_data);

   /* copy keyval into current slot */
   if ( cfld_ptr->fd_type == CHARACTER && cfld_ptr->fd_dim[1] == 0 )
      strncpy(node_slot_ptr, key_val, key_len);
   else
      bytecpy(node_slot_ptr, key_val, key_len);

   /* copy database address into current slot */
   bytecpy(node_slot_ptr + key_len, &dba, sizeof(DB_ADDR));

   if ( node->used_slots == max_slots ) {
      if ( pg == ROOT_ADDR )
	 split_root(node);
      else
	 split_node(pg, node);
   }
   else
      dio_touch(pg);

   MEM_UNLOCK(&curkey->Node_path);
   return( db_status );
}



/* Split node into two nodes
*/
static int split_node(l_pg, l_node )
F_ADDR l_pg;  /* left node's page number */
NODE FAR *l_node; /* left node buffer */
{
   F_ADDR r_pg;
   NODE FAR *r_node;
   char key_val[256];
   DB_ADDR dba;
   char FAR *l_node_slot_ptr;

   /* extract middle key */
   l_node_slot_ptr = &l_node->slots[mid_slot*slot_len];
   bytecpy(&prefix, l_node_slot_ptr + sizeof(F_ADDR), sizeof(INT));
#ifndef	 ONE_DB
   keyno = prefix + curr_db_table->key_offset;
#endif
   fldno = key_info[keyno].fldno;
   cfld_ptr = &field_table[fldno];
   key_len = cfld_ptr->fd_len;
   bytecpy(key_val, l_node_slot_ptr + key_data, key_len);
   bytecpy(&dba, l_node_slot_ptr + key_data + key_len, sizeof(DB_ADDR));

   /* divide left node */
   l_node->used_slots = mid_slot;

   /* allocate new right node */
   if ((dio_pzalloc(keyfile, &r_pg) != S_OKAY) ||
       (dio_get(r_pg, (char FAR * FAR *)&r_node, PGHOLD) != S_OKAY))
      return( db_status );

   /* copy slots from left node at slot mid_slot+1 into right node */
   r_node->used_slots = max_slots - (mid_slot + 1);
   l_node_slot_ptr += slot_len;
   bytecpy(r_node->slots, l_node_slot_ptr, NODE_WIDTH(r_node));

   dio_touch(l_pg);
   dio_touch(r_pg);

   --curkey->level;

   /* expand parent slot to include middle key and new right node ptr */
   return( expand(key_val, dba, r_pg) );
}


/* Split root node
*/
static int split_root(node )
NODE FAR *node;
{
   F_ADDR l_pg, r_pg;
   NODE FAR *l_node, FAR *r_node;
   register int slot_pos;
   char FAR *node_slot_ptr;

   /* allocate two new nodes */
   if ((dio_pzalloc(keyfile, &l_pg) != S_OKAY) ||
       (dio_pzalloc(keyfile, &r_pg) != S_OKAY) ||
       (dio_get(l_pg, (char FAR * FAR *)&l_node, PGHOLD) != S_OKAY) ||
       (dio_get(r_pg, (char FAR * FAR *)&r_node, PGHOLD) != S_OKAY))
      return( db_status );

   /* copy 0..max_slots/2-1 keys from root into left node */
   l_node->used_slots = mid_slot;
   slot_pos = mid_slot*slot_len;
   bytecpy(l_node->slots, node->slots, NODE_WIDTH(l_node));

   /* copy max_slots/2+1..max_slots from root into right node */
   r_node->used_slots = max_slots - (mid_slot + 1);
   node_slot_ptr = &node->slots[slot_pos += slot_len];
   bytecpy(r_node->slots, node_slot_ptr, NODE_WIDTH(r_node));

   /* copy mid_slot into slot[0] of root */
   bytecpy(node->slots, node_slot_ptr - slot_len, slot_len);

   /* copy left page number into p[0] of root */
   bytecpy(node->slots, &l_pg, sizeof(F_ADDR));

   /* copy right page number into p[1] of root */
   bytecpy(&node->slots[slot_len], &r_pg, sizeof(F_ADDR));

   /* reset number of used slots in root */
   node->used_slots = 1;

   dio_touch(l_pg);
   dio_touch(r_pg);
   dio_touch(ROOT_ADDR);

   return( db_status );
}



/* Delete key from B-tree
*/
int
key_delete(fld, key_val, dba )
int fld;
char CONST FAR *key_val;
DB_ADDR dba;
{
   int stat;

   /* initialize key function operation */
   if ( key_init(fld) != S_OKAY ) return( db_status );

   /* locate key to be deleted */
   if ((stat = key_locpos(key_val, &dba)) == S_OKAY) {
      if ( (stat = delete()) == S_OKAY ) {
	 /* save key value and database address for possible repositioning */
         MEM_LOCK(&curkey->Keyval);
	 bytecpy(curkey->keyval, key_val, key_len);
         MEM_UNLOCK(&curkey->Keyval);
	 curkey->dba = dba;

	 /* reset key position */
	 key_reset(curkey->keyfile);
      }
   }
   return( db_status = stat );
}



/* Delete key at current node_path position
*/
static int delete()
{
   F_ADDR pg, p_pg, l_pg, r_pg;
   NODE FAR *node;
   NODE FAR *p_node;
   NODE FAR *l_node;
   NODE FAR *r_node;
   int amt, slot_pos;
   NODE_PATH FAR *np_ptr;
   char FAR *node_slot_ptr;
   char FAR *p_node_slot_ptr;
   char FAR *l_node_slot_ptr;
   char FAR *r_node_slot_ptr;

   MEM_LOCK(&curkey->Node_path);
   np_ptr = &curkey->node_path[curkey->level];

   /* read node containing key to be deleted */
   if (dio_get(pg = np_ptr->node, (char FAR * FAR *)&node, PGHOLD) != S_OKAY) {
      MEM_UNLOCK(&curkey->Node_path);
      return( db_status );
   }
   /* copy pointer to right sub-tree */
   slot_pos = np_ptr->slot*slot_len;
   node_slot_ptr = &node->slots[slot_pos];
   bytecpy(&r_pg, node_slot_ptr + slot_len, sizeof(F_ADDR));

   if ( r_pg != NULL_NODE ) {
      /* find leftmost descendent of right sub-tree */
      ++np_ptr->slot;
      do {
	 if ( dio_get(r_pg, (char FAR * FAR *)&r_node, NOPGHOLD) != S_OKAY ) {
	    MEM_UNLOCK(&curkey->Node_path);
	    return( db_status );
	 }
	 ++curkey->level;
	 ++np_ptr;
	 np_ptr->node = r_pg;
	 np_ptr->slot = 0;
	 bytecpy(&r_pg, r_node->slots, sizeof(F_ADDR));
      } while ( r_pg != NULL_NODE );

      /* copy key from leaf into node */
      node_slot_ptr += sizeof(F_ADDR);
      r_node_slot_ptr = &r_node->slots[sizeof(F_ADDR)];
      bytecpy(node_slot_ptr, r_node_slot_ptr, slot_len - sizeof(F_ADDR));
      dio_touch(pg);
      
      /* set up to delete key from leaf */
      /* (this is more efficient than a recursive call) */
      slot_pos = 0;
      node_slot_ptr = node->slots;
      if (dio_get(pg = np_ptr->node, (char FAR * FAR *)&node, PGHOLD) != S_OKAY) {
	 MEM_UNLOCK(&curkey->Node_path);
	 return( db_status );
      }
   }
shrink: /* delete key from leaf (shrink node ) */
   close_slots(node, slot_pos, 1);

   /* check if necessary to adjust nodes */
   if ((curkey->level > 0) && (node->used_slots < mid_slot)) {
      /* read in parent node */
      if (dio_get(p_pg = (np_ptr - 1)->node, (char FAR * FAR *)&p_node,
		  PGHOLD) != S_OKAY) {
	 MEM_UNLOCK(&curkey->Node_path);
	 return( db_status );
      }
      slot_pos = (np_ptr - 1)->slot*slot_len;
      node_slot_ptr = &node->slots[slot_pos];
      if ((np_ptr - 1)->slot == p_node->used_slots ) {
	 /* pg is right node */
	 r_pg = pg;
	 r_node = node;

	 /* parent slot position should bisect left & right nodes */
	 --(np_ptr - 1)->slot;
	 slot_pos -= slot_len;

	 /* read left node */
	 p_node_slot_ptr = &p_node->slots[slot_pos];
	 bytecpy(&l_pg, p_node_slot_ptr, sizeof(F_ADDR));
	 if ( dio_get(l_pg, (char FAR * FAR *)&l_node, PGHOLD) != S_OKAY ) {
	    MEM_UNLOCK(&curkey->Node_path);
	    return( db_status );
	 }
      }
      else {
	 /* pg is left node */
	 l_pg = pg;
	 l_node = node;

	 /* read right node */
	 p_node_slot_ptr = &p_node->slots[slot_pos + slot_len];
	 bytecpy(&r_pg, p_node_slot_ptr, sizeof(F_ADDR));
	 if (dio_get(r_pg, (char FAR * FAR *)&r_node, PGHOLD) != S_OKAY) {
	    MEM_UNLOCK(&curkey->Node_path);
	    return( db_status );
	 }
      }
      if ((l_node->used_slots + r_node->used_slots + 1) < max_slots) {
	 /* combine left and right nodes */
	 if ((curkey->level == 1) && (p_node->used_slots == 1)) {
	    /* shrink down to root */
	    /* copy right node data into root */
	    p_node_slot_ptr = &p_node->slots[slot_len];
	    bytecpy(p_node_slot_ptr, r_node->slots, NODE_WIDTH(r_node));
	    p_node->used_slots = r_node->used_slots + 1;
	    r_node->used_slots = 0;
	    dio_touch(r_pg);

	    /* copy left node data into root */
	    open_slots(p_node, 0, l_node->used_slots);
	    bytecpy(p_node->slots, l_node->slots, NODE_WIDTH(l_node));
	    l_node->used_slots = 0;
	    dio_touch(l_pg);

	    dio_touch(p_pg);

	    /* free node pages */
	    dio_pzdel(keyfile, r_pg);
	    dio_pzdel(keyfile, l_pg);
            MEM_UNLOCK(&curkey->Node_path);
	    return( db_status );
	 }
	 /* open space for l_node->used_slots+1 slots in right node */
	 open_slots(r_node, 0, l_node->used_slots + 1);

	 /* move left node slots into right node */
	 amt = NODE_WIDTH(l_node);
	 r_node_slot_ptr = r_node->slots;
	 bytecpy(r_node_slot_ptr, l_node->slots, amt);

	 /* move parent slot data into right node */
	 r_node_slot_ptr += amt;
	 p_node_slot_ptr = &p_node->slots[slot_pos + sizeof(F_ADDR)];
	 bytecpy(r_node_slot_ptr, p_node_slot_ptr, slot_len - sizeof(F_ADDR));

	 dio_touch(r_pg);
	 dio_touch(l_pg);

	 /* delete left node */
	 l_node->used_slots = 0;
	 if ( dio_pzdel(keyfile, l_pg) != S_OKAY ) {
	    MEM_UNLOCK(&curkey->Node_path);
	    return( db_status );
	 }
	 /* decrement level & make parent node current node */
	 --curkey->level;
	 --np_ptr;
	 pg = p_pg;
	 node = p_node;
	 goto shrink;  /* delete slot from parent */
      }
      /* acquire needed key from sibling */
      if ((l_node->used_slots + 1) < r_node->used_slots) {
	 /* get key from right node */

	 /* move parent slot to end of left node */
	 l_node_slot_ptr = &l_node->slots[NODE_WIDTH(l_node)];
	 p_node_slot_ptr = &p_node->slots[slot_pos + sizeof(F_ADDR)];
	 bytecpy(l_node_slot_ptr, p_node_slot_ptr, slot_len - sizeof(F_ADDR));

	 ++l_node->used_slots;

	 /* copy slot 0 child from right node to left node orphan */
	 l_node_slot_ptr += slot_len - sizeof(F_ADDR);
	 r_node_slot_ptr = r_node->slots;
	 bytecpy(l_node_slot_ptr, r_node_slot_ptr, sizeof(F_ADDR));

	 /* move slot 0 of right node to parent */
	 r_node_slot_ptr += sizeof(F_ADDR);
	 bytecpy(p_node_slot_ptr, r_node_slot_ptr, slot_len - sizeof(F_ADDR));

	 /* delete slot 0 from right node */
	 close_slots(r_node, 0, 1);
      }
      else if ((r_node->used_slots + 1) < l_node->used_slots) {
	 /* get key from left node */

	 /* open one slot at front of right node */
	 open_slots(r_node, 0, 1);

	 /* move parent slot to slot 0 of right node */
	 r_node_slot_ptr = &r_node->slots[sizeof(F_ADDR)];
	 p_node_slot_ptr = &p_node->slots[slot_pos + sizeof(F_ADDR)];
	 bytecpy(r_node_slot_ptr, p_node_slot_ptr, slot_len - sizeof(F_ADDR));

	 /* move end slot of left node to parent */
	 l_node_slot_ptr = &l_node->slots[NODE_WIDTH(l_node) - slot_len];
	 bytecpy(p_node_slot_ptr, l_node_slot_ptr, slot_len - sizeof(F_ADDR));

	 /* move left orphan to child of slot 0 in right node */
	 l_node_slot_ptr += slot_len - sizeof(F_ADDR);
	 bytecpy(r_node->slots, l_node_slot_ptr, sizeof(F_ADDR));

	 /* delete end slot from left node */
	 --l_node->used_slots;
      }
      dio_touch(l_pg);
      dio_touch(r_pg);
      dio_touch(p_pg);
   }
   else {
      dio_touch(pg);
   }
   MEM_UNLOCK(&curkey->Node_path);
   return( db_status );
}



/* Open n slots in node
*/
static void open_slots(node, slot_pos, n)
NODE FAR *node;
int slot_pos;
int n;
{
   register char FAR *dst, FAR *src;
   register int amt, w, nw;

   nw = NODE_WIDTH(node);
   w = n*slot_len;
   src = &node->slots[nw];
   dst = src + w;
   amt = nw - slot_pos;
   while (amt--)
      *--dst = *--src;

   node->used_slots += n;
}



/* Close n slots in node
*/
static void close_slots(node, slot_pos, n)
NODE FAR *node;
int slot_pos;
int n;
{
   register char FAR *dst, FAR *src;
   register int w, amt;

   node->used_slots -= n;

   w = n*slot_len;
   dst = &node->slots[slot_pos];
   src = dst + w;
   amt = NODE_WIDTH(node) - slot_pos;
   while (amt--)
      *dst++ = *src++;
}




/* Read value of last key scanned
*/
int
d_keyread(key_val TASK_PARM)
char FAR *key_val;
TASK_DECL
{
   register int kt_lc;			/* loop control */
#ifndef	 NO_FLOAT
   float fv;
   double dv;
#endif
   char FAR *fptr;
   char FAR *kptr;
   FIELD_ENTRY FAR *fld_ptr;
   register KEY_ENTRY FAR *key_ptr;

   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(RECORD_IO));

   if ((curkey->lstat != KEYFOUND) &&
       (curkey->lstat != KEYNOTFOUND) &&
       (curkey->lstat != KEYREPOS)) {
      RETURN( dberr(S_KEYSEQ) );
   }
   /* clear key area */
   byteset(key_val, '\0', cfld_ptr->fd_len);

   if ( cfld_ptr->fd_type == COMKEY ) {
      /* copy compound key fields */
      for (kt_lc = size_kt - cfld_ptr->fd_ptr,
				key_ptr = &key_table[cfld_ptr->fd_ptr]; 
	   (--kt_lc >= 0) && (key_ptr->kt_key == fldno); ++key_ptr) {
	 fld_ptr = &field_table[key_ptr->kt_field];
	 fptr = key_type.ks.data + key_ptr->kt_ptr;
	 kptr = key_val + key_ptr->kt_ptr;
	 if ( key_ptr->kt_sort == 'd' ) {
	    switch ( fld_ptr->fd_type ) {
#ifndef	 NO_FLOAT
	       case FLOAT:
		  bytecpy(&fv, fptr, sizeof(float));
		  fv = (float)0.0 - fv;
		  bytecpy(kptr, &fv, sizeof(float));
		  break;
	       case DOUBLE:
		  bytecpy(&dv, fptr, sizeof(double));
		  dv = 0.0 - dv;
		  bytecpy(kptr, &dv, sizeof(double));
		  break;
#endif
	       case CHARACTER:
		  key_cmpcpy(kptr, fptr, fld_ptr->fd_len);
		  if ( fld_ptr->fd_dim[0] > 1 && fld_ptr->fd_dim[1] == 0 ) {
		     /* make sure a null byte is at the end */
		     kptr[fld_ptr->fd_len-1] = '\0';
		  }
		  break;
	       default:
		  key_cmpcpy(kptr, fptr, fld_ptr->fd_len);
	    }
	 }
	 else {
	    bytecpy(kptr, fptr, fld_ptr->fd_len);
	 }
      }
   }
   else if ( cfld_ptr->fd_type == CHARACTER && cfld_ptr->fd_dim[1] == 0 )
      strncpy(key_val, key_type.ks.data, key_len);
   else
      bytecpy(key_val, key_type.ks.data, key_len);

   RETURN( db_status = S_OKAY );
}



/* Build compound key value from record
*/
int
key_bldcom(fld, rec, key, cflag )
int   fld; /* compound key field number */
char FAR *rec; /* ptr to record data */
char FAR *key; /* ptr to array to recv constructed key */
int cflag; /* TRUE to compliment compound descending keys */
{
   register int kt_lc;			/* loop control */
#ifndef	 NO_FLOAT
   float fv;
   double dv;
#endif
   char FAR *fptr;
   FIELD_ENTRY FAR *fld_ptr, FAR *kfld_ptr;
   register KEY_ENTRY FAR *key_ptr;

   /* clear key area */
   fld_ptr = &field_table[fld];
   byteset(key, '\0', fld_ptr->fd_len);

   /* create compound key value */
   rec -= record_table[fld_ptr->fd_rec].rt_data;
   for (kt_lc = size_kt - fld_ptr->fd_ptr,
					key_ptr = &key_table[fld_ptr->fd_ptr];
	(--kt_lc >= 0) && (key_ptr->kt_key == fld); ++key_ptr) {
      kfld_ptr = &field_table[key_ptr->kt_field];
      fptr = rec + kfld_ptr->fd_ptr;

      /* Complement descending keys if permitted (cflag) */
      if ( cflag && ( key_ptr->kt_sort == 'd' )) {
	 switch ( kfld_ptr->fd_type ) {
#ifndef	 NO_FLOAT
	    case FLOAT:
	       bytecpy(&fv, fptr, sizeof(float));
	       fv = (float)0.0 - fv;
	       bytecpy(&key[key_ptr->kt_ptr], &fv, sizeof(float));
	       break;
	    case DOUBLE:
	       bytecpy(&dv, fptr, sizeof(double));
	       dv = 0.0 - dv;
	       bytecpy(&key[key_ptr->kt_ptr], &dv, sizeof(double));
	       break;
#endif
	    case CHARACTER:
	       key_cmpcpy(key+key_ptr->kt_ptr, fptr, kfld_ptr->fd_len);
	       if ( kfld_ptr->fd_dim[0] > 1 && kfld_ptr->fd_dim[1] == 0 ) {
		  /* make sure a null byte is at the end */
		  *(key + key_ptr->kt_ptr + kfld_ptr->fd_len - 1) = '\0';
	       }
	       break;
	    default:
	       key_cmpcpy(key+key_ptr->kt_ptr, fptr, kfld_ptr->fd_len);
	 }
      }
      else {
	 bytecpy(&key[key_ptr->kt_ptr], fptr, kfld_ptr->fd_len);
      }
   }
   return( db_status = S_OKAY );
}



/* Complement and copy bytes
*/
void key_cmpcpy(s1, s2, n)
register char FAR *s1;
register char FAR *s2;
register INT n;
{
   while ( n-- ) {
      *s1++ = ~(*s2++);
   }
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin keyfcns.c */
