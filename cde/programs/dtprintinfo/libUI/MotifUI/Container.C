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
/* $XConsortium: Container.C /main/2 1995/07/17 14:05:19 drk $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "Container.h"
#include "IconObj.h"

#include <Xm/Xm.h>
#include <Xm/DropSMgr.h>
#include "Icon.h"
#include "WorkArea.h"
#include <Xm/ScrolledW.h>
#include <Xm/PanedW.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/DrawingA.h>

#include <stdio.h>

Container::Container(MotifUI *parent,
		     char *name, 
		     ContainerType container_type,
		     SelectionType select_type)
	: MotifUI(parent, name, NULL)
{
   CreateContainer(parent, name, NULL, container_type, select_type);
}

Container::Container(char *category, 
		     MotifUI *parent,
		     char *name, 
		     ContainerType container_type,
		     SelectionType select_type)
	: MotifUI(parent, name, category)
{
   CreateContainer(parent, name, category, container_type, select_type);
}

static XtTranslations trans_tbl = NULL;
static XtActionsRec action_table[] = {{"ResizeRC", NULL}};

void Container::CreateContainer(MotifUI *parent, char * /*name*/,
				char * /*category*/,
		                ContainerType container_type,
		                SelectionType select_type)
{
   Arg args[15];
   int n;
   Pixel pixel;
   Widget parentW;
   Widget superNode;
   int radio_behavior = false;
   int _shadowThickness;

   if (!trans_tbl)
    {
      action_table[0].proc = (XtActionProc) ResizeRC;
      trans_tbl = XtParseTranslationTable("<Configure>:ResizeRC()");
      XtAppAddActions(appContext, action_table, XtNumber(action_table));
    }

   _xm_update_message = NULL;
   _select_type = select_type;
   _last_selected = NULL;
   parentW = parent->InnerWidget();
   if (GuiIsIcon(parentW))
      superNode = parentW;
   else
      superNode = NULL;
   if (!XtIsComposite(parentW))
      parentW = XtParent(parentW);
   if (select_type == SINGLE_SELECT)
      radio_behavior = true;

   switch (_container_type = container_type)
    {
     case FORM:
       _w = XtVaCreateManagedWidget("form", xmFormWidgetClass,
				    parentW, NULL);
       _workArea = _w;
       break;
     case SCROLLED_FORM:
       _w = XtVaCreateManagedWidget("sw", xmScrolledWindowWidgetClass,
				    parentW, NULL);
       _workArea = XtVaCreateManagedWidget("form", xmFormWidgetClass,
				           _w, NULL);
       break;
     case CANVAS:
       _w = XtVaCreateManagedWidget("canvas", xmDrawingAreaWidgetClass,
				    parentW, NULL);
       _workArea = _w;
       break;
     case SCROLLED_CANVAS:
       _w = XtVaCreateManagedWidget("sw", xmScrolledWindowWidgetClass,
				    parentW, NULL);
       _workArea = XtVaCreateManagedWidget("canvas", xmDrawingAreaWidgetClass,
				           _w, NULL);
       break;
     case HORIZONTAL_ROW_COLUMN:
       _w = XtVaCreateManagedWidget("rc", xmRowColumnWidgetClass, parentW,
				    XmNorientation, XmHORIZONTAL,
                                    XmNradioBehavior, radio_behavior, NULL);
       _workArea = _w;
       break;
     case SCROLLED_HORIZONTAL_ROW_COLUMN:
       _w = XtVaCreateManagedWidget("sw", xmScrolledWindowWidgetClass, parentW,
				    GuiNisOpened, false, GuiNisWorkArea, true,
				    GuiNsuperNode, superNode, XmNspacing, 0,
				    XmNscrollBarDisplayPolicy, XmAS_NEEDED,
				    XmNscrollingPolicy, XmAUTOMATIC,
				    XmNheight, Parent()->Height(), NULL);
       XtAddCallback(_w, XmNtraverseObscuredCallback, MakeChildVisible, NULL);
       _workArea = XtVaCreateManagedWidget("rc", xmRowColumnWidgetClass, _w,
					   XmNorientation, XmHORIZONTAL, 
                                           XmNradioBehavior, radio_behavior,
                                           XmNpacking, XmPACK_TIGHT,
					   XmNadjustLast, False,
					   XmNuserData, this, NULL);
       XtAddCallback(XtParent(_workArea), XmNresizeCallback, 
	             ResizeSW, (XtPointer) this);
       XtOverrideTranslations(_workArea, trans_tbl);
       break;
     case VERTICAL_ROW_COLUMN:
       _w = XtVaCreateManagedWidget("rc", xmRowColumnWidgetClass, parentW,
                                    XmNradioBehavior, radio_behavior, NULL);
       _workArea = _w;
       break;
     case SCROLLED_VERTICAL_ROW_COLUMN:
       _w = XtVaCreateManagedWidget("sw", xmScrolledWindowWidgetClass, parentW, 
				    GuiNisOpened, false, GuiNisWorkArea, true,
				    GuiNsuperNode, superNode, XmNspacing, 0,
                                    XmNradioBehavior, radio_behavior,
				    XmNscrollBarDisplayPolicy, XmAS_NEEDED,
				    XmNscrollingPolicy, XmAUTOMATIC, NULL);
       _workArea = XtVaCreateManagedWidget("rc", xmRowColumnWidgetClass, _w,
                                           XmNradioBehavior, radio_behavior,
					   NULL);
       break;
     case PANE:
       _w = XtVaCreateManagedWidget("pane", xmPanedWindowWidgetClass,
				    parentW, NULL);
       _workArea = _w;
       break;
     case SCROLLED_PANE:
       _w = XtVaCreateManagedWidget("sw", xmScrolledWindowWidgetClass,
				    parentW, NULL);
       _workArea = XtVaCreateManagedWidget("form", xmPanedWindowWidgetClass,
				           _w, NULL);
       break;
     case WORK_AREA:
       _w = XtVaCreateManagedWidget("form", workAreaWidgetClass,
				    parentW, NULL);
       _workArea = _w;
       break;
     case ICON_LIST:
       _w = XtVaCreateManagedWidget("form", workAreaWidgetClass,
				    parentW, GuiNisList, True, NULL);
       _workArea = _w;
       break;
     case SCROLLED_ICON_LIST:
     case SCROLLED_WORK_AREA:
       if (Parent()->UIClass() == DIALOG)
	  _shadowThickness = 0;
       else
	  _shadowThickness = shadowThickness;
       _w = XtVaCreateManagedWidget("form", xmFormWidgetClass,
				    parentW,
				    XmNshadowThickness, _shadowThickness,
				    XmNshadowType, XmSHADOW_OUT, NULL);
       n = 0;
       if (_container_type == SCROLLED_ICON_LIST)
	{
          XtSetArg(args[n], GuiNisList, True); n++;
	}
       else
	{
          XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
          XtSetArg(args[n], XmNentryVerticalAlignment, 
		   XmALIGNMENT_CONTENTS_BOTTOM);
          n++;
	}
       XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
       XtSetArg(args[n], XmNtopOffset, 6); n++;
       XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
       XtSetArg(args[n], XmNleftOffset, 6); n++;
       XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
       XtSetArg(args[n], XmNrightOffset, 6); n++;
       XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
       XtSetArg(args[n], XmNbottomOffset, 6); n++;
       XtSetArg(args[n], GuiNlineOffset, 20); n++;
       XtSetArg(args[n], GuiNlineThickness, 2); n++;
       XtSetArg(args[n], XmNuserData, this); n++;
       
       _workArea = GuiCreateScrolledWorkArea(_w, "workArea", args, n);
       XtManageChild(_workArea);
       break;
    }

   if (_workArea == _w)
    {
      _clipWidget = _sw = _vbar = _hbar = NULL;
    }
   else
    {
      _clipWidget = XtParent(_workArea);
      _sw = XtParent(_clipWidget);
      XtVaGetValues(_sw, XmNverticalScrollBar, &_vbar, 
                    XmNhorizontalScrollBar, &_hbar, NULL);
      if (container_type == SCROLLED_HORIZONTAL_ROW_COLUMN)
	 XtUnmanageChild(_vbar);

      if (depth > 1 &&
	  (container_type == SCROLLED_WORK_AREA ||
           container_type == SCROLLED_ICON_LIST))
       {
          XtVaGetValues(_vbar, XmNtroughColor, &pixel, NULL);
          XtVaSetValues(_workArea, XmNbackground, pixel, NULL);
          XtVaSetValues(_clipWidget, XmNbackground, pixel, NULL);
       }
    }
   InstallHelpCB();
   InstallDestroyCB();
}

