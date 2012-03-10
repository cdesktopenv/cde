/* $XConsortium: vmset.c /main/2 1996/05/08 20:05:28 drk $ */
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


/*	Set the control flags for a region.
**
**	Written by (Kiem-)Phong Vo, kpv@research.att.com, 01/16/94.
*/
#if __STD_C
vmset(reg Vmalloc_t* vm, int flags, int on)
#else
vmset(vm, flags, on)
reg Vmalloc_t*	vm;	/* region being worked on		*/
int		flags;	/* flags must be in VM_FLAGS		*/
int		on;	/* !=0 if turning on, else turning off	*/
#endif
{
	reg int		mode;
	reg Vmdata_t*	vd = vm->data;

	if(flags == 0 && on == 0)
		return vd->mode;

	if(!(vd->mode&VM_TRUST) )
	{	if(ISLOCK(vd,0))
			return 0;
		SETLOCK(vd,0);
	}

	mode = vd->mode;

	if(on)
		vd->mode |=  (flags&VM_FLAGS);
	else	vd->mode &= ~(flags&VM_FLAGS);

	if(vd->mode&(VM_TRACE|VM_MTDEBUG))
		vd->mode &= ~VM_TRUST;

	CLRLOCK(vd,0);

	return mode;
}
