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
/*******************************************************************************
**
**  util.c
**
**  $XConsortium: util.c /main/12 1996/11/21 19:44:40 drk $
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
**  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/

/*									*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#ifndef lint
#endif

#include <EUSCompat.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pwd.h> 
#include <netdb.h> 
#include <sys/utsname.h> /* SYS_NMLN */
#if defined(sun) || defined(USL) || defined(__uxp__)
#include <sys/systeminfo.h>
#else
#include <sys/dir.h>
#endif /* sun || USL || __uxp__ */
#include <sys/param.h>

#if (defined(USL) || defined(__uxp__)) && !defined(DOM_NM_LN)
#define DOM_NM_LN  BUFSIZ
#endif

#define X_INCLUDE_STRING_H
#define X_INCLUDE_TIME_H
#define XOS_USE_NO_LOCKING
#if defined(linux)
#undef SVR4
#endif
#include <X11/Xos_r.h>

#include <errno.h>
#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

#include "util.h"
#include "cm_tty.h"

extern int _csa_tick_to_iso8601(time_t, char *);
extern int _csa_iso8601_to_tick(char *, time_t*);

extern FILE 	*popen(const char *, const char *);
extern int   	pclose(FILE *);

/*
 *
 *  Function:	cm_def_printer
 *
 *  Purpose:	get the default printer name for SVR4 	
 *
 *  Parameters:	none     
 *
 *  Returns:	char* (printer name)        
 *
 */
extern char*
cm_def_printer()
{
        FILE *fp;
        char message[257];
	char *tmp=NULL;
        char *printer_name=NULL;

#ifdef SVR4
	tmp = (char*)getenv("LPDEST");
	if (tmp != NULL && *tmp != NULL) {
		printer_name = (char*)malloc(strlen(tmp)+1);
		strcpy(printer_name, tmp);
	}
	else {
	
	/* This is really nasty.  lpstat -d does *not* work on the AIX
	   machines.  Just fall back to "lp" here */

#ifndef AIX
	        _Xstrtokparams strtok_buf;

        	fp = (FILE *)popen("lpstat -d", "r");
        	fread(message, 256, 1, fp);
        	tmp = (char *)_XStrtok(message, ":", strtok_buf);
        	tmp = (char *)_XStrtok((char *)NULL, "\n", strtok_buf);
		if (tmp != NULL && *tmp != NULL) {
			printer_name = (char*)malloc(strlen(tmp)+1);
			strcpy(printer_name, tmp);
		}
		else {
			printer_name = (char*)malloc(3);
			strcpy(printer_name, "lp");
		}

		/* close the process connection */
		pclose(fp);
#else
		printer_name = (char*)malloc(3);
		strcpy(printer_name, "lp");
#endif
	}
#else
	tmp = (char*)getenv("PRINTER");
	if (tmp != NULL && *tmp != '\0') {
		printer_name = (char*)malloc(strlen(tmp)+1);
		strcpy(printer_name, tmp);
	}
	else {
		printer_name = (char*)malloc(3);
		strcpy(printer_name, "lw");
	}
#endif
	return printer_name;
}

/*--------------------------------------------------------------------------
 * THE FOLLOWING STRING FUNCTION redefinitions are a HACK !
 * 
 * The cm code should be changed so that
 *   a) the redefined functions use the same headers as in <string.h>
 *   b) no redefinition of these library function is necessary
 *
 * The cm definitions use different function headers than in <string.h>
 * Prefixing the functions will get rid of the resulting compiler error.
 * Now cm functions will use the cm_ string functions, but library functions, 
 * e.g. fprintf, will use strlen etc. which leads to core dumps. 
 * As part of the bootstrapping process, I am including the below redefinitions
 * of the system functions. This should be fixed later.
 * [vmh - 5/31/90]
 *--------------------------------------------------------------------------*/

extern char *
cm_strcpy(register char *s1, register char *s2)
{
	if (s1==NULL || s2==NULL) return(NULL);
	strcpy(s1, s2); 
        return (s1);
}

extern int 
cm_strlen(register char *s)
{
        register int n;
 
	if (s==NULL) return 0;
	return (strlen(s));
}

extern char *
cm_strdup (char *s1)
{
	char *s2;
	if (s1 == NULL) return NULL;
        s2 = (char *) strdup(s1);
	return (s2);
}

extern char *
cm_strcat(char *s1, char *s2)
{
	if (s1==NULL || s2==NULL) return(s1);
	strcat(s1, s2);
	return s1;
}

/*      transform string patterns of \\ into \
        \n into carriage returns and
	\" into "	*/

extern char *
str_to_cr(char *s)
{
        int i, j, k;
        char *newstr;

        if (s==NULL) return(NULL);
        i = cm_strlen(s);

        newstr= (char *) ckalloc((unsigned)i + 1);
        k = 0;
        for (j=0; j<i; j++) {
                if (s[j]=='\\') {
                        if (s[j+1]=='n') {
                                newstr[k] = '\n';
                                j++;
                        }
                        else if (s[j+1]=='\\') {
                                newstr[k] = '\\';
                                j++;
                        }
			else if (s[j+1]=='\"') {
				newstr[k] = '\"';
				j++;
			}
                        else {
                                newstr[k] = s[j];
                        }
                }
                else {
                        newstr[k] = s[j];
                }
                k++;
        }
        newstr[k] = '\0';
        return(newstr);
}

/*      transform string patterns of \ into \\
        carriage returns into \n, and
	" into \"	*/

extern char *
cr_to_str(char *s)
{
	int i, j, k;
	char *newstr;

        if (s==NULL) return(NULL);
	i = cm_strlen(s);

	newstr = (char *) ckalloc((unsigned)((2 * i) + 1));
	k = 0;
	for (j=0; j<i; j++) {
		if (s[j]=='\n') {
			newstr[k] = '\\';
			newstr[k+1] = 'n';
			k+=2;
		}
		else if (s[j]=='\\') {
			newstr[k] = '\\';
			newstr[k+1] = '\\';
			k+=2;
		}
		else if (s[j]=='\"') {
			newstr[k] = '\\';
			newstr[k+1] = '\"';
			k+=2;
		}
		else {
			newstr[k] = s[j];
			k++;
		}
	}
	newstr[k] = '\0';
	return(newstr);
}

/* VARARGS1 */
extern void
syserr(msg, a1, a2, a3)
	char *msg;
{
	/* Taken from Unix World, July 1989, p. 66 */
	int saveerr;

	/* save the error number so fprintf doesn't step on it */
	saveerr = errno;

	(void) fprintf(stderr, "cm: ");
	/* print the actual message itself */
	(void) fprintf(stderr, msg, a1, a2, a3);

#if 0
	/* print the error, if any */
	if (saveerr != 0) {
		if (saveerr < 0 || saveerr > sys_nerr) 
			(void) fprintf(stderr, ":Unknown error %d", saveerr);
		else 
			(void) fprintf(stderr, ":%s", sys_errlist[saveerr]);
	}
#endif

	/* thow a newline on the end */
	(void) fprintf(stderr, "\n");

	/* exit with an error */
	if (saveerr==0)
		saveerr = -1;
	exit(saveerr);
}


/*	Wrapper around standard storage allocation, to localize errors.
	Taken from Unix World, July 1989, p. 66				*/
extern char *
ckalloc(unsigned int size)
{
	register char *p;

	/* try to get the memory */
	p = (char *)calloc(1, size);

	/* if it worked, return the memory directly */
	if (p != NULL) return(p);

	/* try allocation again */
	p = (char *)calloc(1, size);

	/* see if it worked the second time */
	if (p != NULL) return(p);

	/* no recovery available */
	syserr("ckalloc: cannot allocate %d bytes", size, 0, 0);
	return((char *)NULL);
}

	
extern void
print_tick(Tick t)
{
        char *a;
	_Xctimeparams ctime_buf;
 
        a = _XCtime(&t, ctime_buf);
        (void) fprintf (stderr, "%ld %s\n", (long)t, a);
}

int
min(int i1, int i2)
{
	if (i1 > i2) return(i2);
	if (i1 < i2) return(i1);
	return(i1);
}

