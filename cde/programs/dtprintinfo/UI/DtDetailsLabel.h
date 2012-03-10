/* $XConsortium: DtDetailsLabel.h /main/3 1995/11/06 09:34:43 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef DTDETAILSLABEL_H
#define DTDETAILSLABEL_H

#include "MotifUI.h"

class DtDetailsLabel : public MotifUI {

   static void ClickCB(Widget, XtPointer, XtPointer);

   static boolean first_time;
   static XmString bottom_label[];
   static Pixmap blank_pixmap;
   static Pixmap blank_mask;

 public:

   DtDetailsLabel(MotifUI *parent);
   void Update(boolean show_only_my_jobs);

   const UI_Class UIClass()           { return LABEL; }
   const char *const UIClassName()    { return "DtDetailsLabel"; }
};

#endif // DTDETAILSLABEL_H
