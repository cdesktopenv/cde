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
 *   FUNCTIONS: none
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1991,1995
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
#ifndef _SearchE_h
#define _SearchE_h
/***************************** SearchE.h ******************************
 * $XConsortium: SearchE.h /main/5 1996/08/12 13:16:47 cde-ibm $
 * August 1991.
 * Header file for online DtSearch Engine (OE.C etc).
 * SearchE.h was formerly called oe.h (Opera Engine).
 * The old OE engine is pretty much invisible now, having
 * been surrounded by the ausapi/DtSearch interface.
 * 
 * The main data structure for passing and receiving arguments
 * between the callers and the engine is USRBLK which is
 * the only argument in the Opera_Engine() call.
 * The actual input and output arguments in the USRBLK for each
 * function are described below with the function description.
 * 
 * All OE_functions pass a return code
 * to the caller.  OE_OK = successful completion,
 * anything else may be partial completion/success or failure.
 * Return code mnemonics are also defined below.
 *
 * $Log$
 * Revision 2.7  1996/03/20  19:11:21  miker
 * Changed prototype of hilite_cleartext().
 *
 * Revision 2.6  1996/03/13  22:34:53  miker
 * Changed char to UCHAR several places.
 *
 * Revision 2.5  1996/03/05  19:17:50  miker
 * oe_unblob no longer converts to uppercase.
 *
 * Revision 2.4  1996/03/05  15:54:06  miker
 * Minor changes to support yacc-based boolean search.
 *
 * Revision 2.3  1996/02/01  16:14:38  miker
 * Obsoleted requests OE_DITTO2KWIC, OE_VALIDATE_PWD, OE_CHANGE_PWD,
 * OE_FINDSTR_REC, OE_FINDSTR_HITL, OE_ASSIST, OE_KILL.
 * Restored OE_PING as the official "null" function.
 * Added USRDBG_HILITE and USRDBG_PARSE.
 * Replaced STRKIND, OE_kind_of_stems, and oe_stems_to_hitwords
 * with hilite_cleartext.  Deleted oe_ditto2kwic, oe_findstr_hitl.
 * Deleted usrblk.kwiclen.
 *
 * Revision 2.2  1995/10/25  22:35:45  miker
 * Added prolog.
 *
 * Revision 2.1  1995/09/22  22:39:39  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.1  1995/08/31  20:42:33  miker
 * Initial revision
 *
 * The DtSearch return codes wrap around the OE return codes.
 */
#include "SearchP.h"

#define AUDIT_FORMAT	"UID=%-8s TIME=%s ELAP=%-3ld DB=%-8s HITS=%-5ld "
#define AUDIT_WHOWHEN	"UID=%-8s TIME=%s"
#define DISCARD_FORMAT	"%s\t\"%s\"\t%s\t%s\n"
#define HARDCOPY_SCRIPT "opprt.bat"
#define MAX_HITWCOUNT	200	/* max number hitwords that can be hilited */
#define WORDS_HITLIMIT	300000L


/*-------------------- Request Codes ------------------------
 * All Engine requests, in addition to input below, require valid
 *     usrblk.userid
 *     usrblk.request.
 * All Engine requests, in addition to output below,
 *     may return ausapi_msglist.
 * There is no zero request code.
 * See list of return codes for the ones that are marked 'common retncodes'.
 */

#define OE_INITIALIZE	1
/* does lotsa stuff--see the function.
 * input:
 * 	.query =	AUSAPI_VERSION of UI code
 * 	.dblist =	data from user config file
 * 	.dblk = 	data from user config file
 * output:
 * 	.dblist =	modified after load of site config file
 * 	.dblk = 	modified after load of site config file
 * 	.retncode =	OE_OK, OE_NOTAVAIL, OE_ABORT
 */

#define OE_TEXT2FZKEY	2
/* converts problem description text to fzkey.
 * input:
 * 	.query =	problem description text
 * 	.dblk = 	desired database and db parameters
 * output:
 * 	.fzkey =	inference from query text analysis
 * 	.retncode =	common retncodes + OE_BAD_QUERY
 */

#define OE_SRCH_FZKEY	3
/* Converts fzkey to hitlist of dba's.
 * Not available for databases without semantic dictionaries.
 * input:
 * 	.fzkey =	database search vector
 * 	.dblk = 	desired database and db parameters
 * output:
 * 	.workproc =	function to be called until !OE_SEARCHING
 * 	.dittolist =	hitlist sorted by proximity
 * 	.dittocount =	# of nodes on dittolist
 * 	.retncode =	common retncodes + OE_SEARCHING, OE_NOTAVAIL,
 * 			OE_USER_STOP, OE_SYSTEM_STOP, OE_BAD_QUERY
 */

