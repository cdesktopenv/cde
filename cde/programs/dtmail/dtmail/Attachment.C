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
/* $TOG: Attachment.C /main/16 1998/07/23 17:58:44 mgreess $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1993-1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
/*
 *+SNOTICE
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
 *+ENOTICE
 */

#include <EUSCompat.h>   // For strcasecmp()
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <memory.h>
#include <errno.h>
#include <sys/utsname.h>
#include <stdio.h>

#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/BulletinB.h>
#include <Xm/AtomMgr.h>
#include <Xm/DragDrop.h>
#include <Xm/Screen.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/LabelG.h>
#include <X11/IntrinsicP.h>
#include <X11/Xatom.h>

#include "Attachment.h"
#include "Icon.h"
#include "RoamMenuWindow.h"
#include "RoamApp.h"
#include "InfoDialogManager.h"
#include "ViewMsgDialog.h"
#include "RoamMenuWindow.h"
#include "MailMsg.h"
#include "MailSession.hh"
#include <Dt/Dts.h>
#include <DtMail/DtMailError.hh>
#include <DtMail/IO.hh>			// SafeAccess...
#include "MemUtils.hh"
#include "DtMailHelp.hh"
#include "str_utils.h"


extern "C" {
extern XtPointer _XmStringUngenerate (
				XmString string,
				XmStringTag tag,
				XmTextType tag_type,
				XmTextType output_type);
}

extern nl_catd	DtMailMsgCat;

unsigned char validbits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x07, 0x00,
   0x00, 0xfc, 0x3f, 0x00, 0x00, 0xfe, 0x7f, 0x00, 0x00, 0x1f, 0xf8, 0x00,
   0x80, 0x07, 0xe0, 0x01, 0x80, 0x03, 0xc0, 0x01, 0xc0, 0x01, 0x80, 0x03,
   0xc0, 0x01, 0x80, 0x03, 0xe0, 0x00, 0x00, 0x07, 0xe0, 0x80, 0x01, 0x07,
   0xe0, 0xc0, 0x03, 0x07, 0xe0, 0xc0, 0x03, 0x07, 0xe0, 0x80, 0x01, 0x07,
   0xe0, 0x00, 0x00, 0x07, 0xc0, 0x01, 0x80, 0x03, 0xc0, 0x01, 0x80, 0x03,
   0x80, 0x03, 0xc0, 0x01, 0x80, 0x07, 0xe0, 0x01, 0x00, 0x1f, 0xf8, 0x00,
   0x00, 0xfe, 0x7f, 0x00, 0x00, 0xfc, 0x3f, 0x00, 0x00, 0xf0, 0x0f, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

unsigned char invalidbits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x07, 0x00,
   0x00, 0xfc, 0x3f, 0x00, 0x00, 0xfe, 0x7f, 0x00, 0x00, 0x1f, 0xf8, 0x00,
   0x80, 0x0f, 0xe0, 0x01, 0x80, 0x1f, 0xc0, 0x01, 0xc0, 0x3f, 0x80, 0x03,
   0xc0, 0x7d, 0x80, 0x03, 0xe0, 0xf8, 0x00, 0x07, 0xe0, 0xf0, 0x01, 0x07,
   0xe0, 0xe0, 0x03, 0x07, 0xe0, 0xc0, 0x07, 0x07, 0xe0, 0x80, 0x0f, 0x07,
   0xe0, 0x00, 0x1f, 0x07, 0xc0, 0x01, 0xbe, 0x03, 0xc0, 0x01, 0xfc, 0x03,
   0x80, 0x03, 0xf8, 0x01, 0x80, 0x07, 0xf0, 0x01, 0x00, 0x1f, 0xf8, 0x00,
   0x00, 0xfe, 0x7f, 0x00, 0x00, 0xfc, 0x3f, 0x00, 0x00, 0xf0, 0x0f, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#define equal(a, b) (!strcasecmp(a,b))

String typeName;

#ifdef DEAD_WOOD
void runit_callback( int *data );
void norunit_callback( int *data );
#endif /* DEAD_WOOD */
static void okcb(XtPointer);


/*
 * CDExc17304
 *
 * A note on this bug, several related bugs, design of C++ programs and
 * general coding style and proficiency.  This module, as well as most
 * others in the dtmail application, violate many elements of good
 * OO design.  Poor compartmentalization, poor abstraction, ambiguous data
 * ownership, ambiguous data model, lack of a clear Model/View/Controller
 * structure are all examples of things going wrong or being done badly.
 * In the case at hand, we have a much larger problem (inadequate design
 * of mmap()'d file handling) causing numerous small bugs from a
 * collaboration with these design problems.  I will workaround the
 * bug and the design flaw by moving data ownership into this Class thus
 * adding stones to the Chinese Wall that separates this UI-like class
 * from the model-like classes in RFC.  However, it is in no way a
 * real fix for the problems dtmail is experiencing.
 */

