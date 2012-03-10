/*
 * $XConsortium: TermPrimGetPty.h /main/1 1996/04/21 19:17:42 drk $";
 */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#ifndef	_Dt_TermPrimGetPty_h
#define	_Dt_TermPrimGetPty_h

int _DtTermPrimGetPty(char **ptySlave, char **ptyMaster);
int _DtTermPrimSetupPty(char *ptySlave, int ptyFd);
void _DtTermPrimReleasePty(char *ptySlave);
void _DtTermPrimPtyCleanup();

#endif	/* _Dt_TermPrimGetPty_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