boolean Container::SetIcon(IconStyle)
{
   if (_container_type == SCROLLED_HORIZONTAL_ROW_COLUMN)
      XtAppAddTimeOut(appContext, 500, ResizeTimeOut, this);
   return true;
}

void Container::DoBeginUpdate()
{
   XtAddEventHandler(XtParent(_workArea), ExposureMask, FALSE, 
	&Container::UpdateAreaMessage, (XtPointer) this);

   if (_container_type == SCROLLED_WORK_AREA ||
       _container_type == WORK_AREA ||
       _container_type == SCROLLED_ICON_LIST ||
       _container_type == ICON_LIST)
    {
      if (XtIsRealized(_workArea))
         XtUnmapWidget(_workArea);
      GuiWorkAreaDisableRedisplay(_workArea);
      XmDropSiteStartUpdate(XtParent(_workArea));
    }
   else
      XtUnmanageChild(_workArea);
}

void Container::DoEndUpdate()
{
   XtRemoveEventHandler(XtParent(_workArea), ExposureMask, FALSE, 
	&Container::UpdateAreaMessage, (XtPointer) this);
   StringFree(_xm_update_message);
   if (XtIsRealized(_workArea))
      XClearArea(display, XtWindow(XtParent(_workArea)), 0, 0, 0, 0, TRUE);
   _xm_update_message = NULL;
   if (_container_type == SCROLLED_WORK_AREA ||
       _container_type == WORK_AREA ||
       _container_type == SCROLLED_ICON_LIST ||
       _container_type == ICON_LIST)
    {
      if (XtIsRealized(_workArea))
         XtMapWidget(_workArea);
      GuiWorkAreaEnableRedisplay(_workArea);
      XmDropSiteEndUpdate(XtParent(_workArea));
    }
   else
    {
      XtManageChild(_workArea);
      if (_container_type == SCROLLED_HORIZONTAL_ROW_COLUMN)
         XtAppAddTimeOut(appContext, 500, ResizeTimeOut, this);
    }
}

