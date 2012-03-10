/* $TOG: FPExceptions.h /main/4 1998/04/17 11:46:25 mgreess $ */


#ifndef _FPExceptions_h
#define _FPExceptions_h

#include "Exceptions.hh"

/////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////
class hardCopyRendererException : public Exception
{
public:
  DECLARE_EXCEPTION(hardCopyRendererException, Exception);
};

#if defined(linux)
#define CASTHCREXCEPT (hardCopyRendererException*)
#else
#define CASTHCREXCEPT
#endif

#endif /* _FPExceptions_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
