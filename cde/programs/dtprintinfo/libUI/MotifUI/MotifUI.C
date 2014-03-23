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
/* $TOG: MotifUI.C /main/8 1998/08/03 08:59:09 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "MotifUI.h"
#include "Menu.h"
#include "Dialog.h"
#include "Icon.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include <X11/cursorfont.h>
#include <X11/IntrinsicP.h>

#ifdef NO_CDE
#include "xpm.h"
#include <X11/keysym.h>
#else
#include <Dt/xpm.h>
#include <Dt/Help.h>
#endif

#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrolledW.h>
#include <Xm/MainW.h>

PixmapLookupList MotifUI::pixmap_table = NULL;
XtAppContext     MotifUI::appContext = NULL;
XmFontList       MotifUI::userFont = NULL;
Display *        MotifUI::display = NULL;
Widget           MotifUI::topLevel;
Window           MotifUI::root;
Font             MotifUI::font;
Pixel            MotifUI::black;
Pixel            MotifUI::white;
int              MotifUI::shadowThickness;
int              MotifUI::depth;
int		 MotifUI::bMenuButton;
int              MotifUI::n_pixmaps = 0;
PointerCursor    MotifUI::pointer_style = LEFT_SLANTED_ARROW_CURSOR;


MotifUI::MotifUI(MotifUI *parent, 
		 const char *name,
		 const char *category,
		 const char *widgetName)
	: BaseUI(parent, name, category)
{
   _w = NULL;
   if (widgetName)
      _widgetName = STRDUP(widgetName);
   else
      _widgetName = STRDUP(name);
}

MotifUI::~MotifUI()
{
   if (_w)
      XtRemoveCallback(_w, XmNdestroyCallback, &MotifUI::WidgetDestroyCB, 
		       (XtPointer) this);
   delete _widgetName;
}

void MotifUI::ThreadCB(MotifThread *_thread, BaseUI *obj, ThreadCallback cb)
{
   (*cb)(obj, _thread->output, _thread->status);
   delete _thread;
}

void MotifUI::Thread(const char *cmd, ThreadCallback cb, int buf_len)

{
   new MotifThread(this, cmd, &(MotifUI::ThreadCB), cb, buf_len);
}

void MotifUI::Thread(int pid, int fd, ThreadCallback cb, int buf_len)
{
   new MotifThread(this, pid, fd, &(MotifUI::ThreadCB), cb, buf_len);
}

void MotifUI::Thread(int socket, ThreadCallback cb, int buf_len)
{
   new MotifThread(this, socket, &(MotifUI::ThreadCB), cb, buf_len);
}

void MotifUI::SetFocus()
{
   DoSetFocus(_w);
}

void MotifUI::DoSetFocus(Widget w)
{
   XmProcessTraversal(w, XmTRAVERSE_CURRENT);
}

boolean MotifUI::DoIsVisible()
{
   boolean rc = true;
   if (_w)
    {
      if (UIClass() == APPLICATION)
       {
	 if (XtIsRealized(_w))
	  {
	    XWindowAttributes attributes;
	    XGetWindowAttributes(display, XtWindow(_w), &attributes);
            if (attributes.map_state == IsUnmapped)
               rc = false;
	  }
       }
      else if (XmGetVisibility(_w) == XmVISIBILITY_FULLY_OBSCURED)
         rc = false;
    }
   return rc;
}

void MotifUI::DoMakeVisible()
{
   Widget sw = XtParent(_w);
   if (sw && !XmIsScrolledWindow(sw))
      sw = XtParent(sw);
   if (sw && !XmIsScrolledWindow(sw))
      sw = XtParent(sw);
   if (sw && XmIsScrolledWindow(sw) && !XmIsMainWindow(sw))
      XmScrollVisible(sw, _w, 0, 0);
}

void MotifUI::DoContextualHelp()
{
   Widget context_widget, shell;
#ifdef NO_CDE
   XEvent event;
   static Cursor cursor = (Cursor) NULL;

   if (cursor == (Cursor) NULL)
      cursor =  XCreateFontCursor(display, XC_question_arrow);
#endif

   BaseUI *window = this;
   while (window->UIClass() != MAIN_WINDOW)
      window = window->Parent();
   shell = ((MotifUI *)window)->_w;
#ifdef NO_CDE
   context_widget = XmTrackingEvent(shell, cursor, False, &event);

   if (event.type == KeyPress || event.type == KeyRelease)
    {
      int offset;
      KeySym keySym;

      // Look for ESC key press and stop if we get one 
      if (event.xkey.state & ShiftMask)
         offset = 1;
      else
         offset = 0;

      keySym = XLookupKeysym((XKeyEvent *)&event, offset);
      if (keySym == XK_Escape)
	 return;
    }

   if (context_widget != NULL)
    {
#else
   int returnVal = DtHelpReturnSelectedWidgetId(shell, 0, &context_widget);
   if (returnVal == DtHELP_SELECT_VALID)
    {
#endif
      XmAnyCallbackStruct cb;

      cb.reason = XmCR_HELP;
#ifdef NO_CDE
      cb.event = &event;
#endif
      while (context_widget != NULL)
       {
         // If there is no help at this widget, back track to find help 
         if (XtHasCallbacks(context_widget, XmNhelpCallback) ==
                            XtCallbackHasSome)
          {
            XtCallCallbacks(context_widget, XmNhelpCallback, &cb);
            break;
          }
         else
            context_widget = XtParent(context_widget);
       }
    }
}

void MotifUI::WidgetHelpCB(Widget,
			   XtPointer clientData,
			   XtPointer)
{
   MotifUI * obj = (MotifUI *) clientData;

   obj->HandleHelpRequest();
}

void MotifUI::WidgetDestroyCB(Widget,
			      XtPointer clientData,
			      XtPointer)
{
   MotifUI * obj = (MotifUI *) clientData;

   obj->WidgetDestroyed();
}

void MotifUI::WidgetDestroyed()
{
   _w = NULL;
   delete _widgetName;
}

void MotifUI::DoRefresh()
{
   if (_w)
      XmUpdateDisplay(_w);
}

void MotifUI::DoToFront()
{
   if (_w)
    {
      if (XtIsShell(XtParent(_w)))
	 XRaiseWindow(display, XtWindow(XtParent(_w)));
      else
	 XRaiseWindow(display, XtWindow(_w));
    }
}

void MotifUI::InstallDestroyCB()
{
   if (_w)
      XtAddCallback(_w, XmNdestroyCallback, &MotifUI::WidgetDestroyCB, 
		    (XtPointer) this);
}

void MotifUI::InstallHelpCB()
{
   if (!_w)
      return;

   if (UIClass() == MENU && UISubClass() != POPUP_MENU)
      XtAddCallback(((Menu*)this)->GetCascade(), XmNhelpCallback,
		    &MotifUI::WidgetHelpCB, (XtPointer) this);
   else
      XtAddCallback(_w, XmNhelpCallback, &MotifUI::WidgetHelpCB, 
		    (XtPointer) this);
}

boolean MotifUI::SetSelected(boolean flag)
{
   if (!_w)
      return false;

   if (GuiIsIcon(_w))
      XtVaSetValues(_w, GuiNselected, flag, NULL);
   else if (XmIsToggleButton(_w))
      XmToggleButtonSetState(_w, flag, False);

   return true;
}

boolean MotifUI::SetName(char *name)
{
   if (!InnerWidget())
      return false;

   XmString xm_string = StringCreate(name);
   XtVaSetValues(InnerWidget(), XmNlabelString, xm_string, NULL);
   StringFree(xm_string);

   return true;
}

boolean MotifUI::SetActivity(boolean flag)
{
   if (!_w)
      return false;

   if (GuiIsIcon(_w))
      XtVaSetValues(_w, GuiNactive, flag, NULL);
   else
      XtSetSensitive(_w, flag);

   return true;
}

boolean MotifUI::SetVisiblity(boolean flag)
{
   if (!_w)
      return false;

   if (flag)
      XtManageChild(_w);
   else
      XtUnmanageChild(_w);

   return true;
}

void MotifUI::GetResources(const XtResourceList resources, 
			   const int numResources)
{
   if (_w && resources)
      XtGetSubresources(XtParent(_w), (XtPointer) this, _widgetName,
			className(), resources, numResources, NULL, 0);
}

void MotifUI::SetDefaultResources(const Widget,
			          const String *resources)
{
   XrmDatabase rdb = NULL;
   int         i;

   rdb = XrmGetStringDatabase("");

   i = 0;
   while (resources[i])
    {
      char *buf = new char[1000];

      sprintf(buf, "%s%s", _name, resources[i]);
      XrmPutLineResource(&rdb, buf);
      i++;

      delete [] buf;
    }
   if (rdb)
    {
      XrmMergeDatabases(XtDatabase(display), &rdb);
      XrmSetDatabase(display, rdb);
    }
}

void MotifUI::NotifyDelete(BaseUI *obj)
{
   MotifUI *p = (MotifUI *) obj;
   if (p->_w)
    {
      XtRemoveCallback(_w, XmNdestroyCallback, &MotifUI::WidgetDestroyCB, 
		       (XtPointer) this);
      XtDestroyWidget(p->_w);
      p->_w = NULL;
    }
}

void MotifUI::Width(int width)
{
   XtVaSetValues(this->BaseWidget(), XmNwidth, width, NULL);
}

int MotifUI::Width()
{
   Dimension w;
   XtVaGetValues(this->BaseWidget(), XmNwidth, &w, NULL);
   return (int) w;
}

void MotifUI::Height(int height)
{
   XtVaSetValues(this->BaseWidget(), XmNheight, height, NULL);
}

int MotifUI::Height()
{
   Dimension h;
   XtVaGetValues(this->BaseWidget(), XmNheight, &h, NULL);
   return (int) h;
}

void MotifUI::WidthHeight(int width, int height)
{
   XtVaSetValues(this->BaseWidget(), XmNwidth, width, XmNheight, height, NULL);
}

void MotifUI::WidthHeight(int *width, int *height)
{
   Dimension w, h;

   XtVaGetValues(this->BaseWidget(), XmNwidth, &w, XmNheight, &h, NULL);
   *width = (int) w;
   *height = (int) h;
}

void MotifUI::AttachAll(int offset)
{
   XtVaSetValues(this->BaseWidget(),
                 XmNtopAttachment, XmATTACH_NONE,
                 XmNbottomAttachment, XmATTACH_NONE,
                 XmNleftAttachment, XmATTACH_NONE,
                 XmNrightAttachment, XmATTACH_NONE,
                 NULL);
   XtVaSetValues(this->BaseWidget(),
                 XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, offset,
                 XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, offset,
                 XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, offset,
                 XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, offset,
                 NULL);
}

void MotifUI::AttachTop(int offset)
{
   XtVaSetValues(this->BaseWidget(), XmNtopAttachment, XmATTACH_NONE, NULL);
   XtVaSetValues(this->BaseWidget(), XmNtopAttachment, XmATTACH_FORM,
                 XmNtopOffset, offset, NULL);
}

void MotifUI::AttachBottom(int offset)
{
   XtVaSetValues(this->BaseWidget(), XmNbottomAttachment, XmATTACH_NONE, NULL);
   XtVaSetValues(this->BaseWidget(), XmNbottomAttachment, XmATTACH_FORM,
                 XmNbottomOffset, offset, NULL);
}

void MotifUI::AttachLeft(int offset)
{
   XtVaSetValues(this->BaseWidget(), XmNleftAttachment, XmATTACH_NONE, NULL);
   XtVaSetValues(this->BaseWidget(), XmNleftAttachment, XmATTACH_FORM,
                 XmNleftOffset, offset, NULL);
}

void MotifUI::AttachRight(int offset)
{
   XtVaSetValues(this->BaseWidget(), XmNrightAttachment, XmATTACH_NONE, NULL);
   XtVaSetValues(this->BaseWidget(), XmNrightAttachment, XmATTACH_FORM,
                 XmNrightOffset, offset, NULL);
}

void MotifUI::AttachTop(BaseUI *obj, int offset, boolean opposite)
{
   XtVaSetValues(this->BaseWidget(), XmNtopAttachment, XmATTACH_NONE, NULL);
   if (obj)
    {
      Widget w = ((MotifUI *)obj)->BaseWidget();
      int attachment;
      if (opposite)
         attachment = XmATTACH_OPPOSITE_WIDGET;
      else
         attachment = XmATTACH_WIDGET;

      XtVaSetValues(this->BaseWidget(), XmNtopAttachment, attachment,
                    XmNtopWidget, w, XmNtopOffset, offset, NULL);
    }
}

void MotifUI::AttachBottom(BaseUI *obj, int offset, boolean opposite)
{
   XtVaSetValues(this->BaseWidget(), XmNbottomAttachment, XmATTACH_NONE, NULL);
   if (obj)
    {
      Widget w = ((MotifUI *)obj)->BaseWidget();
      int attachment;
      if (opposite)
         attachment = XmATTACH_OPPOSITE_WIDGET;
      else
         attachment = XmATTACH_WIDGET;

      XtVaSetValues(this->BaseWidget(), XmNbottomAttachment, attachment,
                    XmNbottomWidget, w, XmNbottomOffset, offset, NULL);
    }
}

void MotifUI::AttachLeft(BaseUI *obj, int offset, boolean opposite)
{
   XtVaSetValues(this->BaseWidget(), XmNleftAttachment, XmATTACH_NONE, NULL);
   if (obj)
    {
      int attachment;
      if (opposite)
         attachment = XmATTACH_OPPOSITE_WIDGET;
      else
         attachment = XmATTACH_WIDGET;
      Widget w = ((MotifUI *)obj)->BaseWidget();
      XtVaSetValues(this->BaseWidget(), XmNleftAttachment, attachment,
                    XmNleftWidget, w, XmNleftOffset, offset, NULL);
    }
}

void MotifUI::AttachRight(BaseUI *obj, int offset, boolean opposite)
{
   XtVaSetValues(this->BaseWidget(), XmNrightAttachment, XmATTACH_NONE, NULL);
   if (obj)
    {
      Widget w = ((MotifUI *)obj)->BaseWidget();
      int attachment;
      if (opposite)
         attachment = XmATTACH_OPPOSITE_WIDGET;
      else
         attachment = XmATTACH_WIDGET;

      XtVaSetValues(this->BaseWidget(), XmNrightAttachment, attachment,
                    XmNrightWidget, w, XmNrightOffset, offset, NULL);
    }
}

void MotifUI::StringWidthHeight(const char *string, int *width, int *height)
{
   Dimension w, h;

   XmString xm_string = StringCreate((char *)string);
   XmStringExtent(userFont, xm_string, &w, &h);
   *width = w;
   *height = h;
   StringFree(xm_string);
}

int MotifUI::StringWidth(const char *string)
{
   int dummy;
   int width;
   StringWidthHeight(string, &width, &dummy);
   return width;
}

int MotifUI::StringHeight(const char *string)
{
   int dummy;
   int height;
   StringWidthHeight(string, &dummy, &height);
   return height;
}

boolean MotifUI::SetOrder(int new_position)
{
   if (XmIsRowColumn(((MotifUI *)Parent())->InnerWidget()))
       XtVaSetValues(BaseWidget(), XmNpositionIndex, new_position, NULL);
   return true;
}

void MotifUI::Dump(boolean verbose, 
		   int level)
{
   if (verbose)
    {
      BaseUI::Dump(true, level);

      int i;
      for (i = -2; i < level; i++) printf("   ");
      printf("BaseWidget : %08lx\n", _w);
    }
   else
      BaseUI::Dump(false, level);
}

// Time out stuff

typedef struct
{
   TimeOutCallback fp;
   BaseUI *obj;
   void *callback_data;
} TimeOutCallbackData;

static void
ObjectTimeProc(XtPointer callback_data, XtIntervalId * /*id*/)
{
   TimeOutCallbackData *data = (TimeOutCallbackData *) callback_data;
   (*data->fp)(data->obj, data->callback_data);
   delete data;
   data = NULL;
}

