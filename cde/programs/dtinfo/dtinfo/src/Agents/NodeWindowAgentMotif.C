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
/* $TOG: NodeWindowAgentMotif.C /main/94 1998/04/17 11:34:46 mgreess $ */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1994, 1995, 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
/*
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
#include <iostream>
using namespace std;

#define C_FString
#define C_List
#define C_NodeViewInfo
#define C_TOC_Element
#define L_Basic

#define C_Anchor

#define C_Agent
#define C_GraphicAgent
#define C_PrintPanelAgent
#define C_HelpAgent
#define C_NodeWindowAgent
#define C_ListView
#define C_BookTab
#define C_MarkIcon
#define C_AnchorCanvas
#define C_MarkCanvas
#define C_ScopeMenu
#define C_UrlAgent
#define C_IcccmAgent
#define C_BookmarkEdit
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
#define C_LibraryMgr
#define C_EnvMgr
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

#include "Managers/CatMgr.hh"
#include "Managers/WString.hh"
#include "Other/XmStringLocalized.hh"

#include "Marks/MarkInfo.hh"

#include <X11/cursorfont.h>

#include "Prelude.h"

#include "Registration.hh"

// for DtCanvas Help stuff
#include <DtI/XUICreateI.h>
#include <DtI/SetListI.h>
#include <DtI/CallbacksI.h>
#include <DtI/DisplayAreaI.h>
#include <DtI/FontI.h>
#include <DtI/FontAttrI.h>
#include <DtI/RegionI.h>

#include "OnlineRender/SegClientData.hh"

#include "utility/mmdb_exception.h"

#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/PushBG.h>
#include <Xm/PushB.h>
#include <Xm/ToggleBG.h>
#include <Xm/ToggleB.h>
#include <Xm/CascadeBG.h>
#include <Xm/SeparatoG.h>
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
#include <WWL/WXmDialogShell.h>
#include <WWL/WXmMessageBox.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>

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


#if defined(UseWideChars)
# if defined(SunOS4)
#   define mbstowcs(a,b,c) Xmbstowcs(a,b,c)
#   define wcstombs(a,b,c) Xwcstombs(a,b,c)
# elif defined(_IBMR2)
#   include <wcstr.h>
# endif
#endif

#define ON_ACTIVATE(WOBJ,FUNC) \
  (WOBJ).SetActivateCallback (this, (WWL_FUN) &CLASS::FUNC)
#define ON_ARM(WOBJ,FUNC) \
  (WOBJ).SetArmCallback (this, (WWL_FUN) &CLASS::FUNC)
#define ON_DISARM(WOBJ,FUNC) \
  (WOBJ).SetDisarmCallback (this, (WWL_FUN) &CLASS::FUNC)

#define f_node_ptr f_node_view_info->node_ptr()
  
xList<UAS_Pointer<UAS_Common> > g_tab_list;

extern AppPrintData * l_AppPrintData; 

#ifdef CV_HYPER_DEBUG
void
#else
static void
#endif
hypertext_cb (DtHelpDispAreaStruct* pDAS, NodeWindowAgent *agent, 
	      DtHelpHyperTextStruct *callData)
{
#if defined(_IBMR2) && defined(CV_HYPER_BUG)
  _DtCvLinkInfo ceHyper ;
  
  _DtCvUnit downX, downY, upX, upY ;

  downX = upX = pDAS->timerX ;
  downY = upY = pDAS->timerY ;

  if (_DtCvSTATUS_OK ==
      _DtCanvasGetPosLink (pDAS->canvas, downX, downY, upX,
			   upY, &ceHyper))
    {
#ifdef CV_HYPER_DEBUG
      cerr << "hypertext callback(" << callData->window_hint << "): " <<
        ceHyper.specification << endl; 
#endif
      agent->link_to(ceHyper.specification);
    }
#else

  if (callData->window_hint == 0) {
    if (callData->hyper_type < 0) {
      UAS_Pointer<Mark>& mark =
		((MarkCanvas*)(callData->specification))->mark_ptr();
#ifdef CV_HYPER_DEBUG
      cerr << "hypertext callback(" << callData->window_hint << "): " <<
	mark->name() << endl; 
#endif
      mark->edit();
    }
    else {
#ifdef CV_HYPER_DEBUG
      cerr << "hypertext callback(" << callData->window_hint << "): " <<
	callData->specification << endl; 
#endif
      agent->link_to(callData->specification);
    }
  }
#endif
}

static void
resize_cb (NodeWindowAgent *agent)
{
  agent->canvas_resize ();
}

static void
h_scroll_callback (Widget,
		   XtPointer client_data,
		   XtPointer call_data)
{
  NodeWindowAgent *agent = (NodeWindowAgent*)client_data ; 
  agent->hscroll (((XmScrollBarCallbackStruct*) call_data)->value) ;
}

static void
v_scroll_callback (Widget,
		   XtPointer client_data,
		   XtPointer call_data)
{
#ifdef SCROLL_BOOKMARK_DEBUG
  cerr << "our callback" << endl;
#endif
  NodeWindowAgent *agent = (NodeWindowAgent*)client_data ; 
  agent->vscroll (((XmScrollBarCallbackStruct*) call_data)->value) ;
}

static void 
v_scroll_notify(void *client_data, unsigned int value)
{
  ((NodeWindowAgent*)client_data)->vscroll (value);
}

static void
detach_grCB (Widget, XtPointer client_data, XtPointer )
{
  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  agent->detach_gr();
}

static void
raise_grCB (Widget w, XtPointer client_data, XtPointer )
{
  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  agent->raise_gr();
}

static void
attach_grCB (Widget, XtPointer client_data, XtPointer )
{
  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  agent->attach_gr();
}

void
NodeWindowAgent::attach_gr()
{
  _DtCvSegment *seg ;
  XtVaGetValues(f_detach_button, XmNuserData, &seg, NULL);
  SegClientData* pSCD = (SegClientData*)(seg->client_use);
  UAS_Pointer<Graphic> gr = (Graphic*)pSCD->GraphicHandle();
  //GraphicAgent *agent = gr->get_agent();
  //GraphicAgent *agent = graphics_mgr().get_agent(gr->locator());

  if (gr->is_detached())
  {
    //agent->attach_graphic();
    graphics_mgr().reattach_graphic(gr);
  }
}

void
NodeWindowAgent::detach_gr()
{
  _DtCvSegment *seg ;
  XtVaGetValues(f_detach_button, XmNuserData, &seg, NULL);

  SegClientData* pSCD = (SegClientData*)(seg->client_use);
  UAS_Pointer<Graphic> gr = (Graphic*)pSCD->GraphicHandle();
  if (!gr->is_detached())
    graphics_mgr().detach (f_node_view_info->node_ptr(), gr);
}

void
NodeWindowAgent::raise_gr()
{
  _DtCvSegment *seg ;
  XtVaGetValues(f_detach_button, XmNuserData, &seg, NULL);
  SegClientData* pSCD = (SegClientData*)(seg->client_use);
  UAS_Pointer<Graphic> gr = (Graphic*)pSCD->GraphicHandle();
  //GraphicAgent *agent = gr->get_agent();
  UAS_String locator_str = gr->locator();
  GraphicAgent *agent = graphics_mgr().get_agent(locator_str);
  agent->popup();
}

static void
popup_menuCB (Widget, XtPointer client_data, XEvent *event, Boolean*)
{
  XButtonPressedEvent *pevent = (XButtonPressedEvent*)event; 

  if (pevent->type != ButtonPress || pevent->button != Button3)
    return;

  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  agent->popup_menu(pevent);
}

static void
go_to_linkCB (Widget, XtPointer client_data, XtPointer )
{
  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  agent->go_to_link();
}

static void
open_new_nodeCB (Widget, XtPointer client_data, XtPointer )
{
  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  agent->open_new_node();
}

// Display a section in current node window
void
NodeWindowAgent::go_to_link()
{
    link_to((char*)f_link_spec);
    f_link_spec = NULL;
}

// Display a section in a new Node Window
void
NodeWindowAgent::open_new_node()
{
  node_mgr().force_new_window();
  link_to((char*)f_link_spec);
  f_link_spec = NULL;
}

void
NodeWindowAgent::exit_cb()
{
  if (BookmarkEdit::modified_count() > 0)
    {
      bool resp;
      resp = message_mgr().question_dialog (
                (char*)UAS_String(CATGETS(Set_Messages, 2,
                "Do you want to abandon your changes?")), (Widget)f_shell);
      if (! resp)
        return;
    }
 
  message_mgr().quit_dialog (
        (char*)UAS_String(CATGETS(Set_Messages, 6,
        "Do you really want to quit Dtinfo?")), (Widget)f_shell);

}

// pop up one of three menus:
// 1. If cursor is over a graphic, popup detach graphic menu.
// 2. If cursor is over hyperlink, popup link preview menu.
// 3. Else popup default menu.
void
NodeWindowAgent::popup_menu(XButtonPressedEvent* event)
{
  _DtCvSegment *seg;
  _DtCvElemType seg_type;
  seg = xy_to_seg(event->x, event->y, &seg_type);

//  if (seg == NULL)
//    return;

  // see if it's the graphic detach menu to popup
  switch(seg_type)
  {
    case _DtCvREGION_TYPE:
    {
      SegClientData* pSCD =
                (SegClientData*)(seg->client_use);
      UAS_Pointer<Graphic> gr = (Graphic*)pSCD->GraphicHandle();
      if (gr->is_detached())
      {
        XtSetSensitive(f_attach_button, True);
        XtSetSensitive(f_raise_button, True);
        XtSetSensitive(f_detach_button, False);
      }
      else
      {
        XtSetSensitive(f_attach_button, False);
        XtSetSensitive(f_raise_button, False);
        XtSetSensitive(f_detach_button, True);
      }
      // the userdata for the detach buton is used to transfer
      // the canvas segment to the detach, attach, raise callbacks.
      XtVaSetValues(f_detach_button, XmNuserData, seg, NULL);
      XmMenuPosition(f_detach_menu, event);
      XtManageChild(f_detach_menu);

      char* gr_type = (char*)gr->content_type();
      gr_type = strchr(gr_type,'/');
      if(gr_type)
        gr_type++;

      char buff[40];
      sprintf(buff,"%s graphic",gr_type);
      XmTextFieldSetString(f_status_text, buff);
      break;

    }
    case _DtCvSTRING_TYPE:
    {
      _DtCvLinkInfo ceHyper ;
      _DtCvUnit downX, downY, upX, upY ;

      downX = upX = event->x;
      downY = upY = event->y;

      if (XtIsManaged(f_help_dsp_area->vertScrollWid))
      {
        downY += f_vscrollbar_offset; // adjust y-pos for scroll bar
        upY += f_vscrollbar_offset; // adjust y-pos for scroll bar
      }

      if (_DtCvSTATUS_OK ==
          _DtCanvasGetPosLink (f_help_dsp_area->canvas,
                               downX, downY,
                               upX, upY,
                               &ceHyper))
      {
        f_link_spec = ceHyper.specification;
        UAS_Pointer<UAS_Common> doc_ptr =
           f_node_view_info->node_ptr()->create_relative((char*)f_link_spec);
        // NOTE: create_relative may have failed if the infolib associated
        //       with spec had been removed. So check doc_ptr before
        //       calling preview_init
        if (doc_ptr)
        {
          //preview_init(doc_ptr);
          char title[128];
          char preview_buffer[256];
 
          UAS_String pt = doc_ptr->title();
          strncpy (title, (char *) pt, 127);
          title[127] = '\0';
          UAS_String bn = doc_ptr->book_name(UAS_SHORT_TITLE);
          const char *book_name = (char *) bn;
          if (book_name != NULL && *book_name != '\0')
            sprintf (preview_buffer, CATGETS(Set_Messages, 8, "Link to %s: %s"),
                     book_name, title);
          else
            sprintf (preview_buffer, CATGETS(Set_Messages, 9, "Link to %s"),
                     title);
          WXmLabel lb = WXmLabel(f_preview_label);
          lb.LabelString(WXmString(title));
  
          XmTextFieldSetString(f_status_text, preview_buffer);
          XmMenuPosition(f_preview_menu, event);
          XtManageChild(f_preview_menu);
        }
      }
      else
      {
        XmMenuPosition(f_default_menu, event);
        XtManageChild(f_default_menu);
      }
      break;
    }
    default:
      XmProcessTraversal(f_help_dsp_area->dispWid, XmTRAVERSE_CURRENT);
      XmMenuPosition(f_default_menu, event);
      XtManageChild(f_default_menu);
      break;
  }
}

// Given an x,y coordinate, return the segment. The segment
// type that can be returned is a _DtCvREGION_TYPE or a
// _DtCvSTRING.
//
_DtCvSegment *
NodeWindowAgent::xy_to_seg(int x, int y, _DtCvElemType *element)
{
  _DtCvStatus status;
  _DtCvSegment *seg;
  _DtCvUnit offx, offy;
  int xpos, ypos;

  ypos = y;
  xpos = x;

  if (XtIsManaged(f_help_dsp_area->vertScrollWid))
    ypos += f_vscrollbar_offset; // adjust y-pos for scroll bar
  if (XtIsManaged(f_help_dsp_area->horzScrollWid))
    xpos += f_hscrollbar_offset; // adjust x-pos for scroll bar

  status = _DtCanvasGetSpotInfo(f_help_dsp_area->canvas, xpos, ypos,
                  &seg, &offx, &offy, element);

  if (status == _DtCvSTATUS_OK)
    return seg;
  else
    return  (_DtCvSegment *)NULL;
}

void
NodeWindowAgent::disarm()
{
  unpreview();
}

void
NodeWindowAgent::arm()
{
  _DtCvLinkInfo ceHyper ;
  
  _DtCvUnit downX, downY, upX, upY ;

  downX = upX = f_help_dsp_area->timerX ;
  downY = upY = f_help_dsp_area->timerY ;

  if (XtIsManaged(f_help_dsp_area->vertScrollWid))
  {
    downY += f_vscrollbar_offset; // adjust y-pos for scroll bar
    upY += f_vscrollbar_offset; // adjust y-pos for scroll bar
  }

  if (_DtCvSTATUS_OK ==
      _DtCanvasGetPosLink (f_help_dsp_area->canvas, downX, downY, upX,
			   upY, &ceHyper))
    {
      UAS_String spec(ceHyper.specification);
      UAS_Pointer<UAS_Common> doc_ptr =
			f_node_view_info->node_ptr()->create_relative(spec);
      // NOTE: create_relative may have failed if the infolib associated
      //       with spec had been removed. So check doc_ptr before
      //       calling preview_init
      if (doc_ptr)
	preview_init(doc_ptr); 
    }
}

void
NodeWindowAgent::hscroll (unsigned int value)
{
  f_hscrollbar_offset = value ;
}

void
NodeWindowAgent::vscroll (unsigned int value)
{
#ifdef SCROLL_BOOKMARK_DEBUG
  cerr << "agent vscroll: " << value << endl;
#endif  

  f_vscrollbar_offset = value ;

  List_Iterator <MarkIcon *> i (f_mark_icon_list);

  while (i)
    {
      i.item()->TopOffset (i.item()->ypos() - value);
      i++ ;
    }
}

static UAS_Pointer<Mark> &
g_view_mark()
{
  static UAS_Pointer<Mark> da_mark;
  return (da_mark);
}


static unsigned
find_segment_offset (_DtCvSegment *start, const _DtCvSegment *target,
		     unsigned &offset);

static void
arm_callback (void *client_data)
{
  ((NodeWindowAgent*)client_data)->arm();
}
static void
disarm_callback (Widget, XtPointer client_data, XtPointer)
{
  ((NodeWindowAgent*)client_data)->disarm();
}

void
selection_end_callback(Widget, XtPointer, XtPointer)
{
  SelectionChanged msg ;
  node_mgr().UAS_Sender<SelectionChanged>::send_message (msg);
}

// For history jump to position stuff:
#ifdef jbm
AnchorCanvas *g_history_anchor;
#endif

bool g_scroll_to_locator ;
char g_top_locator[4096] ;

// When updating for the purposes of changing style sheets
extern bool g_style_sheet_update ;

static bool g_ignore_wm_delete;

// /////////////////////////////////////////////////////////////////
// ancestral hierarchy handling class
// /////////////////////////////////////////////////////////////////

#define CLASS Ancestor

class Ancestor : public WWL
{
public:
  Ancestor (NodeWindowAgent *nwa, WXmPulldownMenu &p,
	    const char *title, const UAS_Pointer<UAS_Common> &toc_ptr)
    : f_node_window_agent (nwa),
      f_button (p, "button", WAutoManage,
		WArgList (XmNlabelString, (XtArgVal) WXmString (title), NULL)),
      f_toc_ptr (toc_ptr)
    {
      ON_ACTIVATE (f_button,activate);
      // Make it look like a label if the user can't select it.
      // Note bogus assumption of 2 pixel wide shadow thickness. 
      if ((f_toc_ptr == 0) || f_toc_ptr->data_length() == 0)
	{
	  f_button.Set (WArgList (XmNshadowThickness, 0,
				  XmNmarginWidth, 4,
				  XmNmarginHeight, 4,
				  NULL));
	}
    }
  ~Ancestor()
    {
      f_button.Destroy();
    }
  void update (const char *title, UAS_Pointer<UAS_Common> toc_ptr);
  void activate();
  
  WXmPushButton     f_button;
  UAS_Pointer<UAS_Common>      f_toc_ptr;
  NodeWindowAgent  *f_node_window_agent;
};

void
Ancestor::update (const char *title, UAS_Pointer<UAS_Common> toc_ptr)
{
  if (title != NULL)
    {
      f_button.LabelString (title);
      if ((toc_ptr == 0)|| toc_ptr->data_length() == 0)
	{
	  f_button.Set (WArgList (XmNshadowThickness, 0,
				  XmNmarginWidth, 4,
				  XmNmarginHeight, 4,
				  NULL));
	}
      else
	{
	  f_button.Set (WArgList (XmNshadowThickness, 2,
				  XmNmarginWidth, 2,
				  XmNmarginHeight, 2,
				  NULL));
	}
      f_button.Manage();
    }
  else
    {
      f_button.Unmanage();
    }
  f_toc_ptr = toc_ptr;
}

void
Ancestor::activate()
{
    if (f_toc_ptr == (UAS_Pointer<UAS_Common>)NULL) 
    {
	return;
    }
    UAS_Pointer<UAS_Common> current;
    current = f_node_window_agent->f_node_view_info->node_ptr();

    // Avoid redisplaying the current document.
    if (f_toc_ptr == current) 
    {
	return;
    }
    
    if (f_toc_ptr != (UAS_Pointer<UAS_Common>)NULL)
    {
	node_mgr().set_preferred_window (f_node_window_agent);
	f_toc_ptr->retrieve();
    }
}


#undef CLASS
#define CLASS NodeWindowAgent

// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

NodeWindowAgent::NodeWindowAgent (u_int serial_no)
: f_shell (NULL),
  f_help_dsp_area (NULL),
  f_node_view_info (NULL),
  f_preview_timeout (NULL),
  f_serial_number(serial_no),
  f_current_ancestor (NULL),
  f_history_display (FALSE),
  f_form(NULL),
  f_graphics_handler(NULL),
  IcccmAgent((void*)this, (data_handler_t)&NodeWindowAgent::do_search_selection),
  f_close(NULL),
  f_close_sensitive(FALSE),
  f_vscrollbar_offset(0),
  f_hscrollbar_offset(0),
  f_graphic_segment(NULL)
{
    UAS_Common::request ((UAS_Receiver<UAS_LibraryDestroyedMsg> *) this);

    // request MarkCreated messages
    mark_mgr().request(this);
    // request MarkDeleted/Changed etc messages
    Mark::request (this);
}


// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

NodeWindowAgent::~NodeWindowAgent()
{
  List_Iterator<Ancestor *> i (f_ancestor_list);
  while (i)
    {
      delete i.item();
      f_ancestor_list.remove (i);
    }

  List_Iterator<BookTab *> t (f_tab_btn_list);
  while (t)
    {
      delete t.item();
      f_tab_btn_list.remove (t);
    }

#ifdef LICENSE_MANAGEMENT
  f_node_view_info->node_ptr()->returnlicense();
#endif

  cleanup_marks();

  node_mgr().agent_deleted (this);

  // The above call kills all the widgets.  
  if (f_graphics_handler)
    delete f_graphics_handler ;

  // clean things up so that the help display area deletion
  // will not stomp on the memory we free when deleting the nodeviewinfo
  _DtCanvasDestroy(f_help_dsp_area->canvas);
  f_help_dsp_area->canvas = NULL ;
  delete f_node_view_info;

  f_help_dsp_area->canvas = NULL ;

  delete f_scope_menu;
  delete f_wm_delete_callback;
  f_shell->Destroy();
}


// /////////////////////////////////////////////////////////////////
// file Callbacks
// /////////////////////////////////////////////////////////////////

static void
exitCB(Widget, XtPointer client_data, XtPointer)
{
  NodeWindowAgent *agent = (NodeWindowAgent*)client_data;
  agent->exit_cb();
#if 0
  if (BookmarkEdit::modified_count() > 0)
    {
      bool resp;
      resp = message_mgr().question_dialog (
		(char*)UAS_String(CATGETS(Set_Messages, 2,
				"Do you want to abandon your changes?")));
      if (! resp)
	return;
    }

  message_mgr().quit_dialog (
	(char*)UAS_String(CATGETS(Set_Messages, 6,
				"Do you really want to quit Dtinfo?")));
#endif
}

// /////////////////////////////////////////////////////////////////
// options Callbacks
// /////////////////////////////////////////////////////////////////

static void
tool_bar_toggleCB(Widget, XtPointer client_data, XtPointer call_data)
{
  Widget tool_bar = (Widget) client_data;
  XmToggleButtonCallbackStruct *cdata =
      (XmToggleButtonCallbackStruct *) call_data;

  if (cdata->set)
      XtManageChild(tool_bar);
  else
      XtUnmanageChild(tool_bar);
}

static void
search_area_toggleCB(Widget, XtPointer client_data, XtPointer call_data)
{
  Widget search_area = (Widget) client_data;
  XmToggleButtonCallbackStruct *cdata =
      (XmToggleButtonCallbackStruct *) call_data;

  if (cdata->set)
      XtManageChild(search_area);
  else
      XtUnmanageChild(search_area);
}

// /////////////////////////////////////////////////////////////////
// windows Callbacks
// /////////////////////////////////////////////////////////////////

static void
show_booklistCB(Widget, XtPointer, XtPointer)
{
  UAS_List<UAS_String> env_infolibs(env().infolibs());
  library_mgr().init(env_infolibs);
}

static void
show_mapCB(Widget, XtPointer client_data, XtPointer)
{
  Wait_Cursor bob;
  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  UAS_Pointer<UAS_Common> bogus;
  map_mgr().display_centered_on (bogus = agent->node_view_info()->node_ptr());
}

static void query_editorCB(Widget, XtPointer, XtPointer);
static void mark_listCB(Widget, XtPointer, XtPointer);
static void prefsCB(Widget, XtPointer, XtPointer);
static void section_historyCB(Widget, XtPointer, XtPointer);
static void open_urlCB(Widget, XtPointer, XtPointer);
static void search_historyCB(Widget, XtPointer, XtPointer);
static void scope_editorCB(Widget, XtPointer, XtPointer);


// /////////////////////////////////////////////////////////////////
// create_ui - create the user interface
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::create_ui()
{
  Widget widget;
  Arg args[5];
  int n;

  XmStringLocalized mtfstring;
  String	    string;
  KeySym	    mnemonic;

  UAS_Pointer<UAS_String> textstore;

//  Widget w = XtCreatePopupShell("nodeview", topLevelShellWidgetClass,
//				(Widget) toplevel(), NULL, 0);

  f_shell = (WTopLevelShell*)(Widget)
      WTopLevelShell (toplevel(), WPopup, "nodeview");

  window_system().register_shell (f_shell);

  string = CATGETS(Set_NodeWindowAgent, 1, "Dtinfo: Browser");
  XtVaSetValues((Widget)f_shell, XmNtitle, string, NULL);

  f_wm_delete_callback = 
      new WCallback (*f_shell, window_system().WM_DELETE_WINDOW(),
		     this, (WWL_FUN) &NodeWindowAgent::dismiss);

  WindowGeometry wg = pref_mgr().get_geometry (PrefMgr::BrowseGeometry);
  f_shell->Set(WArgList(XmNwidth, wg.width, XmNheight, wg.height, NULL));

  Widget mainw = XtCreateWidget("mainw", xmMainWindowWidgetClass,
				*f_shell, 0, 0);

  n = 0;
  XtSetArg(args[n], XmNscrolledWindowChildType, XmMENU_BAR); n++;
  Widget menu_bar = XmCreateMenuBar(mainw, (char*)"menu_bar", args, n);

  n = 0;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNscrolledWindowChildType, XmCOMMAND_WINDOW); n++;
  Widget tool_bar = XtCreateWidget("tool_bar", xmRowColumnWidgetClass,
				   mainw, args, n);

  n = 0;
  XtSetArg(args[n], XmNscrolledWindowChildType, XmMESSAGE_WINDOW); n++;
  Widget search_area = XtCreateWidget("search_area", xmFormWidgetClass,
				      mainw, args, n);

  // Menus
  Widget fileM = XmCreatePulldownMenu(menu_bar, (char*)"file_menu", 0, 0);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, fileM); n++;
  widget = XtCreateManagedWidget("file", xmCascadeButtonGadgetClass,
			menu_bar, args, n);
  help_agent().add_help_cb(widget);


  mtfstring =  CATGETS(Set_AgentLabel, 62, "Browser");
  mnemonic  = *CATGETS(Set_AgentLabel, 63, "");
  XtVaSetValues(widget, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  widget = XtCreateManagedWidget("new", xmPushButtonGadgetClass, fileM, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, cloneCB, this);

  mtfstring =  CATGETS(Set_AgentLabel, 64, "New Window");
  mnemonic  = *CATGETS(Set_AgentLabel, 65, "");
  XtVaSetValues(widget, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  XtCreateManagedWidget("sep1", xmSeparatorGadgetClass, fileM, 0, 0);

  f_print_as = XtCreateManagedWidget("print_as", xmPushButtonGadgetClass,
				     fileM, 0, 0);
  XtAddCallback(f_print_as, XmNactivateCallback, print_asCB, this);

  mtfstring =  CATGETS(Set_AgentLabel, 72, "Print...");
  mnemonic  = *CATGETS(Set_AgentLabel, 71, "P");
  XtVaSetValues(f_print_as, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  XtCreateManagedWidget("sep2", xmSeparatorGadgetClass, fileM, 0, 0);

  widget = XtCreateManagedWidget("show", xmPushButtonGadgetClass, fileM, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, show_locatorCB, this);

  mtfstring =  CATGETS(Set_AgentLabel, 68, "Show Locator");
  mnemonic  = *CATGETS(Set_AgentLabel, 69, "");
  XtVaSetValues(widget, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  XtCreateManagedWidget("sep3", xmSeparatorGadgetClass, fileM, 0, 0);

  n = 0;
  XtSetArg(args[n], XmNsensitive, f_close_sensitive); n++;
  f_close = XtCreateManagedWidget("close", xmPushButtonGadgetClass,
				  fileM, args, n);
  XtAddCallback(f_close, XmNactivateCallback, dismissCB, this);

  mtfstring =  CATGETS(Set_AgentLabel, 74, "Close");
  mnemonic  = *CATGETS(Set_AgentLabel, 75, "");
  XtVaSetValues(f_close, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  widget = XtCreateManagedWidget("exit", xmPushButtonGadgetClass, fileM, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, exitCB, this);

  mtfstring =  CATGETS(Set_AgentLabel, 76, "Exit");
  mnemonic  = *CATGETS(Set_AgentLabel, 77, "");
  XtVaSetValues(widget, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  // search menu
  Widget searchM = XmCreatePulldownMenu(menu_bar, (char*)"search_menu", 0, 0);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, searchM); n++;
  f_search_menu_button = XtCreateManagedWidget("search",
					       xmCascadeButtonGadgetClass,
					       menu_bar, args, n);
  help_agent().add_help_cb(f_search_menu_button);

  mtfstring =  CATGETS(Set_AgentLabel, 102, "Search");
  mnemonic  = *CATGETS(Set_AgentLabel, 103, "");
  XtVaSetValues(f_search_menu_button, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  widget = XtCreateManagedWidget("on_selection", xmPushButtonGadgetClass,
			      searchM, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, search_on_selectionCB, this);

  mtfstring =  CATGETS(Set_AgentLabel, 78, "On Selection");
  mnemonic  = *CATGETS(Set_AgentLabel, 79, "");
  XtVaSetValues(widget, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  widget = XtCreateManagedWidget("query_editor", xmPushButtonGadgetClass,
			      searchM, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, query_editorCB, 0);

  mtfstring =  CATGETS(Set_AgentLabel, 80, "Query Editor");
  mnemonic  = *CATGETS(Set_AgentLabel, 81, "");
  XtVaSetValues(widget, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  widget = XtCreateManagedWidget("scope_editor", xmPushButtonGadgetClass,
			      searchM, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, scope_editorCB, 0);

  mtfstring =  CATGETS(Set_AgentLabel, 82, "Scope Editor");
  mnemonic  = *CATGETS(Set_AgentLabel, 83, "");
  XtVaSetValues(widget, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  XtCreateManagedWidget("sep1", xmSeparatorGadgetClass, searchM, 0, 0);

  widget = XtCreateManagedWidget("search_history", xmPushButtonGadgetClass,
			      searchM, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, search_historyCB, 0);

  mtfstring =  CATGETS(Set_AgentLabel, 84, "Search History");
  mnemonic  = *CATGETS(Set_AgentLabel, 85, "");
  XtVaSetValues(widget, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  XtCreateManagedWidget("sep2", xmSeparatorGadgetClass, searchM, 0, 0);

  f_clear_search_hits =
	XtCreateManagedWidget("clear_search", xmPushButtonGadgetClass,
			      searchM, 0, 0);
  XtAddCallback(f_clear_search_hits, XmNactivateCallback, clear_searchCB, this);

  XtVaSetValues(f_clear_search_hits,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 86, "Clear Search Hits")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 87, ""),
	NULL);

  XtCreateManagedWidget("sep3", xmSeparatorGadgetClass, searchM, 0, 0);

  f_search_prev = XtCreateManagedWidget("search_prev",
					xmPushButtonGadgetClass, searchM, 0, 0);
  XtAddCallback(f_search_prev, XmNactivateCallback, search_previousCB, this);

  mtfstring =  CATGETS(Set_AgentLabel, 98, "Previous Search");
  mnemonic  = *CATGETS(Set_AgentLabel, 99, "");
  XtVaSetValues(f_search_prev, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  f_search_next = XtCreateManagedWidget("search_next",
					xmPushButtonGadgetClass, searchM, 0, 0);
  XtAddCallback(f_search_next, XmNactivateCallback, search_nextCB, this);

  mtfstring =  CATGETS(Set_AgentLabel, 100, "Next Search");
  mnemonic  = *CATGETS(Set_AgentLabel, 101, "");
  XtVaSetValues(f_search_next, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  // go menu
  Widget goM = XmCreatePulldownMenu(menu_bar, (char*)"go_menu", 0, 0);
  n = 0;
  XtSetArg(args[n], XmNsubMenuId, goM); n++;
  widget = XtCreateManagedWidget("go", xmCascadeButtonGadgetClass,
			menu_bar, args, n);
  help_agent().add_help_cb(widget);

  mtfstring =  CATGETS(Set_AgentLabel, 104, "Go");
  mnemonic  = *CATGETS(Set_AgentLabel, 105, "");
  XtVaSetValues(widget, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  f_history_prev = XtCreateManagedWidget("history_prev",
					 xmPushButtonGadgetClass, goM, 0, 0);
  XtAddCallback(f_history_prev, XmNactivateCallback, history_prevCB, this);
  XtAddCallback(f_history_prev, XmNarmCallback, preview_history_prevCB, this);
  XtAddCallback(f_history_prev, XmNdisarmCallback, unpreviewCB, this);

  mtfstring =  CATGETS(Set_AgentLabel, 88, "Back (History)");
  mnemonic  = *CATGETS(Set_AgentLabel, 89, "");
  XtVaSetValues(f_history_prev, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  f_history_next = XtCreateManagedWidget("history_next",
					 xmPushButtonGadgetClass, goM, 0, 0);
  XtAddCallback(f_history_next, XmNactivateCallback, history_nextCB, this);
  XtAddCallback(f_history_next, XmNarmCallback, preview_history_nextCB, this);
  XtAddCallback(f_history_next, XmNdisarmCallback, unpreviewCB, this);

  mtfstring =  CATGETS(Set_AgentLabel, 90, "Forward (History)");
  mnemonic  = *CATGETS(Set_AgentLabel, 91, "");
  XtVaSetValues(f_history_next, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  widget = XtCreateManagedWidget("section_history",
			      xmPushButtonGadgetClass, goM, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, section_historyCB, 0);

  mtfstring =  CATGETS(Set_AgentLabel, 92, "History...");
  mnemonic  = *CATGETS(Set_AgentLabel, 93, "");
  XtVaSetValues(widget, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  XtCreateManagedWidget("sep1", xmSeparatorGadgetClass, goM, 0, 0);

  f_node_next = XtCreateManagedWidget("section_next",
				      xmPushButtonGadgetClass, goM, 0, 0);
  XtAddCallback(f_node_next, XmNactivateCallback, node_nextCB, this);
  XtAddCallback(f_node_next, XmNarmCallback, preview_nextCB, this);
  XtAddCallback(f_node_next, XmNdisarmCallback, unpreviewCB, this);

  mtfstring =  CATGETS(Set_AgentLabel, 96, "Next Section");
  mnemonic  = *CATGETS(Set_AgentLabel, 97, "");
  XtVaSetValues(f_node_next, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  f_node_prev = XtCreateManagedWidget("section_prev",
				      xmPushButtonGadgetClass, goM, 0, 0);
  XtAddCallback(f_node_prev, XmNactivateCallback, node_previousCB, this);
  XtAddCallback(f_node_prev, XmNarmCallback, preview_previousCB, this);
  XtAddCallback(f_node_prev, XmNdisarmCallback, unpreviewCB, this);

  mtfstring =  CATGETS(Set_AgentLabel, 94, "Previous Section");
  mnemonic  = *CATGETS(Set_AgentLabel, 95, "");
  XtVaSetValues(f_node_prev, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  XtCreateManagedWidget("sep2", xmSeparatorGadgetClass, goM, 0, 0);

  widget = XtCreateManagedWidget("open", xmPushButtonGadgetClass, goM, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, open_urlCB, 0);

  mtfstring =  CATGETS(Set_AgentLabel, 66, "Open Locator...");
  mnemonic  = *CATGETS(Set_AgentLabel, 67, "");
  XtVaSetValues(widget, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  // marks menu
  Widget markM = XmCreatePulldownMenu(menu_bar, (char*)"marks_menu", 0, 0);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, markM); n++;
  widget = XtCreateManagedWidget("marks", xmCascadeButtonGadgetClass,
			menu_bar, args, n);
  help_agent().add_help_cb(widget);

  mtfstring =  CATGETS(Set_AgentLabel, 106, "Marks");
  mnemonic  = *CATGETS(Set_AgentLabel, 107, "");
  XtVaSetValues(widget, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  f_create_bmrk = XtCreateManagedWidget("create_bmrk",
					xmPushButtonGadgetClass,
					markM, 0, 0);
  XtAddCallback(f_create_bmrk, XmNactivateCallback, create_bookmarkCB, this);

  XmFontList bmfont;
  {
    XmFontList font;
    XtVaGetValues(f_create_bmrk, XmNfontList, &font, NULL);
    bmfont = XmFontListCopy(font);
  }

  if (window_system().dtinfo_font())
    bmfont = XmFontListAppendEntry(bmfont, window_system().dtinfo_font());

  mtfstring =  CATGETS(Set_AgentLabel, 108, "Create Bookmark");
  mnemonic  = *CATGETS(Set_AgentLabel, 109, "");
  XtVaSetValues(f_create_bmrk, XmNlabelString, (XmString)mtfstring,
			       XmNmnemonic, mnemonic,
			       XmNfontList, bmfont, NULL);

  f_create_anno = XtCreateManagedWidget("create_anno",
					xmPushButtonGadgetClass,
					markM, 0, 0);
  XtAddCallback(f_create_anno, XmNactivateCallback, create_annotationCB, this);

  mtfstring =  CATGETS(Set_AgentLabel, 110, "Create Note...");
  mnemonic  = *CATGETS(Set_AgentLabel, 111, "");
  XtVaSetValues(f_create_anno, XmNlabelString, (XmString)mtfstring,
			       XmNmnemonic, mnemonic,
			       XmNfontList, bmfont, NULL);

  XtCreateManagedWidget("sep1", xmSeparatorGadgetClass, markM, 0, 0);

  Widget list_marks = XtCreateManagedWidget("list_marks",
					    xmPushButtonGadgetClass,
					    markM, 0, 0);
  XtAddCallback(list_marks, XmNactivateCallback, mark_listCB, 0);

  mtfstring =  CATGETS(Set_AgentLabel, 112, "List Marks");
  mnemonic  = *CATGETS(Set_AgentLabel, 113, "");
  XtVaSetValues(list_marks, XmNlabelString, (XmString)mtfstring,
			    XmNmnemonic, mnemonic,
			    XmNfontList, bmfont, NULL);

  XtCreateManagedWidget("sep2", xmSeparatorGadgetClass, markM, 0, 0);

  f_edit_mark = XtCreateManagedWidget("edit_mark",
				      xmPushButtonGadgetClass, markM, 0, 0);
  XtAddCallback(f_edit_mark, XmNactivateCallback, edit_markCB, this);

  mtfstring =  CATGETS(Set_AgentLabel, 114, "Edit Selection...");
  mnemonic  = *CATGETS(Set_AgentLabel, 115, "");
  XtVaSetValues(f_edit_mark, XmNlabelString, (XmString)mtfstring,
			     XmNmnemonic, mnemonic,
			     XmNfontList, bmfont, NULL);

  f_move_mark = XtCreateManagedWidget("move_mark",
				      xmPushButtonGadgetClass, markM, 0, 0);
  XtAddCallback(f_move_mark, XmNactivateCallback, move_markCB, this);

  mtfstring =  CATGETS(Set_AgentLabel, 116, "Move Selection");
  mnemonic  = *CATGETS(Set_AgentLabel, 117, "");
  XtVaSetValues(f_move_mark, XmNlabelString, (XmString)mtfstring,
			     XmNmnemonic, mnemonic,
			     XmNfontList, bmfont, NULL);

  f_delete_mark = XtCreateManagedWidget("delete_mark",
					xmPushButtonGadgetClass,
					markM, 0, 0);
  XtAddCallback(f_delete_mark, XmNactivateCallback, delete_markCB, this);

  mtfstring =  CATGETS(Set_AgentLabel, 118, "Delete Selection");
  mnemonic  = *CATGETS(Set_AgentLabel, 119, "");
  XtVaSetValues(f_delete_mark, XmNlabelString, (XmString)mtfstring,
			       XmNmnemonic, mnemonic,
			       XmNfontList, bmfont, NULL);

  // Add the icons to the beginning of the label strings
  if (!window_system().nofonts())
    {
	char icon[3];
	icon[1] = OLIAS_SPACE04;
	icon[2] = '\0';
	XmString string;

	icon[0] = OLIAS_BOOKMARK_ICON;
	n = 0;
	XtSetArg(args[n], XmNlabelString, &string); n++;
	XtGetValues(f_create_bmrk, args, n);
	n = 0;
	XtSetArg(args[n], XmNlabelString,
		 XmStringConcat(XmStringCreate(icon, (char*)OLIAS_FONT),
				string)); n++;
	XtSetValues(f_create_bmrk, args, n);

	icon[0] = OLIAS_ANNOTATION_ICON;
	n = 0;
	XtSetArg(args[n], XmNlabelString, &string); n++;
	XtGetValues(f_create_anno, args, n);
	n = 0;
	XtSetArg(args[n], XmNlabelString,
		 XmStringConcat(XmStringCreate(icon, (char*)OLIAS_FONT),
				string)); n++;
	XtSetValues(f_create_anno, args, n);

	icon[0] = OLIAS_SPACE16;
	n = 0;
	XtSetArg(args[n], XmNlabelString, &string); n++;
	XtGetValues(f_move_mark, args, n);
	n = 0;
	XtSetArg(args[n], XmNlabelString,
		 XmStringConcat(XmStringCreate(icon, (char*)OLIAS_FONT),
				string)); n++;
	XtSetValues(f_move_mark, args, n);

	n = 0;
	XtSetArg(args[n], XmNlabelString, &string); n++;
	XtGetValues(list_marks, args, n);
	n = 0;
	XtSetArg(args[n], XmNlabelString,
		 XmStringConcat(XmStringCreate(icon, (char*)OLIAS_FONT),
				string)); n++;
	XtSetValues(list_marks, args, n);

	n = 0;
	XtSetArg(args[n], XmNlabelString, &string); n++;
	XtGetValues(f_edit_mark, args, n);
	n = 0;
	XtSetArg(args[n], XmNlabelString,
		 XmStringConcat(XmStringCreate(icon, (char*)OLIAS_FONT),
				string)); n++;
	XtSetValues(f_edit_mark, args, n);

	n = 0;
	XtSetArg(args[n], XmNlabelString, &string); n++;
	XtGetValues(f_delete_mark, args, n);
	n = 0;
	XtSetArg(args[n], XmNlabelString,
		 XmStringConcat(XmStringCreate(icon, (char*)OLIAS_FONT),
				string)); n++;
	XtSetValues(f_delete_mark, args, n);
    }

  // options menu
  Widget optionsM = XmCreatePulldownMenu(menu_bar, (char*)"options_menu", 0, 0);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, optionsM); n++;
  widget = XtCreateManagedWidget("options", xmCascadeButtonGadgetClass,
			menu_bar, args, n);
  help_agent().add_help_cb(widget);

  mtfstring =  CATGETS(Set_AgentLabel, 120, "Options");
  mnemonic  = *CATGETS(Set_AgentLabel, 121, "");
  XtVaSetValues(widget, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  widget = XtCreateManagedWidget("preferences", xmPushButtonGadgetClass,
			      optionsM, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, prefsCB, 0);

  mtfstring =  CATGETS(Set_AgentLabel, 122, "Preferences...");
  mnemonic  = *CATGETS(Set_AgentLabel, 123, "");
  XtVaSetValues(widget, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  XtCreateManagedWidget("sep1", xmSeparatorGadgetClass, optionsM, 0, 0);

  f_detach_graphic = XtCreateManagedWidget("detach_graphic",
					   xmPushButtonGadgetClass,
					   optionsM, 0, 0);
  XtAddCallback(f_detach_graphic, XmNactivateCallback, detach_graphicCB, this);

  mtfstring =  CATGETS(Set_AgentLabel, 124, "Detach Graphic");
  mnemonic  = *CATGETS(Set_AgentLabel, 125, "");
  XtVaSetValues(f_detach_graphic, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  XtCreateManagedWidget("sep2", xmSeparatorGadgetClass, optionsM, 0, 0);

  Widget tool_barT = XtCreateManagedWidget("show_tool_bar",
					   xmToggleButtonGadgetClass,
					   optionsM, 0, 0);
  XtAddCallback(tool_barT, XmNvalueChangedCallback,
		tool_bar_toggleCB, tool_bar);

  mtfstring =  CATGETS(Set_AgentLabel, 126, "Show Tool Bar");
  mnemonic  = *CATGETS(Set_AgentLabel, 127, "");
  XtVaSetValues(tool_barT, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  Widget search_areaT = XtCreateManagedWidget("show_search_area",
					      xmToggleButtonGadgetClass,
					      optionsM, 0, 0);
  XtAddCallback(search_areaT, XmNvalueChangedCallback,
		search_area_toggleCB, search_area);

  mtfstring =  CATGETS(Set_AgentLabel, 128, "Show Search Area");
  mnemonic  = *CATGETS(Set_AgentLabel, 129, "");
  XtVaSetValues(search_areaT, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  // windows menu
  Widget windowsM = XmCreatePulldownMenu(menu_bar, (char*)"windows_menu", 0, 0);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, windowsM); n++;
  widget = XtCreateManagedWidget("windows", xmCascadeButtonGadgetClass,
			menu_bar, args, n);
  help_agent().add_help_cb(widget);

  mtfstring =  CATGETS(Set_AgentLabel, 130, "Windows");
  mnemonic  = *CATGETS(Set_AgentLabel, 131, "");
  XtVaSetValues(widget, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  widget = XtCreateManagedWidget("booklist", xmPushButtonGadgetClass,
			      windowsM, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, show_booklistCB, 0);

  mtfstring =  CATGETS(Set_AgentLabel, 132, "Book List");
  mnemonic  = *CATGETS(Set_AgentLabel, 133, "");
  XtVaSetValues(widget, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

  widget = XtCreateManagedWidget("graphical_map", xmPushButtonGadgetClass,
			      windowsM, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, show_mapCB, this);

  mtfstring =  CATGETS(Set_AgentLabel, 134, "Graphical Map");
  mnemonic  = *CATGETS(Set_AgentLabel, 135, "");
  XtVaSetValues(widget, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);


  // help menu
  Widget helpM = XmCreatePulldownMenu(menu_bar, (char*)"help_menu", 0, 0);
  n = 0;
  XtSetArg(args[n], XmNsubMenuId, helpM); n++;
  widget = XtCreateManagedWidget("help", xmCascadeButtonGadgetClass,
			      menu_bar, args, n);
  help_agent().add_help_cb(widget);

  n = 0;
  XtSetArg(args[n], XmNmenuHelpWidget, widget); n++;
  XtSetValues(menu_bar, args, n);

  mtfstring =  CATGETS(Set_AgentLabel, 143, "Help");
  mnemonic  = *CATGETS(Set_AgentLabel, 144, "");
  XtVaSetValues(widget, XmNlabelString, (XmString)mtfstring, XmNmnemonic, mnemonic, NULL);

#define AM WAutoManage

  WXmPushButton      on_overview      	(helpM,      "on_overview", AM);
  WXmSeparator       on_sep1		(helpM,      "on_sep1", AM);
  WXmPushButton      on_tasks      	(helpM,      "on_tasks", AM);
  WXmPushButton      on_reference	(helpM,      "on_reference", AM);
  WXmPushButton      on_item      	(helpM,      "on_item", AM);
  WXmSeparator       on_sep2		(helpM,      "on_sep2", AM);
  WXmPushButton      on_help         	(helpM,      "on_help"       , AM);
  WXmSeparator       on_sep3		(helpM,      "on_sep3", AM);
  WXmPushButton      on_about         	(helpM,      "on_about"       , AM);


  //WXmPushButton      on_window       	(helpM,      "on_window"     , AM);
//  WXmPushButton      on_application  (helpM,      "on_application", AM);
//  WXmPushButton      index           (helpM,      "index"         , AM);
  //WXmPushButton      on_keys         (helpM,      "on_keys"       , AM);
  //WXmPushButton      on_version      (helpM,      "on_version"    , AM);
  //ON_ACTIVATE (on_version, 		display_version);

  mtfstring =  CATGETS(Set_AgentLabel, 257, "Overview");
  mnemonic  = *CATGETS(Set_AgentLabel, 258, "");
  XtVaSetValues((Widget)on_overview, XmNlabelString, (XmString)mtfstring,
				    XmNmnemonic, mnemonic, NULL);

  mtfstring =  CATGETS(Set_AgentLabel, 249, "Tasks");
  mnemonic  = *CATGETS(Set_AgentLabel, 250, "");
  XtVaSetValues((Widget)on_tasks, XmNlabelString, (XmString)mtfstring,
				    XmNmnemonic, mnemonic, NULL);

  mtfstring =  CATGETS(Set_AgentLabel, 251, "Reference");
  mnemonic  = *CATGETS(Set_AgentLabel, 252, "");
  XtVaSetValues((Widget)on_reference, XmNlabelString, (XmString)mtfstring,
				    XmNmnemonic, mnemonic, NULL);

  mtfstring =  CATGETS(Set_AgentLabel, 50, "On Item...");
  mnemonic  = *CATGETS(Set_AgentLabel, 51, "");
  XtVaSetValues((Widget)on_item, XmNlabelString, (XmString)mtfstring,
				    XmNmnemonic, mnemonic, NULL);

  mtfstring =  CATGETS(Set_AgentLabel, 253, "Using Help");
  mnemonic  = *CATGETS(Set_AgentLabel, 254, "");
  XtVaSetValues((Widget)on_help, XmNlabelString, (XmString)mtfstring,
				    XmNmnemonic, mnemonic, NULL);

  mtfstring =  CATGETS(Set_AgentLabel, 255, "About Information Manager");
  mnemonic  = *CATGETS(Set_AgentLabel, 256, "");
  XtVaSetValues((Widget)on_about, XmNlabelString, (XmString)mtfstring,
				    XmNmnemonic, mnemonic, NULL);


#ifdef EAM
  mtfstring =  CATGETS(Set_AgentLabel, 147, "On Window");
  mnemonic  = *CATGETS(Set_AgentLabel, 148, "");
  XtVaSetValues((Widget)on_window, XmNlabelString, (XmString)mtfstring,
				    XmNmnemonic, mnemonic, NULL);
//  mtfstring =  CATGETS(Set_AgentLabel, 149, "On Application");
//  mnemonic  = *CATGETS(Set_AgentLabel, 150, "");
//  XtVaSetValues((Widget)on_application, XmNlabelString, (XmString)mtfstring,
//				    XmNmnemonic, mnemonic, NULL);
//  mtfstring =  CATGETS(Set_AgentLabel, 151, "Index");
//  mnemonic  = *CATGETS(Set_AgentLabel, 152, "");
//  XtVaSetValues((Widget)index, XmNlabelString, (XmString)mtfstring,
//				    XmNmnemonic, mnemonic, NULL);
  mtfstring =  CATGETS(Set_AgentLabel, 155, "On Keys");
  mnemonic  = *CATGETS(Set_AgentLabel, 156, "");
  XtVaSetValues((Widget)on_keys, XmNlabelString, (XmString)mtfstring,
				    XmNmnemonic, mnemonic, NULL);
  mtfstring =  CATGETS(Set_AgentLabel, 157, "On Version");
  mnemonic  = *CATGETS(Set_AgentLabel, 158. "");
  XtVaSetValues((Widget)on_version, XmNlabelString, (XmString)mtfstring,
				    XmNmnemonic, mnemonic, NULL);
  
#endif

  help_agent().add_activate_help ((Widget)on_overview, (char*)"on_overview");
  help_agent().add_activate_help ((Widget)on_tasks, (char*)"on_tasks");
  help_agent().add_activate_help ((Widget)on_reference, (char*)"on_reference");
  help_agent().add_activate_help ((Widget)on_reference, (char*)"on_reference");
  help_agent().add_context_help ((Widget)on_item);
  help_agent().add_activate_help ((Widget)on_help, (char*)"on_help");
  help_agent().add_activate_help ((Widget)on_about, (char*)"on_about");


  // Tools
  f_history_prev2 = XtCreateManagedWidget("history_prev",
					  xmPushButtonWidgetClass,
					  tool_bar, 0, 0);
  XtAddCallback(f_history_prev2, XmNactivateCallback, history_prevCB, this);
  XtAddCallback(f_history_prev2, XmNarmCallback, preview_history_prevCB, this);
  XtAddCallback(f_history_prev2, XmNdisarmCallback, unpreviewCB, this);
  help_agent().add_help_cb (f_history_prev2);

  textstore = new UAS_String(CATGETS(Set_AgentQHelp, 1,
				"Go to the previous section in history"));
  add_quick_help(f_history_prev2, (char*)*textstore);
  f_textstore.insert_item(textstore);

  f_history_next2 = XtCreateManagedWidget("history_next",
					  xmPushButtonWidgetClass,
					  tool_bar, 0, 0);
  XtAddCallback(f_history_next2, XmNactivateCallback, history_nextCB, this);
  XtAddCallback(f_history_next2, XmNarmCallback, preview_history_nextCB, this);
  XtAddCallback(f_history_next2, XmNdisarmCallback, unpreviewCB, this);
  help_agent().add_help_cb (f_history_next2);

  textstore = new UAS_String(CATGETS(Set_AgentQHelp, 2,
				"Go to the next section in history"));
  add_quick_help(f_history_next2, (char*)*textstore);
  f_textstore.insert_item(textstore);

  widget = XtCreateManagedWidget("space",
				xmLabelGadgetClass,
				tool_bar, 0, 0);

  f_node_prev2 = XtCreateManagedWidget("section_prev",
				       xmPushButtonWidgetClass,
				       tool_bar, 0, 0);
  XtAddCallback(f_node_prev2, XmNactivateCallback, node_previousCB, this);
  XtAddCallback(f_node_prev2, XmNarmCallback, preview_previousCB, this);
  XtAddCallback(f_node_prev2, XmNdisarmCallback, unpreviewCB, this);
  help_agent().add_help_cb (f_node_prev2);

  textstore = new UAS_String(CATGETS(Set_AgentQHelp, 3,
				"Go to the previous section in document"));
  add_quick_help(f_node_prev2, (char*)*textstore);
  f_textstore.insert_item(textstore);

  f_node_next2 = XtCreateManagedWidget("section_next",
				       xmPushButtonWidgetClass,
				       tool_bar, 0, 0);
  XtAddCallback(f_node_next2, XmNactivateCallback, node_nextCB, this);
  XtAddCallback(f_node_next2, XmNarmCallback, preview_nextCB, this);
  XtAddCallback(f_node_next2, XmNdisarmCallback, unpreviewCB, this);
  help_agent().add_help_cb (f_node_next2);

  textstore = new UAS_String(CATGETS(Set_AgentQHelp, 4,
				"Go to the next section in document"));
  add_quick_help(f_node_next2, (char*)*textstore);
  f_textstore.insert_item(textstore);

  widget = XtCreateManagedWidget("space",
				xmLabelGadgetClass,
				tool_bar, 0, 0);

  f_search_prev2 = XtCreateManagedWidget("search_prev",
					 xmPushButtonWidgetClass,
					 tool_bar, 0, 0);
  XtAddCallback(f_search_prev2, XmNactivateCallback, search_previousCB, this);
  help_agent().add_help_cb (f_search_prev2);

  textstore = new UAS_String(CATGETS(Set_AgentQHelp, 5,
				"Go to the previous search match"));
  add_quick_help(f_search_prev2, (char*)*textstore);
  f_textstore.insert_item(textstore);

  f_search_next2 = XtCreateManagedWidget("search_next",
					 xmPushButtonWidgetClass,
					 tool_bar, 0, 0);
  XtAddCallback(f_search_next2, XmNactivateCallback, search_nextCB, this);
  help_agent().add_help_cb (f_search_next2);

  textstore = new UAS_String(CATGETS(Set_AgentQHelp, 6,
				"Go to the next search match"));
  add_quick_help(f_search_next2, (char*)*textstore);
  f_textstore.insert_item(textstore);


  widget = XtCreateManagedWidget("space",
				xmLabelGadgetClass,
				tool_bar, 0, 0);


  f_print2 = XtCreateManagedWidget("print", xmPushButtonWidgetClass,
				   tool_bar, 0, 0);
  XtAddCallback(f_print2, XmNactivateCallback, printCB, this);
  help_agent().add_help_cb (f_print2);

  textstore = new UAS_String(CATGETS(Set_AgentQHelp, 7,
				"Print current section"));
  add_quick_help(f_print2, (char*)*textstore);
  f_textstore.insert_item(textstore);

  widget = XtCreateManagedWidget("booklist", xmPushButtonWidgetClass,
			      tool_bar, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, show_booklistCB, 0);
  help_agent().add_help_cb (widget);

  textstore = new UAS_String(CATGETS(Set_AgentQHelp, 8, "Display Book List"));
  add_quick_help(widget, (char*)*textstore);
  f_textstore.insert_item(textstore);

  widget = XtCreateManagedWidget("graphical_map", xmPushButtonWidgetClass,
			      tool_bar, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, show_mapCB, this);
  help_agent().add_help_cb (widget);

  textstore = new UAS_String(CATGETS(Set_AgentQHelp, 9,
					"Display Graphical Map"));
  add_quick_help(widget, (char*)*textstore);
  f_textstore.insert_item(textstore);

  f_locked = pref_mgr().get_boolean (PrefMgr::BrowseLock);
  n = 0;
  XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;

  // The opposite state always needs to be the semilocked pixmap. 
  if (f_locked)
    {
	XtSetArg(args[n], XmNset, True); n++;
	XtSetArg(args[n], XmNselectPixmap,
		 window_system().locked_pixmap(tool_bar)); n++;
	XtSetArg(args[n], XmNlabelPixmap,
		 window_system().semilocked_pixmap(tool_bar)); n++;
      }
  else
    {
	XtSetArg(args[n], XmNset, False); n++;
	XtSetArg(args[n], XmNlabelPixmap,
		 window_system().unlocked_pixmap(tool_bar)); n++;
	XtSetArg(args[n], XmNselectPixmap,
		 window_system().semilocked_pixmap(tool_bar)); n++;
    }

  widget = XtCreateManagedWidget("lock", xmToggleButtonWidgetClass,
			      tool_bar, args, n);
  XtAddCallback(widget, XmNvalueChangedCallback, lock_toggleCB, this);
  help_agent().add_help_cb (widget);

  textstore = new UAS_String(CATGETS(Set_AgentQHelp, 10,
		"Pin (unpin) this window, "
		"to prevent (allow) use for new documents."));
  add_quick_help(widget, (char*)*textstore);
  f_textstore.insert_item(textstore);

  mtfstring =  CATGETS(Set_AgentLabel, 159, "Retain");
  XtVaSetValues(widget, XmNlabelString, (XmString)mtfstring, NULL);
  
  // Work Window
  n = 0;
  XtSetArg(args[n], XmNscrolledWindowChildType, XmWORK_AREA); n++;
  f_form = XtCreateWidget("form", xmFormWidgetClass, mainw, args, n);


  f_title_menu = (WXmPulldownMenu*)(Widget)WXmPulldownMenu((WComposite&)f_form, (char*)"title_menu");
  n = 0;
  XtSetArg(args[n], XmNsubMenuId, (Widget) f_title_menu); n++;
  f_title_option = (WXmOptionMenu*)(Widget)
                   WXmOptionMenu ((WComposite&)f_form, (char*)"title", AM, args, n);
  help_agent().add_help_cb ((Widget)*f_title_option);

  f_tab_area = (WXmRowColumn*)(Widget) WXmRowColumn (f_form, "tab_area");
  f_tab_area->Manage();

  // this needs to be here for reference through the resources (attachments)
  Widget message = XtCreateManagedWidget("message", xmFrameWidgetClass,
					 f_form, 0, 0);

  // Status line
  Pixel fg, bg;
  n = 0;
  XtSetArg(args[n], XmNforeground, &fg); n++;
  XtSetArg(args[n], XmNbackground, &bg); n++;
  XtGetValues(message, args, n);
  n = 0;
  XtSetArg(args[n], XmNforeground, fg); n++;
  XtSetArg(args[n], XmNbackground, bg); n++;
  f_status_text = XtCreateManagedWidget("text", xmTextFieldWidgetClass,
					message, args, n);
  string = CATGETS(Set_NodeWindowAgent, 2, "To preview a link destination, "
					   "click and hold on the link.");
  XtVaSetValues(f_status_text, XmNvalue, string, NULL);

#ifdef DEBUG
#ifdef MONITOR

  WXmCascadeButton   windows_cascade (menu_bar,     "windows"       ,   AM);
  WXmPulldownMenu    windows_menu    (menu_bar,     (char*)"windows_menu" );
  WXmPushButton(windows_menu, "monitor", AM).
    SetActivateCallback(this, (WWL_FUN)&NodeWindowAgent::monitor, (void*)1);

  WXmPushButton(windows_menu, "monitor_off", AM).
    SetActivateCallback(this, (WWL_FUN)&NodeWindowAgent::monitor, 0);

#endif
#endif

#ifdef NODEBUG
  WXmCascadeButton   debug_cascade     (menu_bar,   "debug"                );
  WXmPulldownMenu    debug_menu        (menu_bar,   (char*)"debug_menu"    );
  WXmToggleButton    command_processor (debug_menu, "command_processor", AM);
  WXmPushButton	     document_id       (debug_menu, "document_id", 	 AM);
#endif


  f_frame = (WXmFrame *)(Widget)WXmFrame (f_form, "frame"      , AM);

  XmFontList defaultList = NULL;
  n = 0;
  XtSetArg (args[n], XmNfontList, &defaultList); ++n;
  XtGetValues(f_status_text, args, n);

  f_help_dsp_area = (DtHelpDispAreaStruct*)
    _DtHelpCreateDisplayArea(WXmForm(*f_frame, "inform", WAutoManage),//parent
			     (char*)"display_area",	// name
			     _DtHelpSTATIC,	// vert flag
			     _DtHelpAS_NEEDED,	// horz flag
			     FALSE,		// traversal flag
			     1,			// rows
			     1,			// columns
			     (void(*)()) hypertext_cb, // hypertext cb
			     (void(*)())resize_cb,     // resize cb
			     0,			// exec ok routine
			     this,		// client_data
			     defaultList	// default font list
			     );

  assert( f_help_dsp_area && f_help_dsp_area->canvas );

  help_agent().add_help_cb (f_help_dsp_area->dispWid);
  help_agent().add_help_cb (f_help_dsp_area->vertScrollWid);
  help_agent().add_help_cb (f_help_dsp_area->horzScrollWid);

  // create detach graphic popup menu
  f_detach_menu = XmCreatePopupMenu(
                       f_help_dsp_area->dispWid, (char*)"detach_graphic", NULL, 0);
  f_detach_button = XmCreatePushButton(f_detach_menu, (char*)"Detach Graphic", NULL, 0);
  XtAddCallback(f_detach_button, XmNactivateCallback, detach_grCB, this);
  XtManageChild(f_detach_button);
  mtfstring =  CATGETS(Set_AgentLabel, 124, "Detach Graphic");
  XtVaSetValues(f_detach_button, XmNlabelString, (XmString)mtfstring, NULL);

  f_attach_button = XmCreatePushButton(f_detach_menu, (char*)"Attach Graphic", NULL, 0);
  XtAddCallback(f_attach_button, XmNactivateCallback, attach_grCB, this);
  XtManageChild(f_attach_button);
  mtfstring =  CATGETS(Set_AgentLabel, 265, "Attach Graphic");
  XtVaSetValues(f_attach_button, XmNlabelString, (XmString)mtfstring, NULL);

  f_raise_button = XmCreatePushButton(f_detach_menu, (char*)"Raise Graphic", NULL, 0);
  XtAddCallback(f_raise_button, XmNactivateCallback, raise_grCB, this);
  XtManageChild(f_raise_button);
  mtfstring =  CATGETS(Set_AgentLabel, 264, "Raise Graphic");
  XtVaSetValues(f_raise_button, XmNlabelString, (XmString)mtfstring, NULL);

  XtAddEventHandler (f_help_dsp_area->dispWid, ButtonPressMask, False, popup_menuCB, (XtPointer)this);


  // create link preview popup menu
  f_preview_menu = XmCreatePopupMenu(
                       f_help_dsp_area->dispWid, (char*)"preview_menu", NULL, 0);

  f_preview_label = XmCreateLabel(f_preview_menu, (char*)"pre_label", NULL, 0);
  XtManageChild(f_preview_label);

  widget = XmCreateSeparator(f_preview_menu, (char*)"sep", NULL, 0);
  XtVaSetValues(widget, XmNseparatorType, XmDOUBLE_LINE, NULL);
  XtManageChild(widget);

  widget = XmCreatePushButton(f_preview_menu, (char*)"widget", NULL, 0);
  XtAddCallback(widget, XmNactivateCallback, go_to_linkCB, this);
  XtManageChild(widget);
  mtfstring =  CATGETS(Set_AgentLabel, 168, "Display Link");
  XtVaSetValues(widget, XmNlabelString, (XmString)mtfstring, NULL);

  widget = XmCreatePushButton(f_preview_menu, (char*)"widget", NULL, 0);
  XtAddCallback(widget, XmNactivateCallback, open_new_nodeCB, this);
  XtManageChild(widget);
  mtfstring =  CATGETS(Set_AgentLabel, 64, "New Window");
  XtVaSetValues(widget, XmNlabelString, (XmString)mtfstring, NULL);

  // create default popup menu
  f_default_menu = XmCreatePopupMenu(
                       f_help_dsp_area->dispWid, (char*)"default_menu",NULL, 0);

  f_node_next3 = XmCreatePushButton(f_default_menu, (char*)"f_node_next3", NULL, 0);
  XtAddCallback(f_node_next3, XmNactivateCallback, node_nextCB, this);
  XtManageChild(f_node_next3);
  mtfstring =  CATGETS(Set_AgentLabel, 96, "Next Section");
  XtVaSetValues(f_node_next3, XmNlabelString, (XmString)mtfstring, NULL);

  f_node_prev3 = XmCreatePushButton(f_default_menu, (char*)"f_node_prev3", NULL, 0);
  XtAddCallback(f_node_prev3, XmNactivateCallback, node_previousCB, this);
  XtManageChild(f_node_prev3);
  mtfstring =  CATGETS(Set_AgentLabel, 94, "Previous Section");
  XtVaSetValues(f_node_prev3, XmNlabelString, (XmString)mtfstring, NULL);

  widget = XmCreateSeparator(f_default_menu, (char*)"sep", NULL, 0);
  XtManageChild(widget);

  widget = XmCreatePushButton(f_default_menu, (char*)"widget", NULL, 0);
  XtAddCallback(widget, XmNactivateCallback, show_locatorCB, this);
  XtManageChild(widget);
  mtfstring =  CATGETS(Set_AgentLabel, 68, "Show Locator");
  XtVaSetValues(widget, XmNlabelString, (XmString)mtfstring, NULL);

  widget = XmCreateSeparator(f_default_menu, (char*)"sep", NULL, 0);
  XtManageChild(widget);

  widget = XmCreatePushButton(f_default_menu, (char*)"widget", NULL, 0);
  XtAddCallback(widget, XmNactivateCallback, printCB, this);
  XtManageChild(widget);
  mtfstring =  CATGETS(Set_AgentLabel, 70, "Print");
  XtVaSetValues(widget, XmNlabelString, (XmString)mtfstring, NULL);



  f_help_dsp_area->vScrollNotify = v_scroll_notify ;
  f_help_dsp_area->armCallback   = arm_callback;

  f_help_dsp_area->dtinfo = True;

  if (MB_CUR_MAX > 1) {

    // DtHelpDispAreaStruct cant_begin_chars
    WString cant_begin_chars = CATGETS(Set_NodeWindowAgent, 5, "");
    f_help_dsp_area->cant_begin_chars = cant_begin_chars.get_wstr();

    // DtHelpDispAreaStruct cant_end_chars
    WString cant_end_chars = CATGETS(Set_NodeWindowAgent, 6, "");
    f_help_dsp_area->cant_end_chars = cant_end_chars.get_wstr();

#ifdef LB_DEBUG
    int i;
    for (i = 0; f_help_dsp_area->cant_begin_chars[i]; i++);
    fprintf(stderr, "(DEBUG) %d chars, cant_begin_chars=%s\n",
					i, cant_begin_chars.get_mbstr());
    for (i = 0; f_help_dsp_area->cant_end_chars[i]; i++);
    fprintf(stderr, "(DEBUG) %d chars, cant_end_chars=%s\n",
					i, cant_end_chars.get_mbstr());
#endif

    _DtCanvasLoadMetrics((_DtCvHandle)f_help_dsp_area->canvas);

  }
  
  // install accelerators on f_viewport 
  XtInstallAccelerators(f_form, f_search_prev);
  XtInstallAccelerators(f_form, f_search_next);
  
// piggyback on the Help Area vertical scrollbar so we can scroll our
// mark icons
  XtAddCallback (f_help_dsp_area->vertScrollWid,
		 XmNdragCallback,
		 v_scroll_callback, this);

  XtAddCallback (f_help_dsp_area->vertScrollWid,
		 XmNincrementCallback,
		 v_scroll_callback, this);

  XtAddCallback (f_help_dsp_area->vertScrollWid,
		 XmNdecrementCallback,
		 v_scroll_callback, this);

  XtAddCallback (f_help_dsp_area->vertScrollWid,
		 XmNpageDecrementCallback,
		 v_scroll_callback, this);

  XtAddCallback (f_help_dsp_area->vertScrollWid,
		 XmNpageIncrementCallback,
		 v_scroll_callback, this);

  XtAddCallback (f_help_dsp_area->vertScrollWid,
		 XmNtoBottomCallback,
		 v_scroll_callback, this);

  XtAddCallback (f_help_dsp_area->vertScrollWid,
		 XmNtoTopCallback,
		 v_scroll_callback, this);

  XtAddCallback (f_help_dsp_area->vertScrollWid,
		 XmNvalueChangedCallback, 
		 v_scroll_callback, this);

  // piggyback on the Help Area horizontal scrollbar so we can
  // track the detached graphic graphic.
  XtAddCallback (f_help_dsp_area->horzScrollWid,
		 XmNdragCallback,
		 h_scroll_callback, this);

  XtAddCallback (f_help_dsp_area->horzScrollWid,
		 XmNincrementCallback,
		 h_scroll_callback, this);

  XtAddCallback (f_help_dsp_area->horzScrollWid,
		 XmNdecrementCallback,
		 h_scroll_callback, this);

  XtAddCallback (f_help_dsp_area->horzScrollWid,
		 XmNpageDecrementCallback,
		 h_scroll_callback, this);

  XtAddCallback (f_help_dsp_area->horzScrollWid,
		 XmNpageIncrementCallback,
		 h_scroll_callback, this);

  XtAddCallback (f_help_dsp_area->horzScrollWid,
		 XmNtoBottomCallback,
		 h_scroll_callback, this);

  XtAddCallback (f_help_dsp_area->horzScrollWid,
		 XmNtoTopCallback,
		 h_scroll_callback, this);

  XtAddCallback (f_help_dsp_area->horzScrollWid,
		 XmNvalueChangedCallback, 
		 h_scroll_callback, this);

  
  // Request MarkSelectionChanged message for all marks
  MarkCanvas::request (this);

  node_mgr().UAS_Sender<SelectionChanged>::request (this);


  // Search area
  widget = XmCreatePulldownMenu(search_area, (char*)"scope_menu", 0, 0);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, widget); n++;
  XtManageChild(widget = XmCreateOptionMenu(search_area, (char*)"scope", args, n));
  help_agent().add_help_cb (widget);

  textstore = new UAS_String(CATGETS(Set_AgentQHelp, 60,
					"Specify search scope"));
  add_quick_help(widget, (char*)*textstore);
  f_textstore.insert_item(textstore);

  mtfstring =  CATGETS(Set_AgentLabel, 59, "Search:");
  XtVaSetValues(widget, XmNlabelString, (XmString)mtfstring, NULL);

  // hookup the ScopeMenu object here
  f_scope_menu = new ScopeMenu (widget, TRUE); // (require current section scop) 

  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNleftWidget, widget); n++;

  f_search_text = (WXmTextField*)(Widget)
      WXmTextField (search_area, "text", AM, args, n);
  XtAddEventHandler (*f_search_text, EnterWindowMask, False,
		     (XtEventHandler) search_help, (XtPointer) this);
  XtAddEventHandler (*f_search_text, LeaveWindowMask, False,
		     (XtEventHandler) search_help, (XtPointer) this);
  XtAddEventHandler (*f_search_text, FocusChangeMask, False,
		     (XtEventHandler) search_help, (XtPointer) this);
  ON_ACTIVATE (*f_search_text,		text_callback);
  help_agent().add_help_cb (*f_search_text);

  widget = XtCreateManagedWidget("scope_editor", xmPushButtonWidgetClass,
			      search_area, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, scope_editorCB, 0);
  help_agent().add_help_cb (widget);

  mtfstring =  CATGETS(Set_AgentLabel, 46, "Scope Editor");
  XtVaSetValues(widget, XmNlabelString, (XmString)mtfstring, NULL);

  textstore = new UAS_String(CATGETS(Set_AgentQHelp, 11,
			"Specify search scope using the Scope Editor"));
  add_quick_help(widget, (char*)*textstore);
  f_textstore.insert_item(textstore);

  // set right attachment here since it cannot be set at creation time
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNrightWidget, widget); n++;
  XtSetValues(*f_search_text, args, n);
  
#ifdef NODEBUG
  /* -------- Debugging setup.  Should be a function. -------- */
  
  bool debugging = window_system().get_boolean_app_resource("debug");
  
  if (debugging && False)
    debug_cascade.Manage();
  debug_cascade.SubMenuId (debug_menu);
  ON_ACTIVATE (document_id, document_id_display);
