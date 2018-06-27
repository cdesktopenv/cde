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
/*
 * $TOG: shellscan.C /main/9 1999/10/14 15:05:42 mgreess $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define X_INCLUDE_PWD_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>
#include <codelibs/nl_hack.h>

# include <unistd.h>

#include "stringio.h"
#include "buf.h"
#include <codelibs/shellutils.h>

#include <codelibs/boolean.h>
#include <codelibs/stringx.h>
#include "DtSvcLock.h"

#ifdef XTHREADS
extern "C" {
    extern void XtProcessLock(void);
    extern void XtProcessUnlock(void);
}
#endif

#define ISIDENT(CH) (isalnum(CH) || (CH) == '_')

static _SHXbuf *buf = NULL;
static const char *getvar(const char *var, char *);

// Make this a global someday:
static const char *(*shellvarfn)(const char *, char *) = getvar;

// Parse a sequence of the ksh meta-characters ;&|<> and whitespace
// into a single ksh token.  Return a pointer to the token as a
// string.  All whitespace characters are mapped to a single space
// character.  If ch is not a meta-character, return a NULL pointer.
static char *
parsemeta(int ch, _StringIO &in, char *ifs, unsigned opts, char *meta)
{
    if (ch == '\0')
	return " ";		// whitespace

    _DtSvcProcessLock();
    if (buf->quote() != NOQUOTE) {
	_DtSvcProcessUnlock();
	return NULL;		// normal character
    }

    if (!(opts & SHX_NOSPACE) && strchr(ifs, ch) != NULL) {
	_DtSvcProcessUnlock();
	return " ";		// whitespace
    }

    if (!(opts & SHX_NOMETA))
    {
	int len = 0;

	if (buf->new_token() && isascii(ch) && isdigit(ch))
	    if (in.next() == '<' || in.next() == '>')
	    {
		meta[len++] = (char)ch;
		ch = in.get();
	    }

	switch (meta[len++] = (char)ch, ch)
	{
	case ';': 
	case '&': 
	case '(': 
	case ')': 
	    meta[len++] = (in.next() == ch) ? in.get() : '\0';
	    meta[len] = '\0';
	    _DtSvcProcessUnlock();
	    return meta;
	case '|': 
	    meta[len++] = (in.next() == '|' || in.next() == '&') ?
		in.get() : '\0';
	    meta[len] = '\0';
	    _DtSvcProcessUnlock();
	    return meta;
	case '>': 
	case '<': 
		if (in.next() == ch || in.next() == '&')
		    meta[len++] = (char)in.get();
		meta[len] = '\0';
	    _DtSvcProcessUnlock();
	    return meta;
	}
    }

    _DtSvcProcessUnlock();
    return NULL; // normal character
}

// Takes the name of a variable, and looks up it's value.  Someday,
// this will be replaceable by the user.
static const char *
getvar(const char *name, char *buff)
{
    if (name[0] != '\0' && name[1] == '\0')
	switch (name[0])
	{
	case '$': 
	    sprintf(buff, "%d", getpid());
	    return buff;
	case '#':
	case '?':
	    return "0";
	}

    return getenv(name);
}

// Parse an environment variable name from the _StringIO stream,
// and push its value into the _StringIO stream stack.
static boolean
pushvar(_StringIO &in, char *buff)
{
    _StringIO tmp;
    privbuf_charbuf name;

    tmp = in;
    int ch = tmp.get();		// get the first character after the $

    if (!isascii(ch))
	return FALSE;

    if (ch == '{')
	while ((ch = tmp.get()) != '\0')
	{
	    // ${foo!bar} form, grab everything inside {} as name
	    if (ch == '\\')	// Only \ does quoting inside ${}
		    ch = tmp.get();
	    else if (ch == '}')
		break;
	    name.end() = ch;
	}
    else if (ispunct(ch))
	switch (ch)		// Special non-alnum shell variables
	{
	case '#': 
	case '?': 
	case '$': 
	case '!': 
	case '-': 
	case '*': 
	case '@': 
	case '_':
	    name.end() = ch;
	    break;
	default: 
	    return FALSE;
	}
    else if (isdigit(ch))
	name.end() = ch;	// single-digit variables
    else if (ISIDENT(ch))
    {
	// normal variable
	do
	    name.end() = ch;
	while (isascii(ch = tmp.get()) && ISIDENT(ch));
	tmp.unget();
    }
    else
	return FALSE;

    name.end() = '\0';

    in = tmp;
    in.push(shellvarfn(name.getarr(), buff));
    return TRUE;
}

static boolean
pushenv(_StringIO &in, char const *name)
{
    char *str = getenv(name);
    if (str == NULL || *str == '\0')
	return FALSE;
    else
    {
	in.push(str);
	return TRUE;
    }
}

static boolean
pushtilde(_StringIO &in)
{
    _StringIO tmp;
    int namelen = 0;
    privbuf_charbuf name;

    tmp = in;
    int ch;

    while ((ch = tmp.get()) != '\0' && ch != '/')
	name[namelen++] = ch;
    name[namelen] = '\0';
    tmp.unget();

    char *str = name.getarr();
    switch (*str)
    {
    case '\0': 
	if (!pushenv(tmp, "HOME"))
	    return FALSE;
	break;
    case '+': 
	if (!pushenv(tmp, "PWD"))
	    return FALSE;
	break;
    case '-': 
	if (!pushenv(tmp, "OLDPWD"))
	    return FALSE;
	break;
    default: 
	{
	  _Xgetpwparams pwd_buf;
	  memset((char*) &pwd_buf, 0, sizeof(_Xgetpwparams));
	  struct passwd * pwd_ret = _XGetpwnam(str, pwd_buf);

	  if (pwd_ret == NULL)
	    return FALSE;
	  tmp.push(pwd_ret->pw_dir);
	}
	break;
    }

    in = tmp;
    return TRUE;
}

void
pushgrave(_StringIO &in, const char endchar, boolean quotes, privbuf_charbuf &result)
{
    int ch;
    char quote = NOQUOTE;
    privbuf_charbuf cmd;

    do
    {
	ch = in.get();

	if (quotes)
	    switch (ch)
	    {
	    case '"': 
		quote = DOUBLEQUOTE;
		continue;
	    case '\'': 
		if (quote == '"')
		    break;	// not recognized inside of ""
		do
		    cmd.end() = ch;
		while ((ch = in.get()) != '\'' && ch != '\0');
		cmd.end() = '\'';
		quote = NOQUOTE;
		continue;
	    case '\\': 
		cmd.end() = ch;
		ch = in.get();
		if (ch != '\0')
		    cmd.end() = ch;
		continue;
	    }

	if (ch == endchar)
	    ch = '\0';

	cmd.end() = ch;
    } while (ch != '\0');

    result.reset();

    FILE *fp = popen(cmd.getarr(), "r");
    if (fp == NULL)
	return;
    while ((ch = getc(fp)) != EOF)
	result.end() = ch;
    pclose(fp);

    // Remove trailing newline, if any
    long end = result.size() - 1;
    if (result[end] == '\n')
	result.reset(end);

    result.end() = '\0';
    in.push(result.getarr());
}

char const *const *
shellscan(char const *str, int *argc, unsigned opts)
{
    if (opts & SHX_COMPLETE)
	opts |= SHX_NOSPACE | SHX_NOMETA;

    char *ifs = getenv("IFS");

    if (ifs == NULL)
	ifs = " \t\n";

    _DtSvcProcessLock();
    if (buf == NULL)
	buf = new _SHXbuf;
    buf->reset((boolean)!(opts & SHX_NOGLOB), (boolean)(opts & SHX_COMPLETE));

    _StringIO in(str);
    int ch;
    char buff[10], meta_buff[4];
    privbuf_charbuf result;

    do
    {
	ch = in.get();

	// Don't recognize special characters if this is a shell
	// variable or command substitution.
	if (!in.in_expansion())
	{
	    // Handle quoting rules, setting the flag array and
	    // quote variable appropriately.
	    if (!(opts & SHX_NOQUOTES))
		switch (ch)
		{
		case '"': 
		    buf->quote(DOUBLEQUOTE);
		    continue;
		case '\'': 
		    if (buf->quote() == DOUBLEQUOTE)
			break;	// not recognized inside of ""
		    buf->quote(SINGLEQUOTE);
		    while ((ch = in.get()) != '\'' && ch != '\0')
			buf->append(ch);
		    buf->quote(SINGLEQUOTE);
		    continue;
		case '\\': 
		    ch = in.get();
		    if (ch == '\n') // ignore \<newline>
			continue;
		    if (ch == '\0')
		    {
#if defined(__aix)  /* Our Macro doesn't like '\\' (ignores rest of line) */
			buf->append('\\',
                           SINGLEQUOTE);
