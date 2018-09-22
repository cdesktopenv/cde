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
/* Xrm.c 1.1 - Fujitsu source for CDEnext    95/11/06 20:32:11 	*/
/* $XConsortium: _falrm.c /main/1 1996/04/08 15:21:43 cde-fuj $ */

/***********************************************************
Copyright 1987, 1988, 1990 by Digital Equipment Corporation, Maynard

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/*

Copyright (c) 1987, 1988, 1990  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.

*/

#include	<stdio.h>
#include	<ctype.h>
#include	"_fallibint.h"
#include	<X11/Xresource.h>
#include	"_fallcint.h"
#ifdef XTHREADS
#include	"_falloking.h"
#endif
#include 	"_falrmI.h"
#include	<X11/Xos.h>

/*

These Xrm routines allow very fast lookup of resources in the resource
database.  Several usage patterns are exploited:

(1) Widgets get a lot of resources at one time.  Rather than look up each from
scratch, we can precompute the prioritized list of database levels once, then
search for each resource starting at the beginning of the list.

(2) Many database levels don't contain any leaf resource nodes.  There is no
point in looking for resources on a level that doesn't contain any.  This
information is kept on a per-level basis.

(3) Sometimes the widget instance tree is structured such that you get the same
class name repeated on the fully qualified widget name.  This can result in the
same database level occurring multiple times on the search list.  The code below
only checks to see if you get two identical search lists in a row, rather than
look back through all database levels, but in practice this removes all
duplicates I've ever observed.

Joel McCormack

*/

/*

The Xrm representation has been completely redesigned to substantially reduce
memory and hopefully improve performance.

The database is structured into two kinds of tables: LTables that contain
only values, and NTables that contain only other tables.

Some invariants:

The next pointer of the top-level node table points to the top-level leaf
table, if any.

Within an LTable, for a given name, the tight value always precedes the
loose value, and if both are present the loose value is always right after
the tight value.

Within an NTable, all of the entries for a given name are contiguous,
in the order tight NTable, loose NTable, tight LTable, loose LTable.

Bob Scheifler

*/

typedef unsigned long Signature;

static XrmQuark XrmQString, XrmQANY;

typedef	Bool (*DBEnumProc)(
/* this is Nested on purpose, to match Xlib.h */
    XrmDatabase*	/* db */,
    XrmBindingList	/* bindings */,
    XrmQuarkList	/* quarks */,
    XrmRepresentation*	/* type */,
    XrmValue*		/* value */,
    XPointer		/* closure */
);

typedef struct _VEntry {
    struct _VEntry	*next;		/* next in chain */
    XrmQuark		name;		/* name of this entry */
    unsigned int	tight:1;	/* 1 if it is a tight binding */
    unsigned int	string:1;	/* 1 if type is String */
    unsigned int	size:30;	/* size of value */
} VEntryRec, *VEntry;


typedef struct _DEntry {
    VEntryRec		entry;		/* entry */
    XrmRepresentation	type;		/* representation type */
} DEntryRec, *DEntry;

/* the value is right after the structure */
#define StringValue(ve) (XPointer)((ve) + 1)
#define RepType(ve) ((DEntry)(ve))->type
/* the value is right after the structure */
#define DataValue(ve) (XPointer)(((DEntry)(ve)) + 1)
#define RawValue(ve) (char *)((ve)->string ? StringValue(ve) : DataValue(ve))

typedef struct _NTable {
    struct _NTable	*next;		/* next in chain */
    XrmQuark		name;		/* name of this entry */
    unsigned int	tight:1;	/* 1 if it is a tight binding */
    unsigned int	leaf:1;		/* 1 if children are values */
    unsigned int	hasloose:1;	/* 1 if has loose children */
    unsigned int	hasany:1;	/* 1 if has ANY entry */
    unsigned int	pad:4;		/* unused */
    unsigned int	mask:8;		/* hash size - 1 */
    unsigned int	entries:16;	/* number of children */
} NTableRec, *NTable;

/* the buckets are right after the structure */
#define NodeBuckets(ne) ((NTable *)((ne) + 1))
#define NodeHash(ne,q) NodeBuckets(ne)[(q) & (ne)->mask]

/* leaf tables have an extra level of indirection for the buckets,
 * so that resizing can be done without invalidating a search list.
 * This is completely ugly, and wastes some memory, but the Xlib
 * spec doesn't really specify whether invalidation is OK, and the
 * old implementation did not invalidate.
 */
typedef struct _LTable {
    NTableRec		table;
    VEntry		*buckets;
} LTableRec, *LTable;

#define LeafHash(le,q) (le)->buckets[(q) & (le)->table.mask]

/* An XrmDatabase just holds a pointer to the first top-level table.
 * The type name is no longer descriptive, but better to not change
 * the Xresource.h header file.  This type also gets used to define
 * XrmSearchList, which is a complete crock, but we'll just leave it
 * and caste types as required.
 */
typedef struct _XrmHashBucketRec {
    NTable table;
    XPointer mbstate;
    XrmMethods methods;
#ifdef XTHREADS
    LockInfoRec linfo;
#endif
} XrmHashBucketRec;

/* closure used in get/put resource */
typedef struct _VClosure {
    XrmRepresentation	*type;		/* type of value */
    XrmValuePtr		value;		/* value itself */
} VClosureRec, *VClosure;

/* closure used in get search list */
typedef struct _SClosure {
    LTable		*list;		/* search list */
    int			idx;		/* index of last filled element */
    int			limit;		/* maximum index */
} SClosureRec, *SClosure;

/* placed in XrmSearchList to indicate next table is loose only */
#define LOOSESEARCH ((LTable)1)

/* closure used in enumerate database */
typedef struct _EClosure {
    XrmDatabase db;			/* the database */
    DBEnumProc proc;			/* the user proc */
    XPointer closure;			/* the user closure */
    XrmBindingList bindings;		/* binding list */
    XrmQuarkList quarks;		/* quark list */
    int mode;				/* XrmEnum<kind> */
} EClosureRec, *EClosure;

/* predicate to determine when to resize a hash table */
#define GrowthPred(n,m) ((unsigned)(n) > (((m) + 1) << 2))

#define GROW(prev) \
    if (GrowthPred((*prev)->entries, (*prev)->mask)) \
	GrowTable(prev)

/* pick a reasonable value for maximum depth of resource database */
#define MAXDBDEPTH 100

/* macro used in get/search functions */

/* find an entry named ename, with leafness given by leaf */
#define NFIND(ename) \
    q = ename; \
    entry = NodeHash(table, q); \
    while (entry && entry->name != q) \
	entry = entry->next; \
    if (leaf && entry && !entry->leaf) { \
	entry = entry->next; \
	if (entry && !entry->leaf) \
	    entry = entry->next; \
	if (entry && entry->name != q) \
	    entry = (NTable)NULL; \
    }

/* resourceQuarks keeps track of what quarks have been associated with values
 * in all LTables.  If a quark has never been used in an LTable, we don't need
 * to bother looking for it.
 */

static unsigned char *resourceQuarks = (unsigned char *)NULL;
static XrmQuark maxResourceQuark = -1;

/* determines if a quark has been used for a value in any database */
#define IsResourceQuark(q)  ((q) > 0 && (q) <= maxResourceQuark && \
			     resourceQuarks[(q) >> 3] & (1 << ((q) & 7)))

typedef unsigned char XrmBits;

#define BSLASH  ((XrmBits) (1 << 5))
#define NORMAL	((XrmBits) (1 << 4))
#define EOQ	((XrmBits) (1 << 3))
#define SEP	((XrmBits) (1 << 2))
#define ENDOF	((XrmBits) (1 << 1))
#define SPACE	(NORMAL|EOQ|SEP|(XrmBits)0)
#define RSEP	(NORMAL|EOQ|SEP|(XrmBits)1)
#define EOS	(EOQ|SEP|ENDOF|(XrmBits)0)
#define EOL	(EOQ|SEP|ENDOF|(XrmBits)1)
#define BINDING	(NORMAL|EOQ)
#define ODIGIT	(NORMAL|(XrmBits)1)

#define next_char(ch,str) xrmtypes[(unsigned char)((ch) = *(++(str)))]
#define next_mbchar(ch,len,str) xrmtypes[(unsigned char)(ch = (*db->methods->mbchar)(db->mbstate, str, &len), str += len, ch)]

#define is_space(bits)		((bits) == SPACE)
#define is_EOQ(bits)		((bits) & EOQ)
#define is_EOF(bits)		((bits) == EOS)
#define is_EOL(bits)		((bits) & ENDOF)
#define is_binding(bits)	((bits) == BINDING)
#define is_odigit(bits)		((bits) == ODIGIT)
#define is_separator(bits)	((bits) & SEP)
#define is_nonpcs(bits)		(!(bits))
#define is_normal(bits)		((bits) & NORMAL)
#define is_simple(bits)		((bits) & (NORMAL|BSLASH))
#define is_special(bits)	((bits) & (ENDOF|BSLASH))

