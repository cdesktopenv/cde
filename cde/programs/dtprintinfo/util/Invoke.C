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
/* $TOG: Invoke.C /main/7 1997/07/30 15:42:39 samborn $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "Invoke.h"

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#if !defined(CSRG_BASED)
#include <values.h>
#endif
#ifdef _AIX
#include <strings.h>		/* need to get bzero defined */
#endif /* _AIX */

const int BUFFER_SIZE = 512;

Invoke::Invoke(const char *command,   // Command to Run
               char **out_ptr,        // ptr to output buffer ptr 
               char **err_ptr,        // ptr to error buffer ptr
               uid_t _uid)            // run command as this UID
{
   int m_stdout[2], m_stderr[2];       // progname file descriptors
   pid_t c_pid;                        // child's pid 
   pid_t w;                            // temp vars 
   int out_num,err_num;                // # of chars read 
   char *out_tmp, *err_tmp;            // temp buffer ptrs 
   char *out_end,*err_end;             // ptr to end of buffer 
   int outb_size,errb_size;            // buffer size 
   int out_count, err_count;           // # of buffers allocated 
   int trap_out,trap_err;              // flags; if >0, trap output 
   fd_set rdmask;                      // for select system call 
   fd_set wrmask;                      // for select system call 
   fd_set exmask;                      // for select system call 
   int Nfdsmsgs;

   struct sigaction action;            // parameters of sigaction 
   struct sigaction oldsigint_act;
   struct sigaction oldsigquit_act;

   status = 0;

   trap_out = (out_ptr != NULL);
   trap_err = (err_ptr != NULL);

   // initialize internal variables
   out_num = err_num = 0;

   // setup pipes if specified
   if (trap_out)
    {
      *out_ptr = 0;

      if (pipe(m_stdout) < 0)
       {
         status = -1;
         return;
       }
    }

   if (trap_err)
    {
      *err_ptr = 0;

      if (pipe(m_stderr) < 0)
       {
         if (trap_out)
            close(m_stdout[0]);
         status = -1;
         return;
       }
    }
   if (trap_err)
      Nfdsmsgs = m_stderr[0] + 1;
   else if (trap_out)
      Nfdsmsgs = m_stdout[0] + 1;
   else
      Nfdsmsgs = 0;

   // ignore these signals
   memset(&action, '\0', sizeof (struct sigaction));
   memset(&oldsigquit_act, '\0', sizeof (struct sigaction));
   memset(&oldsigint_act, '\0', sizeof (struct sigaction));

#if defined(__OSF1__) || defined(__osf__) 
   action.sa_handler = (void (*)(int))SIG_IGN;
#elif defined(USL) || defined(__uxp__) || \
      ( defined(sun) && OSMAJORVERSION == 5 && OSMINORVERSION <= 4)
   action.sa_handler = (void (*)())SIG_IGN;
#else
   action.sa_handler = SIG_IGN;
#endif

   sigaction(SIGINT, &action, &oldsigint_act);
   sigaction(SIGQUIT, &action, &oldsigquit_act);

   if ((c_pid = fork()) == 0)
    { // ------------------------ child process --------------------------

      if (_uid != (uid_t)-1)
	 setuid(_uid);

      if (trap_out)
       { // duplicate stdout
         close(m_stdout[0]);
         close(1);
         dup(m_stdout[1]);
         close(m_stdout[1]);
       }

      if (trap_err)
       { // duplicate stderr
         close(m_stderr[0]);
         close(2);
         dup(m_stderr[1]);
         close(m_stderr[1]);
       }

      // start the program 
      execlp("/bin/ksh", "ksh", "-c", command, (char *) 0);

      exit(-1);
    }
   else if (c_pid == -1)
    {
      if (trap_err)
         close(m_stderr[0]);

      if (trap_out)
         close(m_stdout[0]);

      status = -1;
      return;
    }

   // -------------------------- parent process --------------------------

   // restore signals
   sigaction(SIGINT, &oldsigint_act, NULL);
   sigaction(SIGQUIT, &oldsigquit_act, NULL);

   // close the write side of the pipe for the parent
   if (trap_out)
    {
      close(m_stdout[1]);
      fcntl(m_stdout[0], F_SETFL, O_NDELAY);
    }

   if (trap_err)
    {
      close(m_stderr[1]);
      fcntl(m_stderr[0], F_SETFL, O_NDELAY);
    }

   if (!trap_out && !trap_err)
    { // no piped output 
      // wait for the child to die
      while ((w = wait(&status)) != c_pid && w != -1)
	 ;
      status = (status >> 8) & 0xFF;
      return;
    }

   // initialize buffer pointers
   if (trap_out)
    {
      *out_ptr = (char *) malloc(BUFFER_SIZE);
      if (*out_ptr == NULL)
       {
         close(m_stdout[0]);
         if (trap_err)
            close(m_stderr[0]);
         status = -1;
         return;
       }

      out_tmp = *out_ptr;
      out_end = *out_ptr + BUFFER_SIZE - 1;
      out_count = 1;
      outb_size = BUFFER_SIZE;
    }

   if (trap_err)
    {
      *err_ptr = (char *) malloc(BUFFER_SIZE);
      if (*err_ptr == NULL)
       {
         close(m_stderr[0]);
         if (trap_out)
            close(m_stdout[0]);

         status = -1;
         return;
       }

      *err_ptr = (char *) malloc(BUFFER_SIZE);
      err_tmp = *err_ptr;
      err_end = *err_ptr + BUFFER_SIZE - 1;
      err_count = 1;
      errb_size = BUFFER_SIZE;
    }

   while (trap_out || trap_err)
    {
      // reset the file descriptor masks
      FD_ZERO(&rdmask);
      FD_ZERO(&wrmask);
      FD_ZERO(&exmask);

      // set the bit masks for the descriptors to be checked
      if (trap_out)
         FD_SET(m_stdout[0], &rdmask);
      if (trap_err)
         FD_SET(m_stderr[0], &rdmask);

      // check the status
      if (select(Nfdsmsgs,&rdmask,&wrmask,&exmask,(struct timeval *)NULL) == -1)
       {
         if (errno == EINTR)
            continue;
         else
          {
            if (trap_out)
               close(m_stdout[0]);
            if (trap_err)
               close(m_stderr[0]);

            status = -1;
            return;
          }
       }
      if (trap_out && FD_ISSET(m_stdout[0], &rdmask))

       {
         // read the child's stdout
         if ((out_num = read(m_stdout[0], out_tmp, outb_size)) < 0)
          {
            close(m_stdout[0]);
            if (trap_err)
               close(m_stderr[0]);

            status = -1;
            return;
          }

         if (out_num == 0)
          {
            // no more to read
            trap_out = 0;
            close(m_stdout[0]);
            *out_tmp = '\0';
          }
         else if (out_num == outb_size)
          {
            // filled up a buffer; allocate another one
            out_count++;
            *out_ptr = (char *)realloc(*out_ptr, (out_count * BUFFER_SIZE));
            if (*out_ptr == NULL)
             {
               close(m_stdout[0]);
               if (trap_err)
                  close(m_stderr[0]);
               status = -1;
               return;
             }

            out_tmp = *out_ptr + ((out_count - 1) * BUFFER_SIZE);
            out_end = out_tmp + BUFFER_SIZE - 1;
            outb_size = BUFFER_SIZE;
	  }
         else if (out_num > 0)
          {
            // read less than a full buffer; reset amount to read next
            out_tmp += out_num;
            outb_size = out_end - out_tmp + 1;
            outb_size = (outb_size > 0) ? outb_size : 0;
          }
       } // if trap_out

      if (trap_err && FD_ISSET(m_stderr[0], &rdmask))
       {
         // read the child's stderr
         if ((err_num = read(m_stderr[0], err_tmp, errb_size)) == -1)
          {
            if (trap_out)
               close(m_stdout[0]);

            close(m_stderr[0]);

            status = -1;
            return;
          }

         if (err_num == 0)
          {
            // no more to read
            trap_err = 0;
            close(m_stderr[0]);
            *err_tmp = '\0';
          }
         else if (err_num == errb_size)
          {
            // filled up a buffer; allocate another one
            err_count++;
            *err_ptr = (char *)realloc(*err_ptr, (err_count * BUFFER_SIZE));
            if (*err_ptr == NULL)
             {
               close(m_stderr[0]);
               if (trap_out)
                  close(m_stdout[0]);
               status = -1;
               return;
             }

            err_tmp = *err_ptr + ((err_count - 1) * BUFFER_SIZE);
            err_end = err_tmp + BUFFER_SIZE - 1;
            errb_size = BUFFER_SIZE;
          }
         else if (err_num > 0)
          {
            // read less than a full buffer; reset amount to read next
            err_tmp += err_num;
            errb_size = err_end - err_tmp + 1;
            errb_size = (errb_size > 0) ? errb_size : 0;
          }
       } // if trap_err
    } // while trap_out or trap_err

   while ((w = wait(&status)) != c_pid && w != -1);
   status = (status >> 8) & 0xFF;
}
