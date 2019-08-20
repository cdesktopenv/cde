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
/* $XConsortium: vmprofile.c /main/2 1996/05/08 20:04:45 drk $ */
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

/*	Method to profile space usage.
**
**	Written by (Kiem-)Phong Vo, kpv@research.att.com, 03/23/94.
*/

#define PFHASH(pf)	((pf)->data.data.hash)
#define PFVM(pf)	((pf)->data.data.vm)
#define PFFILE(pf)	((pf)->data.data.fm.file)
#define PFLINE(pf)	((pf)->line)
#define PFNAME(pf)	((pf)->data.f)
#define PFNALLOC(pf)	((pf)->data.data.nalloc)
#define PFALLOC(pf)	((pf)->data.data.alloc)
#define PFNFREE(pf)	((pf)->data.data.nfree)
#define PFFREE(pf)	((pf)->data.data.free)
#define PFREGION(pf)	((pf)->data.data.region)
#define PFMAX(pf)	((pf)->data.data.fm.max)

typedef struct _pfdata_	Pfdata_t;
struct _pfdata_
{	ulong		hash;	/* hash value			*/
	union
	{ char*		file;	/* file name			*/
	  ulong		max;	/* max busy space for region	*/
	} fm;
	Vmalloc_t*	vm;	/* region alloc from 		*/
	Pfobj_t*	region;	/* pointer to region record	*/
	ulong		nalloc;	/* number of alloc calls	*/
	ulong		alloc;	/* amount allocated		*/
	ulong		nfree;	/* number of free calls		*/
	ulong		free;	/* amount freed			*/
};
struct _pfobj_
{	Pfobj_t*	next;	/* next in linked list	*/
	int		line;	/* line #, 0 for name holder	*/
	union
	{
	Pfdata_t	data;
	char		f[1];	/* actual file name		*/
	} data;
};

static Pfobj_t**	Pftable;	/* hash table		*/
#define PFTABLE		1019		/* table size		*/
static Vmalloc_t*	Vmpf;		/* heap for our own use	*/

#if __STD_C
static Pfobj_t* pfsearch(Vmalloc_t* vm, char* file, int line )
#else
static Pfobj_t* pfsearch(vm, file, line)
Vmalloc_t*	vm;	/* region allocating from			*/
char*		file;	/* the file issuing the allocation request	*/
int		line;	/* line number					*/
#endif
{
	reg Pfobj_t		*pf, *last;
	reg ulong		h;
	reg int			n;
	reg char*		cp;

	if(!Vmpf && !(Vmpf = vmopen(Vmdcheap,Vmpool,0)) )
		return NIL(Pfobj_t*);

	/* make hash table; PFTABLE'th slot hold regions' records */
	if(!Pftable)
	{	if(!(Pftable = (Pfobj_t**)vmalloc(Vmheap,(PFTABLE+1)*sizeof(Pfobj_t*))) )
			return NIL(Pfobj_t*);
		for(n = PFTABLE; n >= 0; --n)
			Pftable[n] = NIL(Pfobj_t*);
	}

	/* see if it's there with a combined hash value of vm,file,line */
	h = line + (((ulong)vm)>>4);
	for(cp = file; *cp; ++cp)
		h += (h<<7) + ((*cp)&0377) + 987654321L;
	n = (int)(h%PFTABLE);
	for(last = NIL(Pfobj_t*), pf = Pftable[n]; pf; last = pf, pf = pf->next)
		if(PFLINE(pf) == line && PFVM(pf) == vm &&
		   strcmp(PFFILE(pf),file) == 0)
			break;

	/* insert if not there yet */
	if(!pf)
	{	reg Pfobj_t*	fn;
		reg Pfobj_t*	pfvm;
		reg ulong	hn;

		/* first get/construct the file name slot */
		hn = 0;
		for(cp = file; *cp; ++cp)
			hn += (hn<<7) + ((*cp)&0377) + 987654321L;
		n = (int)(hn%PFTABLE);
		for(fn = Pftable[n]; fn; fn = pf->next)
			if(PFLINE(fn) < 0 && strcmp(PFNAME(fn),file) == 0)
				break;
		if(!fn)
		{	reg size_t	s;
			s = sizeof(Pfobj_t) - sizeof(Pfdata_t) + strlen(file) + 1;
			if(!(fn = (Pfobj_t*)vmalloc(Vmheap,s)) )
				return NIL(Pfobj_t*);
			fn->next = Pftable[n];
			Pftable[n] = fn;
			PFLINE(fn) = -1;
			strcpy(PFNAME(fn),file);
		}

		/* get region record; note that these are ordered by vm */
		last = NIL(Pfobj_t*);
		for(pfvm = Pftable[PFTABLE]; pfvm; last = pfvm, pfvm = pfvm->next)
			if(vm >= PFVM(pfvm))
				break;
		if(!pfvm || PFVM(pfvm) > vm)
		{	if(!(pfvm = (Pfobj_t*)vmalloc(Vmpf,sizeof(Pfobj_t))) )
				return NIL(Pfobj_t*);
			if(last)
			{	pfvm->next = last->next;
				last->next = pfvm;
			}
			else
			{	pfvm->next = Pftable[PFTABLE];
				Pftable[PFTABLE] = pfvm;
			}
			PFNALLOC(pfvm) = PFALLOC(pfvm) = 0;
			PFNFREE(pfvm) = PFFREE(pfvm) = 0;
			PFMAX(pfvm) = 0;
			PFVM(pfvm) = vm;
			PFLINE(pfvm) = 0;
		}

		if(!(pf = (Pfobj_t*)vmalloc(Vmpf,sizeof(Pfobj_t))) )
			return NIL(Pfobj_t*);
		n = (int)(h%PFTABLE);
		pf->next = Pftable[n];
		Pftable[n] = pf;
		PFLINE(pf) = line;
		PFFILE(pf) = PFNAME(fn);
		PFREGION(pf) = pfvm;
		PFVM(pf) = vm;
		PFNALLOC(pf) = 0;
		PFALLOC(pf) = 0;
		PFNFREE(pf) = 0;
		PFFREE(pf) = 0;
		PFHASH(pf) = h;
	}
	else if(last)	/* do a move-to-front */
	{	last->next = pf->next;
		pf->next = Pftable[n];
		Pftable[n] = pf;
	}

	return pf;
}