/* parsing types */
static XrmBits const xrmtypes[256] = {
    EOS,0,0,0,0,0,0,0,
    0,SPACE,EOL,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    SPACE,NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,
    NORMAL,NORMAL,BINDING,NORMAL,NORMAL,NORMAL,BINDING,NORMAL,
    ODIGIT,ODIGIT,ODIGIT,ODIGIT,ODIGIT,ODIGIT,ODIGIT,ODIGIT,
    NORMAL,NORMAL,RSEP,NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,
    NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,
    NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,
    NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,
    NORMAL,NORMAL,NORMAL,NORMAL,BSLASH,NORMAL,NORMAL,NORMAL,
    NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,
    NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,
    NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,
    NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,NORMAL,0
    /* The rest will be automatically initialized to zero. */
};

void falrmInitialize(void)
{
    XrmQString = falrmPermStringToQuark("String");
    XrmQANY = falrmPermStringToQuark("?");
}

XrmDatabase falrmGetDatabase(display)
    Display *display;
{
    XrmDatabase retval;
    LockDisplay(display);
    retval = display->db;
    UnlockDisplay(display);
    return retval;
}

void falrmSetDatabase(Display *display, XrmDatabase database)
{
    LockDisplay(display);
    display->db = database;
    UnlockDisplay(display);
}

void falrmStringToQuarkList(const char *name, XrmQuarkList quarks)/*RETURN*/
{
    XrmBits	bits;
    Signature  	sig = 0;
    char       	ch, *tname;
    int 	i = 0;

    if (tname = (char *)name) {
	tname--;
	while (!is_EOF(bits = next_char(ch, tname))) {
	    if (is_binding (bits)) {
		if (i) {
		    /* Found a complete name */
		    *quarks++ = _falrmInternalStringToQuark(name,tname - name,
							  sig, False);
		    i = 0;
		    sig = 0;
		}
		name = tname+1;
	    }
	    else {
		sig = (sig << 1) + ch; /* Compute the signature. */
		i++;
	    }
	}
	*quarks++ = _falrmInternalStringToQuark(name, tname - name, sig, False);
    }
    *quarks = NULLQUARK;
}

void falrmStringToBindingQuarkList(
    const char   *name,
    XrmBindingList bindings,   /* RETURN */
    XrmQuarkList   quarks)     /* RETURN */
{
    XrmBits	bits;
    Signature  	sig = 0;
    char       	ch, *tname;
    XrmBinding 	binding;
    int 	i = 0;

    if (tname = (char *)name) {
	tname--;
	binding = XrmBindTightly;
	while (!is_EOF(bits = next_char(ch, tname))) {
	    if (is_binding (bits)) {
		if (i) {
		    /* Found a complete name */
		    *bindings++ = binding;
		    *quarks++ = _falrmInternalStringToQuark(name, tname - name,
							  sig, False);

		    i = 0;
		    sig = 0;
		    binding = XrmBindTightly;
		}
		name = tname+1;

		if (ch == '*')
		    binding = XrmBindLoosely;
	    }
	    else {
		sig = (sig << 1) + ch; /* Compute the signature. */
		i++;
	    }
	}
	*bindings = binding;
	*quarks++ = _falrmInternalStringToQuark(name, tname - name, sig, False);
    }
    *quarks = NULLQUARK;
}

#ifdef DEBUG

static void PrintQuarkList(XrmQuarkList quarks, FILE *stream)
{
    Bool	    firstNameSeen;

    for (firstNameSeen = False; *quarks; quarks++) {
	if (firstNameSeen) {
	    (void) fprintf(stream, ".");
	}
	firstNameSeen = True;
	(void) fputs(falrmQuarkToString(*quarks), stream);
    }
} /* PrintQuarkList */

#endif /* DEBUG */

/*ARGSUSED*/
static void mbnoop(XPointer state)
{
}

/*ARGSUSED*/
static char mbchar(XPointer state, char *str, int *lenp)
{
    *lenp = 1;
    return *str;
}

/*ARGSUSED*/
static char *lcname(XPointer state)
{
    return "C";
}

static const XrmMethodsRec mb_methods = {
    mbnoop,
    mbchar,
    mbnoop,
    lcname,
    mbnoop
};

static XrmDatabase NewDatabase(void)
{
    XrmDatabase db;

    db = (XrmDatabase) Xmalloc(sizeof(XrmHashBucketRec));
    if (db) {
	_XCreateMutex(&db->linfo);
	db->table = (NTable)NULL;
	db->methods = _falrmInitParseInfo(&db->mbstate);
	if (!db->methods)
	    db->methods = (XrmMethods)&mb_methods;
    }
    return db;
}

/* move all values from ftable to ttable, and free ftable's buckets.
 * ttable is guaranteed empty to start with.
 */
static void MoveValues(LTable ftable, LTable ttable)
{
    VEntry fentry, nfentry;
    VEntry *prev;
    VEntry *bucket;
    VEntry tentry;
    int i;

    for (i = ftable->table.mask, bucket = ftable->buckets; i >= 0; i--) {
	for (fentry = *bucket++; fentry; fentry = nfentry) {
	    prev = &LeafHash(ttable, fentry->name);
	    tentry = *prev;
	    *prev = fentry;
	    /* chain on all with same name, to preserve invariant order */
	    while ((nfentry = fentry->next) && nfentry->name == fentry->name)
		fentry = nfentry;
	    fentry->next = tentry;
	}
    }
    Xfree((char *)ftable->buckets);
}

/* move all tables from ftable to ttable, and free ftable.
 * ttable is quaranteed empty to start with.
 */
static void MoveTables(NTable ftable, NTable ttable)
{
    NTable fentry, nfentry;
    NTable *prev;
    NTable *bucket;
    NTable tentry;
    int i;

    for (i = ftable->mask, bucket = NodeBuckets(ftable); i >= 0; i--) {
	for (fentry = *bucket++; fentry; fentry = nfentry) {
	    prev = &NodeHash(ttable, fentry->name);
	    tentry = *prev;
	    *prev = fentry;
	    /* chain on all with same name, to preserve invariant order */
	    while ((nfentry = fentry->next) && nfentry->name == fentry->name)
		fentry = nfentry;
	    fentry->next = tentry;
	}
    }
    Xfree((char *)ftable);
}

/* grow the table, based on current number of entries */
static void GrowTable(NTable *prev)
{
    NTable table;
    int i;

    table = *prev;
    i = table->mask;
    if (i == 255) /* biggest it gets */
	return;
    while (i < 255 && GrowthPred(table->entries, i))
	i = (i << 1) + 1;
    i++; /* i is now the new size */
    if (table->leaf) {
	LTable ltable;
	LTableRec otable;

	ltable = (LTable)table;
	/* cons up a copy to make MoveValues look symmetric */
	otable = *ltable;
	ltable->buckets = (VEntry *)Xmalloc(i * sizeof(VEntry));
	if (!ltable->buckets) {
	    ltable->buckets = otable.buckets;
	    return;
	}
	ltable->table.mask = i - 1;
	bzero((char *)ltable->buckets, i * sizeof(VEntry));
	MoveValues(&otable, ltable);
    } else {
	NTable ntable;

	ntable = (NTable)Xmalloc(sizeof(NTableRec) + i * sizeof(NTable));
	if (!ntable)
	    return;
	*ntable = *table;
	ntable->mask = i - 1;
	bzero((char *)NodeBuckets(ntable), i * sizeof(NTable));
	*prev = ntable;
	MoveTables(table, ntable);
    }
}

/* merge values from ftable into *pprev, destroy ftable in the process */
static void MergeValues(LTable ftable, NTable *pprev, Bool override)
{
    VEntry fentry, tentry;
    VEntry *prev;
    LTable ttable;
    VEntry *bucket;
    int i;
    XrmQuark q;

    ttable = (LTable)*pprev;
    if (ftable->table.hasloose)
	ttable->table.hasloose = 1;
    for (i = ftable->table.mask, bucket = ftable->buckets;
	 i >= 0;
	 i--, bucket++) {
	for (fentry = *bucket; fentry; ) {
	    q = fentry->name;
	    prev = &LeafHash(ttable, q);
	    tentry = *prev;
	    while (tentry && tentry->name != q)
		tentry = *(prev = &tentry->next);
	    /* note: test intentionally uses fentry->name instead of q */
	    /* permits serendipitous inserts */
	    while (tentry && tentry->name == fentry->name) {
		/* if tentry is earlier, skip it */
		if (!fentry->tight && tentry->tight) {
		    tentry = *(prev = &tentry->next);
		    continue;
		}
		if (fentry->tight != tentry->tight) {
		    /* no match, chain in fentry */
		    *prev = fentry;
		    prev = &fentry->next;
		    fentry = *prev;
		    *prev = tentry;
		    ttable->table.entries++;
		} else if (override) {
		    /* match, chain in fentry, splice out and free tentry */
		    *prev = fentry;
		    prev = &fentry->next;
		    fentry = *prev;
		    *prev = tentry->next;
		    /* free the overridden entry */
		    Xfree((char *)tentry);
		    /* get next tentry */
		    tentry = *prev;
		} else {
		    /* match, discard fentry */
		    prev = &tentry->next;
		    tentry = fentry; /* use as a temp var */
		    fentry = fentry->next;
		    /* free the overpowered entry */
		    Xfree((char *)tentry);
		    /* get next tentry */
		    tentry = *prev;
		}
		if (!fentry)
		    break;
	    }
	    /* at this point, tentry cannot match any fentry named q */
	    /* chain in all bindings together, preserve invariant order */
	    while (fentry && fentry->name == q) {
		*prev = fentry;
		prev = &fentry->next;
		fentry = *prev;
		*prev = tentry;
		ttable->table.entries++;
	    }
	}
    }
    Xfree((char *)ftable->buckets);
    Xfree((char *)ftable);
    /* resize if necessary, now that we're all done */
    GROW(pprev);
}