void Container::DoUpdateMessage(char *message)
{
   StringFree(_xm_update_message);
   _xm_update_message = StringCreate(message);
   if (XtIsRealized(_workArea))
      XClearArea(display, XtWindow(XtParent(_workArea)), 0, 0, 0, 0, TRUE);
   Refresh();
}

void Container::SelectionPolicy(SelectionType)
{
}

void Container::NotifyDelete(BaseUI *obj)
{
   MotifUI::NotifyDelete(obj);
   if (_last_selected == obj)
      _last_selected = NULL;
   if (_container_type == SCROLLED_HORIZONTAL_ROW_COLUMN)
      XtAppAddTimeOut(appContext, 500, ResizeTimeOut, this);
}

void Container::NotifyVisiblity(BaseUI *obj)
{
   if (_last_selected == obj)
      _last_selected->Selected(false);
}

void Container::NotifySelected(BaseUI *obj)
{
   switch (_select_type)
   {
   case SINGLE_SELECT:
      if (_last_selected && _last_selected != obj)
       {
	 if (obj->Selected())
	    _last_selected->Selected(false);
       }
      if (obj->Selected())
         _last_selected = obj;
      else
         _last_selected = NULL;
      break;
   case MULTIPLE_SELECT:
      break;
   case EXTENDED_SELECT:
      break;
   case BROWSE_SELECT:
      break;
   }
}

