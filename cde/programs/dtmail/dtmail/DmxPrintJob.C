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
/* $TOG: DmxPrintJob.C /main/30 1998/07/23 18:11:50 mgreess $ */

/*
 *+SNOTICE
 *
 *	$:$
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
 *	Copyright 1994 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */
/*
 *		     Common Desktop Environment
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *   (c) Copyright 1995 Digital Equipment Corp.
 *   (c) Copyright 1995 Fujitsu Limited
 *   (c) Copyright 1995 Hitachi, Ltd.
 *                                                                   
 *
 *                     RESTRICTED RIGHTS LEGEND                              
 *
 *Use, duplication, or disclosure by the U.S. Government is subject to
 *restrictions as set forth in subparagraph (c)(1)(ii) of the Rights in
 *Technical Data and Computer Software clause in DFARS 252.227-7013.  Rights
 *for non-DOD U.S. Government Departments and Agencies are as set forth in
 *FAR 52.227-19(c)(1,2).

 *Hewlett-Packard Company, 3000 Hanover Street, Palo Alto, CA 94304 U.S.A.
 *International Business Machines Corp., Route 100, Somers, NY 10589 U.S.A. 
 *Sun Microsystems, Inc., 2550 Garcia Avenue, Mountain View, CA 94043 U.S.A.
 *Novell, Inc., 190 River Road, Summit, NJ 07901 U.S.A.
 *Digital Equipment Corp., 111 Powdermill Road, Maynard, MA 01754, U.S.A.
 *Fujitsu Limited, 1015, Kamikodanaka Nakahara-Ku, Kawasaki 211, Japan
 *Hitachi, Ltd., 6, Kanda Surugadai 4-Chome, Chiyoda-ku, Tokyo 101, Japan
 */


#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/param.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/Print.h>
#include "Dmx.h"
#include "DmxPrintJob.h"
#include "DmxPrintOptions.h"
#include "DmxPrintOutput.h"
#include "DmxPrintSetup.h"
#include "DtMailGenDialog.hh"
#include "DtMailDialogCallbackData.hh"
#include "MailMsg.h"
#include "OptCmd.h"
#include "RoamApp.h"


//
// Public class methods
//

DmxPrintJob::DmxPrintJob(
			char		*filename,
			DtMailBoolean	silent,
			MainWindow	*window
			 ) : UIComponent( "PrintJob" )
{
    _next_msg = (DmxMsg *) NULL;
    _filename = strdup(filename);
    _mailbox = (DmxMailbox *) NULL;
    _parent = window;
    _print_output = (DmxPrintOutput *) NULL;
    _print_setup = (DmxPrintSetup *) NULL;
    _print_data = (DtPrintSetupData *) XtMalloc(sizeof(DtPrintSetupData));
    memset((void*) _print_data, 0, sizeof(DtPrintSetupData));
    _pshell = (Widget) NULL;
    _silent = silent;

    _spool_msg_info = (DmxMsgInfo *) NULL;
    _spool_nmsgs_done = 0;
    _spool_nmsgs_total = 0;
    _spool_npages_done = 0;
    _spool_npages_total = 0;
    _w = XtCreateWidget(_name, coreWidgetClass, _parent->baseWidget(), NULL, 0);
    installDestroyHandler();

    _nextpage_shell = NULL;
    _nextpage_button = NULL;
}

DmxPrintJob::~DmxPrintJob (void)
{
    //
    // Unregister the fact that we have a print job running so
    // dtmail can terminate.
    //
    if (_parent != NULL)
      _parent->unregisterPendingTask();

    if (_filename)
    {
	free(_filename);
	_filename = NULL;
    }
    if (_mailbox)
    {
        delete _mailbox;
	_mailbox = NULL;
    }
    if (_spool_msg_info)
    {
        XtFree((char*) _spool_msg_info);
	_spool_msg_info = NULL;
    }
    if (_print_output)
    {
        delete _print_output;
	_print_output = NULL;
    }
    if (_print_setup)
    {
        delete _print_setup;
	_print_setup = NULL;
    }
    if (_pshell)
    {
        XtDestroyWidget(_pshell);
	_pshell = NULL;
    }
    if (_print_data)
    {
	DtPrintFreeSetupData(_print_data);
        XtFree((char *) _print_data);
	_print_data = NULL;
    }
    if (_nextpage_shell)
    {
        XtDestroyWidget(_nextpage_shell);
	_nextpage_shell = NULL;
    }
    if (_w)
    {
	//
	//  Do not destroy this widget.
	//  BasicComponent takes care of this for us.
	//
    }
}