#endif  

  XtManageChild(f_form);
  XtManageChild(menu_bar);
  if (XmToggleButtonGadgetGetState(tool_barT))
      XtManageChild(tool_bar);
  if (XmToggleButtonGadgetGetState(search_areaT))
      XtManageChild(search_area);
  XtManageChild(mainw);

  setStatus (eSuccess);
}

// /////////////////////////////////////////////////////////////////
// lock_toggle - callback for lock button
// /////////////////////////////////////////////////////////////////

void 
NodeWindowAgent::lock_toggleCB(Widget lock,
			       XtPointer client_data, XtPointer call_data)
{
  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  XmToggleButtonCallbackStruct *cdata =
      (XmToggleButtonCallbackStruct*) call_data;
  Arg args[3];
  int n;
  
  agent->f_locked = cdata->set;

  n = 0;
  if (agent->f_locked)
    {
	XtSetArg(args[n], XmNselectPixmap,
		 window_system().locked_pixmap(XtParent(lock))); n++;
	XtSetArg(args[n], XmNlabelPixmap,
		 window_system().semilocked_pixmap(XtParent(lock))); n++;
      }
  else
    {
	XtSetArg(args[n], XmNlabelPixmap,
		 window_system().unlocked_pixmap(XtParent(lock))); n++;
	XtSetArg(args[n], XmNselectPixmap,
		 window_system().semilocked_pixmap(XtParent(lock))); n++;
    }
  XtSetValues(lock, args, n);
  
  // NOTE: This needs to be real time, not bogus X event time. [DJB]
  agent->f_last_access_time = (unsigned int) cdata->event->xany.serial;
}

