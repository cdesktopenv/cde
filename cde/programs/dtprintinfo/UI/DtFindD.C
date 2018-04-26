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
/* $TOG: DtFindD.C /main/5 1998/07/24 16:12:13 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "DtFindD.h"
#include "DtMainW.h"
#include "DtApp.h"
#include "DtSetModList.h"
#include "DtWorkArea.h"
#include "Button.h"
#include "Prompt.h"
#include "LabelObj.h"
#include "Container.h"
#include "Sep.h"
#include "DtPrtJobIcon.h"

#include "dtprintinfomsg.h"

#include <unistd.h> // This is for the getuid function

DtJobList::DtJobList(AnyUI *parent)
	: Container(parent, "found_container", SCROLLED_VERTICAL_ROW_COLUMN,
		    MULTIPLE_SELECT)
{
   IconView(SMALL_ICON);
}

class _JobIcon : public IconObj
{
 public:
   DtPrtJobIcon *job_icon;
   BaseUI *printer;
   BaseUI *job_list;

   _JobIcon(AnyUI *parent, DtPrtJobIcon *obj, IconFields);
};

_JobIcon::_JobIcon(AnyUI *parent, DtPrtJobIcon *obj, IconFields fields)
	: IconObj(parent, (char *)obj->Name(), "DtPrtjb", NULL, NULL, NULL,
		  fields)
{
   job_icon = obj;
   job_list = obj->Parent();
   printer = job_list->Parent();
}

void DtJobList::NotifySelected(BaseUI *obj)
{
   Container::NotifySelected(obj);

   int n_items;
   Selection(&n_items);
   DtFindD *findD = (DtFindD *) Parent();
   if (n_items == 0)
    {
      findD->cancel_jobs->Active(false);
      findD->goto_job->Active(false);
    }
   else
    {
      if (n_items == 1)
         findD->goto_job->Active(true);
      else
         findD->goto_job->Active(false);
      findD->cancel_jobs->Active(true);
    }
}

DtFindD::DtFindD(MotifUI *parent,
		 char *name,
		 SelectProc select_proc)
	: Dialog(parent, name)
{
   _has_been_posted = false;

   mainw = (DtMainW *) parent;
   _select_proc = select_proc;
   if (getuid() == 0) // check to see if we are root
      prompt = new Prompt(this, MESSAGE(JobName1L));
   else
      prompt = new Prompt(this, MESSAGE(MyJobNameL));
   prompt->AttachRight();
   prompt->AttachLeft();
   prompt->AttachTop();

   Container *rc = new Container(this, "rc", HORIZONTAL_ROW_COLUMN,
				 MULTIPLE_SELECT);
   if (getuid() == 0) // check to see if we are root
      match_any_user = new Button(rc, MESSAGE(OnlyRootJobsL), TOGGLE_BUTTON);
   else
      match_any_user = NULL;
   ignore_case = new Button(rc, MESSAGE(IgnoreCaseL), TOGGLE_BUTTON);
   ignore_case->Selected(true);
   exact_match = new Button(rc, MESSAGE(ExactMatchL), TOGGLE_BUTTON);
   rc->AttachRight();
   rc->AttachTop(prompt, 5);

   Sep *sep = new Sep(this);
   sep->AttachRight();
   sep->AttachLeft();

   sep->AttachTop(rc, 5);

   field1 = new LabelObj(this, MESSAGE(JobName1L));
   field1->AttachLeft(22);
   field1->AttachTop(sep, 5);
   field2 = new LabelObj(this, MESSAGE(PrinterL));
   field2->AttachLeft(field1);
   field2->AttachTop(sep, 5);
   field3 = new LabelObj(this, MESSAGE(OwnerL));
   field3->AttachLeft(field2);
   field3->AttachTop(sep, 5);
   field3->Visible(false);
   field4 = new LabelObj(this, MESSAGE(PositionL));
   field4->AttachLeft(field2);
   field4->AttachRight();
   field4->AttachTop(sep, 5);

   Container *form = new Container(this, "form", FORM);
   goto_job = new Button(form, MESSAGE(GotoL), PUSH_BUTTON, GotoCB, this);
   goto_job->Active(false);
   cancel_jobs = new Button(form, MESSAGE(CancelPrintJobsL), PUSH_BUTTON,
			    CancelJobsCB, this);
   cancel_jobs->Active(false);
   form->AttachBottom(5);
   form->AttachRight();
   form->AttachLeft();
   goto_job->AttachBottom();
   goto_job->AttachTop();
   cancel_jobs->AttachBottom();
   cancel_jobs->AttachTop();

   found_container = new DtJobList(this);
   found_container->AttachRight();
   found_container->AttachLeft();
   found_container->AttachBottom(form, 10);
   found_container->AttachTop(field1);

   start = new Button(this, MESSAGE(StartFindL), PUSH_BUTTON, StartCB, this);
   stop = new Button(this, MESSAGE(StopFindL), PUSH_BUTTON, StopCB, this);
   close_it = new Button(this, MESSAGE(CloseL), PUSH_BUTTON, CancelCB, this);
   stop->Active(false);
   help = new Button(this, MESSAGE(HelpL), PUSH_BUTTON, HelpCB, this);

   DefaultButton(start);
   CancelButton(close_it);
}

DtFindD::~DtFindD()
{
   // Empty
}

boolean DtFindD::SetVisiblity(boolean flag)
{
   if (_has_been_posted == false)
    {
      Dialog::SetVisiblity(flag);
      Refresh();
      int width = StringWidth(Name()) + 30;
      if (width < 400)
	 width = 400;
      if (Width() < width)
	 Width(width);
      _has_been_posted = true;
      int w1 = cancel_jobs->Width();
      int w2 = goto_job->Width();
      int offset = (width - (w1 + w2)) / 3;
      goto_job->AttachLeft(offset);
      cancel_jobs->AttachRight(offset);
      cancel_jobs->AttachLeft(NULL, 0);
    }
   found_container->DeleteChildren();
   cancel_jobs->Active(false);
   goto_job->Active(false);
   if (match_any_user)
    {
      match_any_user->Selected(false);
      match_any_user->Active(mainw->setPrefD->ShowOnlyMyJobs() ? false : true);
    }
   Dialog::SetVisiblity(flag);
   if (flag)
      prompt->SetFocus();
   return true;
}

void DtFindD::UpdateMatchAnyUser()
{
   if (match_any_user)
      match_any_user->Active(mainw->setPrefD->ShowOnlyMyJobs() ? false : true);
}

boolean DtFindD::MatchAnyUser()
{
   if (match_any_user)
    {
      if (match_any_user->Active() == false)
         return false;
      else
         return (match_any_user->Selected() ? false : true);
    }
   else
      return false;
}

void DtFindD::Start()
{
   _working = true;
   mainw->in_find = true;
   mainw->WorkingCursor(true);
   stop->PointerShape(LEFT_SLANTED_ARROW_CURSOR);
   help->PointerShape(LEFT_SLANTED_ARROW_CURSOR);
   stop->Active(true);
   DefaultButton(stop);
   close_it->Active(false);
   start->Active(false);
   prompt->Active(false);
   ignore_case->Active(false);
   exact_match->Active(false);
   found_container->DeleteChildren();
   cancel_jobs->Active(false);
   goto_job->Active(false);
   if (match_any_user)
      match_any_user->Active(false);
   found_container->BeginUpdate();

   _cur_obj = 0;
   if (mainw->container->NumChildren())
    {
      BaseUI *icon = mainw->container->Children()[0];
      _prev_visible = icon->Visible();
      _prev_opened = icon->Open();
    }
   AddTimeOut(CheckQueue, NULL, 200);
}

void DtFindD::Cancel()
{
   Visible(false);
   Stop();
}

void DtFindD::Stop()
{
   if (_cur_obj > mainw->container->NumChildren())
      return;
   _cur_obj = mainw->container->NumChildren() + 1;
   found_container->EndUpdate();
   start->Active(true);
   DefaultButton(start);
   stop->Active(false);
   close_it->Active(true);
   prompt->Active(true);
   ignore_case->Active(true);
   exact_match->Active(true);
   prompt->SetFocus();
   if (mainw->setModList && mainw->setModList->Visible())
      mainw->setModList->Reset();
   if (match_any_user)
      match_any_user->Active(mainw->setPrefD->ShowOnlyMyJobs() ? false : true);
   if (Visible())
    {
       int n_matches;
       BaseUI **matches;
       char *value1 = NULL;
       char *value = prompt->Value();
       if (!value || *value == '\0')
	  value = ".*";
       else if (*value == '*')
	{
	  value1 = new char[strlen(value) + 2];
	  sprintf(value1, ".%s", value);
	  value = value1;
	}
       mainw->container->FindByName(value, 0, &n_matches, &matches,
				    _select_proc,
				    exact_match->Selected() ? false : true,
				    ignore_case->Selected() ? false : true);
       delete [] value1;
       if (n_matches)
	{
	  IconFields fields = new IconFieldsRec;
	  int n_fields, i, w;

          if (MatchAnyUser())
	     n_fields = 3;
	  else
	     n_fields = 2;
	  fields->n_fields = n_fields;
	  fields->field_spacing = 20;
	  fields->fields = new char *[n_fields]; 
	  fields->fields_widths = new int[n_fields];
	  fields->alignments = new LabelType[n_fields];
	  fields->draw_fields = NULL;
	  fields->selected = NULL;
	  fields->active = NULL;

	  fields->name_width = StringWidth(MESSAGE(JobName1L));
	  fields->fields_widths[0] = StringWidth(MESSAGE(PrinterL));
	  fields->alignments[0] = LEFT_JUSTIFIED;
	  if (n_fields == 3)
	   {
	     fields->fields_widths[1] = StringWidth(MESSAGE(OwnerL));
	     fields->fields_widths[2] = StringWidth(MESSAGE(PositionL));
	     fields->alignments[1] = LEFT_JUSTIFIED;
	     fields->alignments[2] = RIGHT_JUSTIFIED;
	     if (field3->Visible() == false)
	      {
	        field3->Visible(true);
	        field4->AttachLeft(field3);
	      }
	   }
	  else
	   {
	     fields->alignments[1] = RIGHT_JUSTIFIED;
	     fields->fields_widths[1] = StringWidth(MESSAGE(PositionL));
	     if (field4->Visible())
	      {
	        field4->AttachLeft(field2);
	        field3->Visible(false);
	      }
	   }
          for (i = 0; i < n_matches; i++)
	   {
	     DtPrtJobIcon *job = (DtPrtJobIcon *)matches[i];
	     if ((w = StringWidth(job->Name())) > fields->name_width)
	        fields->name_width = w;
	     w = StringWidth(job->Parent()->Parent()->Name());
	     if (w > fields->fields_widths[0])
	        fields->fields_widths[0] = w;
	     if (n_fields == 3)
	      {
	        char *s = job->PrintJobObj()->AttributeValue((char *)OWNER);
	        if ((w = StringWidth(s)) > fields->fields_widths[1])
	           fields->fields_widths[1] = w;
	      }
	   }
	  if (fields->name_width % 2)
	     fields->name_width += 1;

	  if (fields->fields_widths[0] % 2)
	     fields->fields_widths[0] += 1;

	  if (n_fields == 3)
	   {
	     if (fields->fields_widths[1] % 2)
	        fields->fields_widths[1] += 1;
	     if (fields->fields_widths[2] % 2)
	        fields->fields_widths[2] += 1;
	   }
	  else
	   {
	     if (fields->fields_widths[1] % 2)
	        fields->fields_widths[1] += 1;
	   }

	  field1->Width(fields->name_width + 20);
	  field2->Width(fields->fields_widths[0] + 20);
	  field3->Width(fields->fields_widths[1] + 20);
	  if (n_fields == 3)
	     field4->Width(fields->fields_widths[2] + 20);

          for (i = 0; i < n_matches; i++)
	   {
	     DtPrtJobIcon *job = (DtPrtJobIcon *)matches[i];
	     char number[9];
	     sprintf(number, "%d", job->Order() + 1);
	     fields->fields[0] = (char *)job->Parent()->Parent()->Name();
	     if (n_fields == 3)
	      {
	        char *s = job->PrintJobObj()->AttributeValue((char *)OWNER);
                fields->fields[1] = s;
                fields->fields[2] = number;
	      }
	     else
                fields->fields[1] = number;
             _JobIcon *icon = new _JobIcon(found_container, job, fields);
	   }
          delete []matches;
	  delete fields->fields;
	  delete fields->fields_widths;
	  delete fields->alignments;
	  delete fields;
	}
       else
	  new LabelObj(found_container, MESSAGE(NoMatchesL));
    }
   mainw->WorkingCursor(false);
   _working = false;
   mainw->in_find = false;
}

void DtFindD::UpdateQueue()
{
   if (_cur_obj < mainw->container->NumChildren())
    {
      BaseUI *icon = mainw->container->Children()[_cur_obj];
      char *message = new char[200];
      sprintf(message, MESSAGE(SearchL), icon->Name());
      found_container->UpdateMessage(message);
      delete [] message;
      icon->Visible(true);
      DtPrinterIcon *icon1 = (DtPrinterIcon *)icon;
      icon1->waitForChildren = true;
      icon1->Open(true);
      icon1->waitForChildren = false;
      boolean no_children;
      if (icon1->QueueObj()->NumChildren() == 0)
	 no_children = true;
      else
       {
	 int n_matches;
         char *value1 = NULL;
         char *value = prompt->Value();
         if (!value || *value == '\0')
	    value = ".*";
         else if (*value == '*')
	  {
	    value1 = new char[strlen(value) + 2];
	    sprintf(value1, ".%s", value);
	    value = value1;
          }
         icon1->FindByName(value, 0, &n_matches, NULL, _select_proc,
                           exact_match->Selected() ? false : true,
			   ignore_case->Selected() ? false : true);
	 delete value1;
	 if (n_matches)
	    no_children = false;
	 else
	    no_children = true;
       }
      if (no_children)
       {
         if (_prev_visible == false)
	    icon->Visible(false);
         if (_prev_opened == false)
	    icon->Open(false);
       }
      _cur_obj++;
      if (_cur_obj < mainw->container->NumChildren())
       {
         icon = mainw->container->Children()[_cur_obj];
         _prev_visible = icon->Visible();
         _prev_opened = icon->Open();
       }
      AddTimeOut(CheckQueue, NULL, 200);
    }
   else if (_cur_obj == mainw->container->NumChildren())
      Stop();
}

boolean DtFindD::HandleHelpRequest()
{
   mainw->DisplayHelp("FindDialogDE");
   return true;
}

void DtFindD::CheckQueue(BaseUI *obj, void *)
{
   ((DtFindD *)obj)->UpdateQueue(); 
}

BaseUI *DtFindD::FindJob(BaseUI *obj)
{
   int i;
   BaseUI *job = ((_JobIcon *)obj)->job_icon;
   BaseUI **children = ((_JobIcon *)obj)->job_list->Children();
   int n_children = ((_JobIcon *)obj)->job_list->NumChildren();
   for (i = 0; i < n_children; i++)
      if (job == children[i])
	 return children[i];
   Dialog *dialog = new Dialog(mainw, (char *) Name(),
			       MESSAGE(NotFoundMessageL), INFORMATION,
			       MESSAGE(OKL));
   dialog->Visible(true);
   return NULL;
}

void DtFindD::GotoCB(void *data)
{
   DtFindD *obj = (DtFindD *) data;
   BaseUI **selection;
   int n_items;
   obj->found_container->Selection(&n_items, &selection);
   if (n_items == 1)
    {
      BaseUI *print_job = obj->FindJob(selection[0]);
      if (print_job)
       {
         print_job->MakeVisible();
         print_job->Selected(true);
       }
    }
   delete []selection;
}

void DtFindD::CancelJobsCB(void *data)
{
   DtFindD *obj = (DtFindD *) data;
   extern void ActionCB(void *data, BaseUI *obj, char *actionReferenceName);

   BaseUI **selection;
   int n_items;
   obj->found_container->Selection(&n_items, &selection);
   if (n_items)
    {
      int i;
      for (i = 0; i < n_items; i++)
       {
         BaseUI *print_job = obj->FindJob(selection[i]);
         if (print_job)
	  {
	    DtApp *app = (DtApp *)obj->mainw->Parent();
	    app->ActionCB(print_job, (char *)CANCEL_PRINT_JOB);
	  }
       }
    }
   delete []selection;
}

void DtFindD::CloseCB()
{
   Cancel();
}

void DtFindD::CancelCB(void *data)
{
   DtFindD *obj = (DtFindD *) data;
   obj->Cancel();
}

void DtFindD::StartCB(void *data)
{
   DtFindD *obj = (DtFindD *) data;
   obj->Start();
}

void DtFindD::StopCB(void *data)
{
   DtFindD *obj = (DtFindD *) data;
   obj->_cur_obj = obj->mainw->container->NumChildren();
}

void DtFindD::HelpCB(void *data)
{
   DtFindD *obj = (DtFindD *) data;
   obj->HandleHelpRequest();
}

void DtFindD::DeleteJobFromList(BaseUI *obj)
{
   int i;
   int n_children = found_container->NumChildren();
   _JobIcon **children = (_JobIcon **)found_container->Children();
   for (i = 0; i < n_children; i++)
    {
      if (obj->Parent() == children[i]->job_list)
       {
         if (children[i]->job_icon == obj)
	  {
	    children[i]->Selected(false);
	    delete children[i];
	    return;
	  }
       }
    }
}

void DtFindD::UpdatePrinter(BaseUI *printer)
{
   int n_children = found_container->NumChildren();
   if (n_children == 0)
      return;

   _JobIcon **children = (_JobIcon **)found_container->Children();
   int i, w1;
   int width = StringWidth(MESSAGE(PrinterL));
   BaseUI *last_printer = NULL;
   for (i = 0; i < n_children; i++)
    {
      if (last_printer != children[i]->printer)
       {
         last_printer = children[i]->printer;
	 if ((w1 = StringWidth(last_printer->Name())) > width)
	    width = w1;
       }
    }
   if (width % 2)
      width += 1;

   // Create a widget and resize it to the desired width, then resize the
   // row column to the width + margin space.  I have to do this because
   // the row column widget does not resize itself correctly.
   Dimension wid;
   XtVaGetValues(found_container->InnerWidget(), XmNwidth, &wid, NULL);
   wid -= field2->Width();
   wid += width;
   new LabelObj(found_container, " ");
   children = (_JobIcon **)found_container->Children();
   children[n_children]->Order(0);
   n_children++;
   children[0]->Width((int)wid);
   wid += 6;
   XtVaSetValues(found_container->InnerWidget(), XmNwidth, (int)wid, NULL);

   field2->Width(width + 20);
   for (i = 1; i < n_children; i++)
    {
      if (printer == children[i]->printer)
	 children[i]->Field(0, (char *)printer->Name(), width);
      else
	 children[i]->Field(0, NULL, width);
    }
   delete children[0];
}

void DtFindD::UpdatePositions(BaseUI *printer)
{
   if (mainw->in_find)
      return;

   int n_children = found_container->NumChildren();
   if (n_children == 0)
      return;

   _JobIcon **children = (_JobIcon **)found_container->Children();
   int i, index = children[0]->NumberFields() - 1;
   for (i = 0; i < n_children; i++)
    {
      if (printer == children[i]->printer)
	 children[i]->Field(index, children[i]->job_icon->TopString(), 0);
    }
}
