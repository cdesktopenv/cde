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
/* $XConsortium: malloc.c /main/2 1996/05/08 20:00:43 drk $ */
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
#include	<ast_lib.h>

#if _std_malloc || _INSTRUMENT_ || cray

int	_STUB_malloc;

#else

#include	"vmhdr.h"

/*	malloc compatibility functions.
**	These are aware of debugging/profiling and driven by the environment variables:
**	VMETHOD: select an allocation method by name.
**	VMPROFILE: if is a file name, profile memory usage and write output to it.
**	VMDEBUG:
**		a:	abort on any warning
**		#:	period to check arena.
**		0x#:	address to watch.
**	VMTRACE: if is a file name, turn on tracing and output
**		trace data to the given file.
**
**	Written by (Kiem-)Phong Vo, kpv@research.att.com, 01/16/94.
*/

#if !_PACKAGE_ast
_BEGIN_EXTERNS_
extern int	atexit _ARG_(( void(*)() ));
extern char*	getenv _ARG_(( const char* ));
extern int	creat _ARG_(( const char*, int ));
extern int	close _ARG_(( int ));
extern int	getpid _ARG_(( void ));
_END_EXTERNS_
#endif

#ifdef malloc
#undef malloc
#endif
#ifdef free
#undef free
#endif
#ifdef realloc
#undef realloc
#endif
#ifdef calloc
#undef calloc
#endif
#ifdef cfree
#undef cfree
#endif
#ifdef memalign
#undef memalign
#endif
#ifdef valloc
#undef valloc
#endif

#if __STD_C
static ulong atou(char** sp)
#else
static ulong atou(sp)
char**	sp;
#endif
{
	char*	s = *sp;
	ulong	v = 0;

	if(s[0] == '0' && (s[1] == 'x' || s[1] == 'X') )
	{	for(s += 2; *s; ++s)
		{	if(*s >= '0' && *s <= '9')
				v = (v << 4) + (*s - '0');
			else if(*s >= 'a' && *s <= 'f')
				v = (v << 4) + (*s - 'a') + 10;
			else if(*s >= 'A' && *s <= 'F')
				v = (v << 4) + (*s - 'A') + 10;
			else break;
		}
	}
	else
	{	for(; *s; ++s)
		{	if(*s >= '0' && *s <= '9')
				v = v*10 + (*s - '0');
			else break;
		}
	}

	*sp = s;
	return v;
}

static int		_Vmflinit = 0;
static ulong		_Vmdbcheck = 0;
static ulong		_Vmdbtime = 0;
static int		_Vmpffd = -1;
#define VMFLINIT() \
	{ if(!_Vmflinit)	vmflinit(); \
	  if(_Vmdbcheck && (_Vmdbtime++ % _Vmdbcheck) == 0 && \
	     Vmregion->meth.meth == VM_MTDEBUG) \
		vmdbcheck(Vmregion); \
	}

#if __STD_C
static char* insertpid(char* begs, char* ends)
#else
static char* insertpid(begs,ends)
char*	begs;
char*	ends;
#endif
{	int	pid;
	char*	s;

	if((pid = getpid()) < 0)
		return NIL(char*);

	s = ends;
	do
	{	if(s == begs)
			return NIL(char*);
		*--s = '0' + pid%10;
	} while((pid /= 10) > 0);
	while(s < ends)
		*begs++ = *s++;

	return begs;
}

#if __STD_C
static int createfile(char* file)
#else
static createfile(file)
char*	file;
#endif
{
	char	buf[1024];
	char	*next, *endb;

	next = buf;
	endb = buf + sizeof(buf);
	while(*file)
	{	if(*file == '%')
		{	switch(file[1])
			{
			case 'p' :
				if(!(next = insertpid(next,endb)) )
					return -1;
				file += 2;
				break;
			default :
				goto copy;
			}
		}
		else
		{ copy:
			*next++ = *file++;
		}

		if(next >= endb)
			return -1;
	}

	*next = '\0';
	return creat(buf,0644);
}