/*
 * Name: DmxPrintJob::cancel
 * Description:
 *	Public method used to cancel the print job.
 */
void
DmxPrintJob::cancel(void)
{

    _parent->clearStatus();

    if (NULL != _pshell)
    {
        Display	*display = XtDisplay(_pshell);

        theRoamApp.unregisterActivePrintDisplay(display);
        if (display == theRoamApp.getErrorPrintDisplay())
        {
	    //
	    // Need to display an error dialog;
	    //
            DtMailGenDialog *genDialog = new DtMailGenDialog(
							"Dialog",
							_parent->baseWidget());
            char *errMsg =
	      GETMSG(
	        DT_catd, 21, 23,
	        "The X Print Server is temporarily out of resources.");

            genDialog->setToErrorDialog(GETMSG(DT_catd, 1, 6, "Mailer"),
                            		errMsg);
            genDialog->post_and_return(GETMSG(DT_catd, 3, 9, "OK"), NULL);
            delete genDialog;

	    theRoamApp.setErrorPrintDisplay(NULL);
        }
    }
    delete this;
}

/*
 * Name: DmxPrintJob::execute
 * Description:
 *	Public method used to execute the print job.
 */
void
DmxPrintJob::execute (void)
{
    theRoamApp.busyAllWindows();
    _print_setup = new DmxPrintSetup(
				_parent->baseWidget(),
				&DmxPrintJob::printCB, (XtPointer) this,
				&DmxPrintJob::cancelCB, (XtPointer) this,
				&DmxPrintJob::closeDisplayCB, (XtPointer) this,
				&DmxPrintJob::pdmSetupCB, (XtPointer) this);

    //
    // load in the messages
    //
    _mailbox = new DmxMailbox(_filename);
    _mailbox->loadMessages();
    _next_msg = _mailbox->firstMessage();

    if (_mailbox->numMessages() == 1)
    {
	int	i,j,last_space;
	char	*orig_subject = _next_msg->getMessageHeader(DMXSUBJ);
	int	orig_len = strlen(orig_subject);
	char	*subject = (char*) malloc(orig_len+1);
	char	*filename = (char*) malloc(MAXPATHLEN+1);

        
        for (i=0,j=0,last_space=0; i<orig_len; i++)
        {
            if (isspace(orig_subject[i]))
            {
                if (last_space < i-1)
		  subject[j++] = ' ';
                last_space = i;
            }
            else if (orig_subject[i] == '/')
              subject[j++] = '\\';
            else
              subject[j++] = orig_subject[i];
        }
        subject[j] = '\0';
        sprintf(filename, "%s/%s.ps", getenv("HOME"), subject);

	_print_setup->setPrintToFileName(filename);
        free(orig_subject);
        free(subject);
        free(filename);
    }
    theRoamApp.unbusyAllWindows();

    if (_silent) {
	if (DTM_TRUE != _print_setup->getDefaultPrintData(_print_data))
	  //
	  // DtPrintSetupDialog is popped up automatically.
	  //
	  return;

	createPrintShell();
	doPrint();
    } else {
        _print_setup->display();
    }
}



//
// Private class methods
//

/*
 * Name: DmxPrintJob::createPrintShell
 * Description:
 *	Creates the XmPrint Shell widget.
 */
