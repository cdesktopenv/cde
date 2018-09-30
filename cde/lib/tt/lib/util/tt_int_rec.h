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
/*%%  $XConsortium: tt_int_rec.h /main/3 1995/10/23 10:40:28 rswiston $ 			 				 */
/*
 *
 * tt_int_rec.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#ifndef TT_INT_REC_H
#define TT_INT_REC_H
#include <util/tt_object.h>
#include <util/tt_list.h>

/* 
 * _Tt_int_rec is essentially just a wrapper around an integer so we can
 * have lists of integers (which isn't allowed in the implementation of
 * the list utility).
 */
class _Tt_int_rec : public _Tt_object {
      public:
	_Tt_int_rec();
	_Tt_int_rec(int val);
	virtual ~_Tt_int_rec();
	int			val;
	bool_t			xdr(XDR *xdrs);
	void			print(const _Tt_ostream &os) const;
};
declare_list_of(_Tt_int_rec)


class _Tt_pid_t_rec : public _Tt_object {
      public:
	_Tt_pid_t_rec();
	_Tt_pid_t_rec(pid_t val);
	virtual ~_Tt_pid_t_rec();
	pid_t			val;
	bool_t			xdr(XDR *xdrs);
	void			print(const _Tt_ostream &os) const;
};
declare_list_of(_Tt_pid_t_rec)

#endif				/*  TT_INT_REC_H */
