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
/* $TOG: ActionTt.c /main/12 1999/09/16 14:56:00 mgreess $ */
/* 
 * (c) Copyright 1997, The Open Group 
 */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:         ActionTt.c
 **
 **   Project:      CDE Execution Management
 **
 **   Description:  This file contains the Tooltalk portions of the 
 **                 action library source code.
 **
 **
 **       by Hewlett-Packard Company
 **
 **
 ** (c) Copyright 1993, 1994 Hewlett-Packard Company
 ** (c) Copyright 1993, 1994 International Business Machines Corp.
 ** (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 ** (c) Copyright 1993, 1994 Novell, Inc.
 ****************************************************************************
 ************************************<+>*************************************/

/******************************************************************************
 *
 * TT_Message Key Assignments For Attached Data  (please note additions here!)
 *
 *    0 - client data                                    5/94  --tg
 *    1 - ms_timeout                                     7/94  --tg
 *    2 - invocation id    - currently unused
 *    3 - child id         - currently unused
 *
 *****************************************************************************/

/*LINTLIBRARY*/
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>

#ifdef _SUN_OS /* Need this for the strtod () call */
#include <floatingpoint.h>
#endif /* _SUN_OS */

#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include <X11/Intrinsic.h>

#include <Dt/DtP.h>
#include <Dt/Dts.h>
#include <Dt/Help.h>
#include <Dt/Message.h>
#include <Dt/Connect.h>
#include <Dt/Indicator.h>
#include <Dt/DtNlUtils.h>
#include <Dt/CommandM.h>
#include <Dt/Utility.h>
#include <Dt/SvcTT.h>
#include <Dt/Service.h>

#include <Dt/ActionUtilP.h>
#include <Dt/ActionDb.h>
#include <Dt/ActionFind.h>
#include <Tt/tttk.h>

#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/BulletinB.h>
#include <Xm/DialogS.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/TextF.h>
#include <Xm/SeparatoG.h>
#include <Xm/PushBG.h>
#include <Xm/MessageB.h>
#include <Xm/MwmUtil.h>

#include <Dt/ActionP.h>
#include <Dt/Action.h>
#include <Dt/EnvControlP.h>

#include "myassertP.h"
#include "DtSvcLock.h"

/********    Public Function Declarations    ********/
void _DtProcessTtRequest(
                        Widget w,
                        ActionRequest *request,
                        char * relPathHost,
                        char * relPathDir ) ;

Tt_status _DtInitializeToolTalk(Widget w);

extern void _DtCreateErrorDialog( 
                        Widget w,
                        char * actionName,
                        XmString msg) ;
extern Boolean _DtCompileMessagePiece(
                        Widget w,
                        ActionRequest *request,
                        char * relPathHost,
                        char * relPathDir,
                        parsedMsg * piece,
                        Boolean initialize,
                        unsigned long processingMask,
			Boolean ** paramUsed,
			int * promptDataIndex ) ;
extern ActionRequest * _DtCloneRequest (
                        ActionRequest * request) ;
extern void _DtFreeRequest( 
                        register ActionRequest *request) ;
/********    End Public Function Declarations    ********/



/********    Static Function Declarations    ********/

static Boolean ResolveTtRequestMessagePieces(
                        Widget w,
                        ActionRequest *request,
                        char * relPathHost,
                        char * relPathDir ) ;

static void InitiateTtRequest(
                        Widget w,
                        ActionRequest *request ) ;

static Tt_callback_action TtRequestCallbackHandler(
                        Tt_message message,
                        Tt_pattern pattern ) ;

static char * WrapMessageLines( 
                        char * errorMsg ) ;

static void ReportToolTalkError(
                        Widget w,
                        String actionName,
                        String errorMsg) ;

static Tt_callback_action _DtActReceiveExecId(
                        Tt_message msg,
                        Tt_pattern pat );
static Tt_callback_action _DtActReceiveDoneMsg(
                        Tt_message msg,
                        Tt_pattern pat );
static Tt_message _DtTtContractIgnoreMsgCB(
	Tt_message	msg,
	void		*clientdata,
	Tt_message	contract);

/********    End Static Function Declarations    ********/



/* Pointers to localizable strings */
/******************************************************************************
 RWV:
     These Strings are static to the Action.c File.  Defining these here
     creates a new strings static to this file which are never initialized.
     References to these uninitialized strings by the GETMESSAGE11 macro
     will cause a core dump.
     
     For the short term, to keep things running  -- I've marked those
     strings actually used in this file as external both here and in
     Action.c.  This violates naming conventions and the desire to keep
     global symbols to a minimum.  We should come up with a permanent
     solution for this problem later.

static String PromptDialogTitle;
static String ErrorPostfix;
static String PromptDialogLabel;
static String ContinueMessage;
static String HostErrorMsg;
static String HostErrorMsg2;
static String NoActionMsg;
static String MapErrorMsg;
static String ArgumentErrorMsg;
static String NoActionMsg2;
static String InvalidFileMsg;
static String MultiHostErrorMsg;
static String IcccmReqErrorMsg;
static String NoToolTalkConnMsg;
**************************************************************************/
extern String ToolTalkErrorMsg;
extern String ToolTalkErrorMsg2;
extern String TtFileArgMapErr;


/*****************************************************************************
 *
 * Routines to field message patterns from dtexec
 *
 *****************************************************************************/

static Tt_callback_action _DtActReceiveExecId( 
    Tt_message msg,
    Tt_pattern pat )
{
   int invId;
   int childId;
   char *procId;
   _DtActChildRecT *childp;

   switch(tt_message_arg_ival(msg,0,&invId))
   {
       case TT_OK: /* got invocation Id (arg 0 value ) */ 
           break;
       case TT_ERR_NOMP:	/* tt_session_not_running */
       case TT_ERR_NUM:		/* integer value out of range */
       case TT_ERR_POINTER:	/* pointer does not point to correct object */
           return  TT_CALLBACK_CONTINUE;
           break;
       default:
           return  TT_CALLBACK_CONTINUE;
           break;
   }
   switch(tt_message_arg_ival(msg,1,&childId) )	/* child Id (arg 1 value ) */ 
   {
       case TT_OK: /* got child Id (arg 1 value ) */ 
           break;
       case TT_ERR_NOMP:	/* tt_session_not_running */
       case TT_ERR_NUM:		/* integer value out of range */
       case TT_ERR_POINTER:	/* pointer does not point to correct object */
           return  TT_CALLBACK_CONTINUE;
           break;
       default:
           return  TT_CALLBACK_CONTINUE;
           break;
   }
   switch ( tt_ptr_error(procId = tt_message_arg_val(msg,2)) )
   {
       case TT_OK: /* got dtexec's proc Id (arg 2 value ) */ 
           break;
       case TT_ERR_NOMP:	/* tt_session_not_running */
       case TT_ERR_NUM:		/* integer value out of range */
       case TT_ERR_POINTER:	/* pointer does not point to correct object */
           return  TT_CALLBACK_CONTINUE;
           break;
       default:
           return  TT_CALLBACK_CONTINUE;
           break;
   }
   
   /*
    * Save the proc Id for dtexec returned in this message
    */
   childp = _DtActFindChildRec( invId, childId );
   myassert(childp);  /* we should be able to find the child record */
 
   if ( childp )
   {
       /*
        * This should be a command action
        */
       myassert( IS_CMD(childp->mask) );
       childp->u.cmd.TTProcId = XtNewString( procId );

       /*
        * Note that the child HAS identified itself.
        */
       if (childp->childState == _DtActCHILD_ALIVE_UNKNOWN)
           childp->childState = _DtActCHILD_ALIVE;
   }

   tt_message_reply(msg);
   tttk_message_destroy(msg);
   tt_free(procId);
   return TT_CALLBACK_PROCESSED;
}

static Tt_callback_action _DtActReceiveDoneMsg( 
               Tt_message msg,
               Tt_pattern pat )
{
   int              invId;
   int              childId;
   _DtActChildRecT *childp;
   DtActionArg     *retArgv;    /* returnable arguments */ 
   int		    retArgc;	/* returnable argument count */
   int              doneCode;	/* returnable _DtActCHILD_ code */

   switch(tt_message_arg_ival(msg,0,&invId))
   {
       case TT_OK: /* got invocation Id (arg 0 value ) */ 
           break;
       case TT_ERR_NOMP:	/* tt_session_not_running */
       case TT_ERR_NUM:		/* integer value out of range */
       case TT_ERR_POINTER:	/* pointer does not point to correct object */
           return  TT_CALLBACK_CONTINUE;
           break;
       default:
           return  TT_CALLBACK_CONTINUE;
           break;
   }

   switch(tt_message_arg_ival(msg,1,&childId) )	/* child Id (arg 1 value ) */ 
   {
       case TT_OK: /* got child Id (arg 1 value ) */ 
           break;
       case TT_ERR_NOMP:	/* tt_session_not_running */
       case TT_ERR_NUM:		/* integer value out of range */
       case TT_ERR_POINTER:	/* pointer does not point to correct object */
           return  TT_CALLBACK_CONTINUE;
           break;
       default:
           return  TT_CALLBACK_CONTINUE;
           break;
   }

   switch(tt_message_arg_ival(msg,2,&doneCode) ) /* done code (arg 2 value ) */ 
   {
       case TT_OK: /* got done code (arg 2 value ) */ 
           break;
       case TT_ERR_NOMP:	/* tt_session_not_running */
       case TT_ERR_NUM:		/* integer value out of range */
       case TT_ERR_POINTER:	/* pointer does not point to correct object */
           return  TT_CALLBACK_CONTINUE;
           break;
       default:
           return  TT_CALLBACK_CONTINUE;
           break;
   }

   /*
    * In some cases, dtexec may send a _DtActDtexecDone(Request) but
    * be under pressure to go down ASAP and not wait around for a Reply.
    */
   tt_message_reply(msg);
   tttk_message_destroy(msg);

   childp = _DtActFindChildRec( invId, childId );
   myassert(childp);  /* we should be able to find the child record */

   /*
    * Init return args incase there is not a childp.
    */
   retArgv = NULL;
   retArgc = 0;

   if (childp)
   {
       childp->childState = doneCode;	/* usually _DtActCHILD_DONE */
       retArgc = _DtActGetCmdReturnArgs(invId,childp,&retArgv);
   }
   _DtActExecutionLeafNodeCleanup(invId, retArgv, retArgc, True);

   return TT_CALLBACK_PROCESSED;
}


/******************************************************************************
 ******************************************************************************
 *
 * Routine to query DtActionQuit() behavior if done now.
 *
 * DtActionStatus
 * DtActionQuitType(
 *         DtActionInvocationID    id,
 *         int                     silent)
 *
 * This function has been obsoleted.  See revision
 * number 1.6 for the implementation.
 *
 ******************************************************************************
 *****************************************************************************/

/******************************************************************************
 ******************************************************************************
 *
 * Routines to quit actions.
 *
 * static Tt_callback_action
 * _DtActQuitCB(
 *     Tt_message message,
 *     Tt_pattern pattern)
 *
 * This function has been obsoleted.  See revision
 * number 1.6 for the implementation.
 *
 ******************************************************************************
 *****************************************************************************/

/******************************************************************************
 *
 * Routine used to Quit a specific action.  Note
 * that status is returned via the childState
 * flag.
 *
 * static void
 * _DtActionQuitChild(
 * 	DtActionInvocationID   id,
 * 	_DtActChildRecT       *childRecP,
 * 	int                    silent,
 * 	int                    force,
 * 	XtAppContext           context,
 * 	int                    ms_timeout)
 *
 * This function has been obsoleted.  See revision
 * number 1.6 for the implementation.
 *
 ******************************************************************************/

/******************************************************************************
 *
 * Public API
 *
 * Routine used to Quit all actions associated with
 * a DtActionInvocationID.   CMD and TT actions will
 * be quit-able using the same code.
 *
 * DtActionStatus
 * DtActionQuit(
 *	DtActionInvocationID	id,
 *	unsigned long		ms_timeout,
 *	int			silent)
 *
 * This function has been obsoleted.  See revision
 * number 1.6 for the implementation.
 *
 ******************************************************************************/

/******************************************************************************
 ******************************************************************************
 *
 * Error Dialog Code
 *
 ******************************************************************************
 *****************************************************************************/
static void
ReportToolTalkError(
        Widget w,
        String actionName,
        String errorMsg )

{
   XmString msg;
   char * buf;

   if (errorMsg)
   {
      buf = XtMalloc(strlen(ToolTalkErrorMsg2) + strlen(errorMsg) + 10);
      sprintf(buf, ToolTalkErrorMsg2, errorMsg);
   }
   else
      buf = XtNewString(ToolTalkErrorMsg);
   msg = XmStringCreateLocalized(buf);
   _DtCreateErrorDialog(w, actionName, msg);
   XmStringFree(msg);
   XtFree(buf);
}



/******************************************************************************
 ******************************************************************************
 *
 * ToolTalk Specific Functions
 *
 ******************************************************************************
 *****************************************************************************/

/******************************************************************************
 *
 * This is the entry point into the ToolTalk message world.  All of the code
 * before this has been written to handle any of the different transport
 * types.  The code from this point on will know specifically how to
 * process a ToolTalk message.  We will start by taking each
 * of the pieces of information making up the ToolTalk message, and
 * resolving any of the keywords, by replacing them with the appropriate
 * information.  If this fails (which it only should do if we try to map
 * a file to a host which cannot be accessed), then we will terminate the
 * request, posting an error dialog for the user.  Unlike a Command Invoker
 * request, where we may try several times to handle it (once for each
 * of the specified execution hosts), a ToolTalk message is handled only
 * once.
 */
void 
_DtProcessTtRequest(
        Widget w,
        ActionRequest *request,
        char * relPathHost,
        char * relPathDir)

{
   ActionPtr action = request->clonedAction;

   if (ResolveTtRequestMessagePieces(w, request, relPathHost, relPathDir))
   {
      /* 
       * Issue the request; the success/failure notification comes 
       * asynchronously; that's when everything gets cleaned up.
       */
      InitiateTtRequest( w, request );
   }
   else
   {
      /* Display error dialog */
      /*
       * fdt: there really needs to be some policy defining where an error
       * message will be returned.  Also, is this really the right message
       * to display here?  Was the failure really caused by a file mapping
       * problem?  In fact, is this a case we even can get to???? [Yes]
       */
      /* fdt: this string must be localized */
      ReportToolTalkError(w, request->clonedAction->label, TtFileArgMapErr);
   }
}


/******************************************************************************
 *
 * This function takes all of the pieces making up a ToolTalk message,
 * and resolves any references to keywords, using both the passed-in
 * arguments, and any information collected from the prompt dialog.
 */
static Boolean 
ResolveTtRequestMessagePieces(
        Widget w,
        ActionRequest *request,
        char * relPathHost,
        char * relPathDir )

{
   ActionPtr action = request->clonedAction;
   tt_msgAttr * tt = &(action->u.tt_msg);
   int i;
   Boolean success;
   Boolean * paramUsed = NULL;
   int promptDataIndex = 0;

   if (_DtCompileMessagePiece(w, request, relPathHost, relPathDir,
                           &(tt->tt_op), True, 0, &paramUsed, &promptDataIndex)
        &&
       _DtCompileMessagePiece(w, request, relPathHost, relPathDir,
                           &(tt->tt_file), False, 0, &paramUsed, &promptDataIndex))
   {
      for (i = 0, success = True; (i < tt->value_count) && success; i++)
      {
         success = _DtCompileMessagePiece(w, request, relPathHost, relPathDir,
                           &(tt->tt_argn_value[i]), False, _DTAct_TT_ARG, &paramUsed, &promptDataIndex);
      }
      for (i = 0; (i < tt->vtype_count) && success; i++)
      {
         success = _DtCompileMessagePiece(w, request, relPathHost, relPathDir,
                           &(tt->tt_argn_vtype[i]), False, _DTAct_TT_VTYPE, &paramUsed, &promptDataIndex);
      }

      if (success)
      {
         /* We must have at least the op strings */
         if ((tt->tt_op.compiledMessage) && 
             (strlen(tt->tt_op.compiledMessage) > 0))
         {
	   XtFree(paramUsed);
	   return(True);
         }
      }
   }

   /* ERROR: Free up any intermediate work we've done here */
   XtFree(tt->tt_op.compiledMessage);
   XtFree(tt->tt_file.compiledMessage);
   tt->tt_op.compiledMessage = NULL;
   tt->tt_file.compiledMessage = NULL;
   for (i = 0; i < tt->value_count; i++)
   {
      XtFree(tt->tt_argn_value[i].compiledMessage);
      tt->tt_argn_value[i].compiledMessage = NULL;
   }
   for (i = 0; i < tt->vtype_count; i++)
   {
      XtFree(tt->tt_argn_vtype[i].compiledMessage);
      tt->tt_argn_vtype[i].compiledMessage = NULL;
   }
   XtFree(paramUsed);
   return(False);
}

/******************************************************************************
 ******************************************************************************
 *
 * Routines to handle Tooltalk responses.
 *
 ******************************************************************************
 *****************************************************************************/

/******************************************************************************
 *
 * Translate a ttmedia_load() callback into a regular
 * looking DtActionCallbackProc() for the user.
 */
Tt_message
Ttmedia_to_Dt_StatusUpdateCB(
    Tt_message     message,
    void           *clientdata,
    Tttk_op        op,
    unsigned char  *contents,
    int            len,
    char           *file)
{
    CallbackData *data;
    char * errorMsg;
    Tt_state state;
    Tt_status status;
    Boolean wrapMessageLines = False;
    _DtActChildRecT *childRec;
    _DtActInvRecT *invRec;
    DtActionInvocationID id;
    DtActionArg *newArgp = NULL;	/* hanger for returned data if any */
    int  newArgc = 0;
    unsigned long evalStatus;
    DtActionStatus userStatus;
    int i, j, upIdx;
    char *upVType;


    status = (Tt_status) tt_message_status(message);
    state  = (Tt_state) tt_message_state(message);

    if (state == TT_STARTED) {
	/*
	 * Handler is just getting started.  Eat it.
	 */
	tttk_message_destroy( message );
	return( (Tt_message) NULL );           /* like TT_CALLBACK_PROCESSED */
    }

    if ( (state != TT_HANDLED) && (state != TT_FAILED) &&
	 (state != TT_RETURNED) && (state != TT_SENT)      )
    {
	/*
	 * This address space is probably the handler.  Pass
	 * on it, so that the handler gets a chance to handle it.
	 */
	return( (Tt_message) message );         /* like TT_CALLBACK_CONTINUE */
    }

    /*
     * Process a TT_FAILED, TT_HANDLED, TT_RETURNED or TT_SEND related to our
     * original request.
     */

    /*
     * Use ttmedia's client data capability since it can associate our
     * original client data with any reply *or* new request associated
     * with the original request.
     *
     * data = (CallbackData *) tt_message_user( message, 0 );   not good enough
     */
    data = (CallbackData *) clientdata;

    id = data->actInvId;
    invRec = _DtActFindInvRec( id );
    myassert(invRec);
    childRec = _DtActFindChildRec( id, data->childId );

    if (state == TT_FAILED) {
	if (status != TT_DESKTOP_ECANCELED) {
	    /*
	     * Determine whether ToolTalk or the receiver failed the message
	     */
	    if (status < TT_ERR_LAST) {
		errorMsg = tt_status_message(status);
		wrapMessageLines = True;
	    }
	    else {
		errorMsg = tt_message_status_string(message);
	    }

	    if ((tt_pointer_error(errorMsg) == TT_OK) && errorMsg) {
		if (wrapMessageLines)
		    errorMsg = WrapMessageLines(errorMsg);
		ReportToolTalkError(data->associatedWidget, 
					data->actionLabel, errorMsg);
		if (wrapMessageLines)
		    tt_free(errorMsg);
	    }
	    else {
		ReportToolTalkError(data->associatedWidget, 
					data->actionLabel, NULL);
	    }
	}
    }
    else /* if (state == TT_HANDLED) or other things (?) */ {
	/*
	 * We have a possible update - see if user wants arg back
	 */

	newArgc = invRec->ac;
	newArgp = _DtActMallocEmptyArgArray( newArgc );

	/*
	 * Our only return argument is TT_ARG0_VALUE or TT_FILE.
	 *
	 * If a docname went out, it went out as TT_IN, so we
	 * won't need to push it back up.
	 *
	 * We will lean on the fact that we carefully selected
	 * what action definitions were run through the ttmedia
	 * machinery, and that we can assume from the ttmedia
	 * reply where the arguments go back.
	 */
	i = 0;	/* arg 0 */
	if (file == NULL) {
	    /*
	     * DtACTION_BUFFER - possible update.
	     */

	    /*
	     * Calculate index into original argument list from the
	     * user.
	     */
	    upIdx = childRec->argMap[i].argIdx;

	    if ( upIdx != -1 ) {
		if ( (IS_BUFFER_OBJ(invRec->info[upIdx].mask) &&
		      IS_WRITE_OBJ(invRec->info[upIdx].mask))    ) {
		    /*
		     * Need to push this object up.
		     */

		    /*
		     * Determine VTYPE
		     */
		    upVType = tt_message_arg_type( message, i );

		    newArgp[upIdx].argClass = DtACTION_BUFFER;
		    newArgp[upIdx].u.buffer.size = len;
		    newArgp[upIdx].u.buffer.type = XtNewString(upVType);
		    newArgp[upIdx].u.buffer.writable = 1;

		    newArgp[upIdx].u.buffer.bp = (void *) XtMalloc(len);
		    memcpy( newArgp[upIdx].u.buffer.bp, contents, len );

		    tt_free(upVType);
		}
	    }

	    /*
	     * Done with the data.
	     */
	    tt_free((char *) contents);
	}
	else {
	    /*
	     * DtACTION_FILE
	     */

	    /*
	     * Calculate index into original argument list from the
	     * user.  Notice that we're shipping data because of TT_FILE
             * and not TT_ARG0_FILE.  To do this, argMap is arranged
	     * as:
	     *      argMap[ ARG0, ... , ARGn, TT_FILE].argIdx
	     *
	     * The array-index for TT_FILE is *one beyond* all the normal
	     * TT_ARGn_VALUE array-index values.   The .argN for TT_FILE
	     * will be -1 fyi.
	     *
	     * As the spec says, the action service "may choose to"
	     * return objects, which is why it is o.k. to return
	     * their "TT_FILE %Arg_n%" value for less than obvious
	     * reasons.
	     */
	    upIdx = childRec->argMap[i+1].argIdx;

	    if ( upIdx != -1 ) {
		newArgp[upIdx].argClass = DtACTION_FILE;
		newArgp[upIdx].u.file.name = XtNewString( file );
	    }

	    tt_free(file);
	}
    }

    /*
     * If a final response to our original message, we're done.
     */
    if ((message == childRec->u.tt.reqMessage) && (state == TT_HANDLED))
	childRec->childState = _DtActCHILD_DONE;
    else if ((message == childRec->u.tt.reqMessage) && (state == TT_FAILED)) {
	if (status == TT_DESKTOP_ECANCELED)
	    childRec->childState = _DtActCHILD_CANCELED;
	else
	    childRec->childState = _DtActCHILD_FAILED;
    }

    /*
     * If this was a deposit, reply that we've handled it.
     */
    if (op == TTME_DEPOSIT) {
	tt_message_reply( message );
	/*
	 * Cleanup here since this message is not part of our long
	 * term storage.
	 */
	tttk_message_destroy( message );
    }

    /*
     * Cleanup message - handled by _DtActExecutionLeafNodeCleanup()
     *
     * tttk_message_destroy( message );
     */

    _DtActExecutionLeafNodeCleanup( id, newArgp, newArgc, 1 );

    return( (Tt_message) NULL );	/* message consumed */
}


/******************************************************************************
 *
 * Guess at the REP_TYPE for a given argument.
 * String is just a variation of a buffer, so
 * DtACT_TT_REP_BUFFER is returned for buffers and
 * strings.
 */
static int _DtAct_tt_message_arg_reptype( Tt_message message, int arg )
{

    int       testVal;
    Tt_status status;

    status = tt_message_arg_ival( message, arg, &testVal );

    if (status == TT_OK)
	return( DtACT_TT_REP_INT);
    else if (status == TT_ERR_NUM)
	return( DtACT_TT_REP_BUFFER );
}

/******************************************************************************
 *
 * Translate a free-form request callback into a
 * regular looking DtActionCallbackProc() for the user.
 */
static Tt_callback_action 
TtRequestCallbackHandler(
        Tt_message message,
        Tt_pattern pattern )

{
    CallbackData *data;
    char * errorMsg;
    Tt_state state;
    Tt_status status;
    Boolean wrapMessageLines = False;
    _DtActChildRecT *childRec;
    _DtActInvRecT *invRec;
    DtActionInvocationID id;
    unsigned long evalStatus;
    DtActionStatus userStatus;
    DtActionArg *newArgp = NULL;	/* hanger for returned data if any */
    int  newArgc = 0, totalArgs, i, j, upIdx;
    int upArgClass;
    int argRepType;
    char *upttbuf;
    int upttbuflen, ivalue;
    char *upVType, *upVType2;


    status = (Tt_status) tt_message_status(message);
    state  = (Tt_state) tt_message_state(message);

    if (state == TT_STARTED) {
	/*
	 * Handler is just getting started.  Eat it.
	 */
	return TT_CALLBACK_PROCESSED;
    }

    if ((state != TT_HANDLED) && (state != TT_FAILED) && (state != TT_RETURNED))
    {
	/*
	 * This address space is probably the handler.  Pass
	 * on it, so that the handler gets a chance to handle it.
	 */
	return TT_CALLBACK_CONTINUE;
    }

    /*
     * Process a TT_FAILED, TT_HANDLED or TT_RETURNED related to our
     * original request.
     */
    if ( !(data = (CallbackData *) tt_message_user( message, 0 )) ) {
	myassert(data);  /* data should always be non-null */
        return( (Tt_callback_action) TT_CALLBACK_PROCESSED ); 
    }

    id = data->actInvId;
    invRec = _DtActFindInvRec( id );
    myassert(invRec);
    childRec = _DtActFindChildRec( id, data->childId );
    myassert(childRec);

    if (state == TT_FAILED) {
	if (status != TT_DESKTOP_ECANCELED) {
	    /*
	     * Determine whether ToolTalk or the receiver failed the message
	     */
	    if (status < TT_ERR_LAST) {
		errorMsg = tt_status_message(status);
		wrapMessageLines = True;
	    }
	    else {
		errorMsg = tt_message_status_string(message);
	    }

	    if ((tt_pointer_error(errorMsg) == TT_OK) && errorMsg) {
		if (wrapMessageLines)
		    errorMsg = WrapMessageLines(errorMsg);
		ReportToolTalkError(data->associatedWidget,
					data->actionLabel, errorMsg);
		tt_free(errorMsg);
	    }
	    else {
		ReportToolTalkError(data->associatedWidget, 
					data->actionLabel, NULL);
	    }
	}
    }
    else if (state == TT_HANDLED) {
	/*
	 * We have an update
	 */
	newArgc = invRec->ac;
	newArgp = _DtActMallocEmptyArgArray( newArgc );

        totalArgs = tt_message_args_count(message);

	/*
	 * Look through all the arguments returned in the request.
	 */
	for ( i = 0; i < totalArgs; i++ ) {
	    /*
	     * Look through the child's arg list to see which ones need to
	     * be returned.
	     */
	    upIdx = childRec->argMap[i].argIdx;

	    if ( upIdx != -1 ) {
		    if ( (IS_BUFFER_OBJ(invRec->info[upIdx].mask) &&
			  IS_WRITE_OBJ(invRec->info[upIdx].mask))   ||
			  IS_FILE_OBJ(invRec->info[upIdx].mask)        ) {
			/*
			 * Need to push this object up.
			 */

			/*
			 * Determine argClass
			 */
			if ( IS_BUFFER_OBJ(invRec->info[upIdx].mask) )
			    upArgClass = DtACTION_BUFFER;
			else if ( IS_FILE_OBJ(invRec->info[upIdx].mask) )
			    upArgClass = DtACTION_FILE;
			else if ( IS_STRING_OBJ( invRec->info[upIdx].mask) )
			    upArgClass = DtACTION_STRING;
			else
			    upArgClass = DtACTION_NULLARG; /* error situation */

			/*
			 * Determine VTYPE
			 */
			upVType = tt_message_arg_type( message, i );

			/*
			 * Determine REP_TYPE
			 */
			argRepType = _DtAct_tt_message_arg_reptype(message, i);

			/*
			 * If we think TT_REP_BUFFER so far, but don't have
			 * an argClass of DtACTION_BUFFER, then we really have
			 * a DtACT_TT_REP_STRING.
			 */
			if ( (argRepType == DtACT_TT_REP_BUFFER) &&
				 !(upArgClass == DtACTION_BUFFER) )
			    argRepType = DtACT_TT_REP_STRING;

			/*
			 * Do appropriate unpacking.
			 */
			switch (argRepType) {
			    case DtACT_TT_REP_BUFFER:
			    case DtACT_TT_REP_STRING:
				/*
				 * By convention, if we're here, the buffer
				 * is writable.   Also, STRINGs can be
				 * fetched using bval.
				 */
				status = tt_message_arg_bval( message, i,
					 (unsigned char **) &upttbuf,
					 &upttbuflen );

				if ( status != TT_OK ) {
				    /*
				     * Give up - something bad happened.
				     */
				    upArgClass = DtACTION_NULLARG;
				}
				break;
			    case DtACT_TT_REP_INT:
				/*
				 * Unpack an integer.
				 */
				status = tt_message_arg_ival( message, i,
								 &ivalue );
				if ( status != TT_OK ) {
				    upArgClass = DtACTION_NULLARG;
				}
				else {
				    /*
				     * Fake up enough information that we
				     * could return the argument in either
				     * a DtACTION_FILE or DtACTION_BUFFER
				     * argClass argument.
				     */
				    upttbuflen = 64;
				    upttbuf = XtMalloc( upttbuflen );
				    sprintf( upttbuf, "%u", ivalue );
				}
				break;
			    case DtACT_TT_REP_UNDEFINED:
				/*
				 * Cannot figure out how to unpack arg.
				 */
				upArgClass = DtACTION_NULLARG;
				break;
			}

			switch (upArgClass) {
			    case DtACTION_NULLARG:
				/*
				 * We failed above, so give up.
				 */
				break;
			    case DtACTION_BUFFER:
				/*
				 * Convert from tt_free()-able to
				 * XtFree()-able.
				 */
				newArgp[upIdx].argClass = upArgClass;
				newArgp[upIdx].u.buffer.size = upttbuflen;
				newArgp[upIdx].u.buffer.type = XtNewString(
								upVType);
				newArgp[upIdx].u.buffer.writable = 1;

				newArgp[upIdx].u.buffer.bp = (void *)
						XtMalloc( upttbuflen );
				memcpy( newArgp[upIdx].u.buffer.bp, upttbuf,
						upttbuflen );

				if (argRepType == DtACT_TT_REP_INT) {
				    /*
				     * upttbuf was XtMalloc'ed above
				     */
				    XtFree( upttbuf );
				    upttbuf = 0;
				}
				break;
			    case DtACTION_FILE:
				/*
				 * Convert from tt_free()-able to
				 * XtFree()-able.
				 */
				newArgp[upIdx].argClass = upArgClass;
				newArgp[upIdx].u.file.name = XtNewString(
						upttbuf );

				if (argRepType == DtACT_TT_REP_INT) {
				    /*
				     * upttbuf was XtMalloc'ed above
				     */
				    XtFree( upttbuf );
				    upttbuf = 0;
				}
				break;
			}
	 	    }
		    tt_free( upVType );
		    if (upttbuf != 0) {
			tt_free( upttbuf );
		    }
	    }
	}
    }

    /*
     * If a final response to our original message, we're done.
     */
    if ((message == childRec->u.tt.reqMessage) && (state == TT_HANDLED))
	childRec->childState = _DtActCHILD_DONE;
    else if ((message == childRec->u.tt.reqMessage) && (state == TT_FAILED)) {
	if (status == TT_DESKTOP_ECANCELED)
	    childRec->childState = _DtActCHILD_CANCELED;
	else
	    childRec->childState = _DtActCHILD_FAILED;
    }

    /*
     * Message is destroyed by _DtActExecutionLeafNodeCleanup()
     */

    _DtActExecutionLeafNodeCleanup( id, newArgp, newArgc, 1 );

    return( (Tt_callback_action) TT_CALLBACK_PROCESSED ); /* message consumed */
}

/******************************************************************************
 ******************************************************************************
 *
 * Process a ToolTalk request.
 *
 ******************************************************************************
 *****************************************************************************/

/******************************************************************************
 *
 * _DtTtContractIgnoreMsgCB
 *
 *	Contract callback to ignore all messages received.  This is consume
 *	status messages sent back by some message handlers.  If these messages
 *	aren't consumed by the library, then a tooltalk client can become
 *	confused and also spurious error messages are written to syslog or the
 *	system console.
 ******************************************************************************/

static Tt_message
_DtTtContractIgnoreMsgCB(
	Tt_message	msg,
	void		*clientdata,
	Tt_message	contract)
{
    /*
     * Artificially consume unwanted messages.
     */
    tttk_message_abandon(msg);
    return NULL;
}


static void 
InitiateTtRequest(
	Widget         w,
	ActionRequest  *request )

{
    CallbackData *data;
    ActionPtr action = request->clonedAction;
    tt_msgAttr * tt = &(action->u.tt_msg);
    int i;
    char * p;
    Tt_message message;
    int destroy_message = 0;
    int mode;
    int repType;
    int intValue;
    char * value;
    char * vtype;
    char * errorMsg;
    Tt_status status;
    unsigned char * contents;
    int length;
    char * file;
    char * docname;
    Tt_pattern *subcon_patterns;
    _DtActInvRecT *actInvRecP;
    _DtActChildRecT *actChildRecP;
    unsigned long evalStatus;
    DtActionStatus userStatus;
    int j;
    char *bufFilename = NULL;
    int ttmedia_test;


    /*
     * Find associated long term records for this request
     */
    actInvRecP   = _DtActFindInvRec( request->invocId );
    myassert(actInvRecP);
    actChildRecP = _DtActFindChildRec( request->invocId, request->childId );
    myassert(actChildRecP);

    actChildRecP->childState = _DtActCHILD_UNKNOWN;	/* really don't know */

    /*
     * Future:
     *
     * Consider doing a check of argClass -vs- TT_ARGn_REP_TYPE,
     * and issue errors where appropriate.
     *
     * argClass=DtACTION_FILE
     *    o REP_UNDEFINED         - o.k.  - will guess convert to REP_STRING
     *    o REP_BUFFER            - error - no sense, use DtACTION_BUFFER
     *    o REP_STRING            - o.k.  - normal case
     *    o REP_INTEGER           - o.k.  - normal case
     *
     * argClass=DtACTION_BUFFER
     *    o REP_UNDEFINED         - o.k.  - will guess convert to REP_BUFFER
     *    o REP_BUFFER            - o.k.  - normal case
     *    o REP_STRING            - ?     - will try to handle
     *    o REP_INTEGER           - error - use DtACTION_FILE
     */

    /*
     * Test request for ttmedia_load() suitability.
     *
     * To be ttmedia_load() suitable, action defs must look like:
     *
     * Action TtmediaBufferStyle
     * {
     *     ARG_CLASS         BUFFER
     *     ARG_COUNT         <3
     *
     *     TYPE              TT_MSG
     *     TT_OPERATION      <op>
     *     TT_CLASS          TT_REQUEST
     *     TT_SCOPE          TT_SESSION
     *     TT_FILE           --- must be null or unset ---
     *
     *     TT_ARG0_MODE      <mode>
     *     TT_ARG0_VTYPE     <vtype> | %Arg_n%
     *     TT_ARG0_REP_TYPE  TT_REP_BUFFER or TT_REP_UNDEFINED
     *     TT_ARG0_VALUE     %Arg_n%
     *
     *     optional TT_ARG1 for docname
     *
     *     TT_ARG1_MODE      TT_IN
     *     TT_ARG1_VTYPE     title
     *     TT_ARG1_REP_TYPE  TT_REP_STRING
     *     TT_ARG1_VALUE     <docname> | %Arg_n%
     * }
     * 
     * 
     * Action TtmediaFileStyle
     * {
     *     ARG_CLASS         FILE
     *     ARG_COUNT         <3
     *
     *     TYPE              TT_MSG
     *     TT_OPERATION      <op>
     *     TT_CLASS          TT_REQUEST
     *     TT_SCOPE          TT_SESSION
     *     TT_FILE           <filename> | %(File)Arg_n%
     *
     *     TT_ARG0_MODE      <mode>
     *     TT_ARG0_VTYPE     <vtype> | %Arg_n%
     *     TT_ARG0_REP_TYPE  TT_REP_STRING
     *     TT_ARG0_VALUE     --- must be null or unset ---
     *
     *     optional TT_ARG1 for docname
     *
     *     TT_ARG1_MODE      TT_IN
     *     TT_ARG1_VTYPE     title
     *     TT_ARG1_REP_TYPE  TT_REP_STRING
     *     TT_ARG1_VALUE     <docname> | %Arg_n%
     * }
     */

    ttmedia_test = 0;	/* default to free-form */

    if ( (tt->tt_class == TT_REQUEST) &&
	 (tt->tt_scope == TT_SESSION) &&
	 ((tt->mode_count == 1) || (tt->mode_count == 2)) &&
	 (tt->tt_argn_rep_type[0] != DtACT_TT_REP_INT) ) {

	/*
	 * 1- and 2-arg session-scoped requests not using ivals
	 * can be sent via ttmedia_load()...
	 */
	    ttmedia_test = 1;

	/*
	 * ...as long as the second argument is the title.  So:
	 * Allow for an optional "docname" in TT_ARG1.   If there is a
	 * second argument, it must follow some specific rules or we'll
	 * disqualify this request.
	 */
	if (tt->mode_count == 2) {
	    if ((tt->tt_argn_mode[1] != TT_IN) ||
		(tt->tt_argn_rep_type[1] != DtACT_TT_REP_STRING) ||
		(strcmp( tt->tt_argn_vtype[1].compiledMessage, "title" )) ) {
		ttmedia_test = 0;
	    }
	}
    }

    if (ttmedia_test)
    {
	/**********************************************************************
         *
         * TT_MEDIA_LOAD()
         *
	 * Special case media-load requests *and* any other requests that
	 * follow the "ttmedia style".
	 *
	 * If rep_type is INT, then ttmedia_load() cannot be used, and
	 * a rep_type of UNDEFINED is too chancy to guess on.
	 *
	 * Using ttmedia_load() sets up additional machinery.
	 *
	 */

	Tttk_op op = tttk_string_op(tt->tt_op.compiledMessage);

	/*
	 * Some media ops are overloaded, and the actual media op that
	 * should be used depends on the TT_ARGn_MODE of the argument.
	 */
	switch (tt->tt_argn_mode[0]) {
	    case TT_IN:
		op = TTME_DISPLAY;
		break;
	    case TT_OUT:
		op = TTME_COMPOSE;
		break;
	    case TT_INOUT:
		op = TTME_EDIT;
		break;
	}

	contents = (unsigned char *) NULL;
	length   = (int)             0;
	/*
	 * Determine what the data looks like: file or a buffer?
	 */
	if (   tt->tt_argn_rep_type[0] == DtACT_TT_REP_BUFFER
	    || tt->tt_argn_rep_type[0] == DtACT_TT_REP_UNDEFINED)
	{
	    /*
	     * Passing a buffer of data.
	     */

	    /*
	     * See if we should pluck the buffer out of a tmp file.
	     */
	    length = -1;	/* -1 means not a buffer */
	    if ( actChildRecP->argMap[0].argIdx != -1 ) {
		if ( IS_BUFFER_OBJ(actInvRecP->
				info[actChildRecP->argMap[0].argIdx].mask) ) {
		    length = actInvRecP->
				info[actChildRecP->argMap[0].argIdx].size;
		    bufFilename = actInvRecP->
				info[actChildRecP->argMap[0].argIdx].name;
		}
	    }
	}

	if (bufFilename) {
	    contents = (unsigned char *)
			    _DtActReadTmpFileToBuffer(bufFilename, &length);
	    /* length plucked from above */
	}
	else {
	    contents = (unsigned char *)
			    tt->tt_argn_value[0].compiledMessage;
	    length = tt->tt_argn_value[0].msgLen;
	}

	file     = (char *) tt->tt_file.compiledMessage;
	if (tt->mode_count == 2)
	    docname  = (char *) tt->tt_argn_value[1].compiledMessage;
	else
	    docname  = (char *) NULL;

	/*
	 * Fill out a callback structure.
	 */
	data = (CallbackData *)XtMalloc(sizeof(CallbackData));
	data->actionLabel = XtNewString(request->clonedAction->label);
	data->associatedWidget = w;
	data->actionPtr = action;
	data->requestPtr = _DtCloneRequest(request);
	data->actInvId = actInvRecP->id;
	data->childId = actChildRecP->childId;

	/*
	 * Let tttk build the message and setup additional machinery.
	 */
	message = ttmedia_load(	(Tt_message) 0,		/* Am top level req */
				Ttmedia_to_Dt_StatusUpdateCB,
				(void *) data,
				op,
				tt->tt_argn_vtype[0].compiledMessage,
				contents,
				length,
				file,
				docname,
				FALSE );
        status = tt_ptr_error(message);
        if (! tt_is_err(status))
        {
	    /*
	     * (Re)set the op.  This allows arbitrary Media-like
	     * TT_MSG actions to use the Media action machinery.
	     */
	    tt_message_op_set(message, tt->tt_op.compiledMessage);

	    /*
	     * Attach client data to message using slot 0.
	     */
	    tt_message_user_set(message, 0, (void *) data);

	    actChildRecP->childState = _DtActCHILD_PENDING_START; /* starting */

	    /*
	     * I'm assuming w is non-NULL, hence the lack of a callback
	     * and callback data.
	     */
	    subcon_patterns = ttdt_subcontract_manage(message,
			        (Ttdt_contract_cb) _DtTtContractIgnoreMsgCB,
			        w, (void *) NULL);

	    status = tt_message_send(message);
	    destroy_message = 1;
        }
   
	/*
	 * See if we failed
	 */
	if (status != TT_OK) {
	    /*
	     * The child error'ed out.
	     */
	    actChildRecP->childState = _DtActCHILD_FAILED;

	    /*
	     * Report the fact that the request failed
	     */
	    errorMsg = tt_status_message(status);
	    if ((tt_pointer_error(errorMsg) == TT_OK) && errorMsg)
	    {
		errorMsg = WrapMessageLines(errorMsg);
		ReportToolTalkError(w, request->clonedAction->label, errorMsg);
		tt_free(errorMsg);
	    }
	    else
		ReportToolTalkError(w, request->clonedAction->label, NULL);
   
	    /*
	     * Free information since no callbacks will be triggered:
	     *    - Callback Client Data
	     *    - The message
	     */
	    XtFree(data->actionLabel);
	    _DtFreeRequest(data->requestPtr);
	    XtFree((char *)data);

	    if (destroy_message)
	      tttk_message_destroy(message);
	}
	else {
	    /*
	     * The child is alive and well (so far as we can tell).
	     */
	    actChildRecP->childState = _DtActCHILD_ALIVE;

	    /*
	     * Stash data away for long term usage.
	     */
	    actChildRecP->u.tt.TTProcId   = tt_message_sender( message );
	    actChildRecP->u.tt.reqMessage = message;
	    actChildRecP->u.tt.isTtMedia  = 1;
	    actChildRecP->u.tt.TtMediaOp  = op;
	    actChildRecP->u.tt.subConPats = subcon_patterns;
	}

	/*
	 * Throw away transition buffer if there was one.
	 */
	if (bufFilename)
	    XtFree((char *) contents);
   }
   else
   {
	/**********************************************************************
         *
         * FREE FORM TT_MSG
         *
	 * All other Requests and Notices go out "free form".  Do best case.
	 */
	message = tttk_message_create( 0, tt->tt_class, tt->tt_scope, 0,
					tt->tt_op.compiledMessage,
					TtRequestCallbackHandler );

	tt_message_file_set(message, tt->tt_file.compiledMessage);

	/*
	 * Process each of the message arguments
	 *
	 * note: As of CDE 1.0, all the tt->xxx_count variables are
	 * set the same.   I doubt they would ever diverge, and
	 * much of this code would break if they did.
	 */
	for (i = 0; i < tt->mode_count; i++)
	{
	    /*
	     * Pluck some data out of the argMap to help us make
	     * some decisions here.
	     */
	    length = -1;			/* -1 means not a buffer */
	    bufFilename = (char *) NULL;	/* possible buffer tmp file */

	    if ( actChildRecP->argMap[i].argIdx != -1 ) {
		if ( IS_BUFFER_OBJ(actInvRecP->
				info[actChildRecP->argMap[i].argIdx].mask) ) {
		    length = actInvRecP->
				info[actChildRecP->argMap[i].argIdx].size;
		    bufFilename = actInvRecP->
				info[actChildRecP->argMap[i].argIdx].name;
		}
	    }

            /*
	     * Determine MODE
	     */
	    mode = tt->tt_argn_mode[i];
   
            /*
	     * Determine REP TYPE
	     */
	    if ((i < tt->rep_type_count) && 
		(tt->tt_argn_rep_type[i] != DtACT_TT_REP_UNDEFINED))
	    {    
		/* The representation type was supplied; use it */
		repType = tt->tt_argn_rep_type[i];
	    }
	    else
	    {
		/*
		 * No representation type given. If the argument was provided
		 * and is a DtACTION_BUFFER, then use DtACT_TT_REP_BUFFER.
		 * In all other cases, use DtACT_TT_REP_STRING.
		 */
		if ( length != -1 )
		    repType = DtACT_TT_REP_BUFFER;
		else
		    repType = DtACT_TT_REP_STRING;
	    }

	    /*
	     * Determine VALUE and VTYPE
	     */
	    if (i < tt->value_count)
	    {
		/*
		 * Use the specified vtype
		 */
		vtype = tt->tt_argn_vtype[i].compiledMessage;

		/*
		 * Determine where to get the value from
		 */
		if ( length != -1 ) {
		    /*
		     * Have a buffer - see if it's value can be fetched
		     * from a tmp file.   If so, will need to free later!
		     */
		    if (bufFilename) {
			value = _DtActReadTmpFileToBuffer(bufFilename, &length);
		    }
		    else {
			value = tt->tt_argn_value[i].compiledMessage;
		    }
		}
		else {
		    value = tt->tt_argn_value[i].compiledMessage;
		}
	    }
	    else
	    {
		/* No value specified; use 'NULL' */
		value = NULL;
   
		/*
		 * Use the specified vtype setting, if supplied; else
		 * default it
		 */
		if (i < tt->vtype_count)
		    vtype = tt->tt_argn_vtype[i].compiledMessage;
		else
		{
		    /*
		     * Time to guess what the vtype should be.  This
		     * is very funky.
		     */
		    if (repType == DtACT_TT_REP_INT)
			vtype = "integer";
		    else if (repType == DtACT_TT_REP_STRING)
			vtype = "string";
		    else if (repType == DtACT_TT_REP_BUFFER)
			vtype = "buffer";
		}
	    }

	    /*
	     * Based on REP_TYPE, add the argument to the message. In
	     * effect, rep_type is an instruction on how to package
	     * the argument in the message.
	     */
	    if (repType == DtACT_TT_REP_INT)
	    {
		if (value)
		    intValue = strtol(value, &p, 0);
		else
		    intValue = 0;

		tt_message_iarg_add(message, mode, vtype, intValue);
	    }
	    else if (repType == DtACT_TT_REP_STRING)
	    {
		tt_message_arg_add(message, mode, vtype, value);
	    }
	    else if (repType == DtACT_TT_REP_BUFFER)
	    {
		tt_message_barg_add(message, mode, vtype,
					(unsigned char *) value, length);
	    }

	    if ( (length != -1) && (bufFilename) ) {
		/*
		 * The value was from a buffer that we pulled from a tmp
		 * file. Free the transition buffer since Tooltalk will have
		 * made its own copy by now.
		 */
		XtFree(value);
	    }
	}

	if (tt->tt_class == TT_REQUEST)
	{
	    /*
	     * For a request, we need to attach a callback, which will be
	     * invoked when the reply is received.  When the reply is
	     * received, then we can free up the original message, and
	     * determine whether we succeeded or failed.
	     */
   
	    /* Fill out the callback structure */
	    data = (CallbackData *)XtMalloc(sizeof(CallbackData));
	    data->actionLabel = XtNewString(request->clonedAction->label);
	    data->associatedWidget = w;
	    data->actionPtr = action;
	    data->requestPtr = _DtCloneRequest(request);
	    data->actInvId = actInvRecP->id;
	    data->childId = actChildRecP->childId;
   
	    /*
	     * Attach client data to message using slot 0.
	     */
	    tt_message_user_set(message, 0, (void *) data);
   
	    tt_message_callback_add(message, TtRequestCallbackHandler);
	}
   
	actChildRecP->childState = _DtActCHILD_PENDING_START;	/* starting */

	/*
	 * I'm assuming w is non-NULL, hence the lack of a callback
	 * and callback data.
	 */
	subcon_patterns = ttdt_subcontract_manage(message,
			(Ttdt_contract_cb) _DtTtContractIgnoreMsgCB,
			w, (void *) NULL);

	status = tt_message_send(message);
   
	/* See if we failed right off the bat */
	if (status != TT_OK)
	{
	    /*
	     * The child error'ed out.
	     *
	     * By marking the child as _DtActCHILD_DONE*, see the comment
	     * at the end of this routine that describes how DtActionInvoke()
	     * will do a child evaluation and cleanup.
	     */
	    actChildRecP->childState = _DtActCHILD_FAILED;

	    /*
	     * Report the fact that the request failed
	     */
	    errorMsg = tt_status_message(status);
	    if ((tt_pointer_error(errorMsg) == TT_OK) && errorMsg)
	    {
		errorMsg = WrapMessageLines(errorMsg);
		ReportToolTalkError(w, request->clonedAction->label, errorMsg);
		tt_free(errorMsg);
	    }
	    else
		ReportToolTalkError(w, request->clonedAction->label, NULL);
   
	    /*
	     * Clean up now, cause there will not be a reply coming back
	     */
	    if (tt->tt_class == TT_REQUEST)
	    {
		_DtFreeRequest(data->requestPtr);
		XtFree(data->actionLabel);
		XtFree((char *)data);
	    }
	    tttk_message_destroy(message);
	}
	else {
	    /*
	     * The child is alive and well (so far as we can tell).
	     */
	    actChildRecP->childState = _DtActCHILD_ALIVE;

            if (tt->tt_class == TT_NOTICE) {
		/*
		 * The child is done.
		 *
		 * By marking the child as _DtActCHILD_DONE*, see the
		 * comment at the end of this routine that describes how
		 * DtActionInvoke() will do a child evaluation and cleanup.
		 */
		actChildRecP->childState = _DtActCHILD_DONE;

		/*
		 * Need to clean up after a notify, since there is no
		 * response.
		 */
		tttk_message_destroy(message);
	    }
	    else {
		/*
		 * Stash data away for long term usage.
		 */
		actChildRecP->u.tt.TTProcId   = tt_message_sender( message );
		actChildRecP->u.tt.reqMessage = message;
		actChildRecP->u.tt.isTtMedia  = 0;
		actChildRecP->u.tt.TtMediaOp  = (Tttk_op) NULL;
		actChildRecP->u.tt.subConPats = subcon_patterns;
	    }
	}
    }

    /*
     * _DtActExecutionLeafNodeCleanup( id, (DtActionArg *) NULL, 0, 1 );
     *
     * This is an execution leaf node, but because SET_INV_COMPLETE()
     * has NOT been called yet, doing a _DtActExecutionLeafNodeCleanup()
     * is useless.  As we bubble back up, let DtActionInvoke() flip
     * SET_INV_COMPLETE and then do a _DtActExecutionLeafNodeCleanup()
     * via a timer.
     */
}

/******************************************************************************
 *
 * This function checks to see if there is already a default ToolTalk
 * session active for this client.  If there is one, then we will use
 * it for the ToolTalk message we are about to send.  If there is not
 * a default ToolTalk session, then we will try to establish a connection
 * to a new session.  If we succeed in connecting to ToolTalk, then we
 * will return 'True'; 'False' is returned if we fail.
 */
Tt_status
_DtInitializeToolTalk(Widget w)
{
   static int RegisteredPatterns = 0;
   char * procid;
   Tt_status status;
   int fd;
   Tt_pattern pat;	/* pattern to register for exec*  messages */
   char *session_id;
   extern Widget _DtInitTtContextWidget;

   /*
    * See if the context widget is set.
    */
   if ( w && !_DtInitTtContextWidget )
   {
	_DtInitTtContextWidget = w;
   }


   /* The _Dt_context_widget should have been set in DtBigInitialize() */
   myassert (_DtInitTtContextWidget != NULL);

   if ( (status = _DtSvcInitToolTalk(_DtInitTtContextWidget)) != TT_OK )
	return status;

   _DtSvcProcessLock();
   if ( !RegisteredPatterns )
   {
      if ( !(session_id = tt_default_session()))
      {
          /*
           * Unable to get session id -- what now?
           */
          myassert(0);
	  _DtSvcProcessUnlock();
          return(TT_ERR_NOMP);
      }

      /*
       * Register to recieve interesting messages from dtexec
       */
      pat = tt_pattern_create();

      tt_pattern_category_set(pat,TT_HANDLE);
      tt_pattern_scope_add(pat, TT_SESSION);
      tt_pattern_op_add(pat,"_DtActDtexecID");
      tt_pattern_callback_add( pat, (Tt_message_callback) _DtActReceiveExecId);
      tt_pattern_session_add(pat,session_id);

      tt_pattern_register(pat);

      /*
       * Create/register a new pattern for the done callback
       */
      pat = tt_pattern_create();
    
      tt_pattern_category_set(pat,TT_HANDLE);
      tt_pattern_scope_add(pat, TT_SESSION);
      tt_pattern_op_add(pat,"_DtActDtexecDone");
      tt_pattern_callback_add(pat, (Tt_message_callback) _DtActReceiveDoneMsg);
      tt_pattern_session_add(pat,session_id);

      tt_pattern_register(pat);

      tt_free(session_id);
      RegisteredPatterns = 1;
   }

   _DtSvcProcessUnlock();      
   return (TT_OK);
}


/******************************************************************************
 *
 * fdt: This function needs to be written!
 *
 * All of the error messages generated by the tooltalk layer itself (as
 * opposed to those messages generated by the handler of a message) are
 * returned as one long message string, with no line breaks.  When one of
 * these messages is displayed in an error dialog, the dialog stretches
 * across the full width of the display, and even then, some of the message
 * string is not visible.  To remedy this, we need to 'wrap' these lines,
 * so that they are of a reasonable length, and we also need to expand
 * 'tab' characters, since these messages also use them.
 *
 * tt_free()s the string passed in.
 * The returned character string must be tt_free()d by the caller.
 */

static char *
WrapMessageLines( 
   char * errorMsg )
{
   return errorMsg;
}

