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
/* $XConsortium: sftmp.c /main/3 1995/11/01 18:38:02 rswiston $ */
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

/*	Create a temporary stream for read/write.
**	The stream is originally created as a memory-resident stream.
**	When this memory is exceeded, a real temp file will be created.
**	The temp file creation sequence is somewhat convoluted so that
**	pool/stack/discipline will work correctly.
**
**	Written by David Korn and Kiem-Phong Vo (12/10/90)
*/

#if !_PACKAGE_ast

#if _lib_unlink
#define remove	unlink
#endif

_BEGIN_EXTERNS_
#if _lib_mktemp
extern char*	mktemp _ARG_((char*));
#else
extern int	remove _ARG_((const char*));
extern int	access _ARG_((char*, int));
#endif
_END_EXTERNS_

#if !_lib_mktemp

static char	**Tmp;
static int	Ntmp, Mtmp;

/* remove all created temp files */
#if __STD_C
static void rmtemp(void)
#else
static void rmtemp()
#endif
{
	reg int	i;
	for(i = 0; i < Ntmp; ++i)
		remove(Tmp[i]);
}

/* make a new temp file name */
#if __STD_C
static char *mktemp(char *name)
#else
static char *mktemp(name)
char	*name;
#endif
{	reg char	*p;

	if(Ntmp == 0)
		atexit(rmtemp);

	if(Ntmp >= Mtmp)
	{	/* space for new entry */
		if(Ntmp <= 0)
			Tmp = (char**)malloc(8*sizeof(char*));
		else	Tmp = (char**)realloc((char*)Tmp,(Mtmp+8)*sizeof(char*));
		if(!Tmp)
		{	Mtmp = Ntmp = 0;
			return NIL(char*);
		}
		else	Mtmp += 8;
	}

	while(access(name,0) == 0)
	{	p = name+7;	/* 7 == strlen("/tmp/sf") */
		while(*p == '9')
			*p++ = '0';
		if(*p == '\0')
			return NIL(char*);
		*p += 1;
	}

	if(!(Tmp[Ntmp] = (char*)malloc(strlen(name)+1)))
		return NIL(char*);
	strcpy(Tmp[Ntmp++],name);

	return name;
}
#endif

#endif /*_PACKAGE_ast*/

#if __STD_C
static int _tmpexcept(Sfio_t* f, int type, Sfdisc_t* disc)
#else
static _tmpexcept(f,type,disc)
Sfio_t*		f;
int		type;
Sfdisc_t*	disc;
#endif
{
	reg char*	file;
	Sfio_t		newf, savf;
	void		(*notifyf)_ARG_((Sfio_t*, int, int));
#if _PACKAGE_ast
	char		Name[128];
#else
#if !_lib_mktemp
	static char	Name[] = "/tmp/sf0000000000000";
#else
	static char	Name[] = "/tmp/sfXXXXXXXX";
#endif
#endif /*_PACKAGE_ast*/

	/* the discipline needs to change only under the following exceptions */
	if(type != SF_WRITE && type != SF_SEEK &&
	   type != SF_DPUSH && type != SF_DPOP && type != SF_DBUFFER)
		return 0;

	/* notify function */
	notifyf = _Sfnotify;

	/* try to create the temp file */
	SFCLEAR(&newf);
	newf.flags = SF_STATIC;
	newf.mode = SF_AVAIL;
#if _PACKAGE_ast
	file = pathtemp(Name,NiL,"sf");
#else
	file = mktemp(Name);
#endif
	/* make sure that the notify function won't be called here since
	   we are only interested in creating the file, not the stream */
	_Sfnotify = 0;
	sfopen(&newf,file,"w+");
	_Sfnotify = notifyf;
	if(newf.file < 0)
		return -1;

#if _PACKAGE_ast || _lib_mktemp
	/* remove the temp file */
	while(remove(file) < 0 && errno == EINTR)
		errno = 0;
#endif

	/* set close-on-exec so temp files are not shared across processes */
	SETCLOEXEC(newf.file);

	if(!disc)
		return newf.file;

	/* now remake the old stream into the new image */
	memcpy((Void_t*)(&savf),(Void_t*)f,sizeof(Sfio_t));
	memcpy((Void_t*)f,(Void_t*)(&newf),sizeof(Sfio_t));
	f->push = savf.push;
	f->pool = savf.pool;

	if(savf.data)
	{	SFSTRSIZE(&savf);
		if(!(savf.flags&SF_MALLOC) )
			(void)sfsetbuf(f,(Void_t*)savf.data,savf.size);
		if(savf.extent > 0)
			(void)sfwrite(f,(Void_t*)savf.data,(int)savf.extent);
		(void)sfseek(f,(long)(savf.next - savf.data),0);
		if((savf.flags&SF_MALLOC) )
			free((Void_t*)savf.data);
	}

	/* announce change of status */
	if(notifyf)
		(*notifyf)(f,SF_NEW,f->file);

	return 1;
}

#if __STD_C
Sfio_t* sftmp(reg int s)
#else
Sfio_t* sftmp(s)
reg int	s;
#endif
{
	reg Sfio_t	*f;
	static Sfdisc_t	Tmpdisc;

	if(s != 0)
	{	/* start with a memory resident stream */
		f = sfnew(NIL(Sfio_t*),NIL(char*),s,-1,SF_STRING|SF_READ|SF_WRITE);

		/* set up a discipline for out-of-bound, etc. */
		if(f && s > 0)
		{	Tmpdisc.exceptf = _tmpexcept;
			f->disc = &Tmpdisc;
		}
	}
	else
	{	/* make a real file stream */
		reg int	file;
		if((file = _tmpexcept(NIL(Sfio_t*),SF_WRITE,NIL(Sfdisc_t*))) < 0)
			return NIL(Sfio_t*);
		if(!(f = sfnew(NIL(Sfio_t*),NIL(char*),-1,file,SF_READ|SF_WRITE)) )
			CLOSE(file);
	}

	return f;
}
