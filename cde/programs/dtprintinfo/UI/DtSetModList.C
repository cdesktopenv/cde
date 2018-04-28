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
/* $XConsortium: DtSetModList.C /main/2 1995/07/17 14:04:17 drk $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "DtSetModList.h"
#include "DtMainW.h"
#include "DtWorkArea.h"
#include "Button.h"
#include "IconObj.h"
#include "LabelObj.h"
#include "Container.h"


#include "dtprintinfomsg.h"

DtSetModList::DtSetModList(AnyUI *parent,
		           char *name,
                           BaseUI *workarea,
			   ButtonCallback filterCB)
	: Dialog(parent, name)
{
   mainw = (DtMainW *) parent;
   _has_been_posted = false;
   _info = new LabelObj(this, MESSAGE(SelectPrintersToBeShowL));
   _container = new Container(this, "FilterList", SCROLLED_VERTICAL_ROW_COLUMN,
			      MULTIPLE_SELECT);
   _container->IconView(SMALL_ICON);
   _filterCB = filterCB;
   _workarea = (Container *) workarea;
   Container *form = new Container(this, "Form", FORM);

   _info->AttachRight();
   _info->AttachLeft();
   _info->AttachTop();
   form->AttachBottom();
   form->AttachRight();
   form->AttachLeft();
   _container->AttachBottom(form, 5);
   _container->AttachRight();
   _container->AttachLeft();
   _container->AttachTop(_info);
   select_all = new Button(form, MESSAGE(SelectAllL), PUSH_BUTTON, SelectAllCB,
			   _container);
   unselect_all = new Button(form, MESSAGE(DeselectAllL), PUSH_BUTTON,
			     UnSelectAllCB, _container);

   int i;
   IconObj **objs = (IconObj **) workarea->Children();
   for (i = 0; i < workarea->NumChildren(); i++)
    {
      if (objs[i]->UIClass() == ICON)
       {
         IconObj *icon = new IconObj(_container, (char *)objs[i]->Name(),
				     objs[i]->IconFile());
	 icon->ApplicationData = objs[i];
       }
    }

   ok = new Button(this, MESSAGE(OKL), PUSH_BUTTON, OkCB, this);
   apply = new Button(this, MESSAGE(ApplyL), PUSH_BUTTON, ApplyCB, this);
   cancel = new Button(this, MESSAGE(CancelL), PUSH_BUTTON, CancelCB, this);
   help = new Button(this, MESSAGE(HelpL), PUSH_BUTTON, HelpCB, this);
   DefaultButton(ok);
   CancelButton(cancel);
   Reset();
}

DtSetModList::~DtSetModList()
{
   // Empty
}

boolean DtSetModList::SetVisiblity(boolean flag)
{
   if (_has_been_posted == false)
    {
      int height;
      if (_container->NumChildren())
       {
         BaseUI **kids = _container->Children();
         height = (kids[0]->Height() + 4) * _container->NumChildren();
         if (height > 300)
            height = 300;
       }
      else
	 height = 200;
      int width = StringWidth(Name()) + 30;
      if (Width() < width)
       {
	 width -= 10;
	 _container->WidthHeight(width, height);
       }
      else
         _container->Height(height);
      _has_been_posted = true;

      int offset = (width - (select_all->Width() + unselect_all->Width())) / 3;
      select_all->AttachLeft(offset);
      unselect_all->AttachRight(offset);
      unselect_all->AttachLeft(NULL, 0);
    }
   Reset();
   Dialog::SetVisiblity(flag);
   return true;
}

void DtSetModList::Reset()
{
   int i;
   IconObj **children = (IconObj **) _container->Children();
   mainw->container->BeginUpdate();
   for (i = 0; i < _container->NumChildren(); i++)
    {
      IconObj *icon = (IconObj *)children[i]->ApplicationData;
      children[i]->Selected(icon->Visible());
      if (strcmp(icon->Name(), children[i]->Name()))
	 children[i]->Name((char *)icon->Name());
      if (strcmp(icon->IconFile(), children[i]->IconFile()))
	 children[i]->IconFile(icon->IconFile());
    }
   mainw->container->EndUpdate();
}

void DtSetModList::Apply()
{
   if (_filterCB)
      (*_filterCB)((void *)_container);
   
}

void DtSetModList::OkCB(void *data)
{
   DtSetModList *obj = (DtSetModList *) data;
   
   obj->Apply();
   obj->Visible(false);
}

void DtSetModList::Cancel()
{
   Reset();
   Visible(false);
}

void DtSetModList::CloseCB()
{
   Cancel();
}

void DtSetModList::CancelCB(void *data)
{
   DtSetModList *obj = (DtSetModList *) data;
   
   obj->Cancel();
}

void DtSetModList::ApplyCB(void *data)
{
   DtSetModList *obj = (DtSetModList *) data;

   obj->Apply();
}

void DtSetModList::SelectAllCB(void *data)
{
   Container *obj = (Container *) data;
   obj->SelectAll();
}

void DtSetModList::UnSelectAllCB(void *data)
{
   Container *obj = (Container *) data;
   obj->SelectAll(false);
}

void DtSetModList::HelpCB(void *data)
{
   DtSetModList *obj = (DtSetModList *) data;
   obj->HandleHelpRequest();
}

boolean DtSetModList::HandleHelpRequest()
{
   mainw->DisplayHelp("ShowPrintersDE");
   return true;
}
