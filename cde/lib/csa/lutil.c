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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */

/* $XConsortium: lutil.c /main/4 1996/11/21 20:00:35 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <sys/param.h>
#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rpc/rpc.h>
#include <unistd.h>
#if defined(sun) || defined(USL) || defined(__uxp__)
#include <netdb.h>
#include <sys/systeminfo.h>
#endif
#define X_INCLUDE_PWD_H
#define XOS_USE_XT_LOCKING
#if defined(linux)
#undef SVR4
#endif
#include <X11/Xos_r.h>
#if defined(linux)
#define SVR4
#endif

#if !defined(linux)
extern char * strdup(const char *);
#endif

extern char *
_DtCmGetPrefix(char *str, char sep)
{
        char buf[BUFSIZ];
        char *ptr;

        if (str == NULL)
                return(NULL);

        ptr = buf;
        while (*str && *str != sep)
                *ptr++ = *str++;
        if (ptr == buf)
                return(NULL);
        else {
                *ptr = '\0';
                return(strdup(buf));
        }
}

extern char *
_DtCmGetLocalHost()
{
	static char *host = NULL;

	if (host == NULL) {
		host = (char *)malloc(MAXHOSTNAMELEN+1);
#if defined(sun) || defined(USL) || defined(__uxp__)
		(void)sysinfo(SI_HOSTNAME, host, MAXHOSTNAMELEN);
#else
		(void)gethostname(host, MAXHOSTNAMELEN);
#endif /* sun || USL || __uxp__ */
	}

	return (host);
}

extern char *
_DtCmGetLocalDomain(char *hostname)
{
	static char	*domain = NULL;
	char		buf[BUFSIZ], *ptr;
	CLIENT		*cl;

	if (domain == NULL) {
		domain = (char *)malloc(BUFSIZ);
#if defined(sun) || defined(USL) || defined(__uxp__)
		sysinfo(SI_SRPC_DOMAIN, domain, BUFSIZ - 1);
#else
		getdomainname(domain, BUFSIZ - 1);
#endif /* sun || USL || __uxp__ */

		/* check domain name */
		/* this is a hack to find out the domain name that
		 * is acceptable to the rpc interface, e.g.
		 * DGDO.Eng.Sun.COM is returned by sysinfo but
		 * this name is not acceptable to the rpc interface
		 * hence we need to stripe out the first component
		 */
		ptr = domain;
		if (hostname == NULL) hostname = _DtCmGetLocalHost();
		while (1) {
			snprintf(buf, sizeof buf, "%s.%s", hostname, ptr);
			if ((cl = clnt_create(buf, 100068, 5, "udp")) == NULL) {
				ptr = strchr(ptr, '.');
				if (ptr)
					ptr++;
				else
					break;
			} else {
				clnt_destroy(cl);
				break;
			}
		}
		if (ptr && ptr != domain)
			domain = ptr;
	}

	return (domain);
}

extern char *
_DtCmGetHostAtDomain()
{
	static char	*hostname = NULL;
	char		*host;

	if (hostname == NULL) {
		hostname = malloc(BUFSIZ);

		host = _DtCmGetLocalHost();
		if (strchr(host, '.') == NULL)
			snprintf(hostname, BUFSIZ, "%s.%s", host,
				_DtCmGetLocalDomain(host));
		else
			/* XXX strcpy unsafe here */
			strcpy(hostname, host);
	}

	return (hostname);
}

extern char *
_DtCmGetUserName()
{
        static char *name = NULL;
	_Xgetpwparams	pwd_buf;
	struct passwd *	pwd_ret;

        if (name == NULL) {
	  name = malloc(BUFSIZ);

	  if ((pwd_ret = _XGetpwuid(geteuid(), pwd_buf)) == NULL)
	    strcpy(name, "nobody");
	  else
	    strcpy(name, pwd_ret->pw_name);
        }

	return name;
}

/*
 * this routine checks whether the given name is a valid user name
 */
extern boolean_t
_DtCmIsUserName(char *user)
{
	_Xgetpwparams	pwd_buf;
	struct passwd *	pwd_ret;

	pwd_ret = _XGetpwnam(user, pwd_buf);
	if (pwd_ret == NULL)
		return (B_FALSE);
	else
		return (B_TRUE);
}