void MotifUI::SetAddTimeOut(TimeOutCallback timeoutCB,
			    void *callback_data,
			    long interval)
{
   if (!_w)
      return;

   TimeOutCallbackData * data = new TimeOutCallbackData;
   data->fp = timeoutCB;
   data->obj = this;
   data->callback_data = callback_data;
   XtAppAddTimeOut(appContext, (unsigned long) interval, ObjectTimeProc, data);
}

void MotifUI::FillBackground(Widget widget, Pixmap pixmap, Pixmap mask)
{
   static Pixmap temp = 0L;
   static GC gc = 0L;
   static unsigned int old_width = 0, old_height = 0, old_depth = 0;
   unsigned int width, height, junk, dep;
   Window root;
   XGCValues xgc;

   XGetGeometry(display, pixmap, &root, (int *) &junk,
		(int *) &junk, &width, &height, &junk, &dep);
   if (temp &&
       (old_width < width || old_height < height || old_depth != dep))
    {
      // Free resources
      XFreeGC(display, gc);
      XFreePixmap(display, temp);
      temp = 0L;
      gc = 0L;
    }

   old_width = width;
   old_height = height;
   old_depth = dep;
   if (dep == 1 && UIClass() == MAIN_WINDOW)
      XtVaGetValues(widget, XmNforeground, &xgc.foreground, NULL);
   else
      XtVaGetValues(widget, XmNbackground, &xgc.foreground, NULL);
   if (!temp)
    {
      temp = XCreatePixmap(display, RootWindowOfScreen(XtScreen(widget)),
                           width, height, dep);
      xgc.function = GXcopy;
      gc = XCreateGC(display, pixmap, GCForeground|GCFunction, &xgc);
    }
   else
      XSetForeground(display, gc, xgc.foreground);

   XFillRectangle(display, temp, gc, 0, 0, width, height);
   if (mask != XmUNSPECIFIED_PIXMAP)
      XSetClipMask(display, gc, mask);
   else
      XSetClipMask(display, gc, None);
   XCopyArea(display, pixmap, temp, gc, 0, 0, width, height, 0, 0);
   XSetClipMask(display, gc, None);
   XCopyArea(display, temp, pixmap, gc, 0, 0, width, height, 0, 0);
}

