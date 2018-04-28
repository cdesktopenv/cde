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
/* $XConsortium: callback.h /main/4 1995/11/09 12:40:25 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _CALLBACK_H
#define _CALLBACK_H

#include "ansi_c.h"
#include "rtable4.h"

typedef struct __DtCmsRegistrationInfo {
	char	*client;
	int	types;
	u_long	prognum;
	u_long	versnum;
	u_long	procnum;
	int	pid;
	struct	__DtCmsRegistrationInfo *next;
} _DtCmsRegistrationInfo;

extern _DtCmsRegistrationInfo *_DtCmsMakeRegistrationInfo P((
				char *client,
				int types,
				u_long prognum,
				u_long versnum,
				u_long procnum,
				int pid));

extern void _DtCmsFreeRegistrationInfo P((_DtCmsRegistrationInfo *w));

extern _DtCmsRegistrationInfo *_DtCmsGetRegistration P((
				_DtCmsRegistrationInfo **rlist,
				char *client,
				u_long prognum,
				u_long versnum,
				u_long procnum,
				int pid));

extern _DtCmsRegistrationInfo *_DtCmsRemoveRegistration P((
				_DtCmsRegistrationInfo *rlist,
				_DtCmsRegistrationInfo *rinfo));

extern _DtCmsRegistrationInfo *_DtCmsCheckRegistrationList P((
				_DtCmsRegistrationInfo *rlist));

extern _DtCmsRegistrationInfo * _DtCmsDoV1CbForV4Data P((
				_DtCmsRegistrationInfo	*rlist,
				char			*source,
				int			pid,
				cms_key			*key1,
				cms_key			*key2));

extern _DtCmsRegistrationInfo *_DtCmsDoV1Callback P((
					_DtCmsRegistrationInfo *rlist,
					char *source,
					int pid,
					Appt_4 *a));

extern _DtCmsRegistrationInfo *_DtCmsDoOpenCalCallback P((
					_DtCmsRegistrationInfo *rlist,
					char *cal,
					char *user,
					int pid));

extern _DtCmsRegistrationInfo *_DtCmsDoRemoveCalCallback P((
					_DtCmsRegistrationInfo *rlist,
					char *cal,
					char *user,
					int pid));

extern _DtCmsRegistrationInfo *_DtCmsDoUpdateCalAttrsCallback P((
					_DtCmsRegistrationInfo	*rlist,
					char			*cal,
					char			*user,
					uint			num_attrs,
					cms_attribute		*attrs,
					int			pid));

extern _DtCmsRegistrationInfo *_DtCmsDoInsertEntryCallback P((
					_DtCmsRegistrationInfo *rlist,
					char *cal,
					char *source,
					long id,
					int pid));

extern _DtCmsRegistrationInfo *_DtCmsDoDeleteEntryCallback P((
					_DtCmsRegistrationInfo *rlist,
					char *cal,
					char *source,
					long id,
					int scope,
					time_t time,
					int pid));

extern _DtCmsRegistrationInfo *_DtCmsDoUpdateEntryCallback P((
					_DtCmsRegistrationInfo *rlist,
					char *cal,
					char *source,
					long newid,
					long oldid,
					int scope,
					long time,
					int pid));

extern void _DtCmsListRegistration P((_DtCmsRegistrationInfo *rlist,
				      char *cal));


#endif
