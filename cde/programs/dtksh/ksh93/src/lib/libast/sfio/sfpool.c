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
/* $XConsortium: sfpool.c /main/3 1995/11/01 18:32:53 rswiston $ */
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

/*	Management of pools of streams.
**	If pf is not nil, f is pooled with pf and f becomes current;
**	otherwise, f is isolated from its pool. flag can be one of
**	0 or SF_SHARE.
**
**	Written by Kiem-Phong Vo (6/27/90).
*/

/* Note that we do not free the space for a pool once it is allocated.
** This is to prevent memory faults in calls such as sfsync(NULL) that walk the pool
** link list and during such walks may free up streams&pools. Free pools will be
** reused in newpool().
*/
#if __STD_C
static void delpool(reg Sfpool_t* p)
#else
static void delpool(p)
reg Sfpool_t	*p;
#endif
{
	if(p->s_sf && p->sf != p->array)
		free((Void_t*)p->sf);
}

#if __STD_C
static Sfpool_t* newpool(reg int mode)
#else
static Sfpool_t* newpool(mode)
reg int	mode;
#endif
{
	reg Sfpool_t	*p, *last;

	/* look to see if there is a free pool */
	for(last = &_Sfpool, p = _Sfpool.next; p; last = p, p = p->next)
		if(p->n_sf == 0)
			break;
	if(p)
	{	last->next = p->next;
		while(last->next)
			last = last->next;
	}
	else if(!(p = (Sfpool_t*) malloc(sizeof(Sfpool_t))) )
		return NIL(Sfpool_t*);

	/* note that the new pool is added at the end of the pool list so that
	   if this was done during a walk, we'll see this pool eventually.
	*/
	last->next = p;

	p->next = NIL(Sfpool_t*);
	p->mode = mode&SF_SHARE;
	p->n_sf = 0;
	p->s_sf = sizeof(p->array)/sizeof(p->array[0]);
	p->sf = p->array;

	return p;
}

/* move a stream to head */
#if __STD_C
static int _sfphead(Sfpool_t* p, Sfio_t* f, int n)
#else
static _sfphead(p, f, n)
Sfpool_t*	p;	/* the pool			*/
Sfio_t*		f;	/* the stream			*/
int		n;	/* current position in pool	*/
#endif
{
	reg Sfio_t*	head;
	reg int		k, w, v;

	if(n == 0)
		return 0;

	head = p->sf[0];
	if(SFFROZEN(head) )
		return -1;

	if(p->mode&SF_SHARE)	/* shared streams */
	{	if(head->mode != SF_WRITE && _sfmode(head,SF_WRITE,0) < 0)
			return -1;
		/**/ASSERT((f->mode&(SF_WRITE|SF_POOL)) == (SF_WRITE|SF_POOL) );
		/**/ASSERT(f->next == f->data);

		v = head->next - head->data;	/* pending data		*/
		if((k = v - (f->endb-f->data)) <= 0)
			k = 0;
		else	/* try to write out amount exceeding f's capacity */
		{	SFLOCK(head,0);
			w = SFWR(head,head->data,k,head->disc);
			SFOPEN(head,0);
			if(w == k)
				v -= k;
			else	/* write failed, recover buffer then quit */
			{	if(w > 0)
				{	v -= w;
					memcpy((Void_t*)head->data,
						(Void_t*)(head->data+w),v);
				}
				head->next = head->data+v;
				return -1;
			}
		}

		/* move data from head to f */
		memcpy((Void_t*)f->data,(Void_t*)(head->data+k),v);
		f->mode &= ~SF_POOL;
		f->next = f->data+v;
		f->endw = f->endb;
	}
	else
	{	SFLOCK(head,0); v = SFSYNC(head); SFOPEN(head,0);
		if(v < 0)
			return v;
		f->mode &= ~SF_POOL;
		_SFOPEN(f);
	}

	head->mode |= SF_POOL;
	head->next = head->endr = head->endw = head->data;

	p->sf[n] = head;
	p->sf[0] = f;
	return 0;
}

/* delete a stream from its pool */
#if __STD_C
static void _sfpdelete(Sfpool_t* p, Sfio_t* f, int n)
#else
static void _sfpdelete(p, f, n)
Sfpool_t*	p;	/* the pool		*/
Sfio_t*		f;	/* the stream		*/
int		n;	/* position in pool	*/
#endif
{
	p->n_sf -= 1;
	for(; n < p->n_sf; ++n)
		p->sf[n] = p->sf[n+1];

	f->pool = NIL(Sfpool_t*);
	f->mode &= ~SF_POOL;
	if(!SFFROZEN(f))
		_SFOPEN(f);
}

