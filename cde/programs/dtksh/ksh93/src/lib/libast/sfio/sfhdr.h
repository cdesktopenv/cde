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
/* $XConsortium: sfhdr.h /main/3 1995/11/01 18:30:18 rswiston $ */
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
#ifndef _SFHDR_H
#define _SFHDR_H	1

/*	Internal definitions for sfio.
**	Written by Kiem-Phong Vo (07/16/90)
**	AT&T Bell Laboratories
*/

#include	"FEATURE/sfio"
#include	"sfio_t.h"

/* file system info */
#if _PACKAGE_ast

#if defined(__STDPP__directive) && defined(__STDPP__hide)
__STDPP__directive pragma pp:hide ecvt fcvt getpagesize
#else
#define ecvt		______ecvt
#define fcvt		______fcvt
#define getpagesize	______getpagesize
#endif

#include	<ast.h>
#include	<ast_tty.h>
#include	<ls.h>

#if defined(__STDPP__directive) && defined(__STDPP__hide)
__STDPP__directive pragma pp:nohide ecvt fcvt getpagesize
#else
#undef	ecvt
#undef	fcvt
#undef	getpagesize
#endif

#if _mem_st_blksize_stat
#define _stat_blksize	1
#endif

#if _lib_localeconv && _hdr_locale
#define _lib_locale	1
#endif

#else

#if __STD_C
#include	<stdarg.h>
#include	<stddef.h>
#else
#include	<varargs.h>
#endif

#if _hdr_time
#include	<time.h>
#endif
#if _sys_types
#include	<sys/types.h>
#endif
#if _hdr_stat
#include	<stat.h>
#else
#if _sys_stat
#include	<sys/stat.h>
#endif
#endif

#include	<fcntl.h>

#ifndef F_SETFD
#ifndef FIOCLEX
#if _hdr_filio
#include	<filio.h>
#else
#if _sys_filio
#include	<sys/filio.h>
#endif /*_sys_filio*/
#endif /*_hdr_filio*/
#endif /*_FIOCLEX*/
#endif /*F_SETFD*/

#endif /*_PACKAGE_ast*/

#if _lib_poll_fd_1 || _lib_poll_fd_2
#define _lib_poll	1
#endif

#if _lib_select
#include	<sys/time.h>
#if _lib_poll
#undef _lib_poll
#endif
#endif

#if _lib_poll
#include	<poll.h>
#endif

#if _stream_peek
#include	<stropts.h>
#endif

#if _socket_peek
#include	<sys/socket.h>
#endif

#if _typ_long_double
#define Double_t	long double
#define _sfio_cvt	1
#else
#define Double_t	double
#	if _i386_cvt
#	define _sfio_cvt	0
#	else
#	define _sfio_cvt	1
#	endif
#endif

#include	<errno.h>
#include	<ctype.h>

/* NOTE: these flags share the same space with the public flags */
#define SF_MMAP		0010000	/* in memory mapping mode		*/
#define SF_PROCESS	0020000	/* this stream is sfpopen		*/
#define SF_BOTH		0040000	/* both read/write			*/
#define SF_HOLE		0100000	/* a hole of zero's was created		*/

/* bits for the mode field, SF_INIT defined in sfio_t.h */
#define SF_RC		0000010	/* peeking for a record			*/
#define SF_RV		0000020	/* reserve a block of data without read	*/
#define SF_LOCK		0000040	/* stream is locked for io op		*/
#define SF_PUSH		0000100	/* stream has been pushed		*/
#define SF_POOL		0000200	/* stream is in a pool but not current	*/
#define SF_PEEK		0000400	/* there is a pending peek		*/
#define SF_PKRD		0001000	/* did a peek read			*/
#define SF_GETR		0002000	/* did a getr on this stream		*/
#define SF_SYNCED	0004000	/* stream was synced			*/
#define SF_STDIO	0010000	/* given up the buffer to stdio		*/
#define SF_AVAIL	0020000	/* was closed, available for reuse	*/
#define SF_OPEN		0040000	/* file descriptor was from sfopen()	*/
#define SF_LOCAL	0100000	/* sentinel for a local call		*/

#ifdef DEBUG
_BEGIN_EXTERNS_
extern void abort();
_END_EXTERNS_
#define ASSERT(p)	((p) ? 0 : (abort(),0) )
#else
#define ASSERT(p)
#endif

