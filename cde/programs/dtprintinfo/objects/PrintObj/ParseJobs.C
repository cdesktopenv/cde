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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $TOG: ParseJobs.C /main/12 1998/08/03 16:30:29 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#if defined(_AIX)
   /*
    * XXXXX - AIX hack alert!!! This is needed for an AIX defect
    * in the definition of inet_addr in <arpa/inet.h>
    */
extern "C" in_addr_t inet_addr(const char *);
#endif
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include <X11/Intrinsic.h>

#include "dtprintinfomsg.h"

#ifdef NO_REGCOMP
  #include <ctype.h>
  #if defined(SVR4) || defined(SYSV)
    #include <libgen.h>
  #endif
#else
  #include <regex.h>
#endif

#if defined(aix) || defined(__osf__) || defined(USL) || defined(__uxp__)
extern "C" { int rresvport(int *); }
#endif

#ifdef hpux
  #define SETEUID(id) setresuid(getuid(), (uid_t)id, (uid_t)0);
#else
  #if defined(aix) || defined(USL) || defined(__uxp__)
    extern "C" { extern int seteuid(uid_t); }
  #endif
  #define SETEUID(id) seteuid((uid_t)id)
#endif

#include "ParseJobs.h"
#include "Invoke.h"

static int G_AIX_LOCAL = 0;

int RemotePrintJobs(char *server, char *printer, char **job_list, int *n_jobs)
{
   *n_jobs = 0;
   *job_list = NULL;

   // hookup to Server
   int sockfd;
   if ((sockfd = ConnectToPrintServer(server, 15)) == -1)
      return 0;

   char *output = NULL;
   int rc = 0;
   if (SendPrintJobStatusReguest(sockfd, printer))
    {
      int len = 512;
      int ctr = 1;
      int n;
      output = (char *)malloc(512);
      char *out1 = output;
      char *out2 = out1 + 511;

      while (1)
       {
         if ((n = read(sockfd, out1, len)) == len)
          {
            len = 512;
            ctr++;
            output = (char *)realloc(output, (ctr * 512));
            out1 = output + ((ctr - 1) * 512);
            out2 = out1 + 511;
          }
         else if (n > 0)
          {
            out1 += n;
            len = out2 - out1 + 1;
          }
         else
	    break;
       }
      *out1 = 0;
      close(sockfd);
      rc = 1;
    }
   if (output)
    {
      if (*output)
         rc = ParseRemotePrintJobs(printer, output, job_list, n_jobs);
      free(output);
    }
   return rc;
}

int ParseRemotePrintJobs(char *printer, char *jobs, char **job_list,
			 int *n_jobs)
{
   *n_jobs = 0;
   *job_list = NULL;

   int rc = 0;
   if (jobs && *jobs)
    {
      switch (DetermineOutput(jobs))
      {
      case AIX_V2_OUTPUT:
         rc = ParseAIXv2PrintJobs(printer, jobs, job_list, n_jobs);
         break;
      case AIX_V3_OUTPUT:
         rc = ParseAIXv3PrintJobs(printer, jobs, job_list, n_jobs);
         break;
      case BSD_OUTPUT:
         rc = ParseBSDPrintJobs(printer, jobs, job_list, n_jobs);
         break;
      default:
      case UNKNOWN_OUTPUT:
	 rc = 1;
         break;
      }
    }
   return rc;
}

static void connect_timeout(int not_used)
{
   not_used = 0;
}

// Create a connection to the remote printer server.
int ConnectToPrintServer(const char *rhost, int timeout)
{
   struct hostent *hp;
   struct servent *sp;
   struct sockaddr_in sin;
   int s, lport = IPPORT_RESERVED - 1;

   // Get the host address and port number to connect to.
#if defined(__uxp__) || (defined(USL) && (OSMAJORVERSION > 1))
   if (!(hp = gethostbyname((char *) rhost)))
    {
      unsigned long tmpaddr = inet_addr((char *) rhost);
#else
   if (!(hp = gethostbyname(rhost)))
    {
      unsigned long tmpaddr = inet_addr(rhost);
#endif
      hp = gethostbyaddr((char *)&tmpaddr, sizeof(tmpaddr), AF_INET);
    }
   if (hp == NULL)
      return -1;
   if (!(sp = getservbyname("printer", "tcp")))
      return -1;

   memset((char *)&sin, 0, sizeof(sin));
   memcpy((char *)&sin.sin_addr, hp->h_addr, hp->h_length);
   sin.sin_family = hp->h_addrtype;
   sin.sin_port = sp->s_port;

   // run as user's UID, but with privileges from root
   SETEUID(0);

   // Try connecting to the server.
   s = rresvport(&lport);

   signal(SIGALRM, connect_timeout);
   if (timeout > 0)
      alarm(timeout);
   else
      alarm(15);
   if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) 
    {
      close(s);
      s = -1;
    }
   alarm(0);
   SETEUID(getuid());
   return s;
}

int SendPrintJobStatusReguest(int sockfd, const char *printer)
{
   char *buf = new char[100];
   int size;

   sprintf(buf, "%c%s \n", '\4', printer);

   size=strlen(buf);
   size = write(sockfd, buf, size);
   if (write(sockfd, buf, size) != size)
    {
      close(sockfd);
      delete [] buf;
      return 0;
    }
   delete [] buf;
   return 1;
}

