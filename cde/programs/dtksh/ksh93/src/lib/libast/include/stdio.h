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
/* $XConsortium: stdio.h /main/3 1995/11/01 17:40:15 rswiston $ */
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
#ifndef _SFSTDIO_H	/* protect against multiple #includes */
#define _SFSTDIO_H	1

#define stdin		sfstdin
#define stdout		sfstdout
#define stderr		sfstderr
#define BUFSIZ		SF_BUFSIZE
#if !defined(__linux__)
#ifdef FILE
#undef FILE
#endif
#define FILE		Sfio_t
#endif

#include	<sfio.h>

#define _IOFBF		0
#define _IONBF		1
#define _IOLBF		2
#define L_ctermid	9
#define L_cuserid	9
#define P_tmpdir	"/usr/tmp/"
#define L_tmpnam	(sizeof(P_tmpdir)+15)

_BEGIN_EXTERNS_
extern char*	ctermid _ARG_((char*));
extern char*	cuserid _ARG_((char*));
extern char*	tmpnam _ARG_((char*));
extern char*	tempnam _ARG_((const char*, const char*));
#ifndef remove
extern int	remove _ARG_((const char*));
#endif
extern void	perror _ARG_((const char*));
extern Sfio_t*	_stdopen _ARG_((int, const char*));
extern char*	_stdgets _ARG_((Sfio_t*, char*, int n, int isgets));
extern int	_stdprintf _ARG_((const char*, ...));
extern int	_stdsprintf _ARG_((char*, const char*, ...));
extern int	_stdscanf _ARG_((const char*, ...));
extern int	_stdsetvbuf _ARG_((Sfio_t*, char*, int, int));
_END_EXTERNS_

#define _SFSIZEOF(s)	(sizeof(s) != sizeof(char*) ? sizeof(s) : BUFSIZ)

#define	printf		_stdprintf
#define _doscan		sfvscanf
#define fdopen		_stdopen
#define fprintf		sfprintf
#define fscanf		sfscanf
#define scanf		_stdscanf
#define setvbuf		_stdsetvbuf
#define sprintf		_stdsprintf
#define snprintf	sfsprintf
#define sscanf		sfsscanf
#define vfprintf	sfvprintf
#define vfscanf		sfvscanf
#define vsprintf	_stdvsprintf
#define vsnprintf	_stdvsnprintf
#define vsscanf		_stdvssanf

#if __cplusplus

inline FILE* fopen(const char* f, const char* m)
	{ return sfopen((Sfio_t*)0,f,m); }
inline FILE* freopen(const char* f, const char* m, FILE* p)
	{ return sfopen(p,f,m); }
inline FILE* popen(const char* cmd, const char* m)
	{ return sfpopen((Sfio_t*)0,cmd,m); }
inline FILE* tmpfile()
	{ return sftmp(SF_BUFSIZE); }
inline int fclose(FILE* f)
	{ return sfclose(f); }
inline int pclose(FILE* f)
	{ return sfclose(f); }

inline int fwrite(const void* p, int s, int n, FILE* f)
	{ return ((_Sfi = sfwrite(f,p,(s)*(n))) <= 0 ? _Sfi : _Sfi/(s)); }
inline int fputc(int c, FILE* f)
	{ return sfputc(f,c); }
inline int putc(int c, FILE* f)
	{ return sfputc(f,c); }
inline int putw(int w, FILE* f)
	{ return (_Sfi = (int)w, sfwrite(f,&_Sfi,sizeof(int)) <= 0 ? 1 : 0); }
inline int putchar(int c)
	{ return sfputc(sfstdout,c); }
inline int fputs(const char* s, FILE* f)
	{ return sfputr(f,s,-1); }
inline int puts(const char* s)
	{ return sfputr(sfstdout,s,'\n'); }
inline int vprintf(const char* fmt, va_list a)
	{ return sfvprintf(sfstdout,fmt,a); }
inline int _doprnt(const char* fmt, va_list a, FILE* f)
	{ return sfvprintf(f,fmt,a); }

inline int fread(void* p, int s, int n, FILE* f)
	{ return ((_Sfi = sfread(f,p,(s)*(n))) <= 0 ? _Sfi : _Sfi/(s)); }
inline int fgetc(FILE* f)
	{ return sfgetc(f); }
inline int getc(FILE* f)
	{ return sfgetc(f); }
inline int getw(FILE* f)
	{ return (sfread(f,&_Sfi,sizeof(int)) == sizeof(int) ? _Sfi : -1); }
inline int getchar()
	{ return sfgetc(sfstdin); }
inline int ungetc(int c, FILE* f)
	{ return sfungetc(f,c); }
