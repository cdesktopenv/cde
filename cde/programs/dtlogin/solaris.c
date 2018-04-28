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
/* $XConsortium: solaris.c /main/4 1995/10/27 16:15:38 rswiston $ */
/*******************************************************************************
**
**  solaris.c 1.15 95/09/10
**
**  Copyright 1993, 1994, 1995 Sun Microsystems, Inc.  All rights reserved.
**  
**  This file contains procedures specific to Sun Solaris login
**
*******************************************************************************/
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*******************************************************************************
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
**  Copyright 1994 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/


/*
 * Header Files
 */

#include <sys/types.h>
#include <utmpx.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/param.h>
#include <pwd.h>
#include "solaris.h"

#ifdef SUNAUTH
#include <security/ia_appl.h>
#endif


/*
 * Local function declarations, structures, and variables
 */

static char* create_devname(char* short_devname);
static void  dir_dev_acc(char *, uid_t, gid_t, mode_t, char *);
static void logindevperm(char *, uid_t, gid_t);
static void resetdevperm(char *);

#ifdef SUNAUTH
static int login_conv(int conv_id, int num_msg, struct ia_message **msg,
    		      struct ia_response **response, void *appdata_ptr);
static void end_conv();

static struct ia_conv ia_conv = {login_conv, login_conv, end_conv, NULL};
static char *saved_user_passwd;
#endif SUNAUTH


/****************************************************************************
 * solaris_setdevperm
 *
 * Modify permission of devices as specified in Solaris /etc/logindevperm file
 ****************************************************************************/

int 
solaris_setdevperm(char* gettyLine, uid_t uid, gid_t gid) 
{
	char *devname = create_devname(gettyLine);

	if (devname == NULL) { 
	    Debug ("solaris_setdevperm:  NULL devname\n");
	    return(0);
	};

	logindevperm(devname, uid, gid);
	free(devname);
	return(1);
}


/****************************************************************************
 * solaris_resetdevperm
 *
 * Reset permission on devices specified in /etc/logindevperm file 
 ****************************************************************************/

int 
solaris_resetdevperm(char* gettyLine) 
{
	char *devname = create_devname(gettyLine);

	if (devname == NULL) {
	    Debug ("solaris_resetdevperm:  NULL devname\n");
	    return(0);
	}

        resetdevperm(devname);
        free(devname);
	return(1);
}


#ifdef SUNAUTH 
/****************************************************************************
 * solaris_authentication
 *
 * Authenticate that user / password combination is legal for this system
 *
 ****************************************************************************/

int
solaris_authenticate ( char*   prog_name,
		       char*   display_name,
		       char*   user_passwd,
		       char*   user, 
		       char*   line )
{
	int status;
	struct ia_status out;
	void *iah;		/* PAM authentication handle */
        char* user_str = user ? user : "NULL";
        char* line_str = line ? line : "NULL";
	char *line_dev = create_devname(line_str);  

        Debug("solaris_authenticate: prog_name=%s\n", prog_name);
        Debug("solaris_authenticate: display_name=%s\n", display_name);
        Debug("solaris_authenticate: user=%s\n", user_str);
        Debug("solaris_authenticate: line=%s\n", line_str);
        Debug("solaris_authenticate: line_dev=%s\n", line_dev);

	if (user_passwd) {
	    if (strlen(user_passwd) == 0) {
	        Debug("solaris_authenticate: user passwd empty\n"); 
	    } else {
	        Debug("solaris_authenticate: user passwd present\n"); 
	    }
        }
        else {
	    Debug("solaris_authenticate: user passwd NULL\n");

	    /* Password challenge required for solaris authentication */
	    return(IA_AUTHTEST_FAIL);
        }
        
        /* Solaris BSM Audit trail */

	audit_login_save_host(display_name);
	audit_login_save_ttyn(line_dev);
	audit_login_save_port();

        /* Open Solaris PAM (Plugable Authentication module ) connection */

	status = ia_start( prog_name, user, line_dev, 
			   display_name, &ia_conv, &iah );

	if (status == 0) {
	   int flags = AU_CHECK_PASSWD | AU_CONTINUE;
	   struct passwd *pwd;

	   saved_user_passwd = user_passwd;
           status = ia_auth_user( iah, flags, &pwd, &out );
	  	 
	   audit_login_save_pw(pwd);

	   if (status != 0) {
	      audit_login_bad_pw();
	      if (status == IA_MAXTRYS) {
		 audit_login_maxtrys();
	      }
	   }
	}

	if (status != 0) {
            Debug("solaris_authenticate: PAM error=%d\n", status);
	}  

	ia_end(iah);

	return(status);
}




