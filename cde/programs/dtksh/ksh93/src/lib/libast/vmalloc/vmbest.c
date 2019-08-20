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
/* $XConsortium: vmbest.c /main/2 1996/05/08 20:01:03 drk $ */
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
#include	"vmhdr.h"

/*	Best-fit allocation method. This is based on a best-fit strategy
**	using a splay tree for storage of lists of free blocks of the same
**	size. Recent free blocks may be cached for fast reuse.
**
**	Written by (Kiem-)Phong Vo, kpv@research.att.com, 01/16/94.
*/

#ifdef DEBUG
static int	N_free;		/* # of free calls			*/
static int	N_alloc;	/* # of alloc calls			*/
static int	N_resize;	/* # of resize calls			*/
static int	N_wild;		/* # allocated from the wild block	*/
static int	N_cache;	/* # allocated from cache		*/
static int	N_last;		/* # allocated from last free block	*/
static int	P_junk;		/* # of semi-free pieces		*/
static int	P_free;		/* # of free pieces			*/
static int	P_busy;		/* # of busy pieces			*/
static size_t	M_junk;		/* max size of a junk piece		*/
static size_t	M_free;		/* max size of a free piece		*/
static size_t	M_busy;		/* max size of a busy piece		*/
static size_t	S_free;		/* total free space			*/
static size_t	S_junk;		/* total junk space			*/
static int	Vmcheck=0;	/* 1 if checking			*/

/* Check to see if a block is in the free tree */
#if __STD_C
static vmintree(Block_t* node, Block_t* b)
#else
static vmintree(node,b)
Block_t*	node;
Block_t*	b;
#endif
{	Block_t*	t;

	for(t = node; t; t = LINK(t))
		if(t == b)
			return 1;
	if(LEFT(node) && vmintree(LEFT(node),b))
		return 1;
	if(RIGHT(node) && vmintree(RIGHT(node),b))
		return 1;
	return 0;
}

/* check to see if the tree is in good shape */
#if __STD_C
static vmchktree(Block_t* node)
#else
static vmchktree(node)
Block_t*	node;
#endif
{	Block_t*	t;

	for(t = LINK(node); t; t = LINK(t))
		/**/ASSERT(SIZE(t) == SIZE(node));
	if((t = LEFT(node)) )
	{	/**/ASSERT(SIZE(t) < SIZE(node));
		vmchktree(t);
	}
	if((t = RIGHT(node)) )
	{	/**/ASSERT(SIZE(t) > SIZE(node));
		vmchktree(t);
	}
	return 1;
}

#if __STD_C
static vmcheck(Vmdata_t* vd, size_t size, int wild)
#else
static vmcheck(vd, size, wild)
Vmdata_t*	vd;
size_t		size;	/* if > 0, checking that no large free block >size	*/
int		wild;	/* if != 0, do above but allow wild to be >size		*/
#endif
{
	reg Seg_t	*seg, *sp;
	reg Block_t	*b, *endb, *t, *np;
	reg size_t	s;

	if(!Vmcheck)
		return 1;

	/**/ASSERT(size <= 0 || !vd->free);
	/**/ASSERT(!vd->root || vmchktree(vd->root));

	P_junk = P_free = P_busy = 0;
	M_junk = M_free = M_busy = S_free = 0;
	for(seg = vd->seg; seg; seg = seg->next)
	{	b = SEGBLOCK(seg);
		endb = (Block_t*)(seg->baddr - sizeof(Head_t));
		while(b < endb )
		{	s = SIZE(b)&~BITS;
			np = (Block_t*)((uchar*)DATA(b) + s);

			if(!ISBUSY(SIZE(b)) )
			{	/**/ ASSERT(!ISJUNK(SIZE(b)));
				/**/ ASSERT(!ISPFREE(SIZE(b)));
				/**/ ASSERT(TINIEST(b) || SEG(b)==seg);
				/**/ ASSERT(ISBUSY(SIZE(np)));
				/**/ ASSERT(ISPFREE(SIZE(np)));
				/**/ ASSERT(*SELF(b) == b);
				/**/ ASSERT(size<=0 || SIZE(b)<size ||
					    SIZE(b) < MAXTINY ||
					    (wild && b==vd->wild));
				P_free += 1;
				S_free += s;
				if(s > M_free)
					M_free = s;

				if(s < MAXTINY)
				{	for(t = TINY(vd)[INDEX(s)]; t; t = LINK(t))
						if(b == t)
							goto fine;
				}
				if(b == vd->wild)
				{	/**/ASSERT(VMWILD(vd,b));
					goto fine;
				}
				if(vd->root && vmintree(vd->root,b))
					goto fine;

				/**/ ASSERT(0);
			}
			else if(ISJUNK(SIZE(b)) )
			{	/**/ ASSERT(ISBUSY(SIZE(b)));
				/**/ ASSERT(!ISPFREE(SIZE(np)));
				P_junk += 1;
				S_junk += s;
				if(s > M_junk)
					M_junk = s;

				if(b == vd->free)
					goto fine;
				if(s < MAXCACHE)
				{	for(t = CACHE(vd)[INDEX(s)]; t; t = LINK(t))
						if(b == t)
							goto fine;
				}
				for(t = CACHE(vd)[S_CACHE]; t; t = LINK(t))
					if(b == t)
						goto fine;
				/**/ ASSERT(0);
			}
			else
			{	/**/ ASSERT(!ISPFREE(SIZE(b)) || !ISBUSY(SIZE(LAST(b))));
				/**/ ASSERT(SEG(b) == seg);
				/**/ ASSERT(!ISPFREE(SIZE(np)));
				P_busy += 1;
				if(s > M_busy)
					M_busy = s;
				goto fine;
			}
		fine:
			b = np;
		}
	}

	return 1;
}

