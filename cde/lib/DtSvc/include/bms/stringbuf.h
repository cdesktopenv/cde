/*
 * File:         stringbuf.h $XConsortium: stringbuf.h /main/3 1995/10/26 15:49:00 rswiston $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#ifndef _stringbuf_h
#define _stringbuf_h

typedef struct _XeStringBuffer {
   int last_char;  /* index of trailing NULL */
   int size;
   int increment;  /* how much to expand when more space is needed */
   XeString buffer;
} *XeStringBuffer;


int XeAppendToStringBuffer 
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (XeStringBuffer buffer, XeString string);
#else
   ();
#endif

XeStringBuffer XeMakeStringBuffer 
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (int increment_size);
#else
   ();
#endif

	/* erase the contents (but don't deallocate any space). */
void XeClearStringBuffer 
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (XeStringBuffer buffer);
#else
   ();
#endif

/*** add nothing after the following endif ***/
#endif /* _stringbuf_h */
