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
/*
 * $TOG: pty.c /main/10 1999/10/14 15:06:11 mgreess $
 * Language:     C
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1988,1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#define __need_fd_set

#if  defined(hpux) || defined(_hpux) || defined(__hpux) || defined(hp)
#define __hpux_pty
#endif

#ifdef __hpux_pty
#define __need_timeval  /* need struct timeval */
#endif

#include <bms/sbport.h> /* NOTE: sbport.h must be the first include. */
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>

#ifdef SVR4
#include <sys/filio.h>
#else
#include <sys/ioctl.h>
#endif

#ifdef __hpux_pty
#include <time.h>
#include <sys/ptyio.h>
#endif

#if defined(__cplusplus)
#include <osfcn.h>
#endif

#if defined(__apollo) && !defined(ONLCR) 
# define ONLCR 0		/* This guy (XPG3), not on apollo yet */
#endif

#ifdef __bsd
#include <sys/file.h>
#include <sgtty.h>
#endif


/*
#ifdef __sun
#include <sys/ttycom.h>
#endif 
*/

#include <SPC/spcP.h>
#include "DtSvcLock.h"

/* External declarations */

/*
 * Pseudo-Terminal device file definitions:
 * Switch _1 and _2 around if BSD or SYSV conventions are more likely
 */

#if defined (SVR4) || defined(AIX)
#define MPREFIX_1	"/dev/pty"	/* BSD convention ?? */
#define SPREFIX_1	"/dev/tty"

#define MPREFIX_2	"/dev/ptym/pty"	/* AT&T convention ?? */
#define SPREFIX_2	"/dev/pty/tty"

#else
#define MPREFIX_1	"/dev/ptym/pty"	/* AT&T convention ?? */
#define SPREFIX_1	"/dev/pty/tty"

#define MPREFIX_2	"/dev/pty"	/* BSD convention ?? */
#define SPREFIX_2	"/dev/tty"
#endif

#ifdef HAVE_PTMS
#include <sys/ptms.h>
static char *MASTER_PATH = "/dev/ptmx";
#endif

static char *MASTER_NAMES = "pqrstuvwabcefghijklmnoxyz";

#define SCANBITS(sfds,step)                                     \
   {int __i;                                                    \
    for(__i=0; __i<FD_SETSIZE; __i++)                           \
      if(FD_ISSET(__i, sfds))                                   \
	step(__i);                                              \
	}
    
#define IS_FD_SET(fdarr, res)                                   \
   {int __i;                                                    \
    res=0;                                                      \
    for(__i=0; __i<FD_SETSIZE; __i++)                           \
      if(FD_ISSET(__i, fdarr)) {                                \
	res = 1;                                                \
	break;                                                  \
      }}

static int send_eof_pty_channel_object(SPC_Channel_Ptr channel);

void pty_channel_class_init(object_clasp t)
{

  pty_channel_clasp c = (pty_channel_clasp) t;
  
  c->new_obj    = alloc_channel_object;
  
  c->open       = open_pty_channel_object;
  c->close      = close_local_channel_object;
  c->read       = read_pty_channel_object;
  c->write      = write_local_channel_object;
  c->reset      = reset_pty_channel_object;
  c->pre_fork   = pre_fork_pty_channel_object;
  c->post_fork  = post_fork_pty_channel_object;
  c->exec_proc  = exec_proc_local_channel_object;
  c->signal     = signal_local_channel_object;
  c->wait_for_termination=local_channel_object_wait_for_termination;
  c->attach     = attach_pty_channel_object;
  c->add_input  = add_input_pty_channel_object;
  c->input      = local_channel_object_input_handler;
  c->remove_logfile = remove_logfile_local_channel_object;

  /* New B.00 methods */

  c->send_eof = send_eof_pty_channel_object;
  c->set_termio = set_termio_pty_channel_object;
   
}

static struct pty_channel_class pty_channel_class_struct = {
  (channel_clasp) &channel_class, /* base class pointer */
  "pty_channel",           /* class name */
  pty_channel_class_init,  /* class initialize function */
  sizeof(SPC_Channel),      /* size */
  0
  };

pty_channel_clasp pty_channel_class = &pty_channel_class_struct;

/* Local variable */
static XeChar *hexdigits = "0123456789abcdef";

