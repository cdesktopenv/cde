/* $XConsortium: main.h /main/5 1996/10/21 15:28:26 mgreess $ */
/*********************************************************************
*  (c) Copyright 1993, 1994 Hewlett-Packard Company	
*  (c) Copyright 1993, 1994 International Business Machines Corp.
*  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
*  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
*      Novell, Inc.
**********************************************************************/
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        main.h
 **
 **   Project:     DT 3.0
 **
 **  This file contains global header file information for dticon
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1992.
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _main_h
#define _main_h


#ifndef NO_MESSAGE_CATALOG
#  ifdef __ultrix
#    define _CLIENT_CAT_NAME "dticon.cat"
#  else
#    define _CLIENT_CAT_NAME "dticon"
#  endif  /* __ultrix */
extern char *_DtGetMessage(char *filename, int set, int n, char *s);
#  define GETSTR(set, number, string) \
          (_DtGetMessage(_CLIENT_CAT_NAME, set, number, string))
#else
#  define GETSTR(set, number, string) string
#endif


/* get a message catlog string and convert it to an XmString */
#define GETXMSTR(set,num,str)	XmStringCreateLocalized (GETSTR(set, num, str))

/* convert a regular string to an XmString */
#define XMSTR(str)		XmStringCreateLocalized (str)


#endif /* _main_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */

