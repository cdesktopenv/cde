/*
 *+SNOTICE
 *
 *
 *	$XConsortium: BigMalloc.C /main/4 1996/04/21 19:47:03 drk $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#if defined(NEED_MMAP_WRAPPER)
extern "C" {
#endif

#include <sys/mman.h>

#if defined(NEED_MMAP_WRAPPER)
}
#endif

#include "BigMalloc.hh"

BigMalloc::BigMalloc(unsigned int size)
{
    _size = size;

#if defined(MMAP_NORESERVE)
    _fd = open("/dev/zero", O_RDONLY);

    _buffer = mmap(NULL, size * 4, (PROT_READ | PROT_WRITE),
		   MAP_PRIVATE | MAP_NORESERVE, _fd, 0);
#else
    _buffer = malloc(size);
#endif
}

BigMalloc::~BigMalloc(void)
{
#if defined(MAP_NORESERVE)
    if (_buffer) {
	munmap((char *)_buffer, _size * 4);
    }

    if (_fd >= 0) {
	close(_fd);
    }
#else
    free(_buffer);
#endif
}

void
BigMalloc::Realloc(unsigned int size)
{
#if defined(MAP_NORESERVE)
    if (size < (_size * 4)) {
	return;
    }

    unsigned int additional = (size * 4) - (_size * 4);

    char * new_buf;
    new_buf = mmap(((char *)_buffer) + (_size * 4), additional, 
		   (PROT_READ | PROT_WRITE),
		   MAP_FIXED | MAP_PRIVATE | MAP_NORESERVE, _fd, (_size * 4));

    if (new_buf == (char *)-1) {
	// Fixed mapping failed. We will have to map a new region and copy.
	//
    	new_buf = mmap(NULL, size * 4, (PROT_READ | PROT_WRITE),
		       MAP_PRIVATE | MAP_NORESERVE, _fd, 0);

	memcpy(new_buf, _buffer, _size * 4);
	munmap((char *)_buffer, _size * 4);
    }

    _buffer = new_buf;
    _size = size;
#else
    _size = size;
    _buffer = realloc(_buffer, _size);
#endif

}

BigMalloc::operator char*(void)
{
    return((char *)_buffer);
}
