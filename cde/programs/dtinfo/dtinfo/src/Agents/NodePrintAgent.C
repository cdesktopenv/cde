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
/*	copyright (c) 1994,1995,1996 FUJITSU LIMITED	*/
/*	All Rights Reserved				*/

/*
 * $XConsortium: NodePrintAgent.C /main/21 1996/12/06 14:46:28 rcs $
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

#include <sstream>
using namespace std;

#define C_FString
#define C_List
#define C_NodeViewInfo
#define C_TOC_Element
#define L_Basic

#define C_Anchor

#define C_Agent
#define C_GraphicAgent
#define C_HelpAgent
#define C_NodeWindowAgent
#define C_NodePrintAgent
#define C_ListView
#define C_BookTab
#define C_MarkIcon
#define C_AnchorCanvas
#define C_MarkCanvas
#define C_Launcher
#define C_ScopePopup
#define C_UrlAgent
#define C_IcccmAgent
#define C_PrintPanelAgent
#define L_Agents

#define C_WindowGeometryPref
#define L_Preferences

#define C_PrintMgr
#define C_MessageMgr
#define C_SearchMgr
#define C_SearchResultsMgr
#define C_NodeMgr
#define C_SearchScopeMgr
#define C_PrefMgr
#define C_MarkMgr
#define C_MapMgr
#define C_GlobalHistoryMgr
#define C_NodeParser
#define C_GhostScriptEventMgr
#define C_GraphicsMgr
#define C_UrlMgr
#define L_Managers


#define C_Path
#define C_Node
#define C_TOC
#define C_Tab
#define L_Doc

#define C_xList
#define L_Support

#define C_PixmapGraphic
#define L_Graphics

#define USES_OLIAS_FONT

#if 0
#  define RCS_DEBUG(statement) cerr << statement << endl
#else
#  define RCS_DEBUG(statement) 
#endif

#include "Prelude.h"

#include "Managers/CatMgr.hh"
#include "Registration.hh"

// for DtCanvas Help stuff
#include <DtI/Access.h>
#include <DtI/XUICreateI.h>
#include <DtI/SetListI.h>
#include <DtI/CallbacksI.h>
#include <DtI/ActionsI.h>

#include <DtI/DisplayAreaP.h>
#include <DtI/DisplayAreaI.h>
#include <DtI/FontI.h>
#include <DtI/FontAttrI.h>
#include <DtI/RegionI.h>

#include <WWL/WTopLevelShell.h>
#include <WWL/WXmPushButton.h>
#include <WWL/WXmCascadeButtonGadget.h>
#include <WWL/WXmForm.h>
#include <WWL/WXmRowColumn.h>
#include <WWL/WXmMenu.h>
#include <WWL/WXmFrame.h>
#include <WWL/WXmLabel.h>
#include <WWL/WXmToggleButton.h>
#include <WWL/WXmArrowButton.h>
#include <WWL/WXmSeparator.h>
#include <WWL/WXmTextField.h>
#include <WWL/WXmDrawingArea.h>
#include <Xm/Text.h>

#include <ctype.h>

#if defined(sun)
#if defined(SVR4)
#define SunOS5
#else
#define SunOS4
#endif
#endif

#if defined(Internationalize) && defined(SunOS5)
#include <libintl.h>
#endif

//-------- External References ---------------------------------

// When updating for the purposes of changing style sheets
extern bool g_style_sheet_update ;

// exporting the display area so that the CanvasRenderer has
// access to it to load the fonts 
extern DtHelpDispAreaStruct *gHelpDisplayArea ;

// print margins


extern int left_margin;
extern int right_margin;
extern int top_margin;
extern int bottom_margin;


//-------- Constants ----------------------------------------------

#define f_node_ptr f_node_view_info->node_ptr()

//-------- Global variables -------------------------------------

Widget footer_form, footer_book_name, footer_page_no, f_page;

//------- Local static variables ---------------------------------

static char another_g_top_locator[4096] ;

/*
 * DtHelpDialog widget action list - required by the DisplayArea
 */
static XtActionsRec DrawnBActions[] =
  {
    {(char*)"DeSelectAll"    , (DeSelectAll_ptr)_DtHelpDeSelectAll         },
    {(char*)"SelectAll"      , (SelectAll_ptr)_DtHelpSelectAll             },
    {(char*)"ActivateLink"   , (ActivateLink_ptr)_DtHelpActivateLink       },
    {(char*)"CopyToClipboard", (CopyAction_ptr)_DtHelpCopyAction           },
    {(char*)"PageUpOrDown"   , (PageUpOrDown_ptr)_DtHelpPageUpOrDown       },
    {(char*)"PageLeftOrRight", (PageLeftOrRight_ptr)_DtHelpPageLeftOrRight },
    {(char*)"NextLink"       , (NextLink_ptr)_DtHelpNextLink               }
  };

//-------  Functions --------------------------------------------

