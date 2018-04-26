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
/*
 * File:	exception.h $XConsortium: exception.h /main/3 1995/10/26 16:12:07 rswiston $
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Novell, Inc.
 */
/* C++ exception handler package */

#ifndef __EXCEPTION_H_
#define __EXCEPTION_H_

#include <stddef.h>
#include <setjmp.h>


typedef const char *const Exception;

class _Exception
{
	char raised, accepted;
	_Exception *prev;
public:
	const char *val;
	jmp_buf buf;

	_Exception();
	~_Exception();
	int accept(Exception val);
	int recover();
	void raise(Exception val);
};
extern _Exception *_curr_exception;
extern void _raise_exception(Exception val);

#define TRY _Exception _new_exception; if (setjmp(_new_exception.buf) == 0)
#define RECOVER else if (_new_exception.recover())
#define HANDLE(val) else if (_new_exception.accept(val))
#define RAISE(val) _raise_exception(val)
#define EXCEPTION (_curr_exception != NULL ? _curr_exception->val : NULL)


#endif /* __EXCEPTION_H_ */
/*
@(#)REV: 2.18 90/12/13                                               
*/