/****************************************************************************
 * solaris_accounting
 *
 * Updates "utmp/utmpx: and "wtmp/wtmpx" system accounting entries
 *
 ****************************************************************************/

int
solaris_accounting( char*   prog_name,
		    char*   display_name,
		    char*   entry_id,
		    char*   user, 
		    char*   line, 
		    pid_t   pid,
		    int     entry_type,
		    int     exitcode )
{
	int session_type, status;
	struct ia_status out;
	void *iah;		/* PAM authentication handle */
        char* user_str = user ? user : "NULL";
        char* line_str = line ? line : "NULL";
	char *line_dev = create_devname(line_str);  

        Debug("solaris_accounting: prog_name=%s\n", prog_name);
        Debug("solaris_accounting: display_name=%s\n", display_name);
        Debug("solaris_accounting: entry_id=%c %c %c %c\n", entry_id[0],
				   entry_id[1], entry_id[2], entry_id[3]);
        Debug("solaris_accounting: user=%s\n", user_str);
        Debug("solaris_accounting: line=%s\n", line_str);
        Debug("solaris_accounting: line_dev=%s\n", line_dev);
        Debug("solaris_accounting: pid=%d\n", pid);
        Debug("solaris_accounting: entry_type=%d\n", entry_type);
        Debug("solaris_accounting: exitcode=%d\n", exitcode);


        /* Open Solaris PAM (Plugable Authentication module ) connection */

	if (entry_type == ACCOUNTING) {
	    status = ia_start(prog_name, user, line, 
			      display_name, &ia_conv, &iah);
	}
	else {
	    status = ia_start(prog_name, user, line_dev, 
			      display_name, &ia_conv, &iah);
	}


	/* Session accounting */

	if (status != IA_SUCCESS) {
            Debug("solaris_accounting: PAM error=%d)\n", status);
	} 
	else {
	    switch(entry_type) {

	    case USER_PROCESS:
		/* New user session, open session accounting logs */
		session_type = IS_LOGIN;
		status = ia_open_session(iah, session_type, 
					 entry_type, entry_id, &out); 
	        if (status == IA_SUCCESS) audit_login_success();
		break;


	    case ACCOUNTING:
		/* 
		 * User session has terminated, mark it DEAD and close 
		 * the sessions accounting logs. 
		 */
		entry_type = DEAD_PROCESS;
		session_type = IS_NOLOG;
		status = ia_open_session(iah, session_type, 
					 entry_type, entry_id, &out); 
	        if (status != IA_SUCCESS) {
                    Debug("solaris_accounting: PAM check seuser error=%d\n", 
			   status);
 		}
		/* Intentional fall thru */


	    case DEAD_PROCESS:
		/* Cleanup account files for dead processes */
		status = ia_close_session(iah, 0, pid, 
					  exitcode, entry_id, &out); 
		break;


	    case LOGIN_PROCESS:
	    default:	
		session_type = 0;
		status = ia_open_session(iah, session_type, 
					 entry_type, entry_id, &out); 
		break;
	    }
	}

	if (status != IA_SUCCESS) {
            Debug("solaris_accounting: PAM session error=%d\n", status);
 	}

	ia_end(iah);
	free(line_dev);
	return(status);
}



