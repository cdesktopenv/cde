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
 *	$TOG: util.c /main/6 1998/04/06 13:13:09 mgreess $
 *
 * @(#)util.c	1.21 16 Feb 1994	cde_app_builder/src/libAButil
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, Fujitsu, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */


/*
 * File:  util.c - general utilities
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <sys/param.h>
#include <sys/utsname.h>
#include <ab_private/AB.h>		/* include first! */
#include <ab_private/util.h>
#include <ab_private/istr.h>
#include "utilP.h"

static char *const	util_verbosity_env_var_name= "AB_DEBUG_VERBOSITY";
static char *const	default_program_name=	"AppBuilder";
static ISTRING		program_name= NULL;

const STRING Util_null_string= "(nil)";
const STRING Util_empty_string= "";

const int Util_major_version = 1;
const int Util_minor_version = 0;

int utilP_verbosity= 1;


/*
 * argv0 may be NULL
 */
int
util_init(int *argc_in_out, STRING **argv_in_out)
{
#define argc (*argc_in_out)
#define argv (*argv_in_out)
    STRING	argv0 = argv[0];
    char	*verboseVar= NULL;
    int		verboseVarValue= -1;
    int		verbosity= 0;

    verbosity = util_get_verbosity();

    /*
     * If this is a debugging build, look for the debugging level env. var
     */
#ifdef DEBUG
    verboseVar= getenv(util_verbosity_env_var_name);
    if (verboseVar != NULL)
    {
	verboseVarValue= atoi(verboseVar);
	verbosity= verboseVarValue;
    }
#endif /* DEBUG */

    /*
     * When debugging, keeps the output order consistent.  A little
     * slower, though.
     */
    if (verbosity >= 3)
    {
	/* there's really no good reason to unbuffer stdin */
	util_unbuffer_file(stdout);
	util_unbuffer_file(stderr);
    }

    if (argv0 != NULL)
    {
	util_set_program_name_from_argv0(argv0);
    }

    if (verbosity >= 3)
    {
	util_puts("Debugging output enabled, because verbosity > 2.\n");
    }
    util_set_verbosity(verbosity);
    verbosity= util_get_verbosity();

    return 0;
#undef argc
#undef argv
}


int
util_set_verbosity(int newVerbosity)
{
    int		oldVerbosity = utilP_verbosity;

    utilP_verbosity= newVerbosity;

    /*
     * If this is not a debugging build, do not allow verbosity to
     * go outside the range that is normal for an end-user.
     */
#ifndef DEBUG
    utilP_verbosity = util_max(0, utilP_verbosity);
    utilP_verbosity = util_min(2, utilP_verbosity);
#endif

    /*
     * Nifty message if we're debugging.
     */
#ifdef DEBUG
    if (  ((oldVerbosity >= 3) || (newVerbosity >= 3))
	&& (oldVerbosity != newVerbosity) )
    {
	int	newDebugLevel = debug_level();
	utilP_verbosity = 3;	/* make sure dprintf() prints something */
	util_dprintf(0, "Verbosity = %d", newVerbosity);
	if (newDebugLevel > 0)
	{
	    util_dprintf(0," [ Debugging level %d ]", newDebugLevel);
	}
	util_dprintf(0,"\n");
	utilP_verbosity = newVerbosity;
    }
#endif /* DEBUG */

    return 0;
}


int
util_set_program_name(STRING new_program_name)
{
    istr_destroy(program_name);
    if (new_program_name == NULL)
    {
	program_name= istr_const(default_program_name);
    }
    else
    {
	program_name= istr_create(new_program_name);
    }
    return 0;
}


int
util_set_program_name_from_argv0(STRING argv0)
{
    char	progName[MAXPATHLEN];
    util_get_file_name_from_path(argv0, progName, MAXPATHLEN);
    if (strlen(progName) > (size_t)0)
    {
	util_set_program_name(progName);
    }
    return 0;
}


STRING
util_get_program_name(void)
{
    return (program_name == NULL? 
		default_program_name
		:istr_string(program_name));
}


/*
 * Performs a lighter-weight fork that does not immediately copy
 * all the pages of the application. This is ideal for doing a
 * a fork and immediately execing another application.
 *
 * Not all UNIX implementations support a lightweight fork.  For
 * the ones that don't we'll do a normal fork.
 */
pid_t
util_vfork(void)
{
    return fork();
}


/*
 * putenv() is non-POSIX, so the parameter types can vary a bit...
 */
#ifdef __cplusplus
extern "C" {
#endif
#if defined(__hpux) || (defined(sun) && OSMAJORVERSION >= 5 && OSMINORVERSION >= 4 && OSMINORVERSION <=10 )
    extern int putenv(const char *string);
#elif !(defined(__aix) || defined(__NetBSD__))
    extern int putenv(char *string);
#endif
#ifdef __cplusplus
} // extern "C"
#endif
int
util_putenv(STRING string)
{
    return putenv(string);
}

/*
 * 
 */
AB_OS_TYPE
util_get_os_type(void)
{
    static AB_OS_TYPE		osType = AB_OS_UNDEF;
    static BOOL			osTypeDefined = FALSE;
    int				rc = 0;		/* return code */
    struct utsname 		sysInfo;
    int				aixIndex = -1;
    int				hpIndex = -1;
    int				sunIndex = -1;
    int                         lnxIndex = -1;
    int                         fbsdIndex = -1;
    int                         nbsdIndex = -1;
    int                         obsdIndex = -1;

    if (osTypeDefined)
    {
	goto epilogue;
    }
    if ((rc = uname(&sysInfo)) < 0)
    {
	goto epilogue;
    }

    aixIndex = util_strcasestr(sysInfo.sysname, "aix");
    hpIndex = util_strcasestr(sysInfo.sysname, "hp");
    sunIndex = util_strcasestr(sysInfo.sysname, "sun");
    lnxIndex = util_strcasestr(sysInfo.sysname, "linux");
    fbsdIndex = util_strcasestr(sysInfo.sysname, "freebsd");
    nbsdIndex = util_strcasestr(sysInfo.sysname, "netbsd");
    obsdIndex = util_strcasestr(sysInfo.sysname, "openbsd");

    if (aixIndex < 0) aixIndex = INT_MAX;
    if (hpIndex < 0) hpIndex = INT_MAX;
    if (sunIndex < 0) sunIndex = INT_MAX;
    if (lnxIndex < 0) lnxIndex = INT_MAX;
    if (fbsdIndex < 0) fbsdIndex = INT_MAX;
    if (nbsdIndex < 0) nbsdIndex = INT_MAX;
    if (obsdIndex < 0) obsdIndex = INT_MAX;

#define IS_MATCH(a,b,c,d,e,f,g)	\
  (((a) < (b)) && ((a) < (c)) && ((a) < (d)) && ((a) < (e)) && ((a) < (f)) && \
   ((a) < (g)))

    if (IS_MATCH(aixIndex, hpIndex, sunIndex, lnxIndex, fbsdIndex, nbsdIndex,
		 obsdIndex))
    {
	return AB_OS_AIX;
    }
    if (IS_MATCH(hpIndex, aixIndex, sunIndex, lnxIndex, fbsdIndex, nbsdIndex,
		 obsdIndex))
    {
	return AB_OS_HPUX;
    }
    if (IS_MATCH(sunIndex, aixIndex, hpIndex, lnxIndex, fbsdIndex, nbsdIndex,
		 obsdIndex))
    {
	return AB_OS_SUNOS;
    }
    if (IS_MATCH(lnxIndex, aixIndex, hpIndex, sunIndex, fbsdIndex, nbsdIndex,
		 obsdIndex))
    {
	return AB_OS_LNX;
    }
    if (IS_MATCH(fbsdIndex, aixIndex, hpIndex, sunIndex, lnxIndex, nbsdIndex,
		 obsdIndex))
    {
	return AB_OS_FBSD;
    }
    if (IS_MATCH(nbsdIndex, aixIndex, hpIndex, sunIndex, lnxIndex, fbsdIndex,
		 obsdIndex))
    {
	return AB_OS_NBSD;
    }
    if (IS_MATCH(obsdIndex, aixIndex, hpIndex, sunIndex, lnxIndex, fbsdIndex,
		 nbsdIndex))
    {
	return AB_OS_OBSD;
    }

    osTypeDefined = TRUE;

epilogue:
    return osType;
}