JobOutputType DetermineOutput(char *output)
{
   char *s, *s1;
   int i;
   static int first_time = 1;
#ifdef NO_REGCOMP
   static char *aixv2_pattern;
   static char *aixv3_pattern;
   static char *bsd_pattern;
   /* 
    * Due to bug in regcmp/regex (bug id 1191772), change to C locale for
    * this parsing work.  And change back at end of procedure.  This workaround
    * appears to be ok since the print service output parsed here is in a
    * "C locale" 8-bit form for inter-machine networking.
    */
   setlocale(LC_ALL, "C"); 

   if (first_time)
    {
      aixv2_pattern = regcmp("dev.*arg.*status.*request", (char *)NULL);
      aixv3_pattern = regcmp("Queue.*Dev.*Status.*Job", (char *)NULL);
      bsd_pattern = regcmp(":.*\\[.*\\]", (char *)NULL);
      first_time = 0;
    }
#else
   static regex_t aixv2_pattern;
   static regex_t aixv3_pattern;
   static regex_t bsd_pattern;

   if (first_time)
    {
      regcomp(&aixv2_pattern, "dev.*arg.*status.*request", REG_NOSUB);
      regcomp(&aixv3_pattern, "Queue.*Dev.*Status.*Job", REG_NOSUB);
      regcomp(&bsd_pattern, ":.*\\[.*\\]", REG_NOSUB);
      first_time = 0;
    }
#endif

   s = output;
   if (s1 = strchr(s, '\n'))
      s1++;
   i = 0;
   while (i < 10 && s && *s)
    {
#ifdef NO_REGCOMP
      if (regex(bsd_pattern, s)) {
   	 setlocale(LC_ALL, ""); 
	 return BSD_OUTPUT;
      }
      if (regex(aixv3_pattern, s)) {
   	 setlocale(LC_ALL, ""); 
	 return AIX_V3_OUTPUT;
      }
      if (regex(aixv2_pattern, s)) {
   	 setlocale(LC_ALL, ""); 
	 return AIX_V2_OUTPUT;
      }
#else
      if (!regexec(&bsd_pattern, s, (size_t)0, NULL, 0))
	 return BSD_OUTPUT;
      if (!regexec(&aixv3_pattern, s, (size_t)0, NULL, 0))
	 return AIX_V3_OUTPUT;
      if (!regexec(&aixv2_pattern, s, (size_t)0, NULL, 0))
	 return AIX_V2_OUTPUT;
#endif
      s = s1;
      if (s1 = strchr(s, '\n'))
         s1++;
      i++;
    }
#ifdef NO_REGCOMP
   setlocale(LC_ALL, "");
#endif
   return UNKNOWN_OUTPUT;
}

/////////////////////////////   LOCAL PARSERS /////////////////////////////////

#ifdef aix
void LocalPrintJobs(char *printer, char **job_list, int *n_jobs)
{
   char *output;
   char *cmd = new char[60];

   sprintf(cmd, "LANG=C enq -LP%s", printer);
   Invoke *_thread = new Invoke(cmd, &output);

   // run as user's UID, but with privileges from root
   SETEUID(0);
   G_AIX_LOCAL = 1;
   ParseAIXv3PrintJobs(printer, output, job_list, n_jobs);
   G_AIX_LOCAL = 0;
   SETEUID(getuid());

   delete [] cmd;
   delete output;
   delete _thread;
}
#endif

#if defined(sun) || defined(USL) || defined(__uxp__)

// SUN LOCAL PARSER, actually this gets the local information from the file
// system, it should have the good performance since no processes (lpstat)
// are needed.

typedef struct
{
   int immediate;
   long secs;
   char *line;
} StatusLineStruct, *StatusLine, **StatusLineList;

#define TMP_DIR "/usr/spool/lp/tmp"
#define REQ_DIR "/usr/spool/lp/requests/%s/"
#define SPOOL_DIR "/var/spool/lp/tmp/%s/"

static void check_dir(char *printer, char *tmp_dir, StatusLineList *job_list,
                      int *n_jobs, int prev_n_jobs);
static int SortJobs(const void *, const void *);

void LocalPrintJobs(char *printer, char **return_job_list, int *return_n_jobs)
{
   DIR *lp_tmp_dir;
   struct dirent *dir_struct;
   struct stat statbuff;
   int n_jobs = 0;
   int i;
   char oldpwd[300];
   int current_size;
   int len;

   static char *job_list1 = NULL;
   static int prev_buf_size = 0;
   static StatusLineList job_list = NULL;
   static int prev_n_jobs = 0;

   if (prev_buf_size == 0)
    {
      prev_buf_size = BUFSIZ;
      job_list1 = (char *)malloc(prev_buf_size);
    }

   current_size = 0;
   *job_list1 = '\0';
   *return_n_jobs = 0;
	
   // run as user's UID, but with privileges from root
   SETEUID(0);

   getcwd(oldpwd, 300);
   if (getenv("TMP_DIR"))
      chdir(getenv("TMP_DIR"));
   else
      chdir(TMP_DIR);
   if (!(lp_tmp_dir = opendir(".")))
    {
      *return_job_list = NULL;
      *return_n_jobs = 0;
      chdir(oldpwd);
      SETEUID(getuid());
      return;
    }

   dir_struct = readdir(lp_tmp_dir);
   for ( ; dir_struct ; dir_struct = readdir(lp_tmp_dir))
    {
      if (*dir_struct->d_name == '.')
         continue;
      if (stat(dir_struct->d_name, &statbuff) >= 0 &&
          statbuff.st_mode & S_IFDIR)
       {
         chdir(dir_struct->d_name);
         check_dir(printer, dir_struct->d_name, &job_list, &n_jobs,
		   prev_n_jobs);
         chdir("..");
       }
    }
   closedir(lp_tmp_dir);
   prev_n_jobs = prev_n_jobs > n_jobs ? prev_n_jobs : n_jobs;
   qsort(job_list, n_jobs, sizeof(StatusLineList), SortJobs);
   for (i = 0; i < n_jobs; i++)
    {
      len = strlen(job_list[i]->line);
      if (prev_buf_size < (current_size + len + 1))
         job_list1 = (char *) realloc(job_list1, (current_size + len + 1) *
				      sizeof(char *));
      memcpy(job_list1 + current_size, job_list[i]->line, len);
      current_size += len;
    }
   *(job_list1 + current_size) = '\0';
   prev_buf_size = prev_buf_size > current_size ? prev_buf_size : current_size;
   *return_job_list = job_list1;
   *return_n_jobs = n_jobs;
   chdir(oldpwd);
   SETEUID(getuid());
}