inline char* fgets(char* s, int n, FILE* f)
	{ return _stdgets(f,s,n,0); }
inline char* gets(char* s)
	{ return _stdgets(sfstdin,s,_SFSIZEOF(s),1); }
inline int vscanf(const char* f, va_list a)
	{ return sfvscanf(sfstdin,f,a); }

inline int fflush(FILE* f)
	{ return sfsync(f); }
inline int fseek(FILE* f, long o, int t)
	{ return (sfseek(f,o,t) < 0L ? -1 : 0); }
inline void rewind(FILE* f)
	{ (void) sfseek((f),0L,0); }
inline long ftell(FILE* f)
	{ return sftell(f); }
inline int fgetpos(FILE* f, long* pos)
	{ return (*pos = sftell(f)) >= 0 ? 0 : -1; }
inline int fsetpos(FILE* f, long* pos)
	{ return (!pos || *pos < 0 || sfseek(f,*pos,0) != *pos) ? -1 : 0; }
inline void setbuf(FILE* f, char* b)
	{ (void)sfsetbuf(f,b,(b) ? BUFSIZ : 0); }
inline int setbuffer(FILE* f, char* b, int n)
	{ return sfsetbuf(f,b,n) ? 0 : -1; }
inline int setlinebuf(FILE* f)
	{ return sfset(f,SF_LINE,1); }

inline int fileno(FILE* f)
	{ return sffileno(f); }
inline int feof(FILE* f)
	{ return sfeof(f); }
inline int ferror(FILE* f)
	{ return sferror(f); }
inline void clearerr(FILE* f)
	{ (void)(sfclrerr(f),sfclrlock(f)); }

#else

#define fopen(f,m)	sfopen((Sfio_t*)0,(f),(m))
#define freopen(f,m,p)	sfopen((p),(f),(m))
#define popen(cmd,m)	sfpopen((Sfio_t*)0,(cmd),(m))
#define tmpfile()	sftmp(SF_BUFSIZE)
#define fclose(f)	sfclose(f)
#define pclose(f)	sfclose(f)

#define fwrite(p,s,n,f)	((_Sfi = sfwrite((f),(p),(s)*(n))) <= 0 ? _Sfi : _Sfi/(s))
#define fputc(c,f)	sfputc((f),(c))
#define putc(c,f)	sfputc((f),(c))
#define putw(w,f)	(_Sfi = (int)(w), sfwrite((f),&_Sfi,sizeof(int)) <= 0 ? 1 : 0)
#define putchar(c)	sfputc(sfstdout,(c))
#define fputs(s,f)	sfputr((f),(s),-1)
#define puts(s)		sfputr(sfstdout,(s),'\n')
#define vprintf(fmt,a)	sfvprintf(sfstdout,(fmt),(a))
#define _doprnt(fmt,a,f) sfvprintf((f),(fmt),(a))

#define fread(p,s,n,f)	((_Sfi = sfread((f),(p),(s)*(n))) <= 0 ? _Sfi : _Sfi/(s))
#define fgetc(f)	sfgetc(f)
#define getc(f)		sfgetc(f)
#define getw(f)		(sfread((f),&_Sfi,sizeof(int)) == sizeof(int) ? _Sfi : -1)
#define getchar()	sfgetc(sfstdin)
#define ungetc(c,f)	sfungetc((f),(c))
#define fgets(s,n,f)	_stdgets((f),(s),(n),0)
#define gets(s)		_stdgets(sfstdin,(s),_SFSIZEOF(s),1)
#define vscanf(fmt,a)	sfvscanf(sfstdin,(fmt),(a))

#define fflush(f)	sfsync(f)
#define fseek(f,o,t)	(sfseek((f),(o),(t)) < 0L ? -1 : 0)
#define rewind(f)	sfseek((f),0L,0)
#define ftell(f)	sftell(f)
#define fgetpos(f,pos)	((*(pos) = sftell(f)) >= 0 ? 0 : -1)
#define fsetpos(f,pos)	(sfseek(f,*(pos),0) != (*pos) ? -1 : 0)
#define setbuf(f,b)	(void)sfsetbuf((f),(b),(b) ? BUFSIZ : 0)
#define setbuffer(f,b,n) (sfsetbuf((f),(b),(n)) ? 0 : -1)
#define setlinebuf(f)	sfset((f),SF_LINE,1)

#define fileno(f)	sffileno(f)
#define feof(f)		sfeof(f)
#define ferror(f)	sferror(f)
#define clearerr(f)	(void)(sfclrerr(f),sfclrlock(f))

#endif

#endif /* _SFSTDIO_H */
