/*
 * $XConsortium: PrintMgr.C /main/15 1996/12/06 14:41:31 rcs $
 *
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
 * All  $(MISC_LIB)rights reserved.  Unpublished -- rights reserved under
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

#include <X11/Xlib.h>

#define C_xList
#define C_Support

#define C_TOC_Element
#define C_NodeViewInfo
#define L_Basic

#define C_PrintPanelAgent
#define C_NodePrintAgent
#define L_Agents

#define C_PrintMgr
#define C_PrefMgr
#define C_NodeMgr
#define C_NodeWindowMgr
#define C_StyleSheetMgr
#define C_LibraryMgr
#define L_Managers

#include "Prelude.h"

#include "Managers/CatMgr.hh"
#include "Registration.hh"

#include "DocParser.h"
#include "Resolver.h" 
#include "StyleSheet.h"
#include "StyleSheetExceptions.h"
#include "../OnlineRender/CanvasRenderer.hh"

#if 0
#  define RCS_DEBUG(statement) cerr << statement << endl
#else
#  define RCS_DEBUG(statement) 
#endif

extern "C" { const char *unique_id(void); }

LONG_LIVED_CC(PrintMgr,print_mgr);

class PrintNode : public UAS_Receiver<UAS_DocumentRetrievedMsg>
{
public:
    PrintNode()
    { 
	UAS_Common::request ((UAS_Receiver<UAS_DocumentRetrievedMsg> *) this); 
    }
private:
    void receive(UAS_DocumentRetrievedMsg &message, void *client_data);
};

void
PrintNode::receive (UAS_DocumentRetrievedMsg &message, void *client_data)
{
    RCS_DEBUG("PrintNode::receive called.");

    int cd = (int)client_data;

    //  1 == print request
    if (cd == 1) {
	print_mgr().print(message.fDoc);
    }
    else {
	RCS_DEBUG("PrintNode::receive exiting -- not a print request.");
	return;
    }
    
    RCS_DEBUG("PrintNode::receive exiting.");
    return;
}

static PrintNode node_printer;

// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

PrintMgr::PrintMgr()
    : f_print_panel(NULL), f_npa(NULL), f_font_scale(0)
{
    RCS_DEBUG("PrintMgr::PrintMgr called.");
    set_font_scale(pref_mgr().get_int(PrefMgr::FontScale));
    RCS_DEBUG("PrintMgr::PrintMgr exiting.");
}

// /////////////////////////////////////////////////////////////////
// print - Just do it.
// /////////////////////////////////////////////////////////////////


void
PrintMgr::print(UAS_Pointer<UAS_Common> &node_ptr)
{
    Wait_Cursor bob;
    f_last_printed = node_ptr;
    
    RCS_DEBUG("PrintMgr::print called.");

    if (f_npa == NULL) {
	f_npa = new NodePrintAgent();
    }

    f_npa->display(node_ptr);

    RCS_DEBUG("PrintMgr::print exiting.");
}


/*
 * ------------------------------------------------------------------------
 * Name: PrintMgr::destroy_agent
 *
 * Description:
 *
 *     Deletes the print agent so that the next print doesn't hang.
 *
 */
void
PrintMgr::destroy_agent()
{
    RCS_DEBUG("PrintMgr::destroy_agent called.");

    delete f_npa;
    f_npa = NULL;    

    RCS_DEBUG("PrintMgr::destroy_agent exiting.");
}

void
PrintMgr::set_font_scale(int value)
{
    int real_value = value * 2 ;
    if (real_value < - MIN_DOWNSCALE) {
	real_value = - MIN_DOWNSCALE ;
    }
    
    int change_amount = real_value - f_font_scale ;
    if (change_amount == 0) {
	return ;
    }
    
    // save the new font scale
    f_font_scale = real_value ;
    
}



NodeViewInfo *
PrintMgr::load(UAS_Pointer<UAS_Common> &node_ptr)
{
  static int first = 0;

  extern int styleparse();
  extern void stylerestart(FILE *);
  extern NodeViewInfo *gNodeViewInfo;

  try
    {
      style_sheet_mgr().initPrintStyleSheet(node_ptr);
    }
  catch_noarg (StyleSheetSyntaxError)
    {
      message_mgr().error_dialog(
	  (char*)UAS_String(CATGETS(Set_Messages, 39, 
    "There is an error in the Style Sheet for this Section.\nDefault features have been applied.")
	      ));
    }
  end_try ;

  istrstream input((char *) node_ptr->data());

  try
    {
      window_system().setPrinting(True);
      // assign node_ptr to global variable that TmlRenderer can pick up 
      gNodeViewInfo = new NodeViewInfo(node_ptr);
      CanvasRenderer renderer (pref_mgr().get_int(PrefMgr::FontScale)) ;
      Resolver resolver(*gPathTab, renderer);
      DocParser docparser(resolver);
      docparser.parse(input);
      window_system().setPrinting(False);
    }
  catch_any()
    {
      ON_DEBUG(cerr << "PrintMgr::load...exception thrown" << endl);
      delete gNodeViewInfo ;
      gNodeViewInfo = 0;
      rethrow;
    }
  end_try;

  // TmlRenderer set this up for us 
  return gNodeViewInfo ;
}

