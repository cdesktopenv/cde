/* 
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2
*/ 
/*   $XConsortium: WmError.h /main/5 1996/09/14 14:49:17 drk $ */
/*
 * (c) Copyright 1987, 1988, 1989, 1990 HEWLETT-PACKARD COMPANY */

#ifdef WSM
#ifdef DEBUGGER
extern void PrintFormatted(char *f, char *s0, char *s1, char *s2, char *s3, char *s4, char *s5, char *s6, char *s7, char *s8, char *s9);
#endif /* DEBUGGER */
#endif /* WSM */
extern void WmInitErrorHandler (Display *display);
extern int WmXErrorHandler (Display *display, XErrorEvent *errorEvent);
extern int WmXIOErrorHandler (Display *display);
extern void WmXtErrorHandler (char *message);
extern void WmXtWarningHandler (char *message);
extern void Warning (char *message);
#ifndef NO_MESSAGE_CATALOG
extern char * GetMessage(int set, int n, char * s);
#endif
#ifdef WSM
/****************************   eof    ***************************/
#endif /* WSM */
