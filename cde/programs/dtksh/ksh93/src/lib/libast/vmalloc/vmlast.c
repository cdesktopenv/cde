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
/* $XConsortium: vmlast.c /main/2 1996/05/08 20:03:35 drk $ */
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

/*	Allocation with freeing and reallocing of last allocated block only.
**
**	Written by Kiem-Phong Vo, kpv@research.att.com, 01/16/94.
*/

#if __STD_C
static Void_t* lastalloc(Vmalloc_t* vm, size_t size)
#else
static Void_t* lastalloc(vm, size)
Vmalloc_t*	vm;
size_t		size;
#endif
{
	reg Block_t	*tp, *next;
	reg Seg_t	*seg, *last;
	reg size_t	s;
	reg Vmdata_t*	vd = vm->data;
	reg int		local;
	size_t		orgsize;

	if(!(local = vd->mode&VM_TRUST))
	{	GETLOCAL(vd,local);
		if(ISLOCK(vd,local))
			return NIL(Void_t*);
		SETLOCK(vd,local);
		orgsize = size;
	}

	size = size < ALIGN ? ALIGN : ROUND(size,ALIGN);
	for(;;)
	{	for(last = NIL(Seg_t*), seg = vd->seg; seg; last = seg, seg = seg->next)
		{	if(!(tp = seg->free) || (SIZE(tp)+sizeof(Head_t)) < size)
				continue;
			if(last) /* move this segment to the front of the list */
			{	last->next = seg->next;
				seg->next = vd->seg;
				vd->seg = seg;
			}
			goto got_block;
		}

		/* there is no usable free space in region, try extending */
		if((tp = (*_Vmextend)(vm,size,NIL(Vmsearch_f))) )
		{	seg = SEG(tp);
			goto got_block;
		}
		else if(vd->mode&VM_AGAIN)
			vd->mode &= ~VM_AGAIN;
		else	goto done;
	}

got_block:
	if((s = SIZE(tp)) >= size)
	{	/* there is some more space */
		next = (Block_t*)((uchar*)tp+size);
		SIZE(next) = s - size;
		SEG(next) = seg;
		seg->free = next;
	}
	else	seg->free = NIL(Block_t*);

	vd->pool += 1;
	vd->free  = tp;

	if(!local && (vd->mode&VM_TRACE) && _Vmtrace)
		(*_Vmtrace)(vm, NIL(uchar*), (uchar*)tp, orgsize);

done:
	CLRLOCK(vd,local);
	return (Void_t*)tp;
}

#if __STD_C
static int lastfree(Vmalloc_t* vm, reg Void_t* data )
#else
static int lastfree(vm, data)
Vmalloc_t*	vm;
reg Void_t*	data;
#endif
{
	reg Seg_t*	seg;
	reg Block_t*	fp;
	reg size_t	s;
	reg Vmdata_t*	vd = vm->data;
	reg int		local;

	if(!data)
		return 0;
	if(!(local = vd->mode&VM_TRUST) )
	{	if(ISLOCK(vd,0))
			return -1;
		SETLOCK(vd,0);
	}
	if(data != (Void_t*)vd->free)
	{	if(!local && vm->disc->exceptf)
			(void)(*vm->disc->exceptf)(vm,VM_BADADDR,data,vm->disc);
		CLRLOCK(vd,0);
		return -1;
	}

	seg = vd->seg;
	if(!local && (vd->mode&VM_TRACE) && _Vmtrace)
	{	if(seg->free )
			s = (uchar*)(seg->free) - (uchar*)data;
		else	s = (uchar*)BLOCK(seg->baddr) - (uchar*)data;
		(*_Vmtrace)(vm, (uchar*)data, NIL(uchar*), s);
	}

	vd->free = NIL(Block_t*);
	fp = (Block_t*)data;
	SEG(fp)  = seg;
	SIZE(fp) = ((uchar*)BLOCK(seg->baddr) - (uchar*)data) - sizeof(Head_t);
	seg->free = fp;
	vd->pool -= 1;

	CLRLOCK(vd,0);
	return 0;
}

