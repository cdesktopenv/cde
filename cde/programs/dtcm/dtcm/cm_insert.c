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
**  cm_insert.c
**
**  $TOG: cm_insert.c /main/8 1998/04/20 13:08:52 mgreess $
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
static  char sccsid[] = "@(#)cm_insert.c 1.31 95/05/19 Copyr 1993 Sun Microsystems, Inc.";
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
#include "util.h"
#include "getdate.h"
#include "timeops.h"
#include "props.h"
#include "cm_tty.h"
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
static nl_catd	DT_catd;
static char cm_target[256] = "";	/* target for table (user@host) */
static int cm_today = 0;		/* today's date (in epoch time) */
static char cm_date[256] = "";		/* appointment date */
static char cm_view[16] = "day";	/* view span (day,week,month) */
static char cm_start[16] = "";		/* start time for appointment list*/
static char cm_end[16] = "";		/* end time for appointment list */
static char cm_repeatstr[256] = "One Time";/* repeat period */
static char cm_for[256] = "";		/* number of repeats */
static char cm_what[1024] = "Appointment";
					/* what appointment is about */
static char cm_appt_file[1024] = "";
					/* file to find an appointment template in */
static int cm_repeat = 0;		/* how often to repeat this */
static int cm_flash = 0;		/* flash reminder */
static int cm_beep = 0;			/* beep reminder */
static int cm_open = 0;			/* open reminder */
static int cm_mail = 0;			/* mail reminder */

static void cm_args();			/* parse command line */

static char**
grab(char **argv,				/* command line arguments */
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
		case 's':
			argv = grab(++argv,cm_start,'-');
			break;
		case 'e':
			argv = grab(++argv,cm_end,'-');
			break;
		case 'w':
			argv = grab(++argv,cm_what,'-');
			break;
		case 'a':
			argv = grab(++argv,cm_appt_file,'-');
			break;
		default:
			fprintf(stderr, "%s", catgets(DT_catd, 1, 191, "Usage:\n\tdtcm_insert [ -c calendar ] [-d <mm/dd/yy>] [ -v view ]\n"));
			fprintf(stderr, "%s", catgets(DT_catd, 1, 192, "                   [-w what string] [-s <HH:MMam/pm>] [-e <HH:MMam/pm>]\n"));
			exit(1);
		}
	}
}

static void
prompt_for_line(char *prompt, char *defval, char *buffer)
{
	char input_buf[1024];

	printf(prompt, defval);

        *input_buf = '\0';
        fgets (input_buf, sizeof(input_buf), stdin);
        if (strlen(input_buf) && input_buf[strlen(input_buf)-1] == '\n')
          input_buf[strlen(input_buf)-1] = '\0';

	if (input_buf[0] && (input_buf[0] != '\n'))
		cm_strcpy(buffer, input_buf);
	else
		cm_strcpy(buffer, defval);
}

static void
prompt_for_insert(Props *p) {
	char		date_str[BUFSIZ], what_buffer[BUFSIZ], buf[BUFSIZ], *timecopy;
	int		index, next, valid = FALSE;
	DisplayType	dt = get_int_prop(p, CP_DEFAULTDISP);

        format_tick(now(), get_int_prop(p, CP_DATEORDERING),
		    get_int_prop(p, CP_DATESEPARATOR), date_str);

	printf("%s", catgets(DT_catd, 1, 193, "Please enter the information for the appointment you wish to add.\nDefaults will be shown in parentheses.\n"));
	prompt_for_line(catgets(DT_catd, 1, 194, 
		"Calendar (%s): "), cm_get_credentials(), cm_target);
	prompt_for_line(catgets(DT_catd, 1, 195, 
		"Date (%s): "), date_str, cm_date);
	while (valid != TRUE)
	{
		format_time(now(), dt, cm_start);
		prompt_for_line(catgets(DT_catd, 1, 196, 
			"Start (%s): "), cm_start, cm_start);
		if (cm_start && cm_start[0])
		{
			timecopy = (char *)cm_strdup(cm_start);
			if (valid_time(p, timecopy))
				valid = TRUE;
			else
				printf("%s", catgets(DT_catd, 1, 197, "You have entered an invalid time.  Please try again:\n"));
			free(timecopy);
		}
	}

	sprintf(buf, "%s %s", date_str, cm_start);
	next = (int) cm_getdate(buf, NULL);
        next = next + hrsec;

	format_time(next, dt, cm_end);
	if (cm_start && cm_start[0])
		prompt_for_line(
			catgets(DT_catd, 1, 198, "End (%s): "), cm_end, cm_end);
	else
		prompt_for_line(
			catgets(DT_catd, 1, 199, "End (%s): "), "None", cm_end);

	strcpy(cm_repeatstr, catgets(DT_catd, 1, 200, "One Time"));

	prompt_for_line(catgets(DT_catd, 1, 201, 
			"Repeat (%s): "), cm_repeatstr, cm_repeatstr);

	if (strcmp(cm_repeatstr, catgets(DT_catd, 1, 200, "One Time"))) {
		sprintf(buf, "%s", catgets(DT_catd, 1, 203, "no default"));
		prompt_for_line(
			catgets(DT_catd, 1, 204, "For (%s): "), buf, cm_for);
	}

	printf("%s", catgets(DT_catd, 1, 205, 
		"What (you may enter up to 5 lines, use ^D to finish):\n"));
	cm_what[0] = '\0';
	for (index = 0; index < 5; index++)
	{
        	*what_buffer = '\0';
        	fgets (what_buffer, sizeof(what_buffer), stdin);
        	if (strlen(what_buffer) &&
		    what_buffer[strlen(what_buffer)-1] == '\n')
          	  what_buffer[strlen(what_buffer)-1] = '\0';

		if (what_buffer[0] == '\000')
			break;
		else
		{
			strcat(cm_what, what_buffer);
			strcat(cm_what, "\\n");
		}
		memset(what_buffer, '\000', 256);
	}
	
}