/* merge tables from ftable into *pprev, destroy ftable in the process */
static void MergeTables(NTable ftable, NTable *pprev, Bool override)
{
    NTable fentry, tentry;
    NTable nfentry;
    NTable *prev;
    NTable ttable;
    NTable *bucket;
    int i;
    XrmQuark q;

    ttable = *pprev;
    if (ftable->hasloose)
	ttable->hasloose = 1;
    if (ftable->hasany)
	ttable->hasany = 1;
    for (i = ftable->mask, bucket = NodeBuckets(ftable);
	 i >= 0;
	 i--, bucket++) {
	for (fentry = *bucket; fentry; ) {
	    q = fentry->name;
	    prev = &NodeHash(ttable, q);
	    tentry = *prev;
	    while (tentry && tentry->name != q)
		tentry = *(prev = &tentry->next);
	    /* note: test intentionally uses fentry->name instead of q */
	    /* permits serendipitous inserts */
	    while (tentry && tentry->name == fentry->name) {
		/* if tentry is earlier, skip it */
		if ((fentry->leaf && !tentry->leaf) ||
		    (!fentry->tight && tentry->tight &&
		     (fentry->leaf || !tentry->leaf))) {
		    tentry = *(prev = &tentry->next);
		    continue;
		}
		nfentry = fentry->next;
		if (fentry->leaf != tentry->leaf ||
		    fentry->tight != tentry->tight) {
		    /* no match, just chain in */
		    *prev = fentry;
		    *(prev = &fentry->next) = tentry;
		    ttable->entries++;
		} else {
		    if (fentry->leaf)
			MergeValues((LTable)fentry, prev, override);
		    else
			MergeTables(fentry, prev, override);
		    /* bump to next tentry */
		    tentry = *(prev = &(*prev)->next);
		}
		/* bump to next fentry */
		fentry = nfentry;
		if (!fentry)
		    break;
	    }
	    /* at this point, tentry cannot match any fentry named q */
	    /* chain in all bindings together, preserve invariant order */
	    while (fentry && fentry->name == q) {
		*prev = fentry;
		prev = &fentry->next;
		fentry = *prev;
		*prev = tentry;
		ttable->entries++;
	    }
	}
    }
    Xfree((char *)ftable);
    /* resize if necessary, now that we're all done */
    GROW(pprev);
}

void falrmCombineDatabase(XrmDatabase from, XrmDatabase *into, Bool override)
{
    NTable *prev;
    NTable ftable, ttable, nftable;

    if (!*into) {
	*into = from;
    } else if (from) {
	_XLockMutex(&from->linfo);
	_XLockMutex(&(*into)->linfo);
	if (ftable = from->table) {
	    prev = &(*into)->table;
	    ttable = *prev;
	    if (!ftable->leaf) {
		nftable = ftable->next;
		if (ttable && !ttable->leaf) {
		    /* both have node tables, merge them */
		    MergeTables(ftable, prev, override);
		    /* bump to into's leaf table, if any */
		    ttable = *(prev = &(*prev)->next);
		} else {
		    /* into has no node table, link from's in */
		    *prev = ftable;
		    *(prev = &ftable->next) = ttable;
		}
		/* bump to from's leaf table, if any */
		ftable = nftable;
	    } else {
		/* bump to into's leaf table, if any */
		if (ttable && !ttable->leaf)
		    ttable = *(prev = &ttable->next);
	    }
	    if (ftable) {
		/* if into has a leaf, merge, else insert */
		if (ttable)
		    MergeValues((LTable)ftable, prev, override);
		else
		    *prev = ftable;
	    }
	}
	(from->methods->destroy)(from->mbstate);
	_XFreeMutex(&from->linfo);
	Xfree((char *)from);
	_XUnlockMutex(&(*into)->linfo);
    }
}

void falrmMergeDatabases(XrmDatabase from, XrmDatabase *into)
{
    falrmCombineDatabase(from, into, True);
}

/* store a value in the database, overriding any existing entry */
static void PutEntry(
    XrmDatabase		db,
    XrmBindingList	bindings,
    XrmQuarkList	quarks,
    XrmRepresentation	type,
    XrmValuePtr		value)
{
    NTable *pprev, *prev;
    NTable table;
    XrmQuark q;
    VEntry *vprev;
    VEntry entry;
    NTable *nprev, *firstpprev;

#define NEWTABLE(q,i) \
    table = (NTable)Xmalloc(sizeof(LTableRec)); \
    if (!table) \
	return; \
    table->name = q; \
    table->hasloose = 0; \
    table->hasany = 0; \
    table->mask = 0; \
    table->entries = 0; \
    if (quarks[i]) { \
	table->leaf = 0; \
	nprev = NodeBuckets(table); \
    } else { \
	table->leaf = 1; \
	if (!(nprev = (NTable *)Xmalloc(sizeof(VEntry *)))) \
	    return; \
	((LTable)table)->buckets = (VEntry *)nprev; \
    } \
    *nprev = (NTable)NULL; \
    table->next = *prev; \
    *prev = table

    if (!db || !*quarks)
	return;
    table = *(prev = &db->table);
    /* if already at leaf, bump to the leaf table */
    if (!quarks[1] && table && !table->leaf)
	table = *(prev = &table->next);
    pprev = prev;
    if (!table || (quarks[1] && table->leaf)) {
	/* no top-level node table, create one and chain it in */
	NEWTABLE(NULLQUARK,1);
	table->tight = 1; /* arbitrary */
	prev = nprev;
    } else {
	/* search along until we need a value */
	while (quarks[1]) {
	    q = *quarks;
	    table = *(prev = &NodeHash(table, q));
	    while (table && table->name != q)
		table = *(prev = &table->next);
	    if (!table)
		break; /* not found */
	    if (quarks[2]) {
		if (table->leaf)
		    break; /* not found */
	    } else {
		if (!table->leaf) {
		    /* bump to leaf table, if any */
		    table = *(prev = &table->next);
		    if (!table || table->name != q)
			break; /* not found */
		    if (!table->leaf) {
			/* bump to leaf table, if any */
			table = *(prev = &table->next);
			if (!table || table->name != q)
			    break; /* not found */
		    }
		}
	    }
	    if (*bindings == XrmBindTightly) {
		if (!table->tight)
		    break; /* not found */
	    } else {
		if (table->tight) {
		    /* bump to loose table, if any */
		    table = *(prev = &table->next);
		    if (!table || table->name != q ||
			!quarks[2] != table->leaf)
			break; /* not found */
		}
	    }
	    /* found that one, bump to next quark */
	    pprev = prev;
	    quarks++;
	    bindings++;
	}
	if (!quarks[1]) {
	    /* found all the way to a leaf */
	    q = *quarks;
	    entry = *(vprev = &LeafHash((LTable)table, q));
	    while (entry && entry->name != q)
		entry = *(vprev = &entry->next);
	    /* if want loose and have tight, bump to next entry */
	    if (entry && *bindings == XrmBindLoosely && entry->tight)
		entry = *(vprev = &entry->next);
	    if (entry && entry->name == q &&
		(*bindings == XrmBindTightly) == entry->tight) {
		/* match, need to override */
		if ((type == XrmQString) == entry->string &&
		    entry->size == value->size) {
		    /* update type if not String, can be different */
		    if (!entry->string)
			RepType(entry) = type;
		    /* identical size, just overwrite value */
		    memcpy(RawValue(entry), (char *)value->addr, value->size);
		    return;
		}
		/* splice out and free old entry */
		*vprev = entry->next;
		Xfree((char *)entry);
		(*pprev)->entries--;
	    }
	    /* this is where to insert */
	    prev = (NTable *)vprev;
	}
    }
    /* keep the top table, because we may have to grow it */
    firstpprev = pprev;
    /* iterate until we get to the leaf */
    while (quarks[1]) {
	/* build a new table and chain it in */
	NEWTABLE(*quarks,2);
	if (*quarks++ == XrmQANY)
	    (*pprev)->hasany = 1;
	if (*bindings++ == XrmBindTightly) {
	    table->tight = 1;
	} else {
	    table->tight = 0;
	    (*pprev)->hasloose = 1;
	}
	(*pprev)->entries++;
	pprev = prev;
	prev = nprev;
    }
    /* now allocate the value entry */
    entry = (VEntry)Xmalloc(((type == XrmQString) ?
			     sizeof(VEntryRec) : sizeof(DEntryRec)) +
			    value->size);
    if (!entry)
	return;
    entry->name = q = *quarks;
    if (*bindings == XrmBindTightly) {
	entry->tight = 1;
    } else {
	entry->tight = 0;
	(*pprev)->hasloose = 1;
    }
    /* chain it in, with a bit of type cast ugliness */
    entry->next = *((VEntry *)prev);
    *((VEntry *)prev) = entry;
    entry->size = value->size;
    if (type == XrmQString) {
	entry->string = 1;
    } else {
	entry->string = 0;
	RepType(entry) = type;
    }
    /* save a copy of the value */
    memcpy(RawValue(entry), (char *)value->addr, value->size);
    (*pprev)->entries++;
    /* this is a new leaf, need to remember it for search lists */
    if (q > maxResourceQuark) {
	unsigned oldsize = (maxResourceQuark + 1) >> 3;
	unsigned size = ((q | 0x7f) + 1) >> 3; /* reallocate in chunks */
	if (resourceQuarks)
	    resourceQuarks = (unsigned char *)Xrealloc((char *)resourceQuarks,
						       size);
	else
	    resourceQuarks = (unsigned char *)Xmalloc(size);
	if (resourceQuarks) {
	    bzero((char *)&resourceQuarks[oldsize], size - oldsize);
	    maxResourceQuark = (size << 3) - 1;
	} else {
	    maxResourceQuark = -1;
	}
    }
    if (q > 0 && resourceQuarks)
	resourceQuarks[q >> 3] |= 1 << (q & 0x7);
    GROW(firstpprev);

#undef NEWTABLE
}

