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
#if defined(linux) || defined(CSRG_BASED)
  friend std::ostream & operator<< (std::ostream &, TTFile &);
#else
  friend ostream & operator<< (ostream &, TTFile &);
#endif
#endif

  int  ttFileOpFailed () { return status != TT_OK; }
  Tt_status getStatus()  { return status; }

 private:
  Tt_status status;
};
#endif