#if __STD_C
static void pfclose(Vmalloc_t* vm)
#else
static void pfclose(vm)
Vmalloc_t*	vm;
#endif
{
	reg int		n;
	reg Pfobj_t	*pf, *next, *last;

	/* free all records related to region vm */
	for(n = PFTABLE; n >= 0; --n)
	{	for(last = NIL(Pfobj_t*), pf = Pftable[n]; pf; )
		{	next = pf->next;

			if(PFLINE(pf) >= 0 && PFVM(pf) == vm)
			{	if(last)
					last->next = next;
				else	Pftable[n] = next;
				vmfree(Vmpf,pf);
			}
			else	last = pf;

			pf = next;
		}
	}
}

#if __STD_C
static void pfsetinfo(Vmalloc_t* vm, uchar* data, size_t size, char* file, int line)
#else
static void pfsetinfo(vm, data, size, file, line)
Vmalloc_t*	vm;
uchar*		data;
size_t		size;
char*		file;
int		line;
#endif
{
	reg Pfobj_t*	pf;
	reg ulong	s;

	/* let vmclose knows that there are records for region vm */
	_Vmpfclose = pfclose;

	if(!file || line <= 0)
	{	file = "";
		line = 0;
	}

	if((pf = pfsearch(vm,file,line)) )
	{	PFALLOC(pf) += size;
		PFNALLOC(pf) += 1;
	}
	PFOBJ(data) = pf;
	PFSIZE(data) = size;

	if(pf)
	{	/* update region statistics */
		pf = PFREGION(pf);
		PFALLOC(pf) += size;
		PFNALLOC(pf) += 1;
		if((s = PFALLOC(pf) - PFFREE(pf)) > PFMAX(pf) )
			PFMAX(pf) = s;
	}
}

/* sort by file names and line numbers */
#if __STD_C
static Pfobj_t* pfsort(Pfobj_t* pf)
#else
static Pfobj_t* pfsort(pf)
Pfobj_t*	pf;
#endif
{
	reg Pfobj_t	*one, *two, *next;
	reg int		cmp;

	if(!pf->next)
		return pf;

	/* partition to two equal size lists */
	one = two = NIL(Pfobj_t*);
	while(pf)
	{	next = pf->next;
		pf->next = one;
		one = pf;

		if((pf = next) )
		{	next = pf->next;
			pf->next = two;
			two = pf;
			pf = next;
		}
	}

	/* sort and merge the lists */
	one = pfsort(one);
	two = pfsort(two);
	for(pf = next = NIL(Pfobj_t*);; )
	{	/* make sure that the "<>" file comes first */	
		if(PFLINE(one) == 0 && PFLINE(two) == 0)
			cmp = PFVM(one) > PFVM(two) ? 1 : -1;
		else if(PFLINE(one) == 0)
			cmp = -1;
		else if(PFLINE(two) == 0)
			cmp = 1;
		else if((cmp = strcmp(PFFILE(one),PFFILE(two))) == 0)
		{	cmp = PFLINE(one) - PFLINE(two);
			if(cmp == 0)
				cmp = PFVM(one) > PFVM(two) ? 1 : -1;
		}

		if(cmp < 0)
		{	if(!pf)
				pf = one;
			else	next->next = one;
			next = one;
			if(!(one = one->next) )
			{	if(two)
					next->next = two;
				return pf;
			}
		}
		else
		{	if(!pf)
				pf = two;
			else	next->next = two;
			next = two;
			if(!(two = two->next) )
			{	if(one)
					next->next = one;
				return pf;
			}
		}
	}
}

