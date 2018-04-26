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
/* $XConsortium: error.c /main/3 1995/11/01 17:55:12 rswiston $ */
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
#include <ast.h>
#include <ctype.h>
#include <error.h>
#include <namval.h>
#include <sig.h>
#include <stk.h>
#include <times.h>

#if _DLL_INDIRECT_DATA && !_DLL
static Error_info_t	error_info_data =
#else
Error_info_t		error_info =
#endif

{ 2, exit, write };

#if _DLL_INDIRECT_DATA && !_DLL
Error_info_t		error_info = &error_info_data;
#endif

#define OPT_CORE	1
#define OPT_FD		2
#define OPT_LIBRARY	3
#define OPT_MASK	4
#define OPT_SYSTEM	5
#define OPT_TIME	6
#define OPT_TRACE	7

static const Namval_t		options[] =
{
	"core",		OPT_CORE,
	"debug",	OPT_TRACE,
	"fd",		OPT_FD,
	"library",	OPT_LIBRARY,
	"mask",		OPT_MASK,
	"system",	OPT_SYSTEM,
	"time",		OPT_TIME,
	"trace",	OPT_TRACE,
	0,		0
};

/*
 * called by stropt() to set options
 */

static int
setopt __PARAM__((__V_* a, const __V_* p, register int n, register const char* v), (a, p, n, v)) __OTORP__(__V_* a; const __V_* p; register int n; register const char* v;){
	NoP(a);
	if (p) switch (((Namval_t*)p)->value)
	{
	case OPT_CORE:
		if (n) switch (*v)
		{
		case 'e':
		case 'E':
			error_info.core = ERROR_ERROR;
			break;
		case 'f':
		case 'F':
			error_info.core = ERROR_FATAL;
			break;
		case 'p':
		case 'P':
			error_info.core = ERROR_PANIC;
			break;
		default:
			error_info.core = strtol(v, NiL, 0);
			break;
		}
		else error_info.core = 0;
		break;
	case OPT_FD:
		error_info.fd = n ? strtol(v, NiL, 0) : -1;
		break;
	case OPT_LIBRARY:
		if (n) error_info.set |= ERROR_LIBRARY;
		else error_info.clear |= ERROR_LIBRARY;
		break;
	case OPT_MASK:
		if (n) error_info.mask = strtol(v, NiL, 0);
		else error_info.mask = 0;
		break;
	case OPT_SYSTEM:
		if (n) error_info.set |= ERROR_SYSTEM;
		else error_info.clear |= ERROR_SYSTEM;
		break;
	case OPT_TIME:
		error_info.time = n ? 1 : 0;
		break;
	case OPT_TRACE:
		if (n) error_info.trace = -strtol(v, NiL, 0);
		else error_info.trace = 0;
		break;
	}
	return(0);
}

/*
 * print a name with optional delimiter, converting unprintable chars
 */

static void
print __PARAM__((register Sfio_t* sp, register char* name, char* delim), (sp, name, delim)) __OTORP__(register Sfio_t* sp; register char* name; char* delim;){
	register int	c;

	while (c = *name++)
	{
		if (c & 0200)
		{
			c &= 0177;
			sfputc(sp, '?');
		}
		if (c < ' ')
		{
			c += 'A' - 1;
			sfputc(sp, '^');
		}
		sfputc(sp, c);
	}
	if (delim) sfputr(sp, delim, -1);
}

/*
 * print error context FIFO stack
 */

static void
context __PARAM__((register Sfio_t* sp, register Error_context_t* cp), (sp, cp)) __OTORP__(register Sfio_t* sp; register Error_context_t* cp;){
	if (cp->context) context(sp, cp->context);
	if (!(cp->flags & ERROR_SILENT))
	{
		if (cp->id) print(sp, cp->id, NiL);
		if (cp->line > ((cp->flags & ERROR_INTERACTIVE) != 0))
		{
			if (cp->file) sfprintf(sp, ": \"%s\", %s %d", cp->file, ERROR_translate("line", 0), cp->line);
			else sfprintf(sp, "[%d]", cp->line);
		}
		sfputr(sp, ": ", -1);
	}
}

void
error __PARAM__((int level, ...), (va_alist)) __OTORP__(va_dcl)
{ __OTORP__(int level; )
	va_list	ap;

	__VA_START__(ap, level); __OTORP__(level = va_arg(ap, int );)
	errorv(NiL, level, ap);
	va_end(ap);
}