#else
			buf->append('\\', SINGLEQUOTE);
#endif
			break;
		    }
		    if (buf->quote() == NOQUOTE)
		    {
			buf->append(ch, SINGLEQUOTE);
			continue;
		    }
		    else
		    {
			// inside "", \ only quotes these 4 characters:
			switch (ch)
			{
			case '$': 
			case '\\': 
			case '`': 
			case '"': 
			    buf->append(ch, SINGLEQUOTE);
			    continue;
			default: 
			    // treat the \ and the following char normally
			    buf->append('\\');
			    break;
			}
		    }
		    break;
		}

	    if (!(opts & SHX_NOCMD))
		switch (ch)
		{
		case '`': 
		    pushgrave(in, '`', (boolean)!(opts & SHX_NOQUOTES), result);
		    continue;
		case '$': 
		    if (in.next() != '(')
			break;
		    in.get();	// skip the '('
		    pushgrave(in, ')', (boolean)!(opts & SHX_NOQUOTES), result);
		    continue;
		}

	    if (ch == '~' && buf->new_token() && buf->quote() == NOQUOTE)
		if (!(opts & SHX_NOTILDE))
		{
		    if (pushtilde(in))
			continue;
		    buf->append('~');
		    continue;
		}

	    if (ch == '$' && !(opts & SHX_NOVARS))
	    {
		if (pushvar(in, buff))
		    continue;
		buf->append('$');
		continue;
	    }
	}

	// If the next item is an unquoted whitespace character or
	// metacharacter token, terminate the current token.  The NUL
	// character is considered to be whitespace.
	{
	    int curr_opts = opts;

	    if (in.in_expansion())
		curr_opts |= SHX_NOMETA;

	    char *meta = parsemeta(ch, in, ifs, curr_opts, meta_buff);

	    if (meta != NULL)	// is it a meta-character?
	    {
		// Terminate current token, if any
		if (!buf->new_token())
		    buf->append('\0');
		if (*meta == ' ')   // whitespace
		{
		    // ignore contiguous whitespace chars
		    if (buf->new_token())
			continue;
		}
		else		// append the metachar token
		    buf->append(meta);
		continue;
	    }
	}

	buf->append(ch);
    } while (ch != '\0');

    if (argc != NULL)
	*argc = buf->ntokens();

    _DtSvcProcessUnlock();
    return ( (char const *const *) buf->vector() );
/* !!! error 1325: `)' missing at end of input */
}