/****************************************************************************
 * solaris_setcred
 *
 * Set Users login credentials: uid, gid, and group lists 
 ****************************************************************************/

int
solaris_setcred(char* prog_name, char* user, uid_t uid, gid_t gid) 
{
	int cred_type, status;
	struct ia_status out;
	void *iah;		/* PAM authentication handle */
        char* user_str = user ? user : "NULL";

        Debug("solaris_setcred: prog_name=%s\n", prog_name);
        Debug("solaris_setcred: user=%s\n", user_str);
        Debug("solaris_setcred: uid=%d\n", uid);
        Debug("solaris_setcred: gid=%d\n", gid);


	/* Open PAM connection */	

	status = ia_start(prog_name, user, NULL, NULL, &ia_conv, &iah);
	if (status != 0) {
            Debug("solaris_setcred: PAM start error=%d\n", status);
	    return(status);
	}

	/* Set users credentials */

        ia_set_item(iah, IA_AUTHTOK, saved_user_passwd);

	cred_type = SC_INITGPS|SC_SETRID;
	status = ia_setcred(iah, cred_type, uid, gid, 0, NULL, &out);

	if (status != 0) {
            Debug("solaris_setcred: user=%s, err=%d)\n", user, status);
	}

	ia_end(iah);

        Debug("solaris_setcred: return status =%d\n", status);

	return(status);
}
#endif SUNAUTH 


/***************************************************************************
 * create_devname
 *
 * A utility function.  Takes short device name like "console" and returns 
 * a long device name like "/dev/console"
 ***************************************************************************/

static char* 
create_devname(char* short_devname)
{
       char* long_devname;

       if (short_devname == NULL)
	  short_devname = "";

       long_devname = (char *) malloc (strlen(short_devname) + 5);

       if (long_devname == NULL)
	  return(NULL); 

       strcpy(long_devname,"/dev/");
       strcat(long_devname, short_devname);
	
       return(long_devname);
}


/****************************************************************************
 * logindevperm 
 *
 * change owner/group/permissions of devices list in /etc/logindevperm.
 *
 * This code is directly from Sun Solaris 5.3 login.  Eventually this will
 * be a fuction in a Solaris library.  Till then, this is duplicate copy.  
 ***************************************************************************/

#define	LOGINDEVPERM	"/etc/logindevperm"
#define	DIRWILD		"/*"			/* directory wildcard */
#define	DIRWLDLEN	2			/* strlen(DIRWILD) */


static void
logindevperm(char *ttyn, uid_t uid, gid_t gid)
{
	char *field_delims = " \t\n";
	char *permfile = LOGINDEVPERM;
	char line[MAXPATHLEN];
	char *console;
	char *mode_str;
	char *dev_list;
	char *device;
	char *ptr;
	int mode;
	FILE *fp;
	size_t l;
	int lineno;

	if ((fp = fopen(permfile, "r")) == NULL)
		return;

	lineno = 0;
	while (fgets(line, MAXPATHLEN, fp) != NULL) {
		lineno++;

		if ((ptr = strchr(line, '#')) != NULL)
			*ptr = '\0';	/* handle comments */

		if ((console = strtok(line, field_delims)) == NULL)
			continue;	/* ignore blank lines */

		if (strcmp(console, ttyn) != 0)
			continue;	/* not our tty, skip */

		mode_str = strtok((char *)NULL, field_delims);
		if (mode_str == NULL) {
			(void) fprintf(stderr,
			    "%s: line %d, invalid entry -- %s\n", permfile,
			    lineno, line);
			continue;
		}

		/* convert string to octal value */
		mode = strtol(mode_str, &ptr, 8);
		if (mode < 0 || mode > 0777 || *ptr != '\0') {
			(void) fprintf(stderr,
			    "%s: line %d, invalid mode -- %s\n", permfile,
			    lineno, mode_str);
			continue;
		}

		dev_list = strtok((char *)NULL, field_delims);
		if (dev_list == NULL) {
			(void) fprintf(stderr,
			    "%s: line %d, %s -- empty device list\n",
			    permfile, lineno, console);
			continue;
		}

		device = strtok(dev_list, ":");
		while (device != NULL) {
			l = strlen(device);
			ptr = &device[l - DIRWLDLEN];
			if ((l > DIRWLDLEN) && (strcmp(ptr, DIRWILD) == 0)) {
				*ptr = '\0';	/* chop off wildcard */
				dir_dev_acc(device, uid, gid, mode, permfile);
			} else {
				/*
				 * change the owner/group/permission;
				 * nonexistent devices are ignored
				 */
				if (chown(device, uid, gid) == -1) {
					if (errno != ENOENT) {
						(void) fprintf(stderr, "%s: ",
						    permfile);
						perror(device);
					}
				} else {
					if ((chmod(device, mode) == -1) &&
					    (errno != ENOENT)) {
						(void) fprintf(stderr, "%s: ",
						    permfile);
						perror(device);
					}
				}
			}
			device = strtok((char *)NULL, ":");
		}
	}
	(void) fclose(fp);
}

