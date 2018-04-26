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
 * $XConsortium: privbuf.C /main/4 1996/04/21 19:09:31 drk $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
#define __PRIVATE_
#include <codelibs/privbuf.h>

implement_array(privbuf_charbuf, char, 128)
implement_array(privbuf_strvec, char*, 128)

void privbuf_release(void **var)
{
    if (var == NULL || *var == NULL)
	return;
    char *buf = (char*)*var;
    privbuf_func *p = (privbuf_func*)(void*)buf;
    *var = NULL;
    (*p)((void*)buf);
}

void privbuf_freeprivbuf(void *buf)
{
    privbuf_buffer *b = (privbuf_buffer*)buf;
    //delete b->buf;
    //delete b->vec;
    delete b;
}

privbuf_buffer *privbuf_allocprivbuf()
{
    privbuf_buffer *b = new privbuf_buffer;
    b->func = privbuf_freeprivbuf;
    //b->buf = new privbuf_charbuf;
    //b->vec = new privbuf_strvec;
    return b;
}