Attachment::Attachment(
    AttachArea *classparent,
    String name,
    DtMail::BodyPart *body_part,
    int indx
)
    : UIComponent (
	name
),
    _parent(classparent),
    _body_part(body_part),
    _index(indx),
    _canKillSelf(TRUE),
    _myActionIds(5),
    _myAllocContents(NULL),
    _myContents(NULL),
    _myContentsSize(0),
    _haveContents(FALSE),
    _myType(NULL),
    _subtype(NULL),
    _myActionsList(NULL)
{
    if(strchr(name, '/') == NULL) // The name does not include a slash
	_label = XmStringCreateLocalized(name);
    else			   // The name does include a slash
	_label = XmStringCreateLocalized(strrchr(name, '/')+1);

    _key = theRoamApp.session()->session()->newObjectKey();
}

Attachment::~Attachment(
)
{
    
    theRoamApp.session()->session()->removeObjectKey(_key);

    if (_label) {
      XmStringFree (_label);
    }
    if (_subtype) {
      free (_subtype);
    }
    if (_myType) {
      free (_myType);
    }

    delete myIcon;
    if (_myActionsList) {
	delete []_myActionsList;
    }
    if (_myAllocContents) {
	delete [] _myAllocContents;
    }
    _myAllocContents = NULL;
    _myContents = NULL;
    _myContentsSize = 0;
}


void
Attachment::setAttachArea(AttachArea *aa)
{
    _parent = aa;
}
    

void
Attachment::initialize()
{
    Widget widgetparent;

    assert( _parent != NULL);

    this->setContents();

    widgetparent = _parent->getClipWindow();

    _foreground = _parent->owner()->textEditor()->get_text_foreground();
    _background = _parent->owner()->textEditor()->get_text_background();

    _selected = FALSE;

    // We already have the name from the classing engine.  Now map
    // the name to the MIME type and subtype
    name_to_type();

    myIcon = new Icon(this, _name, _label, _type, widgetparent, _index);
    
    _w = myIcon->baseWidget();

     _deleted = FALSE;
     XmUpdateDisplay(_w);

    {
    Arg args[2];
    int n;
    Dimension aaHeight;
    Widget sw;

    sw = _parent->getSWWindow();

    n = 0;
    XtSetArg(args[n], XmNwidth, &_attachmentWidth);  n++;
    XtSetArg(args[n], XmNheight, &_attachmentHeight);  n++;
    XtGetValues(_w, args, n);
    XtSetArg(args[0], XmNheight, &aaHeight);  n++;
    XtGetValues(sw, args, 1);
    if( aaHeight < (_attachmentHeight+20) )
    {
        XtSetArg(args[0],XmNheight,(_attachmentHeight+20));
        XtSetValues(sw, args, 1);
    
    }
    }
    
    installDestroyHandler();
}

#ifdef DEAD_WOOD
Boolean
Attachment::check_if_binary(String contents, unsigned long size)
{
    int i;

    for(i=0;i<size;i++) {
	if((!isprint(contents[i])) && (!isspace(contents[i])))
	    return True;
    }
    return False;
}
#endif /* DEAD_WOOD */

//
// Map the name (which we got from the classing engine) to a 
// type and subtype
//