int
max(int i1, int i2)
{
	if (i1 > i2) return(i1);
	if (i1 < i2) return(i2);
	return(i1);
}
	
extern Lines *
text_to_lines(char *s, int n)
{
	char *string, *line;
	Lines *prev_l = NULL, *l = NULL, *head= NULL;
	int i = 0;
	char *_p;
	int clen;

	if (s == NULL || n <= 0) return NULL;

	string = cm_strdup(s);
	/*
	 * Here, look for \n, which is (in)famous character in IBM-932.
	 * Therefore, don't use strtok(). It is not i18n'ed.
	 */
	for ( _p = string; *_p != '\0'; _p += clen ) {
	    clen = mblen( _p, MB_CUR_MAX );
	    if ( clen <= 0 ) {
		*_p = '\0';
		break;
	    }
	    if ( ( clen == 1 ) && ( *_p == '\n' ) ) {
		*_p = '\0';
		_p++;
		break;
	    }
	}
	line = string;
	do {
		if (line == NULL) break;
		l = (Lines*)ckalloc(sizeof(Lines));
		if (head == NULL) head = l;
		if (prev_l != NULL) prev_l->next = l;
		l->s = cm_strdup(line);
		prev_l = l;
		i++;
		if ( ( *_p == '\0' ) || ( clen == -1 ) )
		    break;
		line = _p;
		for ( ; *_p != '\0'; _p += clen ) {
		    clen = mblen( _p, MB_CUR_MAX );
		    if ( clen <= 0 ) {
			*_p = '\0';
			break;
		    }
		    if ( ( clen == 1 ) && ( *_p == '\n' ) ) {
			*_p = '\0';
			_p++;
			break;
		    }
		}

	} while (i < n);
		
	free(string);
	return head;
}
 
extern void
destroy_lines(Lines *l)
{
        Lines *p;

        while (l != NULL) {
                free(l->s); l->s=NULL;
                p = l;
                l = l->next;
                free((char *)p); p=NULL;
        }
}

/*
 * Expand any escape characters in passed string
 */
extern void
expand_esc_chars(char *string) {
	char	*from, *to;

	from = to = string;
	while (from && *from) {
		int	len = mblen(from, MB_CUR_MAX);

		if (len <= 0) break; /* invalid char */
		if (len > 1) {  /* move over multibyte char */
			from += len;
			to += len;
			continue;
		}

		switch (*from++) {
		case '\\':
			switch (*from++) {
			case 'n':
				*to++ = '\n';
				break;
			case 't':
				*to++ = '\t';
				break;
			default:
				*to++ = *(from-2);
				*to++ = *(from-1);
				break;
			}
			break;
		default:
			*to++ = *(from-1);
			break;
		}
	}
	*to = '\0';
}

extern char *
get_head(char *str, char sep)
{
        static char buf[BUFSIZ];
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
                return(cm_strdup(buf));
        }
}

extern char *
get_tail(char *str, char sep)
{
        char *ptr;
 
        if (str == NULL)
                return(NULL);
 
        while (*str && *str != sep)
                str++;
        if (*str)
                return(cm_strdup(++str));
        else
                return(NULL);
}

extern char *
cm_get_credentials()
{
	char *name, *host;
	static char *login = NULL;

	if (login==NULL)
	{
		name = (char*)cm_get_uname();
		host = (char*)cm_get_local_host();
		login = (char *) ckalloc (cm_strlen(name) + cm_strlen(host) + 2);
		sprintf(login, "%s@%s", name, host);
	}
	return (login);
}

extern char *
cm_get_local_host()
{
	static char *local_host;

        if (local_host == NULL) {
#if defined(sun) || defined(USL) || defined(__uxp__)
                local_host = (char *)ckalloc(MAXHOSTNAMELEN);
                (void) sysinfo(SI_HOSTNAME, local_host, MAXHOSTNAMELEN);
#else
                local_host = (char *)ckalloc(MAXHOSTNAMELEN);
                (void) gethostname(local_host, MAXHOSTNAMELEN);
#endif /* sun || USL || __uxp__ */
        }
        return local_host;
}

extern char *
cm_get_uname()
{
        static char *name;
        struct passwd *pw; 

        if (name == NULL) {
                if ((pw = (struct passwd *)getpwuid(geteuid())) == NULL)
                         name = (char *) cm_strdup("nobody");
                else
                        name = (char *) cm_strdup(pw->pw_name);
        }
        return name;
    
}

extern char *
cm_get_local_domain()
{
	static char *local_domain;

        if (local_domain == NULL) {
                local_domain = ckalloc(BUFSIZ);
#if defined(sun) || defined(USL) || defined(__uxp__)
                sysinfo(SI_SRPC_DOMAIN, local_domain, DOM_NM_LN);
#else
		if(-1 == getdomainname(local_domain, BUFSIZ)) {
			fprintf(stderr, "getdomainname() failed %d '%s'\n", errno, strerror(errno));
		}
		
#endif /* sun || USL || __uxp__ */
	}
        return(local_domain);
}

/* partially qualified target */
extern char*
cm_pqtarget(char *name)
{
        char *host, *target=NULL;
 
        host = (char*)strchr(name, '@');
        if (host == NULL) {
                host = (char*)cm_get_local_host();
                target = (char *)ckalloc(cm_strlen(name) +
                                cm_strlen(host) + 2);
                sprintf(target, "%s@%s", name, host);
        }
        else
                target = (char *) cm_strdup(name);
 
        return target;
}
/*
 * calendar_name@host[.domain] -> calendar_name
 */
extern char *
cm_target2name(char *target)
{
        return(get_head(target, '@'));
}
 
/*
 * calendar_name@host[.domain] -> host[.domain]
 */
extern char *
cm_target2location(char *target)
{
        return(get_tail(target, '@'));
}
 
/*
 * calendar_name@host[.domain] -> host
 */
extern char *
cm_target2host(char *target)
{
        char *location, *host;
 
        location = get_tail(target, '@');
        if (location != NULL) {
                host = get_head(location, '.');
                free(location);
                return(host);
        } else
                return(NULL);
}
/*
 * calendar_name@host[.domain] -> domain
 */
extern char *
cm_target2domain(char *target)
{
        char *location, *domain;
 
        location = get_tail(target, '@');
        if (location != NULL) {
                domain = get_tail(location, '.');
                free(location);
                return(domain);
        } else
                return(NULL);
}

/*
 * str consists of components separated by token
 * get and copy the first component into comp and
 * strip it out of str, so str would point to the first
 * token or the null terminator.
 */
static void
get_component(char **str, char *comp, char token)
{
	char *ptr;

	*comp = 0;

	if (str == NULL)
		return;
	else
		ptr = *str;

	while (ptr && *ptr != 0 && *ptr != token)
		*comp++ = *ptr++;

	*str = ptr;

	*comp = 0;
}

/*
 * head and tail points to the first and last character
 * of a string which consists of components separated by token.
 * get and copy the last component into comp and
 * strip it out of the string, so tail would point to the last
 * token or the head of the string.
 */
static void
get_last_component(char *head, char **tail, char *comp, char token)
{
	char *ptr, *cptr;

	*comp = 0;

	if (tail == NULL)
		return;
	else
		cptr = *tail;

	while (cptr != head && *cptr != token)
		cptr--;

	if (*cptr == token)
		ptr = cptr + 1;
	else
		ptr = cptr;

	while (ptr != (*tail + 1))
		*comp++ = *ptr++;

	*tail = cptr;

	*comp = 0;
}

static boolean_t
match_forward(char *str1, char *str2)
{
	char com1[BUFSIZ], com2[BUFSIZ];

	if (str1 == NULL || str2 == NULL)
		return (B_FALSE);

	while (B_TRUE) {
		get_component(&str1, com1, '.');
		get_component(&str2, com2, '.');

		if (*com1) {
			if (*com2 == '\0')
				return (B_TRUE);
		} else {
			if (*com2 == '\0')
				return (B_TRUE);
			else
				return (B_FALSE);
		}

		if (strcasecmp(com1, com2) != 0)
			return (B_FALSE);

		/* take care of case: a.b a. */
		if (strcmp(str2, ".") == 0
		    && (strcmp(str1, ".") != 0 || *str1 != '\0'))
			return (B_FALSE);

		/* skip "." */
		if (*str1 == '.') {
			if (*str2 == '\0')
				return (B_TRUE);
			else {
				str1++;
				str2++;
			}
		} else if (strcmp(str2, ".") == 0 || *str2 == '\0')
			return (B_TRUE);
		else
			return (B_FALSE);
	}
}

