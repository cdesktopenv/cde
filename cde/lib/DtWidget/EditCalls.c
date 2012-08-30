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
/* $XConsortium: EditCalls.c /main/4 1996/03/26 19:53:27 drk $
 **********************************<+>*************************************
 ***************************************************************************
 **
 **  File:        EditCalls.c
 **
 **  Project:     DtEditor widget interface for text edit services.
 **
 **  Description: Contains the public functions related to undo,
 **               cut, copy, paste, and the internal Modify/Verify callback.
 **  -----------
 **
 *******************************************************************
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1993, 1994, 1996 Hewlett-Packard Company.
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp.
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 * (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of Novell, Inc.
 ********************************************************************
 **
 **
 **************************************************************************
 **********************************<+>*************************************/

#include "EditorP.h"
#include "DtWidgetI.h"
#include <Xm/XmPrivate.h>  /* _XmStringSourceGetString */

void
_DtEditorResetUndo(
        DtEditorWidget editor)
{

  /*
   * Reset deletion & insertion contexts.
   */

  if( (M_deletedText(editor) != (char *)NULL) &&
      (strlen(M_deletedText(editor)) != 0) )
  {
    XtFree(M_deletedText(editor));
    M_deletedText(editor) = (char *) NULL;
  } 
    
  M_deletionStart(editor) = NO_DELETION_IN_PROGRESS;
  M_insertStart(editor) = 0;
  M_insertionLength(editor) = 0;

} /* end _DtEditorResetUndo */


Boolean
DtEditorUndoEdit(
	Widget widget)
{
    DtEditorWidget pPriv = (DtEditorWidget) widget;
    _DtWidgetToAppContext(widget);
    _DtAppLock(app);

    /*
     * Remove any insertion, and then put back any previous deletion.  
     * The tricky part is that removing the insertion looks like a deletion 
     * and the modifyVerify callback will save in the undo buffer, wiping
     * out any previous deletion (before we can reinsert it).  So we have to
     * save the previous deletion before undoing the insertion so we can
     * reinsert it.  Make sense?
     */
    if(M_insertionLength(pPriv) != 0)
    {
	/*
	 * There is an insertion so remove everything we just inserted
	 */
	if( (M_deletedText(pPriv) != (char *)NULL) && 
	    (strlen(M_deletedText(pPriv)) != 0) )
	{
	    /*
	     * If there is a current deletion, save it before removing the
	     * insertion so we can put it back (i.e. undo the deletion).
	     */
	    int oldDeleteStart = M_deletionStart(pPriv);
	    char *oldDeletion = M_deletedText(pPriv);
	    M_deletedText(pPriv) = (char *)NULL;

	    /*
	     * 1. Delete the last insertion.
	     */
	    XmTextSetSelection(M_text(pPriv), M_insertStart(pPriv), 
			       M_insertStart(pPriv) + M_insertionLength(pPriv),
			       CurrentTime);

	    M_insertionLength(pPriv)=0;
	    XmTextRemove(M_text(pPriv));

	    /*
	     * 2. Put back the previous deletion.
	     */
	    XmTextInsert(M_text(pPriv), oldDeleteStart, oldDeletion);

	    XtFree(oldDeletion);
	}
	else
	{
	    /* 
	     * There is no deletion, so we just have to remove the 
	     * insertion.
	     */
	    XmTextSetSelection(M_text(pPriv), M_insertStart(pPriv), 
			       M_insertStart(pPriv) + M_insertionLength(pPriv),
			       CurrentTime);
	    M_insertionLength(pPriv) = 0;
	    XmTextRemove(M_text(pPriv));
	}
    }
    else if(M_deletedText(pPriv) != (char *)NULL)
    {
	/*
	 * Nothing has been inserted so just undo the previous deletion.
	 */

	char *oldDeletion = M_deletedText(pPriv);
	M_deletedText(pPriv) = (char *)NULL;

	XmTextInsert(M_text(pPriv), M_deletionStart(pPriv), oldDeletion);

	XtFree(oldDeletion);
    }
    else
    {
      /* 
       * There is no insertion to remove or deletion to put back in
       * (i.e. nothing to undo) so return False.
       */
      _DtAppUnlock(app);
      return(False);
    }

  _DtAppUnlock(app);
  return(True);

} /* end DtEditorUndoEdit */