/***************************************************************************
 * dir_dev_acc
 *
 * Apply owner/group/perms to all files (except "." and "..") in a directory.
 *
 * This code is directly Sun Solaris 5.3 login.   Eventually this will
 * be internal to a Solaris library.  Till then, this is a duplicate copy.  
 ***************************************************************************/

static void
dir_dev_acc(char *dir, uid_t uid, gid_t gid, mode_t mode, char *permfile)
{
	DIR *dirp;
	struct dirent *direntp;
	char *name, path[MAXPATHLEN];

	dirp = opendir(dir);
	if (dirp == NULL)
		return;

	while ((direntp = readdir(dirp)) != NULL) {
		name = direntp->d_name;
		if ((strcmp(name, ".") == 0) || (strcmp(name, "..") == 0))
			continue;

		(void) sprintf(path, "%s/%s", dir, name);
		if (chown(path, uid, gid) == -1) {
			if (errno != ENOENT) {
				(void) fprintf(stderr, "%s: ", permfile);
				perror(path);
			}
		} else {
			if ((chmod(path, mode) == -1) && (errno != ENOENT)) {
				(void) fprintf(stderr, "%s: ", permfile);
				perror(path);
			}
		}
	}
	(void) closedir(dirp);
}

/*****************************************************************************
 * resetdevperm
 *
 * Clean up access of devices in /etc/logindevperm by resetting the
 * owner/group/permissions back to the login pseudo user.
 *
 *****************************************************************************/

extern struct passwd   puser;	/* pseudo_user password entry	*/

static void
resetdevperm(char *ttyn)
{
	logindevperm(ttyn, puser.pw_uid, puser.pw_gid);
}

#ifdef SUNAUTH 
/*****************************************************************************
 * login_conv():
 *
 * This is a conv (conversation) function called from the PAM 
 * authentication scheme.  It returns the user's password when requested by
 * internal PAM authentication modules and also logs any internal PAM error
 * messages.
 *****************************************************************************/

static int
login_conv(int conv_id, int num_msg, struct ia_message **msg,
           struct ia_response **response, void *appdata_ptr)
{
	struct ia_message	*m;
	struct ia_response	*r;
	char 			*temp;
	int			k;

#ifdef lint
	conv_id = conv_id;
#endif
	if (num_msg <= 0)
		return (IA_CONV_FAILURE);

	*response = (struct ia_response*) 
				calloc(num_msg, sizeof (struct ia_response));
	if (*response == NULL)
		return (IA_CONV_FAILURE);

	(void) memset(*response, 0, sizeof (struct ia_response));