/* short-hands */
#define uchar		unsigned char
#define ulong		unsigned long
#define uint		unsigned int
#define reg		/*NO register keyword*/
#define REG		reg

#define SECOND		1000	/* millisecond units */

#ifndef S_IFMT
#define S_IFMT	0
#endif
#ifndef S_IFDIR
#define S_IFDIR	0
#endif
#ifndef S_IFREG
#define S_IFREG	0
#endif
#ifndef S_IFCHR
#define S_IFCHR	0
#endif
#ifndef S_IFIFO
#define S_IFIFO	0
#endif

#ifndef S_ISDIR
#define S_ISDIR(m)	(((m)&S_IFMT) == S_IFDIR)
#endif
#ifndef S_ISREG
#define S_ISREG(m)	(((m)&S_IFMT) == S_IFREG)
#endif
#ifndef S_ISCHR
#define S_ISCHR(m)	(((m)&S_IFMT) == S_IFCHR)
#endif

#ifndef S_ISFIFO
#ifdef S_IFIFO
#define S_ISFIFO(m)	(((m)&S_IFIFO) == S_IFIFO)
#else
#define S_ISFIFO(m)	(0)
#endif
#endif

/* set close-on-exec */
#ifdef F_SETFD
#ifndef FD_CLOEXEC
#define FD_CLOEXEC	1
#endif /*FD_CLOEXEC*/
#define SETCLOEXEC(fd)	((void)fcntl((fd),F_SETFD,FD_CLOEXEC))
#else
#ifdef FIOCLEX
#define SETCLOEXEC(fd)	((void)ioctl((fd),FIOCLEX,0))
#else
#define SETCLOEXEC(fd)
#endif /*FIOCLEX*/
#endif /*F_SETFD*/

/* see if we can use memory mapping for io */
#if _lib_mmap
#	if _hdr_mman
#		include	<mman.h>
#	else
#		if _sys_mman
#			include	<sys/mman.h>
#		endif
#	endif
#endif

/* function to get the decimal point for local environment */
#if _lib_locale
#include	<locale.h>
#define GETDECIMAL(dc,lv) \
	(dc ? dc : \
	 (dc = ((lv = localeconv()) && lv->decimal_point && *lv->decimal_point) ? \
	   *lv->decimal_point : '.' ) )
#else
#define GETDECIMAL(dc,lv)	('.')
#endif

/* stream pool structure. */
typedef struct _sfpl_
{	struct _sfpl_*	next;
	int		mode;		/* type of pool			*/
	int		s_sf;		/* size of pool array		*/
	int		n_sf;		/* number currently in pool	*/
	Sfio_t**	sf;		/* array of streams		*/
	Sfio_t*		array[3];	/* start with 3			*/
} Sfpool_t;

/* reserve buffer structure */
typedef struct _rsrv_
{	struct _rsrv_*	next;		/* link list			*/
	Sfio_t*		sf;		/* stream associated with	*/
	int		slen;		/* last string length		*/
	int		size;		/* buffer size			*/
	uchar		data[1];	/* data buffer			*/
} Sfrsrv_t;

/* extension structures for sfvprintf/sfvscanf */
typedef int(*Argf_s)_ARG_((char,char*,uint));
typedef int(*Extf_s)_ARG_((Sfio_t*,int,int,char**));
typedef int(*Argf_p)_ARG_((int,char*,char*,int));
typedef int(*Extf_p)_ARG_((char*,int,int,char**,int,char*,int));
typedef struct _fa_
{
	char		*form;		/* format string		*/
	va_list		args;		/* corresponding arglist	*/
	union
	{ Argf_s	s;		/* argf for sfvscanf		*/
	  Argf_p	p;		/* argf for sfvprintf		*/
	}		argf;
	union
	{ Extf_s	s;		/* extf for sfvscanf		*/
	  Extf_p	p;		/* extf for sfvprintf		*/
	}		extf;
	struct _fa_	*next;		/* next on the stack		*/
} Fa_t;

/* memory management for the Fa_t structures */
#define FAMALLOC(f)	((f = _Fafree) ? (_Fafree = f->next, f) : \
				(f = (Fa_t*)malloc(sizeof(Fa_t))))
#define FAFREE(f)	(f->next = _Fafree, _Fafree = f)

