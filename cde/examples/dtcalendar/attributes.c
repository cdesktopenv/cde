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

