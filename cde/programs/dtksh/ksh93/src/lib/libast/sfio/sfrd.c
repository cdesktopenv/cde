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
/* $XConsortium: sfrd.c /main/3 1995/11/01 18:34:43 rswiston $ */
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

/*	Internal function to do a hard read.
**	This knows about discipline and memory mapping, peek read.
**
**	Written by Kiem-Phong Vo (02/11/91)
*/

/* synchronize unseekable write streams */
static void _sfwrsync()
{	reg Sfpool_t*	p;
	reg Sfio_t*	f;
	reg int		n;

	/* sync all pool heads */
	for(p = _Sfpool.next; p; p = p->next)
	{	if(p->n_sf <= 0)
			continue;
		f = p->sf[0];
		if(!SFFROZEN(f) && (f->mode&SF_WRITE) && f->next > f->data &&
		   (f->extent < 0 /*|| (f->flags&SF_SHARE)*/) )
			(void)_sfflsbuf(f,-1);
	}

	/* and all the ones in the discrete pool */
	for(n = 0; n < _Sfpool.n_sf; ++n)
	{	f = _Sfpool.sf[n];
		if(!SFFROZEN(f) && (f->mode&SF_WRITE) && f->next > f->data &&
		   (f->extent < 0 /*|| (f->flags&SF_SHARE)*/) )
			(void)_sfflsbuf(f,-1);
	}
}

