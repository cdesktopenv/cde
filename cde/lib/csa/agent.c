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
/* $TOG: agent.c /main/2 1998/03/16 14:42:16 mgreess $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <rpc/rpc.h>
#if !defined(linux) && !defined(CSRG_BASED)
# include <poll.h>
#endif
#if defined(SunOS) || defined(USL) || defined(__uxp__)
#include <netconfig.h>
#include <netdir.h>
#else
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/errno.h>
#endif /* SunOS || USL || __uxp__ */
#include <X11/Intrinsic.h>

#include "agent.h"
#include "cmcb.h"
#include "entry.h"
#include "debug.h"
#include "iso8601.h"
#include "free.h"
#include "misc.h"

/*
 * callback info
 * - contain update information from backend
 */
typedef struct cb_info {
	int	vers;
	char	*cal;
	char	*user;
	int	reason;
	void	*data;
	struct cb_info *next;
} _CallbackInfo;

static _CallbackInfo *cb_head = NULL, *cb_tail = NULL;

u_long	_DtCm_transient = 0;

static u_long	prognum = 0x40000000;
static int	mapped = 0;

/******************************************************************************
 * forward declaration of static functions used within the file
 ******************************************************************************/
#if defined(SunOS) || defined(USL) || defined(__uxp__)
static u_long gettransient (u_long version);
#else
static u_long gettransient (int proto, u_long vers, int *sockp);
#endif
static void _DtCm_handle_callback();
static CSA_return_code _ConvertCallbackData(cmcb_update_callback_args *args,
					_CallbackInfo **cbi);
static CSA_return_code _CopyAttributeNames(uint fnum, char **fnames,
					CSA_uint32 *tnum, char ***tnames);
static boolean_t _ProcessV1Callback(_CallbackInfo *ptr);
static boolean_t _ProcessV2Callback(_CallbackInfo *ptr);
static void _FreeCallbackInfo(_CallbackInfo *ptr);

/*****************************************************************************
 * extern functions used in the library
 *****************************************************************************/

/*
 * Register rpc service for server callback
 */
extern void
_DtCm_init_agent()
{
	int 		s = RPC_ANYSOCK;

#if defined(SunOS) && SunOS < 55 || defined(USL) || defined(__uxp__)
	extern boolean_t rpc_reg(const u_long, const u_long, const u_long,
		const char *(*)(), const xdrproc_t, const xdrproc_t,
		const char *);
#endif

#if defined(SunOS) || defined(USL)|| defined(__uxp__)
	extern void (*sigset(int, void (*)(int)))(int);
#else
	extern void (*sigset())();
#endif

	/* locking candidate for MT-safe purpose */
	if (mapped == 1)
		return;

	DP(("libdtcm: _DtCm_init_agent\n"));

#if defined(SunOS) || defined(USL) || defined(__uxp__)
	(void)rpcb_unset(_DtCm_transient, AGENTVERS, NULL);
	if ((_DtCm_transient = gettransient((u_long)1)) == 0) {
		_DtCm_print_errmsg("Cannot get transient program number\n");
		_DtCm_print_errmsg("Callback cannot be enabled.\n");
		return;
	}
 
	/* register v1 callback */
	if (rpc_reg(_DtCm_transient, AGENTVERS, update_callback,
	    (const char *(*)())_DtCm_update_callback_1, _DtCm_xdr_Table_Res_4,
	    _DtCm_xdr_Update_Status, "udp") == -1) {
		_DtCm_print_errmsg("Cannot register v1 callback handler\n");
		_DtCm_print_errmsg("Callback cannot be enabled.\n");
	}
 
	/* register v2 callback */
	if (rpc_reg(_DtCm_transient, AGENTVERS_2, CMCB_UPDATE_CALLBACK,
	    (const char *(*)())cmcb_update_callback_2_svc,
	    xdr_cmcb_update_callback_args, xdr_void, "udp") == -1) {
		_DtCm_print_errmsg("Cannot register v2 callback handler\n");
		_DtCm_print_errmsg("Callback cannot be enabled.\n");
	}
 
#else
        (void)pmap_unset(_DtCm_transient, AGENTVERS);
        if ((_DtCm_transient = gettransient(IPPROTO_UDP,(u_long)1, &s)) == 0) {
		_DtCm_print_errmsg("Cannot get transient program number\n");
		_DtCm_print_errmsg("Callback cannot be enabled.\n");
		return;
	}
 
	if (registerrpc(_DtCm_transient, AGENTVERS, update_callback,
	    (char *(*)())_DtCm_update_callback_1, (xdrproc_t)_DtCm_xdr_Table_Res_4,
	    (xdrproc_t)_DtCm_xdr_Update_Status) == -1) {
		_DtCm_print_errmsg("Cannot register v1 callback handler\n");
		_DtCm_print_errmsg("Callback cannot be enabled.\n");
	}

	if (registerrpc(_DtCm_transient, AGENTVERS_2, CMCB_UPDATE_CALLBACK,
	    (char *(*)())cmcb_update_callback_2_svc,
	    (xdrproc_t)xdr_cmcb_update_callback_args, (xdrproc_t)xdr_void) == -1) {
		_DtCm_print_errmsg("Cannot register v2 callback handler\n");
		_DtCm_print_errmsg("Callback cannot be enabled.\n");
	}

#endif /* SunOS || USL || __uxp__ */
 
	/* locking candidate for MT-safe purpose */
	mapped = 1;
}
 