/* local variables used across sf-functions */
#define _Sfpool		(_Sfextern.sf_pool)
#define _Sffree		(_Sfextern.sf_free)
#define _Fafree		(_Sfextern.fa_free)
#define _Sfpage		(_Sfextern.sf_page)
#define _Sfpmove	(_Sfextern.sf_pmove)
#define _Sfstack	(_Sfextern.sf_stack)
#define _Sfnotify	(_Sfextern.sf_notify)
#define _Sfstdio	(_Sfextern.sf_stdio)
#define _Sfudisc	(&(_Sfextern.sf_udisc))
#define _Sfcleanup	(_Sfextern.sf_cleanup)
#define _Sfexiting	(_Sfextern.sf_exiting)
typedef struct _sfext_
{
	Sfpool_t	sf_pool;
	Sfio_t*		sf_free;
	Fa_t*		fa_free;
	int		sf_page;
	int(*		sf_pmove)_ARG_((Sfio_t*, int));
	Sfio_t*(*	sf_stack)_ARG_((Sfio_t*, Sfio_t*));
	void(*		sf_notify)_ARG_((Sfio_t*, int, int));
	int(*		sf_stdio)_ARG_((Sfio_t*));
	Sfdisc_t	sf_udisc;
	void(*		sf_cleanup)_ARG_((void));
	int		sf_exiting;
} Sfext_t;

/* function to clear an sfio structure */
#define SFCLEAR(f) \
	((f)->next = (f)->endw = (f)->endr = (f)->endb = (f)->data = NIL(uchar*), \
	 (f)->flags = 0, (f)->file = -1, (f)->extent = -1L, (f)->here = 0L, \
	 (f)->getr = 0, (f)->mode = 0, (f)->size = -1, (f)->disc = NIL(Sfdisc_t*), \
	 (f)->pool = NIL(Sfpool_t*), (f)->push = NIL(Sfio_t*))

/* get the real value of a byte in a coded long or ulong */
#define SFUVALUE(v)	(((ulong)(v))&(SF_MORE-1))
#define SFSVALUE(v)	((( long)(v))&(SF_SIGN-1))

/* amount of precision to get in each iteration during coding of doubles */
#define SF_PRECIS	(SF_UBITS-1)

/* grain size for buffer increment */
#define SF_GRAIN	1024
#define SF_PAGE		(SF_GRAIN*sizeof(int)*2)

/* number of pages to memory map at a time */
#define SF_NMAP		8

/* the bottomless bit bucket */
#define DEVNULL		"/dev/null"
#define SFSETNULL(f)	((f)->extent = -1, (f)->flags |= SF_HOLE)
#define SFISNULL(f)	((f)->extent < 0 && ((f)->flags&SF_HOLE) )

/* exception types */
#define SF_EDONE	0	/* stop this operation and return	*/
#define SF_EDISC	1	/* discipline says it's ok		*/
#define SF_ESTACK	2	/* stack was popped			*/
#define SF_ECONT	3	/* can continue normally		*/

#define SETLOCAL(f)	((f)->mode |= SF_LOCAL)
#define GETLOCAL(f,v)	((v) = ((f)->mode&SF_LOCAL), (f)->mode &= ~SF_LOCAL, (v))
#define SFSK(f,a,o,d)	(SETLOCAL(f),sfsk(f,(long)a,o,d))
#define SFRD(f,b,n,d)	(SETLOCAL(f),sfrd(f,(Void_t*)b,n,d))
#define SFWR(f,b,n,d)	(SETLOCAL(f),sfwr(f,(Void_t*)b,n,d))
#define SFSYNC(f)	(SETLOCAL(f),sfsync(f))
#define SFCLOSE(f)	(SETLOCAL(f),sfclose(f))
#define SFFLSBUF(f,n)	(SETLOCAL(f),_sfflsbuf(f,n))
#define SFFILBUF(f,n)	(SETLOCAL(f),_sffilbuf(f,n))
#define SFSETBUF(f,s,n)	(SETLOCAL(f),sfsetbuf(f,s,n))
#define SFWRITE(f,s,n)	(SETLOCAL(f),sfwrite(f,s,n))
#define SFREAD(f,s,n)	(SETLOCAL(f),sfread(f,s,n))
#define SFSEEK(f,p,t)	(SETLOCAL(f),sfseek(f,p,t))
#define SFNPUTC(f,c,n)	(SETLOCAL(f),sfnputc(f,c,n))

