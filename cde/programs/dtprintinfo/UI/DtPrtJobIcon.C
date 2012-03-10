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
      strcpy(job_number, job->AttributeValue((char *)JOB_NUMBER));
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