static boolean_t
match_backward(char *str1, char *str2)
{
	int len1, len2;
	char *ptr1, *ptr2;
	char com1[BUFSIZ], com2[BUFSIZ];

	if (str1 == NULL || str2 == NULL)
		return (B_FALSE);

	len1 = strlen(str1);
	len2 = strlen(str2);
	if (len2 > len1)
		return (B_FALSE);
	else if (len2 == 0)
		return (B_TRUE);

	ptr1 = (len1 ? (str1 + len1 - 1) : str1);
	ptr2 = (len2 ? (str2 + len2 - 1) : str2);

	if (*ptr1 == '.' && ptr1 != str1)
		ptr1--;

	if (*ptr2 == '.' && ptr2 != str2)
		ptr2--;

	while (B_TRUE) {
		get_last_component(str1, &ptr1, com1, '.');
		get_last_component(str2, &ptr2, com2, '.');

		if (*com1) {
			if (*com2 == '\0')
				return (B_TRUE);
		} else {
			if (*com2 == '\0')
				return (B_TRUE);
			else
				return (B_FALSE);
		}

		if (strcasecmp(com1, com2) != 0)
			return (B_FALSE);

		/* skip "." */
		if (*ptr1 == '.') {
			if (ptr1 != str1)
				ptr1--;
			else
				return (B_FALSE); /* bad format */
		} else
			return (B_TRUE); /* done */

		if (*ptr2 == '.') {
			if (ptr2 != str2)
				ptr2--;
			else
				return (B_FALSE); /* bad format */
		} else
			return (B_TRUE); /* done */
	}
}

/*
 * Correct format assumed, i.e. str = label1[.label2 ...]
 * Compare str2 against str1 which should be more fully qualified than str2
 */
extern boolean_t
same_path(char *str1, char *str2)
{
	char *ptr1,*ptr2;
	char *user;
	int res, n;

	if (str1 == NULL || str2 == NULL)
		return(B_FALSE);

	/* check format */
	if (*str1 == '.' || *str2 == '.')
		return (B_FALSE); /* bad format */

	if (match_forward(str1, str2) == B_TRUE)
		return (B_TRUE);
	else
		return (match_backward(str1, str2));
}

/*
 * compare user1 and user2
 * user1 = user@host[.domain]
 * user2 = any format in (user, user@host[.domain], user@domain)
 */
extern boolean_t
same_user(char *user1, char *user2)
{
	char *str1, *str2;
	char *host, *domain;
	char buf[BUFSIZ];
	boolean_t res;

	if (user1 == NULL || user2 == NULL)
		return B_FALSE;

	/* compare user name */
	str1 = get_head(user1, '@');
	str2 = get_head(user2, '@');

	if (str1 == NULL || str2 == NULL)
		return(B_FALSE);

	if (strcmp(str1, str2)) {
		free(str1);
		free(str2);
		return(B_FALSE);
	}
	free(str1);
	free(str2);

	/* if only user name is specified, don't need to check domain */
	str2 = strchr(user2, '@');
	if (str2 == NULL)
		return(B_TRUE);

	/* first assume user2=user@domain */
	str1 = strchr(user1, '.');
	if (str1 == NULL) {
		if (same_path(cm_get_local_domain(), ++str2))
			return(B_TRUE);
	} else {
		if (same_path(++str1, ++str2))
			return(B_TRUE);
	}

	/* assume user2=user@host[.domain] */
	if (str1 == NULL) {
		str1 = strchr(user1, '@');
		sprintf(buf, "%s.%s", ++str1, cm_get_local_domain());
		str1 = buf;
	} else {
		str1 = strchr(user1, '@');
		str1++;
	}

	if (same_path(str1, str2))
		return(B_TRUE);
	else
		return(B_FALSE);
}

/*
 * A blank line is one that consists of only \b, \t or \n.
 */
extern int
blank_buf(char *buf)
{
	char *ptr = buf;

	if (ptr == NULL) return B_TRUE;
	while (ptr && (*ptr == ' ' || *ptr == '\t' || *ptr == '\n'))
		ptr++;
	if (*ptr == '\0')
		return B_TRUE;
	else
		return B_FALSE;
}

extern int
embedded_blank(char *buf)
{
        char *ptr = buf;
 
        if (ptr == NULL) return B_TRUE;
        while (ptr && *ptr) {
                if ((*ptr == ' ') || (*ptr == '\t'))
                        return B_TRUE;
                *ptr++;
        }
 
        return B_FALSE;
}

extern int
get_data_version(CSA_session_handle session) {
	int		ver = 0;
	Dtcm_calendar	*c;
	CSA_attribute_reference names[1];
	CSA_uint32	number_attrs_returned;
	CSA_attribute	*attrs_returned;

	names[0] = CSA_X_DT_CAL_ATTR_DATA_VERSION;


	if (csa_read_calendar_attributes(session, 
					 1, 
					 names, 
					 &number_attrs_returned, 
					 &attrs_returned, 
					 NULL) == CSA_SUCCESS) {
		ver = attrs_returned[0].value->item.uint32_value;
		csa_free(attrs_returned);
	}

	return ver;
}

extern int
get_server_version(CSA_session_handle session) {
	int		ver = 0;
	Dtcm_calendar	*c;
	CSA_attribute_reference names[1];
	CSA_uint32	number_attrs_returned;
	CSA_attribute	*attrs_returned;

	names[0] = CSA_X_DT_CAL_ATTR_SERVER_VERSION;

	if (csa_read_calendar_attributes(session, 
					 1, 
					 names, 
					 &number_attrs_returned, 
					 &attrs_returned, 
					 NULL) == CSA_SUCCESS) {
		ver = attrs_returned[0].value->item.uint32_value;
		csa_free(attrs_returned);
	}

	return ver;
}

extern CSA_sint32
privacy_set(Dtcm_appointment *appt) {

	CSA_sint32	privacy = CSA_CLASS_PUBLIC;

	if (!appt)
		return(privacy);

	if (!appt->private)
		return(privacy);

	if (!appt->private->value)
		return(privacy);

	privacy = appt->private->value->item.sint32_value;
	return(privacy);

}

extern CSA_sint32
showtime_set(Dtcm_appointment *appt) {
 
        CSA_sint32	showtime = 0;
 
        if (!appt)
                return(showtime);
 
        if (!appt->show_time)
                return(showtime);
 
        if (!appt->show_time->value)
                return(showtime);
 
        showtime = appt->show_time->value->item.sint32_value;
        return(showtime);
 
}

/*
**  Parse the date string and get the month, day, and year
*/
extern int
parse_date(OrderingType order, SeparatorType sep, char *datestr, char *m,
        char *d, char *y) {
 
        char *first, *second, *third;
        char *tmp_date, *str = separator_str(sep);
	_Xstrtokparams strtok_buf;
 
        m[0] = '\0';
        d[0] = '\0';
        y[0] = '\0';
 
        if (datestr == NULL)
                return 0;
 
        tmp_date = cm_strdup(datestr);
        first = _XStrtok(tmp_date, str, strtok_buf);
                /*
                ** Check to see if the date entered has legit separator
                */
                if ( strcoll(first, datestr) == 0 ) {
                        free(tmp_date);
                        return 0;
                }
        second = _XStrtok(NULL, str, strtok_buf);
        third = _XStrtok(NULL, str, strtok_buf);
 
        switch (order) {
        case ORDER_DMY:
                if (second)
                        cm_strcpy(m, second);
                if (first)
                        cm_strcpy(d, first);
                if (third)
                        cm_strcpy(y, third);
                break;
        case ORDER_YMD:
                if (second)
                        cm_strcpy(m, second);
                if (third)
                        cm_strcpy(d, third);
                if (first)
                        cm_strcpy(y, first);
                break;
        case ORDER_MDY:
        default:
                if (first)
                        cm_strcpy(m, first);
                if (second)
                        cm_strcpy(d, second);
                if (third)
                        cm_strcpy(y, third);
                break;
        }
        free(tmp_date);
                return 1;
}

