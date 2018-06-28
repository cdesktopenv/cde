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

/*
 *	$XConsortium: obj_notify.c /main/4 1996/10/02 16:03:34 drk $
 *
 *	@(#)obj_notify.c	1.41 02 Feb 1995	cde_app_builder/src/libABobj
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */


/*
 *  obj_notify.c - object change notification
 *
 *  ALL FUNCTIONS THAT ACTUALLY MODIFY, UPDATE, SEND OR QUEUE NOTIFICATIONS
 *  ARE IN THIS FILE.  NO NOTIFICATION CODE SHOULD EXIST OUTSIDE OF THIS
 *  FILE!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "objP.h"			/* include this first! */
#include <ab_private/obj_notify.h>
#include "obj_notifyP.h"

/*************************************************************************
**                                                                      **
**       Constants (#define and const)					**
**                                                                      **
**************************************************************************/
#define MAX_MODE_DEPTH		100
#define MAX_EVENT_QUEUE_SIZE	5000

/*************************************************************************
**                                                                      **
**       Types								**
**                                                                      **
**************************************************************************/


typedef int (*GenericCallback)(void *);

typedef struct
{
    GenericCallback	callback;
    OBJEV_ATT_FLAGS	atts;
    ISTRING		name;
} NAMED_CALLBACK, *NamedCallback;

typedef NamedCallback NamedCallbackList;

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
static int call_callbacks_in_list(NamedCallbackList list, 
		void *data, ObjEvent event);
static int add_to_callback_list(NamedCallbackList *list, 
		GenericCallback callback, OBJEV_ATT_FLAGS atts,
		ISTRING name);
static int objP_dispatch_event(ObjEvent event);
static int objP_dispatch_event_batched(ObjEvent event);
static int objP_dispatch_event_now(ObjEvent event);
static int objP_send_event(ObjEvent event);
static int flush_queue(void);
static int force_flush_queue(void);
static int event_destruct(ObjEvent, BOOL wasBatched);
static int cleanup_ev_att_change(ObjEvent event);
static int cleanup_ev_destroy(ObjEvent event, BOOL wasBatched);
static int cleanup_ev_update_with_data(ObjEvent event);
static BOOL event_is_in_queue(ObjEvent event);
static BOOL events_equal(ObjEvent event1, ObjEvent event2);

#define should_print_callbacks() \
		((forcePrintCallbacks) || (debug_level() >= 3))
#define current_notify_mode() (modeStack[modeStackIndex])

#define queue_next_index(i)  (((i) >= (MAX_EVENT_QUEUE_SIZE-1))? 0:(i)+1)
#define queue_is_empty() (firstEventIndex == lastEventIndex)
#define queue_is_full()	 (queue_next_index(lastEventIndex) == firstEventIndex)

/*
 * Pops a mode from the mode stack, but won't flush the queue
 */
#define objP_notify_pop_mode_dont_flush() \
	((modeStackIndex <= 0)? ERR_CLIENT : --modeStackIndex)


/*************************************************************************
**                                                                      **
**       Data     	                                                **
**                                                                      **
**************************************************************************/
static BOOL		ignoreEvents= FALSE;	/* critical error conditions*/
static BOOL		forcePrintCallbacks= FALSE;
static BYTE		modeStack[MAX_MODE_DEPTH]= {OBJEV_MODE_NORMAL };
static int		modeStackIndex= 0;
static int		recurseLevel= 0;
OBJ_EVENT		eventQueue[MAX_EVENT_QUEUE_SIZE];
int			firstEventIndex= 0;
int			lastEventIndex= 0;
NamedCallbackList	callbacksForAllowGeometryChange= NULL;
NamedCallbackList	callbacksForAllowReparent= NULL;
NamedCallbackList	callbacksForAttChange= NULL;
NamedCallbackList	callbacksForCreate= NULL;
NamedCallbackList	callbacksForDestroy= NULL;
NamedCallbackList	callbacksForReparent= NULL;
NamedCallbackList	callbacksForUpdate= NULL;
NamedCallbackList	callbacksForUpdateWithData= NULL;


/*************************************************************************
**                                                                      **
**       PUBLIC FUNCTIONS for callback lists				**
**                                                                      **
**************************************************************************/

int
obj_add_allow_geometry_change_callback(ObjAllowGeometryChangeCallback cb,
		STRING debug_name)
{
    return add_to_callback_list(&callbacksForAllowGeometryChange, 
		(GenericCallback)cb, OBJEV_ATT_NONE,
		istr_create(debug_name));
}