void
main(int argc, char **argv)
{
	int		cnt, status = 0;
	char		*date = NULL, *view = NULL, *target = NULL,
			*start = NULL, *end = NULL, *repeat = NULL,
			*numrepeat = NULL, *what = NULL, *uname, *loc;
	Props		*p = NULL;
	CSA_entry_handle	*list;
	CSA_session_handle	c_handle;
	CSA_return_code		stat;
	CSA_calendar_user	csa_user;
	DisplayType		dt;
	int			version;
	char			date_str[256];
#ifdef FNS
	char		buf[256];
#endif

	init_time();
        _DtEnvControl(DT_ENV_SET); /* set up environment variables */
	setlocale(LC_ALL, "");
	DT_catd = catopen(DTCM_CAT, NL_CAT_LOCALE);
	cm_tty_load_props(&p);
	dt = get_int_prop(p, CP_DEFAULTDISP);
#ifdef FNS
	dtfns_init();
#endif

	if (argc > 1)
	{
		cm_args(argc,argv);		/* parse command line */
		if (cm_strlen(cm_target)) 
			target = cm_target;
		else
			target = cm_get_credentials();
#ifdef FNS
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
		  	char *format = cm_strdup(catgets(DT_catd, 1, 206, 
					   "\nCould not open calendar %s\n"));
			fprintf(stderr, format,
				target ? target : 
				catgets(DT_catd, 1, 209, "UNKNOWN"));
			free(format);
			free(uname);
			free(loc);
			exit(1);
		}
		version = get_data_version(c_handle);
		if (!cm_date[0])
        		format_tick(now(), get_int_prop(p, CP_DATEORDERING),
		    		    get_int_prop(p, CP_DATESEPARATOR), cm_date);
		if (cm_strlen(cm_date)) date = cm_date;
		if (cm_strlen(cm_view)) view = cm_view;
		if (cm_strlen(cm_start)) start = cm_start;

		if (!cm_end[0] && cm_start[0]) {
			format_time((int)cm_getdate(cm_start, NULL) + hrsec,
				    dt, cm_end);
		}

		if (cm_strlen(cm_end)) end = cm_end;
		if (cm_strlen(cm_repeatstr)) repeat = cm_repeatstr;
		if (cm_strlen(cm_for)) numrepeat = cm_for;
		if (cm_strlen(cm_what)) what = cm_what;
		if (!cm_appt_file[0])
			status = cm_tty_insert(DT_catd, c_handle, version, 
				      date, start, end, repeat, numrepeat,
				      what, NULL, p);
		else
			status = cm_tty_insert(DT_catd, c_handle, version, date,
				      start, end, repeat, numrepeat,
				      what, cm_appt_file, p);
	} else {
		prompt_for_insert(p);
		if (cm_strlen(cm_target)) target = cm_target;
		uname = cm_target2name(target);
		loc = cm_target2location(target);

		csa_user.user_name = target;
		csa_user.user_type = 0;
		csa_user.calendar_user_extensions = NULL;
		csa_user.calendar_address = target;
		stat = csa_logon(NULL, &csa_user, NULL, NULL, NULL, &c_handle, NULL);
		if (stat !=CSA_SUCCESS) {
		  	char *format = cm_strdup(catgets(DT_catd, 1, 206, 
					   "\nCould not open calendar %s\n"));
			fprintf(stderr, format, 
				target ? target : 
				catgets(DT_catd, 1, 209, "UNKNOWN"));
			free(format);
			free(uname);
			free(loc);
			exit(1);
		}
		version = get_data_version(c_handle);
		if (cm_strlen(cm_date)) date = cm_date;
		if (cm_strlen(cm_view)) view = cm_view;
		if (cm_strlen(cm_start)) start = cm_start;
		if (cm_strlen(cm_end)) end = cm_end;
		if (cm_strlen(cm_repeatstr)) repeat = cm_repeatstr;
		if (cm_strlen(cm_for)) numrepeat = cm_for;
		if (cm_strlen(cm_what)) what = cm_what;
		status = cm_tty_insert(DT_catd, c_handle, version, date, 
			      start, end, repeat, numrepeat, what, NULL, p);
	}
	if ((cnt = cm_tty_lookup(DT_catd, c_handle, version, date, view, 
					&list, p)) > 0)
		csa_free(list);
	csa_logoff(c_handle, NULL);
	props_clean_up(p);
	free(p);
	free(uname);
	free(loc);
        exit(status);
}
