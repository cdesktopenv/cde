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
/* $XConsortium: sfpkrd.c /main/3 1995/11/01 18:32:27 rswiston $ */
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
#if !_PACKAGE_ast
#ifndef FIONREAD
#if _sys_ioctl
#include	<sys/ioctl.h>
#endif
#endif
#endif

/*	Read/Peek a record from an unseekable device
**
**	Written by Kiem-Phong Vo (03/25/93)
*/

#define STREAM_PEEK	001
#define SOCKET_PEEK	002

#if __STD_C
int sfpkrd(int fd, Void_t* argbuf, int n, int rc, long tm, int action)
#else
sfpkrd(fd, argbuf, n, rc, tm, action)
int	fd;	/* file descriptor */
Void_t*	argbuf;	/* buffer to read data */
int	n;	/* buffer size */
int	rc;	/* record character */
long	tm;	/* time-out */
int	action;	/* >0: peeking,
		   <0: no peeking, if rc>=0, get rec if able to, else just read,
		   =0; no peeking, if rc>=0, must get a rec only
		*/
#endif
{
	reg int		r, ntry, type;
	reg char*	buf = (char*)argbuf;

	if(rc < 0 && tm < 0 && action <= 0)
		return read(fd,buf,n);

	type = (action > 0 || rc >= 0) ? (STREAM_PEEK|SOCKET_PEEK) : 0;
#if !_stream_peek
	type &= ~STREAM_PEEK;
#endif
#if !_socket_peek
	type &= ~SOCKET_PEEK;
#endif

	for(ntry = 0; ntry < 2; ++ntry)
	{
		r = -1;
#if _stream_peek
		if((type&STREAM_PEEK) && (ntry == 1 || tm < 0) )
		{
			struct strpeek	pbuf;
			pbuf.flags = 0;
			pbuf.ctlbuf.maxlen = -1;
			pbuf.ctlbuf.len = 0;
			pbuf.ctlbuf.buf = NIL(char*);
			pbuf.databuf.maxlen = n;
			pbuf.databuf.buf = buf;
			pbuf.databuf.len = 0;

			if((r = ioctl(fd,I_PEEK,&pbuf)) < 0)
			{	if(errno == EINTR)
					return -1;
				type &= ~STREAM_PEEK;
			}
			else
			{	type &= ~SOCKET_PEEK;
				if(r > 0 && (r = pbuf.databuf.len) <= 0)
				{	if(action <= 0)	/* read past eof */
						r = read(fd,buf,1);
					return r;
				}
				if(r == 0)
					r = -1;
				else if(r > 0)
					break;
			}
		}
#endif /* stream_peek */

		if(ntry == 1)
			break;

		/* poll or select to see if data is present.  */
		while(tm >= 0 || action > 0 ||
			/* block until there is data before peeking again */
			((type&STREAM_PEEK) && rc >= 0) ||
			/* let select be interrupted instead of recv which autoresumes */
			(type&SOCKET_PEEK) )
		{	r = -2;
#if _lib_poll
			if(r == -2)
			{
				struct pollfd	po;
				po.fd = fd;
				po.events = POLLIN;
				po.revents = 0;

				if((r = SFPOLL(&po,1,tm)) < 0)
				{	if(errno == EINTR)
						return -1;
					else if(errno == EAGAIN)
					{	errno = 0;
						continue;
					}
					else	r = -2;
				}
				else	r = (po.revents&POLLIN) ? 1 : -1;
			}
#endif /*_lib_poll*/
#if _lib_select
			if(r == -2)
			{
				fd_set		rd;
				struct timeval	tmb, *tmp;
				FD_ZERO(&rd);
				FD_SET(fd,&rd);
				if(tm < 0)
					tmp = NIL(struct timeval*);
				else
				{	tmp = &tmb;
					tmb.tv_sec = tm/SECOND;
					tmb.tv_usec = (tm%SECOND)*SECOND;
				}
				r = select(fd+1,&rd,NIL(fd_set*),NIL(fd_set*),tmp);
				if(r < 0)
				{	if(errno == EINTR)
						return -1;
					else if(errno == EAGAIN)
					{	errno = 0;
						continue;
					}
					else	r = -2;
				}
				else	r = FD_ISSET(fd,&rd) ? 1 : -1;
			}
#endif /*_lib_select*/
			if(r == -2)
			{
#if !_lib_poll && !_lib_select	/* both poll and select cann't be used */
#ifdef FIONREAD			/* quick and dirty check for availability */
				long	nsec = tm < 0 ? 0 : (tm+999)/1000;
				while(nsec > 0 && r < 0)
				{	long	avail = -1;
					if((r = ioctl(fd,FIONREAD,&avail)) < 0)
					{	if(errno == EINTR)
							return -1;
						else if(errno == EAGAIN)
						{	errno = 0;
							continue;
						}
						else	/* ioctl failed completely */
						{	r = -2;
							break;
						}
					}
					else if((r = avail) <= 0)
						r = -1;
					if(r < 0 && nsec-- > 0)
						sleep(1);
				}
#endif
#endif
			}

			if(r > 0)		/* there is data now */
			{	if(action <= 0 && rc < 0)
					return read(fd,buf,n);
				else	r = -1;
			}
			else if(tm >= 0)	/* timeout exceeded */
				return -1;
			else	r = -1;
			break;
		}

#if _socket_peek
		if(type&SOCKET_PEEK)
		{
			while((r = recv(fd,(char*)buf,n,MSG_PEEK)) < 0)
			{	if(errno == EINTR)
					return -1;
				else if(errno == EAGAIN)
				{	errno = 0;
					continue;
				}
				type &= ~SOCKET_PEEK;
				break;
			}
			if(r >= 0)
			{	type &= ~STREAM_PEEK;
				if(r > 0)
					break;
				else	/* read past eof */
				{	if(action <= 0)
						r = read(fd,buf,1);
					return r;
				}
			}
		}
#endif
	}

	if(r < 0)
	{	if(tm >= 0 || action > 0)
			return -1;
		else if(action < 0 || rc < 0)
			return read(fd,buf,n);
		else	/* read 1 byte at a time for a record */
		{	if((r = read(fd,buf,1)) == 1)
			{	while(r < n)
				{	if(read(fd,buf+r,1) <= 0)
						break;
					if(buf[r++] == rc)
						break;
				}
			}
			return r;
		}
	}

	/* successful peek, find the record end */
	if(rc >= 0)
	{	reg char*	sp;	
#if _lib_memchr
		if((sp = (char*)memchr(buf,rc,r)) )
			r = (sp-buf) + 1;
#else
		reg char*	endbuf;
		for(endbuf = (sp = buf)+r; sp < endbuf; )
			if(*sp++ == rc)
				break;
		r = sp - buf;
#endif
	}

	/* advance */
	if(action <= 0)
		r = read(fd,buf,r);

	return r;
}
