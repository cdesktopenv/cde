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
 *+SNOTICE
 *
 *	$TOG: DtMailEditor.C /main/10 1998/07/24 16:05:41 mgreess $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#ifndef I_HAVE_NO_IDENT
#else
#endif

#include <Xm/Form.h>
#include <Xm/SeparatoG.h>
#include <Dt/Dnd.h>
#include "DtMailEditor.hh"
#include "XmTextEditor.h"
#include "AttachArea.h"
#include "Attachment.h"
#ifdef DTEDITOR
#include "DtEditor.hh"
#endif

#include "EUSDebug.hh"


extern "C" {
extern XtPointer _XmStringUngenerate (
				XmString string,
				XmStringTag tag,
				XmTextType tag_type,
				XmTextType output_type);
}

#ifndef ABS
#define ABS(x) (((x) > 0) ? (x) : (-(x)))
#endif

#ifndef DRAG_THRESHOLD
#define DRAG_THRESHOLD 4
#endif

DtMailEditor::DtMailEditor(
    Widget parent,
    AbstractEditorParent *owner
) : UIComponent("DtMailEditor")
{
    _myOwner      = owner;
    
    // Create a manager widget (say a form) and set it to the private
    // variable _container. Parent private instances to this widget.
    // Expose only _container externally (for attachment stuff...)

    _w = XmCreateForm(parent, "DtMailEditor", NULL, 0);
    installDestroyHandler();

#ifdef DTEDITOR
    if ( use_XmTextEditor ) {
	_myTextEditor = new XmTextEditor(_w, this);
    } else {
	_myTextEditor = new CDEM_DtWidgetEditor(_w, this);
    }
#else
    _myTextEditor = new XmTextEditor(_w, this);
#endif

    _myAttachArea = new AttachArea(_w, this, "AttachPane");

    _showAttachArea = TRUE;
    _doingDrag = FALSE;
    _separator = NULL;
    _msgHandle = NULL;
    _dragX = -1;
    _dragY = -1;
    _editable = FALSE;
    _container = NULL;

}

DtMailEditor::~DtMailEditor()
{
    unmanageAttachArea();
    delete _myAttachArea;
    delete _myTextEditor;
}

void
DtMailEditor::initialize()
{

    Widget editor_widget;

    _myTextEditor->initialize();

    _separator = XtVaCreateManagedWidget("Sep1",
					 xmSeparatorGadgetClass,
					 _w,
					 XmNtopOffset, 1,
					 XmNbottomOffset, 1,
					 XmNrightAttachment, XmATTACH_FORM,
					 XmNleftAttachment, XmATTACH_FORM,
					 NULL);

    // Create an *UNMANAGED* attachArea.
    // If the message has an attachment, the DtMailEditor instance
    // will receive a manageAttachArea where it will adjust the
    // attachments and manage the attachArea accordingly 
    
    _myAttachArea->initialize();

    attachDropRegister();
    if (!_editable)
	attachDropDisable();

    editor_widget = _myTextEditor->get_editor();

    XtVaSetValues(editor_widget,
		  XmNtopAttachment,XmATTACH_FORM, 
		  XmNbottomAttachment, XmATTACH_WIDGET,
		  XmNbottomWidget, _separator,
		  XmNrightAttachment,XmATTACH_FORM,
		  XmNrightOffset, 4,
		  XmNleftAttachment,XmATTACH_FORM, 
		  XmNleftOffset, 3,
		  NULL );

    XtVaSetValues(_myAttachArea->baseWidget(),
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNrightOffset, 3,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNleftOffset, 5,
		  XmNbottomAttachment, XmATTACH_FORM,
		  NULL);
    
    XtVaSetValues(_separator,
		  XmNbottomAttachment, XmATTACH_WIDGET,
		  XmNbottomWidget, _myAttachArea->baseWidget(),
		  NULL);

    // Unmanage the attachArea.  If a message has attachments,
    // manageAttachArea() will get called by the consumer of this
    // class.

    this->unmanageAttachArea(); 

    XtManageChild(_w);

}

AbstractEditorParent *
DtMailEditor::owner()
{
    return (_myOwner);
}

Editor*
DtMailEditor::textEditor()
{
    return(_myTextEditor);
}

AttachArea*
DtMailEditor::attachArea()
{
    return(_myAttachArea);
}

Widget
DtMailEditor::container()
{
    return(_w);
}

void
DtMailEditor::setEditable(Boolean bval)
{
    textEditor()->set_editable(bval);
    _editable = bval;
    if (_editable)
	attachDropEnable();
    else
	attachDropDisable();
}

