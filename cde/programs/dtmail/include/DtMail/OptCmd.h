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
 *	$TOG: OptCmd.h /main/12 1999/07/13 08:42:43 mgreess $
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

///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//         This example code is from the book:
//
//           Object-Oriented Programming with C++ and OSF/Motif
//         by
//           Douglas Young
//           Prentice Hall, 1992
//           ISBN 0-13-630252-1	
//
//         Copyright 1991 by Prentice Hall
//         All Rights Reserved
//
//  Permission to use, copy, modify, and distribute this software for 
//  any purpose except publication and without fee is hereby granted, provided 
//  that the above copyright notice appear in all copies of the software.
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// OptCmd.h: Display the Mail Options dialog
////////////////////////////////////////////////////////////
#ifndef OPTCMD_H
#define OPTCMD_H

#include "PropUi.hh"
#include "DtVirtArray.hh"
#include "DmxPrintOptions.h"
#include "DtMailGenDialog.hh"
#include "MainWindow.h"
#include "MailRetrievalOptions.h"
#include "WarnNoUndoCmd.h"
#include "options_ui.h"

void vaca_msg_changedCB(Widget w, 
			XtPointer ClientData, 
			XtPointer calldata);
void options_set_category_pane(Widget pane);
void opt_help_button_CB(Widget wid, XtPointer clientData, XtPointer calldata);


class OptCmd: public WarnNoUndoCmd {
    
  protected:

    virtual void doit();      // Call exit
    DtbOptionsDialogInfo _opt_dlog;

  public:
    
    OptCmd ( char *, char *, int, Widget parent);
    virtual ~OptCmd();
    void create_dlog();
    void unmanage();
    virtual const char *const className () { return "OptCmd"; }
    virtual void execute(); // Overrides the AskFirstCmd member function

    void update_panes(); // sync up ui with hash tables
    void updateUiFromBackEnd(DtVirtArray<PropUiItem *> *list);

    void update_source();
    void updateBackEndFromUi(DtVirtArray<PropUiItem *> *list);

    int  doFileLockingCheck();
    void doVacationCheck();
    void setVacationChanged();
    void set_vaca_msg_Changed();
    Boolean startVacation();
    void stopVacation();
    void help_button_CB(Widget pane); 
    void setNewCategoryStr();
    void setOldCategoryStr();
    DtMailGenDialog *genDialog() { return generalDialog; }
    void setCurrentPane(Widget pane) { CurrentPane = pane; }
    Widget currentPane() { return CurrentPane; }

    void displayAliasesOptionsPane();
    void displayPrintingOptionsPane();
    Boolean optionsAreValid();
    void displayMailRetrievalOptionsPane();

  private:
    virtual void init_panes(); // hook up front and back ends

    virtual void init_msg_hdr_pane();
    virtual void init_msg_view_pane();
    virtual void init_compose_pane();
    virtual void init_msg_filing_pane();
    virtual void init_vacation_pane();
    virtual void init_template_pane();
    virtual void init_aliases_pane();
    virtual void init_advanced_pane();
    virtual void init_printing_pane();
    virtual void init_mailretrieval_pane();
    
    DtVirtArray<PropUiItem *> *mh_pane_list;
    DtVirtArray<PropUiItem *> *msg_view_pane_list;
    DtVirtArray<PropUiItem *> *compose_pane_list;
    DtVirtArray<PropUiItem *> *msg_filing_pane_list;
    DtVirtArray<PropUiItem *> *vacation_pane_list;
    DtVirtArray<PropUiItem *> *template_pane_list;
    DtVirtArray<PropUiItem *> *aliases_pane_list;
    DtVirtArray<PropUiItem *> *advanced_pane_list;
    DtVirtArray<PropUiItem *> *printing_pane_list;
    DtVirtArray<PropUiItem *> *mailretrieval_pane_list;
    
    Boolean vacation_status_changed;
    Boolean vacation_msg_changed;
    Boolean _reopen_mailboxes;

    XmString vaca_sub;
    XmString vaca_msg;
    XmString catstr;
    DtMailGenDialog *generalDialog;
    Widget CurrentPane;
    DmxPrintOptions *_PrintingOptions;
    MailRetrievalOptions *_MailRetrievalOptions;
};
#endif
