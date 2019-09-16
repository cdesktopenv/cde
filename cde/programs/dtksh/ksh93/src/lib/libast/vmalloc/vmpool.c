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
/* $XConsortium: vmpool.c /main/2 1996/05/08 20:04:14 drk $ */
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

/*	Method for pool allocation.
**	All elements in a pool have the same size.
**	The following fields of Vmdata_t are used as:
**		pool:	size of a block.
**		free:	list of free blocks.
**
**	Written by (Kiem-)Phong Vo, kpv@research.att.com, 01/16/94.
*/

#if __STD_C
static Void_t* poolalloc(Vmalloc_t* vm, size_t size)
#else
static Void_t* poolalloc(vm, size )
Vmalloc_t*	vm;
size_t		size;
#endif
{
	reg Block_t	*tp, *next;
	reg size_t	s;
	reg Seg_t*	seg;
	reg Vmdata_t*	vd = vm->data;
	reg int		local;

	if(size <= 0)
		return NIL(Void_t*);

	if(!(local = vd->mode&VM_TRUST) )
	{	if(ISLOCK(vd,0))
			return NIL(Void_t*);
		if(vd->pool > 0 && size != vd->pool)
			return NIL(Void_t*);
		SETLOCK(vd,0);
	}

	if(vd->pool <= 0)	/* region initialization */
	{	vd->pool = size;

		/* build free list out of available space */
		size = ROUND(size,ALIGN);
		for(seg = vd->seg; seg; seg = seg->next)
		{	if(!(tp = seg->free) )
				continue;
			seg->free = NIL(Block_t*);
			for(s = (SIZE(tp)&~BITS)+sizeof(Head_t); s >= size; s -= size)
			{	SEGLINK(tp) = vd->free;
				vd->free = tp;
				tp = (Block_t*)((uchar*)tp+size);
			} 
		}
	}

	for(;;)
	{	if((tp = vd->free) )
		{	vd->free = SEGLINK(tp);
			goto done;
		}

		/* if get here, no current free block works, so we must extend */
		if((tp = (*_Vmextend)(vm,ROUND(vd->pool,vd->incr),NIL(Vmsearch_f))) )
			break;
		else if(vd->mode&VM_AGAIN)
			vd->mode &= ~VM_AGAIN;
		else	goto done;
	}

	/* make remainder into free list */
	size = ROUND(vd->pool,ALIGN);
	next = (Block_t*)((uchar*)tp+size);
	s = (uchar*)BLOCK(vd->seg->baddr) - (uchar*)next;
	for(; s >= size; s -= size)
	{	SEGLINK(next) = vd->free;
		vd->free = next;
		next = (Block_t*)((uchar*)next + size);
	}

done:
	if(!local && (vd->mode&VM_TRACE) && _Vmtrace && tp)
		(*_Vmtrace)(vm,NIL(uchar*),(uchar*)tp,vd->pool);

	CLRLOCK(vd,0);
	return (Void_t*)tp;
}

#if __STD_C
static long pooladdr(Vmalloc_t* vm, Void_t* addr)
#else
static long pooladdr(vm, addr)
Vmalloc_t*	vm;
Void_t*		addr;
#endif
{
	reg Seg_t*	seg;
	reg uchar	*laddr, *baddr;
	reg Block_t*	bp;
	reg size_t	size;
	reg long	offset;
	reg Vmdata_t*	vd = vm->data;
	reg int		local;

	if(!(local = vd->mode&VM_TRUST))
	{	GETLOCAL(vd,local);
		if(ISLOCK(vd,local))
			return -1L;
		SETLOCK(vd,local);
	}

	offset = -1L;
	for(seg = vd->seg; seg; seg = seg->next)
	{	laddr = (uchar*)SEGBLOCK(seg);
		baddr = seg->baddr-sizeof(Head_t);
		if((uchar*)addr < laddr || (uchar*)addr >= baddr)
			continue;

		size = ROUND(vd->pool,ALIGN);

		/* see if already freed */
		for(bp = vd->free; bp; bp = SEGLINK(bp))
		{	if((uchar*)bp <= (uchar*)addr &&
			   (uchar*)addr < ((uchar*)bp)+size)
				goto done;
		}

		offset = (uchar*)addr - (laddr + (((uchar*)addr-laddr)/size)*size);
		goto done;
	}

done :
	CLRLOCK(vd,local);
	return offset;
}

#if __STD_C
static int poolfree(Vmalloc_t* vm, Void_t* data )
#else
static int poolfree(vm, data)
Vmalloc_t*	vm;
Void_t*		data;
#endif
{
	reg Block_t*	bp;
	reg Vmdata_t*	vd = vm->data;
	reg int		local;

	if(!data)
		return -1;

	if(!(local = vd->mode&VM_TRUST))
	{	if(ISLOCK(vd,0) || vd->pool <= 0)
			return -1;

		if(KPVADDR(vm,data,pooladdr) != 0)
		{	if(vm->disc->exceptf)
				(void)(*vm->disc->exceptf)(vm,VM_BADADDR,data,vm->disc);
			return -1;
		}

		SETLOCK(vd,0);
	}

	bp = (Block_t*)data;
	SEGLINK(bp) = vd->free;
	vd->free = bp;

	if(!local && (vd->mode&VM_TRACE) && _Vmtrace)
		(*_Vmtrace)(vm, (uchar*)data, NIL(uchar*), vd->pool);

	CLRLOCK(vd,local);
	return 0;
}

#if __STD_C
static Void_t* poolresize(Vmalloc_t* vm, Void_t* data, size_t size, int type )
#else
static Void_t* poolresize(vm, data, size, type )
Vmalloc_t*	vm;
Void_t*		data;
size_t		size;
int		type;
#endif
{
	reg Vmdata_t*	vd = vm->data;

	NOTUSED(type);

	if(!data)
		return poolalloc(vm,size);
	else if(size == 0)
	{	(void)poolfree(vm,data);
		return NIL(Void_t*);
	}

	if(!(vd->mode&VM_TRUST) )
	{	if(ISLOCK(vd,0) )
			return NIL(Void_t*);

		if(size != vd->pool || KPVADDR(vm,data,pooladdr) != 0)
		{	if(vm->disc->exceptf)
				(void)(*vm->disc->exceptf)(vm,VM_BADADDR,data,vm->disc);
			return NIL(Void_t*);
		}

		if((vd->mode&VM_TRACE) && _Vmtrace)
			(*_Vmtrace)(vm, (uchar*)data, (uchar*)data, size);
	}

	return data;
}

#if __STD_C
static long poolsize(Vmalloc_t* vm, Void_t* addr)
#else
static long poolsize(vm, addr)
Vmalloc_t*	vm;
Void_t*		addr;
#endif
{
	return pooladdr(vm,addr) == 0 ? (long)vm->data->pool : -1L;
}

#if __STD_C
static int poolcompact(Vmalloc_t* vm)
#else
static poolcompact(vm)
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

	for(seg = vd->seg; seg; seg = next)
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
static Void_t* poolalign(Vmalloc_t* vm, size_t size, size_t align)
#else
static Void_t* poolalign(vm, size, align)
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

/* Public interface */
Vmethod_t _Vmpool =
{
	poolalloc,
	poolresize,
	poolfree,
	pooladdr,
	poolsize,
	poolcompact,
	poolalign,
	VM_MTPOOL
};
