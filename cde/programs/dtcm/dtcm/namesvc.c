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
**  namesvc.c
**
**  $XConsortium: namesvc.c /main/3 1995/11/03 10:30:13 rswiston $
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
static  char sccsid[] = "@(#)namesvc.c 1.5 94/11/07 Copyr 1993 Sun Microsystems, Inc.";
#endif

#if 0

#include <rpcsvc/nis.h>
#include <rpcsvc/ypclnt.h>
#include <rpcsvc/yp_prot.h>
#include "util.h"
#include "namesvc.h"
#include "gettext.h"

extern int debug;

#define MAPNAME "mail.aliases"

static char *
get_userscontext()
{
	static char *userscontext;

	if (userscontext == NULL) {
		if ((userscontext = cm_strdup(
			(char *)getenv("USERSCONTEXT"))) == NULL)
			userscontext = USERSCONTEXT;
	}
	return(userscontext);
}


static int
get_principal(table, ent, udata)
char *table;
nis_object *ent;
void *udata;
{
	char **principal = (char **)udata;
	entry_col *ec = ent->EN_data.en_cols.en_cols_val;
	int ncol = ent->EN_data.en_cols.en_cols_len;

	if (ec[PRINCIPALCOLNUM].ec_value.ec_value_len)
		*principal = cm_strdup(ec[PRINCIPALCOLNUM].ec_value.ec_value_val);
	else	*principal = NULL;
	return(1);
}

extern Lookup_stat
cm_get_nis_location(name, calname, host)
char *name;
char *calname;
char **host;
{
	nis_result *tres, *eres;
	char cmtbl[NIS_MAXNAMELEN];
	char indexname[NIS_MAXNAMELEN];
	char *user, *domain;
	Lookup_stat res;
	u_long tflag = EXPAND_NAME;
	entry_col *ec;
	int ncol;
	nis_object *nis_obj;

	*host = NULL;
	user = get_head(name, '@');
	domain = get_tail(name, '@');
	sprintf(cmtbl, "%s.%s.%s", USERMAP, user, get_userscontext());
	if (domain) {
		strcat(cmtbl, ".");
		strcat(cmtbl, domain);
	}

	tres = nis_lookup(cmtbl, tflag);
	if (tres->status != NIS_SUCCESS) {
		nis_freeresult(tres);
		return(l_no_table);
	}

	sprintf(indexname, "[%s=%s],%s.", NAMECOLNAME, calname,
		tres->objects.objects_val[0].zo_name);
	if (*(tres->objects.objects_val[0].zo_domain) != '.')
		strcat(indexname, tres->objects.objects_val[0].zo_domain);

	eres = nis_list(indexname, tflag, 0, 0);

	if (eres->status == NIS_SUCCESS || eres->status == NIS_CBRESULTS) {
		nis_obj = NIS_RES_OBJECT(eres);
		if (nis_obj == NULL)  
			res = l_nis_error;
		else {
			ec = nis_obj->EN_data.en_cols.en_cols_val;
			ncol = nis_obj->EN_data.en_cols.en_cols_len;
			if (ncol >= NUMTBLCOLS && ec != NULL &&
				ec[LOCATIONCOLNUM].ec_value.ec_value_len) {
				*host = cm_strdup(
					ec[LOCATIONCOLNUM].ec_value.ec_value_val);
				res = l_ok;
			}
			else {	
				*host = NULL;
				res = l_nis_error;
			}
		}
	}
	else {
		if (debug)
			nis_perror(tres->status, "nis_list");
		res = l_nis_error;
	}
	nis_freeresult(tres);
	nis_freeresult(eres);
	return(res);
}

extern char *
netname2principal(netname)
char *netname;
{
	nis_result *eres;
	char indexname[NIS_MAXNAMELEN];
	char *domain;
	u_long tflag = EXPAND_NAME;
	char *principal = NULL;

	domain = get_tail(netname, '@');
	if (domain == NULL)
		return(NULL);

	sprintf(indexname, "[%s=%s],%s.%s", AUTHNAMECOLNAME, netname,
			CREDDIR, domain);
	eres = nis_list(indexname, tflag, get_principal, (void *)&principal);
	if (eres->status != NIS_SUCCESS && eres->status != NIS_CBRESULTS)
		if (debug)
			nis_perror(eres->status, "nis_list");
	nis_freeresult(eres);
	return(principal);
}