void falrmQPutResource(
    XrmDatabase		*pdb,
    XrmBindingList      bindings,
    XrmQuarkList	quarks,
    XrmRepresentation	type,
    XrmValuePtr		value)
{
    if (!*pdb) *pdb = NewDatabase();
    _XLockMutex(&(*pdb)->linfo);
    PutEntry(*pdb, bindings, quarks, type, value);
    _XUnlockMutex(&(*pdb)->linfo);
}

void falrmPutResource(
    XrmDatabase     *pdb,
    const char    *specifier,
    const char    *type,
    XrmValuePtr	    value)
{
    XrmBinding	    bindings[MAXDBDEPTH+1];
    XrmQuark	    quarks[MAXDBDEPTH+1];

    if (!*pdb) *pdb = NewDatabase();
    _XLockMutex(&(*pdb)->linfo);
    falrmStringToBindingQuarkList(specifier, bindings, quarks);
    PutEntry(*pdb, bindings, quarks, falrmStringToQuark(type), value);
    _XUnlockMutex(&(*pdb)->linfo);
}

void falrmQPutStringResource(
    XrmDatabase     *pdb,
    XrmBindingList  bindings,
    XrmQuarkList    quarks,
    const char    *str)
{
    XrmValue	value;

    if (!*pdb) *pdb = NewDatabase();
    value.addr = (XPointer) str;
    value.size = strlen(str)+1;
    _XLockMutex(&(*pdb)->linfo);
    PutEntry(*pdb, bindings, quarks, XrmQString, &value);
    _XUnlockMutex(&(*pdb)->linfo);
}

/*	Function Name: GetDatabase
 *	Description: Parses a string and stores it as a database.
 *	Arguments: db - the database.
 *                 str - a pointer to the string containing the database.
 *                 filename - source filename, if any.
 *                 doall - whether to do all lines or just one
 */

/*
 * This function is highly optimized to inline as much as possible.
 * Be very careful with modifications, or simplifications, as they
 * may adversely affect the performance.
 *
 * Chris Peterson, MIT X Consortium		5/17/90.
 */

#define LIST_SIZE 101
#define BUFFER_SIZE 100

static void GetIncludeFile();

static void GetDatabase(
    XrmDatabase db,
    char *str,
    char *filename,
    Bool doall)
{
    char *ptr;
    XrmBits bits = 0;
    char c;
    int len;
    Signature sig;
    char *ptr_max;
    XrmQuarkList t_quarks;
    XrmBindingList t_bindings;

    int alloc_chars = BUFSIZ;
    char buffer[BUFSIZ], *value_str;
    XrmQuark quarks[LIST_SIZE];
    XrmBinding bindings[LIST_SIZE];
    XrmValue value;
    Bool only_pcs;
    Bool dolines;

    if (!db)
	return;

    if (!(value_str = Xmalloc(sizeof(char) * alloc_chars)))
	return;

    (*db->methods->mbinit)(db->mbstate);
    str--;
    dolines = True;
    while (!is_EOF(bits) && dolines) {
	dolines = doall;

	/*
	 * First: Remove extra whitespace.
	 */

	do {
	    bits = next_char(c, str);
	} while is_space(bits);

	/*
	 * Ignore empty lines.
	 */

	if (is_EOL(bits))
	    continue;		/* start a new line. */

	/*
	 * Second: check the first character in a line to see if it is
	 * "!" signifying a comment, or "#" signifying a directive.
	 */

	if (c == '!') { /* Comment, spin to next newline */
	    while (is_simple(bits = next_char(c, str))) {}
	    if (is_EOL(bits))
		continue;
	    while (!is_EOL(bits = next_mbchar(c, len, str))) {}
	    str--;
	    continue;		/* start a new line. */
	}

	if (c == '#') { /* Directive */
	    /* remove extra whitespace */
	    only_pcs = True;
	    while (is_space(bits = next_char(c, str))) {};
	    /* only "include" directive is currently defined */
	    if (!strncmp(str, "include", 7)) {
		str += (7-1);
		/* remove extra whitespace */
		while (is_space(bits = next_char(c, str))) {};
		/* must have a starting " */
		if (c == '"') {
		    char *fname = str+1;
		    len = 0;
		    do {
			if (only_pcs) {
			    bits = next_char(c, str);
			    if (is_nonpcs(bits))
				only_pcs = False;
			}
			if (!only_pcs)
			    bits = next_mbchar(c, len, str);
		    } while (c != '"' && !is_EOL(bits));
		    /* must have an ending " */
		    if (c == '"')
			GetIncludeFile(db, filename, fname, str - len - fname);
		}
	    }
	    /* spin to next newline */
	    if (only_pcs) {
		while (is_simple(bits))
		    bits = next_char(c, str);
		if (is_EOL(bits))
		    continue;
	    }
	    while (!is_EOL(bits))
		bits = next_mbchar(c, len, str);
	    str--;
	    continue;		/* start a new line. */
	}

	/*
	 * Third: loop through the LHS of the resource specification
	 * storing characters and converting this to a Quark.
	 *
	 * If the number of quarks is greater than LIST_SIZE - 1.  This
	 * function will trash your memory.
	 *
	 * If the length of any quark is larger than BUFSIZ this function
	 * will also trash memory.
	 */

	t_bindings = bindings;
	t_quarks = quarks;

	sig = 0;
	ptr = buffer;
	*t_bindings = XrmBindTightly;
	for(;;) {
	    if (!is_binding(bits)) {
		while (!is_EOQ(bits)) {
		    *ptr++ = c;
		    sig = (sig << 1) + c; /* Compute the signature. */
		    bits = next_char(c, str);
		}

		*t_quarks++ = _falrmInternalStringToQuark(buffer, ptr - buffer,
							sig, False);

		if (is_separator(bits))  {
		    if (!is_space(bits))
			break;

		    /* Remove white space */
		    do {
			*ptr++ = c;
			sig = (sig << 1) + c; /* Compute the signature. */
		    } while (is_space(bits = next_char(c, str)));

		    /*
		     * The spec doesn't permit it, but support spaces
		     * internal to resource name/class
		     */

		    if (is_separator(bits))
			break;
		    t_quarks--;
		    continue;
		}

		if (c == '.')
		    *(++t_bindings) = XrmBindTightly;
		else
		    *(++t_bindings) = XrmBindLoosely;

		sig = 0;
		ptr = buffer;
	    }
	    else {
		/*
		 * Magic unspecified feature #254.
		 *
		 * If two separators appear with no Text between them then
		 * ignore them.
		 *
		 * If anyone of those separators is a '*' then the binding
		 * will be loose, otherwise it will be tight.
		 */

		if (c == '*')
		    *t_bindings = XrmBindLoosely;
	    }

	    bits = next_char(c, str);
	}

	*t_quarks = NULLQUARK;

	/*
	 * Make sure that there is a ':' in this line.
	 */

	if (c != ':') {
	    char oldc;

	    /*
	     * A parsing error has occurred, toss everything on the line
	     * a new_line can still be escaped with a '\'.
	     */

	    while (is_normal(bits))
		bits = next_char(c, str);
	    if (is_EOL(bits))
		continue;
	    bits = next_mbchar(c, len, str);
	    do {
		oldc = c;
		bits = next_mbchar(c, len, str);
	    } while (c && (c != '\n' || oldc == '\\'));
	    str--;
	    continue;
	}

	/*
	 * I now have a quark and binding list for the entire left hand
	 * side.  "c" currently points to the ":" separating the left hand
	 * side for the right hand side.  It is time to begin processing
	 * the right hand side.
	 */

	/*
	 * Fourth: Remove more whitespace
	 */

	for(;;) {
	    if (is_space(bits = next_char(c, str)))
		continue;
	    if (c != '\\')
		break;
	    bits = next_char(c, str);
	    if (c == '\n')
		continue;
	    str--;
	    bits = BSLASH;
	    c = '\\';
	    break;
	}

	/*
	 * Fifth: Process the right hand side.
	 */

	ptr = value_str;
	ptr_max = ptr + alloc_chars - 4;
	only_pcs = True;
	len = 1;

	for(;;) {

	    /*
	     * Tight loop for the normal case:  Non backslash, non-end of value
	     * character that will fit into the allocated buffer.
	     */

	    if (only_pcs) {
		while (is_normal(bits) && ptr < ptr_max) {
		    *ptr++ = c;
		    bits = next_char(c, str);
		}
		if (is_EOL(bits))
		    break;
		if (is_nonpcs(bits)) {
		    only_pcs = False;
		    bits = next_mbchar(c, len, str);
		}
	    }
	    while (!is_special(bits) && ptr + len <= ptr_max) {
		len = -len;
		while (len)
		    *ptr++ = str[len++];
		bits = next_mbchar(c, len, str);
	    }

	    if (is_EOL(bits)) {
		str--;
		break;
	    }

	    if (c == '\\') {
		/*
		 * We need to do some magic after a backslash.
		 */
		Bool read_next = True;

		if (only_pcs) {
		    bits = next_char(c, str);
		    if (is_nonpcs(bits))
			only_pcs = False;
		}
		if (!only_pcs)
		    bits = next_mbchar(c, len, str);

		if (is_EOL(bits)) {
		    if (is_EOF(bits))
			continue;
		} else if (c == 'n') {
		    /*
		     * "\n" means insert a newline.
		     */
		    *ptr++ = '\n';
		} else if (c == '\\') {
		    /*
		     * "\\" completes to just one backslash.
		     */
		    *ptr++ = '\\';
		} else {
		    /*
		     * pick up to three octal digits after the '\'.
		     */
		    char temp[3];
		    int count = 0;
		    while (is_odigit(bits) && count < 3) {
			temp[count++] = c;
			if (only_pcs) {
			    bits = next_char(c, str);
			    if (is_nonpcs(bits))
				only_pcs = False;
			}
			if (!only_pcs)
			    bits = next_mbchar(c, len, str);
		    }

		    /*
		     * If we found three digits then insert that octal code
		     * into the value string as a character.
		     */

		    if (count == 3) {
			*ptr++ = (unsigned char) ((temp[0] - '0') * 0100 +
						  (temp[1] - '0') * 010 +
						  (temp[2] - '0'));
		    }
		    else {
			int tcount;

			/*
			 * Otherwise just insert those characters into the
			 * string, since no special processing is needed on
			 * numerics we can skip the special processing.
			 */

			for (tcount = 0; tcount < count; tcount++) {
			    *ptr++ = temp[tcount]; /* print them in
						      the correct order */
			}
		    }
		    read_next = False;
		}
		if (read_next) {
		    if (only_pcs) {
			bits = next_char(c, str);
			if (is_nonpcs(bits))
			    only_pcs = False;
		    }
		    if (!only_pcs)
			bits = next_mbchar(c, len, str);
		}
	    }

	    /*
	     * It is important to make sure that there is room for at least
	     * four more characters in the buffer, since I can add that
	     * many characters into the buffer after a backslash has occurred.
	     */

	    if (ptr + len > ptr_max) {
		char * temp_str;

		alloc_chars += BUFSIZ/10;
		temp_str = Xrealloc(value_str, sizeof(char) * alloc_chars);

		if (!temp_str) {
		    Xfree(value_str);
		    (*db->methods->mbfinish)(db->mbstate);
		    return;
		}

		ptr = temp_str + (ptr - value_str); /* reset pointer. */
		value_str = temp_str;
		ptr_max = value_str + alloc_chars - 4;
	    }
	}

	/*
	 * Lastly: Terminate the value string, and store this entry
	 * 	   into the database.
	 */

	*ptr++ = '\0';

	/* Store it in database */
	value.size = ptr - value_str;
	value.addr = (XPointer) value_str;

	PutEntry(db, bindings, quarks, XrmQString, &value);
    }

    Xfree(value_str);
    (*db->methods->mbfinish)(db->mbstate);
}