static void check_dir(char *printer, char *tmp_dir, StatusLineList *job_list,
                      int *n_jobs, int prev_n_jobs)
{
   DIR *lp_tmp_dir;
   struct dirent *dir_struct;
   char buf[256];
   int req_len;
   int spool_len;
   int line_ct;
   char *line;
   char date_str[100];
   char owner[100];
   char job_size[100];
   FILE *req, *job;
   time_t secs;
   int found;
   int immediate;
   char *s;
   char title[200];
   char *jobname;
   StatusLineList j_list;

   char *filename = new char[200];
   char *filename1 = new char[200];
   char *request = new char[300];
   char *spool_dir = new char[300];

   if (getenv("REQ_DIR"))
      sprintf(request, getenv("REQ_DIR"), tmp_dir);
   else
      sprintf(request, REQ_DIR, tmp_dir);
   req_len = strlen(request);
   sprintf(spool_dir, SPOOL_DIR, tmp_dir);
   spool_len = strlen(spool_dir);

   if (!(lp_tmp_dir = opendir(".")))
    {
      delete [] filename;
      delete [] filename1;
      delete [] request;
      delete [] spool_dir;
      return;
    }

   dir_struct = readdir(lp_tmp_dir);
   for ( ; dir_struct ; dir_struct = readdir(lp_tmp_dir))
    {
      int len = strlen(dir_struct->d_name);
      if (len < 3)
	 continue;
      if (strcmp(dir_struct->d_name + len - 2, "-0"))
	 continue;
      *(request + req_len) = '\0';
      strcat(request + req_len, dir_struct->d_name);
      if (!(req = fopen(request, "r")))
	 continue;
      if (!(job = fopen(dir_struct->d_name, "r")))
       {
         fclose(req);
	 continue;
       }
      found = 1;
      immediate = 0;
      title[0] = '\0';
      filename[0] = '\0';
      filename1[0] = '\0';
      date_str[0] = '\0';
      owner[0] = '\0';
      job_size[0] = '\0';
      while (found && fgets(buf, 100, job))
       {
	 line = strtok(buf, "\n");
	 switch (*line)
	 {
	 case 'H':
	    if (!strcmp("immediate", line + 2))
	       immediate = 1;
	    break;
	 case 'D':
	    if (strcmp(printer, line + 2))
	       found = 0;
	    break;
	 case 'F':
            *(spool_dir + spool_len) = '\0';
            strncat(spool_dir, dir_struct->d_name, len - 1);
	    strcat(spool_dir, "1");
            if (strcmp(spool_dir, line + 2))
	       strcpy(filename1, line + 2);
            else
	       *filename1 = '\0';
	    break;
         case 'O':
            if (s = strrchr(line, ':'))
               *s = '\0';
            if (s = strrchr(line, '\''))
             {
               s++;
               for ( ; *s == ' '; s++)
                  ;
             }
            strcpy(filename, s);
	    if (*s == '\0')
	     {
	       if (s = strstr(line, "-T"))
		{
		  int i = 0;
		  s += 2;
		  while (1)
		   {
		     if (*s == ' ')
		      {
			if (i == 0)
			 {
                           i++;
			   break;
			 }
			else 
			 {
			   i--;
			   if (filename[i] != '\\')
			    {
			      i++;
			      break;
			    }
			 }
		      }
                     filename[i++] = *s;
		     s++;
		   }
                 filename[i] = '\0';
		}
	     }
	    break;
         case 'T':
	    if (s = strstr(line, "\\n"))
	       *s = '\0';
	    strcpy(title, line + 2);
	    break;
	 }
       }
      if (found)
       {
         line_ct = 1;
         while (line_ct)
          {
	    fgets(buf, 100, req);
	    line = strtok(buf, "\n");
	    switch (line_ct)
	    {
	    case 3:
	       if (s = strchr(line, '!'))
                {
		  *s = '\0';
		  s++;
                  strcpy(owner, s); 
                  strcat(owner, "@"); 
                  strcat(owner, line); 
		  if (s = strchr(owner, '.'))
		     *s = '\0';
                }
               else
                  strcpy(owner, line); 
	       break;
	    case 5: strcpy(job_size, line); break;
	    case 6: 
               secs = (time_t)atoi(line);
	       strftime(date_str, 100, "%b %e|%T", localtime(&secs));
	       line_ct = -1;
	       break;
	    }
	    line_ct++;
          }
         if (s = strstr(dir_struct->d_name, "-0"))
            *s = '\0';
         if (*filename)
            jobname = filename;
         else if (*filename1)
            jobname = filename1;
         else if (*title)
            jobname = title;
         else
          {
            sprintf(filename, "%s-%s", printer, dir_struct->d_name);
            jobname = filename;
          }
	 if (*n_jobs >= prev_n_jobs)
	  {
            if (*n_jobs == 0)
               *job_list = (StatusLineList) malloc(sizeof(StatusLine));
	    else
               *job_list = (StatusLineList) realloc(*job_list, (*n_jobs + 1) *
						    sizeof(StatusLine));
            j_list = *job_list;
	    j_list[*n_jobs] = (StatusLine) malloc(sizeof(StatusLineStruct));
	    len = 7 + strlen(printer) + strlen(jobname) + strlen(date_str) +
		  strlen(dir_struct->d_name) + strlen(owner) + strlen(job_size);
	    j_list[*n_jobs]->line = (char *) malloc(len);
	  }
	 else
	  {
	    j_list = *job_list;
	    len = 7 + strlen(printer) + strlen(jobname) + strlen(date_str) +
		  strlen(dir_struct->d_name) + strlen(owner) + strlen(job_size);
	    j_list[*n_jobs]->line = (char *)realloc(j_list[*n_jobs]->line, len);
	  }
         sprintf(j_list[*n_jobs]->line, "%s|%s|%s|%s|%s|%s\n", printer,
		 jobname, dir_struct->d_name, owner, date_str, job_size);
	 j_list[*n_jobs]->secs = (long) secs;
	 j_list[*n_jobs]->immediate = immediate;
	 (*n_jobs)++;
       }
      fclose(req);
      fclose(job);
    }
   closedir(lp_tmp_dir);
   delete [] filename;
   delete [] filename1;
   delete [] request;
   delete [] spool_dir;
}

