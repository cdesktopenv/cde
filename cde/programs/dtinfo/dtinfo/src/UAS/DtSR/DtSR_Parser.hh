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
// $XConsortium: DtSR_Parser.hh /main/4 1996/10/16 14:24:46 cde-hal $
/*	Copyright (c) 1995 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

#ifndef __DtSR_Parser_HH__
#define __DtSR_Parser_HH__

#include "Registration.hh"

#include "UAS_String.hh"
#include "UAS_OQLParser.hh"

class DtSR_Parser : public UAS_OQLParser
{
public:
  DtSR_Parser();

  int stemming_suggested() { return f_completion_specified; }

private:
  UAS_String se_construct (OQL_Token, UAS_String, UAS_String, UAS_String);

  UAS_String validate(UAS_String &seql);

private:

  // flag to see if completion has been specified in a query
  int f_completion_specified;
  int f_completion_specified_transient;
};

#endif
