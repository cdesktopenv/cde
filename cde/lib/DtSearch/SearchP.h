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
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS:
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1990,1995
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
#ifndef _SearchP_h
#define _SearchP_h
/************************ SearchP.h ********************************
 * $XConsortium: SearchP.h /main/9 1996/10/12 16:10:00 cde-ibm $
 * June 1990.
 * Private header file for DtSearch/AusText used by
 * both build tools and search engine.
 * Derived from original IBM fuzzy.h.
 *
 * Hierarchies of DtSearch/AusText Search*.h #includes:
 *	Search.h is Dt/Aus public header, defines _XOPEN_SOURCE.
 *	SearchP.h is private Dt/Aus, includes Search.h.
 *	semantic.h is private Aus only, defines _ALL_SOURCE,
 *		includes SearchP.h.
 *		(Should be specified in place of SearchP.h).
 *	SearchE.h is private engine header Dt/Aus.  Invokes either
 *		SearchP.h or semantic.h depending on whether DTSEARCH defined.
 * Only one of the above should be specified.  They should always
 * be first include to get the _SOURCE defines right.
 * DTSEARCH should be defined on compiler cmd line.
 *
 * $Log$
 * Revision 2.11  1996/04/10  19:44:43  miker
 * AusText 2.1.13, DtSearch 0.5.  Documentation cleanup.
 *
 * Revision 2.10  1996/03/13  22:35:14  miker
 * Added UCHAR definition.  Changed char to UCHAR several places.
 *
 * Revision 2.9  1996/03/05  16:48:56  miker
 * Add COMMENT_CHARS from lang.c.
 *
 * Revision 2.8  1996/03/05  15:58:57  miker
 * Replace vewords with yacc-based boolean search.
 *
 * Revision 2.7  1996/02/01  16:26:06  miker
 * AUSAPI_VERSION 2.1.11, DtSearch 0.3:
 * Changed parsers to use character reading cofunctions.
 *
 * Revision 2.6  1995/12/27  15:51:36  miker
 * Version 2.1.10: First GA release of AusBuild.
 *
 * Revision 2.5  1995/12/07  23:24:38  miker
 * Version 2.1.9e: Freezing AusBuild version with
 * everything except multiple source files dialog box.
 *
 * Revision 2.4  1995/10/25  22:29:29  miker
 * Added prolog.
 *
 * Revision 2.3  1995/10/19  19:07:42  miker
 * Changed AUSAPI_VERSION to 2.1.9 (but not yet released).
 *
 * Revision 2.2  1995/10/03  21:36:13  miker
 * Added 'const' to strdup prototype for greater portability.
 *
 * Revision 2.1  1995/09/22  22:39:41  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.3  1995/09/19  21:38:59  miker
 * Removed debugging #defines.
 */
#include "Search.h"	/* the "public" header file */
#include <nl_types.h>	/* for nl_catd below and all cat...() funcs */
#include <sys/types.h>
#include <netinet/in.h>

#define AUSAPI_VERSION		"2.1.13"
    /* Format: "v.r.m", where v,r,m are integer substrings
     * meaning version, revision, and mod respectively.
     * Complete new version austomatically sets revision=1, mod=0.
     * New revision within a version just sets mod=0.
     * When mod=0, full string may be abbreviated to "v.r".
     * Full string will contain no whitespace.
     * AUSAPI_VERSION is compared to current versions of database
     * schemas and client/server protocols by ausapi_init().
     * By convention, AusText programs display AUSAPI_VERSION
     * at the top of the main gui window and in usage statements.
     */
#define SCHEMA_VERSION		"2.0"
	/* Identifies when schema last changed.
	 * Same format as AUSAPI_VERSION, except only "v.r" substring used.
	 * To ensure compatibility, database programs must have v.r integers
	 * within the range between SCHEMA_VERSION and AUSAPI_VERSION.
	 * The 3rd integer, the 'mod' number, never matters.
         */
#define PROTOCOL_VERSION	"2.1"
	/* Identifies when protocol between ui and engine/ausapi
	 * last changed.  Typical changes would be changes to major
	 * structures like usrblk, dblk, and socblk, or changes to
	 * the arguments for either ausapi or Opera_Engine calls.
	 * Only meaningful for client/server systems.
	 * Same format as AUSAPI_VERSION, except only "v.r" substring used.
	 * To ensure compatibility, clients must have v.r integers
	 * within the range between PROTOCOL_VERSION and AUSAPI_VERSION
	 * of the engine.  The 3rd integer, the 'mod' number, never matters.
         */

/*------------- #define Compiler and related #defines ------------------
 * #define TURBO_COMPILER...
 * #define PS2AIX_COMPILER...
 * #define AIX370_COMPILER...
 * #define HP_COMPILER...
 * #define _AIX...
 * ... define one of the above at top of Search.h to provide
 * structures and functions that are compiler dependent.
 * Since the compiler definition must precede all other headers,
 * the #include for Search.h must be the first statement of all source
 * files.
 * 
 * UCHAR corrects for compilers whose default char is signed.
 */
#define UCHAR	unsigned char
#ifdef TURBO_COMPILER
#define	LINT_ARGS	/* establishes strong prototyping in vista */
#define ANSI		/* allow multiple databases in vista */

#else
#define SINGLE_USER	/* eliminate vista lockmgr */
#define UNIX		/* used in vista.h and elsewhere */
#endif

#ifdef HP_COMPILER
#define _INCLUDE_HPUX_SOURCE
#define _INCLUDE_POSIX_SOURCE
#define _INCLUDE_XOPEN_SOURCE
#endif


/*----------------------- AUSTEXT.H ---------------------
 * DBMS record/key structure and constant declarations.
 * These represent the database schema formerly in austext.h
 * and derived from austext.sch.
 */
