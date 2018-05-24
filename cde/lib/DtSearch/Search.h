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
/* $TOG: Search.h /main/12 1998/04/03 17:10:35 mgreess $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: none
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1994,1995
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
#ifndef _Search_h
#define _Search_h
/********************** Search.h ***********************
 * $TOG: Search.h /main/12 1998/04/03 17:10:35 mgreess $
 * Jan 1994.
 * Public header file for DtSearch/AusText API.
 * Formerly named ausapi.h.  Externals renamed
 * from ausapi_... or aa_... to DtSearch conventions.
 * Documentation in dtsearch.doc.
 *
 */
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE	1
#endif
#include <time.h>
#include <stdio.h>
#include <string.h>

#if defined(__linux__)
# define __SVR4_I386_ABI_L1__
#endif
#include <limits.h>
#if defined(__linux__)
# undef __SVR4_I386_ABI_L1__
# ifndef WORD_BIT
# define WORD_BIT 32
# endif
# ifndef LONG_BIT
# define LONG_BIT 32
# endif
#endif

#define DtSrVERSION		"0.6"
#define DtSrMAXHITS		20
#define DtSrMAX_KTNAME		13
#define DtSrMAX_DB_KEYSIZE	32
#define DtSrMAXWIDTH_HWORD	134
#define DtSrMAX_STEMCOUNT	8

/****************************************/
/*					*/
/*		RETURN CODES		*/
/*					*/
/****************************************/
#define DtSrOK		0	/* normal, successful response */
#define DtSrPASSWD	1	/* failed communications authentication */
#define DtSrNOTAVAIL	2	/* no hits on search, no such rec, etc */
#define DtSrFAIL	3	/* misc unsuccessful engine returns */
#define DtSrREINIT	4	/* engine reinitialized, request canceled */
#define DtSrERROR	5	/* fatal caller (client) program error */
#define DtSrABORT	6	/* fatal engine failure, caller must abort */
#define DtSrCOMM	7	/* fatal communications failure */

/* In order to share databases among heterogenous machines,
 * certain integer types must be defined that are invariably
 * 2 and 4 bytes on all architectures.
 * Note WORD_BIT == 32 does not guarantee short == 16.
 */
#if   (WORD_BIT == 16)
  typedef int		  DtSrINT16;
  typedef unsigned int	  DtSrUINT16;
#elif (WORD_BIT == 32)                                       
  typedef short		  DtSrINT16;    
  typedef unsigned short  DtSrUINT16;
#else                                       
#error Unable to typedef DtSrINT16 and DtSrUINT16
#endif
   
#if   (WORD_BIT == 32)
  typedef int		  DtSrINT32;
  typedef unsigned int	  DtSrUINT32;
#elif (LONG_BIT == 32)
  typedef long		  DtSrINT32;
  typedef unsigned long	  DtSrUINT32;
#else                                                      
#error Unable to typedef DtSrINT32 and DtSrUINT32         
#endif

#ifndef DB_ADDR	
  typedef DtSrINT32	  DB_ADDR;	/* vista.h: must be 4 bytes */
#endif

#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif

typedef DtSrUINT32	DtSrObjdate;


/****************************************/
/*					*/
/*		DtSrKeytype		*/
/*					*/
/****************************************/
typedef struct {
    char	is_selected;	     /* TRUE: user has selected this keytype */
    char	ktchar;		     /* first char of opera record key */
    char	name [DtSrMAX_KTNAME+1]; /* label string used in UI */
}	DtSrKeytype;