// /////////////////////////////////////////////////////////////////
// dismiss - callback for close button
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::dismissCB(Widget, XtPointer client_data, XtPointer)
{
  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  agent->dismiss();
}

void
NodeWindowAgent::dismiss()
{
  if (g_ignore_wm_delete)
    return;
  if (!XtIsSensitive(f_close)) {
    exitCB(0, (XtPointer) this, 0);
    return;
  }
  f_shell->Popdown();
  delete this;
}

// /////////////////////////////////////////////////////////////////
// search_on_selection - perform query on X selection
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::do_search_selection (const char* value, unsigned long)
{
#ifdef SEARCH_SELECTION_DEBUG
  fprintf(stderr, "search selection = %s\n", value);
#endif

  if (value == NULL || *value == '\0')
    return;

  unsigned char* p = (unsigned char*)value;
  for (; *p; p++)
    if (*p < ' ') *p = ' ';
  
  
#if 0
  Xassert (value != NULL);
#ifdef UseWideChars
  char* buffer = (char *)value;
  TML_CHAR_TYPE *p, *ws = new TML_CHAR_TYPE[strlen(buffer) + 1];
  int assert_var = mbstowcs(ws, buffer, strlen(buffer) + 1);
  assert( assert_var >= 0 );
  for (p = ws; *p; p++) { // looking for control chars
    if (*p < (TML_CHAR_TYPE)' ')
      *p = ' ';
  }
  int length = strlen(buffer) + 1;
  assert_var = wcstombs(buffer, ws, length);
  assert( assert_var >=  0 );
  delete[] ws;
#else
  char *p = (char *) value;
  while (*p != NULL)
    {
      if (*p < ' ' /* || *p > '~' DTS 13944 */ )
	*p = ' ';
      p++;
    }
  // make selection string a literal
  
  // 1) Scan string for quotes. 
  // make room for start and end quotes 
  
  int buflen = strlen((char *)value) + 3 ;
  char *buffer = new char[buflen] ;
  
  const char *src = (char *)value ;
  char *dest = buffer ;
  
  // make first character a quote 
  if (*src != '"')
    *dest++ = '"';
  else
    *dest++ = *src++ ;
  
  while (*src)
    {
      if (dest >= (buffer + buflen - 2))
	{
	  buflen += 16 ;
	  char *newbuf = new char[buflen] ;
	  strcpy(newbuf, buffer);
	  int depth = dest - buffer ;
	  delete buffer ;
	  buffer = newbuf ;
	  dest = buffer + depth ;
	}
      if (*src == '"')
	*dest++ = '\\' ; // escape the quote 
      
      *dest++ = *src++ ;
    }
  if (*(src - 1) == '"')
    {
      // if it ends with a quote, we escaped it, so remove the escape. 
      *(dest - 2)  = '"' ;	
      *(dest - 1) = 0 ; // terminate string 
    }
  else
    {
      // otherwise add our own quote
      *dest++ = '"' ;
      *dest = 0 ;
    }
#endif
#endif
  
  char *search_string = (char*)value;
  
  ON_DEBUG(printf("!! search on selection\n-->%s<--\n", (char*)value));
  
  f_search_text->Value(search_string);
  XmUpdateDisplay (window_system().toplevel());

  UAS_SearchScope *scope = f_scope_menu->current_scope();
  if (scope)
  {
    if (scope->search_zones().section())
      {
	node_mgr().set_preferred_window(this);
	search_mgr().search_section(f_node_view_info->node_ptr()->id());
#ifdef CURRENT_SECTION_DEBUG
        fprintf(stderr, "(DEBUG) search_section (before search)=%s\n",
				(char*)search_mgr().search_section());
#endif
      }

    {
      Wait_Cursor bob;
      search_mgr().parse_and_search(search_string, scope);
      search_mgr().search_section(UAS_String(""));
    }
  }
}

