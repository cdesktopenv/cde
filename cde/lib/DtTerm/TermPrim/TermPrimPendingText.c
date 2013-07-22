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
#ifndef lint
#ifdef  VERBOSE_REV_INFO
static char rcs_id[] = "$TOG: TermPrimPendingText.c /main/3 1997/07/03 15:40:05 samborn $";
#endif  /* VERBOSE_REV_INFO */
#endif  /* lint */

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include <stdio.h>
#include <X11/Intrinsic.h>
#include "TermHeader.h"
#include "TermPrimDebug.h"
#include "TermPrimPendingTextP.h"

#ifdef __osf__
#include <termios.h>
#include <poll.h>
#endif /* __osf__ */

static
PendingTextChunk
mallocChunk(int len);

#ifdef    DEBUG
void
walkPendingText
(
    PendingText     list
)
{
    PendingTextChunk chunk;

    fprintf(stderr, "head: %lx\n", list->head);
    fprintf(stderr, "tail: %lx\n", list->tail);

    for (chunk = list->head; chunk != list->tail; chunk = chunk->next)
    {
        fprintf(stderr, "chunk: 0x%lx\n", chunk);
        fprintf(stderr, "    buffer : %c\n", chunk->buffer[0]);
        fprintf(stderr, "    buffLen: %d\n", chunk->buffLen);
        fprintf(stderr, "    bufPtr : %c\n", chunk->bufPtr[0]);
        fprintf(stderr, "    len    : %d\n", chunk->len);
        fprintf(stderr, "    next   : 0x%lx\n", chunk->next);
        fprintf(stderr, "    prev   : 0x%lx\n", chunk->prev);
    }
}
#endif /* DEBUG  */

/* 
** Allocate, and initialize a new PendingTextChunk.
*/
static 
PendingTextChunk
mallocChunk(int len)
{
    PendingTextChunk    newChunk;
    
    Debug('q', fprintf(stderr, ">>mallocChunk() starting\n"));

    newChunk = (PendingTextChunk) XtMalloc(sizeof(PendingTextChunkRec));
    if (!newChunk)
    {
        return((PendingTextChunk) NULL);
    }

    newChunk->buffLen = len;
    newChunk->buffer  = (unsigned char *) XtMalloc(newChunk->buffLen);
    if (!newChunk->buffer)
    {
        XtFree((char *) newChunk);
        return((PendingTextChunk) NULL);
    }
    
    /*
    ** Finish initializing the new chunk.
    */
    newChunk->bufPtr = newChunk->buffer;
    newChunk->len   = 0;
    newChunk->next  = (PendingTextChunk)NULL;
    newChunk->prev  = (PendingTextChunk)NULL;
    return(newChunk);
}

/* 
** Add a new pending text chunk to the end of the list.  If possible, reuse
** an existing chunk, else allocate a new one.   Return true if successful,
** else return false.
*/
PendingTextChunk
_DtTermPrimPendingTextAppendChunk
(
    PendingText list,
    int		len
)
{
    PendingTextChunk    newChunk;

    Debug('q', fprintf(stderr, ">>_DtTermPrimPendingTextAppendChunk() starting\n"));
#ifdef    DEBUG
    walkPendingText(list);
#endif /* DEBUG  */
#ifdef    RECYCLE_CHUNKS
    if (list->free)
    {
        /* 
        ** There are chunks on the free list, this will
        ** be easy.
        */
        newChunk = list->free;
    }
    else
    {
        /*
        ** We have no free chunks.
        */
        newChunk = mallocChunk(len);
        
        if (!newChunk)
        {
            return(PendingTextChunk(NULL));
        }
    }   
    /* 
    ** append the new chunk to the list...
    */
    list->free       = list->free->next;
    newChunk->next   = (PendingTextChunk)NULL;
    list->tail->next = newChunk;
    list->tail       = newChunk;
    return(True);
#else  /* RECYCLE_CHUNKS */
    /*
    ** Create a new chunk.
    */
    newChunk = mallocChunk(len);
    
    if (!newChunk)
    {
        return((PendingTextChunk)NULL);
    }
    newChunk->next = list->tail;
    newChunk->prev = list->tail->prev;
    list->tail->prev->next = newChunk;
    list->tail->prev = newChunk;
    return(newChunk);
#endif /* RECYCLE_CHUNKS */
}

