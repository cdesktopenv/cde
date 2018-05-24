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
/* $XConsortium: garbage.c /main/6 1996/11/21 19:44:58 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include	<EUSCompat.h>
#include	<stdio.h>
#include	<fcntl.h>
#include	<unistd.h>
#include	<stdlib.h>
#define XOS_USE_NO_LOCKING
#define X_INCLUDE_TIME_H
#if defined(__linux__)
#undef SVR4
#endif
#include <X11/Xos_r.h>
#include	"log.h"
#include	"rtable4.h"
#include	"cm.h"
#include	"cmscalendar.h"
#include	"tree.h"
#include	"garbage.h"

extern char *pgname;

static CSA_return_code dump_error;
static int fd;

/*
 * forward declaration of functions used within this file
 */
static boolean_t magic_time(time_t t);
static boolean_t visit1(caddr_t node, caddr_t d);
static boolean_t visit2(caddr_t node, caddr_t d);
static void print_file_error(char *file, char *msg);

extern void
_DtCmsCollectOne(_DtCmsCalendar *cal)
{
	Rb_Status	status;
	CSA_return_code	 	stat;
	char *bak, *temp, *clog;

	if (cal == NULL)
		return;

	status	= rb_check_tree(APPT_TREE(cal));
	clog	= _DtCmsGetLogFN(cal->calendar);
	temp	= _DtCmsGetTmpFN(cal->calendar);
	bak	= _DtCmsGetBakFN(cal->calendar);

	if (status != rb_ok || clog==NULL || temp==NULL || bak==NULL) {
		fprintf(stderr, "%s: cannot acquire files to execute garbage collection.\n", pgname);
		fprintf(stderr, "possible causes: cannot find home directory.\n");
		fprintf(stderr, "\tNIS or your host server might be down.\n");
		fprintf(stderr, "damage: none\n\n");
		goto cleanup;
	}

	/* Make sure that the temp file does not exist before garbage collect */
	unlink (temp);
	if (cal->fversion == _DtCMS_VERSION1)
		stat = _DtCmsCreateLogV1(cal->owner, temp);
	else
		stat = _DtCmsCreateLogV2(cal->owner, temp);

	if (stat != CSA_SUCCESS) {
		if (stat == (CSA_X_DT_E_BACKING_STORE_PROBLEM))
			print_file_error(temp,
				"file error during garbage collection");
		else {
			fprintf(stderr, "%s: file error on %s during garbage collection\n",
				pgname, temp);
			fprintf(stderr, "Reason: getpwnam() failed. %s%s\n",
				"No passwd entry for owner of ",
				cal->calendar);

			fprintf(stderr, "damage: none\n\n");
		}
		goto cleanup;
	}

	if (cal->fversion == _DtCMS_VERSION1)
		stat = _DtCmsDumpDataV1(temp, cal);
	else
		stat = _DtCmsDumpDataV2(temp, cal);

	if (stat != CSA_SUCCESS) {
		print_file_error(temp,
			(stat == (CSA_X_DT_E_BACKING_STORE_PROBLEM) ?
			 "file error during garbage collection" :
			 "can't dump data structure to file during garbage collection"));
		goto cleanup;
	}

	/* mv -f .callog .calbak; mv -f temp .callog */
	if (rename (clog, bak) < 0) {
		perror ("rpc.cmsd: Can't backup callog to .calbak.\nreason:");
		goto cleanup;
	}

	if (rename (temp, clog) < 0) {
		perror("rpc.cmsd: Can't move .caltemp to callog.\nreason:");
		fprintf(stderr, "%s: you may recover %s from %s.\n", pgname,
			clog, bak);
	}

cleanup:
	if (bak != NULL) {
		free(bak);
		bak = NULL;
	}
	if (temp != NULL) {
		free(temp);
		temp = NULL;
	}
	if (clog != NULL) {
		free(clog);
		clog = NULL;
	}
}

