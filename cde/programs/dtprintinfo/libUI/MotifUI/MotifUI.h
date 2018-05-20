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
/* $XConsortium: MotifUI.h /main/6 1996/10/21 17:29:46 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef MOTIFUI_H
#define MOTIFUI_H

#include <Xm/Xm.h>

#include "BaseUI.h"
#include "MotifThread.h"

extern "C" {
extern XtPointer _XmStringUngenerate (
				XmString string,
				XmStringTag tag,
				XmTextType tag_type,
				XmTextType output_type);
}


#define AnyUI MotifUI
#define HasDumpUI 1

#define PointInRect(RECT, X, Y) \
    (X >= (int) (RECT).x && \
     Y >= (int) (RECT).y && \
     X <= (int) (RECT).x + (int) (RECT).width && \
     Y <= (int) (RECT).y + (int) (RECT).height)

typedef struct
{
   char *name;
   Pixmap pixmap;
   Pixmap mask;
} PixmapLookupStruct, *PixmapLookup, **PixmapLookupList;

class DtDND;
class MotifThread;

class MotifUI : public BaseUI {

 friend class DtDND;

 private:

   static void WidgetDestroyCB(Widget, XtPointer, XtPointer);
   static void WidgetHelpCB(Widget, XtPointer, XtPointer);
   Cursor InitHourGlassCursor();
   static void DumpWidget(Widget w, boolean verbose = false, int level = 0);
   static void DumpWidgets(Widget w, boolean verbose = false, int level = 0);
   static void ThreadCB(MotifThread *_thread, BaseUI *obj, ThreadCallback cb);

 protected:

   Widget _w;
   char *_widgetName;

   MotifUI(MotifUI *parent, 
	   const char *name,
	   const char *category,
	   const char *widgetName = NULL);
   void InstallDestroyCB();
   void InstallHelpCB();
   virtual void WidgetDestroyed();
   void GetPixmaps(Widget widget, char *iconFile, Pixmap *pixmap,
		   Pixmap *mask = NULL);
   void FillBackground(Widget widget, Pixmap pixmap, Pixmap mask);
   void SetDefaultResources(const Widget, const String *);
   void GetResources(const XtResourceList, const int);
   XmString StringCreate(char *string)
       { if (string) return XmStringCreateLocalized(string);
	 else return NULL;
       }
   void StringFree(XmString string) 
       { if (string) XmStringFree(string); string = NULL; }
   char * StringExtract(XmString string)
       { char *s = NULL;
  	 s = (char *) _XmStringUngenerate(
			string, NULL,
			XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
	 return s; };

   // Definitions for BaseUI virtual functions
   virtual boolean SetCategory(char * /*name*/) { return true; }
   virtual boolean SetName(char *name);
   virtual boolean SetActivity(boolean flag);
   virtual boolean SetVisiblity(boolean flag);
   virtual boolean SetSelected(boolean flag);
   void DoSetFocus(Widget);
   void DoRefresh();
   void DoToFront();
   void DoContextualHelp();
   virtual void DoMakeVisible();
   virtual boolean DoIsVisible();
   virtual void DoBeginUpdate() { }
   virtual void DoEndUpdate() { }
   virtual void DoUpdateMessage(char * /*message*/) { }
   virtual boolean SetOrder(int new_postion);

   // Containers widgets should override SetView
   virtual boolean SetView(ViewStyle) { return true; }

   // Containers and icon widgets should override these
   virtual boolean SetOpen(boolean) { return true; }

   // Icon widgets should override these
   virtual boolean SetIcon(IconStyle) { return true; }
   virtual boolean SetParent(BaseUI *) { return true; }

   virtual void NotifyDelete(BaseUI *);
   static PixmapLookupList pixmap_table;
   static int n_pixmaps;
   static PointerCursor pointer_style;

 public:

   static Widget topLevel;
   static Display *display;
   static XtAppContext appContext;
   static XmFontList userFont;
   static Font font;
   static Window root;
   static Pixel black;
   static Pixel white;
   static int depth;
   static int shadowThickness;
   static int bMenuButton;

   virtual ~MotifUI();

   void Thread(const char *cmd, ThreadCallback, int buf_len);
   void Thread(int pid, int fd, ThreadCallback, int buf_len);
   void Thread(int socket, ThreadCallback, int buf_len);

   virtual void SetFocus();
   void PointerShape(PointerCursor style);
   PointerCursor PointerShape();
   virtual void WidthHeight(int w, int h);
   virtual void WidthHeight(int *w, int *h);
   virtual void Width(int w);
   virtual int Width();
   virtual void Height(int h);
   virtual int Height();
   void AttachAll(int offset = 0);
   void AttachTop(int offset = 0);
   void AttachBottom(int offset = 0);
   void AttachLeft(int offset = 0);
   void AttachRight(int offset = 0);
   void AttachTop(BaseUI *, int offset = 0, boolean opposite = false);
   void AttachBottom(BaseUI *, int offset = 0, boolean opposite = false);
   void AttachLeft(BaseUI *, int offset = 0, boolean opposite = false);
   void AttachRight(BaseUI *, int offset = 0, boolean opposite = false);
   void StringWidthHeight(const char *string, int *w, int *h);
   int StringWidth(const char *string);
   int StringHeight(const char *string);
   void SetAddTimeOut(TimeOutCallback, void *callback_data, long interval);
   void DumpUIHierarchy(boolean verbose = false, int level = 0);

   const Widget BaseWidget()             { return _w; }
   virtual const Widget InnerWidget()    { return _w; }

   void Dump(boolean verbose = false,
	     int level = 0);

   // returns a classname to be used in GetResources
   virtual const char *const className() { return "MotifUI"; }

};

#endif /* MOTIFUI_H */