void falrmPutStringResource(
    XrmDatabase *pdb,
    const char*specifier,
    const char*str)
{
    XrmValue	value;
    XrmBinding	bindings[MAXDBDEPTH+1];
    XrmQuark	quarks[MAXDBDEPTH+1];

    if (!*pdb) *pdb = NewDatabase();
    falrmStringToBindingQuarkList(specifier, bindings, quarks);
    value.addr = (XPointer) str;
    value.size = strlen(str)+1;
    _XLockMutex(&(*pdb)->linfo);
    PutEntry(*pdb, bindings, quarks, XrmQString, &value);
    _XUnlockMutex(&(*pdb)->linfo);
}


void falrmPutLineResource(XrmDatabase *pdb, char *line)
{
    if (!*pdb) *pdb = NewDatabase();
    _XLockMutex(&(*pdb)->linfo);
    GetDatabase(*pdb, line, (char *)NULL, False);
    _XUnlockMutex(&(*pdb)->linfo);
}

XrmDatabase falrmGetStringDatabase(char *data)
{
    XrmDatabase     db;

    db = NewDatabase();
    _XLockMutex(&db->linfo);
    GetDatabase(db, data, (char *)NULL, True);
    _XUnlockMutex(&db->linfo);
    return db;
}

/*	Function Name: ReadInFile
 *	Description: Reads the file into a buffer.
 *	Arguments: filename - the name of the file.
 *	Returns: An allocated string containing the contents of the file.
 */

static char *
ReadInFile(char *filename)
{
    int fd, size;
    char * filebuf;

    if ( (fd = OpenFile(filename)) == -1 )
	return (char *)NULL;

    GetSizeOfFile(filename, size);

    if (!(filebuf = Xmalloc(size + 1))) { /* leave room for '\0' */
	close(fd);
	return (char *)NULL;
    }

    size = ReadFile(fd, filebuf, size);
    if (size < 0) {
	CloseFile(fd);
	Xfree(filebuf);
	return (char *)NULL;
    }
    CloseFile(fd);

    filebuf[size] = '\0';	/* NULL terminate it. */
    return filebuf;
}

static void
GetIncludeFile(
    XrmDatabase db,
    char *base,
    char *fname,
    int fnamelen)
{
    int len;
    char *str;
    char realfname[BUFSIZ];

    if (fnamelen <= 0 || fnamelen >= BUFSIZ)
	return;
    if (*fname != '/' && base && (str = strrchr(base, '/'))) {
	len = str - base + 1;
	if (len + fnamelen >= BUFSIZ)
	    return;
	strncpy(realfname, base, len);
	strncpy(realfname + len, fname, fnamelen);
	realfname[len + fnamelen] = '\0';
    } else {
	strncpy(realfname, fname, fnamelen);
	realfname[fnamelen] = '\0';
    }
    if (!(str = ReadInFile(realfname)))
	return;
    GetDatabase(db, str, realfname, True);
    Xfree(str);
}

XrmDatabase falrmGetFileDatabase(char *filename)
{
    XrmDatabase db;
    char *str;

    if (!(str = ReadInFile(filename)))
	return (XrmDatabase)NULL;

    db = NewDatabase();
    _XLockMutex(&db->linfo);
    GetDatabase(db, str, filename, True);
    _XUnlockMutex(&db->linfo);
    Xfree(str);
    return db;
}

Status falrmCombineFileDatabase(
    char    *filename,
    XrmDatabase     *target,
    Bool             override)
{
    XrmDatabase db;
    char *str;

    if (!(str = ReadInFile(filename)))
	return 0;
    if (override) {
	db = *target;
	if (!db)
	    *target = db = NewDatabase();
    } else
	db = NewDatabase();
    _XLockMutex(&db->linfo);
    GetDatabase(db, str, filename, True);
    _XUnlockMutex(&db->linfo);
    Xfree(str);
    if (!override)
	falrmCombineDatabase(db, target, False);
    return 1;
}

/* call the user proc for every value in the table, arbitrary order.
 * stop if user proc returns True.  level is current depth in database.
 */