int
obj_add_allow_reparent_callback(ObjAllowReparentCallback cb,
		STRING debug_name)
{
    return add_to_callback_list(&callbacksForAllowReparent, 
		(GenericCallback)cb, OBJEV_ATT_NONE,
		istr_create(debug_name));
}

int
obj_add_create_callback(ObjCreateCallback cb,
		STRING debug_name)
{
    return add_to_callback_list(&callbacksForCreate, 
		(GenericCallback)cb, OBJEV_ATT_NONE,
		istr_create(debug_name));
}

int
obj_add_destroy_callback(ObjDestroyCallback cb,
		STRING debug_name)
{
    return add_to_callback_list(&callbacksForDestroy, 
		(GenericCallback)cb, OBJEV_ATT_NONE,
		istr_create(debug_name));
}

int
obj_add_geometry_change_callback(ObjAttChangeCallback cb,
		STRING debug_name)
{
    return add_to_callback_list(&callbacksForAttChange,
		(GenericCallback)cb, 
		OBJEV_ATT_GEOMETRY | OBJEV_ATT_POSITION | OBJEV_ATT_SIZE,
		istr_create(debug_name));
}

int
obj_add_rename_callback(ObjAttChangeCallback cb,
		STRING debug_name)
{
    return add_to_callback_list(&callbacksForAttChange,
		(GenericCallback)cb, OBJEV_ATT_NAME,
		istr_create(debug_name));
}

int
obj_add_reparent_callback(ObjReparentCallback cb,
		STRING debug_name)
{
    return add_to_callback_list(&callbacksForReparent, 
		(GenericCallback)cb, OBJEV_ATT_NONE,
		istr_create(debug_name));
}

int
obj_add_selected_change_callback(ObjAttChangeCallback cb,
                STRING debug_name)
{
    return add_to_callback_list(&callbacksForAttChange,
                (GenericCallback)cb, OBJEV_ATT_SELECTED,
                istr_create(debug_name));
}

int 
obj_add_update_callback(ObjUpdateCallback cb,
		STRING debug_name)
{
    return add_to_callback_list(&callbacksForUpdate,
		(GenericCallback)cb, OBJEV_ATT_NONE,
		istr_create(debug_name));
}

int
obj_add_update_with_data_callback(ObjUpdateWithDataCallback cb,
		STRING debug_name)
{
    return add_to_callback_list(&callbacksForUpdateWithData,
		(GenericCallback)cb, OBJEV_ATT_NONE,
		istr_create(debug_name));
}


/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/
#define event_is_allow(ev) \
	(   ((ev)->type == OBJEV_ALLOW_GEOMETRY_CHANGE) \
	 || ((ev)->type == OBJEV_ALLOW_REPARENT) )

#define event_is_abortable(ev) (event_is_allow(ev))

/*************************************************************************
**                                                                      **
**       Mode stack							**
**									**
**  The bottom stack entry is MODE_NORMAL, and is always there.		**
**                                                                      **
**************************************************************************/

int
objP_notify_push_mode(void)
{
    if (modeStackIndex >= (MAX_MODE_DEPTH-1))
    {
	return ERR_NO_MEMORY;
    }
    ++modeStackIndex;
    modeStack[modeStackIndex]= modeStack[modeStackIndex-1];
    return 0;
}

int
objP_notify_set_mode(OBJ_EVENT_MODE newMode)
{
    modeStack[modeStackIndex] |= newMode;
    return 0;
}

int
objP_notify_clear_mode(OBJ_EVENT_MODE deadMode)
{
    modeStack[modeStackIndex] &= ~deadMode;
    return 0;
}


/*
 * Pops a mode from the mode stack.  If a non-batching mode is entered,
 * the notify queue is flushed.
 */
int
objP_notify_pop_mode(void)
{
    objP_notify_pop_mode_dont_flush();

    /*
     * If we've popped into a non-batching context, send the events!
     */
    if (!objP_notify_mode_is_set(OBJEV_MODE_BATCH_NOTIFY_EVS))
    {
	flush_queue();
    }

    return 0;
}


OBJ_EVENT_MODE
objP_notify_get_mode(void)
{
    return current_notify_mode();
}


/*************************************************************************
**                                                                      **
**       SHARED send_event functions					**
**                                                                      **
**************************************************************************/