#define OE_SRCH_STEMS	4
/* converts string of wordstems and booleans to
 * hitlist of dba's and stems array.
 * input:
 * 	.query =	text string of wordstems and booleans
 * 	.dblk = 	desired database and db parameters
 * output:
 * 	.fzkey = 	(value undefined)
 * 	.workproc =	function to be called until !OE_SEARCHING
 * 	.dittolist =	hitlist
 * 	.dittocount =	# of nodes on dittolist
 * 	.stems =	array of stems for hiliting in text of hits
 * 	.stemcount =	size of 'stems' array
 * 	.retncode =	common retncodes + OE_BAD_QUERY, OE_SEARCHING,
 * 			OE_NOTAVAIL, OE_USER_STOP
 */

#define OE_SRCH_WORDS	5
/* converts string of exact words and booleans to
 * hitlist of dba's and stems array.
 * input:
 * 	.query =	text string of exact words and booleans
 * 	.dblk = 	desired database and db parameters
 * output:
 * 	.fzkey = 	(value undefined)
 * 	.workproc =	function to be called until !OE_SEARCHING
 * 	.dittolist =	hitlist
 * 	.dittocount =	# of nodes on dittolist
 * 	.stems =	array of words for hiliting in text of hits
 * 	.stemcount =	size of 'stems' array
 * 	.retncode =	common retncodes + OE_BAD_QUERY, OE_SEARCHING,
 * 			OE_NOTAVAIL, OE_USER_STOP
 */

#define OE_STOP_SRCH	6
/* sets global switch to cancel search work procedure.
 * workproc actually cancels itself after reading switch.
 * input:	.request =	OE_STOP_SEARCH
 * output:	.retncode =	'common retncodes' only
 */

#define OE_APPEND_NOTES  7
/* appends user's notes to record at current dba.
 * input:
 * 	.query =	freeform text of append
 * 	.dba =		address of record to append
 * 	.dblk = 	desired database and db parameters
 * output:
 * 	.retncode =	common retncodes + OE_TIMEOUT, OE_DISABLED
 */

#define OE_GETREC	8
/* retrieves record, cleartext, and notes for specified dba.
 * Clears hitwords array, does not use or change stems array.
 * input:
 * 	.dba =		address of record to retrieve
 * 	.dblk = 	desired database and db parameters
 * output:
 * 	.objrec =	austext record itself, as is from vista
 * 	.cleartext =	NULL or uncompressed text string
 * 	.clearlen =	0 or size of cleartext
 * 	.hitwords =	array pointer cleared to NULL
 * 	.hitwcount =	size of hitwords array set to 0
 * 	.notes =	NULL, or list of notes records, as is
 * 	.retncode =	common retncodes + OE_NOTAVAIL
 */

#define OE_GETREC_STEMS   9
/* retrieves record, cleartext, notes, and hitwords
 * array for specified dba and stems (from OE_SRCH_STEMS).
 * If no text in repository, equivalent to OE_GETREC.
 * input:
 * 	.dba =		address of record to retrieve
 * 	.stems =	array of word stems to hilite
 * 	.stemcount =	size of stems array
 * 	.dblk = 	desired database and db parameters
 * output:
 * 	.objrec =	austext record itself, as is from vista
 * 	.cleartext =	NULL or uncompressed text string
 * 	.clearlen =	0 or size of cleartext
 * 	.hitwords =	NULL or array of words in cleartext to hilite
 * 	.hitwcount =	0 or size of hitwords array
 * 	.notes =	NULL, or list of notes records, as is
 * 	.retncode =	common retncodes + OE_NOTAVAIL
 */

#define OE_GETREC_WORDS   10
/* retrieves record, cleartext, notes, and hitwords
 * array for specified dba and stems array (from OE_SRCH_WORDS).
 * If no text in repository, equivalent to OE_GETREC.
 * input:
 * 	.dba =		address of record to retrieve
 * 	.stems =	array of exact words to hilite
 * 	.stemcount =	size of stems array
 * 	.dblk = 	desired database and db parameters
 * output:
 * 	.objrec =	austext record itself, as is from vista
 * 	.cleartext =	NULL or uncompressed text string
 * 	.clearlen =	0 or size of cleartext
 * 	.hitwords =	NULL or array of words in cleartext to hilite
 * 	.hitwcount =	0 or size of hitwords array
 * 	.notes =	NULL, or list of notes records, as is
 * 	.retncode =	common retncodes + OE_NOTAVAIL
 */

