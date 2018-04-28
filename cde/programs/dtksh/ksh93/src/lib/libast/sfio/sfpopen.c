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
/* $XConsortium: sfpopen.c /main/3 1995/11/01 18:33:06 rswiston $ */
/***************************************************************
*                                                              *
*                      AT&T - PROPRIETARY                      *
*                                                              *
*         THIS IS PROPRIETARY SOURCE CODE LICENSED BY          *
*                          AT&T CORP.                          *
*                                                              *
*                Copyright (c) 1995 AT&T Corp.                 *
*                     All Rights Reserved                      *
*                                                              *
*           This software is licensed by AT&T Corp.            *
*       under the terms and conditions of the license in       *
*       http://www.research.att.com/orgs/ssr/book/reuse        *
*                                                              *
*               This software was created by the               *
*           Software Engineering Research Department           *
*                    AT&T Bell Laboratories                    *
*                                                              *
*               For further information contact                *
*                     gsf@research.att.com                     *
*                                                              *
***************************************************************/
#include	"sfhdr.h"

#if _PACKAGE_ast

#include	<proc.h>

#else

#if _lib_vfork
#define fork	vfork
#if _hdr_vfork
#include	<vfork.h>
#endif
#if _sys_vfork
#include	<sys/vfork.h>
#endif
#endif

#include	<signal.h>
typedef void(*Handler_t)_ARG_((int));
#define EXIT_NOTFOUND	127

_BEGIN_EXTERNS_
extern int	fork _ARG_((void));
extern int	waitpid _ARG_((int, int*, int));
extern int	wait _ARG_((int*));
extern int	pipe _ARG_((int*));
extern int	execl _ARG_((char*,char*,...));
_END_EXTERNS_

/* pipe ends */
#define READ	0
#define WRITE	1

#endif /*_PACKAGE_ast*/

#if __STD_C
Sfio_t *sfpopen(Sfio_t* f, const char* command, const char* mode)
#else
Sfio_t *sfpopen(f,command,mode)
Sfio_t	*f;
char	*command;	/* command to execute */
char	*mode;		/* mode of the stream */
#endif
{
#if _PACKAGE_ast
	reg Proc_t*	proc;
	reg int		sflags;
	reg long	flags = PROC_IGNORE;
	char*		av[4];

	if (!command || !command[0] || !(sflags = _sftype(mode, NiL)))
		return 0;
	if (sflags & SF_READ)
		flags |= PROC_READ;
	if (sflags & SF_WRITE)
		flags |= PROC_WRITE;
	av[0] = "sh";
	av[1] = "-c";
	av[2] = (char*)command;
	av[3] = 0;
	if (!(proc = procopen(NiL, av, NiL, NiL, flags)))
		return 0;
	if (!(f = sfnew(f, NiL, -1, (sflags&SF_READ) ? proc->rfd : proc->wfd, sflags)) ||
	    _sfpopen(f, (sflags&SF_READ) ? proc->wfd : -1, proc->pid) < 0)
	{
		if (f) sfclose(f);
		procclose(proc);
		return 0;
	}
	procfree(proc);
	return f;
#else
	reg int		pid, pkeep, ckeep, sflags;
	int		parent[2], child[2];
	Sfio_t		sf;
	Handler_t	sig;

	/* sanity check */
	if(!command || !command[0] || !(sflags = _sftype(mode,NIL(int*))))
		return NIL(Sfio_t*);

	/* make pipes */
	parent[0] = parent[1] = child[0] = child[1] = -1;
	if(pipe(parent) < 0)
		goto error;
	if((sflags&SF_RDWR) == SF_RDWR && pipe(child) < 0)
		goto error;

	switch(pid = fork())
	{
	default :	/* in parent process */
		if(sflags&SF_READ)
			{ pkeep = READ; ckeep = WRITE; }
		else	{ pkeep = WRITE; ckeep = READ; }

		/* make the streams */
		if(!(f = sfnew(f,NIL(char*),-1,parent[pkeep],sflags)))
			goto error;
		CLOSE(parent[!pkeep]);

		if((sflags&SF_RDWR) == SF_RDWR)
			CLOSE(child[!ckeep]);

		/* save process info */
		if(_sfpopen(f,(sflags&SF_RDWR) == SF_RDWR ? child[ckeep] : -1,pid) < 0)
		{	(void)sfclose(f);
			goto error;
		}

#ifdef SIGPIPE	/* protect from broken pipe signal */
		if((sflags&SF_WRITE) &&
		   (sig = signal(SIGPIPE,SIG_IGN)) != SIG_DFL && sig != SIG_IGN)
			signal(SIGPIPE,sig);
#endif

		return f;

	case 0 :	/* in child process */
		(void)_sfpclose(NIL(Sfio_t*));

		/* determine what to keep */
		if(sflags&SF_READ)
			{ pkeep = WRITE; ckeep = READ; }
		else	{ pkeep = READ; ckeep = WRITE; }

		/* zap fd that we don't need */
		CLOSE(parent[!pkeep]);
		if((sflags&SF_RDWR) == SF_RDWR)
			CLOSE(child[!ckeep]);

		/* use sfsetfd to make these descriptors the std-ones */
		SFCLEAR(&sf);

		/* must be careful so not to close something useful */
		if((sflags&SF_RDWR) == SF_RDWR && pkeep == child[ckeep])
			if((child[ckeep] = dup(pkeep)) < 0)
				_exit(EXIT_NOTFOUND);

		if(parent[pkeep] != pkeep)
		{	sf.file = parent[pkeep];
			CLOSE(pkeep);
			if(sfsetfd(&sf,pkeep) != pkeep)
				_exit(EXIT_NOTFOUND);
		}

		if((sflags&SF_RDWR) == SF_RDWR && child[ckeep] != ckeep)
		{	sf.file = child[ckeep];
			CLOSE(ckeep);
			if(sfsetfd(&sf,ckeep) != ckeep)
				_exit(EXIT_NOTFOUND);
		}

		/* now exec the command */
		execl("/bin/sh", "sh", "-c", command, NIL(char*));
		_exit(EXIT_NOTFOUND);
		return NIL(Sfio_t*);

	case -1 :	/* error */
	error:
		if(parent[0] >= 0)
			{ CLOSE(parent[0]); CLOSE(parent[1]); }
		if(child[0] >= 0)
			{ CLOSE(child[0]); CLOSE(child[1]); }
		return NIL(Sfio_t*);
	}
#endif /*_PACKAGE_ast*/
}