void
NodeWindowAgent::search_on_selectionCB(Widget, XtPointer client_data,
				       XtPointer call_data)
{
  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  XmPushButtonCallbackStruct* cdata =
      (XmPushButtonCallbackStruct *) call_data;
 
  get_selection_value(*agent->f_shell, XA_PRIMARY, agent,
		      cdata->event->xbutton.time);
}

// /////////////////////////////////////////////////////////////////
// search text callbacks
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::text_callback(WCallback *wcb)
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*)wcb->CallData();
    char *text = XmTextGetString(wcb->GetWidget());
    if (text == NULL)
      return;

    // Allow if debug resource is set, not ifdef DEBUG - 17:25 01/11/93 DJB
    UAS_SearchScope* scope = f_scope_menu->current_scope();
    if (scope != NULL && *text) // if context makes sense
    {
      if (scope->search_zones().section())
      {
	node_mgr().set_preferred_window(this);
	search_mgr().search_section(f_node_view_info->node_ptr()->id());
#ifdef CURRENT_SECTION_DEBUG
        fprintf(stderr, "(DEBUG) search_section (before search)=%s\n",
				(char*)search_mgr().search_section());
#endif
      }

      {
	Wait_Cursor bob;
	search_mgr().parse_and_search (text, scope);
	search_mgr().search_section (UAS_String(""));
      }
    }

    XtFree (text);
}

void
NodeWindowAgent::search_help (Widget, XtPointer client_data,
			      XEvent *event, Boolean *)
{

  // Make sure the event is what we expect.
  if (event->type != EnterNotify && event->type != FocusIn &&
      event->type != LeaveNotify && event->type != FocusOut)
    return;

  NodeWindowAgent* agent = (NodeWindowAgent*)client_data;
  if (agent == NULL)
    return;

  // clear quick help
  if (event->type == LeaveNotify || event->type == FocusOut)
  {
    XmTextFieldSetString(agent->f_status_text, (char*)"");
    return;
  }


  static String help_text	= NULL;
  static String default_scope	= NULL;
  
  if (help_text == NULL) {
    UAS_Pointer<UAS_String> str;

    str = new UAS_String(CATGETS(Set_AgentQHelp, 64, "Search %s"));
    help_text = (char*)*str;
    agent->f_textstore.insert_item(str);

    str = new UAS_String(CATGETS(Set_NodeWindowAgent, 3, "Information Library"));
    default_scope = (char*)*str;
    agent->f_textstore.insert_item(str);
  }
  
  // Format the search help string. 
  UAS_SearchScope *scope = agent->f_scope_menu->current_scope();
  char buffer[128];
  if (scope != NULL)
    sprintf (buffer, help_text, (char *) scope->name());
  else
    sprintf (buffer, help_text, default_scope);
  
  // Finally, display it in the quick help field.
  XmTextFieldSetString(agent->f_status_text, buffer);
}


// /////////////////////////////////////////////////////////////////
// history_display
// /////////////////////////////////////////////////////////////////

static void
section_historyCB(Widget, XtPointer, XtPointer)
{
  Wait_Cursor bob;
  ON_DEBUG (puts ("history_display() called"));
  global_history_mgr().display();
}

// /////////////////////////////////////////////////////////////////
// history_previous
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::history_prevCB(Widget, XtPointer client_data, XtPointer)
{
  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  UAS_Pointer<UAS_Common> doc_ptr = agent->f_history_list.previous();
  Xassert (doc_ptr != (UAS_Pointer<UAS_Common>)NULL);
  
#ifdef jbm
  g_history_anchor = agent->f_history_list.previous_anchor();
#endif  
  // Set the history_display flag which is checked in the display routine. 
  if (!agent->f_locked)
    {
      agent->f_history_display = TRUE;
      agent->record_history_position();
      agent->f_history_list.go_back();
    }
  
  node_mgr().set_preferred_window (agent);
  doc_ptr->retrieve();
}

// /////////////////////////////////////////////////////////////////
// history_next
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::history_nextCB(Widget, XtPointer client_data, XtPointer)
{
  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  UAS_Pointer<UAS_Common> doc_ptr = agent->f_history_list.next();
  Xassert (doc_ptr != (UAS_Pointer<UAS_Common>)NULL);
  
#ifdef jbm
  g_history_anchor = agent->f_history_list.next_anchor();
#endif
  
  // Set the history_display flag which is checked in the display routine. 
  if (!agent->f_locked)
    {
      agent->f_history_display = TRUE;
      agent->record_history_position();
      agent->f_history_list.go_forward();
    }
  
  node_mgr().set_preferred_window (agent);
  doc_ptr->retrieve();
}


// /////////////////////////////////////////////////////////////////
// history preview callbacks
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::preview_history_nextCB(Widget, XtPointer client_data,
					XtPointer)
{
  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  agent->f_preview_document = agent->f_history_list.next();
  agent->f_preview_timeout =
    new WTimeOut (window_system().app_context(), 550,
		  agent, (WTimeOutFunc) &NodeWindowAgent::preview);
}

void
NodeWindowAgent::preview_history_prevCB(Widget, XtPointer client_data,
					XtPointer)
{
  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  agent->f_preview_document = agent->f_history_list.previous();
  agent->f_preview_timeout =
    new WTimeOut (window_system().app_context(), 550,
		  agent, (WTimeOutFunc) &NodeWindowAgent::preview);
}


// /////////////////////////////////////////////////////////////////
// initialize_history - add the current node to hist and set arrows
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::initialize_history()
{
  // Add the new node to the history if it didn't come from the history
  // list itself.  Currently this has the effect of truncating the list
  // at the user's current position in the list. 
  if (!f_history_display)
    {
      UAS_Pointer<UAS_Common> bogus;
      f_history_list.append (bogus = f_node_ptr);
    }
  else
    f_history_display = FALSE;	// reset 
  
  if (f_history_list.previous() != (UAS_Pointer<UAS_Common>)NULL) {
    XtSetSensitive(f_history_prev, True);
    XtSetSensitive(f_history_prev2, True);
  } else {
    XtSetSensitive(f_history_prev, False);
    XtSetSensitive(f_history_prev2, False);
  }
  
  if (f_history_list.next() != (UAS_Pointer<UAS_Common>)NULL) {
    XtSetSensitive(f_history_next, True);
    XtSetSensitive(f_history_next2, True);
  } else {
    XtSetSensitive(f_history_next, False);
    XtSetSensitive(f_history_next2, False);
  }
}

