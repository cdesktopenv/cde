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
/* $XConsortium: noio.c /main/8 1996/11/21 19:53:13 drk $
 *
 * File:         noio.c 
 * Language:     C
 *
 * (c) Copyright 1989, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#include <bms/sbport.h> /* NOTE: sbport.h must be the first include. */
#include <bms/MemoryMgr.h>
#define X_INCLUDE_PWD_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>
#include <limits.h>

#include <SPC/spcP.h>
#include "DtSvcLock.h"

/* Global variables */

/*
 * The name of the directory used for authentication and
 * for temporary logfiles.
 */
XeString SPCD_Authentication_Dir = NULL;

/*
 * This array contains the names of the logfiles the SPC daemon
 * creates.  Before the daemon exits, if any logfiles exist, they 
 * will be removed.
 */
char **SPC_logfile_list = NULL;

/*----------------------------------------------------------------------+*/
void noio_channel_class_init(object_clasp t)
/*----------------------------------------------------------------------+*/
{
  noio_channel_clasp c =(noio_channel_clasp) t;
  
  c->new_obj    = alloc_channel_object;
  
  c->open       = open_noio_channel_object;
  c->close      = close_local_channel_object;
  c->read       = read_noio_channel_object;
  c->write      = write_noio_channel_object;
  c->reset      = reset_noio_channel_object;
  c->pre_fork   = pre_fork_noio_channel_object;
  c->post_fork  = post_fork_noio_channel_object;
  c->exec_proc  = exec_proc_local_channel_object;
  c->signal     = signal_local_channel_object;
  c->wait_for_termination = local_channel_object_wait_for_termination;
  c->attach     = attach_noio_channel_object;
  c->input      = noio_channel_object_input_handler;
  c->remove_logfile = remove_logfile_local_channel_object;

  /* New B.00 methods */

  c->send_eof   = send_eof_noio_channel_object;
  c->set_termio = set_termio_noio_channel_object;
}

static struct noio_channel_class noio_channel_class_struct = {
  (channel_clasp) &channel_class, /* base class pointer */
  "noio_channel",           /* class name */
  noio_channel_class_init,  /* class initialize function */
  sizeof(SPC_Channel),      /* size */
  0
  };

noio_channel_clasp noio_channel_class = &noio_channel_class_struct;

/*
 * Forward declarations
 */

static int is_dir_usable (
	char		*dir);
static char * get_tmp_dir (
	void);


/****************************************************************************
 *
 * is_dir_usable - given a directory name, return 1 if:
 *
 *    #1 the directory exists (determined by stat() succeeding)
 *
 *    #2 the directory is readable
 *
 *    #3 the directory is writeable
 *
 * otherwise, return 0.
 *
 * Parameters:
 *
 *    char	*dir	- the directory to check
 *
 * Return Value:
 *
 *    1 if 'dir' is usable and 0 if it is not
 *
 ****************************************************************************/

static int is_dir_usable (
	char		*dir)
{
   struct stat	stat_buffer;

   if ((stat (dir, &stat_buffer)) != 0) 
      return (0);

   if ((S_ISDIR(stat_buffer.st_mode)) && (access (dir, W_OK | R_OK) == 0))
      return (1);

   return (0);
}

/****************************************************************************
 *
 * get_tmp_dir - determine the name of a directory to use
 *    for temporary logfiles.  The first dirctory in the 
 *    following list that passes the usability test in 
 *    'is_dir_usable' will be returned:
 *
 *       ~/.dt/tmp
 *
 *       ~/.dt
 *
 *       ~/
 *
 * Parameters: NONE
 *
 * Return Value:
 *
 *    char *	-  the name of the temporary directory to use
 *                 or NULL if no usable directory is found.
 *
 * Notes:  
 *
 *    o The caller is responsible for free'ing the returned 
 *      string (if it is not NULL)
 *
 ****************************************************************************/

static char * get_tmp_dir (
	void)
{
   char			*dir;
   _Xgetpwparams	pwd_buf;
   struct passwd *	pwd_ret;

   if ((pwd_ret = _XGetpwuid(getuid(), pwd_buf)) == NULL)
      return (NULL);

   dir = malloc (strlen (pwd_ret->pw_dir) +
		 strlen (SPCD_ENV_HOME_DIRECTORY) + 8);

   if (!dir)
      return (NULL);

   (void) sprintf (dir, "%s/%s/tmp", pwd_ret->pw_dir, SPCD_ENV_HOME_DIRECTORY);
   if (is_dir_usable (dir))
      return (dir);

   (void) sprintf (dir, "%s/%s", pwd_ret->pw_dir, SPCD_ENV_HOME_DIRECTORY);
   if (is_dir_usable (dir))
      return (dir);

   (void) sprintf (dir, "%s", pwd_ret->pw_dir);
   if (is_dir_usable (dir))
      return (dir);

   free(dir);
   return (NULL);
}

/*
 ***
 *** Method definitions for noio channel objects
 ***
*/

/*
 * This routine handles initialization for noio channels
 */

/*----------------------------------------------------------------------+*/
SPC_Channel_Ptr open_noio_channel_object(SPC_Channel_Ptr channel,
					 int iomode,
					 XeString hostname)
