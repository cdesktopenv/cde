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
/* $XConsortium: DtSetPref.C /main/2 1995/07/17 14:04:26 drk $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "DtSetPref.h"
#include "DtMainW.h"
#include "DtFindD.h"
#include "IconObj.h"
#include "Group.h"
#include "LabelObj.h"
#include "Container.h"
#include "Application.h"

#include "dtprintinfomsg.h"

DtSetPref::DtSetPref(AnyUI *parent,
		     char *name,
                     AnyUI *container,
                     PreferenceCallback callback,
		     void *callback_data)
	: Dialog(parent, name, MODELESS, false)
{
   mainw = (DtMainW *) parent;
   _has_been_posted = false;
   _container = container;
   _callback = callback;
   _callback_data = callback_data;

   // initialize previous values
   _previous_style = container->IconView();
   _previous_status_flag = true;
   _previous_status_line = true;
   _previous_show_only_mine = false;
   _previous_show_details_label = true;
   _previous_update_interval = 30;

   view_group = new Group(this, MESSAGE(RepresentationL));
   by_icon = new Button(view_group, MESSAGE(LargeIconL), TOGGLE_BUTTON,
                        RepCB, this);
   by_smallIcon = new Button(view_group, MESSAGE(SmallIconL), TOGGLE_BUTTON,
                             RepCB, this);
   by_name = new Button(view_group, MESSAGE(NameOnlyL), TOGGLE_BUTTON,
                        RepCB, this);
   by_properties = new Button(view_group, MESSAGE(DetailsL), TOGGLE_BUTTON,
                              RepCB, this);
   Container *form = new Container(view_group, "form", FORM);
   show_labels = new Button(form, MESSAGE(ShowLabelsL), TOGGLE_BUTTON);
   show_labels->Selected(true);
   show_labels->Active(false);
   show_labels->AttachTop();
   show_labels->AttachBottom();
   show_labels->AttachRight();
   show_labels->AttachLeft(20);

   show_jobs = new Group(this, MESSAGE(JobsToShowL));
   only_mine = new Button(show_jobs, MESSAGE(OnlyMineL), TOGGLE_BUTTON);
   everyones = new Button(show_jobs, MESSAGE(EveryoneL), TOGGLE_BUTTON);
   everyones->Selected(true);

   update_group = new Group(this, MESSAGE(UpdateIntervalL), FORM_BOX);
   update_interval = new ScaleObj(update_group, MESSAGE(IntervalL),
				  30, 0, 5, 300, 5, HORIZONTAL_SCALE, true);
   update_interval->AttachAll();
   update_group->AttachRight();
   update_group->AttachLeft();
   update_group->AttachBottom();

   status = new Group(this, MESSAGE(StatusL), CHECK_BOX);
   status_flag = new Button(status, MESSAGE(ProblemFlagL), TOGGLE_BUTTON);
   status_flag->Selected(true);
   status_line = new Button(status, MESSAGE(InformationLineL), TOGGLE_BUTTON);
   status_line->Selected(true);

   status->AttachRight();
   status->AttachLeft(view_group, 5);
   status->AttachBottom(update_group, 5);

   view_group->AttachBottom(update_group, 5);
   show_jobs->AttachBottom(status, 5);

   view_group->AttachLeft();
   view_group->AttachTop();
   show_jobs->AttachLeft(view_group, 5);
   show_jobs->AttachRight();
   show_jobs->AttachTop();

   ok = new Button(this, MESSAGE(OKL), PUSH_BUTTON, OkCB, this);
   apply = new Button(this, MESSAGE(ApplyL), PUSH_BUTTON, ApplyCB, this);
   cancel = new Button(this, MESSAGE(CancelL), PUSH_BUTTON, CancelCB, this);
   help = new Button(this, MESSAGE(HelpL), PUSH_BUTTON, HelpCB, this);

   DefaultButton(ok);

   Reset();
}

DtSetPref::~DtSetPref()
{
   // Empty
}

boolean DtSetPref::SetVisiblity(boolean flag)
{
   Dialog::SetVisiblity(flag);
   if (_has_been_posted == false)
    {
      _has_been_posted = true;
      CancelButton(cancel);
    }
   return true;
}

void DtSetPref::ShowOnlyMyJobs(boolean value)
{
   only_mine->Selected(value);
   everyones->Selected(value ? false : true);
}

void DtSetPref::Representation(IconStyle value)
{
   by_name->Selected(false);
   by_icon->Selected(false);
   by_smallIcon->Selected(false);
   by_properties->Selected(false);
   switch (value)
    {
     case NAME_ONLY: by_name->Selected(true); break;
     case LARGE_ICON: by_icon->Selected(true); break;
     case SMALL_ICON: by_smallIcon->Selected(true); break;
     case DETAILS: by_properties->Selected(true); break;
    }
   show_labels->Active(by_properties->Selected());
}

void DtSetPref::Reset()
{
   Representation(_previous_style);
   status_flag->Selected(_previous_status_flag);
   status_line->Selected(_previous_status_line);
   (void)update_interval->Value();
   update_interval->Value(_previous_update_interval);
   ShowOnlyMyJobs(_previous_show_only_mine);
   show_labels->Selected(_previous_show_details_label);
}

static void TurnOffHourGlass(BaseUI *obj, void *data)
{
   ((DtMainW *)obj)->WorkingCursor(false);
   ((Container *)data)->EndUpdate();
}

void DtSetPref::Apply()
{
   IconStyle style;
   boolean add_timeout = false;

   if (by_name->Selected())
      style = NAME_ONLY;
   else if (by_icon->Selected())
      style = LARGE_ICON;
   else if (by_smallIcon->Selected())
      style = SMALL_ICON;
   else if (by_properties->Selected())
      style = DETAILS;
   if (_previous_style != style)
    {
      if (mainw->PrinterAppMode() == PRINT_MANAGER)
       {
         mainw->WorkingCursor(true);
         _container->BeginUpdate();
         add_timeout = true;
       }
      _container->IconView(style);
      if (_callback)
       {
	 if (_previous_style == DETAILS)
            (*_callback)(_callback_data, DETAILS_OFF, NULL);
	 else if (style == DETAILS)
            (*_callback)(_callback_data, DETAILS_ON, NULL);
       }
      _previous_style = style;
      if (_callback)
       {
	 if (_previous_show_details_label)
            (*_callback)(_callback_data, SHOW_DETAILS_LABEL_ON, NULL);
	 else
            (*_callback)(_callback_data, SHOW_DETAILS_LABEL_OFF, NULL);
       }
    }

   if (_previous_status_flag != status_flag->Selected())
    {
      _previous_status_flag = status_flag->Selected();
      if (_callback)
       {
	 if (_previous_status_flag)
            (*_callback)(_callback_data, STATUS_FLAG_ON, NULL);
	 else
            (*_callback)(_callback_data, STATUS_FLAG_OFF, NULL);
       }
    }
   if (_previous_status_line != status_line->Selected())
    {
      _previous_status_line = status_line->Selected();
      if (_callback)
       {
	 if (_previous_status_line)
            (*_callback)(_callback_data, STATUS_LINE_ON, NULL);
	 else
            (*_callback)(_callback_data, STATUS_LINE_OFF, NULL);
       }
    }
   if (_previous_show_only_mine != only_mine->Selected())
    {
      if (mainw->PrinterAppMode() == PRINT_MANAGER)
       {
         mainw->WorkingCursor(true);
         _container->BeginUpdate();
         add_timeout = true;
       }
      _previous_show_only_mine = only_mine->Selected();
      if (_callback)
       {
	 if (_previous_show_only_mine)
            (*_callback)(_callback_data, SHOW_ONLY_MINE_ON, NULL);
	 else
            (*_callback)(_callback_data, SHOW_ONLY_MINE_OFF, NULL);
	 if (_previous_show_details_label)
            (*_callback)(_callback_data, SHOW_DETAILS_LABEL_ON, NULL);
	 else
            (*_callback)(_callback_data, SHOW_DETAILS_LABEL_OFF, NULL);
       }
    }
   if (_previous_show_details_label != show_labels->Selected())
    {
      _previous_show_details_label = show_labels->Selected();
      if (_callback)
       {
	 if (_previous_show_details_label)
            (*_callback)(_callback_data, SHOW_DETAILS_LABEL_ON, NULL);
	 else
            (*_callback)(_callback_data, SHOW_DETAILS_LABEL_OFF, NULL);
       }
    }
   if (update_interval->Value() != _previous_update_interval)
    {
      _previous_update_interval = update_interval->Value();
      if (_callback)
         (*_callback)(_callback_data, UPDATE_INTERVAL_CHANGED,
		     (char *) _previous_update_interval);
    }
   if (mainw->findD)
      mainw->findD->UpdateMatchAnyUser();
   if (add_timeout)
      mainw->AddTimeOut(TurnOffHourGlass, _container, 1500);
}

void DtSetPref::OkCB(void *data)
{
   DtSetPref *obj = (DtSetPref *) data;
   
   obj->Apply();
   obj->Visible(false);
}

void DtSetPref::Cancel()
{
   Reset();
   Visible(false);
}

void DtSetPref::CancelCB(void *data)
{
   DtSetPref *obj = (DtSetPref *) data;
   
   obj->Cancel();
}

void DtSetPref::CloseCB()
{
   Cancel();
}

void DtSetPref::ApplyCB(void *data)
{
   DtSetPref *obj = (DtSetPref *) data;

   obj->Apply();
}

void DtSetPref::HelpCB(void *data)
{
   DtSetPref *obj = (DtSetPref *) data;
   obj->HandleHelpRequest();
}

void DtSetPref::RepCB(void *data)
{
   DtSetPref *obj = (DtSetPref *) data;
   obj->show_labels->Active(obj->by_properties->Selected());
}

boolean DtSetPref::HandleHelpRequest()
{
   mainw->DisplayHelp("SetOptionsDE");
   return true;
}

