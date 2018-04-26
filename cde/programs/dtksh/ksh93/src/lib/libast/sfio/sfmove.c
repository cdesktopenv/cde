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
/* $XConsortium: sfmove.c /main/3 1995/11/01 18:31:10 rswiston $ */
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

/*	Move data from one stream to another.
**	This code is written so that it'll work even in the presence
**	of stacking streams, pool, and discipline.
**	If you must change it, be gentle.
**
**	Written by Kiem-Phong Vo (12/07/90)
*/

#if __STD_C
long sfmove(Sfio_t* fr, Sfio_t* fw, long n, reg int rc)
#else
long sfmove(fr,fw,n,rc)
Sfio_t*	fr;	/* moving data from this stream */
Sfio_t*	fw;	/* moving data to this stream */
long		n;	/* number of bytes/records to move. <0 for unbounded move */
reg int		rc;	/* record separator */
#endif
{
	reg uchar	*cp, *next;
	reg int		r, w;
	reg uchar	*endb;
	reg int		direct;
	reg long	n_move;
	uchar		*rbuf = NIL(uchar*);
	int		rsize = 0;
	reg int		frsize;

	if(!fr)
		return 0;
	frsize = fr->size;

	for(n_move = 0; n != 0; )
	{	/* get the streams into the right mode */
		if(fr->mode != SF_READ && _sfmode(fr,SF_READ,0) < 0)
			goto done;

		SFLOCK(fr,0);

		/* flush the write buffer as necessary to make room */
		if(fw)
		{	if(fw->mode != SF_WRITE && _sfmode(fw,SF_WRITE,0) < 0 )
				break;
			SFLOCK(fw,0);
			if(fw->next >= fw->endb ||
			   (fw->next > fw->data && fr->extent < 0 &&
			    (fw->extent < 0 || (fw->flags&SF_SHARE)) ) )
				if(SFFLSBUF(fw,-1) < 0 )
					break;
		}

		/* about to move all, set map to a large amount */
		if(n < 0 && (fr->flags&SF_MMAP))
			fr->size = fr->size*SF_NMAP;

		/* try reading a block of data */
		direct = 0;
		if((r = fr->endb - (next = fr->next)) <= 0)
		{	/* amount of data remained to be read */
			if((w = n > SF_MAXINT ? SF_MAXINT : (int)n) < 0)
			{	if(fr->extent < 0)
					w = fr->data == fr->tiny ? SF_GRAIN : fr->size;
				else if((fr->extent-fr->here) > SF_NMAP*SF_PAGE)
					w = SF_NMAP*SF_PAGE;
				else	w = (int)(fr->extent-fr->here);
			}

			/* use a decent buffer for data transfer but make sure
			   that if we overread, the left over can be retrieved
			*/
			if(!(fr->flags&(SF_MMAP|SF_STRING)) &&
			   (n < 0 || fr->extent >= 0L) )
			{	reg int	maxw = 4*(_Sfpage > 0 ? _Sfpage : SF_PAGE);

				/* direct transfer to a seekable write stream */
				if(fw && fw->extent >= 0 && w <= (fw->endb-fw->next) )
				{	w = fw->endb - (next = fw->next);
					direct = SF_WRITE;
				}
				else if(w > fr->size && maxw > fr->size)
				{	/* making our own buffer */
					if(w >= maxw)
						w = maxw;
					else	w = ((w+fr->size-1)/fr->size)*fr->size;
					if(rsize <= 0 && (rbuf = (uchar*)malloc(w)) )
						rsize = w;
					if(rbuf)
					{	next = rbuf;
						w = rsize;
						direct = SF_STRING;
					}
				}
			}

			if(!direct)
			{	/* make sure we don't read too far ahead */
				if(n > 0 && fr->extent < 0 && (fr->flags&SF_SHARE) )
				{	if(rc >= 0)
					{	/* try peeking a large buffer */
						fr->mode |= SF_RV;
						if((r = SFFILBUF(fr,-1)) > 0)
							goto done_filbuf;
						else /* get a single record */
						{	fr->getr = rc;
							fr->mode |= SF_RC;
							r = -1;
						}
					}
					else if((long)(r = fr->size) > n)
						r = (int)n;
				}
				else	r = -1;
				if((r = SFFILBUF(fr,r)) <= 0)
					break;
			done_filbuf:
				next = fr->next;
			}
			else
			{	/* actual amount to be read */
				if(rc < 0 && n > 0 && n < w)
					w = (int)n;

				if((r = SFRD(fr,next,w,fr->disc)) > 0)
					fr->next = fr->endb = fr->endr = fr->data;
				else if(r == 0)
					break;		/* eof */
				else	goto again;	/* popped stack */
			}
		}

		/* compute the extent of data to be moved */
		endb = next+r;
		if(rc < 0)
		{	if(n > 0)
			{	if(r > n)
					r = (int)n;
				n -= r;
			}
			n_move += r;
			cp = next+r;
		}
		else
		{	/* count records */
			reg int	rdwr = (fr->flags&(SF_BOTH|SF_MALLOC|SF_MMAP));

			if(rdwr)
			{	w = endb[-1];
				endb[-1] = rc;
			}
			else	w = 0;
			for(cp = next; cp < endb; )
			{	/* find the line extent */
				if(rdwr)
					while(*cp++ != rc)
						;
				else	while(r-- && *cp++ != rc)
						;
				if(cp < endb || w == rc)
				{	n_move += 1;
					if(n > 0 && (n -= 1) == 0)
						break;
				}
			}
			if(rdwr)
				endb[-1] = w;
			r = cp-next;
			if(fr->mode&SF_PKRD)
			{	/* advance the read point by proper amount */
				fr->mode &= ~SF_PKRD;
				(void)read(fr->file,(Void_t*)next,r);
				fr->here += r;
				if(!direct)
					fr->endb = cp;
				else	endb = cp;
			}
		}

		if(!direct)
			fr->next += r;
		else if((w = endb-cp) > 0)
		{	/* move left-over to read stream */
			if(w > fr->size)
				w = fr->size;
			memcpy((Void_t*)fr->data,(Void_t*)cp,w);
			fr->endb = fr->data+w;
			if((w = endb - (cp+w)) > 0)
				(void)SFSK(fr,(long)(-w),1,fr->disc);
		}

		if(fw)
		{	if(direct == SF_WRITE)
				fw->next += r;
			else if(r <= (fw->endb-fw->next) )
			{	memcpy((Void_t*)fw->next,(Void_t*)next,r);
				fw->next += r;
			}
			else if((w = SFWRITE(fw,(Void_t*)next,r)) != r)
			{	/* a write error happened */
				if(w > 0)
				{	r -= w;
					if(rc < 0)
						n_move -= r;
				}
				if(fr->extent >= 0L)
					(void)SFSEEK(fr,(long)(-r),1);
				break;
			}
		}

	again:
		SFOPEN(fr,0);
		if(fw)
			SFOPEN(fw,0);
	}

done:
	if(n < 0 && (fr->flags&SF_MMAP) )
		fr->size = frsize;

	if(rbuf)
		free((char*)rbuf);

	SFOPEN(fr,0);
	if(fw)
		SFOPEN(fw,0);

	return n_move;
}