void
DmxPrintJob::createPrintShell (void)
{
#if defined(PRINTING_SUPPORTED)
    DtMailEnv		dmxenv;
    DtMail::Session	*d_session = theRoamApp.session()->session();

    if (_pshell != NULL) return;

    //
    // Create the print shell and
    // the print output widgets
    //
#ifdef PRINT_TO_VIDEO 
    XmString	xms;

    /*
     * Create a dialog shell widget on the video display.
     */
    _pshell = XmCreateDialogShell(_parent->baseWidget(), "PrintVideo", NULL, 0);

    _nextpage_shell = XtVaCreateWidget(
			"PrintNextPage",
                         topLevelShellWidgetClass,
                         _parent->baseWidget(),
			 XmNx, 0,
			 XmNy, 0,
                         NULL);

    xms = XmStringCreateLocalized("Push for Next Page");
    _nextpage_button = XtVaCreateManagedWidget(
				"NextPageButton",
				xmPushButtonWidgetClass,
				_nextpage_shell,
				XmNlabelString, xms,
				XmNwidth, 300,
				XmNheight, 300,
				NULL);
    XmStringFree(xms);

    XtAddCallback(
		_nextpage_button,
		XmNactivateCallback, 
		&DmxPrintJob::printOnePageCB,
		(XtPointer) this);

    XtRealizeWidget(_nextpage_shell);
    XtManageChild(_nextpage_shell);
#else
    /*
     * Create an XmPrintShell widget on the video display.
     */
    _pshell = XmPrintSetup(
               	_parent->baseWidget(),
		XpGetScreenOfContext(
				_print_data->print_display,
				_print_data->print_context),
               	"Print",
               	NULL, 0);

    XtAddCallback(
                _pshell,
                XmNpageSetupCallback,
		&DmxPrintJob::printOnePageCB,
                (XtPointer) this);
    XtAddCallback(
                _pshell,
                XmNpdmNotificationCallback,
                &DmxPrintJob::pdmNotificationCB,
                (XtPointer) this);
#endif

#endif  /* PRINTING_SUPPORTED */
}


/*
 * Name: DmxPrintJob::createOutputWidgets
 * Description:
 *	Creates the DmxPrintOutput object to be used for printing.
 */
void
DmxPrintJob::createOutputWidgets (void)
{
    DtMailEnv		dmxenv;
    DtMail::Session	*d_session = theRoamApp.session()->session();
    DtMailBoolean	parse_error = DTM_FALSE;
    const char		*top;
    const char		*right;
    const char		*bottom;
    const char		*left;

    _print_output = new DmxPrintOutput( _pshell );
    _print_output->setWrapToFit(_print_setup->useWordWrap());
    top		= DmxPrintOptions::getMarginSpec(DTPRINT_OPTION_MARGIN_TOP);
    right	= DmxPrintOptions::getMarginSpec(DTPRINT_OPTION_MARGIN_RIGHT);
    bottom	= DmxPrintOptions::getMarginSpec(DTPRINT_OPTION_MARGIN_BOTTOM);
    left	= DmxPrintOptions::getMarginSpec(DTPRINT_OPTION_MARGIN_LEFT);
    _print_output->setPageMargins(top, right, bottom, left, &parse_error);
    if (parse_error)
    {
	DtMailGenDialog *genDialog = new DtMailGenDialog(
							"Dialog",
							_parent->baseWidget());
        char		*errMsg = (char *) XtMalloc(1024);
	char		*i18nMsg;

	i18nMsg = GETMSG(
			DT_catd, 21, 2,
			"One of the following margin specifiers \n has incorrect syntax: \n %s \n %s \n %s \n %s \nContinue using default margins?"
			);

        sprintf(errMsg, i18nMsg, top, right, bottom, left);
        genDialog->setToErrorDialog(
                            GETMSG(DT_catd, 21, 3, "Mailer"),
                            errMsg);
        XtFree(errMsg);

        genDialog->post_and_return(
                            GETMSG(DT_catd, 21, 4, "OK"),
                            NULL);
        delete genDialog;
    }

    XtRealizeWidget( _pshell );

    if (NULL != top)
      free((void*) top);
    if (NULL != right)
      free((void*) right);
    if (NULL != bottom)
      free((void*) bottom);
    if (NULL != left)
      free((void*) left);
}

void
ok_cb(DtMailGenDialog *genDialog)
{
    delete genDialog;
}

/*
 * Name: DmxPrintJob::doPrint
 * Description:
 *	Creates a DmxMailbox and loads the contents of the message file.
 *	Initializes the mailbox to iterate through the messages.
 *	Calls XmPrintToFile or XpStartJob to initiate printing.
 */
