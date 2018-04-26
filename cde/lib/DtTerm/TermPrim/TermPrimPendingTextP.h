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
** $TOG: TermPrimPendingTextP.h /main/3 1997/07/03 15:40:29 samborn $
*/
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#ifndef   _Dt_TermPrimPendingTextP_h
#define   _Dt_TermPrimPendingTextP_h

#include "TermPrimPendingText.h"

#ifdef	   __cplusplus
extern "C" {
#endif	/* __cplusplus */

#define DEFAULT_CHUNK_BUF_SIZE  1024

#define	TextIsPending(list)	(list->head->next != list->tail)

typedef struct _PendingTextChunkRec
{
    unsigned char      *buffer;         /* beginning of buffer             */
    int                 buffLen;        /* length of buffer                */
    unsigned char      *bufPtr;         /* text remaining to be processed  */
    int                 len;            /* bytes remaining to be processed */
    PendingTextChunk    next;           /* next chunk in list              */
    PendingTextChunk    prev;           /* prev chunk in list              */
} PendingTextChunkRec;

typedef struct _PendingTextRec
{
    PendingTextChunk    head;
    PendingTextChunk    tail;
#ifdef   RECYCLE_CHUNKS
    PendingTextChunk    free;
#endif /* RECYCLE_CHUNKS */
} PendingTextRec;
    
/*
 * define the maximum number of character per write for a pty.  This
 * makes dtterm behave similarly to xterm and dxterm and worksaround
 * the pty flow control problems.
 */
#define MAX_PTY_WRITE 128 /* this is 1/2 POSIX minimum MAX_INPUT */

#ifdef	__cplusplus
} /* close scope of 'extern "C"'... */
#endif	/* __cplusplus */

#endif /* _Dt_TermPrimPendingTextP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
