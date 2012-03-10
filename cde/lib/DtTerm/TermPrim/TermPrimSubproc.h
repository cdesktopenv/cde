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
