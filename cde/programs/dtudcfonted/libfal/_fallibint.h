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
/* Xlibint.h 1.2 - Fujitsu source for CDEnext    95/12/07 10:53:12 	*/
/* $XConsortium: _fallibint.h /main/1 1996/04/08 15:21:22 cde-fuj $ */

/*

Copyright (c) 1984, 1985, 1987, 1989  X Consortium

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

/*
 *	Xlibint.h - Header definition and support file for the internal
 *	support routines used by the C subroutine interface
 *	library (Xlib) to the X Window System.
 *
 *	Warning, there be dragons here....
 */
/*
 * Copyright 1995 by FUJITSU LIMITED
 * This is source code modified by FUJITSU LIMITED under the Joint
 * Development Agreement for the CDEnext PST.
 * This is unpublished proprietry source code of FUJITSU LIMITED
 *
 * Modifier: Takanori Tateno   FUJITSU LIMITED
 *
 */


#if !defined(NEED_EVENTS) && !NeedFunctionPrototypes
#define _XEVENT_
#endif

#include "syncx.h"

struct _XGC
{
    XExtData *ext_data;	/* hook for extension to hang data */
    GContext gid;	/* protocol ID for graphics context */
    Bool rects;		/* boolean: TRUE if clipmask is list of rectangles */
    Bool dashes;	/* boolean: TRUE if dash-list is really a list */
    unsigned long dirty;/* cache dirty bits */
    XGCValues values;	/* shadow structure of values */
};

struct _XDisplay
{
	XExtData *ext_data;	/* hook for extension to hang data */
	struct _XFreeFuncs *free_funcs; /* internal free functions */
	int fd;			/* Network socket. */
	int conn_checker;         /* ugly thing used by _XEventsQueued */
	int proto_major_version;/* maj. version of server's X protocol */
	int proto_minor_version;/* minor version of server's X protocol */
	char *vendor;		/* vendor of the server hardware */
        XID resource_base;	/* resource ID base */
	XID resource_mask;	/* resource ID mask bits */
	XID resource_id;	/* allocator current ID */
	int resource_shift;	/* allocator shift to correct bits */
	XID (*resource_alloc)(); /* allocator function */
	int byte_order;		/* screen byte order, LSBFirst, MSBFirst */
	int bitmap_unit;	/* padding and data requirements */
	int bitmap_pad;		/* padding requirements on bitmaps */
	int bitmap_bit_order;	/* LeastSignificant or MostSignificant */
	int nformats;		/* number of pixmap formats in list */
	ScreenFormat *pixmap_format;	/* pixmap format list */
	int vnumber;		/* Xlib's X protocol version number. */
	int release;		/* release of the server */
	struct _XSQEvent *head, *tail;	/* Input event queue. */
	int qlen;		/* Length of input event queue */
	unsigned long last_request_read; /* seq number of last event read */
	unsigned long request;	/* sequence number of last request. */
	char *last_req;		/* beginning of last request, or dummy */
	char *buffer;		/* Output buffer starting address. */
	char *bufptr;		/* Output buffer index pointer. */
	char *bufmax;		/* Output buffer maximum+1 address. */
	unsigned max_request_size; /* maximum number 32 bit words in request*/
	struct _XrmHashBucketRec *db;
	int (*synchandler)();	/* Synchronization handler */
	char *display_name;	/* "host:display" string used on this connect*/
	int default_screen;	/* default screen for operations */
	int nscreens;		/* number of screens on this server*/
	Screen *screens;	/* pointer to list of screens */
	unsigned long motion_buffer;	/* size of motion buffer */
	unsigned long flags;	   /* internal connection flags */
	int min_keycode;	/* minimum defined keycode */
	int max_keycode;	/* maximum defined keycode */
	KeySym *keysyms;	/* This server's keysyms */
	XModifierKeymap *modifiermap;	/* This server's modifier keymap */
	int keysyms_per_keycode;/* number of rows */
	char *xdefaults;	/* contents of defaults from server */
	char *scratch_buffer;	/* place to hang scratch buffer */
	unsigned long scratch_length;	/* length of scratch buffer */
	int ext_number;		/* extension number on this display */
	struct _XExten *ext_procs; /* extensions initialized on this display */
	/*
	 * the following can be fixed size, as the protocol defines how
	 * much address space is available.
	 * While this could be done using the extension vector, there
	 * may be MANY events processed, so a search through the extension
	 * list to find the right procedure for each event might be
	 * expensive if many extensions are being used.
	 */
	Bool (*event_vec[128])();  /* vector for wire to event */
	Status (*wire_vec[128])(); /* vector for event to wire */
	KeySym lock_meaning;	   /* for XLookupString */
	struct _XLockInfo *lock;   /* multi-thread state, display lock */
	struct _XInternalAsync *async_handlers; /* for internal async */
	unsigned long bigreq_size; /* max size of big requests */
	struct _XLockPtrs *lock_fns; /* pointers to threads functions */
	void (*idlist_alloc)();	   /* XID list allocator function */
	/* things above this line should not move, for binary compatibility */
	struct _XKeytrans *key_bindings; /* for XLookupString */
	Font cursor_font;	   /* for XCreateFontCursor */
	struct _XDisplayAtoms *atoms; /* for falInternAtom */
	unsigned int mode_switch;  /* keyboard group modifiers */
	unsigned int num_lock;  /* keyboard numlock modifiers */
	struct _XContextDB *context_db; /* context database */
	Bool (**error_vec)();      /* vector for wire to error */
	/*
	 * Xcms information
	 */
	struct {
	   XPointer defaultCCCs;  /* pointer to an array of default XcmsCCC */
	   XPointer clientCmaps;  /* pointer to linked list of XcmsCmapRec */
	   XPointer perVisualIntensityMaps;
				  /* linked list of XcmsIntensityMap */
	} cms;
	struct _XIMFilter *im_filters;
	struct _XSQEvent *qfree; /* unallocated event queue elements */
	unsigned long next_event_serial_num; /* inserted into next queue elt */
	struct _XExten *flushes; /* Flush hooks */
	struct _XConnectionInfo *im_fd_info; /* _XRegisterInternalConnection */
	int im_fd_length;	/* number of im_fd_info */
	struct _XConnWatchInfo *conn_watchers; /* XAddConnectionWatch */
	int watcher_count;	/* number of conn_watchers */
	XPointer filedes;	/* struct pollfd cache for _XWaitForReadable */
	int (*savedsynchandler)(); /* user synchandler when Xlib usurps */
	XID resource_max;	/* allocator max ID */
	int xcmisc_opcode;	/* major opcode for XC-MISC */
	struct _XkbInfoRec *xkb_info; /* XKB info */
	struct _XtransConnInfo *trans_conn; /* transport connection object */
};

