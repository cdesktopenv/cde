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
 * File:         spc-util.c $XConsortium: spc-util.c /main/5 1996/06/21 17:33:16 ageorge $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#include <bms/sbport.h> /* NOTE: sbport.h must be the first include. */
#include <stdarg.h>

#include <SPC/spcP.h>
#include <bms/MemoryMgr.h>
#include "DtSvcLock.h"

/* The application's SPC activation list */
SPC_Channel_Ptr spc_activation_list = NULL;

/* Allocate us up a wire */

/*----------------------------------------------------------------------+*/
Wire *get_new_wire(void)
/*----------------------------------------------------------------------+*/
{
  Wire *tmp_wire;

  tmp_wire=(Wire *)XeMalloc(sizeof(Wire));
  memset(tmp_wire, 0, sizeof(Wire));

  tmp_wire->master_name=(XeString)XeMalloc(PTY_NAMLEN);
  memset(tmp_wire->master_name, 0, PTY_NAMLEN);
  tmp_wire->slave_name =(XeString)XeMalloc(PTY_NAMLEN);
  memset(tmp_wire->slave_name,  0, PTY_NAMLEN);
  tmp_wire->fd[0] = tmp_wire->fd[1] = (-1);
  tmp_wire->read_toolkit_id   = (-1);
  tmp_wire->except_toolkit_id = (-1);
  return(tmp_wire);
}

/*----------------------------------------------------------------------+*/
void free_wire(Wire *wire)
/*----------------------------------------------------------------------+*/
{
  free(wire->master_name);
  free(wire->slave_name);
  free((char *)wire);
}

/*
 * Channel object access
 */

/*----------------------------------------------------------------------+*/
SPC_Channel_Ptr SPC_Find_PID(int pid)
/*----------------------------------------------------------------------+*/
{
  /* Attempt to return a channel which currently handles process number PID */
  SPC_Channel_Ptr spc;

  _DtSvcProcessLock();
  for (spc = spc_activation_list; spc != NULL; spc = spc->next) {
    if (spc->pid == pid) break;
  }
  _DtSvcProcessUnlock();
  return spc;			/* NULL when not found */
}

/*
 * Miscellaneous
 */

/*----------------------------------------------------------------------+*/
spc_close(int fd)
/*----------------------------------------------------------------------+*/
{
  /* Close a file descriptor which was referenced through an SPC structure */
  if(fd != -1)
    close(fd);
  
  return(fd);
  
}

/*----------------------------------------------------------------------+*/
spc_dup2(int from, int to)
/*----------------------------------------------------------------------+*/
{
  int retval;
  
  /* Dup file descriptors.  If a null descriptor, then use /dev/null */
  static int devnull = 0;

  if (from == to)
    return(TRUE);
  
  if (from == -1) {
    _DtSvcProcessLock();
    if (!devnull)
      devnull = open("/dev/null", 0);
    /* Use /dev/null when no source file descriptor */
    from = devnull;
    _DtSvcProcessUnlock();
  }
  
  /* Now do the dup2 */
  retval=dup2(from, to);
  return(retval);
}

/*----------------------------------------------------------------------+*/
SPC_fd_to_connector(SPC_Channel_Ptr channel,
		    int fd)
/*----------------------------------------------------------------------+*/
{
  if(Stdout(channel) == fd)
    return(STDOUT);
  if(Stdin(channel) == fd)
    return(STDIN);
  if(Stderr(channel) == fd)
    return(STDERR);
  return(ERROR);
}

XeString *Alloc_Argv(int n)
{
  /* Allocate an array to hold argv list */
  XeString *av;
  
  av = (XeString *)XeMalloc((n + 1) * sizeof(XeString));
  
  /* Zero the space so we don't have to worry about trailing NULL */
  memset((XeString) av, 0, (n + 1) * sizeof(XeString));
  
  return(av);
}

/*
 ***
 *** Toolkit integration stuff, without direct calls to the toolkit.
 ***
*/

int break_on_termination=FALSE;

