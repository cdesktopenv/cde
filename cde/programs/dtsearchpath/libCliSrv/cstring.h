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

#include <iostream.h>
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

  friend ostream & operator<< (ostream &, const CString &);

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