// /////////////////////////////////////////////////////////////////
// display_version - display the version dialog
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::display_version()
{
  UAS_String version = CATGETS(Set_Version, 1, "");
  if (*(char*)version) {
    message_mgr().info_dialog ((char*)version);
  }
}

// /////////////////////////////////////////////////////////////////
// display_search_history
// /////////////////////////////////////////////////////////////////

static void
search_historyCB(Widget, XtPointer, XtPointer)
{
  search_mgr().search_history_list_view().display();
}

static void
open_urlCB(Widget, XtPointer client_data, XtPointer)
{
  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  url_mgr().display(agent);
}

// NOTE: There's got to be an easier way to attach callbacks!?
// Maybe something in the resource file associated with the button?
// There needs to be a central way to address things like a
// search history list view just by knowing where it it.  Maybe
// we should be able to call it's notify proc with some kind
// of message? Like some kind of real O-O or something!?
// 6/23/92 djb


// /////////////////////////////////////////////////////////////////
// update_fonts - user has scaled fonts
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::update_fonts (unsigned int serial_number)
{
  f_serial_number = serial_number ;
  // NOTE: do not call this from within ourselves,
  // it is to be called by the NodeMgr - jbm
  
}


// /////////////////////////////////////////////////////////////////
// initialize_controls - init ui controls based on current node
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::initialize_controls()
{
  initialize_hierarchy();
  initialize_tabs();
  select_tab();
  initialize_path();
  initialize_history();
  
  //
  // SWM: Need to disable searching if the node we're displaying is
  //      not searchable...
  //
  XtSetSensitive(f_search_menu_button,
		 f_node_view_info->node_ptr()->searchable());

  bool search_previous_sensitive = FALSE, search_next_sensitive = FALSE;

  if (f_node_view_info->node_ptr()->searchable() &&
      f_node_view_info->hit_entries() > 0) {
    if (f_node_view_info->search_hit_idx() > 0)
      search_previous_sensitive = TRUE;
    if (f_node_view_info->search_hit_idx() <
				f_node_view_info->hit_entries() - 1)
      search_next_sensitive = TRUE;
  }

  XtSetSensitive(f_search_prev, search_previous_sensitive);
  XtSetSensitive(f_search_prev2, search_previous_sensitive);
  XtSetSensitive(f_search_next, search_next_sensitive);
  XtSetSensitive(f_search_next2, search_next_sensitive);

  XtSetSensitive(f_print2, True);
  XtSetSensitive(f_print_as, True);

  XtSetSensitive(f_detach_graphic, True);

#ifdef NOTYET
  // See if next and previous exist and sensitize buttons accordingly.
  if (f_node_handle->node().next_node() != NULL)
    {
      XtSetSensitive(f_node_next, True);
      XtSetSensitive(f_node_next2, True);
    }
  else
    {
      XtSetSensitive(f_node_next, False);
      XtSetSensitive(f_node_next2, False);
    }
  
  if (f_node_handle->node().previous_node() != NULL)
    {
      XtSetSensitive(f_node_previous, True);
      XtSetSensitive(f_node_previous2, True);
    }
  else
    {
      XtSetSensitive(f_node_previous, False);
      XtSetSensitive(f_node_previous2, False);
    }
  
  
  int i;
  /* -------- See if the current node is in the current hierarchy. -------- */
  for (i = 9; i >= 0; i--)
    {
      if (!XtIsManaged (*f_title_button_list[i]))
	break;
      
      if (((NodeHandle *) f_title_button_list[i]->UserData())
	  ->equals (*f_node_handle))
	{
	  f_title_option->MenuHistory (*f_title_button_list[i]);
	  select_tab();
	  return;
	}
    }
  
  /* -------- Set up the title option menu for hierarchial path. -------- */
  
  ON_DEBUG(printf ("*** Unmanaging push buttons in option menu ***\n"));
  //  f_title_menu->Unmanage();
  for (i = 0; i < 10; i++)
    f_title_button_list[i]->Unmanage();
  
  NodeHandle *current_node = new NodeHandle (*f_node_handle);
  
  i = 9;
  while (current_node != NULL)
    {
      // NOTE: WWL WXmString needs fixing to take a const char *, djb
      const char *current_name = current_node->node().node_name();
      
      f_title_button_list[i]->
	LabelString (WXmString ((char *)current_name));
      ON_DEBUG(printf ("*** Remanaging button %d -- %s***\n", i,
		       current_name));
      f_title_button_list[i]->Manage();
      // Free old node associated with the button
      delete (NodeHandle *) f_title_button_list[i]->UserData();
      f_title_button_list[i]->UserData (current_node);
      if (--i < 0)
	{
	  // NOTE: This will need to be fixed, 7/8/92 djb
	  fputs ("Unreasonable include file nesting!", stderr);
	  fputs ("  Section hierarchy too deep!!", stderr);
	  abort();
	}
      
      current_node = current_node->node().parent_node();
      if (current_node != NULL)
	current_node = new NodeHandle (*current_node);
    }
  
  // OPTION MENU BUGGY STUFF BELOW: 
  //  f_title_menu->Manage();
  // Work around Motif 1.1 (1.2 ~too?) bug
  WXmCascadeButtonGadget opb (XmOptionButtonGadget (*f_title_option));
  opb.Unmanage();
  // Make the last item the visible one.  See Motif docs.
  f_title_option->MenuHistory (*f_title_button_list[9]);
  opb.Manage();
  //  printf ("*** Done with option menu changes ***\n");
  
  display_tabs (*((ObjectId *) f_title_button_list[i+1]->UserData()));
  select_tab();
#endif
}

// /////////////////////////////////////////////////////////////////////////
// printCB
// /////////////////////////////////////////////////////////////////////////

/*
 * ------------------------------------------------------------------------
 * Name: LibraryAgent::printCB
 *
 * Description:
 *
 *     Called when the user hits "Print" quick button.  Prints without
 *     displaying the Print setup dialog.
 *     
 */
void
NodeWindowAgent::printCB(Widget w, XtPointer client_data, XtPointer)
{
    NodeWindowAgent *agent = (NodeWindowAgent *)client_data;

    //  Get a handle to the AppPrintData allocated in the WindowSystem class

    AppPrintData* p = window_system().GetAppPrintData();

    //  Need to put this single node on the print list so that
    //  reset_ui() can determine the number of nodes being printed 

    xList<UAS_Pointer<UAS_Common> > * print_list = new xList<UAS_Pointer<UAS_Common> >;
    UAS_Pointer<UAS_Common> bogus;
    bogus = agent->f_node_view_info->node_ptr();
    print_list->append(bogus);
    
    p->f_outline_element = NULL;
    p->f_print_list = print_list;
 
    CreatePrintSetup(w, p);

    // check if the DtPrintSetupBox ("Print...") has been called yet 

    if(p->f_print_data->print_display == (Display*)NULL)
    {
	
	// first time thru print setup, so get default data 
	
        if (DtPrintFillSetupData(p->f_print_dialog, p->f_print_data)
	    != DtPRINT_SUCCESS) {

	    // NOTE: DtPrintFillSetupData() already posts an error
	    // dialog on failure - no need to post our own.

	    return ;
	}
    }
 
    DoPrint(w, p) ;

}

/*
 * ------------------------------------------------------------------------
 * Name: LibraryAgent::print_asCB
 *
 * Description:
 *
 *     This is called for "Print..." and will always bring up the
 *     print setup dialog.
 */
void
NodeWindowAgent::print_asCB(Widget w, XtPointer client_data, XtPointer)
{
    NodeWindowAgent *agent = (NodeWindowAgent *)client_data;

    AppPrintData* p = window_system().GetAppPrintData();

    //  Need to put this single node on the print list so that
    //  reset_ui() can determine the number of nodes being printed 

    xList<UAS_Pointer<UAS_Common> > * print_list = new xList<UAS_Pointer<UAS_Common> >;
    UAS_Pointer<UAS_Common> bogus;
    bogus = agent->f_node_view_info->node_ptr();
    print_list->append(bogus);
    
    p->f_print_list = print_list;
    p->f_outline_element = NULL;
 
    CreatePrintSetup(w, p);
    
    XtManageChild(p->f_print_dialog); /* popup dialog each time */
}

// /////////////////////////////////////////////////////////////////
// map_activate
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::map_activate()
{
  Wait_Cursor bob;
  UAS_Pointer<UAS_Common> bogus;
  map_mgr().display_centered_on (bogus = f_node_ptr);
}


// /////////////////////////////////////////////////////////////////
// search previous 
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::search_previousCB(Widget, XtPointer client_data, XtPointer)
{
  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  agent->search_previous();
}

void
NodeWindowAgent::search_previous()
{
    NodeViewInfo::trav_status_t status;
    status = f_node_view_info->
		adjust_current_search_hit(NodeViewInfo::PREV);

    if (status != NodeViewInfo::NOT_MOVED)
      {

	if (! XtIsSensitive(f_search_next))
	  {
	    XtSetSensitive(f_search_next, True);
	    XtSetSensitive(f_search_next2, True);
	  }
	if (status == NodeViewInfo::REACH_LIMIT)
	  {
	    XtSetSensitive(f_search_prev, False);
	    XtSetSensitive(f_search_prev2, False);
	  }

	_DtCvUnit width, height;
	_DtCanvasResize((_DtCvHandle)f_help_dsp_area->canvas,
						_DtCvTRUE, &width, &height);

	_DtCanvasRender((_DtCvHandle)f_help_dsp_area->canvas, 0, 0,
		width, height, _DtCvRENDER_PARTIAL, _DtCvFALSE, NULL, NULL);

	_DtHelpSearchMoveTraversal((XtPointer)f_help_dsp_area,
					f_node_view_info->search_hit_idx());
      }
}

// /////////////////////////////////////////////////////////////////
// search next
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::search_nextCB(Widget, XtPointer client_data, XtPointer)
{
  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  agent->search_next();
}

void
NodeWindowAgent::search_next()
{
    NodeViewInfo::trav_status_t status;
    status = f_node_view_info->
		adjust_current_search_hit(NodeViewInfo::NEXT);

    if (status != NodeViewInfo::NOT_MOVED)
      {

	if (! XtIsSensitive(f_search_prev))
	  {
	    XtSetSensitive(f_search_prev, True);
	    XtSetSensitive(f_search_prev2, True);
	  }
	if (status == NodeViewInfo::REACH_LIMIT)
	  {
	    XtSetSensitive(f_search_next, False);
	    XtSetSensitive(f_search_next2, False);
	  }
	
	_DtCvUnit width, height;
	_DtCanvasResize((_DtCvHandle)f_help_dsp_area->canvas,
						_DtCvTRUE, &width, &height);

	_DtCanvasRender((_DtCvHandle)f_help_dsp_area->canvas, 0, 0,
		width, height, _DtCvRENDER_PARTIAL, _DtCvFALSE, NULL, NULL);

	_DtHelpSearchMoveTraversal((XtPointer)f_help_dsp_area,
					f_node_view_info->search_hit_idx());
      }
}


// /////////////////////////////////////////////////////////////////
// display_search_scope
// /////////////////////////////////////////////////////////////////

static void
scope_editorCB(Widget, XtPointer, XtPointer)
{
  search_scope_mgr().display();
}


#ifdef DEBUG
// /////////////////////////////////////////////////////////////////
// routines for items on the debug menu
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::document_id_display()
{
}

#endif


// /////////////////////////////////////////////////////////////////
// initialize_tabs - set up the tabs for this book
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::initialize_tabs()
{
  List_Iterator<UAS_Pointer<UAS_Common> > tabs (g_tab_list);
  List_Iterator<BookTab *> btns (f_tab_btn_list);
  BookTab *current;
  bool changed = FALSE;
  static int old_count = -1;
  int count = 0;
  
  while (tabs)
    {
      // Update exisiting tab buttons, adding new ones when necessary. 
      if (btns)
	{
	  // Only update the tab button if it has changed since last time.
	  if (btns.item()->tab() == (UAS_Pointer<UAS_Common>)NULL ||
	      btns.item()->tab() != tabs.item())
	    {
	      ON_DEBUG (printf ("  Updating\n"));
	      current = btns.item();
	      current->set_tab (tabs.item());
	      changed = TRUE;
	    } else {
	    }
	  btns++;
	}
      else
	{
	  ON_DEBUG (printf ("Creating a tab\n"));
	  // Add a new button.  The tabs have definitely changed.
	  current = new BookTab (this, *f_tab_area, tabs.item());
	  f_tab_btn_list.append (current);
	  changed = TRUE;
	}
      count++;
      g_tab_list.remove (tabs); // remove item and increment iterator 
    }
  
  // Unset any remaining buttons.
  UAS_Pointer<UAS_Common> null_tab;
  for (; btns; btns++)
    {
      if (btns.item()->tab() != (UAS_Pointer<UAS_Common>)NULL)
	{
	  ON_DEBUG (printf ("Emptying a tab\n"));
	  btns.item()->set_tab (null_tab);
	  changed = TRUE;
	}
    }
  
  if (!changed)
    return;
  
  ON_DEBUG (printf ("Tabs have changed (%d)\n", count));
  
  // Make the tab area appear or disappear as needed.
  if (count == 0 && (old_count > 0 || old_count == -1))
    f_frame->TopOffset (-12);
  else if (count > 0 && old_count <= 0)
    f_frame->TopOffset (-4);
  
  // Work around bogus Motif bug that prevents things from resizing
  // correctly if the tabs change but there's still the same number.
  if (old_count == count)
    {
      current->Unmanage();
      current->Manage();
    }
  else
    {
      old_count = count;
    }
}


// /////////////////////////////////////////////////////////////////
// select_tab - if current node has a tab, select that tab
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::select_tab()
{
  List_Iterator<BookTab *> btns (f_tab_btn_list);
  UAS_Pointer<UAS_Common> d;
  
  while (btns && XtIsManaged (*btns.item()))
    {
      btns.item()->select_if_same (d = f_node_view_info->node_ptr());
      btns++;
    }
}

// /////////////////////////////////////////////////////////////////
// preview_init
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::preview_init (UAS_Pointer<UAS_Common> &doc_ptr)
{
  // NOTE: Link preview timeout shouldn't be hardcoded! DJB 10/7/92
  f_preview_document = doc_ptr;
  f_preview_timeout =
    new WTimeOut (window_system().app_context(), 550,
		  this, (WTimeOutFunc) &NodeWindowAgent::preview);
}


// /////////////////////////////////////////////////////////////////
// preview - display the link preview
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::preview (WTimeOut *)
{
  char title[128];
  char preview_buffer[256];
  
  UAS_String pt = f_preview_document->title();
  strncpy (title, (char *) pt, 127);
  title[127] = '\0';
  UAS_String bn = f_preview_document->book_name(UAS_SHORT_TITLE);
  const char *book_name = (char *) bn;
  if (book_name != NULL && *book_name != '\0')
    sprintf (preview_buffer, CATGETS(Set_Messages, 8, "Link to %s: %s"),
	     book_name, title);
  else
    sprintf (preview_buffer, CATGETS(Set_Messages, 9, "Link to %s"),
	     title);
  XmTextFieldSetString(f_status_text, preview_buffer);
  f_preview_timeout = NULL;
  f_preview_document = NULL;
}


// /////////////////////////////////////////////////////////////////
// unpreview
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::unpreviewCB(Widget, XtPointer client_data, XtPointer)
{
    NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
    agent->unpreview();
}

void
NodeWindowAgent::unpreview()
{
  // Cancel document preview...
  if (f_preview_timeout != NULL)
    {
      // If the timeout hasn't triggered delete it and clear the document. 
      delete f_preview_timeout;
      // XXX SWM -- zero out the timeout
      f_preview_timeout = NULL;
    }
  else
    {
      // If the timeout has already happened, just clear the preview area. 
      XmTextFieldSetString(f_status_text, (char*)"");
    }
}



// /////////////////////////////////////////////////////////////////
// create_annotation/bookmark - create a new user mark
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::create_annotationCB(Widget, XtPointer client_data, XtPointer)
{
  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  agent->make_bookmark(True);
}

void
NodeWindowAgent::create_bookmarkCB(Widget, XtPointer client_data, XtPointer)
{
  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  agent->make_bookmark(False);
}

int
NodeWindowAgent::make_bookmark (Boolean edit, MarkCanvas* refmark)
{
  Wait_Cursor bob;

  UAS_Pointer<UAS_Common> d;

  UAS_String bm_name, bm_anno;

  int status = -1; // default return status (fail)

  if (refmark == NULL)
  {
    char *name;
    _DtCanvasGetSelection(f_help_dsp_area->canvas,
			  _DtCvSELECTED_TEXT, ((_DtCvPointer *)&name));

#ifdef BOOKMARK_DEBUG
    cerr << "Bookmark Name: [" << name << "]" << endl;
#endif

    //Xassert (name != NULL);
    if (name == NULL)
    {
      message_mgr().error_dialog ((char*)UAS_String(CATGETS(Set_Messages, 10,
			"Dtinfo is unable to create this bookmark.")),
                          (Widget)f_shell);
      return status;
    }

    // replace control char with a space
    for (unsigned char* p = (unsigned char*)name; *p; p++)
      *p = (*p < ' ') ? ' ' : *p;

    int len, next_mblen;
    for (len = 0, next_mblen = 0; len < 40 && name[len]; len += next_mblen)
    {
      if ((next_mblen = mblen(name + len, MB_CUR_MAX)) < 0)
	break;
      else if (len + next_mblen > 40)
	break;
    }
    name[len] = 0;

    bm_name = name;

    free (name);
  }
  else {
    bm_name = refmark->mark_ptr()->name();
    bm_anno = refmark->mark_ptr()->notes();
  }

#ifdef BOOKMARK_DEBUG
   cerr << "Creating mark <" << (char*)bm_name << ">" << endl;
#endif

  _DtCvSegPts **segs = NULL ;
  mtry
  {
      int ret_x, ret_y ;
      // find the selection segment and offset
      _DtCanvasGetSelectionPoints(f_help_dsp_area->canvas,
				  &segs, 
				  &ret_x, &ret_y); // ret_x, ret_y

      // NOTE: have to allow for segments that are added in by the
      // style sheet -> check appflag1
      // also have to account for it at the other end as well

      if (segs == NULL || *segs == NULL)
	throw(CASTEXCEPT Exception());

      // sort segments in order of vcc offset
      _DtCvSegPts** segpts_iter;
      for (segpts_iter = segs; *segpts_iter; segpts_iter++) {
	_DtCvSegPts* *p = segpts_iter + 1;
	for (; *p; p++)
	{
	  if ((NodeViewInfo::segment_to_vcc((*segpts_iter)->segment) >
	       NodeViewInfo::segment_to_vcc((*p)->segment)))
	  {
	    _DtCvSegPts* anon = *segpts_iter;
	    *segpts_iter = *p;
	    *p = anon;
	  }
	}
      }

#ifdef BOOKMARK_DEBUG
      for (segpts_iter = segs; *segpts_iter; segpts_iter++) {
	_DtCvSegPts& segpts = **segpts_iter;
	fprintf(stderr, "(DEBUG) offset=%d,len=%d\n",
				segpts.offset, segpts.len);
      }
#endif

      MarkInfo markinfo(bm_name, bm_anno);

      _DtCvSegPts* prev_segpts = NULL;
      unsigned int vcc         = 0;
      unsigned int length      = 0;
      unsigned int offset      = 0;

      int i;
      for (i = 0; segs[i]; i++)
      {
	_DtCvSegPts* segpts = segs[i];

        if (prev_segpts && prev_segpts->segment != segpts->segment) {

	  // new segment, save the previous one
	  markinfo.insert_item(UAS_Pointer<MarkUnitInfo>(
		new MarkUnitInfo(vcc, offset, length, prev_segpts->segment)));

	  if (NodeViewInfo::segment_to_vcc(segpts->segment)==(unsigned int)-1)
	  {
	    // ignore it, initialize
	    vcc = length = offset = 0;
	    prev_segpts = NULL;
	    continue;
	  }
	  else {
	    offset = segpts->offset;
	    vcc = NodeViewInfo::segment_to_vcc(segpts->segment);
	    length = 0;
	  }
	}
	else if (prev_segpts) {
	  if (segpts->offset > prev_segpts->offset + prev_segpts->len)
	      length += segpts->offset - (prev_segpts->offset +
					  prev_segpts->len);
	}
	else {
	  offset = segpts->offset;
	  vcc = NodeViewInfo::segment_to_vcc(segpts->segment);
	}

	length += segpts->len;

	prev_segpts = segpts;
      }

      if (prev_segpts)
	markinfo.insert_item(UAS_Pointer<MarkUnitInfo>(
		new MarkUnitInfo(vcc, offset, length, prev_segpts->segment)));

      // remove bad (vcc < 0) MarkUnitInfo from markinfo
      MarkInfo bad_markinfo;
      for (i = 0; i < markinfo.length(); i++) {
	UAS_Pointer<MarkUnitInfo>& mui = markinfo[i];
	if (mui->vcc() == (unsigned int)-1)
	  bad_markinfo.insert_item(mui);
      }

      for (i = 0; i < bad_markinfo.length(); i++)
	markinfo.remove_item(bad_markinfo[i]);

#ifdef BOOKMARK_DEBUG
      for (i = 0; i < markinfo.length(); i++)
      {
	MarkUnitInfo& mui = *markinfo[i];
	fprintf(stderr, "(DEBUG) item%d: vcc=%d,offset=%d,length=%d\n",
			i, mui.vcc(), mui.offset(), mui.length());
      }
#endif

#ifdef BOOKMARK_DEBUG      
      cerr << "selection at: ( " << ret_x << ", " << ret_y << ")" << endl;
#endif

      // find the offset of the first segment from the top of the node
      // NOTE - should really do it from the nearest locator

      if (markinfo.length() > 0)
      {
	  // persistent mark info should be a list of
	  // vcc of the segment, internal offset, internal length

	  UAS_Pointer<Mark> mark =
	    mark_mgr().create_mark (d = f_node_ptr,
				    AnchorCanvas(f_node_ptr->locator(),
						 markinfo),
				    (char*)markinfo.name(),
				    (char*)markinfo.anno());

	  if (edit && (mark != (UAS_Pointer<Mark>)NULL))
	  {
	      mark->edit();
	  }

          XtSetSensitive(f_create_bmrk, False);
          XtSetSensitive(f_create_anno, False);

	  status = 0; // success
    }
    else if (bad_markinfo.length() > 0)
	message_mgr().error_dialog (
		(char*)UAS_String(CATGETS(Set_NodeWindowAgent, 7,
		"The selected text is dynamically inserted and cannot\n"
		"be used alone to create a bookmark reference. Either\n"
		"select other text near the point, or include more of\n"
		"the surrounding text.")),
                          (Widget)f_shell);
    else
	throw(CASTEXCEPT Exception());
  }
  mcatch_any()
  {
      if (! refmark)
	message_mgr().error_dialog ((char*)UAS_String(CATGETS(Set_Messages, 10,
		"Dtinfo is unable to create this bookmark.")),
                          (Widget)f_shell);
  }
  end_try;

  if (segs) {
      for (int i = 0; segs[i]; i++)
        free (segs[i]);
      free (segs);
  }

  return status;
}


