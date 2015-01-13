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
/* $XConsortium: callback.c /main/5 1996/10/03 10:40:51 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <rpc/rpc.h>
#include "agent.h"
#include "cmcb.h"
#include "callback.h"
#include "appt4.h"
#include "utility.h"
#include "lutil.h"

extern int debug;
extern char *pgname;

/*
 * forward declaration of static functions used within the file
 */
static _DtCmsRegistrationInfo * _DtCmsDoCallback(
	_DtCmsRegistrationInfo	*rlist,
	uint			type,
	char			*source,
	int			pid,
	int			version,
	void			*args);

/*****************************************************************************
 * extern functions
 *****************************************************************************/

extern _DtCmsRegistrationInfo *
_DtCmsMakeRegistrationInfo(
	char	*client,
	int	types,
	u_long	prognum,
	u_long	versnum,
	u_long	procnum,
	int	pid)
{
	_DtCmsRegistrationInfo *rinfo;

	if ((rinfo = (_DtCmsRegistrationInfo *)calloc(1,
	    sizeof(_DtCmsRegistrationInfo))) == NULL)
		return (NULL);

	if ((rinfo->client = strdup(client)) == NULL) {
		free(rinfo);
		return (NULL);
	}

	rinfo->types	= types;
	rinfo->prognum	= prognum;
	rinfo->versnum	= versnum;
	rinfo->procnum	= procnum;
	rinfo->pid	= pid;
	rinfo->next	= NULL;
	return(rinfo);
}

extern void
_DtCmsFreeRegistrationInfo(_DtCmsRegistrationInfo *rinfo)
{
	if (rinfo==NULL) return;
	if (rinfo->client != NULL)
		free(rinfo->client);
	free(rinfo);
}

/*
 * this routine is for v4, so type is not checked
 */
extern _DtCmsRegistrationInfo *
_DtCmsGetRegistration(
	_DtCmsRegistrationInfo	**_rlist,
	char			*client,
	u_long			prognum,
	u_long			versnum,
	u_long			procnum,
	int			pid)
{
	_DtCmsRegistrationInfo	*rlist = *_rlist, 
				*prev, 
				*ptr, 
				*tmp_ptr;

        for (prev = ptr = rlist; ptr != NULL; ) {
                if ((strcmp(ptr->client, client)==0) &&
                    (ptr->prognum == prognum)  &&
                    (ptr->versnum == versnum)  &&
                    (ptr->procnum == procnum))
                {
                        if (ptr->pid == pid) {
				*_rlist = rlist;
                                return (ptr);
                        } else {
                                /* ptr points to a stale record
                                 * remove it from the linked list
                                 * and update ptr and prev appropriately
                                 */
				if (ptr == prev) { /* top of list */
					rlist = rlist->next;
					_DtCmsFreeRegistrationInfo(ptr);
					ptr = prev = rlist;
				} else {
					prev->next = ptr->next;
					tmp_ptr = ptr;
					ptr = ptr->next;
					_DtCmsFreeRegistrationInfo(tmp_ptr);
				}
                        }
                } else {
                        prev = ptr;
                        ptr = ptr->next;
                }
        }

	*_rlist = rlist;
        return (NULL);
}

/*
 * Go through the registration list and ping each client.
 * Deregister clients that do not respond.
 */
extern _DtCmsRegistrationInfo *
_DtCmsCheckRegistrationList(_DtCmsRegistrationInfo *rlist)
{
	int nclients=0, ndereg=0;
	char *sourcehost=NULL;
	_DtCmsRegistrationInfo *p_next;
	_DtCmsRegistrationInfo *p_prev;
	_DtCmsRegistrationInfo *head;
	struct timeval timeout_tv;
	CLIENT *cl;
	boolean_t advance = B_TRUE;

	timeout_tv.tv_sec = 10;
	timeout_tv.tv_usec = 0;

	/* loop through the registration list */

	head = p_prev = rlist;
	while (rlist != NULL) {

		p_next = rlist->next;

		if (debug) {
			fprintf(stderr,
			  "%s: pinging %s on prog: %ld, vers: %ld, proc: %ld\n",
			  pgname, rlist->client, rlist->prognum, rlist->versnum,
			  rlist->procnum);
		}

		sourcehost = _DtCmsTarget2Location(rlist->client);
		cl = clnt_create(sourcehost, rlist->prognum, rlist->versnum,
			"udp");

		if (cl != NULL) {
			clnt_control(cl, CLSET_TIMEOUT, (char *)&timeout_tv);
			timeout_tv.tv_sec = 5;
			clnt_control(cl, CLSET_RETRY_TIMEOUT,
				(char*)&timeout_tv);
		}
				
		/* no client or client not responding */
		if (cl == NULL || clnt_call(cl, 0, (xdrproc_t)xdr_void, (char *)NULL,
			(xdrproc_t)xdr_void, (char *)NULL, timeout_tv) != RPC_SUCCESS)
		{
			if (debug) {
				clnt_pcreateerror(sourcehost);
				fprintf(stderr, "%s: %s deregistered, pid %d\n",
					pgname, rlist->client, rlist->pid);
			}

			if (rlist == p_prev) { /* top of list */
				head = p_next;
				p_prev = p_next;
				advance = B_FALSE;
			} else {
				p_prev->next = p_next;
			}

			/* deregister client */
			_DtCmsFreeRegistrationInfo(rlist);

			rlist = p_prev;

			ndereg++;
		}

		if (cl)
			clnt_destroy(cl);

		free(sourcehost);
		nclients++;

		if (advance) {
			p_prev = rlist;
                	rlist = p_next;
		} else
			advance = B_TRUE;
        }

	if (debug) {
		fprintf(stderr, "%s: number of clients before cleanup = %d\n",
			pgname, nclients);
		fprintf(stderr, "%s: number of clients deregistered = %d\n",
			pgname, ndereg);
	}

	return (head);
}