void
DtMailEditor::manageAttachArea()
{

    if (!_showAttachArea && (_myAttachArea->getIconCount() == 0)) {
	return;
    }

    _myAttachArea->manage();
    XtManageChild(_separator);

    Widget editor_widget = _myTextEditor->get_editor();

    XtVaSetValues(editor_widget,
	XmNbottomAttachment, XmATTACH_WIDGET,
	XmNbottomWidget, _separator,
  	NULL );

    XtVaSetValues(_separator,
		  XmNbottomAttachment, XmATTACH_WIDGET,
		  XmNbottomWidget, _myAttachArea->baseWidget(),
		  NULL);

    XtVaSetValues(_myAttachArea->baseWidget(),
		XmNbottomAttachment, XmATTACH_FORM,
		NULL);

}

void
DtMailEditor::unmanageAttachArea()
{

    // Already unmanaged?
    if (!XtIsManaged(_myAttachArea->baseWidget())) return;

    Widget editor_widget = _myTextEditor->get_editor();

    XtVaSetValues(editor_widget,
	XmNbottomAttachment, XmATTACH_FORM,
  	NULL );

    _myAttachArea->unmanage();

    XtUnmanageChild(_separator);

}

// Initialize _msgHandle
void
DtMailEditor::setMsgHnd(DtMail::Message * msgHandle)
{
    _msgHandle = msgHandle;
}

// attachTransferCallback
//
// Handles the transfer of data that is dropped on the attachment list.
// The data is turned into an attachment and appended to the list.
//
void
DtMailEditor::attachTransferCallback(
    Widget	/* widget */,
    XtPointer	client_data,
    XtPointer	call_data)
{
    DtDndTransferCallbackStruct *transferInfo =
				(DtDndTransferCallbackStruct *) call_data;
    DtDndContext *dropData = transferInfo->dropData;
    int		 numItems = dropData->numItems, ii;
    DtMailEditor *editor = (DtMailEditor *) client_data;
    DtMailEnv	 mail_error;
    DtMailBuffer buf;
    char 	 *attachname;

    DebugPrintf(3, "In DtMailEditor::attachTransferCallback\n");

    // Initialize mail_error.
    mail_error.clear();

    switch (transferInfo->dropData->protocol) {

	case DtDND_FILENAME_TRANSFER:

	    // Loop through the dropped files and turn each
	    // into an attachment.

	    for (ii = 0; ii < numItems; ii++) {
		editor->owner()->add_att(dropData->data.files[ii]);
	    }
	    break;

	case DtDND_BUFFER_TRANSFER:

	    // Loop through the dropped buffers and turn each
	    // into an attachment.

	    for (ii = 0; ii < numItems; ii++) {

		buf.buffer = (char *)dropData->data.buffers[ii].bp;
		buf.size = (unsigned long)dropData->data.buffers[ii].size;
		attachname = dropData->data.buffers[ii].name;

		if (!attachname)
		    attachname = "Untitled";

		editor->owner()->add_att(attachname, buf);
	    }
	    break;

	default:
	    transferInfo->status = DtDND_FAILURE;
	    return;
    }
}


// attachDropRegister
//
// Register the attachment list to accept drops of buffer and files
//
void
DtMailEditor::attachDropRegister()
{
    static XtCallbackRec transferCBRec[] = { 
	{&DtMailEditor::attachTransferCallback, NULL}, {NULL, NULL} };

    // Pass the DtMailEditor object (this) as clientData.
    transferCBRec[0].closure = (XtPointer) this;

    DtDndVaDropRegister(_myAttachArea->baseWidget(),
	DtDND_FILENAME_TRANSFER | DtDND_BUFFER_TRANSFER,
	(unsigned char)(XmDROP_COPY), transferCBRec,
	DtNtextIsBuffer, TRUE,
	NULL);
}

// attachDropEnable
//
// Enable the attachment list for drops by restoring the operation
//
void
DtMailEditor::attachDropEnable()
{
    Arg	args[1];

    XtSetArg(args[0], XmNdropSiteOperations, XmDROP_MOVE | XmDROP_COPY);
    XmDropSiteUpdate(_myAttachArea->baseWidget(), args, 1);
}

// attachDropDisable
//
// Disable the attachment list for drops by setting the operation to noop
//
void
DtMailEditor::attachDropDisable()
{
    Arg	args[1];

    XtSetArg(args[0], XmNdropSiteOperations, XmDROP_NOOP);
    XmDropSiteUpdate(_myAttachArea->baseWidget(), args, 1);
}