void
Attachment::name_to_type()
{
    char *subtype = new char[128];

    // Hack hack!
    // Hardcode ce_name to be text for now.
    // It should actually be determined dynamically from the type of
    // the attachment.

    _ce_name = "text";

    // If the type and subtype are already set then we don't need to
    // map a classing engine type to a MIME type; We already have
    // the MIME type, so just return
    if(_subtype != NULL)
	return;
    if(equal(_ce_name, "text")) {			// text
	_type = TYPETEXT;
	_subtype = strdup("plain");
    } else if(equal(_ce_name, "richtext")) {		// richtext
	_type = TYPETEXT;
	sprintf(subtype, "X-sun-%s",_ce_name);
	_subtype = strdup(subtype);
    } else if(equal(_ce_name, "audio-file")) {		// audio-file
	_type = TYPEAUDIO;
	_subtype = strdup("basic");
    } else if(equal(_ce_name, "default")) {		// default
	_type = TYPEAPPLICATION;
	sprintf(subtype, "X-sun-%s",_ce_name);
	_subtype = strdup(subtype);
    } else if(equal(_ce_name, "oda")) {			// oda
	_type = TYPEAPPLICATION;
	_subtype = strdup("oda");
    } else if(equal(_ce_name, "postscript-file")) {	// postscript
	_type = TYPEAPPLICATION;
	_subtype = strdup("PostScript");
    } else if(equal(_ce_name, "sun-raster")) {		// sun-raster
	_type = TYPEIMAGE;
	sprintf(subtype, "X-%s",_ce_name);
	_subtype = strdup(subtype);
    } else if(equal(_ce_name, "jpeg-file")) {		// jpeg-file
	_type = TYPEIMAGE;
	_subtype = strdup("jpeg");
    } else if(equal(_ce_name, "g3-file")) {		// g3-file
	_type = TYPEIMAGE;
	sprintf(subtype, "X-sun-%s",_ce_name);
	_subtype = strdup(subtype);
    } else if(equal(_ce_name, "gif-file")) {		// gif-file
	_type = TYPEIMAGE;
	_subtype = strdup("gif");
    } else if(equal(_ce_name, "pbm-file")) {		// pbm-file
	_type = TYPEIMAGE;
	sprintf(subtype, "X-sun-%s",_ce_name);
	_subtype = strdup(subtype);
    } else if(equal(_ce_name, "pgm-file")) {		// pgm-file
	_type = TYPEIMAGE;
	sprintf(subtype, "X-sun-%s",_ce_name);
	_subtype = strdup(subtype);
    } else if(equal(_ce_name, "ppm-file")) {		// ppm-file
	_type = TYPEIMAGE;
	sprintf(subtype, "X-sun-%s",_ce_name);
	_subtype = strdup(subtype);
    } else if(equal(_ce_name, "xpm-file")) {		// xpm-file
	_type = TYPEIMAGE;
	sprintf(subtype, "X-sun-%s",_ce_name);
	_subtype = strdup(subtype);
    } else if(equal(_ce_name, "tiff-file")) {		// tiff-file
	_type = TYPEIMAGE;
	sprintf(subtype, "X-sun-%s",_ce_name);
	_subtype = strdup(subtype);
    } else if(equal(_ce_name, "troff")) {		// troff
	_type = TYPETEXT;
	sprintf(subtype, "X-sun-%s",_ce_name);
	_subtype = strdup(subtype);
    } else if(equal(_ce_name, "nroff")) {		// nroff
	_type = TYPETEXT;
	sprintf(subtype, "X-sun-%s",_ce_name);
	_subtype = strdup(subtype);
    } else if(equal(_ce_name, "h-file")) {		// h-file
	_type = TYPETEXT;
	sprintf(subtype, "X-sun-%s",_ce_name);
	_subtype = strdup(subtype);
    } else if(equal(_ce_name, "c-file")) {		// c-file
	_type = TYPETEXT;
	sprintf(subtype, "X-sun-%s",_ce_name);
	_subtype = strdup(subtype);
    } else if(equal(_ce_name, "makefile")) {		// makefile
	_type = TYPETEXT;
	sprintf(subtype, "X-sun-%s",_ce_name);
	_subtype = strdup(subtype);
    } else if(equal(_ce_name, "mail-file")) {		// mail-file
	_type = TYPETEXT;
	sprintf(subtype, "X-sun-%s",_ce_name);
	_subtype = strdup(subtype);
    } else if(equal(_ce_name, "mail-message")) {	// mail-message
	_type = TYPETEXT;
	sprintf(subtype, "X-sun-%s",_ce_name);
	_subtype = strdup(subtype);
    } else {
	_type = TYPEAPPLICATION;
	sprintf(subtype, "X-sun-%s",_ce_name);
	_subtype = strdup(subtype);
    }
    delete [] subtype;
}

