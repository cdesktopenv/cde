/*	Copyright (c) 1994,1995,1996 FUJITSU LIMITED	*/
/*	All Rights Reserved				*/

/*
 * $XConsortium: NodePrintAgent.hh /main/6 1996/08/21 15:57:04 drk $
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


#ifndef _NodePrintAgent_hh
#define _NodePrintAgent_hh

#include "UAS.hh"

typedef void _DtHelpDeSelectAll_t(_WidgetRec*,_XEvent*,char**,unsigned*);
typedef _DtHelpDeSelectAll_t * DeSelectAll_ptr;
typedef void _DtHelpSelectAll_t(_WidgetRec*,_XEvent*,char**,unsigned*);
typedef _DtHelpSelectAll_t * SelectAll_ptr;
typedef void _DtHelpActivateLink_t(_WidgetRec*,_XEvent*,char**,unsigned*);
typedef _DtHelpActivateLink_t * ActivateLink_ptr;
typedef void _DtHelpCopyAction_t(_WidgetRec*,_XEvent*,char**,unsigned*);
typedef _DtHelpCopyAction_t * CopyAction_ptr;
typedef void _DtHelpPageUpOrDown_t(_WidgetRec*,_XEvent*,char**,unsigned*);
typedef _DtHelpPageUpOrDown_t * PageUpOrDown_ptr;
typedef void _DtHelpPageLeftOrRight_t(_WidgetRec*,_XEvent*,char**,unsigned*);
typedef _DtHelpPageLeftOrRight_t * PageLeftOrRight_ptr;
typedef void _DtHelpNextLink_t(_WidgetRec*,_XEvent*,char**,unsigned*);
typedef _DtHelpNextLink_t * NextLink_ptr;

class NodeViewInfo;
class NodeHandle;
class Manager;
class WXmForm;
class Ancestor;
class PixmapGraphic;
class ScopePopup;
class Graphic;

class GraphicsHandler ;
// self is used by some MACROS somewhere, but the X include files use
// it as something different

// we require #undef XtIsManaged, XtWindow and XtDisplay because they are defined
// as a macros in the Private Xm Header files, but we rely on the
// Function definitions to cause automatic casts to Widgets in our .C
// file - (brad) 


#undef self
#include <DtI/DisplayAreaP.h>
#define self *this
#undef XtIsManaged
#undef XtWindow
#undef XtDisplay

class NodePrintAgent : public Agent
{
public: // functions

   NodePrintAgent();
  ~NodePrintAgent();

   void display(UAS_Pointer<UAS_Common> &n);

public: // Motif-version functions (usable by other Motif agents only)


private: // functions
   DtHelpDispAreaStruct *create_ui(Widget parent);

  // Callbacks

  void SetTopic(_DtCvTopicPtr);

private: // class

private: // variables

    NodeViewInfo *         f_node_view_info;   // Display specific view info. 
    DtHelpDispAreaStruct * f_help_dsp_area ;
    Widget		   f_form;

};

#endif
