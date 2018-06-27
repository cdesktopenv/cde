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
/* $XConsortium: vmhdr.h /main/2 1996/05/08 20:03:20 drk $ */
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
#ifndef _VMHDR_H
#define _VMHDR_H	1

/*	Common types, and macros for vmalloc functions.
**
**	Written by (Kiem-)Phong Vo, kpv@research.att.com, 01/16/94.
*/

#if _PACKAGE_ast
#include	<ast.h>
#else
#include	"FEATURE/vmalloc"
#if __STD_C
#include	<stddef.h>
#else
#include	<sys/types.h>
#endif
#endif

#ifdef uchar
#undef uchar
#endif
#ifdef ulong
#undef ulong
#endif
#ifdef ushort
#undef ushort
#endif
#define uchar			unsigned char
#define ushort			unsigned short
#define ulong			unsigned long
typedef union _head_		Head_t;
typedef union _body_		Body_t;
typedef struct _block_		Block_t;
typedef struct _seg_		Seg_t;
typedef struct _pfobj_		Pfobj_t;

#define reg			/*no register keywords*/
#define NOTUSED(x)		(&x,1)

/* convert an address to an integral value */
#define ULONG(addr)	((ulong)((char*)(addr) - (char*)0) )

/* Round x up to a multiple of y. ROUND2 does powers-of-2 and ROUNDX does others */
#define ROUND2(x,y)	(((x) + ((y)-1)) & ~((y)-1))
#define ROUNDX(x,y)	((((x) + ((y)-1)) / (y)) * (y))
#define ROUND(x,y)	(((y)&((y)-1)) ? ROUNDX((x),(y)) : ROUND2((x),(y)) )

/* compute a value that is a common multiple of x and y */
#define MULTIPLE(x,y)	((x)%(y) == 0 ? (x) : (y)%(x) == 0 ? (y) : (y)*(x))

#ifndef DEBUG
#define ASSERT(p)
#define COUNT(n)
#else
#define ASSERT(p)	((p) ? 0 : (abort(),0) )
#define COUNT(n)	((n) += 1)
#endif /*DEBUG*/

#ifndef PAGESIZE
#define PAGESIZE	8192
#endif
#if _lib_getpagesize
#define GETPAGESIZE(x)	((x) ? (0) : ((x) = getpagesize()))
#else
#define GETPAGESIZE(x)	((x) = PAGESIZE)
#endif

/* Blocks are allocated such that their sizes are 0%(BITS+1)
** This frees up enough low order bits to store state information
*/
#define BUSY		(01)	/* block is busy				*/
#define PFREE		(02)	/* preceding block is free			*/
#define JUNK		(04)	/* marked as freed but not yet processed	*/
#define BITS		(07)	/* (BUSY|PFREE|JUNK)				*/
#define ALIGNB		(8)	/* size must be a multiple of BITS+1		*/

#define ISBITS(w)	((w) & BITS)
#define CLRBITS(w)	((w) &= ~BITS)
#define CPYBITS(w,f)	((w) |= ((f)&BITS) )

#define ISBUSY(w)	((w) & BUSY)
#define SETBUSY(w)	((w) |= BUSY)
#define CLRBUSY(w)	((w) &= ~BUSY)

#define ISPFREE(w)	((w) & PFREE)
#define SETPFREE(w)	((w) |= PFREE)
#define CLRPFREE(w)	((w) &= ~PFREE)

#define ISJUNK(w)	((w) & JUNK)
#define SETJUNK(w)	((w) |= JUNK)
#define CLRJUNK(w)	((w) &= ~JUNK)

#define OFFSET(t,e)	((size_t)(&(((t*)0)->e)) )

/* these bits share the "mode" field with the public bits */
#define VM_AGAIN	0010000		/* research the arena for space */
#define VM_LOCK		0020000		/* region is locked		*/
#define VM_LOCAL	0040000		/* local call, bypass lock	*/
#define VM_UNUSED	0104060
#define VMETHOD(vd)	((vd)->mode&VM_METHODS)

/* test/set/clear lock state */
#define SETLOCAL(vd)	((vd)->mode |= VM_LOCAL)
#define GETLOCAL(vd,l)	(((l) = (vd)->mode&VM_LOCAL), ((vd)->mode &= ~VM_LOCAL) )
#define ISLOCK(vd,l)	((l) ? 0 : ((vd)->mode &  VM_LOCK) )
#define SETLOCK(vd,l)	((l) ? 0 : ((vd)->mode |= VM_LOCK) )
#define CLRLOCK(vd,l)	((l) ? 0 : ((vd)->mode &= ~VM_LOCK) )

