/* $TOG: PrintJob.C /main/6 1998/07/24 16:17:39 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "PrintJob.h"

// Object Class Name
const char *PRINTJOB = "PrintJob";

// Actions
const char *CANCEL_PRINT_JOB  = "CancelPrintJob";

// Attributes
const char *PRINTJOB_NAME     = "PrintJobName";
const char *OWNER             = "Owner";
const char *JOB_NUMBER        = "JobNumber";
const char *JOB_SIZE          = "JobSize";
const char *SUBMITTED         = "Submitted";
const char *DATE_SUBMITTED     = "DateSubmitted";
const char *TIME_SUBMITTED     = "TimeSubmitted";

PrintJob::PrintJob(BaseObj *parent,
		   char *JobName,
		   char *JobNumber,
                   char *Owner,
		   char *Date,
		   char *Time,
		   char *Size)
	: BaseObj(parent, JobName)
{
   AddAction(&PrintJob::CancelJob, CANCEL_PRINT_JOB, MESSAGE(CancelChoiceL),
	     MESSAGE(CancelMnemonicL), NULL, NULL, true,
	     MESSAGE(CancelAcceleratorL), "<Key>osfDelete");

   char *Help = NULL, *ContextualHelp = NULL, *Listing = NULL;
   Characteristics Mask = OPTIONAL;
   ValueList ValueListType = NO_LIST;
   int n = 0;

   AddAttribute(PRINTJOB_NAME, MESSAGE(JobNameL),
                Help, ContextualHelp, Mask, ValueListType, Listing);
   _attributes[n]->Value = STRDUP(JobName);
   _attributes[n]->DisplayValue = STRDUP(JobName);
   n++;
   AddAttribute(OWNER, MESSAGE(OwnerL),
                Help, ContextualHelp, Mask, ValueListType, Listing);
   _attributes[n]->Value = STRDUP(Owner);
   _attributes[n]->DisplayValue = STRDUP(Owner);
   n++;
   AddAttribute(JOB_NUMBER, MESSAGE(JobNumberL),
                Help, ContextualHelp, Mask, ValueListType, Listing);
   _attributes[n]->Value = STRDUP(JobNumber);
   _attributes[n]->DisplayValue = STRDUP(JobNumber);
   _jobNumber = _attributes[n]->DisplayValue;
   n++;
   AddAttribute(JOB_SIZE, MESSAGE(SizeL),
                Help, ContextualHelp, Mask, ValueListType, Listing);
   _attributes[n]->Value = STRDUP(Size);
   _attributes[n]->DisplayValue = STRDUP(Size);
   n++;
   ValueListType = INFORMATION_LINE;
   AddAttribute(SUBMITTED, MESSAGE(SubmittedL),
                Help, ContextualHelp, Mask, ValueListType, Listing);
   _attributes[n]->Value = STRDUP(Time);
   _attributes[n]->DisplayValue = STRDUP(Time);
   n++;
   ValueListType = NO_LIST;
   char *message = new char [strlen(MESSAGE(TimeL)) + 4];
   sprintf(message, "   %s", MESSAGE(TimeL));
   AddAttribute(TIME_SUBMITTED, message,
                Help, ContextualHelp, Mask, ValueListType, Listing);
   _attributes[n]->Value = STRDUP(Time);
   _attributes[n]->DisplayValue = STRDUP(Time);
   delete [] message;
   n++;
   message = new char [strlen(MESSAGE(DateL)) + 4];
   sprintf(message, "   %s", MESSAGE(DateL));
   AddAttribute(DATE_SUBMITTED, message, 
                Help, ContextualHelp, Mask, ValueListType, Listing);
   _attributes[n]->Value = STRDUP(Date);
   _attributes[n]->DisplayValue = STRDUP(Date);
   delete [] message;
}

PrintJob::~PrintJob()
{
   // Empty
}

int PrintJob::CancelJob(BaseObj *obj, char **output, BaseObj * /*requestor*/)
{
   static char command[256];
   PrintJob *me = (PrintJob *) obj;

#ifdef aix
   sprintf(command, "enq -P%s -x%s", me->Parent()->Name(), me->_jobNumber);
#elif __osf__
   sprintf(command, "lprm -P%s %s", me->Parent()->Name(), me->_jobNumber);
#else
   sprintf(command, "cancel %s-%s", me->Parent()->Name(), me->_jobNumber);
#endif
   return me->RunCommand(command, NULL, output);
}