#endif /*DEBUG*/

/* Tree rotation functions */
#define RROTATE(x,y)	(LEFT(x) = RIGHT(y), RIGHT(y) = (x), (x) = (y))
#define LROTATE(x,y)	(RIGHT(x) = LEFT(y), LEFT(y) = (x), (x) = (y))
#define RLINK(s,x)	((s) = LEFT(s) = (x))
#define LLINK(s,x)	((s) = RIGHT(s) = (x))

/* Find and delete a suitable element in the free tree. */
#if __STD_C
static Block_t* bestsearch(Vmdata_t* vd, reg size_t size, Block_t* wanted)
#else
static Block_t* bestsearch(vd, size, wanted)
Vmdata_t*	vd;
reg size_t	size;
Block_t*	wanted;
#endif
{
	reg size_t	s;
	reg Block_t	*t, *root, *l, *r;
	Block_t		link;

	/* extracting a tiniest block from its list */
	if((root = wanted) && size == TINYSIZE)
	{	reg Seg_t*	seg;

		l = TLEFT(root);
		if((r = LINK(root)) )
			TLEFT(r) = l;
		if(l)
			LINK(l) = r;
		else	TINY(vd)[0] = r;

		seg = vd->seg;
		if(!seg->next)
			SEG(root) = seg;
		else for(;; seg = seg->next)
		{	if((uchar*)root > (uchar*)seg->addr && (uchar*)root < seg->baddr)
			{	SEG(root) = seg;
				break;
			}
		}

		return root;
	}

	/* find the right one to delete */
	l = r = &link;
	if((root = vd->root) ) do
	{	/**/ ASSERT(!ISBITS(size) && !ISBITS(SIZE(root)));
		if(size == (s = SIZE(root)) )
			break;
		if(size < s)
		{	if((t = LEFT(root)) )
			{	if(size <= (s = SIZE(t)) )
				{	RROTATE(root,t);
					if(size == s)
						break;
					t = LEFT(root);
				}
				else
				{	LLINK(l,t);
					t = RIGHT(t);
				}
			}
			RLINK(r,root);
		}
		else
		{	if((t = RIGHT(root)) )
			{	if(size >= (s = SIZE(t)) )
				{	LROTATE(root,t);
					if(size == s)
						break;
					t = RIGHT(root);
				}
				else
				{	RLINK(r,t);
					t = LEFT(t);
				}
			}
			LLINK(l,root);
		}
	} while((root = t) );

	if(root)	/* found it, now isolate it */
	{	RIGHT(l) = LEFT(root);
		LEFT(r) = RIGHT(root);
	}
	else		/* nothing exactly fit	*/
	{	LEFT(r) = NIL(Block_t*);
		RIGHT(l) = NIL(Block_t*);

		/* grab the least one from the right tree */
		if((root = LEFT(&link)) )
		{	while((t = LEFT(root)) )
				RROTATE(root,t);
			LEFT(&link) = RIGHT(root);
		}
	}

	if(root && (r = LINK(root)) )
	{	/* head of a link list, use next one for root */
		LEFT(r) = RIGHT(&link);
		RIGHT(r) = LEFT(&link);
	}
	else if(!(r = LEFT(&link)) )
		r = RIGHT(&link);
	else /* graft left tree to right tree */
	{	while((t = LEFT(r)) )
			RROTATE(r,t);
		LEFT(r) = RIGHT(&link);
	}
	vd->root = r;

	/**/ ASSERT(!wanted || wanted == root);
	return root;
}

