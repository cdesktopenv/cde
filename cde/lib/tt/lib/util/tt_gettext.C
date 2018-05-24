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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $TOG: tt_gettext.C /main/4 1998/03/20 14:27:20 mgreess $ 			 				
/*
 *
 * tt_gettext.cc
 *
 * Copyright (c) 1988, 1990 by Sun Microsystems, Inc.
 *
 */

#include <stdlib.h>
#include "util/tt_string.h"
#include "tt_options.h"

#if defined(OPT_DGETTEXT)
# if defined(OPT_BUG_SUNOS_4)
    extern "C" {
        char *bindtextdomain(const char *, const char *);
        char *dgettext(const char *, const char *);
    }
# else
#   include <libintl.h>
# endif

#elif defined(OPT_CATGETS)
# if defined(__linux__)
    extern "C" {
# endif
# include <nl_types.h>
# if defined(__linux__)
    }
# endif
# if !defined(NL_CAT_LOCALE)
#    define NL_CAT_LOCALE 0
# endif
#endif

static const char tt_err_domain[] = "SUNW_TOOLTALK";

#if defined(OPT_DGETTEXT)
/*
 * This routine wraps around the code to set the domain and call
 * dgettext, so we don't have to replicate this logic all over
 * the place.
 */
char *
_tt_gettext(
	const char *msgid
)
{
	static int bindtextdomain_done = 0;

	if (!bindtextdomain_done) {
		char *envvar;
		_Tt_string path;

		// For development purposes, find
		// the catalogs under $TOOLTALKHOME/locale.
		// When running normally, as part of Open Windows,
		// find the catalogs under $OPENWINHOME/lib/locale.
		// Otherwise try /usr/openwin/lib/locale, the standard
		// location, as a last resort.

		if(envvar = getenv("TOOLTALKHOME")) {
			path = envvar;
			path = path.cat("/locale");
		} else if (envvar = getenv("OPENWINHOME")) {
			path = envvar;
			path = path.cat("/lib/locale");
		} else {
			path = "/usr/openwin/lib/locale";
		}
		bindtextdomain(tt_err_domain, path);
		bindtextdomain_done = 1;
	}
	return dgettext(tt_err_domain,msgid);
}
#endif // OPT_DGETTEXT

#if defined(OPT_CATGETS)
static char *
_tt__catgets(
	int		set_num,
	int		msg_num,
	const char     *default_string
)
{
	static nl_catd catalog = 0;
	if (catalog == 0) {
		catalog = catopen(
#if defined(OPT_BUG_AIX)
			(char *)
#endif
				  tt_err_domain, NL_CAT_LOCALE );
	}
	return catgets( catalog, set_num, msg_num,
#if defined(OPT_BUG_SUNOS_5) || defined(OPT_BUG_AIX)
				(char *)
#endif
			default_string );
}
#endif // OPT_CATGETS

char *
_tt_catgets(
	int		set_num,
	int		msg_num,
	const char     *default_string
)
{
#if defined(OPT_DGETTEXT)
	return _tt_gettext( default_string );
#elif defined(OPT_CATGETS)
	return _tt__catgets( set_num, msg_num, default_string );
#else
	return default_string;
#endif
}
