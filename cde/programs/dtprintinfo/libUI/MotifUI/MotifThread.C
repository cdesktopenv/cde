/* $TOG: MotifThread.C /main/3 1998/07/24 16:15:54 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "MotifThread.h"

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>


MotifThread::MotifThread(MotifUI *obj, const char *cmd,
                         MotifThreadCallback cb, ThreadCallback cb1,
			 int buf_len)
{
   CreateThread(obj, cmd, -1, -1, cb, cb1, buf_len);
}

MotifThread::MotifThread(MotifUI *obj, int pid, int fd,
                         MotifThreadCallback cb, ThreadCallback cb1,
			 int buf_len)
{
   CreateThread(obj, NULL, pid, fd, cb, cb1, buf_len);
}

MotifThread::MotifThread(MotifUI *obj, int socket,
                         MotifThreadCallback cb, ThreadCallback cb1,
			 int buf_len)
{
   CreateThread(obj, NULL, -1, socket, cb, cb1, buf_len);
}

void MotifThread::CreateThread(MotifUI *_obj, const char *cmd, int _pid,
			       int _fd, MotifThreadCallback _cb,
			       ThreadCallback _cb1, int _buf_len)
{
   int m_stdout[2];

   cb = _cb;
   cb1 = _cb1;
   obj = _obj;
   output = NULL;
   out1 = NULL;
   inputID = 0;
   if (_buf_len < 0)
      buf_len = 512;
   else
      buf_len = _buf_len;
   if (cmd)
    {
      fd = -1;
      if (pipe(m_stdout) < 0)
       {
         output = strdup(strerror(errno));
         status = -1;
         Halt();
         return;
       }

      if ((pid = fork()) == 0)  // In Child
       {
         close(m_stdout[0]);
         close(1);
         dup(m_stdout[1]);
         close(m_stdout[1]);

         execlp("/bin/ksh", "ksh", "-c", cmd, NULL);

         char *msg = strerror(errno);
         write(1, msg, strlen(msg));
         exit(-1);
       } 
      else if (pid == -1)
       {
         close(m_stdout[0]);
         output = strdup(strerror(errno));
         status = -1;
         Halt();
         return;
       }

      close(m_stdout[1]);
      fd = m_stdout[0];
    }
   else
    {
      pid = (pid_t)_pid;
      fd = _fd;
    }
   fcntl(fd, F_SETFL, O_NDELAY);

   len = buf_len;
   ctr = 1;
   output = (char *)malloc(buf_len);
   out1 = output;
   out2 = out1 + buf_len - 1;

   long mask = XtInputReadMask | XtInputExceptMask;
   inputID = XtAppAddInput(obj->appContext, fd, (XtPointer)mask,
			   &(MotifThread::GetOutputCB), this);
}

MotifThread::~MotifThread()
{
   if (fd >= 0)
      Halt();
   if (inputID)
      XtRemoveInput(inputID);
   free(output);
}

void MotifThread::Halt()
{
   if (out1)
      *out1 = '\0';
   close(fd);
   fd = -1;
   if (pid != -1)
    {
      pid_t w;
      while ((w = wait(&status)) != pid && w != -1);
      status = (status >> 8) & 0xFF;
    }
   else
      status = 0;
   (*cb)(this, obj, cb1);
}

void MotifThread::GetOutput()
{
   int n = read(fd, out1, len);
   if (n == len)
    {
      len = buf_len;
      ctr++;
      output = (char *)realloc(output, (ctr * len)); 
      out1 = output + ((ctr - 1) * len);
      out2 = out1 + len - 1;
    }
   else if (n > 0)
    {
      out1 += n;
      len = out2 - out1 + 1;
    }
   else
     Halt();
}

void MotifThread::GetOutputCB(XtPointer closure, int * /*fid*/,
			      XtInputId * /*id*/)
{
   MotifThread *obj = (MotifThread *)closure;
   obj->GetOutput();
}