/* lock/open a stream */
#define SFMODE(f,l)	((f)->mode & ~(SF_RV|SF_RC|((l) ? SF_LOCK : 0)) )
#define SFLOCK(f,l)	((f)->mode |= SF_LOCK, (f)->endr = (f)->endw = (f)->data)
#define _SFOPEN(f)	((f)->endr=((f)->mode == SF_READ) ? (f)->endb : (f)->data, \
			 (f)->endw=(((f)->mode == SF_WRITE) && !((f)->flags&SF_LINE)) ? \
				      (f)->endb : (f)->data )
#define SFOPEN(f,l)	((l) ? 0 : ((f)->mode &= ~(SF_LOCK|SF_RC|SF_RV), _SFOPEN(f), 0) )

/* check to see if the stream can be accessed */
#define SFFROZEN(f)	((f)->mode&(SF_PUSH|SF_LOCK|SF_PEEK) ? 1 : \
			 ((f)->mode&SF_STDIO) ? (*_Sfstdio)(f) : 0)


/* set discipline code */
#define SFDISC(f,disc,iof,local) \
	{	Sfdisc_t* d; \
		if(!(disc)) \
			d = (disc) = (f)->disc; \
		else 	d = (local) ? (disc) : ((disc) = (disc)->disc); \
		while(d && !(d->iof))	d = d->disc; \
		if(d)	(disc) = d; \
	}

/* fast peek of a stream */
#define _SFAVAIL(f,s,n)	((n) = (f)->endb - ((s) = (f)->next) )
#define SFRPEEK(f,s,n)	(_SFAVAIL(f,s,n) > 0 ? (n) : \
				((n) = SFFILBUF(f,-1), (s) = (f)->next, (n)) )
#define SFWPEEK(f,s,n)	(_SFAVAIL(f,s,n) > 0 ? (n) : \
				((n) = SFFLSBUF(f,-1), (s) = (f)->next, (n)) )

/* malloc and free of streams */
#define SFFREE(f)	(f->push = _Sffree, _Sffree = f)
#define SFALLOC(f)	((f = _Sffree) ? (_Sffree = f->push, f) : \
				   (f = (Sfio_t*)malloc(sizeof(Sfio_t))))

/* more than this for a line buffer, we might as well flush */
#define HIFORLINE	128

/* safe closing function */
#define CLOSE(f)	{ while(close(f) < 0 && errno == EINTR) errno = 0; }

/* string stream extent */
#define SFSTRSIZE(f)	{ reg long s = (f)->next - (f)->data; \
			  if(s > (f)->here) \
			    { (f)->here = s; if(s > (f)->extent) (f)->extent = s; } \
			}

/* control flags for open() */
#ifndef O_CREAT	/* research UNIX */
#define NO_OFLAGS
#define O_CREAT		004
#define O_TRUNC		010
#define O_APPEND	020

#ifndef O_RDONLY
#define	O_RDONLY	000
#endif
#ifndef O_WRONLY
#define O_WRONLY	001
#endif
#ifndef O_RDWR
#define O_RDWR		002
#endif
#endif /*O_CREAT*/

#ifndef O_BINARY
#define O_BINARY	000
#endif
#ifndef O_TEXT
#define O_TEXT		000
#endif

#define	SF_RADIX	64	/* maximum integer conversion base */

#if _PACKAGE_ast
#define SF_MAXINT	INT_MAX
#define SF_MAXLONG	LONG_MAX
#else
#define SF_MAXINT	((int)(((uint)~0) >> 1))
#define SF_MAXLONG	((long)(((ulong)~0L) >> 1))
#endif

/* floating point to ascii conversion */
#define SF_MAXEXP10	6
#define SF_MAXPOW10	(1 << SF_MAXEXP10)
#if _typ_long_double
#define SF_FDIGITS	1024		/* max allowed fractional digits */
#define SF_IDIGITS	(8*1024)	/* max number of digits in int part */
#else
#define SF_FDIGITS	256		/* max allowed fractional digits */
#define SF_IDIGITS	1024		/* max number of digits in int part */
#endif
#define SF_MAXDIGITS	(((SF_FDIGITS+SF_IDIGITS)/sizeof(int) + 1)*sizeof(int))