#define OE_NEXT_DBA	11
/* advances dba to next valid b-tree address.  Wraps if necessary.
 * input:
 * 	.dba =		current address of record
 * 	.dblk = 	desired database and db parameters
 * output:
 * 	.dba =		address of next record in b-tree
 * 	.retncode =	common retncodes + OE_WRAPPED
 */

#define OE_PREV_DBA	12
/* retreats dba to previous valid b-tree address.
 * Wraps if necessary.
 * input:
 * 	.dba =		current address of record
 * 	.dblk = 	desired database and db parameters
 * output:
 * 	.dba =		address of previous record in b-tree
 * 	.retncode =	common retncodes + OE_WRAPPED
 */

#define OE_RECKEY2DBA	13
/* converts an austext record key into a dba.  Wraps if not found.
 * input:
 * 	.query =        desired record key
 * 	.dblk = 	desired database and db parameters
 * output:
 * 	.dba =		address of record found,
 * 			or next avail address if not found
 * 	.retncode =	common retncodes + OE_WRAPPED
 */

#define OE_MARK_DELETION  14
/* writes record id to an external file for
 * possible later deletion by external program.
 * input:
 * 	.query =	record key to be marked for deletion
 * 	.dblk =		database where record is located
 * output:
 * 	.retncode =	common retncodes + OE_NOTAVAIL, OE_DISABLED
 */

#define OE_GETREC_DIC	15
/* THIS FUNCTION IS NO LONGER SUPPORTED.
 * IF RECEIVED BY ENGINE, IT IS TREATED EXACTLY AS OE_GETREC.
 * Retrieves record, cleartext, notes, and hitwords
 * array for specified dba.  Hitwords are not derived from the
 * stems array; they are all the cleartext words that are in
 * the dictionary. (Compare to OE_GETREC_WORDS and OE_GETREC_STEMS).
 * If no text in repository, equivalent to OE_GETREC.
 * input:
 * 	.dba =		address of record to retrieve
 * 	.dblk = 	desired database and db parameters
 * output:
 * 	.objrec =	austext record itself, as is from vista
 * 	.cleartext =	NULL or uncompressed text string
 * 	.clearlen =	0 or size of cleartext
 * 	.hitwords =	NULL or array of words in cleartext to hilite
 * 	.hitwcount =	0 or size of hitwords array
 * 	.notes =	NULL, or list of notes records, as is
 * 	.retncode =	common retncodes + OE_NOTAVAIL
 */

#define OE_DITTO2KWIC	16
#define OE_VALIDATE_PWD	17
#define OE_CHANGE_PWD	18
/* (These functions are obsolete) */

#define OE_DELETE_RECID	19
/* Deletes header record, all text, user notes,
 * and word/stems references for specified record.
 * Currently can only be called from offline program
 * when all online austext users have been logged off.
 * THIS FUNCTION IS <<<VERY>>> SLOW (about 15 min on large dbase)!
 * input:
 * 	.query =        desired record key
 * 	.dblk = 	desired database and db parameters
 * output:
 * 	.dba =		address of record deleted
 * 			THIS ADDRESS IS NO LONGER VALID!
 * 	.dbatab =	(undefined)
 * 	.dbacount =	1
 * 	.retncode =	common retncodes + OE_NOTAVAIL
 */

#define OE_DELETE_BATCH	20
/* Deletes header records, all text, user notes,
 * and word/stems references for all records in a 
 * datbase address table.  Currently can only be called
 * from offline program when all online austext users
 * have been logged off.  This function is the preferred 
 * deletion method because it is faster than deleting
 * single records at a time.  Addresses not found are ignored.
 * input:
 * 	.dblk = 	desired database and db parameters
 * 	.dbatab =       table of valid addresses to be deleted
 * 	.dbacount = 	number of addresses on the table
 * output:
 * 	.retncode =	common retncodes only
 */

#define OE_ASSIST	21
#define OE_FINDSTR_REC	22
#define OE_FINDSTR_HITL	23
/* (These functions are obsolete) */