#if __STD_C
char* pfsummary(char* buf, ulong na, ulong sa, ulong nf, ulong sf, ulong max, ulong size)
#else
char* pfsummary(buf, na, sa, nf, sf, max, size)
char*	buf;
ulong	na;
ulong	sa;
ulong	nf;
ulong	sf;
ulong	max;
ulong	size;
#endif
{
	buf = (*_Vmstrcpy)(buf,"n_alloc", '=');
	buf = (*_Vmstrcpy)(buf, (*_Vmitoa)(na,-1), ':');
	buf = (*_Vmstrcpy)(buf,"n_free", '=');
	buf = (*_Vmstrcpy)(buf, (*_Vmitoa)(nf,-1), ':');
	buf = (*_Vmstrcpy)(buf,"s_alloc", '=');
	buf = (*_Vmstrcpy)(buf, (*_Vmitoa)(sa,-1), ':');
	buf = (*_Vmstrcpy)(buf,"s_free", '=');
	buf = (*_Vmstrcpy)(buf, (*_Vmitoa)(sf,-1), ':');
	if(max > 0)
	{	buf = (*_Vmstrcpy)(buf,"max_busy", '=');
		buf = (*_Vmstrcpy)(buf, (*_Vmitoa)(max,-1), ':');
		buf = (*_Vmstrcpy)(buf,"extent", '=');
		buf = (*_Vmstrcpy)(buf, (*_Vmitoa)(size,-1), ':');
	}
	*buf++ = '\n';

	return buf;
}

/* print profile data */
#if __STD_C
int vmprofile(Vmalloc_t* vm, int fd)
#else
vmprofile(vm, fd)
Vmalloc_t*	vm;
int		fd;
#endif
{
	reg Pfobj_t	*pf, *list, *next, *last;
	reg int		n;
	reg ulong	nalloc, alloc, nfree, free;
	reg Seg_t*	seg;
	char		buf[1024], *bufp, *endbuf;
#define INITBUF()	(bufp = buf, endbuf = buf+sizeof(buf)-128)
#define CHKBUF()	(bufp >= endbuf ? (write(fd,buf,bufp-buf), bufp=buf) : bufp)
#define FLSBUF()	(bufp > buf ? write(fd,buf,bufp-buf) : 0)

	if(fd < 0)
		return -1;

	/* initialize functions from vmtrace.c that we use below */
	if((n = vmtrace(-1)) >= 0)
		vmtrace(n);

	alloc = free = nalloc = nfree = 0;
	list = NIL(Pfobj_t*);
	for(n = PFTABLE-1; n >= 0; --n)
	{	for(pf = Pftable[n], last = NIL(Pfobj_t*); pf; )
		{	next = pf->next;

			if(PFLINE(pf) < 0  || (vm && vm != PFVM(pf)) )
			{	last = pf;
				goto next_pf;
			}

			/* remove from hash table */
			if(last)
				last->next = next;
			else	Pftable[n] = next;

			/* put on output list */
			pf->next = list;
			list = pf;
			nalloc += PFNALLOC(pf);
			alloc += PFALLOC(pf);
			nfree += PFNFREE(pf);
			free += PFFREE(pf);

		next_pf:
			pf = next;
		}
	}

	INITBUF();
	bufp = (*_Vmstrcpy)(bufp,"ALLOCATION USAGE SUMMARY", ':');
	bufp = pfsummary(bufp,nalloc,alloc,nfree,free,0,0);

	/* print regions' summary data */
	for(pf = Pftable[PFTABLE]; pf; pf = pf->next)
	{	if(vm && PFVM(pf) != vm)
			continue;
		alloc = 0;
		for(seg = PFVM(pf)->data->seg; seg; seg = seg->next)
			alloc += seg->extent;
		bufp = (*_Vmstrcpy)(bufp,"region", '=');
		bufp = (*_Vmstrcpy)(bufp, (*_Vmitoa)(ULONG(PFVM(pf)),0), ':');
		bufp = pfsummary(bufp,PFNALLOC(pf),PFALLOC(pf),
				 PFNFREE(pf),PFFREE(pf),PFMAX(pf),alloc);
	}

	/* sort then output detailed profile */
	list = pfsort(list);
	for(pf = list; pf; )
	{	/* compute summary for file */
		alloc = free = nalloc = nfree = 0;
		for(last = pf; last; last = last->next)
		{	if(strcmp(PFFILE(last),PFFILE(pf)) != 0)
				break;
			nalloc += PFNALLOC(pf);
			alloc += PFALLOC(last);
			nfree += PFNFREE(last);
			free += PFFREE(last);
		}
		CHKBUF();
		bufp = (*_Vmstrcpy)(bufp,"file",'=');
		bufp = (*_Vmstrcpy)(bufp,PFFILE(pf)[0] ? PFFILE(pf) : "<>" ,':');
		bufp = pfsummary(bufp,nalloc,alloc,nfree,free,0,0);

		while(pf != last)	/* detailed data */
		{	CHKBUF();
			bufp = (*_Vmstrcpy)(bufp,"\tline",'=');
			bufp = (*_Vmstrcpy)(bufp, (*_Vmitoa)(PFLINE(pf),-1), ':');
			bufp = (*_Vmstrcpy)(bufp, "region", '=');
			bufp = (*_Vmstrcpy)(bufp, (*_Vmitoa)(ULONG(PFVM(pf)),0), ':');
			bufp = pfsummary(bufp,PFNALLOC(pf),PFALLOC(pf),
					 PFNFREE(pf),PFFREE(pf),0,0);

			/* reinsert into hash table */
			next = pf->next;
			n = (int)(PFHASH(pf)%PFTABLE);
			pf->next = Pftable[n];
			Pftable[n] = pf;
			pf = next;
		}
	}

	FLSBUF();
	return 0;
}