#if __STD_C
static Void_t* lastresize(Vmalloc_t* vm, reg Void_t* data, size_t size, int flags )
#else
static Void_t* lastresize(vm, data, size, flags )
Vmalloc_t*	vm;
reg Void_t*	data;
size_t		size;
int		flags;
#endif
{
	reg Block_t*	tp;
	reg Seg_t*	seg;
	reg size_t	oldsize, s;
	reg Vmdata_t*	vd = vm->data;
	reg int		local;
	Void_t*		orgdata;
	size_t		orgsize;

	if(!data)
	{	if((data = lastalloc(vm,size)) && (flags & VM_RSZERO))
		{	reg int*	nd;
			s = (size+sizeof(int)-1)/sizeof(int);
			for(nd = (int*)data; s-- > 0; )
				*nd++ = 0;
		}
		return data;
	}
	else if(size <= 0)
	{	(void)lastfree(vm,data);
		return NIL(Void_t*);
	}

	if(!(local = vd->mode&VM_TRUST))
	{	if(ISLOCK(vd,0))
			return NIL(Void_t*);
		SETLOCK(vd,0);
		orgdata = data;
		orgsize = size;
	}

	if(data != (Void_t*)vd->free)
	{	if(!local && vm->disc->exceptf)
			(void)(*vm->disc->exceptf)(vm,VM_BADADDR,data,vm->disc);
		CLRLOCK(vd,0);
		return NIL(Void_t*);
	}

	/* current available space */
	seg = vd->seg;
	s = (uchar*)BLOCK(seg->baddr) - (uchar*)data;
	if(!(tp = seg->free) )
		oldsize = s;
	else
	{	oldsize = (uchar*)tp - (uchar*)data;
		seg->free = NIL(Block_t*);
	}

	if((size = size < ALIGN ? ALIGN : ROUND(size,ALIGN)) > s)
	{	reg size_t	incr;
		reg Void_t*	addr = seg->addr;
		reg Vmemory_f	memoryf = vm->disc->memoryf;

		/* amount to extend */
		incr = size-s; incr = ROUND(incr,vd->incr);
		if((*memoryf)(vm,addr,seg->extent,seg->extent+incr,vm->disc) == addr)
		{	s += incr;
			seg->size += incr;
			seg->extent += incr;
			seg->baddr += incr;
			SIZE(BLOCK(seg->baddr)) = BUSY;
		}
		else if(!(flags & VM_RSFREE))
			data = NIL(Void_t*);
		else if(!(addr = KPVALLOC(vm,size,lastalloc)) )
		{	vd->free = (Block_t*)data;
			data = NIL(Void_t*);
		}
		else
		{	if(flags & VM_RSZERO)
			{	reg int*	di;
				reg size_t	ni;
				ni = (size-oldsize+sizeof(int)-1)/sizeof(int);
				for(di = (int*)((char*)data+oldsize); ni-- > 0; )
					*di++ = 0;
			}

			if(flags & VM_RSCOPY)
			{	reg int	*od = (int*)data, *nd = (int*)addr;
				for(oldsize /= sizeof(int); oldsize-- > 0; )
					*nd++ = *od++;
			}

			if(seg != vd->seg)
			{	tp = (Block_t*)data;
				SEG(tp) = seg;
				SIZE(tp) = s - sizeof(Head_t);
				seg->free = tp;
			}

			/* new block and size */
			data = addr;
			seg = vd->seg;
			s = (uchar*)BLOCK(seg->baddr) - (uchar*)data;
			seg->free = NIL(Block_t*);
		}
	}

	if(data)
	{	if(s >= size+sizeof(Head_t))
		{	tp = (Block_t*)((uchar*)data + size);
			SEG(tp) = seg;
			SIZE(tp) = (s - size) - sizeof(Head_t);
			seg->free = tp;
		}

		vd->free = (Block_t*)data;

		if(!local && (vd->mode&VM_TRACE) && _Vmtrace)
			(*_Vmtrace)(vm,(uchar*)orgdata,(uchar*)data,orgsize);
	}

	CLRLOCK(vd,0);
	return data;
}


#if __STD_C
static long lastaddr(Vmalloc_t* vm, Void_t* addr)
#else
static long lastaddr(vm, addr)
Vmalloc_t*	vm;
Void_t*		addr;
#endif
{
	reg Vmdata_t*	vd = vm->data;

	if(!(vd->mode&VM_TRUST) && ISLOCK(vd,0))
		return -1L;
	if(!vd->free || addr < (Void_t*)vd->free || addr >= (Void_t*)vd->seg->baddr)
		return -1L;
	else	return (uchar*)addr - (uchar*)vd->free;
}

#if __STD_C
static long lastsize(Vmalloc_t* vm, Void_t* addr)
#else
static long lastsize(vm, addr)
Vmalloc_t*	vm;
Void_t*		addr;
#endif
{
	reg Vmdata_t*	vd = vm->data;

	if(!(vd->mode&VM_TRUST) && ISLOCK(vd,0))
		return -1L;
	if(!vd->free || addr != (Void_t*)vd->free )
		return -1L;
	else if(vd->seg->free)
		return (uchar*)vd->seg->free - (uchar*)addr;
	else	return (uchar*)vd->seg->baddr - (uchar*)addr - sizeof(Head_t);
}

#if __STD_C
static int lastcompact(Vmalloc_t* vm)
#else
static lastcompact(vm)
Vmalloc_t*	vm;
#endif
{
	reg Block_t*	fp;
	reg Seg_t	*seg, *next;
	reg size_t	s;
	reg Vmdata_t*	vd = vm->data;

	if(!(vd->mode&VM_TRUST))
	{	if(ISLOCK(vd,0))
			return -1;
		SETLOCK(vd,0);
	}

	for(seg = vd->seg; seg; )
	{	next = seg->next;

		if(!(fp = seg->free))
			goto loop;

		seg->free = NIL(Block_t*);
		if(seg->size == (s = SIZE(fp)&~BITS))
			s = seg->extent;
		else	s += sizeof(Head_t);

		if((*_Vmtruncate)(vm,seg,s,1) < 0)
			seg->free = fp;
	loop:
		seg = next;
	}

	CLRLOCK(vd,0);
	return 0;
}

#if __STD_C
static Void_t* lastalign(Vmalloc_t* vm, size_t size, size_t align)
#else
static Void_t* lastalign(vm, size, align)
Vmalloc_t*	vm;
size_t		size;
size_t		align;
#endif
{
	NOTUSED(vm);
	NOTUSED(size);
	NOTUSED(align);
	return NIL(Void_t*);
}

/* Public method for free-1 allocation */
Vmethod_t _Vmlast =
{
	lastalloc,
	lastresize,
	lastfree,
	lastaddr,
	lastsize,
	lastcompact,
	lastalign,
	VM_MTLAST
};