/*
**  Reformat the date string into m/d/y format and write it into the buffer
*/
extern int
datestr2mdy(char *datestr, OrderingType order, SeparatorType sep, char *buf) {
        char m[3], d[3], y[5];
 
        buf[0] = '\0';
        if (datestr == NULL)
                return 0;
 
        if (order == ORDER_MDY && sep == SEPARATOR_SLASH)
                cm_strcpy(buf, datestr);
        else {
                if ( parse_date(order, sep, datestr, m, d, y) ) {
                        sprintf(buf, "%s/%s/%s", m, d, y);
                } else {
                        return 0;
                }
 
        }
        return 1;
}

/*
**  Format the date according to display property and write it into buffer
*/
extern void
format_tick(Tick tick, OrderingType order, SeparatorType sep, char *buff) {
	char		*str = separator_str(sep);
        struct tm	*tm;
	_Xltimeparams	localtime_buf;
 
        buff[0] = '\0';
        tm = _XLocaltime(&tick, localtime_buf);
 
        switch (order) {
        case ORDER_DMY:
                sprintf(buff, "%d%s%d%s%d", tm->tm_mday, str,
                        tm->tm_mon+1, str, tm->tm_year+1900);
                break;
        case ORDER_YMD:
                sprintf(buff, "%d%s%d%s%d", tm->tm_year+1900, str,
                        tm->tm_mon+1, str, tm->tm_mday);
                break;
        case ORDER_MDY:
        default:
                sprintf(buff, "%d%s%d%s%d", tm->tm_mon+1, str,
                        tm->tm_mday, str, tm->tm_year+1900);
                break;
        }
}

extern void
format_time(Tick t, DisplayType dt, char *buffer) {
	int		hr = hour(t);
	boolean_t	am;

	if (t == 0) {
		buffer[0] = '\0';

	} else if (dt == HOUR12) {
		am = adjust_hour(&hr);
                sprintf(buffer, "%2d:%02d%s",
			hr, minute(t), (am) ? "am" : "pm");
	} else
		sprintf(buffer, "%02d%02d", hr, minute(t));
}

/*
 * The V5 back end uses arrays to pass attributes back and forth.  However,
 * keeping hard coded references into those arrays (i.e. declaring the tick
 * value will always be into position 0 of the array, the what value in
 * position 3, etc.) is a bad idea and hard to maintain.
 *
 * Thus these convenience functions will translate from an attribute array
 * received from the back end into defined structure which the front end can
 * use.
 *
 * IF YOU UPDATE THE STRUCTURES, MAKE SURE YOU UPDATE THESE COUNT CONSTANTS!!
 */
static const int APPT_ATTR_COUNT = 35;
static const int RW_APPT_ATTR_COUNT = 15;
static const int CAL_ATTR_COUNT = 12;
static const int RW_CAL_ATTR_COUNT = 2;
static const int DEF_V5_APPT_ATTR_COUNT = 22;
static const int DEF_V4_APPT_ATTR_COUNT = 20;
static const int DEF_V3_APPT_ATTR_COUNT = 17;
static const int DEF_CAL_ATTR_COUNT = 6;
static const int default_appt_attrs[] = {CSA_ENTRY_ATTR_REFERENCE_IDENTIFIER_I,
			CSA_ENTRY_ATTR_LAST_UPDATE_I,
			CSA_ENTRY_ATTR_ORGANIZER_I,
			CSA_ENTRY_ATTR_START_DATE_I,
			CSA_ENTRY_ATTR_TYPE_I,
			CSA_ENTRY_ATTR_SUBTYPE_I,
			CSA_ENTRY_ATTR_CLASSIFICATION_I,
			CSA_ENTRY_ATTR_END_DATE_I,
			CSA_X_DT_ENTRY_ATTR_SHOWTIME_I,
			CSA_ENTRY_ATTR_SUMMARY_I,
			CSA_ENTRY_ATTR_STATUS_I,
			CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I,
			CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES_I,
			CSA_ENTRY_ATTR_AUDIO_REMINDER_I,
			CSA_ENTRY_ATTR_FLASHING_REMINDER_I,
			CSA_ENTRY_ATTR_MAIL_REMINDER_I,
			CSA_ENTRY_ATTR_POPUP_REMINDER_I,
			CSA_X_DT_ENTRY_ATTR_REPEAT_OCCURRENCE_NUM_I,
			CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL_I,
			CSA_X_DT_ENTRY_ATTR_SEQUENCE_END_DATE_I,
			CSA_ENTRY_ATTR_RECURRENCE_RULE_I,
			CSA_ENTRY_ATTR_NUMBER_RECURRENCES_I
			};
static const int default_cal_attrs[] = {CSA_CAL_ATTR_ACCESS_LIST_I,
					CSA_CAL_ATTR_CALENDAR_NAME_I,
					CSA_CAL_ATTR_CALENDAR_SIZE_I,
					CSA_CAL_ATTR_NUMBER_ENTRIES_I,
					CSA_CAL_ATTR_TIME_ZONE_I,
					CSA_X_DT_CAL_ATTR_DATA_VERSION_I
					};

/*
 * NOTE that this loop is dependent on the first appointment attribute define
 * (DT_CM_ATTR_IDENTIFER_I) in the api - if that is changed and is no longer
 * the first appointment define, this needs to be changed.
 *
 * NOTE that this function checks if the api indexes specified are read-only:
 * This assumes that if you need value space (and have set the need_value_space
 * flag to B_TRUE), you're setting attributes and since you can't set read-only
 * attributes, it will ignore read-only attributes if the need_value_space flag
 * is B_TRUE.
 */
Dtcm_appointment *allocate_appt_struct (Allocation_reason reason, int version, ...) {
	int			idx = 0, api_idx;
	va_list			pvar;
	CmDataList		*api_ids = CmDataListCreate();
	Dtcm_appointment	*appt;
	int			def_attr_count;

	/*
	 * The Dtcm_appointment wrapper array
	 */
	idx = sizeof(Dtcm_appointment);
	appt = (Dtcm_appointment *)ckalloc(idx);
	memset(appt, 0, idx);
	appt->reason = reason;
	appt->version = version;

	/*
	 * Step through the variable argument list and build the list of
	 * attributes we're looking for
	 */
	va_start(pvar, version);
	api_idx = va_arg(pvar, int);
	while (api_idx) {
		if ((reason == appt_read) || !entry_ident_index_ro(api_idx, version))
			CmDataListAdd(api_ids, (void *) (intptr_t) api_idx, 0);
		api_idx = va_arg(pvar, int);
	}
	va_end(pvar);

	/*
	 * No attributes specified, assume the caller wanted all of them
	 */
	if (api_ids->count <= 0) {

		if ((version == DATAVER2) || (version == DATAVER1))
			def_attr_count = DEF_V3_APPT_ATTR_COUNT;
		else if (version == DATAVER3)
			def_attr_count = DEF_V4_APPT_ATTR_COUNT;
		else if (version == DATAVER4)
			def_attr_count = DEF_V5_APPT_ATTR_COUNT;
		else if (version == DATAVER_ARCHIVE)
			def_attr_count = DEF_V5_APPT_ATTR_COUNT;

		for (idx = 0; idx < def_attr_count; idx++) {
			if ((reason == appt_write) && entry_ident_index_ro(default_appt_attrs[idx], version))
				continue;
			CmDataListAdd(api_ids, (void *) (intptr_t) default_appt_attrs[idx], 0);
		}
	}

	/*
	 * We've determined the number of attributes we're retrieving, so
	 * allocate the name array, and the attribute array (if we are 
	 * going to be writing attributes).
	 */

	appt->num_names = api_ids->count;
	idx = sizeof(CSA_attribute_reference *) * appt->num_names;
	appt->names = (CSA_attribute_reference *)ckalloc(idx);
	memset(appt->names, 0, idx);

	appt->count = api_ids->count;
	if (reason == appt_write) {
		idx = sizeof(CSA_attribute) * appt->count;
		appt->attrs = (CSA_attribute *)ckalloc(idx);
		memset(appt->attrs, 0, idx);
	}

	/*
	 * Now loop through and set the names and initialize the attributes
	 */
	for (idx = 0; idx < appt->count; idx++) {
		api_idx = (int) (intptr_t) CmDataListGetData(api_ids, idx + 1);
		appt->names[idx] = strdup(_CSA_entry_attribute_names[api_idx]);
		if (reason == appt_write)
			initialize_entry_attr(api_idx, &appt->attrs[idx], reason, version);
	}

	if (reason == appt_write)
		set_appt_links(appt);

	CmDataListDestroy(api_ids, 0);
	return appt;
}

