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