void
DmxPrintJob::doPrint (void)
{
#if defined(PRINTING_SUPPORTED)
    DtMailEnv		dmxenv;
    DtMail::Session	*d_session = theRoamApp.session()->session();
    XPSaveData		dest = XPSpool;

    //
    // Notify the user that we're printing
    //
    if (_parent != NULL) {
        char *buf = new char[1024];
	char *msg = GETMSG(DT_catd, 21, 1, "Printing %s ...");
        sprintf(buf, msg, _filename);
        _parent->setStatus(buf);

        //
        // Register the fact that we have a print job running so
        // dtmail will not terminate in the middle.
        //
        _parent->registerPendingTask();
	delete [] buf;
    }

    //
    // load in the messages
    //
    // Moved to DmxPrintJob::execute
    //
    //_mailbox = new DmxMailbox(_filename);
    //_mailbox->loadMessages();
    //_next_msg = _mailbox->firstMessage();

    if (_next_msg)
      _spool_msg_info = (DmxMsgInfo *) XtCalloc(
					_mailbox->numMessages(),
					sizeof(DmxMsgInfo));

#ifndef PRINT_TO_VIDEO
    if (_print_data == NULL) return;

    if (_print_data->destination == DtPRINT_TO_FILE)
      dest = XPGetData;

    theRoamApp.registerActivePrintDisplay(XtDisplay(_pshell));
    XpStartJob(XtDisplay(_pshell), dest);
    XFlush(XtDisplay(_pshell));

    if (_print_data->destination == DtPRINT_TO_FILE)
    {
        if (FALSE == XmPrintToFile(
			XtDisplay(_pshell),
			_print_data->dest_info,
			(XPFinishProc) &DmxPrintJob::finishedPrintToFile,
			(XPointer) this))
        {
            char        	*fmt = "%s\n%s:  %s";
	    DtMailGenDialog	*genDialog;
            char        	*appmessage = NULL;
            char        	*message = NULL;
            char        	*sysmessage = strerror(errno);
            Widget		w = (Widget) this->_parent->baseWidget();
 
            XpCancelJob(XtDisplay(_pshell), False);
            XFlush(XtDisplay(_pshell));

	    genDialog = new DtMailGenDialog("Dialog", w);
            appmessage = (char *) GETMSG(
                                DT_catd, 21, 14,
                                "'Print to File'  was unsuccessful.");
 
            if (NULL == sysmessage)
            {
                message = XtMalloc(strlen(appmessage) + 1);
                sprintf(message, "%s", appmessage);
            }
            else
            {
                message = XtMalloc(
                                strlen(appmessage) +
                                strlen(sysmessage) +
                                strlen(_print_data->dest_info) +
                                strlen(fmt) + 1);
                sprintf(
			message, fmt,
			appmessage, _print_data->dest_info, sysmessage);
            }
 
            genDialog->setToErrorDialog(
                            GETMSG(DT_catd, 21, 3, "Mailer"),
                            message);
#if 0
            genDialog->post_and_return(
                            GETMSG(DT_catd, 21, 4, "OK"),
                            NULL);
            delete genDialog;
#else
            genDialog->post((void*) genDialog, (DialogCallback) ok_cb);
#endif
            XtFree(message);
 
            this->cancel();
        }
    }
#endif
#endif  /* PRINTING_SUPPORTED */
}



