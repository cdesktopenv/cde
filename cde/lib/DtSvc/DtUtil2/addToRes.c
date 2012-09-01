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
/*                                                                            *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                           *
 * (c) Copyright 1993, 1994 International Business Machines Corp.             *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                            *
 * (c) Copyright 1993, 1994 Novell, Inc.                                      *
 */
/*Add a string to the XA_RESOURCE_MANAGER*/
/*
 *                        COPYRIGHT 1987
 *                 DIGITAL EQUIPMENT CORPORATION
 *                     MAYNARD, MASSACHUSETTS
 *                      ALL RIGHTS RESERVED.
 *
 * THE INFORMATION IN THIS SOFTWARE IS SUBJECT TO CHANGE WITHOUT NOTICE AND
 * SHOULD NOT BE CONSTRUED AS A COMMITMENT BY DIGITAL EQUIPMENT CORPORATION.
 * DIGITAL MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THIS SOFTWARE FOR
 * ANY PURPOSE.  IT IS SUPPLIED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
 *
 * IF THE SOFTWARE IS MODIFIED IN A MANNER CREATING DERIVATIVE COPYRIGHT RIGHTS,
 * APPROPRIATE LEGENDS MAY BE PLACED ON THE DERIVATIVE WORK IN ADDITION TO THAT
 * SET FORTH ABOVE.
 *
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Digital Equipment Corporation not be
 * used in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 */

/*Lifted from xrdb(1X)*/
/* -*-C-*-
*******************************************************************************
*
* File:         addToResource.c
* RCS:          $TOG: addToRes.c /main/8 1999/10/14 16:01:17 mgreess $
* Description:  Source code for adding strings to RESOURCE_PROPERTY on 
                  default root window
* Author:       DEC, Robert Williams
* Created:      Thu Apr 26 14:42:08 PDT 1990
* Modified:	Kim Dronesen
* Language:     C
* Package:      N/A
* Status:       Experimental (Do Not Distribute)
*
* (C) Copyright 1990, Hewlett-Packard, all rights reserved.
*
*******************************************************************************
*/
/*$TOG: addToRes.c /main/8 1999/10/14 16:01:17 mgreess $ */

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <Dt/DtP.h>
#include <Dt/SessionM.h>
#include "DtSvcLock.h"

/****************************************/
/* Global variables */
/****************************************/

typedef struct _Entry {
    char *tag, *value;
    int lineno;
    Bool usable;
} Entry;
typedef struct _Buffer {
    char *buff;
    int  room, used;
    Bool freebuff;
} Buffer;
typedef struct _Entries {
    Entry *entry;
    int   room, used;
} Entries;

#define INIT_BUFFER_SIZE 10000
#define INIT_ENTRY_SIZE 500


/********    Public Function Declarations    ********/

static Buffer * _DtAllocBuffer(const char **buff);
static void _DtFreeBuffer(Buffer *b);
static void _DtAppendToBuffer( 
                        register Buffer *b,
                        char *str,
                        register int len) ;
static void _DtAppendEntryToBuffer( 
                        register Buffer *buffer,
                        Entry entry) ;

static Entries * _DtAllocEntries( void) ;
static  void _DtFreeEntries( Entries *) ;
static void _DtAddEntry( 
                        register Entries *e,
                        Entry entry) ;
static int _DtCompareEntries( 
                        Entry *e1,
                        Entry *e2) ;
static char * _DtFindFirst( 
                        register char *string,
                        register char dest) ;
static void _DtGetEntries( 
                        register Entries *entries,
                        Buffer *buff,
                        int dosort) ;
static void _DtMergeEntries( 
                        Buffer *buffer,
                        Entries new,
                        Entries old) ;
static void _DtAddToResProp(
                        Display *dpy,
                        unsigned int id,
                        Entries db) ;
static void _getWinProp(
                        Display *dpy,
                        unsigned int id,
                        Window *win,
                        Atom *prop);

/********    End Public Function Declarations    ********/

/****************************************/
/*The meat*/
/****************************************/

static Buffer *
_DtAllocBuffer(
        const char **buff )
{
    Buffer *b = (Buffer *)malloc(sizeof(Buffer));
    b->room = INIT_BUFFER_SIZE;
    b->buff = buff ? (char*) *buff :
		     (char *)malloc(INIT_BUFFER_SIZE*sizeof(char));
    b->used = buff && *buff ? strlen(*buff) : 0;
    b->freebuff = buff ? False : True;
    return(b);
}

static void
_DtFreeBuffer(
        Buffer *b)
{
    if (b->freebuff == True) free(b->buff);
    free(b);
}

static void 
_DtAppendToBuffer(
        register Buffer *b,
        char *str,
        register int len )
{
    while (b->used + len > b->room) {
        b->buff = (char *)realloc(b->buff, 2*b->room*(sizeof(char)));
        b->room *= 2;
    }
    strncpy(b->buff + b->used, str, len);
    b->used += len;
}