// /////////////////////////////////////////////////////////////////
// selection_changed - do stuff based on the current selection
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::selection_changed()
{
#ifdef SELECTION_DEBUG
  cerr << "NodeWindowAgent::selection_changed()" << endl;
#endif
  SelectionChanged msg ;
  UAS_Sender<SelectionChanged>::send_message (msg);
}

bool
NodeWindowAgent::selection_contains_graphic()
{
  _DtCvStatus status ;
  _DtCvSegPts **segs ;

  status=
      _DtCanvasGetSelectionPoints(f_help_dsp_area->canvas, // canvas
				  &segs,		  // returned segments
				  NULL, NULL);	       // ret_x, ret_y

  if ((_DtCvSTATUS_OK == status) && (NULL != segs))
    {
      for (int i = 0 ; segs[i] ; i++)
	if (_DtCvIsSegRegion(segs[i]->segment))
	  {
	    return TRUE ;
	  }
    }
  return FALSE ;
}


void
NodeWindowAgent::receive (SelectionChanged &, void* /*client_data*/)
{
#ifdef SELECTION_DEBUG
  cerr << "NodeWindowAgent::receive(<SelectionChanged>) --> " ;
  if (f_help_dsp_area->primary)
    cerr << "true" << endl;
  else
    cerr << "false" << endl;
  if (f_help_dsp_area->text_selected)
    cerr << "text_selected: true" << endl;
  else
    cerr << "text_selected: false" << endl;
#endif

#if 1
  Boolean sensitivity  = f_help_dsp_area->text_selected;
#else
  // primary is the wrong flag to use here--it is always
  // true (except before the first time text has been selected),
  // so the create mark menu is always active.
  Boolean sensitivity  = f_help_dsp_area->primary ;
#endif

  XtSetSensitive(f_create_bmrk, sensitivity);
  XtSetSensitive(f_create_anno, sensitivity);

  if (MarkCanvas::selected_mark() &&
      MarkCanvas::selected_mark()->agent() == this)
  {
#ifdef MOVE_MARK_DEBUG
    fprintf(stderr, "(DEBUG) agent=0x%x move_mark sensitivity=%d\n",
					(void*)this, sensitivity);
#endif
    XtSetSensitive(f_move_mark, sensitivity);
  }
  
#ifdef JBM
  // Update the move function if a Mark is selected somewhere. 
  if (MarkCanvas::selected_mark() != NULL)
    f_move_mark->SetSensitive (sensitivity);
#endif
  

#ifdef EAM
  if (sensitivity)
    XtSetSensitive(f_detach_graphic, selection_contains_graphic());
  else
    XtSetSensitive(f_detach_graphic, False);
#endif
}


// /////////////////////////////////////////////////////////////////
// mark_selection_changed
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::receive (MarkSelectionChanged &message, void* /*client_data*/)
{
  Boolean sensitivity;
  if (message.f_selection_type == MarkSelectionChanged::SELECT)
    sensitivity = True;
  else
    sensitivity = False;
  
  // Update edit and delete controls based on mark selection.
  // We only consider marks that are selected in our viewport. 
  if (MarkCanvas::selected_mark()->agent() == this)
    {
      XtSetSensitive(f_edit_mark, sensitivity);
      XtSetSensitive(f_delete_mark, sensitivity);
      XtSetSensitive(f_move_mark,
		     sensitivity && f_help_dsp_area->text_selected);

      if (sensitivity)
	select_mark_in_canvas(MarkCanvas::selected_mark(), message.f_move_to);
      else
	deselect_mark_in_canvas(MarkCanvas::selected_mark());
    }
  
#ifdef JBM
  // Update the move function only if our viewport has the selection.d 
  if (f_viewport_agent->selection_range() != NULL)
    f_move_mark->SetSensitive (sensitivity);
#endif
}

void
NodeWindowAgent::deselect_mark_in_canvas(MarkCanvas *mark)
{
  if (mark == NULL)
    return;

  _DtCvPointInfo point_info;

  point_info.client_data = mark;	// this is how we
					// identify it
  
  _DtCanvasActivatePts (f_help_dsp_area->canvas, // canvas
			_DtCvACTIVATE_MARK_OFF,	 // mask
			&point_info,		 // mark identification
			NULL,			 // *ret_y1
			NULL			 // *ret_y2
			);
}

void
NodeWindowAgent::select_mark_in_canvas(MarkCanvas *mark_to_show, bool show_it)
{
#ifdef BOOKMARK_SELECT_DEBUG
  cerr << "select_mark_in_canvas" << endl;
#endif
  _DtCvUnit ret_x, ret_y, ret_baseline, ret_height ;

  _DtCvStatus status = 
    _DtCanvasMoveTraversal(f_help_dsp_area->canvas, // canvas
			   _DtCvTRAVERSAL_MARK,	    // traverse to id
			   _DtCvFALSE,		    // no wrapping of traversal
			   _DtCvTRUE,		    // render - what should this be?
			   mark_to_show,	    // mark id
			   &ret_x, 
			   &ret_y, 
			   &ret_baseline, 
			   &ret_height);

  _DtCvPointInfo point_info ;
  
  List_Iterator <MarkCanvas *> m (f_mark_list);
  
  for (; m; m++)
  {
    point_info.client_data = m.item();	// this is how we
					// identify it

    _DtCanvasActivatePts (f_help_dsp_area->canvas, // canvas
			_DtCvACTIVATE_MARK_OFF,	 // mask
			&point_info,		 // mark identification
			NULL,			 // *ret_y1
			NULL			 // *ret_y2
			);
  }

  point_info.client_data = mark_to_show;
  _DtCvStatus status2 =
    _DtCanvasActivatePts (f_help_dsp_area->canvas,
			  _DtCvACTIVATE_MARK_ON, // mask
			  &point_info,		 // info
			  0,			 // ret_y1
			  0			 // ret_y2
			  );
  // if requested, attempt to bring it into focus

  // this may make more sense to move it into the canvas code, but I'm
  // first trying to do it from the application level

  if ((_DtCvSTATUS_OK == status) && show_it)
    {
      if ((ret_y > (f_help_dsp_area->firstVisible + 10)) &&
	  (ret_y <= (f_help_dsp_area->firstVisible + f_help_dsp_area->dispUseHeight)))
	{
	  /* already on screen */
	  /* do nothing */
	}
      else
	{
	  _DtCvUnit the_position = ret_y ;
	  if ((the_position + f_help_dsp_area->dispUseHeight) > f_help_dsp_area->maxYpos)
	    {
	      // avoid going beyond top of last page
	      the_position = f_help_dsp_area->maxYpos - f_help_dsp_area->dispUseHeight ;
	    }
	  else
	    {
	      the_position -= 2/3 * f_help_dsp_area->dispUseHeight ;
	    }
	  if (the_position < 0)
	    the_position = 0 ;

#ifdef BOOKMARK_JUMP_DEBUG
	  cerr << "Clean and draw at: " << the_position << "\twas: " << f_help_dsp_area->firstVisible << endl;
#endif
	  f_help_dsp_area->firstVisible = the_position ;

	  // adjust the scrollbar
	  Arg arg[1] ; 
	  XtSetArg(arg[0], XmNvalue, the_position);
	  XtSetValues(f_help_dsp_area->vertScrollWid, arg, 1);
	  
	  // call the scrollbar notify callback because we adjusted it
	  if (f_help_dsp_area->vScrollNotify)
	    (f_help_dsp_area->vScrollNotify)(this, the_position);

	}
    }
}

// /////////////////////////////////////////////////////////////////
// edit_mark - edit the selected mark
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::edit_markCB(Widget, XtPointer, XtPointer)
{
  Xassert (MarkCanvas::selected_mark() != NULL);
  MarkCanvas::selected_mark()->mark_ptr()->edit();
}


// /////////////////////////////////////////////////////////////////
// delete_mark - delete the selected mark
// /////////////////////////////////////////////////////////////////
void
NodeWindowAgent::delete_markCB(Widget, XtPointer client_data, XtPointer)
{
  NodeWindowAgent* agent = (NodeWindowAgent*)client_data;
  agent->delete_mark();
}

void
NodeWindowAgent::delete_mark()
{
  Xassert (MarkCanvas::selected_mark() != NULL);
  mtry
    {
      MarkCanvas::selected_mark()->mark_ptr()->remove();

      XtSetSensitive(f_create_bmrk, False);
      XtSetSensitive(f_create_anno, False);
    }
  mcatch_any()
    {
      message_mgr().error_dialog (
		(char*)UAS_String(CATGETS(Set_Messages, 11,
			"Dtinfo is unable to delete this bookmark.")),
                          (Widget)f_shell);
    }
  end_try;
}


// /////////////////////////////////////////////////////////////////
// move_mark - move the selected mark to the selected text region
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::move_mark()
{
  // f_move_mark must be insensetive if there's no mark selected
  if (MarkCanvas::selected_mark() == NULL ||
      MarkCanvas::selected_mark()->agent() != this) {
#ifdef BOOKMARK_DEBUG
    cerr << "bookmark not selected in this reading window" << endl;
#endif
    return;
  }

  if (! (make_bookmark(False, MarkCanvas::selected_mark()) < 0))
    delete_mark();
}

void
NodeWindowAgent::move_markCB(Widget, XtPointer client_data, XtPointer)
{
  NodeWindowAgent* agent = (NodeWindowAgent*)client_data;
  agent->move_mark();

#ifdef JBM
  Xassert (MarkCanvas::selected_mark() != NULL);
  Xassert (f_viewport_agent->selection_range() != NULL);
  UAS_Pointer<Mark> mark_ptr = MarkTml::selected_mark()->mark_ptr();
  Wait_Cursor bob;
  // Create a new mark based on the old one at the new location.
  mtry
    {
      f_viewport_agent->mark_creator (this); // 94/10/27 yuji bug fix for TC-530
      UAS_Pointer<UAS_Common> d;
      mark_mgr().move_mark (d = f_node_ptr,
			    AnchorCanvas (f_viewport_agent->selection_range()),
			    mark_ptr);
    }
  mcatch_any()
    {
      message_mgr().error_dialog (
		(char*)UAS_String(CATGETS(Set_Messages, 12,
			"Dtinfo is unable to remove this bookmark.")),
                          (Widget)f_shell);
    }
  end_try;
#endif
}

void
NodeWindowAgent::set_min_size()
{
  Dimension width, height, vwidth, vheight;
  
  f_shell->Get (WArgList (XmNheight, (XtArgVal) &height,
			  XmNwidth, &width, NULL));

  Widget wid = XtParent(XtParent(f_help_dsp_area->dispWid));
  WXmFrame(wid).Get(
		    WArgList (XmNheight, (XtArgVal) &vheight,
			      XmNwidth, &vwidth, NULL)
		    );
  
  
  ON_DEBUG (printf ("shell size = %d x %d\n", width, height));
  ON_DEBUG (printf ("viewport size = %d x %d\n", vheight, vwidth));
  
  f_shell->Set (WArgList (XmNminHeight, height - vheight + 60,
			  XmNminWidth, width - vwidth + 170, NULL));
}

void
NodeWindowAgent::detach_graphicCB(Widget, XtPointer client_data, XtPointer)
{
  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  agent->detach_graphic();
}

void
NodeWindowAgent::dialog_mapped (Widget w, XtPointer client_data,
                          XEvent *, Boolean *)
{
  XmUpdateDisplay (w);

  *((int *) client_data) = TRUE;
}


void
NodeWindowAgent::detach_graphic()
{
  Wait_Cursor wc ; 
  XEvent event;
  static Cursor pick_cursor;


  // Display an instructional dialog.
  WXmMessageDialog
    info_dialog ((Widget)f_shell, (char*)"detach_msg",
                 WArgList (XmNdialogType, XmDIALOG_INFORMATION, NULL));
  WXmDialogShell shell (info_dialog.Parent());
  // set window title
  String string = CATGETS(Set_Messages, 72, "Dtinfo: Detach Graphic");
  XtVaSetValues((Widget)shell, XmNtitle, string, NULL);
  info_dialog.MessageString (
        (char*)UAS_String(CATGETS(Set_Messages, 73, "Click on graphic")));
  XtUnmanageChild (info_dialog.OkPB());
  XtUnmanageChild (info_dialog.HelpPB());

  XtVaSetValues((Widget)info_dialog, XmNdefaultPosition, False, NULL);

  // Wait for the dialog to appear.
  int mapped = FALSE;
  info_dialog.Manage();
  XtAddEventHandler (info_dialog, ExposureMask, False,
                     dialog_mapped, (XtPointer) &mapped);
  XtAppContext app_context = window_system().app_context();
  while (!mapped)
  {
    XtAppNextEvent (app_context, &event);
    XtDispatchEvent (&event);
  }

  pick_cursor = XCreateFontCursor (window_system().display(), XC_crosshair);
  Widget target = XmTrackingEvent ((Widget)f_shell, pick_cursor, False, &event);

  if(target != NULL && target == f_help_dsp_area->dispWid)
  {
    _DtCvSegment *seg;
    _DtCvElemType element;
    seg = xy_to_seg(event.xbutton.x, event.xbutton.y, &element);
    if (seg && element == _DtCvREGION_TYPE)
    {
      SegClientData* pSCD = (SegClientData*)(seg->client_use);
      UAS_Pointer<Graphic> gr = (Graphic*)pSCD->GraphicHandle();

    if (!gr->is_detached())
        graphics_mgr().detach (f_node_view_info->node_ptr(), gr);
    }
  }

  // Nuke the dialog.
  info_dialog.Unmanage();
  XtDestroyWidget (info_dialog.Parent());
}

// /////////////////////////////////////////////////////////////////
// show_locator
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::show_locatorCB(Widget, XtPointer closure, XtPointer)
{
  NodeWindowAgent *agent = (NodeWindowAgent *)closure;
  agent->show_locator();
}

void
NodeWindowAgent::show_locator()
{
  // initially this only gets the locator for the current section
  // and does not go into a section
  char buffer[BUFSIZ];
  //const char *locator = f_node_ptr->locator();
  UAS_String locator_str = f_node_ptr->id();
  const char *locator = (const char *)locator_str;
  sprintf (buffer,
	   CATGETS(Set_NodeWindowAgent, 4,
                   "The locator for this section is %s"), locator);
  XmTextFieldSetString(f_status_text, buffer);
  XmTextFieldSetSelection(f_status_text,
			  strlen(buffer) - strlen(locator),
                          strlen(buffer), CurrentTime);
}


// /////////////////////////////////////////////////////////////////////////
// Preferences
// /////////////////////////////////////////////////////////////////////////

static void
prefsCB(Widget, XtPointer, XtPointer)
{
  pref_mgr().display();
}

// /////////////////////////////////////////////////////////////////
// list_marks - pop up the marks list
// /////////////////////////////////////////////////////////////////

static void
mark_listCB(Widget, XtPointer, XtPointer)
{
  mark_mgr().display_mark_list();
}

// /////////////////////////////////////////////////////////////////////////
// Query Editor
// /////////////////////////////////////////////////////////////////////////

static void
query_editorCB(Widget, XtPointer, XtPointer)
{
  Wait_Cursor wc ;
  search_mgr().display_editor();
}

void
NodeWindowAgent::clear_searchCB(Widget, XtPointer client_data, XtPointer)
{
  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  agent->clear_search_hits_activate();
}

void
NodeWindowAgent::clear_search_hits_activate()
{
    f_node_view_info->clear_search_hits();

    _DtCvUnit width, height;

    _DtCvStatus status;
    status = _DtCanvasResize((_DtCvHandle)f_help_dsp_area->canvas,
						_DtCvTRUE, &width, &height);

    if (status == _DtCvSTATUS_OK)
    {
	XClearArea(XtDisplay(f_help_dsp_area->dispWid),
		   XtWindow(f_help_dsp_area->dispWid),
		   0, 0, 0, 0, TRUE);

	_DtCanvasRender((_DtCvHandle)f_help_dsp_area->canvas, 0, 0,
			width, height, _DtCvRENDER_PARTIAL, _DtCvFALSE, NULL, NULL);
    }
#ifdef DEBUG
    else if (status == _DtCvSTATUS_NONE)
	fprintf(stderr, "(WARNING) re-layout did not occur.\n");
    else if (status == _DtCvSTATUS_BAD)
	fprintf(stderr, "(ERROR) there were problems during re-layout.\n");
    else
	fprintf(stderr, "(ERROR) unknown code returned.\n");
#endif

    if (XtIsSensitive(f_search_next)) {
	XtSetSensitive(f_search_next, False);
	XtSetSensitive(f_search_next2, False);
    }
    if (XtIsSensitive(f_search_prev)) {
	XtSetSensitive(f_search_prev, False);
	XtSetSensitive(f_search_prev2, False);
    }

    if (XtIsSensitive(f_clear_search_hits))
	XtSetSensitive(f_clear_search_hits, False);
}


#ifdef DEBUG
#ifdef MONITOR
extern "C" {
  extern void moncontrol(int)  ;
}
void
NodeWindowAgent::monitor(WCallback *wcb)
{
  moncontrol((int)wcb->CallData());
}
#endif
#endif