#ifdef __hpux_pty
/*----------------------------------------------------------------------+*/
static SPC_Disable_Trapping(int fd)
/*----------------------------------------------------------------------+*/
{
  int flag=0;
  int disable=0;
  struct request_info req_info;

  /* Disable trapping */
  ioctl(fd, TIOCTRAP, &disable);

  /* Just in case, flush any queued requests */
  
  while((ioctl(fd, TIOCTRAPSTATUS, &flag) != ERROR) && flag) {
    ioctl(fd, TIOCREQGET, &req_info);
    ioctl(fd, TIOCREQSET, &req_info);
  }
  return(TRUE);
}
#endif /* __hpux_pty */

/*
 * Routines for opening pty master/slave devices
 */

#ifdef HAVE_PTMS

/*----------------------------------------------------------------------+*/
static int getspec1170ptypair(Wire *wire)
/*----------------------------------------------------------------------+*/
{
  char *slaveName;
  struct sigaction newAction, oldAction;

  strcpy(wire->master_name, MASTER_PATH);

  if ((wire->fd[MASTER_SIDE] = open(wire->master_name, O_RDWR)) < OK)
  {
    /* open(master) failed. */
    return FALSE;
  }

  /* SIGCHLD handler, if any, must be disabled during grantpt! */
  sigaction(SIGCHLD, (struct sigaction *)NULL, &oldAction);
  if (oldAction.sa_handler != SIG_DFL)
  {
    newAction = oldAction;
    newAction.sa_handler = SIG_DFL;
    sigaction(SIGCHLD, &newAction, (struct sigaction *)NULL);
  }

  if (grantpt(wire->fd[MASTER_SIDE]) != OK)
  {
    /* cannot access the slave pty. */
    close(wire->fd[MASTER_SIDE]);
    return FALSE;
  }

  /* Restore SIGCHLD handler. */
  if (oldAction.sa_handler != SIG_DFL)
    sigaction(SIGCHLD, &oldAction, (struct sigaction *)NULL);

  if ((unlockpt(wire->fd[MASTER_SIDE]) != OK) ||
      ((slaveName = ptsname(wire->fd[MASTER_SIDE])) == (char *)NULL) ||
      (access(slaveName, R_OK | W_OK) == ERROR))
  {
    /* cannot access the slave pty. */
    close(wire->fd[MASTER_SIDE]);
    return FALSE;
  }

  /* we have opened a master with a slave we can access */
  strcpy(wire->slave_name, slaveName);
  return TRUE;
}

#endif

/* This is not exactly what should be done.  We really should open the
   directory which contains "suspected" ptys.  Then read the directory
   to get filenames, then check each file name to tell whether it is a
   master pty.  However, for the time being, we will use the old way.
*/   
   

/*----------------------------------------------------------------------+*/
static int getptypair(Wire *wire)
/*----------------------------------------------------------------------+*/
{
  /* Attempt to open the master/slave pair of preset pty */

  XeString master=wire->master_name;
  XeString slave =wire->slave_name;
  
  int c;
  XeChar d;
  XeString mptr;
  XeString sptr;	/* Point to end of preset prefixes */
  int c_len = strlen (MASTER_NAMES);

  /* Set things up to quickly alter last 2 chars of path strings */
  mptr = master + strlen(master);
  *(mptr+2) = (XeChar)'\0';

  sptr = slave + strlen(slave);
  *(sptr+2) = (XeChar)'\0';

  for (c = 0; c < c_len; c++) { /* 1st char */

    /* Get the next character in order */
    *(mptr) = *(sptr) = MASTER_NAMES[c];

    for (d = 0; d < 15; d++) {	/* 2nd char:  0..9,a..f */

      /* Get the next hex number in order */
      *(mptr+1) = hexdigits[d];

      /* Attempt to open this master side of pty */

      if ((wire->fd[MASTER_SIDE] = open(master, O_RDWR)) < OK) {
	/* open(master) failed, try next master... */
	continue;
      }

      *(sptr+1) = hexdigits[d];

      /* check that we can eventually open the slave side, using
	 the access system call */
      if(access(slave, R_OK | W_OK) == ERROR) {
	/* cannot access the slave pty.  Close master and try next one */
	close(wire->fd[MASTER_SIDE]);
	continue;
      }

      /* we have opened a master with a slave we can access */
      
      return TRUE;
    }				/* End for 2nd char */
  }				/* End for 1st char */

  return FALSE;
}

