/* $XConsortium: vmclose.c /main/2 1996/05/08 20:01:42 drk $ */
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

/*	Close down a region.
**
**	Written by (Kiem-)Phong Vo, kpv@research.att.com, 01/16/94.
*/
#if __STD_C
vmclose(Vmalloc_t* vm)
#else
vmclose(vm)
Vmalloc_t*	vm;
#endif
{
	reg Seg_t	*seg, *vmseg;
	reg Vmemory_f	memoryf;
	reg Vmdata_t*	vd = vm->data;

	if(vm == Vmheap)
		return -1;

	if(!(vd->mode&VM_TRUST) && ISLOCK(vd,0))
		return -1;

	if(vm->disc->exceptf &&
	   (*vm->disc->exceptf)(vm,VM_CLOSE,NIL(Void_t*),vm->disc) < 0)
		return -1;

	/* make this region inaccessible until it disappears */
	vd->mode &= ~VM_TRUST;
	SETLOCK(vd,0);

	if((vd->mode&VM_MTPROFILE) && _Vmpfclose)
		(*_Vmpfclose)(vm);

	/* free all non-region segments */
	memoryf = vm->disc->memoryf;
	vmseg = NIL(Seg_t*);
	for(seg = vd->seg; seg; )
	{	reg Seg_t*	next = seg->next;
		if(seg->extent != seg->size)
			(void)(*memoryf)(vm,seg->addr,seg->extent,0,vm->disc);
		else	vmseg = seg;
		seg = next;
	}

	/* this must be done here because even though this region is freed,
	   there may still be others that share this space.
	*/
	CLRLOCK(vd,0);

	/* free the segment that contains the region data */
	if(vmseg)
		(void)(*memoryf)(vm,vmseg->addr,vmseg->extent,0,vm->disc);

	/* free the region itself */
	vmfree(Vmheap,vm);

	return 0;
}