/*ARGSUSED*/
static Bool EnumLTable(
    LTable		table,
    XrmNameList		names,
    XrmClassList 	classes,
    int			level,
    EClosure		closure)
{
    VEntry *bucket;
    int i;
    VEntry entry;
    XrmValue value;
    XrmRepresentation type;
    Bool tightOk;

    closure->bindings[level] = (table->table.tight ?
				XrmBindTightly : XrmBindLoosely);
    closure->quarks[level] = table->table.name;
    level++;
    tightOk = !*names;
    closure->quarks[level + 1] = NULLQUARK;
    for (i = table->table.mask, bucket = table->buckets;
	 i >= 0;
	 i--, bucket++) {
	for (entry = *bucket; entry; entry = entry->next) {
	    if (entry->tight && !tightOk)
		continue;
	    closure->bindings[level] = (entry->tight ?
					XrmBindTightly : XrmBindLoosely);
	    closure->quarks[level] = entry->name;
	    value.size = entry->size;
	    if (entry->string) {
		type = XrmQString;
		value.addr = StringValue(entry);
	    } else {
		type = RepType(entry);
		value.addr = DataValue(entry);
	    }
	    if ((*closure->proc)(&closure->db, closure->bindings+1,
				 closure->quarks+1, &type, &value,
				 closure->closure))
		return True;
	}
    }
    return False;
}

static Bool EnumAllNTable(NTable table, int level, EClosure closure)
{
    NTable *bucket;
    int i;
    NTable entry;
    XrmQuark empty = NULLQUARK;

    if (level >= MAXDBDEPTH)
	return False;
    for (i = table->mask, bucket = NodeBuckets(table);
	 i >= 0;
	 i--, bucket++) {
	for (entry = *bucket; entry; entry = entry->next) {
	    if (entry->leaf) {
		if (EnumLTable((LTable)entry, &empty, &empty, level, closure))
		    return True;
	    } else {
		closure->bindings[level] = (entry->tight ?
					    XrmBindTightly : XrmBindLoosely);
		closure->quarks[level] = entry->name;
		if (EnumAllNTable(entry, level+1, closure))
		    return True;
	    }
	}
    }
    return False;
}

/* recurse on every table in the table, arbitrary order.
 * stop if user proc returns True.  level is current depth in database.
 */
static Bool EnumNTable(
    NTable		table,
    XrmNameList		names,
    XrmClassList 	classes,
    int			level,
    EClosure		closure)
{
    NTable	entry;
    XrmQuark	q;
    unsigned int leaf;
    Bool (*get)();
    Bool bilevel;

/* find entries named ename, leafness leaf, tight or loose, and call get */
#define ITIGHTLOOSE(ename) \
    NFIND(ename); \
    if (entry) { \
	if (leaf == entry->leaf) { \
	    if (!leaf && !entry->tight && entry->next && \
		entry->next->name == q && entry->next->tight && \
		(bilevel || entry->next->hasloose) && \
		EnumLTable((LTable)entry->next, names+1, classes+1, \
			   level, closure)) \
		return True; \
	    if ((*get)(entry, names+1, classes+1, level, closure)) \
		return True; \
	    if (entry->tight && (entry = entry->next) && \
		entry->name == q && leaf == entry->leaf && \
		(*get)(entry, names+1, classes+1, level, closure)) \
		return True; \
	} else if (entry->leaf) { \
	    if ((bilevel || entry->hasloose) && \
		EnumLTable((LTable)entry, names+1, classes+1, level, closure))\
		return True; \
	    if (entry->tight && (entry = entry->next) && \
		entry->name == q && (bilevel || entry->hasloose) && \
		EnumLTable((LTable)entry, names+1, classes+1, level, closure))\
		return True; \
	} \
    }

/* find entries named ename, leafness leaf, loose only, and call get */
#define ILOOSE(ename) \
    NFIND(ename); \
    if (entry && entry->tight && (entry = entry->next) && entry->name != q) \
	entry = (NTable)NULL; \
    if (entry) { \
	if (leaf == entry->leaf) { \
	    if ((*get)(entry, names+1, classes+1, level, closure)) \
		return True; \
	} else if (entry->leaf && (bilevel || entry->hasloose)) { \
	    if (EnumLTable((LTable)entry, names+1, classes+1, level, closure))\
		return True; \
	} \
    }

    if (level >= MAXDBDEPTH)
	return False;
    closure->bindings[level] = (table->tight ?
				XrmBindTightly : XrmBindLoosely);
    closure->quarks[level] = table->name;
    level++;
    if (!*names) {
	if (EnumAllNTable(table, level, closure))
	    return True;
    } else {
	if (names[1] || closure->mode == XrmEnumAllLevels) {
	    get = EnumNTable; /* recurse */
	    leaf = 0;
	    bilevel = !names[1];
	} else {
	    get = EnumLTable; /* bottom of recursion */
	    leaf = 1;
	    bilevel = False;
	}
	if (table->hasloose && closure->mode == XrmEnumAllLevels) {
	    NTable *bucket;
	    int i;
	    XrmQuark empty = NULLQUARK;

	    for (i = table->mask, bucket = NodeBuckets(table);
		 i >= 0;
		 i--, bucket++) {
		q = NULLQUARK;
		for (entry = *bucket; entry; entry = entry->next) {
		    if (!entry->tight && entry->name != q &&
			entry->name != *names && entry->name != *classes) {
			q = entry->name;
			if (entry->leaf) {
			    if (EnumLTable((LTable)entry, &empty, &empty,
					   level, closure))
				return True;
			} else {
			    if (EnumNTable(entry, &empty, &empty,
					   level, closure))
				return True;
			}
		    }
		}
	    }
	}

	ITIGHTLOOSE(*names);   /* do name, tight and loose */
	ITIGHTLOOSE(*classes); /* do class, tight and loose */
	if (table->hasany) {
	    ITIGHTLOOSE(XrmQANY); /* do ANY, tight and loose */
	}
	if (table->hasloose) {
	    while (1) {
		names++;
		classes++;
		if (!*names)
		    break;
		if (!names[1] && closure->mode != XrmEnumAllLevels) {
		    get = EnumLTable; /* bottom of recursion */
		    leaf = 1;
		}
		ILOOSE(*names);   /* loose names */
		ILOOSE(*classes); /* loose classes */
		if (table->hasany) {
		    ILOOSE(XrmQANY); /* loose ANY */
		}
	    }
	    names--;
	    classes--;
	}
    }
    /* now look for matching leaf nodes */
    entry = table->next;
    if (!entry)
	return False;
    if (entry->leaf) {
	if (entry->tight && !table->tight)
	    entry = entry->next;
    } else {
	entry = entry->next;
	if (!entry || !entry->tight)
	    return False;
    }
    if (!entry || entry->name != table->name)
	return False;
    /* found one */
    level--;
    if ((!*names || entry->hasloose) &&
	EnumLTable((LTable)entry, names, classes, level, closure))
	return True;
    if (entry->tight && entry == table->next && (entry = entry->next) &&
	entry->name == table->name && (!*names || entry->hasloose))
	return EnumLTable((LTable)entry, names, classes, level, closure);
    return False;

#undef ITIGHTLOOSE
#undef ILOOSE
}

/* call the proc for every value in the database, arbitrary order.
 * stop if the proc returns True.
 */
Bool falrmEnumerateDatabase(
    XrmDatabase		db,
    XrmNameList		names,
    XrmClassList	classes,
    int			mode,
    DBEnumProc		proc,
    XPointer		closure)
{
    XrmBinding  bindings[MAXDBDEPTH+2];
    XrmQuark	quarks[MAXDBDEPTH+2];
    NTable table;
    EClosureRec	eclosure;
    Bool retval = False;

    if (!db)
	return False;
    _XLockMutex(&db->linfo);
    eclosure.db = db;
    eclosure.proc = proc;
    eclosure.closure = closure;
    eclosure.bindings = bindings;
    eclosure.quarks = quarks;
    eclosure.mode = mode;
    table = db->table;
    if (table && !table->leaf && !*names && mode == XrmEnumOneLevel)
	table = table->next;
    if (table) {
	if (!table->leaf)
	    retval = EnumNTable(table, names, classes, 0, &eclosure);
	else
	    retval = EnumLTable((LTable)table, names, classes, 0, &eclosure);
    }
    _XUnlockMutex(&db->linfo);
    return retval;
}

static void PrintBindingQuarkList(
    XrmBindingList      bindings,
    XrmQuarkList	quarks,
    FILE		*stream)
{
    Bool	firstNameSeen;

    for (firstNameSeen = False; *quarks; bindings++, quarks++) {
	if (*bindings == XrmBindLoosely) {
	    (void) fprintf(stream, "*");
	} else if (firstNameSeen) {
	    (void) fprintf(stream, ".");
	}
	firstNameSeen = True;
	(void) fputs(falrmQuarkToString(*quarks), stream);
    }
}