// /////////////////////////////////////////////////////////////////
// display
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::display (UAS_Pointer<UAS_Common> &node_ptr)
{
  bool first_time = False;
  
  if (f_shell == NULL)
    {
      create_ui();
      first_time = TRUE;
    }
  
#ifdef JBM
  f_viewport_agent->pre_display();
#endif
  
  // Clear the old DisplayData.  This is necessary because we
  // are about to delete the value that it is set to. 
  //  f_viewport->DisplayData (NULL);  not necessary, I think  DJB
  
  // Grab the scrolled position of the old node and save it in the history.
  // If the node was displayed from the history, don't bother, because the
  // position has already been recorded since the history had to be advanced. 
  if (!f_history_display && !g_style_sheet_update)
    record_history_position();
  
  
  int sameNode = f_node_view_info && f_node_view_info->node_ptr() == node_ptr;
  if (sameNode && !g_style_sheet_update && !f_history_display) {
//    extern char g_top_locator[];
#ifdef JBM
    // need to change this to pass in the locator to SetTopic so the
    // Canvas knows where to scroll to when it displays
    f_viewport_agent->show_locator (0, g_top_locator);
#endif
  } else {

    if (f_node_view_info) {
      _DtCvTopicInfo topic;
      memset(&topic, 0, sizeof(_DtCvTopicInfo));
      _DtCanvasSetTopic(f_help_dsp_area->canvas, &topic, _DtCvFALSE,
			NULL, NULL, NULL);

      // deleting this object causes massive error messages on novell

      delete f_node_view_info;
      f_node_view_info = NULL;

    }
    
    // Create a new view for the specified node.
    
    // exporting the display area so that the CanvasRenderer has
    // access to it to load the fonts 
    
    extern DtHelpDispAreaStruct *gHelpDisplayArea ;
    gHelpDisplayArea = f_help_dsp_area ;

    //  if (f_node_view_info == NULL)
    //  {
    //      f_node_view_info = new NodeViewInfo(node_ptr);
    //      f_node_view_info = node_mgr().load(node_ptr);
    //  }
    
    f_node_view_info = node_mgr().load(node_ptr);
    
    
    XtAddCallback (f_help_dsp_area->dispWid, 
		   XmNdisarmCallback, selection_end_callback,
		   (XtPointer)this);    

    XtAddCallback (f_help_dsp_area->dispWid,
                   XmNdisarmCallback, disarm_callback, (XtPointer)this);
  }
  
  UAS_Pointer<UAS_List<UAS_TextRun> >
	current_search_hits = search_mgr().current_hits();

  if (current_search_hits != (UAS_Pointer<UAS_List<UAS_TextRun> >)NULL)
  {
      f_node_view_info->set_search_hits(current_search_hits);

      if (! XtIsSensitive(f_clear_search_hits))
	XtSetSensitive(f_clear_search_hits, True);
  }

  f_node_view_info->comp_pixel_values(
					XtDisplay(f_help_dsp_area->dispWid),
					f_help_dsp_area->colormap
				     );
  
  _DtCvTopicInfo *topic = f_node_view_info->topic();
  SetTopic(f_node_view_info->topic());
  
  initialize_controls();
  
  if (f_graphics_handler) {
    delete f_graphics_handler;
    f_graphics_handler = NULL;
  }
  f_graphics_handler = new GraphicsHandler (this);
  // tell the pixmaps what viewport they are attached to 
  //List_Iterator<UAS_Pointer<Graphic> > cursor(&f_node_view_info->graphics());
  //for (; cursor; cursor++)
    //{
      //cursor.item()->pixmap_graphic()->register_handler(f_graphics_handler);
    //}


  char buffer[256];
  strcpy (buffer, "Dtinfo: ");
  strncat (buffer,
	   f_node_view_info->node_ptr()->book_name (UAS_LONG_TITLE),
	   127);
  f_shell->Title (buffer);
  f_shell->IconName (buffer);
  
  
  if (!g_style_sheet_update)
    {
      f_shell->Popup();
      if (first_time)
	{
	  set_min_size(); // NOTE: this really needs to happen before the popup!! 
	  XLowerWindow (XtDisplay (*f_tab_area), XtWindow (*f_tab_area));
	}
      
      f_shell->DeIconify();
    }
  
  if (first_time == TRUE) {
    XtUnmanageChild(f_form);
    XtRealizeWidget(f_form);
    XtManageChild(f_form);
  }

  bool move_to_mark = TRUE;
    
  if (current_search_hits != (UAS_Pointer<UAS_List<UAS_TextRun> >)NULL 
      && f_node_view_info->hit_entries() > 0) 
  {
      _DtHelpSearchMoveTraversal((XtPointer)f_help_dsp_area, 0);
      
      move_to_mark = FALSE;
  }

  // do bookmarks
  do_bookmarks(move_to_mark);

  if (first_time && *g_top_locator) // Try again... (really Canvas bug)
  {
    node_mgr().set_preferred_window (this);

    bool locked = f_locked; // force display in this window
    f_locked = FALSE;

    node_ptr->retrieve();

    f_locked = locked;

    return;
  }
  else {
    // reset sub-component specifier here at last
    *g_top_locator = 0;
  }

  _DtHelpCleanAndDrawWholeCanvas(f_help_dsp_area);
}

void 
NodeWindowAgent::do_bookmarks(bool move_to_mark)
{
  unsigned bookmark_errors = 0 ;
  xList<UAS_Pointer<Mark> > marks ;
  UAS_Pointer<UAS_Common> bogus ;
  bogus = f_node_view_info->node_ptr() ;
  mark_mgr().get_marks (bogus, marks);
  List_Iterator<UAS_Pointer<Mark> > i (marks);
  
  MarkCanvas *jump_to = NULL ;

#ifdef BOOKMARK_DEBUG
  cerr << "id = : " << bogus->id() << endl;
  cerr << "** Marks: " << marks.length() << " **" << endl;
#endif
  while (i)
    {
      mtry {
	MarkCanvas *mc = add_mark (i.item());
	if (g_view_mark() == i.item())
	  jump_to = mc ;
      }
      mcatch_any()
	{
#ifdef BOOKMARK_DEBUG	  
	  cerr << "error adding bookmark " << i.item()->name() << endl;
#endif
	  bookmark_errors++ ;
	}
      end_try ;

      i++ ;
    }
  if (bookmark_errors > 0)
    {
      message_mgr().error_dialog (
		(char*)UAS_String(CATGETS(Set_Messages, 10,
			"Dtinfo is unable to create this bookmark.")),
                          (Widget)f_shell);
    }
#ifdef BOOKMARK_JUMP_DEBUG
  if (g_view_mark() != 0)
    cerr << "have jump" << endl;
  else
    cerr << "have NO jump" << endl;
#endif

  show_mark (jump_to, move_to_mark);
}


// /////////////////////////////////////////////////////////////////
// clone - clone the current node
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::cloneCB(Widget, XtPointer client_data, XtPointer)
{
  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  node_mgr().force_new_window();
  agent->f_node_view_info->node_ptr()->retrieve();
}


// /////////////////////////////////////////////////////////////////
// initialize_hierarchy - init the ancestral hierarchy control
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::initialize_hierarchy()
{
  List_Iterator<Ancestor *> i (f_ancestor_list);
  UAS_Pointer<UAS_Common> toc_ptr = f_node_view_info->node_ptr();
  int t;
  
  // If there's no TOC document for this node, use the node title.
  UAS_String tt = f_node_view_info->node_ptr()->title();
  if (toc_ptr == (UAS_Pointer<UAS_Common>)NULL)
    {
      if (i)
	i.item()->update ((char *) tt, NULL);
      else
	{
	  Ancestor *a = new Ancestor (this, *f_title_menu,
				      (char *) tt,
				      NULL);
	  // Each time an entry is activated we must reset the hierarchy
	  // control in case the node in this window does not change. 
	  ON_ACTIVATE (a->f_button, reset_hierarchy);
	  f_ancestor_list.append (a);
	  // Reset the iterator so that the rest are properly unmanaged. 
	  i.reset();
		       }
      i++;
    }
  else
    {
      // See if the node is in the current hierarchy.
      while (i && XtIsManaged (i.item()->f_button))
	{
	  if (i.item()->f_toc_ptr == toc_ptr)
	    {
	      // Found it -- set the menu history and return.
	      f_current_ancestor = i.item();
	      f_title_option->MenuHistory (f_current_ancestor->f_button);
	      // Finish the traversal and get the tabs from here on up.
	      while (i && XtIsManaged (i.item()->f_button))
		{
		  toc_ptr = i.item()->f_toc_ptr;
		  UAS_List<UAS_Common> tabList = toc_ptr->book_tab_list();
		  t = tabList.length();
		  while (t-- > 0)
		    g_tab_list.insert (tabList[t]);
		  i++;
		}
	      return;
	    }
	  i++;
	}
      
      // Didn't find it, so revise the current hierarchy.
      // The hierarcy list is stored from the last entry to the
      // first because we have to traverse the hierarchy from
      // top to bottom, but we can only traverse a list forward.
      // Therefore, the XmNpositionIndex of the button must be
      // set to 0 in the resource file so that new buttons are
      // always inserted at the beginning of the popup menu. 
      int depth = 0;
      i.reset();
      while (toc_ptr != (UAS_Pointer<UAS_Common>)NULL)
	{
	  // Update existing entries until we run out, then start
	  // adding new entries. 
	  UAS_String ttt = toc_ptr->title();
	  if (i)
	    {
	      i.item()->update ((char *) ttt, toc_ptr);
	      i++;
	    }
	  else
	    {
	      Ancestor *a =
		new Ancestor (this, *f_title_menu, (char *) ttt, toc_ptr);
	      ON_ACTIVATE (a->f_button, reset_hierarchy);
	      f_ancestor_list.append (a);
	    }
	  // 15 is the magic maximum TOC depth... 
	  if (++depth > 15)
	    {
	      message_mgr().error_dialog(
		(char*)UAS_String(CATGETS(Set_Messages, 13, "File a Bug")),
                          (Widget)f_shell);
	      break;
	    }
	  
	  // Push the tabs for each TOC entry onto the tab list.
	  // The list is assumed to be empty at the start of this method.
	  UAS_List<UAS_Common> tabList = toc_ptr->book_tab_list ();
	  t = tabList.length();
	  while (t-- > 0)
	    {
	      ON_DEBUG (puts ("Inserting in g_tab_list"));
	      g_tab_list.insert (tabList[t]);
	    }
	  
	  toc_ptr = toc_ptr->parent();
	}
    }
  
  // Clear and unmanage the leftover buttons, if any.
  for (; i; i++)
    i.item()->update (NULL, NULL);
  
  // Set the menu history to the current node entry.
  i.reset();
  // Work around Motif 1.2 bug
  WXmCascadeButtonGadget opb (XmOptionButtonGadget (*f_title_option));
  opb.Unmanage();
  f_current_ancestor = i.item();
  f_title_option->MenuHistory (f_current_ancestor->f_button);
  opb.Manage();
}


void
NodeWindowAgent::reset_hierarchy()
{
  // If the MenuHistory of the title_menu doesn't match what we
  // set it to, then we must reset it.
  // This can happen if the window is locked and the user chooses
  // a node form the ancestral hierarchy, or if an entry in the
  // ancestral hierarchy is not a Node document. 
  Widget actual = f_title_option->MenuHistory();
  Widget expected = f_current_ancestor->f_button;
  if (actual != expected)
    f_title_option->MenuHistory (expected);
}


// /////////////////////////////////////////////////////////////////
// path handling stuff
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::initialize_path()
{
    UAS_Pointer<UAS_Common> path = f_node_view_info->node_ptr();
  
    if ((path != (UAS_Pointer<UAS_Common>)NULL)
	&& (path->next() != (UAS_Pointer<UAS_Common>)NULL)) {
	XtSetSensitive(f_node_next, True);
	XtSetSensitive(f_node_next2, True);
	XtSetSensitive(f_node_next3, True);
    } else {
	XtSetSensitive(f_node_next, False);
	XtSetSensitive(f_node_next2, False);
	XtSetSensitive(f_node_next3, False);
    }
    
    if ((path != (UAS_Pointer<UAS_Common>)NULL) && 
	(path->previous() != (UAS_Pointer<UAS_Common>)NULL)) {
	XtSetSensitive(f_node_prev, True);
	XtSetSensitive(f_node_prev2, True);
	XtSetSensitive(f_node_prev3, True);
    } else {
	XtSetSensitive(f_node_prev, False);
	XtSetSensitive(f_node_prev2, False);
	XtSetSensitive(f_node_prev3, False);
    }
}

void
NodeWindowAgent::refresh(const UAS_Pointer<Graphic>& pg)
{
#ifdef JBM
  f_viewport_agent->refresh(pg);
#endif
}

void
NodeWindowAgent::preview_nextCB(Widget, XtPointer client_data, XtPointer)
{
  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  agent->f_preview_document = agent->f_node_view_info->node_ptr()->next();
  agent->f_preview_timeout =
    new WTimeOut (window_system().app_context(), 550,
		  agent, (WTimeOutFunc) &NodeWindowAgent::preview);
}

void
NodeWindowAgent::preview_previousCB(Widget, XtPointer client_data, XtPointer)
{
  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  agent->f_preview_document = agent->f_node_view_info->node_ptr()->previous();
  agent->f_preview_timeout =
    new WTimeOut (window_system().app_context(), 550,
		  agent, (WTimeOutFunc) &NodeWindowAgent::preview);
}


void
NodeWindowAgent::node_nextCB(Widget, XtPointer client_data, XtPointer)
{
  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  // If this callback is called, the node must have a next. 
  node_mgr().set_preferred_window (agent);
  agent->f_node_view_info->node_ptr()->next()->retrieve();
}

void
NodeWindowAgent::node_previousCB(Widget, XtPointer client_data, XtPointer)
{
  NodeWindowAgent *agent = (NodeWindowAgent *)client_data;
  // If this callback is called, the node must have a previous. 
  node_mgr().set_preferred_window (agent);
  agent->f_node_view_info->node_ptr()->previous()->retrieve();
}


// /////////////////////////////////////////////////////////////////
// add_quick_help
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::add_quick_help (Widget w, const char* help_text)
{
  if (help_text == NULL || *help_text == '\0') {
    XtVaSetValues(w, XmNuserData, NULL, NULL);
    return;
  }
  else
    XtVaSetValues(w, XmNuserData, (void*)this, NULL);

  // Now add the event handlers to display the quick help.
  XtAddEventHandler (w, EnterWindowMask, False, quick_helpEH, (XtPointer)help_text);
  XtAddEventHandler (w, LeaveWindowMask, False, quick_helpEH, (XtPointer)help_text);
  XtAddEventHandler (w, FocusChangeMask, False, quick_helpEH, (XtPointer)help_text);
}


// /////////////////////////////////////////////////////////////////
// quick_help
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::quick_helpEH (Widget w, XtPointer client_data,
			  XEvent *event, Boolean *)
{
  // Ok, now we can display the help text. 
  Arg args[2];
  int n;
  NodeWindowAgent *agent;

  n = 0;
  XtSetArg(args[n], XmNuserData, &agent); n++;
  XtGetValues(w, args, n);

  // display quick help if moving into widget or clear quick help
  // if moving out of widget.
  if (event->type == EnterNotify || event->type == FocusIn)
    XmTextFieldSetString(agent->f_status_text, (char*)client_data);
  else if (event->type == LeaveNotify || event->type != FocusOut)
    XmTextFieldSetString(agent->f_status_text, (char*)"");
}


// /////////////////////////////////////////////////////////////////
// record_history_position - save the scrolled node position
// /////////////////////////////////////////////////////////////////

// This routine recors the scrolled position of the currently
// displayed node in the current position of the local hist list. 

void
NodeWindowAgent::record_history_position()
{
#ifdef JBM
  // If the node isn't going to be replaced, forget it.
  if (f_locked || f_node_view_info == NULL)
    return;
  
  model_pos *top = f_viewport_agent->top_model_pos();
  if (top != NULL) // just in case 
    {
      model_range range;
      range.set_start (*top);
      range.set_end (*top);
      f_history_list.set_anchor (new AnchorCanvas (&range));
    }
#ifdef DEBUG
  else // this shouldn't happen
    {
      // Save the anchor, since expose events in the viewport with the
      // message dialog up will attempt to scroll to the (bogus) anchor. 
      AnchorCanvas *save = g_history_anchor;
      g_history_anchor = NULL;
      message_mgr().info_dialog ("Can't get scroll position for history.",
                          (Widget)f_shell);
      g_history_anchor = save;
    }
#endif
#endif
}

void
NodeWindowAgent::re_display()
{
#ifdef JBM
  AnchorCanvas *save_anchor = g_history_anchor ;
  AnchorCanvas *anchor = NULL ;
  g_history_anchor = NULL ;
  // set up to display it at current position 
  model_range range ;
  model_pos *top = f_viewport_agent->top_model_pos();
  if (top != NULL) // just in case 
    {
      range.set_start (*top);
      range.set_end (*top);
      // g_history anchor is set to NULL during display
      anchor = new AnchorCanvas(&range);
      g_history_anchor = anchor;
    }
#ifdef DEBUG
  else // this shouldn't happen
    {
      // Save the anchor, since expose events in the viewport with the
      // message dialog up will attempt to scroll to the (bogus) anchor. 
      AnchorCanvas *save = g_history_anchor;
      g_history_anchor = NULL;
      message_mgr().info_dialog ("Can't get scroll position for re-display.");
      g_history_anchor = save;
    }
#endif
  
  bool lock_flag = f_locked ;
  f_locked = False ; // have to force it to happen regardless of lock 
  
  mtry
    {
      
      node_mgr().set_preferred_window(this); // make re-display happen in this window 
      f_node_view_info->node_ptr()->retrieve(); // same node 
      
      f_locked = lock_flag ;
      
    }
  mcatch_any()
    {
      delete anchor ;
      g_history_anchor = save_anchor ;
      rethrow;
    }
  end_try;
  delete anchor;
  g_history_anchor = save_anchor; 
#endif
}

void
NodeWindowAgent::SetTopic(_DtCvTopicPtr topic)
{
  cleanup_marks();

  /*
     this routine was paraphrased from _DtHelpDisplayAreaSetList
   */

  _DtHelpClearSelection(f_help_dsp_area);

  _DtCvUnit ret_width, ret_height, ret_y = 0;

  { // set topic with component SGML id set
    if (g_top_locator[0] != 0)
      topic->id_str = g_top_locator ;

    _DtCvStatus status;
    status = _DtCanvasSetTopic(f_help_dsp_area->canvas, topic, _DtCvFALSE,
			       &ret_width, &ret_height, &ret_y);
#ifdef CV_HYPER_DEBUG
    cerr << "top locator=" << topic->id_str << ' ' 
	 << "firstVisible=" << ret_y << endl;
#endif
   // This statement was for debug purposes only and is no
   // longer needed.
   // if (status != _DtCvSTATUS_OK)
   //   cerr << "(ERROR) _DtCanvasSetTopic failed, error# " << status << endl;
  }

  f_vscrollbar_offset = ret_y ;

  /*
   SetMaxPositions (f_help_dsp_area, ret_width, ret_height);
   */
  f_help_dsp_area->maxX = ret_width ;
  f_help_dsp_area->maxYpos = ret_height +
    f_help_dsp_area->marginHeight ;

  f_help_dsp_area->firstVisible = ret_y ;

  (void) _DtHelpSetScrollBars (f_help_dsp_area, 
			       f_help_dsp_area->dispWidth,
			       f_help_dsp_area->dispHeight); 
}

// helper functions for create_canvas_mark
// finds the next string segment after the
// given segment

// this is highly inefficient when we have to start
// finding parents, as there are no up pointers in the tree so we walk
// down from the top each time, but this should be rare, and probably
// is more efficient in time (and definitely space) than keeping a
// stack of the parents


static _DtCvSegment *
contains(_DtCvSegment *root, _DtCvSegment *segment)
{
  // need to find the parent container of
  // the given segment
  
  if (_DtCvIsSegContainer(root))
    {
      _DtCvSegment *seglist = root->handle.container.seg_list ;
      while (seglist)
	{
	  if (seglist == segment)
	    return root ;

	  if (_DtCvIsSegContainer(seglist))
	    {
	      _DtCvSegment *tseg = contains(seglist, segment);
	      if (tseg)
		return tseg ;
	    }

	  seglist = seglist->next_seg ;
	}
    }

    return 0 ;

}

static _DtCvSegment *
find_string_seg(_DtCvSegment *segment)
{
  // finds the next string segment moving
  // forward from (and counting) segment

  if (!segment)
    return 0 ;

  while (segment && !_DtCvIsSegString(segment))
    {
      if (_DtCvIsSegContainer (segment))
	{
	  _DtCvSegment *rseg = find_string_seg(segment->handle.container.seg_list);
	  if (rseg)
	    return rseg ;
	}
      segment = segment->next_seg ;
    }
  return segment ;
}

static 
_DtCvSegment *
next_string_segment(_DtCvSegment *root, _DtCvSegment *start)
{
  _DtCvSegment *rseg = 0;


  rseg = find_string_seg(start->next_seg); // recursive procedure

  if (rseg)
    return rseg ;

  // walk up the tree
  while (!rseg)
    {
      start = contains(root, start);
      assert (_DtCvIsSegContainer (start));
      if (!start)
	throw (CASTEXCEPT Exception());
	  
      return next_string_segment(root, start);
    }

  return 0;
}



#define CURRENT_FORMAT "TML-1"

inline unsigned min(unsigned a, unsigned b)
{
  return a < b ? a : b ;
}

MarkCanvas *
NodeWindowAgent::create_canvas_mark(_DtCvHandle  canvas,
				    NodeViewInfo *nvinfo,
				    UAS_Pointer<Mark> &mark)
{
  if (canvas == NULL || nvinfo == NULL || mark == (UAS_Pointer<Mark>)NULL)
    return NULL;

  const Anchor &anchor = mark->anchor();

  if (strcmp (anchor.format(), CURRENT_FORMAT) != 0)
    return NULL;
  
  MarkInfo markinfo;

  UAS_String location(anchor.location());

  UAS_String locator, marks_loc;
  location.split(';', locator, marks_loc);
#ifdef BOOKMARK_DEBUG
  fprintf(stderr, "(DEBUG) marked section=\"%s\"\n", (char*)location);
#endif

  UAS_List<UAS_String> marks_loc_list;
  marks_loc_list = marks_loc.splitFields(',');

  int i;
  for (i = 0; i < marks_loc_list.length(); i++)
  {
    UAS_String& mark_loc = *marks_loc_list[i];
    unsigned int vcc, offset, length;

    sscanf((char*)mark_loc, "%u\t%u\t%u", &vcc, &offset, &length);

    markinfo.insert_item(UAS_Pointer<MarkUnitInfo>(
		new MarkUnitInfo(vcc, offset, length, NULL)));
#ifdef BOOKMARK_DEBUG
    fprintf(stderr, "(DEBUG) marked position=(%u,%u,%u)\n",
					vcc, offset, length);
#endif
  }

  if (markinfo.length() == 0)
    return NULL;

  _DtCvSegPts **segs = new _DtCvSegPts*[markinfo.length() + 1];
  memset(segs, 0, (markinfo.length() + 1) * sizeof(_DtCvSegPts*));

  for (i = 0; i < markinfo.length(); i++)
  {
    MarkUnitInfo& mui = *markinfo[i];

    segs[i] = new _DtCvSegPts;

    segs[i]->segment = nvinfo->get_segment(mui.vcc());
    segs[i]->offset  = mui.offset() ;
    segs[i]->len     = mui.length();
  }  

  MarkCanvas *mark_canvas =
		new MarkCanvas(mark, this, markinfo[0]->offset(), 0);

  // place the mark in the Canvas 
      
  _DtCvPointInfo point_info;
      
  point_info.client_data = mark_canvas ;
  point_info.segs =  segs ;
      
#ifdef BOOKMARK_DEBUG
  _DtCvSegPts** iter = segs;
  for (; *iter; iter++) {
	cerr << "relative offset=" << (*iter)->offset << " " <<
		"len=" << (*iter)->len << " " <<
		(char*)(*iter)->segment->handle.string.string << endl;
  }
#endif

  int ret_y1, ret_y2 ;

  _DtCvStatus status = _DtCanvasActivatePts(canvas, 
			_DtCvACTIVATE_MARK /* | _DtCvACTIVATE_MARK_ON */,
					&point_info, &ret_y1, &ret_y2);
  (void) _DtCanvasProcessSelection(canvas, 0, 0, _DtCvSELECTION_CLEAR);

#ifdef MARK_ON_DEBUG
  status  = _DtCanvasActivatePts(canvas,
			     _DtCvACTIVATE_MARK_ON, 0, 0, 0);
#endif

  // clean up segpts
  for (i = 0; i < markinfo.length(); i++)
    delete segs[i];

  delete segs ;
      
#ifdef BOOKMARK_DEBUG
  if (status == _DtCvSTATUS_BAD)
    cerr << "Activate Pts --> bad return" << endl;
  else
    cerr << "Activate Pts y1( " << ret_y1 << "), y2( " << ret_y2 << ")" << endl;
#endif    
  
  if (status == _DtCvSTATUS_BAD) {
    delete mark_canvas ;
    mark_canvas = NULL;
  }
  else
    mark_canvas->y_position (ret_y1);
  
  return mark_canvas ;
}

