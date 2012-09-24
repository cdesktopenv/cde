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
/* $TOG: DtDND.C /main/7 1998/08/03 08:58:53 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "stdlib.h"

#include "DtDND.h"
#include "IconObj.h"
#include "Icon.h"
#include "Dt/Dnd.h"
#include <Xm/DragCP.h>
#include <Dt/Wsm.h>
#include <Dt/Action.h>

#include <stdio.h>

extern "C" {
extern XtPointer _XmStringUngenerate (
				XmString string,
				XmStringTag tag,
				XmTextType tag_type,
				XmTextType output_type);
}

#define DRAG_THRESHOLD 10

// Absolute value macro
#ifndef ABS
#define ABS(x) (((x) > 0) ? (x) : (-(x)))
#endif

boolean DtDND::FirstTime = true;
XtCallbackRec DtDND::transferCBRec[] =
{
   {&DtDND::TransferCB, NULL}, {NULL, NULL}
};
XtCallbackRec DtDND::convertCBRec[] =
{
   {&DtDND::ConvertCB, NULL}, {NULL, NULL}
};
XtCallbackRec DtDND::dragFinishCBRec[] =
{
   {&DtDND::DragFinishCB, NULL}, {NULL, NULL}
};
XtCallbackRec DtDND::dropOnRootCBRec[] =
{
   {&DtDND::DropOnRootCB, NULL}, {NULL, NULL}
};
XtCallbackRec DtDND::animateCBRec[] =
{
   {&DtDND::AnimateCB, NULL}, {NULL, NULL}
};
boolean DtDND::DoingDrag = false;
GC DtDND::gc;
GC DtDND::gc_mask;

DtDND::DtDND(MotifUI *_obj, DNDCallback _dndCB, boolean _can_drop_on_root)
{
   if (FirstTime)
    {
      MotifUI *tmp = (MotifUI *)_obj;
      Pixmap tmp_pixmap;
      BaseUI *parent = _obj;
      while (parent->Parent())
         parent = parent->Parent();
      tmp_pixmap = XCreatePixmap(tmp->display, tmp->root, 1, 1, tmp->depth);
      gc = XCreateGC(tmp->display, tmp_pixmap, 0, NULL);
      tmp_pixmap = XCreatePixmap(tmp->display, tmp->root, 1, 1, 1);
      gc_mask = XCreateGC(tmp->display, tmp_pixmap, 0, NULL);
      XSetFont(tmp->display, gc, tmp->font);
      FirstTime = false;
    }

   obj = _obj;
   can_drop_on_root = _can_drop_on_root;
   dndCB = _dndCB;
   dragIcon = NULL;
   sourceIcon = NULL;

   // Set up Drop
   transferCBRec[0].closure = (XtPointer)this;
   animateCBRec[0].closure = (XtPointer)this;
   if (obj->UIClass() == ICON)
    {
      GetRects();
      XtVaSetValues(obj->BaseWidget(), XmNshadowThickness, 2, NULL);
      Arg args[6];
      XtSetArg(args[0], XmNdropSiteType, XmDROP_SITE_SIMPLE);
      XtSetArg(args[1], XmNanimationStyle, XmDRAG_UNDER_SHADOW_IN);
      XtSetArg(args[2], XmNnumDropRectangles, 2);
      XtSetArg(args[3], XmNdropRectangles, &rects);
      XtSetArg(args[4], DtNdropAnimateCallback, animateCBRec);
      XtSetArg(args[5], DtNtextIsBuffer, True);
      DtDndDropRegister(obj->BaseWidget(), DtDND_FILENAME_TRANSFER|
			DtDND_BUFFER_TRANSFER,
                        (unsigned char)XmDROP_COPY, transferCBRec, args, 6); 

      // Set up Drag on Button1
      XtAddEventHandler(obj->BaseWidget(), Button1MotionMask,  False, 
                        (XtEventHandler)DragMotionHandler, (XtPointer)this);

      // Set up Drag on Button2 if not using it for BMenu already
      if (MotifUI::bMenuButton != Button2)
       {
         XtAddEventHandler(obj->BaseWidget(), Button2MotionMask,  False, 
                        (XtEventHandler)DragMotionHandler, (XtPointer)this);
       }
    }
   else
    {
      Arg args[2];
      XtSetArg(args[0], DtNdropAnimateCallback, animateCBRec);
      XtSetArg(args[1], DtNtextIsBuffer, True);
      DtDndDropRegister(obj->BaseWidget(), DtDND_FILENAME_TRANSFER|
			DtDND_BUFFER_TRANSFER,
                        (unsigned char)XmDROP_COPY, transferCBRec, args, 2);
    }
   pixmap = 0L;
   mask = 0L;
   name = NULL;
   iconFile = NULL;
   string = NULL;
}

DtDND::~DtDND()
{
   MotifUI *icon = (MotifUI *)obj;
   if (pixmap && pixmap != XmUNSPECIFIED_PIXMAP)
      XFreePixmap(icon->display, pixmap);
   if (mask && mask != XmUNSPECIFIED_PIXMAP)
      XFreePixmap(icon->display, mask);
   free(name);
   delete iconFile;
   XmStringFree(string);
   if (sourceIcon)
    {
      XtDestroyWidget(sourceIcon);
      sourceIcon = NULL;
    }
   if (dragIcon)
    {
      XtDestroyWidget(dragIcon);
      dragIcon = NULL;
    }
}

void DtDND::GetDragPixmaps()
{
   IconObj *icon = (IconObj *)obj;
   Pixmap tmp_pixmap, tmp_mask;

   delete iconFile;
   free(name);
   XmStringFree(string);
   if (pixmap && pixmap != XmUNSPECIFIED_PIXMAP)
      XFreePixmap(icon->display, pixmap);
   if (mask && mask != XmUNSPECIFIED_PIXMAP)
      XFreePixmap(icon->display, mask);
   mask = 0L;
   pixmap = 0L;
   name = strdup(icon->Name());
   icon_size = icon->IconView();
   selected = icon->Selected();
   iconFile = new char[strlen(icon->IconFile()) + 6];

   Dimension height, width;
   unsigned int w, h, junk;
   Window junkwin;

   XtVaGetValues(icon->BaseWidget(), XmNbackground, &bg, XmNforeground, &fg,
                 GuiNselectColor, &selectColor, XmNalignment, &alignment,
                 GuiNshowSelectedPixmap, &showSelectedPixmap,
                 XmNstringDirection, &stringDirection,
                 GuiNtextSelectColor, &textSelectColor,
                 XmNlabelString, &string, XmNfontList, &fontList, NULL);

   XmStringExtent(fontList, string, &width, &height);
   if (icon_size == LARGE_ICON)
      string_border_width = 1;
   else
      string_border_width = 0;
   height += 2 * string_border_width;
   width += 2 * string_border_width;
   s_x = s_y = string_border_width;
   p_x = p_y = 0;
   s_w = width;
   s_h = height;
   p_w = p_h = 0;
   switch (icon_size)
   {
   case DETAILS:
   case SMALL_ICON:
      sprintf(iconFile, "%s.t.pm", icon->IconFile());
      icon->GetPixmaps(icon->BaseWidget(), iconFile, &tmp_pixmap, &tmp_mask);
      if (tmp_pixmap && tmp_pixmap != XmUNSPECIFIED_PIXMAP)
       {
	 XGetGeometry(icon->display, tmp_pixmap, &junkwin,
		      (int *) &junk, (int *) &junk, &w, &h, &junk, &junk);
	 p_w = w;
	 p_h = h;
	 w += 2;
	 s_x += w;
         width += w;
         if (h > height)
	  {
	    s_y = (h - height) / 2;
	    height = h;
	  }
	 else
	    p_y = (height - h) / 2;
       }
      break;
   case LARGE_ICON:
      sprintf(iconFile, "%s.m.pm", icon->IconFile());
      icon->GetPixmaps(icon->BaseWidget(), iconFile, &tmp_pixmap, &tmp_mask);
      if (tmp_pixmap && tmp_pixmap != XmUNSPECIFIED_PIXMAP)
       {
	 XGetGeometry(icon->display, tmp_pixmap, (Window *) &junk, 
		      (int *) &junk, (int *) &junk, &w, &h, &junk, &junk);
	 p_w = w;
	 p_h = h;
	 h += 2;
	 s_y += h;
         height += h;
         if (w > width)
            width = w;
       }
      break;
   default:
      sprintf(iconFile, "%s.m.pm", icon->IconFile());
      icon->GetPixmaps(icon->BaseWidget(), iconFile, &tmp_pixmap, &tmp_mask);
      break;
   }


   // Create pixmap
   pixmap = XCreatePixmap(icon->display, icon->root, width, height,
			  icon->depth);
   if (icon_size != NAME_ONLY &&
       tmp_pixmap && tmp_pixmap != XmUNSPECIFIED_PIXMAP)
    {
      mask = XCreatePixmap(icon->display, icon->root, width, height, 1);
      // Clear mask
      XSetForeground(icon->display, gc_mask, 0);
      XFillRectangle(icon->display, mask, gc_mask, 0, 0, width, height);

      // Set Mask Bits
      XSetForeground(icon->display, gc_mask, 1);
      if (icon_size == LARGE_ICON)
       {
#ifndef hpux
         if (tmp_mask && tmp_mask != XmUNSPECIFIED_PIXMAP)
	  {
            XSetClipOrigin(icon->display, gc_mask, p_x, p_y);
            XSetClipMask(icon->display, gc_mask, tmp_mask);
            XFillRectangle(icon->display, mask, gc_mask, p_x, p_y, p_w, p_h);
            XSetClipMask(icon->display, gc_mask, None);
            XSetClipOrigin(icon->display, gc_mask, 0, 0);
	  }
	 else
#endif
            XFillRectangle(icon->display, mask, gc_mask, p_x, p_y, p_w, p_h);
       }
      else
         XFillRectangle(icon->display, mask, gc_mask, p_x, p_y, p_w - 1,
			p_h - 1);

      if (icon_size == LARGE_ICON)
         XFillRectangle(icon->display, mask, gc_mask, s_x - string_border_width,
		        s_y - string_border_width,
			s_w + 2 * string_border_width,
		        s_h + 2 * string_border_width);
      else
	 XFillRectangle(icon->display, mask, gc_mask, s_x, s_y, s_w - 1,
			s_h - 1);
    }
   // If selected use selectColor as background
   if (showSelectedPixmap && selected)
      bg = selectColor;

   // Copy Pixmap to new pixmap
   XSetClipMask(icon->display, gc, None);
   XSetFillStyle(icon->display, gc, FillSolid);
   XSetForeground(icon->display, gc, bg);
   if (icon_size == LARGE_ICON)
      XFillRectangle(icon->display, pixmap, gc, p_x, p_y, p_w, p_h);
   else
      XFillRectangle(icon->display, pixmap, gc, p_x, p_y, p_w - 1, p_h - 1);
   XSetClipOrigin(icon->display, gc, p_x, p_y);
   if (tmp_mask && tmp_mask != XmUNSPECIFIED_PIXMAP)
      XSetClipMask(icon->display, gc, tmp_mask);
   else
      XSetClipMask(icon->display, gc, None);
   if (icon_size == LARGE_ICON)
      XCopyArea(icon->display, tmp_pixmap, pixmap, gc, 0, 0, p_w, p_h,
		p_x, p_y);
   else
      XCopyArea(icon->display, tmp_pixmap, pixmap, gc, 0, 0, p_w - 1, p_h - 1,
		p_x, p_y);
   XSetClipMask(icon->display, gc, None);
   XSetClipOrigin(icon->display, gc, 0, 0);
   DrawString();

   Arg args[11];
   int n = 0;
   w = width;
   h = height;
   XtSetArg(args[n], XmNwidth, w);  n++;
   XtSetArg(args[n], XmNheight, h);  n++;
   XtSetArg(args[n], XmNmaxWidth, w);  n++;
   XtSetArg(args[n], XmNmaxHeight, h);  n++;
   XtSetArg(args[n], XmNdepth, icon->depth);  n++;
   XtSetArg(args[n], XmNforeground, bg);  n++;
   XtSetArg(args[n], XmNbackground, fg);  n++;
   XtSetArg(args[n], XmNpixmap, pixmap);  n++;
   if (mask)
    {
      XtSetArg(args[n], XmNmask, mask);
      n++;
    }

   dragIcon = XmCreateDragIcon(icon->BaseWidget(), "dragIcon", args, n);

   n = 0;
   if (icon_size == LARGE_ICON)
      sprintf(iconFile, "%s.m.bm", icon->IconFile());
   else
      sprintf(iconFile, "%s.t.bm", icon->IconFile());
   icon->GetPixmaps(icon->BaseWidget(), iconFile, &tmp_pixmap);

   if (!(tmp_pixmap && tmp_pixmap != XmUNSPECIFIED_PIXMAP))
    {
      static Pixmap l_pixmap = (Pixmap)NULL, s_pixmap = (Pixmap)NULL;
      if (icon_size == LARGE_ICON)
       {
	 if (!l_pixmap)
	  {
            l_pixmap = XCreatePixmap(icon->display, icon->root, p_w, p_h, 1);
            XFillRectangle(icon->display, l_pixmap, gc_mask, 0, 0, p_w, p_h);
	  }
	 tmp_pixmap = l_pixmap;
       }
      else
       {
         if (icon_size == NAME_ONLY)
	  {
	    p_w = 16;
	    p_h = 16;
	  }
	 if (!s_pixmap)
	  {
            s_pixmap = XCreatePixmap(icon->display, icon->root, p_w, p_h, 1);
            XFillRectangle(icon->display, s_pixmap, gc_mask, 0, 0, p_w, p_h);
	  }
	 tmp_pixmap = s_pixmap;
       }
    }

   if (icon_size == NAME_ONLY)
    {
      w = 16;
      h = 16;
    }
   else
    {
      w = p_w;
      h = p_h;
    }
   XtSetArg(args[n], XmNwidth, w);  n++;
   XtSetArg(args[n], XmNheight, h);  n++;
   XtSetArg(args[n], XmNmaxWidth, w);  n++;
   XtSetArg(args[n], XmNmaxHeight, h);  n++;
   XtSetArg(args[n], XmNdepth, 1);  n++;
   XtSetArg(args[n], XmNforeground, icon->black);  n++;
   XtSetArg(args[n], XmNbackground, icon->white);  n++;
   XtSetArg(args[n], XmNpixmap, tmp_pixmap);  n++;
   if (tmp_mask && tmp_mask != XmUNSPECIFIED_PIXMAP)
    {
      XtSetArg(args[n], XmNmask, tmp_mask);
      n++;
    }

   sourceIcon = XmCreateDragIcon(icon->BaseWidget(), "sourceIcon", args, n);
   strcpy(iconFile, icon->IconFile());
}

void DtDND::DrawString()
{
   IconObj *icon = (IconObj *)obj;

   // Draw String
   if (selected)
      XSetForeground(icon->display, gc, selectColor);
   else
      XSetForeground(icon->display, gc, bg);
   if (icon_size == LARGE_ICON)
      XFillRectangle(icon->display, pixmap, gc, s_x - string_border_width,
		     s_y - string_border_width, s_w + 2 * string_border_width,
		     s_h + 2 * string_border_width);
   else
      XFillRectangle(icon->display, pixmap, gc, s_x, s_y, s_w - 1, s_h - 1);
   // If selected use selectColor as background
   if (selected)
      XSetForeground(icon->display, gc, textSelectColor);
   else
      XSetForeground(icon->display, gc, fg);
   XmStringDraw(icon->display, pixmap, fontList, string, gc, s_x, s_y, s_w,
		alignment, stringDirection, NULL);
}

void DtDND::GetRects()
{
   Dimension shadow, highlight, margin;

   XtVaGetValues(obj->BaseWidget(), XmNhighlightThickness, &highlight, 
                 GuiNiconMarginThickness, &margin, 
                 GuiNiconShadowThickness, &shadow, NULL);

   margin += (shadow + highlight);
   GuiIconGetRects(obj->BaseWidget(), &rects[0], &rects[1]);
   rects[0].x -= margin;
   rects[0].y -= margin;
   rects[0].width += (2 * margin);
   rects[0].height += (2 * margin);
   rects[1].x -= margin;
   rects[1].y -= margin;
   rects[1].width += (2 * margin);
   rects[1].height += (2 * margin);
}

void DtDND::UpdateActivity(boolean flag)
{
   Arg args[3];
   int n = 0;

   if (flag)
    {
      XtSetArg(args[n], XmNdropSiteOperations, XmDROP_COPY | XmDROP_MOVE); n++;
      XtSetArg(args[n], XmNdropSiteActivity, XmDROP_SITE_ACTIVE); n++;
      if (obj->UIClass() == ICON)
         XtSetArg(args[n], XmNanimationStyle, XmDRAG_UNDER_SHADOW_IN);
      else
         XtSetArg(args[n], XmNanimationStyle, XmDRAG_UNDER_HIGHLIGHT);
      n++;
    }
   else
    {
      XtSetArg(args[n], XmNdropSiteOperations, XmDROP_NOOP); n++;
      XtSetArg(args[n], XmNdropSiteActivity, XmDROP_SITE_INACTIVE); n++;
      XtSetArg(args[n], XmNanimationStyle, XmDRAG_UNDER_NONE); n++;
    }
   XmDropSiteUpdate(obj->BaseWidget(), args, n);
   if (flag)
      XmDropSiteConfigureStackingOrder(obj->BaseWidget(), NULL, XmABOVE);
   else
      XmDropSiteConfigureStackingOrder(obj->BaseWidget(), NULL, XmBELOW);
}

void DtDND::UpdateRects()
{
   Arg args[2];

   GetRects();
   XtSetArg(args[0], XmNnumDropRectangles, 2);
   XtSetArg(args[1], XmNdropRectangles, &rects);
   XmDropSiteUpdate(obj->BaseWidget(), args, 2);
}

void DtDND::AnimateCB(Widget /*widget*/, XtPointer client_data,
		      XtPointer call_data)
{
   DtDND *obj = (DtDND *)client_data;
   if (obj->dndCB)
    {
      DtDndDropAnimateCallbackStruct *animateInfo;
      animateInfo = (DtDndDropAnimateCallbackStruct *) call_data;
      int i = 0, numItems;
      numItems = animateInfo->dropData->numItems;
      //for (i = 0; i < numItems; i++)
       {
	 (*obj->dndCB)(obj->obj, NULL, NULL, ANIMATE);
       }
    }
}

