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
 *	$TOG: DtMailGenDialog.hh /main/7 1998/02/04 18:38:09 mgreess $
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

//////////////////////////////////////////////////////////
// DtMailGenDialog.hh: A generic dialog based on MessageBox
//////////////////////////////////////////////////////////
#ifndef DTMAILGENDIALOG_HH
#define DTMAILGENDIALOG_HH

#include <stdlib.h>
#include <Xm/TextF.h>
#include "UIComponent.h"
#include "DtMailDialogCallbackData.hh"

class DtMailGenDialog : public UIComponent {
    
  private:
    int		_info_dialog;
    Widget	_otherWidget;
    Widget	_parentshell;

    Widget	_textField;
    int		_shroudText;
    int		_maxTextlen;
    char	*_clearText;

    void cleanup ( Widget, DtMailDialogCallbackData* );

  protected:

    void forceUpdate( Widget );
    
    static void okCallback(Widget, XtPointer, XtPointer);
    static void cancelCallback(Widget, XtPointer, XtPointer);
    static void helpCallback(Widget, XtPointer, XtPointer);
    static void otherCallback(Widget, XtPointer, XtPointer);

    static void verifyCallback(Widget, XtPointer, XtPointer);
    void	verify(XmTextVerifyPtr cbs);

    void setDialog(char * title, char * text, unsigned char type);

  public:
    
    DtMailGenDialog(char*,Widget, int style=XmDIALOG_PRIMARY_APPLICATION_MODAL);
    ~DtMailGenDialog() { if (NULL != _clearText) free(_clearText); };
    
    char	*getTextFieldValue(void);

    void	setToTextFieldDialog(char *, char *, int shroud = FALSE);
    void	setToQuestionDialog(char *, char *);
#ifdef DEAD_WOOD
    void	setToInfoDialog(char *, char *);
#endif /* DEAD_WOOD */
    void	setToErrorDialog(char *, char *);
    void	setToWarningDialog(char *, char *);
    void	setToAboutDialog(void);

    virtual Widget post (
			 void *clientData      = NULL,
			 DialogCallback ok     = NULL,
			 DialogCallback cancel = NULL,
			 DialogCallback other  = NULL,
			 DialogCallback help   = NULL,
			 char *helpId          = NULL);
    virtual int	   post_and_return(char *);
    virtual int	   post_and_return(char *, char *);
    virtual int	   post_and_return(char *, char *, char *);
    virtual int	   post_and_return(char *, char *, char *, char *);
};
#endif