static void pfprint()
{
	if(Vmregion->meth.meth == VM_MTPROFILE)
		vmprofile(Vmregion,_Vmpffd);
}

static int vmflinit()
{
	char*		env;
	Vmalloc_t*	vm;
	int		fd;
	ulong		addr;

	/* this must be done now to avoid any inadvertent recursion (more below) */
	_Vmflinit = 1;

	/* if getenv() calls malloc(), this may not be caught by the eventual region */
	vm = NIL(Vmalloc_t*);
	if((env = getenv("VMETHOD")) )
	{	if(strcmp(env,"Vmdebug") == 0 || strcmp(env,"vmdebug") == 0)
			vm = vmopen(Vmdcsbrk,Vmdebug,0);
		else if(strcmp(env,"Vmprofile") == 0 || strcmp(env,"vmprofile") == 0 )
			vm = vmopen(Vmdcsbrk,Vmprofile,0);
		else if(strcmp(env,"Vmlast") == 0 || strcmp(env,"vmlast") == 0 )
			vm = vmopen(Vmdcsbrk,Vmlast,0);
		else if(strcmp(env,"Vmpool") == 0 || strcmp(env,"vmpool") == 0 )
			vm = vmopen(Vmdcsbrk,Vmpool,0);
		else if(strcmp(env,"Vmbest") == 0 || strcmp(env,"vmbest") == 0 )
			vm = Vmheap;
	}

	if((!vm || vm->meth.meth == VM_MTDEBUG) &&
	   (env = getenv("VMDEBUG")) && env[0] )
	{	if(vm || (vm = vmopen(Vmdcsbrk,Vmdebug,0)) )
		{	reg int	setcheck = 0;

			while(*env)
			{	if(*env == 'a')
					vmset(vm,VM_DBABORT,1);

				if(*env < '0' || *env > '9')
					env += 1;
				else if(env[0] == '0' &&
					(env[1] == 'x' || env[1] == 'X') )
				{	if((addr = atou(&env)) != 0)
						vmdbwatch((Void_t*)addr);
				}
				else
				{	_Vmdbcheck = atou(&env);
					setcheck = 1;
				}
			}
			if(!setcheck)
				_Vmdbcheck = 1;
		}
	}

	if((!vm || vm->meth.meth == VM_MTPROFILE) &&
	   (env = getenv("VMPROFILE")) && env[0] )
	{	_Vmpffd = createfile(env);
		if(!vm)
			vm = vmopen(Vmdcsbrk,Vmprofile,0);
	}

	/* slip in the new region now so that malloc() will work fine */
	if(vm)
		Vmregion = vm;

	/* turn on tracing if requested */
	if((env = getenv("VMTRACE")) && env[0] && (fd = createfile(env)) >= 0)
	{	vmset(Vmregion,VM_TRACE,1);
		vmtrace(fd);
	}

	/* make sure that profile data is output upon exiting */
	if(vm && vm->meth.meth == VM_MTPROFILE)
	{	if(_Vmpffd < 0)
			_Vmpffd = 2;
		/* this may wind up calling malloc(), but region is ok now */
		atexit(pfprint);
	}
	else if(_Vmpffd >= 0)
	{	close(_Vmpffd);
		_Vmpffd = -1;
	}

	return 0;
}

#if __STD_C
Void_t* malloc(reg size_t size)
#else
Void_t* malloc(size)
reg size_t	size;
#endif
{
	VMFLINIT();
	return (*Vmregion->meth.allocf)(Vmregion,size);
}

#if __STD_C
Void_t* realloc(reg Void_t* data, reg size_t size)
#else
Void_t* realloc(data,size)
reg Void_t*	data;	/* block to be reallocated	*/
reg size_t	size;	/* new size			*/
#endif
{
	VMFLINIT();
	return (*Vmregion->meth.resizef)(Vmregion,data,size,VM_RSCOPY|VM_RSFREE);
}