CSA_return_code
query_appt_struct(CSA_session_handle session, 
		  CSA_entry_handle entry_handle, 
		  Dtcm_appointment *appt) {

	CSA_return_code 	status;

	/* if there is old query material laying around, toss it */

	if (appt->filled) {
		csa_free(appt->attrs);
		appt->filled = False;
	}


	if ((status = csa_read_entry_attributes(session, 
					 entry_handle, 
					 appt->num_names, 
					 appt->names, 
					 &appt->count, 
					 &appt->attrs, 
					 NULL)) == CSA_SUCCESS) {
		set_appt_links(appt);
		appt->filled = True;
	}

	return(status);
}


/*
 * NOTE that this function checks if the api indexes specified are read-only:
 * This assumes that if you need value space (and have set the need_value_space
 * flag to B_TRUE), you're setting attributes and since you can't set read-only
 * attributes, it will ignore read-only attributes if the need_value_space flag
 * is B_TRUE.
 */
Dtcm_calendar*
allocate_cal_struct(Allocation_reason reason, int version, ...) {
	int		idx = 0, api_idx;
	va_list		pvar;
	CmDataList	*api_ids = CmDataListCreate();
	Dtcm_calendar	*cal;

	/*
	 * The Dtcm_apopintment wrapper array
	 */
	idx = sizeof(Dtcm_calendar);
	cal = (Dtcm_calendar *)ckalloc(idx);
	memset(cal, 0, idx);
	cal->reason = reason;
	cal->version = version;

	/*
	 * Step through the variable argument list and build the list of
	 * attributes we're looking for
	 */
	va_start(pvar, version);
	api_idx = va_arg(pvar, int);
	while (api_idx) {
		if ((reason == appt_read) || !cal_ident_index_ro(api_idx, version))
			CmDataListAdd(api_ids, (void *) (intptr_t) api_idx, 0);
		api_idx = va_arg(pvar, int);
	}
	va_end(pvar);

	/*
	 * No attributes specified, assume the caller wanted all of them
	 */
	if (api_ids->count <= 0) {
		for (idx = 0; idx < DEF_CAL_ATTR_COUNT; idx++) {
			if ((reason == appt_write) && cal_ident_index_ro(default_cal_attrs[idx], version))
				continue;
			CmDataListAdd(api_ids, (void *) (intptr_t) default_cal_attrs[idx], 0);
		}
	}

	/*
	 * We've determined the number of attributes we're retrieving, so
	 * allocate the name arrya, and the attribute array (if we are 
	 * going to be writing attributes).
	 */

	cal->num_names = api_ids->count;
	idx = sizeof(CSA_attribute_reference) * cal->num_names;
	cal->names = (CSA_attribute_reference *)ckalloc(idx);
	memset(cal->names, 0, idx);

	cal->count = api_ids->count;
	if (reason == appt_write) {
		idx = sizeof(CSA_attribute) * cal->count;
		cal->attrs = (CSA_attribute *)ckalloc(idx);
		memset(cal->attrs, 0, idx);
	}

	/*
	 * Now loop through and set the names and initialize the attributes
	 */
	for (idx = 0; idx < cal->count; idx++) {
		api_idx = (int) (intptr_t) CmDataListGetData(api_ids, idx + 1);
		cal->names[idx] = strdup(_CSA_calendar_attribute_names[api_idx]);
		if (reason == appt_write)
			initialize_cal_attr(api_idx, &cal->attrs[idx], reason, version);
	}

	if (reason == appt_write) 
		set_cal_links(cal);

	CmDataListDestroy(api_ids, 0);


	return cal;
}

CSA_return_code
query_cal_struct(CSA_session_handle session, 
	         Dtcm_calendar *cal) {

	CSA_return_code status;

	/* if there is old query material laying around, toss it */

	if (cal->filled) {
		csa_free(cal->attrs);
		cal->filled = False;
	}

	if ((status = csa_read_calendar_attributes(session, 
					 cal->num_names, 
					 cal->names, 
					 &cal->count, 
					 &cal->attrs, 
					 NULL)) == CSA_SUCCESS) {
		set_cal_links(cal);
		cal->filled = True;
	}

	return(status);
}

extern void
scrub_cal_attr_list(Dtcm_calendar *cal) {

	int	i;

	for (i = 0; i < cal->count; i++) {
		if (cal->attrs[i].value->type == CSA_VALUE_REMINDER) {
			if ((cal->attrs[i].value->item.reminder_value->lead_time == NULL) || 
			     (cal->attrs[i].value->item.reminder_value->lead_time[0] == '\0')) {
				free(cal->attrs[i].name);
				cal->attrs[i].name = NULL;
			}
		}
		else if ((cal->attrs[i].value->type == CSA_VALUE_ACCESS_LIST) && (cal->attrs[i].value->item.access_list_value == NULL)) {
			free(cal->attrs[i].name);
			cal->attrs[i].name = NULL;
		}
		else if ((cal->attrs[i].value->type == CSA_VALUE_STRING) && (cal->attrs[i].value->item.string_value == NULL)) {
			free(cal->attrs[i].name);
			cal->attrs[i].name = NULL;
		}
		else if ((cal->attrs[i].value->type == CSA_VALUE_DATE_TIME) && (cal->attrs[i].value->item.date_time_value == NULL)) {
			free(cal->attrs[i].name);
			cal->attrs[i].name = NULL;
		}
	}
}

extern boolean_t
cal_ident_index_ro(int id, int version) {
	boolean_t	r_ro;

	switch(id) {
	case CSA_CAL_ATTR_CALENDAR_NAME_I:
	case CSA_CAL_ATTR_CALENDAR_OWNER_I:
	case CSA_CAL_ATTR_CALENDAR_SIZE_I:
	case CSA_CAL_ATTR_CHARACTER_SET_I:
	case CSA_CAL_ATTR_NUMBER_ENTRIES_I:
	case CSA_CAL_ATTR_DATE_CREATED_I:
	case CSA_CAL_ATTR_PRODUCT_IDENTIFIER_I:
	case CSA_X_DT_CAL_ATTR_DATA_VERSION_I:
	case CSA_CAL_ATTR_TIME_ZONE_I:
		r_ro = B_TRUE;
		break;
	default:
		r_ro = B_FALSE;
		break;
	}

	return r_ro;
}

extern boolean_t
entry_ident_index_ro(int id, int version) {
	boolean_t	r_ro;

	switch(id) {
	case CSA_ENTRY_ATTR_DATE_CREATED_I:
	case CSA_ENTRY_ATTR_LAST_UPDATE_I:
	case CSA_ENTRY_ATTR_NUMBER_RECURRENCES_I:
	case CSA_ENTRY_ATTR_ORGANIZER_I:
	case CSA_ENTRY_ATTR_REFERENCE_IDENTIFIER_I:
	case CSA_ENTRY_ATTR_SEQUENCE_NUMBER_I:
		r_ro = B_TRUE;
		break;
	case CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I:
	case CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES_I:
	case CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL_I:
	case CSA_X_DT_ENTRY_ATTR_REPEAT_OCCURRENCE_NUM_I:
	case CSA_X_DT_ENTRY_ATTR_SEQUENCE_END_DATE_I:
		if (version >= DATAVER4)
			r_ro = B_TRUE;
		else
			r_ro = B_FALSE;
		break;
	default:
		r_ro = B_FALSE;
		break;
	}

	return r_ro;
}