Boolean
DmxPrintJob::loadOutputWidgets(void)
{
    if (! _next_msg)
      return FALSE;

    _print_output->clearContents();
    _spool_nmsgs_done = 0;
    _spool_nmsgs_total = 0;
    _spool_npages_done = 0;
    _spool_npages_total = 0;

    do
    {
#if 0
	_next_msg->display(
			DmxPrintOptions::getPrintedHeaders(),
			(DmxMsg::DmxPrintOutputProc) fprintf,
			(XtPointer) stdout);
#endif
	_next_msg->display(
			DmxPrintOptions::getPrintedHeaders(),
			&DmxPrintOutput::appendContents,
			(XtPointer) _print_output);

	switch (DmxPrintOptions::getMessageSeparator())
	{
	  case DMX_SEPARATOR_BLANK_LINE:
	    _print_output->appendNewLine();
	    _print_output->appendNewLine();
	    break;
	  case DMX_SEPARATOR_CHARACTER_LINE:
	    {
		#define DMXPJ_MAX_SEPARATOR_LENGTH 100
		char	*buffer;
		const char *separator_string;
		int	len, nchars;

	        separator_string = DmxPrintOptions::getSeparatorString();
	        len = strlen(separator_string);
	        nchars = _print_output->getCharactersPerLine() - 5;
#ifdef PRINT_TO_VIDEO
		fprintf(stderr, "getCharactersPerLine:  <%d>\n", nchars);
#endif
		nchars = (nchars < DMXPJ_MAX_SEPARATOR_LENGTH) ?
			 nchars : DMXPJ_MAX_SEPARATOR_LENGTH;
	        buffer = XtMalloc(nchars+len+1);
		*buffer = '\0';
	        for (int i=0; i<nchars-1; i+=len)
		{
		    int nbytes = (len < nchars-i) ? len : nchars-i;
	            strncat(buffer, separator_string, nbytes);
		}
	        buffer[nchars-1] = '\0';

	        _print_output->appendNewLine();
	        _print_output->appendContents((void*) _print_output, buffer);
	        _print_output->appendNewLine();
	        _print_output->appendNewLine();

		XtFree(buffer);
		if (NULL != separator_string)
		  free((void*) separator_string);
	        
		break;
	    }
	  case DMX_SEPARATOR_PAGE_BREAK:
	  case DMX_SEPARATOR_NEW_JOB:
	    _print_output->appendPageBreak();
	    break;
	  case DMX_SEPARATOR_NEW_LINE:
	  default:
	    _print_output->appendNewLine();
	    break;
        }

	assert(_spool_nmsgs_total < _mailbox->numMessages());
	_spool_msg_info[_spool_nmsgs_total].msg = _next_msg;
	_spool_msg_info[_spool_nmsgs_total].end_position =
	    _print_output->getLastPosition();

	_spool_nmsgs_total++;

    } while ( (  (_next_msg = _mailbox->nextMessage()) != (DmxMsg *) NULL  ) &&
	      (  (! _print_setup->printSeparately()) ||
		 (_print_data->destination == DtPRINT_TO_FILE )  )
	    );


    _print_output->setTopPosition(0);
    _spool_npages_total = _print_output->getNumLines() +
			_print_output->getLinesPerPage() - 1;
    _spool_npages_total /= _print_output->getLinesPerPage();

    return TRUE;
}

void
DmxPrintJob::printOnePageCB(
			Widget,
			XtPointer client_data,
			XtPointer call_data)
{
#if defined(PRINTING_SUPPORTED)
    DmxPrintJob		*thisJob = (DmxPrintJob *) client_data;

    XmPrintShellCallbackStruct	*pscbs = (XmPrintShellCallbackStruct*)call_data;
    DmxMsg			*currmsg;
    int				top = 0;

    if (thisJob->_print_output == NULL)
    {
        
	XtArgVal /* Dimension */	width, height;

        width = 0; height=0;
        XtVaGetValues(
                thisJob->_pshell,
                XmNwidth, &width,
                XmNheight, &height,
                NULL);
#if PRINT_TO_VIDEO
        printf("PrintShell in PrintOnePageCB: <W %d - H %d>\n", width, height);
#endif

	if (width < 100 || height < 100)
	{
            width = 2550; height=3250;
            XtVaSetValues(
                    thisJob->_pshell,
                    XmNwidth, width,
                    XmNheight, height,
                    NULL);
            width = 0; height=0;
            XtVaGetValues(
                    thisJob->_pshell,
                    XmNwidth, &width,
                    XmNheight, &height,
                    NULL);

#if PRINT_TO_VIDEO
            printf("PrintShell in PrintOnePageCB: <W %d - H %d>\n",
		   width,height);
#endif
        }

	thisJob->createOutputWidgets();
	if (! thisJob->loadOutputWidgets())
	{
	    thisJob->cancel();
	    return;
        }
    }

#ifndef PRINT_TO_VIDEO 
    if (pscbs->last_page)
#else
    if (thisJob->_spool_npages_done == thisJob->_spool_npages_total)
#endif
    {
        int	dest = XPSpool;

        //
        // This spool job is done.
        // Try spooling the next one.
        //
	if (! thisJob->loadOutputWidgets())
	{
            if (thisJob->_print_data->destination != DtPRINT_TO_FILE)
	      thisJob->cancel();
	    return;
        }

        //
        // Register a work proc to print one page at a time
        //
#ifndef PRINT_TO_VIDEO 
        if (thisJob->_print_data->destination == DtPRINT_TO_FILE)
          dest = XPGetData;

        XpStartJob(XtDisplay(thisJob->_pshell), dest);
        XFlush(XtDisplay(thisJob->_pshell));

        if (thisJob->_print_data->destination == DtPRINT_TO_FILE)
          XmPrintToFile(
		XtDisplay(thisJob->_pshell),
		thisJob->_print_data->dest_info,
		(XPFinishProc) &DmxPrintJob::finishedPrintToFile,
		(XPointer) thisJob);
#endif

	return;
    }

    if (thisJob->_spool_npages_done == 0)
    {
        thisJob->_print_output->setTopPosition(0);
    }
    else if (! thisJob->_print_output->pageDown())
    {
#ifndef PRINT_TO_VIDEO 
        pscbs->last_page = TRUE;
#endif
	return;
    }
    else
    {
        int top = thisJob->_print_output->getTopPosition();
        int currmsg_i = thisJob->_spool_nmsgs_done;

        //
        // Find the index into the ._spool_msg_info array of the
        // message which will begin the next page.  This message
        // will be used to set the header and footer strings.
        //
        while (top > thisJob->_spool_msg_info[currmsg_i].end_position &&
	       currmsg_i < thisJob->_spool_nmsgs_total)
          currmsg_i++;

        assert(top <= thisJob->_spool_msg_info[currmsg_i].end_position);
        thisJob->_spool_nmsgs_done = currmsg_i;
    }

    currmsg = thisJob->_spool_msg_info[thisJob->_spool_nmsgs_done].msg;
    assert(currmsg != (DmxMsg *) NULL);
    if (currmsg == (DmxMsg *) NULL)
    {
#ifndef PRINT_TO_VIDEO 
        pscbs->last_page = TRUE;
#endif
	return;
    }

    //
    // Update header and footer strings.
    //
    thisJob->_spool_npages_done++;
    thisJob->updatePageHeaders(
		currmsg,
		DmxPrintOptions::getHdrFtrSpec(DTPRINT_OPTION_HEADER_LEFT),
		DmxPrintOptions::getHdrFtrSpec(DTPRINT_OPTION_HEADER_RIGHT),
		DmxPrintOptions::getHdrFtrSpec(DTPRINT_OPTION_FOOTER_LEFT),
		DmxPrintOptions::getHdrFtrSpec(DTPRINT_OPTION_FOOTER_RIGHT)
		);

#ifndef PRINT_TO_VIDEO 
    if (thisJob->_spool_npages_done >= thisJob->_spool_npages_total)
      pscbs->last_page = TRUE;
#endif
#endif  /* PRINTING_SUPPORTED */
}


