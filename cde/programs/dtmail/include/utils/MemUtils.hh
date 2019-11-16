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
/* $TOG: MemUtils.hh /main/5 1998/04/06 13:25:12 mgreess $ */
/*
 *+SNOTICE
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

#ifndef _MEMUTILS_HH
#define _MEMUTILS_HH

#include <string.h>

// The FORCE_SEGV_DECL macro is used to declare pointers on the stack. It
// will initialize the pointer in such a way as to force SEGV if it is
// used before set, and created detectable errors if it is freed before
// set. This macro should be used any time an uninitialized pointer would
// be created.
//
// Usage:
//	Instead of:
//		char * a_string;
//
//	Do:
//		FORCE_SEGV_DECL(char, a_string);
//
//	Note that the * is not specified on the type or variable name.
//
#ifdef DEBUG
#define FORCE_SEGV_CONST	(0x13)
#define FORCE_SEGV_DECL(a, b)	a *b = (a *)FORCE_SEGV_CONST
#else
#define FORCE_SEGV_CONST	(0)
#define FORCE_SEGV_DECL(a, b)	a *b = (a *)FORCE_SEGV_CONST
#endif

// The strdup_n function replaces the functionality of strdup(3C), but uses
// the C++ new operator. This is provided to allow consistent use of
// the delete operator when handling strings.
//
inline
char *
strdup_n(const char * old_str)
{
    char * new_str = (char *) NULL;
    
    if (old_str) new_str = new char[strlen(old_str) + 1];
    return(new_str ? strcpy(new_str, old_str) : (char*) NULL);
}

// The grow_region function provides the functionality of realloc(3C) for
// memory that has been allocated with new. This is provided to allow
// consistent use of the delete operator for memory regions that must
// be resized.
//
inline
void *
grow_region(void * old_region, const int old_size, const int new_size)
{
    void * new_region = new unsigned char[new_size];
    if (new_region) {
        memcpy(new_region, old_region, old_size);
    }
    delete (unsigned char*) old_region;
    return(new_region);
}

// The StackProbe class is used to debug stack over runs. It works by
// allocating two pages on the stack, and mprotecting one of the pages.
// The reason two pages are used is mprotect must protect entire pages.
// The top of the stack in any given routine will most likely not be
// on an even page boundary, so two pages are allocated to get one that
// can be protected.
//
#if defined(DEBUG) && defined(STACK_PROBE)

#ifndef STACK_PAGE_SIZE
#define STACK_PAGE_SIZE		4096
#endif

#include <sys/mman.h>

class StackProbe {
  public:
    StackProbe(void) {
	caddr_t page_start = &region[((int)&region % STACK_PAGE_SIZE)];
	mprotect(page_start, STACK_PAGE_SIZE, PROT_NONE);
    }

    ~StackProbe(void) {
	caddr_t page_start = &region[((int)&region % STACK_PAGE_SIZE)];
	mprotect(page_start, STACK_PAGE_SIZE, PROT_READ | PROT_WRITE);
    }

  private:
    char	region[STACK_PAGE_SIZE * 2];
};

#define INSERT_STACK_PROBE	StackProbe _stack_probe;
#endif

#ifndef INSERT_STACK_PROBE
#define INSERT_STACK_PROBE
#endif

#if defined(SENTINEL) && defined(DEBUG)

#include <stdio.h>
#include <sentinel.h>

inline
void
MdbgAddWatchPoint(const void * ptr, const size_t len)
{
    SeAddWatchPoint((void *)ptr, (size_t)len, "check");
}

inline
void
MdbgChainCheck(void)
{
    SeChainCheck(1);
}

inline
void
MdbgCheckData(const void * ptr, const int len)
{
    SeCheckData(SE_READ, (char *)ptr, (int)len, 1);
}

inline
void
MdbgCheckString(const char * str)
{
    SeCheckString((char *)str, 1);
}

inline
void
MdbgCheckWatchPoints(void)
{
    SeCheckWatchPoints();
}

inline
void
MdbgRemoveWatchPoint(const void * ptr)
{
    SeRemoveWatchPoint((void *)ptr);
}

#else /* Sentinel */

inline void MdbgAddWatchPoint(const void *, const size_t) {}
inline void MdbgChainCheck(void) {}
inline void MdbgCheckData(const void *, const int) {}
inline void MdbgCheckString(const char *) {}
inline void MdbgCheckWatchPoints(void) {}
inline void MdbgRemoveWatchPoint(const void *) {}

#endif /* Sentinel */

#endif