/*----------------------------------------------------------------------+*/
{
  int i=0;
  SPC_Channel_Ptr result;
  XeString temp_dir_name = NULL;
  
  call_parent_method(channel, open, (channel, iomode, hostname), result);

  if(result==SPC_ERROR)
    return(SPC_ERROR);

  if(IS_SPCIO_USE_LOGFILE(iomode)) {

    _DtSvcProcessLock();
    /* Storage from tempnam() freed in remove_logfile_local_channel_object */
    if (SPCD_Authentication_Dir != NULL)
      channel->logfile=tempnam(SPCD_Authentication_Dir,"SPC");
    else {
      temp_dir_name = get_tmp_dir ();
      channel->logfile=tempnam(temp_dir_name,"SPC");
      free(temp_dir_name);
    }

    /*
     * Save the name of the logfile so it can removed by the
     * daemon before it exits.
     */
    if (SPC_logfile_list == NULL)
      /*
       * Create the first block plus the NULL terminator.
       */
      SPC_logfile_list = (char **) malloc (2 * sizeof (char *));
    else {
      /*
       * Need to add this file to the end of the list.
       */
      for (i = 0; SPC_logfile_list[i] != NULL; i++);
      SPC_logfile_list = (char **) realloc (SPC_logfile_list, 
					    (i+2) * sizeof (char *));
    }
    if (channel->logfile != NULL)
       SPC_logfile_list[i] = strdup (channel->logfile);
    else
       SPC_logfile_list[i] = (char *) NULL;
    SPC_logfile_list[i+1] = (char *) NULL;
    _DtSvcProcessUnlock();

    if(!channel->logfile) {
      SPC_Error(SPC_Out_Of_Memory);
      return(SPC_ERROR);
    }
  }
  
  return(channel);
  
}

/*----------------------------------------------------------------------+*/
int read_noio_channel_object(SPC_Channel_Ptr UNUSED_PARM(channel),
			     int             UNUSED_PARM(connector), /* STDOUT or STDERR */
			     XeString 	     UNUSED_PARM(buffer),
			     int             UNUSED_PARM(nbytes))
/*----------------------------------------------------------------------+*/
  
{
  SPC_Error(SPC_Bad_Operation);
  return(SPC_ERROR);
}

/*----------------------------------------------------------------------+*/
int write_noio_channel_object(SPC_Channel_Ptr UNUSED_PARM(channel),
			      XeString        UNUSED_PARM(buffer),
			      int             UNUSED_PARM(nbytes))
/*----------------------------------------------------------------------+*/
{
  
  SPC_Error(SPC_Bad_Operation);
  return(SPC_ERROR);
}

/*----------------------------------------------------------------------+*/
int pre_fork_noio_channel_object(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{

  int result;
  
  call_parent_method(channel, pre_fork, (channel), result);
  
  if(result==SPC_ERROR)
    return(SPC_ERROR);

  return(TRUE);
}

/*----------------------------------------------------------------------+*/
int post_fork_noio_channel_object(SPC_Channel_Ptr channel,
				  int parentp)
/*----------------------------------------------------------------------+*/
{
  int result, fd;
  
  call_parent_method(channel, post_fork, (channel, parentp), result);

  if(result==SPC_ERROR)
    return(SPC_ERROR);

  
  if (parentp) {		/* Master process */
  }
  else {			/* Slave process */

    /* set up STDIN, STDOUT & STDERR to go to /dev/null,
       or to the appropriate logfile */

    fd=(-1);
    
    spc_dup2(fd, STDIN);

    /* Open logfile if necessary */
    
    if(IS_SPCIO_USE_LOGFILE(channel->IOMode))
      if((fd=open(channel->logfile, O_WRONLY | O_CREAT, 0666)) == ERROR) {
	SPC_Error(SPC_Cannot_Open_Log, channel->logfile);
	return(SPC_ERROR);
      }
    spc_dup2(fd, STDOUT);
    spc_dup2(fd, STDERR);
    
    /* close all other file descriptors */
    
    SPC_Close_Unused();
  }
  return(TRUE);
}

/*----------------------------------------------------------------------+*/
int reset_noio_channel_object(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{
  int result;
  int iomode=channel->IOMode;
  
  call_parent_method(channel, reset, (channel), result);

  if(result==SPC_ERROR)
    return(SPC_ERROR);

  return(TRUE);

}

/*----------------------------------------------------------------------+*/
int attach_noio_channel_object(SPC_Channel_Ptr UNUSED_PARM(channel), 
			       int             UNUSED_PARM(foo))
/*----------------------------------------------------------------------+*/
{
  /* It is an error to try to attach to a noio channel... */
  SPC_Error(SPC_Bad_Operation);
  return(SPC_ERROR);
}

/*----------------------------------------------------------------------+*/
void noio_channel_object_input_handler(void  *UNUSED_PARM(client_data),
				  int        *UNUSED_PARM(source),
				  SPCInputId *UNUSED_PARM(id))
/*----------------------------------------------------------------------+*/
{

  SPC_Error(SPC_Bad_Operation);
  return /* (SPC_ERROR) */;
}

int send_eof_noio_channel_object(SPC_Channel_Ptr UNUSED_PARM(channel))
{
  SPC_Error(SPC_Bad_Operation);
  return(SPC_ERROR);
}

int set_termio_noio_channel_object(SPC_Channel_Ptr UNUSED_PARM(channel),
				   int UNUSED_PARM(connection),
				   int UNUSED_PARM(side),
				   struct termios * UNUSED_PARM(termio))
{
  SPC_Error(SPC_Bad_Operation);
  return(SPC_ERROR);
}