// Pixmap caching utility
void MotifUI::GetPixmaps(Widget w,
                         char *name,
                         Pixmap *pixmap,
                         Pixmap *mask)
{
   // Try to find pixmap in cache
   PixmapLookupList pixmaps = pixmap_table;
   int i;
   for (i = 0; i < n_pixmaps; i++, pixmaps++)
      if (!strcmp((**pixmaps).name, name))
       {
	 *pixmap = (**pixmaps).pixmap;
	 if (mask)
	    *mask = (**pixmaps).mask;
	 return;
       }

   Pixmap _mask;
   char *s;
   SubstitutionRec subs[1];
   char *bmPath;
   char *PIXMAP_DIR = "/usr/dt/appconfig/icons/%L/%B:"
                      "/usr/dt/appconfig/icons/C/%B:"
                      "/usr/include/X11/bitmaps/%B";

   if (*name == '/')
     s = name;
   else
    {
#ifdef NO_CDE
      if ((s = getenv("XBMLANGPATH")) && *s)
#else
      if ((s = getenv("XMICONSEARCHPATH")) && *s)
#endif
       {
         bmPath = new char [strlen(s) + strlen(PIXMAP_DIR) + 2];
         sprintf(bmPath, "%s:%s", PIXMAP_DIR, s);
       }
      else
         bmPath = PIXMAP_DIR;
      subs[0].match = 'B';
      subs[0].substitution = name;
      s = XtFindFile(bmPath, subs, XtNumber(subs), NULL);
      if (bmPath != PIXMAP_DIR)
         delete [] bmPath;
    }

   struct stat statbuf;
   if (!s || stat(s, &statbuf) < 0)
    {
      *pixmap = XmUNSPECIFIED_PIXMAP;
      if (mask)
         *mask = XmUNSPECIFIED_PIXMAP;
      return;
    }

   int len = strlen(s);
   if (!strcmp(s + len - 3, ".pm"))
    {
      XpmAttributes attributes;
      memset((char *)&attributes, 0, sizeof(XpmAttributes));
#ifdef NO_CDE
      XpmReadFileToPixmap(display, root, s, pixmap, &_mask, &attributes);
#else
      _DtXpmReadFileToPixmap(display, root, s, pixmap, &_mask, &attributes);
#endif

      if (_mask)
         FillBackground(w, *pixmap, _mask);
#ifdef NO_CDE
      XpmFreeAttributes(&attributes);
#else
      _DtXpmFreeAttributes(&attributes);
#endif
    }
   else
    {
      if (UIClass() == MAIN_WINDOW)
         *pixmap = XmGetPixmapByDepth(XtScreen(w), s, white, black, depth);
      else
         *pixmap = XmGetPixmapByDepth(XtScreen(w), s, black, white, depth);
      char *s1 = new char [len + 3];
      strcpy(s1, s);
      strcpy(s1 + len - 3, "_m.bm");
      if (stat(s1, &statbuf) < 0)
         _mask = XmUNSPECIFIED_PIXMAP;
      else
       {
         _mask = XmGetPixmapByDepth(XtScreen(w), s1, white, black, 1);
         FillBackground(w, *pixmap, _mask);
       }
      delete [] s1;
    }
   if (mask)
      *mask = _mask;
   if (s != name)
      XtFree(s);

   // Add pixmap to table
   if (!(n_pixmaps % 8))
    {
      pixmaps = new PixmapLookup [n_pixmaps + 8];
      for (i = 0; i < n_pixmaps; i++)
	 pixmaps[i] = pixmap_table[i];
      for (i = n_pixmaps; i < n_pixmaps + 8; i++)
	 pixmaps[i] = new PixmapLookupStruct;
      delete []pixmap_table;
      pixmap_table = pixmaps;
    }
   pixmap_table[n_pixmaps]->name = strdup(name);
   pixmap_table[n_pixmaps]->pixmap = *pixmap;
   pixmap_table[n_pixmaps]->mask = _mask;
   n_pixmaps++;
}