Lookup_stat
test_cm_get_target(name, name2)
	char *name, **name2;
{
	FILE *f;
	char    *b, *tmp, *uname, *ptr, buffer[BUFSIZ];
	char *tail;
	Boolean domain = False;

	*name2 = NULL;

	f = fopen("/home2/curry/myds/src/cm2/testfile", "r");
	tail = get_tail(name, '@');
	if (tail != NULL && strcasecmp(tail, "eng") == 0) {
		domain = True;
		free(tail);
	}
	uname = cm_target2name(name);
	while (fgets(buffer, BUFSIZ, f)) {
		b = (char*)cm_strdup(buffer);
		tmp = (char*)strtok(b, " ");
		if (strcmp(uname, tmp) == 0) {
			ptr = (char*)strrchr(buffer, ' ');
			ptr++;
			ptr[strlen(ptr)-1] = 0;
			*name2 = ckalloc(cm_strlen(name) + cm_strlen(ptr) + 7);
			if (domain) 
				sprintf(*name2, "%s@%s.eng", uname, ptr);	
			else
				sprintf(*name2, "%s@%s", uname, ptr);
			free(uname); free(b);
			fclose(f);
			return l_ok; 
		}
		free(b);
	}
	fclose(f);
	free(uname);
	return l_other;
}

/* principal.domain -> principal */
extern char *
principal_to_user(principal)
char *principal;
{
	return(get_head(principal, '.'));
}

/* principal.domain -> domain */
extern char *
principal_to_domain(principal)
char *principal;
{
	return(get_tail(principal, '.'));
}
static void
new_entryobj(nobj, loc)
	nis_object *nobj;
	char 	*loc;
{
	entry_col *ec;
	entry_obj *eo;

	nobj->zo_data.zo_type = ENTRY_OBJ;
	eo = &(nobj->EN_data);
	eo->en_cols.en_cols_len = NUMTBLCOLS;
	eo->en_type = "cm_location_tbl";
	ec = (entry_col*)ckalloc(sizeof(entry_col) * NUMTBLCOLS);
	eo->en_cols.en_cols_val = ec;
        ec[NAMECOLNUM].ec_value.ec_value_len = cm_strlen(DEFAULT_CALNAME)+1; 
        ec[NAMECOLNUM].ec_value.ec_value_val = DEFAULT_CALNAME;
        ec[NAMECOLNUM].ec_flags = 0;
	ec[LOCATIONCOLNUM].ec_value.ec_value_len = cm_strlen(loc)+1;
        ec[LOCATIONCOLNUM].ec_value.ec_value_val = cm_strdup(loc); 
        ec[LOCATIONCOLNUM].ec_flags = 0;
}
static nis_object*
new_nisobj()
{
	nis_object *nobj;

	nobj = (nis_object*)ckalloc(sizeof(nis_object));
	nobj->zo_owner = nis_local_principal();
	nobj->zo_group = nis_local_group();
	nobj->zo_access = DEFAULT_RIGHTS; 
	nobj->zo_ttl = 43200;
	
	return nobj;
}

