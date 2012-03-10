/* $XConsortium: Container.h /main/3 1995/11/06 09:39:43 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef CONTAINER_H
#define CONTAINER_H

#include "MotifUI.h"

const long OPEN_TIME = 40000L;

class Container : public MotifUI {

   friend void UpdateAreaMessage(Widget, XtPointer, XEvent *, Boolean *);
   friend void MakeChildVisible(Widget, XtPointer, XtPointer);
   friend void ResizeTimeOut(void *, XtIntervalId *id);
   friend void ResizeSW(Widget, XtPointer, XtPointer);
   friend void ResizeRC(Widget, XEvent *, String *, int *);


   static void UpdateAreaMessage(Widget, XtPointer, XEvent *, Boolean *);
   static void MakeChildVisible(Widget, XtPointer, XtPointer);
   static void ResizeTimeOut(void *, XtIntervalId *id);
   static void ResizeSW(Widget, XtPointer, XtPointer);
   static void ResizeRC(Widget, XEvent *, String *, int *);

 protected:

   Widget _sw;
   Widget _workArea;
   Widget _clipWidget;
   Widget _vbar;
   Widget _hbar;
   XmString _xm_update_message;
   BaseUI *_last_selected;
   IconStyle _style;
   IconStyle _previous_style;
   ContainerType _container_type;
   SelectionType _select_type;
   virtual boolean SetIcon(IconStyle);

   void NotifySelected(BaseUI *);
   void NotifyDelete(BaseUI *);
   void NotifyVisiblity(BaseUI *);
   void DoBeginUpdate();
   void DoEndUpdate();
   void DoUpdateMessage(char *);
   void CreateContainer(MotifUI *,char *, char *, ContainerType, SelectionType);

 public:

   Container(MotifUI *parent,
	     char *name,
	     ContainerType container_type = SCROLLED_WORK_AREA,
	     SelectionType select_type = SINGLE_SELECT);

   Container(char *category,
	     MotifUI *parent,
	     char *name,
	     ContainerType container_type = SCROLLED_WORK_AREA,
	     SelectionType select_type = SINGLE_SELECT);

   void OpenAnimation(MotifUI *obj);
   void SelectionPolicy(SelectionType);
   const SelectionType SelectionPolicy() { return _select_type; }
   const Widget InnerWidget()            { return _workArea; }

   const UI_Class UIClass()              { return CONTAINER; }
   const int UISubClass()                { return _container_type; }

   const char *const UIClassName()       { return "Container"; }
};

#endif /* CONTAINER_H */
