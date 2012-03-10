/* $TOG: cc_exceptions.h /main/5 1998/04/17 11:44:44 mgreess $ */

#ifndef _cc_exception_h
#define _cc_exception_h 1

#include <fstream.h>
#include "Exceptions.hh"

#define END_TRY end_try 

#include <X11/Xosdefs.h>
#include <errno.h>
#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

#if defined(linux)
#define CASTCCEXCEPT (ccException*)
#define CASTCCSEXCEPT (ccStringException*)
#define CASTCCBEXCEPT (ccBoundaryException*)
#else
#define CASTCCEXCEPT
#define CASTCCSEXCEPT
#define CASTCCBEXCEPT
#endif

class ccException : public Exception
{
public:
   DECLARE_EXCEPTION(ccException, Exception);

   virtual ~ccException() {};

   virtual ostream& asciiOut(ostream&);

   friend ostream& operator <<(ostream& out, ccException& e) {
      return e.asciiOut(out);
   }
};


class ccStringException : public ccException
{
protected:
   char* msg;

public:
   DECLARE_EXCEPTION(ccStringException, ccException);

   ccStringException(char* m) : msg(m) {};
   ~ccStringException() {};

   virtual ostream& asciiOut(ostream&);
};

class ccBoundaryException : public ccException
{

protected:
   int low;
   int high;
   int index;

public:
   DECLARE_EXCEPTION(ccBoundaryException, ccException);

   ccBoundaryException(int l, int h, int i) : 
     low(l), high(h), index(i) {};
   ~ccBoundaryException() {};

   virtual ostream& asciiOut(ostream&);
};


#endif
