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
/* $XConsortium: xfn_mapping.c /main/4 1996/05/09 04:28:00 drk $ */

/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident  "@(#)xfn_mapping.c 1.6     95/09/19 SMI"

#include <security/pam_appl.h>
#include <xfn/xfn.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

#include "xfn_mapping.h"

static char	hexval(char);
static int	hex2bin(int, char *, char *);
static int	bin2hex(int, unsigned char *, char *);

/*
 * attempt to get mapped password from XFN
 */

int
xfn_get_mapped_password(
	int flags,
	char *user,		/* xfn user */
	char *xfn_attr,		/* xfn attribute that holds the target pass */
	char *key,		/* key (password) encrypting target password */
	char *out,		/* un-encrypted target password */
	int out_max)		/* buffer size of out */
{

	FN_status_t		*status = NULL;
	FN_composite_name_t	*compname = NULL;
	FN_ctx_t		*ctx = NULL;
	FN_string_t		*str = NULL;
	FN_attribute_t		*attr = NULL;
	const FN_attrvalue_t	*attr_val = NULL;
	FN_identifier_t		attr_id;	/* attr identifier */
	FN_identifier_t		attr_syn;	/* attr syntax */
	char			hexpass[MAP_HEXLEN+1];
	char			pass[MAP_PASSLEN+1];
	char			composed_user[128]; /* XXX 128? */
	int			result = 0;
	void			*pos;

	pass[0] = '\0';

	status = fn_status_create();	/* free status */
	if (status == NULL)
		goto out;

	ctx = fn_ctx_handle_from_initial(status); /* free ctx */
	if (ctx == NULL)
		goto out;

	attr_id.format = FN_ID_STRING;
	attr_id.length = strlen(xfn_attr);
	attr_id.contents = xfn_attr;

	if (strlen(user) > (sizeof (composed_user)-6))
		goto out;

	sprintf(composed_user, "user/%s", user);

	str = fn_string_from_str(
			(unsigned char *)composed_user); /* free str */
	if (str == NULL)
		goto out;

	compname = fn_composite_name_from_string(str);	/* free compname */
	if (compname == NULL)
		goto out;

	attr = fn_attr_get(ctx, compname, &attr_id, status); /* free attr */
	if (attr == NULL)
		goto out;

	attr_val = fn_attribute_first(attr, &pos); /* don't have to free */
	if (attr_val == NULL)
		goto out;

	if (attr_val->length  != MAP_HEXLEN)
		goto out;

	memcpy(hexpass, attr_val->contents, attr_val->length);

	if (xdecrypt(hexpass, key) == 0)
		goto out;

	hexpass[MAP_HEXLEN] = '\0';

	hex2bin(MAP_PASSLEN, hexpass, pass); /* we have the password!! */
	memset(hexpass, 0, MAP_HEXLEN);

	if (strlen(pass)+1 > out_max)
		goto out;  /* sigh... */

	strcpy(out, pass);
	memset(pass, 0, strlen(pass));

	result = 1;

out:
	if (pass[0])
		memset(pass, 0, MAP_PASSLEN);
	if (status)
		fn_status_destroy(status);
	if (ctx)
		fn_ctx_handle_destroy(ctx);
	if (str)
		fn_string_destroy(str);
	if (compname)
		fn_composite_name_destroy(compname);
	if (attr)
		fn_attribute_destroy(attr);

	return (result);
}

/*
 * attempt to update mapped password from XFN
 */

