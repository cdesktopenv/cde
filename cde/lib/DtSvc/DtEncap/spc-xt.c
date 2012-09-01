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
 * File:         spc-xt.c $TOG: spc-xt.c /main/6 1998/03/16 14:41:02 mgreess $
 * Language:     C
 *
 * (c) Copyright 1989, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#define __need_fd_set

#include <bms/sbport.h> /* NOTE: sbport.h must be the first include. */
#include <time.h>
#include <SPC/spcP.h>
/* #include <bms/SbEvent.h>  */ /* This file now included by spcP.h */
#include "DtSvcLock.h"

/* Externals */

extern int break_on_termination;

/* Utility functions */

/* First, declarations */

typedef struct {
  SbInputId 			read_id;
  SbInputId			except_id;
} SPC_Callback_Struct;

static SPC_Callback_Struct **SPC_Fd_Mapping = NULL;

#define SPC_LOOKUP_FD_MAPPING(fd)   SPC_Fd_Mapping[(fd)]

static Boolean spc_xe_termination_flag;

/*-----------------------------------------------------------------------+*/
static int SPC_AddInput(int 			source,
		 SPC_Callback_Condition		condition,
		 SbInputId	 		id)
/*-----------------------------------------------------------------------+*/
{
  SPC_Callback_Struct *structptr = NULL;

  _DtSvcProcessLock();
  if (SPC_Fd_Mapping == NULL) {
    SPC_Fd_Mapping = (SPC_Callback_Struct **) 
	             XeMalloc (FD_SETSIZE * sizeof (SPC_Callback_Struct *));
    memset(SPC_Fd_Mapping, 0, FD_SETSIZE * sizeof(SPC_Callback_Struct *));
  }
  structptr=SPC_LOOKUP_FD_MAPPING(source);

  if(!structptr) {
    structptr=(SPC_Callback_Struct *) XeMalloc(sizeof(SPC_Callback_Struct));
    SPC_LOOKUP_FD_MAPPING(source)=structptr;
  }
  _DtSvcProcessUnlock();

  switch (condition) {

  case SPC_Input:
  case SPC_Terminator:
  case SPC_Client:
    structptr->read_id   = id;
    break;

  case SPC_Exception:
    structptr->except_id = id;
    break;

  default:
    break;
  }

  return(source);
}


/*-----------------------------------------------------------------------+*/
static SbInputId SPC_RemoveInput(int 			 source,
			  SPC_Callback_Condition condition)
/*-----------------------------------------------------------------------+*/
{
  SPC_Callback_Struct *structptr = NULL;
  
  _DtSvcProcessLock();
  if (SPC_Fd_Mapping == NULL) {
    SPC_Fd_Mapping = (SPC_Callback_Struct **) 
		     XeMalloc (FD_SETSIZE * sizeof (SPC_Callback_Struct *));
    memset(SPC_Fd_Mapping, 0, FD_SETSIZE * sizeof(SPC_Callback_Struct *));
  }
  structptr=SPC_LOOKUP_FD_MAPPING(source);
  _DtSvcProcessUnlock();

  switch(condition) {
    
  case SPC_Input:
  case SPC_Terminator:
  case SPC_Client:
     return structptr->read_id;

  case SPC_Exception:
     return structptr->except_id;
    
  }

  return 0;
}
  
/*-----------------------------------------------------------------------+*/
static SPC_Select(void )
/*-----------------------------------------------------------------------+*/
{
    break_on_termination=TRUE;

    _DtSvcProcessLock();
    spc_xe_termination_flag= FALSE;

    /* Use a function pointer so we don't have explict dependancy */
    /* on libXe.a						    */
    /* ---------------------------------------------------------- */
    if (SbMainLoopUntil_hookfn == NULL)
      (void) fprintf (stderr, "Error: SbMainLoopUntil = NULL\n");
    else
      (*SbMainLoopUntil_hookfn)(&spc_xe_termination_flag);
    _DtSvcProcessUnlock();

    break_on_termination=FALSE;
    return(TRUE);
}


/*-----------------------------------------------------------------------+*/
int SPC_Wait_For_Termination(SPC_Channel_Ptr channel)
/*-----------------------------------------------------------------------+*/
{
  int result;
  
  call_parent_method(channel, wait_for_termination, (channel), result);
  
  if(result==SPC_ERROR) return(SPC_ERROR);

  do {

    if(SPC_Select() == SPC_ERROR)
      return(SPC_ERROR);
    
  } while(IS_ACTIVE(channel));
  
  return(TRUE);
  
}


/*-----------------------------------------------------------------------+*/
void SPC_XtBreak(void)
/*-----------------------------------------------------------------------+*/
{
  _DtSvcProcessLock();
  if(!spc_xe_termination_flag) {
     spc_xe_termination_flag = TRUE;
     if (SbBreakMainLoop_hookfn == NULL)
       (void) fprintf (stderr, "Error: SbBreakMainLoop = NULL\n");
     else
       (*SbBreakMainLoop_hookfn)();      
  } 
  _DtSvcProcessUnlock();
}


  
/*-----------------------------------------------------------------------+*/
void SPC_XtAddInput(SPC_Channel_Ptr channel,
		    int *id_addr,
		    int fd,
		    spc_handler_func_type handler,
		    SPC_Callback_Condition condition)
/*-----------------------------------------------------------------------+*/
{
  SbInputId id;
  
  switch(condition) {
    
  case SPC_Input:
  case SPC_Terminator:
  case SPC_Client:
    /* fprintf(stderr, "SPC add input/terminator for %d\n", fd); */
    if (SbAddInput_hookfn == NULL)
      (void) fprintf (stderr, "Error: SbAddInput = NULL\n");
    else
      id = (*SbAddInput_hookfn)(fd, handler, channel);
    break;
    
  case SPC_Exception:
    /* fprintf(stderr, "SPC add exception for %d\n", fd); */
    if (SbAddException_hookfn == NULL)
      (void) fprintf (stderr, "Error: SbAddException = NULL\n");
    else
      id = (*SbAddException_hookfn)(fd, handler, channel);
    break;
    
  }
  
  *id_addr=SPC_AddInput(fd, condition, id);
}


/*-----------------------------------------------------------------------+*/
void SPC_XtRemoveInput(int *id_addr, 
		       SPC_Callback_Condition condition)
/*-----------------------------------------------------------------------+*/
{
  if((*id_addr) != -1) {
    
    switch(condition) {
      
    case SPC_Input:
    case SPC_Terminator:
    case SPC_Client:
      /* fprintf(stderr, "SPC remove input/terminator\n"); */
      if (SbRemoveInput_hookfn == NULL)
        (void) fprintf (stderr, "Error: SbRemoveInput = NULL\n");
      else
        (*SbRemoveInput_hookfn)(SPC_RemoveInput(*id_addr, condition));
      break;
      
    case SPC_Exception:
      /* fprintf(stderr, "SPC remove exception"); */
      if (SbRemoveException_hookfn == NULL)
        (void) fprintf (stderr, "Error: SbRemoveException = NULL\n");
      else
        (*SbRemoveException_hookfn)(SPC_RemoveInput(*id_addr, condition));
      break;
      
    }
    
    *id_addr=(-1);
  }
}