// Open Animation stuff

static GC CreateGC(Widget w);

void Container::OpenAnimation(MotifUI *obj)
{
   if (!_update || !_w || !XtIsRealized(_w))
      return;

   Dimension w, h, w2, h2;
   int x0, y0, x1, y1, x2, y2, x_inc1, y_inc1, x_inc2, y_inc2, n;
   Window window, area;
   Widget widget;
   GC gc;

   if (_clipWidget)
      widget = _clipWidget;
   else
      widget = _w;

   XtVaGetValues(widget, XmNwidth, &w, XmNheight, &h, NULL);
   XtVaGetValues(obj->BaseWidget(), XmNwidth, &w2, XmNheight, &h2, NULL);

   area = XtWindow(widget);
   x1 = (int)w2 / 2;
   y1 = (int)h2 / 2;
   XTranslateCoordinates(display, XtWindow(obj->BaseWidget()), area, 
        x1, y1, &x0, &y0, &window);
   n = 8;
   x_inc1 = -x0 / n;
   if (x_inc1 == 0)
      x_inc1 = -1;
   y_inc1 = -y0 / n;
   if (y_inc1 == 0)
      y_inc1 = -1;
   x_inc2 = (int)(w - x0) / n;
   if (x_inc2 == 0)
      x_inc2 = 1;
   y_inc2 = (int)(h - y0) / n;
   if (y_inc2 == 0)
      y_inc2 = 1;
   x1 = x0 + x_inc1;
   y1 = y0 + y_inc1;
   x2 = x0 + x_inc2;
   y2 = y0 + y_inc2;

   gc = CreateGC(widget);
   while (--n)
    {
      XDrawRectangle(display, area, gc, x1, y1, x2 - x1, y2 - y1);
      XFlush(display);
      x1 += x_inc1;
      y1 += y_inc1;
      x2 += x_inc2;
      y2 += y_inc2;
      MicroSleep(OPEN_TIME);
    }
   n = 8;
   x1 = x0 + x_inc1;
   y1 = y0 + y_inc1;
   x2 = x0 + x_inc2;
   y2 = y0 + y_inc2;

   while (--n)
    {
      XDrawRectangle(display, area, gc, x1, y1, x2 - x1, y2 - y1);
      XFlush(display);
      x1 += x_inc1;
      y1 += y_inc1;
      x2 += x_inc2;
      y2 += y_inc2;
      MicroSleep(OPEN_TIME);
    }
   XtReleaseGC(widget, gc);
}

static GC CreateGC(Widget w)
{
   XGCValues values;

   XtVaGetValues(w, XmNforeground, &values.foreground, 
                 XmNbackground, &values.background, NULL);
   // Set the fg to the XOR of the fg and bg, so if it is
   // XOR'ed with bg, the result will be fg and vice-versa.
   // This effectively achieves inverse video for the line.
    
   values.foreground = values.foreground ^ values.background;
   // Set the rubber band gc to use XOR mode and draw a dashed line.

   values.function   = GXxor;
   values.subwindow_mode  = IncludeInferiors;
   return (XtGetGC(w, GCForeground | GCBackground |
                   GCSubwindowMode | GCFunction, &values));
}