static Entries *
_DtAllocEntries( void )
{
    Entries *e = (Entries *)malloc(sizeof(Entries));
    e->room = INIT_ENTRY_SIZE;
    e->used = 0;
    e->entry = (Entry *)malloc(INIT_ENTRY_SIZE*sizeof(Entry));
    return(e);
}

static void
_DtFreeEntries( 
    Entries *e)
{
    int n = 0;

    while (n < e->used)
    {
	free(e->entry[n].tag);
	free(e->entry[n].value);
	n++;
    }
    free(e->entry);
    free(e);
}

static void 
_DtAddEntry(
        register Entries *e,
        Entry entry )
{
    register int n;

    for (n = 0; n < e->used; n++)
    {
        if (strcmp(e->entry[n].tag, entry.tag) == 0)
	{ /* overwrite old entry  - free its memory first*/
	    free(e->entry[n].tag);
	    free(e->entry[n].value);
            e->entry[n] = entry;
            return ;  /* ok to leave, now there's only one of each tag in db */
        }
    }

    if (e->used == e->room) {
        e->entry = (Entry *)realloc(e->entry, 2*e->room*(sizeof(Entry)));
        e->room *= 2;
    }
    entry.usable = True;
    e->entry[e->used++] = entry;
}

static int 
_DtCompareEntries(
        Entry *e1,
        Entry *e2 )
{
    return strcmp(e1->tag, e2->tag);
}

static void 
_DtAppendEntryToBuffer(
        register Buffer *buffer,
        Entry entry )
{
    _DtAppendToBuffer(buffer, entry.tag, strlen(entry.tag));
    _DtAppendToBuffer(buffer, ":\t", 2);
    _DtAppendToBuffer(buffer, entry.value, strlen(entry.value));
    _DtAppendToBuffer(buffer, "\n", 1);
}

static char * 
_DtFindFirst(
        register char *string,
        register char dest )
{
    int len;

    for (;;) {
        if((len = mblen(string, MB_CUR_MAX)) > 1) {
            string += len;
            continue;
        }
        if (*string == '\0')
            return NULL;
        if (*string == '\\') {
            if (*++string == '\0')
                return NULL;
        }
        else if (*string == dest)
            return string;
        string++;
    }
}

static void 
_DtGetEntries(
        register Entries *entries,
        Buffer *buff,
        int dosort )
{
    register char *line, *colon, *temp, *str, *temp2;
    Entry entry;
    register int length;
    int lineno = 0;

    str = buff->buff;
    if (!str) return;
    for (; str < buff->buff + buff->used; str = line + 1)
    {
        line = _DtFindFirst(str, '\n');
        lineno++;
        if (line == NULL)
            break;
        if (str[0] == '!')
            continue;
        if (str[0] == '\n')
            continue;
        if (str[0] == '#')
	{
            int dummy;
            if (sscanf (str, "# %d", &dummy) == 1) lineno = dummy - 1;
            continue;
        }
        for (temp = str;
             *temp && *temp != '\n' && isascii(*temp) && isspace((u_char)*temp);
             temp++) ;
        if (!*temp || *temp == '\n') continue;

        colon = _DtFindFirst(str, ':');
        if (colon == NULL)
            break;
        if (colon > line)
	{
            line[0] = '\0';
            fprintf (stderr,
                     "%s:  colon missing on line %d, ignoring entry \"%s\"\n",
                     "dtprefs", lineno, str);
            continue;
        }

	temp2 = str;
        while (temp2[0] == ' ' || temp2[0] == '\t')
	{
            temp2++;
        }
        temp = (char *)malloc((length = colon - temp2) + 1);
        strncpy(temp, temp2, length);
        temp[length] = '\0';
        while (temp[length-1] == ' ' || temp[length-1] == '\t')
            temp[--length] = '\0';
        entry.tag = temp;
	

	temp2 = colon + 1;
        while (temp2[0] == ' ' || temp2[0] == '\t')
	{
            temp2++;
        }
        temp = (char *)malloc((length = line - temp2) + 1);
        strncpy(temp, temp2, length);
        temp[length] = '\0';
        entry.value = temp;
        entry.lineno = lineno;

        _DtAddEntry(entries, entry);
    }
    
    if (dosort && (entries->used > 0))
      qsort(entries->entry, entries->used, sizeof(Entry), 
	    (int (*)(const void *, const void *))_DtCompareEntries);
}

static void 
_DtMergeEntries(
        Buffer *buffer,
        Entries new,
        Entries old )
{
    int n, o, cmp;

    buffer->used = 0;
    n = o = 0;
    while ((n < new.used) && (o < old.used))
    {
        cmp = strcmp(new.entry[n].tag, old.entry[o].tag);
        if (cmp > 0)
	{
            _DtAppendEntryToBuffer(buffer, old.entry[o]);
	    o++;
	}
        else
	{
            _DtAppendEntryToBuffer(buffer, new.entry[n]);
	    n++;
            if (cmp == 0)
	    {
                o++;
	    }
        }
    }
    
    while (n < new.used)
    {
        _DtAppendEntryToBuffer(buffer, new.entry[n]);
	n++;
    }
    while (o < old.used)
    {
        _DtAppendEntryToBuffer(buffer, old.entry[o]);
	o++;
    }
    
    _DtAppendToBuffer(buffer, "\0", 1);
}

