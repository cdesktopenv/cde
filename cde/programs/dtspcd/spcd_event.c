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
 * File:         spcd_event.c $XConsortium: spcd_event.c /main/4 1996/01/15 13:49:32 rswiston $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#define __need_fd_set

#include <bms/sbport.h>	 /* NOTE: sbport.h must be the first include. */
#include <SPC/spcP.h>
#include <bms/SbEvent.h> 
#include <sys/types.h>   /* for fd_set, FD_SET macros, et. al. */
#include <errno.h>

#ifndef __hpux
# define FD_SET_CAST(x) (x)
#else
# define FD_SET_CAST(x) ((int *)(x))
#endif /* __hpux */

struct {SbInputCallbackProc handler; void* data; }
  SPCD_input_handlers [FD_SETSIZE],
  SPCD_except_handlers[FD_SETSIZE];

int SPCD_max_fd = 1;

fd_set Sb_Input_Mask, Sb_Except_Mask;

SbInputId SPCD_AddInput(int fd, SbInputCallbackProc proc, void* data)
 {SPCD_input_handlers[fd].handler = proc;
  SPCD_input_handlers[fd].data    = data;
  FD_SET(fd, &Sb_Input_Mask);
  if (SPCD_max_fd < fd) SPCD_max_fd = fd;
  return fd; }

SbInputId SPCD_AddException(int fd, SbInputCallbackProc proc, void* data)
 {SPCD_except_handlers[fd].handler = proc;
  SPCD_except_handlers[fd].data    = data;
  FD_SET(fd, &Sb_Except_Mask);
  if (SPCD_max_fd < fd) SPCD_max_fd = fd;
  return fd; }

void SPCD_RemoveInput(SbInputId id)
 {FD_CLR(id, &Sb_Input_Mask);
 }

void SPCD_RemoveException(SbInputId id)
 {FD_CLR(id, &Sb_Except_Mask);
 }


void SPCD_MainLoopUntil(Boolean *flag)
{
  
  int fd_vec_size = howmany(SPCD_max_fd, NFDBITS);
  fd_set input_mask, except_mask;
  int n, fd;
  int result;

  do {
    memcpy(&input_mask, &Sb_Input_Mask, sizeof(fd_set));
    memcpy(&except_mask, &Sb_Except_Mask, sizeof(fd_set));

    
    do result=select(SPCD_max_fd + 1, FD_SET_CAST(&input_mask),
		     FD_SET_CAST(NULL),
		     FD_SET_CAST(&except_mask), NULL);
    while(result == -1 && errno==EINTR);
    
    if(result < 0) {
      SPC_Error(SPC_Bad_Select);
      SPC_Format_Log((XeString)"Exiting server ...");
      SPC_Close_Log();
      exit (3);
    }

    /* Modified loop to break after a single hit on the select.  This */
    /* is necessary because there is nothing which stops the lower */
    /* level input handlers from reading the data from a random file */
    /* descriptor.  If this happens, and the random file descriptor */
    /* happens to be one on which this loop detected input, the */
    /* process might hang.  The solution is therefore to go back to */
    /* the select after every input handler call. */
    
    for (fd=0; fd < SPCD_max_fd+1; fd++)
      {
	SbInputId id = fd;
	if(FD_ISSET(fd, &except_mask)) {
	  (*SPCD_except_handlers[fd].handler)(SPCD_except_handlers[fd].data, &fd, &id);
	  break;
	}
	if(FD_ISSET(fd, &input_mask)) {
	  (*SPCD_input_handlers[fd].handler)(SPCD_input_handlers[fd].data, &fd, &id);
	  break;
	}
      }
  } while (!*flag);
}

void SPCD_BreakMainLoop(void) 
{ 
  /* no need to do anything */ 
}


