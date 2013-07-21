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
/*
 * File:         connect.c $TOG: connect.c /main/8 1998/04/09 17:44:33 mgreess $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#include <Dt/UserMsg.h>

#include <bms/sbport.h> 		/* NOTE: sbport.h must be the first include. */

#include <sys/socket.h>         /**** needed by gethostbyname et al *****/
#define X_INCLUDE_NETDB_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>

#include <bms/bms.h>              
#include <bms/connect.h>          
#include <bms/MemoryMgr.h>       /* Xe_make_struct, make_str ...       */
#include <bms/XeUserMsg.h>
#include <bms/pathwexp.h>        /* Xe_shellexp			   */
#include "DtSvcLock.h"

#include <codelibs/pathutils.h>

/*
 * local variables
 */
static XeString context_host = NULL;

static XeString FindDomainHost (XeString host_spec);
static void UnParseFileString (XeString host, XeString path);
static int GetDomainName (XeString buffer, unsigned int bufsize);

#define strequal(xxx_str1, xxx_str2) (!strcmp(xxx_str1, xxx_str2))

/*------------------------------------------------------------------------+*/
XeString 
XeCreateContextString(XeString host,
		      XeString directory,
		      XeString file)
/*------------------------------------------------------------------------+*/
{
   XeString context_string;

   host = XeFindShortHost(host);

   if ((strequal(directory, (XeString)"")) || (directory == NULL)){
      if ((strequal(file, (XeString)"")) || (file == NULL)){
	 return((XeString) NULL);
      } else {
	 context_string = XeMalloc (strlen(host) + strlen(file) + 2);
	 sprintf (context_string, "%s:%s", host, file);
      }
   } else {
      if ((strequal(file, (XeString)"")) || (file == NULL)){
	 context_string = XeMalloc(strlen(host) + strlen(directory) + 2);
	 sprintf (context_string, "%s:%s", host, directory);
      }
      else {
	 context_string = XeMalloc(strlen(host) + strlen(directory) + 
			  strlen(file) + 3);
	 sprintf (context_string, "%s:%s/%s", host, directory, file);
      }
   }
   Xe_release_str(host);
   return(context_string);
}

/*------------------------------------------------------------------------+*/
XeString 
XeFindShortHost(XeString host_spec)
/*------------------------------------------------------------------------+*/
{
   XeString host, ptr, ptr2;
   XeChar localhost[MAXHOSTNAMELEN];
   
   if (!host_spec || !host_spec[0] || strequal(host_spec, (XeString)"-")) {
      _DtSvcProcessLock();
      if (!context_host){
	 context_host = (XeString) XeMalloc (MAXHOSTNAMELEN);
         Xegetcwd(context_host, MAXHOSTNAMELEN);
      }
      _DtSvcProcessUnlock();
      host_spec = context_host;
   }
   host_spec = Xe_shellexp(host_spec);
   ptr = strstr(host_spec, (XeString)".");
   if (!ptr) /* short name already */
      host = strdup(host_spec);
   else {
      GetDomainName(localhost, MAXHOSTNAMELEN);
      ptr2 = strstr(localhost, (XeString)".");
      if (ptr2 && strequal(ptr, ptr2)) { /* domains same, can eliminate */
         host = Xe_make_ntype(ptr-host_spec+1, XeChar);
         strncpy(host, host_spec, ptr-host_spec); /* copy only up to "." */
         host[ptr-host_spec] = '\0'; /* NULL terminate copy */
      }
      else
         host = strdup(host_spec);
   }
   return host;
}

/* temporary without domain comparisons */
/*------------------------------------------------------------------------+*/
XeString 
XeFindHost(XeString host_spec)
/*------------------------------------------------------------------------+*/
{
   if (!host_spec || !host_spec[0] || strequal(host_spec, (XeString)"-")) {
      _DtSvcProcessLock();
      if (!context_host){
	 context_host = (XeString) XeMalloc (MAXHOSTNAMELEN);
         Xegetcwd(context_host, MAXHOSTNAMELEN);
      }
      _DtSvcProcessUnlock();
      host_spec = context_host;
   }
   host_spec = Xe_shellexp(host_spec);
   return strdup(host_spec);
}


/* this should be XeFindHost but is called other name for 1.0 operation
   using only simple names if in short domain.  See defect HZNlp05737 */