/*----------------------------------------------------------------------+*/
static int initpty(Wire *wire)
/*----------------------------------------------------------------------+*/
{
  /* Find first available master/slave pair */

  /* set both sides of wire to -1 (unitialized convention) */
  wire->fd[MASTER_SIDE] = wire->fd[SLAVE_SIDE] = -1;

#ifdef HAVE_PTMS
  if (!getspec1170ptypair(wire))
#endif
  {
    /* Start with convention 1 */
    strcpy(wire->master_name, MPREFIX_1);
    strcpy(wire->slave_name, SPREFIX_1);

    if (!getptypair(wire)) {
      /* Cannot get that pair, so try convention 2 */
      strcpy(wire->master_name, MPREFIX_2);
      strcpy(wire->slave_name, SPREFIX_2);
      if (!getptypair(wire)) {
	/* No available pty's ?? */
	wire->master_name[0] = wire->slave_name[0] = (XeChar)'\0';
	wire->fd[MASTER_SIDE] = wire->fd[SLAVE_SIDE] = -1;
	SPC_Error(SPC_No_Pty);
	return(SPC_ERROR);
      }
    }
  }

  return(TRUE);
}

/*----------------------------------------------------------------------+*/
static int set_pty_state(int fd, struct termios *term_state)
/*----------------------------------------------------------------------+*/
{

  if (fd < 0 || !isatty(fd))
    return(TRUE);

  /* Go to cooked mode (modify terminal state) */
  if(tcsetattr(fd, TCSANOW, term_state)==ERROR) {
    SPC_Error(SPC_Bad_tc_Call,(XeString)"tcsetattr");
    return(SPC_ERROR);
  }
#ifdef __sun
  {
    struct winsize size;
    size.ws_row = 0;
    size.ws_col = 0;
    size.ws_xpixel = 0;
    size.ws_ypixel = 0;

    if(ioctl(fd, TIOCSWINSZ, &size) == ERROR) {
      SPC_Error(SPC_Bad_tc_Call,(XeString)"tc_setwinsize");
      return(SPC_ERROR);
    }

  }
#endif  
  return(TRUE);
}

/*----------------------------------------------------------------------+*/
int master_pty(int fd, struct termios *state)
/*----------------------------------------------------------------------+*/
{
  /* Make any special circumstances required on master side of pty */
  int enable = 1;

  if (fd < 0)
    return(TRUE);
  
#ifdef __hpux_pty
  /* Enable trapping of ioctl/open/close (we care about close()) */
  if(ioctl(fd, TIOCTRAP, &enable)==ERROR) {
    SPC_Error(SPC_Bad_Ioctl);
    return(SPC_ERROR);
  }
#endif /* __hpux_pty */  

  set_pty_state(fd, state);


  return(TRUE);
}


/*----------------------------------------------------------------------+*/
static Wire *getpty(Wire *prevwire)
/*----------------------------------------------------------------------+*/
{
  Wire *wire_ptr=get_new_wire();

  if(!wire_ptr)
    return(SPC_ERROR);

  if(initpty(wire_ptr)==SPC_ERROR) {
    free_wire(wire_ptr);
    return(SPC_ERROR);
  }
  
  wire_ptr->next=prevwire;
  return(wire_ptr);
}

static void init_termio(struct termios *term_state)
{
	
  /* Basically, we want the pty channel to act like like a pipe
     (perhaps later we should get more fancy).  This means that we
     do not do any input processing for KILL / ERASE characters, we don't
     echo the data, and reads will return when there is at least one character
     in the buffer to be read. */
	
  term_state->c_iflag = 0;
  term_state->c_oflag = 0;
  term_state->c_cflag = CS8 | CREAD | HUPCL;
  term_state->c_lflag = 0;

  cfsetispeed(term_state, B9600);
  cfsetospeed(term_state, B9600);
	
  term_state->c_cc[VMIN]=1;
  term_state->c_cc[VTIME]=0;
}

/*
 ***
 *** Method definitions
 ***
*/

/*----------------------------------------------------------------------+*/
SPC_Channel_Ptr open_pty_channel_object(SPC_Channel_Ptr channel,
					int iomode,
					XeString hostname)