int
objP_notify_send_allow_geometry_change(ABObj obj, int x, int y, int w, int h)
{
    OBJ_EVENT	event;

    event.type= OBJEV_ALLOW_GEOMETRY_CHANGE;
    /* event.info.allow_geometry_change.abortable= TRUE; */
    event.info.allow_geometry_change.obj= obj;
    event.info.allow_geometry_change.new_x= x;
    event.info.allow_geometry_change.new_y= y;
    event.info.allow_geometry_change.new_width= w;
    event.info.allow_geometry_change.new_height= h;
    return objP_dispatch_event(&event);
}


int
objP_notify_send_allow_reparent(ABObj obj, ABObj newParent)
{
    OBJ_EVENT	event;

    event.type= OBJEV_ALLOW_REPARENT;
    /* event.info.allow_reparent.abortable= TRUE; */
    event.info.allow_reparent.obj= obj;
    event.info.allow_reparent.new_parent= newParent;
    return objP_dispatch_event(&event);
}

int
objP_notify_send_att_change(ABObj obj, OBJEV_ATT_FLAGS atts)
{
    OBJ_EVENT	event;

    event.type= OBJEV_ATT_CHANGE;
    event.info.att_change.obj= obj;
    event.info.att_change.atts= atts;
    return objP_dispatch_event(&event);
}

int
objP_notify_send_create(ABObj obj)
{
    OBJ_EVENT	event;

    event.type= OBJEV_CREATE;
    /* event.info.create.abortable= FALSE; */
    event.info.create.obj= obj;
    return objP_dispatch_event(&event);
}

int
objP_notify_send_destroy(ABObj obj)
{
    OBJ_EVENT	event;

    event.type= OBJEV_DESTROY;
    /* event.info.destroy_info.abortable= FALSE; */
    event.info.destroy_info.obj= obj;
    return objP_dispatch_event(&event);
}

int
objP_notify_send_geometry_change(ABObj obj, 
		int oldX, int oldY, int oldWidth, int oldHeight)
{
    OBJ_EVENT		event;
    OBJEV_ATT_FLAGS 	geomflag = (OBJEV_ATT_FLAGS)0;

    if (oldX != obj->x || oldY != obj->y)
	geomflag |= OBJEV_ATT_POSITION;
    if (oldWidth != obj->width || oldHeight != obj->height)
	geomflag |= OBJEV_ATT_SIZE;

    event.type= OBJEV_ATT_CHANGE;
    event.info.att_change.atts = geomflag;
    event.info.att_change.obj = obj;
    event.info.att_change.old_name = NULL;
    return objP_dispatch_event(&event);
}

int
objP_notify_send_rc_geometry_change(ABObj obj)
{
    OBJ_EVENT           event;

    event.type= OBJEV_ATT_CHANGE;
    event.info.att_change.atts = OBJEV_ATT_SIZE;
    event.info.att_change.obj = obj;
    event.info.att_change.old_name = NULL;
    return objP_dispatch_event(&event);
}

int
objP_notify_send_name_change(ABObj obj, ISTRING old_name)
{
    OBJ_EVENT	event;
    event.type= OBJEV_ATT_CHANGE;
    event.info.att_change.atts = OBJEV_ATT_NAME;
    event.info.att_change.obj = obj;
    event.info.att_change.old_name = istr_dup(old_name);
    return objP_dispatch_event(&event);
}

int
objP_notify_send_reparent(ABObj obj, ABObj oldParent)
{
    OBJ_EVENT	event;

    event.type= OBJEV_REPARENT;
    event.info.reparent.obj= obj;
    /* event.info.reparent.abortable= FALSE; */
    event.info.reparent.old_parent= oldParent;
    return objP_dispatch_event(&event);
}

int
objP_notify_send_selected_change(ABObj obj)
{
    OBJ_EVENT   event;

    event.type = OBJEV_ATT_CHANGE;
    event.info.att_change.atts = OBJEV_ATT_SELECTED;
    event.info.att_change.obj= obj;
    event.info.att_change.old_name = NULL;
    return objP_dispatch_event(&event);
}

int
objP_notify_send_update(ABObj obj, BOOL update_subtree)
{
    OBJ_EVENT	event;

    event.type= OBJEV_UPDATE;
    event.info.update.obj= obj;
    event.info.update.update_subtree= update_subtree;
    return objP_dispatch_event(&event);
}