// Cursor Shape support

#define time32_width 32
#define time32_height 32
#define time32_x_hot 15
#define time32_y_hot 15
static unsigned char time32_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0x7f, 0xfe, 0xff, 0xff, 0x7f,
   0x8c, 0x00, 0x00, 0x31, 0x4c, 0x00, 0x00, 0x32, 0x4c, 0x00, 0x00, 0x32,
   0x4c, 0x00, 0x00, 0x32, 0x4c, 0x00, 0x00, 0x32, 0x4c, 0x00, 0x00, 0x32,
   0x8c, 0x00, 0x00, 0x31, 0x0c, 0x7f, 0xfe, 0x30, 0x0c, 0xfe, 0x7f, 0x30,
   0x0c, 0xfc, 0x3f, 0x30, 0x0c, 0xf8, 0x1f, 0x30, 0x0c, 0xe0, 0x07, 0x30,
   0x0c, 0x80, 0x01, 0x30, 0x0c, 0x80, 0x01, 0x30, 0x0c, 0x60, 0x06, 0x30,
   0x0c, 0x18, 0x18, 0x30, 0x0c, 0x04, 0x20, 0x30, 0x0c, 0x02, 0x40, 0x30,
   0x0c, 0x01, 0x80, 0x30, 0x8c, 0x00, 0x00, 0x31, 0x4c, 0x80, 0x01, 0x32,
   0x4c, 0xc0, 0x03, 0x32, 0x4c, 0xf0, 0x1f, 0x32, 0x4c, 0xff, 0xff, 0x32,
   0xcc, 0xff, 0xff, 0x33, 0x8c, 0xff, 0xff, 0x31, 0xfe, 0xff, 0xff, 0x7f,
   0xfe, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00};