	k = num_msg;
	m = *msg;
	r = *response;
	while (k--) {

		switch (m->msg_style) {

		case IA_PROMPT_ECHO_OFF:
                    if (saved_user_passwd != NULL) {
                        r->resp = (char *) malloc(strlen(saved_user_passwd)+1);
                        if (r->resp == NULL) {
                     	    free_resp(num_msg, *response);
                            *response = NULL;
                            return (IA_CONV_FAILURE);
                        }
                        (void) strcpy(r->resp, saved_user_passwd);
                        r->resp_len = strlen(r->resp);
                    }

                    m++;
                    r++;
                    break;


		case IA_ERROR_MSG:
			if (m->msg != NULL) { 
	    	 	    Debug ("login_conv: %s\n", m->msg); 
			}
			m++;
			r++;
			break;

		default:
	    	 	Debug ("login_conv: Unexpected case %d\n", 
				m->msg_style);
			break;
		}
	}
	return (IA_SUCCESS);
}


/*****************************************************************************
 * end_conv():
 *
 * End of conversation function.  Called from PAM.  
 * Currently a noop for dtlogin.
 *****************************************************************************/

static void
end_conv()
{}
#endif SUNAUTH 

#ifdef PAM
/****************************************************************************
 * Following Solaris utmp management code was taken from portions of the 
 * private libauth open and close session code.  There are no equivalent calls
 * in libpam that replaces it, so the code is copied to here.  Eventually this
 * code may become functions in either PAM or other appropriate libraries.
 ***************************************************************************/

#define	INIT_PROC_PID	1
#define	PAMTXD		"SUNW_OST_SYSOSPAM"
#define	SCPYN(a, b)	(void) strncpy(a, b, sizeof (a))

/* utility function to do UTMP/WTMP management */
int
solaris_setutmp_mgmt(
	char *user,	/* user */
	char *ttyn,	/* ttyn */
	char *rhost,	/* remote hostname */
	int flags,	/* Flags - see below */
	int type,	/* type of utmp/wtmp entry */
	char id[]	/* 4 byte id field for utmp */
);

/*
 * solaris_reset_utmp_mgmt 
 *		a utility function which terminates UTMP/WTMP mgmt
 */
int
solaris_reset_utmp_mgmt(
	char **user,	/* user */
	char **ttyn,	/* tty name */
	char **rhost,	/* remote host */
	int flags,	/* flags - see below */
	int status,	/* logout process status */
	char id[]	/* logout ut_id (/etc/inittab id) */
);

/* flags for the flags field */
#define	__NOOP	8	/* No utmp action desired */

/* __setproc_cred is a utility function to set process credentials */
int
__setproc_cred(
	char *user,		/* user */
	int flags,		/* flags - see below */
	uid_t uid,		/* User ID to set for this process */
	gid_t gid,		/* Group ID */
	int ngroups,		/* Number of groups */
	gid_t *grouplist	/* Group list */
);

/* flags indicates specific set credential actions */

#define	__INITGROUPS	0x00000001	/* Request to initgroups() */
#define	__SETGROUPS	0x00000002	/* Request to setgroups() */
#define	__SETEGID	0x00000004	/* Set effective gid only */
#define	__SETGID	0x00000008	/* Set real gid */
#define	__SETEUID	0x00000010	/* Set effective uid only */
#define	__SETUID	0x00000020	/* Set real uid */
#define	__SETEID	(__SETEGID|__SETEUID)	/* Set effective ids only */
#define	__SETRID	(__SETGID|__SETUID)	/* Set real ids */

/*
 * solaris_setutmp_mgmt - A utility function used to do UTMP/WTMP management.
 *
 *	"user" is the current username.
 *	"ttyn" is the tty name.
 *	"rhost" is the remote hostname.
 *	The following flags may be set in the "flags" field:
 *
 *	SOLARIS_UPDATE_ENTRY	No new entry will be created if utmp
 *				entry not found - return __NOENTRY
 *	SOLARIS_NOLOG		Generate a wtmp/wtmpx entry only
 *	SOLARIS_LOGIN		Caller is a login application - update
 *				utmp entry accordingly
 *
 *	"type" is used to indicate the type of utmp/wtmp entry written
 *		(see also utmp.h and utmpx.h)
 *	"id is an optional application-defined 4 byte array that represents
 *	the /sbin/inittab id (created by the process that puts an entry in
 *	utmp).
 *
 *	Upon successful completion, SOLARIS_SUCCESS is returned.
 *	Error values may include:
 *
 *	SOLARIS_NOENTRY	    An entry for the specified process was not found
 *	SOLARIS_ENTRYFAIL   Could not make/remove entry for specified process
 */

