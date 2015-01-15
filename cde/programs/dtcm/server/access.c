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
/* $XConsortium: access.c /main/5 1996/10/08 16:41:05 barstow $ */
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
#include <pwd.h>
#ifdef SunOS
#include <sys/systeminfo.h>
#endif
#include "access.h"
#include "appt4.h"
#include "utility.h"
#include "log.h"
#include "lutil.h"
#include "laccess.h"

#define NUM_CACHE               50      /* cache for unix user name */

extern int debug;

typedef struct uname_cache {
	uid_t	uid;			/* unix user id */
	char	*name;			/* user name */
	struct	uname_cache *next;
} Uname_cache;

static	Uname_cache	*ucache_list = NULL;

/******************************************************************************
 * forward declaration of static functions used within the file
 ******************************************************************************/
static Uname_cache * in_u_cache(uid_t uid);
static char * get_uname(uid_t uid);
static Access_Entry_4 * in_access_list(Access_Entry_4 *l, char *s);
static Access_Entry_4 * combine_access_list(Access_Entry_4 *p_list, 
				Access_Entry_4 *p_head, int type, int *p_world);
static CSA_return_code _GetV4AccessRights(_DtCmsCalendar *cal, char *target,
				char *sender, uint *access);
static CSA_return_code _GetV5AccessRights(_DtCmsCalendar *cal, char *target,
				char *sender, uint *access);

/*****************************************************************************
 * extern functions used in the library
 *****************************************************************************/

/*
 * If the requester is the owner, "*sender" will be set to NULL.
 */
extern CSA_return_code
_DtCmsV4LoadAndCheckAccess(
	struct svc_req	*svcrq,
	char		*target,
	char		**sender,
	uint		*access,
	_DtCmsCalendar	**cal)
{
	CSA_return_code	stat;

	if (target == NULL || sender == NULL || cal == NULL)
		return (CSA_E_INVALID_PARAMETER);

	if ((stat = _DtCmsGetClientInfo(svcrq, sender)) != CSA_SUCCESS)
		return (stat);

	if ((stat = _DtCmsGetCalendarByName(target, B_TRUE, cal))
	    != CSA_SUCCESS)
		return (stat);

	if ((*cal)->fversion == _DtCMS_VERSION1)
		return (_GetV4AccessRights(*cal, target, *sender, access));
	else
		return (_GetV5AccessRights(*cal, target, *sender, access));

}

/*
 * If the requester is the owner, "*p_src" will be set to NULL.
 */
extern CSA_return_code
_DtCmsV5LoadAndCheckAccess(
	struct svc_req	*svcrq,
	char		*target,
	char		**sender,
	uint		*access,
	_DtCmsCalendar	**cal)
{
	CSA_return_code		stat;
	cms_access_entry	*alist;
	cms_attribute_value	*owner;
	int			worldaccess = 0, useraccess = 0;
	boolean_t		isowner;


	if (target == NULL || sender == NULL || cal == NULL)
		return (CSA_E_INVALID_PARAMETER);

	if ((stat = _DtCmsGetClientInfo(svcrq, sender)) != CSA_SUCCESS)
		return (stat);

	if ((stat = _DtCmsGetCalendarByName(target, B_TRUE, cal))
	    != CSA_SUCCESS)
		return (stat);

	if ((*cal)->fversion == _DtCMS_VERSION1)
		return (_GetV4AccessRights(*cal, target, *sender, access));
	else
		return (_GetV5AccessRights(*cal, target, *sender, access));
}

extern CSA_return_code
_DtCmsGetClientInfo(struct svc_req *svcrq, char **source)
{
	char *name;
	char *uname;
	struct authunix_parms *unix_cred;

	if (source == NULL)
	{
		return (CSA_E_INVALID_PARAMETER);
	}

	switch (svcrq->rq_cred.oa_flavor) {
	case AUTH_UNIX:
		unix_cred = (struct authunix_parms *) svcrq->rq_clntcred;
		if (unix_cred == NULL)
			return (CSA_E_NO_AUTHORITY);
		if ((name = get_uname (unix_cred->aup_uid)) == NULL)
			return (CSA_E_INSUFFICIENT_MEMORY);
		if ((uname = malloc(strlen(name) +
		    strlen(unix_cred->aup_machname) + 2)) == NULL)
			return (CSA_E_INSUFFICIENT_MEMORY);
		else {
			sprintf(uname, "%s@%s", name, unix_cred->aup_machname);
			*source = uname;
			return (CSA_SUCCESS);
		}

	case AUTH_NULL:
	default:
		svcerr_weakauth(svcrq->rq_xprt);
		return (CSA_E_NO_AUTHORITY);
	}
}