int
objP_notify_send_update_with_data(
		ABObj			obj, 
		BOOL			update_subtree,
		int 			update_code, 
		void			*update_data, 
		UpdateDataFreeFunc 	update_data_free_func
		)
{
    OBJ_EVENT	event;

    event.type= OBJEV_UPDATE_WITH_DATA;
    event.info.update_with_data.obj= obj;
    event.info.update_with_data.update_subtree= update_subtree;
    event.info.update_with_data.update_code= update_code;
    event.info.update_with_data.update_data= update_data;
    event.info.update_with_data.update_data_free_func= update_data_free_func;
    return objP_dispatch_event(&event);
}


/*************************************************************************
**                                                                      **
**       PRIVATE functions to deal with dispatching, queueing events	**
**                                                                      **
**************************************************************************/

static int
objP_dispatch_event(OBJ_EVENT *event)
{
    int		iReturn= 0;
    BOOL	eventIsNotify= !event_is_allow(event);

    if (ignoreEvents)
    {
	/*
	 * A critical error has occurred (e.g., an event loop).  Just
	 * pretend everything is hunky-dory and ignore dispatch requests.
	 */
	iReturn= OK;
	goto epilogue;
    }

    if (objP_notify_mode_is_set(OBJEV_MODE_DISALLOW_ALL_EVS))
    {
	iReturn= ERR_NOT_ALLOWED;
	goto epilogue;
    }

    if (   eventIsNotify
	&& (!(objP_notify_mode_is_set(OBJEV_MODE_SEND_NOTIFY_EVS))) )
    {
	/* we're not sending notify events - assume OK */
	iReturn= OK;
	goto epilogue;
    }

    if (   (!eventIsNotify)
	&& (!(objP_notify_mode_is_set(OBJEV_MODE_SEND_ALLOW_EVS))) )
    {
	/* we're not sending allow events - assume OK */
	iReturn= OK;
	goto epilogue;
    }

    /*
     * Actually send an event! (or batch it...)
     */
    if (   eventIsNotify 
	&& objP_notify_mode_is_set(OBJEV_MODE_BATCH_NOTIFY_EVS) )
    {
	iReturn= objP_dispatch_event_batched(event);
    }
    else
    {
	iReturn= objP_dispatch_event_now(event);
    }

epilogue:
   return iReturn;
}


/* Effects: queues an event for later delivery
 * Assumes: mode NO_NOTIFY is not in effect
 * Modifies: adds event to global queue
 */ 
static int
objP_dispatch_event_batched(ObjEvent event)
{
    int	iReturn= OBJ_NOTIFY_BATCHED;	/* must return this! */

    if (event_is_in_queue(event))
    {
	goto epilogue;		/* it's already there! */
    }

    if (queue_is_full())
    {
	iReturn= ERR_DATA_SPACE_FULL;
	goto epilogue;
    }

    eventQueue[lastEventIndex]= *event;
    lastEventIndex= queue_next_index(lastEventIndex);

epilogue:
    return iReturn;
}


/* Effects: sends an event immediately.  Flushes all events resulting from
 *          callbacks
 * Assumes: NO_NOTIFY is not in effect
 */
static int
objP_dispatch_event_now(ObjEvent event)
{
    int		iReturn= 0;
    int		iRC= 0;

    iRC= objP_send_event(event);
    event_destruct(event, FALSE);
    if (iRC < 0)
    {
	iReturn= iRC;
    }

    /*
     * If we've popped into a non-batching context, send the events!
     */  
    if (!objP_notify_mode_is_set(OBJEV_MODE_BATCH_NOTIFY_EVS))
    {
        iRC = flush_queue();
    }
    if (iRC < 0)
    {
	iReturn= iRC;
    }

/* epilogue: */
    return iReturn;
}


/* Effects: sends an event immediately to all registered clients
 * Assumes: mode NO_NOTIFY is not in effect
 * Modifies:
 *
 * Will not flush and messages resulting from the callbacks
 */
