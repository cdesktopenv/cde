/* $XConsortium: TTFile.h /main/3 1995/11/03 12:33:57 rswiston $ */
/*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1990, 1991, 1992, 1993.
**  All rights are reserved.  Copying or other reproduction of this
**  program except for archival purposes is prohibited without prior
**  written consent of Hewlett-Packard Company.
********************************************************************
****************************<+>*************************************/

#ifndef _TTFILE_H_
#define _TTFILE_H_

#include "cstring.h"
#include "Tt/tt_c.h"

#ifdef HAS_EXCEPTIONS
#define Throw(ex)  throw ex
#define Try try
#define Catch(type,file) catch(type ex)
#else
#define Throw(ex)  { ex; return; }
#define Try
#define Catch(type,file)  if (file->ttFileOpFailed())
#endif
 
class TTFile : public CString {
 public:
  TTFile () : status(TT_OK) {}
  TTFile (const CString &, const CString &);
  TTFile (const TTFile &);
  ~TTFile();

  TTFile & operator=(const TTFile &);

#ifdef HAS_EXCEPTIONS
  class TT_Exception {
   public:
    TT_Exception (char * str) 
      { cerr << tt_status_message(tt_pointer_error(str)); }
    ~TT_Exception() {}
    
    friend ostream & operator<< (ostream &, TTFile &);
  };
#else
  void TT_Exception (char *);
  friend ostream & operator<< (ostream &, TTFile &);
#endif

  int  ttFileOpFailed () { return status != TT_OK; }
  Tt_status getStatus()  { return status; }

 private:
  Tt_status status;
};
#endif
