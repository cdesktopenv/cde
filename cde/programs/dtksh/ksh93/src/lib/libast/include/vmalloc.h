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
/* $XConsortium: vmalloc.h /main/2 1996/05/08 19:44:30 drk $ */
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
#ifndef _VMALLOC_H
#define _VMALLOC_H	1

/*	Public header file for the virtual malloc package.
**	If installation fails because "size_t" is not defined,
**	look for the comments concerning "size_t" below.
**
**	Written by Kiem-Phong Vo, kpv@research.att.com, 01/16/94.
*/

#if _DLL_INDIRECT_DATA && !_DLL
#define _ADR_
#define _PTR_	*
#else
#define _ADR_	&
#define _PTR_
#endif

/* standardize conventions to include ANSI-C and C++ */
#ifndef __KPV__
#define __KPV__		1

#ifndef __STD_C
#ifdef __STDC__
#define	__STD_C		1
#else
#if __cplusplus
#define __STD_C		1
#else
#define __STD_C		0
#endif /*__cplusplus*/
#endif /*__STDC__*/
#endif /*__STD_C*/

#ifndef _BEGIN_EXTERNS_
#if __cplusplus
#define _BEGIN_EXTERNS_	extern "C" {
#define _END_EXTERNS_	}
#else
#define _BEGIN_EXTERNS_
#define _END_EXTERNS_
#endif
#endif /*_BEGIN_EXTERNS_*/

#ifndef _ARG_
#if __STD_C
#define _ARG_(x)	x
#else
#define _ARG_(x)	()
#endif
#endif /*_ARG_*/

#ifndef Void_t
#if __STD_C
#define Void_t		void
#else
#define Void_t		char
#endif
#endif /*Void_t*/

#ifndef NIL
#define NIL(type)	((type)0)
#endif /*NIL*/

#endif /*__KPV__*/

/* The below #includes are to define the type "size_t". If it doesn't work,
** try to figure out what system header file defines "size_t" and include
** that. If none does, then uncomment the below "typedef" statement.
*/
#ifndef	_AST_STD_H
#if __STD_C
#include	<stddef.h>
#else
#include	<sys/types.h>	/* for UNIX systems */
#endif
/* typedef unsigned int		size_t; */
#endif

typedef struct _vmalloc_s	Vmalloc_t;
typedef struct _vmstat_s	Vmstat_t;
typedef struct _vmdisc_s	Vmdisc_t;
typedef struct _vmethod_s	Vmethod_t;
typedef Void_t*	(*Vmemory_f)_ARG_((Vmalloc_t*, Void_t*, size_t, size_t, Vmdisc_t*));
typedef int	(*Vmexcept_f)_ARG_((Vmalloc_t*, int, Void_t*, Vmdisc_t*));

struct _vmstat_s
{	int	n_busy;			/* number of busy blocks	*/
	int	n_free;			/* number of free blocks	*/
	size_t	s_busy;			/* total amount of busy space	*/
	size_t	s_free;			/* total amount of free space	*/
	size_t	m_busy;			/* largest busy piece		*/
	size_t	m_free;			/* largest free piece		*/
	int	n_seg;			/* number of segments		*/
	size_t	extent;			/* total size of region		*/
};

struct _vmdisc_s
{	Vmemory_f	memoryf;	/* memory manipulator		*/
	Vmexcept_f	exceptf;	/* exception handler		*/
	size_t		round;		/* rounding requirement		*/
};

struct _vmethod_s
{	Void_t*		(*allocf)_ARG_((Vmalloc_t*,size_t));
	Void_t*		(*resizef)_ARG_((Vmalloc_t*,Void_t*,size_t,int));
	int		(*freef)_ARG_((Vmalloc_t*,Void_t*));
	long		(*addrf)_ARG_((Vmalloc_t*,Void_t*));
	long		(*sizef)_ARG_((Vmalloc_t*,Void_t*));
	int		(*compactf)_ARG_((Vmalloc_t*));
	Void_t*		(*alignf)_ARG_((Vmalloc_t*,size_t,size_t));
	unsigned short	meth;
};

struct _vmalloc_s
{	Vmethod_t	meth;		/* method for allocation	*/
#ifdef _VM_PRIVATE_
	_VM_PRIVATE_
#endif
};

