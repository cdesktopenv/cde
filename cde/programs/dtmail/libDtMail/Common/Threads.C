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
 *+SNOTICE
 *
 *
 *	$TOG: Threads.C /main/5 1997/09/03 17:26:05 mgreess $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#if defined(I_HAVE_SELECT_H)
#include <sys/select.h>
#endif

#if defined(POSIX_THREADS)
#include <thread.h>
#include <synch.h>
#endif

#include <DtMail/DtMail.hh>
#include <DtMail/Threads.hh>
#include <DtMail/IO.hh>

static long	DUMMY_MUTEX;

void *
MutexInit(void)
{
#if defined(POSIX_THREADS)
    mutex_t	*mutex = (mutex_t *)malloc(sizeof(mutex_t));

    mutex_init(mutex, USYNC_THREAD, NULL);
    return(mutex);
#else
    return(&DUMMY_MUTEX);
#endif
}

void
MutexDestroy(void * mutex)
{
#if defined(POSIX_THREADS)
    mutex_destroy((mutex_t *)mutex);
    free(mutex);
#else
    mutex = NULL;
#endif

    return;
}

MutexLock::MutexLock(void * mutex)
{
#if defined(POSIX_THREADS)
    mutex_lock((mutex_t *)mutex);
#endif
    _mutex = mutex;
    _locked = 1;
}

MutexLock::~MutexLock(void)
{
#if defined(POSIX_THREADS)
    if (_locked) {
	mutex_unlock((mutex_t *)_mutex);
    }
#endif

}

void
MutexLock::unlock(void)
{
#if defined(POSIX_THREADS)
    if (_locked) {
	mutex_unlock((mutex_t *)_mutex);
    }
    _locked = 0;
#endif

}

void
MutexLock::unlock_and_destroy(void)
{
#if defined(POSIX_THREADS)
    if (_locked) {
	mutex_unlock((mutex_t *)_mutex);
    }
    _locked = 0;
    MutexDestroy(_mutex);
#endif
}

SafeScalarImpl::SafeScalarImpl(void)
{
    _mutex = MutexInit();
    _value = 0;
}

SafeScalarImpl::~SafeScalarImpl(void)
{
    MutexDestroy(_mutex);
}

long
SafeScalarImpl::operator = (const long val)
{
    MutexLock lock_scope(_mutex);

    _value = val;
    return(_value);
}

long
SafeScalarImpl::operator += (const long val)
{
    MutexLock lock_scope(_mutex);

    _value += val;
    return(_value);
}

long
SafeScalarImpl::operator -= (const long val)
{
    MutexLock lock_scope(_mutex);

    _value -= val;
    return(_value);
}

long
SafeScalarImpl::operator *= (const long val)
{
    MutexLock lock_scope(_mutex);

    _value *= val;
    return(_value);
}

long
SafeScalarImpl::operator /= (const long val)
{
    MutexLock lock_scope(_mutex);

    _value /= val;
    return(_value);
}

int
SafeScalarImpl::operator == (const long val)
{
    MutexLock lock_scope(_mutex);

    return(_value == val);
}

int
SafeScalarImpl::operator <= (const long val)
{
    MutexLock lock_scope(_mutex);

    return(_value <= val);
}

int
SafeScalarImpl::operator < (const long val)
{
    MutexLock lock_scope(_mutex);

    return(_value < val);
}

int
SafeScalarImpl::operator >= (const long val)
{
    MutexLock lock_scope(_mutex);

    return(_value >= val);
}

int
SafeScalarImpl::operator > (const long val)
{
    MutexLock lock_scope(_mutex);

    return(_value > val);
}

int
SafeScalarImpl::operator != (const long val)
{
    MutexLock lock_scope(_mutex);

    return(_value != val);
}

SafeScalarImpl::operator long(void)
{
    MutexLock lock_scope(_mutex);

    return(_value);
}

Condition::Condition(void)
{
    _mutex = MutexInit();

#if defined(POSIX_THREADS)
    _condition = malloc(sizeof(cond_t));
    cond_init((cond_t *)_condition, USYNC_THREAD, NULL);
#endif

}

Condition::~Condition(void)
{
    MutexDestroy(_mutex);

#if defined(POSIX_THREADS)
    cond_destroy((cond_t *)_condition);
    free(_condition);
#endif

}

void
Condition::setTrue(void)
{
    MutexLock lock_scope(_mutex);

    _state = 1;

#if defined(POSIX_THREADS)
    cond_broadcast((cond_t *)_condition); // Wake all sleepers.
#endif

}

void
Condition::setFalse(void)
{
    MutexLock lock_scope(_mutex);

    _state = 0;

#if defined(POSIX_THREADS)
    cond_broadcast((cond_t *)_condition); // Wake all sleepers.
#endif
}

int
Condition::operator=(int new_state)
{
    MutexLock lock_scope(_mutex);

    _state = new_state;

#if defined(POSIX_THREADS)
    cond_broadcast((cond_t *)_condition); // Wake all sleepers.
#endif

    return(new_state);
}

