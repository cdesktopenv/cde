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
/* $XConsortium: sfsetbuf.c /main/3 1995/11/01 18:35:58 rswiston $ */
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

/*	Set a (new) buffer for a stream.
**	If size < 0, it is assigned a suitable value depending on the
**	kind of stream. The actual buffer size allocated is dependent
**	on how much memory is available.
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#if __STD_C
Void_t* sfsetbuf(reg Sfio_t* f, reg Void_t* buf, reg int size)
#else
Void_t* sfsetbuf(f,buf,size)
reg Sfio_t*	f;	/* stream to be buffered */
reg Void_t*	buf;	/* new buffer */
reg int		size;	/* buffer size, -1 for default size */
#endif
{
	reg int		sf_malloc;
	struct stat	st;
	reg uchar*	obuf;
	reg Sfdisc_t*	disc;
	reg int		osize, oflags, blksize;
	reg int		justopen, init, okmmap, local;

	GETLOCAL(f,local);

	if(size == 0 && buf)
	{	/* special case to get buffer info */
		_Sfi = (f->flags&SF_MMAP) ? (f->endb-f->data) : f->size;
		return (Void_t*)f->data;
	}

	/* cleanup actions already done, don't allow write buffering any more */
	if(_Sfexiting && !(f->flags&SF_STRING) && (f->mode&SF_WRITE))
		return NIL(Void_t*);

	if((init = f->mode&SF_INIT) )
	{	if(!f->pool && _sfsetpool(f) < 0)
			return NIL(Void_t*);
	}
	else if((f->mode&SF_RDWR) != SFMODE(f,local) && _sfmode(f,0,local) < 0)
		return NIL(Void_t*);

	justopen = f->mode&SF_OPEN;
	f->mode &= ~SF_OPEN;
	if(init)
		f->mode = (f->mode&SF_RDWR)|SF_LOCK;
	else
	{	int	rv;

		/* make sure there is no hidden read data */
		if((f->flags&(SF_PROCESS|SF_READ)) == (SF_PROCESS|SF_READ) &&
		   (f->mode&SF_WRITE) && _sfmode(f,SF_READ,local) < 0)
			return NIL(Void_t*);

		/* synchronize first */
		SFLOCK(f,local); rv = SFSYNC(f); SFOPEN(f,local);
		if(rv < 0)
			return NIL(Void_t*);

		/* turn off the SF_SYNCED bit because buffer is changing */
		f->mode &= ~SF_SYNCED;
	}

	SFLOCK(f,local);

	blksize = 0;
	oflags = f->flags;

	/* see if memory mapping is possible (see sfwrite for SF_BOTH) */
	okmmap = (buf || (f->flags&SF_STRING) || (f->flags&SF_RDWR) == SF_RDWR) ? 0 : 1;

	/* save old buffer info */
#ifdef MAP_TYPE
	if(f->flags&SF_MMAP)
	{	if(f->data)
		{	(void)munmap((caddr_t)f->data,f->endb-f->data);
			f->data = NIL(uchar*);
		}
	} else
#endif
	if(f->data == f->tiny)
	{	f->data = NIL(uchar*);
		f->size = 0;
	}
	obuf  = f->data;
	osize = f->size;

	f->flags &= ~(SF_MMAP|SF_MALLOC);

	/* pure read/string streams must have a valid string */
	if((f->flags&(SF_RDWR|SF_STRING)) == SF_RDSTR && (size < 0 || !buf))
		size = 0;

	/* set disc to the first discipline with a seekf */
	for(disc = f->disc; disc; disc = disc->disc)
		if(disc->seekf)
			break;

	if((init || local) && !(f->flags&SF_STRING))
	{	/* ASSERT(f->file >= 0) */
		st.st_mode = 0;

		/* if has discipline, set size by discipline if possible */
		if(disc)
		{	if((f->here = SFSK(f,0L,1,disc)) < 0)
				goto unseekable;
			else
			{	f->extent = SFSK(f,0L,2,disc);
				(void)SFSK(f,f->here,0,disc);
				goto setbuf;
			}
		}

		/* get file descriptor status */
		if(fstat(f->file,&st) < 0)
			goto unseekable;

#if _stat_blksize	/* preferred io block size */
		if((blksize = (int)st.st_blksize) > 0)
			while((blksize + (int)st.st_blksize) <= SF_PAGE)
				blksize += (int)st.st_blksize;
#endif
		if(S_ISREG(st.st_mode) || S_ISDIR(st.st_mode))
			f->here = justopen ? 0L : SFSK(f,0L,1,f->disc);
		else	f->here = -1;
		if(f->here >= 0)
		{	/* normal file, set file extent */
			f->extent = (long)st.st_size;

			/* don't MMAP directories */
			if(S_ISDIR(st.st_mode) )
				okmmap = 0;

			/* seekable std-devices are share-public by default */
			if(f == sfstdin || f == sfstdout || f == sfstderr)
				f->flags |= SF_SHARE|SF_PUBLIC;
		}
		else
		{
		unseekable:
			f->extent = -1L;
			f->here = 0L;

			if(init)
			{	/* pipe std-devices are automatically shared */
				if(S_ISFIFO(st.st_mode) )
				{	if(f == sfstdin || f == sfstdout || f == sfstderr) 						f->flags |= SF_SHARE;
				}
				else if(S_ISCHR(st.st_mode) )
				{	/* set line mode for terminals */
					if(!(f->flags&SF_LINE) && isatty(f->file))
						f->flags |= SF_LINE;
	
					/* special case /dev/null for efficiency */
					else
					{	reg int	dev, ino;
						dev = (int)st.st_dev;	
						ino = (int)st.st_ino;	
						if(stat(DEVNULL,&st) >= 0 &&
						   dev == st.st_dev && ino == st.st_ino)
						{	SFSETNULL(f);
							blksize = 1024;
						}
					}
				}

				/* initialize save input buffer for r+w streams */
				if(!(f->flags&SF_PROCESS) && (f->flags&SF_BOTH) )
					(void)_sfpopen(f,-1,-1);
			}
		}

		/* set page size, this is also the desired default buffer size */
#if _lib_getpagesize
		if(_Sfpage <= 0)
			_Sfpage = (int)getpagesize();
#endif
		if(_Sfpage <= 0)
			_Sfpage = SF_PAGE;
	}

#ifdef MAP_TYPE
	if(okmmap && size && (f->mode&SF_READ) && f->extent >= 0 )
	{	/* see if we can try memory mapping */
		if(!disc)
			for(disc = f->disc; disc; disc = disc->disc)
				if(disc->readf)
					break;
		if(!disc)
		{	f->flags |= SF_MMAP;
			if(size < 0)
			{	size = (blksize > 0 ? blksize : _Sfpage) * SF_NMAP;
				size = ((size+_Sfpage-1)/_Sfpage)*_Sfpage;
			}
		}
	}
#endif

	/* get buffer space */
setbuf:
	if(size < 0)
	{	/* define a default size suitable for block transfer */
		if(init && osize > 0)
			size = osize;
		else if(f == sfstderr && (f->mode&SF_WRITE))
			size = 0;
		else if((f->flags&SF_STRING) || f->extent < 0)
			size = SF_GRAIN;
		else if(blksize > 0)
			size = blksize;
		else if((f->flags&(SF_READ|SF_BOTH)) == SF_READ &&
			f->extent > 0 && f->extent < _Sfpage )
			size = (((int)f->extent + SF_GRAIN-1)/SF_GRAIN)*SF_GRAIN;
		else	size = _Sfpage;

		buf = NIL(Void_t*);
	}

	sf_malloc = 0;
	if(size > 0 && !buf && !(f->flags&SF_MMAP))
	{	/* try to allocate a buffer */
		if(obuf && size == osize)
		{	buf = (Void_t*)obuf;
			obuf = NIL(uchar*);
			sf_malloc = (oflags&SF_MALLOC);
		}
		if(!buf)
		{	/* do allocation */
			while(!(buf = (Void_t*) malloc(size)) && size > 0)
				size /= 2;
			if(size > 0)
				sf_malloc = SF_MALLOC;
		}
	}

	if(size == 0 && !(f->flags&(SF_MMAP|SF_STRING)) && (f->mode&SF_READ))
	{	/* use the internal buffer */
		size = sizeof(f->tiny);
		buf = (Void_t*)f->tiny;
	}

	/* set up new buffer */
	f->size = size;
	f->next = f->data = f->endr = f->endw = (uchar*)buf;
	f->endb = (f->mode&SF_READ) ? f->data : f->data+size;
	if(f->flags&SF_STRING)
	{	/* these fields are used to test actual size - see sfseek() */
		f->extent = (!sf_malloc && (f->flags&(SF_BOTH|SF_READ))) ? size : 0;
		f->here = 0L;

		/* read+string stream should have all data available */
		if((f->mode&SF_READ) && !sf_malloc)
			f->endb = f->data+size;
	}

	f->flags = (f->flags & ~SF_MALLOC)|sf_malloc;

	if(obuf && obuf != f->data && osize > 0 && (oflags&SF_MALLOC))
	{	free((Void_t*)obuf);
		obuf = NIL(uchar*);
	}

	_Sfi = obuf ? osize : 0;

	SFOPEN(f,local);

	return (Void_t*)obuf;
}