/*
 * Unregister with the rpc service.
 */
extern void
_DtCm_destroy_agent()
{
	if (mapped == 0)
		return;

	DP(("libdtcm: _DtCm_destroy_agent\n"));

#if defined(SunOS) || defined(USL) || defined(__uxp__)
        (void) rpcb_unset(_DtCm_transient, AGENTVERS, NULL);
        (void) rpcb_unset(_DtCm_transient, AGENTVERS_2, NULL);
#else
        (void) pmap_unset(_DtCm_transient, AGENTVERS);
        (void) pmap_unset(_DtCm_transient, AGENTVERS_2);
#endif /* SunOS || USL || __uxp__ */

	/* locking candidate for MT-safe purpose */
	mapped = 0;
}

extern void
_DtCm_process_updates()
{
#if defined(CSRG_BASED) || defined(linux) 
        int     i, nfd;
        fd_set  rpc_bits;

        while (B_TRUE) {
          rpc_bits = svc_fdset;

          nfd = select(FD_SETSIZE, &rpc_bits, NULL, NULL, NULL);

          if (nfd <= 0)
            /* done */
            return;

          
          for (i = 0; i < FD_SETSIZE; i++) {
            if (FD_ISSET(i, &rpc_bits)) {
              svc_getreqset(&rpc_bits);
              break;
            }
          }
        }

#else

	int	i, j, nfd;
	fd_set	rpc_bits;
	fd_mask	fmask, *inbits;
	struct	pollfd pfd[FD_SETSIZE];
	struct	pollfd *p;
	int	last;
	int	do_rpc;

	while (B_TRUE) {
		rpc_bits = svc_fdset;

		/* convert to pollfd structure */
		inbits = rpc_bits.fds_bits;
		p = pfd;
		for (i = 0; i < FD_SETSIZE; i += NFDBITS) {
			fmask = *inbits;
			for (j = 0; fmask != 0 ; j++, fmask >>= 1) {
				if (fmask & 0x1) {
					p->fd = i + j;
					if (p->fd >= FD_SETSIZE)
						break;
					p->events = POLLIN;
					p++;
				}
			}
			inbits++;
		}

		/* poll and return right away */
		i = p - pfd;

		nfd = poll(pfd, i, 0);

		if (nfd <= 0)
			/* done */
			return;

		/* if set, handle rpc calls */

		/* convert back to fd_set structure */
		last = -1;
		do_rpc = 0;
		FD_ZERO(&rpc_bits);
		for (p = pfd; i-- > 0; p++) {
			j = p->fd / NFDBITS;
			if (j != last) {
				inbits = &rpc_bits.fds_bits[j];
				last = j;
			}
			if (p->revents & POLLIN) {
				*inbits |= (1 << (p->fd % NFDBITS));
				do_rpc = 1;
			}
		}

		if (do_rpc)
			svc_getreqset(&rpc_bits);
	}
#endif /* CSRG_BASED || linux */
}

/*
 * The server calls this routine when an update event occurs;  
 * It's job is to notify CM asynchronously that an
 * update has occurred.  It has to do it this
 * way (i.e. raise a signal) because the client
 * can't make an rpc call until this procedure call has
 * returned to the server.
 */
 
