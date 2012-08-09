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
 * File:	filegen.C $TOG: filegen.C /main/7 1999/10/14 15:05:25 mgreess $
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Novell, Inc.
 */
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#define X_INCLUDE_DIRENT_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>
#include <codelibs/stringx.h>
#include <codelibs/nl_hack.h>
#include "buf.h"
#include "DtSvcLock.h"

#ifndef MAXPATHLEN
# define MAXPATHLEN 1024
#endif

#ifdef XTHREADS
extern "C" {
    extern void XtProcessLock(void);
    extern void XtProcessUnlock(void);
}
#endif

struct _SHXcomponent
{
    boolean is_pattern;
    long offset;		// subscript in path buffer
    char *ptr;                  // pointer into path buffer
};

declare_array(_SHXcomponents, _SHXcomponent, 4)

// recursive routine to expand the wildcard path represented in stack into
// all possible expansions.  The expansions are appended to _SHXbuf::vec.
// filebuf is a scratch buffer passed in by the caller and is used to build
// intermediate paths.  end is a pointer to the position in filebuf where
// the calling routine left off.
void
_SHXbuf::expand(_SHXcomponents &stack,
	char *const filebuf, char *end, int compnum)
{
    *end = '\0';

    if (compnum == stack.size())
	return;

    _SHXcomponent &comp = stack[compnum];

    // double-slash?
    if (comp.ptr[0] == '\0')
    {
	if (compnum + 1 == stack.size())
	    append(filebuf, EXPANDQUOTE);
	else
	{
	    *end++ = '/';
	    expand(stack, filebuf, end, compnum + 1);
	}
	return;
    }

    // performance optimization:  if this path component
    // doesn't contain a wildcard, avoid doing an opendir()
    if (!comp.is_pattern)
    {
	strcpy(end, comp.ptr);
	if (compnum + 1 == stack.size())
	{
	    // last component, just see if the path really points to something
	    if (access(filebuf, F_OK) != -1)
		append(filebuf, EXPANDQUOTE);
	}
	else
	{
	    // intermediate directory just append this component and keep going
	    char *end2 = strend(end);
	    *end2++ = '/';
	    expand(stack, filebuf, end2, compnum + 1);
	}
	return;
    }

    // We have a wildcard component, open and scan its parent directory
    // and look for matches.
    DIR *dir = opendir(filebuf[0] == '\0' ? "." : filebuf);
    if (dir == NULL)
	return;

    _Xreaddirparams dir_buf;
    struct dirent *ent;

    memset((char*) &dir_buf, 0, sizeof(_Xreaddirparams));
    while ((ent = _XReaddir(dir, dir_buf)) != NULL)
    {
	// deleted file?
	if (ent->d_ino == 0 || ent->d_name[0] == '\0')
	    continue;

	// right name?
	if (comp.is_pattern)
	{
	    wchar_t __nlh_char[1];

	    // Must have explicit match for leading '.'
	    if (CHARAT(ent->d_name) == '.' && CHARAT(comp.ptr) != '.')
		continue;

	    if (strwcmp(comp.ptr, ent->d_name) != 0)
		continue;
	}
	else if (strcmp(comp.ptr, ent->d_name) != 0)
	    continue;

	strcpy(end, ent->d_name);
	if (compnum + 1 == stack.size())
	    append(filebuf, EXPANDQUOTE);
	else
	{
	    char *end2 = end + strlen (ent->d_name);
	    *end2++ = '/';
	    expand(stack, filebuf, end2, compnum + 1);
	}
    }

    closedir(dir);
}


//extern "C" { void qsort(void *, unsigned, int, ...); };

static char *bufptr;

static int
//compar(int &v1, int &v2)
compar(const void *v1, const void *v2)
{
    int result;

    _DtSvcProcessLock();
    result = strcmp(&bufptr[*(int*)v1], &bufptr[*(int*)v2]);
    _DtSvcProcessUnlock();
    return (result);
}

void
_SHXbuf::filegen()
{
    privbuf_charbuf path;
    _SHXcomponents stack;

    long vecstart = vec.size() - 1;
    if (vecstart < 0)
	return;

    long bufstart = long(vec[vecstart]);

    // Parse the file path, breaking it up into individual components.
    // Each component is marked as being either a wildcard component
    // or not.  The wildcard components will have a '\' placed before
    // any quoted wildcard characters.  The non-wildcard components
    // will be left unchanged.
    int bufpos = (int) bufstart;
    while (bufpos < buf.size())
    {
	_SHXcomponent & comp = stack[stack.size()];
	comp.is_pattern = FALSE;
	comp.offset = path.size();
	comp.ptr = NULL;
	int startpos = bufpos;

	int ch;
	do
	{
	    ch = buf[bufpos];
	    switch (ch)
	    {
	    case '/': 
		ch = '\0';
		break;
	    case '*': 
	    case '?': 
	    case '[': 
		if (flags[bufpos] == NOQUOTE)
		    comp.is_pattern = TRUE;
		else
		    path[path.size()] = '\\';
		break;
	    }

	    path[path.size()] = ch;
	    bufpos++;
	} while (ch != '\0');

	// Add a '*' to the end of the last component if needed
	// for completion.
	if (bufpos >= buf.size())   // last component?
	    if (bufpos > bufstart + 1)	// non-null string?
		if (completion && !is_pattern)
		{
		    path[path.size() - 1] = '*';
		    path[path.size()] = '\0';
		    comp.is_pattern = TRUE;
		    break;
		}

	// If it wasn't a pattern, remove all of the '\' characters
	// that were added.
	if (!comp.is_pattern)
	{
	    int len = bufpos - startpos - 1;
	    strncpy(&path[comp.offset], &buf[startpos], len);
	    path[comp.offset + len] = '\0';
	}
    }

    // Fill in the character pointer values for all of the components.
    // We couldn't do this in the first pass because path is a
    // dynamic array.
    char *pathbuf = path.getarr();
    for (int i = 0; i < stack.size(); i++)
	stack[i].ptr = &pathbuf[stack[i].offset];

    // Remove the token that we just copied from the return vector
    // so that we can replace it with its expansion.
    vec.reset(vecstart);

    char filebuf[MAXPATHLEN];
    expand(stack, filebuf, filebuf, 0);

    if (vec.size() == vecstart)	// no matches?
    {
	vec[vecstart] = (char *)bufstart;   // restore orig. token
	return;
    }

    // alphabetize the expansion to make it look pretty like ksh does.
    _DtSvcProcessLock();
    bufptr = buf.getarr();
    qsort(&vec[vecstart], (unsigned int)(vec.size() - vecstart),
	    sizeof (char *), compar);

    // Find the longest match if we are doing completion:
    if (completion)
    {
	long i;
	// compare all entries to a copy of the first entry
	strcpy(filebuf, &bufptr[long(vec[0])]);

	for (i = 1; i < vec.size(); i++)
	{
	    register char *ref = filebuf;
	    register char *ptr = &bufptr[long(vec[i])];
	    while (*ref == *ptr && *ref != '\0' && *ptr != '\0')
		ref++, ptr++;

	    *ref = '\0';	// shorten the reference copy
	}

	// Now store the best match as the first token.  We will
	// have to shift the expansion vector down by one to
	// make room.
	for (i = vec.size(); i > 0; --i)
	{
	    register char *val = vec[i - 1];
	    vec[i] = val;
	}
	vec[0] = (char *)buf.size();
	append(filebuf, EXPANDQUOTE);
	vec.reset(vec.size() - 1);  // adjust for the append
    }
    _DtSvcProcessUnlock();
}

implement_array(_SHXcomponents, _SHXcomponent, 4)
