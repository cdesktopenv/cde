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
/* $XConsortium: msgs.h /main/5 1996/09/14 14:51:38 drk $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        msgs.h
 **
 **   Project:     CDE
 **
 **   Description: Public include file for msgs.c
 **
 **
 **   (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992
 **       by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _Dtksh_msgs_h
#define _Dtksh_msgs_h



char * GetSharedMsg(
                    int msgId );



/*
 * macro to get message catalog strings
 */

#ifndef NO_MESSAGE_CATALOG
# define _CLIENT_CAT_NAME "dtksh"

/*
 * Without this proto, standard C says that _DtGetMessage() returns
 * an int, even though it really returns a pointer.  The compiler is
 * then free to use the high 32-bits of the return for
 * something else (like scratch), and that can garble the pointer.
 */
# ifdef _NO_PROTO
extern char *_DtGetMessage();
# else  /* _NO_PROTO */
extern char *_DtGetMessage(char *filename, int set, int n, char *s );
# endif /* _NO_PROTO */

# define GETMESSAGE(set, number, string)\
    (_DtGetMessage(_CLIENT_CAT_NAME, set, number, string))
#else
# define GETMESSAGE(set, number, string)\
    string
#endif


#define DT_BAD_FIELD_NAME           0
#define DT_CONV_BUF_OVFL            1
#define DT_HASHING_FAILURE          2
#define DT_NO_FUNC_NAME             3
#define DT_TK_NOT_INIT              4
#define DT_WIDGET_CREATE_FAILED     5
#define DT_BAD_DISPLAY              6
#define DT_BAD_ATOM                 7
#define DT_UNDEF_RESOURCE           8
#define DT_BAD_POSITION             9
#define DT_BAD_FONT                10
#define DT_BAD_WIDGET_HANDLE       11
#define DT_CMD_WIDGET              12
#define DT_MAIN_WIN_WIDGET         13
#define DT_SCALE_WIDGET            14
#define DT_SCROLLBAR_WIDGET        15
#define DT_TOGGLE_WIDGET           16
#define DT_BAD_WINDOW              17
#define DT_ALLOC_FAILURE           18
#define DT_NO_PARENT               19
#define DT_UNDEF_SYMBOL            20
#define DT_UNDEF_TYPE              21
#define DT_BAD_DECL                22
#define DT_UNKNOWN_CHILD_TYPE      23
#define DT_UNKNOWN_OPTION          24
#define DT_USAGE_WIDGET_POS        25
#define DT_USAGE_WIDGET_TIME       27
#define DT_USAGE_WIDGET            28
#define DT_ERROR                   29
#define DT_WARNING                 30
#define DT_XK_FREE_NO_MEMBER       31
#define DT_XK_PARSE_ERROR          32
#define DT_XK_PARSE_SET_NULL       33
#define DT_XK_PARSE_ARRAY_OVERFLOW 34
#define DT_USAGE_DISPLAY_WINDOW    35
#define DT_USAGE_DISPLAY_WINDOW_VAR 36
#define DT_USAGE_DISPLAY_ROOT_VAR  37
#define DT_BAD_TIMEOUT             38
#define DT_BAD_MESSAGE             39
#define DT_BAD_PATTERN             40

#endif /* _Dtksh_msgs_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
