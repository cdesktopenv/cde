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
