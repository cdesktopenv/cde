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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * $TOG: privbuf.h /main/5 1999/10/15 17:23:22 mgreess $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
#ifndef __PRIVBUF_H_
#define __PRIVBUF_H_

#ifdef DOMAIN_ALLOW_MALLOC_OVERRIDE
#include "/usr/include/apollo/shlib.h"
#endif
#ifdef apollo
#include <stdlib.h>
#endif

typedef void (*privbuf_func)(void *v);

#ifdef __PRIVATE_
// this is only for internal library routines needing dynamic buffers:
#include <stddef.h>
#include <stdlib.h>

#if defined(__osf__) || defined(sun) || defined(linux) || defined(CSRG_BASED)
// The DEC C++ compiler rejects valid inline declarations, claiming
// they have both internal and external linkage.
#else
inline static void *operator new(size_t size) { return malloc((unsigned)size); }
inline static void operator delete(void *p) { if (p) free((char*)p); }
#endif

#include <codelibs/dynarray.h>
declare_array(privbuf_charbuf, char, 128)
declare_array(privbuf_strvec, char*, 128)
struct privbuf_buffer
{
    privbuf_func func;
    privbuf_charbuf buf;
    privbuf_strvec vec;
};
extern void privbuf_freeprivbuf(void *buf);
extern privbuf_buffer *privbuf_allocprivbuf();
#undef __PRIVATE_
#endif /* __PRIVATE_ */

#ifdef __cplusplus
extern "C" {
    void privbuf_release(void **var);
}
#else
extern void privbuf_release();
#endif

#endif /* __PRIVBUF_H_ */