static int SortJobs(const void *_first, const void *_second)
{
   StatusLineList first = (StatusLineList)_first;
   StatusLineList second = (StatusLineList)_second;

   if ((**first).immediate && !(**second).immediate)
      return -1;
   if (!(**first).immediate && (**second).immediate)
      return 1;
   if ((**first).immediate)
      return (int)((**second).secs - (**first).secs);
   else
      return (int)((**first).secs - (**second).secs);
}

#endif // SUN and USL LOCAL PARSER

#ifdef hpux

/* HP LOCAL PARSER - have to parse the following

 Need to parse the following 2 forms of output:

 coseps-28           guest          priority 0  Aug  9 12:54 on coseps
	 test.ps                                  31160 bytes

 OR

 coseps-29           guest          priority 0  Aug  9 12:56 on hostname
	 (standard input)                         31160 bytes
*/

void LocalPrintJobs(char *printer, char **return_job_list, int *return_n_jobs)
{
   char *buf = new char[300];
   char *s, *s1, *c;
   char *qname;
   char *jname;
   char *jnumber;
   char *owner;
   char *month;
   char *day;
   char *stime;
   char *jsize;
   char *hostname;
   int current_size;
   int len;
   char *output;

   static char *job_list = NULL;
   static int prev_buf_size = 0;

   sprintf(buf, "lpstat -i -o%s", printer);
   Invoke *_thread = new Invoke(buf, &output);

   if (prev_buf_size == 0)
    {
      prev_buf_size = BUFSIZ;
      job_list = (char *)malloc(prev_buf_size);
    }

   current_size = 0;
   *job_list = '\0';
   *return_n_jobs = 0;
   s = output;
   if (s1 = strchr(s, '\n'))
      *s1++ = '\0';
   while (s && *s)
    {
      qname = strtok(s, "-");
      jnumber = strtok(NULL, " ");
      owner = strtok(NULL, " ");
      strtok(NULL, " ");
      strtok(NULL, " ");
      month = strtok(NULL, " ");
      day = strtok(NULL, " ");
      stime = strtok(NULL, " \n");
      strtok(NULL, " ");
      hostname = strtok(NULL, " \n");
      s = s1;
      if (s1 = strchr(s, '\n'))
         *s1++ = '\0';
      for (jname = s; *jname == '\t' || *jname == ' '; jname++)
	 ;
      jsize = strrchr(s, ' ');
      *jsize = '\0';
      jsize = strrchr(s, ' ') + 1;
      for (c = jsize - 1; *c == ' '; c--)
	 ;
      *(c + 1) = '\0';
      if (hostname && strcmp(qname, hostname))
         sprintf(buf, "%s|%s|%s|%s@%s|%s %s|%s|%s\n", qname, jname, jnumber,
                 owner, hostname, month, day, stime, jsize);
      else
         sprintf(buf, "%s|%s|%s|%s|%s %s|%s|%s\n", qname, jname, jnumber,
                 owner, month, day, stime, jsize);

      len = strlen(buf);
      if (prev_buf_size < (current_size + len + 1))
         job_list = (char *) realloc(job_list, (current_size + len + 1) *
				   sizeof(char *));
      memcpy(job_list + current_size, buf, len);
      current_size += len;
      (*return_n_jobs)++;
      s = s1;
      if (s1 = strchr(s, '\n'))
         *s1++ = '\0';
    }
   *(job_list + current_size) = '\0';
   prev_buf_size = prev_buf_size > current_size ? prev_buf_size : current_size;
   *return_job_list = job_list;
   delete [] buf;
   delete output;
   delete _thread;
}
#endif // HP LOCAL PARSER