/* Reclaim all delayed free blocks into the free tree */
#if __STD_C
static int bestreclaim(reg Vmdata_t* vd, Block_t* wanted, int c)
#else
static bestreclaim(vd, wanted, c)
reg Vmdata_t*	vd;
Block_t*	wanted;
int		c;
#endif
{
	reg size_t	size, s;
	reg Block_t	*fp, *np, *t, *list, **cache;
	reg int		n, count;
	Block_t		tree;

	if((fp = vd->free) )
	{	LINK(fp) = *(cache = CACHE(vd) + C_INDEX(SIZE(fp)) );
		*cache = fp;
		vd->free = NIL(Block_t*);
	}

	LINK(&tree) = NIL(Block_t*);
	count = 0;
	for(n = S_CACHE; n >= c; --n)
	{	list = *(cache = CACHE(vd) + n);
		*cache = NIL(Block_t*);
		while((fp = list) )
		{	/* Note that below here we allow ISJUNK blocks to be
			** forward-merged even though they are not removed from
			** the list immediately. In this way, the list is
			** scanned only once. It works because the LINK and SIZE
			** fields are not destroyed during the merging. This can
			** be seen by observing that a tiniest block has a 2-word
			** header and a 2-word body. Merging a tiniest block
			** (1seg) and the next block (2seg) looks like this:
			**	1seg  size  link  left  2seg size link left ....
			**	1seg  size  link  left  rite xxxx xxxx .... self
			** After the merge, the 2seg word is replaced by the RIGHT
			** pointer of the new block and somewhere beyond the
			** two xxxx fields, the SELF pointer will replace some
			** other word. The important part is that the two xxxx
			** fields are kept intact.
			*/
			count += 1;
			list = LINK(list);
			size = SIZE(fp);
			if(!ISJUNK(size))	/* already done */
				continue;

			if(ISPFREE(size))	/* backward merge */
			{	fp = LAST(fp);
				s = SIZE(fp);
				DELETE(vd,fp,INDEX(s),t,bestsearch);
				size = (size&~BITS) + s + sizeof(Head_t);
			}
			else	size &= ~BITS;

			for(;;)	/* forward merge */
			{	np = (Block_t*)((uchar*)fp+size+sizeof(Head_t));
				s = SIZE(np);	/**/ASSERT(s > 0);
				if(!ISBUSY(s))
				{	if(np == vd->wild)
						vd->wild = NIL(Block_t*);
					else	DELETE(vd,np,INDEX(s),t,bestsearch);
				}
				else if(ISJUNK(s) && C_INDEX(s) >= c )
				{	SIZE(np) = 0;
					CLRBITS(s);
				}
				else	break;
				size += s + sizeof(Head_t);
			}
			SIZE(fp) = size;

			if(fp == wanted) /* about to be consumed by bestresize */
				continue;

			/* tell next block that this one is free */
			SETPFREE(SIZE(np)); /**/ ASSERT(ISBUSY(SIZE(np)) );
			*(SELF(fp)) = fp;

			if(np->body.data >= vd->seg->baddr)
			{	vd->wild = fp;
				continue;
			}

			/* tiny block goes to tiny list */
			if(size < MAXTINY)
			{	s = INDEX(size);
				np = LINK(fp) = TINY(vd)[s];
				if(s == 0)	/* TINIEST block */
				{	if(np)
						TLEFT(np) = fp;
					TLEFT(fp) = NIL(Block_t*);
				}
				else
				{	if(np)
						LEFT(np)  = fp;
					LEFT(fp) = NIL(Block_t*);
					SETLINK(fp);
				}
				TINY(vd)[s] = fp;
				continue;
			}

			/* don't put in free tree yet because they may be merged soon */
			np = &tree;
			if((LINK(fp) = LINK(np)) )
				LEFT(LINK(fp)) = fp;
			LINK(np) = fp;
			LEFT(fp) = np;
			SETLINK(fp);
		}
	}

	/* insert all free blocks into the free tree */
	for(list = LINK(&tree); list; )
	{	fp = list;
		list = LINK(list);

		/**/ASSERT(!ISBITS(SIZE(fp)));
		/**/ASSERT(ISBUSY(SIZE(NEXT(fp))) );
		/**/ASSERT(ISPFREE(SIZE(NEXT(fp))) );
		LEFT(fp) = RIGHT(fp) = LINK(fp) = NIL(Block_t*);
		if(!(np = vd->root) )	/* inserting into an empty tree	*/
		{	vd->root = fp;
			continue;
		}

		size = SIZE(fp);
		while(1)	/* leaf insertion */
		{	if((s = SIZE(np)) > size)
			{	if((t = LEFT(np)) )
					np = t;
				else
				{	LEFT(np) = fp;
					break;
				}
			}
			else if(s < size)
			{	if((t = RIGHT(np)) )
					np = t;
				else
				{	RIGHT(np) = fp;
					break;
				}
			}
			else /* s == size */
			{	if((t = LINK(np)) )
				{	LINK(fp) = t;
					LEFT(t) = fp;
				}
				LINK(np) = fp;
				LEFT(fp) = np;
				SETLINK(fp);
				break;
			}
		}
	}

	return count;
}