typedef struct or_dbrec {
   DtSrUINT32	or_dbflags;
   DtSrUINT32	or_dbuflags;
   DtSrINT32	or_reccount;
   DtSrINT32	or_maxdba;
   DtSrINT32	or_availd99;
   DtSrINT32	or_unavaild99;
   DtSrINT32	or_hufid;
   DtSrINT32	or_dbsecmask;
   char		or_version[8];
   char		or_dbfill[50];
   DtSrINT16	or_dbotype;
   DtSrINT16	or_compflags;
   DtSrINT16	or_dbaccess;
   DtSrINT16	or_minwordsz;
   DtSrINT16	or_maxwordsz;
   DtSrINT16	or_recslots;
   DtSrINT16	or_fzkeysz;
   DtSrINT16	or_abstrsz;
   DtSrINT16	or_language;
}	DBREC;

struct or_dbmiscrec {
   DtSrINT16	or_dbmisctype;
   char		or_dbmisc[1][106];
};
struct or_objrec {
   DtSrUINT32	or_objflags;
   DtSrUINT32	or_objuflags;
   DtSrINT32	or_objsize;
   DtSrINT32	or_objdate;
   DtSrINT32	or_objsecmask;
   char		or_objkey[32];
   char		or_objfill[34];
   DtSrINT16	or_objaccess;
   DtSrINT16	or_objtype;
   DtSrINT16	or_objcost;
   DtSrINT16	or_objhdroffset;
   DtSrINT16	or_objeureka;
};
struct or_miscrec {
   DtSrINT16	or_misctype;
   char		or_misc[1][105];
};
struct or_blobrec {
   DtSrINT16	or_bloblen;
   char		or_blob[234][1];
};
struct or_swordrec {
   char		or_swordkey[16];
   DtSrINT32	or_swoffset;
   DtSrINT32	or_swfree;
   DtSrINT32	or_swaddrs;
};
struct or_lwordrec {
   char		or_lwordkey[40];
   DtSrINT32	or_lwoffset;
   DtSrINT32	or_lwfree;
   DtSrINT32	or_lwaddrs;
};
struct or_hwordrec {
   char		or_hwordkey[134];
   DtSrINT32	or_hwoffset;
   DtSrINT32	or_hwfree;
   DtSrINT32	or_hwaddrs;
};

/* File Id Constants */
#define OR_D00 0
#define OR_D01 1
#define OR_D21 2
#define OR_D22 3
#define OR_D23 4
#define OR_K00 5
#define OR_K01 6
#define OR_K21 7
#define OR_K22 8
#define OR_K23 9

/* Record Name Constants */
#define OR_DBREC 10000
#define OR_DBMISCREC 10001
#define OR_OBJREC 10002
#define OR_MISCREC 10003
#define OR_BLOBREC 10004
#define OR_SWORDREC 10005
#define OR_LWORDREC 10006
#define OR_HWORDREC 10007

/* Field Name Constants */
#define OR_DBFLAGS 0L
#define OR_DBUFLAGS 1L
#define OR_RECCOUNT 2L
#define OR_MAXDBA 3L
#define OR_AVAILD99 4L
#define OR_UNAVAILD99 5L
#define OR_HUFID 6L
#define OR_DBSECMASK 7L
#define OR_VERSION 8L
#define OR_DBFILL 9L
#define OR_DBOTYPE 10L
#define OR_COMPFLAGS 11L
#define OR_DBACCESS 12L
#define OR_MINWORDSZ 13L
#define OR_MAXWORDSZ 14L
#define OR_RECSLOTS 15L
#define OR_FZKEYSZ 16L
#define OR_ABSTRSZ 17L
#define OR_LANGUAGE 18L
#define OR_DBMISCTYPE 1000L
#define OR_DBMISC 1001L
#define OR_OBJFLAGS 2000L
#define OR_OBJUFLAGS 2001L
#define OR_OBJSIZE 2002L
#define OR_OBJDATE 2003L
#define OR_OBJSECMASK 2004L
#define OR_OBJKEY 2005L
#define OR_OBJFILL 2006L
#define OR_OBJACCESS 2007L
#define OR_OBJTYPE 2008L
#define OR_OBJCOST 2009L
#define OR_OBJHDROFFSET 2010L
#define OR_OBJEUREKA 2011L
#define OR_MISCTYPE 3000L
#define OR_MISC 3001L
#define OR_BLOBLEN 4000L
#define OR_BLOB 4001L
#define OR_SWORDKEY 5000L
#define OR_SWOFFSET 5001L
#define OR_SWFREE 5002L
#define OR_SWADDRS 5003L
#define OR_LWORDKEY 6000L
#define OR_LWOFFSET 6001L
#define OR_LWFREE 6002L
#define OR_LWADDRS 6003L
#define OR_HWORDKEY 7000L
#define OR_HWOFFSET 7001L
#define OR_HWFREE 7002L
#define OR_HWADDRS 7003L

/* Set Name Constants */
#define OR_DB_MISCS 20000
#define OR_OBJ_BLOBS 20001
#define OR_OBJ_MISCS 20002
/*----------------------- end AUSTEXT.H ---------------------*/



/*------------- DMACROS.H -------------------
 * The following were formerly in dmacros.h.
 * They provide macro enhancements to the
 * DBMS function prototypes in vista.h.
 * Each DBMS function is coded in a macro which includes
 * a location string (usually module name + line number).
 * The location string is printed whenever there is
 * a user or system failure.
 * Warning: These should not be coded naked within 'if-else' statements
 * because the 'if' in the macros will interfere with the outside else.
 */