void
Attachment::invokeAction(int index)
{
    char		*actionCommand = NULL;
    DtActionArg		*actionArg = NULL;
    DtActionBuffer	 bufArg;

    char		*exble = NULL;
    char		*type = NULL;

    if (_myActionsList == NULL || NULL == _myActionsList[index]) return;

    this->setContents();
    actionCommand = _myActionsList[index];
    memset(&bufArg, 0, sizeof(bufArg));

    bufArg.bp = (void *)_myContents;
    bufArg.size = (int) _myContentsSize;

    // Determine the type based on the contents.
    // This is to compensate for errors that other MUAs could have
    // generated:  some claim a bodyPart is rfc822 but deliver something else.  
    type = bufArg.type = DtDtsBufferToDataType(
					(char*) _myContents,
					(const int) _myContentsSize,
					_name);

    if (_parent->isOwnerShellEditable())
      bufArg.writable = TRUE;
    else
      bufArg.writable = FALSE;

    // If this is the default action (aka Run) and the attachment is executable,
    // display a dialog informing the user of the risks of executing it.
    exble = DtDtsDataTypeToAttributeValue(type, DtDTS_DA_IS_EXECUTABLE, _name);
    if (0 == index && NULL != exble && DtDtsIsTrue(exble))
    {
	int answer;
	char *buf = new char[2048];

	sprintf(buf, "%s",
		GETMSG(DT_catd, 3, 81, "This attachment may contain commands that can cause serious\ndamage.  It is recommended that you only execute it after you\nare certain it is safe to do so.\n\nPress OK if you are certain it is safe,\nCancel to cancel execution."));

	answer = parent()->handleQuestionDialog(
					GETMSG(DT_catd, 1, 86, "Mailer"),
					buf,
					DTMAILHELPEXECUTEOK);
	delete [] buf;
	if (answer == 2) return;
    }

    // Passing a buffer
    actionArg = (DtActionArg *) malloc(sizeof(DtActionArg) * 1);
    memset(actionArg, 0, sizeof(DtActionArg));
    actionArg->argClass = 2;
    actionArg->u.buffer = bufArg;
	
    ActionCallback *acb = new ActionCallback(_key,  this);
    DtActionInvoke(
		_parent->ownerShellWidget(),
		actionCommand, actionArg,
		1, NULL, NULL, NULL, 1,
 	       (DtActionCallbackProc)& Attachment::actionCallback, acb);
}

void
Attachment::handleDoubleClick()
{
    _parent->attachmentFeedback(TRUE);
    invokeAction(0);
    _parent->attachmentFeedback(FALSE);
}

static void okcb( XtPointer )
{
    //Empty
    // This function exists so that the OK button will appear on the
    // Info Dialog. It doesn't have to do anything because the dialog
    // automatically pops down. It is for information only.
}

#ifdef DEAD_WOOD
void
runit_callback(int *data)
{
    *data=1;
}

void
norunit_callback(int *data)
{
    *data=2;
}
#endif /* DEAD_WOOD */

int
Attachment::operator==
(
const Attachment &
)
{
    return 1;
}

void
Attachment::set_selected()
{
    char	*actions_list = NULL;	//Comma separated list of actions
    char*	anAction=NULL;
    int		numActions = 0;

    _selected = TRUE;

    parent()->attachmentSelected(this);

    // Crude Hack.
    // Assuming that only 10 actions are possible.
    // Need a mechanism to query, determine how many possible and
    // allocate memory for that many.

    _myActionsList = new char*[10];

    // Retrieve the actions list.  Walk through the list and
    // for each item in it, ask the parent to create a menu item
    // in its parent's menubar.

    actions_list = DtDtsDataTypeToAttributeValue(
				_myType,
				DtDTS_DA_ACTION_LIST,
				NULL
		   );

    char **tmpActionCommand = _myActionsList;

    if (actions_list != NULL)
    	anAction = strtok(actions_list, ",");
    if (anAction == NULL) {
	return;
    }

    *tmpActionCommand = strdup(anAction);

    while (*tmpActionCommand != NULL) {
	
	// strtok() requires that calls other than the first have NULL as
	// the first arg..

	tmpActionCommand++;
	numActions++;

	anAction = strtok(NULL, ",");
	if (anAction == NULL) {
	    *tmpActionCommand = NULL;
	}
	else {
	    *tmpActionCommand = strdup(anAction);
	}
    }
    parent()->addAttachmentActions(
			_myActionsList,
			numActions
		);

    free((void*) actions_list);
}

void
Attachment::unselect()
{
    _selected = FALSE;
    myIcon->unselect(); 
}