extern CSA_enum
cal_ident_index_tag(int id) {
	CSA_enum	r_tag;

	switch(id) {
	case CSA_CAL_ATTR_CALENDAR_NAME_I:
	case CSA_CAL_ATTR_CHARACTER_SET_I:
	case CSA_CAL_ATTR_COUNTRY_I:
	case CSA_CAL_ATTR_PRODUCT_IDENTIFIER_I:
	case CSA_CAL_ATTR_TIME_ZONE_I:
	case CSA_CAL_ATTR_LANGUAGE_I:
		r_tag = CSA_VALUE_STRING;
		break;
	case CSA_CAL_ATTR_CALENDAR_OWNER_I:
		r_tag = CSA_VALUE_CALENDAR_USER;
		break;
	case CSA_CAL_ATTR_DATE_CREATED_I:
		r_tag = CSA_VALUE_DATE_TIME;
		break;
	case CSA_CAL_ATTR_CALENDAR_SIZE_I:
	case CSA_CAL_ATTR_NUMBER_ENTRIES_I:
	case CSA_X_DT_CAL_ATTR_DATA_VERSION_I:
		r_tag = CSA_VALUE_UINT32;
		break;
	case CSA_CAL_ATTR_ACCESS_LIST_I:
		r_tag = CSA_VALUE_ACCESS_LIST;
		break;
	case CSA_CAL_ATTR_WORK_SCHEDULE_I:
	default:
		r_tag = CSA_VALUE_OPAQUE_DATA;
		break;
	}

	return r_tag;
}

extern CSA_enum
entry_ident_index_tag(int id) {
	CSA_enum	r_tag;

	switch(id) {
	case CSA_ENTRY_ATTR_DESCRIPTION_I:
	case CSA_ENTRY_ATTR_EXCEPTION_RULE_I:
	case CSA_ENTRY_ATTR_RECURRENCE_RULE_I:
	case CSA_ENTRY_ATTR_SUBTYPE_I:
	case CSA_ENTRY_ATTR_SUMMARY_I:
		r_tag = CSA_VALUE_STRING;
		break;
	case CSA_ENTRY_ATTR_DATE_COMPLETED_I:
	case CSA_ENTRY_ATTR_DATE_CREATED_I:
	case CSA_ENTRY_ATTR_DUE_DATE_I:
	case CSA_ENTRY_ATTR_END_DATE_I:
	case CSA_ENTRY_ATTR_LAST_UPDATE_I:
	case CSA_ENTRY_ATTR_START_DATE_I:
	case CSA_X_DT_ENTRY_ATTR_SEQUENCE_END_DATE_I:
		r_tag = CSA_VALUE_DATE_TIME;
		break;
	case CSA_ENTRY_ATTR_EXCEPTION_DATES_I:
	case CSA_ENTRY_ATTR_RECURRING_DATES_I:
		r_tag = CSA_VALUE_DATE_TIME_LIST;
		break;
	case CSA_ENTRY_ATTR_CLASSIFICATION_I:
	case CSA_ENTRY_ATTR_NUMBER_RECURRENCES_I:
	case CSA_ENTRY_ATTR_PRIORITY_I:
	case CSA_ENTRY_ATTR_SEQUENCE_NUMBER_I:
	case CSA_ENTRY_ATTR_STATUS_I:
	case CSA_ENTRY_ATTR_TYPE_I:
	case CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES_I:
	case CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL_I:
		r_tag = CSA_VALUE_UINT32;
		break;
	case CSA_ENTRY_ATTR_TIME_TRANSPARENCY_I:
	case CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I:
	case CSA_X_DT_ENTRY_ATTR_REPEAT_OCCURRENCE_NUM_I:
	case CSA_X_DT_ENTRY_ATTR_SHOWTIME_I:
		r_tag = CSA_VALUE_SINT32;
		break;
	case CSA_ENTRY_ATTR_AUDIO_REMINDER_I:
	case CSA_ENTRY_ATTR_FLASHING_REMINDER_I:
	case CSA_ENTRY_ATTR_MAIL_REMINDER_I:
	case CSA_ENTRY_ATTR_POPUP_REMINDER_I:
		r_tag = CSA_VALUE_REMINDER;
		break;
	case CSA_ENTRY_ATTR_ORGANIZER_I:
	case CSA_ENTRY_ATTR_SPONSOR_I:
		r_tag = CSA_VALUE_CALENDAR_USER;
		break;
	case CSA_ENTRY_ATTR_ATTENDEE_LIST_I:
		r_tag = CSA_VALUE_ATTENDEE_LIST;
		break;
	case CSA_ENTRY_ATTR_REFERENCE_IDENTIFIER_I:
	default:
		r_tag = CSA_VALUE_OPAQUE_DATA;
		break;
	}

	return r_tag;
}

extern boolean_t
ident_name_ro(char *name, int version) {
	boolean_t	r_ro = B_FALSE;

	if (strcmp(name, CSA_CAL_ATTR_CALENDAR_NAME) == 0 ||
	    strcmp(name, CSA_CAL_ATTR_CALENDAR_OWNER) == 0 ||
	    strcmp(name, CSA_CAL_ATTR_CALENDAR_SIZE) == 0 ||
	    strcmp(name, CSA_CAL_ATTR_DATE_CREATED) == 0 ||
	    strcmp(name, CSA_CAL_ATTR_PRODUCT_IDENTIFIER) == 0 ||
	    strcmp(name, CSA_X_DT_CAL_ATTR_DATA_VERSION) == 0 ||
	    strcmp(name, CSA_ENTRY_ATTR_SEQUENCE_NUMBER) == 0 ||
	    strcmp(name, CSA_ENTRY_ATTR_REFERENCE_IDENTIFIER) == 0 ||
	    strcmp(name, CSA_ENTRY_ATTR_ORGANIZER) == 0 ||
	    strcmp(name, CSA_ENTRY_ATTR_LAST_UPDATE) == 0 ||
	    strcmp(name, CSA_ENTRY_ATTR_DATE_CREATED) == 0 ||
	    strcmp(name, CSA_ENTRY_ATTR_NUMBER_RECURRENCES) == 0)
		r_ro = B_TRUE;

	if ((version >= DATAVER4) &&
	    (strcmp(name, CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE) == 0 ||
	     strcmp(name, CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES) == 0 ||
	     strcmp(name, CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL) == 0 ||
	     strcmp(name, CSA_X_DT_ENTRY_ATTR_REPEAT_OCCURRENCE_NUM) == 0))
		r_ro = B_TRUE;

	return r_ro;
}

extern void
initialize_cal_attr(int id, CSA_attribute *attrs, Allocation_reason reason, int version) {
	int	size;

	attrs->name = cm_strdup(_CSA_calendar_attribute_names[id]);
	if ((reason == appt_write) && !cal_ident_index_ro(id, version)) {
		size = sizeof(CSA_attribute_value);
		attrs->value = (CSA_attribute_value *)ckalloc(size);
		memset(attrs->value, 0, size);
		attrs->value->type = cal_ident_index_tag(id);
		if (attrs->value->type == CSA_VALUE_REMINDER)
			attrs->value->item.reminder_value = (CSA_reminder *) calloc(sizeof(CSA_reminder), 1);
	}
}

static void
free_attr(CSA_attribute *attr) {

	if (attr == NULL)
		return;

	if (attr->name)
		free(attr->name);


	if (attr->value){
		if ((attr->value->type == CSA_VALUE_STRING) && attr->value->item.string_value != NULL)
			free(attr->value->item.string_value);
		else if ((attr->value->type == CSA_VALUE_DATE_TIME) && attr->value->item.date_time_value != NULL)
			free(attr->value->item.date_time_value);
		else if ((attr->value->type == CSA_VALUE_REMINDER) && attr->value->item.reminder_value != NULL) {
			if (attr->value->item.reminder_value->lead_time)
				free(attr->value->item.reminder_value->lead_time);
			if (attr->value->item.reminder_value->reminder_data.data)
				free(attr->value->item.reminder_value->reminder_data.data);

			free(attr->value->item.reminder_value);
		}

		free(attr->value);

	}
}