#define time32m_width 32
#define time32m_height 32
static unsigned char time32m_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xcf, 0x00, 0x00, 0xf3, 0x6e, 0x00, 0x00, 0x76, 0x6e, 0x00, 0x00, 0x76,
   0x6e, 0x00, 0x00, 0x76, 0x6e, 0x00, 0x00, 0x76, 0x6e, 0x00, 0x00, 0x76,
   0xce, 0x00, 0x00, 0x73, 0x8e, 0x7f, 0xfe, 0x71, 0x0e, 0xff, 0xff, 0x70,
   0x0e, 0xfe, 0x7f, 0x70, 0x0e, 0xfc, 0x3f, 0x70, 0x0e, 0xf8, 0x1f, 0x70,
   0x0e, 0xe0, 0x07, 0x70, 0x0e, 0xe0, 0x07, 0x70, 0x0e, 0x78, 0x1e, 0x70,
   0x0e, 0x1c, 0x38, 0x70, 0x0e, 0x06, 0x60, 0x70, 0x0e, 0x03, 0xc0, 0x70,
   0x8e, 0x01, 0x80, 0x71, 0xce, 0x00, 0x00, 0x73, 0x6e, 0x80, 0x01, 0x76,
   0x6e, 0xc0, 0x03, 0x76, 0x6e, 0xf0, 0x1f, 0x76, 0x6e, 0xff, 0xff, 0x76,
   0xee, 0xff, 0xff, 0x77, 0xcf, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