/* output out the entry in correct file syntax */
/*ARGSUSED*/
static Bool DumpEntry(
    XrmDatabase		*db,
    XrmBindingList      bindings,
    XrmQuarkList	quarks,
    XrmRepresentation   *type,
    XrmValuePtr		value,
    XPointer		data)
{
    FILE			*stream = (FILE *)data;
    unsigned int	i;
    char		*s;
    char		c;

    if (*type != XrmQString)
	(void) putc('!', stream);
    PrintBindingQuarkList(bindings, quarks, stream);
    s = value->addr;
    i = value->size;
    if (*type == XrmQString) {
	(void) fputs(":\t", stream);
	if (i)
	    i--;
    }
    else
	(void) fprintf(stream, "=%s:\t", XrmRepresentationToString(*type));
    if (i && (*s == ' ' || *s == '\t'))
	(void) putc('\\', stream); /* preserve leading whitespace */
    while (i--) {
	c = *s++;
	if (c == '\n') {
	    if (i)
		(void) fputs("\\n\\\n", stream);
	    else
		(void) fputs("\\n", stream);
	} else if (c == '\\')
	    (void) fputs("\\\\", stream);
	else if ((c < ' ' && c != '\t') ||
		 ((unsigned char)c >= 0x7f && (unsigned char)c < 0xa0))
	    (void) fprintf(stream, "\\%03o", (unsigned char)c);
	else
	    (void) putc(c, stream);
    }
    (void) putc('\n', stream);
    return ferror(stream) != 0;
}

#ifdef DEBUG

void falPrintTable(NTable table, FILE *file)
{
    XrmBinding  bindings[MAXDBDEPTH+1];
    XrmQuark	quarks[MAXDBDEPTH+1];
    EClosureRec closure;
    XrmQuark	empty = NULLQUARK;

    closure.db = (XrmDatabase)NULL;
    closure.proc = DumpEntry;
    closure.closure = (XPointer)file;
    closure.bindings = bindings;
    closure.quarks = quarks;
    closure.mode = XrmEnumAllLevels;
    if (table->leaf)
	EnumLTable((LTable)table, &empty, &empty, 0, &closure);
    else
	EnumNTable(table, &empty, &empty, 0, &closure);
}

#endif /* DEBUG */

void falrmPutFileDatabase(XrmDatabase db, const char *fileName)
{
    FILE	*file;
    XrmQuark empty = NULLQUARK;

    if (!db) return;
    if (!(file = fopen(fileName, "w"))) return;
    if (falrmEnumerateDatabase(db, &empty, &empty, XrmEnumAllLevels,
			     DumpEntry, (XPointer) file))
	unlink((char *)fileName);
    fclose(file);
}

/* macros used in get/search functions */

/* find entries named ename, leafness leaf, tight or loose, and call get */
#define GTIGHTLOOSE(ename,looseleaf) \
    NFIND(ename); \
    if (entry) { \
	if (leaf == entry->leaf) { \
	    if (!leaf && !entry->tight && entry->next && \
		entry->next->name == q && entry->next->tight && \
		entry->next->hasloose && \
		looseleaf((LTable)entry->next, names+1, classes+1, closure)) \
		return True; \
	    if ((*get)(entry, names+1, classes+1, closure)) \
		return True; \
	    if (entry->tight && (entry = entry->next) && \
		entry->name == q && leaf == entry->leaf && \
		(*get)(entry, names+1, classes+1, closure)) \
		return True; \
	} else if (entry->leaf) { \
	    if (entry->hasloose && \
		looseleaf((LTable)entry, names+1, classes+1, closure)) \
		return True; \
	    if (entry->tight && (entry = entry->next) && \
		entry->name == q && entry->hasloose && \
		looseleaf((LTable)entry, names+1, classes+1, closure)) \
		return True; \
	} \
    }

/* find entries named ename, leafness leaf, loose only, and call get */
#define GLOOSE(ename,looseleaf) \
    NFIND(ename); \
    if (entry && entry->tight && (entry = entry->next) && entry->name != q) \
	entry = (NTable)NULL; \
    if (entry) { \
	if (leaf == entry->leaf) { \
	    if ((*get)(entry, names+1, classes+1, closure)) \
		return True; \
	} else if (entry->leaf && entry->hasloose) { \
	    if (looseleaf((LTable)entry, names+1, classes+1, closure)) \
		return True; \
	} \
    }

/* add tight/loose entry to the search list, return True if list is full */
/*ARGSUSED*/
static Bool AppendLEntry(
    LTable		table,
    XrmNameList		names,
    XrmClassList 	classes,
    SClosure		closure)
{
    /* check for duplicate */
    if (closure->idx >= 0 && closure->list[closure->idx] == table)
	return False;
    if (closure->idx == closure->limit)
	return True;
    /* append it */
    closure->idx++;
    closure->list[closure->idx] = table;
    return False;
}

/* add loose entry to the search list, return True if list is full */
/*ARGSUSED*/
static Bool AppendLooseLEntry(
    LTable		table,
    XrmNameList		names,
    XrmClassList 	classes,
    SClosure		closure)
{
    /* check for duplicate */
    if (closure->idx >= 0 && closure->list[closure->idx] == table)
	return False;
    if (closure->idx >= closure->limit - 1)
	return True;
    /* append it */
    closure->idx++;
    closure->list[closure->idx] = LOOSESEARCH;
    closure->idx++;
    closure->list[closure->idx] = table;
    return False;
}

/* search for a leaf table */
static Bool SearchNEntry(
    NTable		table,
    XrmNameList		names,
    XrmClassList 	classes,
    SClosure		closure)
{
    NTable	entry;
    XrmQuark	q;
    unsigned int leaf;
    Bool		(*get)();

    if (names[1]) {
	get = SearchNEntry; /* recurse */
	leaf = 0;
    } else {
	get = AppendLEntry; /* bottom of recursion */
	leaf = 1;
    }
    GTIGHTLOOSE(*names, AppendLooseLEntry);   /* do name, tight and loose */
    GTIGHTLOOSE(*classes, AppendLooseLEntry); /* do class, tight and loose */
    if (table->hasany) {
	GTIGHTLOOSE(XrmQANY, AppendLooseLEntry); /* do ANY, tight and loose */
    }
    if (table->hasloose) {
	while (1) {
	    names++;
	    classes++;
	    if (!*names)
		break;
	    if (!names[1]) {
		get = AppendLEntry; /* bottom of recursion */
		leaf = 1;
	    }
	    GLOOSE(*names, AppendLooseLEntry);   /* loose names */
	    GLOOSE(*classes, AppendLooseLEntry); /* loose classes */
	    if (table->hasany) {
		GLOOSE(XrmQANY, AppendLooseLEntry); /* loose ANY */
	    }
	}
    }
    /* now look for matching leaf nodes */
    entry = table->next;
    if (!entry)
	return False;
    if (entry->leaf) {
	if (entry->tight && !table->tight)
	    entry = entry->next;
    } else {
	entry = entry->next;
	if (!entry || !entry->tight)
	    return False;
    }
    if (!entry || entry->name != table->name)
	return False;
    /* found one */
    if (entry->hasloose &&
	AppendLooseLEntry((LTable)entry, names, classes, closure))
	return True;
    if (entry->tight && entry == table->next && (entry = entry->next) &&
	entry->name == table->name && entry->hasloose)
	return AppendLooseLEntry((LTable)entry, names, classes, closure);
    return False;
}

Bool falrmQGetSearchList(
    XrmDatabase     db,
    XrmNameList	    names,
    XrmClassList    classes,
    XrmSearchList   searchList,	/* RETURN */
    int		    listLength)
{
    NTable		table;
    SClosureRec		closure;

    if (listLength <= 0)
	return False;
    closure.list = (LTable *)searchList;
    closure.idx = -1;
    closure.limit = listLength - 2;
    if (db) {
	_XLockMutex(&db->linfo);
	table = db->table;
	if (*names) {
	    if (table && !table->leaf) {
		if (SearchNEntry(table, names, classes, &closure)) {
		    _XUnlockMutex(&db->linfo);
		    return False;
		}
	    } else if (table && table->hasloose &&
		       AppendLooseLEntry((LTable)table, names, classes,
					 &closure)) {
		_XUnlockMutex(&db->linfo);
		return False;
	    }
	} else {
	    if (table && !table->leaf)
		table = table->next;
	    if (table &&
		AppendLEntry((LTable)table, names, classes, &closure)) {
		_XUnlockMutex(&db->linfo);
		return False;
	    }
	}
	_XUnlockMutex(&db->linfo);
    }
    closure.list[closure.idx + 1] = (LTable)NULL;
    return True;
}