#define VM_TRUST	0000001		/* forgo some security checks	*/
#define VM_TRACE	0000002		/* generate trace 		*/
#define VM_DBCHECK	0000004		/* check for boundary overwrite	*/
#define VM_DBABORT	0000010		/* abort on any warning		*/
#define VM_FLAGS	0000017		/* user-settable flags		*/

#define VM_MTBEST	0000100		/* Vmbest method		*/
#define VM_MTPOOL	0000200		/* Vmpool method		*/
#define VM_MTLAST	0000400		/* Vmlast method		*/
#define VM_MTDEBUG	0001000		/* Vmdebug method		*/
#define VM_MTPROFILE	0002000		/* Vmdebug method		*/
#define VM_METHODS	0003700		/* available allocation methods	*/

/* resize flags */
#define VM_RSCOPY	01		/* copy old contents		*/
#define VM_RSFREE	02		/* free old contents		*/
#define VM_RSZERO	04		/* set uncopied part to 0	*/

/* exception types */
#define VM_OPEN		0		/* region being opened		*/
#define VM_CLOSE	1		/* region being closed		*/
#define VM_DCCHANGE	2		/* vmdisc() discipline change	*/
#define VM_NOMEM	3		/* can't obtain memory		*/
#define VM_BADADDR	4		/* bad addr in vmfree/vmresize	*/

#define Vmbest		(_ADR_ _Vmbest)
#define Vmlast		(_ADR_ _Vmlast)
#define Vmpool		(_ADR_ _Vmpool)
#define Vmdebug		(_ADR_ _Vmdebug)
#define Vmprofile	(_ADR_ _Vmprofile)

#define Vmdcheap	(_ADR_ _Vmdcheap)
#define Vmdcsbrk	(_ADR_ _Vmdcsbrk)

#define Vmheap		(_ADR_ _Vmheap)

#define Vmregion	(_PTR_ _Vmregion)

_BEGIN_EXTERNS_		/* externally visible declarations */

extern Vmethod_t	_PTR_ _Vmbest;	/* best allocation		*/
extern Vmethod_t	_PTR_ _Vmlast;	/* last-block allocation	*/
extern Vmethod_t	_PTR_ _Vmpool;	/* pool allocation		*/
extern Vmethod_t	_PTR_ _Vmdebug;	/* allocation with debugging	*/
extern Vmethod_t	_PTR_ _Vmprofile;/* profiling memory usage	*/

extern Vmdisc_t		_PTR_ _Vmdcheap;/* heap discipline		*/
extern Vmdisc_t		_PTR_ _Vmdcsbrk;/* sbrk discipline		*/

extern Vmalloc_t	_PTR_ _Vmheap;	/* heap region			*/
extern Vmalloc_t*	Vmregion;	/* current region		*/

extern Vmalloc_t*	vmopen _ARG_(( Vmdisc_t*, Vmethod_t*, int ));
extern int		vmclose _ARG_(( Vmalloc_t* ));
extern int		vmclear _ARG_(( Vmalloc_t* ));
extern int		vmcompact _ARG_(( Vmalloc_t* ));

extern Void_t*		vmalloc _ARG_(( Vmalloc_t*, size_t ));
extern Void_t*		vmalign _ARG_(( Vmalloc_t*, size_t, size_t ));
extern Void_t*		vmresize _ARG_(( Vmalloc_t*, Void_t*, size_t, int ));
extern int		vmfree _ARG_(( Vmalloc_t*, Void_t* ));

extern long		vmaddr _ARG_(( Vmalloc_t*, Void_t* ));
extern long		vmsize _ARG_(( Vmalloc_t*, Void_t* ));

extern Vmalloc_t*	vmregion _ARG_(( Void_t* ));
extern Void_t*		vmsegment _ARG_(( Vmalloc_t*, Void_t* ));
extern int		vmset _ARG_(( Vmalloc_t*, int, int ));
extern Vmdisc_t*	vmdisc _ARG_(( Vmalloc_t*, Vmdisc_t* ));

extern Void_t*		vmdbwatch _ARG_(( Void_t* ));
extern int		vmdbcheck _ARG_(( Vmalloc_t* ));

extern int		vmprofile _ARG_(( Vmalloc_t*, int ));

extern int		vmtrace _ARG_(( int ));

extern int		vmstat _ARG_((Vmalloc_t*, Vmstat_t*));