#define time16_x_hot 7
#define time16_y_hot 7
#define time16_width 16
#define time16_height 16
static unsigned char time16_bits[] = {
   0x00, 0x00, 0xfe, 0x7f, 0x14, 0x28, 0x14, 0x28, 0x14, 0x28, 0x24, 0x24,
   0x44, 0x22, 0x84, 0x21, 0x84, 0x21, 0x44, 0x22, 0x24, 0x24, 0x14, 0x28,
   0x94, 0x29, 0xd4, 0x2b, 0xfe, 0x7f, 0x00, 0x00};

#define time16m_width 16
#define time16m_height 16
static unsigned char time16m_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f,
   0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f,
   0xfe, 0x7f, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff};

Cursor MotifUI::InitHourGlassCursor()
{
    unsigned int width;
    unsigned int height;
    unsigned int xHotspot;
    unsigned int yHotspot;
    Pixmap       pixmap;
    Pixmap       maskPixmap;
    XColor       xcolors[2];
    char *	 bits;
    char *	 maskBits;
    Cursor       cursor = None;

    if (XQueryBestCursor(display, root, 32, 32, &width, &height))
      {
	if ((width >= 32) && (height >= 32))
	  {
	    width = time32_width;
	    height = time32_height;
	    bits = (char *)time32_bits;
	    maskBits = (char *)time32m_bits;
	    xHotspot = time32_x_hot;
	    yHotspot = time32_y_hot;
	  }
	else
	  {
	    width = time16_width;
	    height = time16_height;
	    bits = (char *)time16_bits;
	    maskBits = (char *)time16m_bits;
	    xHotspot = time16_x_hot;
	    yHotspot = time16_y_hot;
	  }

        pixmap = XCreateBitmapFromData(display, root, bits, width, height);

        maskPixmap = XCreateBitmapFromData(display, root, maskBits,
					   width, height);
        xcolors[0].pixel = black;
        xcolors[1].pixel = white;

        XQueryColors(display, 
	    DefaultColormapOfScreen(DefaultScreenOfDisplay (display)),
	    xcolors, 2);
	cursor = XCreatePixmapCursor(display, pixmap, maskPixmap,
	    &(xcolors[0]), &(xcolors[1]), xHotspot, yHotspot);
        XFreePixmap(display, pixmap);
        XFreePixmap(display, maskPixmap);
      }
    return cursor;
}