#define OE_SRCH_STATISTICAL	24
/* Converts string of natural language text to
 * hitlist of dba's and stems array.  Uses stems only,
 * no booleans, all words are ORed together.  Hitlist sorted
 * based on statistics of included word stems.
 * input:
 * 	.query =	natural language text string
 * 	.dblk = 	desired database and db parameters
 * output:
 *	.stems =	array of stems for hiliting in text of hits
 *	.stemcount =	size of 'stems' array (up to max allowed)
 * 	.workproc =	function to be called until !OE_SEARCHING
 * 	.dittolist =	hitlist
 * 	.dittocount =	# of nodes on dittolist
 * 	.retncode =	common retncodes + OE_BAD_QUERY, OE_SEARCHING,
 * 			OE_NOTAVAIL, OE_USER_STOP
 */

#define OE_HILITE_STEMS   25
/* Creates a hitwords array for hiliting using the text
 * in cleartext (however it may have been obtained),
 * and the stems array from the last search.
 * input:
 * 	.cleartext =	text to be hilited
 * 	.clearlen =	size of cleartext
 * 	.stems =	array of words or stems to hilite
 * 	.stemcount =	size of stems array
 *	.search_type =	'W', 'P' or 'S', indicating type of
 *			search that generated stems array.
 * output:
 * 	.hitwords =	array of words in cleartext to hilite
 * 	.hitwcount =	size of hitwords array
 * 	.retncode =	common retncodes + OE_NOTAVAIL, OE_BAD_QUERY
 */

#define OE_GET_EXPIRE	26
/* Returns in 'dba' field the expiration date of OE as a timestamp.
 * Zero means no expiration date.  Overlays previous value in dba.
 * input:	.request =	OE_GET_EXPIRE
 * output:	.dba =		unix timestamp of expiration date or 0	
 * 		.retncode =	common retncodes only
 */

#define OE_KILL		9997
#define OE_PING		9998
#define OE_SHUTDOWN	9999
/* (These functions are obsolete) */

/*-------------------- Return Codes ------------------------
 * The return codes marked 'common retncodes' are OE_OK, OE_NOOP, OE_REINIT,
 * and OE_ABORT, and can be returned by almost all functions.
 * OE_BAD_QUERY will be returned for any unknown function request.
 * There is no zero return code.
 */
#define	OE_OK		1	/* normal successful completion */
#define OE_REINIT	2	/* request canceled: OE reinitialized
				 * databases so UI's dba's may be bad */
#define OE_SEARCHING	3	/* keep calling workproc */
#define OE_BAD_DBLK	4
#define OE_BAD_REQUEST	5	/* invalid request field */
#define OE_BAD_QUERY	6	/* invalid query or other input fld */
#define OE_NOTAVAIL	7	/* no record, hits, function disabled */
#define OE_TIMEOUT	8
#define OE_WRAPPED	9	/* got next item instead of reqstd item */
#define OE_SYSTEM_STOP	10	/* error: search canceled by OE */
#define OE_BAD_PASSWD	11	/* invalid password */
#define OE_BAD_HITLIST	12	/* invalid hitlist */
#define OE_DISABLED	13	/* requested function disabled at this site */
#define OE_USER_STOP	14	/* search canceled by user */
#define OE_BAD_COMM	15	/* request canceled by comm layer */
#define OE_NOOP		888	/* No Operation, nothing done */
#define OE_ABORT	999	/* fatal OE error, OE permanently disabled */


/****************************************/
/*					*/
/*		OEFTAB			*/
/*					*/
/****************************************/
/* Table used in load_ocf() oe_uninitialize() to allow overriding default
 * locations of various files.  Complete discussion in .ocf documentation.
 */
typedef struct
	{
	char	*id;			/* keyword identifier */
	char	**OEFptr;		/* addr of variable to change */
	char	previously_specified;	/* bool ensures only one spec */
	} OEFTAB;

