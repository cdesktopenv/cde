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
/*%%  $XConsortium: tttkutils.h /main/3 1995/10/23 10:34:07 rswiston $ 			 				 */
/*
 * @(#)tttkutils.h	1.3 93/09/07
 *
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */
#ifndef ttdtutils_h
#define ttdtutils_h

extern char		_TttkKeys[];

#define                 _TttkCBKey              ((int)(long)&_TttkKeys[0])
#define                 _TttkClientCBKey        ((int)(long)&_TttkKeys[1])
#define                 _TttkClientDataKey      ((int)(long)&_TttkKeys[2])
#define                 _TttkDepositPatKey      ((int)(long)&_TttkKeys[3])
#define                 _TttkJoinInfoKey        ((int)(long)&_TttkKeys[4])
#define                 _TttkContractKey        ((int)(long)&_TttkKeys[5])
#define                 _TttkSubContractKey     ((int)(long)&_TttkKeys[6])

const int		_TttkNumKeys		= 7;

void			_ttDtPrint(
				const char     *whence,
				const char     *msg
			);
void			_ttDtPrintStatus(
				const char     *whence,
				const char     *expr,
				Tt_status	err
			);
void			_ttDtPrintInt(
				const char     *whence,
				const char     *msg,
				int		n
			);
void			_ttDtPError(
				const char     *whence,
				const char     *msg
			);
const char	       *_ttDtOp(
				Tttk_op		op
			);
Tt_callback_action	_ttDtCallbackAction(
				Tt_message msg
			);

#endif