static int
objP_send_event(ObjEvent event)
{
    int			iReturn= 0;
    int			iRC= 0;		/* return code */
    NamedCallbackList	callbackList= NULL;
    void		*data= NULL;

    switch (event->type)
    {
	case OBJEV_ALLOW_GEOMETRY_CHANGE:
	    callbackList= callbacksForAllowGeometryChange;
	    data= (void *)&(event->info.allow_geometry_change);
	break;

	case OBJEV_ALLOW_REPARENT:
	    callbackList= callbacksForAllowReparent;
	    data= (void *)&(event->info.allow_reparent);
	break;

	case OBJEV_ATT_CHANGE:
	    callbackList= callbacksForAttChange;
	    data= (void *)&(event->info.att_change);
	break;

	case OBJEV_CREATE:
	    callbackList= callbacksForCreate;
	    data= (void *)&(event->info.create);
	break;

	case OBJEV_DESTROY:
	    callbackList= callbacksForDestroy;
	    data= (void *)&(event->info.destroy_info);
	break;

	case OBJEV_REPARENT:
	    callbackList= callbacksForReparent;
	    data= (void *)&(event->info.reparent);
	break;

	case OBJEV_UPDATE:
	    callbackList= callbacksForUpdate;
	    data= (void *)&(event->info.update);
 	break;

	case OBJEV_UPDATE_WITH_DATA:
	    callbackList= callbacksForUpdateWithData;
	    data= (void *)&(event->info.update_with_data);
	break;

	default:
	    iReturn= ERR_INTERNAL;
	    goto epilogue;
	/* break; */
    }

    if (recurseLevel > 0)
    {
	iReturn= ERR_RECURSION;
	goto epilogue;
    }

    /*
     * Batch all events that are generated by callback
     */
    objP_notify_push_mode();
	objP_notify_set_mode(  OBJEV_MODE_SEND_ALLOW_EVS
			     | OBJEV_MODE_SEND_NOTIFY_EVS
			     | OBJEV_MODE_BATCH_NOTIFY_EVS);
	if (event_is_allow(event))
	{
	    objP_notify_set_mode(OBJEV_MODE_DISALLOW_ALL_EVS);
	}
        iReturn= iRC= call_callbacks_in_list(callbackList, data, event);
    objP_notify_pop_mode_dont_flush();

epilogue:
    return iReturn;
}


/*
 * Effects: flushes all events in the queue, including any new events
 *    		generated while sending.  Guarantees empty queue on return.
 */
static int
flush_queue(void)
{
    int		iReturn= 0;
    int		numEventsProcessed= 0;
    ObjEvent	event= NULL;

    while (!queue_is_empty())
    {
	event= &(eventQueue[firstEventIndex]);
	objP_send_event(event);
	event_destruct(event, TRUE);
	firstEventIndex= queue_next_index(firstEventIndex);

	++numEventsProcessed;
	if (numEventsProcessed >= MAX_EVENT_QUEUE_SIZE)
	{
	    /* 
	     * an event cycle has been detected.
	     */
	    iReturn= force_flush_queue();
	}
    }

    return iReturn;
}


/*
 * Guarantees that the event queue will be empty upon return.  Turns
 * off queuing of events from the callbacks
 *
 * Allows 20 or so more events to be queued, while printing them out
 */
static int
force_flush_queue(void)
{
    int		iReturn= ERR_RECURSION;
    int		numEventsProcessed= 0;
    ObjEvent	event= NULL;

    /*
     * We have probably run into an endless event cycle.  To fix this,
     * we're going ignore any more events generated while flushing the
     * queue.  
     *
     * If we're verbose, we're going to print out the remainder of the
     * events in the queue as they are dispatched.
     */

    ignoreEvents= TRUE;

#ifdef DEBUG
    if (debugging())
    {
	ignoreEvents= FALSE;		/* for debugging, allow some events */
        forcePrintCallbacks= TRUE;
        fprintf(stderr, 
            "\n\nAPPARENT EVENT CYCLE DETECTED.  STARTING EVENT TRACING.\n\n");
	fflush(stderr);
        sleep(3);
    }
#endif /* DEBUG */

    while (!queue_is_empty())
    {
	event= &(eventQueue[firstEventIndex]);
        objP_send_event(event);
	event_destruct(event, TRUE);
        firstEventIndex= queue_next_index(firstEventIndex);

	++numEventsProcessed;
	if (numEventsProcessed > 20)
	{
	    ignoreEvents= TRUE;
	}
    }

    ignoreEvents= FALSE;
    forcePrintCallbacks= FALSE;
    return iReturn;
}


/*
 * Effects: invalidates the event, freeing any resources it is holding
 */