#define XAllocIDs(dpy,ids,n) (*(dpy)->idlist_alloc)(dpy,ids,n)

/*
 * define DataRoutineIsProcedure if you want the Data macro to be a procedure
 * instead
 */

#ifndef _XEVENT_
/*
 * _QEvent datatype for use in input queueing.
 */
typedef struct _XSQEvent
{
    struct _XSQEvent *next;
    XEvent event;
    unsigned long qserial_num;	/* so multi-threaded code can find new ones */
} _XQEvent;
#endif

#ifdef XTHREADS			/* for xReply */
#define NEED_REPLIES
#endif

#define NEED_EVENTS
#define NEED_REPLIES
#include <X11/Xproto.h>
#include <errno.h>
#define _XBCOPYFUNC _Xbcopy
#include <X11/Xfuncs.h>
#include <X11/Xosdefs.h>

#ifndef X_NOT_STDC_ENV
#include <stdlib.h>
#include <string.h>
#else
char *malloc(), *realloc(), *calloc();
void exit();
#ifdef SYSV
#include <string.h>
#else
#include <strings.h>
#endif
#endif

/*
 * The following definitions can be used for locking requests in multi-threaded
 * address spaces.
 */
#ifdef XTHREADS
/* Author: Stephen Gildea, MIT X Consortium
 *
 * declarations for C Threads locking
 */

#include <X11/Xfuncproto.h>

struct _XLockPtrs {
    /* used by all, including extensions; do not move */
    void (*lock_display)();
    void (*unlock_display)();
};

typedef struct _LockInfoRec *LockInfoPtr;

/* in XlibInt.c */
extern void (*_XCreateMutex_fn)(
    LockInfoPtr /* lock */
);
extern void (*_XFreeMutex_fn)(
    LockInfoPtr /* lock */
);
extern void (*_XLockMutex_fn)(
    LockInfoPtr	/* lock */
#if defined(XTHREADS_WARN) || defined(XTHREADS_FILE_LINE)
    , char * /* file */
    , int /* line */
#endif
);
extern void (*_XUnlockMutex_fn)(
    LockInfoPtr	/* lock */
#if defined(XTHREADS_WARN) || defined(XTHREADS_FILE_LINE)
    , char * /* file */
    , int /* line */
#endif
);

