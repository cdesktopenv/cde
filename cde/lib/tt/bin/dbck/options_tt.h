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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: options_tt.h /main/3 1995/10/20 16:25:55 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * options_tt.h
 *
 * Class definitions for classes to parse and hold options from
 * the command lines.
 *
 * Part of the ToolTalk/Link Service data base inspect and repair tool.
 * 
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#ifndef _OPTIONS_TT_H
#define _OPTIONS_TT_H
#include "util/tt_string.h"
#include "options.h"

// options unique to ttdbck

class Dbck_specoptions : public Dbck_options {

      public:
	Dbck_specoptions();

	// Repair options

	virtual int		repairing_p()	const{
		return Dbck_options::repairing_p() |
		       _repair_filename_p;
	};
	int			repair_filename_p() const{
		return _repair_filename_p;
	};
	const _Tt_string       &repair_filename() const{
		return _repair_filename;
	};

	virtual	char *		type_string() const{
		return "Dbck_specoptions";
	};
	virtual void 		print(FILE *f = stdout) const;

      protected:

	// Repair options
	
	int			_repair_filename_p;
	_Tt_string		_repair_filename;

	virtual char *		optstring();
	virtual int		set_option(int optchar, const char *optval);
};

#endif