extern CSA_return_code
_DtCmsDumpDataV1(char *file, _DtCmsCalendar *cal)
{
	CSA_return_code	stat;

	dump_error = CSA_SUCCESS;

	/* Keep the temp log file open during garbage collection. */
	if ((fd = open(file, O_WRONLY | O_APPEND | O_SYNC)) < 0)
	{
		return (CSA_X_DT_E_BACKING_STORE_PROBLEM);
	}

	if ((stat = _DtCmsAppendAccessByFD(fd, access_read_4,
	    GET_R_ACCESS(cal))) != CSA_SUCCESS)
		return (stat);

	if ((stat = _DtCmsAppendAccessByFD(fd, access_write_4,
	    GET_W_ACCESS(cal))) != CSA_SUCCESS)
		return (stat);

	if ((stat = _DtCmsAppendAccessByFD (fd, access_delete_4,
	    GET_D_ACCESS(cal))) != CSA_SUCCESS)
		return (stat);

	if ((stat = _DtCmsAppendAccessByFD (fd, access_exec_4,
	    GET_X_ACCESS(cal))) != CSA_SUCCESS)
		return (stat);

	_DtCmsEnumerateUp(cal, visit1);	/* dump the tree */

	if (close(fd) == EOF)
		return (CSA_X_DT_E_BACKING_STORE_PROBLEM);

	return (dump_error);
}

extern CSA_return_code
_DtCmsDumpDataV2(char *file, _DtCmsCalendar *cal)
{
	CSA_return_code	stat;

	dump_error = CSA_SUCCESS;

	/* Keep the temp log file open during garbage collection. */
	if ((fd = open(file, O_WRONLY | O_APPEND | O_SYNC)) < 0)
	{
		return (CSA_X_DT_E_BACKING_STORE_PROBLEM);
	}

	/* dump calendar attributes */
	if (cal->num_attrs > 0) {
		if ((stat = _DtCmsAppendCalAttrsByFD(fd, cal->num_attrs,
		    cal->attrs)) != CSA_SUCCESS)
			return (stat);
	}

	if ((stat = _DtCmsAppendHTableByFD(fd, cal->entry_tbl->size,
	    cal->entry_tbl->names, cal->types)) != CSA_SUCCESS)
		return (stat);

	_DtCmsEnumerateUp(cal, visit2);	/* dump the tree */

	if (close(fd) == EOF)
		return (CSA_X_DT_E_BACKING_STORE_PROBLEM);

	return (dump_error);
}

static boolean_t
magic_time(time_t t)
{
        boolean_t magic = B_FALSE;
        struct tm *tm;
	_Xltimeparams localtime_buf;

        tm = _XLocaltime(&t, localtime_buf);

        if (tm->tm_hour == 3 && tm->tm_min == 41)
                 magic = B_TRUE;

        return(magic);
}

static boolean_t
visit1(caddr_t node, caddr_t d)
{
	boolean_t stop = B_FALSE;

	switch (((Appt_4 *) d)->tag->tag)
	{
	case otherTag_4:
		/*
		 * otherTags = events read in from files.
		 * Don't write to log.
		 */
		return(stop);

	case appointment_4:
		/*
		 * a little hack to get us off the
		 * totally hokey magic-time business.
		 */
		if (magic_time(((Appt_4 *)d)->appt_id.tick)) {
			((Appt_4 *)d)->tag->showtime = 0;
		}
		break;
	}

	if ((dump_error = _DtCmsAppendAppt4ByFD(fd, (Appt_4 *)d, _DtCmsLogAdd)) != CSA_SUCCESS)
        	stop = B_TRUE;

	return(stop);
}

static boolean_t
visit2(caddr_t node, caddr_t d)
{
	boolean_t stop = B_FALSE;

	if ((dump_error = _DtCmsAppendEntryByFD(fd, (cms_entry *)d,
	    _DtCmsLogAdd)) != CSA_SUCCESS)
        	stop = B_TRUE;

	return(stop);
}

static void
print_file_error(char *file, char *msg)
{
	if (file)
		fprintf(stderr, "%s: (%s)%s\n", pgname, file, msg);
	else
		fprintf(stderr, "%s: %s\n", pgname, msg);
	fprintf(stderr, "possible causes: %s, %s, %s, %s\n",
		"host server is down", "disk is full", "out of memory",
		"file protections have changed.");
	fprintf(stderr, "damage: none\n\n");
}


