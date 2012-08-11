/*
 *+SNOTICE
 *
 *
 *	$TOG: Threads.hh /main/5 1997/09/03 17:27:34 mgreess $
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

#ifndef _THREADS_HH
#define _THREADS_HH

#include <sys/time.h>
#include <DtMail/DtLanguages.hh>

// This mutex must be used before calling any Dt* functions. They
// are extremely MT-UNSAFE and must be protected.
//
extern void * _DtMutex;

// This function will create an initialized mutex.
//
void * MutexInit(void);
void MutexDestroy(void * mutex);

// This class locks the specified mutex when constructed, and
// releases the lock on destruction. It is useful for mutexing
// a region of code automatically by including it in the scope
// of a set of braces.
//
class MutexLock : public DtCPlusPlusAllocator {
  public:
    MutexLock(void * mutex);
    ~MutexLock(void);

    void unlock(void);
    void unlock_and_destroy(void);
  private:
    void *	_mutex;
    int		_locked;
};

// The SafeScalar class is designed to allow safe access to scalar values
// from within an MT app. It should only be used with the common 32 bit
// integral values.
//
class SafeScalarImpl : public DtCPlusPlusAllocator {
  public:
    SafeScalarImpl(void);
    ~SafeScalarImpl(void);

    long operator = (const long);
    long operator += (const long);
    long operator -= (const long);
    long operator *= (const long);
    long operator /= (const long);

    int operator == (const long);
    int operator <= (const long);
    int operator < (const long);
    int operator >= (const long);
    int operator > (const long);
    int operator != (const long);

    operator long(void);

  private:
    void *	_mutex;
    long	_value;
};

template <class Scalar>
class SafeScalar : public DtCPlusPlusAllocator {
  public:
    SafeScalar(void) : _scalar() { }
    ~SafeScalar(void) { }

    Scalar operator= (const Scalar val) { return((Scalar)(long)(_scalar = (long)val)); }
    Scalar operator += (const Scalar val) { return((Scalar)(long)(_scalar += (long)val)); }
    Scalar operator -= (const Scalar val) { return((Scalar)(long)(_scalar -= (long)val)); }
    Scalar operator *= (const Scalar val) { return((Scalar)(long)(_scalar *= (long)val)); }
    Scalar operator /= (const Scalar val) { return((Scalar)(long)(_scalar /= (long)val)); }

    int operator == (const Scalar val) { return(_scalar == (long)val); }
    int operator <= (const Scalar val) { return(_scalar <= (long)val); }
    int operator < (const Scalar val) { return(_scalar < (long)val); }
    int operator >= (const Scalar val) { return(_scalar >= (long)val); }
    int operator > (const Scalar val) { return(_scalar > (long)val); }
    int operator != (const Scalar val) { return(_scalar != (long)val); }

    operator Scalar (void) { return((Scalar)(long)_scalar); }

  private:
    SafeScalarImpl	_scalar;
};

// The condition class is used to block an appliction until the
// condition has changed. This is typically used while creating or
// destroying a class.
//
class Condition : public DtCPlusPlusAllocator {
  public:
    Condition(void);
    ~Condition(void);

    void setTrue(void);
    void setFalse(void);

    int state(void);
    int operator=(int);
    int operator += (int);
    operator int(void);

    void wait(void);

    void waitTrue(void);
#ifdef DEAD_WOOD
    void waitFalse(void);
    void waitFor(int);
    void waitGT(int); // wait >
    void waitLT(int); // wait <
    void waitProcStatus(void);
#endif /* DEAD_WOOD */

  private:
    void *	_mutex;
    void *	_condition;
    int		_state;
};

typedef unsigned int Thread;

typedef void * (*ThreadEntryPoint)(void *);

Thread ThreadCreate(ThreadEntryPoint, void * client_data);
Thread ThreadSelf(void);
void ThreadPrio(Thread, const int prio);
void ThreadKill(Thread, const int signal);
void ThreadExit(const int status);
void ThreadJoin(Thread);
time_t ThreadSleep(time_t seconds);

#endif