#if __STD_C
static Void_t* pfalloc(Vmalloc_t* vm, size_t size)
#else
static Void_t* pfalloc(vm, size)
Vmalloc_t*	vm;
size_t		size;
#endif
{
	reg size_t	s;
	reg Void_t*	data;
	reg char*	file;
	reg int		line;
	reg Vmdata_t*	vd = vm->data;

	VMFILELINE(file,line);
	if(!(vd->mode&VM_TRUST) && ISLOCK(vd,0))
		return NIL(Void_t*);
	SETLOCK(vd,0);

	s = ROUND(size,ALIGN) + PF_EXTRA;
	if(!(data = KPVALLOC(vm,s,(*(Vmbest->allocf))) ) )
		goto done;

	pfsetinfo(vm,(uchar*)data,size,file,line);

	if(!(vd->mode&VM_TRUST) && (vd->mode&VM_TRACE) && _Vmtrace)
	{	_Vmfile = file; _Vmline = line;
		(*_Vmtrace)(vm,NIL(uchar*),(uchar*)data,size);
	}
done:
	CLRLOCK(vd,0);
	return data;
}

#if __STD_C
static int pffree(Vmalloc_t* vm, Void_t* data)
#else
static int pffree(vm, data)
Vmalloc_t*	vm;
Void_t*		data;
#endif
{
	reg Pfobj_t*	pf;
	reg size_t	s;
	reg char*	file;
	reg int		line;
	reg Vmdata_t*	vd = vm->data;

	VMFILELINE(file,line);

	if(!data)
		return 0;

	if(!(vd->mode&VM_TRUST) )
	{	if(ISLOCK(vd,0))
			return -1;
		SETLOCK(vd,0);
	}

	if(KPVADDR(vm,data,Vmbest->addrf) != 0 )
	{	if(vm->disc->exceptf)
			(void)(*vm->disc->exceptf)(vm,VM_BADADDR,data,vm->disc);
		CLRLOCK(vd,0);
		return -1;
	}

	pf = PFOBJ(data);
	s = PFSIZE(data);
	if(pf)
	{	PFNFREE(pf) += 1;
		PFFREE(pf) += s;
		pf = PFREGION(pf);
		PFNFREE(pf) += 1;
		PFFREE(pf) += s;
	}

	if(!(vd->mode&VM_TRUST) && (vd->mode&VM_TRACE) && _Vmtrace)
	{	_Vmfile = file; _Vmline = line;
		(*_Vmtrace)(vm,(uchar*)data,NIL(uchar*),s);
	}

	CLRLOCK(vd,0);
	return (*(Vmbest->freef))(vm,data);
}

