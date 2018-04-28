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
/* $XConsortium: dtksh.h /main/4 1996/03/13 13:19:34 rswiston $ */

/*	Copyright (c) 1991, 1992 UNIX System Laboratories, Inc. */
/*	All Rights Reserved     */

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF          */
/*	UNIX System Laboratories, Inc.			        */
/*	The copyright notice above does not evidence any        */
/*	actual or intended publication of such source code.     */

#ifndef _Dtksh_dtksh_h
#define _Dtksh_dtksh_h

#if !defined(NO_AST)
#include "nval.h"
#endif

#define SUCCESS		0
#define FAIL		(-1)

/* bits for the flags field of wtab_t */

#define F_CHILD		1
#define F_TOPLEVEL	2

/*
 * Table types
 */
#define TAB_EMPTY	0
#define TAB_FIELDS	1
#define TAB_ITEMS	2
#define TAB_WIDGET	3


/*
 * Widget entry states
 */
#define DT_PENDING_DESTROY 0x01

typedef struct {
	char *name;
	char *class;
	char *type;
	int size;
} resfixup_t;

typedef struct {
   char * callbackName;
   void * discipline;
} discInfo;

typedef struct {
	char *cname;
	WidgetClass class;	/* Class record */
	resfixup_t  *resfix;   /* fixup list for resources */
	resfixup_t  *confix;   /* fixup list for constraint resources */
	discInfo * disciplines;  /* Callback data environment disciplines */
	char	*res;	/* Hashed list of resources */
	char	*con;	/* Hashed list of constraint resources */
} classtab_t;

typedef struct wtab {
	int    type;		/* entry type (TAB_) */
	int    size;		/* entry size */
	Widget w;		/* widget pointer */ 
	char   *wname;		/* name of widget */
	char   *widid;		/* id of widget */
	classtab_t   *wclass;	/* widget's class */
	struct wtab *parent;	/* pointer to widget's parent wtab_t */
	char *envar;		/* initial environment variable user gave */
	unsigned char mask;	/* keeps track of state; i.e. pending destroy */
	XtPointer info;	/* some widgets use this for any other info */
} wtab_t;

typedef struct {
	char *ksh_cmd;
	wtab_t *w;
	char *cbname;
	int refCount;
	Atom propAtom;
	XtPointer handle;
} dtksh_client_data_t;

typedef struct {
	char *ksh_cmd;
	wtab_t *w;
        EventMask eventMask;
	Boolean nonMaskable;
} dtksh_event_handler_data_t;

typedef struct {
	char *name;
	EventMask mask;
} EventMaskTable;

typedef struct {
	char *name;
	int value;
} MWMTable;

#define LINESIZE 1024

/* Input buffer modes */
#define LINE_INPUT_MODE             (1<<0)
#define RAW_INPUT_MODE              (1<<1)
#define INPUT_SOURCE_BUSY           (1<<2)
#define INPUT_SOURCE_PENDING_DELETE (1<<3)

typedef struct {
	int   fd;		/* the input source */
	char *lnbuf;          	/* a line being built */
	int   lnbufsize;	/* size of line buffer */
	int   lnend;		/* current end of the line */
	char *cmd;		/* the ksh command to execute given the line */
	unsigned char flags;    /* mode flags */
	char  lastCharIsBackslash;  /* Used during backslash processing */
	char  lineWasTouched;   /* Empty line, but had a backslash */
} inputrec_t;

#ifndef CONSTCHAR
#define CONSTCHAR (const char *)
#endif

#define PRINTER_INFO_PROC    0
#define SELECT_FILE_PROC     1
#define SELECT_PRINTER_PROC  2
#define SETUP_PROC           3
#define VERIFY_PRINTER_PROC  4

typedef struct {
	String printerInfoProcCommand;
	String selectFileProcCommand;
	String selectPrinterProcCommand;
	String setupProcCommand;
	String verifyPrinterProcCommand;
} ProcInfo_t;

#endif /* _Dtksh_dtksh_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