/*----------------------------------------------------------------------+*/
void SPC_Conditional_Packet_Handler(void      * UNUSED_PARM(client_data),
				   int        * source,
				   SPCInputId * UNUSED_PARM(id))
/*----------------------------------------------------------------------+*/
{

  SPC_Channel_Ptr channel;
  
  channel = XeSPCHandleTerminator(*source);
  
  /* Okay, blast out of our wait */
  _DtSvcProcessLock();
  if( (channel==SPC_ERROR || !IS_ACTIVE(channel)) &&
      break_on_termination)
    SPC_XtBreak();
  _DtSvcProcessUnlock();
  /* return(TRUE); */
}

/*----------------------------------------------------------------------+*/
int sprintf_len (XeString s, XeString format, ...)
/*----------------------------------------------------------------------+*/
{
  va_list ap;

  va_start(ap, format);
  if (s) *s = XeChar_NULL;
  vsprintf(s, format, ap);
  return(strlen(s));
}

/*
 ***
 *** _path_search is a generalized function used for parsing the PATH
 *** environment variable wrt a passed filename.  It will run down the
 *** passed path variable looking for ':'.  When it finds one (or hits
 *** the end of the string), it will concatenate the substring with
 *** filename, passing the result to the passed path_search_predicate.
 *** If this predicate returns true, the function will return with a
 *** true value.  If all elements of the path are processed with no
 *** true result from the predicate, the function will return false.
 ***
 *** A few special cases:
 ***  - If the filename begins with '/' (an absolute path), the
 ***    funciton will return the value of the predicate on the
 ***    filename (e.g. PATH processing will not be done).
 ***  - a NULL value of any path component will be interpreted as the
 ***    current directory ('.').
 ***  - a NULL value for the path parameter means use the PATH
 ***    environment variable.
 ***  - a NULL value for filename will return FALSE
 ***
*/

/*
 ***
 *** MAXPATHLEN is defined in sbstdinc.h
 ***
*/ 

/*----------------------------------------------------------------------+*/
Boolean _path_search (XeString path, XeString filename, path_search_predicate p)
/*----------------------------------------------------------------------+*/
{

  XeString path_rest, next_colon=NULL;
  XeChar buffer[MAXPATHLEN+1];
  XeChar path_component[MAXPATHLEN+1];
  int path_component_len;
  int filename_len;
  int component_seperator=(int)':';  /* this is here because strchr takes an in */
  
  if(!filename)
    return(FALSE);
  
  if(*filename == '/')
    return((*p)(filename, NULL, filename));

  filename_len = strlen(filename);
  if(!path)
    path=getenv("PATH");

  for( (path_rest=path ,
	next_colon=strchr(path_rest, component_seperator));
      path_rest && *path_rest;
      next_colon=strchr(path_rest, component_seperator)) {
    
    /*
     ** Copy path component into buffer
     */
    
    if(next_colon) { /* found colon */
      path_component_len = next_colon-path_rest;
      strncpy(buffer, path_rest, path_component_len);
      buffer[path_component_len]='\0';
      path_rest=next_colon+1;
      if(!*path_rest)
	/* We've seen a ':' at the end of the string.  Make path_rest be "."
	   next go-round */
	path_rest=".";
    } else {         /* no colon */
      path_component_len = strlen(path_rest);
      strcpy(buffer, path_rest);
      path_rest=NULL;
    }

    /*
     ** if path component is NULL, use default ('.');
     */

    if(!buffer[0]) {
      buffer[0] = '.';
      buffer[1] = '\0';
    }
    
    /*
     ** Concatenate filename
     */

    if((path_component_len + filename_len + 1) < MAXPATHLEN) {
      path_component[0]=0;
      strcat(path_component, buffer);
      strcat(buffer, "/");
      strcat(buffer, filename);
      
      /*
       ** Check this file.  If the predicate returns true, we return true.
       */
      
      if((*p)(buffer, path_component, filename))
	return(TRUE);
    }
  }

  /*
   ** We've checked all components.  Return False.
   */

  return(FALSE);
}