int
solaris_setutmp_mgmt(
	char 	*user,
	char	*ttyn,
	char	*rhost,
	int	flags,
	int	type,
	char	id[])
{
	int			tmplen;
	struct utmpx    	*u = (struct utmpx *)0;
	struct utmpx		utmp;
	char			*ttyntail;
	int			err = PAM_SUCCESS;

	Debug("Enter: solaris_setutmp_mgmt()\n");

	(void) memset((void *)&utmp, 0, sizeof (utmp));

	(void) time(&utmp.ut_tv.tv_sec);
	utmp.ut_pid = getpid();
	if (rhost != NULL && rhost[0] != '\0') {
		(void) strcpy(utmp.ut_host, rhost);
		tmplen = strlen(rhost) + 1;
		if (tmplen < sizeof (utmp.ut_host))
			utmp.ut_syslen = tmplen;
		else
			utmp.ut_syslen = sizeof (utmp.ut_host);
	} else {
		(void) memset(utmp.ut_host, 0, sizeof (utmp.ut_host));
		utmp.ut_syslen = 0;
	}

	(void) strcpy(utmp.ut_user, user);
	/*
	 * Copy in the name of the tty minus the "/dev/" if a /dev/ is
	 * in the path name.
	 */

	if (!(flags&SOLARIS_LOGIN))
		SCPYN(utmp.ut_line, ttyn);

	ttyntail = ttyn;

	utmp.ut_type = type;

	if (id != NULL)
		(void) memcpy(utmp.ut_id, id, sizeof (utmp.ut_id));

	if ((flags & SOLARIS_NOLOG) == SOLARIS_NOLOG) {
		updwtmpx(WTMPX_FILE, &utmp);
	} else {
		/*
		 * Go through each entry one by one, looking only at INIT,
		 * LOGIN or USER Processes.  Use the entry found if flags == 0
		 * and the line name matches, or if the process ID matches if
		 * the UPDATE_ENTRY flag is set.  The UPDATE_ENTRY flag is
		 * mainly for login which normally only wants to update an
		 * entry if the pid fields matches.
		 */

		if (flags & SOLARIS_LOGIN) {
		    while ((u = getutxent()) != NULL) {
			if ((u->ut_type == INIT_PROCESS ||
			    u->ut_type == LOGIN_PROCESS ||
			    u->ut_type == USER_PROCESS) &&
			    ((flags == SOLARIS_LOGIN && ttyn != NULL &&
					strncmp(u->ut_line, ttyntail,
						sizeof (u->ut_line)) == 0) ||
			    u->ut_pid == utmp.ut_pid)) {
				if (ttyn)
					SCPYN(utmp.ut_line,
					    (ttyn + sizeof ("/dev/") - 1));
				if (id == NULL) {
					(void) memcpy(utmp.ut_id, u->ut_id,
						    sizeof (utmp.ut_id));
				}
				(void) pututxline(&utmp);
				break;
			}
		    }	   /* end while */
		    endutxent();		/* Close utmp file */
		}

		if (u == (struct utmpx *)NULL) {
			/* audit_login_main11(); */
			if (flags & SOLARIS_UPDATE_ENTRY)
				err =  SOLARIS_NOENTRY;
			else
				(void) makeutx(&utmp);
		}
		else
			updwtmpx(WTMPX_FILE, &utmp);
	}
	return (err);
}

