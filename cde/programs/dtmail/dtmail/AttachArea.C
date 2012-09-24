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
 *+SNOTICE
 *
 *	$TOG: AttachArea.C /main/18 1999/03/25 14:16:24 mgreess $
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
#endif

#include <EUSCompat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#if defined(USL) || defined(__uxp__)
#define S_ISSOCK(mode) ((mode & S_IFMT) == S_IFSOCK)
#endif
#include <stdio.h>
#include <Dt/Editor.h>
#include <Xm/ColorObjP.h>

#include "EUSDebug.hh"

#if defined(NEED_MMAP_WRAPPER)
extern "C" {
#endif

#include <sys/mman.h>

#if defined(NEED_MMAP_WRAPPER)
}
#endif


extern "C" {
extern XtPointer _XmStringUngenerate (
				XmString string,
				XmStringTag tag,
				XmTextType tag_type,
				XmTextType output_type);
}


#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>


#include <Xm/BulletinB.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrolledW.h>
#include <Xm/DrawingA.h>
#include <Xm/ScrollBar.h>
#include <Xm/PushBG.h>
#include <Xm/FileSB.h>
#include <Xm/SelectioB.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/AtomMgr.h>
#include <Xm/Xm.h>
#include <Xm/Screen.h>
#include <Xm/ToggleB.h>
#include <X11/IntrinsicP.h>
#include <X11/Xatom.h>

#include "Attachment.h"
#include "AttachArea.h"
#include "Icon.h"
#include "MenuBar.h"
#include "RoamApp.h"
#include "RoamMenuWindow.h"

#include "MsgScrollingList.hh"
#include "ViewMsgDialog.h"
#include "SendMsgDialog.h"
#include "MailMsg.h"		// DT_catd defined here
#include <DtMail/DtMail.hh>    // time_t defined here
#include <DtMail/IO.hh>                    // SafeAccess...

#include "Help.hh"
#include "DtMailHelp.hh"

extern nl_catd	DtMailMsgCat;

#define equal(a, b) (!strcmp(a,b))
#define HSPACE 10
#define VSPACE 10
#define MAXATOM 2048	// ?????

// This is the new one
AttachArea::AttachArea ( 
	Widget parent, 
	DtMailEditor *owner,
	char *name
) : UIComponent (name)
{
    // Unique stuff

    _myOwner = owner;
    _parent  = parent;

    _attachmentList=NULL;

    _w = NULL;
    _iconCount = 0;
    _iconSelectedCount = 0;
    _deleteCount = 0;

    _fsDialog = NULL;
    _fsState = NOTSET;
    _lastRow = 0;
    _currentRow = 0;
    _attachmentsSize = 0;
    _selectedAttachmentsSize = 0;
    _clientData = NULL;
    _renameDialog = NULL;

    _myRMW = NULL;
    _myVMD = NULL;
    _mySMD = NULL;
    
    _pendingAction = FALSE;
    _numPendingActions = 0;

    _attach_area_selection_state = AA_SEL_NONE;
    _cache_single_attachment = NULL;

}

void
AttachArea::initialize()
{

    // We're making the assumption here that this widget's parent`
    // is also a form

    XtWidgetGeometry size;
    Dimension parWid, parHeight;
    Dimension txt_w, txt_h;
    XmFontList fl;
    XmString xms;

    int         colorUse;
    short       act, inact, prim, second, text;
    XmPixelSet  pixels[XmCO_NUM_COLORS];


    _w = XtVaCreateManagedWidget (
			"AttachPane",
			xmFormWidgetClass, _parent, 
			NULL);

    // Get pixel data.
    XmeGetColorObjData(XtScreen(_parent), &colorUse, pixels, XmCO_NUM_COLORS,
		       &act, &inact, &prim, &second, &text);
    _foreground = pixels[text].fg;
    _background = pixels[text].sc;

    parWid = _myOwner->textEditor()->get_text_width();

    fl = _myOwner->textEditor()->get_text_fontList();
    xms = XmStringCreateLocalized("Xyb");
    XmStringExtent(fl, xms, &txt_w, &txt_h);
    parHeight = txt_h + Icon::maxIconHeight() + (2*VSPACE);

    _appBackground = _background;
    _appForeground = _foreground;

    _sw = XtVaCreateManagedWidget ( 
				    "AttachPane_ScrolledWindow", 
				    xmScrolledWindowWidgetClass, _w, 
				    XmNscrollingPolicy,  XmAPPLICATION_DEFINED,
				    XmNrightAttachment,  XmATTACH_FORM,
				    XmNleftAttachment,   XmATTACH_FORM,
				    XmNtopAttachment,    XmATTACH_FORM,
		    		    XmNshadowThickness, (Dimension)1,
				    XmNspacing, 2,
				    XmNwidth,	     parWid,
				    XmNheight,	     parHeight,
				    NULL);

    rowOfAttachmentsStatus = XtCreateManagedWidget("Attachments_Status",
				xmFormWidgetClass,
				_w, NULL, 0);

     XtVaSetValues(rowOfAttachmentsStatus,
  	XmNrightAttachment,	XmATTACH_FORM, 
  	XmNleftAttachment,	XmATTACH_FORM, 
 	XmNtopAttachment,       XmATTACH_WIDGET,
 	XmNtopWidget,	        _sw,
 	XmNtopOffset,		5,
	XmNbottomOffset,	5,
 	NULL );

    this->addToRowOfAttachmentsStatus();

    size.request_mode = CWHeight;
    XtQueryGeometry(rowOfAttachmentsStatus, NULL, &size);

    XtVaSetValues(
	rowOfAttachmentsStatus,
	XmNpaneMaximum, size.height,
	XmNpaneMinimum, size.height,
	NULL
    );

    _vsb = XtVaCreateManagedWidget("vsb", xmScrollBarWidgetClass, _sw,
		    XmNorientation, XmVERTICAL,
		    XmNsliderSize, 1,
		    XmNmaximum, 1,
		    XmNpageIncrement, 1,
		    NULL);

    XtAddCallback(
	_vsb, 
	XmNvalueChangedCallback,&AttachArea::valueChangedCallback,
	(XtPointer) this 
    );

    XtAddCallback(
	_vsb, 
	XmNdragCallback, &AttachArea::dragCallback,
	(XtPointer) this 
    );

    _clipWindow = XtVaCreateManagedWidget("AttachArea_clipWindow", 
		    xmDrawingAreaWidgetClass, _sw,
		    XmNresizePolicy, XmRESIZE_NONE,
		    XmNbackground, _background,
		    XmNwidth,	     parWid,
		    XmNheight,	     parHeight,
		    NULL);
    XmScrolledWindowSetAreas(_sw, NULL, _vsb, _clipWindow);

    XtManageChild(_clipWindow);
    XtManageChild(_vsb);
    XtManageChild(_sw);

    
    // Set RowCol to NULL here.
    // It gets set in the expose_all_attachments.

    _rc = NULL;

    CalcSizeOfAttachPane();

    installDestroyHandler();
}