extern LockInfoPtr _Xglobal_lock;

#if defined(XTHREADS_WARN) || defined(XTHREADS_FILE_LINE)
#define LockDisplay(d)	     if ((d)->lock_fns) (*(d)->lock_fns->lock_display)((d),__FILE__,__LINE__)
#define UnlockDisplay(d)     if ((d)->lock_fns) (*(d)->lock_fns->unlock_display)((d),__FILE__,__LINE__)
#define _XLockMutex(lock)		if (_XLockMutex_fn) (*_XLockMutex_fn)(lock,__FILE__,__LINE__)
#define _XUnlockMutex(lock)	if (_XUnlockMutex_fn) (*_XUnlockMutex_fn)(lock,__FILE__,__LINE__)
#else
/* used everywhere, so must be fast if not using threads */
#define LockDisplay(d)	     if ((d)->lock_fns) (*(d)->lock_fns->lock_display)(d)
#define UnlockDisplay(d)     if ((d)->lock_fns) (*(d)->lock_fns->unlock_display)(d)
#define _XLockMutex(lock)		if (_XLockMutex_fn) (*_XLockMutex_fn)(lock)
#define _XUnlockMutex(lock)	if (_XUnlockMutex_fn) (*_XUnlockMutex_fn)(lock)
#endif
#define _XCreateMutex(lock)	if (_XCreateMutex_fn) (*_XCreateMutex_fn)(lock);
#define _XFreeMutex(lock)	if (_XFreeMutex_fn) (*_XFreeMutex_fn)(lock);

#else /* XTHREADS */
#define LockDisplay(dis)
#define _XLockMutex(lock)
#define _XUnlockMutex(lock)
#define UnlockDisplay(dis)
#define _XCreateMutex(lock)
#define _XFreeMutex(lock)
#endif

#define Xfree(ptr) free((ptr))

/*
 * Note that some machines do not return a valid pointer for malloc(0), in
 * which case we provide an alternate under the control of the
 * define MALLOC_0_RETURNS_NULL.  This is necessary because some
 * Xlib code expects malloc(0) to return a valid pointer to storage.
 */
#ifdef MALLOC_0_RETURNS_NULL

# define Xmalloc(size) malloc(((size) == 0 ? 1 : (size)))
# define Xrealloc(ptr, size) realloc((ptr), ((size) == 0 ? 1 : (size)))
# define Xcalloc(nelem, elsize) calloc(((nelem) == 0 ? 1 : (nelem)), (elsize))

#else

# define Xmalloc(size) malloc((size))
# define Xrealloc(ptr, size) realloc((ptr), (size))
# define Xcalloc(nelem, elsize) calloc((nelem), (elsize))

#endif

#define LOCKED 1
#define UNLOCKED 0

#ifdef X_NOT_STDC_ENV
extern int errno;			/* Internal system error number. */
#endif

#ifndef BUFSIZE
#define BUFSIZE 2048			/* X output buffer size. */
#endif
#ifndef PTSPERBATCH
#define PTSPERBATCH 1024		/* point batching */
#endif
#ifndef WLNSPERBATCH
#define WLNSPERBATCH 50			/* wide line batching */
#endif
#ifndef ZLNSPERBATCH
#define ZLNSPERBATCH 1024		/* thin line batching */
#endif
#ifndef WRCTSPERBATCH
#define WRCTSPERBATCH 10		/* wide line rectangle batching */
#endif
#ifndef ZRCTSPERBATCH
#define ZRCTSPERBATCH 256		/* thin line rectangle batching */
#endif
#ifndef FRCTSPERBATCH
#define FRCTSPERBATCH 256		/* filled rectangle batching */
#endif
#ifndef FARCSPERBATCH
#define FARCSPERBATCH 256		/* filled arc batching */
#endif
#ifndef CURSORFONT
#define CURSORFONT "cursor"		/* standard cursor fonts */
#endif

/*
 * Display flags
 */