/*----------------------------------------------------------------------+*/
{

  Wire *tmpwire, *newwire;
  SPC_Channel_Ptr result;
  
  call_parent_method(channel, open, (channel, iomode, hostname), result);

  if(result==SPC_ERROR)
    return(SPC_ERROR);

  /* We know that we are going to use one of STDIN, STDOUT, or STDERR
     (or else we would be a NOIO channel), so allocate at least one
     pty pair */

  if(!(tmpwire=getpty(NULL)))
    return(SPC_ERROR);

  if (IS_SPCIO_STDIN(iomode)) {
    channel->wires[STDIN]=tmpwire;
  }
  
  if (IS_SPCIO_STDOUT(iomode)) {
    channel->wires[STDOUT]=tmpwire;
  }
  
  if (IS_SPCIO_SEPARATE(iomode)) {
    if(!(newwire=getpty(tmpwire))) {
      spc_close(tmpwire->fd[MASTER_SIDE]);
      free_wire(tmpwire);
      return(SPC_ERROR);
    } else
      tmpwire=newwire;
  }
  
  if (IS_SPCIO_STDERR(iomode)) {
    channel->wires[STDERR]=tmpwire;
  }
  
  channel->wire_list=tmpwire;
  
  /* set up the channel file descriptors */

  channel->file_descs[STDIN]  = (channel->wires[STDIN]) ->fd[MASTER_SIDE];
  channel->file_descs[STDOUT] = (channel->wires[STDOUT])->fd[MASTER_SIDE];
  channel->file_descs[STDERR] = (channel->wires[STDERR])->fd[MASTER_SIDE];

  for(tmpwire=channel->wire_list; tmpwire; tmpwire=tmpwire->next) {
    init_termio(&tmpwire->master_termio);
    init_termio(&tmpwire->slave_termio);
  }
  
  return(channel);

}

/*----------------------------------------------------------------------+*/
int read_pty_channel_object(SPC_Channel_Ptr channel,
			    int connector,           /* STDOUT or STDERR */
			    XeString buffer,
			    int nbytes)
/*----------------------------------------------------------------------+*/
#ifdef __hpux_pty
{
  
  int result, select_value;
  struct fd_set read_mask, except_mask;
  int fd=channel->file_descs[connector];
  struct request_info req_info;
  struct timeval timeout, *timeptr;
  int i;
 
  call_parent_method(channel,
		     read,
		     (channel, connector, buffer, nbytes),
		     result);
  
  if(result==SPC_ERROR)
    return(SPC_ERROR);

  if(!IS_SPCIO_DATA(channel->wires[connector]->flags))
    return(0);

  FD_ZERO(&read_mask);
  FD_ZERO(&except_mask);

  FD_SET(fd, &read_mask);
  FD_SET(fd, &except_mask);
  
  if(channel->close_timeout) {
    timeout.tv_sec=channel->close_timeout;
    timeout.tv_usec=0;
    timeptr = (&timeout);
  } else
    timeptr=NULL;

  do
    select_value=select(fd+1, &read_mask, NULL, &except_mask, timeptr);
  while(select_value==ERROR && errno==EINTR);
  
  if(select_value==ERROR) {
    SPC_Error(SPC_Bad_Select);
    return(SPC_ERROR);
  }

  /* If there is anything to read, read it & return */
  IS_FD_SET(&read_mask, result);
  if(result) {
    do {
      result = read(fd, buffer, nbytes);
    } while (result<0 && errno == EINTR);
    if(result==ERROR) {
      SPC_Error(SPC_Reading);
      return(SPC_ERROR);
    }
    return(result);
  }
  
  /* Nothing to read.  We either timed out or got an exception. */
  
  if(select_value != 0) {

    /* We got an exception */
    ioctl(fd, TIOCREQGET, &req_info);
    
    /* Clear the request (Not really necessary in the case of a close,
       but do it anyway) */
    
    ioctl(fd, TIOCREQSET, &req_info);
  }

  if((select_value == 0) || (req_info.request == TIOCCLOSE)) {

    /* Close, disable trapping on this fd & return EOF.  We regard
       a timeout as being the same as a close. */

    SPC_Disable_Trapping(fd);
    SPC_Change_State(channel, connector, 0, -1);
    return(0);

  } else

    /* Otherwise (open or IOCTL), return -1 */

    return(EXCEPT_FLAG);
}
#else /* not __hpux_pty */
{
  int result;
  int fd=channel->file_descs[connector];
  long numbytes;
  fd_set read_mask;
  struct timeval tv={0, 50000};

  result=ioctl(fd, FIONREAD, &numbytes);
  if(numbytes == 0)
    do {
      result = kill(channel->pid, 0);
      if((result == -1) && errno == ESRCH) {
	SPC_XtRemoveInput(&channel->wires[connector]->read_toolkit_id, SPC_Input);
	SPC_Change_State(channel, connector, 0, -1);
	return(0);
      }
      FD_ZERO(&read_mask);
      FD_SET(fd, &read_mask);
      /*
       **
       ** This call to select doesn't have the cast to (int*), because
       ** this clause of the ifdef is not compiled on HPUX.
       **
      */ 
      result=select(fd+1, &read_mask, NULL, NULL, &tv);
      if((result == -1) && (errno != EINTR)) {
	SPC_XtRemoveInput(&channel->wires[connector]->read_toolkit_id, SPC_Input);
	SPC_Change_State(channel, connector, 0, -1);
	return(0);
      }
    } while((result <= 0));
	
  do {
    result = read(fd, buffer, nbytes);
  } while (result<0 && errno == EINTR);

  if(result == ERROR) {
    if(errno == EIO) {
      SPC_XtRemoveInput(&channel->wires[connector]->read_toolkit_id, SPC_Input);
      SPC_Change_State(channel, connector, 0, -1);
      return(0);
    } else {
      XeString connection_hostname = (channel->connection ?
				      CONNECTION_HOSTNAME(channel->connection) :
				      Xestrdup(XeString_Empty));
      SPC_Error(SPC_Reading, connection_hostname);
      XeFree(connection_hostname);
      return(SPC_ERROR);
    }
  }

  return(result);
}
#endif /* __hpux_pty */

