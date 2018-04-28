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
/*
 * $XConsortium: TermPrimSubproc.h /main/3 1996/12/03 12:14:57 drk $";
 */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#ifndef	_Dt_TermPrimSubproc_h
#define	_Dt_TermPrimSubproc_h
typedef void (*_termSubprocProc)(Widget w, pid_t pid, int *stat_loc);
typedef unsigned long _termSubprocId;

extern _termSubprocId _DtTermPrimAddSubproc(Widget		w, 
					    pid_t		pid,
					    _termSubprocProc	proc, 
					    XtPointer		client_data);
extern void _DtTermPrimSetChildSignalHandler(void);
extern void _DtTermPrimSubprocRemoveSubproc(Widget w, _termSubprocId id);

extern pid_t _DtTermPrimSubprocExec(Widget	  w,
				    char	 *ptyName,
				    Boolean	  consoleMode,
				    char	 *cwd,
				    char	 *cmd,
				    char	**argv,
				    Boolean	  loginShell);

#endif	/* _Dt_TermPrimSubproc_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