/* tables for numerical translation */
#define _Sfpos10	(_Sftable.sf_pos10)
#define _Sfneg10	(_Sftable.sf_neg10)
#define _Sfdec		(_Sftable.sf_dec)
#define _Sfv36		(_Sftable.sf_v36)
#define _Sfvmax		(_Sftable.sf_vmax)
#define _Sfdigits	(_Sftable.sf_digits)
typedef struct _sftab_
{
	Double_t	sf_pos10[SF_MAXEXP10];	/* positive powers of 10	*/
	Double_t	sf_neg10[SF_MAXEXP10];	/* negative powers of 10	*/
	char		sf_dec[200];	/* ascii reps of values < 100		*/
	char		sf_v36[128];	/* digit translation for base <= 36	*/
	char		sf_vmax[128];	/* digit translation for base > 36	*/
	char*		sf_digits;	/* digits for non-standard bases	*/ 
} Sftab_t;

/* sfucvt() converts decimal integers to ASCII */
#define SFDIGIT(v,scale,digit) \
	{ if(v < 5*scale) \
		if(v < 2*scale) \
			if(v < 1*scale) \
				{ digit = '0'; } \
			else	{ digit = '1'; v -= 1*scale; } \
		else	if(v < 3*scale) \
				{ digit = '2'; v -= 2*scale; } \
			else if(v < 4*scale) \
				{ digit = '3'; v -= 3*scale; } \
			else	{ digit = '4'; v -= 4*scale; } \
	  else	if(v < 7*scale) \
			if(v < 6*scale) \
				{ digit = '5'; v -= 5*scale; } \
			else	{ digit = '6'; v -= 6*scale; } \
		else	if(v < 8*scale) \
				{ digit = '7'; v -= 7*scale; } \
			else if(v < 9*scale) \
				{ digit = '8'; v -= 8*scale; } \
			else	{ digit = '9'; v -= 9*scale; } \
	}
#define sfucvt(v,s,n,list) \
	{ list = _Sfdec; \
	  while((ulong)v >= 10000) \
	  {	n = v; v = ((ulong)v)/10000; n = ((ulong)n) - ((ulong)v)*10000; \
		SFDIGIT(n,1000,s[-4]); \
		SFDIGIT(n,100,s[-3]); \
		*--s = list[(n <<= 1)+1]; \
		*--s = list[n]; \
		s -= 2; \
	  } \
	  if(v >= 100) \
	  {	if(v >= 1000) \
		{	n = 2; \
			SFDIGIT(v,1000,s[-4]); \
		} \
		else	n = 1; \
		SFDIGIT(v,100,s[-3]); \
	  	*--s = list[(v <<= 1)+1]; \
		*--s = list[v]; \
		s -= n; \
	  } \
	  else \
	  {	*--s = list[(v <<= 1)+1]; \
	  	if(v >= 20) \
			*--s = list[v]; \
	  } \
	}

/* handy functions */
#define min(x,y)	((x) < (y) ? (x) : (y))
#define max(x,y)	((x) > (y) ? (x) : (y))

/* fast functions for memory copy and memory clear */
#if _PACKAGE_ast
#define memclear(s,n)	memzero(s,n)
#else
#if _lib_bcopy
#ifndef memcpy
#define memcpy(to,fr,n)	bcopy((fr),(to),(n))
#endif
#endif
#if _lib_bzero
#define memclear(s,n)	bzero((s),(n))
#else
#define memclear(s,n)	memset((s),'\0',(n))
#endif
#endif /*_PACKAGE_ast*/

/* note that MEMCPY advances the associated pointers */
#define MEMCPY(to,fr,n) \
	switch(n) \
	{ default : memcpy((Void_t*)to,(Void_t*)fr,n); to += n; fr += n; break; \
	  case  7 : *to++ = *fr++; \
	  case  6 : *to++ = *fr++; \
	  case  5 : *to++ = *fr++; \
	  case  4 : *to++ = *fr++; \
	  case  3 : *to++ = *fr++; \
	  case  2 : *to++ = *fr++; \
	  case  1 : *to++ = *fr++; \
	  case  0 : break; \
	}