#define CMSTAT(s,a,b) if(d_cmstat(a,b) < S_OKAY) vista_abort(s)
#define CMTYPE(s,a,b,c) if(d_cmtype(a,b,c) < S_OKAY) vista_abort(s)
#define CONNECT(s,a,b) if(d_connect(a,b) < S_OKAY) vista_abort(s)
#define COSTAT(s,a,b) if(d_costat(a,b) < S_OKAY) vista_abort(s)
#define COTYPE(s,a,b,c) if(d_cotype(a,b,c) < S_OKAY) vista_abort(s)
#define CRGET(s,a,b) if(d_crget(a,b) < S_OKAY) vista_abort(s)
#define CRREAD(s,a,b,c) if(d_crread(a,b,c) < S_OKAY) vista_abort(s)
#define CRSET(s,a,b) if(d_crset(a,b) < S_OKAY) vista_abort(s)
#define CRSTAT(s,a) if(d_crstat(a) < S_OKAY) vista_abort(s)
#define CRTYPE(s,a,b) if(d_crtype(a,b) < S_OKAY) vista_abort(s)
#define CRWRITE(s,a,b,c) if(d_crwrite(a,b,c) < S_OKAY) vista_abort(s)
#define CSMGET(s,a,b,c) if(d_csmget(a,b,c) < S_OKAY) vista_abort(s)
#define CSMREAD(s,a,b,c,d) if(d_csmread(a,b,c,d) < S_OKAY) vista_abort(s)
#define CSMSET(s,a,b,c) if(d_csmset(a,b,c) < S_OKAY) vista_abort(s)
#define CSMWRITE(s,a,b,c,d) if(d_csmwrite(a,b,c,d) < S_OKAY) vista_abort(s)
#define CSOGET(s,a,b,c) if(d_csoget(a,b,c) < S_OKAY) vista_abort(s)
#define CSOREAD(s,a,b,c,d) if(d_csoread(a,b,c,d) < S_OKAY) vista_abort(s)
#define CSOSET(s,a,b,c) if(d_csoset(a,b,c) < S_OKAY) vista_abort(s)
#define CSOWRITE(s,a,b,c,d) if(d_csowrite(a,b,c,d) < S_OKAY) vista_abort(s)
#define CSSTAT(s,a,b) if(d_csstat(a,b) < S_OKAY) vista_abort(s)
#define CTBPATH(s,a) if(d_ctbpath(a) < S_OKAY) vista_abort(s)
#define CTSCM(s,a,b,c) if(d_ctscm(a,b,c) < S_OKAY) vista_abort(s)
#define CTSCO(s,a,b,c) if(d_ctsco(a,b,c) < S_OKAY) vista_abort(s)
#define CTSCR(s,a,b) if(d_ctscr(a,b) < S_OKAY) vista_abort(s)
#define DBDPATH(s,a) if(d_dbdpath(a) < S_OKAY) vista_abort(s)
#define DBFPATH(s,a) if(d_dbfpath(a) < S_OKAY) vista_abort(s)
#define DBLOG(s,a) if(d_dblog(a) < S_OKAY) vista_abort(s)
#define DBTAF(s,a) if(d_dbtaf(a) < S_OKAY) vista_abort(s)
#define DBUSERID(s,a) if(d_dbuserid(a) < S_OKAY) vista_abort(s)
#define DECODE_DBA(s,a,b,c) if(d_decode_dba(a,b,c) < S_OKAY) vista_abort(s)
#define DELETE(s,a) if(d_delete(a) < S_OKAY) vista_abort(s)
#define DESTROY(s,a) if(d_desoy(a) < S_OKAY) vista_abort(s)
#define DISCON(s,a,b) if(d_discon(a,b) < S_OKAY) vista_abort(s)
#define DISDEL(s,a) if(d_disdel(a) < S_OKAY) vista_abort(s)
#define ENCODE_DBA(s,a,b,c) if(d_encode_dba(a,b,c) < S_OKAY) vista_abort(s)
#define FILLNEW(s,a,b,c) if(d_fillnew(a,b,c) < S_OKAY) vista_abort(s)
#define FINDCO(s,a,b) if(d_findco(a,b) < S_OKAY) vista_abort(s)
#define FINDFM(s,a,b) if(d_findfm(a,b) < S_OKAY) vista_abort(s)
#define FINDLM(s,a,b) if(d_findlm(a,b) < S_OKAY) vista_abort(s)
#define FINDNM(s,a,b) if(d_findnm(a,b) < S_OKAY) vista_abort(s)
#define FINDPM(s,a,b) if(d_findpm(a,b) < S_OKAY) vista_abort(s)
#define FREEALL(s) if(d_freeall() < S_OKAY) vista_abort(s)
#define GTSCM(s,a,b,c) if(d_gtscm(a,b,c) < S_OKAY) vista_abort(s)
#define GTSCO(s,a,b,c) if(d_gtsco(a,b,c) < S_OKAY) vista_abort(s)
#define GTSCR(s,a,b) if(d_gtscr(a,b) < S_OKAY) vista_abort(s)
#define GTSCS(s,a,b,c) if(d_gtscs(a,b,c) < S_OKAY) vista_abort(s)
#define INITFILE(s,a,b) if(d_initfile(a,b) < S_OKAY) vista_abort(s)
#define INITIALIZE(s,a) if(d_initialize(a) < S_OKAY) vista_abort(s)
#define ISMEMBER(s,a,b) if(d_ismember(a,b) < S_OKAY) vista_abort(s)
#define ISOWNER(s,a,b) if(d_isowner(a,b) < S_OKAY) vista_abort(s)
#define KEYDEL(s,a,b) if(d_keydel(a,b) < S_OKAY) vista_abort(s)
#define KEYEXIST(s,a,b) if(d_keyexist(a,b) < S_OKAY) vista_abort(s)
#define KEYFIND(s,a,b,c) if(d_keyfind(a,b,c) < S_OKAY) vista_abort(s)
#define KEYFREE(s,a,b) if(d_keyfree(a,b) < S_OKAY) vista_abort(s)
#define KEYFRST(s,a,b) if(d_keyfrst(a,b) < S_OKAY) vista_abort(s)
#define KEYLAST(s,a,b) if(d_keylast(a,b) < S_OKAY) vista_abort(s)
#define KEYLOCK(s,a,b,c) if(d_keylock(a,b,c) < S_OKAY) vista_abort(s)
#define KEYLSTAT(s,a,b,c) if(d_keylstat(a,b,c) < S_OKAY) vista_abort(s)
#define KEYNEXT(s,a,b) if(d_keynext(a,b) < S_OKAY) vista_abort(s)
#define KEYPREV(s,a,b) if(d_keyprev(a,b) < S_OKAY) vista_abort(s)
#define KEYREAD(s,a) if(d_keyread(a) < S_OKAY) vista_abort(s)
#define KEYSTORE(s,a,b) if(d_keystore(a,b) < S_OKAY) vista_abort(s)
#define LOCK(s,a,b,c) if(d_lock(a,b,c) < S_OKAY) vista_abort(s)
#define MAKENEW(s,a,b) if(d_makenew(a,b) < S_OKAY) vista_abort(s)
#define MAPCHAR(s,a,b,c,d) if(d_mapchar(a,b,c,d) < S_OKAY) vista_abort(s)
#define MEMBERS(s,a,b,c) if(d_members(a,b,c) < S_OKAY) vista_abort(s)
#define OFF_OPT(s,a) if(d_off_opt(a) < S_OKAY) vista_abort(s)
#define ON_OPT(s,a) if(d_on_opt(a) < S_OKAY) vista_abort(s)
#define OPEN(s,a,b) if(d_open(a,b) < S_OKAY) vista_abort(s)
#define RDCURR(s,a,b) if(d_rdcurr(a,b) < S_OKAY) vista_abort(s)
#define RECFREE(s,a,b) if(d_recfree(a,b) < S_OKAY) vista_abort(s)
#define RECFRST(s,a,b) if(d_recfrst(a,b) < S_OKAY) vista_abort(s)
#define RECLAST(s,a,b) if(d_reclast(a,b) < S_OKAY) vista_abort(s)
#define RECLOCK(s,a,b,c) if(d_reclock(a,b,c) < S_OKAY) vista_abort(s)
#define RECLSTAT(s,a,b,c) if(d_reclstat(a,b,c) < S_OKAY) vista_abort(s)
#define RECNEXT(s,a) if(d_recnext(a) < S_OKAY) vista_abort(s)
#define RECOVER(s,a) if(d_recover(a) < S_OKAY) vista_abort(s)
#define RECPREV(s,a) if(d_recprev(a) < S_OKAY) vista_abort(s)
#define RECREAD(s,a,b) if(d_recread(a,b) < S_OKAY) vista_abort(s)
#define RECSET(s,a,b) if(d_recset(a,b) < S_OKAY) vista_abort(s)
#define RECSTAT(s,a,b,c) if(d_recstat(a,b,c) < S_OKAY) vista_abort(s)
#define RECWRITE(s,a,b) if(d_recwrite(a,b) < S_OKAY) vista_abort(s)
#define RENFILE(s,a,b,c) if(d_renfile(a,b,c) < S_OKAY) vista_abort(s)
#define RLBCLR(s,a) if(d_rlbclr(a) < S_OKAY) vista_abort(s)
#define RLBSET(s,a) if(d_rlbset(a) < S_OKAY) vista_abort(s)
#define RLBTST(s,a) if(d_rlbtst(a) < S_OKAY) vista_abort(s)
#define SETDB(s,a) if(d_setdb(a) < S_OKAY) vista_abort(s)
#define SETFILES(s,a) if(d_setfiles(a) < S_OKAY) vista_abort(s)
#define SETFREE(s,a,b) if(d_setfree(a,b) < S_OKAY) vista_abort(s)
#define SETLOCK(s,a,b,c) if(d_setlock(a,b,c) < S_OKAY) vista_abort(s)
#define SETLSTAT(s,a,b,c) if(d_setlstat(a,b,c) < S_OKAY) vista_abort(s)
#define SETMM(s,a,b,c) if(d_setmm(a,b,c) < S_OKAY) vista_abort(s)
#define SETMO(s,a,b,c) if(d_setmo(a,b,c) < S_OKAY) vista_abort(s)
#define SETMR(s,a,b) if(d_setmr(a,b) < S_OKAY) vista_abort(s)
#define SETOM(s,a,b,c) if(d_setom(a,b,c) < S_OKAY) vista_abort(s)
#define SETOO(s,a,b,c) if(d_setoo(a,b,c) < S_OKAY) vista_abort(s)
#define SETOR(s,a,b) if(d_setor(a,b) < S_OKAY) vista_abort(s)
#define SETPAGES(s,a,b) if(d_setpages(a,b) < S_OKAY) vista_abort(s)
#define SETRM(s,a,b) if(d_setrm(a,b) < S_OKAY) vista_abort(s)
#define SETRO(s,a,b) if(d_setro(a,b) < S_OKAY) vista_abort(s)
#define STSCM(s,a,b,c) if(d_stscm(a,b,c) < S_OKAY) vista_abort(s)
#define STSCO(s,a,b,c) if(d_stsco(a,b,c) < S_OKAY) vista_abort(s)
#define STSCR(s,a,b) if(d_stscr(a,b) < S_OKAY) vista_abort(s)
#define STSCS(s,a,b,c) if(d_stscs(a,b,c) < S_OKAY) vista_abort(s)
#define TIMEOUT(s,a) if(d_timeout(a) < S_OKAY) vista_abort(s)
#define TRABORT(s) if(d_trabort() < S_OKAY) vista_abort(s)
#define TRBEGIN(s,a) if(d_trbegin(a) < S_OKAY) vista_abort(s)
#define TREND(s) if(d_trend() < S_OKAY) vista_abort(s)
#define UTSCM(s,a,b,c) if(d_utscm(a,b,c) < S_OKAY) vista_abort(s)
#define UTSCO(s,a,b,c) if(d_utsco(a,b,c) < S_OKAY) vista_abort(s)
#define UTSCR(s,a,b) if(d_utscr(a,b) < S_OKAY) vista_abort(s)
#define UTSCS(s,a,b,c) if(d_utscs(a,b,c) < S_OKAY) vista_abort(s)
#define WRCURR(s,a) if(d_wrcurr(a) < S_OKAY) vista_abort(s)
/*------------- end DMACROS.H -------------------*/


