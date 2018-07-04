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
/* $TOG: DtPrtJobIcon.C /main/4 1998/07/24 16:13:33 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "DtPrtJobIcon.h"
#include "DtApp.h"
#include "DtMainW.h"
#include "DtProps.h"
#include "DtFindD.h"

const char *PRINTJOB_ID = "PrintJobDE";
const char *PROPS_PRINTJOB_ID = "PrintJobPropsDE";

static char *GetBotString(DtMainW *mainW, BaseUI *parent, PrintJob *job)
{
   boolean need_details = (parent->IconView() == DETAILS ? true : false);
   DtApp *app = (DtApp *)mainW->Parent();
   return app->GetBottomString(job, need_details);
}

static char *GetTopString(int seq_num)
{
   static char number[9];
   sprintf(number, "%d", seq_num);
   return number;
}

DtPrtJobIcon::DtPrtJobIcon(DtMainW *mainW, AnyUI *parent, PrintJob *job,
			   int seq_num)
   : IconObj((char *) job->ObjectClassName(), parent, job->DisplayName(),
	     "DtPrtjb", NULL, GetTopString(seq_num),
	     GetBotString(mainW, parent, job))
{
   mainw = mainW;
   mainw->RegisterPopup(this);
   ApplicationData = job;
   strcpy(job_number, job->AttributeValue((char *)JOB_NUMBER));
   print_job = job;
   props = NULL;
}

DtPrtJobIcon::~DtPrtJobIcon()
{
   if (Selected())
      Selected(false);
   if (mainw->findD)
      mainw->findD->DeleteJobFromList(this);
   delete props;
}

boolean DtPrtJobIcon::HandleHelpRequest()
{
   mainw->DisplayHelp((char *)PRINTJOB_ID);
   return true;
}

void DtPrtJobIcon::DisplayProps()
{
   if (!props)
    {
      char *title = new char[200];
      sprintf(title, MESSAGE(PrintJobPropsTitleL), mainw->Name());
      props = new DtProps(mainw, title, (char *) PROPS_PRINTJOB_ID, false, 
                          print_job->NumAttributes(), 
                          print_job->Attributes());
      delete [] title;
    }
   props->Visible(true);
   props->ToFront();
}

void DtPrtJobIcon::PrintJobObj(PrintJob *job)
{
   ApplicationData = job;
   print_job = job;
   if (strcmp(job_number, job->AttributeValue((char *)JOB_NUMBER)))
    {
      Name(job->DisplayName());
      BottomString(GetBotString(mainw, Parent(), job));
      snprintf(job_number, sizeof(job_number), "%s", job->AttributeValue((char *)JOB_NUMBER));
      if (props)
       {
         props->Visible(false);
         delete props;
         props = NULL;
         DisplayProps();
       }
    }
}

void DtPrtJobIcon::NotifyVisiblity(BaseUI * /*obj*/)
{
   // Empty
}