extern char*		vmstrdup _ARG_((Vmalloc_t*, const char*));

#ifndef	_AST_STD_H

extern Void_t*		malloc _ARG_(( size_t ));
extern Void_t*		realloc _ARG_(( Void_t*, size_t ));
extern void		free _ARG_(( Void_t* ));
extern void		cfree _ARG_(( Void_t* ));
extern Void_t*		calloc _ARG_(( size_t, size_t ));
extern Void_t*		memalign _ARG_(( size_t, size_t ));
extern Void_t*		valloc _ARG_(( size_t ));

#endif

_END_EXTERNS_

#define vmnewof(v,p,t,n,x)	((t*)vmresize(v,p,sizeof(t)*(n)+(x),VM_RSCOPY|VM_RSFREE|VM_RSZERO))
#define vmoldof(v,p,t,n,x)	((t*)vmresize(v,p,sizeof(t)*(n)+(x),VM_RSFREE))

#define vmaddr(vm,addr)		((*((vm)->meth.addrf))((vm),(Void_t*)(addr)) )
#define vmsize(vm,addr)		((*((vm)->meth.sizef))((vm),(Void_t*)(addr)) )
#define vmcompact(vm)		((*((vm)->meth.compactf))((vm)) )

/* enable recording of where a call originates from */
#ifdef VMFL
#ifdef __FILE__
#ifdef __LINE__
#define _Vmfile		(_PTR_ _Vm_file)
#define _Vmline		(_PTR_ _Vm_line)
_BEGIN_EXTERNS_
extern char*	_Vmfile;
extern int	_Vmline;
extern Void_t*	_vmflmalloc _ARG_((size_t, char*, int));
extern Void_t*	_vmflcalloc _ARG_((size_t, size_t, char*, int));
extern Void_t*	_vmflmemalign _ARG_((size_t, size_t, char*, int));
extern Void_t*	_vmflvalloc _ARG_((size_t, char*, int));
extern Void_t*	_vmflrealloc _ARG_((Void_t*, size_t, char*, int));
extern void	_vmflfree _ARG_((Void_t*, char*, int));
_END_EXTERNS_
#define _VMFL_(vm)		(_Vmfile=__FILE__, _Vmline=__LINE__)
#define vmalloc(vm,sz)		(_VMFL_(vm), \
				 (*((vm)->meth.allocf))((vm),(sz)) )
#define vmresize(vm,d,sz,type)	(_VMFL_(vm), \
				 (*((vm)->meth.resizef))((vm),(Void_t*)(d),(sz),(type)) )
#define vmfree(vm,d)		(_VMFL_(vm), \
				 (*((vm)->meth.freef))((vm),(Void_t*)(d)) )
#define vmalign(vm,sz,align)	(_VMFL_(vm), \
				 (*((vm)->meth.alignf))((vm),(sz),(align)) )

#define malloc(s)		_vmflmalloc((size_t)(s), __FILE__, __LINE__)
#define realloc(d,s)		_vmflrealloc((Void_t*)(d),(size_t)(s),__FILE__,__LINE__)
#define calloc(n,s)		_vmflcalloc((size_t)n, (size_t)(s), __FILE__, __LINE__)
#define free(d)			_vmflfree((Void_t*)(d),__FILE__,__LINE__)
#define cfree(d)		_vmflfree((Void_t*)(d),__FILE__,__LINE__)
#define memalign(a,s)		_vmflmemalign((size_t)(a),(size_t)(s),__FILE__,__LINE__)
#define valloc(s)		_vmflvalloc((size_t)(s), __FILE__, __LINE__)
#endif /*__LINE__*/
#endif /*__FILE__*/
#endif /*VMFL*/

/* non-debugging/profiling allocation calls */
#ifndef vmalloc
#define vmalloc(vm,sz)		((*((vm)->meth.allocf))((vm),(sz)) )
#endif
#ifndef vmresize
#define vmresize(vm,d,sz,flags)	((*((vm)->meth.resizef))((vm),(Void_t*)(d),(sz),(flags)) )
#endif
#ifndef vmfree
#define vmfree(vm,d)		((*((vm)->meth.freef))((vm),(Void_t*)(d)) )
#endif
#ifndef vmalign
#define vmalign(vm,sz,align)	((*((vm)->meth.alignf))((vm),(sz),(align)) )
#endif

#endif /* _VMALLOC_H */
