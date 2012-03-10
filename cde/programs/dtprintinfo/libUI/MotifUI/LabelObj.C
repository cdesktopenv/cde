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