#if __STD_C
static Void_t* bestalloc(Vmalloc_t* vm, reg size_t size )
#else
static Void_t* bestalloc(vm,size)
Vmalloc_t*	vm;	/* region allocating from	*/
reg size_t	size;	/* desired block size		*/
#endif
{
	reg Vmdata_t*	vd = vm->data;
	reg size_t	s;
	reg Block_t	*tp, *np, **cache;
	reg int		local;
	size_t		orgsize;

	/**/ COUNT(N_alloc);

	if(!(local = vd->mode&VM_TRUST))
	{	GETLOCAL(vd,local);
		if(ISLOCK(vd,local) )
			return NIL(Void_t*);
		SETLOCK(vd,local);
		orgsize = size;
	}

	/**/ ASSERT(HEADSIZE == sizeof(Head_t));
	/**/ ASSERT(BODYSIZE == sizeof(Body_t));
	/**/ ASSERT((ALIGN%(BITS+1)) == 0 );
	/**/ ASSERT((sizeof(Head_t)%ALIGN) == 0 );
	/**/ ASSERT((sizeof(Body_t)%ALIGN) == 0 );
	/**/ ASSERT((TINYSIZE%ALIGN) == 0 );
	/**/ ASSERT(sizeof(Block_t) == (sizeof(Body_t)+sizeof(Head_t)) );

	/* for ANSI requirement that malloc(0) returns non-NULL pointer */
	size = size <= TINYSIZE ? TINYSIZE : ROUND(size,ALIGN);

	if(size < MAXCACHE && (tp = *(cache = CACHE(vd) + INDEX(size)) ) )
	{	*cache = LINK(tp);
		CLRJUNK(SIZE(tp));
		/**/COUNT(N_cache);
		goto done;
	}

	if((tp = vd->free) )	/* allocate from last free piece */
	{	/**/ASSERT(ISBUSY(SIZE(tp)) );
		/**/ASSERT(ISJUNK(SIZE(tp)) );
		/**/COUNT(N_last);

		vd->free = NIL(Block_t*);
		if((s = SIZE(tp)) < size)
		{	LINK(tp) = *(cache = CACHE(vd)+C_INDEX(s));
			*cache = tp;
		}
		else
		{	if(s >= size + (sizeof(Head_t)+TINYSIZE) )
			{	SIZE(tp) = size;
				np = NEXT(tp);
				SEG(np) = SEG(tp);
				SIZE(np) = ((s&~BITS) - (size+sizeof(Head_t)))|JUNK|BUSY;
				vd->free = np;
				SIZE(tp) |= s&BITS;
			}
			CLRJUNK(SIZE(tp));
			goto done;
		}
	}

	for(;;)
	{	for(;;)	/* best-fit - more or less */
		{	for(s = INDEX(size); s < S_TINY; ++s)
			{	if((tp = TINY(vd)[s]) )
				{	DELETE(vd,tp,s,np,bestsearch);
					CLRPFREE(SIZE(NEXT(tp)));
					goto got_block;
				}
			}

			if(CACHE(vd)[S_CACHE])	/* reclaim big pieces */
				bestreclaim(vd,NIL(Block_t*),S_CACHE);
			if(vd->root && (tp = bestsearch(vd,size,NIL(Block_t*))) )
				goto got_block;
			if(bestreclaim(vd,NIL(Block_t*),0) == 0)
				break;
		}

		/**/ASSERT(!vd->free);
		if((tp = vd->wild) && SIZE(tp) >= size)
		{	/**/ASSERT(vmcheck(vd,size,1));
			/**/COUNT(N_wild);
			vd->wild = NIL(Block_t*);
			goto got_block;
		}
	
		/**/ASSERT(vmcheck(vd,size,0) );
		if((tp = (*_Vmextend)(vm,size,bestsearch)) )
			goto got_block;
		else if(vd->mode&VM_AGAIN)
			vd->mode &= ~VM_AGAIN;
		else
		{	CLRLOCK(vd,local);
			return NIL(Void_t*);
		}
	}

got_block:
	/**/ ASSERT(!ISBITS(SIZE(tp)));
	/**/ ASSERT(SIZE(tp) >= size);
	/**/ ASSERT((SIZE(tp)%ALIGN) == 0);
	/**/ ASSERT(!vd->free);

	/* tell next block that we are no longer a free block */
	CLRPFREE(SIZE(NEXT(tp)));	/**/ ASSERT(ISBUSY(SIZE(NEXT(tp))));

	if((s = SIZE(tp)-size) >= (sizeof(Head_t)+TINYSIZE) )
	{	SIZE(tp) = size;

		np = NEXT(tp);
		SEG(np) = SEG(tp);
		SIZE(np) = (s - sizeof(Head_t)) | BUSY|JUNK;

		if(!vd->root)
			vd->free = np;
		else if(VMWILD(vd,np))
		{	SIZE(np) &= ~BITS;
			*SELF(np) = np;
			SETPFREE(SIZE(NEXT(np)));
			vd->wild = np;
		}
		else
		{	LINK(np) = *(cache = CACHE(vd) + C_INDEX(SIZE(np)));
			*cache = np;
		}
	}

	SETBUSY(SIZE(tp));

done:
	if(!local && (vd->mode&VM_TRACE) && _Vmtrace && VMETHOD(vd) == VM_MTBEST)
		(*_Vmtrace)(vm,NIL(uchar*),(uchar*)DATA(tp),orgsize);

	CLRLOCK(vd,local);
	return DATA(tp);
}