// Save the attachment to the specified file.
void
Attachment::saveToFile(DtMailEnv &, char *filename)
{
    int answer;
    char *buf = new char[2048];
    char *helpId = NULL;

    if (SafeAccess(filename, F_OK) == 0) {

	sprintf(buf, GETMSG(DT_catd, 3, 42, "%s already exists. Replace?"),
		filename);
        helpId = DTMAILHELPALREADYEXISTS;

	answer = parent()->handleQuestionDialog(GETMSG(DT_catd,
							1, 85,
							"Mailer"), 
						buf,
                                                helpId);

	if (answer == 2) { // Pressed cancel
	    delete [] buf;
	    return;
	}

	if (unlink(filename) < 0) {
	    sprintf(buf, GETMSG(DT_catd, 3, 43, "Unable to replace %s."), filename);
            helpId = DTMAILHELPNOREPLACE;
	    answer = parent()->handleErrorDialog(GETMSG(DT_catd,
							 1, 86,
							 "Mailer"), 
						 buf,
                                                 helpId);
	    delete [] buf;
	    return;
	}
    }


//     if (_myContentsSize == 0) {// Oops! BE thinks attachment is of size 0.
// 	sprintf(buf, "Mailer BE thinks attachment is size 0.\nPlease call a Dtmail engineer to attach a debugger\n to this process to begin debugging.  Only after attaching the debugger should you click OK.\n");
// 	answer = parent()->handleErrorDialog("BUG!", buf);
// 	
// 	// Call setContents again.  
// 	// This will help us debug why the body part has bogus contents.
// 	this->setContents();
//     }

    // Create or truncate, and then write the bits.
    //
    int fd = SafeOpen(filename, O_RDWR | O_CREAT | O_TRUNC, 0600);
    if (fd < 0) {
	sprintf(buf, GETMSG(DT_catd, 3, 44, "Unable to create %s."), filename);
        helpId = DTMAILHELPNOCREATE;
	answer = parent()->handleErrorDialog(GETMSG(DT_catd, 1, 87, "Mailer"), 
					     buf,
                                             helpId);
	delete [] buf;
	return;
    }

    if (SafeWrite(fd, _myContents, (unsigned int)_myContentsSize) < _myContentsSize) {
	sprintf(buf, GETMSG(DT_catd, 3, 45, "Unable to create %s."), 
		filename);
        helpId = DTMAILHELPNOCREATE;
	answer = parent()->handleErrorDialog(GETMSG(DT_catd, 1, 88, "Mailer"), 
					     buf,
                                             helpId);
	SafeClose(fd);
	unlink(filename);
	delete [] buf;
	return;
    }
    
    SafeClose(fd);

    // Stat the created file and see if it is of size 0.
    // If it is, engage the user in a dialog and involve a dtmail engineer
    // to attach a debugger to this process.

//     struct stat sbuf;
//     SafeStat(filename, &sbuf);
// 
//     if (sbuf.st_size == 0) {
// 	sprintf(buf, "Mailer produced a zero length file.\nPlease call a Dtmail engineer to attach a debugger\n to this process to begin debugging.  Only after attaching the debugger should you click OK.\n");
// 	answer = parent()->handleErrorDialog("BUG!", buf);
//     }
    delete [] buf;
}

void
Attachment::setX(
    Position x
)
{
    Arg args[2];
    int n = 0;
    Boolean was_managed;

    _positionX = x;

    was_managed = isManaged();
    if (was_managed) unmanageIconWidget();
      
    n = 0;
    XtSetArg(args[n], XtNx, x);	n++;
    XtSetValues( _w, args,n);

    if (was_managed) manageIconWidget();
}

void
Attachment::setY(
    Position y
)
{
    Arg args[2];
    int n = 0;
    Boolean was_managed;

    _positionY = y;

    was_managed = isManaged();
    if (was_managed) unmanageIconWidget();
      
    n = 0;
    XtSetArg(args[n], XtNy, y);	n++;
    XtSetValues( _w, args,n);

    if (was_managed) manageIconWidget();
}

#ifdef DEAD_WOOD
void
Attachment::setRow(
    int row
)
{
    _row = row;
}
#endif /* DEAD_WOOD */


void 
Attachment::actionCallback(
  DtActionInvocationID id,
  XtPointer	clientData,
  DtActionArg	*action_arg,
  int		argCount,
  DtActionStatus status	
)
{
    ActionCallback *acb = (ActionCallback *) clientData;
    if (theRoamApp.session()->session()->validObjectKey(
			acb->_myKey) == DTM_FALSE) {
	// Error out. Post an error?
	return;
    }
    else {
	acb->_myAttachment->action(id, action_arg, argCount, status);
    }
}    