/*------------- COMPILER DEPENDENT CONSTANTS -------------------*/
#ifdef TURBO_COMPILER
  #define FNAME_NULL	"nul"
  #define LOCAL_SLASH	92	/* ascii back slash char (\) */
#endif

#ifdef UNIX
  #define FNAME_NULL	"/dev/null"
  #define LOCAL_SLASH	47	/* ascii forward slash char (/) */
#endif

/*------------ COMPILER INDEPENDENT CONSTANTS -------------------*/

#define CACHE_SIZE	64	/* used in vista d_setpages() function */
#define COMMENT_CHARS	"#*$!\n"  /* identify comment lines in AusText files */
#define CTRL_Z		26
#define DIT_FINDSTR	1L	/* DITTO.flags: retained aft FINDSTR srch */
#define DIT_STOP	2L	/* DITTO.flags: node where user pushed stop */
#define	END_RETAIN_PAGE 11	/* VT = marks end of RETAIN page */
/***#define	END_RETAIN_REC  '\f'****/
#define EXT_CHARTYPE	".chr"	/* user definable wildcards (from LOADCHR) */
#define EXT_CANDI	".can"	/* candidate dictionary words format */
#define EXT_CONFIG	".ocf"	/* standard opera configuration file */
#define EXT_DTBS	".d99"	/* inverted index file for dbase addrs */
#define EXT_FZKEY	".fzk"	/* output of all opera text anal pgms */
#define EXT_HANDEL	".han"  /* standard handel profile file format */
#define EXT_HUFFCODE	".huf"  /* huffman encode tree (from HUFFCODE) */
#define EXT_HDECODE	".c"    /* huffman decode tree (from HUFFCODE) */
#define EXT_INCLIST	".inc"	/* "include" list file name extension */
#define EXT_LIST	".lst"	/* standard wordlist format: 1 word/line */
#define EXT_SCHEMA	".sch"	/* vista database schema format */
#define EXT_STOPLIST	".stp"	/* standard stop list format */
#define EXT_SURVEY	".sur"	/* standard survey file format */
#define EXT_TEMP	".tmp"	/* any kind of temporary file */
#define EXT_TEXT	".txt"	/* freeform ascii text format */
#define EXT_USRNOTES	".not"	/* user notes flatfile format */
#define ETXDELIM	"\f\n"	/* default end-of-text (ETX) delim str */
#define FNAME_AUDIT	"opaudit.lst"
#define FNAME_AUSCAT		"austools.cat"	/* Austools msgs catalog */
#define FNAME_AUSTEXT_TUTORIAL  "austext.tut"
#define FNAME_CONFIRM_LIST      "todscrd.lst"	/* Shankar/Tomita files */
#define FNAME_DISCARD_DATA      "shdscrd.lst"	/* OEF_discard */
#define FNAME_DTSRCAT		"dtsearch"	/* DtSearch msgs catalog */
#define FNAME_HUFFCODE_TAB      "ophuf.huf"	/* huffman encode table */
#define FNAME_MUIRES	"mui.res"	/* motif class resources file */
#define FNAME_NOTES_BAC	"usrnotes.not"
#define FNAME_NOTES_SEM	"usrnotes.sem"
#define FNAME_ORIGSTOP	"orig.stp"
#define FNAME_README	"readme.txt"
#define FNAME_SITECONFIG  "dtsearch.ocf"
#define FNAME_SITENEWS	"sitenews.txt"
#define	MINWIDTH_TOKEN	2	/* default smallest word/stem allowed */
#define MAX_BMHTAB	256	/* max alphabet size in bmstrstr tables */
#define MAX_ETXDELIM	100	/* max size of ETX delim string */
#define MAX_KTCOUNT	64      /* max number of keytypes */
#define	MAXWIDTH_LWORD	40	/* = sizeof(or_lwordrec.or_lwordkey) */
#define	MAXWIDTH_SWORD	16	/* = sizeof(or_swordrec.or_swordkey) */
#define NULLDATESTR	"0/0/0~0:0"
#define NUM_HOLES	256	/* array size for defragmentation */
#define OBJDATESTR	"%02d/%02d/%02d~%02d:%02d"
#define PRODNAME	"DtSearch"
#define PWDMASKSZ	20
#define SCREEN_WIDTH	80	/* max len text line in online OPERA pgms */
#define STEM_CH		15	/* Ctrl-O stemmed word prefix character */

