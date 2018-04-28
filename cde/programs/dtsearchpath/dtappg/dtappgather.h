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
/*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1990, 1991, 1992, 1993.
**  All rights are reserved.  Copying or other reproduction of this
**  program except for archival purposes is prohibited without prior
**  written consent of Hewlett-Packard Company.
********************************************************************
****************************<+>*************************************/
/*******************************************************************
 *
 *  File:              dtappgather.h
 *
 *  Purpose:           Class definitions for the gathering process
 *
 *  Product:           @(#)Common Desktop Environment 1.0          
 *
 *  Revision:          $XConsortium: dtappgather.h /main/3 1995/11/03 12:30:33 rswiston $
 *
 ********************************************************************/

#include "Environ.h"

class AppManagerDirectory {
 public:
  AppManagerDirectory() {}
  AppManagerDirectory(CDEEnvironment *, const CString &);
  ~AppManagerDirectory() { delete user_; }

  void GatherAppsFromASearchElement (const CString & path);
  void TraversePath();

  char * operator()() const { return dirname_.data(); }

 private:
  int goodFile (const CString &, const CString &) const;

  CString          dirname_;
  CDEEnvironment * user_;
  CString          appsp_;
  int              langVersionFound;
};