#if __STD_C
static long bestaddr(Vmalloc_t* vm, Void_t* addr )
#else
static long bestaddr(vm, addr)
Vmalloc_t*	vm;	/* region allocating from	*/
Void_t*		addr;	/* address to check		*/
#endif
{
	reg Seg_t*	seg;
	reg Block_t	*b, *endb;
	reg long	offset;
	reg Vmdata_t*	vd = vm->data;
	reg int		local;

	if(!(local = vd->mode&VM_TRUST) )
	{	GETLOCAL(vd,local);
		if(ISLOCK(vd,local))
			return -1L;
		SETLOCK(vd,local);
	}

	offset = -1L;
	for(seg = vd->seg; seg; seg = seg->next)
	{	b = SEGBLOCK(seg);
		endb = (Block_t*)(seg->baddr - sizeof(Head_t));
		if((uchar*)addr > (uchar*)b && (uchar*)addr < (uchar*)endb)
			break;
	}

	if(local && !(vd->mode&VM_TRUST) ) /* from bestfree or bestresize */
	{	b = BLOCK(addr);
		if(seg && SEG(b) == seg && ISBUSY(SIZE(b)) && !ISJUNK(SIZE(b)) )
			offset = 0;
		if(offset != 0 && vm->disc->exceptf)
			(void)(*vm->disc->exceptf)(vm,VM_BADADDR,addr,vm->disc);
	}
	else if(seg)
	{	while(b < endb)
		{	reg uchar*	data = (uchar*)DATA(b);
			reg size_t	size = SIZE(b)&~BITS;

			if((uchar*)addr >= data && (uchar*)addr < data+size)
			{	if(ISJUNK(SIZE(b)) || !ISBUSY(SIZE(b)))
					offset = -1L;
				else	offset = (uchar*)addr - data;
				goto done;
			}

			b = (Block_t*)((uchar*)DATA(b) + size);
		}
	}

done:	
	CLRLOCK(vd,local);
	return offset;
}

