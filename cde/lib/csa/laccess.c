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
/* $XConsortium: laccess.c /main/1 1996/04/21 19:23:33 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "laccess.h"
#include "lutil.h"

/******************************************************************************
 * forward declaration of static functions used within the file
 ******************************************************************************/
static void get_component(char **str, char *comp, char token);
static void get_last_component(char *head, char **tail, char *comp, char token);
static boolean_t match_forward(char *str1, char *str2);
static boolean_t match_backward(char *str1, char *str2);

/*****************************************************************************
 * extern functions used in the library
 *****************************************************************************/

/*
 * Correct format assumed, i.e. str = label1[.label2 ...]
 * Compare str2 against str1 which should be more fully qualified than str2
 */
extern boolean_t
_DtCmIsSamePath(char *str1, char *str2)
{
	char *ptr1,*ptr2;
	char *user;
	int res, n;

	if (str1 == NULL || str2 == NULL)
		return(B_FALSE);

	/* check format */
	if (*str1 == '.' || *str2 == '.')
		return (B_FALSE); /* bad format */

	if (match_forward(str1, str2) == B_TRUE)
		return (B_TRUE);
	else
		return (match_backward(str1, str2));
}

/*
 * compare user1 and user2
 * user1 = user@host[.domain]
 * user2 = any format in (user, user@host[.domain], user@domain)
 */
extern boolean_t
_DtCmIsSameUser(char *user1, char *user2)
{
	char *str1, *str2;
	char buf[BUFSIZ], *domain;
	boolean_t res;

	if (user1 == NULL || user2 == NULL)
		return (B_FALSE);

	/* compare user name */
	str1 = _DtCmGetPrefix(user1, '@');
	str2 = _DtCmGetPrefix(user2, '@');

	if (str1 == NULL || str2 == NULL)
		return (B_FALSE);

	if (strcmp(str1, str2)) {
		free(str1);
		free(str2);
		return (B_FALSE);
	}
	free(str1);
	free(str2);

	/* if only user name is specified, don't need to check domain */
	str2 = strchr(user2, '@');
	if (str2 == NULL)
		return (B_TRUE);
		
	domain = _DtCmGetLocalDomain(NULL);

	/* first assume user2=user@domain */
	str1 = strchr(user1, '.');
	if (str1 == NULL) {
		if (_DtCmIsSamePath(domain, ++str2)) {
			free(domain);
			return (B_TRUE);
		}
	} else {
		if (_DtCmIsSamePath(++str1, ++str2)) {
			return (B_TRUE);
		}
	}

	/* assume user2=user@host[.domain] */
	if (str1 == NULL) {
		str1 = strchr(user1, '@');
		snprintf(buf, sizeof buf, "%s.%s", ++str1, domain);
		str1 = buf;
	} else {
		str1 = strchr(user1, '@');
		str1++;
	}

	if (_DtCmIsSamePath(str1, str2))
		return (B_TRUE);
	else
		return (B_FALSE);
}

/*****************************************************************************
 * static functions used within the file
 *****************************************************************************/

/*
 * str consists of components separated by token
 * get and copy the first component into comp and
 * strip it out of str, so str would point to the first
 * token or the null terminator.
 */
static void
get_component(char **str, char *comp, char token)
{
	char *ptr;

	*comp = 0;

	if (str == NULL)
		return;
	else
		ptr = *str;

	while (ptr && *ptr != 0 && *ptr != token)
		*comp++ = *ptr++;

	*str = ptr;

	*comp = 0;
}

/*
 * head and tail points to the first and last character
 * of a string which consists of components separated by token.
 * get and copy the last component into comp and
 * strip it out of the string, so tail would point to the last
 * token or the head of the string.
 */
static void
get_last_component(char *head, char **tail, char *comp, char token)
{
	char *ptr, *cptr;

	*comp = 0;

	if (tail == NULL)
		return;
	else
		cptr = *tail;

	while (cptr != head && *cptr != token)
		cptr--;

	if (*cptr == token)
		ptr = cptr + 1;
	else
		ptr = cptr;

	while (ptr != (*tail + 1))
		*comp++ = *ptr++;

	*tail = cptr;

	*comp = 0;
}

static boolean_t
match_forward(char *str1, char *str2)
{
	char com1[BUFSIZ], com2[BUFSIZ];

	if (str1 == NULL || str2 == NULL)
		return (B_FALSE);

	while (B_TRUE) {
		get_component(&str1, com1, '.');
		get_component(&str2, com2, '.');

		if (*com1) {
			if (*com2 == NULL)
				return (B_TRUE);
		} else {
			if (*com2 == NULL)
				return (B_TRUE);
			else
				return (B_FALSE);
		}

		if (strcasecmp(com1, com2) != 0)
			return (B_FALSE);

		/* take care of case: a.b a. */
		if (strcmp(str2, ".") == 0
		    && (strcmp(str1, ".") != 0 || *str1 != NULL))
			return (B_FALSE);

		/* skip "." */
		if (*str1 == '.') {
			if (*str2 == NULL)
				return (B_TRUE);
			else {
				str1++;
				str2++;
			}
		} else if (strcmp(str2, ".") == 0 || *str2 == NULL)
			return (B_TRUE);
		else
			return (B_FALSE);
	}
}

static boolean_t
match_backward(char *str1, char *str2)
{
	int len1, len2;
	char *ptr1, *ptr2;
	char com1[BUFSIZ], com2[BUFSIZ];

	if (str1 == NULL || str2 == NULL)
		return (B_FALSE);

	len1 = strlen(str1);
	len2 = strlen(str2);
	if (len2 > len1)
		return (B_FALSE);
	else if (len2 == 0)
		return (B_TRUE);

	ptr1 = (len1 ? (str1 + len1 - 1) : str1);
	ptr2 = (len2 ? (str2 + len2 - 1) : str2);

	if (*ptr1 == '.' && ptr1 != str1)
		ptr1--;

	if (*ptr2 == '.' && ptr2 != str2)
		ptr2--;

	while (B_TRUE) {
		get_last_component(str1, &ptr1, com1, '.');
		get_last_component(str2, &ptr2, com2, '.');

		if (*com1) {
			if (*com2 == NULL)
				return (B_TRUE);
		} else {
			if (*com2 == NULL)
				return (B_TRUE);
			else
				return (B_FALSE);
		}

		if (strcasecmp(com1, com2) != 0)
			return (B_FALSE);

		/* skip "." */
		if (*ptr1 == '.') {
			if (ptr1 != str1)
				ptr1--;
			else
				return (B_FALSE); /* bad format */
		} else
			return (B_TRUE); /* done */

		if (*ptr2 == '.') {
			if (ptr2 != str2)
				ptr2--;
			else
				return (B_FALSE); /* bad format */
		} else
			return (B_TRUE); /* done */
	}
}


