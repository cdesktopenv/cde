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
/* $XConsortium: ParseJobs.h /main/3 1995/11/06 09:46:45 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef PARSEJOBS_H
#define PARSEJOBS_H

typedef enum
{
   UNKNOWN_OUTPUT,
   AIX_V2_OUTPUT,
   AIX_V3_OUTPUT,
   BSD_OUTPUT
} JobOutputType;   

extern JobOutputType DetermineOutput(char *output);

// returns socket to print server, or -1 if error
// timeout is in seconds, a default of 5 is used if timeout <= 0.
extern int ConnectToPrintServer(const char *server, int timeout);

// return 0 if error, otherwise return 1 if successful
extern int SendPrintJobStatusReguest(int sockfd, const char *printer);

extern void LocalPrintJobs(
   char *printer,
   char **return_job_list,
   int *return_n_jobs);

extern int RemotePrintJobs(
   char *server,
   char *printer,
   char **return_job_list,
   int *return_n_jobs);

extern int ParseRemotePrintJobs(
   char *printer,
   char *jobs,
   char **return_job_list,
   int *return_n_jobs);

extern int ParseBSDPrintJobs(
   char *printer,
   char *jobs,
   char **return_job_list,
   int *return_n_jobs);

extern int ParseAIXv3PrintJobs(
   char *printer,
   char *jobs,
   char **return_job_list,
   int *return_n_jobs);

extern int ParseAIXv2PrintJobs(
   char *printer,
   char *jobs,
   char **return_job_list,
   int *return_n_jobs);

#endif // PARSEJOBS_H
