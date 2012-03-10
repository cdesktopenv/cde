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