// attachConvertCallback
//
// Provides the selected attachments for the drag
//
void
DtMailEditor::attachConvertCallback(
    Widget	/* dragContext */,
    XtPointer	clientData,
    XtPointer	callData)
{
    DtDndConvertCallbackStruct *convertInfo =
				(DtDndConvertCallbackStruct *) callData;
    DtDndBuffer		*buffers = convertInfo->dragData->data.buffers;
    DtMailEditor	*editor = (DtMailEditor *) clientData;
    int			numIcons = editor->attachArea()->getIconCount();
    Attachment		**list = editor->attachArea()->getList();
    int 		ii, current = 0;
    char		*name = NULL;
    XmString		str;
    DtMailEnv	    	mail_error;


    DebugPrintf(3, "In DtMailEditor::attachConvertCallback\n");

    switch(convertInfo->reason) {
	case DtCR_DND_CONVERT_DATA:
	    for (ii = 0; ii < numIcons; ii++) {
		if (!list[ii]->isDeleted() && list[ii]->isSelected()) {
		    buffers[current].bp = list[ii]->getContents();
		    buffers[current].size = (int)list[ii]->getContentsSize();
		    str = list[ii]->getLabel();
        	    buffers[current].name =
			(char *) _XmStringUngenerate(
					str, NULL,
					XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
		    XmStringFree(str);
		    current++;
		}
	    }
	    break;

	case DtCR_DND_CONVERT_DELETE:
	    editor->attachArea()->deleteSelectedAttachments(mail_error);
	    break;

	default:
	    convertInfo->status = DtDND_FAILURE;
    }
}

// attachDragFinishCallback
//
// Clean up from the convert callback and restore state
//
void
DtMailEditor::attachDragFinishCallback(
    Widget	/* widget */,
    XtPointer	clientData,
    XtPointer	callData)
{
    DtDndDragFinishCallbackStruct *finishInfo =
		(DtDndDragFinishCallbackStruct *) callData;
    DtDndContext 	*dragData = finishInfo->dragData;
    DtMailEditor *editor = (DtMailEditor *) clientData;
    int		 ii;

    DebugPrintf(3, "In DtMailEditor::attachDragFinishCallback\n");

    editor->setDoingDrag(FALSE);
    editor->setDragX(-1);
    editor->setDragY(-1);

    if (editor->editable())
	editor->attachDropEnable();

    for (ii = 0; ii < dragData->numItems; ii++) {
	XtFree((char *)dragData->data.buffers[ii].name);
    }
}

void
DtMailEditor::attachDragStart( Widget widget, 
				XEvent *event)
{
    static XtCallbackRec convertCBRec[] = {
	{&DtMailEditor::attachConvertCallback, NULL}, {NULL, NULL} };
    static XtCallbackRec dragFinishCBRec[] = {
	{&DtMailEditor::attachDragFinishCallback, NULL}, {NULL, NULL} };
    int 	itemCount;
    unsigned char operations;

    convertCBRec[0].closure = (XtPointer) this;
    dragFinishCBRec[0].closure = (XtPointer) this;

    attachDropDisable();

    // Count the number of items to be dragged.
    itemCount = attachArea()->getSelectedIconCount();

    setDoingDrag(TRUE);

    if (editable()) {
	operations = (unsigned char)(XmDROP_COPY | XmDROP_MOVE);
    } else {
	operations = (unsigned char)(XmDROP_COPY);
    }

    if (DtDndVaDragStart(widget, event, DtDND_BUFFER_TRANSFER, itemCount,
		operations, convertCBRec, dragFinishCBRec,
//		DtNsourceIcon, dragIcon,
		NULL)
	    == NULL) {
	    
	    DebugPrintf(3, "DragStart returned NULL.\n");
    }
}

void
DtMailEditor::attachDragMotionHandler(
    Widget	widget,
    XEvent	*event)
{
    int		diffX, diffY;

    if (!doingDrag()) {
	// If the drag is just starting, set initial button down coordinates.
	if (dragX() == -1 && dragY() == -1) {
	    setDragX(event->xmotion.x);
	    setDragY(event->xmotion.y);
	}

	// Find out how far the pointer has moved since the button press.
	diffX = dragX() - event->xmotion.x;
	diffY = dragY() - event->xmotion.y;

	if ((ABS(diffX) >= DRAG_THRESHOLD) ||
	    (ABS(diffY) >= DRAG_THRESHOLD)) {
		attachDragStart(widget, event);
	}
    }
}

void
DtMailEditor::stripCRLF(char **buffer, const char * buf, const unsigned long len)
{
    char * out = *buffer;
    int          _len;


    for (const char * in = buf; in < (buf + len);) {
	_len = mblen( in, MB_CUR_MAX );
	if ( _len <= 0 )
	    break;
	if ( ( _len == 1 ) && ( *in == '\r' ) ){
	    in += 1;
	    continue;
	}
	strncpy( out, in, _len );
	out += _len, in += _len;
    }
    *out = 0;
}

void
DtMailEditor::needBuf(char **buffer, unsigned long *buflen, unsigned long newlen)
{
    if (newlen > *buflen) {
	// Need a bigger buffer.
	if (*buffer) {
	    delete [] *buffer;
	}

	*buffer = new char[newlen];
	*buflen = newlen;
    } else {
	// Clear buffer content -- get ready for new data
	if (*buffer) {
	    memset(*buffer, 0, (unsigned int)*buflen);	
	}
	}
}

void
DtMailEditor::showAttachArea()
{
    _showAttachArea = TRUE;
    this->manageAttachArea();

}

void
DtMailEditor::hideAttachArea()
{
    _showAttachArea = FALSE;
    this->unmanageAttachArea();
}