/****************************************/
/*					*/
/*		USRBLK			*/
/*					*/
/****************************************/
typedef struct
    {
    char	userid [10];	/* 1 - 8 alphanumeric char */
    int		search_type;	/* single char = curr search type.
				 * 'T' = Semantic Text search
				 * 'W' = Exact Words search
				 * 'S' = Stems search
				 * 'Z' = Fzkey search
				 * 'N' = Navigator string (unpacked fzk) srch
				 * 'P' = Statistical (Probabilistic) search
				 */

    long	flags;		/* bit switches... */
#define	USR_BIT_1	0x0001L	/* (reserved) */
#define USR_NO_ITERATE	0x0002L	/* override iterations in workprocs */
#define USR_STOPSRCH	0x0004L	/* the "stop" button, cancels workproc */
#define USR_MAXMIN	0x0008L /* symdif() algorithm = fuzzy max min */
#define USR_OBJDATES	0x0010L /* restrict hitlists to objdate ranges */
#define USR_KWIC_ABSTR	0x0020L /* retn KeyWord In Context for abstract */
#define USR_NO_INFOMSGS	0x0040L /* do not retn information-only msgs to UI */
#define USR_MAXHITS_MSG	0x0080L /* show # hits each keytype if sum > maxhits */
#define USR_SORT_WHITL	0x0100L /* sort word/stem hitlists by semantics */

    long	debug;		/* Nonproduction bit switches */
#define USRDBG_RARE	0x0001L	/* 1 Misc initialzatn trace msgs */
#define USRDBG_SRCHCMPL	0x0002L	/* 2 trace ui search_completed functions */
#define USRDBG_RETRVL	0x0004L	/* 4 trace record retrieval funcs */
#define USRDBG_ITERATE	0x0008L	/* 8 forces iteration on all iterable cmds */
#define USRDBG_UTIL	0x0010L	/* 16 trace misc utility functions */
#define USRDBG_MEDPRMPT	0x0020L	/* 32 Prints prompt of medley sockets cmds */
#define USRDBG_HITLIST	0x0040L	/* 64 print hitlists after searches */
#define USRDBG_SYMP	0x0080L	/* 128 trace symptom search funcs */
#define USRDBG_DELETE	0x0100L	/* 256 trace record deletion functions */
#define USRDBG_RPC	0x0200L	/* 512 trace RPC communications funcs */
#define USRDBG_VERBOSE	0x0400L	/* 1024 verbose debugging: iterative details */
#define USRDBG_HILITE	0x0800L	/* 2048 trace hiliting functions */
#define USRDBG_PARSE	0x1000L	/* 4096 trace linguistic parse/stem funcs */
#define USRDBG_BOOL	0x2000L	/* 8192 trace boolean parse funcs */

    int		request;
    int		retncode;
    char	*query;		  /* input data for text searches */
    DtSrObjdate	objdate1;	  /* only retn hit objects >= (after) date1 */
    DtSrObjdate	objdate2;	  /* only retn hit objects <= (before) date2 */
    DB_ADDR	dba;              /* for direct dba reads */
    DB_ADDR	*dbatab;	  /* array of dba's for batch deletes */
    int		dbacount;	  /* # of dba's in dbatab */
    void	(*workproc) (void);
	/* (1) If single tasking (iterative), OE places ptr to work
	procedure.  (2) If multitasking (no iterations), UI places
	ptr of func to call when OE's spawned subtask is done. */

    DBLK	*dblist;	/* linked list of all databases */
    DBLK	*dblk;		/* users curr database selection */

    DtSrResult	*dittolist;	/* hitlist retnd from various searches */
    long	dittocount;	/* # of items on hitlist */
    int		stemcount;	/* # of wordstems in 'stems' array */
    char	stems [DtSrMAX_STEMCOUNT] [DtSrMAXWIDTH_HWORD];
				/* for hiliting words in text records */

    struct or_objrec  objrec;	/* austext record buffer */
    char	*abstrbuf;	/* buf to hold abstracts */
    int		abstrbufsz;	/* maximum abstract size all databases */
    char	*cleartext;	/* decompressed austext record text */
    long	clearlen;	/* size of cleartext in bytes */
    LLIST	*notes;		/* uncompressed, right out of vista */
    DtSrHitword	*hitwords;	/* array of hit words inside cleartext */
    long	hitwcount;	/* number of elements in hitwords array */

    }	 USRBLK;



/****************************************/
/*					*/
/*		SAVEUSR			*/
/*					*/
/****************************************/
/* The following data is saved between calls of the workprocs.
 * In a future msg passing protocol where there may be multiple UIs
 * per OE, this data would be maintained in a list of structures,
 * one for each currently active search (UI only passes user id name
 * in iterative calls, OE searches list to match curr status).
 * But for now, this will have to do.
 */
