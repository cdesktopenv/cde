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