void
Attachment::action(
  DtActionInvocationID id,
  DtActionArg	*actionArg,
  int,		// argCount,
  int		status
)
{
    DtActionBuffer	bufArg;
    DtMailEnv	mail_error;
    int answer;
    char *buf = new char[2048];
    const void * lclContents(NULL);
    unsigned long lclContentsSize(0);

    // Initialize the mail_error.
    mail_error.clear();

    if (status == DtACTION_INVOKED) {
	registerAction(id);
	_parent->setPendingAction(TRUE);
    }
    else if (status == DtACTION_DONE) {
	unregisterAction(id);	
	_parent->setPendingAction(FALSE);
	
	// Check first if ownerShell is an SMD.
	// Secondly, check if the SMD is still available.  If the user had
	// sent the message while this attachment was up, the SMD's reset()
	// method would have set the attachments' _deleted flag to TRUE.
	// So, check to see that the attachment is still valid before setting
	// its contents.
	//
	// Note: actionArg can be NULL if there were no actions associated
	// with the data type.  Is this an Actions bug? Could be, but 
	// we check for it anyway.

	if (actionArg != NULL && _parent->isOwnerShellEditable() && !_deleted) {
	    if (actionArg->argClass == DtACTION_BUFFER) {
		bufArg = actionArg->u.buffer;
		if (bufArg.writable) {
		    // Assume user edited launched attachment -- as
		    // optimization, we can compare buffers to see if
		    // content actually changed.  For now, assume it changed.
		    // Make a copy of incoming buffer and set body part's
		    // contents, size. Let BE determine type.
		    // Reset private variables.
		    _body_part->setContents(
					mail_error, 
					(char *)bufArg.bp,
					bufArg.size,
					NULL, NULL, 0, NULL);
			
// 		    if (mail_error.isSet()) {
// 			//handle error
// 		    }

		    assert(mail_error.isNotSet());

//
// CDExc17304.  Note the following curiosity.  The caller is considered
// the owner of the _myType argument but the called retains ownership
// of the contents.
//
		    if (_myType) {
			free(_myType);
			_myType = NULL;
		    }
		    _body_part->getContents(
					mail_error,
					&lclContents,
					&lclContentsSize,
					&_myType, 
					NULL, 0, NULL);
// 		    if (mail_error.isSet()) {
// 			//handle error
// 		    }
		    assert(mail_error.isNotSet());
		    _setMyContents(lclContents, int(lclContentsSize));
		}
		// Free the buffer...
		XtFree((char *)bufArg.bp);
	    }
	    else {
		// DtACTION_FILE
		// Read the file into a buffer and do the same stuff
		// as above.

		int tmp_file;
		DtActionFile fileArg = actionArg->u.file;
		struct stat stat_buf;

		if ( SafeStat ( fileArg.name, &stat_buf ) == -1 ) {
		    mail_error.setError(DTME_ObjectAccessFailed);
		    mail_error.logError(DTM_FALSE, "Mailer: Unable to process action, stat failed on file %s.\n", fileArg.name);
		    delete [] buf;
		    return;
		}
		tmp_file = SafeOpen(fileArg.name, O_RDONLY);
		char *tmp_buf = (char*) malloc((size_t) stat_buf.st_size);
		SafeRead(tmp_file, (void *)tmp_buf, (size_t) stat_buf.st_size);
		SafeClose(tmp_file);

		_body_part->setContents(mail_error,
					tmp_buf,
					stat_buf.st_size,
					NULL, NULL, 0, NULL);
			
		assert(mail_error.isNotSet());

		// Free the buffer 
		free(tmp_buf);

		if (_myType) {
		    free(_myType);
		    _myType = NULL;
		}

		_body_part->getContents(mail_error,
					&lclContents,
					&lclContentsSize,
					&_myType,
					NULL, 0, NULL);

		assert(mail_error.isNotSet());
		_setMyContents(lclContents, int(lclContentsSize));
 	    }
	}
    }
    else if (status == DtACTION_INVALID_ID) {
	/* NL_COMMENT
	 * Post a dialog explaining that the action was invalid
	 */
	sprintf(buf, "%s",
		GETMSG(
			DT_catd, 3, 91, "Cannot execute invalid action."));

	answer = parent()->handleErrorDialog(GETMSG(DT_catd, 1, 86, "Mailer"),
					     buf);

	unregisterAction(id);	
	_parent->setPendingAction(FALSE);
    }
    else if (status == DtACTION_FAILED) {

	/* NL_COMMENT 
	 * Post a dialog explaining that the action failed.
	 */
	sprintf(buf, "%s",
		GETMSG(DT_catd, 3, 92, "Executing action failed!"));

	answer = parent()->handleErrorDialog(GETMSG(DT_catd, 1, 86, "Mailer"),
					     buf);

	unregisterAction(id);	
	_parent->setPendingAction(FALSE);
    }
    else if (status == DtACTION_STATUS_UPDATE) {
	// Check if ownerShell is an SMD.  Make sure the message has not
	// been sent before attempting to update things.

	if(actionArg != NULL && _parent->isOwnerShellEditable() && !_deleted) {
	    if (actionArg->argClass == DtACTION_BUFFER) {
		bufArg = actionArg->u.buffer;
		if (bufArg.writable) {
		    // Assume user edited launched attachment -- as
		    // optimization, we can compare buffers to see if
		    // content actually changed.  For now, assume it changed.
		    // Make a copy of incoming buffer and set body part's
		    // contents, size. Let BE determine type.
		    // Reset private variables.
		    _body_part->setContents(
					mail_error, 
					(char *)bufArg.bp,
					bufArg.size,
					NULL, NULL, 0, NULL);
			
// 		    if (mail_error.isSet()) {
// 			//handle error
// 		    }

		    assert(mail_error.isNotSet());

		    if (_myType) {
			free(_myType);
			_myType = NULL;
		    }
		    _body_part->getContents(
					mail_error, 
					&lclContents,
					&lclContentsSize,
					&_myType, 
					NULL, 0, NULL);
// 		    if (mail_error.isSet()) {
// 			//handle error
// 		    }

		    assert(mail_error.isNotSet());
		    _setMyContents(lclContents, int(lclContentsSize));
		}
		// Free the buffer 
		XtFree((char *)bufArg.bp);
	    }
	    else {
		// DtACTION_FILE
		// Read the file into a buffer and do the same stuff
		// as above.

		int tmp_file;
		DtActionFile fileArg = actionArg->u.file;
		struct stat stat_buf;

		if ( SafeStat ( fileArg.name, &stat_buf ) == -1 ) {
		    mail_error.setError(DTME_ObjectAccessFailed);
		    mail_error.logError(DTM_FALSE, "Mailer: Unable to process action, stat failed on file %s.\n", fileArg.name);
		    delete [] buf;
		    return;
		}
		tmp_file = SafeOpen(fileArg.name, O_RDONLY);
		char *tmp_buf = (char*) malloc((size_t) stat_buf.st_size);
		SafeRead(tmp_file, (void *)tmp_buf, (size_t) stat_buf.st_size);
		SafeClose(tmp_file);

		_body_part->setContents(mail_error,
					(char *)tmp_buf,
					(int) stat_buf.st_size,
					NULL, NULL, 0, NULL);
			
		assert(mail_error.isNotSet());

		if (_myType) {
		    free(_myType);
		    _myType = NULL;
		}

		_body_part->getContents(mail_error,
					&lclContents,
					&lclContentsSize,
					&_myType,
					NULL, 0, NULL);

		assert(mail_error.isNotSet());
		_setMyContents(lclContents, int(lclContentsSize));

		// Free the buffer 
		free(tmp_buf);
	    }
	}
    }
    else if (status == DtACTION_CANCELED) {
	unregisterAction(id);	
	_parent->setPendingAction(FALSE);

	if (actionArg != NULL) {
		XtFree((char *)(actionArg->u.buffer.bp));
	}
    }
    delete [] buf;
}