int
Condition::operator+=(int new_state)
{
    MutexLock lock_scope(_mutex);

    _state += new_state;

#if defined(POSIX_THREADS)
    cond_broadcast((cond_t *)_condition); // Wake all sleepers.
#endif

    return(_state);
}

Condition::operator int(void)
{
    return(state());
}

int
Condition::state(void)
{
    MutexLock lock_scope(_mutex);

    int save_state = _state;

    return(save_state);
}

void
Condition::wait(void)
{
    // Wait for anything to change.
    //
#if defined(POSIX_THREADS)
    MutexLock lock_scope(_mutex);

    timestruc_t	abstime;
    abstime.tv_sec = time(NULL) + 1; // Wait for 1 second.
    abstime.tv_nsec = 0;

    cond_timedwait((cond_t *)_condition, (mutex_t *)_mutex, &abstime);
#endif

    return;
}

void
Condition::waitTrue(void)
{
    // Wait for the condition to become true.
    //
#if defined(POSIX_THREADS)
    MutexLock lock_scope(_mutex);

    while(!_state) {
	cond_wait((cond_t *)_condition, (mutex_t *)_mutex);
    }
#else
    _state = 1; // Must set of single threaded apps.
#endif

    return;
}

#ifdef DEAD_WOOD
void
Condition::waitFalse(void)
{
    // Wait for the condition to become true.
    //
#if defined(POSIX_THREADS)
    MutexLock lock_scope(_mutex);

    while(_state) {
	cond_wait((cond_t *)_condition, (mutex_t *)_mutex);
    }
#else
    _state = 0;
#endif

    return;
}

void
Condition::waitFor(int new_state)
{
    // Wait for the condition to become true.
    //
#if defined(POSIX_THREADS)
    MutexLock lock_scope(_mutex);

    while(_state != new_state) {
	cond_wait((cond_t *)_condition, (mutex_t *)_mutex);
    }
#endif

    return;
}

void
Condition::waitGT(int new_state)
{
    // Wait for the condition to become true.
    //
#if defined(POSIX_THREADS)
    MutexLock lock_scope(_mutex);

    while(_state > new_state) {
	cond_wait((cond_t *)_condition, (mutex_t *)_mutex);
    }
#endif

    return;
}

void
Condition::waitLT(int new_state)
{
    // Wait for the condition to become true.
    //
#if defined(POSIX_THREADS)
    MutexLock lock_scope(_mutex);

    while(_state < new_state) {
	cond_wait((cond_t *)_condition, (mutex_t *)_mutex);
    }
#endif

    return;
}

void
Condition::waitProcStatus(void)
{
    // Wait for the condition to become true.
    //
#if defined(POSIX_THREADS)
    MutexLock lock_scope(_mutex);

    while(_state < 0) {
	cond_wait((cond_t *)_condition, (mutex_t *)_mutex);
    }
#else
    _state = 0;
#endif

    return;
}
#endif /* DEAD_WOOD */

Thread
ThreadCreate(
#if defined(POSIX_THREADS)
	ThreadEntryPoint entry, void * client_data)
{
    thread_t	id;

    thr_create(NULL, 0, entry, client_data, THR_BOUND | THR_NEW_LWP, &id);
    thr_continue(id);

    return(id);
}
#else
	ThreadEntryPoint, void*)
{
  return(0);
}
#endif

Thread
ThreadSelf(void)
{
#if defined(POSIX_THREADS)
    return(thr_self());
#else
    return(0);
#endif

}

void
ThreadPrio(
#if defined(POSIX_THREADS)
	Thread thread, const int prio)
{
    thr_setprio((thread_t)thread, prio);
}
#else
	Thread, const int)
{
}
#endif

void
ThreadKill(
#if defined(POSIX_THREADS)
	Thread thread, const int sig)
{
    thr_kill((thread_t)thread, sig);
}
#else
	Thread, const int)
{
}
#endif

void
ThreadExit(
#if defined(POSIX_THREADS)
	const int status)
{
    thr_exit((void *)status);
}
#else
	const int)
{
}
#endif

void
ThreadJoin(
#if defined(POSIX_THREADS)
	Thread thread)
{
    thr_join((thread_t)thread, NULL, NULL);
}
#else
	Thread)
{
}
#endif

// The ThreadSleep function mimics the behavior of sleep(3), but
// uses select to prevent SIGALRM from being sent. This is bad
// on MT because often the main thread gets the signal and exits.
//
time_t
ThreadSleep(time_t secs)
{
    time_t now = time(NULL);

    timeval interval;

    interval.tv_sec = secs;
    interval.tv_usec = 0;

    select(0, NULL, NULL, NULL, &interval);

    time_t slept = time(NULL) - now;
    if (slept < secs) {
	return(secs - slept);
    }

    return(0);
}

#if defined(SPRO_V2)
void * operator new(size_t size)
{
    return(malloc(size));
}

void operator delete(void * ptr)
{
    free(ptr);
}
#endif