Bool falrmQGetSearchResource(
    XrmSearchList	searchList,
    XrmName		name,
    XrmClass		class,
    XrmRepresentation	*pType,  /* RETURN */
    XrmValue		*pValue) /* RETURN */
{
    LTable *list;
    LTable table;
    VEntry entry;
    int flags;

/* find tight or loose entry */
#define VTIGHTLOOSE(q) \
    entry = LeafHash(table, q); \
    while (entry && entry->name != q) \
	entry = entry->next; \
    if (entry) \
	break

/* find loose entry */
#define VLOOSE(q) \
    entry = LeafHash(table, q); \
    while (entry && entry->name != q) \
	entry = entry->next; \
    if (entry) { \
	if (!entry->tight) \
	    break; \
	if ((entry = entry->next) && entry->name == q) \
	    break; \
    }

    list = (LTable *)searchList;
    /* figure out which combination of name and class we need to search for */
    flags = 0;
    if (IsResourceQuark(name))
	flags = 2;
    if (IsResourceQuark(class))
	flags |= 1;
    if (!flags) {
	/* neither name nor class has ever been used to name a resource */
	table = (LTable)NULL;
    } else if (flags == 3) {
	/* both name and class */
	while (table = *list++) {
	    if (table != LOOSESEARCH) {
		VTIGHTLOOSE(name);  /* do name, tight and loose */
		VTIGHTLOOSE(class); /* do class, tight and loose */
	    } else {
		table = *list++;
		VLOOSE(name);  /* do name, loose only */
		VLOOSE(class); /* do class, loose only */
	    }
	}
    } else {
	/* just one of name or class */
	if (flags == 1)
	    name = class;
	while (table = *list++) {
	    if (table != LOOSESEARCH) {
		VTIGHTLOOSE(name); /* tight and loose */
	    } else {
		table = *list++;
		VLOOSE(name); /* loose only */
	    }
	}
    }
    if (table) {
	/* found a match */
	if (entry->string) {
	    *pType = XrmQString;
	    pValue->addr = StringValue(entry);
	} else {
	    *pType = RepType(entry);
	    pValue->addr = DataValue(entry);
	}
	pValue->size = entry->size;
	return True;
    }
    *pType = NULLQUARK;
    pValue->addr = (XPointer)NULL;
    pValue->size = 0;
    return False;

#undef VTIGHTLOOSE
#undef VLOOSE
}

/* look for a tight/loose value */
static Bool GetVEntry(
    LTable		table,
    XrmNameList		names,
    XrmClassList 	classes,
    VClosure		closure)
{
    VEntry entry;
    XrmQuark q;

    /* try name first */
    q = *names;
    entry = LeafHash(table, q);
    while (entry && entry->name != q)
	entry = entry->next;
    if (!entry) {
	/* not found, try class */
	q = *classes;
	entry = LeafHash(table, q);
	while (entry && entry->name != q)
	    entry = entry->next;
	if (!entry)
	    return False;
    }
    if (entry->string) {
	*closure->type = XrmQString;
	closure->value->addr = StringValue(entry);
    } else {
	*closure->type = RepType(entry);
	closure->value->addr = DataValue(entry);
    }
    closure->value->size = entry->size;
    return True;
}

/* look for a loose value */
static Bool GetLooseVEntry(
    LTable		table,
    XrmNameList		names,
    XrmClassList 	classes,
    VClosure		closure)
{
    VEntry	entry;
    XrmQuark	q;

#define VLOOSE(ename) \
    q = ename; \
    entry = LeafHash(table, q); \
    while (entry && entry->name != q) \
	entry = entry->next; \
    if (entry && entry->tight && (entry = entry->next) && entry->name != q) \
	entry = (VEntry)NULL;

    /* bump to last component */
    while (names[1]) {
	names++;
	classes++;
    }
    VLOOSE(*names);  /* do name, loose only */
    if (!entry) {
	VLOOSE(*classes); /* do class, loose only */
	if (!entry)
	    return False;
    }
    if (entry->string) {
	*closure->type = XrmQString;
	closure->value->addr = StringValue(entry);
    } else {
	*closure->type = RepType(entry);
	closure->value->addr = DataValue(entry);
    }
    closure->value->size = entry->size;
    return True;

#undef VLOOSE
}

/* recursive search for a value */
static Bool GetNEntry(
    NTable		table,
    XrmNameList		names,
    XrmClassList 	classes,
    VClosure		closure)
{
    NTable	entry;
    XrmQuark	q;
    unsigned int leaf;
    Bool		(*get)();
    NTable		otable;

    if (names[2]) {
	get = GetNEntry; /* recurse */
	leaf = 0;
    } else {
	get = GetVEntry; /* bottom of recursion */
	leaf = 1;
    }
    GTIGHTLOOSE(*names, GetLooseVEntry);   /* do name, tight and loose */
    GTIGHTLOOSE(*classes, GetLooseVEntry); /* do class, tight and loose */
    if (table->hasany) {
	GTIGHTLOOSE(XrmQANY, GetLooseVEntry); /* do ANY, tight and loose */
    }
    if (table->hasloose) {
	while (1) {
	    names++;
	    classes++;
	    if (!names[1])
		break;
	    if (!names[2]) {
		get = GetVEntry; /* bottom of recursion */
		leaf = 1;
	    }
	    GLOOSE(*names, GetLooseVEntry);   /* do name, loose only */
	    GLOOSE(*classes, GetLooseVEntry); /* do class, loose only */
	    if (table->hasany) {
		GLOOSE(XrmQANY, GetLooseVEntry); /* do ANY, loose only */
	    }
	}
    }
    /* look for matching leaf tables */
    otable = table;
    table = table->next;
    if (!table)
	return False;
    if (table->leaf) {
	if (table->tight && !otable->tight)
	    table = table->next;
    } else {
	table = table->next;
	if (!table || !table->tight)
	    return False;
    }
    if (!table || table->name != otable->name)
	return False;
    /* found one */
    if (table->hasloose &&
	GetLooseVEntry((LTable)table, names, classes, closure))
	return True;
    if (table->tight && table == otable->next) {
	table = table->next;
	if (table && table->name == otable->name && table->hasloose)
	    return GetLooseVEntry((LTable)table, names, classes, closure);
    }
    return False;
}

Bool falrmQGetResource(
    XrmDatabase         db,
    XrmNameList		names,
    XrmClassList 	classes,
    XrmRepresentation	*pType,  /* RETURN */
    XrmValuePtr		pValue)  /* RETURN */
{
    NTable table;
    VClosureRec closure;

    if (db && *names) {
	_XLockMutex(&db->linfo);
	closure.type = pType;
	closure.value = pValue;
	table = db->table;
	if (names[1]) {
	    if (table && !table->leaf) {
		if (GetNEntry(table, names, classes, &closure)) {
		    _XUnlockMutex(&db->linfo);
		    return True;
		}
	    } else if (table && table->hasloose &&
		    GetLooseVEntry((LTable)table, names, classes, &closure)) {
		_XUnlockMutex (&db->linfo);
		return True;
	    }
	} else {
	    if (table && !table->leaf)
		table = table->next;
	    if (table && GetVEntry((LTable)table, names, classes, &closure)) {
		_XUnlockMutex(&db->linfo);
		return True;
	    }
	}
	_XUnlockMutex(&db->linfo);
    }
    *pType = NULLQUARK;
    pValue->addr = (XPointer)NULL;
    pValue->size = 0;
    return False;
}

Bool falrmGetResource(
    XrmDatabase         db,
    const char		*name_str,
    const char		*class_str,
    XrmString		*pType_str,  /* RETURN */
    XrmValuePtr		pValue)      /* RETURN */
{
    XrmName		names[MAXDBDEPTH+1];
    XrmClass		classes[MAXDBDEPTH+1];
    XrmRepresentation   fromType;
    Bool		result;

    XrmStringToNameList(name_str, names);
    XrmStringToClassList(class_str, classes);
    result = falrmQGetResource(db, names, classes, &fromType, pValue);
    (*pType_str) = falrmQuarkToString(fromType);
    return result;
}

/* destroy all values, plus table itself */
static void DestroyLTable(LTable table)
{
    int i;
    VEntry *buckets;
    VEntry entry, next;

    buckets = table->buckets;
    for (i = table->table.mask; i >= 0; i--, buckets++) {
	for (next = *buckets; entry = next; ) {
	    next = entry->next;
	    Xfree((char *)entry);
	}
    }
    Xfree((char *)table->buckets);
    Xfree((char *)table);
}

/* destroy all contained tables, plus table itself */
static void DestroyNTable(NTable table)
{
    int i;
    NTable *buckets;
    NTable entry, next;

    buckets = NodeBuckets(table);
    for (i = table->mask; i >= 0; i--, buckets++) {
	for (next = *buckets; entry = next; ) {
	    next = entry->next;
	    if (entry->leaf)
		DestroyLTable((LTable)entry);
	    else
		DestroyNTable(entry);
	}
    }
    Xfree((char *)table);
}

char *falrmLocaleOfDatabase(XrmDatabase db)
{
    char* retval;
    _XLockMutex(&db->linfo);
    retval = (*db->methods->lcname)(db->mbstate);
    _XUnlockMutex(&db->linfo);
    return retval;
}

void falrmDestroyDatabase(XrmDatabase db)
{
    NTable table, next;

    if (db) {
	_XLockMutex(&db->linfo);
	for (next = db->table; table = next; ) {
	    next = table->next;
	    if (table->leaf)
		DestroyLTable((LTable)table);
	    else
		DestroyNTable(table);
	}
	_XFreeMutex(&db->linfo);
	(*db->methods->destroy)(db->mbstate);
	Xfree((char *)db);
    }
}
