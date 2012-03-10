/* $XConsortium: DtWorkArea.C /main/2 1995/07/17 14:04:34 drk $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "DtWorkArea.h"
#include "DtMainW.h"
#include "DtActions.h"

DtWorkArea::DtWorkArea(char *category,
		       AnyUI *parent,
	               char *name,
		       ContainerType container_type,
		       SelectionType select_type)
	: Container(category, parent, name, container_type, select_type)
{
   _mainW = (DtMainW *) parent->Parent();
}

void DtWorkArea::NotifySelected(BaseUI *obj)
{
   Container::NotifySelected(obj);

   if (_mainW->UIClass() != MAIN_WINDOW)
      return;

   BaseUI **selection;
   int n_items;
   Selection(&n_items, &selection);
   if (n_items)
      _mainW->actionsMenu->UpdateActions(n_items, selection[0]);
   else
      _mainW->actionsMenu->UpdateActions(n_items, NULL);
   delete []selection;
}

void DtWorkArea::NotifyOpen(BaseUI *obj) 
{
   if (_mainW->UIClass() != MAIN_WINDOW)
      return;

   _mainW->OpenClose(obj);
}