void
_DtTermPrimPendingTextReplace
(
    PendingTextChunk chunk,
    unsigned char *buffer,
    int bufferLen
)
{

    chunk->buffer = (unsigned char *) XtRealloc((char *) chunk->buffer,
	    bufferLen);
    chunk->buffLen = bufferLen;
    chunk->bufPtr = chunk->buffer;
    chunk->len = bufferLen;
    (void) memmove(chunk->buffer, buffer, bufferLen);
}

/* 
** Remove a pending text chunk from the head of the list, and add it to
** the free list.
*/
void
_DtTermPrimPendingTextRemoveChunk
(
    PendingText list,
    PendingTextChunk chunk
)
{
    Debug('q', fprintf(stderr, ">>_DtTermPrimPendingTextRemoveChunk() starting\n"));
#ifdef    DEBUG
    walkPendingText(list);
#endif /* DEBUG  */
    /* 
    ** add the chunk to the free list...
    */
    /* don't allow removal of either head or tail... */
    if ((chunk == list->head) || (chunk == list->tail)) {
	return;
    }

    chunk->prev->next = chunk->next;
    chunk->next->prev = chunk->prev;
#ifdef    RECYCLE_CHUNKS
    chunk->len  = 0;
    chunk->next = list->free;
    list->free  = chunk;
#else  /* RECYCLE_CHUNKS */
    XtFree((char *) chunk->buffer);
    XtFree((char *) chunk);    
#endif /* RECYCLE_CHUNKS */
}

/* Check to see if there is any pending text in the pending text list...
 */
Boolean
_DtTermPrimPendingTextIsPending
(
    PendingText list
)
{
    if (list->head->next != list->tail) {
	return(True);
    }
    return(False);
}

/* 
** Get a pending text chunk from the head of the list.
*/
PendingTextChunk
_DtTermPrimPendingTextGetChunk
(
    PendingText list
)
{
    Debug('q', fprintf(stderr, ">>_DtTermPrimPendingTextGetChunk() starting\n"));
#ifdef    DEBUG
    walkPendingText(list);
#endif /* DEBUG  */

    if (list->head->next != list->tail) {
	return(list->head->next);
    } else {
	return((PendingTextChunk) 0);
    }
}

/* 
** Flush a pending text list.  This function takes the easy way out of
** calling _DtTermPrimPendingTextGetChunk() to get each chunk and
** _DtTermPrimPendingTextRemoveChunk() to remove them.
*/
void
_DtTermPrimPendingTextFlush
(
    PendingText list
)
{
    PendingTextChunk chunk;

    Debug('q', fprintf(stderr, ">>_DtTermPrimPendingTextFlush() starting\n"));
#ifdef	DEBUG
    walkPendingText(list);
#endif	/* DEBUG */
    
    while ((chunk = _DtTermPrimPendingTextGetChunk(list))) {
	(void) _DtTermPrimPendingTextRemoveChunk(list, chunk);
    }
}