/****************************************/
/*					*/
/*		DtSrResult		*/
/*					*/
/****************************************/
/* DtSrResult ormerly DITTO.  DtSrObj types formerly ORT_. */
typedef struct _DtSrResult {
    struct _DtSrResult *link;	/* ptr to next list node */
    DtSrUINT32	flags;		/* (reserved) */
    DtSrINT32	objflags;	/* flags from object record */
#define DtSrFlDELETE    (1<<0)  /* (reserved) */
#define DtSrFlOLDNOTES  (1<<1)  /* (reserved) */
#define DtSrFlNOTAVAIL  (1<<2)  /* object not retrievable from engine */
    DtSrUINT32	objuflags;	/* user flags from database rec */
    DtSrINT32	objsize;	/* in uncompressed bytes */
    DtSrObjdate	objdate;	/* zero means object 'undated' */
    DtSrINT16	objtype;	/* from database record */
#define DtSrObjUNKNOWN	0    /* object type unknown or not applicable */
#define DtSrObjTEXT	1    /* generic, unformatted flat text */
#define DtSrObjBINARY	2    /* generic binary object */
#define DtSrObjSGML	3    /* generic SGML formatted document */
#define DtSrObjHTML	4    /* HTML formatted document */
#define DtSrObjPOSTSCR	6    /* Postscript document */
#define DtSrObjINTERLF	7    /* Interleaf document */
#define DtSrObjDTINFO	8    /* DtInfo document */
    DtSrINT16	objcost;	/* (reserved) */
    int		dbn;		/* dbnamesv index */
    DB_ADDR	dba;		/* data base addr within database */
    DtSrINT16	language;	/* language of the database */
#define DtSrLaENG	0    /* English, ASCII char set (default) */
#define DtSrLaENG2	1    /* English, ISO Latin-1 char set */
#define DtSrLaESP	2    /* Spanish, ISO Latin-1 char set */
#define DtSrLaFRA	3    /* French, ISO Latin-1 char set */
#define DtSrLaITA	4    /* Italian, ISO Latin-1 char set */
#define DtSrLaDEU	5    /* German, ISO Latin-1 char set */
#define DtSrLaJPN	6    /* Japanese, EUC, auto kanji compounds */
#define DtSrLaJPN2	7    /* Japanese, EUC, listed kanji compounds */
#define DtSrLaLAST	7   /* Last supported DtSrLa constant */
    char	reckey [DtSrMAX_DB_KEYSIZE]; /* object's db key */
    int		proximity;	/* 'distance' of object from query */
    char	*abstractp;     /* object's abstract from db*/
}	DtSrResult;

/****************************************/
/*					*/
/*		 DtSrHitword		*/
/*					*/
/****************************************/
typedef struct {
    long	offset;		/* word location in cleartext */
    long	length;		/* length of word */
}	DtSrHitword;


/************************************************/
/*						*/
/*		   Functions			*/
/*						*/
/************************************************/
extern void	DtSearchAddMessage (char *msg);
extern int	DtSearchHasMessages (void);
extern char	*DtSearchGetMessages (void);
extern void	DtSearchFreeMessages (void);
extern int	DtSearchGetMaxResults (void);
extern void	DtSearchSetMaxResults (int new_max_results);
extern char	*DtSearchFormatObjdate (DtSrObjdate objdate);
extern void	DtSearchExit (int return_code);
extern void	DtSearchAddUserExit (void (*user_exit)(int));
extern void	DtSearchRemoveUserExit (void);

extern int	DtSearchSortResults (DtSrResult **list, int sort_type);
  #define DtSrSORT_PROX	1	/* sort on proximity */
  #define DtSrSORT_DATE	2	/* sort on objdate */
extern int	DtSearchMergeResults (DtSrResult **targ, DtSrResult **src);
extern int	DtSearchFreeResults (DtSrResult **list);
extern DtSrObjdate
		DtSearchValidDateString (char *date_string);
extern int	DtSearchInit (
			char	*argv0,
			char	*userid,
			long	switches,
			char	*config_file,
			FILE	*err_file,
			char	***dbnames,
			int	*dbcount);
  /* Bit values valid for switches argument: */
  #define DtSrInNOLOCALE   0x0001L  /* do not exec setlocale() */
  #define DtSrInSIGNAL     0x0002L  /* register abort signal handlers */
  #define DtSrInUSEROCF    0x0004L  /* (reserved) */
  #define DtSrInENAB_NOHUP 0x0008L  /* enab nohup for backgnd progms */
  #define DtSrInIDEBUG     0x0010L  /* set initialization debugging */
  #define DtSrInSDEBUG     0x0020L  /* set search debugging */
  #define DtSrInRDEBUG     0x0040L  /* set retrieval debugging */
  #define DtSrInCDEBUG     0x0080L  /* set communications debugging */
  #define DtSrInANY_DEBUG \
	(DtSrInIDEBUG | DtSrInSDEBUG | DtSrInRDEBUG | DtSrInCDEBUG)
  #define DtSrInRDWR       0x0100L  /* open db files for read and write */

extern int	DtSearchReinit (
	                char		***dbnames,
	                int		*dbcount);

extern int	DtSearchGetKeytypes (
	                char		*dbname,
	                int		*ktcount,
	                DtSrKeytype	**keytypes);

extern int	DtSearchQuery (
			void		*qry,
			char		*dbname,
			int		search_type,
			char   		*date1,
			char   		*date2,
			DtSrResult 	**results,
			long		*resultscount,
			char		*stems,
			int		*stemcount);

extern int	DtSearchRetrieve (
			char		*dbname,
			DB_ADDR		dba,
			char		**cleartext,
			long		*clearlen,
			int		*fzkeyi);

extern int	DtSearchHighlight (
			char		*dbname,
			char		*cleartext,
			DtSrHitword	**hitwptr,
			long		*hitwcount,
	                int		search_type,
			char		*stems,
			int		stemcount);

/********************** Search.h ***********************/
#endif  /* _Search_h */