/*----------------------------------------------------------------------+*/
int pre_fork_pty_channel_object(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{

  int result;
  Wire *wirelist;
  
  call_parent_method(channel, pre_fork, (channel), result);
  
  if(result==SPC_ERROR)
    return(SPC_ERROR);

  result=TRUE;
  
  for(wirelist=channel->wire_list; wirelist; wirelist=wirelist->next) {
    if(master_pty(wirelist->fd[MASTER_SIDE], &wirelist->master_termio)
       == SPC_ERROR)
      result=SPC_ERROR;
  }

#ifndef __hpux_pty
  if(pipe(channel->sync_pipe) < 0) {
    SPC_Error(SPC_No_Pipe);
    return(SPC_ERROR);
  }
#endif /* __hpux_pty */

  return(result);
}

#ifdef __hpux_pty
/*----------------------------------------------------------------------+*/
/* clear_trap */
/*----------------------------------------------------------------------+*/

/* I am not particularly enamored of this macro.  However, the style of
   the SCANBITS macro kinda forces me to write it this way.  In particular,
   I am a bit worried about the reference to except_mask, which is a
   "nonlocal reference" */

#define clear_trap(fd)  {struct request_info req_info;        \
			 int my_fd=(fd);                      \
			 ioctl(my_fd, TIOCREQGET, &req_info); \
			   if(req_info.request != TIOCOPEN) { \
                             SPC_Error(SPC_Bad_Ioctl);        \
			     return(SPC_ERROR);               \
			    }                                 \
			 ioctl(my_fd, TIOCREQSET, &req_info); \
                         FD_CLR(my_fd, &except_mask);         \
			 }
#endif /* __hpux_pty */

/*----------------------------------------------------------------------+*/
int post_fork_pty_channel_object(SPC_Channel_Ptr channel,
				 int parentp)
/*----------------------------------------------------------------------+*/