#if __STD_C
static int bestfree(Vmalloc_t* vm, Void_t* data )
#else
static bestfree(vm, data )
Vmalloc_t*	vm;
Void_t*		data;
#endif
{
	reg Vmdata_t*	vd = vm->data;
	reg Block_t	*bp, *fp, **cache;
	reg size_t	s;
	reg int		local;

	/**/COUNT(N_free);

	if(!data)	/* ANSI-ism */
		return 0;

	if(!(local = vd->mode&VM_TRUST) )
	{	if(ISLOCK(vd,0) )
			return -1;
		if(KPVADDR(vm,data,bestaddr) != 0 )
			return -1;
		SETLOCK(vd,0);
	}

	bp = BLOCK(data);	/**/ASSERT(ISBUSY(SIZE(bp)) && !ISJUNK(SIZE(bp)));
	SETJUNK(SIZE(bp));
	if((s = SIZE(bp)) < MAXCACHE)
	{	LINK(bp) = *(cache = CACHE(vd) + INDEX(s));
		*cache = bp;
	}
	else
	{	if((!(fp = vd->free) || VMWILD(vd,fp)) && !VMWILD(vd,bp) )
			vd->free = bp;
		else	fp = bp;
		if(fp)
		{	LINK(fp) = *(cache = CACHE(vd) + C_INDEX(SIZE(fp)));
			*cache = fp;
		}
	}

	if(!local && _Vmtrace && (vd->mode&VM_TRACE) && VMETHOD(vd) == VM_MTBEST )
		(*_Vmtrace)(vm,(uchar*)data,NIL(uchar*),SIZE(BLOCK(data))&~BITS);

	CLRLOCK(vd,0);
	return 0;
}

#if __STD_C
static Void_t* bestresize(Vmalloc_t* vm, Void_t* data, reg size_t size, int flags)
#else
static Void_t* bestresize(vm,data,size,flags)
Vmalloc_t*	vm;		/* region allocating from	*/
Void_t*		data;		/* old block of data		*/
reg size_t	size;		/* new size			*/
int		flags;		/* VM_RS*			*/
#endif
{
	reg Vmdata_t*	vd = vm->data;
	reg Block_t	*rp, *np, *t, **cache;
	reg size_t	s, bs;
	reg int		local, *nd, *od;
	Void_t*		orgdata;
	size_t		orgsize;

	/**/ COUNT(N_resize);

	if(!data)
	{	if((data = bestalloc(vm,size)) && (flags & VM_RSZERO))
		{	s = (size+sizeof(int)-1)/sizeof(int);
			for(nd = (int*)data; s-- > 0; )
				*nd++ = 0;
		}
		return data;
	}
	if(size == 0)
	{	(void)bestfree(vm,data);
		return NIL(Void_t*);
	}

	if(!(local = vd->mode&VM_TRUST) )
	{	GETLOCAL(vd,local);
		if(ISLOCK(vd,local) )
			return NIL(Void_t*);
		if(!local && KPVADDR(vm,data,bestaddr) != 0 )
			return NIL(Void_t*);
		SETLOCK(vd,local);

		orgdata = data;	/* for tracing */
		orgsize = size;
	}

	size = size <= TINYSIZE ? TINYSIZE : ROUND(size,ALIGN);
	rp = BLOCK(data);	/**/ASSERT(ISBUSY(SIZE(rp)) && !ISJUNK(SIZE(rp)));
	if((bs = SIZE(rp)) < size)
	{	CLRBITS(SIZE(rp));
		np = NEXT(rp);
		do	/* forward merge as much as possible */
		{	s = SIZE(np);
			if(np == vd->free)
			{	vd->free = NIL(Block_t*);
				CLRBITS(s);
			}
			else if(ISJUNK(s) )
			{	CPYBITS(SIZE(rp),bs);
				bestreclaim(vd,np,C_INDEX(s));
				s = SIZE(np);
				bs = SIZE(rp);
				CLRBITS(SIZE(rp));
			}
			else if(!ISBUSY(s) )
			{	if(np == vd->wild)
					vd->wild = NIL(Block_t*);
				else	DELETE(vd,np,INDEX(s),t,bestsearch); 
			}
			else	break;

			SIZE(rp) += (s += sizeof(Head_t));
			np = (Block_t*)((uchar*)np + s);
			CLRPFREE(SIZE(np));
		} while(SIZE(rp) < size);

		if(SIZE(rp) < size && size > vd->incr && SEGWILD(rp) )
		{	reg Seg_t*	seg;

			s = (size - SIZE(rp)) + sizeof(Head_t);
			s = ROUND(s,vd->incr);
			seg = SEG(rp);
			if((*vm->disc->memoryf)(vm,seg->addr,seg->extent,seg->extent+s,
				      vm->disc) == seg->addr )
			{	SIZE(rp) += s;
				seg->extent += s;
				seg->size += s;
				seg->baddr += s;
				SEG(NEXT(rp)) = seg;
				SIZE(NEXT(rp)) = BUSY;
			}
		}

		CPYBITS(SIZE(rp),bs);
	}

	/* If a buffer is resized, it is likely to be resized again. So we
	   make the increment a reasonable size to reduce future work */
#define INCREMENT	128
	if((s = SIZE(rp)) >= (size + (INCREMENT+TINYSIZE+sizeof(Head_t))) )
	{	SIZE(rp) = size;
		np = NEXT(rp);
		SEG(np) = SEG(rp);
		SIZE(np) = (((s&~BITS)-size) - sizeof(Head_t))|BUSY|JUNK;
		CPYBITS(SIZE(rp),s);
		rp = np;
		goto do_free;
	}
	else if(s < size)
	{	if(!(flags & VM_RSFREE))
			data = NIL(Void_t*);
		else
		{	od = (int*)data;
			if(size < (s&~BITS)+INCREMENT)
				size = (s&~BITS)+INCREMENT;
			if((data = KPVALLOC(vm,size,bestalloc)) )
			{	if(flags & VM_RSZERO)
				{	bs = (size-s+sizeof(int)-1)/sizeof(int);
					for(nd = (int*)((char*)data+s); bs-- > 0; )
						*nd++ = 0;
				}
				if(flags & VM_RSCOPY)
				{	nd = (int*)data;
					for(s /= sizeof(int); s-- > 0; )
						*nd++ = *od++;
				}
				SETJUNK(SIZE(rp));
			do_free:
				if((np = vd->free) && VMWILD(vd,np) )
				{	vd->free = rp;
					rp = np;
				}
				cache = CACHE(vd) + C_INDEX(SIZE(rp));
				LINK(rp) = *cache;
				*cache = rp;
			}
		}
	}

	if(!local && _Vmtrace && data && (vd->mode&VM_TRACE) && VMETHOD(vd) == VM_MTBEST)
		(*_Vmtrace)(vm, (uchar*)orgdata, (uchar*)data, orgsize);

	CLRLOCK(vd,local);
	return data;
}

