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
/* $XConsortium: connection.h /main/1 1996/04/21 19:22:23 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _CONNECTION_H
#define _CONNECTION_H

#include <rpc/rpc.h>
#include "ansi_c.h"
#include "csa.h"

#define	_DtCM_DEFAULT_TIMEOUT	5
#define	_DtCM_INITIAL_TIMEOUT	25
#define	_DtCM_LONG_TIMEOUT	60

#define DAYSEC			86400

typedef enum Transport_type
{
	tcp_transport,
	udp_transport
} _DtCm_Transport_Type;

typedef struct targetlist {
	char		*cal;
	unsigned long	update_type;
	struct targetlist *next;
} _DtCm_Target_List;

typedef struct cl_info {
	char		*host;
	CLIENT		*tcpcl;
	CLIENT		*udpcl;
	u_long		vers_out;
	long		last_used;
	int		nregistered;
	_DtCm_Target_List	*tlist;
	struct cl_info *next;
	struct cl_info *prev;
} _DtCm_Client_Info;

typedef struct conn {
	_DtCm_Client_Info	*ci;
	int		retry;
	_DtCm_Transport_Type	use;
	enum clnt_stat	stat;
} _DtCm_Connection;

#ifdef SunOS
extern enum clnt_stat _DtCm_clnt_call P((
				_DtCm_Connection *conn,
				u_long proc,
				xdrproc_t inproc,
				caddr_t in,
				xdrproc_t outproc,
				caddr_t out,
				struct timeval tout));
#endif

extern CSA_return_code _DtCm_add_registration P((
				_DtCm_Client_Info *ci,
				char *cal,
				unsigned long update_type));

extern void _DtCm_remove_registration P((
				_DtCm_Client_Info *ci,
				char *cal));

extern CSA_return_code _DtCm_create_udp_client P((
				char *host,
				int version,
				int timeout,
				_DtCm_Client_Info **clnt));

extern CSA_return_code _DtCm_create_tcp_client P((
				char *host,
				int version,
				int timeout,
				_DtCm_Client_Info **clnt));

extern CSA_return_code _DtCm_get_server_rpc_version P((char *host, int *vers));

extern CSA_return_code _DtCm_clntstat_to_csastat P((enum clnt_stat clntstat));

#endif

