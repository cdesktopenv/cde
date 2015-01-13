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
/*******************************************************************************
**
**  cm_lookup.c
**
**  $TOG: cm_lookup.c /main/8 1998/04/21 10:21:26 mgreess $
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
**  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/

/*									*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#ifndef lint
static  char sccsid[] = "@(#)cm_lookup.c 1.30 95/03/08 Copyr 1993 Sun Microsystems, Inc.";
#endif

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <rpc/rpc.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <nl_types.h>
#include <locale.h>
#include "cm_tty.h"
#include "util.h"
#include "timeops.h"
#ifdef FNS
#include "dtfns.h"
#include "cmfns.h"
#endif
#include <Dt/Dt.h>
#include <Dt/EnvControlP.h>

#if !defined(NL_CAT_LOCALE)
#define NL_CAT_LOCALE       0
#endif

int debug = 0;
static char cm_target[256] = "";	/* target for table (user@host) */
static char cm_date[256] = "";		/* appointment date */
static char cm_view[16] = "";		/* view span (day,week,month) */
static nl_catd	DT_catd;

static char**
grab(char**argv,				/* command line arguments */
    char *buf,				/* buffer for keyed data */
    char stop_key)
{
	if (!argv || !*argv) return(argv);
	cm_strcpy (buf,*argv++);
	while(argv && *argv) {
		if (*(*argv) == stop_key) break;
		cm_strcat(buf," ");
		cm_strcat(buf,*argv++);
	}
	argv--;
	return(argv);
}

static void
cm_args(int argc, char **argv)
{

	while (++argv && *argv) {
		switch(*(*argv+1)) {
		case 't':
		case 'c':
			argv = grab(++argv,cm_target,'-');
			break;
		case 'd':
			argv = grab(++argv,cm_date,'-');
			break;
		case 'v':
			argv = grab(++argv,cm_view,'-');
			break;
		default:
			fprintf(stderr, "%s", catgets(DT_catd, 1, 207, "Usage:\n\tdtcm_lookup [ -c calendar ][ -d <mm/dd/yy> ] [ -v view ]\n"));
			exit(1);
		}
	}
}

int main(int argc, char **argv)
{
	int		cnt;
	char		*target = NULL, *date = NULL, *view = NULL,
			*uname, *loc;
	Props		*p = NULL;
	CSA_session_handle	c_handle;
	CSA_entry_handle	*list;
	CSA_return_code		stat;
	CSA_calendar_user	csa_user;
	int			version;
#ifdef FNS
	char		buf[256];
#endif

	init_time();
	_DtEnvControl(DT_ENV_SET); /* set up environment variables */
	setlocale(LC_ALL, "");
	DT_catd = catopen(DTCM_CAT, NL_CAT_LOCALE);
	cm_tty_load_props(&p);
	cm_args(argc,argv);		/* parse command line */

	target = (cm_strlen(cm_target)) ? cm_target : cm_get_credentials();
#ifdef FNS
	dtfns_init();
	if (cmfns_use_fns(p)) {
		cmfns_lookup_calendar(target, buf, sizeof(buf));
		target = buf;
	}
#endif
	uname = cm_target2name(target);
	loc = cm_target2location(target);

	csa_user.user_name = target;
	csa_user.user_type = 0;
	csa_user.calendar_user_extensions = NULL;
	csa_user.calendar_address = target;
	stat = csa_logon(NULL, &csa_user, NULL, NULL, NULL, &c_handle, NULL);

	if (stat != CSA_SUCCESS) {
		char *format = cm_strdup(catgets(DT_catd, 1, 208, 
				   "\nCould not open calendar \"%s\"\n"));
		fprintf(stderr, format, 
			target ? target : catgets(DT_catd, 1, 209, "UNKNOWN"));
		free(format);
		free(uname);
		free(loc);
		exit(1);
	}
	version = get_data_version(c_handle);
	if (cm_strlen(cm_date)) date = cm_date;
	if (cm_strlen(cm_view)) view = cm_view;

	if ((cnt = cm_tty_lookup(DT_catd, c_handle, version, date, view, 
				 &list, p)) > 0)
		csa_free(list);
	csa_logoff(c_handle, NULL);
	props_clean_up(p);
	free(p);
	free(uname);
	free(loc);
	return 0;
}