static int
event_destruct(ObjEvent event, BOOL wasBatched)
{
    int		return_value = 0;

    switch (event->type)
    {
	case OBJEV_ATT_CHANGE:
	    return_value = cleanup_ev_att_change(event);
	    break;

	case OBJEV_DESTROY:
	    return_value = cleanup_ev_destroy(event, wasBatched);
	    break;

	case OBJEV_UPDATE_WITH_DATA:
	    return_value= cleanup_ev_update_with_data(event);
	    break;
    }

    return return_value;
}


/*
 * cleanup_att_change
 */
static int
cleanup_ev_att_change(ObjEvent event)
{
    if ((event->info.att_change.atts) & OBJEV_ATT_NAME)
    {
	istr_destroy(event->info.att_change.old_name);
    }

    return 0;
}


/*
 * Effects: Completes a destroy operation that was deferred, waiting for
 *    		notification.
 *
 * Assumes: event is of type destroy.
 */
static int
cleanup_ev_destroy(ObjEvent event, BOOL wasBatched)
{
    int		iReturn= 0;

    if (!wasBatched)
    {
	/* if we weren't batched, then the caller will clean up the */
	/* object */
	return 0;
    }

    /*
     * We must push a batching context.  Otherwise, when the called routines
     * push and then pop a mode the new mode (the one we're in) is non-
     * batching, and the queue will be flushed.  Since we are currently
     * flushing the queue, that's us! (it will recurse back here).
     */
    objP_notify_push_mode();
	objP_notify_set_mode(OBJEV_MODE_BATCH_NOTIFY_EVS);
        iReturn= objP_actually_destroy_one(event->info.destroy_info.obj);
    objP_notify_pop_mode_dont_flush();

    return iReturn;
}


/*
 *  Effects: - frees the data for an update_with_data, if it is no longer
 *   		needed
 *  Assumes: - queue is not empty
 *		- event is of type UPDATE_WITH_DATA
 *
 *  If any other events in the queue are using this data, it is not freed.
 */
static int
cleanup_ev_update_with_data(ObjEvent event)
{
    int			iReturn= 0;
    int			eventIndex= firstEventIndex;
    void		*update_data= event->info.update_with_data.update_data;
    UpdateDataFreeFunc	free_func=
			    event->info.update_with_data.update_data_free_func;
    BOOL		dataStillInUse= FALSE;

    if (free_func == NULL)
    {
	goto epilogue;
    }

    eventIndex= queue_next_index(eventIndex);
    while (eventIndex != lastEventIndex)
    {
	if (   (eventQueue[eventIndex].type == OBJEV_UPDATE_WITH_DATA)
	    && (eventQueue[eventIndex].info.update_with_data.update_data
			== update_data) )
	{
	    dataStillInUse= TRUE;
	    break;
	}
    }
    if (!dataStillInUse)
    {
	free_func(event->info.update_with_data.update_code,
			update_data);  
	update_data= NULL;
	event->info.update_with_data.update_data= NULL;
    }

epilogue:
    return iReturn;
}