extern _DtCmsRegistrationInfo *
_DtCmsDoV1CbForV4Data(
	_DtCmsRegistrationInfo	*rlist,
	char			*source,
	int			pid,
	cms_key			*key1,
	cms_key			*key2)
{
	Appt_4	appt1, appt2;

	if (rlist == NULL)
		return (rlist);

	appt1.appt_id.tick = key1->time;
	appt1.appt_id.key = key1->id;

	if (key2) {
		appt2.appt_id.tick = key2->time;
		appt2.appt_id.key = key2->id;
		appt2.next = NULL;
		appt1.next = &appt2;
	} else
		appt1.next = NULL;

	return (_DtCmsDoV1Callback(rlist, source, pid, &appt1));
}

/*
 * this routine takes care of callbacks to clients using v1 of the callback
 * protocol.
 */
extern _DtCmsRegistrationInfo *
_DtCmsDoV1Callback(
	_DtCmsRegistrationInfo	*rlist,
	char			*source,
	int			pid,
	Appt_4			*a)
{
	Uid_4 *k, *ids = NULL;
	Table_Res_4 res;
        int nclients=0, ncallbacks=0;
	char *sourcehost=NULL;
        _DtCmsRegistrationInfo *ptr;
        _DtCmsRegistrationInfo *prev;
	struct timeval timeout_tv;
	CLIENT *cl;
	boolean_t advance = B_TRUE;

	if (rlist == NULL)
		return (rlist);

	/* Callback with appointment ids only for security reason. */
	while (a != NULL)
	{
		if ((k = (Uid_4 *)malloc(sizeof(Uid_4))) == NULL) {
			_DtCm_free_keyentry4(ids);
			return (rlist);
		}

		k->appt_id = a->appt_id;
		k->next = ids;
		ids = k;
		a = a->next;
	}

	res.status = access_ok_4;
	res.res.tag = ID_4;
	res.res.Table_Res_List_4_u.i = ids;

	rlist = _DtCmsDoCallback(rlist, 0, source, pid, AGENTVERS, (void *)&res);

	if (ids != NULL)
		_DtCm_free_keyentry4(ids);

	return (rlist);
}

extern void
_DtCmsListRegistration(_DtCmsRegistrationInfo *rlist, char *cal)
{
	int n = 0;

	fprintf(stderr, "registration list of calendar %s\n", cal);
	while(rlist != NULL) {
		n++;
		fprintf(stderr, "\t%s (pid %d)\n", rlist->client, rlist->pid);
		rlist = rlist->next;
	}
	fprintf(stderr, "\tnumber of registered clients = %d\n", n);
}

extern _DtCmsRegistrationInfo *
_DtCmsRemoveRegistration(
	_DtCmsRegistrationInfo *rlist,
	_DtCmsRegistrationInfo *rinfo)
{
	_DtCmsRegistrationInfo *ptr, *prev;


	for (ptr = prev = rlist; ptr != NULL; prev = ptr, ptr = ptr->next) {
		if (ptr == rinfo) {
			if (ptr == rlist)
				rlist = ptr->next;
			else
				prev->next = ptr->next;

			_DtCmsFreeRegistrationInfo(ptr);
			break;
		}
	}

	return (rlist);
}

extern _DtCmsRegistrationInfo *
_DtCmsDoOpenCalCallback(
	_DtCmsRegistrationInfo	*rlist,
	char			*cal,
	char			*user,
	int			pid)
{
	cmcb_update_callback_args args;
	char calendar[BUFSIZ];

	if (rlist == NULL)
		return (rlist);

	sprintf(calendar, "%s@%s", cal, _DtCmGetLocalHost());
	args.calendar = calendar;
	args.user = user;
	args.data.reason = CSA_CB_CALENDAR_LOGON;

	return (_DtCmsDoCallback(rlist, CSA_CB_CALENDAR_LOGON, user, pid,
		AGENTVERS_2, (void *)&args));
}


