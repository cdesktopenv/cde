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
/* $XConsortium: SearchPath.h /main/4 1996/03/27 14:39:41 barstow $ */
/*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1990, 1991, 1992, 1993.
**  All rights are reserved.  Copying or other reproduction of this
**  program except for archival purposes is prohibited without prior
**  written consent of Hewlett-Packard Company.
********************************************************************
****************************<+>*************************************/

#ifndef _SEARCHPATH_H_
#define _SEARCHPATH_H_

#include "Environ.h"
#include "cstring.h"
#if defined(linux) || defined(CSRG_BASED)
#include <iostream>
#else
#include <iostream.h>
#endif
#include <stdio.h>

class SearchPath {
 public:
  SearchPath() {}
  SearchPath (CDEEnvironment *, const char *, const char *);
  virtual ~SearchPath();

  virtual void ExportPath();
  virtual void Print();
  virtual void AddPredefinedPath();

#if defined(linux) || defined(CSRG_BASED)
  virtual void PrettyPrint (std::ostream &) const;

  friend std::ostream & operator<< (std::ostream &, const SearchPath &);
#else
  virtual void PrettyPrint (ostream &) const;

  friend ostream & operator<< (ostream &, const SearchPath &);
#endif

  const char *  GetEnvVar() const    { return environment_var; }
  CString       GetSearchPath() const { return final_search_path; }

 protected:
  void  AssembleString (const CString &);
  void  NormalizePath();
  void  AddToPath (const CString &);
  void  TraversePath();

  virtual void    MakePath (const CString &) {}
  virtual CString ConstructPath (const CString &, const CString *,
				 unsigned char useTT = 1);

  int     ParseState() const   { return parse_state; }
  CString Separator() const    { return separator; }

  virtual int validSearchPath (const CString &) const;
  virtual int useSystemPath();

  void    setSeparator (const char * sep) { separator = sep; }

  CString          search_path;
  CString          norm_search_path;
  CString          final_search_path;
  CDEEnvironment * user;

 private:
  const char *     environment_var;
  int              parse_state;
  CString          save_host;
  CString          separator;
};


class AppSearchPath : public SearchPath {
 public:
  AppSearchPath (CDEEnvironment *, 
		 const char * = "DTAPP",
		 const char * = ",");
  virtual ~AppSearchPath() {}

  void FixUp();

 protected:
  virtual void    MakePath (const CString &);

 private:
};


class IconSearchPath : public SearchPath {
 public:
  IconSearchPath (CDEEnvironment *,
		  const char * = "XMICON",
		  const char * = ".pm",
		  const char * = ".bm",
		  const char * = ":");
  virtual ~IconSearchPath() {}

 protected:
  virtual void    MakePath (const CString &);
  virtual int     validSearchPath (const CString &) const;

 private:
  CString first;
  CString second;
};


class DatabaseSearchPath : public SearchPath {
 public:
  DatabaseSearchPath (CDEEnvironment *,
		      const char * = "DTDATABASE",
		      const char * = ",");
  virtual ~DatabaseSearchPath() {}

 protected:
  virtual void    MakePath (const CString &);
  virtual CString ConstructPath (const CString &, const CString *,
				 unsigned char useTT = 0);

 private:
};


class HelpSearchPath : public SearchPath {
 public:
  HelpSearchPath (CDEEnvironment *, 
		  const char * = "DTHELP",
		  const char * = ":");
  virtual ~HelpSearchPath() {}

 protected:
  virtual void    MakePath (const CString &);
  virtual int     validSearchPath (const CString &) const;

 private:
};


class InfoLibSearchPath : public SearchPath {
 public:
  InfoLibSearchPath (CDEEnvironment *, 
		  const char * = "DTINFOLIB",
		  const char * = ":");
  virtual ~InfoLibSearchPath() {}

 protected:
  virtual void    MakePath (const CString &);
  virtual int     validSearchPath (const CString &) const;

 private:
};


class ManSearchPath : public SearchPath {
 public:
  ManSearchPath (CDEEnvironment *, 
		 const char * = "MANPATH",
		 const char * = ":");
  virtual ~ManSearchPath() {}

  virtual void    ExportPath ();
  virtual void    Print();

#if defined(linux) || defined(CSRG_BASED)
  friend std::ostream & operator<< (std::ostream &, const ManSearchPath &);
#else
  friend ostream & operator<< (ostream &, const ManSearchPath &);
#endif

 protected:
  virtual void    MakePath (const CString &);
#if defined(__FreeBSD__)
  virtual int     useSystemPath();
#endif

 private:
};

#endif
