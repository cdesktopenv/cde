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
/* $XConsortium: cstring.h /main/3 1995/11/03 12:34:22 rswiston $ */
/*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1990, 1991, 1992, 1993.
**  All rights are reserved.  Copying or other reproduction of this
**  program except for archival purposes is prohibited without prior
**  written consent of Hewlett-Packard Company.
********************************************************************
****************************<+>*************************************/

#ifndef _CSTRING_H_
#define _CSTRING_H_

#if defined(linux) || defined(CSRG_BASED) || defined(sun)
#include <iostream>
#else
#include <iostream.h>
#endif
#include <string.h>

class CString {
 public:
  CString();
  CString(const char * s, unsigned char = 1);
  CString(const char, unsigned char = 1);
  CString(const CString &);
 ~CString();

  int    length() const { return strlen(contents); }
  char * data()   const { return contents; }

  CString & operator= (const CString &);
  CString & operator= (const char *);
  CString & operator+= (const CString &);
  CString & operator+= (const char *);

  int operator!= (const CString &) const;
  int operator== (const CString &) const;

  char & operator[](int) const;

  friend CString operator+ (const CString & s1, const CString & s2);
  friend CString operator+ (const CString & s, const char * cs);
  friend CString operator+ (const char * cs, const CString & s);

  CString   copy (unsigned int, const char *);
  CString   copy (const char *, const char *);
  CString   find (const char *);
  int       contains (const char *, const char * = "", const char * = "") const;
  int       contains (const CString &, const char * = "", const char * = "") const;
  int       isNull() const;
  void      replace (const CString &, const CString &);

#if defined(linux) || defined(CSRG_BASED) || defined(sun)
  friend std::ostream & operator<< (std::ostream &, const CString &);
#else
  friend ostream & operator<< (ostream &, const CString &);
#endif

 protected:
  char *        contents;
  unsigned char skipWhiteSpace;

};


class CTokenizedString : public CString {
 public:
  CTokenizedString();
  CTokenizedString(const CTokenizedString &);
  CTokenizedString(const CString &, char *, unsigned char = 1);
 ~CTokenizedString();

  int     Finished() { return finished == 1; }

  CString next();

 private:
  char *  cursor;
  char *  delimiter;
  int     finished;
};

#endif