#if defined(__osf__) || defined(linux) || defined(CSRG_BASED)
//
// DEC/OSF1 PARSER - Output from "lpstat -o<printer>
//
// Remote:
// 
// everest.unx.dec.com: Tue Nov 29 18:44:33 1994:
// lps20 is ready and printing via network
// Rank   Owner      Job  Files                                 Total Size
// active vsp        231  /etc/printcap                         845 bytes
// Requests on lps20:
// 
// Local:
// 
// Wed Nov 30 12:20:24 1994: Attempting to print dfA000globe.unx.dec.com
// Rank   Pri Owner      Job  Files                                 Total Size
// active 0   root       0    /etc/printcap                        407 bytes
// Requests on la75:
// 

//
// DEC/OSF1 PARSER - Output from "lpq -p<printer>
//
// No Jobs in progress:
//
//  build-dec.qte.x.org: Thu Oct  3 11:17:18 1996: 
//  no entries
//
//
// Jobs in progress:
//   build-dec.qte.x.org: Fri Oct  4 09:08:48 1996: 
//   federal is ready and printing via dev
//   Rank   Owner      Job  Files                                 Total Size
//   active mgreess     65  (standard input)                      13482 bytes
//   1st    mgreess     66  (standard input)                      13482 bytes
//   2nd    mgreess     67  (standard input)                      13482 bytes
//


//
// DEC/OSF1 V4.0 386 alpha PARSER - Output from "lpstat -o<printer>
//
// No Jobs in progress:
//
// Requests on ansel:
// build-dec.osf.org: Wed Jul  2 11:29:53 1997: 
// no entries
//
//
//
// Jobs in progress:
//
// Requests on ansel:
// build-dec.osf.org: Wed Jul  2 11:31:12 1997: 
// Rank   Pri Owner      Job  Files                                 Total Size
// 1st    0   mgreess    7    ParseJobs.C                          33367 bytes
// no entries
//
//


#define NEXT_OUTPUT_LINE(line,rest) \
do \
{ \
  line = rest; \
  if (line && (rest = strchr(line, '\n'))) \
    *rest++ = '\0'; \
} while (NULL != line && 0 == *line);