#if __STD_C
static long bestsize(Vmalloc_t* vm, Void_t* addr )
#else
static long bestsize(vm, addr)
Vmalloc_t*	vm;	/* region allocating from	*/
Void_t*		addr;	/* address to check		*/
#endif
{
	reg Seg_t*	seg;
	reg Block_t	*b, *endb;
	reg long	size;
	reg Vmdata_t*	vd = vm->data;

	if(!(vd->mode&VM_TRUST) )
	{	if(ISLOCK(vd,0))
			return -1L;
		SETLOCK(vd,0);
	}

	size = -1L;
	for(seg = vd->seg; seg; seg = seg->next)
	{	b = SEGBLOCK(seg);
		endb = (Block_t*)(seg->baddr - sizeof(Head_t));
		if((uchar*)addr <= (uchar*)b || (uchar*)addr >= (uchar*)endb)
			continue;
		while(b < endb)
		{	if(addr == DATA(b))
			{	if(!ISBUSY(SIZE(b)) || ISJUNK(SIZE(b)) )
					size = -1L;
				else	size = (long)SIZE(b)&~BITS;
				goto done;
			}
			else if((uchar*)addr <= (uchar*)b)
				break;

			b = (Block_t*)((uchar*)DATA(b) + (SIZE(b)&~BITS) );
		}
	}

done:
	CLRLOCK(vd,0);
	return size;
}