/*--------------- BYTE SWAP DECLARATIONS ------------
 * The default database record format is the data ordering
 * for big endian machines (most significant byte first),
 * also known as "network" order.  For non big_endian platforms,
 * all compiles should include the BYTE_SWAP define.
 * SWABDIR is direction of io to indicate correct byte swap function.
 * HTON is host to network, internal memory to external database file.
 * NTOH is network to host, file to memory. 
 * Actual host_to_network functions defined in <netinit/in.h>
 * which is not yet standardized.
 */
typedef enum {HTON=1, NTOH} SWABDIR;

extern void swab_dbrec  (struct or_dbrec  *rec,  SWABDIR direction);
extern void swab_objrec (struct or_objrec *rec,  SWABDIR direction);

#ifdef BYTE_SWAP	/* ie (BYTE_ORDER != BIG_ENDIAN) */

#define HTONL(x)	x = htonl(x)
#define HTONS(x)	x = htons(x)
#define NTOHL(x)	x = ntohl(x)
#define NTOHS(x)	x = ntohs(x)

#else	/* !BYTE_SWAP, ie (BYTE_ORDER == BIG_ENDIAN) */

#define HTONL(x)
#define HTONS(x)
#define NTOHL(x)
#define NTOHS(x)

#endif /* BYTE_SWAP */