typedef struct
    {
    time_t	start_time;
    int		iterations;
    int		vistano;
    DtSrResult	*dittolist;
    long	dittocount;
    int		stemcount;
    char	stems [DtSrMAX_STEMCOUNT] [DtSrMAXWIDTH_HWORD];
    char	ktchars [MAX_KTCOUNT + 2];
    char	*lastqry;
    long	ktsum [MAX_KTCOUNT + 1];
    }  SAVEUSR;


/*--------------- GLOBALS in oe.c, loadocf.c -------------------*/
extern char	**ausapi_dbnamesv;
extern int	ausapi_dbnamesc;
extern USRBLK	usrblk;

extern char	*global_memory_ptr;
extern OEFTAB	oef_table[];
extern SAVEUSR	saveusr;	/* (only one for now) */
extern int	shm_id;

extern int	OE_bmhtab_strlen [DtSrMAX_STEMCOUNT];
extern size_t	OE_bmhtables [DtSrMAX_STEMCOUNT] [MAX_BMHTAB];
extern int	OE_dbn;
extern int	OE_enable_markdel;
extern int	OE_enable_usernotes;
extern time_t	*OE_expiration;
extern int	OE_fastdecode;
extern char	*OE_fileio;
extern long	OE_flags;
#define OE_AUDIT      1L	/* enables audit file logging */
#define OE_INITOK     (1L<<1)	/* ensures first reqst was INITIALIZE */
#define OE_PERMERR    (1L<<2)	/* disables engine on fatal errors */
#define OE_NO_ITERATE (1L<<3)	/* override iterations in workprocs */
extern char	*OE_inittab_dir;	/* local dir of server daemon */
extern long	OE_objsize;
extern char	*OE_prodname;
extern int	OE_search_type;
extern char	*OE_server_dir;		/* local dir of server daemon */
extern char	*OE_sitecnfg_fname;
extern time_t	OE_sitecnfg_mtime;
extern int	OE_uppercase_keys;
extern long	OE_words_hitlimit;

/* Global pointers to formerly hardcoded path/file names.
 * The comment names the #define constant under which the
 * filename is/was specified in either fuzzy.h or oe.h.
 */
extern char	*OEF_audit;	/* FNAME_AUDIT */
extern char	*OEF_discard;	/* FNAME_DISCARD_DATA */
extern char	*OEF_news;	/* FNAME_SITENEWS */
extern char	*OEF_notesnot;	/* FNAME_NOTES_BAC */
extern char	*OEF_notessem;	/* FNAME_NOTES_SEM */
extern char	*OEF_readme;	/* FNAME_README */

/*---------------- FUNCTION PROTOTYPES ----------------------*/
extern char	*calloe_getrec (char *dbname, DB_ADDR dba,
			LLIST **global_msglist);
extern long	calloe_hilite (char *cleartext, DtSrHitword *hitwords,
			LLIST **global_msglist);
extern DtSrResult
		*calloe_search (char *qry, char *dbname,
			int search_type, LLIST **global_msglist);
extern int	call_output_script (char *shellcmd, char *text);
extern void	clear_hitwords (void);
extern void	clear_usrblk_record (void);
extern void	dummy_workproc (void);
extern char	*ensure_end_slash (char *charbuf);
extern void	fasthuf (UCHAR *input_bitstring, UCHAR *output_charbuf,
			int outbuf_size, time_t encode_id);
extern char	*get_hitlist_text (int maxlen);
extern long	hilite_cleartext (int parse_type, char *stems, int stemcount);
extern int	load_ocf (void);
extern char	*nowstring (time_t *now);
extern void	oe_initialize (void);
extern int	oe_unblob (LLIST *bloblist);
extern void	oe_write_audit_rec (long numhits);
extern void	Opera_Engine (void);
extern void	print_dittolist (DtSrResult *dittolist, char *label);
extern void	print_stems (int stemcount, void *stems, char *locstr);
extern void	print_usrblk_record (char *label);
extern void	release_shm_mem (void);
extern char	*retncode_str (int num);
extern void	symptom_search (void);
extern int	ve_append_notes (void);
extern void	ve_browse_dba (int direction);
extern LLIST	*ve_getblobs (DtSrINT32 dba, int vistano);
extern int	ve_getrec_dba (LLIST **bloblist);
extern int	ve_initialize (void);
extern void	ve_ditto (void);
extern DtSrINT32 ve_reckey2dba (void);
extern void	ve_statistical (void);
extern void	ve_stem_search (void);
extern void	ve_word_search (void);
extern void	ve_shutdown (void);

/***************************** SearchE.h ******************************/
#endif   /* _SearchE_h */
