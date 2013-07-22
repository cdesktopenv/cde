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
int _DtTermPrimGetPty(char **ptySlave, char **ptyMaster);
void _DtTermPrimReleasePty(char *ptySlave);

#endif	/* _Dt_TermPrimGetPty_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