void
errorv __PARAM__((const char* lib, int level, va_list ap), (lib, level, ap)) __OTORP__(const char* lib; int level; va_list ap;){
	register int	n;
	int		fd;
	int		flags;
	char*		s;
	char*		format;

	int		line;
	char*		file;

	if (!error_info.init)
	{
		error_info.init = 1;
		stropt(getenv("ERROR_OPTIONS"), options, sizeof(*options), setopt, NiL);
	}
	if (level > 0)
	{
		flags = level & ~ERROR_LEVEL;
		level &= ERROR_LEVEL;
	}
	else flags = 0;
	if (level < error_info.trace || lib && (error_info.clear & ERROR_LIBRARY) || level < 0 && error_info.mask && !(error_info.mask & (1<<(-level - 1))))
	{
		if (level >= ERROR_FATAL) (*error_info.exit)(level - 1);
		return;
	}
	if (error_info.trace < 0) flags |= ERROR_LIBRARY|ERROR_SYSTEM;
	flags |= error_info.set | error_info.flags;
	flags &= ~error_info.clear;
	if (!lib || !*lib) flags &= ~ERROR_LIBRARY;
	fd = (flags & ERROR_OUTPUT) ? va_arg(ap, int) : error_info.fd;
	if (error_info.write)
	{
		long	off;
		char*	bas;

		bas = stkptr(stkstd, 0);
		if (off = stktell(stkstd)) stkfreeze(stkstd, 0);
		file = error_info.id;
		if (flags & ERROR_USAGE)
		{
			if (flags & ERROR_NOID) sfprintf(stkstd, "       ");
			else sfprintf(stkstd, "%s: ", ERROR_translate("Usage", 0));
			if (file || opt_info.argv && (file = opt_info.argv[0])) print(stkstd, file, " ");
		}
		else
		{
			if (level && !(flags & ERROR_NOID))
			{
				if (error_info.context && level > 0) context(stkstd, error_info.context);
				if (file) print(stkstd, file, (flags & ERROR_LIBRARY) ? " " : ": ");
				if (flags & ERROR_LIBRARY) sfprintf(stkstd, "[%s library]: ", lib);
			}
			if (level > 0 && error_info.line > ((flags & ERROR_INTERACTIVE) != 0))
			{
				if (error_info.file && *error_info.file) sfprintf(stkstd, "\"%s\", ", error_info.file);
				sfprintf(stkstd, "%s %d: ", ERROR_translate("line", 0), error_info.line);
			}
		}
		switch (level)
		{
		case 0:
			break;
		case ERROR_WARNING:
			error_info.warnings++;
			sfprintf(stkstd, "%s: ", ERROR_translate("warning", 0));
			break;
		case ERROR_PANIC:
			error_info.errors++;
			sfprintf(stkstd, "%s: ", ERROR_translate("panic", 0));
			break;
		default:
			if (level < 0)
			{
				s = ERROR_translate("debug", 0);
				if (error_info.trace < -1) sfprintf(stkstd, "%s%d:%s", s, level, level > -10 ? " " : "");
				else sfprintf(stkstd, "%s: ", s);
				if (error_info.time)
				{
					long		d;
					struct tms	us;

					if (error_info.time == 1 || (d = times(&us) - error_info.time) < 0)
						d = error_info.time = times(&us);
					sfprintf(stkstd, " %05lu.%05lu.%05lu ", d, (unsigned long)us.tms_utime, (unsigned long)us.tms_stime);
				}
				for (n = 0; n < error_info.indent; n++)
				{
					sfputc(stkstd, ' ');
					sfputc(stkstd, ' ');
				}
			}
			else error_info.errors++;
			break;
		}
		if (flags & ERROR_SOURCE)
		{
			/*
			 * source ([version], file, line) message
			 */

			file = va_arg(ap, char*);
			line = va_arg(ap, int);
			s = ERROR_translate("line", 0);
			if (error_info.version) sfprintf(stkstd, "(%s: \"%s\", %s %d) ", error_info.version, file, s, line);
			else sfprintf(stkstd, "(\"%s\", %s %d) ", file, s, line);
		}
		format = va_arg(ap, char*);
		if (!(flags & ERROR_USAGE))
			format = ERROR_translate(format, 0);
		sfvprintf(stkstd, format, ap);
		if (!(flags & ERROR_PROMPT))
		{
			if (error_info.auxilliary && !(*error_info.auxilliary)(stkstd, level, flags))
			{
				stkset(stkstd, bas, off);
				return;
			}
			if ((flags & ERROR_SYSTEM) && errno && errno != error_info.last_errno)
			{
				sfprintf(stkstd, " [%s]", fmterror(errno));
				if (error_info.set & ERROR_SYSTEM) errno = 0;
				error_info.last_errno = (level >= 0) ? 0 : errno;
			}
			sfputc(stkstd, '\n');
		}
		n = stktell(stkstd);
		s = stkptr(stkstd, 0);
		if (fd == sffileno(sfstderr) && error_info.write == write) sfwrite(sfstderr, s, n);
		else (*error_info.write)(fd, s, n);
		stkset(stkstd, bas, off);
	}
	if (level >= error_info.core && error_info.core)
	{
#ifndef SIGABRT
#ifdef	SIGQUIT
#define SIGABRT	SIGQUIT
#else
#ifdef	SIGIOT
#define SIGABRT	SIGIOT
#endif
#endif
#endif
#ifdef	SIGABRT
		signal(SIGABRT, SIG_DFL);
		kill(getpid(), SIGABRT);
		pause();
#else
		abort();
#endif
	}
	if (level >= ERROR_FATAL) (*error_info.exit)(level - ERROR_FATAL + 1);
}
