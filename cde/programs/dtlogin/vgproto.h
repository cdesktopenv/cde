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
/* $XConsortium: vgproto.h /main/4 1995/11/21 14:03:25 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        vgproto.h
 **
 **   Project:     HP Visual User Environment (DT)
 **
 **   Description: Communications protocol between Dtlogin and Dtgreet.
 **
 **
 **   (c) Copyright 1987, 1988, 1989 by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _VGREET_H
#define _VGREET_H

#ifdef SIA
#include <siad.h>
#include <X11/Intrinsic.h>
#endif

/***********************************************************************
 *
 * Environment variable definitions
 *
 ***********************************************************************/

#define ERRORLOG	"ERRORLOG"
#define	GRABSERVER	"GRABSERVER"
#define GRABTIMEOUT	"GRABTIMEOUT"
#define LANGLIST	"LANGLIST"
#define LOCATION	"DTXSERVERLOCATION"
#define PINGINTERVAL	"PINGINTERVAL"
#define PINGTIMEOUT	"PINGTIMEOUT"
#define SESSIONTYPE	"SESSIONTYPE"
#define VERIFYNAME	"VERIFYNAME"
#define VN_AFS		"AFS"
#define VN_HPBLS	"HPBLS"
#define VN_KRB		"Kerberos"
#define DTLITE		"DTLITE"
#define SESSION		"SESSION"
#define SESSION_SET	"SESSION_SET"
#define XFORCEINTERNET	"XFORCE_INTERNET"



/***********************************************************************
 *
 * Exit codes from Greeter
 *
 ***********************************************************************/


# define NOTIFY_OK		0
# define NOTIFY_ABORT		1
# define NOTIFY_RESTART		2
# define NOTIFY_ABORT_DISPLAY	3
# define NOTIFY_NO_WINDOWS	4
# define NOTIFY_LANG_CHANGE	5
# define NOTIFY_FAILSAFE	6
# define NOTIFY_PASSWD_EXPIRED	7
# define NOTIFY_DTLITE		8
# define NOTIFY_DT		9
# define NOTIFY_XDM		10
# define NOTIFY_BAD_SECLEVEL	11
# define NOTIFY_LAST_DT		12
# define NOTIFY_ALT_DTS		20

/***********************************************************************
 *
 * Request protocol definitions
 *
 ***********************************************************************/

#define REQUEST_OP_NONE         0
#define REQUEST_OP_EXIT         2
#define REQUEST_OP_MESSAGE      3
#define REQUEST_OP_CHPASS       4
#define REQUEST_OP_CHALLENGE    5
#define REQUEST_OP_LANG         6
#define REQUEST_OP_HOSTNAME     7
#define REQUEST_OP_EXPASSWORD   8
#define REQUEST_OP_CLEAR        9
#ifdef SIA
#define REQUEST_OP_FORM        10
#endif
#define REQUEST_OP_DEBUG        255

#ifdef SIA
#define REQUEST_LIM_MAXLEN      768
#else
#define REQUEST_LIM_MAXLEN      512
#endif
/*
 * Protocol structures. Offsets and lengths are from top of header.
 *
 * Request header.
 */ 
typedef struct {
  unsigned char opcode;         /* REQUEST_OP_* */
  unsigned char reserved;       /* reserved */
  unsigned short length;        /* total length */
  /* op specific data */
} RequestHeader;

typedef struct {
  RequestHeader hdr;
} RequestExit;

typedef struct {
  RequestHeader hdr;
  unsigned short idMC;          /* MC_* message catalog id */
  unsigned short offMessage;   
  /* variable length message string */
} RequestMessage;

typedef struct {
  RequestHeader hdr;
} RequestHostname;

typedef struct {
  RequestHeader hdr;
} RequestExpassword;

typedef struct {
  RequestHeader hdr;
} RequestChpass;

typedef struct {
  RequestHeader hdr;           
  int bEcho;
  unsigned short idMC;          /* MC_* message catalog id */
  unsigned short offChallenge; 
  unsigned short offUserNameSeed; 
  /* variable length challenge string */
  /* variable length user name seed */
} RequestChallenge;

#ifdef SIA

typedef struct {
  RequestHeader hdr;
  short num_prompts;
  Boolean visible[MAX_PROMPTS];  /* MAX_PROMPTS defined in siad.h */
  int rendition;
  unsigned short offTitle;
  unsigned short offPrompts;
  /* variable length title string */
  /* variable length prompt strings */
} RequestForm;

#endif

/***********************************************************************
 *
 * Response protocol definitions
 *
 ***********************************************************************/

/*
 * Response header.
 */
typedef struct {
  unsigned char opcode;         /* REQUEST_OP_* */
  unsigned char reserved;       /* reserved */
  unsigned short length;        /* total length */
  /* op specific data */
} ResponseHeader;

typedef struct {
  ResponseHeader hdr;
} ResponseExit;

typedef struct {
  ResponseHeader hdr;
} ResponseClear;

typedef struct {
  ResponseHeader hdr;
} ResponseMessage;

typedef struct {
  ResponseHeader hdr;
} ResponseHostname;

typedef struct {
  ResponseHeader hdr;
} ResponseExpassword;

typedef struct {
  ResponseHeader hdr;
  unsigned short offOldPassword;
  unsigned short offNewPassword;
  unsigned short offNewPasswordAgain;
  /* variable length old password string */
  /* variable length new password string */
  /* variable length new password string */
} ResponseChpass;

typedef struct {
  ResponseHeader hdr;
  unsigned short offResponse; 
  /* variable length challenge response string */
} ResponseChallenge;

typedef struct {
  ResponseHeader hdr;
  unsigned short offString;
  /* variable length debug string */
} ResponseDebug;

typedef struct {
  ResponseHeader hdr;
  unsigned short offLang;  
  /* variable length lang string */
} ResponseLang;

#ifdef SIA

typedef struct {
  ResponseHeader hdr;
  int collect_status;
  short num_answers;
  unsigned short offAnswers;
  /* variable length answer strings */
} ResponseForm;


typedef struct {
    int num_prompts;
    int rendition;
    int collect_status;
    Boolean visible[MAX_PROMPTS];
    char *title;
    char *prompts[MAX_PROMPTS];
    char *answers[MAX_PROMPTS];
    Widget sia_form_widget;
    Widget answer_widgets[MAX_PROMPTS];
} SiaFormInfo;

#endif

#endif /* _VGREET_H */
/* DON'T ADD STUFF AFTER THIS #endif */
