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
/* $XConsortium: LabelObj.C /main/2 1995/07/17 14:06:12 drk $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "LabelObj.h"

#include <Xm/Label.h>
#include <Xm/Frame.h>
#include "WorkArea.h"

LabelObj::LabelObj(MotifUI *parent,
                   char *name,
		   LabelType style,
		   boolean has_border)
	: MotifUI(parent, name, NULL)
{
   XmString xm_string = StringCreate(name);
   Widget p, super_node;
   Pixel bg;
   int alignment;

   switch (_style = style)
   {
   case LEFT_JUSTIFIED: alignment = XmALIGNMENT_BEGINNING; break;
   case CENTERED: alignment = XmALIGNMENT_CENTER; break;
   case RIGHT_JUSTIFIED: alignment = XmALIGNMENT_END; break;
   }

   p = parent->InnerWidget();
   if (!XtIsComposite(p))
      p = XtParent(p);
   XtVaGetValues(p, XmNbackground, &bg, NULL);
   // If p is a icon then set superNode to it, otherwise set superNode to NULL
   super_node = parent->UIClass() == ICON ? parent->BaseWidget() : NULL;
   if (has_border)
    {
      _w = XtVaCreateManagedWidget(name, xmFrameWidgetClass, p,
				   XmNbackground, bg, XmNuserData, this,
				   XmNshadowType, XmSHADOW_OUT,
				   XmNshadowThickness, shadowThickness, NULL);
      p = _w;
    }
   _label = XtVaCreateManagedWidget(name, xmLabelWidgetClass, p,
                                    XmNlabelString, xm_string,
				    GuiNsuperNode, super_node,
				    XmNalignment, alignment,
				    XmNbackground, bg, XmNuserData, this, NULL);
   if (!has_border)
      _w = _label;

   StringFree(xm_string);
}

void LabelObj::LabelStyle(LabelType style)
{
   int alignment;

   switch (_style = style)
   {
   case LEFT_JUSTIFIED: alignment = XmALIGNMENT_BEGINNING; break;
   case CENTERED: alignment = XmALIGNMENT_CENTER; break;
   case RIGHT_JUSTIFIED: alignment = XmALIGNMENT_END; break;
   }
   XtVaSetValues(_label, XmNalignment, alignment, NULL);
}