Boolean
DtEditorCutToClipboard(
	Widget widget)
{

    DtEditorWidget editor = (DtEditorWidget) widget;
    XEvent *event;
    _DtWidgetToAppContext(widget);
    _DtAppLock(app);

    /*
     * Create an event with a correct timestamp
     */
    event = (XEvent *) XtMalloc( sizeof(XEvent) );
    event->xkey.time = XtLastTimestampProcessed( M_display(editor) );

    /*
     * Call routine to cut selection to clipboard
     */
    XtCallActionProc(M_text(editor), "cut-clipboard", event, NULL, 0);

    XtFree( (char *) event );

    _DtAppUnlock(app);
    return(True);

}

Boolean
DtEditorCopyToClipboard(
	Widget widget)
{
    DtEditorWidget editor = (DtEditorWidget) widget;
    XEvent *event;
    _DtWidgetToAppContext(widget);
    _DtAppLock(app);

    /*
     * Create an event with a correct timestamp
     */
    event = (XEvent *) XtMalloc( sizeof(XEvent) );
    event->xkey.time = XtLastTimestampProcessed( M_display(editor) );

    /*
     * Call routine to copy selection to clipboard
     */
    XtCallActionProc(M_text(editor), "copy-clipboard", event, NULL, 0);

    XtFree( (char *) event );

    _DtAppUnlock(app);
    return(True);
}

Boolean
DtEditorPasteFromClipboard(
	Widget widget)
{
    DtEditorWidget editor = (DtEditorWidget) widget;
    XEvent *event;
    _DtWidgetToAppContext(widget);
    _DtAppLock(app);

    /*
     * Create an event with a correct timestamp
     */
    event = (XEvent *) XtMalloc( sizeof(XEvent) );
    event->xkey.time = XtLastTimestampProcessed( M_display(editor) );

    /*
     * Call routine to paste contents of clipboard at insertion cursor
     */
    XtCallActionProc(M_text(editor), "paste-clipboard", event, NULL, 0);

    XtFree( (char *) event );

    _DtAppUnlock(app);
    return(True);

}


/*
 * SetUndoDeletionState maintains the contents of editStuff.undo related
 * to deletion (the deleted text & its original starting position).  
 *
 * SetUndoDeletionState can also reset/invalidate the undo contents related to
 * insertion if it detects a "new" deletion.  The idea is to treat consecutive
 * deletions as atomic from the viewpoint of undo.  A delete is 
 * non-consecutive if it's start or end position is not coincidental with
 * the last deletion.  One set of consecutive insertions is allowed following
 * deletions, and will be undone by the DtEditorUndo().  Non-consecutive
 * insertions will reset/invalidate the deletion undo buffer.
 */
static void 
SetUndoDeletionState(
        DtEditorWidget pPriv,
        XmTextVerifyCallbackStruct *cb)
{
    char *pDeletedText;

    /* 
     * Get the text which will be deleted from the text widget.
     */
    pDeletedText = (char *)_XmStringSourceGetString( 
						(XmTextWidget) M_text(pPriv), 
						cb->startPos, 
						cb->endPos, 
						False);

    if( M_deletedText(pPriv) != (char *)NULL    && 
	M_insertionLength(pPriv) == 0           &&
        (cb->startPos == M_deletionStart(pPriv) || 
	 cb->endPos == M_deletionStart(pPriv))      
      )
    {
        /*
         * Continuation of the current deletion.  For a continuation, there
	 * must have been no intervening insertions, and we must be deleting
	 * from the same point, either forward or backward.
         */
        char *oldUndo = M_deletedText(pPriv);
        M_deletedText(pPriv) = XtMalloc( strlen(M_deletedText(pPriv)) +
			                 strlen(pDeletedText) + 1 );
    
        if(cb->startPos == M_deletionStart(pPriv)) {
	    /*
	     * deleting forward - deletionStart remains the same.
	     */
	    strcpy(M_deletedText(pPriv), oldUndo);
            strcat(M_deletedText(pPriv), pDeletedText);
        }
        else {
	    /*
	     * deleting backward (e.g. Backspace)
	     */
            strcpy(M_deletedText(pPriv), pDeletedText);
	    strcat(M_deletedText(pPriv), oldUndo);
	    M_deletionStart(pPriv) = cb->startPos;
        }
        if(oldUndo != (char *)NULL)
	    XtFree(oldUndo);
    }
    else 
    {
        /*
         * Starting a new deletion context.  Replace the old deletion 
	 * context, and remove the insertion context.
         */
	_DtEditorResetUndo( pPriv );

        M_deletedText(pPriv) = XtMalloc(strlen(pDeletedText) + 1);
    
        strcpy(M_deletedText(pPriv), pDeletedText);
        M_deletionStart(pPriv) = cb->startPos;
	M_insertStart(pPriv) = cb->startPos;
	M_insertionLength(pPriv) = 0;
    }

    if( pDeletedText != (char *)NULL )
      XtFree( pDeletedText );
} /* SetUndoDeletionState */