void DtDND::TransferCB(Widget /*widget*/, XtPointer client_data,
		       XtPointer call_data)
{
   DtDndTransferCallbackStruct *transferInfo;
   transferInfo = (DtDndTransferCallbackStruct *) call_data;
   char *value;
   int len, i, numItems;

   DtDND *obj = (DtDND *)client_data;
   numItems = transferInfo->dropData->numItems;
   switch (transferInfo->dropData->protocol)
   {
   case DtDND_FILENAME_TRANSFER:
      if (obj->dndCB)
       {
         for (i = 0; i < numItems; i++)
          {
	    value = transferInfo->dropData->data.files[i];
	    len = strlen(value);
	    (*obj->dndCB)(obj->obj, &value, &len, FILENAME_TRANSFER);
          }
       }
      break;
   case DtDND_TEXT_TRANSFER:
      if (obj->dndCB)
       {
         for (i = 0; i < numItems; i++)
          {
  	    value = (char *) _XmStringUngenerate(
				transferInfo->dropData->data.strings[i], NULL,
				XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
	    len = strlen(value);
	    (*obj->dndCB)(obj->obj, &value, &len, TEXT_TRANSFER);
	    if (NULL != value)
	      XtFree(value);
          }
       }
      break;
   case DtDND_BUFFER_TRANSFER:
      if (obj->dndCB)
       {
	 (*obj->dndCB)(obj->obj, &value, &len, BUFFER_TRANSFER);
	 DtActionArg *aap = new DtActionArg[numItems];
	 memset(aap, 0, numItems * sizeof(DtActionArg));
         for (i = 0; i < numItems; i++)
          {
	    aap[i].argClass = DtACTION_BUFFER;
	    aap[i].u.buffer.bp = transferInfo->dropData->data.buffers[i].bp;
	    aap[i].u.buffer.size = transferInfo->dropData->data.buffers[i].size;
	    aap[i].u.buffer.name = transferInfo->dropData->data.buffers[i].name;
          }
	 MotifUI *tmp = (MotifUI *)obj->obj;
	 DtActionInvoke(tmp->topLevel, value, aap, numItems, NULL, NULL, NULL,
			1, NULL, NULL);
	 delete aap;
	 delete value;
       }
      break;
   default:
      transferInfo->status = DtDND_FAILURE;
      break;
   }
}

void DtDND::DragMotionHandler(Widget /*widget*/, XtPointer client_data,
	                      XEvent *event, Boolean * /*continued*/)
{
   static int initialX = -1;
   static int initialY = -1;
   int diffX, diffY;
   DtDND *obj = (DtDND *)client_data;

   if (obj->DoingDrag)
      return;

   // If the drag is just starting, set initial button down coords
   if (initialX == -1 && initialY == -1)
    {
      initialX = event->xmotion.x;
      initialY = event->xmotion.y;
    }
   // Find out how far pointer has moved since button press
   diffX = initialX - event->xmotion.x;
   diffY = initialY - event->xmotion.y;

   if ((ABS(diffX) >= DRAG_THRESHOLD) ||
       (ABS(diffY) >= DRAG_THRESHOLD))
    {
      obj->DoingDrag = true;
      obj->StartDrag(event);
      initialX = -1;
      initialY = -1;
   }
}

void DtDND::DragFinishCB(Widget /*widget*/, XtPointer client_data,
                         XtPointer /*callData*/)
{
   DtDND *obj = (DtDND *) client_data;
   obj->DoingDrag = false;
}

void DtDND::StartDrag(XEvent *event)
{
   IconObj *icon = (IconObj *)obj;

   convertCBRec[0].closure = (XtPointer)this;
   dragFinishCBRec[0].closure = (XtPointer)this;
   dropOnRootCBRec[0].closure = (XtPointer)this;
   if ((!STRCMP(name, icon->Name()) || !STRCMP(iconFile, icon->IconFile()) ||
       icon_size != icon->IconView()) && dragIcon)
    {
      XtDestroyWidget(dragIcon);
      XtDestroyWidget(sourceIcon);
      dragIcon = NULL;
      sourceIcon = NULL;
    }
   if (!dragIcon)
      GetDragPixmaps();
   else if (selected != icon->Selected())
    {
      selected = icon->Selected();
      DrawString();
    }
   Arg arg[3];
   XtSetArg(arg[0], DtNsourceIcon, (XtArgVal)dragIcon);
   XtSetArg(arg[1], XmNsourceCursorIcon, (XtArgVal)sourceIcon);
   XtSetArg(arg[2], DtNdropOnRootCallback, dropOnRootCBRec);

   Widget dc = DtDndDragStart(obj->BaseWidget(), event, DtDND_FILENAME_TRANSFER,
			      1, (unsigned char)(XmDROP_COPY | XmDROP_MOVE),
                              convertCBRec, dragFinishCBRec, arg,
		              can_drop_on_root ? 3 : 2);
   if (dc)
    {
      XmDragContext xmDragContext = (XmDragContext) dc;
      XtVaSetValues((Widget)xmDragContext->drag.curDragOver,
		    XmNdragOverMode, XmPIXMAP, NULL);
    }
   else
      fprintf(stderr, "DtDndDragStart returned NULL.\n");
}

void DtDND::ConvertCB(Widget /*dragContext*/, XtPointer client_data,
                      XtPointer call_data)
{
   char *value;
   DtDndConvertCallbackStruct *convertInfo;
   int len, i, numFiles;
   DtDND *obj = (DtDND *) client_data;

   convertInfo = (DtDndConvertCallbackStruct *) call_data;
   numFiles = convertInfo->dragData->numItems;
   switch (convertInfo->reason)
   {
   case DtCR_DND_CONVERT_DATA:
      if (obj->dndCB)
       {
         for (i = 0; i < numFiles; i++)
          {
	    value = NULL;
	    (*obj->dndCB)(obj->obj, &value, &len, CONVERT_DATA);
	    if (value && *value)
               convertInfo->dragData->data.files[i] = value;
	    else
	     {
	       convertInfo->status = DtDND_FAILURE;
	       return;
	     }
	  }
       }
      break;
   case DtCR_DND_CONVERT_DELETE:
      if (obj->dndCB)
       {
         for (i = 0; i < numFiles; i++)
          {
            value = convertInfo->dragData->data.files[i];
	    len = strlen(value);
	    (*obj->dndCB)(obj->obj, &value, &len, CONVERT_DELETE);
	  }
       }
      break;
   }
}

void DtDND::DropOnRootCB(Widget /*dragContext*/, XtPointer client_data,
                         XtPointer call_data)
{
   char *value;
   int len, x, y, i, numFiles;
   DtDND *obj = (DtDND *) client_data;
   DtDndDropCallbackStruct *fileList = (DtDndDropCallbackStruct *)call_data;
   MotifUI *tmp = (MotifUI *)obj->obj;
   Atom     pCurrent;
   char *workspace_name = NULL;

   if (DtWsmGetCurrentWorkspace(tmp->display, tmp->root, &pCurrent) ==
       Success)
    {
      workspace_name = XGetAtomName(tmp->display, pCurrent);
    }

   numFiles = fileList->dropData->numItems;
   fileList->completeMove = False;
   x = fileList->x;
   y = fileList->y;
   int max_len = 0;
   for(i = 0; i < numFiles; i++)
      if ((len = strlen(fileList->dropData->data.files[i])) > max_len)
         max_len = len;
   if (workspace_name)
      value = new char[max_len + strlen(workspace_name) + 30];
   else
      value = new char[max_len + 30];
   for(i = 0; i < numFiles; i++)
    {
      if (workspace_name)
         sprintf(value, "%d\n%d\n%s \n%s", x, y,
		 fileList->dropData->data.files[i], workspace_name);
      else
         sprintf(value, "%d\n%d\n%s", x, y, fileList->dropData->data.files[i]);
      len = strlen(value);
      (*obj->dndCB)(obj->obj, &value, &len, DROP_ON_ROOT);
      y += 20;
      x += 20;
    }
   delete value;
}