#if __STD_C
void free(reg Void_t* data)
#else
void free(data)
reg Void_t*	data;
#endif
{
	VMFLINIT();
	(void)(*Vmregion->meth.freef)(Vmregion,data);
}

#if __STD_C
Void_t* calloc(reg size_t n_obj, reg size_t s_obj)
#else
Void_t* calloc(n_obj, s_obj)
reg size_t	n_obj;
reg size_t	s_obj;
#endif
{
	reg int*	di;
	reg char*	dc;
	reg Void_t*	data;

	VMFLINIT();
	if(!(data = (*Vmregion->meth.allocf)(Vmregion,n_obj*s_obj)) )
		return NIL(Void_t*);

	n_obj *= s_obj;
	s_obj  = n_obj/sizeof(int);
	n_obj -= s_obj*sizeof(int);
	for(di = (int*)data; s_obj > 0; --s_obj )
		*di++ = 0;
	for(dc = (char*)di; n_obj > 0; --n_obj )
		*dc++ = 0;

	return data;
}

#if __STD_C
void cfree(reg Void_t* data)
#else
void cfree(data)
reg Void_t*	data;
#endif
{
	VMFLINIT();
	(void)(*Vmregion->meth.freef)(Vmregion,data);
}

#if __STD_C
Void_t* memalign(reg size_t align, reg size_t size)
#else
Void_t* memalign(align, size)
reg size_t	align;
reg size_t	size;
#endif
{
	VMFLINIT();
	return (*Vmregion->meth.alignf)(Vmregion,size,align);
}

#if __STD_C
Void_t* valloc(reg size_t size)
#else
Void_t* valloc(size)
reg size_t	size;
#endif
{
	VMFLINIT();
	GETPAGESIZE(_Vmpagesize);
	return (*Vmregion->meth.alignf)(Vmregion,size,_Vmpagesize);
}

/* the below collection of functions are aware of files and line numbers */
#define VMSETFL(f,l)	(_Vmfile = (f), _Vmline = (l))
#if __STD_C
Void_t* _vmflmalloc(size_t size, char* file, int line)
#else
Void_t* _vmflmalloc(size, file, line)
size_t	size;
char*	file;
int	line;
#endif
{
	VMFLINIT();
	VMSETFL(file,line);
	return malloc(size);
}

#if __STD_C
Void_t* _vmflrealloc(Void_t* data, size_t size, char* file, int line)
#else
Void_t* _vmflrealloc(data,size,file,line)
Void_t*	data;
size_t	size;
char*	file;
int	line;
#endif
{
	VMFLINIT();
	VMSETFL(file,line);
	return realloc(data,size);
}

#if __STD_C
void _vmflfree(Void_t* data, char* file, int line)
#else
void _vmflfree(data,file,line)
Void_t*	data;
char*	file;
int	line;
#endif
{
	VMFLINIT();
	VMSETFL(file,line);
	free(data);
}

#if __STD_C
Void_t* _vmflcalloc(size_t n_obj, size_t s_obj, char* file, int line)
#else
Void_t* _vmflcalloc(n_obj, s_obj, file, line)
size_t	n_obj;
size_t	s_obj;
char*	file;
int	line;
#endif
{
	VMFLINIT();
	VMSETFL(file,line);
	return calloc(n_obj, s_obj);
}

#if __STD_C
Void_t* _vmflmemalign(size_t align, size_t size, char* file, int line)
#else
Void_t* _vmflmemalign(align, size, file, line)
size_t	align;
size_t	size;
char*	file;
int	line;
#endif
{
	VMFLINIT();
	VMSETFL(file,line);
	return memalign(size,align);
}

#if __STD_C
Void_t* _vmflvalloc(size_t size, char* file, int line)
#else
Void_t* _vmflvalloc(size, file, line)
size_t	size;
char*	file;
int	line;
#endif
{
	VMFLINIT();
	VMSETFL(file,line);
	return valloc(size);
}

#endif