/*
 * SetUndoInsertionState maintains the contents of editStuff.undo related
 * to insertions (the number of characters inserted & the position of the
 * first one).
 */
static void 
SetUndoInsertionState(
        DtEditorWidget pPriv,
        XmTextVerifyCallbackStruct *cb)
{
    if(M_insertionLength(pPriv) == 0) {
	/*
	 * We've started a new deletion context, so reset the insertion
	 * context.
	 */
	M_insertStart(pPriv) = cb->startPos;
	M_insertionLength(pPriv) = _DtEditor_CountCharacters(cb->text->ptr,
						    cb->text->length);
    }
    else
    {
	/*
	 * Determine if we're continuing the current insertion context
	 * or beginning a new one.
	 */
	if(cb->startPos == (M_insertStart(pPriv) + M_insertionLength(pPriv)))
	    M_insertionLength(pPriv) += _DtEditor_CountCharacters(
	  				    cb->text->ptr, cb->text->length);
	else
	{
	    /*
	     * We're starting a new insertion context, so invalidate any
	     * existing deletion context, and reset the insertion context.
	     */
	    _DtEditorResetUndo( pPriv );
	    M_insertStart(pPriv) = cb->startPos;
	    M_insertionLength(pPriv) = _DtEditor_CountCharacters(
					cb->text->ptr, cb->text->length);
	}
    }
} /* SetUndoInsertionState */


/************************************************************************
 *
 *  _DtEditorModifyVerifyCB - The modify/verify callback
 *
 *	The modify verify callback handles incoming data and the data which
 *      will be replaced.  The replaced data is saved (for later undos).
 *
 *  Parameters: 
 *	widget      - the text widget
 *	client_data - the edit area widget
 * 	call_data   - callback structure
 *
 ************************************************************************/

/* ARGSUSED */
void 
_DtEditorModifyVerifyCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
  register XmTextVerifyCallbackStruct * cb = 
				(XmTextVerifyCallbackStruct *) call_data;
  DtEditorWidget editor = (DtEditorWidget) client_data;

  /*
   * Loading all new data so no need to set up the data for later undos
   */
  if (M_loadingAllNewData(editor) == True)
  {
    _DtEditorResetUndo( editor );
    M_unreadChanges(editor) = False;
    M_loadingAllNewData(editor) = False;
  }
  else
  { 

    /* 
     * Adding additional data, rather than replacing all of the contents.
     *
     * Mark that the contents have been modified since the last time the
     * application requested a copy.
     */
    M_unreadChanges(editor) = True;

    /*
     * First, account for any data which will be removed by the new data.
     * If text is being deleted, then grab a copy for later undo's.
     */
    if(cb->endPos > cb->startPos)
      SetUndoDeletionState(editor, cb);

    /*
     * If text is being inserted, then change the undo insertion state.
     */
    if(cb->text->length > 0)
	SetUndoInsertionState(editor, cb);

  } 

} /* end ModifyVerifyCB */