/*
 * solaris_reset_utmp_mgmt 
 *		A utility function used to terminate  UTMP/WTMP mgmt.
 *
 *	"user" is the current username.
 *	"ttyn" is the tty name.
 *	"rhost" is the remote hostname.
 *	The following flags may be set in the "flags" field:
 *
 *	SOLARIS_NOLOG		Write a wtmp/wtmpx entry only
 *	SOLARIS_NOOP		Ignore utmp/wtmp processing
 *
 *	"status" is the logout process status.
 *	"id is an optional application-defined 4 byte array that represents
 *	the /sbin/inittab id (created by the process that puts an entry in
 *	utmp).
 *
 *	Upon successful completion, PAM_SUCCESS is returned.
 *	Error values may include:
 *
 *	SOLARIS_NOENTRY	   An entry for the specified process was not found
 *	SOLARIS_ENTRYFAIL  Could not make/remove entry for specified process
 */

int
solaris_reset_utmp_mgmt(
	char	**user,
	char	**ttyn,
	char	**rhost,
	int	flags,
	int	status,
	char	id[])
{
	struct utmpx 		*up;
	struct utmpx  		ut;
	int			err = 0;
	int			pid;

	Debug("Enter: solaris_reset_utmp_mgmt()\n");


	pid = (int) getpid();

	if ((flags & SOLARIS_NOLOG) == SOLARIS_NOLOG) {	
	        /* only write to wtmp files */
			/* clear utmpx entry */
		(void) memset((char *)&ut, 0, sizeof (ut));

		if (id != NULL)
			(void) memcpy(ut.ut_id, id, sizeof (ut.ut_id));

		if (*ttyn != NULL && **ttyn != '\0') {
			if (strstr(*ttyn, "/dev/") != NULL)
			(void) strncpy(ut.ut_line, (*ttyn+sizeof ("/dev/")-1),
							sizeof (ut.ut_line));
			else
				(void) strncpy(ut.ut_line, *ttyn,
							sizeof (ut.ut_line));
		}
		ut.ut_pid  = pid;
		ut.ut_type = DEAD_PROCESS;
		ut.ut_exit.e_termination = 0;
		ut.ut_exit.e_exit = 0;
		ut.ut_syslen = 1;
		(void) gettimeofday(&ut.ut_tv, NULL);
		updwtmpx(WTMPX_FILE, &ut);

		return (PAM_SUCCESS);
	} else {
		setutxent();
		while (up = getutxent()) {
			if (up->ut_pid == pid) {
				if (up->ut_type == DEAD_PROCESS) {
					/*
					 * Cleaned up elsewhere.
					 */
					endutxent();
					return (0);
				}
				if ((*user = (char *) strdup (up->ut_user))
							== NULL ||
				    (*ttyn = (char *) strdup (up->ut_line))
							== NULL ||
				    (*rhost = (char *) strdup (up->ut_host))
							== NULL)
					return (PAM_BUF_ERR);

				up->ut_type = DEAD_PROCESS;
				up->ut_exit.e_termination = status & 0xff;
				up->ut_exit.e_exit = (status >> 8) & 0xff;
				if (id != NULL)
					(void) memcpy(up->ut_id, id,
						sizeof (up->ut_id));
				(void) time(&up->ut_tv.tv_sec);

				/*
				 * For init (Process ID 1) we don't write to
				 * init's pipe, since we are init.
				 */
				if (getpid() == INIT_PROC_PID) {
					(void) pututxline(up);
					/*
					 * Now attempt to add to the end of the
					 * wtmp and wtmpx files.  Do not create
					 * if they don't already exist.
					 */
					updwtmpx("wtmpx", up);
				} else {
					if (modutx(up) == NULL) {
						Debug("modutx failed\n");
						/*
						 * Since modutx failed we'll
						 * write out the new entry
						 * ourselves.
						 */
						(void) pututxline(up);
						updwtmpx("wtmpx", up);
					}
				}

				endutxent();

				return (PAM_SUCCESS);
			}
		}

		endutxent();
		return (SOLARIS_NOENTRY);
	}

}

#endif PAM