static int
event_print(OBJ_EVENT *event, FILE *outFile, BOOL addNewline, STRING name)
{
    ABObj	obj= NULL;
    char	eventName[1024];
    char	eventParams[1024];
    char	*eventParamsPtr= eventParams;
    char	buf1[256];
    char	buf2[256];
    *eventName= 0;
    *eventParams= 0;
    *buf1= 0;
    *buf2= 0;

    strcpy(eventName, "EVENT ");

    switch (event->type)
    {
	case OBJEV_ALLOW_GEOMETRY_CHANGE:
	    strcat(eventName, "AllowGeometryChange");
	    obj= event->info.allow_geometry_change.obj;
	    sprintf(eventParams, "(%s %d %d %d %d)",
		obj_get_safe_name(obj, buf1, 256),
		event->info.allow_geometry_change.new_x,
		event->info.allow_geometry_change.new_y,
		event->info.allow_geometry_change.new_width,
		event->info.allow_geometry_change.new_height);
	break;

	case OBJEV_ALLOW_REPARENT:
	    strcat(eventName, "AllowReparent");
	    obj= event->info.allow_reparent.obj;
	    sprintf(eventParams, "(%s %s)",
		obj_get_safe_name(obj, buf1, 256), 
		obj_get_safe_name(event->info.allow_reparent.new_parent,
				buf2, 256));
	break;

	case OBJEV_ATT_CHANGE:
	    strcat(eventName, "AttChange");
	    obj= event->info.att_change.obj;
	    sprintf(eventParams, "(%s", obj_get_safe_name(obj, buf1, 256));
	    eventParamsPtr= eventParams + strlen(eventParamsPtr);
	    switch (event->info.att_change.atts)
	    {
		case OBJEV_ATT_GEOMETRY:
		{
		    int		x, y, w, h;
		    obj_get_geometry(obj, &x, &y, &w, &h);
		    strcat(eventName, "/Geometry");
		    sprintf(eventParamsPtr, "x:%d  y:%d  w:%d  h:%d",
			x, y, w, h);
		}
		break;

		case OBJEV_ATT_LABEL:
		    strcat(eventName, "Label");
		    sprintf(eventParamsPtr, "%s", 
				util_strsafe(obj_get_label(obj)));
		break;

		case OBJEV_ATT_HSCROLL:
		    strcat(eventName, "HScroll");
		    sprintf(eventParamsPtr, "%s",
			util_cvt_bool_to_string(
				obj_has_hscrollbar(obj), buf1, 256));
		break;

		case OBJEV_ATT_SELECTED:
		    strcat(eventName, "Selected");
		    sprintf(eventParamsPtr, "%s",
			util_cvt_bool_to_string(
				obj_is_selected(obj), buf1, 256));
		break;

		case OBJEV_ATT_VSCROLL:
		    strcat(eventName, "VScroll");
		    sprintf(eventParamsPtr, "%s",
			util_cvt_bool_to_string(
				obj_has_vscrollbar(obj), buf1, 256));
		break;
	    }
	    strcat(eventParams, ")");
	break;

	case OBJEV_CREATE:
	    strcat(eventName, "Create");
	    obj= event->info.create.obj;
	break;

	case OBJEV_DESTROY:
	    strcat(eventName, "Destroy");
	    obj= event->info.destroy_info.obj;
	break;

	case OBJEV_REPARENT:
	    strcat(eventName, "Reparent");
	    obj= event->info.reparent.obj;
	    sprintf(eventParams, "(%s old_parent:%s)",
		obj_get_safe_name(obj, buf1, 256),
		obj_get_safe_name(event->info.reparent.old_parent, 
					buf2, 256));
	break;

	case OBJEV_UPDATE:
	    strcat(eventName, "Update");
	    obj= event->info.update.obj;
	break;

	case OBJEV_UPDATE_WITH_DATA:
	    strcat(eventName, "UpdateWithData");
	    obj= event->info.update_with_data.obj;
	    sprintf(eventParams, "(%s %d 0x%08lx)",
		obj_get_safe_name(obj, buf1, 256),
		event->info.update_with_data.update_code,
		(unsigned long) event->info.update_with_data.update_data);
	break;

	default:
	    obj= event->info.reparent.obj;
	    sprintf(eventName + strlen(eventName), "%d", event->type);
	break;
    }

    if (*eventParams == 0)
    {
	char	objName[1024];
	sprintf(objName, "(%#lx", (unsigned long) obj);
	if (obj_get_name(obj) != NULL)
	{
	    strcat(objName, " = ");
	    strcat(objName, obj_get_name(obj));
	}
	strcat(objName, ")");
	sprintf(eventParams, "(%s)", objName);
    }

    if (name != NULL)
    {
        strcat(eventName, ": ");
	strcat(eventName, name);
    }

    fprintf(outFile, "%s%s", eventName, eventParams);
    if (addNewline)
    {
	fprintf(outFile, "\n"); fflush(outFile);
    }
    return 0;
}


static int
event_queue_print(void)
{
    int		oldFirst= firstEventIndex;
    int		oldLast= lastEventIndex;

    if (queue_is_empty())
    {
	printf("queue is empty!\n");
	return 0;
    }

    printf("Event queue:\n");
    while (!queue_is_empty())
    {
	event_print(&(eventQueue[firstEventIndex]), stdout, TRUE, NULL);
	firstEventIndex= queue_next_index(firstEventIndex);
    }
    printf("\n");
    firstEventIndex= oldFirst;
    lastEventIndex= oldLast;
    return 0;
}


/*************************************************************************
**                                                                      **
**       PRIVATE FUNCTIONS for Callback lists				**
**                                                                      **
**************************************************************************/