{
  int result;
  int iomode=channel->IOMode;
  int fd=channel->file_descs[STDIN];
  int stdinfd, stdoutfd, stderrfd;
#ifdef __hpux_pty
  struct fd_set except_mask, temp_mask;
#endif
  int pid;
  char c;
      
  call_parent_method(channel, post_fork, (channel, parentp), result);
  
  if(result==SPC_ERROR)
    return(SPC_ERROR);
  
  if (parentp) {		/* Master process */
#ifdef __hpux_pty
    { int i;
      int select_value;
      
      stdinfd  = channel->wires[STDIN]->fd[MASTER_SIDE];
      stdoutfd = channel->wires[STDOUT]->fd[MASTER_SIDE];
      stderrfd = channel->wires[STDERR]->fd[MASTER_SIDE];

      FD_ZERO(&except_mask);
      
      if(stdinfd >= 0)
	FD_SET(stdinfd, &except_mask);
      if(stdoutfd >= 0)
	FD_SET(stdoutfd, &except_mask);
      if(stderrfd >= 0)
	FD_SET(stderrfd, &except_mask);
      
      IS_FD_SET(&except_mask, result);
      while (result) {
	temp_mask = except_mask;
	select_value=select(max_fds, NULL, NULL, &temp_mask, NULL);
	SCANBITS(&temp_mask, clear_trap);
	IS_FD_SET(&except_mask, result);
      }
    }
#else				/* not __hpux_pty */
    close(channel->sync_pipe[WRITE_SIDE]);
    read(channel->sync_pipe[READ_SIDE], &c, 1);
    close(channel->sync_pipe[READ_SIDE]);
    channel->sync_pipe[READ_SIDE] = -1;
    channel->sync_pipe[WRITE_SIDE] = -1;
    XeSPCAddInput(channel, NULL, NULL);    
#endif				/* __hpux_pty */
  
  } else {			/* Slave process */

    /* Open the slave pty. Do it up to three times to set up
       stdin, stdout, stderr */
    
    stdinfd =(-1);
    stdoutfd=(-1);
    stderrfd=(-1);


    setsid();
    pid = getpid();
    
    if(IS_SPCIO_STDIN(iomode)) {
      if((stdinfd=open(channel->wires[STDIN]->slave_name, O_RDWR))<0) {
	SPC_Error(SPC_Cannot_Open_Slave,
		  channel->wires[STDIN]->slave_name);
	return(SPC_ERROR);
      }

    }
    
    if(IS_SPCIO_STDOUT(iomode)) {
      /* We will always share the file descriptor with STDIN,
	 if there is any */
      if(stdinfd != -1)
	stdoutfd=stdinfd;
      else {
	if((stdoutfd=open(channel->wires[STDOUT]->slave_name, O_RDWR))<0) {
	  SPC_Error(SPC_Cannot_Open_Slave,
		    channel->wires[STDOUT]->slave_name);
	  spc_close(stdinfd);
	  return(SPC_ERROR);
	} 
      }
    }
    
    if(IS_SPCIO_STDERR(iomode)) {
      /* If we want seperate STDOUT/STDERR, open a new FD */
      if(IS_SPCIO_SEPARATE(iomode)) {
	if((stderrfd=open(channel->wires[STDERR]->slave_name, O_RDWR))<0) {
	  SPC_Error(SPC_Cannot_Open_Slave,
		    channel->wires[STDIN]->slave_name);
	  spc_close(stdinfd);
	  spc_close(stdoutfd);
	  return(SPC_ERROR);
	}
      } else
	stderrfd=stdoutfd;
    }

#ifndef __hpux_pty
    /* The pty trapping stuff handles EOF for us.  Use the "sync" pipe */
    /* to inform the other side when we don't have that code.          */
    c=040;
    write(channel->sync_pipe[WRITE_SIDE], &c, 1);
    close(channel->sync_pipe[READ_SIDE]);
    close(channel->sync_pipe[WRITE_SIDE]);
#endif				/* __hpux_pty */

    /* Duplicate these file descriptors to 3, 4, 5 so we don't have to
       worry about any of std[in|out|err]fd being 0, 1, or 2. */
    
    spc_dup2(stdinfd,  3);
    spc_dup2(stdoutfd, 4);
    spc_dup2(stderrfd, 5);
    
    spc_dup2(3, STDIN);
    spc_dup2(4, STDOUT);
    spc_dup2(5, STDERR);

    if(IS_SPCIO_STDIN(iomode))
      set_pty_state(STDIN, &(channel->wires[STDIN]->slave_termio));
    if(IS_SPCIO_STDOUT(iomode))
      set_pty_state(STDOUT, &(channel->wires[STDOUT]->slave_termio));
    if(IS_SPCIO_STDERR(iomode))
      set_pty_state(STDERR, &(channel->wires[STDERR]->slave_termio));
    
    /* Close any other open file descriptors in the child */
    
    SPC_Close_Unused();
  }
  return(TRUE);
}