int
xfn_update_mapped_password(
	int flags,		/* debug, etc */
	char *user,		/* xfn user */
	char *xfn_attr,		/* xfn attribute that holds the target pass */
	char *key,		/* key (password) encrypting target password */
	char *password)		/* un-encrypted target password */
{

	FN_status_t		*status = NULL;
	FN_composite_name_t	*compname = NULL;
	FN_ctx_t		*ctx = NULL;
	FN_ref_t		*ref = NULL;
	FN_string_t		*str = NULL;
	FN_attribute_t		*attr = NULL;
	FN_identifier_t		attr_id;	/* attr identifier */
	FN_identifier_t		attr_syn;	/* attr syntax */
	FN_attrvalue_t		attr_val;
	char			hexpass[MAP_HEXLEN+1];
	char			pass[MAP_PASSLEN+1];
	char			composed_user[128]; /* XXX 128? */
	int			result;

	result = 0;			/* guilty until proven innocent */
	hexpass[0] = '\0';

	if (strlen(pass) > MAP_PASSLEN)
		goto out;

	status = fn_status_create();	/* free status */
	if (status == NULL)
		goto out;

	ctx = fn_ctx_handle_from_initial(status); /* free ctx */
	if (ctx == NULL)
		goto out;

	attr_id.format = FN_ID_STRING;
	attr_id.length = strlen(xfn_attr);
	attr_id.contents = xfn_attr;
	attr_syn.format = FN_ID_STRING;
#define	PASSWD_ATTR_SYNTAX "fn_attr_syntax_ascii"
	attr_syn.length = sizeof (PASSWD_ATTR_SYNTAX)-1; /* sizeof has null */
	attr_syn.contents = PASSWD_ATTR_SYNTAX;

	attr = fn_attribute_create(&attr_id, &attr_syn); /* free attr */

	if (strlen(user) > (sizeof (composed_user)-6))
		goto out;

	sprintf(composed_user, "user/%s", user);
	str = fn_string_from_str(
			(unsigned char *)composed_user); /* free str */

	if (str == NULL)
		goto out;

	compname = fn_composite_name_from_string(str);	/* free compname */
	if (compname == NULL)
		goto out;

	ref = fn_ctx_lookup(ctx, compname, status);
	if (ref == NULL)
		goto out;

	strcpy(pass, password);
	bin2hex(MAP_PASSLEN, (unsigned char *)pass, hexpass);
	hexpass[MAP_HEXLEN] = '\0';

	memset(pass, 0, sizeof (pass));

	if (xencrypt(hexpass, key) == 0)
		goto out;

	attr_val.length = strlen(hexpass);
	attr_val.contents = hexpass;

	if (fn_attribute_add(attr, &attr_val, 1) == 0)
		goto out;

	if (fn_attr_modify(ctx, compname, FN_ATTR_OP_ADD, attr, status) == 0)
		goto out;

	result = 1;

out:
	if (hexpass[0])
		memset(hexpass, 0, MAP_HEXLEN);
	if (status)
		fn_status_destroy(status);
	if (ctx)
		fn_ctx_handle_destroy(ctx);
	if (str)
		fn_string_destroy(str);
	if (compname)
		fn_composite_name_destroy(compname);
	if (attr)
		fn_attribute_destroy(attr);

	return (result);
}

/*
 * Hex to binary conversion
 */
static
hex2bin(int len, char *hexnum, char *binnum)
{
	int i;
	char v1;
	char v2;

	for (i = 0; i < len; i++) {
		v1 = hexval(hexnum[2 * i]);
		if (v1 == -1)
			return (0);
		v2 = hexval(hexnum[2 * i + 1]);
		if (v2 == -1)
			return (0);
		*binnum++ = (v1 << 4) + v2;
	}
	return (1);
}

/*
 * Binary to hex conversion
 */
static
bin2hex(int len, unsigned char *binnum, char *hexnum)
{
	int i;
	unsigned val;
	static char hex[16] = {
		'0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
	};

	for (i = 0; i < len; i++) {
		val = binnum[i];
		hexnum[i*2] = hex[val >> 4];
		hexnum[i*2+1] = hex[val & 0xf];
	}
	hexnum[len*2] = 0;
	return (1);
}

static char
hexval(char c)
{
	if (c >= '0' && c <= '9') {
		return (c - '0');
	} else if (c >= 'a' && c <= 'f') {
		return (c - 'a' + 10);
	} else if (c >= 'A' && c <= 'F') {
		return (c - 'A' + 10);
	} else {
		return (-1);
	}
}
