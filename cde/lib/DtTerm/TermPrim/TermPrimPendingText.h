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

#ifdef	__cplusplus
} /* close scope of 'extern "C"'... */
#endif	/* __cplusplus */

#endif /* _Dt_TermPrimPendingText_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */

