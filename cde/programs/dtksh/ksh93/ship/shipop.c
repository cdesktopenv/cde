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
/* $XConsortium: shipop.c /main/4 1996/10/04 15:58:45 drk $ */
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

/* : : generated by proto : : */

#if !defined(__PROTO__)
#if defined(__STDC__) || defined(__cplusplus) || defined(_proto) || defined(c_plusplus)
#if defined(__cplusplus)
#define __MANGLE__	"C"
#else
#define __MANGLE__
#endif
#define __STDARG__
#define __PROTO__(x)	x
#define __OTORP__(x)
#define __PARAM__(n,o)	n
#if !defined(__STDC__) && !defined(__cplusplus)
#if !defined(c_plusplus)
#define const
#endif
#define signed
#define void		int
#define volatile
#define __V_		char
#else
#define __V_		void
#endif
#else
#define __PROTO__(x)	()
#define __OTORP__(x)	x
#define __PARAM__(n,o)	o
#define __MANGLE__
#define __V_		char
#define const
#define signed
#define void		int
#define volatile
#endif
#if defined(__cplusplus) || defined(c_plusplus)
#define __VARARG__	...
#else
#define __VARARG__
#endif
#if defined(__STDARG__)
#define __VA_START__(p,a)	va_start(p,a)
#else
#define __VA_START__(p,a)	va_start(p)
#endif
#endif
static const char id[] = "\n@(#)shipop (AT&T Bell Laboratories) 06/11/92\0\n";

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#define elementsof(x)	(sizeof(x)/sizeof(x[0]))

static char*	cmd = "shipop";
static char*	op;

static int	status;

extern __MANGLE__ int	close __PROTO__((int));
extern __MANGLE__ void	exit __PROTO__((int));
extern __MANGLE__ int	open __PROTO__((const char*, int, ...));
extern __MANGLE__ int	read __PROTO__((int, char*, int));
extern __MANGLE__ int	strcmp __PROTO__((const char*, const char*));
extern __MANGLE__ time_t	time __PROTO__((time_t*));

static void
error __PARAM__((int level, const char* file, const char* msg), (level, file, msg)) __OTORP__(int level; const char* file; const char* msg;){
	fprintf(stderr, "%s: %s", cmd, op);
	if (level == 1) fprintf(stderr, ": warning");
	if (file) fprintf(stderr, ": %s", file);
	if (msg) fprintf(stderr, ": %s", msg);
	fprintf(stderr, "\n");
	if (level > status)
	{
		if (level > 2) exit(level - 2);
		status = level;
	}
}

/*
 * shipop newer [ reference [ file ... ] ]
 */

static int
shipnewer __PARAM__((register char** argv), (argv)) __OTORP__(register char** argv;){
	unsigned long	ref;
	struct stat	st;

	if (!*argv || stat(*argv, &st)) return(1);
	ref = (unsigned long)st.st_mtime;
	while (*++argv)
		if (stat(*argv, &st) || ref < (unsigned long)st.st_mtime) return(1);
	return(0);
}

#define HASHPART(h,c)	(h = (h) * 987654321L + 123456879L + (c))

static char		buf[4096];

static unsigned long
sealfile __PARAM__((char* file, int fd, unsigned long s), (file, fd, s)) __OTORP__(char* file; int fd; unsigned long s;){
	register unsigned char*	b;
	register unsigned char*	e;
	register int		n;

	HASHPART(s, 0);
	while ((n = read(fd, buf, sizeof(buf))) > 0)
	{
		b = (unsigned char*)buf;
		e = b + n;
		while (b < e) HASHPART(s, *b++);
	}
	if (n < 0) error(2, file, "read error");
	return(s);
}

/*
 * shipop seal [ file ... ]
 */

static int
shipseal __PARAM__((register char** argv), (argv)) __OTORP__(register char** argv;){
	register char*	f;
	register int	fd;
	unsigned long	s;

	s = 0;
	if (!*argv) s = sealfile("/dev/stdin", 0, s);
	else while (f = *argv++)
	{
		if (*f == '-' && !*(f + 1)) s = sealfile("/dev/stdin", 0, s);
		else if ((fd = open(f, 0)) < 0) error(2, f, "cannot read");
		else
		{
			s = sealfile(f, fd, s);
			close(fd);
		}
	}
	printf("%08lx\n", s);
	return(status);
}

/*
 * shipop state reference [ file ... | <file-list ]
 */

static int
shipstate __PARAM__((register char** argv), (argv)) __OTORP__(register char** argv;){
	register char*	s;
	register int	c;
	long		ref;
	struct stat	st;

	if (!(s = *argv++) || stat(s, &st)) error(3, (char*)0, "reference file omitted");
	ref = (long)st.st_mtime;
	if (s = *argv++) do
	{
		if (!stat(s, &st))
			printf("%s %ld\n", s, (long)st.st_mtime - ref);
	} while (s = *argv++);
	else do
	{
		s = buf;
		while ((c = getchar()) != EOF && c != ' ' && c != '\n')
			if (s < buf + sizeof(buf) - 1) *s++ = c;
		if (s > buf)
		{
			*s = 0;
			if (!stat(buf, &st))
				printf("%s %ld\n", buf, (long)st.st_mtime - ref);
		}
	} while (c != EOF);
	return(status);
}

/*
 * shipop time [ file ]
 */

static int
shiptime __PARAM__((register char** argv), (argv)) __OTORP__(register char** argv;){
	struct stat	st;
	time_t		date;

	if (*argv && !stat(*argv, &st)) date = st.st_mtime;
	else time(&date);
	printf("%08lx\n", (long)date);
	return(status);
}

/*
 * shipop xap [ file ... ]
 */

static int
shipxap __PARAM__((char** argv), (argv)) __OTORP__(char** argv;){
	error(2, "xap not implemented yet", (char*)0);
	return(1);
}

struct optab
{
	char*	name;
	int	(*func) __PROTO__((char**));
};

static struct optab ops[] =
{
	"newer",	shipnewer,
	"seal",		shipseal,
	"state",	shipstate,
	"time",		shiptime,
	"xap",		shipxap,
};

int
main __PARAM__((int argc, char** argv), (argc, argv)) __OTORP__(int argc; char** argv;){
	register char*	s;
	register int	i;

	if (!(s = *++argv))
		exit(0);
	for (i = 0; i < elementsof(ops); i++)
		if (!strcmp(s, ops[i].name))
		{
			op = ops[i].name;
			exit((*ops[i].func)(argv + 1));
		}
	fprintf(stderr, "Usage: %s %s", cmd, ops[0].name);
	for (i = 1; i < elementsof(ops); i++)
		fprintf(stderr, " | %s", ops[i].name);
	fprintf(stderr, " [ file ... ]\n");
	exit(2);
}
