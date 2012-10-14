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
/* $TOG: cc_exceptions.h /main/5 1998/04/17 11:44:44 mgreess $ */

#ifndef _cc_exception_h
#define _cc_exception_h 1

#include <fstream>
#include <iostream>
using namespace std;
#include "Exceptions.hh"

#define END_TRY end_try 

#include <X11/Xosdefs.h>
#include <errno.h>
#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

#define CASTCCEXCEPT
#define CASTCCSEXCEPT
#define CASTCCBEXCEPT

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

   ccStringException(char const* m) : msg((char*)m) {};
   ~ccStringException() {};

   virtual ostream& asciiOut(ostream&);
};

class ccBoundaryException : public ccException
{

protected:
   int low;
   int high;
   int mindex;

public:
   DECLARE_EXCEPTION(ccBoundaryException, ccException);

   ccBoundaryException(int l, int h, int i) : 
     low(l), high(h), mindex(i) {};
   ~ccBoundaryException() {};

   virtual ostream& asciiOut(ostream&);
};


#endif