void
Attachment::deleteIt()
{
    DtMailEnv mail_error;

    // Initialize the mail_error.
    
    mail_error.clear();

    _deleted = TRUE;

    // Need to remove the view from display

    // Get the BE to mark the bodyPart as deleted
    _body_part->setFlag(mail_error, DtMailBodyPartDeletePending);

}

void
Attachment::undeleteIt()
{
    DtMailEnv mail_error;

    // Initialize the mail_error.

    mail_error.clear();

    _deleted = FALSE;

    _body_part->resetFlag(mail_error, DtMailBodyPartDeletePending);

}

void
Attachment::registerAction(
    DtActionInvocationID id
)
{
    _canKillSelf = FALSE;
    _myActionIds.append(id);

}	


void
Attachment::unregisterAction(
    DtActionInvocationID id
)
{
    if (_myActionIds.length() == 0) {
	// error.  Choke!
    }
    else {
	_myActionIds.remove(id);
	if (_canKillSelf) {
	    // See copious documentation above.
	    delete this;
	}
    }
}

void
Attachment::quit()
{
    _canKillSelf = TRUE;
    if (_myActionIds.length() == 0) {
	delete this;
    }
}

// ActionCallback

ActionCallback::ActionCallback(
    DtMailObjectKey key,
    Attachment *att
)
{
    _myKey = key;
    _myAttachment = att;
}

ActionCallback::~ActionCallback()
{

}


void
Attachment::unmanageIconWidget(void)
{

    XtUnmanageChild(_w);
}

void
Attachment::manageIconWidget(void)
{

    XtManageChild(_w);
}

void
Attachment::setLabel(XmString str)
{
	Arg args[2];
	int n;
	_label = XmStringCopy(str);

	n = 0;
	XtSetArg(args[n], XmNstring, _label); n++;
   //The Icon widget needs to be unmanaged first before involking XtSetValues
   // Otherwise, the parent of the Icon widget does not allow the icon widget
   // resize. The Icon widget will be remanaged after its dimensions (w and h)
   // are obtained by XtGetValues. This is a fix of the defect 176690.
        unmanageIconWidget();
	XtSetValues(_w, args, n);

	n = 0;
	XtSetArg(args[n], XmNwidth, &_attachmentWidth); n++;
	XtSetArg(args[n], XmNheight, &_attachmentHeight); n++;
	XtGetValues(_w, args, n);
        manageIconWidget();
}

void
Attachment::primitive_select()
{
	_selected = TRUE;
	myIcon->primitiveSelect();

}