/*----------------------------------------------------------------------+*/
int reset_pty_channel_object(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{
  int result;
  Wire *wirelist;
  
  call_parent_method(channel, reset, (channel), result);

  if(result==SPC_ERROR)
    return(SPC_ERROR);

  result=TRUE;
  
  /* Make any special circumstances required on master side of pty */
  
  for(wirelist=channel->wire_list; wirelist; wirelist=wirelist->next) {
    
#ifdef __hpux_pty
  {
    int fd=wirelist->fd[MASTER_SIDE];
    /* Disable trapping of ioctl/open/close */
    if(SPC_Disable_Trapping(fd) == SPC_ERROR)
	result=SPC_ERROR;
  }
#endif /* __hpux_pty */    

    wirelist->flags &= ~SPCIO_DATA;
  }

  return(result);
}

/*----------------------------------------------------------------------+*/
int attach_pty_channel_object(SPC_Channel_Ptr channel, int pid)
/*----------------------------------------------------------------------+*/
{
  set_pty_state(channel->file_descs[STDIN],
		&(channel->wires[STDIN]->master_termio));
  set_pty_state(channel->file_descs[STDOUT],
		&(channel->wires[STDOUT]->master_termio));
  set_pty_state(channel->file_descs[STDERR],
		&(channel->wires[STDERR]->master_termio));
  
  XeSPCReset(channel);

  if(!mempf0(channel, pre_fork))
    return(SPC_ERROR);
  channel->pid = pid;
  return(TRUE);
}

/*----------------------------------------------------------------------+*/
int add_input_pty_channel_object(SPC_Channel_Ptr channel,
				 SbInputHandlerProc handler,
				 void *data)
/*----------------------------------------------------------------------+*/
{
  int result, fd;
  Wire *wirelist, *stdinwire;
  
  call_parent_method(channel, add_input, (channel, handler, data), result);

  if(result==SPC_ERROR)
    return(SPC_ERROR);
  
  stdinwire=channel->wires[STDIN];

  for(wirelist=channel->wire_list; wirelist; wirelist=wirelist->next) {

    if((wirelist->read_toolkit_id   != -1) ||
       (wirelist->except_toolkit_id != -1))
      continue;
    
    fd=wirelist->fd[READ_SIDE];
    SPC_XtAddInput(channel,
		   &wirelist->read_toolkit_id,
		   fd,
		   channel->class_ptr->input,
		   SPC_Input);
#ifdef __hpux_pty
    SPC_XtAddInput(channel,
		   &wirelist->except_toolkit_id,
		   fd,
		   channel->class_ptr->input,
		   SPC_Exception);
#endif /* __hpux_pty */
  }
  
  return(TRUE);
  
}

Wire *setpgrp_wire = NULL;

struct termios *XeTermioStruct = NULL;
struct termios XeDefaultTermioStruct;

/*----------------------------------------------------------------------+*/
void InitDefaultTermioStruct(void)
/*----------------------------------------------------------------------+*/
{
    int i;
    
    XeDefaultTermioStruct.c_iflag = BRKINT | IGNPAR | ICRNL | IXON;
    XeDefaultTermioStruct.c_oflag = OPOST | ONLCR;
    XeDefaultTermioStruct.c_cflag = CS8 | CREAD | CLOCAL;
    XeDefaultTermioStruct.c_lflag = ISIG | ICANON | ECHO | ECHOE | ECHOK;

    cfsetispeed(&XeDefaultTermioStruct, B9600);
    cfsetospeed(&XeDefaultTermioStruct, B9600);
    
    for(i=0; i<NCCS; i++) 
	XeDefaultTermioStruct.c_cc[i] = 0;
    
    XeDefaultTermioStruct.c_cc[VEOF]   = 04;		/* ^d */
    XeDefaultTermioStruct.c_cc[VEOL]   = 0;		/* no extra eol char */
    XeDefaultTermioStruct.c_cc[VERASE] = 010; 		/* ^h */
    XeDefaultTermioStruct.c_cc[VINTR]  = 03; 		/* ^c */
    XeDefaultTermioStruct.c_cc[VKILL]  = 025;  		/* ^u */
    XeDefaultTermioStruct.c_cc[VQUIT]  = 034;		/* ^\ */
    XeDefaultTermioStruct.c_cc[VSTART] = 021;		/* ^q */
    XeDefaultTermioStruct.c_cc[VSTOP]  = 023;		/* ^s */
    XeDefaultTermioStruct.c_cc[VSUSP]  = 032;         	/* ^z */

    /* MIN and TIME are not needed in canonical ("line" or "cooked") mode */

}


/*----------------------------------------------------------------------+*/
struct termios *SPC_Get_Current_Termio(void)
/*----------------------------------------------------------------------+*/
{

  struct termios *termio_struct;
  int tty_fd, retval;
  static Boolean default_is_initialized = FALSE;

  _DtSvcProcessLock();
  if (!default_is_initialized)
  {
      default_is_initialized = TRUE;
      InitDefaultTermioStruct();
  }
  _DtSvcProcessUnlock();

  termio_struct = (struct termios *)XeMalloc(sizeof(struct termios));

  /* See if we can open /dev/tty go get default settings for this system */
#ifdef DEBUG
  tty_fd = -1;
#else
  tty_fd = open("/dev/tty", O_RDWR);
#endif     

  if (tty_fd >= 0)
  {
      /* retval=ioctl(tty_fd, TCGETA, termio_struct); */
      retval = tcgetattr(tty_fd, termio_struct);
      spc_close(tty_fd);

      if(retval == ERROR) {
	  SPC_Error(SPC_Bad_tc_Call,(XeString)"tcgetattr");

	  /* Fall through and use default settings */
      }
      else
	  return(termio_struct);
  }
  
  /* We get here if we can't open /dev/tty or the tcgetattr() call failed */

  memcpy(termio_struct, &XeDefaultTermioStruct, sizeof(struct termios));
  return(termio_struct);
}

/*----------------------------------------------------------------------+*/
int SPC_Setpgrp(int read_current_termio)
/*----------------------------------------------------------------------+*/
{
  _DtSvcProcessLock();
  if(setpgrp_wire == NULL)
    setpgrp_wire = get_new_wire();
  
  if(read_current_termio || XeTermioStruct == NULL) {
    
    if(XeTermioStruct)
      free((char *)XeTermioStruct);
    
    if((XeTermioStruct=SPC_Get_Current_Termio()) == SPC_ERROR) {
      _DtSvcProcessUnlock();
      return(SPC_ERROR);
    }
  }    
  
  spc_close(setpgrp_wire->fd[MASTER_SIDE]);
  spc_close(setpgrp_wire->fd[SLAVE_SIDE]);
  
  if((initpty(setpgrp_wire)) == SPC_ERROR) {
    spc_close(setpgrp_wire->fd[MASTER_SIDE]);
    spc_close(setpgrp_wire->fd[SLAVE_SIDE]);
    _DtSvcProcessUnlock();
    return(SPC_ERROR);
  }
  
  /* Point of no return */
  
  setsid();
  
  if((setpgrp_wire->fd[SLAVE_SIDE]=open(setpgrp_wire->slave_name, O_RDWR)) < 0) {
    _DtSvcProcessUnlock();
    return(SPC_ERROR);
  }

#if !defined(USL) && !defined(__uxp__)
  if(tcsetattr(setpgrp_wire->fd[SLAVE_SIDE], TCSANOW, XeTermioStruct)==ERROR) {
    SPC_Error(SPC_Bad_tc_Call,(XeString)"tcsetattr");
    _DtSvcProcessUnlock();
    return(SPC_ERROR);
  }
#endif

  _DtSvcProcessUnlock();
  return(TRUE);  
}

/*
 **
 ** New B.00 methods
 **
*/

int set_termio_pty_channel_object(SPC_Channel_Ptr channel,
				  int connection,
				  int side,
				  struct termios *termio)
{
  struct termios *old_termio;

  if(side == MASTER_SIDE)
    old_termio = &channel->wires[connection]->master_termio;
  else
    old_termio = &channel->wires[connection]->slave_termio;

  memcpy(old_termio, termio, sizeof(struct termios));

  return(TRUE);
  
}


static int send_eof_pty_channel_object(SPC_Channel_Ptr channel)
{
  Wire *wire = channel->wires[STDIN];
  char output_char;
  int fd, ret;

  if(wire == NULL)
    return(TRUE);

  if((wire->slave_termio.c_lflag & ~ICANON) == 0)
    return(FALSE);
  
  output_char = wire->slave_termio.c_cc[VEOF];
  fd = channel->file_descs[STDIN];
  
  /* Write twice -- once to flush output, and once to have 0 bytes sent. */
  
  ret = write(fd, &output_char, 1);
  ret = write(fd, &output_char, 1);

  return(TRUE);
}