/*--------------- TESKEY PARSE CHARACTER TYPES ------------
 * Used in langmap.c for linguistic parsing modules.
 * Low order byte reserved for uppercase image of character
 * as locale independent replacement for toupper() and strupr().
 */
#define CONCORDABLE	0x4000  /* alpha, numeric: inside word */
#define OPT_CONCORD	0x2000  /* "./-": maybe inside word, maybe outside */
#define NON_CONCORD	0x1000  /* space, punctuation, etc: outside word */

#define VOWEL		0x0800  /* concordable subtype: aeiou */
#define CONSONANT	0x0400  /* concordable subtype: all alpha - vowels */
#define NUMERAL		0x0200  /* concordable subtype: 0-9 */
#define WHITESPACE	0x0100  /* locale indep replacement for isspace() */


/*--------------- SCHEMA CONSTANTS ----------------
 * Associated with fields in database schema (austext.h)
 * and in DITTO structures.
 * Values 20000 - 29999 reserved for custom user applic in all cardinal ints.
 * ORA_ retrieval access location cardinal integers in or_access.
 * ORC_ compression id bit switches in or_compflags.
 * ORD_ bit switches in or_dbflags.
 * (ORO_ bit switches in or_objflags
 * renamed to DtSrFlxxx and moved to Search.h)
 * ORM_ type cardinal integers in or_misctype.
 * (ORT_ object type (or_objtype, or_dbotype, DITTO)
 * renamed to DtSrObj... and moved to Search.h)
 * (ORL_ language id cardinal integers in or_language
 * renamed to DtSrLa... and moved to Search.h)
 */
#define ORA_VARIES	0	  /* object accessibility unspec at curr lvl */
#define ORA_NOTAVAIL	1	  /* obj not directly accessible from engine */
#define ORA_BLOB	2	  /* obj stored in blob recs */
#define ORA_REFBLOB	3	  /* svr ref (filename of object) in blob */
#define ORA_REFKEY	4	  /* svr ref in or_objkey */
#define ORA_REFHUGEKEY	5	  /* svr ref in 'huge' key (misc) rec */
#define ORA_REFABSTR	6	  /* svr ref in abstract (misc rec) */
#define ORA_CREFBLOB	13	  /* clt ref (filename of object) in blob */
#define ORA_CREFKEY	14	  /* clt ref in or_objkey */
#define ORA_CREFABSTR	16	  /* clt ref in abstract (misc rec) */

#define ORC_COMPBLOB	(1<<0)	  /* blobs are compressed */
#define ORC_COMPABSTR	(1<<1)	  /* abstracts are compressed */

#define ORD_XWORDS	(1L<<0)   /* inverted index includes exact words */
#define ORD_XSTEMS	(1L<<1)   /* inverted index includes word stems */
#define ORD_USEHUGEKEYS	(1L<<4)   /* all direct user access via huge keys */
#define ORD_NOMARKDEL	(1L<<8)   /* permanently disables mark-for-deletion */
#define ORD_NONOTES	(1L<<9)   /* permanently disables user notes */
#define ORD_WIDECHAR	(1L<<10)  /* text is multibyte or wide chars */

#define ORO_DELETE	(1L<<0)	  /* obj is marked for deletion */
#define ORO_OLDNOTES	(1L<<1)	  /* obj has old style usernotes in misc rec */

#define ORM_FZKABS	1	  /* fzkey[fzkeysz] + abstract[abstrsz] */
#define ORM_HUGEKEY	2	  /* optional 'huge' key */
#define ORM_OLDNOTES	3	  /* old style user notes */
#define ORM_KEYTYPE	4	  /* database keytype rec (KEYTYPE struct) */

/*------------------------- MACROS ---------------------------*/
/* (Use offsetof() in stddef.h to replace my old OFFSET macro) */
#define NULLORSTR(str)      ((str)?str:catgets(dtsearch_catd,1,1,"<null>"))
#define NUMARRAY(arr)       ((sizeof(arr) / sizeof(arr[0])))

/****************************************/
/*					*/
/*		  LLIST			*/
/*					*/
/****************************************/
typedef struct llist_tag {
    struct llist_tag	*link;
    void		*data;
}	LLIST;

/****************************************/
/*					*/
/*	     FREE_SPACE_STR		*/
/*					*/
/****************************************/
/* 'holes' structure used for dynamic defragmentation */
typedef struct	holes_str {
	DtSrINT32	hole_size;
	DtSrINT32	offset;
	}	FREE_SPACE_STR;