extern void
initialize_entry_attr(int id, CSA_attribute *attrs, Allocation_reason reason, int version) {
	int	size;

	attrs->name = cm_strdup(_CSA_entry_attribute_names[id]);
	if ((reason == appt_write) && !entry_ident_index_ro(id, version)) {
		size = sizeof(CSA_attribute_value);
		attrs->value = (CSA_attribute_value *)ckalloc(size);
		memset(attrs->value, 0, size);
		attrs->value->type = entry_ident_index_tag(id);
		if (attrs->value->type == CSA_VALUE_REMINDER)
			attrs->value->item.reminder_value = (CSA_reminder *) calloc(sizeof(CSA_reminder), 1);
	}
}

extern void
free_appt_struct(Dtcm_appointment **appt) {
	int	i;

	if (!appt)
		return;

	if ((*appt)->names) {
		for (i = 0; i < (*appt)->num_names; i++)
			if ((*appt)->names[i])
				free((*appt)->names[i]);

		free((*appt)->names);
	}

	/* potential memory leak here.  We must be careful, as results 
	   from querys should be thrown away with csa_free(), while 
	   structures we've set up to do update/write operations were 
	   allocated by the client, and need to be freed by that client. */

	if (((*appt)->reason == appt_read) && ((*appt)->filled == True))
		csa_free((*appt)->attrs);
	else 
		if ((*appt)->attrs) {
			for (i = 0; i < (*appt)->count; i++) 
				free_attr(&((*appt)->attrs[i]));

			free((*appt)->attrs);
		}

	free(*appt);
	*appt = NULL;
}

extern void
free_cal_struct(Dtcm_calendar **cal) {
	int	i;

	if (!cal)
		return;

	if ((*cal)->names) {
		for (i = 0; i < (*cal)->num_names; i++)
			if ((*cal)->names[i])
				free((*cal)->names[i]);

		free((*cal)->names);
	}

	/* potential memory leak here.  We must be careful, as results 
	   from querys should be thrown away with csa_free(), while 
	   structures we've set up to do update/write operations were 
	   allocated by the client, and need to be freed by that client. */

	if (((*cal)->reason == appt_read) && ((*cal)->filled == True))
		csa_free((*cal)->attrs);
	else
		if ((*cal)->attrs) {
			for (i = 0; i < (*cal)->count; i++)
				free_attr(&((*cal)->attrs[i]));
			free((*cal)->attrs);
		}

	free(*cal);
	*cal = NULL;
}

extern void
set_appt_links(Dtcm_appointment *appt) {
	int	idx;
	char	*idx_name;

        appt->identifier = NULL;
        appt->modified_time = NULL;
        appt->author = NULL;
	appt->number_recurrence = NULL;
        appt->time = NULL;
        appt->type = NULL;
        appt->subtype = NULL;
        appt->private = NULL;
        appt->end_time = NULL;
        appt->show_time = NULL;
        appt->what = NULL;
        appt->state = NULL;
        appt->repeat_type = NULL;
        appt->repeat_times = NULL;
        appt->repeat_interval = NULL;
        appt->repeat_week_num = NULL;
        appt->recurrence_rule = NULL;
        appt->beep = NULL;
        appt->flash = NULL;
        appt->mail = NULL;
        appt->popup = NULL;
	appt->sequence_end_date = NULL;

	for (idx = 0; idx < appt->count; idx++) {
		idx_name = appt->attrs[idx].name;
		if (!idx_name)
			continue;
		else if (strcmp(idx_name, CSA_ENTRY_ATTR_REFERENCE_IDENTIFIER) == 0)
			appt->identifier = &appt->attrs[idx];
		else if (strcmp(idx_name, CSA_ENTRY_ATTR_LAST_UPDATE) == 0)
			appt->modified_time = &appt->attrs[idx];
		else if (strcmp(idx_name, CSA_ENTRY_ATTR_ORGANIZER) == 0)
			appt->author = &appt->attrs[idx];
		else if (strcmp(idx_name, CSA_ENTRY_ATTR_NUMBER_RECURRENCES) == 0)
			appt->number_recurrence = &appt->attrs[idx];
		else if (strcmp(idx_name, CSA_ENTRY_ATTR_START_DATE) == 0)
			appt->time = &appt->attrs[idx];
		else if (strcmp(idx_name, CSA_ENTRY_ATTR_TYPE) == 0)
			appt->type = &appt->attrs[idx];
		else if (strcmp(idx_name, CSA_ENTRY_ATTR_SUBTYPE) == 0)
			appt->subtype = &appt->attrs[idx];
		else if (strcmp(idx_name, CSA_ENTRY_ATTR_CLASSIFICATION) == 0)
			appt->private = &appt->attrs[idx];
		else if (strcmp(idx_name, CSA_ENTRY_ATTR_END_DATE) == 0)
			appt->end_time = &appt->attrs[idx];
		else if (strcmp(idx_name, CSA_X_DT_ENTRY_ATTR_SHOWTIME) == 0)
			appt->show_time = &appt->attrs[idx];
		else if (strcmp(idx_name, CSA_ENTRY_ATTR_SUMMARY) == 0)
			appt->what = &appt->attrs[idx];
		else if (strcmp(idx_name, CSA_ENTRY_ATTR_STATUS) == 0)
			appt->state = &appt->attrs[idx];
		else if (strcmp(idx_name, CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE) == 0)
			appt->repeat_type = &appt->attrs[idx];
		else if (strcmp(idx_name, CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES) == 0)
			appt->repeat_times = &appt->attrs[idx];
		else if (strcmp(idx_name, CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL) == 0)
			appt->repeat_interval = &appt->attrs[idx];
		else if (strcmp(idx_name, CSA_X_DT_ENTRY_ATTR_REPEAT_OCCURRENCE_NUM) == 0)
			appt->repeat_week_num = &appt->attrs[idx];
		else if (strcmp(idx_name, CSA_ENTRY_ATTR_RECURRENCE_RULE) == 0)
			appt->recurrence_rule = &appt->attrs[idx];
		else if (strcmp(idx_name, CSA_ENTRY_ATTR_AUDIO_REMINDER) == 0)
			appt->beep = &appt->attrs[idx];
		else if (strcmp(idx_name, CSA_ENTRY_ATTR_FLASHING_REMINDER) == 0)
			appt->flash = &appt->attrs[idx];
		else if (strcmp(idx_name, CSA_ENTRY_ATTR_MAIL_REMINDER) == 0)
			appt->mail = &appt->attrs[idx];
		else if (strcmp(idx_name, CSA_ENTRY_ATTR_POPUP_REMINDER) == 0)
			appt->popup = &appt->attrs[idx];
		else if (strcmp(idx_name, CSA_X_DT_ENTRY_ATTR_SEQUENCE_END_DATE) == 0)
			appt->sequence_end_date = &appt->attrs[idx];
	}
}

extern void
set_cal_links(Dtcm_calendar *cal) {
	int	idx;
	char	*idx_name;

	for (idx = 0; idx < cal->count; idx++) {
		idx_name = cal->attrs[idx].name;
		if (strcmp(idx_name, CSA_CAL_ATTR_CALENDAR_NAME) == 0)
			cal->cal_name = &cal->attrs[idx];
		else if (strcmp(idx_name, CSA_X_DT_CAL_ATTR_DATA_VERSION) == 0)
			cal->server_version = &cal->attrs[idx];
		else if (strcmp(idx_name, CSA_CAL_ATTR_NUMBER_ENTRIES) == 0)
			cal->num_entries = &cal->attrs[idx];
		else if (strcmp(idx_name, CSA_CAL_ATTR_CALENDAR_SIZE) == 0)
			cal->cal_size = &cal->attrs[idx];
		else if (strcmp(idx_name, CSA_CAL_ATTR_ACCESS_LIST) == 0)
			cal->access_list = &cal->attrs[idx];
		else if (strcmp(idx_name, CSA_CAL_ATTR_TIME_ZONE) == 0)
			cal->time_zone = &cal->attrs[idx];
	}
}

