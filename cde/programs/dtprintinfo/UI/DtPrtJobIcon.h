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
/* $XConsortium: DtPrtJobIcon.h /main/3 1995/11/06 09:36:33 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef DTPRTJOBICON_H
#define DTPRTJOBICON_H

#include "IconObj.h"
#include "PrintJob.h"

class DtMainW;
class DtProps;

extern const char *PROPS_PRINTJOB_ID;
extern const char *PRINTJOB_ID;

class DtPrtJobIcon : public IconObj 
{

   DtMainW *mainw;
   DtProps *props;
   PrintJob *print_job;
   char job_number[10];

   boolean HandleHelpRequest();
   void NotifyVisiblity(BaseUI *);

 public:
   DtPrtJobIcon(DtMainW *, AnyUI *parent, PrintJob *printJob,
		int SequenceNumber);
   ~DtPrtJobIcon();

   void DisplayProps();
   void PrintJobObj(PrintJob *printJob);
   PrintJob *PrintJobObj() { return print_job; }
   const char *JobNumber() { return job_number; }

};

#endif // DTPRTJOBICON_H
