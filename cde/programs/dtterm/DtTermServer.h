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
/*
 * $XConsortium: DtTermServer.h /main/3 1995/10/31 11:17:48 rswiston $";
 */

/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef	_DtTermServer_h
#define	_DtTermServer_h

#ifdef	__cplusplus
extern "C" {
#endif	/* __cplusplus */

extern char *ServerFailureMessage;
extern int ServerFailureErrno;
extern int InstanceCount;
extern int PingInterval;

extern Boolean ServerStartSession(
    Widget		  topLevel,
    int			  argc,
    char		**argv,
    Boolean		  server,
    char		 *serverId,
    Boolean		  exitOnLastClose,
    Boolean		  block,
    Boolean		  loginShell,
    char		**commandToExec
);

extern void ServerInstanceTerminated(
    Widget		  w
);

#ifdef	__cplusplus
} /* close scope of 'extern "C"'... */
#endif	/* __cplusplus */

/* DON'T ADD ANYTHING AFTER THIS #endif... */
#endif	/* _DtTermServer_h */