#if __STD_C
static int bestcompact(Vmalloc_t* vm)
#else
static bestcompact(vm)
Vmalloc_t*	vm;
#endif
{
	reg Seg_t	*seg, *next;
	reg Block_t	*bp, *t;
	reg size_t	size;
	reg Vmdata_t*	vd = vm->data;

	if(!(vd->mode&VM_TRUST) )
	{	if(ISLOCK(vd,0))
			return -1;
		SETLOCK(vd,0);
	}

	bestreclaim(vd,NIL(Block_t*),0);

	for(seg = vd->seg; seg; )
	{	next = seg->next;
		bp = BLOCK(seg->baddr);
		if(!ISPFREE(SIZE(bp)) )
			goto loop;

		bp = LAST(bp);	/**/ ASSERT(!ISBUSY(SIZE(bp)));
		size = SIZE(bp);
		if(VMWILD(vd,bp))
			vd->wild = NIL(Block_t*);
		else	DELETE(vd,bp,INDEX(size),t,bestsearch);
		CLRPFREE(SIZE(NEXT(bp)));

		if(size < seg->size)
			size += sizeof(Head_t);

		if((*_Vmtruncate)(vm,seg,size,1) >= 0)
		{	if((size = (seg->baddr - ((uchar*)bp) - sizeof(Head_t))) > 0)
				SIZE(bp) = size - sizeof(Head_t);
			else	bp = NIL(Block_t*);
		}

		if(bp)
		{	/**/ ASSERT(SIZE(bp) >= TINYSIZE);
			/**/ ASSERT(SEGWILD(bp));
			SIZE(bp) |= BUSY|JUNK;
			LINK(bp) = CACHE(vd)[C_INDEX(SIZE(bp))];
			CACHE(vd)[C_INDEX(SIZE(bp))] = bp;
		}

	loop:
		seg = next;
	}

	CLRLOCK(vd,0);
	return 0;
}

#if __STD_C
static Void_t* bestalign(Vmalloc_t* vm, size_t size, size_t align)
#else
static Void_t* bestalign(vm, size, align)
Vmalloc_t*	vm;
size_t		size;
size_t		align;
#endif
{
	reg uchar	*data;
	reg Block_t	*tp, *np;
	reg Seg_t*	seg;
	reg size_t	s;
	reg Vmdata_t*	vd = vm->data;

	if(size <= 0 || align <= 0)
		return NIL(Void_t*);

	if(!(vd->mode&VM_TRUST) )
	{	if(ISLOCK(vd,0) )
			return NIL(Void_t*);
		SETLOCK(vd,0);
	}

	align = MULTIPLE(align,ALIGN);
	s = (size <= TINYSIZE ? TINYSIZE : ROUND(size,ALIGN)) + align;
	if(!(data = (uchar*)KPVALLOC(vm,s,bestalloc)) )
		goto done;

	tp = BLOCK(data);
	seg = SEG(tp);

	/* get an aligned address that we can live with */
	if((s = (size_t)(ULONG(data)%align)) != 0)
		data += align-s;
	np = BLOCK(data);
	if(((uchar*)np - (uchar*)tp) < sizeof(Block_t) )
		data += align;

	/* np is the usable block of data */
	np = BLOCK(data);
	s  = (uchar*)np - (uchar*)tp;
	SIZE(np) = ((SIZE(tp)&~BITS) - s)|BUSY;
	SEG(np) = seg;
	data = (uchar*)DATA(np);

	/* now free the left part */
	SIZE(tp) = (s - sizeof(Head_t)) | (SIZE(tp)&BITS) | JUNK;
	/**/ ASSERT(SIZE(tp) >= sizeof(Body_t) );
	if(!vd->free)
		vd->free = tp;
	else
	{	LINK(tp) = CACHE(vd)[C_INDEX(SIZE(tp))];
		CACHE(vd)[C_INDEX(SIZE(tp))] = tp;
	}

	if(!(vd->mode&VM_TRUST) && _Vmtrace && (vd->mode&VM_TRACE) &&
	   VMETHOD(vd) == VM_MTBEST )
		(*_Vmtrace)(vm,NIL(uchar*),data,size);

done:
	CLRLOCK(vd,0);
	return (Void_t*)data;
}

/* The world knows us by this */
Vmethod_t	_Vmbest =
{
	bestalloc,
	bestresize,
	bestfree,
	bestaddr,
	bestsize,
	bestcompact,
	bestalign,
	VM_MTBEST
};

/* The heap region */
static Vmdata_t	_Vmdata =
{
	VM_MTBEST|VM_TRUST,		/* mode		*/
	0,				/* incr		*/
	0,				/* pool		*/
	NIL(Seg_t*),			/* seg		*/
	NIL(Block_t*),			/* free		*/
	NIL(Block_t*),			/* wild		*/
	NIL(Block_t*),			/* root		*/
};
Vmalloc_t _Vmheap =
{
	{ bestalloc,
	  bestresize,
	  bestfree,
	  bestaddr,
	  bestsize,
	  bestcompact,
	  bestalign,
	  VM_MTBEST
	},
	Vmdcsbrk,			/* disc		*/
	&_Vmdata			/* data		*/
};

Vmalloc_t*	Vmregion = &_Vmheap;	/* region for malloc/free/realloc	*/
