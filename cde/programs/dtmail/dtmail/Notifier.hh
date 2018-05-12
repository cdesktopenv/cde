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
 *	$XConsortium: Notifier.hh /main/4 1996/04/21 19:42:50 drk $
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

#ifndef NOTIFIER_HH
#define NOTIFIER_HH

#include <X11/Intrinsic.h>

#include <DtMail/DtMailTypes.h>
#include <DtMail/DtVirtArray.hh>
#include <DtMail/HashTable.hh>

class NotifyEvent {
  public:
    NotifyEvent(void);
    virtual ~NotifyEvent(void);

    virtual void eventTriggered(void); // PURE VIRTUAL
};

class Notifier {
  public:
    Notifier(XtAppContext);
    virtual ~Notifier(void);

    void notify(NotifyEvent & event,
		DtMailBoolean fast_path = DTM_FALSE);

    typedef void * IntervalId;
    IntervalId addInterval(int interval_ms,
			   DtMailBoolean multi_shot,
			   NotifyEvent & event);
    void removeInterval(IntervalId id);

    void signalEvent(int sig, NotifyEvent & event);
    void removeSignal(int sig);

  private:
    int				_event_fds[2];
    DtVirtArray<NotifyEvent *>	_events;

    struct TimerSearch;
    friend TimerSearch;

    class EventKey : public ObjectKey {
      public:
	    EventKey(void * key);
	    ~EventKey(void);
	    
	    virtual int operator==(ObjectKey &);
	    virtual int operator!=(ObjectKey &);
	    virtual int operator<(ObjectKey &);
	    virtual int operator>(ObjectKey &);
	    virtual int operator<=(ObjectKey &);
	    virtual int operator>=(ObjectKey &);
	    
	    virtual HashVal hashValue(void);

	    void * keyValue(void) { return _key; }
	  private:
	    void *	_key;
	};

    struct TimerEvent {
	NotifyEvent	*event;
	DtMailBoolean	multi_shot;
	int		interval;
	XtIntervalId	id;
    };

    HashTable<TimerEvent *>	_timer_events;

    XtAppContext		_context;
    XtInputId			_id;

    static void eventProc(XtPointer client_data, int * fd, XtInputId *);
    static void timerProc(XtPointer, XtIntervalId *);

    static int deleteTimerEvent(ObjectKey &, TimerEvent *, void *);

    struct TimerSearch {
	TimerEvent *	srch_event;
	EventKey *	key;
    };
    static int searchTimer(ObjectKey &, TimerEvent *, void *);
};

#endif