#if __STD_C
int sfrd(reg Sfio_t* f, reg Void_t* buf, reg int n, reg Sfdisc_t* disc)
#else
sfrd(f,buf,n,disc)
reg Sfio_t	*f;
reg Void_t*	buf;
reg int		n;
reg Sfdisc_t	*disc;
#endif
{
	reg long	r;
	reg int		local, string, rcrv, dosync, oerrno;

	GETLOCAL(f,local);
	if((rcrv = f->mode & (SF_RC|SF_RV)) )
		f->mode &= ~(SF_RC|SF_RV);

	if((f->mode&SF_PKRD) || (!local && !(f->mode&SF_LOCK)) )
		return -1;

	/* find the continuation discipline */
	if(!(string = (f->flags&SF_STRING)) )
		SFDISC(f,disc,readf,local);
	dosync = 0;

	for(;;)
	{
		/* stream locked by sfsetfd() */
		if(!string && f->file < 0)
			return 0;

		/* clear current error states */
		f->flags &= ~(SF_EOF|SF_ERROR);

		if(string)
		{	if((r = (f->data+f->extent) - f->next) < 0)
				r = 0;
			if(r > 0)
				return r;
			else	goto do_except;
		}

		/* warn that a read is about to happen */
		if(disc && disc->exceptf && (f->flags&SF_IOCHECK) )
		{	reg int	rv;
			if(local)
				SETLOCAL(f);
			if((rv = _sfexcept(f,SF_READ,n,disc)) > 0)
				n = rv;
			else if(rv < 0)
			{	f->flags |= SF_ERROR;
				return rv;
			}
		}

#ifdef MAP_TYPE
		if(f->flags&SF_MMAP)
		{	reg int		a, mmsz;
			reg uchar*	data;
			struct stat	st;

			/* determine if we have to copy data to buffer */
			if((uchar*)buf >= f->data && (uchar*)buf <= f->endb)
			{	n += f->endb - f->next;
				buf = NIL(char*);
			}

			/* actual seek location */
			if((f->flags&(SF_SHARE|SF_PUBLIC)) == (SF_SHARE|SF_PUBLIC) &&
			   (r = SFSK(f,0L,1,disc)) != f->here)
				f->here = r;
			else	f->here -= f->endb-f->next;

			/* before mapping, make sure we have data to map */
			if((f->flags&SF_SHARE) || (r = f->extent-f->here) < n)
			{	if((r = fstat(f->file,&st)) < 0)
					goto do_except;
				if((r = st.st_size - f->here) <= 0 )
				{	r = 0;	/* eof */
					goto do_except;
				}
			}

			/* make sure current position is page aligned */
			if((a = (int)(f->here%_Sfpage)) != 0)
			{	f->here -= a;
				r += a;
			}

			/* only map what is required */
			if(f->size > n+a)
			{	if(r > f->size)
					r = f->size;
			}
			else if(r > n+a)
				r = n+a;

			mmsz = 0;
			if((data = f->data) && r != (mmsz = f->endb-data))
			{	f->endb = f->endr = f->endw =
				f->next = f->data = NIL(uchar*);
				(void)munmap((caddr_t)data,mmsz);
				data = NIL(uchar*);
			}

			f->data = (uchar*) mmap((caddr_t)data, (int)r,
					(PROT_READ|PROT_WRITE),
					(MAP_PRIVATE|(data ? MAP_FIXED : 0)),
					f->file, f->here);

			if((caddr_t)f->data != (caddr_t)(-1) )
			{	/* success */
				f->next = f->data+a;
				f->endr = f->endb = f->data+r;
				f->endw = f->data;
				f->here += r;

				/* if share-public, make known our seek location */
				if((f->flags&(SF_SHARE|SF_PUBLIC)) ==
				   (SF_SHARE|SF_PUBLIC))
					(void)SFSK(f,f->here,0,disc);

				if(buf)
				{	if(n > (r-a))
						n = (int)(r-a);
					memcpy((char*)buf,(char*)f->next,n);
					f->next += n;
					return n;
				}
				else	return f->endb-f->next;
			}
			else
			{	if(data)
				{	f->endb = f->endr = f->endw =
					f->next = f->data = NIL(uchar*);
					(void)munmap((caddr_t)data,mmsz);
				}

				r = -1;
				f->here += a;
				f->data = NIL(uchar*);

				/* reset seek pointer to its physical location */
				(void)SFSK(f,f->here,0,disc);

				/* make a buffer */
				(void)SFSETBUF(f,(char*)f->tiny,-1);

				if(!buf)
				{	buf = (char*)f->data;
					n = f->size;
				}
			}
		}
#endif

		/* sync unseekable write streams to prevent deadlock */
		if(!dosync && f->extent < 0)
		{	dosync = 1;
			_sfwrsync();
		}

		/* make sure file pointer is right */
		if(f->extent >= 0 && (f->flags&SF_SHARE) )
		{	if(!(f->flags&SF_PUBLIC) )
				f->here = SFSK(f,f->here,0,disc);
			else	f->here = SFSK(f,0L,1,disc);
		}

		oerrno = errno;
		errno = 0;

		if(disc && disc->readf)
		{	/* pass on rcrv to handle possible future continuations */
			if(rcrv)
				f->mode |= rcrv;
			r = (*(disc->readf))(f,buf,n,disc);
			if(rcrv)
				f->mode &= ~rcrv;
		}
		else if(SFISNULL(f))
			r = 0;
		else if(f->extent < 0 && (f->flags&SF_SHARE) && rcrv)
		{	/* try peek read */
			r = sfpkrd(f->file, (char*)buf, n,
				    (rcrv&SF_RC) ? (int)f->getr : -1,
				    -1L, (rcrv&SF_RV) ? 1 : 0);
			if(r > 0)
			{	if(rcrv&SF_RV)
					f->mode |= SF_PKRD;
				else	f->mode |= SF_RC;
			}
		}
		else	r = read(f->file,(char*)buf,n);

		if(errno == 0 )
			errno = oerrno;

		if(local && r > 0)
		{	if(!(f->mode&SF_PKRD))
			{	f->here += r;
				if(f->extent >= 0 && f->extent < f->here)
					f->extent = f->here;
			}
			if((uchar*)buf >= f->data &&
			   (uchar*)buf < f->data+f->size)
				f->endb = f->endr = ((uchar*)buf) + r;
		}

		if(r > 0)
			return (int)r;

	do_except:
		if(local)
			SETLOCAL(f);
		switch(_sfexcept(f,SF_READ,(int)r,disc))
		{
		case SF_ECONT :
			continue;
		case SF_EDONE :
			return local ? 0 : r;
		case SF_EDISC :
			if(!local && !string)
				continue;
			/* else fall thru */
		case SF_ESTACK :
			return -1;
		}
	}
}