extern _DtCmsRegistrationInfo *
_DtCmsDoRemoveCalCallback(
	_DtCmsRegistrationInfo	*rlist,
	char			*cal,
	char			*user,
	int			pid)
{
	cmcb_update_callback_args args;
	char calendar[BUFSIZ];

	if (rlist == NULL)
		return (rlist);

	sprintf(calendar, "%s@%s", cal, _DtCmGetLocalHost());
	args.calendar = calendar;
	args.user = user;
	args.data.reason = CSA_CB_CALENDAR_DELETED;

	return (_DtCmsDoCallback(rlist, CSA_CB_CALENDAR_DELETED, user, pid,
		AGENTVERS_2, (void *)&args));
}

extern _DtCmsRegistrationInfo *
_DtCmsDoUpdateCalAttrsCallback(
	_DtCmsRegistrationInfo	*rlist,
	char			*cal,
	char			*user,
	uint			num_attrs,
	cms_attribute		*attrs,
	int			pid)
{
	cmcb_update_callback_args	args;
	cmcb_cal_attr_data		cdata; 
	_DtCmsRegistrationInfo		*res;
	char buf[80];
	char calendar[BUFSIZ];
	int i;

	if (rlist == NULL)
		return (rlist);

	sprintf(calendar, "%s@%s", cal, _DtCmGetLocalHost());

	/* set up update info */
	if (num_attrs > 0 &&
	    (cdata.names = (char **)calloc(1, sizeof(char *)*num_attrs)))
	{
		for (i = 0; i < num_attrs; i++)
			cdata.names[i] = attrs[i].name.name;
	} else {
		cdata.num_names = 0;
		cdata.names = NULL;
	}

	args.calendar = calendar;
	args.user = user;
	args.data.reason = CSA_CB_CALENDAR_ATTRIBUTE_UPDATED;
	args.data.data.cdata = &cdata;

	res = _DtCmsDoCallback(rlist, CSA_CB_CALENDAR_ATTRIBUTE_UPDATED, user,
		pid, AGENTVERS_2, (void *)&args);

	if (num_attrs > 0) free(cdata.names);

	return (res);
}

extern _DtCmsRegistrationInfo *
_DtCmsDoInsertEntryCallback(
	_DtCmsRegistrationInfo	*rlist,
	char			*cal,
	char			*source,
	long			id,
	int			pid)
{
	cmcb_update_callback_args args;
	cmcb_add_entry_data adata; 
	char buf[80];
	char calendar[BUFSIZ];

	if (rlist == NULL)
		return (rlist);

	sprintf(calendar, "%s@%s", cal, _DtCmGetLocalHost());

	/* set up update info */
	sprintf(buf, "%ld", id);
	adata.id = buf;
	args.calendar = calendar;
	args.user = source;
	args.data.reason = CSA_CB_ENTRY_ADDED;
	args.data.data.adata = &adata;

	return (_DtCmsDoCallback(rlist, CSA_CB_ENTRY_ADDED, source, pid,
		AGENTVERS_2, (void *)&args));
}

extern _DtCmsRegistrationInfo *
_DtCmsDoDeleteEntryCallback(
	_DtCmsRegistrationInfo	*rlist,
	char			*cal,
	char			*source,
	long			id,
	int			scope,
	time_t time,
	int			pid)
{
	cmcb_update_callback_args args;
	cmcb_delete_entry_data ddata; 
	char buf[80];
	char calendar[BUFSIZ];

	if (rlist == NULL)
		return (rlist);

	sprintf(calendar, "%s@%s", cal, _DtCmGetLocalHost());

	/* set up update info */
	sprintf(buf, "%ld", id);
	ddata.id = buf;
	ddata.scope = scope;
	ddata.time = time;
	args.calendar = calendar;
	args.user = source;
	args.data.reason = CSA_CB_ENTRY_DELETED;
	args.data.data.ddata = &ddata;

	return (_DtCmsDoCallback(rlist, CSA_CB_ENTRY_DELETED, source, pid,
		AGENTVERS_2, (void *)&args));
}