#define XlibDisplayIOError	(1L << 0)
#define XlibDisplayClosing	(1L << 1)
#define XlibDisplayNoXkb	(1L << 2)
#define XlibDisplayPrivSync	(1L << 3)
#define XlibDisplayProcConni	(1L << 4) /* in _XProcessInternalConnection */
#define XlibDisplayReadEvents	(1L << 5) /* in _XReadEvents */
#define XlibDisplayReply	(1L << 5) /* in _XReply */
#define XlibDisplayWriting	(1L << 6) /* in _XFlushInt, _XSend */

/*
 * X Protocol packetizing macros.
 */

#define WORD64ALIGN

/*
 * GetReq - Get the next available X request packet in the buffer and
 * return it.
 *
 * "name" is the name of the request, e.g. CreatePixmap, OpenFont, etc.
 * "req" is the name of the request pointer.
 *
 */

#if (defined(__STDC__) && !defined(UNIXCPP)) || defined(ANSICPP)
#define GetReq(name, req) \
        WORD64ALIGN\
	if ((dpy->bufptr + SIZEOF(x##name##Req)) > dpy->bufmax)\
		_XFlush(dpy);\
	req = (x##name##Req *)(dpy->last_req = dpy->bufptr);\
	req->reqType = X_##name;\
	req->length = (SIZEOF(x##name##Req))>>2;\
	dpy->bufptr += SIZEOF(x##name##Req);\
	dpy->request++

#else  /* non-ANSI C uses empty comment instead of "##" for token concatenation */
#define GetReq(name, req) \
        WORD64ALIGN\
	if ((dpy->bufptr + SIZEOF(x/**/name/**/Req)) > dpy->bufmax)\
		_XFlush(dpy);\
	req = (x/**/name/**/Req *)(dpy->last_req = dpy->bufptr);\
	req->reqType = X_/**/name;\
	req->length = (SIZEOF(x/**/name/**/Req))>>2;\
	dpy->bufptr += SIZEOF(x/**/name/**/Req);\
	dpy->request++
#endif

/* GetReqExtra is the same as GetReq, but allocates "n" additional
   bytes after the request. "n" must be a multiple of 4!  */

#if (defined(__STDC__) && !defined(UNIXCPP)) || defined(ANSICPP)
#define GetReqExtra(name, n, req) \
        WORD64ALIGN\
	if ((dpy->bufptr + SIZEOF(x##name##Req) + n) > dpy->bufmax)\
		_XFlush(dpy);\
	req = (x##name##Req *)(dpy->last_req = dpy->bufptr);\
	req->reqType = X_##name;\
	req->length = (SIZEOF(x##name##Req) + n)>>2;\
	dpy->bufptr += SIZEOF(x##name##Req) + n;\
	dpy->request++
#else
#define GetReqExtra(name, n, req) \
        WORD64ALIGN\
	if ((dpy->bufptr + SIZEOF(x/**/name/**/Req) + n) > dpy->bufmax)\
		_XFlush(dpy);\
	req = (x/**/name/**/Req *)(dpy->last_req = dpy->bufptr);\
	req->reqType = X_/**/name;\
	req->length = (SIZEOF(x/**/name/**/Req) + n)>>2;\
	dpy->bufptr += SIZEOF(x/**/name/**/Req) + n;\
	dpy->request++
#endif


/*
 * GetResReq is for those requests that have a resource ID
 * (Window, Pixmap, GContext, etc.) as their single argument.
 * "rid" is the name of the resource.
 */

#if (defined(__STDC__) && !defined(UNIXCPP)) || defined(ANSICPP)
#define GetResReq(name, rid, req) \
        WORD64ALIGN\
	if ((dpy->bufptr + SIZEOF(xResourceReq)) > dpy->bufmax)\
	    _XFlush(dpy);\
	req = (xResourceReq *) (dpy->last_req = dpy->bufptr);\
	req->reqType = X_##name;\
	req->length = 2;\
	req->id = (rid);\
	dpy->bufptr += SIZEOF(xResourceReq);\
	dpy->request++
#else
#define GetResReq(name, rid, req) \
        WORD64ALIGN\
	if ((dpy->bufptr + SIZEOF(xResourceReq)) > dpy->bufmax)\
	    _XFlush(dpy);\
	req = (xResourceReq *) (dpy->last_req = dpy->bufptr);\
	req->reqType = X_/**/name;\
	req->length = 2;\
	req->id = (rid);\
	dpy->bufptr += SIZEOF(xResourceReq);\
	dpy->request++
#endif

/*
 * GetEmptyReq is for those requests that have no arguments
 * at all.
 */
#if (defined(__STDC__) && !defined(UNIXCPP)) || defined(ANSICPP)
#define GetEmptyReq(name, req) \
        WORD64ALIGN\
	if ((dpy->bufptr + SIZEOF(xReq)) > dpy->bufmax)\
	    _XFlush(dpy);\
	req = (xReq *) (dpy->last_req = dpy->bufptr);\
	req->reqType = X_##name;\
	req->length = 1;\
	dpy->bufptr += SIZEOF(xReq);\
	dpy->request++
#else
#define GetEmptyReq(name, req) \
        WORD64ALIGN\
	if ((dpy->bufptr + SIZEOF(xReq)) > dpy->bufmax)\
	    _XFlush(dpy);\
	req = (xReq *) (dpy->last_req = dpy->bufptr);\
	req->reqType = X_/**/name;\
	req->length = 1;\
	dpy->bufptr += SIZEOF(xReq);\
	dpy->request++
#endif

#define MakeBigReq(req,n) \
    { \
    long _BRdat; \
    unsigned long _BRlen = req->length - 1; \
    req->length = 0; \
    _BRdat = ((long *)req)[_BRlen]; \
    memmove(((char *)req) + 8, ((char *)req) + 4, _BRlen << 2); \
    ((unsigned long *)req)[1] = _BRlen + n + 2; \
    Data32(dpy, &_BRdat, 4); \
    }

#define SetReqLen(req,n,badlen) \
    if ((req->length + n) > (unsigned)65535) { \
	if (dpy->bigreq_size) { \
	    MakeBigReq(req,n) \
	} else { \
	    n = badlen; \
	    req->length += n; \
	} \
    } else \
	req->length += n

#define SyncHandle() \
	if (dpy->synchandler) (*dpy->synchandler)(dpy)

#define FlushGC(dpy, gc) \
	if ((gc)->dirty) _XFlushGCCache((dpy), (gc))
/*
 * Data - Place data in the buffer and pad the end to provide
 * 32 bit word alignment.  Transmit if the buffer fills.
 *
 * "dpy" is a pointer to a Display.
 * "data" is a pinter to a data buffer.
 * "len" is the length of the data buffer.
 */
#ifndef DataRoutineIsProcedure
#define Data(dpy, data, len) \
	if (dpy->bufptr + (len) <= dpy->bufmax) {\
		memcpy(dpy->bufptr, data, (int)len);\
		dpy->bufptr += ((len) + 3) & ~3;\
	} else\
		_XSend(dpy, data, len)
#endif /* DataRoutineIsProcedure */


/* Allocate bytes from the buffer.  No padding is done, so if
 * the length is not a multiple of 4, the caller must be
 * careful to leave the buffer aligned after sending the
 * current request.
 *
 * "type" is the type of the pointer being assigned to.
 * "ptr" is the pointer being assigned to.
 * "n" is the number of bytes to allocate.
 *
 * Example:
 *    xTextElt *elt;
 *    BufAlloc (xTextElt *, elt, nbytes)
 */

#define BufAlloc(type, ptr, n) \
    if (dpy->bufptr + (n) > dpy->bufmax) \
        _XFlush (dpy); \
    ptr = (type) dpy->bufptr; \
    dpy->bufptr += (n);

#define Data16(dpy, data, len) Data((dpy), (char *)(data), (len))
#define _XRead16Pad(dpy, data, len) _XReadPad((dpy), (char *)(data), (len))
#define _XRead16(dpy, data, len) _XRead((dpy), (char *)(data), (len))
#ifdef LONG64
#define Data32(dpy, data, len) _XData32(dpy, (long *)data, len)
#else
#define Data32(dpy, data, len) Data((dpy), (char *)(data), (len))
#define _XRead32(dpy, data, len) _XRead((dpy), (char *)(data), (len))
#endif

#define PackData16(dpy,data,len) Data16 (dpy, data, len)
#define PackData32(dpy,data,len) Data32 (dpy, data, len)

/* Xlib manual is bogus */
#define PackData(dpy,data,len) PackData16 (dpy, data, len)

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

#define CI_NONEXISTCHAR(cs) (((cs)->width == 0) && \
			     (((cs)->rbearing|(cs)->lbearing| \
			       (cs)->ascent|(cs)->descent) == 0))

/*
 * CI_GET_CHAR_INFO_1D - return the charinfo struct for the indicated 8bit
 * character.  If the character is in the column and exists, then return the
 * appropriate metrics (note that fonts with common per-character metrics will
 * return min_bounds).  If none of these hold true, try again with the default
 * char.
 */
#define CI_GET_CHAR_INFO_1D(fs,col,def,cs) \
{ \
    cs = def; \
    if (col >= fs->min_char_or_byte2 && col <= fs->max_char_or_byte2) { \
	if (fs->per_char == NULL) { \
	    cs = &fs->min_bounds; \
	} else { \
	    cs = &fs->per_char[(col - fs->min_char_or_byte2)]; \
	    if (CI_NONEXISTCHAR(cs)) cs = def; \
	} \
    } \
}

#define CI_GET_DEFAULT_INFO_1D(fs,cs) \
  CI_GET_CHAR_INFO_1D (fs, fs->default_char, NULL, cs)



/*
 * CI_GET_CHAR_INFO_2D - return the charinfo struct for the indicated row and
 * column.  This is used for fonts that have more than row zero.
 */
#define CI_GET_CHAR_INFO_2D(fs,row,col,def,cs) \
{ \
    cs = def; \
    if (row >= fs->min_byte1 && row <= fs->max_byte1 && \
	col >= fs->min_char_or_byte2 && col <= fs->max_char_or_byte2) { \
	if (fs->per_char == NULL) { \
	    cs = &fs->min_bounds; \
	} else { \
	    cs = &fs->per_char[((row - fs->min_byte1) * \
			        (fs->max_char_or_byte2 - \
				 fs->min_char_or_byte2 + 1)) + \
			       (col - fs->min_char_or_byte2)]; \
	    if (CI_NONEXISTCHAR(cs)) cs = def; \
        } \
    } \
}

#define CI_GET_DEFAULT_INFO_2D(fs,cs) \
{ \
    unsigned int r = (fs->default_char >> 8); \
    unsigned int c = (fs->default_char & 0xff); \
    CI_GET_CHAR_INFO_2D (fs, r, c, NULL, cs); \
}


#ifdef MUSTCOPY

/* for when 32-bit alignment is not good enough */
#define OneDataCard32(dpy,dstaddr,srcvar) \
  { dpy->bufptr -= 4; Data32 (dpy, (char *) &(srcvar), 4); }

#else

/* srcvar must be a variable for large architecture version */
#define OneDataCard32(dpy,dstaddr,srcvar) \
  { *(CARD32 *)(dstaddr) = (srcvar); }

#endif /* MUSTCOPY */

typedef struct _XInternalAsync {
    struct _XInternalAsync *next;
    Bool (*handler)();
    XPointer data;
} _XAsyncHandler;

typedef struct _XAsyncEState {
    unsigned long min_sequence_number;
    unsigned long max_sequence_number;
    unsigned char error_code;
    unsigned char major_opcode;
    unsigned short minor_opcode;
    unsigned char last_error_received;
    int error_count;
} _XAsyncErrorState;

#define DeqAsyncHandler(dpy,handler) { \
    if (dpy->async_handlers == (handler)) \
	dpy->async_handlers = (handler)->next; \
    else \
	_XDeqAsyncHandler(dpy, handler); \
    }

/*
 * This structure is private to the library.
 */
typedef struct _XFreeFuncs {
    void (*atoms)();		/* _XFreeAtomTable */
    int (*modifiermap)();	/* XFreeModifierMap */
    void (*key_bindings)();	/* _XFreeKeyBindings */
    void (*context_db)();	/* _XFreeContextDB */
    void (*defaultCCCs)();	/* _XcmsFreeDefaultCCCs */
    void (*clientCmaps)();	/* _XcmsFreeClientCmaps */
    void (*intensityMaps)();	/* _XcmsFreeIntensityMaps */
    void (*im_filters)();	/* _XFreeIMFilters */
    void (*xkb)();		/* _XkbFreeInfo */
} _XFreeFuncRec;

/*
 * This structure is private to the library.
 */
typedef struct _XExten {	/* private to extension mechanism */
	struct _XExten *next;	/* next in list */
	XExtCodes codes;	/* public information, all extension told */
	int (*create_GC)();	/* routine to call when GC created */
	int (*copy_GC)();	/* routine to call when GC copied */
	int (*flush_GC)();	/* routine to call when GC flushed */
	int (*free_GC)();	/* routine to call when GC freed */
	int (*create_Font)();	/* routine to call when Font created */
	int (*free_Font)();	/* routine to call when Font freed */
	int (*close_display)();	/* routine to call when connection closed */
	int (*error)();		/* who to call when an error occurs */
        char *(*error_string)();  /* routine to supply error string */
	char *name;		/* name of this extension */
	void (*error_values)(); /* routine to supply error values */
	void (*before_flush)();	/* routine to call when sending data */
	struct _XExten *next_flush; /* next in list of those with flushes */
} _XExtension;

/* extension hooks */

_XFUNCPROTOBEGIN

#ifdef DataRoutineIsProcedure
extern void Data();
#endif
extern int _XError(
    Display*	/* dpy */,
    xError*	/* rep */
);
extern int _XIOError(
    Display*	/* dpy */
);
extern int (*_XIOErrorFunction)(
    Display*	/* dpy */
);
extern int (*_XErrorFunction)(
    Display*		/* dpy */,
    XErrorEvent*	/* error_event */
);
extern void _XEatData(
    Display*		/* dpy */,
    unsigned long	/* n */
);
extern char *_XAllocScratch(
    Display*		/* dpy */,
    unsigned long	/* nbytes */
);
extern char *_XAllocTemp(
    Display*		/* dpy */,
    unsigned long	/* nbytes */
);
extern void _XFreeTemp(
    Display*		/* dpy */,
    char*		/* buf */,
    unsigned long	/* nbytes */
);
extern Visual *_XVIDtoVisual(
    Display*	/* dpy */,
    VisualID	/* id */
);
extern unsigned long _XSetLastRequestRead(
    Display*		/* dpy */,
    xGenericReply*	/* rep */
);
extern int _XGetHostname(
    char*	/* buf */,
    int		/* maxlen */
);
extern Screen *_XScreenOfWindow(
    Display*	/* dpy */,
    Window	/* w */
);
extern Bool _XAsyncErrorHandler(
    Display*	/* dpy */,
    xReply*	/* rep */,
    char*	/* buf */,
    int		/* len */,
    XPointer	/* data */
);
extern char *_XGetAsyncReply(
    Display*	/* dpy */,
    char*	/* replbuf */,
    xReply*	/* rep */,
    char*	/* buf */,
    int		/* len */,
    int		/* extra */,
    Bool	/* discard */
);
extern _XFlush(
    Display*	/* dpy */
);
extern int _XEventsQueued(
    Display*	/* dpy */,
    int 	/* mode */
);
extern _XReadEvents(
    Display*	/* dpy */
);
extern _XRead(
    Display*	/* dpy */,
    char*	/* data */,
    long	/* size */
);
extern _XReadPad(
    Display*	/* dpy */,
    char*	/* data */,
    long	/* size */
);
extern _XSend(
    Display*		/* dpy */,
    const char*		/* data */,
    long		/* size */
);
extern Status _XReply(
    Display*	/* dpy */,
    xReply*	/* rep */,
    int		/* extra */,
    Bool	/* discard */
);
extern _XEnq(
    Display*	/* dpy */,
    xEvent*	/* event */
);
extern _XDeq(
    Display*	/* dpy */,
    _XQEvent*	/* prev */,
    _XQEvent*	/* qelt */
);

extern int (*XESetCreateGC(
    Display*		/* display */,
    int			/* extension */,
    int (*) (
	      Display*			/* display */,
	      GC			/* gc */,
	      XExtCodes*		/* codes */
	    )		/* proc */
))(
    Display*, GC, XExtCodes*
);

extern int (*XESetCopyGC(
    Display*		/* display */,
    int			/* extension */,
    int (*) (
	      Display*			/* display */,
              GC			/* gc */,
              XExtCodes*		/* codes */
            )		/* proc */
))(
    Display*, GC, XExtCodes*
);

extern int (*XESetFlushGC(
    Display*		/* display */,
    int			/* extension */,
    int (*) (
	      Display*			/* display */,
              GC			/* gc */,
              XExtCodes*		/* codes */
            )		/* proc */
))(
    Display*, GC, XExtCodes*
);

extern int (*XESetFreeGC(
    Display*		/* display */,
    int			/* extension */,
    int (*) (
	      Display*			/* display */,
              GC			/* gc */,
              XExtCodes*		/* codes */
            )		/* proc */
))(
    Display*, GC, XExtCodes*
);

extern int (*XESetCreateFont(
    Display*		/* display */,
    int			/* extension */,
    int (*) (
	      Display*			/* display */,
              XFontStruct*		/* fs */,
              XExtCodes*		/* codes */
            )		/* proc */
))(
    Display*, XFontStruct*, XExtCodes*
);

extern int (*XESetFreeFont(
    Display*		/* display */,
    int			/* extension */,
    int (*) (
	      Display*			/* display */,
              XFontStruct*		/* fs */,
              XExtCodes*		/* codes */
            )		/* proc */
))(
    Display*, XFontStruct*, XExtCodes*
);

extern int (*XESetCloseDisplay(
    Display*		/* display */,
    int			/* extension */,
    int (*) (
	      Display*			/* display */,
              XExtCodes*		/* codes */
            )		/* proc */
))(
    Display*, XExtCodes*
);

extern int (*XESetError(
    Display*		/* display */,
    int			/* extension */,
    int (*) (
	      Display*			/* display */,
              xError*			/* err */,
              XExtCodes*		/* codes */,
              int*			/* ret_code */
            )		/* proc */
))(
    Display*, xError*, XExtCodes*, int*
);

extern char* (*XESetErrorString(
    Display*		/* display */,
    int			/* extension */,
    char* (*) (
	        Display*		/* display */,
                int			/* code */,
                XExtCodes*		/* codes */,
                char*			/* buffer */,
                int			/* nbytes */
              )		/* proc */
))(
    Display*, int, XExtCodes*, char*, int
);

extern void (*XESetPrintErrorValues (
    Display*		/* display */,
    int			/* extension */,
    void (*)(
	      Display*			/* display */,
	      XErrorEvent*		/* ev */,
	      void*			/* fp */
	     )		/* proc */
))(
    Display*, XErrorEvent*, void*
);

extern Bool (*XESetWireToEvent(
    Display*		/* display */,
    int			/* event_number */,
    Bool (*) (
	       Display*			/* display */,
               XEvent*			/* re */,
               xEvent*			/* event */
             )		/* proc */
))(
    Display*, XEvent*, xEvent*
);

extern Status (*XESetEventToWire(
    Display*		/* display */,
    int			/* event_number */,
    Status (*) (
	      Display*			/* display */,
              XEvent*			/* re */,
              xEvent*			/* event */
            )		/* proc */
))(
    Display*, XEvent*, xEvent*
);

extern Bool (*XESetWireToError(
    Display*		/* display */,
    int			/* error_number */,
    Bool (*) (
	       Display*			/* display */,
	       XErrorEvent*		/* he */,
	       xError*			/* we */
            )		/* proc */
))(
    Display*, XErrorEvent*, xError*
);

extern void (*XESetBeforeFlush(
    Display*		/* display */,
    int			/* error_number */,
    void (*) (
	       Display*			/* display */,
	       XExtCodes*		/* codes */,
	       char*			/* data */,
	       long			/* len */
            )		/* proc */
))(
    Display*, XExtCodes*, char*, long
);

/* internal connections for IMs */

typedef void (*_XInternalConnectionProc)(
    Display*			/* dpy */,
    int				/* fd */,
    XPointer			/* call_data */
);


extern Status _XRegisterInternalConnection(
    Display*			/* dpy */,
    int				/* fd */,
    _XInternalConnectionProc	/* callback */,
    XPointer			/* call_data */
);

extern void _XUnregisterInternalConnection(
    Display*			/* dpy */,
    int				/* fd */
);

/* Display structure has pointers to these */

struct _XConnectionInfo {	/* info from _XRegisterInternalConnection */
    int fd;
    _XInternalConnectionProc read_callback;
    XPointer call_data;
    XPointer *watch_data;	/* set/used by XConnectionWatchProc */
    struct _XConnectionInfo *next;
};

struct _XConnWatchInfo {	/* info from XAddConnectionWatch */
    XConnectionWatchProc fn;
    XPointer client_data;
    struct _XConnWatchInfo *next;
};

extern int XTextHeight(
    XFontStruct*	/* font_struct */,
    const char*		/* string */,
    int			/* count */
);

extern int XTextHeight16(
    XFontStruct*	/* font_struct */,
    const XChar2b*	/* string */,
    int			/* count */
);

_XFUNCPROTOEND