/* local calls */
#define KPVALLOC(vm,sz,func)		(SETLOCAL((vm)->data), func((vm),(sz)) )
#define KPVALIGN(vm,sz,al,func)		(SETLOCAL((vm)->data), func((vm),(sz),(al)) )
#define KPVFREE(vm,d,func)		(SETLOCAL((vm)->data), func((vm),(d)) )
#define KPVRESIZE(vm,d,sz,fl,func)	(SETLOCAL((vm)->data), func((vm),(d),(sz),(fl)) )
#define KPVADDR(vm,addr,func)		(SETLOCAL((vm)->data), func((vm),(addr)) )

/* ALIGN is chosen so that a block can store all primitive types.
** It should also be a multiple of ALIGNB==(BITS+1) so the size field
** of Block_t will always be 0%(BITS+1) as noted above.
** Of paramount importance is the ALIGNA macro below. If the local compile
** environment is strange enough that the below method does not calculate
** ALIGNA right, then the code below should be commented out and ALIGNA
** redefined to the appropriate requirement.
*/
union _align_
{	char		c, *cp;
	int		i, *ip;
	long		l, *lp;
	double		d, *dp, ***dppp[8];
	size_t		s, *sp;
	void(*		fn)();
	union _align_*	align;
	Head_t*		head;
	Body_t*		body;
	Block_t*	block;
	uchar		a[ALIGNB];
#if _long_double
	long double	ld, *ldp;
#endif
};
struct _a_
{	char		c;
	union _align_	a;
};
#define ALIGNA	(sizeof(struct _a_) - sizeof(union _align_))
struct _align_s
{	char	data[MULTIPLE(ALIGNA,ALIGNB)];
};
#define ALIGN	sizeof(struct _align_s)

/* make sure that the head of a block is a multiple of ALIGN */
struct _head_s
{	union
	{ Seg_t*	seg;	/* the containing segment	*/
	  Block_t*	link;	/* possible link list usage	*/
	  Pfobj_t*	pf;	/* profile structure pointer	*/
	  char*		file;	/* for file name in Vmdebug	*/
	} seg;
	union
	{ size_t	size;	/* size of data area in bytes	*/
	  Block_t*	link;	/* possible link list usage	*/
	  int		line;	/* for line number in Vmdebug	*/
	} size;
};
#define HEADSIZE	ROUND(sizeof(struct _head_s),ALIGN)
union _head_
{	uchar		data[HEADSIZE];	/* to standardize size		*/
	struct _head_s	head;
};
	
/* now make sure that the body of a block is a multiple of ALIGN */
struct _body_s
{	Block_t*	link;	/* next in link list		*/
	Block_t*	left;	/* left child in free tree	*/
	Block_t*	right;	/* right child in free tree	*/
	Block_t**	self;	/* self pointer when free	*/
};
#define BODYSIZE	ROUND(sizeof(struct _body_s),ALIGN)
union _body_
{	uchar		data[BODYSIZE];	/* to standardize size		*/
	struct _body_s	body;
};

/* After all the songs and dances, we should now have:
**	sizeof(Head_t)%ALIGN == 0
**	sizeof(Body_t)%ALIGN == 0
** and	sizeof(Block_t) = sizeof(Head_t)+sizeof(Body_t)
*/
struct _block_
{	Head_t	head;
	Body_t	body;
};

/* requirements for smallest block type */
struct _tiny_s
{	Block_t*	link;
	Block_t*	self;
};
#define TINYSIZE	ROUND(sizeof(struct _tiny_s),ALIGN)
#define S_TINY		7				/* # of tiny blocks	*/
#define MAXTINY		(S_TINY*ALIGN + TINYSIZE)
#define TLEFT(b)	((b)->head.head.seg.link)	/* instead of LEFT	*/
#define TINIEST(b)	(SIZE(b) == TINYSIZE)		/* this type uses TLEFT	*/

#define DIV(x,y)	((y) == 8 ? ((x)>>3) : (x)/(y) )
#define INDEX(s)	DIV((s)-TINYSIZE,ALIGN)

/* number of small block types that can be cached after free */
#define S_CACHE		7
#define MAXCACHE	(S_CACHE*ALIGN + TINYSIZE)
#define C_INDEX(s)	(s < MAXCACHE ? INDEX(s) : S_CACHE)

#define TINY(vd)	((vd)->tiny)
#define CACHE(vd)	((vd)->cache)

typedef struct _vmdata_s
{	int		mode;		/* current mode for region		*/
	size_t		incr;		/* allocate in multiple of this		*/
	size_t		pool;		/* size	of an elt in a Vmpool region	*/
	Seg_t*		seg;		/* list of segments			*/
	Block_t*	free;		/* most recent free block		*/
	Block_t*	wild;		/* wilderness block			*/
	Block_t*	root;		/* root of free tree			*/
	Block_t*	tiny[S_TINY];	/* small blocks				*/
	Block_t*	cache[S_CACHE+1]; /* delayed free blocks		*/
} Vmdata_t;