void LocalPrintJobs(char *printer, char **return_job_list, int *return_n_jobs)
{
   char *buf = new char[1000];
   char *s, *s1, *c;
   char *qname = NULL;
   char *jname;
   char *jnumber;
   char *owner;
   char *month;
   char *day;
   char *date;
   char *year;
   char *network;
   char *output;
   char *stime;
   char *jsize;
   char *hostname;
   int current_size;
   int len;
   int has_pri = 0;

   static char *job_list = NULL;
   static int prev_buf_size = 0;

#if defined(__osf__)
   sprintf(buf, "lpstat -o%s", printer);
#endif
#if defined(linux) || defined(CSRG_BASED)
   sprintf(buf, "lpq -P%s", printer);
#endif

   Invoke *_thread = new Invoke(buf, &output);

   if (prev_buf_size == 0)
   {
      prev_buf_size = BUFSIZ;
      job_list = (char*) malloc(prev_buf_size);
   }

   current_size = 0;
   *job_list = '\0';
   *return_n_jobs = 0;

   s1 = output;
   NEXT_OUTPUT_LINE(s,s1);

   //
   // Parse the optional "Requests on" line of output to verify printer name.
   //   "Requests on ansel:"
   //
   {
       char *opt_requests_on_line = "Requests on ";
       if (0 == strncmp(s, opt_requests_on_line, strlen(opt_requests_on_line)))
       {
           char *requests	= strtok(s, " ");
           char *on		= strtok(NULL, " ");

           qname = strtok(NULL, " :\n");
           if (0 != strncmp(qname, printer, strlen(qname)))
	     fprintf(
		stderr,
		"Found the wrong printer:  looking for '%s'; found '%s'\n",
		printer, qname);

   	   NEXT_OUTPUT_LINE(s,s1);
       }
   }

   //
   // Parse the host/date line of output to collect host, date, and time.
   //   "build-dec.qte.x.org: Fri Oct  4 09:08:48 1996: "
   //
   {
       hostname = strtok(s, ":");
       day = strtok(NULL, " ");
       month = strtok(NULL, " ");
       date = strtok(NULL, " ");
       stime = strtok(NULL, " ");
       year = strtok(NULL, " \n");
   }

   NEXT_OUTPUT_LINE(s,s1);

   //
   // If this is the "no entries" line, we're done.  Return.
   //   "no entries"
   //
   {
       char *no_entries_line = "no entries";
       if (0 == strncmp(s, no_entries_line, strlen(no_entries_line)))
       {
           *return_job_list = job_list;
           free(output);
	   return;
       }
   }

   //
   // Parse the optional printer status line.
   //   "federal is ready and printing via dev"
   //
   {
       char *buffer = new char[128];
       char *opt_status_line = buffer;

       sprintf(buffer, "%s is ready and printing via ", printer);
       if (0 == strncmp(s, opt_status_line, strlen(opt_status_line)))
       {
           qname = strtok(s, " \t");
           strtok(NULL, " ");
           strtok(NULL, " ");
           strtok(NULL, " ");
           strtok(NULL, " ");
           strtok(NULL, " ");
           network = strtok(s, " \n");

           NEXT_OUTPUT_LINE(s,s1);
       }

       delete [] buffer;
   }

   //
   // Parse the legend line.
   //   "Rank Owner Job Files Total Size"
   //   or
   //   "Rank Pri Owner Job Files Total Size"
   //
   {
       char *tmp;

       tmp = strtok(s, " ");
       if (strcmp(tmp, "Rank"))
         fprintf(stderr, "Unexpected legend column: %s != Rank\n", tmp);

       tmp = strtok(NULL, " ");
       if (0 == strcmp(tmp, "Pri"))
       {
           has_pri = TRUE;
           tmp = strtok(NULL, " ");
       }
       if (strcmp(tmp, "Owner"))
         fprintf(stderr, "Unexpected legend column: %s != Owner\n", tmp);
       
       tmp = strtok(NULL, " ");
       if (strcmp(tmp, "Job"))
         fprintf(stderr, "Unexpected legend column: %s != Job\n", tmp);
       
       tmp = strtok(NULL, " ");
       if (strcmp(tmp, "Files"))
         fprintf(stderr, "Unexpected legend column: %s != Files\n", tmp);
       
       tmp = strtok(NULL, " ");
       if (strcmp(tmp, "Total"))
         fprintf(stderr, "Unexpected legend column: %s != Total\n", tmp);
       
       tmp = strtok(NULL, " \n");
       if (strcmp(tmp, "Size"))
         fprintf(stderr, "Unexpected legend column: %s != Size\n", tmp);
   }


   NEXT_OUTPUT_LINE(s,s1);

   //
   // Parse the job lines.
   //   "active mgreess    65  (standard input)                   13482 bytes"
   //   or
   //   "1st 0  mgreess    7   ParseJobs.C                        33367 bytes"
   //
   while (s && *s)
   {
      strtok(s, " \t");
      if (has_pri)
        strtok(NULL, " ");
      owner = strtok(NULL, " ");
      jnumber = strtok(NULL, " ");
      jname = strtok(NULL," ");
      jsize = strtok(NULL," ");

      if (NULL == owner || NULL == jnumber || NULL == jname || NULL == jsize)
      {
          NEXT_OUTPUT_LINE(s,s1);
	  continue;
      }

      if (NULL == qname)
	qname = printer;

      if (hostname && strcmp(qname, hostname))
         sprintf(buf, "%s|%s|%s|%s@%s|%s %s|%s|%s\n", qname, jname, jnumber,
                 owner, hostname, month, date, stime, jsize);
      else
         sprintf(buf, "%s|%s|%s|%s|%s %s|%s|%s\n", qname, jname, jnumber,
                 owner, month, date, stime, jsize);

      printf("qname, jname, jnumber, owner, ");
      printf("hostname, month, date, stime, jsize\n");
      printf("%s\n", buf);

      len = strlen(buf);
      if (prev_buf_size < (current_size + len + 1))
        job_list =
	    (char*) realloc(job_list, (current_size + len + 1) * sizeof(char*));
      memcpy(job_list + current_size, buf, len);
      current_size += len;
      (*return_n_jobs)++;

      NEXT_OUTPUT_LINE(s,s1);
   }

   *(job_list + current_size) = '\0';
   prev_buf_size = prev_buf_size > current_size ? prev_buf_size : current_size;
   *return_job_list = job_list;
   free(output);
   delete [] buf;
}
#endif        // DEC local parser


/* BSD PARSER - have to parse the following

BSD, HP, and SUN output
Warning: test1 is down: b906ps3 is ready and printing
Warning: no daemon present

root: 1st                                [job 006b906ps3.austin.ibm.com]
	/etc/motd                        421 bytes
root: 2nd                                [job 006b906ps3.austin.ibm.com]
	<File name not available>        421 bytes
*/