DtHelpDispAreaStruct *
_DtHelpCreatePrintArea(Widget parent,
		       char *name,
		       Dimension width,
		       Dimension height)
{
    extern AppPrintData * l_AppPrintData;
    DtHelpDispAreaStruct * DisplayArea;
    String string_resolution;
    int resolution;

    //  get print resolution from default-printer-resolution
    
    string_resolution =  
	XpGetOneAttribute(XtDisplay(parent), l_AppPrintData->f_print_data->print_context,
			  XPDocAttr, (char*)"default-printer-resolution");
    resolution = atoi(string_resolution);
    XFree(string_resolution);

    // if not printing, or default-printer-resolution not defined, calcuate from 
    // the X screen resolution

    if (!resolution) {

	// calculate the printer resolution in dots per inch.
	// 25.4mm = 1 inch
	resolution = 
	    (254 * WidthOfScreen(window_system().screen()) +
	     5 * WidthMMOfScreen(window_system().screen())) /
	    (10 * WidthMMOfScreen(window_system().screen()));
    }
	
    DisplayArea = (DtHelpDispAreaStruct*)_DtHelpCreateOutputArea(
        parent, 
	name,
	_DtHelpNONE, 
	_DtHelpNONE,
	False,
	_DtCvUSE_BOUNDARY,
	_DtCvRENDER_COMPLETE,
	width, 
	height, 
	resolution,
	NULL, NULL, NULL, NULL, NULL);

    // fix display area resource setting 

    DisplayArea->decorThickness = 0;
    XtVaSetValues(DisplayArea->dispWid,
		  XmNhighlightThickness, 0,
		  XmNshadowThickness, 0, NULL, NULL);

    return DisplayArea;
}

#undef CLASS
#define CLASS NodePrintAgent

// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

NodePrintAgent::NodePrintAgent()
    :  f_node_view_info (NULL), f_help_dsp_area(NULL)
{
}

// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

NodePrintAgent::~NodePrintAgent()
{

    RCS_DEBUG("PrintPanelAgent destructor called.\n");     

    // clean things up so that the help display area deletion
    // will not stomp on the memory we free when deleting the nodeviewinfo

    _DtCanvasDestroy(f_help_dsp_area->canvas);
    f_help_dsp_area->canvas = NULL ;
    
    delete f_node_view_info;

    f_node_view_info = NULL;
    
    f_help_dsp_area->canvas = NULL ;

    gHelpDisplayArea = 0;

    RCS_DEBUG("PrintPanelAgent destructor exiting.\n");     

}

// /////////////////////////////////////////////////////////////////
// create_ui - create the user interface
// /////////////////////////////////////////////////////////////////

DtHelpDispAreaStruct *
NodePrintAgent::create_ui(Widget parent)
{
    RCS_DEBUG("NodePrintAgent::create_ui entered.\n");

    Arg	args[15];
    int n;
    Dimension width, height;
    XmString label;
    char buf[6]; // good enuf for 99,999 pages!
    
    // If we haven't created the form yet we haven't figured
    // out the page size yet so go figure out the page size.
    
    width = 0; height=0;
    XtVaGetValues(
	f_print_shell,
	XmNwidth, &width,
	XmNheight, &height,
	NULL);
    
    // Setup up an outer form to hold the different page elements 

    n = 0;
    f_form = XmCreateForm(parent, (char*)"outer_page", NULL, n);

    //  make sure the DtHelpDialog's actions are declared in order to satisfy
    //  the need of the DisplayArea 

    XtAppAddActions(XtWidgetToApplicationContext(f_form),
		    DrawnBActions, XtNumber(DrawnBActions));

 
    // Create form for page footer 

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++; // bottom
    footer_form = XmCreateForm(f_form, (char*)"footer", args, n);

    // Create a left attached label for the book name

    label = XmStringCreate((char*)"", XmFONTLIST_DEFAULT_TAG);
    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNlabelString, label); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    footer_book_name = XmCreateLabel(footer_form, (char*)"bookname", args, n);
    XmStringFree(label);

    XtManageChild(footer_book_name);
    
    // Create a right attached label for the book number

    snprintf(buf, sizeof(buf), "%d", 1);
    label = XmStringCreate(buf, XmFONTLIST_DEFAULT_TAG);
    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNlabelString, label); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    footer_page_no = XmCreateLabel(footer_form, (char*)"page_no", args, n);
    XmStringFree(label);

    XtManageChild(footer_page_no);
    XtManageChild(footer_form);

    f_help_dsp_area = _DtHelpCreatePrintArea(f_form, (char*)"DisplayArea", width, height);
    gHelpDisplayArea = f_help_dsp_area ;

    //  Setup Display Area attachments 
    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++; 
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;  // bottom

    XtSetArg(args[n], XmNbottomWidget, footer_form); n++;  // bottom
    XtSetValues(XtParent(f_help_dsp_area->dispWid), args, n);

    RCS_DEBUG("NodePrintAgent::create_ui exiting.\n");

    return(f_help_dsp_area); 
}