void
DmxPrintJob::updatePageHeaders(
				DmxMsg			*msg,
				DmxStringTypeEnum	hl_type,
				DmxStringTypeEnum	hr_type,
				DmxStringTypeEnum	fl_type,
				DmxStringTypeEnum	fr_type
				)
{
    char *hl_string,
	 *hr_string,
	 *fl_string,
	 *fr_string;

    if (msg == (DmxMsg *) NULL)
      return;

    hl_string = getPageHeaderString(msg, hl_type);
    hr_string = getPageHeaderString(msg, hr_type);
    fl_string = getPageHeaderString(msg, fl_type);
    fr_string = getPageHeaderString(msg, fr_type);
    
    _print_output->setHdrFtrStrings(
				hl_string,
				hr_string,
				fl_string,
				fr_string);
    
    if (DMX_NONE_STRING == hl_type && DMX_NONE_STRING == hr_type)
      _print_output->hideHeaders();
    else
      _print_output->showHeaders();

    if (DMX_NONE_STRING == fl_type && DMX_NONE_STRING == fr_type)
      _print_output->hideFooters();
    else
      _print_output->showFooters();

    free(hl_string);
    free(hr_string);
    free(fl_string);
    free(fr_string);
}

char *
DmxPrintJob::getPageHeaderString(
				DmxMsg			*msg,
				DmxStringTypeEnum	type
				   )
{
    char *format,
	 *buf = (char *) NULL;


    if (msg == (DmxMsg *) NULL)
      return NULL;

    switch (type)
    {
	case DMX_NONE_STRING:
	    buf = strdup(" ");
	    break;
	case DMX_CC_HEADER_STRING:
	    {
		char	*hdrstr = msg->getMessageHeader(DMXCC);

	        format = GETMSG(DT_catd, 21, 5, "Cc:  %s");
	        buf = (char *) malloc(strlen(format) + strlen(hdrstr) + 1); 
	        if (buf != (char *) NULL)
	          sprintf(buf, format, hdrstr);
		
		free(hdrstr);
	    }
	    break;
	case DMX_DATE_HEADER_STRING:
	    {
		char	*hdrstr = msg->getMessageHeader(DMXDATE);

	        format = GETMSG(DT_catd, 21, 6, "Date:  %s");
	        buf = (char *) malloc(strlen(format) + strlen(hdrstr) + 1); 
	        if (buf != (char *) NULL)
	          sprintf(buf, format, hdrstr);
		
		free(hdrstr);
	    }
	    break;
	case DMX_FROM_HEADER_STRING:
	    {
		char	*hdrstr = msg->getMessageHeader(DMXFROM);

	        format = GETMSG(DT_catd, 21, 7, "From:  %s");
	        buf = (char *) malloc(strlen(format) + strlen(hdrstr) + 1); 
	        if (buf != (char *) NULL)
	          sprintf(buf, format, hdrstr);
		
		free(hdrstr);
	    }
	    break;
	case DMX_SUBJECT_HEADER_STRING:
	    {
		char	*hdrstr = msg->getMessageHeader(DMXSUBJ);

	        format = GETMSG(DT_catd, 21, 8, "Subject:  %s");
	        buf = (char *) malloc(strlen(format) + strlen(hdrstr) + 1); 
	        if (buf != (char *) NULL)
	          sprintf(buf, format, hdrstr);
		
		free(hdrstr);
	    }
	    break;
	case DMX_TO_HEADER_STRING:
	    {
		char	*hdrstr = msg->getMessageHeader(DMXTO);

	        format = GETMSG(DT_catd, 21, 9, "To:  %s");
	        buf = (char *) malloc(strlen(format) + strlen(hdrstr) + 1); 
	        if (buf != (char *) NULL)
	          sprintf(buf, format, hdrstr);
		
		free(hdrstr);
	    }
	    break;
	case DMX_PAGE_NUMBER_STRING:
	    // 
	    // Allocate space for the format and the translated page number.
	    //
	    {
	        format = GETMSG(DT_catd, 21, 10, "Page %d of %d");
	        buf = (char *) malloc(strlen(format) + 16); 
	        if (buf != (char *) NULL)
	          sprintf(buf, format, _spool_npages_done, _spool_npages_total);
	    }
	    break;
	case DMX_USER_NAME_STRING:
	    // 
	    // Allocate space for the format and the username.
	    //
	    {
		struct passwd	*pw;
	        
		format = GETMSG(DT_catd, 21, 11, "Mail For:  %s");
		pw = getpwuid(getuid());
	        buf = (char *) malloc(strlen(format) + strlen(pw->pw_name) + 1);
	        if (buf != (char *) NULL)
	          sprintf(buf, format, pw->pw_name);
	    }
	    break;
	default:
	    buf = strdup("DEFAULT not impld");
	    break;
    }
    return buf;
}

