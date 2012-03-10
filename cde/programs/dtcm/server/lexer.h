/* $XConsortium: lexer.h /main/4 1995/11/09 12:45:02 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _LEXER_H
#define _LEXER_H

#include "ansi_c.h"
#include "rtable4.h"

extern int		yyylineno;
extern int		externNumberVal;
extern char		*externQuotedString;
extern Period_4		externPeriod;
extern Tag_4		externTag;
extern Appt_Status_4	externApptStatus;
extern Privacy_Level_4	externPrivacy;

extern void		setinput	P((FILE*));
extern int		yyylex		P(());

#endif