/*
 * good format of owner and user assumed:
 * owner: user[@host[.domain]]
 * user: user@host[.domain]
 * target: name@host[.domain]
 */
extern boolean_t
_DtCmsIsFileOwner(char *owner, char *user, char *target)
{
	char *ptr1, *ptr2, *ptr3;

	if (debug) {
		fprintf(stderr, "rpc.cmsd: %s, %s = %s, %s = %s, %s = %s\n",
			"check file owner",
			"owner", ((owner == NULL) ? "NULL" : owner),
			"user", ((user == NULL) ? "NULL" : user),
			"target", ((target == NULL) ? "NULL" : target));
	}

	if (owner == NULL || user == NULL || target == NULL)
		return (B_FALSE);

	ptr1 = _DtCmGetPrefix(owner, '@');
	ptr2 = _DtCmGetPrefix(user, '@');
	if (strcmp(ptr1, ptr2)) {
		free(ptr1);
		free(ptr2);
		return(B_FALSE);
	}
	free(ptr1);
	free(ptr2);

	/* check domain if domain info is available */

	ptr1 = strchr(user, '.');
	ptr2 = strchr(target, '@');
	if (ptr2)
		ptr3 = strchr(ptr2, '.');
	else
		ptr3 = NULL;

	if (ptr1 == NULL || ptr3 == NULL)
		/* assume that user is in the local domain */
		return B_TRUE;
	else
		return(_DtCmIsSamePath(++ptr1, ++ptr3));
}

extern void
_DtCmsShowAccessList(Access_Entry_4 *l)
{
	while (l!=NULL) {
		fprintf(stderr, "Access: %s(%c%c%c)\n", l->who,
			l->access_type & access_read_4   ? 'r' : '_',
			l->access_type & access_write_4  ? 'w' : '_',
			l->access_type & access_delete_4 ? 'd' : '_');
		l = l->next;
	}
}

extern Access_Entry_4 *
_DtCmsCalendarAccessList(_DtCmsCalendar *cal)
{
	int		world = access_none_4;
	Access_Entry_4	*a;
	Access_Entry_4	*l = NULL;

	l = combine_access_list(GET_R_ACCESS(cal), l, access_read_4, &world);
	l = combine_access_list(GET_W_ACCESS(cal), l, access_write_4, &world);
	l = combine_access_list(GET_D_ACCESS(cal), l, access_delete_4, &world);
	l = combine_access_list(GET_X_ACCESS(cal), l, access_exec_4, &world);

	/* WORLD exists in one of the lists, add her to the combined list. */
	if (world != access_none_4)
	{
		a = _DtCm_make_access_entry4(WORLD, world);
		a->next = l;
		l = a;
	}
	return (l);
}

extern Privacy_Level_4
_DtCmCheckPrivacyLevel(char **p_src, Appt_4 *p_appt)
{
	if (*p_src == NULL)
		return(public_4);

	if (p_appt != NULL) {
		/*
		 * if p_src is the author of the appointment,
		 * it should see everything.
		 */
		if (_DtCmIsSameUser(*p_src, p_appt->author)) {
			*p_src = NULL;
			return(public_4);
		} else
			return(p_appt->privacy);
	} else
		return(private_4);
}

/*
 * the user can view the entry if it has OWNER rights,
 * the appropriate VIEW rights or he is the organizer of
 * the entry and has ORGANIZER rights.
 */
extern CSA_return_code
_DtCmsCheckViewAccess(char *user, uint access, cms_entry *eptr)
{
	uint	need;
	cms_attribute_value *oval, *sval;

	need = (_DtCmsClassToViewAccess(eptr)) | CSA_OWNER_RIGHTS;
	if (access & need) {
		return (CSA_SUCCESS);
	} else {
		oval = eptr->attrs[CSA_ENTRY_ATTR_ORGANIZER_I].value;
		sval = eptr->attrs[CSA_ENTRY_ATTR_SPONSOR_I].value;
		if (((access & CSA_ORGANIZER_RIGHTS) &&
		    _DtCmIsSameUser(user, oval->item.calendar_user_value)) ||
		    ((access & CSA_SPONSOR_RIGHTS) && sval &&
		    _DtCmIsSameUser(user, sval->item.calendar_user_value)))
			return (CSA_SUCCESS);
		else if ( (need & ~CSA_OWNER_RIGHTS) == (CSA_VIEW_CONFIDENTIAL_ENTRIES) ) {
			return (CSA_E_TIME_ONLY);
		} else
			return (CSA_E_NO_AUTHORITY);
	}
}

extern CSA_return_code
_DtCmsCheckChangeAccess(char *user, uint access, cms_entry *eptr)
{
	uint	need;
	cms_attribute_value *oval, *sval;

	need = (_DtCmsClassToChangeAccess(eptr)) | CSA_OWNER_RIGHTS;

	if (access & need) {
		return (CSA_SUCCESS);
	} else {
		oval = eptr->attrs[CSA_ENTRY_ATTR_ORGANIZER_I].value;
		sval = eptr->attrs[CSA_ENTRY_ATTR_SPONSOR_I].value;
		if (((access & CSA_ORGANIZER_RIGHTS) &&
		    _DtCmIsSameUser(user, oval->item.calendar_user_value)) ||
		    ((access & CSA_SPONSOR_RIGHTS) && sval &&
		    _DtCmIsSameUser(user, sval->item.calendar_user_value)))
			return (CSA_SUCCESS);
		else
			return (CSA_E_NO_AUTHORITY);
	}
}

extern uint
_DtCmsClassToViewAccess(cms_entry *entry)
{
	cms_attribute_value *val;

	val = entry->attrs[CSA_ENTRY_ATTR_CLASSIFICATION_I].value;

	switch (val->item.uint32_value) {
	case CSA_CLASS_PUBLIC:
		return (CSA_VIEW_PUBLIC_ENTRIES);
	case CSA_CLASS_PRIVATE:
		return (CSA_VIEW_PRIVATE_ENTRIES);
	case CSA_CLASS_CONFIDENTIAL:
		return (CSA_VIEW_CONFIDENTIAL_ENTRIES);
	}
	
	fprintf(stderr, "_DtCmsClassToInsertAccess: Unsupported Class %lu\n", val->item.uint32_value);
	exit(EXIT_FAILURE);
}

extern uint
_DtCmsClassToInsertAccess(cms_entry *entry)
{
	cms_attribute_value *val;

	val = entry->attrs[CSA_ENTRY_ATTR_CLASSIFICATION_I].value;

	switch (val->item.uint32_value) {
	case CSA_CLASS_PUBLIC:
		return (CSA_INSERT_PUBLIC_ENTRIES);
	case CSA_CLASS_PRIVATE:
		return (CSA_INSERT_PRIVATE_ENTRIES);
	case CSA_CLASS_CONFIDENTIAL:
		return (CSA_INSERT_CONFIDENTIAL_ENTRIES);
	}

	fprintf(stderr, "_DtCmsClassToInsertAccess: Unsupported Class %lu\n", val->item.uint32_value);
	exit(EXIT_FAILURE);
}

extern uint
_DtCmsClassToChangeAccess(cms_entry *entry)
{
	cms_attribute_value *val;

	val = entry->attrs[CSA_ENTRY_ATTR_CLASSIFICATION_I].value;

	switch (val->item.uint32_value) {
	case CSA_CLASS_PUBLIC:
		return (CSA_CHANGE_PUBLIC_ENTRIES);
	case CSA_CLASS_PRIVATE:
		return (CSA_CHANGE_PRIVATE_ENTRIES);
	case CSA_CLASS_CONFIDENTIAL:
		return (CSA_CHANGE_CONFIDENTIAL_ENTRIES);
	}

	fprintf(stderr, "_DtCmsClassToChangeAccess: Unsupported Class %lu\n", val->item.uint32_value);
	exit(EXIT_FAILURE);
}

/*****************************************************************************
 * static functions used within the file
 *****************************************************************************/

static Uname_cache *
in_u_cache(uid_t uid)
{
	int	cache = NUM_CACHE;
	Uname_cache *p_prev;
	Uname_cache *p_cache;

	p_prev = NULL;
	p_cache = ucache_list;
	while (p_cache != NULL)
	{
		if (p_cache->uid == uid)
			return (p_cache);
		if (--cache < 0)
		{
			/* Assume that the cache size is at least 1 */
			p_prev->next = p_cache->next;
			free (p_cache->name);
			free (p_cache);
			p_cache = p_prev->next;
		}
		else
		{
			p_prev = p_cache;
			p_cache = p_cache->next;
		}
	}
	return (NULL);
}

