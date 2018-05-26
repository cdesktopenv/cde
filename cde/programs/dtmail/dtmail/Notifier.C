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
 *+SNOTICE
 *
 *	$XConsortium: Notifier.C /main/4 1996/04/21 19:42:47 drk $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
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
 *+ENOTICE
 */

#include <assert.h>
#include <unistd.h>
#include <signal.h>
#include "Notifier.hh"

NotifyEvent::NotifyEvent(void)
{
}

NotifyEvent::~NotifyEvent(void)
{
}

void
NotifyEvent::eventTriggered(void)
{
    assert(!"Pure virtual NotifyEvent::eventTriggered called");
}

Notifier::EventKey::EventKey(void * key)
: ObjectKey("Notifier::EventKey")
{
    _key = key;
}

Notifier::EventKey::~EventKey(void)
{
}

Notifier::EventKey::operator==(ObjectKey & other)
{
    EventKey * ok = (EventKey *)&other;

    return(_key == ok->_key);
}

Notifier::EventKey::operator!=(ObjectKey & other)
{
    EventKey * ok = (EventKey *)&other;

    return(_key != ok->_key);
}

Notifier::EventKey::operator<(ObjectKey & other)
{
    EventKey * ok = (EventKey *)&other;

    return(_key < ok->_key);
}

Notifier::EventKey::operator>(ObjectKey & other)
{
    EventKey * ok = (EventKey *)&other;

    return(_key > ok->_key);
}

Notifier::EventKey::operator<=(ObjectKey & other)
{
    EventKey * ok = (EventKey *)&other;

    return(_key <= ok->_key);
}

Notifier::EventKey::operator>=(ObjectKey & other)
{
    EventKey * ok = (EventKey *)&other;

    return(_key >= ok->_key);
}

HashVal
Notifier::EventKey::hashValue(void)
{
    unsigned long lkey = (unsigned long)_key;

    return(((lkey >> 16) & 0xffff) ^ (lkey & 0xffff));
}

// These constants are used to keep the ends of the pipe straight.
//
static const int	READ = 0;
static const int	WRITE = 1;

Notifier::Notifier(XtAppContext context)
: _events(5), // Pending events that need to be sent.
  _timer_events(8) // Events for timing.
{
    // Create a pipe for sending events.
    //
    pipe(_event_fds);

    // Register the input file descriptor for callback.
    //
    XtAppAddInput(context,
		  _event_fds[READ],
		  (XtPointer)(XtInputReadMask | XtInputExceptMask),
		  eventProc,
		  this);

    _context = context;
}

int
Notifier::deleteTimerEvent(ObjectKey &, TimerEvent * event, void *)
{
    XtRemoveTimeOut(event->id);
    delete event->event;
    delete event;
    return(1);
}

Notifier::~Notifier(void)
{
    XtRemoveInput(_id);

    close(_event_fds[READ]);
    close(_event_fds[WRITE]);

    // We need to run through all of the pending events. They may
    // be interesting to the recipient and should be delivered before
    // we shutdown.
    //
    while (_events.length()) {
	NotifyEvent * event = _events[0];
	event->eventTriggered();
	delete event;
	_events.remove(0);
    }

    // Now we want to throw away all of the events we have laying about
    // for timers and signals.
    //
    _timer_events.forEach(deleteTimerEvent, NULL);
}

void
Notifier::notify(NotifyEvent & event, DtMailBoolean fast_path)
{
    // The fast path means use an immediate call. Really pretty
    // simple. Fire the event and throw away the memory for it.
    //
    if (fast_path == DTM_TRUE) {
	event.eventTriggered();
	delete &event;
    }

    // A little more complex. We will add the event to the queue, and
    // send a byte through the pipe. This will cause us to wake up
    // later, after going through the queue.
    //
    _events.append(&event);

    char bogus_buf = 0;
    write(_event_fds[WRITE], &bogus_buf, 1);
    return;
}

Notifier::IntervalId
Notifier::addInterval(int interval_ms,
		      DtMailBoolean multi_shot,
		      NotifyEvent & event)
{
    TimerEvent * t_event = new TimerEvent;

    t_event->interval = interval_ms;
    t_event->multi_shot = multi_shot;
    t_event->event = &event;

    t_event->id = XtAppAddTimeOut(_context, interval_ms, timerProc, this);

    EventKey * key = new EventKey((void *)t_event->id);
    _timer_events.set(*key, t_event);

    return(t_event);
}

void
Notifier::removeInterval(IntervalId id)
{
    // The Id is really the TimerEvent structure pointer. We don't have
    // a key for these objects so we will have to enumerate the entire
    // list of timer events until we find the appropriate key.
    //
    TimerEvent * t_event = (TimerEvent *)id;

    TimerSearch t_srch;
    t_srch.srch_event = t_event;
    t_srch.key = NULL;

    _timer_events.forEach(searchTimer, &t_srch);

    if (t_srch.key) {
	_timer_events.remove(*t_srch.key);
	delete t_event->event;
	delete t_event;
    }

    return;
}

void
Notifier::eventProc(XtPointer client_data, int * fd, XtInputId *)
{
    Notifier * self = (Notifier *)client_data;

    // There was activity on the pipe. Read one byte, and fire one
    // event. We don't want to fire more than that or we will run
    // the risk of spending too much time in the callbacks.
    //
    char bogus_buf;
    read(*fd, &bogus_buf, 1);

    if (self->_events.length()) {
	NotifyEvent * event = self->_events[0];
	event->eventTriggered();
	delete event;
	self->_events.remove(0);
    }

    return;
}

void
Notifier::timerProc(XtPointer client_data, XtIntervalId *id)
{
    Notifier * self = (Notifier *)client_data;
    EventKey key((void *)*id);

    TimerEvent * t_event = self->_timer_events.lookup(key);
    if (!t_event) {
	// Bogus event.
	return;
    }

    t_event->event->eventTriggered();

    // We remove the event from the list. We do this because the
    // key will change because we will get a new interval id.
    //
    self->_timer_events.remove(key);

    // If this is a multi-shot event we have to register it again.
    //
    if (t_event->multi_shot == DTM_TRUE) {
	t_event->id = XtAppAddTimeOut(self->_context,
				      t_event->interval,
				      timerProc,
				      self);
	EventKey * new_key = new EventKey((void *)t_event->id);
	self->_timer_events.set(*new_key, t_event);
    }
    else {
	delete t_event->event;
	delete t_event;
    }
}

int
Notifier::searchTimer(ObjectKey & key, TimerEvent * event, void * client_data)
{
    TimerSearch * srch = (TimerSearch *)client_data;

    if (event == srch->srch_event) {
	srch->key = (EventKey *)&key;
	return(0);
    }

    return(1);
}
