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
/* $XConsortium: I18nEnv.h /main/1 1996/03/25 00:51:43 pascale $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        I18nEnv.h
 **
 **
 **  This file contains function definitions for the corresponding .c
 **  file
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#ifndef _i18nEnv_h
#define _i18nEnv_h

#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netdb.h>

#ifndef	MAXPATHLEN
#define	MAXPATHLEN		1024
#endif

#define DtUSER_TMP_DIR          "./dt/tmp"
#define DtUSER_IMSFS_DIR        "/.dt/ims"

#define	NAME_NONE		"none"
#define	NAME_LOCAL		"local"
#define	NAME_BUILTIN		"builtin"

#define	STR_PREFIX_CHAR		'@'
#define	STR_DEFAULTIMS		"DefaultIms"
#define	STR_SELECTMODE		"SelectMode"
#define	STR_IMSNAME		"ImsName"
#define	STR_HOSTNAME		"HostName"
#define  TAG_END_CHAR           ':'

    /* host type */
#define	HOST_LOCAL		0
#define	HOST_REMOTE		1
#define	HOST_UNKNOWN		2

typedef struct _FileSel {
    char        *fname;         /* IM selection file full path name */
    char        *hostname;      /* User selected hostname */
    char        *im_name;       /* User selected Input Method */
    int         start_mode;     /* User selected start mode */
} FileSel;

typedef struct {
    int         inactive;       /* Status of the IM */
    char	*im_name;       /* File name of the IM as found in the locale
				   entry file */
    char	*im_label;      /* Label to present to the user */
    Boolean     im_default;     /* Is this the default one? */
} ImsEnt;


typedef struct {
    char       *host_name;       /* hostname selected in the ComboBox */
    ImsEnt     *ims_list ;       /* list of IMS entities on this host */
    int        ims_list_size ;   /* number of IMS entities */
} ImsSel;

typedef struct _UserEnv {
    char	*localhostname;   /* hostname the user has started dtstyle */
    char	*username;        /* user name */
    char        *displayname;     /* display dtstyle is running on */
    char	*locale;          /* system-specific locale from $LANG */
    char        *CDE_locale;      /* CDE generic locale name */
    char        *homedir;         
} UserEnv;

typedef struct _PipeRec {
    FILE  *pipe ;
    XtInputId input_id ;
    XtIntervalId timer_id ;
} PipeRec;

typedef struct _I18nEnv {
    UserEnv        *user_env;     /* User environmnet */
    ImsSel         *ims_sel;      /* Available IMS */
    FileSel        *file_sel;     /* User IMS Selection File values */
    PipeRec        *pipe_info;    /* Info needed for the pipe execution */
    Widget         shell;
} I18nEnv;


    /* Error Codes */
#define	NO_ERROR		0

typedef enum {
    NoError = 0,
    ErrNoHome,			
    ErrNoLocale,	
    ErrNoCDELocale,

	/* File I/O */
    ErrFileCreate,		

	/* Selection */
    ErrNoSelectionFile,		
    ErrSaveSelection,		

	/* Remote */
    ErrUnknownHost,		
    ErrNoDtimsstart,	
    ErrRemoteFailed,

        /* Pipe */
    ErrNoPopen,
    ErrTimeOut,
    ErrUnknown,

    LastErrorCode	       
} ximsError;


/* External declarations */
extern I18nEnv i18n_env;

extern int _DtI18nGetEnvValues(I18nEnv *env);
extern int _DtI18nGetImList(I18nEnv *env, char *hostname);
extern void _DtI18nSetSensitiveImTB(I18nEnv *env, Boolean sensitivity);
extern int  _DtI18nWriteImSelectionFile(I18nEnv *env);
extern void _DtI18nErrorDialog(int err) ;

#include "I18nUtil.h"

#endif /* _i18nEnv_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
