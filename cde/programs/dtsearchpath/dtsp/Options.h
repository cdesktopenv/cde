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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: Options.h /main/3 1995/11/03 12:32:15 rswiston $ */
/*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1990, 1991, 1992, 1993.
**  All rights are reserved.  Copying or other reproduction of this
**  program except for archival purposes is prohibited without prior
**  written consent of Hewlett-Packard Company.
********************************************************************
****************************<+>*************************************/

#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include "cstring.h"

class Options {
 public:
  Options (unsigned int, char **);
 ~Options ();

  int       Verbose () const             { return (flags & 1); }
  int       CheckingUser () const        { return (flags & 2); }
  int       doingTooltalk () const       { return !doingICCCM(); }
  int       doingICCCM () const          { return (flags & 4); }
  int       dontOptimize () const        { return (flags & 8); }
  int       removeAutoMountPoint() const { return (flags & 16); }
  int       useCshEnv() const            { return (flags & 32); }
  int       useKshEnv() const            { return (flags & 64); }

  CString   getAutoMountPoint () const   { return *automountpoint; }
  CString * getUserID () const           { return user_id; }
  CString * getHomeDir () const          { return home_dir; }

  void      setUserID (const CString &);
  void      setAutoMountPoint (const CString &);

 private:
  unsigned char flags;
  CString *     user_id;
  CString *     home_dir;
  CString *     automountpoint;
};

#endif