PointerCursor MotifUI::PointerShape()
{
   return pointer_style;
}

void MotifUI::PointerShape(PointerCursor style)
{
   static Cursor left = (Cursor) 0L;
   static Cursor right = (Cursor) 0L;
   static Cursor watch = (Cursor) 0L;
   static Cursor hour_glass = (Cursor) 0L;
   static Cursor ibeam = (Cursor) 0L;
   static Cursor cross_hair = (Cursor) 0L;

   if (!_w)
      return;

   Cursor cursor;
   switch (pointer_style = style)
   {
   case LEFT_SLANTED_ARROW_CURSOR: 
      if (!left)
         left = XCreateFontCursor(display, XC_left_ptr);
      cursor = left;
      break;
   case RIGHT_SLANTED_ARROW_CURSOR:
      if (!right)
         right = XCreateFontCursor(display, XC_right_ptr);
      cursor = right;
      break;
   case WATCH_CURSOR:
      if (!watch)
         watch = XCreateFontCursor(display, XC_watch);
      cursor = watch;
      break;
   case HOUR_GLASS_CURSOR:
      if (!hour_glass)
         hour_glass = InitHourGlassCursor();
      cursor = hour_glass;
      break;
   case IBEAM_CURSOR:
      if (!ibeam)
         ibeam = XCreateFontCursor(display, XC_xterm);
      cursor = ibeam;
      break;
   case CROSS_HAIRS_CURSOR:
      if (!cross_hair)
         cross_hair = XCreateFontCursor(display, XC_crosshair);
      cursor = cross_hair;
      break;
   default: cursor = None; break;
   }

   if (XtIsRealized(_w))
      XDefineCursor(display, XtWindow(_w), cursor);
}