extern _DtCmsRegistrationInfo *
_DtCmsDoUpdateEntryCallback(
	_DtCmsRegistrationInfo	*rlist,
	char			*cal,
	char			*source,
	long			newid,
	long			oldid,
	int			scope,
	long			time,
	int			pid)
{
	cmcb_update_callback_args args;
	cmcb_update_entry_data udata; 
	char nbuf[80], obuf[80];
	char calendar[BUFSIZ];

	if (rlist == NULL)
		return (rlist);

	sprintf(calendar, "%s@%s", cal, _DtCmGetLocalHost());

	/* set up update info */
	sprintf(obuf, "%ld", oldid);
	udata.oldid = obuf;
	if (newid > 0)
		sprintf(nbuf, "%ld", newid);
	else
		nbuf[0] = '\0';
	udata.newid = nbuf;
	udata.scope = scope;
	udata.time = time;
	args.calendar = calendar;
	args.user = source;
	args.data.reason = CSA_CB_ENTRY_UPDATED;
	args.data.data.udata = &udata;

	return (_DtCmsDoCallback(rlist, CSA_CB_ENTRY_UPDATED, source, pid,
		AGENTVERS_2, (void *)&args));
}

/*
 * this routine takes care of callbacks to clients using either
 * v1 or v2 of the callback protocol.
 */
static _DtCmsRegistrationInfo *
_DtCmsDoCallback(
	_DtCmsRegistrationInfo	*rlist,
	uint			type,
	char			*source,
	int			pid,
	int			version,
	void			*args)
{
        int nclients=0, ncallbacks=0;
	char *sourcehost=NULL;
        _DtCmsRegistrationInfo *ptr;
        _DtCmsRegistrationInfo *prev;
	struct timeval timeout_tv;
	CLIENT *cl;
	boolean_t advance = B_TRUE;
        
	/*
	 * loop through the registration list looking for parties
	 * interested in this transaction.
	 */

        for (ptr = prev = rlist; ptr != NULL; ) {

		/* The caller will get the results of the rpc call.
		 * If he's registered on the callback list, don't call him -
		 * UNLESS the process id of his client differs from the
		 * original ticket. However, if the pid is a VOIDPID (-1),
		 * a version 2 client has registered and there's no way
	         * of telling which instance of the client it is.  So,
		 * to be safe (avoid deadlock) we won't callback version
		 * 2 clients registered on version 3 daemons if their
		 * registration name entry matches the caller's. [Nanno]
		 */

		if (version != ptr->versnum ||
		    (type && !(type & ptr->types)) ||
		    ((strcmp(source, ptr->client) == 0) &&
		    ((pid == ptr->pid) || (pid == -1 ) || (ptr->pid == -1)))) {
			prev = ptr;
			ptr = ptr->next;
			nclients++;
			continue;
		}

		sourcehost = _DtCmsTarget2Location(ptr->client);
		if (debug) {
			fprintf(stderr,
			  "%s: calling back %s on prog: %ld, vers: %ld, proc: %ld\n",
			  pgname, ptr->client, ptr->prognum, ptr->versnum,
			  ptr->procnum);
		}
		cl = clnt_create(sourcehost, ptr->prognum, ptr->versnum, "udp");

		/* deregister client if fails to create handle */
		if (cl == NULL) {
			if (debug) {
				clnt_pcreateerror(sourcehost);
			}

			if (ptr == rlist) { /* top of list */
				rlist = ptr->next;
				prev = rlist;
				advance = B_FALSE;
			} else {
				prev->next = ptr->next;
			}

			/* deregister client */
			_DtCmsFreeRegistrationInfo(ptr);

			ptr = prev;

		} else {
			/* Set timeout to zero so that the call
			 * returns right away.
			 */
			timeout_tv.tv_sec = 0;
			timeout_tv.tv_usec = 0;

#ifndef SunOS
			/* for non-sun systems, clnt_call won't
			 * return right away unless timeout is set
			 * to zero using clnt_control(), (rpc bug?)
			 */
			clnt_control(cl, CLSET_TIMEOUT,
					(char *)&timeout_tv);
#endif
			if (version == AGENTVERS) {
				(void)clnt_call(cl, ptr->procnum,
					(xdrproc_t)_DtCm_xdr_Table_Res_4, (char *)args,
					(xdrproc_t)xdr_void, (char *)0, timeout_tv);
			} else if (version == AGENTVERS_2) {
				(void)clnt_call(cl, ptr->procnum,
					(xdrproc_t)xdr_cmcb_update_callback_args,
					(char *)args, (xdrproc_t)xdr_void, (char *)0,
					timeout_tv);
			}
			ncallbacks++;
			nclients++;
		}

		if (cl)
			clnt_destroy(cl);

		free(sourcehost);

		if (advance) {
			prev = ptr;
                	ptr = ptr->next;
		} else
			advance = B_TRUE;
        }

	if (debug) {
		fprintf(stderr, "%s: number of registered clients = %d\n",
			pgname, nclients);
		fprintf(stderr, "%s: number of clients called back= %d\n",
			pgname, ncallbacks);
	}

	return (rlist);
}