static unsigned
find_segment_offset (_DtCvSegment *start, const _DtCvSegment *target,
		     unsigned &offset)
{
  // find the offset of the target segment from the starting point
  // offset is accumulated recursively
  
  if (start == target)
  {
    return 1 ;
  }
  else if(start == NULL)
  {
    return False;
  }
  
  Boolean found = False ;
  switch (start->type & _DtCvPRIMARY_MASK)
    {
    case _DtCvCONTAINER:
      found = find_segment_offset (start->handle.container.seg_list,
				   target, offset);
      break ;
    case _DtCvSTRING:
      {
#ifdef BOOKMARK_DEBUG
	//cerr << "[" << offset << "]\t" << start->handle.string.string << endl;
#endif
	// do not count this string if it was added data
	if (!(start->type & _DtCvAPP_FLAG1))
	  offset += strlen( (const char *)(start->handle.string.string) ) ;
      }
      break ;
    case _DtCvTABLE:
      {
        for (int i = 0; start->handle.table.cells[i] != NULL; i++)
        {
          found = find_segment_offset (start->handle.table.cells[i],
                                       target, offset);
          if (found)
             break;
        }
      }
      break;
    }
  
  if (!found && start->next_seg)
    return find_segment_offset (start->next_seg, target, offset);
  
  return found ;
}

void
NodeWindowAgent::link_to (const char *locator)
{
  UAS_String loc(locator);
  UAS_Pointer<UAS_Common> target =
		f_node_view_info->node_ptr()->create_relative (loc);

  // NOTE: create_relative may fail if the infolib associated with
  //       has been removed. If so, do not try to retrieve.
  if (target) {
    node_mgr().set_preferred_window (this);
  
    strcpy(g_top_locator, (char*)loc);
  
    target->retrieve() ;
  }
}

void
NodeWindowAgent::canvas_resize()
{
#ifdef RESIZE_DEBUG
  cerr << "canvas_resize" << endl;
#endif  
  
  layout_mark_icons();
}

int g_blew_away_marks_too_bad_you_lose_dts_14590;
void
NodeWindowAgent::layout_mark_icons()
{
  if (f_help_dsp_area == NULL)
    return;

  f_vscrollbar_offset = f_help_dsp_area->firstVisible;

  if (f_mark_list.length() == 0)
    return;
  
  /* -------- Delete any existing icons. -------- */
  
  g_blew_away_marks_too_bad_you_lose_dts_14590 = 1 ; // inform marks chooser 

  // first we destroy all the icons
#if 1
  while (f_mark_icon_list.length() > 0) {
#ifdef BOOKMARK_DEBUG
    fprintf(stderr, "(DEBUG) still %d remain, removing icon...\n",
		    f_mark_icon_list.length());
#endif
    List_Iterator<MarkIcon *> iter (f_mark_icon_list);
    delete iter.item();
    f_mark_icon_list.remove(iter);
  }
#else
  List_Iterator <MarkIcon *> i (f_mark_icon_list);
  
  while (i)
    {
      delete i.item();
      f_mark_icon_list.remove (i);
    }
#endif
  
  /* -------- Now re-layout the icons from the list 'o Marks. -------- */
  
  // first we have to compute the new location of each MarkCanvas
  
  _DtCvMarkPos **return_positions = NULL;
  _DtCanvasGetMarkPositions(f_help_dsp_area->canvas,
			    &return_positions);
  
  for (int mpi = 0 ; return_positions[mpi]; mpi++)
    {
      _DtCvMarkPos *mark_pos = return_positions[mpi] ;
#ifdef BOOKMARK_DEBUG
      cerr << "Recalc Mark Pos:" << mark_pos->y1 << endl;
#endif
      ((MarkCanvas*) mark_pos->client_data)->y_position (mark_pos->y1);

      free(mark_pos);
    }
  free(return_positions);
  
  // The list of MarkCanvas objects is already in order of occurance
  // in the node, so we can just process them in order to create
  // the revised set of MarkIcons.
  
  
  List_Iterator <MarkCanvas *> m (f_mark_list);
  MarkIcon *mark_icon = NULL;
  MarkCanvas  *jump_to = NULL;
  
  while (m)
    {
      if (g_view_mark() == m.item()->mark_ptr())
	{
	  g_view_mark() = NULL;
	  jump_to = m.item();
	}

      MarkIcon *mi = NULL;
      List_Iterator<MarkIcon *> iter (f_mark_icon_list);
      while (iter)
      {
	mi = iter.item();

	if (mi && m.item()->y_position() > mi->ypos() - mi->Height()
	       && m.item()->y_position() < mi->ypos() + mi->Height()) {
	  mi->append(m.item());
	  break;
	}

	mi = NULL;
	iter++;
      }

      if (mi == NULL) {
	MarkIcon *mark_icon =
	    new MarkIcon (XtParent (XtParent (f_help_dsp_area->dispWid)),
			  *f_shell, m.item(),
			  m.item()->y_position(), f_vscrollbar_offset);

        f_mark_icon_list.append (mark_icon);
      }

      m++;
    }
}

void
NodeWindowAgent::show_mark(MarkCanvas *jump_to, bool move_to)
{
  if (jump_to)
   {
      // Make sure the mark has an icon
      if (jump_to->mark_icon() != NULL)
	{
	  jump_to->select(move_to);
	}
      else
	{
	  message_mgr().warning_dialog (
		(char*)UAS_String(CATGETS(Set_Messages, 14,
			"The bookmark cannot be displayed.")),
                          (Widget)f_shell);
	}
    }
}


// /////////////////////////////////////////////////////////////////
// cleanup_marks
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::cleanup_marks()
{
  // Delete all items in these lists.
  List_Iterator<MarkIcon *> mi (f_mark_icon_list);
  while (mi)
    {
      delete mi.item();
      f_mark_icon_list.remove (mi);
    }
  
  List_Iterator<MarkCanvas *> mt (f_mark_list);
  while (mt)
    {
      delete mt.item();
      f_mark_list.remove (mt);
    }
}


void
NodeWindowAgent::receive (MarkCreated &message, void* /*client_data*/)
{
#ifdef BOOKMARK_DEBUG
  cerr << "receive <Mark Created>" << endl;
#endif
  // Make sure the Mark in question is dispalyed in this Viewport. 
  if (message.f_mark_ptr->doc_ptr() != f_node_ptr) {
    // Nov 11 '94 yuji - Another doc_ptr may refer to the same contents.
    char *locator = strdup(message.f_mark_ptr->doc_ptr()->locator());
    if (strcmp(locator, f_node_ptr->locator())) {
      free(locator);
      return;
    }
    free(locator);
  }
  
  add_mark (message.f_mark_ptr);
}
void
NodeWindowAgent::receive (MarkDeleted &message, void* /*client_data*/)
{
#ifdef BOOKMARK_DEBUG
  cerr << "receive <Mark Deleted>" << endl;
#endif
  delete_mark_visuals (message.f_mark_ptr) ; 
}


// /////////////////////////////////////////////////////////////////
// delete_mark - delete the MarkTml and maybe icon of a Mark
// /////////////////////////////////////////////////////////////////

void
NodeWindowAgent::delete_mark_visuals (UAS_Pointer<Mark> &mark_ptr)
{
  List_Iterator <MarkCanvas *> m (f_mark_list);
  
  // Try to find the corresponding MarkCanvas object in the mark list. 
  while (m)
    {
      if (mark_ptr == m.item()->mark_ptr())
	{
	  MarkCanvas *mark_canvas = m.item();
	  
	  // Deselect the Mark first if necessary.
	  if (mark_canvas->selected())
	    mark_canvas->deselect();
	  
	  
	  // turn traversal off
	  _DtCanvasMoveTraversal(f_help_dsp_area->canvas,
				 _DtCvTRAVERSAL_OFF, // cmd
				 _DtCvTRUE,	     // wrap
				 _DtCvTRUE,	     // render
				 0, 0, 0, 0, 0);


	  // Remove It from the DtCanvas
	  
	  _DtCvPointInfo point_info ;
	  point_info.client_data = mark_canvas; // this is how we
						// identify it
	  

	  _DtCanvasActivatePts (f_help_dsp_area->canvas, // canvas
				_DtCvDEACTIVATE,	 // mask
				&point_info,		 // mark identification
				NULL,			 // *ret_y1
				NULL			 // *ret_y2
				);
	  
	  // Disconnect the MarkCanvas from the MarkIcon. 
	  MarkIcon *mark_icon = mark_canvas->mark_icon();
	  if (mark_icon != NULL)
	    mark_icon->remove (mark_canvas);
	  
	  // Delete the MarkCanvas object and remove it from the list. 
	  f_mark_list.remove (m);
	  delete mark_canvas;
	  
	  // Delete the MarkIcon when no more Marks are associated with it. 
	  if (mark_icon && mark_icon->mark_count() == 0)
	    {
	      f_mark_icon_list.remove (mark_icon);
	      delete mark_icon;
	    }

	  // turn traversal back on
	  // turn traversal off
	  _DtCanvasMoveTraversal(f_help_dsp_area->canvas,
				 _DtCvTRAVERSAL_ON, // cmd
				 _DtCvTRUE,	    // wrap
				 _DtCvTRUE,	    // render
				 0, 0, 0, 0, 0);
	  return;
	}
      m++;
    }
}

// /////////////////////////////////////////////////////////////////
// add_mark
// /////////////////////////////////////////////////////////////////

MarkCanvas *
NodeWindowAgent::add_mark (UAS_Pointer<Mark> &mark_ptr)
{
  MarkCanvas *mark_canvas = create_canvas_mark(f_help_dsp_area->canvas,
					       f_node_view_info,
					       mark_ptr);
  if (mark_canvas == NULL)
    return mark_canvas;
  
  // Insert the mark into the Mark list.
  // The list is ordered by starting position of the Mark. 
  List_Iterator<MarkCanvas *> m (f_mark_list);
  while (m && mark_canvas->offset() > m.item()->offset())
    m++;
  
  f_mark_list.insert_before (m, mark_canvas);
  
  /* -------- Make an icon appear for the new Mark. -------- */
  
  // First see if there's already an overlapping icon at the proper position.
  // Start by finding the icons on either side of this one. 
  
  List_Iterator<MarkIcon *> i (f_mark_icon_list);

  while (i && mark_canvas->offset() > i.item()->offset())
    {
      i++;
    }

  unsigned int ypos = mark_canvas->y_position();

  MarkIcon *mi = NULL;
  List_Iterator<MarkIcon *> iter (f_mark_icon_list);
  while (iter) {

      mi = iter.item();

      if (mi && ypos > mi->ypos() - mi->Height() 
	     && ypos < mi->ypos() + mi->Height())
      {
	// Add to the end of the icon Mark list. 
	mi->append (mark_canvas);
	break;
      }

      mi = NULL;
      iter++;
  }

  if (mi == NULL) {
    MarkIcon *mark_icon = 
	new MarkIcon (XtParent (XtParent (f_help_dsp_area->dispWid)), 
		      *f_shell,
		      mark_canvas, ypos, f_vscrollbar_offset); 
    f_mark_icon_list.insert_before (i, mark_icon);
  }

#ifdef BOOKMARK_DEBUG
  {
    _DtCvMarkPos **return_positions = NULL;
    _DtCanvasGetMarkPositions(f_help_dsp_area->canvas,
			      &return_positions);

    for (int i = 0; return_positions[i]; i++)
    {
      _DtCvMarkPos* mark_pos = return_positions[i];
      fprintf(stderr, "(DEBUG) _DtCvMarkPos %dth ypos=%d\n", i,
							     mark_pos->y1);
      free(mark_pos);
    }
    free(return_positions);
  }
#endif

  return mark_canvas ;
}

// /////////////////////////////////////////////////////////////////
// code to initiate a mark view
// /////////////////////////////////////////////////////////////////

class MarkViewer : public UAS_Receiver<ViewMark>
{
public:
  MarkViewer()
    { Mark::request (this); }
private:
  void receive (ViewMark &message, void *client_data);
};

void
MarkViewer::receive (ViewMark &message, void* /*client_data*/)
{
  // Slight hack... Set a global that tells the ViewportAgent to
  // select and jump to the specified mark when the corresponding
  // node is displayed (see "jump_to" below).  15:17 02-Dec-93 DJB
  mtry
    {
      g_view_mark() = message.f_mark_ptr;

      UAS_Pointer<UAS_Common>& section = message.f_mark_ptr->doc_ptr();
      if (section == (UAS_Pointer<UAS_Common>)NULL)
	throw(CASTEXCEPT Exception());

      section->retrieve();
    }
  mcatch_noarg(demoException &)
    {
      // this is a hack. what should happen, is that the catch_any below this
      // should only be catching a specific (or set of) exception so that others
      // can pass through - 14:32 06/ 1/94 - jbm
      rethrow ;
    }
  mcatch_any()
    {
      message_mgr().info_dialog (
		(char*)UAS_String(CATGETS(Set_Messages, 15,
				"The bookcase is not accessible.")));
    }
  end_try;
}

static MarkViewer g_mark_viewer;


void
NodeWindowAgent::receive (UAS_LibraryDestroyedMsg &msg, void* /*client_data*/)
{
  //cerr << "NodeWindowAgent::receive (UAS_LibraryDestroyedMsg)\n";
  UAS_Pointer<UAS_Common> myNode = f_node_view_info->node_ptr();
  
  //
  //  First, if we're displaying a node from the dead library,
  //  just commit suicide.
  //
  if (myNode != 0 && myNode->get_library() == msg.fLib) {
    myNode = 0;		// Just to be safe.
    delete this;
    return;
  }
  
  //
  //  We're not displaying a node from the dead library,
  //  however, our local history list might contain such
  //  entries. Need to search the list and blow them away NOW.
  //
  f_history_list.library_removed (msg.fLib);

  if (f_history_list.previous() != (UAS_Pointer<UAS_Common>)NULL) {
    XtSetSensitive(f_history_prev, True);
    XtSetSensitive(f_history_prev2, True);
  } else {
    XtSetSensitive(f_history_prev, False);
    XtSetSensitive(f_history_prev2, False);
  }
  
  if (f_history_list.next() != (UAS_Pointer<UAS_Common>)NULL) {
    XtSetSensitive(f_history_next, True);
    XtSetSensitive(f_history_next2, True);
  } else {
    XtSetSensitive(f_history_next, False);
    XtSetSensitive(f_history_next2, False);
  }
}

// find the segment that contains a graphic

static 
_DtCvSegment *find_graphic(_DtCvSegment *segment, const Graphic *gr)
{
  if (NULL == segment)
    return NULL ;

  switch (segment->type & _DtCvPRIMARY_MASK)
    {
    SegClientData* pSCD;
    case _DtCvREGION:
      pSCD = (SegClientData*)segment->client_use;
      if (((Graphic*)pSCD->GraphicHandle())->locator() ==
          ((Graphic*)gr)->locator())
	return segment ;
      break;
    case _DtCvCONTAINER:
      {
	_DtCvSegment *found =
	  find_graphic(segment->handle.container.seg_list, gr);
	if (found)
	  return found ;
      }
      break ;
    }
  return find_graphic (segment->next_seg, gr);
}

void 
NodeWindowAgent::detach(UAS_Pointer<Graphic> &gr)
{
  replace (gr);
  _DtCvSegment *grseg;

  // get graphic segment
  grseg = find_graphic(f_node_view_info->topic()->seg_list, gr);
  if (grseg == NULL)
    return;

  SegClientData* pSCD = (SegClientData*)(grseg->client_use);
  UAS_Pointer<Graphic> curr_gr = (Graphic*)pSCD->GraphicHandle();
  curr_gr->set_detached(TRUE);
  //curr_gr->set_agent(gr->get_agent());
  
}

void 
NodeWindowAgent::reattach(UAS_Pointer<Graphic> &gr)
{
  _DtCvSegment *grseg;

  // get graphic segment
  grseg = find_graphic(f_node_view_info->topic()->seg_list, gr);
  if (grseg == NULL)
    return;

  SegClientData* pSCD = (SegClientData*)(grseg->client_use);
  UAS_Pointer<Graphic> nodeview_gr = (Graphic*)pSCD->GraphicHandle();
  nodeview_gr->set_detached(FALSE);
  //curr_gr->set_agent(gr->get_agent());
  //replace (gr);
  replace (nodeview_gr);
}

void 
NodeWindowAgent::replace(UAS_Pointer<Graphic> &gr)
{
  // have to find the graphic in the canvas, remove it from the canvas
  // and replace it with the detached graphic graphic.
  _DtCvSegment *grseg =
    find_graphic(f_node_view_info->topic()->seg_list, gr); 

  if(grseg == NULL)
  {
    return;
  }
  _DtHelpDARegion     *pReg = (_DtHelpDARegion*) grseg->handle.region.info ;
  DtHelpGraphicStruct *pGS  = (DtHelpGraphicStruct *) pReg->handle ;

  pGS->pix = gr->graphic()->pixmap();
  pGS->width = gr->graphic()->width();
  pGS->height = gr->graphic()->height();
  
  grseg->handle.region.width = pGS->width ;
  grseg->handle.region.height = pGS->height ;

  // inform our canvas that it needs to re-render
  _DtCvUnit width = 0, height = 0;
  _DtCvStatus status;
  status = _DtCanvasResize((_DtCvHandle)f_help_dsp_area->canvas,
			   _DtCvTRUE, &width, &height);
  // There's no C/RE APIs provided to set maxYpos correctly
  // after canvas reformatting contents, so dtinfo manually
  // does it for now...
  f_help_dsp_area->maxYpos = f_help_dsp_area->marginHeight + height;

  // now get the canvas wiped out
  XClearArea(XtDisplay(f_help_dsp_area->dispWid),
	     XtWindow(f_help_dsp_area->dispWid),
	     0, 0, 0, 0, TRUE);

  _DtHelpSetScrollBars(f_help_dsp_area, f_help_dsp_area->dispWidth,
					f_help_dsp_area->dispHeight);

  // _DtCanvasRender is supposed to re-render the canvas
  _DtCanvasRender((_DtCvHandle)f_help_dsp_area->canvas, 0, 0,
		width, height, _DtCvRENDER_PARTIAL, _DtCvFALSE, NULL, NULL);
  if (XtIsManaged(f_help_dsp_area->vertScrollWid))
  {
     // reset the scrollbar offset
     Arg arg[1] ; 
     XtSetArg(arg[0], XmNvalue, &f_vscrollbar_offset);
     XtGetValues(f_help_dsp_area->vertScrollWid, arg, 1);
  }

  // mark icons need to be re-positioned
  layout_mark_icons();
}

/*
 * Graphics  Handler
 */

GraphicsHandler::GraphicsHandler(NodeWindowAgent *agent)
: f_agent(agent)
{
  graphics_mgr().UAS_Sender<DetachGraphic>::
                 request((UAS_Receiver<DetachGraphic>*)this);
  graphics_mgr().UAS_Sender<ReAttachGraphic>::
                 request((UAS_Receiver<ReAttachGraphic>*)this);
}

GraphicsHandler::~GraphicsHandler()
{
}

void
GraphicsHandler::display_graphic(UAS_Pointer<Graphic> &gr)
{
  UAS_Sender<DisplayGraphic>::send_message(DisplayGraphic(gr));
}

void
GraphicsHandler::receive(DetachGraphic &message, void* /*client_data*/)
{
  UAS_Sender<DisplayGraphic>::request(message.agent());
  if(message.agent()->node_ptr()->id() == 
     f_agent->node_view_info()->node_ptr()->id())
  {
    f_agent->detach(message.graphic());
  }
}

void
GraphicsHandler::receive(ReAttachGraphic &message, void* /*client_data*/)
{
  f_agent->reattach(message.graphic());
}