static int
add_to_callback_list(NamedCallbackList *ppsList, 
	GenericCallback	callback, 
	OBJEV_ATT_FLAGS atts,
	ISTRING		callbackName)
{
#define list (*ppsList)
    int			iNumEntries;
    NamedCallbackList	newList= NULL;

    if (list == NULL)
    {
	list= (NamedCallbackList)util_malloc(1 * sizeof(NAMED_CALLBACK));
	if (list == NULL)
	{
	    return ERR_NO_MEMORY;
	}
	list[0].callback= NULL;
    }

    for (iNumEntries= 0; list[iNumEntries].callback != NULL; )
    {
	++iNumEntries;
    }
    iNumEntries+= 2;
    newList= (NamedCallbackList)
		realloc(list, iNumEntries * sizeof(NAMED_CALLBACK));
    if (newList == NULL)
    {
	return ERR_NO_MEMORY;
    }
    list= newList;
    list[iNumEntries-2].callback= callback;
    list[iNumEntries-2].atts= atts;
    list[iNumEntries-2].name= callbackName;
    list[iNumEntries-1].callback= NULL;
    return 0;
#undef list
}


static int
call_callbacks_in_list(NamedCallbackList list, void *data, ObjEvent event)
{
    int			iRC= 0;		/* return code */
    int			iReturn= 0;	/* our return value */
    int			i= 0;
    GenericCallback	func= NULL;

    if (list == NULL)
    {
	return 0;
    }

    for (i= 0; list[i].callback != NULL; ++i)
    {
	/*
	 * Only call attribute callbacks where masks match
	 */
	if (   (event->type == OBJEV_ATT_CHANGE)
	    && ((event->info.att_change.atts & list[i].atts) 
			!= event->info.att_change.atts) )
	{
	    continue;
	}

	if (should_print_callbacks())
	{
	    event_print(event, stderr, FALSE, istr_string(list[i].name));
	}

	func= list[i].callback;
	iRC= func(data);
	if ((iRC < 0) && (iReturn >= 0))
	{
	    iReturn= iRC;
	}

	if (should_print_callbacks())
	{
	    fprintf(stderr, " -> %d\n", iRC); fflush(stderr);
	}
    }

    return iReturn;
}


static BOOL
event_is_in_queue(ObjEvent event)
{
    int		eventIndex= firstEventIndex;
    BOOL	foundIt= FALSE;

    if (queue_is_empty())
    {
	return FALSE;
    }

    eventIndex= queue_next_index(eventIndex);

    for (; (!foundIt) && (eventIndex != lastEventIndex);
		eventIndex= queue_next_index(eventIndex))
    {
	foundIt= events_equal(event, &(eventQueue[eventIndex]));
    }

    return foundIt;
}


static BOOL
events_equal(ObjEvent event1, ObjEvent event2)
{
    BOOL	equal= FALSE;

    if (event1->type != event2->type)
    {
	return FALSE;
    }

    switch (event1->type)
    {
	case OBJEV_ATT_CHANGE:
	{
	    if ((event1->info.att_change.obj != event2->info.att_change.obj)||
		(event1->info.att_change.atts != event2->info.att_change.atts)
		)
	    {
		break;
	    }
	    switch (event1->info.att_change.atts)
	    {
		case OBJEV_ATT_GEOMETRY:
		case OBJEV_ATT_POSITION:
		case OBJEV_ATT_SIZE:
		break;
		case OBJEV_ATT_HSCROLL:
		break;
		case OBJEV_ATT_LABEL:
		break;
		case OBJEV_ATT_SELECTED:
		break;
		case OBJEV_ATT_VSCROLL:
		break;
	    }
	}
	break;

	case OBJEV_CREATE:
		equal= (event1->info.create.obj == event2->info.create.obj);
	break;

	case OBJEV_DESTROY:
  	    equal= (event1->info.destroy_info.obj 
				== event2->info.destroy_info.obj);
	break;

	case OBJEV_REPARENT:
	    equal= (event1->info.reparent.obj == event2->info.reparent.obj);
	break;

	case OBJEV_UPDATE:
	    equal= (event1->info.update.obj == event2->info.update.obj);
	break;

	case OBJEV_UPDATE_WITH_DATA:
	{
	    OBJEV_UPDATE_WITH_DATA_INFO	*info1= 
			&(event1->info.update_with_data);
	    OBJEV_UPDATE_WITH_DATA_INFO	*info2= 
			&(event2->info.update_with_data);

	    equal= (   (info1->obj == info2->obj)
		    && (info1->update_data == info2->update_data)
		    && (info1->update_data_free_func 
				== info2->update_data_free_func) );
	}
	break;
    }

    return equal;
}


