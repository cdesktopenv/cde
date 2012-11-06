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
/* $TOG: connection.c /main/4 1999/10/14 17:47:12 mgreess $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

/*
 * This file manages server connections.
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#ifdef HPUX
#include <sys/resource.h>
#endif
#include "connection.h"
#include "rtable2.h"
#include "rtable3.h"
#include "rtable4.h"
#include "cm.h"
#include "debug.h"
#include "agent.h"
#include "convert2-4.h"
#include "convert3-4.h"
#include "rpccalls.h"

#ifdef HPUX
#define MAX_COUNT	10
#else
#define MAX_COUNT	40
#endif

static struct timeval timeout_tv;
static struct timeval retry_tv;
static AUTH *unix_credential = NULL;
static tcp_count = 0;
static cl_count = 0;
static _DtCm_Client_Info *client_cache_head = NULL;
static _DtCm_Client_Info *client_cache_tail = NULL;

/*****************************************************************************
 * forward declaration of static functions.
 *****************************************************************************/
static void create_auth(CLIENT *cl);
static void destroy_auth(CLIENT *cl);
static _DtCm_Client_Info * get_client_info(char *host, int version);
static void destroy_target_list(_DtCm_Target_List *tlist);
static void destroy_client_info(_DtCm_Client_Info *ci);
static void insert_client_info(_DtCm_Client_Info *ci);
static void delete_client_info(_DtCm_Client_Info *oldci);
static void cleanup_some_connection(_DtCm_Client_Info *dontclose);
static void check_registration(_DtCm_Connection *conn);
static _DtCm_Client_Info * get_new_client_handle(_DtCm_Connection *conn);
#ifdef __osf__
static CSA_return_code get_client_handle(const char *host, const u_int prognum,
                        u_long *vers_outp, const u_long vers_low,
                        const u_long vers_high, char *nettype,
                        CLIENT **clnt);
#else
static CSA_return_code get_client_handle(const char *host, const u_long prognum,
			u_long *vers_outp, const u_long vers_low,
			const u_long vers_high, char *nettype,
			CLIENT **clnt);
#endif
static CSA_return_code regstat4_to_dtcmstatus(Registration_Status_4 stat4);