/*------------------------------------------------------------------------+*/
static XeString 
FindDomainHost(XeString host_spec)
/*------------------------------------------------------------------------+*/
{
   XeString host;
   XeString ptr;
   XeChar localhost[MAXHOSTNAMELEN];
   
   if (!host_spec || !host_spec[0] || strequal(host_spec, (XeString)"-")) {
      _DtSvcProcessLock();
      if (!context_host){
	 context_host = (XeString) XeMalloc (MAXHOSTNAMELEN);
         Xegetcwd(context_host, MAXHOSTNAMELEN);
      }
      _DtSvcProcessUnlock();
      host_spec = context_host;
   }
   else if (strequal(host_spec, (XeString)"*")) {
      host_spec = strdup((XeString)"*");
      return host_spec;
   }
   host_spec = Xe_shellexp(host_spec);
   ptr = strstr(host_spec, (XeString)".");
   if (ptr)
      host = strdup(host_spec);
   else {
      GetDomainName(localhost, MAXHOSTNAMELEN);
      ptr = strstr(localhost, (XeString)"."); /* points to domain name if one exists */
      if (ptr) {
	 host = Xe_make_ntype(strlen(host_spec)+strlen(ptr)+1, XeChar);
	 strcpy(host, host_spec);
	 strcat(host, ptr);
      }
      else
	 host = strdup(host_spec);
   }
   return(host);
}

/******  XeParseFileString and UnParseFileString work together to munge and
         unmunge a path into a host path pair.  UnParseFileString DOES NOT WORK
	 FOR ARBITRARY STRINGS.  It is not a general functions (it is a hack).
	 Do not use It as a general function.
******/

/*------------------------------------------------------------------------+*/
static void
UnParseFileString(XeString host, XeString path)
/*------------------------------------------------------------------------+*/
{
   if (host) { /* there was a host in the original string */
      *--path = (XeChar) ':';
   }
}


/*------------------------------------------------------------------------+*/
int
XeParseFileString(XeString line,
		  XeString *host_addr,
		  XeString *path_addr)
/*------------------------------------------------------------------------+*/
{
   XeString current_position = line;

   if ((XeChar)'/' == *line) {  
      *host_addr = XeString_NULL;
      *path_addr = line;
      return(0);
   }
     
   while (*current_position && ((XeChar) ':' != *current_position)) {
     int len;
     if ((len = mblen(current_position, MB_CUR_MAX)) > 1)
         current_position += len;
     else
         current_position++;
   }
   if (*current_position) {                 /* host was specified */
      *current_position++ = XeChar_NULL;  /* ":" goes to NULL */
      *host_addr = line;
      *path_addr = current_position;
   } else {
      *host_addr = XeString_NULL;
      *path_addr = line;
   }
   return(0);
}

/***** Xegethostname is a replacement for gethostname which always returns the
       canonical (domain) hostname
******/

static XeString domainname = XeString_NULL;

/* this should be Xegethostname but is called other name for 1.0 operation
   using only simple names if in short domain.  See defect HZNlp05737 */

/*------------------------------------------------------------------------+*/
static int
GetDomainName(XeString buffer, unsigned int bufsize)
/*------------------------------------------------------------------------+*/
{
   XeString tmpbuf = Xe_make_buffer(bufsize);
   XeString ptr;
   struct hostent	*host_ret;
   _Xgethostbynameparams host_buf; 
   static Boolean firstPass = TRUE;
   int status;
   
   /* try to get domain name from hostname */
   if ((status = gethostname(tmpbuf, bufsize))) {
      XeFree(tmpbuf);
      return status; /* failed gethostname */
   }
   ptr = strstr(tmpbuf, (XeString)".");
   _DtSvcProcessLock();
   if (domainname && ptr && strcmp(domainname, ptr)) /* domains are different */
      _DtSimpleError(XeProgName, XeWarning, NULL, (XeString) "><Domain configured in hostname and domain server are different: '%s', '%s'", domainname, ptr);
   if (!domainname && ptr)
      domainname = strdup(ptr);
   if (ptr) { /* "." in hostname */
      strncpy(buffer, tmpbuf, bufsize);
      XeFree(tmpbuf);
      _DtSvcProcessUnlock();
      return 0;
   }

   /* look up domain name in domain server */
   if (!domainname && (firstPass)) {
      firstPass = FALSE;
      host_ret = _XGethostbyname(tmpbuf, host_buf);
      if (host_ret == NULL) {
	 _DtSimpleError(XeProgName, XeWarning, NULL, (XeString) "><%s not found in hosts database", tmpbuf);
	 _DtSvcProcessUnlock();
	 return -1;
      }
      if ((ptr = strstr(host_ret->h_name, (XeString)"."))) /* if dot in canonical name */
	 domainname = strdup(ptr);
   }

   /* construct full domain name for return */
   strncpy(buffer, tmpbuf, bufsize);
   if (domainname)
      strncat(buffer, domainname, bufsize - strlen(tmpbuf));
   XeFree(tmpbuf);
   _DtSvcProcessUnlock();
   return 0;
}

