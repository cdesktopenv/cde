/* $XConsortium: PrintJob.h /main/3 1995/11/06 09:47:08 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef PRINTJOB_H
#define PRINTJOB_H

#include "BaseObj.h"
#include "dtprintinfomsg.h"

#include <string.h>

// Object Class Name
extern const char *PRINTJOB;

// Actions
extern const char *CANCEL_PRINT_JOB;

// Attributes
extern const char *PRINTJOB_NAME;
extern const char *OWNER;
extern const char *JOB_NUMBER;
extern const char *JOB_SIZE;
extern const char *SUBMITTED;
extern const char *DATE_SUBMITTED;
extern const char *TIME_SUBMITTED;

class PrintJob : public BaseObj {

   friend int CancelJob(BaseObj *, char **output, BaseObj *requestor);

 protected:

   char *_jobNumber;
   static int CancelJob(BaseObj *, char **output, BaseObj *requestor);

 public:

   PrintJob(BaseObj *parent, char *JobName, char *JobNumber, char *Owner,
	    char *Date, char *Time, char *Size);

   virtual ~PrintJob();

   const char *JobNumber() { return _jobNumber; }

   virtual const char *const ObjectClassName() { return PRINTJOB; }

};

#endif // PRINTJOB_H 