int ParseBSDPrintJobs(char *printer, char *jobs,
                      char **return_job_list, int *return_n_jobs)
{
   char *s, *s1;
   char *jname;
   char jnumber[9];
   char *owner;
   char *sdate;
   char *stime;
   char *jsize;
   char *hostname;
   int current_size;
   int len;
   char *buf = new char[300];
   static char *job_list = NULL;
   static int prev_buf_size = 0;
   static int first_time = 1;
   int rc = 1;

#ifdef NO_REGCOMP
   static char *bsd_pattern;
   /*
    * Due to bug in regcmp/regex (bug id 1191772), change to C locale for
    * this parsing work.  And change back at end of procedure.  This workaround
    * appears to be ok since the print service output parsed here is in a
    * "C locale" 8-bit form for inter-machine networking.
    */
   setlocale(LC_ALL, "C"); 

   if (first_time)
    {
      bsd_pattern = regcmp(":.*\\[.*\\]", (char *)NULL);
      first_time = 0;
    }
#else
   static regex_t bsd_pattern;

   if (first_time)
    {
      regcomp(&bsd_pattern, ":.*\\[.*\\]", REG_NOSUB);
      first_time = 0;
    }
#endif

   if (prev_buf_size == 0)
    {
      prev_buf_size = BUFSIZ;
      job_list = (char *)malloc(prev_buf_size);
    }

   current_size = 0;
   *job_list = '\0';
   *return_n_jobs = 0;
   sdate = MESSAGE(NotAvailableL);
   stime = sdate;

   s = strtok(jobs, "\n");
   while (s)
    {
#ifdef NO_REGCOMP
      if (regex(bsd_pattern, s))
#else
      if (!regexec(&bsd_pattern, s, (size_t)0, NULL, 0))
#endif
       {
	 owner = s;
	 s = strchr(s, ':');
	 *s++ = '\0';
	 s = strchr(s, '[');
	 s1 = jnumber;
	 for (s = strchr(s, ' ') + 1; isdigit((int)*s); s++)
	    *s1++ = *s;
	 *s1++ = '\0';
	 for (s1 = s; *s1 == ' '; s1++)
	    ;
	 hostname = s1;
	 if (s = strchr(hostname, '.'))
	    *s = '\0';
	 else
	    *(strchr(hostname, ']')) = '\0';

         s = strtok(NULL, "\n");
	 for (s1 = s; *s1 == ' ' || *s1 == '\t'; s1++)
	    ;
	 jname = s1;
	 s1 = strrchr(s, ' ');
	 *s1-- = '\0';
	 s1 = strrchr(s, ' ');
	 jsize = s1 + 1;
	 for ( ; *s1 == ' '; s1--)
	    ;
         *(s1 + 1) = '\0';
         sprintf(buf, "%s|%s|%s|%s@%s|%s|%s|%s\n", printer, jname, jnumber,
	         owner, hostname, sdate, stime, jsize);
         len = strlen(buf);
         if (prev_buf_size < (current_size + len + 1))
            job_list = (char *) realloc(job_list, (current_size + len + 1) *
				      sizeof(char *));
         memcpy(job_list + current_size, buf, len);
         current_size += len;
         (*return_n_jobs)++;
       }
      else
       {
	 if (strstr(s, "down"))
	    rc = 0;
       }
      s = strtok(NULL, "\n");
    }
   *(job_list + current_size) = '\0';
   prev_buf_size = prev_buf_size > current_size ? prev_buf_size : current_size;
   *return_job_list = job_list;

#ifdef NO_REGCOMP
   setlocale(LC_ALL, ""); 
#endif

   delete [] buf;
   return rc;
}

/* AIX version 3 and 4 PARSER - have to parse the following 

 Queue   Dev   Status    Job     Name           From           To            
                         Submitted        Rnk Pri       Blks  Cp          PP %
 ------- ----- --------- ---------        --- ---      ----- ---        ---- --
 bsh     bshde RUNNING   956     STDIN.14937    root           root          
                        08/02/94 09:55:44    1  15          1   1           0  0
                               /var/spool/qdaemon/tOlkCSM

 OR 

 Queue   Dev   Status    Job     Name           From           To            
                         Submitted        Rnk Pri       Blks  Cp          PP %
 ------- ----- --------- ---------        --- ---      ----- ---        ---- --
 john1   lp0   DOWN     
               QUEUED    957     STDIN.14952    root           root          
                        08/02/94 10:17:53    1  15          1   1               
                               /var/spool/qdaemon/tOmgCrx


Also need to parse filenames that are titles: Example, "My Report" below.

               QUEUED    957     My Report      root           root          
                        08/02/94 10:17:53    1  15          1   1               
                               /var/spool/qdaemon/tOmgCrx

*/

int ParseAIXv3PrintJobs(char *_printer, char *jobs,
                        char **return_job_list, int *return_n_jobs)
{
   char *buf = new char[300];
   struct stat statbuff;
   char *s, *s1, *c;
   char *jname;
   char *jnumber;
   char *owner;
   char *sdate;
   char *stime;
   char *jsize;
   char *dollar1;
   int current_size;
   int len;
   char printer[60];
   int rc = 1;

   static char *job_list = NULL;
   static int prev_buf_size = 0;

   strcpy(printer, _printer);
   if (s = strchr(printer, ':'))
      *s = '\0';
	
   if (prev_buf_size == 0)
    {
      prev_buf_size = BUFSIZ;
      job_list = (char *)malloc(prev_buf_size);
    }

   current_size = 0;
   *job_list = '\0';
   *return_n_jobs = 0;

   s = jobs;
   s1 = strchr(s, '\n');
   s = s1 + 1;
   s1 = strchr(s, '\n');
   s = s1 + 1;
   s1 = strchr(s, '\n');
   s = s1 + 1;
   if (s1 = strchr(s, '\n'))
      *s1++ = '\0';
   if (strstr(s, "DOWN"))
      rc = 0;
   while (s && *s)
    {
      dollar1 = strtok(s, " ");
      if (!strncmp(dollar1, printer, strlen(dollar1)))
       {
         strtok(NULL, " ");
         strtok(NULL, " ");
       }
      jnumber = strtok(NULL, " ");
      if (!(jnumber && *jnumber))
       {
         s = s1;
         if (s1 = strchr(s, '\n'))
            *s1++ = '\0';
	 continue;
       }
      jname = strtok(NULL, "\n");
      if (!(jname && *jname))
       {
         s = s1;
         if (s1 = strchr(s, '\n'))
            *s1++ = '\0';
	 continue;
       }
      for (c = jname + strlen(jname) - 1; *c == ' '; c--)
	 ;
      for ( ; *c != ' '; c--)
	 ;
      for ( ; *c == ' '; c--)
	 ;
      *(c + 1) = '\0';
      for ( ; *c != ' '; c--)
	 ;
      owner = c + 1;
      for ( ; *c == ' '; c--)
	 ;
      *(c + 1) = '\0';
      for (c = jname; *c == ' '; c++)
	 ;
      jname = c;
      s = s1;
      if (s1 = strchr(s, '\n'))
         *s1++ = '\0';
      sdate = strtok(s, " ");
      stime = strtok(NULL, " ");
      strtok(NULL, " ");
      strtok(NULL, " ");
      jsize = strtok(NULL, " ");
      s = s1;
      if (s1 = strchr(s, '\n'))
         *s1++ = '\0';
      c = strtok(s, " \n");
      if (strstr(c, jname))
	 jname = c;
      if (!G_AIX_LOCAL)
         statbuff.st_size = atoi(jsize) * 512;
      else if (stat(c, &statbuff) < 0)
         statbuff.st_size = atoi(jsize) * 512;
      strtok(owner, ".");
      sprintf(buf, "%s|%s|%s|%s|%s|%s|%d\n", printer, jname, jnumber,
	      owner, sdate, stime, (int)statbuff.st_size);
      len = strlen(buf);
      if (prev_buf_size < (current_size + len + 1))
         job_list = (char *) realloc(job_list, (current_size + len + 1) *
				   sizeof(char *));
      memcpy(job_list + current_size, buf, len);
      current_size += len;
      (*return_n_jobs)++;
      s = s1;
      if (s1 = strchr(s, '\n'))
         *s1++ = '\0';
    }
   *(job_list + current_size) = '\0';
   prev_buf_size = prev_buf_size > current_size ? prev_buf_size : current_size;
   *return_job_list = job_list;
   delete [] buf;
   return rc;
}

