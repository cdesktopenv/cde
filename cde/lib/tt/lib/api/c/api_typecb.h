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
/*%%  $TOG: api_typecb.h /main/5 1999/10/14 18:40:11 mgreess $ 			 				 */
/* -*-C++-*-
 *
 * @(#)api_typecb.h	1.2 93/07/30 SMI
 *
 * Copyright (c) 1993 by Sun Microsystems, Inc.
 * 
 * _Tt_typecb is a class which holds a <typename, opnum, callback list>
 * triple.  Tables of these are attached to _Tt_c_procid instances to hold
 * the callbacks registerd by tt_ptype_opnum_callback_add and
 * tt_otype_opnum_callback_add.
 */

#ifndef _API_TYPECB_H
#define _API_TYPECB_H

#include "util/tt_object.h"
#include "api/c/tt_c.h"
#include "api/c/api_handle.h"

#if defined(__linux__)
// Avoid g++ compiler errors on linux.
#define typename typenm
#endif

class _Tt_typecb : public _Tt_object {
      public:
	_Tt_typecb();
	_Tt_typecb(const _Tt_string &typenm, int opnum);
	virtual ~_Tt_typecb();
	_Tt_string &	get_typename();
	int		opnum();
	void		add_callback(Tt_message_callback f);
	Tt_callback_action run_callbacks(Tt_message mh, Tt_pattern ph);
	void	       *clientdata;
      private:
	_Tt_string	_typename;
	int		_opnum;
	_Tt_api_callback_list_ptr cblp;
};
	


#endif				/* _MP_PATTERN_H */
