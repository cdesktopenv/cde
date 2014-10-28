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
 * $XConsortium: mmdb_exception.h /main/6 1996/11/01 10:19:24 drk $
 *
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */


#ifndef _mmdb_exception_h
#define _mmdb_exception_h 1

#include "Exceptions.hh"

#ifdef C_API
#include "utility/c_fstream.h"
#else
#include <fstream>
#include <iostream>
using namespace std;
#endif

#define END_TRY end_try 

#include <X11/Xosdefs.h>
#include <errno.h>
#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

class mmdbException : public Exception
{
public:
   DECLARE_EXCEPTION(mmdbException, Exception)

   virtual ~mmdbException() {};

   virtual ostream& asciiOut(ostream&);

   friend ostream& operator <<(ostream& out, mmdbException& e) {
      return e.asciiOut(out);
   }
};


class stringException : public mmdbException
{
protected:
   char* msg;

public:
   DECLARE_EXCEPTION(stringException, mmdbException)

   stringException(char const* m) : msg((char*)m) {};
   ~stringException() {};

   virtual ostream& asciiOut(ostream&);
};

class formatException : public stringException
{

protected:

public:
   DECLARE_EXCEPTION(formatException, stringException)

   formatException(char const* m) : stringException(m) {};
   ~formatException() {};
};


class intException : public mmdbException
{

protected:
   int v_code;

public:
   DECLARE_EXCEPTION(intException, mmdbException)

   intException(int c) : v_code(c) {};
   ~intException() {};

   int code() { return v_code; };

   virtual ostream& asciiOut(ostream&);

};

class systemException : public intException
{

public:
   DECLARE_EXCEPTION(systemException, intException)

   systemException(int c) : intException(c) {};
   ~systemException() {};
};

class streamException : public intException
{

protected:

public:
   DECLARE_EXCEPTION(streamException, intException)

   streamException(int c) : intException(c) {};
   ~streamException() {};
};

class boundaryException : public mmdbException
{

protected:
   long low;
   long high;
   long mindex;

public:
   DECLARE_EXCEPTION(boundaryException, mmdbException)

   boundaryException(long l, long h, long i) : 
     low(l), high(h), mindex(i) {};
   ~boundaryException() {};

   virtual ostream& asciiOut(ostream&);
};


class beginTransException: public mmdbException
{
public:
   DECLARE_EXCEPTION(beginTransException, mmdbException)
   beginTransException() {};
   ~beginTransException() {};
};

class commitTransException: public mmdbException
{
public:
   DECLARE_EXCEPTION(commitTransException, mmdbException)
   commitTransException() {};
   ~commitTransException() {};
};

class rollbackTransException: public mmdbException
{
public:
   DECLARE_EXCEPTION(rollbackTransException, mmdbException)
   rollbackTransException() {};
   ~rollbackTransException() {};
};


class demoException : public mmdbException
{
protected:
   const char* f_path;
   const char* f_name;

public:
   DECLARE_EXCEPTION(demoException, mmdbException)

   demoException(const char* p, const char* n) : f_path(p), f_name(n) {};
   virtual ~demoException() {};

   const char* path() { return f_path; };
   const char* name() { return f_name; };

   virtual ostream& asciiOut(ostream& out) {
      out << f_path << "\t" << f_name << "\n";
      return out;
   }

   friend ostream& operator <<(ostream& out, demoException& e) {
       return e.asciiOut(out);
   }
};


   
#endif