void
Attachment::rename(
    XmString new_name
)
{
    char *name = NULL;
    DtMailEnv mail_error;
    
    mail_error.clear();

    name = (char *) _XmStringUngenerate(new_name, NULL,
					XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
    
    _body_part->setContents(
			mail_error,
			NULL, 1, NULL,
			name, 0, NULL);

    this->setLabel(new_name);
}

void
Attachment::setContents()
{
    DtMailEnv mail_error;
    const void * lclContents;
    unsigned long lclContentsSize;

    // Initialize the mail_error.
    mail_error.clear();

    if (_myType) {
	free(_myType);
	_myType = NULL;
    }
    _body_part->getContents(
			mail_error,
			&lclContents,
			&lclContentsSize,
			&_myType,
			NULL, 0, NULL);

    // BE has returned an error condition...

    // It would be nice to popup a dialog to let the user know that 
    // dtmail has run into a problem that it can't resolve.  Unfortunately,
    // the code for postFatalErrorDialog() has been ifdef'ed out.  Perhaps
    // the error dialog can be enabled at some point in the future.

    // if (mail_error.isSet())
    //   parent()->myRMW->postFatalErrorDialog(mail_error);
    assert ( mail_error.isNotSet() );
    _setMyContents(lclContents, int(lclContentsSize));

    _haveContents = TRUE;

    // If it is a rfc822 message, check if it has From stuffing.
    // From stuffing is ">From".
    // If it has, advance the pointer to step past the ">" character.

    if (_myType && !strcmp(_myType,"DTMAIL_FILE")) {

	// If it has a From header, return.  The classing engine uses
	// that to determine the client that needs to be launched.

	if (_myContents &&
	    0 == strncasecmp((char *)_myContents, "From", 4)) {
	    return;
	}

	// Message doesn't begin with From. It may have From stuffing --
	// ">From".  Or it may have no From header at all. The MIME 
	// specs are vague on what headers an Message/RFC822 body part
	// has to have.  We need From to help the classing engine and
	// therefore, we will by force (or hack) make it have a From :-)

	if (_myContents &&
	    0 == strncasecmp((char *)_myContents, ">From", 5)) {

	    // Has From stuffing.

	    char *ptr = (char *)_myContents;
	    ptr++;			    // Step past the first char
	    _myContents = (void *) ptr;	   // Reset contents
	    _myContentsSize--;
	}

	else {
	
	    // No "From" header.
	    // Generate a new content string by prepending an
	    // "From UNKNOWN" header.

	    char	*buffer = NULL;
	    char	*from_hdr = "From UNKNOWN";
	    int		size = 0;
	    
	    // Allocate a buffer for the new contents.
	    // One for the '\0' and two for the extra newlines.
	    size = strlen(from_hdr) + int(_myContentsSize) + 3;
	    buffer = new char[size];

	    // Look for the first occurance of a colon or a newline.
	    char *sptr;
	    for (sptr = (char*) _myContents;
		 *sptr && *sptr != '\n' && *sptr != ':';
		 sptr++) {;}

	    // Copy in the default From header.
	    // Add a second newline if there are no rfc822 message headers.
	    // Assume that finding a colon prior to a newline indicates
	    // an rfc822 message header.
	    if (*sptr == ':')
	      sprintf(buffer, "%s\n", from_hdr);
	    else
	      sprintf(buffer, "%s\n\n", from_hdr);

	    // Copy in the original contents.
	    size = strlen(buffer);
	    memcpy(&buffer[size], (char *) _myContents, (int) _myContentsSize);
	    size += (int) _myContentsSize;
	    buffer[size] = '\0';

	    _setMyContents(buffer, size);
	    delete [] buffer;
	}
    }
}

void *
Attachment::getContents()
{
    if (_myContents) {
	return ( (void *)_myContents ); 
    }
    else {
	this->setContents();
	return ( (void *)_myContents ); 
    }
}

/*
 * _setMyContents
 *
 * Makes a real copy of the specified data to be used as the
 * attachments's data.  Note that a substring of the existing value
 * can be passed in as an argument and the right thing will happen.
 */
void
Attachment::_setMyContents(const void * data, int size)
{
    char * new_contents;
    int new_size;

    if (size > 0) {
	new_size = size;
	new_contents = new char [new_size + 1];
	memcpy(new_contents, data, new_size);
	new_contents[new_size] = '\0';
    }
    else {
	new_size = 0;
	new_contents = NULL;
    }
    if (_myAllocContents != NULL) {
	delete [] _myAllocContents;
    }
    _myAllocContents = new_contents;
    _myContents = new_contents;
    _myContentsSize = new_size;
    return;
}
