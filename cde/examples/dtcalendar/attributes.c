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
/* $XConsortium: attributes.c /main/3 1995/10/27 10:37:52 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company	
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 */

/*
 * attributes.c - retrieve calendar attributes
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>

#include <csa/csa.h>

void main(int argc, char **argv)
{
	CSA_session_handle cal;
	CSA_calendar_user user;
	CSA_uint32 num_names;
	CSA_attribute_reference *names;
	CSA_return_code	stat;
	int i;

	if (argc < 2) {
		printf("usage: %s user@host\n", argv[0]);
		return;
	}

	memset((void *)&user, NULL, sizeof(CSA_calendar_user));
	user.calendar_address = argv[1];

	if ((stat = csa_logon(NULL, &user, NULL, NULL, NULL, &cal, NULL))
	    != CSA_SUCCESS)
	{
		printf("Logon to %s failed, stat = %d\n", argv[1],
			stat);
		return;
	}

	if ((stat = csa_list_calendar_attributes(cal, &num_names, &names,
	    NULL)) == CSA_SUCCESS) {

		printf("List calendar attributes:\n");
		for (i = 0; i < num_names; i++)
			printf("%s\n", names[i]);

		csa_free(names);
	} else
		printf("csa_list_calendar_attributes failed, stat = %d\n",
			stat);

	(void)csa_logoff(cal, NULL);
}