/*
 * Name: DmxPrintJob::finishedPrintToFile
 * Description:
 */
void
DmxPrintJob::finishedPrintToFile(
			Display		*display,
			XPContext,
                        XPGetDocStatus  status,
                        XPointer        client_data)

{
    DmxPrintJob	*thisJob = (DmxPrintJob*) client_data;
    Widget	w = (Widget) thisJob->_parent->baseWidget();
    char	*message = NULL;

    if (status != XPGetDocFinished)
    {
	DtMailGenDialog *genDialog = new DtMailGenDialog("Dialog", w);

        message = (char *) GETMSG(
                                DT_catd, 21, 14,
                                "'Print to File'  was unsuccessful.");
        genDialog->setToErrorDialog(
                            GETMSG(DT_catd, 21, 3, "Mailer"),
                            message);
        genDialog->post_and_return(
                            GETMSG(DT_catd, 21, 4, "OK"),
                            NULL);
        delete genDialog;
    }
    else if (display != theRoamApp.getErrorPrintDisplay())
    {
	DtMailGenDialog *genDialog = new DtMailGenDialog("Dialog", w);

        message = (char *) GETMSG(
                                DT_catd, 21, 15,
                                "'Print to File' completed successfully");

        genDialog->setToWarningDialog(
                            GETMSG(DT_catd, 21, 3, "Mailer"),
                            message);
        genDialog->post_and_return(
                            GETMSG(DT_catd, 21, 4, "OK"),
                            NULL);
        delete genDialog;
    }
    thisJob->cancel();
}

