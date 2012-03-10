/* $XConsortium: Sep.C /main/2 1995/07/17 14:07:18 drk $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "Sep.h"

#include <Xm/Separator.h>

Sep::Sep(MotifUI *parent,
	 SeparatorStyle style)
	: MotifUI(parent, "sep", NULL)
{
   _style = style;

   _w = XtVaCreateManagedWidget("sep", xmSeparatorWidgetClass,
				parent->InnerWidget(), 
                                XmNseparatorType, style, NULL);
}

Sep::Sep(char *category,
	 MotifUI *parent,
	 SeparatorStyle style)
	: MotifUI(parent, "sep", category)
{
   _style = style;

   _w = XtVaCreateManagedWidget("sep", xmSeparatorWidgetClass,
				parent->InnerWidget(), 
                                XmNseparatorType, style, NULL);
}