Update_Status *
_DtCm_update_callback_1(Table_Res_4 *t, _DtCm_Connection *conn)
{
	static Update_Status status = update_succeeded;
	_CallbackInfo *cbi;

	DP(("agent.c: _DtCm_update_callback_1()\n"));

	/*
	 * no point to save data for version 4 and before
	 * since the info from old backends does not contain
	 * calendar info
	 * so we just invoke all registered callback with no data
	 */
	if (cbi = (_CallbackInfo *)calloc(1, sizeof(_CallbackInfo))) {
		cbi->vers = AGENTVERS;

		if (cb_tail == NULL)
			cb_head = cbi;
		else
			cb_tail->next = cbi;

		cb_tail = cbi;
	}

	/* handle callback from backend */
	_DtCm_handle_callback();

	return (&status);
}

/*
 * Handler for v2 callback protocol
 */
void *
cmcb_update_callback_2_svc(cmcb_update_callback_args *args, struct svc_req *d)
{
	_CallbackInfo *cbi;

	DP(("agent.c: cmcb_update_callback_2_svc()\n"));

	if (args == NULL)
		return (NULL);

	if (_ConvertCallbackData(args, &cbi) == CSA_SUCCESS) {
		cbi->vers = AGENTVERS_2;

		if (cb_tail == NULL)
			cb_head = cbi;
		else
			cb_tail->next = cbi;

		cb_tail = cbi;

		/* handle callback from backend */
		_DtCm_handle_callback();
	}

	return (NULL);
}

/******************************************************************************
 * static functions used within in the file
 ******************************************************************************/

/*
 * get transient program number for callbacks.
 */
#if defined(SunOS) || defined(USL) || defined(__uxp__)
static u_long
gettransient (u_long version)
{
	int stat;
	struct nd_hostserv host = {HOST_SELF, "rpcbind"};
	struct nd_addrlist *addrp;

        struct netbuf    *addr;
	struct netconfig *netconf;

	netconf = getnetconfigent("udp");
	if (!netconf) {
		DP(("(gettransient) getnetconfigent(udp) failed\n"));
		freenetconfigent(netconf);
		return 0;
	}

	stat = netdir_getbyname(netconf, &host, &addrp);
	if (stat) {
		DP(("(gettransient) netdir_getbyname failed\n"));
		netdir_free(addrp, ND_ADDRLIST);
		freenetconfigent(netconf);
		return 0;
	}

 	if (addrp->n_cnt < 1) {
		DP(("(gettransient) netdir_getbyname - zero addresses\n"));
		netdir_free(addrp, ND_ADDRLIST);
		freenetconfigent(netconf);
		return 0;
	}

        addr = addrp->n_addrs;

	while (!rpcb_set(prognum++, version, netconf, addr))
	    continue;
	netdir_free(addrp, ND_ADDRLIST);
	freenetconfigent(netconf);

	prognum--;
	return prognum;
}

#else /* SunOS || USL || __uxp__ */

static u_long
gettransient (int proto, u_long vers, int *sockp)
{
        unsigned int len;
	int s, socktype;
	struct sockaddr_in addr;

	switch (proto) {
		case IPPROTO_UDP:
			socktype = SOCK_DGRAM;
			break;
		case IPPROTO_TCP:
			socktype = SOCK_STREAM;
			break;
		default:
			DP(("unknown protocol type\n"));
			return 0;
	}

	if (*sockp == RPC_ANYSOCK) {
		if ((s = socket(AF_INET, socktype, 0)) < 0) {
			perror("socket");
			return 0;
		}
		*sockp = s;
	} else
		s = *sockp;
	
	addr.sin_addr.s_addr = 0;
	addr.sin_family = AF_INET;
	addr.sin_port = 0;
	len = sizeof(addr);

	if (bind(s, (struct sockaddr *)&addr, len) != 0) {
		perror("bind");
		return 0;
	}

	if (getsockname(s, (struct sockaddr *)&addr, &len) < 0) {
		perror("getsockname");
		return 0;
	}

	while (!pmap_set(prognum++, vers, proto, ntohs(addr.sin_port)))
		continue;

	return (prognum-1);
}

#endif /* not SunOS || USL || __uxp__ */

static void
_DtCm_handle_callback()
{
	_CallbackInfo		*ptr, *prev;
	boolean_t		keep = B_FALSE;

	DP(("agent.c: _DtCm_handle_callback()\n"));

	for (ptr = cb_head, prev = NULL; ptr != NULL; ) {
		/* we only handle version 1 and version 2 */
		if (ptr->vers == AGENTVERS)
			keep = _ProcessV1Callback(ptr);
		else
			keep = _ProcessV2Callback(ptr);

		if (!keep) {
			if (prev == NULL)
				cb_head = ptr->next;
			else
				prev->next = ptr->next;

			_FreeCallbackInfo(ptr);
                        ptr = NULL; /* freed by _FreeCallbackInfo() */

		} else {
			prev = ptr;
		}
                if (ptr)
                  ptr = ptr->next;
	}
	cb_tail = prev;
}

static CSA_return_code
_ConvertCallbackData(cmcb_update_callback_args *args, _CallbackInfo **cbi)
{
	_CallbackInfo					*ncbi;
	CSA_calendar_user				*user;
	CSA_logon_callback_data				*ldata;
	CSA_calendar_deleted_callback_data		*rdata;
	CSA_calendar_attr_update_callback_data	*cdata;
	CSA_add_entry_callback_data			*adata;
	CSA_delete_entry_callback_data			*ddata;
	CSA_update_entry_callback_data			*udata;
	char						timebuf[BUFSIZ];
	
	if ((ncbi = (_CallbackInfo *)calloc(1, sizeof(_CallbackInfo))) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	if (args->calendar && (ncbi->cal = strdup(args->calendar)) == NULL) {
		free(ncbi);
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	if (args->user && (ncbi->user = strdup(args->user)) == NULL) {
		_FreeCallbackInfo(ncbi);
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	if ((user = (CSA_calendar_user *)calloc(1, sizeof(CSA_calendar_user)))
	    == NULL) {
		_FreeCallbackInfo(ncbi);
		return (CSA_E_INSUFFICIENT_MEMORY);
	} else
		user->user_name = ncbi->user;

	ncbi->reason = args->data.reason;
	switch (ncbi->reason) {
	case CSA_CB_CALENDAR_LOGON:
		if ((ldata = (CSA_logon_callback_data *)calloc(1,
		    sizeof(CSA_logon_callback_data))) == NULL) {
			free(user);
			_FreeCallbackInfo(ncbi);
			return (CSA_E_INSUFFICIENT_MEMORY);
		}
		ldata->user = user;
		ncbi->data = (void *)ldata;
		break;

	case CSA_CB_CALENDAR_DELETED:
		if ((rdata = (CSA_calendar_deleted_callback_data *)calloc(1,
		    sizeof(CSA_calendar_deleted_callback_data))) == NULL) {
			free(user);
			_FreeCallbackInfo(ncbi);
			return (CSA_E_INSUFFICIENT_MEMORY);
		}
		rdata->user = user;
		ncbi->data = (void *)rdata;
		break;

	case CSA_CB_CALENDAR_ATTRIBUTE_UPDATED:
		if ((cdata = (CSA_calendar_attr_update_callback_data *)
		    calloc(1, sizeof(
		    CSA_calendar_attr_update_callback_data))) == NULL) {
			free(user);
			_FreeCallbackInfo(ncbi);
			return (CSA_E_INSUFFICIENT_MEMORY);
		}
		cdata->user = user;
		ncbi->data = (void *)cdata;

		if (_CopyAttributeNames(args->data.data.cdata->num_names,
		    args->data.data.cdata->names, &cdata->number_attributes,
		    &cdata->attribute_names)) {
			_FreeCallbackInfo(ncbi);
			return (CSA_E_INSUFFICIENT_MEMORY);
		}

		break;

	case CSA_CB_ENTRY_ADDED:
		if ((adata = (CSA_add_entry_callback_data *)calloc(1,
		    sizeof(CSA_add_entry_callback_data))) == NULL) {
			free(user);
			_FreeCallbackInfo(ncbi);
			return (CSA_E_INSUFFICIENT_MEMORY);
		}
		adata->user = user;
		ncbi->data = (void *)adata;

		if (args->data.data.adata->id && (adata->added_entry_id.data =
		    (unsigned char *)strdup(args->data.data.adata->id))
		    == NULL) {
			_FreeCallbackInfo(ncbi);
			return (CSA_E_INSUFFICIENT_MEMORY);
		} else
			adata->added_entry_id.size =
				strlen((char *)adata->added_entry_id.data);

		break;

	case CSA_CB_ENTRY_DELETED:
		if ((ddata = (CSA_delete_entry_callback_data *)calloc(1,
		    sizeof(CSA_delete_entry_callback_data))) == NULL) {
			free(user);
			_FreeCallbackInfo(ncbi);
			return (CSA_E_INSUFFICIENT_MEMORY);
		}
		ddata->user = user;
		ncbi->data = (void *)ddata;

		if (args->data.data.ddata->id && (ddata->deleted_entry_id.data =
		    (unsigned char *)strdup(args->data.data.ddata->id))
		    == NULL) {
			_FreeCallbackInfo(ncbi);
			return (CSA_E_INSUFFICIENT_MEMORY);
		} else
			ddata->deleted_entry_id.size =
				strlen((char *)ddata->deleted_entry_id.data);

		_csa_tick_to_iso8601(args->data.data.ddata->time, timebuf);
		if ((ddata->date_and_time = strdup(timebuf)) == NULL) {
			_FreeCallbackInfo(ncbi);
			return (CSA_E_INSUFFICIENT_MEMORY);
		}

		ddata->scope = args->data.data.ddata->scope;
		break;

	case CSA_CB_ENTRY_UPDATED:
		if ((udata = (CSA_update_entry_callback_data *)calloc(1,
		    sizeof(CSA_update_entry_callback_data))) == NULL) {
			free(user);
			_FreeCallbackInfo(ncbi);
			return (CSA_E_INSUFFICIENT_MEMORY);
		}
		udata->user = user;
		ncbi->data = (void *)udata;

		if (args->data.data.udata->newid && (udata->new_entry_id.data =
		    (unsigned char *)strdup(args->data.data.udata->newid))
		    == NULL) {
			_FreeCallbackInfo(ncbi);
			return (CSA_E_INSUFFICIENT_MEMORY);
		} else
			udata->new_entry_id.size =
				strlen((char *)udata->new_entry_id.data);

		if (args->data.data.udata->oldid && (udata->old_entry_id.data =
		    (unsigned char *)strdup(args->data.data.udata->oldid))
		    == NULL) {
			_FreeCallbackInfo(ncbi);
			return (CSA_E_INSUFFICIENT_MEMORY);
		} else
			udata->old_entry_id.size =
				strlen((char *)udata->old_entry_id.data);

		_csa_tick_to_iso8601(args->data.data.udata->time, timebuf);
		if ((udata->date_and_time = strdup(timebuf)) == NULL) {
			_FreeCallbackInfo(ncbi);
			return (CSA_E_INSUFFICIENT_MEMORY);
		}

		udata->scope = args->data.data.udata->scope;
		break;
	}

	*cbi = ncbi;
	return (CSA_SUCCESS);
}

static CSA_return_code
_CopyAttributeNames(uint fnum, char **fnames, CSA_uint32 *tnum, char ***tnames)
{
	int	i;
	char	**nnames;

	if (fnum == 0) {
		*tnum = 0;
		*tnames = NULL;
		return (CSA_SUCCESS);
	}

	if ((nnames = calloc(1, sizeof(char *) * fnum)) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	for (i = 0; i < fnum; i++) {
		if ((nnames[i] = strdup(fnames[i])) == NULL)
			break;
	}

	if (i == fnum) {
		*tnum = i;
		*tnames = nnames;
		return (CSA_SUCCESS);
	} else {
		 _DtCm_free_character_pointers(i, nnames);
		free(nnames);
		return (CSA_E_INSUFFICIENT_MEMORY);
	}
}

/*
 * pre callback protocol V2, there is no distinction
 * between different reasons, and no data passed.  So
 * there's only one possible thing to do, and that's
 * run all the callbacks.
 */
static boolean_t
_ProcessV1Callback(_CallbackInfo *ptr)
{
	Calendar		*cal;
	_DtCmCallbackEntry	*cb;
	boolean_t		keep = B_FALSE;

	for (cal = _DtCm_active_cal_list; cal != NULL; cal = cal->next) {
		if (cal->rpc_version >= _DtCM_FIRST_EXTENSIBLE_SERVER_VERSION)
			continue;

		if (cal->do_reasons &
		    (CSA_CB_ENTRY_ADDED | CSA_CB_ENTRY_DELETED |
		    CSA_CB_ENTRY_UPDATED))
		{
			/* only do dumb processing if it was a V4 server */
			for (cb = cal->cb_list; cb != NULL; cb = cb->next) {
				if (cal->do_reasons & cb->reason) {
					cb->handler((CSA_session_handle)cal,
						cal->do_reasons & cb->reason,
						(CSA_buffer) NULL,
						cb->client_data,
						(CSA_extension*) NULL);
				}
			}
		} else if (cal->all_reasons & (CSA_CB_ENTRY_ADDED |
		    CSA_CB_ENTRY_DELETED | CSA_CB_ENTRY_UPDATED))
			keep = B_TRUE;
	}

	return (keep);
}

static boolean_t
_ProcessV2Callback(_CallbackInfo *ptr)
{
	Calendar		*cal;
	_DtCmCallbackEntry	*cb;
	boolean_t		keep = B_FALSE;

	for (cal = _DtCm_active_cal_list; cal != NULL; cal = cal->next) {
		if (cal->rpc_version < _DtCM_FIRST_EXTENSIBLE_SERVER_VERSION ||
		    strcmp(ptr->cal, cal->name))
			continue;

		if (cal->do_reasons & ptr->reason) {

			/* only do dumb processing if it was a V4 server */
			for (cb = cal->cb_list; cb != NULL; cb = cb->next) {
				if (ptr->reason & cb->reason) {
					cb->handler((CSA_session_handle)cal,
						ptr->reason,
						(CSA_buffer)ptr->data,
						cb->client_data,
						(CSA_extension*) NULL);
				}
			}
		} else if (cal->all_reasons & ptr->reason)
			keep = B_TRUE;
	}

	return (keep);
}

static void
_FreeCallbackInfo(_CallbackInfo *ptr)
{
	CSA_logon_callback_data				*ldata;
	CSA_calendar_deleted_callback_data		*rdata;
	CSA_calendar_attr_update_callback_data	*cdata;
	CSA_add_entry_callback_data			*adata;
	CSA_delete_entry_callback_data			*ddata;
	CSA_update_entry_callback_data			*udata;

	if (ptr) {
		if (ptr->cal) free(ptr->cal);
		if (ptr->user) free(ptr->user);

		if (ptr->data) switch (ptr->reason) {
		case CSA_CB_CALENDAR_LOGON:
			ldata = ptr->data;
			if (ldata->user) free(ldata->user);
			free(ldata);
			break;
		case CSA_CB_CALENDAR_DELETED:
			rdata = ptr->data;
			if (rdata->user) free(rdata->user);
			free(rdata);
			break;
		case CSA_CB_CALENDAR_ATTRIBUTE_UPDATED:
			cdata = (CSA_calendar_attr_update_callback_data *)
				ptr->data;
			if (cdata->user) free(cdata->user);
			if (cdata->number_attributes > 0)
				_DtCm_free_character_pointers(
					cdata->number_attributes,
					cdata->attribute_names);
			free(cdata);
			break;
		case CSA_CB_ENTRY_ADDED:
			adata = (CSA_add_entry_callback_data *)ptr->data;
			if (adata->user) free(adata->user);
			if (adata->added_entry_id.data)
				free(adata->added_entry_id.data);
			free(adata);
			break;
		case CSA_CB_ENTRY_DELETED:
			ddata = (CSA_delete_entry_callback_data *)ptr->data;
			if (ddata->date_and_time) free(ddata->date_and_time);
			if (ddata->user) free(ddata->user);
			if (ddata->deleted_entry_id.data)
				free(ddata->deleted_entry_id.data);
			free(ddata);
			break;
		case CSA_CB_ENTRY_UPDATED:
			udata = (CSA_update_entry_callback_data *)ptr->data;
			if (udata->user) free(udata->user);
			if (udata->date_and_time) free(udata->date_and_time);
			if (udata->old_entry_id.data)
				free(udata->old_entry_id.data);
			if (udata->new_entry_id.data)
				free(udata->new_entry_id.data);
			free(udata);
		}

		free(ptr);
	}
}


