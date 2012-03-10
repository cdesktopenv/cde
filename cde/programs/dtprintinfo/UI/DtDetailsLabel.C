/* $TOG: DtDetailsLabel.C /main/3 1998/07/24 16:11:46 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "DtDetailsLabel.h"
#include "Icon.h"
#include "WorkArea.h"

#include "dtprintinfomsg.h"

boolean DtDetailsLabel::first_time = true;
Pixmap DtDetailsLabel::blank_pixmap;
Pixmap DtDetailsLabel::blank_mask;
XmString DtDetailsLabel::bottom_label[2];

DtDetailsLabel::DtDetailsLabel(MotifUI *parent)
	: MotifUI(parent, "Details", NULL)
{
   if (first_time)
    {
      char *label = new char[strlen(MESSAGE(OwnerL)) + strlen(MESSAGE(SizeL)) +
			     strlen(MESSAGE(TimeL)) + strlen(MESSAGE(DateL)) +
			     strlen(MESSAGE(JobNumberL)) + 5];
      sprintf(label, "%s\n%s\n%s\n%s", MESSAGE(SizeL), 
              MESSAGE(JobNumberL), MESSAGE(TimeL), MESSAGE(DateL));
      bottom_label[0] = StringCreate(label);
      sprintf(label, "%s\n%s\n%s\n%s\n%s",
              MESSAGE(OwnerL), MESSAGE(SizeL), 
              MESSAGE(JobNumberL), MESSAGE(TimeL), MESSAGE(DateL));
      bottom_label[1] = StringCreate(label);

      Dimension highlight;
      XtVaGetValues(parent->BaseWidget(),
		    XmNhighlightThickness, &highlight, NULL);
      int height = 21 + 2 * highlight;
      blank_pixmap = XCreatePixmap(display, root, 1, height, depth);
      blank_mask = XCreatePixmap(display, root, 1, height, 1);
      GC gc_mask = XCreateGC(display, blank_mask, 0, NULL);
      XSetForeground(display, gc_mask, 0);
      XFillRectangle(display, blank_mask, gc_mask, 0, 0, 1, height);

      XFreeGC(display, gc_mask);
      delete [] label;
      first_time = false;
    }

   XmString top_string = StringCreate(MESSAGE(Position1L));
   XmString label_string = StringCreate(MESSAGE(JobNameL));
   Widget p = XtParent(parent->BaseWidget());
   Pixel bg;
   XtVaGetValues(p, XmNbackground, &bg, NULL);
   _w = XtVaCreateManagedWidget("DtDetailsLabel", iconWidgetClass, p,
				GuiNsuperNode, parent->BaseWidget(),
				XmNalignment, XmALIGNMENT_END,
				XmNtraversalOn, False,
				XmNbackground, bg,
				GuiNiconMarginThickness, 0,
				XmNhighlightThickness, 0,
				XmNlabelString, label_string,
				GuiNtopLabelString, top_string,
				XmNlabelPixmap, blank_pixmap,
				GuiNiconMask, blank_mask,
				GuiNpixmapPlacement, GuiPIXMAP_LEFT,
				NULL);
   StringFree(top_string);
   StringFree(label_string);
   XtAddCallback(_w, GuiNsingleClickCallback, &DtDetailsLabel::ClickCB, NULL);
   XtAddCallback(_w, GuiNdoubleClickCallback, &DtDetailsLabel::ClickCB, NULL);
}

void DtDetailsLabel::Update(boolean show_only_my_jobs)
{
   int i = 1;
   if (show_only_my_jobs)
      i = 0;
   XtVaSetValues(_w, GuiNbottomLabelString, bottom_label[i], NULL);
}

void DtDetailsLabel::ClickCB(Widget w, XtPointer, XtPointer)
{
   XtVaSetValues(w, GuiNselected, False, NULL);
}