// /////////////////////////////////////////////////////////////////
// display
// /////////////////////////////////////////////////////////////////
void
NodePrintAgent::display(UAS_Pointer<UAS_Common> &node_ptr)
{
    Arg	args[15];
    int n;
    XmString label;
    char buf[6];  // good enuf for 99,999 pages!
    int margin;

    RCS_DEBUG("NodePrintAgent::display called.\n");

    bool first_time = False;

    // if f_help_dsp_area is NULL then this is the first time through
    // if gHelpDisplayArea is 0 it has been reset because a new print
    // server was selected.  in either case we need to create a new
    // ui

    if ((f_help_dsp_area == NULL) || (gHelpDisplayArea == 0)) {
	create_ui(f_print_shell);
	first_time = TRUE;
    }
    
    // update book name (long title) in footer
    
    n = 0;
    label = XmStringCreate(node_ptr->book_name(UAS_LONG_TITLE), XmFONTLIST_DEFAULT_TAG);
    
    XtSetArg(args[n], XmNlabelString, label); n++;
    XtSetValues(footer_book_name, args, n);
    XmStringFree(label);
    
    // update page number in footer
    
    n = 0;
    snprintf(buf, sizeof(buf), "%d", 1);
    label = XmStringCreate(buf, XmFONTLIST_DEFAULT_TAG);
    
    XtSetArg(args[n], XmNlabelString, label); n++;
    XtSetValues(footer_page_no, args, n);
    XmStringFree(label);

    // load section 
    
    f_node_view_info = print_mgr().load(node_ptr);

    _DtCvTopicInfo *topic = f_node_view_info->topic();
    
    if (another_g_top_locator[0] != 0) {
	topic->id_str = another_g_top_locator ;
    }
    
    SetTopic(f_node_view_info->topic());
    another_g_top_locator[0] = 0;

    // set margins on the footer form containing both 
    // the book title and the page number

    n = 0;
    XtSetArg(args[n], XmNleftOffset, left_margin); n++;
    XtSetArg(args[n], XmNrightOffset, right_margin); n++;

    // offset bottom by fixed offset of 1/4 inch or 18 pts

    XtSetArg(
	args[n], 
	XmNbottomOffset, 
	XmConvertUnits(window_system().printToplevel(), XmVERTICAL, XmPOINTS, 18, XmPIXELS)
	); n++;
	
    XtSetValues(footer_form, args, n);

    // if the bottom margin is larger than 18 points, subtract 18 points from the margin
    // and used the reduced size

    margin = XmConvertUnits(
	window_system().printToplevel(), XmVERTICAL, XmPIXELS, bottom_margin, XmPOINTS);

    if (margin > 18) {
	margin = margin - 18;

	margin = XmConvertUnits(
	    window_system().printToplevel(), XmVERTICAL, XmPOINTS, margin, XmPIXELS);
    }
   
    // if bottom margin smaller than 18 points just use the entire margin

    else {
	margin = bottom_margin;
    }

    // set offsets for margins on the canvas 

    n = 0;
    XtSetArg(args[n], XmNleftOffset, left_margin); n++;
    XtSetArg(args[n], XmNrightOffset, right_margin); n++;
    XtSetArg(args[n], XmNtopOffset, top_margin); n++;
    XtSetArg(args[n], XmNbottomOffset, margin); n++;
    XtSetValues(f_help_dsp_area->dispWid, args, n);
    
    if (first_time == TRUE) {
	XtManageChild(f_form);
    }

    RCS_DEBUG("NodePrintAgent::display exiting.\n");   
}

void
NodePrintAgent::SetTopic(_DtCvTopicPtr topic)
{
  
  //   This routine was paraphrased from _DtHelpDisplayAreaSetList

  _DtCvUnit ret_width, ret_height, ret_y;

  _DtHelpClearSelection(f_help_dsp_area);

  _DtCvStatus status;
  status = _DtCanvasSetTopic(f_help_dsp_area->canvas, topic, 
			     f_help_dsp_area->honor_size,
			     &ret_width, &ret_height, &ret_y);

  if (status != _DtCvSTATUS_OK) {
      cerr << "(ERROR) _DtCanvasSetTopic failed, error# " << status << endl;
  }

  f_help_dsp_area->lst_topic = topic;

  f_help_dsp_area->maxX = ret_width ;
  f_help_dsp_area->maxYpos = ret_height + f_help_dsp_area->marginHeight ;
  f_help_dsp_area->firstVisible = ret_y ;

  if (f_help_dsp_area->firstVisible + ((int)f_help_dsp_area->dispUseHeight) > f_help_dsp_area->maxYpos) {
      f_help_dsp_area->firstVisible = f_help_dsp_area->maxYpos - f_help_dsp_area->dispUseHeight;
  }
  
  if (f_help_dsp_area->firstVisible < 0) {
      f_help_dsp_area->firstVisible = 0; 
  }
  
  if (XtIsRealized (f_help_dsp_area->dispWid)) {
      XClearArea(XtDisplay(f_help_dsp_area->dispWid), XtWindow(f_help_dsp_area->dispWid),
		 0, 0, 0, 0, True);
  }
}