void Container::UpdateAreaMessage(Widget widget, XtPointer client_data, 
                                  XEvent * /*event*/, Boolean * /*continued*/)
{
   unsigned char direction;
   XGCValues	  values;
   XtGCMask	  valueMask;
   Dimension	  width, height;
   Dimension	  w, h;
   int		  x, y;

   static GC gc = (GC) NULL;

   Container *container = (Container *) client_data;

   if (!container->_xm_update_message)
      return;

   if (!gc)
    {
      XtVaGetValues(widget, XmNforeground, &values.foreground, NULL);
      if (container->font)
         valueMask = GCFont | GCForeground;
      else
         valueMask = GCForeground;
      values.font = container->font;
      gc = XCreateGC(display, root, valueMask, &values);
    }
   XClearArea(display, XtWindow(widget), 0, 0, 0, 0, FALSE);
   XtVaGetValues(widget, XmNwidth, &width, XmNheight, &height, NULL);
   XtVaGetValues(container->_workArea, XmNstringDirection, &direction, NULL);
   XmStringExtent(container->userFont, container->_xm_update_message, &w, &h);
   x = (Dimension)(width - w) / 2;
   y = (Dimension)(height - h) / 2;
   XmStringDraw(display, XtWindow(widget), container->userFont,
      container->_xm_update_message,
      gc, x, y, w, XmALIGNMENT_CENTER, direction, NULL);
}

void Container::MakeChildVisible(Widget widget, XtPointer, XtPointer call_data)
{
   XmTraverseObscuredCallbackStruct *cb;
   Widget w;

   cb = (XmTraverseObscuredCallbackStruct *) call_data;
   w = cb->traversal_destination;
   XmScrollVisible(widget, w, 0, 0);
}

void Container::ResizeTimeOut(void *data, XtIntervalId * /*id*/)
{
   Container *obj = (Container *)data;
   int h4 = obj->Parent()->Height();
   Dimension h1, h2, h3;

   if (obj->NumChildren())
    {
      Dimension max_height;
      MotifUI **children = (MotifUI **)obj->Children();
      int i;
      XtWidgetGeometry preferred;

      max_height = 0;
      for (i = 0; i < obj->NumChildren(); i++)
       {
	 if (XtIsManaged(children[i]->BaseWidget()))
	  {
            XtQueryGeometry(children[i]->BaseWidget(), NULL, &preferred);
            if ((h1 = preferred.height) > max_height)
	     {
	       max_height = h1;
	       break;
	     }
	  }
       }
      Position y;
      if (max_height)
         XtVaGetValues(children[0]->BaseWidget(), XmNy, &y, NULL);
      else
	 y = 0;
      h1 = max_height + (2 * y);
    }
   else
      XtVaGetValues(obj->_workArea, XmNheight, &h1, NULL);
   XtVaGetValues(obj->_clipWidget, XmNheight, &h2, NULL);
   XtVaGetValues(obj->_w, XmNheight, &h3, NULL);
   h3 = h3 - h2 + h1;
   if (h2 != h1)
    {
      if (h4 > (int)h3)
         h3 = h4;
      else if (obj->NumChildren() == 0)
         return;
      h4 = h3;
    }
   else if (h4 <= (int)h3)
      return;
   XtVaSetValues(obj->_w, XmNheight, h4, NULL);
   obj->Refresh();
   XtVaGetValues(obj->_w, XmNheight, &h3, NULL);
   if (h3 != h4)
      XtAppAddTimeOut(obj->appContext, 500, ResizeTimeOut, obj);
}

void Container::ResizeSW(Widget w, XtPointer client_data, XtPointer)
{
   // Don't calculate resize here,  since the scroll bar hasn't been
   // updated.  Instead,  add a sufficient timeout to calculate the resize
   // after the scroll bar has updated it's XmNvalue resource
   XtAppAddTimeOut(XtWidgetToApplicationContext(w), 
	           500, ResizeTimeOut, client_data);
}

void Container::ResizeRC(Widget w, XEvent *, String *, int *)
{
   Container *obj;
   XtVaGetValues(w, XmNuserData, &obj, NULL);
   XtAppAddTimeOut(obj->appContext, 500, ResizeTimeOut, obj);
}