extern CSA_return_code
_DtCm_create_udp_client(
	char *host,
	int version,
	int timeout,
	_DtCm_Client_Info **clnt)
{
	CSA_return_code	stat;
	_DtCm_Client_Info	*ci;
	u_long		vers_out;
	CLIENT		*cl=NULL;

	if (host == NULL || clnt == NULL)
		return (CSA_E_INVALID_PARAMETER);

	/* if client info is found, we have at least the udp handle */
	if (((*clnt) = get_client_info(host, version)) != NULL) {
		return (CSA_SUCCESS);
	}

#if defined(SunOS) || defined(USL) || defined(__uxp__)
	cl = clnt_create_vers(host, TABLEPROG, 
		&vers_out, TABLEVERS_2, version, "udp");
	if (cl==NULL) {
		_DtCm_print_errmsg(clnt_spcreateerror(host));
		return (_DtCm_clntstat_to_csastat(rpc_createerr.cf_stat));
	}
#else
	stat = get_client_handle(host, (u_int)TABLEPROG, &vers_out, TABLEVERS_2,
		version, "udp", &cl);
	if (stat != CSA_SUCCESS)
		return (stat);
#endif

	/* if version is lower than requested, check the list again */
	if (vers_out < version) {
		if ((ci = get_client_info(host, vers_out)) != NULL) {
			clnt_destroy(cl);
			*clnt = ci;
			return (CSA_SUCCESS);
		}
	}

	create_auth(cl);

	/* Adjust Timeout */
	if (timeout==0) timeout = _DtCM_DEFAULT_TIMEOUT;
	timeout_tv.tv_sec =  timeout;
	timeout_tv.tv_usec = 0;
	clnt_control(cl, CLSET_TIMEOUT, (char*)&timeout_tv);		

	/*	UDP only!
		time rpc waits for server to reply before retransmission =
		'timeout'. since the retry timeout is set to timeout + 10;
		this guarantees there won't
		be any retransmisssions resulting in duplicate 
		transactions in the database.
	*/

	retry_tv.tv_sec =  timeout + 10;
	retry_tv.tv_usec = 0;
	clnt_control(cl, CLSET_RETRY_TIMEOUT, (char*)&retry_tv);

	if ((ci  = (_DtCm_Client_Info *)calloc(1, sizeof(_DtCm_Client_Info))) == NULL) {
		destroy_auth(cl);
		clnt_destroy(cl);
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	if ((ci->host = strdup(host)) == NULL) {
		destroy_auth(cl);
		clnt_destroy(cl);
		free(ci);
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	ci->udpcl = cl;
	ci->vers_out = vers_out;
	insert_client_info(ci);
	*clnt = ci;
	return (CSA_SUCCESS);
}

/*
 * Creates tcp client handle.  Used for calls that potentially return
 * large amount of data.  If it fails to create a tcp client handle,
 * a udp client handle will be returned.
 */
extern CSA_return_code
_DtCm_create_tcp_client(
	char *host,
	int version,
	int timeout,
	_DtCm_Client_Info **clnt)
{
	CSA_return_code	stat;
	_DtCm_Client_Info	*ci;
	u_long		vers_out;
	CLIENT		*cl=NULL;

	if (host == NULL || clnt == NULL)
		return (CSA_E_INVALID_PARAMETER);

	/* Get a udp client handle.  This serves two purposes:	       	   */ 
	/* - to get a udp handle for an old server which talks only udp    */
	/* - to invoke a server through inetd since only udp is registered.*/

	if ((stat = _DtCm_create_udp_client(host, version, timeout, &ci))
	    != CSA_SUCCESS) {
		return (stat);
	} else if (ci->tcpcl) {
		*clnt = ci;
		return (CSA_SUCCESS);
	} else {
		/* create tcp connection */
#if defined(SunOS) || defined(USL) || defined(__uxp__)
		cl = clnt_create_vers(host, TABLEPROG, &vers_out,
			TABLEVERS_2, version, "tcp");
#else
		stat = get_client_handle(host, (u_int)TABLEPROG, &vers_out,
			TABLEVERS_2, version, "tcp", &cl);
#endif

		/* if can't create tcp connection, use udp */
		if (cl==NULL) {
			_DtCm_print_errmsg(clnt_spcreateerror(host));
			*clnt = ci;
			return (CSA_SUCCESS);
		}

		create_auth(cl);

		/* Adjust Timeout */
		if (timeout==0) timeout = _DtCM_DEFAULT_TIMEOUT;
		timeout_tv.tv_sec =  timeout;
		timeout_tv.tv_usec = 0;
		clnt_control(cl, CLSET_TIMEOUT, (char*)&timeout_tv);		

		/* dont need to set vers_out since it should
		 * be the same as that of the udp transport
		 */
		ci->tcpcl = cl;
		if (++tcp_count > MAX_COUNT)
			/* clean up tcp connections */
			cleanup_some_connection(ci);
		*clnt = ci;
		return (CSA_SUCCESS);
	}
}

/*
 * Used instead of clnt_call by rtableX_clnt.c
 *
 * Might need locking for the client handle here since
 * it might be purged if something's wrong
 */
extern enum clnt_stat
_DtCm_clnt_call(
	_DtCm_Connection *conn,
	u_long proc,
	xdrproc_t inproc,
	caddr_t in,
	xdrproc_t outproc,
	caddr_t out,
	struct timeval tout)
{
	_DtCm_Client_Info	*ci;
	_DtCm_Transport_Type	ttype;
	enum clnt_stat status = RPC_FAILED;
	int retry = conn->retry;

	while (B_TRUE) {
		if (conn->ci == NULL)
			break;
		else {
			ci = conn->ci;
			ci->last_used = time(0);
		}

		if (conn->use == udp_transport || ci->tcpcl == NULL)
			ttype = udp_transport;
		else
			ttype = tcp_transport;

		status = clnt_call((ttype == tcp_transport ? ci->tcpcl :
				ci->udpcl), proc, inproc, in,
				outproc, out, tout);

		if ((ttype == udp_transport && status == RPC_TIMEDOUT) ||
		     (status == RPC_CANTRECV)) {

			if (retry) {
				retry--;

				/* don't retry when stat is RPC_TIMEDOUT
				 * and transpart is tcp since if the server
				 * is down, stat would be something else
				 * like RPC_CANTRECV
				 */

				/* get new client handle */
				if (get_new_client_handle(conn) == NULL)
					break;

			} else {
				/* purge the client handle */
				delete_client_info(conn->ci);
				conn->ci = NULL;
				break;
			}
		} else
			break;
	}

	if (status != RPC_SUCCESS && conn->ci != NULL) {
		_DtCm_print_errmsg(clnt_sperror((ttype == tcp_transport ? ci->tcpcl :
				ci->udpcl), ci->host));
	}
	conn->stat = status;
	return status;
}

extern CSA_return_code
_DtCm_add_registration(
	_DtCm_Client_Info *ci,
	char *cal,
	unsigned long update_type)
{
	_DtCm_Target_List *listp, *prev;
	_DtCm_Target_List *listitem;
	int result;

	if (ci == NULL || cal == NULL)
		return (CSA_E_INVALID_PARAMETER);

	for (listp = prev = ci->tlist; listp != NULL;
	     prev = listp, listp = listp->next) {
		if ((result = strcmp(listp->cal, cal)) == 0) {
			/* registered already */
			return (CSA_SUCCESS);
		} else if (result > 0)
			break;
	}

	/* register the first time, insert in list in ascending order */
	if ((listitem = (_DtCm_Target_List *)calloc(1, sizeof(_DtCm_Target_List))) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	if ((listitem->cal = strdup(cal)) == NULL) {
		free(listitem);
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	listitem->update_type = update_type;

	if (prev == NULL || listp == prev)
		ci->tlist = listitem;
	else
		prev->next = listitem;
	listitem->next = listp;

	ci->nregistered++;

	return (CSA_SUCCESS);
}

extern void
_DtCm_remove_registration(_DtCm_Client_Info *ci, char *cal)
{
	_DtCm_Target_List *listp, *prev;
	_DtCm_Client_Info *c;
	int result;

	if (cal == NULL) return;

	/* if found, just increment the number of registration */
	for (listp = prev = ci->tlist; listp != NULL;
	     prev = listp, listp = listp->next) {
		if ((result = strcmp(listp->cal, cal)) == 0) {
			if (listp == prev)
				ci->tlist = listp->next;
			else
				prev->next = listp->next;

			/* free target item */
			free(listp->cal);
			free(listp);

			/*
			 * if no calendar is registered, close tcp connection
			 */
			if (--(ci->nregistered) == 0) {
				if (ci->tcpcl) {
					destroy_auth(ci->tcpcl);
					clnt_destroy(ci->tcpcl);
					ci->tcpcl = NULL;
					tcp_count--;
				}

				/* find other tcp connection for the
				 * same host
				 */
				for (c = client_cache_head; c != NULL;
				    c = c->next) {
					if ((result = strcmp(c->host,
					    ci->host)) == 0) {
						if (c->nregistered == 0 &&
						    c->tcpcl) {
							destroy_auth(c->tcpcl);
							clnt_destroy(c->tcpcl);
							c->tcpcl = NULL;
							tcp_count--;
						}
					} else if (result > 0)
						break;
				}
			}
			return;
		} else if (result > 0)
			break;
	}
	/* not found; impossible */
}

extern CSA_return_code
_DtCm_get_server_rpc_version(char *host, int *version)
{
	CSA_return_code stat;
	_DtCm_Client_Info *ci;

	if (host == NULL) {
		return (CSA_E_INVALID_PARAMETER);
	}

	if ((stat = _DtCm_create_tcp_client(host, TABLEVERS,
	    _DtCM_INITIAL_TIMEOUT, &ci)) == CSA_SUCCESS)
		*version = ci->vers_out;

	return (stat);
}

extern CSA_return_code
_DtCm_clntstat_to_csastat(enum clnt_stat clntstat)
{
	switch (clntstat) {
#if defined(SunOS) || defined(USL) || defined(__uxp__)
	case RPC_N2AXLATEFAILURE:
#endif
	case RPC_UNKNOWNHOST:
		return (CSA_X_DT_E_INVALID_SERVER_LOCATION);
	case RPC_PROGNOTREGISTERED:
		return (CSA_X_DT_E_SERVICE_NOT_REGISTERED);
	case RPC_TIMEDOUT:
		return (CSA_X_DT_E_SERVER_TIMEOUT);
	default:
		return (CSA_E_SERVICE_UNAVAILABLE);
	}
}

/*****************************************************************************
 * static functions used within the file
 *****************************************************************************/

static void
create_auth(CLIENT *cl)
{
	/* Always cache the Unix style credentials. */
	if (unix_credential == NULL)
#if defined(SunOS) || defined(USL) || defined(__uxp__)
		unix_credential = authsys_create_default ();
#else
		unix_credential = authunix_create_default ();
#endif

	cl->cl_auth = unix_credential;
}

static void
destroy_auth(CLIENT *cl)
{
	/* It is a no-op for unix-authentication because we always cache it.
	 * But we have to destroy it when secure RPC is used.
	 */
}

/*
 * Given a host name, find the _DtCm_Client_Info structure which contains
 * both udp and tcp handle to the server running in the host.
 */
static _DtCm_Client_Info *
get_client_info(char *host, int version)
{
	_DtCm_Client_Info *ci;
	int result;

	if (host==NULL) return(NULL);
	for (ci = client_cache_head; ci != NULL; ci = ci->next) {
		if ((result = strcmp(ci->host, host)) == 0) {
			if (ci->vers_out <= version)
				return(ci); 
		} else if (result > 0)
			break;
	}
	return(NULL);
}

static void
destroy_target_list(_DtCm_Target_List *tlist)
{
	_DtCm_Target_List *listp, *listitem;

	for (listp = tlist; listp != NULL; ) {
		listitem = listp;
		listp = listp->next;

		if (listitem->cal)
			free(listitem->cal);
		free(listitem);
	}
}

static void
destroy_client_info(_DtCm_Client_Info *ci)
{
	if (ci==NULL) return;

	if (ci->host != NULL)
		free(ci->host);
	if (ci->tcpcl) {
		destroy_auth(ci->tcpcl);
		clnt_destroy(ci->tcpcl);
		tcp_count--;
	}
	if (ci->udpcl) {
		destroy_auth(ci->udpcl);
		clnt_destroy(ci->udpcl);
	}
	destroy_target_list(ci->tlist);
	free(ci);
	cl_count--;
}

/*
 * Dont limit the number of cached connections right now.
 * Udp client handle does not use up file descriptor only space.
 * Tcp client handle is kept open only when there's at least one
 * calendar registered with the host and the user probably won't
 * be browsing more than 50 calendar at the same time.
 */
static void
insert_client_info(_DtCm_Client_Info *ci)
{
	_DtCm_Client_Info *citem;

	if (++cl_count > MAX_COUNT)
		cleanup_some_connection(ci);

	/* insert new item alphabetically */
	for (citem = client_cache_head; citem != NULL; citem = citem->next) {
		/* there shouldn't be an entry with the same host name
		 * if there's, it would be picked up in get_client_info()
		 */
		if (strcmp(citem->host, ci->host) > 0)
			break;
	}

	if (citem == NULL) {
		if (client_cache_head == NULL)
			client_cache_head = client_cache_tail = ci;
		else {
			ci->prev = client_cache_tail;
			client_cache_tail->next = ci;
			client_cache_tail = ci;
		}
	} else {
		ci->next = citem;
		ci->prev = citem->prev;
		if (citem == client_cache_head)
			client_cache_head = ci;
		else
			citem->prev->next = ci;
		citem->prev = ci;
	}

#ifdef CM_DEBUG
	fprintf(stderr, "%s: head = %d, tail = %d, newitem = %d\n",
		"insert_client_info", client_cache_head,
		client_cache_tail, ci);
	fprintf(stderr, "tcp_count = %d, cl_count = %d\n", tcp_count, cl_count);
#endif

}

/*
 * remove the client info structure from the list
 */
static void
delete_client_info(_DtCm_Client_Info *oldci)
{
	if (oldci == NULL) return;

	if (oldci == client_cache_head) {
		client_cache_head = oldci->next;
		if (client_cache_head)
			client_cache_head->prev = NULL;
	} else if (oldci == client_cache_tail) {
		client_cache_tail = oldci->prev;
		if (client_cache_tail)
			client_cache_tail->next = NULL;
	} else {
		oldci->prev->next = oldci->next;
		oldci->next->prev = oldci->prev;
	}

	if (oldci == client_cache_tail)
		client_cache_tail = NULL;

	destroy_client_info(oldci);

#ifdef CM_DEBUG
	fprintf(stderr, "%s: head = %d, tail = %d, olditem = %d\n",
		"delete_client_info", client_cache_head,
		client_cache_tail, oldci);
#endif
}

/*
 * Number of open tcp connections reaches the maximum.
 * This is very unlikely in the normal case since
 * a tcp connection is kept open if at least one calendar
 * is registered with the host and a user would not be
 * browsing a large number of calendars at one time.
 * However, when a calendar is deselected in the calendar
 * list on the multi-browser window, a lookup call using
 * the tcp connection is made after the calendar is
 * deregistered.  This keeps the tcp connection open
 * even if that's the last calendar registered with the
 * host.  This routine is used to clean up such tcp connections.
 * This is a good time to clean up connections that are not
 * used for a long time.
 */
static void
cleanup_some_connection(_DtCm_Client_Info *dontclose)
{
	_DtCm_Client_Info *ci, *oldci;
	int total = 0, deleted = 0, done = 0;

	for (ci = client_cache_head; ci != NULL; )
	{
		total++;
#ifdef HPUX
		/* clean up whole list */
		if (ci != dontclose && ci->nregistered == 0) {
#else

		if (ci != dontclose && ci->nregistered == 0 &&
		    (ci->tcpcl || (!done && ci->tcpcl == NULL) ||
		     (ci->tcpcl==NULL && (time(NULL) - ci->last_used)>DAYSEC)))
		{
			if (!done) done = 1;
#endif

			deleted++;
			oldci = ci;
			ci = ci->next;
			delete_client_info(oldci);
		} else
			ci = ci->next;
	}
#ifdef CM_DEBUG
	fprintf(stderr, "%s: total = %d, deleted = %d\n",
		"cleanup_tcp_connection", total, deleted);
#endif
}

/*
 * check registration
 * Deergister the first target:
 * if it succeeded, the old server is still running, just re-register it;
 * else assume that it's a new server so re-register the whole list again.
 */
static void
check_registration(_DtCm_Connection *conn)
{
	_DtCm_Target_List *listp, *prev;
	_DtCm_Transport_Type olduse;
	CSA_return_code	stat;

	if (conn->ci->tlist == NULL)
		return;

	olduse = conn->use;
	conn->use = udp_transport;
	conn->retry = B_FALSE;
	if ((stat = _DtCm_do_unregistration(conn, conn->ci->tlist->cal,
	    conn->ci->tlist->update_type)) == CSA_SUCCESS) {
		if (_DtCm_do_registration(conn, conn->ci->tlist->cal,
			conn->ci->tlist->update_type) != CSA_SUCCESS)
		{
			conn->ci->nregistered--;
			listp =  conn->ci->tlist;
			conn->ci->tlist = listp->next;
			free(listp->cal);
			free(listp);
		}
	} else if (stat == CSA_E_CALLBACK_NOT_REGISTERED || stat == CSA_E_FAILURE) {
		for (listp = prev = conn->ci->tlist; listp != NULL; ) {
			if (_DtCm_do_registration(conn, listp->cal,
			    listp->update_type) != CSA_SUCCESS)
			{
				conn->ci->nregistered--;
				if (listp == prev)
					conn->ci->tlist = prev = listp->next;
				else
					prev->next = listp->next;
				/* free target item */
				free(listp->cal);
				free(listp);
				listp = (prev ? prev->next : NULL);
			} else {
				prev = listp;
				listp = listp->next;
			}
		}
	}
	conn->use = olduse;
}

static _DtCm_Client_Info *
get_new_client_handle(_DtCm_Connection *conn)
{
	CLIENT *cl;
	int oldver;

	if (conn == NULL) return(NULL);

	oldver = conn->ci->vers_out;

	/* always get a udp client handle first */
#if defined(SunOS) || defined(USL) || defined(__uxp__)
	cl = clnt_create_vers(conn->ci->host, TABLEPROG, &(conn->ci->vers_out),
			TABLEVERS_2, oldver, "udp");
	if (cl == NULL) {
		_DtCm_print_errmsg(clnt_spcreateerror(conn->ci->host));
	}
#else
	(void) get_client_handle(conn->ci->host, (u_int)TABLEPROG,
			&(conn->ci->vers_out), TABLEVERS_2, oldver,
			"udp", &cl);
#endif

	if (cl == NULL) {
		delete_client_info(conn->ci);
		conn->ci = NULL;
		return(NULL);
	} else {
		create_auth(cl);

		/* adjust timeout */
		timeout_tv.tv_sec = _DtCM_INITIAL_TIMEOUT;
		timeout_tv.tv_usec = 0;
		clnt_control(cl, CLSET_TIMEOUT, (char *)&timeout_tv);
		retry_tv.tv_sec = _DtCM_INITIAL_TIMEOUT + 10;
		retry_tv.tv_usec = 0;
		clnt_control(cl, CLSET_RETRY_TIMEOUT, (char *)&retry_tv);

		destroy_auth(conn->ci->udpcl);
		clnt_destroy(conn->ci->udpcl);
		conn->ci->udpcl = cl;
	}

	/* check registration */
	/* if there's anything wrong, nregistered could be zero */
	check_registration(conn);

	/* ci might be set to NULL if an rpc call failed */
	if (conn->ci == NULL)
		return (NULL);

	/* now deal with tcp handle */

	/* get rid of old handle first */
	if (conn->ci->tcpcl) {
		destroy_auth(conn->ci->tcpcl);
		clnt_destroy(conn->ci->tcpcl);
		tcp_count--;
		conn->ci->tcpcl = NULL;
	}

	if (conn->use == udp_transport) {
		return(conn->ci);
	} else {

		/* get a tcp client handle */
		oldver = conn->ci->vers_out;
#if defined(SunOS) || defined(USL) || defined(__uxp__)
		cl = clnt_create_vers(conn->ci->host, TABLEPROG,
			&(conn->ci->vers_out), TABLEVERS_2, oldver, "tcp");
		if (cl == NULL)
			_DtCm_print_errmsg(clnt_spcreateerror(conn->ci->host));
#else
		(void) get_client_handle(conn->ci->host, (u_int)TABLEPROG,
			&(conn->ci->vers_out), TABLEVERS_2, oldver, "tcp",
			&cl);
#endif

		if (cl == NULL) {
			conn->ci->vers_out = oldver;
			return(NULL);
		} else {
			create_auth(cl);

			/* adjust timeout */
			timeout_tv.tv_sec = _DtCM_INITIAL_TIMEOUT;
			timeout_tv.tv_usec = 0;
			clnt_control(cl, CLSET_TIMEOUT, (char *)&timeout_tv);

			conn->ci->tcpcl = cl;
			tcp_count++;
			return(conn->ci);
		}
	}
}

/*
 * Get a client handle to a server that supports the highest
 * version between the given range.
 */
static CSA_return_code
get_client_handle(
	const char *host,
#ifdef __osf__
	const u_int prognum,
#else
	const u_long prognum,
#endif	
	u_long *vers_outp,
	const u_long vers_low,
	const u_long vers_high,
	char *nettype,
	CLIENT **clnt)
{
	CLIENT	*cl;
	u_int	vers;
	struct timeval tv;
	enum clnt_stat status;

#ifdef HPUX
	static int bumped = 0;
	struct rlimit rl;

	if (bumped == 0) {
		bumped = 1;

		/* raise the soft limit of number of file descriptor */
		getrlimit(RLIMIT_NOFILE, &rl);
		rl.rlim_cur = rl.rlim_max;
		setrlimit(RLIMIT_NOFILE, &rl);
	}
#endif

#ifdef __osf__
	/*
	 * A longer timeout value may be necessay - for example if
	 * the system is bogged down and/or rpc.cmsd is not running.
	 *
	 * The value below is the same value used when a ToolTalk app connects
	 * to the ToolTalk database server (lib/tt/lib/db/tt_db_client.C).
	 */
	tv.tv_sec = 4;
#else
	tv.tv_sec = 1;
#endif
	tv.tv_usec = 0;

	*clnt = NULL;
	for (vers = vers_high; vers >= vers_low; vers--) {
#if defined(__osf__) || defined(__hpux)
	        if ((cl = clnt_create((char *)host, prognum, vers, nettype)) != NULL) {
#else

		if ((cl = clnt_create(host, prognum, vers, nettype)) != NULL) {
#endif
			clnt_control(cl, CLSET_TIMEOUT, (char *)&tv);
			status = clnt_call(cl, 0, (xdrproc_t) xdr_void, 
					   (char *)NULL, (xdrproc_t) xdr_void,
					   (char *)NULL, tv);

			if (status == RPC_SUCCESS) {
				*vers_outp = vers;
				*clnt = cl;
#ifdef __osf__
				/*
				 * Set the timeout back to the original.
				 */
				tv.tv_sec = 1;
				clnt_control(cl, CLSET_TIMEOUT, (char *)&tv);
#endif
				return (CSA_SUCCESS);
			} else if (status != RPC_PROGVERSMISMATCH) {
				return (_DtCm_clntstat_to_csastat(status));
			}
		} else {
			_DtCm_print_errmsg(clnt_spcreateerror((char *) host));
			return (_DtCm_clntstat_to_csastat(rpc_createerr.cf_stat));
		}
	}

	/* cannot find a server that supports a version in the given range */
	/* Probably will never get here */
	return (CSA_E_SERVICE_UNAVAILABLE);
}

static CSA_return_code
regstat4_to_dtcmstatus(Registration_Status_4 stat4)
{
	switch (stat4) {
	case registered_4:
		return (CSA_SUCCESS);

	case deregistered_4:
		return (CSA_SUCCESS);

	case reg_notable_4:
		return (CSA_E_CALENDAR_NOT_EXIST);

	case failed_4:
	case confused_4:
		return (CSA_E_FAILURE);
	}
}