/* private parts of Vmalloc_t */
#define _VM_PRIVATE_ \
	Vmdisc_t*	disc;		/* discipline to get space		*/ \
	Vmdata_t*	data;		/* the real region data			*/

#include	"vmalloc.h"

/* we don't use these here and they interfere with some local names */
#undef malloc
#undef free
#undef realloc

/* segment structure */
struct _seg_
{	Vmalloc_t*	vm;	/* the region that holds this	*/
	Seg_t*		next;	/* next segment			*/
	Void_t*		addr;	/* starting segment address	*/
	size_t		extent;	/* extent of segment		*/
	uchar*		baddr;	/* bottom of usable memory	*/
	size_t		size;	/* allocable size		*/
	Block_t*	free;	/* recent free blocks		*/
};

/* starting block of a segment */
#define SEGBLOCK(s)	((Block_t*)(((uchar*)(s)) + ROUND(sizeof(Seg_t),ALIGN)))

/* short-hands for block data */
#define SEG(b)		((b)->head.head.seg.seg)
#define SEGLINK(b)	((b)->head.head.seg.link)
#define	SIZE(b)		((b)->head.head.size.size)
#define SIZELINK(b)	((b)->head.head.size.link)
#define LINK(b)		((b)->body.body.link)
#define LEFT(b)		((b)->body.body.left)
#define RIGHT(b)	((b)->body.body.right)
#define VM(b)		(SEG(b)->vm)

#define DATA(b)		((Void_t*)((b)->body.data) )
#define BLOCK(d)	((Block_t*)((char*)(d) - sizeof(Head_t)) )
#define SELF(b)		((Block_t**)((b)->body.data + SIZE(b) - sizeof(Block_t*)) )
#define LAST(b)		(*((Block_t**)(((char*)(b)) - sizeof(Block_t*)) ) )
#define NEXT(b)		((Block_t*)((b)->body.data + SIZE(b)) )

/* functions to manipulate link lists of elts of the same size */
#define SETLINK(b)	(RIGHT(b) =  (b) )
#define ISLINK(b)	(RIGHT(b) == (b) )
#define UNLINK(vd,b,i,t) \
		((((t) = LINK(b)) ? (LEFT(t) = LEFT(b)) : NIL(Block_t*) ), \
		 (((t) = LEFT(b)) ? (LINK(t) = LINK(b)) : (TINY(vd)[i] = LINK(b)) ) )

/* delete a block from a link list or the free tree.
** The test in the below macro is worth scratching your head a bit.
** Even though tiny blocks (size < BODYSIZE) are kept in separate lists,
** only the TINIEST ones require TLEFT(b) for the back link. Since this
** destroys the SEG(b) pointer, it must be carefully restored in bestsearch().
** Other tiny blocks have enough space to use the usual LEFT(b).
** In this case, I have also carefully arranged so that RIGHT(b) and
** SELF(b) can be overlapped and the test ISLINK() will go through.
*/
#define DELETE(vd,b,i,t,func) \
		((!TINIEST(b) && ISLINK(b)) ? UNLINK((vd),(b),(i),(t)) : \
	 		func((vd),SIZE(b),(b)) )

/* see if a block is the wilderness block */
#define SEGWILD(b)	(((b)->body.data+SIZE(b)+sizeof(Head_t)) >= SEG(b)->baddr)
#define VMWILD(vd,b)	(((b)->body.data+SIZE(b)+sizeof(Head_t)) >= vd->seg->baddr)

#define VMFILELINE(f,l)	((f) = _Vmfile, _Vmfile = NIL(char*), (l) = _Vmline, _Vmline = 0)

/* The lay-out of a Vmprofile block is this:
**	seg_ size ----data---- _pf_ size
**	_________ ____________ _________
**	seg_, size: header required by Vmbest.
**	data:	actual data block.
**	_pf_:	pointer to the corresponding Pfobj_t struct
**	size:	the true size of the block.
** So each block requires and extra Head_t.
*/
#define PF_EXTRA	sizeof(Head_t)
#define PFDATA(d)	((Head_t*)((uchar*)(d)+(SIZE(BLOCK(d))&~BITS)-sizeof(Head_t)) )
#define PFOBJ(d)	(PFDATA(d)->head.seg.pf)
#define PFSIZE(d)	(PFDATA(d)->head.size.size)