static char *
get_uname(uid_t uid)
{
	struct passwd *pw;
	char buff[16];
	Uname_cache *ucache, *prev;

	if ((ucache = in_u_cache(uid)) == NULL)
	{
		if ((pw = getpwuid (uid)) == NULL) {
			/* Can't map uid to name.  Don't cache the uid. */
			sprintf (buff, "%ld", (long)uid);
			return (strdup(buff));
		}

		if ((ucache = (Uname_cache *)malloc(sizeof(Uname_cache)))
		    == NULL)
			return (NULL);

		if ((ucache->name = strdup(pw->pw_name)) == NULL) {
			free(ucache);
			return (NULL);
		}
		ucache->uid = uid;
		ucache->next = ucache_list;
		ucache_list = ucache;
	}

	return (strdup(ucache->name));
}

static Access_Entry_4 *
in_access_list(Access_Entry_4 *l, char *s)
{
	char	*name;

	if (l==NULL || s==NULL) return(NULL);
	while(l != NULL) {
		/* only for combining lists, not for authentication */
		if (strcmp(l->who, s) == 0)
			break;
		l = l->next;
	}
	return(l);
}

static Access_Entry_4 *
combine_access_list(
	Access_Entry_4 *p_list, 
	Access_Entry_4 *p_head, 
	int type, 
	int *p_world)
{
	Access_Entry_4	*a;
	Access_Entry_4	*h = p_head;

	while (p_list != NULL)
	{
		/* Delay to put the WORLD into the combined list because 
		 * in_access_list() may return wrong result.
		 */
		if (strcmp (p_list->who, WORLD) == 0)
			*p_world |= type;
		else
		{
			/* The user is not in the combined list, add to list. */
			if ((a = in_access_list (h, p_list->who)) == NULL)
			{
				a = _DtCm_make_access_entry4(p_list->who, type);
				a->next = p_head;
				p_head = a;
			}
			a->access_type |= type;
		}
		p_list = p_list->next;
	}
	return (p_head);
}

static CSA_return_code
_GetV4AccessRights(
	_DtCmsCalendar	*cal,
	char		*target,
	char		*sender,
	uint		*access)
{
	int		worldaccess = 0, useraccess = 0;
	Access_Entry_4	*alist;

	/* first check to see if the user is the owner of the calendar */
	if (_DtCmsIsFileOwner(cal->owner, sender, target)) {
		*access = CSA_OWNER_RIGHTS;
		return (CSA_SUCCESS);
	}

	for (alist = cal->alist; alist != NULL; alist = alist->next) {
		if (strcmp(alist->who, WORLD) == 0)
			worldaccess = alist->access_type;
		else if (_DtCmIsSameUser(sender, alist->who)) {
			useraccess = alist->access_type;
			break;
		}
	}

	*access = worldaccess | useraccess;
	return (CSA_SUCCESS);
}

static CSA_return_code
_GetV5AccessRights(
	_DtCmsCalendar	*cal,
	char		*target,
	char		*sender,
	uint		*access)
{
	cms_access_entry	*alist;
	cms_attribute_value	*owner;
	int			worldaccess = 0, useraccess = 0;
	boolean_t		isowner;

	/* first check to see if the user is the owner of the calendar */
	owner = cal->attrs[CSA_CAL_ATTR_CALENDAR_OWNER_I].value;
	isowner = _DtCmsIsFileOwner(owner->item.calendar_user_value, sender,
			target);

	if (isowner && cal->checkowner == B_FALSE) {
		*access = CSA_OWNER_RIGHTS;
		return (CSA_SUCCESS);
	}

	alist = cal->attrs[CSA_CAL_ATTR_ACCESS_LIST_I].value->\
			item.access_list_value;

	if (alist == NULL) {
		*access = worldaccess | useraccess;
		return (CSA_E_NO_AUTHORITY);
	}

	for (; alist != NULL; alist = alist->next) {
		if (strcmp(alist->user, WORLD) == 0)
			worldaccess = alist->rights;
		else if (_DtCmIsSameUser(sender, alist->user)) {
			useraccess = alist->rights;
			break;
		}
	}

	*access = worldaccess | useraccess;
	return (CSA_SUCCESS);
}