static int G_width;

static void VerbosePass1(Widget w, int level)
{
   int i;
   if (level == 0)
      G_width = 0;
   int new_width = (level * 3) + strlen(XrmQuarkToString(w->core.xrm_name)) +
		   strlen(w->core.widget_class->core_class.class_name) + 3;
   if (new_width > G_width)
      G_width = new_width;
   if (XtIsWidget(w))
      for (i = 0; i < w->core.num_popups; i++)
	 VerbosePass1(w->core.popup_list[i], level + 1);
   if (XtIsComposite(w))
    {
      CompositeWidget cw = (CompositeWidget) w;
      for (i = 0; i < cw->composite.num_children; i++)
	 VerbosePass1(cw->composite.children[i], level + 1);
    }
}

void MotifUI::DumpWidget(Widget w, boolean verbose, int level)
{
   int i;

   for (i = 0; i < level; i++)
      printf("   ");
   printf("%s : %s", XrmQuarkToString(w->core.xrm_name),
	  w->core.widget_class->core_class.class_name);
   if (verbose)
    {
      int n = (level * 3) + strlen(XrmQuarkToString(w->core.xrm_name)) +
	      strlen(w->core.widget_class->core_class.class_name) + 3;
      for ( ; n < G_width; n++)
         printf(" ");
      if (XtIsManaged(w))
	 printf(" Managed  ");
      else
	 printf(" Unmanaged");
      if (XtIsSensitive(w))
	 printf(" Sensitive  ");
      else
	 printf(" Insensitive");
      if (XtIsRealized(w))
	 printf(" Realized  ");
      else
	 printf(" Unrealized");
      if (w->core.visible)
	 printf("  Visible\n");
      else
	 printf("  Invisible\n");
    }
   else
      printf("\n");
}

void MotifUI::DumpWidgets(Widget w, boolean verbose, int level)
{
   DumpWidget(w, verbose, level);

   int i;
   if (XtIsWidget(w))
    {
      for (i = 0; i < w->core.num_popups; i++)
	 DumpWidgets(w->core.popup_list[i], verbose, level + 1);
    }
   if (XtIsComposite(w))
    {
      CompositeWidget cw = (CompositeWidget) w;
      for (i = 0; i < cw->composite.num_children; i++)
	 DumpWidgets(cw->composite.children[i], verbose, level + 1);
    }
}

void MotifUI::DumpUIHierarchy(boolean verbose, int level)
{
   if (verbose)
      VerbosePass1(_w, 0);
   DumpWidgets(_w, verbose, level);
}