/*------------------------------------------------------------------------+*/
int
Xegetshorthostname(XeString buffer, unsigned int bufsize)
/*------------------------------------------------------------------------+*/
{
   XeString ptr;
   int status;
   
   if ((status = gethostname(buffer, bufsize)))
      return status; /* failed gethostname */
   if ((ptr = strstr(buffer, (XeString)".")))
      *ptr = '\0';  /* delete domain name if there is one */
   return 0;
}

/*------------------------------------------------------------------------+*/
int
Xegethostname(XeString buffer, unsigned int bufsize)
/*------------------------------------------------------------------------+*/
{
   return Xegetshorthostname(buffer, bufsize);
}


/*------------------------------------------------------------------------+*/
Boolean
XeIsLocalHostP(XeString hostname)
/*------------------------------------------------------------------------+*/
{
   XeChar localhost[MAXHOSTNAMELEN];
   XeString found_host = FindDomainHost(hostname);
   int status;
   
   GetDomainName(localhost, MAXHOSTNAMELEN);
   status = strcmp(localhost, found_host)==0;
   XeFree(found_host);
   return status;
}

/*------------------------------------------------------------------------+*/
Boolean
XeIsSameHostP(XeString host1, XeString host2)
/*------------------------------------------------------------------------+*/
{
   XeString long_host1 = FindDomainHost(host1);
   XeString long_host2 = FindDomainHost(host2);
   int status = strcmp(long_host1, long_host2) == 0;
   if (!host1 || !host1[0] || !strcmp(host1,(XeString)"-")
       || !host2 || !host2[0] || !strcmp(host2,(XeString)"-"))
	status = strcmp(host1, host2) == 0;

   XeFree(long_host1);
   XeFree(long_host2);
   return status;
}


/* note the following functions use char not Xechar because they are
   plug replacements for the Unix functions */

static char *last_env_string = NULL; /* save env so it can be freed later */

/* Note: only use this function for $PWD as it assumes it can free the
   env variable when a new one is assigned -- this will only be true if
   the same variable is used for all calls to copying_putenv */
static void
copying_putenv(char *env)
{
   char *env_copy = XeCopyStringM(env);
   putenv(env_copy);
   _DtSvcProcessLock();
   if (last_env_string)
      XeFree(last_env_string);
   last_env_string = env_copy;
   _DtSvcProcessUnlock();
}

/*------------------------------------------------------------------------+*/
char *
Xegetcwd(char *buf, int size)
/*------------------------------------------------------------------------+*/
{
   Boolean pwd_ok = FALSE;
   char *env, *current_dir;

   if ((current_dir = getenv("PWD"))) { /* use PWD instead of slow call */
      int s1, s2;
      struct stat sb1, sb2;

      /* The code used to copy size-1 bytes.  This is a waste most of */
      /* the time.  All we need to copy is strlen($PWD) bytes unless  */
      /* there are more bytes that fit into the array passed in.      */

      int len=strlen(current_dir);
      if (len > size-1)
	  len = size-1;
      
      strncpy(buf, current_dir, len);
      buf[len] = '\0';

      /* Make sure $PWD is the same as "." before we trust it. */
      /* All this is still much faster the getcwd() esp. on UX discless. */
      s1 = stat(buf, &sb1);
      s2 = stat(".", &sb2);

      /* If device and inode are the same, we have a match */
      pwd_ok = ((s1 == 0 && s2 == 0) &&
		(sb1.st_dev == sb2.st_dev && sb1.st_ino == sb2.st_ino) );
   }     

   if (!pwd_ok) {
      current_dir = getcwd(buf, size);
      env = XeMalloc(MAXPATHLEN+10);
      sprintf(env, "PWD=%s", buf);
      copying_putenv(env);	/* set PWD if necessary for later cache use */
      if (env) XeFree(env);
   }
   return current_dir;
}

/*------------------------------------------------------------------------+*/
int
Xechdir (const char *path)
/*------------------------------------------------------------------------+*/
{
   int status;
   char *simple_path = NULL;
   char *env;
   char buf[MAXPATHLEN+10];
   if ((status = chdir(path))==0) {
      env = XeMalloc(MAXPATHLEN+10);

      if (path[0] != '/') { /* relative path */
	 path = getcwd(buf, sizeof buf);
	 sprintf(env, "PWD=%s", path);
      }
      else {
	 char *canon_path = pathcollapse(path, NULL, FALSE);
         /* absolute path */
         sprintf(env, "PWD=%s", canon_path);
	 /* note XeFree() not appropriate if Xemalloc not used */
         free(canon_path); 
      }
      
      copying_putenv(env); /* update PWD if directory changed */
      if (env) XeFree(env);
   }
   return status;
}