/****************************************/
/*					*/
/*	     FILE_HEADER		*/
/*					*/
/****************************************/
/* 'holes' structure used for dynamic defragmentation */
typedef	struct		fl_head {
	DtSrINT32	hole_count [2];
	FREE_SPACE_STR	hole_array [2] [NUM_HOLES];
	}	FILE_HEADER;

/****************************************/
/*					*/
/*   CMPLL, sort_llist, compare_llist	*/
/*					*/
/****************************************/
/* Generic LLIST structure typically used for 'lines' of text,
 * as in ausapi.msglist and usrblk.msgs, and binary blobs.
 * See structure typedef below for blob list structure.
 * LLISTs can be sorted by calling sort_llist().
 * The compare function is stored in global pointer 'compare_llist'
 * prior to calling the sort function.
 */
typedef int (*CMPLL) (LLIST *left, LLIST *right);

/****************************************/
/*					*/
/*		 READCFP		*/
/*					*/
/****************************************/
/* Pointer to a function that takes a void
 * pointer and returns a C char (1 octet).
 * Used by linguistic parsers as a character reading cofunction.
 * When parser is reading from a file stream as in dtsrindex,
 * usual cofunction is readchar_ftext().  When parser is reading
 * from a text string as in queries, the usual cofunction
 * is readchar_string() which just accesses next char in string.
 */
typedef UCHAR	(*READCFP) (void *);

/********************************************************/
/*							*/
/*			 WORDTREE			*/
/*							*/
/********************************************************/
/* A generic structure for creating binary trees of words
 * from stoplists, include-lists, etc.
 */
typedef struct _bintree_tag {
    struct _bintree_tag  *rlink;   /* ptr to right node */
    struct _bintree_tag  *llink;   /* ptr to left node */
    int			 len;      /* length of word */
    void		 *word;
    } WORDTREE;


/************************************************/
/*						*/
/*		      PARG			*/
/*						*/
/************************************************/
/* Single argument for first parser call for a text block */
typedef struct {
    void	*dblk;		/* dblk is (DBLK *) */
    FILE	*ftext;		/* Text file being parsed in dtsrindex */
    void 	*string;	/* Query or other string being parsed */
    void	*etxdelim;	/* End of text (ETX) delimiter string */
    long	*offsetp;	/* where parser stores word offset */
    long	flags;
#define PA_INDEXING	0x0001	/* parse for indexing purposes (dtsrindex) */
#define PA_HILITING	0x0002	/* parse for hiliting purposes */
#define PA_MSGS		0x0004	/* explain parse failures with msgs */
    void	*extra;		/* reserved for generic use by parsers */
    }	PARG;


/********************************************************/
/*							*/
/*			  DBLK				*/
/*							*/
/********************************************************/
/* One element of a linked list of universal information
 * about all databases accessible,
 * to an online search engine instance, chained off of
 * the global OE_dblist.  It is also used individually
 * by offline build tools to store database data.
 * It contains pointers to linguistic processors and data structures,
 * data derived from the site configuration file,
 * data derived and used by the DBMS,
 * and some fields reflecting recommended initial user choices. 
 * Changes here should also be reflected in initblks.c,
 * austext.sch, initausd.c, and in oeinit.c.
 */
typedef struct dblk_tag
    {
    struct dblk_tag *link;	/* ptr to next dblk in list */
    char	name [11];	/* 1 - 8 char dictionary name */
    char	is_selected;	/* bool: initial gui selection status */
    char	*label;		/* database description/label string for UI */
    long	flags;
    char	*path;		/* where to find all database files */
    FILE	*syofile;	/* symptom offset (ranges) d97 file */
    FILE	*syifile;	/* symptom index d98 file */
    void	*ranges;	/* contents of syofile read into ram */
    FILE	*iifile;	/* inverted words index d99 file */
    time_t	iimtime;	/* last time d99 was modified */
    int		vistano;	/* vista database number */
    int		ktcount;        /* number of nodes in keytypes array */
    DtSrKeytype	*keytypes;      /* record types in this database */
    DBREC	dbrec;		/* copy of database's dbrec record */
    void	*zblk;		/* used only for semantic processing */

    /*----- Huffman Compression -----*/
    time_t	hufid;		/* tree identification atom */
    int		hufroot;	/* index to inverted tree root (array bottom) */
    int		*huftree;	/* beginning of huffman tree array */

    /*----- Language Fields -----*/
    char	*fname_stp;	/* name of stoplist file */
    char	*fname_inc;	/* name of include-list file */
    char	*fname_sfx;	/* name of suffixes file */
    int		*charmap;	/* parse/stem table for char set */
    WORDTREE	*stoplist;	/* root of stoplist tree */
    WORDTREE	*inclist;	/* root of include-list tree */
    char	*(*lstrupr) (char *string, struct dblk_tag *dblk);
    char	*(*parser)  (PARG *parg);
    void	*parse_extra;	/* eg additional word trees */
    char	*(*stemmer) (char *wordin, struct dblk_tag *dblk);
    long	stem_flags;	/* stemmer options */
    void	*stem_extra;	/* eg suffix tables */
    void	*lang_extra;	/* additional language dependent data */
    long	lang_flags;	/* language dependent switches */
#define LF_DUP_STP	0x0001	/* duplicate stop list */
#define LF_DUP_INC	0x0002	/* duplicate include list */
#define LF_DUP_SFX	0x0004	/* duplicate suffixes list */

    /*----- User Search Parameters -----*/
    int		maxhits;	/* max # hits to be returned from searches */
    void	*resuser;	/* reserved for private use by users */
    }	DBLK;


