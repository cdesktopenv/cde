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
/*******************************************************************************
**
**  resource.c
**
**  $XConsortium: resource.c /main/3 1995/11/03 10:38:34 rswiston $
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
static  char sccsid[] = "@(#)resource.c 1.9 94/11/07 Copyr 1993 Sun Microsystems, Inc.";
#endif

#include <EUSCompat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include "resource.h"
#include "util.h"

static const char       COMMENT         = '#';
static const char       CONTINUATION    = '\\';

#define MAXPROPLEN	MAXNAMELEN * 6

/*
**  free_resources will step the list of resources and free the allocated space
*/
extern void
free_resources(Resource *r) {
	if (r) {
		free_resources(r->next);
		if (r->resource_name)
			free(r->resource_name);
		if (r->resource_value)
			free(r->resource_value);
		free(r);
	}
}

/*
**
**  get_resource will scan the list of resources for the resource
**  class.application.name (example:  deskset.calendar.start_time) passed and
**  return that resource's value if found, otherwise, it will return the
**  passed default.
**
*/
extern char*
get_resource(Resource *r, char *class_name, char *app_name, char *name,
	char *default_value)
{
	char	*buf, *ret;

	buf = (char *)ckalloc(cm_strlen(class_name) + cm_strlen(app_name)
		+ cm_strlen(name) + 3);
	sprintf(buf, "%s.%s.%s", class_name, app_name, name);
	ret = get_resource_by_val(r, buf, default_value);
	free(buf);
	return ret;
}

/*
**  Same as get_resource except it just takes the entire resource name instead
**  of the class.application.name triplet.
*/
extern char*
get_resource_by_val(Resource *r, char *name, char *default_value) {
	Resource	*step = r;

	while(step && strcasecmp(step->resource_name, name) != 0)
		step = step->next;
	if (!step)
		return default_value;
	return step->resource_value;
}

/*
**  Given a file name, read the list of resources in that file into the
**  passed Resource instance pointer.
*/
extern boolean_t
load_resources(Resource **r, char *file_name) {
	int		idx;
	char		*c_ptr, buffer[MAXPROPLEN], value[MAXPROPLEN];
	FILE		*rc_file;
	boolean_t	scan_complete, end_of_file = B_FALSE;
	Resource	*last = *r, *new_resource;

	if (!(rc_file = fopen(file_name, "r")))
                return B_FALSE;

	while(last && last->next)
		last = last->next;
	while (!end_of_file && fgets(buffer, MAXPROPLEN-1, rc_file)) {
		if (!(c_ptr = strchr(buffer, ':')))
			continue;

		/*
		**  Add the new property to the list
		*/
		*c_ptr = '\0';
		new_resource = (Resource *)ckalloc(sizeof(Resource));
		new_resource->resource_name = (char *)cm_strdup(buffer);
		new_resource->resource_value = NULL;
		new_resource->next = NULL;
		if (last)
			last->next = new_resource;
		else
		        *r = new_resource;
		last = new_resource;

		/*
		**  Initialize and read the property value
		*/
		idx = 0;
                scan_complete = B_FALSE;
		memset(value, '\0', MAXPROPLEN);
                c_ptr++;

		while (!scan_complete) {
			/*
			**  For the current buffer, skip the beginning white
			**  space
			*/
			while (*c_ptr != '\0' && *c_ptr != '\n'
				&& (*c_ptr == ' ' || *c_ptr == '\t'))
				c_ptr++;
			if (*c_ptr == '\0' || *c_ptr == '\n') {
				scan_complete = B_TRUE;
				continue;
			}

			/*
			**  We've found a real character, save characters until
			**  we find a comment, continuation, end of line, or we
			**  run out of space.
			*/
                        while (*c_ptr != '\0' && *c_ptr != '\n' &&
				(idx < MAXPROPLEN - 2) && *c_ptr != COMMENT &&
				*c_ptr != CONTINUATION)
				value[idx++] = *c_ptr++;
 
			/*
			**  Anything but continuation, cork value and bail
			*/
			if (*c_ptr != CONTINUATION) {
				value[idx] = '\0';
                                scan_complete = B_TRUE;
				continue;
			}

			/*
			**  A continued line.  Providing we can read another
			**  line from the file, reset the buffer, insert a
			**  space, and continue the loop ...
			*/
			if (!fgets(buffer, MAXPROPLEN, rc_file)) {
				value[idx] = '\0';
				end_of_file = scan_complete = B_TRUE;
				continue;
			}

			value[idx++] = ' ';
                        c_ptr = buffer;
		}
		new_resource->resource_value = (char *)cm_strdup(value);
        }
	fclose(rc_file);
        return B_TRUE;
}

/*
**  This function will write the resources to disk
*/
extern boolean_t
save_resources(Resource *r, char *file_name) {
	char		buf[MAXPROPLEN + 3];
	FILE		*rc_file;
	Resource	*step = r;

	if (!(rc_file = fopen(file_name, "w")))
                return B_FALSE;
	while(step) {
		sprintf(buf, "%s:\t%s\n",
			step->resource_name, step->resource_value);
		fputs(buf, rc_file);
		step = step->next;
	}
	fclose(rc_file);
	return B_TRUE;
}

/*
**  This set's the value for a specified resource
*/
extern boolean_t
set_resource(Resource **r, char *class_name, char *app_name, char *name,
	char *value)
{
	boolean_t	ret;
	char		*buf;

	buf = (char *)ckalloc(cm_strlen(class_name) + cm_strlen(app_name)
		+ cm_strlen(name) + 3);
	sprintf(buf, "%s.%s.%s", class_name, app_name, name);
	ret = set_resource_by_val(r, buf, value);
	free(buf);
	return ret;
}

/*
**  As as set_resource except the name passed in the full name
*/
extern boolean_t
set_resource_by_val(Resource **r, char *name, char *value) {
	Resource	*step, *last;

	last = step = *r;
	while(step && strcasecmp(step->resource_name, name) != 0)
		step = step->next;
	if (step)
		free(step->resource_value);
	else {
		step = (Resource *)ckalloc(sizeof(Resource));
		step->resource_name = (char *)cm_strdup(name);
		step->next = NULL;

		while(last && last->next)
			last = last->next;
		if (last)
			last->next = step;
		else
		        *r = step;
	}
	step->resource_value = (char *)cm_strdup(value);

	return B_TRUE;
}