/* The lay-out of a block allocated by Vmdebug is this:
**	seg_ size file size seg_ magi ----data---- --magi-- magi line
**	--------- --------- --------- ------------ -------- ---------
**	seg_,size: header required by Vmbest management.
**	file:	the file where it was created.
**	size:	the true byte count of the block
**	seg_:	should be the same as the previous seg_.
**		This allows the function vmregion() to work.
**	magi:	magic bytes to detect overwrites.
**	data:	the actual data block.
**	magi:	more magic bytes.
**	line:	the line number in the file where it was created.
** So for each allocated block, we'll need 3 extra Head_t.
*/

/* convenient macros for accessing the above fields */
#define DB_EXTRA	(4*sizeof(Head_t))
#define DBBLOCK(d)	((Block_t*)((uchar*)(d) - 3*sizeof(Head_t)) )
#define DBBSIZE(d)	(SIZE(DBBLOCK(d)) & ~BITS)
#define DBSEG(d)	(((Head_t*)((uchar*)(d) - sizeof(Head_t)))->head.seg.seg )
#define DBSIZE(d)	(((Head_t*)((uchar*)(d) - 2*sizeof(Head_t)))->head.size.size )
#define DBFILE(d)	(((Head_t*)((uchar*)(d) - 2*sizeof(Head_t)))->head.seg.file )
#define DBLN(d)		(((Head_t*)((uchar*)DBBLOCK(d)+DBBSIZE(d)))->head.size.line )
#define DBLINE(d)	(DBLN(d) < 0 ? -DBLN(d) : DBLN(d))

/* forward/backward translation for addresses between Vmbest and Vmdebug */
#define DB2BEST(d)	((uchar*)(d) - 2*sizeof(Head_t))
#define DB2DEBUG(b)	((uchar*)(b) + 2*sizeof(Head_t))

/* set file and line number, note that DBLN > 0 so that DBISBAD will work  */
#define DBSETFL(d,f,l)	(DBFILE(d) = (f), DBLN(d) = (f) ? (l) : 1)

/* set and test the state of known to be corrupted */
#define DBSETBAD(d)	(DBLN(d) > 0 ? (DBLN(d) = -DBLN(d)) : -1)
#define DBISBAD(d)	(DBLN(d) <= 0)

#define DB_MAGIC	0255		/* 10101101	*/

/* compute the bounds of the magic areas */
#define DBHEAD(d,begp,endp) \
		(((begp) = (uchar*)(&DBSEG(d)) + sizeof(Seg_t*)), ((endp) = (d)) )
#define DBTAIL(d,begp,endp) \
		(((begp) = (uchar*)(d) + DBSIZE(d)), ((endp) = (uchar*)(&DBLN(d))) )

/* external symbols for internal use by vmalloc */
typedef Block_t*	(*Vmsearch_f)_ARG_((Vmdata_t*, size_t, Block_t*));
typedef struct _vmextern_
{	Block_t*(*	vm_extend)_ARG_((Vmalloc_t*, size_t, Vmsearch_f ));
	int(*		vm_truncate)_ARG_((Vmalloc_t*, Seg_t*, size_t, int));
	size_t		vm_pagesize;
	char*(*		vm_strcpy)_ARG_((char*, char*, int));
	char*(*		vm_itoa)_ARG_((ulong, int));
	void(*		vm_trace)_ARG_((Vmalloc_t*, uchar*, uchar*, size_t));
	void(*		vm_pfclose)_ARG_((Vmalloc_t*));
} Vmextern_t;

#define _Vmextend	(_Vmextern.vm_extend)
#define _Vmtruncate	(_Vmextern.vm_truncate)
#define _Vmpagesize	(_Vmextern.vm_pagesize)
#define _Vmstrcpy	(_Vmextern.vm_strcpy)
#define _Vmitoa		(_Vmextern.vm_itoa)
#define _Vmtrace	(_Vmextern.vm_trace)
#define _Vmpfclose	(_Vmextern.vm_pfclose)

_BEGIN_EXTERNS_
extern Vmextern_t	_Vmextern;
extern char*		_Vmfile;
extern int		_Vmline;
#ifndef _AIX
extern uchar*		sbrk _ARG_((ssize_t));
extern size_t		getpagesize _ARG_(( ));
#endif
#if !_PACKAGE_ast
extern void		abort _ARG_(( ));
extern int		write _ARG_((int, const char*, int));
extern int		strlen _ARG_((const char*));
extern char*		strcpy _ARG_((char*, const char*));
extern int		strcmp _ARG_((const char*, const char*));
extern Void_t*		memcpy _ARG_((Void_t*, const Void_t*, size_t));
#endif
_END_EXTERNS_

#endif /* _VMHDR_H */
