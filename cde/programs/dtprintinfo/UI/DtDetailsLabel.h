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