/* 
** Append the supplied text to the pending text list.  Return True if
** all text was appended, else return False.
*/
Boolean
_DtTermPrimPendingTextAppend
(
    PendingText     list,
    unsigned char  *text,
    int		    len
)
{
    PendingTextChunk    newChunk;
    PendingTextChunk    oldTail;

    Debug('q', fprintf(stderr, ">>_DtTermPrimPendingTextAppend() starting\n"));
#ifdef    DEBUG
    walkPendingText(list);
#endif /* DEBUG  */
    /* 
    ** remember this if we are unable to get all the text
    ** into the list
    */
    oldTail = list->tail->prev;

    /* 
    ** add a new chunk to the list
    */
    while (len > 0)
    {
        newChunk = _DtTermPrimPendingTextAppendChunk(list,
		(len > DEFAULT_CHUNK_BUF_SIZE) ? DEFAULT_CHUNK_BUF_SIZE : len);
        if (!newChunk)
        {
            /* 
            ** the allocation failed, free up all newly created
            ** chunks and return...
            */
            while (oldTail != list->tail->prev)
            {
                _DtTermPrimPendingTextRemoveChunk(list, list->tail->prev);
            }
            return(False);
        }
        /* 
        ** chunk buffers are a fixed size, copy a much as possible
        ** from 'text' to the chunk buffer, then adjust 'len'...
        */
        newChunk->len = MIN(len, newChunk->buffLen);
        (void)memcpy(newChunk->buffer, text, newChunk->len);
        len -= newChunk->buffLen;
    }    
    return(True);
}

/* 
** Write a pending text chunk from the head of the list.
*/
void
_DtTermPrimPendingTextWrite
(
    PendingText     list,
    int             fd
)
{
    int                 bytesWritten = 0;
    PendingTextChunk	chunk;

    Debug('q', fprintf(stderr, ">>_DtTermPrimPendingTextWrite() starting\n"));
#ifdef    DEBUG
    walkPendingText(list);
#endif /* DEBUG  */
    chunk = list->head->next;
    Debug('q', fprintf(stderr, ">>       len: %3.3d\n", chunk->len));
    Debug('q', fprintf(stderr, ">>    bufPtr: <%*.*s>\n", 
		       chunk->len, chunk->len, chunk->bufPtr));

    bytesWritten = write(fd, chunk->bufPtr, chunk->len <= MAX_PTY_WRITE ?
			 chunk->len : MAX_PTY_WRITE);

    if (bytesWritten < 0) {

#ifdef DEBUG       
        fprintf(stderr, "_DtTermPrimPendingTextWrite: write failed\n");
#endif
       bytesWritten = 0;
    }

    if ((chunk->len -= bytesWritten) <= 0) {
        /* 
	** All text in this chunk has been written,
	** remove it from the list.
	*/
        _DtTermPrimPendingTextRemoveChunk(list, chunk);
    }
    else {
	/* 
	** only some of the text in this chunk was written,
	** simply adjust the write pointer...
	** (list->head->len was adjusted above)
	*/
	chunk->bufPtr += bytesWritten;
	
    }
}

PendingText
_DtTermPrimPendingTextCreate(
    void
)
{
    PendingText ptr;

    Debug('q', fprintf(stderr, ">>_DtTermPrimPendingTextCreate() starting\n"));

    ptr = (PendingText) XtMalloc(sizeof(PendingTextRec));
    (void) memset(ptr, '\0', sizeof(PendingTextRec));
    ptr->head = (PendingTextChunk) XtMalloc(sizeof(PendingTextChunkRec));
    (void) memset(ptr->head, '\0', sizeof(PendingTextChunkRec));
    ptr->tail = (PendingTextChunk) XtMalloc(sizeof(PendingTextChunkRec));
    (void) memset(ptr->tail, '\0', sizeof(PendingTextChunkRec));

    ptr->head->next = ptr->tail;
    ptr->tail->prev = ptr->head;

#ifdef    DEBUG
    walkPendingText(ptr);
#endif /* DEBUG  */
    return(ptr);
}

void
_DtTermPrimPendingTextDestroy(
    PendingText ptr
)
{
    PendingTextChunk chunk;

    Debug('q', fprintf(stderr, ">>_DtTermPrimPendingTextDestroy() starting\n"));
#ifdef    DEBUG
#undef    DEBUG
    walkPendingText(ptr);
#endif /* DEBUG  */
    if (ptr) {
	while (ptr->head) {
	    chunk = ptr->head;
	    ptr->head = ptr->head->next;
	    if (chunk->buffer) {
		(void) XtFree((char *) chunk->buffer);
	    }
	    (void) XtFree((char *) chunk);
	}
	(void) XtFree((char *) ptr);
    }
}