static void
_getWinProp(
        Display *dpy,
        unsigned int id,
        Window *win,
        Atom *prop)
{
  static Bool init = True;
  static Window winprop;
  static Atom xa_resmgr;
  static Atom xa_prefs;

  _DtSvcProcessLock();
  if (init == True)
  {
    winprop = XRootWindow(dpy, 0);
    xa_resmgr = XA_RESOURCE_MANAGER;
    xa_prefs = XInternAtom (dpy, _XA_DT_SM_PREFERENCES, False);
    init = False;
  }
  _DtSvcProcessUnlock();

  *win = winprop;
  *prop = id == _DT_ATR_RESMGR ? xa_resmgr : xa_prefs;
}


static void 
_DtAddToResProp(
        Display *dpy,
        unsigned int id,
        Entries db)
{
    char *xdefs;
    Buffer *oldBuffer, *newBuffer;
    Entries *oldDB;
    int                 defStatus;
    Atom                actualType;
    int                 actualFormat;
    unsigned long       nitems, leftover;
    Window              win;
    Atom                prop;

   /*
    * Get window and property
    */
    _getWinProp(dpy, id, &win, &prop);
    if (win == (Window)0)
    {
      return;
    }

   /*
    * Get resource database from specified window and property.
    */
    defStatus = XGetWindowProperty(dpy, win,
				   prop, 0L,
				   100000000L,False,XA_STRING,&actualType,
				   &actualFormat,&nitems,&leftover,
				   (unsigned char**) &xdefs);


   /*
    * Allocate oldBuffer and init from resource database string
    */
    oldBuffer = _DtAllocBuffer((const char**) &xdefs);

   /*
    * Allocate oldDB
    */
    oldDB = _DtAllocEntries();

   /*
    * Convert oldBuffer to oldDB.
    */
    _DtGetEntries(oldDB, oldBuffer, 1);

   /*
    * Init empty newBuffer, then populate by merging db into oldDB.
    */
    newBuffer = _DtAllocBuffer(NULL);
    _DtMergeEntries(newBuffer, db, *oldDB);

   /*
    * Finally, store newBuffer into resource database.
    */
    XChangeProperty (dpy, win, prop,
		     XA_STRING, 8, PropModeReplace,
		     (unsigned char *)newBuffer->buff, newBuffer->used);

    XSync(dpy, False);

   /*
    * Free buffer memory
    */
    if (oldBuffer->buff) XFree(oldBuffer->buff);
    _DtFreeBuffer(oldBuffer);
    _DtFreeBuffer(newBuffer);
    _DtFreeEntries(oldDB);
}

char *
_DtGetResString(
        Display *dpy,
        unsigned int id)
{
    char *xdefs;
    Buffer *oldBuffer, *newBuffer;
    Entries *oldDB;
    int                 defStatus;
    Atom                actualType;
    int                 actualFormat;
    unsigned long       nitems, leftover;
    Window              win;
    Atom                prop;

   /*
    * Get window and property
    */
    _getWinProp(dpy, id, &win, &prop);
    if (win == (Window)0)
    {
      return NULL;
    }

   /*
    * Get resource database from specified window and property.
    */
    defStatus = XGetWindowProperty(dpy, win,
				   prop, 0L,
				   100000000L,False,XA_STRING,&actualType,
				   &actualFormat,&nitems,&leftover,
				   (unsigned char**) &xdefs);

    return(xdefs);
}

void 
_DtAddToResource(
	Display *dpy,
	const char *data )
{
  _DtAddResString( dpy, data, _DT_ATR_RESMGR|_DT_ATR_PREFS);
}

void 
_DtAddResString(
        Display *dpy,
        const char *data, 
        unsigned int flags)
{
    char *xdefs;
    int i;
    Buffer *buffer; 
    Entries *newDB;
    int                 defStatus;
    Atom                actualType;
    int                 actualFormat;
    unsigned long       nitems, leftover;

    if((data == NULL) || (*data == '\0'))
    {
	return;
    }
  
   /*
    * Init buffer with input data
    */ 
    buffer = _DtAllocBuffer(&data); 

   /*
    * Init, then populate, newDB from buffer
    */
    newDB = _DtAllocEntries();
    _DtGetEntries(newDB, buffer, 1);

    if (flags & _DT_ATR_RESMGR)
    {
     /*
      * Merge newDB into RESOURCE_MANAGER
      */
      _DtAddToResProp(dpy, _DT_ATR_RESMGR, *newDB);
    }

    if (flags & _DT_ATR_PREFS)
    {
     /*
      * Merge newDB into _DT_SM_PREFERENCES
      */
      _DtAddToResProp(dpy, _DT_ATR_PREFS, *newDB);
    }

   /*
    * Free objects
    */
    _DtFreeBuffer(buffer);
    _DtFreeEntries(newDB);
}
