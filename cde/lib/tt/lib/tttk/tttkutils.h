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