extern Lookup_stat
cm_set_nis_location(name, calname, loc)
char *name;
char *calname;
char *loc;
{
	nis_result *eres, *mres, *ares;
	char cmtbl[NIS_MAXNAMELEN];
	char indexname[NIS_MAXNAMELEN];
	char *user, *domain;
	Lookup_stat res = l_ok;
	u_long tflag = EXPAND_NAME;
	nis_object *new_obj, *nis_obj;
	entry_col *ec;
	int ncol;

	user = get_head(name, '@');
	domain = get_tail(name, '@');
	sprintf(cmtbl, "%s.%s.%s", USERMAP, user, get_userscontext());
	if (domain) {
		strcat(cmtbl, ".");
		strcat(cmtbl, domain);
	}
	else {
		strcat(cmtbl, ".");
		strcat(cmtbl, cm_get_local_domain());
	}

	eres = nis_lookup(cmtbl, tflag);
	if (eres->status != NIS_SUCCESS) {
		nis_freeresult(eres);
		return(l_no_table);
	}

	sprintf(indexname, "[%s=%s],%s.", NAMECOLNAME, calname,
		eres->objects.objects_val[0].zo_name);
	if (*(eres->objects.objects_val[0].zo_domain) != '.')
		strcat(indexname, eres->objects.objects_val[0].zo_domain);
	nis_freeresult(eres);

	eres = nis_list(indexname, tflag, 0, 0);

	if (eres->status == NIS_SUCCESS || eres->status == NIS_CBRESULTS) {
		nis_obj = NIS_RES_OBJECT(eres);
		if (nis_obj == NULL) 
			res = l_nis_error;
		else {
			ncol = nis_obj->EN_data.en_cols.en_cols_len;
			ec = nis_obj->EN_data.en_cols.en_cols_val;
			if (ncol >= NUMTBLCOLS && ec != NULL &&
				ec[LOCATIONCOLNUM].ec_value.ec_value_len) {
				new_obj = nis_clone_object(nis_obj, NULL);
				ec = new_obj->EN_data.en_cols.en_cols_val;
				ec[LOCATIONCOLNUM].ec_flags = EN_MODIFIED;
				free(ec[LOCATIONCOLNUM].ec_value.ec_value_val);
				ec[LOCATIONCOLNUM].ec_value.ec_value_val = 
					cm_strdup(loc);
				ec[LOCATIONCOLNUM].ec_value.ec_value_len = 
					cm_strlen(loc)+1;
				mres = nis_modify_entry(indexname, new_obj, 0);
				if (mres->status != NIS_SUCCESS) {
					if (debug)
						nis_perror(eres->status, "nis_modify_entry");  
					res = l_nis_error;
				}
				nis_destroy_object(new_obj);
			}
			else 
				res = l_nis_error;
			nis_freeresult(mres);
		}
	}
	else if (eres->status == NIS_NOTFOUND) {
		/* table exists but no entry; add new entry to NIS+ cm table*/
		nis_obj = (nis_object*)new_nisobj();
		new_entryobj(nis_obj, loc);
		ares = nis_add_entry(cmtbl, nis_obj, 0);
		if (ares->status != NIS_SUCCESS) {
			if (debug)
				nis_perror(eres->status, "nis_add_entry");
			res = l_nis_error;
		}
		nis_destroy_object(nis_obj);
		nis_freeresult(ares);
	}
	else {
		if (debug)
			nis_perror(eres->status, "nis_list");
		res = l_nis_error;
	}
	nis_freeresult(eres);

	return(res);
}
/*
 * user specification -> calendar_name@host[.domain]
 */

extern void
cm_get_target(name, target)
char *name;
char **target;
{
        char *user, *domain;
        int status=0;
        char *host;

        *target = NULL;
 
        if (name == NULL)
                return(l_other);

	test_cm_get_target(name, target);
	return;

        /* assume user@domain and do nis+ lookup */
        status = cm_get_nis_location(name, DEFAULT_CALNAME, &host);

        if (status == l_ok) {
                /*
                 * Nis+ table exists.
                 * If we cannot find the location, default to local host.
                 * Otherwise, build the calendar name. For now,
                 * calendar name is the user login name.
                 */
                if (host) {
                        *target = (char *)ckalloc(cm_strlen(name) +
                                        cm_strlen(host) + 3);
                        user = get_head(name, '@');
                        domain = get_tail(name, '@');
                        sprintf(*target, "%s@%s", user, host);
                        if (domain) {
                                strcat(*target, ".");
                                strcat(*target, domain);
                        }
                        free(user);
                        free(domain);
                        free(host);
                } else  {
			*target = NULL;
			status  = l_nis_error;	
		}
        }
	else {
		/*
                 * Assume nis+ is not use, or no entry in table.
                 * name = user@host[.domain]
                 * If host is not specified, default to local host.
                 */
                *target = NULL;
	}
        return(status);
}

#define YP_SUCCESS 0

extern int
cm_get_yptarget(name, name2)
	char *name, **name2;
{
	char *domain;
	int outlen, inlen, status=0;
	char *save_name=NULL, *tmp=NULL, *tmp2=NULL, *ptr;

	*name2 = NULL;
	domain = cm_get_local_domain();
	inlen = cm_strlen(name)+1;
	status = yp_match(domain, MAPNAME, name, inlen, &tmp, &outlen);
	if (status != YP_SUCCESS)
		return YP_SUCCESS;
	/* Name was mapped successfully, so continue; may be more than 1 */
	/* Lots of white space in map for some dumb reason */
	while (status == YP_SUCCESS) {
		save_name = tmp;
		/* get rid of beginning white space */
		while (tmp != NULL && *tmp == ' ') tmp++;
		/* get rid of trailing white space */
		for (ptr = tmp+cm_strlen(tmp)-1;
			 ptr != NULL && *ptr == ' '; *ptr = 0, ptr--);
		inlen = cm_strlen(tmp)+1;
		status = yp_match(domain, MAPNAME, tmp, inlen, &tmp2, &outlen);
		/* no more maps so return value */
		if (status != YP_SUCCESS)
			*name2 = cm_strdup(tmp);
		else
			tmp = tmp2;
		free(save_name);
	}
	return 1;
}
#endif