#define MEMSET(s,c,n) \
	switch(n) \
	{ default : memset((char*)s,(char)c,n); s += n; break; \
	  case  7 : *s++ = c; \
	  case  6 : *s++ = c; \
	  case  5 : *s++ = c; \
	  case  4 : *s++ = c; \
	  case  3 : *s++ = c; \
	  case  2 : *s++ = c; \
	  case  1 : *s++ = c; \
	  case  0 : break; \
	}

_BEGIN_EXTERNS_

extern Sfext_t		_Sfextern;
extern Sftab_t		_Sftable;

extern int		_sfpopen _ARG_((Sfio_t*, int, int));
extern int		_sfpclose _ARG_((Sfio_t*));
extern int		_sfmode _ARG_((Sfio_t*, int, int));
extern int		_sftype _ARG_((const char*, int*));
extern int		_sfexcept _ARG_((Sfio_t*, int, int, Sfdisc_t*));
extern Sfrsrv_t*	_sfrsrv _ARG_((Sfio_t*, int));
extern int		_sfsetpool _ARG_((Sfio_t*));
extern void		_sfswap _ARG_((Sfio_t*, Sfio_t*, int));
extern char*		_sfcvt _ARG_((Double_t, int, int*, int*, int));

#if _sfio_cvt
extern Double_t		_sfstrtod _ARG_((const char*, char**));
#define strtod		_sfstrtod
#endif

#ifndef errno
extern int	errno;
#endif
extern double	frexp _ARG_((double, int*));
extern double	ldexp _ARG_((double,int));

extern int	getpagesize _ARG_((void));
extern Void_t*	memccpy _ARG_((Void_t*, const Void_t*, int, size_t));

#if !_PACKAGE_ast
extern void	bcopy _ARG_((const Void_t*, Void_t*, int));
extern void	bzero _ARG_((Void_t*, int));
extern Void_t*	malloc _ARG_((int));
extern Void_t*	realloc _ARG_((Void_t*, int));
extern void	free _ARG_((Void_t*));
extern size_t	strlen _ARG_((const char*));
extern char*	strcpy _ARG_((char*, const char*));

extern Void_t*	memset _ARG_((Void_t*, int, size_t));
extern Void_t*	memchr _ARG_((const Void_t*, int, size_t));
#ifndef memcpy
extern Void_t*	memcpy _ARG_((Void_t*, const Void_t*, size_t));
#endif

#if !_sfio_cvt
extern double	strtod _ARG_((const char*, char**));
#endif

extern void	_exit _ARG_((int));
extern int	atexit _ARG_((void(*)(void)));
extern int	onexit _ARG_((void(*)(void)));
extern int	on_exit _ARG_((void(*)(void), char*));

#if _proto_open && __cplusplus
extern int	open _ARG_((const char*, int, ...));
#endif

extern int	close _ARG_((int));
extern int	read _ARG_((int, Void_t*, int));
extern int	write _ARG_((int, const Void_t*, int));
extern long	lseek _ARG_((int, long, int));
extern int	fstat _ARG_((int, struct stat*));
extern int	dup _ARG_((int));
extern int	unlink _ARG_((const char*));
extern int	isatty _ARG_((int));
extern int	waitpid _ARG_((int,int*,int));
extern int	wait _ARG_((int*));
extern int	sleep _ARG_((int));

#if _lib_select
#if __cplusplus
extern int	select _ARG_((int, fd_set*, fd_set*, fd_set*, const struct timeval*));
#else
extern int	select _ARG_((int, fd_set*, fd_set*, fd_set*, struct timeval*));
#endif
#endif /*_lib_select*/

#if _lib_poll
#if _lib_poll_fd_1
extern int			poll _ARG_((struct pollfd*, ulong, int));
#else
extern int			poll _ARG_((ulong, struct pollfd*, int));
#endif
#endif /*_lib_poll*/

#if _stream_peek
extern int ioctl _ARG_((int, int, ...));
#endif /*_stream_peek*/

#if _socket_peek
#if !__cplusplus
extern int recv _ARG_((int, Void_t*, int, int));
#endif
#endif /*_socket_peek*/

#endif /* _PACKAGE_ast */

#if _lib_poll
#if _lib_poll_fd_1
#define SFPOLL(pfd,n,tm)	poll((pfd),(ulong)(n),(tm))
#else
#define SFPOLL(pfd,n,tm)	poll((ulong)(n),(pfd),(tm))
#endif
#endif /*_lib_poll*/

_END_EXTERNS_

#endif /*_SFHDR_H*/
