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
/*%%  $XConsortium: ttdtprocid.h /main/3 1995/10/23 10:32:34 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * @(#)ttdtprocid.h	1.3	93/09/07
 *
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */
#ifndef ttdtprocid_h
#define ttdtprocid_h

#include "util/tt_new.h"
#include "tttk/tttk.h"

class _TtDtProcid: public _Tt_allocated {
    public:
				_TtDtProcid(
					const char	       *toolName,
					const char	       *vendor,
					const char	       *version
				);
				~_TtDtProcid();

	const char	       *toolname()	const {return _toolname;}
	const char	       *vendor()	const {return _vendor;}
	const char	       *version()	const {return _version;}

	Tt_pattern	       *pats_create(
					Tt_message		contract,
					Ttdt_contract_cb	cb,
					Widget			shell,
					void		       *clientData
				) const;
    private:
	char		       *_toolname;
	char		       *_vendor;
	char		       *_version;
};

#endif