/*------------------------- GLOBALS -------------------------*/
/* DtSearchExit (austext_exit) and DtSearchExitUser (austext_exit_user)
 * are in ausexit.c too but are "public" in DtSearch (defined in Search.h).
 */
extern char     *aa_argv0;		/* globals.c */
extern int	aa_maxhits;		/* aajoint.c */
extern FILE     *aa_stderr;		/* globals.c */
extern int	ascii_charmap[];	/* langmap.c */
extern LLIST	*ausapi_msglist;	/* globals.c */
extern void	(*austext_exit_first) (int);	/* ausexit.c */
extern void	(*austext_exit_dbms) (int);	/* ausexit.c */
extern void	(*austext_exit_comm) (int);	/* ausexit.c */
extern void	(*austext_exit_endwin) (int);	/* ausexit.c */
extern void	(*austext_exit_mem) (int);	/* ausexit.c */
extern void	(*austext_exit_last) (int);	/* ausexit.c */
extern void	(*austext_exit_user) (int);	/* ausexit.c */
extern nl_catd	austools_catd;		/* globals.c */
extern CMPLL	compare_llist;
extern nl_catd	dtsearch_catd;		/* globals.c */
extern unsigned long
		duprec_hashsize;	/* isduprec.c */
extern int	latin_charmap[];	/* langmap.c */


/*------------------------ FUNCTION PROTOTYPES ------------------------*/
extern void	add_free_space(FREE_SPACE_STR *del_rec, FILE_HEADER *flh);
extern void	append_ext (char *buffer, int buflen,
		    char *fname, char *fext);
extern int	austext_dopen (
			char		*dbname,
			char		*dbpath,
			char		*d2xpath,
			int		cache_size,
			DBREC		*bufptr);
extern void	*austext_malloc (size_t size, char *location, void *ignored);
extern char	*bmhcore (
			UCHAR	*text,
			size_t	txtlen,
			UCHAR	*pattern,
			size_t	patlen,
			size_t	*bmhtable);
extern void	bmhtable_build (
			UCHAR	*pattern,
			size_t	patlen,
			size_t	*bmhtable);
extern char	*bmstrstr (
			UCHAR	*text,
			size_t	txtlen,
			UCHAR	*pattern,
			size_t	patlen);
extern int	clean_wrap (char *string, int linelen);
extern LLIST	*cutnode_llist (LLIST *node, LLIST **llistp);
extern void	(*dberr_exit)(int exitcode);   /* defaults to exit() */
extern void	delete_whitespace (char *linebuf);
extern void	discard_to_ETX (PARG *parg);
extern int	endstroke (int c);
extern  void    put_new_word(struct or_hwordrec *recbuf, int vistano);
extern FREE_SPACE_STR
		*find_free_space (DtSrINT32 req_size, FILE_HEADER *flh);
extern void	free_llist (LLIST **llhead);
extern int	fread_d99_header (FILE_HEADER *flh, FILE *fp);
extern int	fwrite_d99_header (FILE_HEADER *flh, FILE *fp);
extern char	*get_email_addr (void);
extern char	*getnews (char *fname, int use_copyr);
extern void	hc_decode (UCHAR *input_bitstring, UCHAR *output_charbuf,
			int outbuf_size, time_t encode_id);
extern int	hc_encode (struct or_blobrec *targ, UCHAR *src,
			int srclen, int force_flush);
extern void	init_header (FILE *fp, FILE_HEADER *flh);
extern int	is_compatible_version (char *callers_vers, char *local_vers);
extern int	is_concordable (char *word, int *charmap);
extern int	is_duprec (char *recid);
extern int	is_objdatestr (char *string, DtSrObjdate *objdptr);
extern int	is_objdatetm (struct tm *objdatetm);
extern void	join_llists (LLIST **mainlist, LLIST **sublist);
extern char	*jpn_parser (PARG *parg);
extern int	load_custom_language (DBLK *dblk, DBLK *dblist);
extern int	load_jpn_language (DBLK *dblk, DBLK *dblist);
extern int	load_language (DBLK *dblk, DBLK *dblist);
extern int	load_wordtree (
		    WORDTREE	**treetop,
		    DBLK	*dblk,
		    char	*fname,
		    int		do_teskey_test);
extern char	*null_stemmer (char *word, DBLK *dblk);
extern char	*null_lstrupr (char *string, DBLK *dblk);
extern char	*objdate2fzkstr (DtSrObjdate objdate);
extern struct tm
		*objdate2tm (DtSrObjdate objdate);
extern int	objdate_in_range (DtSrObjdate recdate,
			DtSrObjdate date1, DtSrObjdate date2);
extern int	offline_kbhit (void);
extern int	open_dblk (DBLK **dblist, int numpages, int debugging);
extern LLIST	*pop_llist (LLIST **llistp);
extern void	print_dbrec (char *dbname, struct or_dbrec *dbrec);
extern int	quit_escape(void);
extern UCHAR	readchar_ftext (PARG *parg);
extern UCHAR	readchar_string (UCHAR *string);
extern char	*remove_spaces (char *string);
extern void	replace_ext (char *buffer, int buflen,
		    char *fname, char *fext);
extern LLIST	*sort_llist (LLIST *list_header);
extern char	*teskey_parser (PARG *parg);
extern DtSrObjdate
		tm2objdate (struct tm *tmptr);
#if !defined(__linux__)
#ifndef _ALL_SOURCE
extern char	*strdup (const char *s);
#endif
#endif
extern void	toggle_insert (void);
extern void	unload_custom_language (DBLK *dblk);
extern void	unload_language (DBLK *dblk);
extern void	vista_abort (char *location);
extern char	*vista_msg (char *location);

/************************ SearchP.h ********************************/
#endif  /* _SearchP_h */