#if __STD_C
static int _sfpmove(reg Sfio_t* f, reg int type)
#else
static int _sfpmove(f,type)
reg Sfio_t*	f;
reg int		type;	/* <0 : deleting, 0: move-to-front, >0: inserting */
#endif
{
	reg Sfpool_t*	p;
	reg Sfio_t*	head;
	reg int		n;

	if(type > 0)
		return _sfsetpool(f);

	/* find f position in pool */
	if(!(p = f->pool) )
		return -1;
	for(n = p->n_sf-1; n >= 0; --n)
		if(p->sf[n] == f)
			break;
	/**/ASSERT(n >= 0);
	if(n < 0)
		return -1;

	if(type == 0)	/* move to front */
		return _sfphead(p,f,n);

	head = p->sf[0];
	_sfpdelete(p,f,n);
	if(p->n_sf <= 1 && p != &_Sfpool)
	{	/**/ASSERT(p->n_sf == 1);
		_sfpdelete(p,p->sf[0],0);
		delpool(p);
	}

	if(p->n_sf > 0 && p != &_Sfpool && p->sf[0] != head)
	{	/* make sure that head of pool does not have SF_POOL */
		for(n = 0; n < p->n_sf; ++n)
			if(!SFFROZEN(p->sf[n]) )
				break;

		if(n <= 0 || n >= p->n_sf)
			f = p->sf[0];
		else
		{	f = p->sf[n];
			p->sf[n] = p->sf[0];
			p->sf[0] = f;
		}

		f->mode &= ~SF_POOL;
		if(!SFFROZEN(f))
			_SFOPEN(f);
	}

	return 0;
}

#if __STD_C
Sfio_t* sfpool(reg Sfio_t* f, reg Sfio_t* pf, reg int mode)
#else
Sfio_t* sfpool(f,pf,mode)
reg Sfio_t*	f;
reg Sfio_t*	pf;
reg int		mode;
#endif
{
	reg Sfpool_t*	p;
	reg Sfio_t*	rf;

	_Sfpmove = _sfpmove;

	/* throw away ungetc data */
	if(f && f->disc == _Sfudisc)
		(void)sfclose((*_Sfstack)(f,NIL(Sfio_t*)));
	if(pf && pf->disc == _Sfudisc)
		(void)sfclose((*_Sfstack)(pf,NIL(Sfio_t*)));

	if(f == pf)	/* every stream is in a pool with itself */
		return f;

	if(!f)	/* return the head of the pool that f is in */
		return (!pf->pool || pf->pool == &_Sfpool) ? pf : pf->pool->sf[0];

	/* already isolated */
	if(!pf && (!f->pool || f->pool == &_Sfpool) )
		return NIL(Sfio_t*);

	/* always use current pool mode */
	if(pf && pf->pool && pf->pool != &_Sfpool)
		mode = pf->pool->mode;

	/* a SF_SHARE pool can only have write streams */
	if((mode&SF_SHARE) && pf && f)
	{	if(!(f->mode&SF_WRITE) && _sfmode(f,SF_WRITE,0) < 0)
			return NIL(Sfio_t*);
		if(!(pf->mode&SF_WRITE) && _sfmode(pf,SF_WRITE,0) < 0)
			return NIL(Sfio_t*);
	}

	/* see if these can be manipulated */
	if((f->mode&SF_RDWR) != f->mode && _sfmode(f,0,0) < 0)
		return NIL(Sfio_t*);
	if(pf && (pf->mode&SF_RDWR) != pf->mode && _sfmode(pf,0,0) < 0)
		return NIL(Sfio_t*);

	/* return either the new pool or the old pool for deletion */
	p = f->pool;
	if(!(rf = pf) && (rf = p->sf[0]) == f)
		rf = p->sf[1];

	_sfpmove(f,-1);	/* isolate f from current pool */

	if(!pf)		/* add to the discrete pool */
	{	f->pool = &_Sfpool;
		_sfpmove(f,1);
		return rf;
	}

	if((p = pf->pool) == &_Sfpool)	/* making a new pool */
	{	_sfpmove(pf,-1);
		p = newpool(mode);
		pf->pool = p;
		_sfpmove(pf,1);
	}

	if(f->pool != p)
	{	/* insert f into the pool and make it the head */
		if(mode&SF_SHARE)
		{	/* empty the buffer to start clean */
			if(f->next > f->data)
				{ SFLOCK(f,0); SFSYNC(f); SFOPEN(f,0); }
			f->next = f->endr = f->endw = f->data;
		}
		f->pool = p;
		_sfpmove(f,1);
		_sfpmove(f,0);
	}

	return rf;
}