AttachArea::~AttachArea()
{
}

void AttachArea::addToList( Attachment *attachment )
{
    Attachment **newList;
    int i;

    newList = new Attachment*[ _iconCount + 1 ];
    for(i=0; i < _iconCount; i++)
	newList[i] = _attachmentList[i];
 
    if (_attachmentList)
	delete []_attachmentList;

    _attachmentList = newList;

    _attachmentList[ _iconCount ] = attachment;

    _iconCount++;
    
//    setAttachmentsLabel();
}

#ifdef DEAD_WOOD
void AttachArea::setAttachmentsLabel( )
{
    char *c = new char[256];
    XmString xmstr;
    String str;
    unsigned int last_displayCount, last_selectedCount;
    unsigned int displayCount = _iconCount - _deleteCount;
    unsigned int attachmentsSize;

    if((displayCount) == 0) {
	XtUnmanageChild(_no_attachments_label);
	XtUnmanageChild(_attachments_label);
	XtUnmanageChild(_size_attachments_label);
	XtUnmanageChild(_no_selected_label);
	XtUnmanageChild(_selected_label);
	XtUnmanageChild(_size_selected_label);
    } else {
	CalcAttachmentsSize();
	attachmentsSize = getAttachmentsSize();

	// Number of Attachments
	XtVaGetValues(_no_attachments_label,
		XmNlabelString, &xmstr,
		NULL);
	str = NULL;
        str = (char *) _XmStringUngenerate(
					xmstr, NULL,
					XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
	if (NULL == str) return; // internal error
	last_displayCount = (unsigned int)strtol(str, NULL, 10);
	XtFree(str);

	// Number of Attachments Selected
	XtVaGetValues(_no_selected_label,
		XmNlabelString, &xmstr,
		NULL);
	str = NULL;
        str = (char *) _XmStringUngenerate(
					xmstr, NULL,
					XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
	if (NULL == str) return; // internal error
	last_selectedCount = (unsigned int)strtol(str, NULL, 10);
	XtFree(str);

	if((last_displayCount == 0 && displayCount == 1) ||
	   (last_displayCount == 2 && displayCount == 1)) {
	    sprintf(c, GETMSG(DT_catd, 12, 1, "Attachment"));
	    XtVaSetValues(_attachments_label,
		XtVaTypedArg, XmNlabelString, XtRString, c, strlen(c)+1,
		NULL);
	} else if(last_displayCount == 1 && displayCount == 2) {
	    sprintf(c, GETMSG(DT_catd, 12, 2, "Attachments"));
	    XtVaSetValues(_attachments_label,
		XtVaTypedArg, XmNlabelString, XtRString, c, strlen(c)+1,
		NULL);
	}
	if(last_displayCount != displayCount) {
	    sprintf(c, GETMSG(DT_catd, 12, 3, "displayCount"));
	    XtVaSetValues(_no_attachments_label,
		XtVaTypedArg, XmNlabelString, XtRString, c, strlen(c)+1,
		NULL);
	}
	sprintf(c, "(%s),", calcKbytes(attachmentsSize));
	XtVaSetValues(_size_attachments_label,
	    XtVaTypedArg, XmNlabelString, XtRString, c, strlen(c)+1,
	    NULL);

	if(last_selectedCount != _iconSelectedCount) {
	    sprintf(c, "%d", _iconSelectedCount); 
	    XtVaSetValues(_no_selected_label,
		XtVaTypedArg, XmNlabelString, XtRString, c, strlen(c)+1,
		NULL);
	    sprintf(c, "(%s)", calcKbytes(getSelectedAttachmentsSize())); 
	    XtVaSetValues(_size_selected_label,
		XtVaTypedArg, XmNlabelString, XtRString, c, strlen(c)+1,
		NULL);
	}
	if(!XtIsManaged(_no_attachments_label)) {
	    XtManageChild(_no_attachments_label);
	    XtManageChild(_attachments_label);
	    XtManageChild(_size_attachments_label);
	    XtManageChild(_no_selected_label);
	    XtManageChild(_selected_label);
	    XtManageChild(_size_selected_label);
	}
    }
    delete [] c;
}
#endif /* DEAD_WOOD */

int AttachArea::getSelectedIconCount()
{
    Attachment **list = getList();
    int num_selected = 0;

    for (int i = 0; i < getIconCount(); i++) {
	if (!list[i]->isDeleted() && list[i]->isSelected())
	    num_selected++;
    }
    return (num_selected);
}

#ifdef DEAD_WOOD
void AttachArea::CalcAttachmentsSize( )
{
     Attachment **list = getList();
     unsigned int total = 0;
     int i;

     int num_icons = getIconCount();

 
     for(i=0;i<num_icons;i++)
 	if(!list[i]->isDeleted())
 	    total += (unsigned int)list[i]->getContentsSize();
     
     setAttachmentsSize(total);
}
#endif /* DEAD_WOOD */

Attachment *
AttachArea::getSelectedAttachment()
{
    return _cache_single_attachment;
}


void
AttachArea::MenuButtonHandler(
    Widget ,
    XtPointer cd,
    XEvent *event,
    Boolean *)
{
	AttachArea *obj = (AttachArea *)cd;

	if(event->xany.type != ButtonPress)
		return;

	XButtonEvent *be = (XButtonEvent *)event;

	if(be->button == theApplication->bMenuButton())
		obj->_myOwner->owner()->postAttachmentPopup(event);
}

void 
AttachArea::inputCallback(Widget, XtPointer client_data, XtPointer call_data)
{
    AttachArea *obj = (AttachArea *) client_data;
    XmDrawingAreaCallbackStruct *cb = (XmDrawingAreaCallbackStruct *)call_data;

    if(cb->reason != XmCR_INPUT 	||
	cb->event->xany.type != ButtonPress)
	return;

    if(((XButtonEvent *)cb->event)->button == Button1)
    	obj->unselectOtherSelectedAttachments(NULL);
}

void AttachArea::resizeCallback ( 
				  Widget w, 
				  XtPointer clientData, 
				  XtPointer //callData
				  )
{
    XtArgVal wid;

    AttachArea *obj = (AttachArea *) clientData;
    XtVaGetValues(
	w,
	XmNwidth, &wid,
	NULL
    );

    obj->resize((Dimension)wid);
}

void AttachArea::resize(
    Dimension wid
)
{
    int i;
    Attachment **list = getList();

    _attachAreaWidth = wid;
    XtVaSetValues(_clipWindow, XmNwidth, _attachAreaWidth, NULL);

    for (i=0; i<getIconCount(); i++)
      list[i]->unmanageIconWidget();

    CalcAllAttachmentPositions();
    CalcLastRow();
    AdjustCurrentRow();
    SetScrollBarSize(getLastRow()+1);
    DisplayAttachmentsInRow(_currentRow);
}

void AttachArea::CalcSizeOfAttachPane( )
{
    _attachAreaWidth = _myOwner->textEditor()->get_text_width();
    XtVaSetValues(_clipWindow, XmNwidth, _attachAreaWidth, NULL);
}

void AttachArea::activateDeactivate()
{
    //
    // If exactly one icon is selected then activate the open command 
    //

    if(getIconSelectedCount() == 1) {
	openCmd()->activate();
    } else {
	openCmd()->deactivate();
    }

    // If no icons are selected then deactivate the OK button on the FS Dialog

    if(getIconSelectedCount() > 0) {
	if(getFsDialog())
	    XtSetSensitive(
		XmSelectionBoxGetChild( 
		    getFsDialog(), XmDIALOG_OK_BUTTON), TRUE
		);
    } else {
	if(getFsDialog())
	    XtSetSensitive(
		XmSelectionBoxGetChild( 
		    getFsDialog(), XmDIALOG_OK_BUTTON
		), 
		(getFsState() == SAVEAS) ? FALSE : TRUE
	    );
    }
}

// Used by Compose window (SMD)
// Given a filename, add it to message and to attachArea.

Attachment*
AttachArea::addAttachment(
			  DtMail::Message* msg,
			  DtMail::BodyPart *lastAttBP,
			  char *filename,
			  char *name
			  )
{
    int fd;
    struct stat s;
    Boolean validtype = TRUE;
    DtMail::BodyPart * bp = NULL;
    DtMailEnv mail_error;
    int answer;
    char *helpId = NULL;

    mail_error.clear();

    char *errormsg = new char[512];
    char *buf = new char[2048];
    char *buffer = NULL, *lbl;
    char *fname_start;

    for (fname_start = filename + strlen(filename) - 1;
	 fname_start >= filename && *fname_start != '/'; fname_start--) {
	continue;
    }
    if (*fname_start == '/') {
	fname_start += 1;
    }

    bp = msg->newBodyPart(mail_error, lastAttBP);	

    if (SafeAccess(filename, F_OK) != 0) {
	sprintf(buf, GETMSG(DT_catd, 3, 34, "%s does not exist."),
		filename);
	answer = this->handleErrorDialog(GETMSG(DT_catd, 1, 81, "Mailer"), 
					 buf);
	delete [] buf;
	delete [] errormsg;
	return(NULL);
    }

    SafeStat(filename, &s);

    if(S_ISFIFO(s.st_mode)) {
	sprintf(errormsg,
		GETMSG(DT_catd, 12, 4, "Cannot attach FIFO files: %s"), filename);
	validtype = FALSE;
    } else if(S_ISCHR(s.st_mode)) {
	sprintf(
	    errormsg,
	    GETMSG(DT_catd, 12, 5, "Cannot attach character special files: %s"), filename
	);
	validtype = FALSE;
    } else if(S_ISDIR(s.st_mode)) {
	sprintf(
	    errormsg,
	    GETMSG(DT_catd, 12, 6, "Cannot attach directories: %s"), filename
	);
	validtype = FALSE;
    } else if(S_ISBLK(s.st_mode)) {
	sprintf(errormsg,
		GETMSG(DT_catd, 12, 7, "Cannot attach block special files: %s"), filename
	);
	validtype = FALSE;
    } else if(S_ISSOCK(s.st_mode)) {
	sprintf(errormsg,
		GETMSG(DT_catd, 12, 8, "Cannot attach socket files: %s"), filename
	);
	validtype = FALSE;
    }
    if(validtype == FALSE) {
	answer = this->handleErrorDialog(GETMSG(DT_catd, 1, 81, "Mailer"), 
					 errormsg,
                                         NULL);
	delete [] buf;
	delete [] errormsg;
	return(NULL);
    }

    fd = SafeOpen(filename, O_RDONLY);
	
    if (fd < 0) {
	sprintf(buf, GETMSG(DT_catd, 3, 35, "Unable to open %s."), filename);
        helpId = DTMAILHELPNOOPEN;
	answer = this->handleErrorDialog(GETMSG(DT_catd, 1, 82, "Mailer"), 
					 buf,
                                         helpId);
	delete [] buf;
	delete [] errormsg;
	return(NULL);
    }

    int page_size = (int)sysconf(_SC_PAGESIZE);
    size_t map_size = (size_t) (s.st_size + 
				    (page_size - (s.st_size % page_size)));
    char * map;

#if defined(__osf__)
    // This version of mmap does NOT allow requested length to be
    // greater than the file size ...  in contradiction to the
    // documentation (don't round up).
    map = (char *) mmap(0, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
#else
    map = (char *) mmap(0, map_size, PROT_READ, MAP_PRIVATE, fd, 0);
#endif

    if (map == (char *)-1) {
	// We could not map it for some reason. Let's just read it into
	// buffer and pass it to XmText.
	//

	buffer = new char[s.st_size + 1];

	if (!buffer) {
	    sprintf(buf, 
		    GETMSG(DT_catd, 3, 36, "Unable to allocate memory."));
            helpId = DTMAILHELPNOALLOCMEM;
	    answer = this->handleErrorDialog(GETMSG(DT_catd, 1, 83, "Mailer"), 
					     buf,
                                             helpId);
	    return(NULL);
	}

	if (read(fd, buffer, (unsigned int) s.st_size) < 0) {
	    SafeClose(fd);
	    return(NULL);
	}
	buffer[s.st_size] = 0;
	bp->setContents(
		mail_error, buffer, s.st_size, NULL, fname_start, 0, NULL
		);
    }
    else {
	// We now have a mapped file. XmText wants a zero terminated
	// buffer. We get luck with mmap because unless the file is
	// an even page size, we will have some zero fill bytes that
	// are legal to access.
	//
	// Of course in the case of an even page size file we must
	// copy the buffer, terminate it and then give it to XmText.
	//
	bp->setContents(
	    mail_error, map, s.st_size, NULL, fname_start, 0, NULL
	);
	munmap(map, map_size);
    }
    SafeClose(fd);


    // _iconCount + 1 because iconCount starts at 0 and we want 
    // attachmentCount to begin at 1.  attachmentCount is set to be
    // in the widget's userData.  


    if(name)
	lbl = strdup(name);
    else {
	if(strchr(filename, '/') == NULL) // The name does not include a slash
	    lbl = strdup(filename);
	else			   // The name does include a slash
	    lbl = strdup(strrchr(filename, '/')+1);
    }    
    Attachment *attachment = new Attachment(this, lbl, bp, _iconCount + 1);
    attachment->setAttachArea(this);
    attachment->initialize();
    addToList( attachment );

    // Update the display.  The Compose Window needs immediate update.

    this->manageList();

    delete [] buf;
    delete [] errormsg;
    return(attachment);
}

Attachment*
AttachArea::addAttachment(
    DtMail::Message *msg,
    DtMail::BodyPart *lastAttBP,
    String name,
    DtMailBuffer buf
)
{
    DtMailEnv mail_error;
    DtMail::BodyPart * bp = NULL;

    if (!name)
	name = "noname";

    mail_error.clear();

    bp = msg->newBodyPart(mail_error, lastAttBP);
    bp->setContents(mail_error, buf.buffer, buf.size, NULL, name, 0, NULL);

    Attachment *attachment = new Attachment(this,
					    name,
					    bp,
					    _iconCount + 1);
    attachment->setAttachArea(this);
    attachment->initialize();
    addToList(attachment);

    // Update the display.  The Compose Window needs immediate update.

    this->manageList();

    return(attachment);
}

Attachment* 
AttachArea::addAttachment( 
    String name,
    DtMail::BodyPart *body_part
)
{
    // _iconCount + 1 because iconCount starts at 0 and we want 
    //  attachmentCount to begin at 1.  attachmentCount is set to be
    // in the widget's userData.  

    Attachment *attachment = new Attachment(
					this, 
					name, 
					body_part, 
					_iconCount + 1
				);
    attachment->setAttachArea(this);
    attachment->initialize();
    addToList( attachment );

    return(attachment);
}

#ifdef DEAD_WOOD
void
AttachArea::add_attachment(
    Attachment *attachment
)
{
    
     attachment->setAttachArea(this);
     attachment->initialize();
     addToList( attachment );
}

//
// This function truly deletes all the attachments in the AttachArea
// The widgets are unmanaged and the attachment classes are deleted.
//

void AttachArea::deleteAttachments( )
{
    int i;
    WidgetList deleteList;
    int count;

    Attachment **list = getList();
 
     // First, unmanaged all the attachment at once so there is no
     // flickering when we delete them

     deleteList = (WidgetList)XtMalloc(sizeof(Widget) * getIconCount());

     for(i=0;i<getIconCount();i++)
 	deleteList[i] = list[i]->baseWidget();
     XtUnmanageChildren(deleteList, i);

     delete deleteList;
     XtFree((char *)deleteList);
 
    // Delete each attachment in the list
    count = getIconCount();
    for(i=count-1;i>=0;i--) {
 	delete list[i];
 	decIconCount();
    }

     _iconCount = 0;
     _iconSelectedCount = 0;
     _deleteCount = 0;
     CalcLastRow();
     AdjustCurrentRow();
     SetScrollBarSize(getLastRow()+1);
     activateDeactivate();
     _attachmentList=NULL;
     _attachmentsSize = 0;
     _selectedAttachmentsSize = 0;
}
#endif /* DEAD_WOOD */

void AttachArea::manageList( )
{
    int i;
    Attachment **list = getList();

    for (i=0; i<getIconCount(); i++)
      list[i]->unmanageIconWidget();

    CalcAllAttachmentPositions();
    CalcLastRow();
    AdjustCurrentRow();
    SetScrollBarSize(getLastRow()+1);
    DisplayAttachmentsInRow(_currentRow);
}

#ifdef DEAD_WOOD
//
// Find the x and y position for a newly created attachment
//

void AttachArea::CalcAttachmentPosition(Attachment *item)
{
    int i, j;
    Boolean found_managed = FALSE;

     Attachment **list = getList();
     for(i=0, j=0;i<getIconCount();i++)
 	if(!list[i]->isDeleted()) {
 	    j = i;
 	    found_managed = TRUE;
 	}
    calculate_attachment_position(
       found_managed ? list[j] : (Attachment *)NULL, item
       );

}      
#endif /* DEAD_WOOD */

//
// Display the attachments in row X
//

void AttachArea::DisplayAttachmentsInRow(unsigned int X)
{
    int i;
    int managecount, unmanagecount;

    Attachment **list = getList();
    WidgetList manageIconList, unmanageIconList;

    if (getIconCount())
    {
        manageIconList = (WidgetList) XtMalloc(sizeof(Widget)*getIconCount());
        unmanageIconList = (WidgetList) XtMalloc(sizeof(Widget)*getIconCount());
     
        managecount = unmanagecount = 0;
        for(i=0;i<getIconCount();i++) {
 	    if(!list[i]->isDeleted()) {
 	        if(list[i]->getRow() == X) {
		    if(!list[i]->isManaged()) {
		        manageIconList[managecount] = list[i]->baseWidget();
		        managecount++;
		    } 
	        }
	        else { // if deleted
		    if(list[i]->isManaged()) {
		        unmanageIconList[unmanagecount] = list[i]->baseWidget();
		        unmanagecount++;
		    }
	        }
	    }
        }

        XtUnmanageChildren(unmanageIconList, unmanagecount);
        XtManageChildren(manageIconList, managecount);


        XtFree((char *)manageIconList);
        XtFree((char *)unmanageIconList);
    }

    this->attachment_summary(_iconCount - _deleteCount, _deleteCount);
}

//
// Calculate the position of every non-deleted Attachment
//

void AttachArea::CalcAllAttachmentPositions()
{
    int i, j;
    Attachment **list = getList();
 
    j = -1;
    for(i=0;i<getIconCount();i++) {
	if(!list[i]->isDeleted()) {
	    calculate_attachment_position(
			(j == -1) ? (Attachment *)NULL : list[j],
			list[i]);
	    j = i;
	}
    }
}

//
// Determine the position of attachment "item" given reference
// attachment "ref"
//

void AttachArea::calculate_attachment_position(
    Attachment *ref,
    Attachment *item
)
{
    if(ref == NULL) {
	item->setX(HSPACE);
	item->setY(VSPACE);
	item->setRow(0);
	return;
    }
    if(((Dimension)(ref->getX() + 
		    ref->getWidth() + 
		    HSPACE + 
		    item->getWidth())) 	> getAAWidth() ) {
	item->setX(HSPACE);
	item->setY(VSPACE);
	item->setRow(ref->getRow() + 1);
    } else {
	item->setX(ref->getX() + ref->getWidth() + HSPACE);
	item->setRow(ref->getRow());
	item->setY(VSPACE);
    }
}

//
// Invoked when the user moves the slider by any method
// If the user is dragging the slider then this callback
// is only invoked when the user releases the mouse button
//

void AttachArea::valueChangedCallback ( 
    Widget, 
    XtPointer clientData, 
    XtPointer callData 
)
{
    AttachArea *obj = (AttachArea *) clientData;

    obj->valueChanged( callData );
}

void AttachArea::valueChanged( XtPointer callData )
{
    XmScrollBarCallbackStruct *cbs = (XmScrollBarCallbackStruct *)callData;

    _currentRow = cbs->value;
    DisplayAttachmentsInRow(_currentRow);
}

//
// Invoked when the user drags the slider
//

void AttachArea::dragCallback ( 
    Widget, 
    XtPointer clientData, 
    XtPointer callData 
)
{
    AttachArea *obj = (AttachArea *) clientData;

    obj->dragSlider( callData );
}

void AttachArea::dragSlider( XtPointer callData )
{
    XmScrollBarCallbackStruct *cbs = (XmScrollBarCallbackStruct *)callData;

    if(cbs->value == _currentRow)
	return;
    _currentRow = cbs->value;
    DisplayAttachmentsInRow(_currentRow);
}

//
// Calculate the number of rows
//

void AttachArea::CalcLastRow()
{
    int i;
    unsigned row = 0;
     Attachment **list = getList();
 
     for(i=0;i<getIconCount();i++) {
 	if(!list[i]->isDeleted()) {
 	    row = list[i]->getRow();
 	}
     }
    _lastRow = row;
}

//
// Set the XmmNmaximum resource to size
//

void AttachArea::SetScrollBarSize(unsigned int size)
{
    XtVaSetValues(_vsb, 
	XmNmaximum, size,
	NULL);
}

//
// If the current row is greater than the last row, adjust the
// current row to be equal to the last row.
//

void AttachArea::AdjustCurrentRow()
{
    if(_currentRow > _lastRow) {
	_currentRow = _lastRow;
	XtVaSetValues(_vsb, XmNvalue, _currentRow, NULL);
    }
}

XmString
AttachArea::getSelectedAttachName()
{
    XmString str = (XmString)NULL;

    if(_cache_single_attachment)
	str = _cache_single_attachment->getLabel();

    return(str);
}

void
AttachArea::setSelectedAttachName(
    XmString new_name
)
{
    int i;
    Boolean set = FALSE;
    Attachment **list = this->getList();

    XmString nn = XmStringCopy(new_name);

    // Set name of first selected attachment to new_name
    for(i=0;i<this->getIconCount() && !set;i++)
 	if(list[i]->isSelected()) {
	    list[i]->rename(nn);
	    set = TRUE;
 	}
    this->manageList();
}


void
AttachArea::attachmentSelected(
    Attachment *attachment
) 
{
    // First deselect other selected attachments
    this->unselectOtherSelectedAttachments(attachment);
    
    // Enable the menu item at the toplevel shell's menubar.
    if(_cache_single_attachment == NULL)
    	_myOwner->owner()->attachment_selected();

    // Cache the single selected attachment
    _cache_single_attachment = attachment;
    _attach_area_selection_state = AA_SEL_SINGLE;
}

void
AttachArea::attachmentFeedback(
    Boolean value
)
{
    _myOwner->owner()->attachmentFeedback(value);
}
    
char *
AttachArea::calcKbytes(unsigned int bytes)
{
    static char kstring[64];

    if(bytes < 103)
	sprintf(kstring, "%d bytes",bytes);
    else if(bytes < 1024)
	sprintf(kstring, " .%dk", bytes/103);
    else
	sprintf(kstring, "%dk", bytes/1024);

    return kstring;
}

void
AttachArea::addToRowOfAttachmentsStatus()
{
    XmString labelStr2;

    // Size of first label
    
    labelStr2 = XmStringCreateLocalized(
			GETMSG(DT_catd, 3, 37, "Summary of attachments"));


    _attachments_summary = XtCreateManagedWidget("Attachments_Summary", 
						 xmLabelWidgetClass,
						 rowOfAttachmentsStatus, NULL, 0);

    XtVaSetValues(_attachments_summary,
		  XmNalignment, XmALIGNMENT_END,
		  XmNlabelString, labelStr2,
		  XmNrightAttachment, XmATTACH_FORM,
		  NULL );

     XmStringFree(labelStr2);
}

void
AttachArea::parseAttachments(
    DtMailEnv &mail_error,
    DtMail::Message* msg,
    Boolean empty,
    int startBP
)
{
    DtMail::BodyPart * tmpBP;
    int index = 1;
    int num_attachments = 0;
    char * name;


    // First unmanage and empty out the current contents.

    // SMD sets this boolean to FALSE so that previous message's attachments
    // are not cleared.  E.g. Including/forwarding multiple messages each
    // with attachments.
    // RMW sets this boolean to TRUE so that all attachments are cleared in
    // the attachment pane everytime a new message is displayed.

    if ( empty ) {

	// First unmanage the clipWindow.
	// Unmanaging the attachment pane is visually ugly

	XtUnmanageChild(_clipWindow);
	this->clearAttachArea();
    }

    _deleteCount = 0;

    // Now fill list with new attachments.

    tmpBP = msg->getFirstBodyPart(mail_error);
    if (mail_error.isSet()) {
	// do something
    }

    // Sync up the index with the bodyPart from which to begin
    // adding attachments into attachPane.
    // 
    while (startBP > index) {
	tmpBP = msg->getNextBodyPart(mail_error, tmpBP);
	index++;
    }
    
    while (tmpBP != NULL) {
	num_attachments++;
	tmpBP->getContents(
			mail_error,
			NULL,
			NULL,
			NULL,
			&name,
			NULL,
			NULL);
	
	if (mail_error.isSet()) {
	    // do something
	}

	// It is possible for an attachment to not have a name.
	if (!name) {
	    name = "NoName";
	}

	this->addAttachment(name, tmpBP);
	tmpBP = msg->getNextBodyPart(mail_error, tmpBP);
	if (mail_error.isSet()) {
	    // do something
	}

	free(name);
    }
}

void
AttachArea::attachment_summary(
    int live,
    int dead
)
{

    char *buf = NULL;
    char * tmp1;
    char * tmp2;

    if ((live == 1) && (dead == 0)) {
	tmp1 = GETMSG(DT_catd, 3, 38, "attachment");
	buf = new char[strlen(tmp1) + 64];
    	sprintf(buf, "%d %s", live, tmp1);
    }
    else if ((live >= 0) && (dead == 0)) {
	/* NL_COMMENT
	 * "attachments" is the plural form of "attachment".
	 */
	tmp1 = GETMSG(DT_catd, 3, 39, "attachments");
	buf = new char[strlen(tmp1) + 64];
	sprintf(buf, "%d %s", live, tmp1);
    }
    else if ((live >= 0) && (dead > 0)) {
	tmp1 = GETMSG(DT_catd, 3, 40, "attachments");
	tmp2 = GETMSG(DT_catd, 3, 41, "deleted");
	buf = new char[strlen(tmp1) + strlen(tmp2) + 64];
	sprintf(buf, "%d %s, %d %s", live, tmp1, dead, tmp2);
    }
    
    if (buf) {
	XmString buf_str = XmStringCreateLocalized(buf);
	XtVaSetValues(_attachments_summary,
	    XmNlabelString, buf_str,
	    NULL );
    
	delete [] buf;
	XmStringFree(buf_str);
    }
}

void
AttachArea::manage()
{
    XtArgVal /* Dimension */ ht;
    XtArgVal /* Dimension */ pht;
    Widget sww;

    // Update the display
    sww = getSWWindow();
    this->manageList();

    XtAddCallback(
		_clipWindow, 
		XmNresizeCallback, &AttachArea::resizeCallback,
		(XtPointer) this );

    XtAddCallback(
		_clipWindow, 
		XmNinputCallback, &AttachArea::inputCallback,
		(XtPointer) this);

    XtAddEventHandler(
		_clipWindow, ButtonPressMask,
                FALSE, MenuButtonHandler,
		(XtPointer) this);

    // hack
    XtVaGetValues(sww, XmNheight, &ht, NULL);
    XtVaGetValues(_w, XmNheight, &pht, NULL);
    // Manage the clipWindow back
    if (!XtIsManaged(_clipWindow)) XtManageChild(_clipWindow);

    // hack
    XtVaSetValues(sww, XmNheight, ht, NULL);
    UIComponent::manage();
    XtVaSetValues(_w, XmNheight, pht, NULL);
}

void
AttachArea::unmanage()
{
    int i;
    Attachment **list = getList();

    // Unmanage the widgets it currently has
    for (i=0; i<getIconCount(); i++)
      list[i]->unmanageIconWidget();

    XtRemoveCallback(
	_clipWindow, 
	XmNresizeCallback, &AttachArea::resizeCallback,
	(XtPointer) this );

    XtRemoveCallback(
	_clipWindow, 
	XmNinputCallback, &AttachArea::inputCallback,
	(XtPointer) this );

    XtRemoveEventHandler(
	_clipWindow, ButtonPressMask,
        FALSE, MenuButtonHandler,
	(XtPointer) this);

    UIComponent::unmanage();
}

void
AttachArea::removeCurrentAttachments()
{

    Attachment **list = getList();
    int i;

    // Unmanage the widgets it currently has

    for (i=0; i<getIconCount(); i++) {

	list[i]->unmanageIconWidget();
	list[i]->deleteIt();
    }

    // Reset
    if (_attachmentList)
	delete []_attachmentList;
    _attachmentList = NULL;
    _iconCount = 0;
    _deleteCount = 0;
    _attach_area_selection_state = AA_SEL_NONE;
    _cache_single_attachment = NULL;

    this->attachment_summary(_iconCount, _deleteCount);
}

// Similar to removeCurrentAttachments().
// Except we don't display a summary that there are no attachments.
// Plus has potential for other (different) usage.

void
AttachArea::clearAttachArea()
{

    Attachment **list = getList();
    int i;

    // Unmanage the widgets it currently has

    for (i=0; i<getIconCount(); i++) {

	list[i]->unmanageIconWidget();
	delete list[i];
    }

    // Reset
    if (_attachmentList)
	delete []_attachmentList;
    _attachmentList = NULL;
    _iconCount = 0;
    _deleteCount = 0;
    _attach_area_selection_state = AA_SEL_NONE;
    _cache_single_attachment = NULL;
}

#ifdef DEAD_WOOD
void
AttachArea::saveAttachmentToFile(
    DtMailEnv &mail_error,
    char *save_path
)
{

    Attachment *attachment = this->getSelectedAttachment();

    if(attachment != NULL)
    	attachment->saveToFile(mail_error, save_path);

}
#endif /* DEAD_WOOD */

void
AttachArea::deleteSelectedAttachments(
    DtMailEnv &		//mail_error
)
{
    Attachment **list = getList();
    int i;
    
    for (i = 0; i<getIconCount(); i++) {
	if (list[i]->isSelected() && !list[i]->isDeleted()) {

	    // unselect it first.  Else, when undeleted it comes
	    // off selected
	    
	    list[i]->unselect();
	    list[i]->deleteIt();
	    _deleteCount++;
	}
    }
    
    // Unmanage all.
    // Their positions need to get recomputed and the undeleted
    // ones get remanaged in manageList().

    for (i=0; i<getIconCount(); i++) {
	list[i]->unmanageIconWidget();
    }

    _cache_single_attachment = NULL;
    _attach_area_selection_state = AA_SEL_NONE;

    this->manageList();
}


#ifdef DEAD_WOOD
void
AttachArea::undeleteAllDeletedAttachments(
    DtMailEnv &		//mail_error
)
{
}
#endif /* DEAD_WOOD */

void
AttachArea::undeleteLastDeletedAttachment(
    DtMailEnv &mail_error
)
{

    Attachment *tmpAttachment;
    Attachment **list;
    time_t time_deleted = NULL, tmpTime = NULL;
    int i;

    if (_deleteCount == 0) {
	return;
    }

    list = getList();

    tmpAttachment = list[0];
    time_deleted = tmpAttachment->getBodyPart()->getDeleteTime(mail_error);
    if (mail_error.isSet()) {
	// do something
    }

    for (i=1; i<getIconCount(); i++) {
	if (list[i]->isDeleted()) {
	    tmpTime = list[i]->getBodyPart()->getDeleteTime(mail_error);
	    if (mail_error.isSet()) {
		// do something
	    }
	    if ( tmpTime > time_deleted) {
		time_deleted = tmpTime;
		tmpAttachment = list[i];
	    }
	}
    }

    tmpAttachment->undeleteIt();
    _deleteCount--;

    // Unmanage all.
    // Their positions need to get recomputed and the deleted
    // ones get remanaged in manageList().

    for (i=0; i<getIconCount(); i++) {
	list[i]->unmanageIconWidget();
    }


    this->manageList();

}

    
void
AttachArea::unselectOtherSelectedAttachments(
    Attachment *attachment
)
{
    if(_attach_area_selection_state == AA_SEL_NONE)
	return;

    if(_attach_area_selection_state == AA_SEL_ALL) {
    	int i;
    	Attachment **list;

    	list = getList(); 

    	for (i=0; i < getIconCount(); i++) 
	    if (list[i]->isSelected() && list[i] != attachment) 
		list[i]->unselect();
    } 
    else if (_cache_single_attachment && 
	    (attachment != _cache_single_attachment)) {
	_cache_single_attachment->unselect();
	_cache_single_attachment = NULL;
    }

   if(attachment == NULL) {
   	// Grey out the appropriate menu items in the RMW...
   	_myOwner->owner()->all_attachments_deselected();
	_attach_area_selection_state = AA_SEL_NONE;
	_cache_single_attachment = NULL;
   }
}

void
AttachArea::addAttachmentActions(
    char **actions,
    int indx
)
{

    _myOwner->owner()->addAttachmentActions(
				actions,
				indx
			);

}

void
AttachArea::setOwnerShell(
    RoamMenuWindow *rmw
)
{
    _myRMW = rmw;
}

void
AttachArea::setOwnerShell(
    ViewMsgDialog *vmd
)
{
    _myVMD = vmd;
}

void
AttachArea::setOwnerShell(
    SendMsgDialog *smd
)
{
    _mySMD = smd;
}

Widget
AttachArea::ownerShellWidget()
{
    if (_myRMW) {
	return(_myRMW->baseWidget());
    }
    else if (_myVMD) {
	return(_myVMD->baseWidget());
    }
    else if (_mySMD) {
	return(_mySMD->baseWidget());
    }
    else {
	// Error out
    }
    return((Widget) NULL);
}

Boolean
AttachArea::isOwnerShellEditable()
{
    // only SMD is editable
    if (_mySMD != NULL) {
	return TRUE;
    }
    else {
	return FALSE;
    }
}

void
AttachArea::setPendingAction(
    Boolean bval
)
{
    _pendingAction = bval;
    if (bval) {
	_numPendingActions++;
    }
    else {
	if (_numPendingActions > 0) {
	    _numPendingActions--;
	}
    }
}

void
AttachArea::resetPendingAction()
{

    _numPendingActions = 0;
}

int
AttachArea::getNumPendingActions()
{
    return(_numPendingActions);
}

void
AttachArea::selectAllAttachments()
{
    
    Attachment **list;
    int numAttachments = getIconCount();

    list = getList(); 
    
    if(list == NULL)
	return;

    // if there's only 1 attachment, select it and
    // add its actions to the menu bar...

    if (numAttachments == 1) {
	list[0]->primitive_select();
        list[0]->set_selected();
    }
    else {
	// More than 1 attachment.
	// Select them all. Don't enable their actions however.

	for (int i=0; i < numAttachments; i++) 
	    list[i]->primitive_select();

	// Grey out the appropriate menu items in the RMW...
	_myOwner->owner()->all_attachments_selected();
	_cache_single_attachment = NULL;
	_attach_area_selection_state = AA_SEL_ALL;
    }
}

int
AttachArea::handleQuestionDialog(
    char *title,
    char *buf,
    char * helpId
)
{
    DtMailGenDialog *dialog;
    int answer;

    if (_myRMW) {
	dialog = _myRMW->genDialog();
    }
    else if (_myVMD) {
	dialog = _myVMD->genDialog();
    }
    else if ( _mySMD) {
	dialog = _mySMD->genDialog();
    }
    else return(-1);

    if (!dialog) return(-1);

    dialog->setToQuestionDialog(
			title,
			buf);
    answer = dialog->post_and_return(helpId);
    return(answer);
}

int
AttachArea::handleErrorDialog(
    char *title,
    char *buf,
    char *helpId
)
{
    DtMailGenDialog *dialog;
    int answer;

    if (_myRMW) {
	dialog = _myRMW->genDialog();
    }
    else if (_myVMD) {
	dialog = _myVMD->genDialog();
    }
    else if ( _mySMD) {
	dialog = _mySMD->genDialog();
    }
    else return(-1);

    if (!dialog) return(-1);

    dialog->setToErrorDialog(title, buf);
    answer = dialog->post_and_return(helpId);
    return(answer);
}
