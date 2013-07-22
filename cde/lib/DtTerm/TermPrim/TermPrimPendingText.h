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
/* 
** $XConsortium: TermPrimPendingText.h /main/1 1996/04/21 19:18:33 drk $
*/
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#ifndef   _Dt_TermPrimPendingText_h
#define   _Dt_TermPrimPendingText_h

#include <X11/Intrinsic.h>

#define TextIsPendingWrite(list) (list.head != NULL)
    
#ifdef	   __cplusplus
extern "C" {
#endif	/* __cplusplus */

typedef struct _PendingTextChunkRec   *PendingTextChunk;
typedef struct _PendingTextRec        *PendingText;

extern
Boolean
_DtTermPrimPendingTextIsPending
(
    PendingText list
);

extern
PendingTextChunk
_DtTermPrimPendingTextGetChunk
(
    PendingText list
);

extern
PendingTextChunk
_DtTermPrimPendingTextAppendChunk
(
    PendingText list,
    int		len
);

extern
void
_DtTermPrimPendingTextReplace
(
    PendingTextChunk chunk,
    unsigned char *buffer,
    int bufferLen
);

extern
void
_DtTermPrimPendingTextRemoveChunk
(
    PendingText		list,
    PendingTextChunk	chunk
);

extern
void
_DtTermPrimPendingTextFlush
(
    PendingText list
);

extern
Boolean
_DtTermPrimPendingTextAppendText
(
    PendingText     list,
    unsigned char  *text,
    int		    len
);

extern
void
_DtTermPrimPendingTextWrite
(
    PendingText     list,
    int             fd
);

extern
PendingText
_DtTermPrimPendingTextCreate
(
    void
);

extern
void
_DtTermPrimPendingTextDestroy
(
    PendingText	    ptr
);

extern
Boolean
_DtTermPrimPendingTextAppend
(
    PendingText     list,
    unsigned char  *text,
    int             len
);

#ifdef	__cplusplus
} /* close scope of 'extern "C"'... */
#endif	/* __cplusplus */

#endif /* _Dt_TermPrimPendingText_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */

