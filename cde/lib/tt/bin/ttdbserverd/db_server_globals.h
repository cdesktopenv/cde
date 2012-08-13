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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: db_server_globals.h /main/6 1996/05/07 13:51:43 drk $ 			 				 */
/*
 * db_server_globals.h - Declares the global variables that were defined
 *                       in the old DB server and needed by the new DB
 *                       server.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 */

#ifndef _DB_SERVER_GLOBALS_H
#define _DB_SERVER_GLOBALS_H

#include <limits.h>

#if defined(OPT_GARBAGE_THREADS)
#include <synch.h>
#endif // OPT_GARBAGE_THREADS

extern uid_t _tt_uid;
extern gid_t _tt_gid;
extern gid_t _tt_gidlist [NGROUPS];
extern int   _tt_gidlen;
extern int   _tt_auth_level;

#if defined(OPT_GARBAGE_THREADS)
extern mutex_t 	rpc_client_busy; /* Used to sync calls with the RPC clients.*/
extern mutex_t	garbage_run_in_process;

#define		LOCK_RPC()	mutex_lock(&rpc_client_busy);
#define		UNLOCK_RPC()	mutex_unlock(&rpc_client_busy);

#else /* Else if ! defined(OPT_GARBAGE_THREADS) */

#define		LOCK_RPC()	/*EMPTY*/
#define		UNLOCK_RPC()	/*EMPTY*/

extern int	  _tt_garbage_id;
extern char	**global_argv;
extern char	**global_envp;
#endif /* OPT_GARBAGE_THREADS */

#endif /* _DB_SERVER_GLOBALS_H */