extern void
setup_range(CSA_attribute **attrs, CSA_enum **ops, int *count, time_t start,
	    time_t stop, CSA_sint32 type, CSA_sint32 state, boolean_t use_state,
	    int version) {
	int		a_size, o_size;
	CSA_enum	*op_ptr;
	CSA_attribute	*attr_ptr;


	if (use_state)
		*count = 4;
	else
		*count = 3;

	a_size = sizeof(CSA_attribute) * (*count);
	attr_ptr = (CSA_attribute *)ckalloc(a_size);
	memset(attr_ptr, 0, a_size);

	o_size = sizeof(CSA_enum) * (*count);
	op_ptr = (CSA_enum *)ckalloc(o_size);
	memset(op_ptr, 0, o_size);

	initialize_entry_attr(CSA_ENTRY_ATTR_START_DATE_I, &attr_ptr[0], appt_write, version);
	attr_ptr[0].value->item.string_value = malloc(BUFSIZ);
	_csa_tick_to_iso8601(start, attr_ptr[0].value->item.string_value);

	op_ptr[0] = CSA_MATCH_GREATER_THAN_OR_EQUAL_TO;

	initialize_entry_attr(CSA_ENTRY_ATTR_START_DATE_I, &attr_ptr[1], appt_write, version);
	attr_ptr[1].value->item.string_value = malloc(BUFSIZ);
	_csa_tick_to_iso8601(stop, attr_ptr[1].value->item.string_value);
	op_ptr[1] = CSA_MATCH_LESS_THAN_OR_EQUAL_TO;

	initialize_entry_attr(CSA_ENTRY_ATTR_TYPE_I, &attr_ptr[2], appt_write, version);
	attr_ptr[2].value->item.sint32_value = type;
	op_ptr[2] = CSA_MATCH_EQUAL_TO;

	if (use_state) {
		initialize_entry_attr(CSA_ENTRY_ATTR_STATUS_I, &attr_ptr[3], appt_write, version);
		attr_ptr[3].value->item.sint32_value = state;
		op_ptr[3] = CSA_MATCH_EQUAL_TO;
	}

	*attrs = attr_ptr;
	*ops = op_ptr;
}

extern void
free_range(CSA_attribute **attrs, CSA_enum **ops, int count) {
	int	i;

	for (i = 0; i < count; i++) {
		free((*attrs)[i].name);
		if (((*attrs)[i].value->type == CSA_VALUE_STRING) ||
		    ((*attrs)[i].value->type == CSA_VALUE_DATE_TIME))
			if ((*attrs)[i].value->item.string_value)
				free((*attrs)[i].value->item.string_value);

		free((*attrs)[i].value);
	}

	/* This memory was allocated by the client, and must be freed 
	   by the client */

	free(*attrs);

	*attrs = NULL;

	free(*ops);
	*ops = NULL;
}

/*
 * In Motif you can't associate user data with items in a list.  To get around
 * this we have the following simple functions (CmDataList*) that maintain
 * a list of user data.  We follow the intrinscs coding style to re-inforce
 * the relationship these routines have to the XmList* functions.
 */

/*
 * Create a list to store user data
 */
CmDataList *
CmDataListCreate(void)

{
	return (CmDataList *)calloc(1, sizeof(CmDataList));
}

/*
 * Destroy list
 */
void
CmDataListDestroy(CmDataList *list, int free_data)

{
	CmDataListDeleteAll(list, free_data);
	free(list);
}

/*
 * Create node to hold data in list.
 */
static CmDataItem *
CmDataItemCreate(void)

{
	return (CmDataItem *)calloc(1, sizeof(CmDataItem));
}

/*
 * Add user data to list at specified position. Note that this
 * routine must be called for every item added to a list.
 * If the item has no user data, just pass NULL.
 *
 *	list		List to add data to
 *	data		User data. NULL for no data.
 *	position	Where to insert data, starting with 1 for the
 *			first item.  0 to append to end of list.
 *
 * Returns
 *		1	Success
 *		-1	Invalid position
 */
int
CmDataListAdd(CmDataList *list, void *data, int position)

{
	CmDataItem	*item, *p;
	int		n;

	/* Create new node to hold data */
	item = CmDataItemCreate();
	item->data = data;

	/* Insert node into list at appropriate spot */
	if (list->head == NULL) {
		list->head = item;
	} else if (position == 0) {
		/* Special case.  0 means append to end */
		list->tail->next = item;
	} else if (position == 1) {
		item->next = list->head;
		list->head = item;
	} else {
		for (n = 2, p = list->head; p != NULL && n < position;
		     p = p->next, n++)
			;

		if (p == NULL) {
			return -1;
		}

		item->next = p->next;
		p->next = item;
	}

	/* If new item is at the end of the list, update tail */
	if (item->next == NULL) {
		list->tail = item;
	}

	list->count++;

	return 1;
}

/*
 * Delete user data from a position in the list.  If free_data is
 * True then this routine will call free(3C) on the user data, otherwise
 * the user data is returned so that the caller can dispose of it.
 * This routine should be called anytime you delete an item from a
 * scrolling list.
 *
 *	list		List to delete data from
 *	position	Location of item to delete. 0 for last item
 *	free_data	True if you want this routine to call free()
 *			on the data for you.  Otherwise this routine
 *			will return the address of the data.
 *
 * Returns
 *	NULL	No data found for item at the specified position
 *	1	Success (free_data was True)
 *	other	Address of data for item at the specified position.
 *		(free_data was False)
 */
void *
CmDataListDeletePos(CmDataList *list, int position, int free_data)

{
	void		*data;
	CmDataItem	*p, *item;
	int		n;

	/* Special case. 0 means delete last item */
	if (position == 0) {
		position = list->count;
	}

	if (list->head == NULL) {
		return NULL;
	} else if (position == 1) {
		item = list->head;
		list->head = item->next;
		if (list->tail == item) {
			list->tail = item->next;
		}
	} else {
		for (n = 2, p = list->head; p->next != NULL && n < position;
		     p = p->next, n++)
			;
		
		if (p->next == NULL) {
			return NULL;
		}

		item = p->next;
		p->next = item->next;
		if (list->tail == item) {
			list->tail = p;
		}
	}

	list->count--;

	data = item->data;
	free(item);

	if (free_data) {
		if (data != NULL) 
			free(data);
		return (void *)1;
	} else {
		return data;
	}
}

/* 
 * Delete all nodes in the list.
 *
 * 	list		List to delete nodes from
 *	free_data	True if you want this routine to call free()
 *			on the data for you.
 */
void
CmDataListDeleteAll(CmDataList *list, int free_data)

{
	CmDataItem	*p, *tmp;

	p = list->head;
	while (p != NULL) {
		if (free_data && p->data != NULL) {
			free(p->data);
		}

		tmp = p;
		p = p->next;
		free(tmp);
	}

	list->count = 0;
	list->head = NULL;
	list->tail = NULL;

	return;
}

/*
 * Get data for the item at a particular position in a list.
 */
void *
CmDataListGetData(CmDataList *list, int position)

{
	void		*data;
	CmDataItem	*p, *item;
	int		n;

	if (list->head == NULL) {
		return NULL;
	} else if (position == 0) {
		data = list->tail->data;
	} else {
		for (n = 1, p = list->head; p != NULL && n < position;
		     p = p->next, n++)
			;
		if (p == NULL) {
			return NULL;
		}
		data = p->data;
	}

	return data;
}

#ifdef DEBUG
/*
 * For dumping contents of list
 */
void
CmDataListDump(CmDataList *list)

{
	CmDataItem	*p;
	int		n;

	printf("***** %d items:\n", list->count);

	for (p = list->head, n = 1; p != NULL; p = p->next, n++) {
		printf("%3d: %s\n", n, p->data ? (char *)p->data : "<nil>");
	}

	return;
}
#endif
