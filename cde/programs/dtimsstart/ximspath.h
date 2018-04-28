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
/* @(#)$XConsortium: ximspath.h /main/3 1996/05/07 14:03:38 drk $ */

#ifndef	_XIMSPATH_H_
#define	_XIMSPATH_H_

#include	<sys/param.h>

#ifndef	MAXPATHLEN
#define	MAXPATHLEN		1024
#endif


    /* dtimsstart */
	/* command */
#define	DTIMS_PROGNAME		"dtimsstart"
#define	DTIMS_CLASS		"Dtimsstart"

    /* atom for properties */
#define	DTIMS_ATOM_MAIN		"_DTIMSSTART_MAIN"
#define	DTIMS_ATOM_STATUS	"_DTIMSSTART_STATUS"
#define	DTIMS_ATOM_DATA		"_DTIMSSTART_DATA"

	/* environment */
#define	ENV_DTIMS_STARTCONF	"DTIMS_STARTCONF"
#define	ENV_NO_DTIMSSTART	"NODTIMSSTART"
#define	ENV_XFILESEARCHPATH	"XFILESEARCHPATH"
#define	ENV_XFILESEARCHPATH_STRING \
	"/etc/dt/app-defaults/%L/%N:/etc/dt/app-defaults/C/%N:/usr/dt/app-defaults/%L/%N:/usr/dt/app-defaults/C/%N"
#define	ENV_NLSPATH		"NLSPATH"
#define	ENV_NLSPATH_STRING \
	"/usr/dt/lib/nls/msg/%L/%N.cat:/usr/dt/lib/nls/msg/C/%N.cat"

	/* configuration */
#define	DTIMS_CONFDIR		"/usr/dt/config/ims"
#define	DTIMS_APPDIR		"/usr/dt/app-defaults"
#define	DTIMS_CONFFILE		"start.conf"

	/* path of executable (used for remote execution) */
#define	DTIMS_CMDPATH		"/usr/dt/bin/dtimsstart"

	/* user dir */
#define	DTIMS_IMSDIR		"ims"		/* [%b] */
#define	DTIMS_LOGDIR		"/var/dt"	/* [%G] */
#define	DTIMS_LOGFILE		"imslog"	/* [%g] */

	/* user dir */
#define	DTIMS_USRIMSDIR		"%U/ims"	/* [%S] $HOME/.dt/ims/ */
#define	DTIMS_USRTMPDIR		"%U/tmp"	/* [%T] $HOME/.dt/tmp/ */
#define	DTIMS_USRALTDIR		"/var/tmp"	/* [%A] */
#define	DEFAULT_LOGPATH		"%S/%g"		/* $HOME/.dt/ims/imslog */
#define	ALT_LOGPATH		"%A/%g.%u"	/* /var/tmp/imslog.$USER */
#define	MAX_LOGSIZE		(20 * 1024)	/* 20 KB */

	/* Actions */
#define	NAME_ACT_GETREMCONF	"DtImsGetRemoteConf"
#define	NAME_ACT_RUNREMIMS	"DtImsRunRemoteIms"


    /* IMS */
	/* environment */
#define	ENV_XMODIFIERS		"XMODIFIERS"
#define	ENV_DISPLAY		"DISPLAY"
#define	ENV_LANG		"LANG"
#define	ENV_MOD_IM		"@im="

	/* format of XMODFIERS=@im */
#define	IM_XMOD_XIM		"%n"
#define	IM_XMOD_XIMP		"_XIMP_%L#%n.%s"
#define	IM_XMOD_XSI		"_XIM_INPUTMETHOD"
	/* protocol atom name */
#define	IM_ATOM_XIM		"@server=%N"
#define	IM_ATOM_XIMP		"_XIMP_%L@%n.%s"
#define	IM_ATOM_XSI		"_XIM_INPUTMETHOD"


    /* command path */
#define	SH_PATH			"/usr/bin/sh"
#define	CAT_PATH		"/usr/bin/cat"
#define	XRDB_PATH		"/usr/bin/X11/xrdb"
#define	DTSESSION_RES_PATH	"/usr/dt/bin/dtsession_res"

# ifdef	old_hpux
    /* VUE dirs */
#define	VUE_CONFDIR		"/etc/vue/config"
#define	VUE_USERDIR		"%H/.vue"
# endif	/* old_hpux */

    /* DT dirs */
#define	DT_CONFDIR		"/usr/dt/config"
#define	DT_USERDIR		"%H/.dt"

#endif	/* _XIMSPATH_H_ */
