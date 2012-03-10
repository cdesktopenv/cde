/* $XConsortium: DtWorkArea.h /main/3 1995/11/06 09:37:19 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef DTWORKAREA_H
#define DTWORKAREA_H

#include "Container.h"

class DtMainW;

class DtWorkArea : public Container
{

 private:

   DtMainW * _mainW;
   void NotifySelected(BaseUI *obj);
   void NotifyOpen(BaseUI *obj);

 public:

   DtWorkArea(char *category, AnyUI *parent, char *name,
	      ContainerType = SCROLLED_WORK_AREA, 
	      SelectionType = SINGLE_SELECT);

};

#endif // DTWORKAREA_H
