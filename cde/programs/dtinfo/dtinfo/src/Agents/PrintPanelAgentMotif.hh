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
 * $XConsortium: PrintPanelAgentMotif.hh /main/10 1996/10/04 17:50:50 rswiston $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */

#include <Xm/XmAll.h>
#if 0 && defined(PRINTING_SUPPORTED)
#include <Xm/Print.h>
#endif /* PRINTING_SUPPORTED */
#include <Dt/Print.h>

#include "UAS.hh"

#ifndef DtNsetupCallback
#   define DtNsetupCallback "setupCallback"
#endif

class WTopLevelShell;

#include <WWL/WXmForm.h>
#include <WWL/WXmLabel.h>
#include <WWL/WXmToggleButton.h>
#include <WWL/WXmPushButton.h>
#include <WWL/WXmTextField.h>
#include <WWL/WXmFileSelectionBox.h>

// get pid_t typedef 
#ifdef SVR4
#   include <sys/types.h>
#endif

// This belongs in f'ing config.h
#if defined(SVR4) || defined(hpux)
#   define pid_type pid_t 
#else
#   define pid_type int
#endif

//  global variable for print shell

extern Widget f_print_shell;

template <class T> class xList;

// callbacks to support X Print Service integration

void PrintCB(Widget, XtPointer, XtPointer);
void StartJobCB(Widget, XtPointer, XtPointer);
void PdmNotifyCB(Widget, XtPointer, XtPointer);
void PrintSetupCB(Widget, XtPointer, XtPointer);
void ToggleWhatCB(Widget, XtPointer, XtPointer);


// new X print service functions

void customizePrintSetupBox(AppPrintData * p);
void DoPrint(Widget widget, AppPrintData * p);
void Print(AppPrintData *p);
void PrintCloseDisplayCB(Widget, XtPointer, XtPointer);
#if 0 && defined(PRINTING_SUPPORTED)
void FinishPrintToFile(Display*, XPointer);
#endif /* PRINTING_SUPPORTED */
void CreatePrintShell(Widget, AppPrintData*);
void CreatePrintSetup(Widget parent, AppPrintData* p);

class PrintPanelAgent : public Agent
{
public:
    
    // Constructors
    
    PrintPanelAgent();
    PrintPanelAgent(AppPrintData *);
    
    // Destructor
    
    ~PrintPanelAgent();
    
    void display(WTopLevelShell &source_window,
		 xList<UAS_Pointer<UAS_Common> > &print_list);
    
private: // functions 
    
    void create_ui();
    void reset_ui();
    
    /* -------- callbacks -------- */
    
    void cancel();
    void print();
    void ok();
    
    void print_to_file_toggle(WCallback *);
    void sensitize_ok (WXmTextField field);
    
    bool can_write(char *filename);
    
};