/*
 * Name: DmxPrintJob::cancelCB
 * Description:
 *	An XtCallbackProc which can be added to the callback list of
 *	a widget to cancel the print job passed back as client_data.
 */
void
DmxPrintJob::cancelCB (Widget, XtPointer client_data, XtPointer)
{
    DmxPrintJob *thisJob = (DmxPrintJob *) client_data;

    thisJob->cancel();
}

/*
 * Name: DmxPrintJob::closeDisplayCB
 * Description:
 *	An XtCallbackProc which can be added to the callback list of
 *	a widget to react to the print setup closing the display.
 */
void
DmxPrintJob::closeDisplayCB (Widget, XtPointer client_data, XtPointer call_data)
{
    DmxPrintJob *thisJob = (DmxPrintJob *) client_data;
    DtPrintSetupCallbackStruct *pbs = (DtPrintSetupCallbackStruct *) call_data;

    if (thisJob->_pshell != NULL)
    {
	XtDestroyWidget(thisJob->_pshell);
	thisJob->_pshell = NULL;
    }
    DtPrintFreeSetupData(thisJob->_print_data);
}

/*
 * Name: DmxPrintJob::pdmNotificationCB
 * Description:
 *	 XmNpdmNotificationCallback for the XmPrintShell
 */
void
DmxPrintJob::pdmNotificationCB(
			Widget,
			XtPointer client_data,
			XtPointer call_data)
{
#if defined(PRINTING_SUPPORTED)
    DmxPrintJob			*thisJob = (DmxPrintJob *) client_data;
    XmPrintShellCallbackStruct  *pscbs = (XmPrintShellCallbackStruct*)call_data;
    char			*message = NULL;

    switch (pscbs->reason)
    {
    
        case XmCR_PDM_NONE:
        case XmCR_PDM_START_ERROR:
        case XmCR_PDM_START_VXAUTH:
        case XmCR_PDM_START_PXAUTH:
	    message = (char *) GETMSG(
			DT_catd, 21, 24,
			"Print Dialog Manager error - setup failed.");
	    break;
	default:
	    message = NULL;
	    break;
    }

    if (message != NULL)
    {
	DtMailGenDialog *genDialog = new DtMailGenDialog(
						"Dialog",
						thisJob->_parent->baseWidget());
        genDialog->setToErrorDialog(
                            GETMSG(DT_catd, 21, 3, "Mailer"),
                            message);
        genDialog->post_and_return(
                            GETMSG(DT_catd, 21, 25, "Continue"),
                            NULL);
        delete genDialog;
    }
#endif  /* PRINTING_SUPPORTED */
}

/*
 * Name: DmxPrintJob::pdmSetupCB
 * Description:
 *	An XtCallbackProc which can be added to the callback list of
 *	a widget to execute the print job passed back as client_data.
 */
void
DmxPrintJob::pdmSetupCB(
		Widget print_setup,
		XtPointer client_data,
		XtPointer call_data)
{
#if defined(PRINTING_SUPPORTED)
    DmxPrintJob *thisJob = (DmxPrintJob *) client_data;
    DtPrintSetupCallbackStruct *pbs = (DtPrintSetupCallbackStruct *) call_data;

    DtPrintCopySetupData(thisJob->_print_data, pbs->print_data);
    thisJob->createPrintShell();

    if (thisJob->_pshell)
    {
	Widget	shell = print_setup;

	while (! (shell == NULL || XtIsShell(shell)) )
	  shell = XtParent(shell);

	if (shell)
          XmPrintPopupPDM(thisJob->_pshell, shell);
	else
	  fprintf(stderr, "Internal Error:  Missing XmPrintShell\n");
    }
#endif  /* PRINTING_SUPPORTED */
}

/*
 * Name: DmxPrintJob::printCB
 * Description:
 *	An XtCallbackProc which can be added to the callback list of
 *	a widget to execute the print job passed back as client_data.
 */
void
DmxPrintJob::printCB (Widget, XtPointer client_data, XtPointer call_data)
{
    DmxPrintJob *thisJob = (DmxPrintJob *) client_data;
    DtPrintSetupCallbackStruct *pbs = (DtPrintSetupCallbackStruct *) call_data;

    /* TBD: Save options??? */

    DtPrintCopySetupData(thisJob->_print_data, pbs->print_data);
    thisJob->createPrintShell();
    thisJob->doPrint();
}