#if __STD_C
static Void_t* pfresize(Vmalloc_t* vm, Void_t* data, size_t size, int flags)
#else
static Void_t* pfresize(vm, data, size, flags)
Vmalloc_t*	vm;
Void_t*		data;
size_t		size;
int		flags;
#endif
{
	reg Pfobj_t*	pf;
	reg size_t	s, news;
	reg Void_t*	addr;
	reg char*	file;
	reg int		line;
	reg Vmdata_t*	vd = vm->data;

	if(!data)
		return pfalloc(vm,size);
	else if(size == 0)
	{	(void)pffree(vm,data);
		return NIL(Void_t*);
	}

	VMFILELINE(file,line);
	if(!(vd->mode&VM_TRUST))
	{	if(ISLOCK(vd,0))
			return NIL(Void_t*);
		SETLOCK(vd,0);
	}

	if(KPVADDR(vm,data,Vmbest->addrf) != 0 )
	{	if(vm->disc->exceptf)
			(void)(*vm->disc->exceptf)(vm,VM_BADADDR,data,vm->disc);
		CLRLOCK(vd,0);
		return NIL(Void_t*);
	}

	pf = PFOBJ(data);
	s = PFSIZE(data);

	news = ROUND(size,ALIGN) + PF_EXTRA;
	if((addr = KPVRESIZE(vm,data,news,flags,Vmbest->resizef)) )
	{	if(pf)
		{	PFFREE(pf) += s;
			PFNFREE(pf) += 1;
			pf = PFREGION(pf);
			PFFREE(pf) += s;
			PFNFREE(pf) += 1;
			pfsetinfo(vm,(uchar*)addr,size,file,line);
		}

		if(!(vd->mode&VM_TRUST) && (vd->mode&VM_TRACE) && _Vmtrace)
		{	_Vmfile = file; _Vmline = line;
			(*_Vmtrace)(vm,(uchar*)data,(uchar*)addr,size);
		}
	}
	else if(pf)	/* reset old info */
	{	PFALLOC(pf) -= s;
		PFNALLOC(pf) -= 1;
		pf = PFREGION(pf);
		PFALLOC(pf) -= s;
		PFNALLOC(pf) -= 1;
		file = PFFILE(pf);
		line = PFLINE(pf);
		pfsetinfo(vm,(uchar*)data,s,file,line);
	}

	CLRLOCK(vd,0);
	return addr;
}

#if __STD_C
static long pfsize(Vmalloc_t* vm, Void_t* addr)
#else
static long pfsize(vm, addr)
Vmalloc_t*	vm;
Void_t*		addr;
#endif
{
	return (*Vmbest->addrf)(vm,addr) != 0 ? -1L : PFSIZE(addr);
}

#if __STD_C
static long pfaddr(Vmalloc_t* vm, Void_t* addr)
#else
static long pfaddr(vm, addr)
Vmalloc_t*	vm;
Void_t*		addr;
#endif
{
	return (*Vmbest->addrf)(vm,addr);
}

#if __STD_C
static int pfcompact(Vmalloc_t* vm)
#else
static int pfcompact(vm)
Vmalloc_t*	vm;
#endif
{
	return (*Vmbest->compactf)(vm);
}

#if __STD_C
static Void_t* pfalign(Vmalloc_t* vm, size_t size, size_t align)
#else
static Void_t* pfalign(vm, size, align)
Vmalloc_t*	vm;
size_t		size;
size_t		align;
#endif
{
	reg size_t	s;
	reg Void_t*	data;
	reg char*	file;
	reg int		line;
	reg Vmdata_t*	vd = vm->data;

	VMFILELINE(file,line);

	if(!(vd->mode&VM_TRUST) && ISLOCK(vd,0))
		return NIL(Void_t*);
	SETLOCK(vd,0);

	s = (size <= TINYSIZE ? TINYSIZE : ROUND(size,ALIGN)) + PF_EXTRA;
	if(!(data = KPVALIGN(vm,s,align,Vmbest->alignf)) )
		goto done;

	pfsetinfo(vm,(uchar*)data,size,file,line);

	if(!(vd->mode&VM_TRUST) && (vd->mode&VM_TRACE) && _Vmtrace)
	{	_Vmfile = file; _Vmline = line;
		(*_Vmtrace)(vm,NIL(uchar*),(uchar*)data,size);
	}
done:
	CLRLOCK(vd,0);
	return data;
}

Vmethod_t	_Vmprofile =
{
	pfalloc,
	pfresize,
	pffree,
	pfaddr,
	pfsize,
	pfcompact,
	pfalign,
	VM_MTPROFILE
};
