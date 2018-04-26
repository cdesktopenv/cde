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