/* AIX version 2 PARSER - have to parse the following

dev  arg         status      request               pp output  % done
---  ----------  ----------  --------------------  ---------  ------
bp0  b906ps5     READY     
bp1  s906ps5     READY     
bp2  lp0         READY     
ts1  t1          OFF       
ts2  t2          OFF       

queue  user               request              blks cops pri  time   to
-----  -----------------  -------------------- ---- ---- ---  -----  --------
ts1    root               /etc/motd               2    1  15  15:57  root    
ts1    root@warpspeed     /etc/rc.afs             3    1  15  16:03  root@war
ts2    root               /.profile               1    1  15  16:05  root    
ts2    root               /.profile               1    1  15  16:06  root    
ts2    root@warpspeed     /etc/rc.afs             3    1  15  16:07  root@war

*/

int ParseAIXv2PrintJobs(char *printer, char *jobs,
                        char **return_job_list, int *return_n_jobs)
{
   char *buf = new char[300];
   char *s, *s1, *c;
   char *jname;
   char *jnumber;
   char *owner;
   char *sdate;
   char *stime;
   char *jsize;
   char *device;
   char *tmp;
   int current_size;
   int len;
   int rc = 1;

   static char *job_list = NULL;
   static int prev_buf_size = 0;

   if (prev_buf_size == 0)
    {
      prev_buf_size = BUFSIZ;
      job_list = (char *)malloc(prev_buf_size);
    }

   current_size = 0;
   *job_list = '\0';
   *return_n_jobs = 0;
   sdate = MESSAGE(NotAvailableL);
   jnumber = sdate;

   s = jobs;
   s1 = strchr(s, '\n');
   s = s1 + 1;
   s1 = strchr(s, '\n');
   s = s1 + 1;
   if (s1 = strchr(s, '\n'))
      *s1++ = '\0';
   while (s)
    {
      device = strtok(s, " ");
      tmp = strtok(NULL, " ");
      c = strtok(NULL, " ");
      if (!strcmp(tmp, printer))
       {
	 if (strstr(c, "OFF"))
	    rc = 0;
	 break;
       }
      s = s1;
      if (s1 = strchr(s, '\n'))
         *s1++ = '\0';
    }
   s = s1;
   if (s1 = strchr(s, '\n'))
      *s1++ = '\0';
   while (s)
    {
      tmp = strtok(s, " ");
      if (!tmp || strcmp(tmp, device))
       {
         s = s1;
         if (s1)
	    if (s1 = strchr(s, '\n'))
               *s1++ = '\0';
	 continue;
       }
      owner = strtok(NULL, " ");
      jname = strtok(NULL, " ");
      jsize = strtok(NULL, " ");
      strtok(NULL, " ");
      strtok(NULL, " ");
      stime = strtok(NULL, " ");
      if (c = strchr(owner, '.'))
         *c = '\0';
      sprintf(buf, "%s|%s|%s|%s|%s|%s|%d\n", printer, jname, jnumber,
	      owner, sdate, stime, atoi(jsize) * 512);
      len = strlen(buf);
      if (prev_buf_size < (current_size + len + 1))
         job_list = (char *) realloc(job_list, (current_size + len + 1) *
				   sizeof(char *));
      memcpy(job_list + current_size, buf, len);
      current_size += len;
      (*return_n_jobs)++;
      s = s1;
      if (s1 = strchr(s, '\n'))
         *s1++ = '\0';
    }
   *(job_list + current_size) = '\0';
   prev_buf_size = prev_buf_size > current_size ? prev_buf_size : current_size;
   *return_job_list = job_list;
   delete [] buf;
   return rc;
}
