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
/* $XConsortium: SunDtHelp.c /main/6 1996/08/22 10:39:25 rswiston $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/*
 *+SNOTICE
 *
 *      RESTRICTED CONFIDENTIAL INFORMATION:
 *
 *      The information in this document is subject to special
 *      restrictions in a confidential disclosure agreement bertween
 *      HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *      document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *      Sun's specific written approval.  This documment and all copies
 *      and derivative works thereof must be returned or destroyed at
 *      Sun's request.
 *
 *      Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#include <stdio.h>
#include <dlfcn.h>
#include "Help.h"
#include "DtSvcLock.h"

#pragma weak DtCreateHelpDialog = _DtCreateHelpDialog
#pragma weak DtCreateHelpQuickDialog = _DtCreateHelpQuickDialog
#pragma weak DtHelpQuickDialogGetChild = _DtHelpQuickDialogGetChild
#pragma weak DtHelpReturnSelectedWidgetId = _DtHelpReturnSelectedWidgetId

typedef Widget (*SUNWWidgetProc)();
typedef int (*SUNWIntProc)();

typedef struct _SUNWHelpProcList {
    SUNWWidgetProc	DtCreateHelpDialogSym;
    SUNWWidgetProc	DtCreateHelpQuickDialogSym;
    SUNWWidgetProc	DtHelpQuickDialogGetChildSym;
    SUNWIntProc		DtHelpReturnSelectedWidgetIdSym;
} SUNWHelpProcList;

static SUNWHelpProcList		*pmySUNWProcList = NULL;

int SUNWDtHelpdlopen()
{
    void *libDtHelpHandle = NULL;

    _DtSvcProcessLock();
    pmySUNWProcList = (SUNWHelpProcList *)malloc(sizeof(SUNWHelpProcList));
    libDtHelpHandle = dlopen("libDtHelp.so.2.1", RTLD_LAZY | RTLD_GLOBAL);
    if (libDtHelpHandle == NULL) {
	char *my_err_msg;

	my_err_msg = dlerror();
	printf("%s\n", my_err_msg);
	_DtSvcProcessUnlock();
	return(FALSE);
    }
    pmySUNWProcList->DtCreateHelpDialogSym = (SUNWWidgetProc)
			 dlsym(libDtHelpHandle, "DtCreateHelpDialog");
    pmySUNWProcList->DtCreateHelpQuickDialogSym = (SUNWWidgetProc)
			 dlsym(libDtHelpHandle, "DtCreateHelpQuickDialog");
    pmySUNWProcList->DtHelpQuickDialogGetChildSym = (SUNWWidgetProc)
			 dlsym(libDtHelpHandle, "DtHelpQuickDialogGetChild");
    pmySUNWProcList->DtHelpReturnSelectedWidgetIdSym = (SUNWIntProc)
			 dlsym(libDtHelpHandle, "DtHelpReturnSelectedWidgetId");

    _DtSvcProcessUnlock();
    return(TRUE);
}

Widget _DtCreateHelpDialog(
    Widget parent,
    char *name,
    ArgList al,
    Cardinal ac)
{
    int status;

    _DtSvcProcessLock();
    status = pmySUNWProcList  || SUNWDtHelpdlopen();
    _DtSvcProcessUnlock();

    if (!status)
        return(NULL);

    return ((*pmySUNWProcList->DtCreateHelpDialogSym)(parent, name, al, ac));
}

Widget _DtCreateHelpQuickDialog(
    Widget parent,
    char *name,
    ArgList al,
    Cardinal ac)
{
    int status;

    _DtSvcProcessLock();
    status = pmySUNWProcList  || SUNWDtHelpdlopen();
    _DtSvcProcessUnlock();

    if (!status)
        return(NULL);

    return ((*pmySUNWProcList->DtCreateHelpQuickDialogSym)(parent, name, al, ac));
}

Widget _DtHelpQuickDialogGetChild(
        Widget widget,
        unsigned char child )
{
    int status;

    _DtSvcProcessLock();
    status = pmySUNWProcList  || SUNWDtHelpdlopen();
    _DtSvcProcessUnlock();

    if (!status)
        return(NULL);

    return ((*pmySUNWProcList->DtHelpQuickDialogGetChildSym)(widget, child));
}

int _DtHelpReturnSelectedWidgetId(
    Widget parent,
    Cursor cursor,
    Widget  *widget)
{
    int status;

    _DtSvcProcessLock();
    status = pmySUNWProcList  || SUNWDtHelpdlopen();
    _DtSvcProcessUnlock();

    if (!status)
        return(NULL);

    return ((*pmySUNWProcList->DtHelpReturnSelectedWidgetIdSym)(parent, cursor,
							    widget));
}
