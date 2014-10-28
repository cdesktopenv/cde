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
/* $TOG: pam_svc.c /main/5 1997/06/04 16:30:21 samborn $ */
/*******************************************************************************
**
**  pam_svc.c 1.10 95/11/25
**
**  Copyright 1993, 1994, 1995 Sun Microsystems, Inc.  All rights reserved.
**
**  This file contains procedures specific to dtlogin's use of
**  PAM (Pluggable Authentication Module) security library.
**
*******************************************************************************/
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.               	*
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

/*
 * Header Files
 */

#include <utmpx.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/param.h>
#include <pwd.h>
#include <security/pam_appl.h>
#include "pam_svc.h"
#include "dm.h"
#include "solaris.h"

/*
 * Local function declarations
 */

static int login_conv(int num_msg, struct pam_message **msg,
    		      struct pam_response **response, void *appdata_ptr);

static char* create_devname(char* short_devname);



/*
 * Local structures and variables 
 */

static struct pam_conv pam_conv = {login_conv, NULL};
static char *saved_user_passwd;
static pam_handle_t *pamh = NULL;
static int pam_auth_trys = 0;

/****************************************************************************
 * PamInit
 *
 * Initialize or Update PAM datastructures.
 *
 ****************************************************************************/

int 
PamInit(char* prog_name,
        char* user, 
	char* line_dev,
        char* display_name)
{
	int status=PAM_SUCCESS;

        if (!pamh) {
            /* Open PAM (Plugable Authentication module ) connection */
	    status = pam_start( prog_name, user, &pam_conv, &pamh );
	    if (status != PAM_SUCCESS) {
        	Debug("PamInit: pam_start error=%d\n", status);
	        pamh = NULL;
	    } else {
        	Debug("PamInit: pam_start success\n");
	    }
	} else {
	    if (prog_name) pam_set_item(pamh, PAM_SERVICE, prog_name);
	    if (user) pam_set_item(pamh, PAM_USER, user);
	}

	if (status == PAM_SUCCESS) {
            if (line_dev) pam_set_item(pamh, PAM_TTY, line_dev);
	    if (display_name) pam_set_item(pamh, PAM_RHOST, display_name);
	}

	return(status);
}

/****************************************************************************
 * PamAuthenticate
 *
 * Authenticate that user / password combination is legal for this system
 *
 ****************************************************************************/

int
PamAuthenticate ( char*   prog_name,
		  char*   display_name,
		  char*   user_passwd,
		  char*   user, 
		  char*   line )
{
	int status;
        char* user_str = user ? user : "NULL";
        char* line_str = line ? line : "NULL";
	char* line_dev = create_devname(line_str);  

        Debug("PamAuthenticate: prog_name=%s\n", prog_name);
        Debug("PamAuthenticate: display_name=%s\n", display_name);
        Debug("PamAuthenticate: user=%s\n", user_str);
        Debug("PamAuthenticate: line=%s\n", line_str);
        Debug("PamAuthenticate: line_dev=%s\n", line_dev);

	if (user_passwd) {
	    if (strlen(user_passwd) == 0) {
	        Debug("PamAuthenticate: user passwd empty\n"); 
	    } else {
	        Debug("PamAuthenticate: user passwd present\n"); 
	    }
        }
        else {
	    Debug("PamAuthenticate: user passwd NULL\n");

	    /* Password challenge required for dtlogin authentication */
	    return(PAM_AUTH_ERR);
        }

#ifdef legacysun
        /* Solaris BSM Audit trail */

	audit_login_save_host(display_name);
	audit_login_save_ttyn(line_dev);
	audit_login_save_port();
#endif

	status = PamInit(prog_name, user, line_dev, display_name);

	if (status == PAM_SUCCESS) {
	   struct passwd *pwd;

	   saved_user_passwd = user_passwd;
           status = pam_authenticate( pamh, 0 );
	   pam_auth_trys++;

#ifdef legacysun
	   pwd = getpwnam(user);
	   audit_login_save_pw(pwd);

	   if (status != PAM_SUCCESS) {
	      audit_login_bad_pw();

              if (status == PAM_MAXTRIES) {
		 pam_auth_trys = PAM_LOGIN_MAXTRIES+1;
	      }	

	      if (pam_auth_trys > PAM_LOGIN_MAXTRIES) {
		 audit_login_maxtrys();
	      }
	   }
#endif

	   if (status != PAM_SUCCESS) {
	      sleep(PAM_LOGIN_SLEEPTIME);
	      if (pam_auth_trys > PAM_LOGIN_MAXTRIES) {
	         sleep(PAM_LOGIN_DISABLETIME);
	      }
	   }
	};

	if (status != PAM_SUCCESS) {
            Debug("PamAuthenticate: PAM error=%d\n", status);
	    if (pamh) {
                Debug("PamAuthenticate: calling pam_end\n");
	        pam_end(pamh, PAM_ABORT);
		pamh=NULL;
	    }
	}  

	return(status);
}




/****************************************************************************
 * PamAccounting
 *
 * Work related to open and close of user sessions
 ****************************************************************************/

int
PamAccounting( char*   prog_name,
	       char*   display_name,
	       char*   entry_id,
	       char*   user, 
	       char*   line, 
	       pid_t   pid,
	       int     entry_type,
	       int     exitcode )
{
	int session_type, status;
        char *user_str = user ? user : "NULL";
        char *line_str = line ? line : "NULL";
	char *line_dev = create_devname(line_str);  
	char *tty_line;

        Debug("PamAccounting: prog_name=%s\n", prog_name);
        Debug("PamAccounting: display_name=%s\n", display_name);
        Debug("PamAccounting: entry_id=%c %c %c %c\n", entry_id[0],
				   entry_id[1], entry_id[2], entry_id[3]);
        Debug("PamAccounting: user=%s\n", user_str);
        Debug("PamAccounting: line=%s\n", line_str);
        Debug("PamAccounting: line_dev=%s\n", line_dev);
        Debug("PamAccounting: pid=%d\n", pid);
        Debug("PamAccounting: entry_type=%d\n", entry_type);
        Debug("PamAccounting: exitcode=%d\n", exitcode);

        /* Open Solaris PAM (Plugable Authentication module ) connection */

	if (entry_type == ACCOUNTING) {
	    tty_line = line;
	}
	else {
	    tty_line = line_dev;
	}

	status = PamInit(prog_name, user, tty_line, display_name);


	/* Session accounting */

	if (status == PAM_SUCCESS) switch(entry_type) {
	    case USER_PROCESS:
		/* New user session, open session accounting logs */
		status = pam_open_session(pamh, 0);
		if (status != PAM_SUCCESS) {
                    Debug("PamAccounting: USER_PROCESS open_session error=%d\n",
								 status); 
		}
#ifdef legacysun
	        if (status == PAM_SUCCESS) audit_login_success();
#endif
		session_type = SOLARIS_LOGIN;
		status = solaris_setutmp_mgmt(user, tty_line, display_name, 
					session_type, entry_type, entry_id); 
	        if (status != SOLARIS_SUCCESS) {
                    Debug("PamAccounting: USER_PRCESS set_utmp error=%d\n", 
								status);
 		}
		break;


	    case ACCOUNTING:
		/* 
		 * User session has terminated, mark it DEAD and close 
		 * the sessions accounting logs. 
		 */
		status = pam_open_session(pamh, 0);
	        if (status != PAM_SUCCESS) {
                    Debug("PamAccounting: ACCOUNTING open_session error=%d\n",
								 status); 
	        }
		entry_type = DEAD_PROCESS;
		session_type = SOLARIS_NOLOG;
		status = solaris_setutmp_mgmt(user, tty_line, display_name, 
					 session_type, entry_type, entry_id); 

	        if (status != SOLARIS_SUCCESS) {
                    Debug("PamAccounting: ACCOUNTING set_utmp error=%d\n", 
								status);
 		}
		/* Intentional fall thru */


	    case DEAD_PROCESS:
		/* Cleanup account files for dead processes */
		status = pam_close_session(pamh, 0);
	        if (status != PAM_SUCCESS) {
                    Debug("PamAccounting: DEAD_PROCES close_session error=%d\n",
								 status); 
	        }

		status = solaris_reset_utmp_mgmt(&user, &tty_line,
						 &display_name, 0,
						 entry_type, entry_id);

		if (status != SOLARIS_SUCCESS) {
                    Debug("PamAccounting: DEAD_PROCESS reset_utmp error=%d\n",
								      status);
		}
		break;


	    case LOGIN_PROCESS:
	    default:	
		status = pam_open_session(pamh, 0);
	        if (status != PAM_SUCCESS) {
                    Debug("PamAccounting:LOGIN_PROCESS open_session error=%d\n",
								 status); 
	        }
		session_type = 0;
		status = solaris_setutmp_mgmt(user, tty_line, display_name, 
					 session_type, entry_type, entry_id); 
	        if (status != SOLARIS_SUCCESS) {
                    Debug("PamAccounting: LOGIN_PROCESS set_utmp error=%d\n",
								status);
 		}
		break;
	}

	free(line_dev);
	return(status);
}



/****************************************************************************
 * PamSetCred
 *
 * Set Users login credentials: uid, gid, and group lists 
 ****************************************************************************/

int
PamSetCred(char* prog_name, char* user, uid_t uid, gid_t gid) 
{
	int cred_type, status;
        char* user_str = user ? user : "NULL";

        Debug("PamSetCred: prog_name=%s\n", prog_name);
        Debug("PamSetCred: user=%s\n", user_str);
        Debug("PamSetCred: uid=%d\n", uid);
        Debug("PamSetCred: gid=%d\n", gid);

	status = PamInit(prog_name, user, NULL, NULL);

	/* Set users credentials */

	if (status == PAM_SUCCESS && setgid(gid) == -1) 
	    status = SOLARIS_BAD_GID; 

	if (status == PAM_SUCCESS &&
	    ( !user) || (initgroups(user, gid) == -1) ) 
	     status = SOLARIS_INITGROUP_FAIL;

	if (status == PAM_SUCCESS)
	    status = pam_setcred(pamh, PAM_ESTABLISH_CRED);

	if (status == PAM_SUCCESS && (setuid(uid) == -1))
	    status = SOLARIS_BAD_UID;

	if (status != PAM_SUCCESS) {
            Debug("PamSetCred: user=%s, err=%d)\n", user, status);
	}

	return(status);
}


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


/*****************************************************************************
 * login_conv():
 *
 * This is a conv (conversation) function called from the PAM 
 * authentication scheme.  It returns the user's password when requested by
 * internal PAM authentication modules and also logs any internal PAM error
 * messages.
 *****************************************************************************/

static int
login_conv(int num_msg, struct pam_message **msg,
           struct pam_response **response, void *appdata_ptr)
{
	struct pam_message	*m;
	struct pam_response	*r;
	char 			*temp;
	int			k;

#ifdef lint
	conv_id = conv_id;
#endif
	if (num_msg <= 0)
		return (PAM_CONV_ERR);

	*response = (struct pam_response*) 
				calloc(num_msg, sizeof (struct pam_response));
	if (*response == NULL)
		return (PAM_CONV_ERR);

	(void) memset(*response, 0, sizeof (struct pam_response));

	k = num_msg;
	m = *msg;
	r = *response;
	while (k--) {

		switch (m->msg_style) {

		case PAM_PROMPT_ECHO_OFF:
                    if (saved_user_passwd != NULL) {
                        r->resp = (char *) malloc(strlen(saved_user_passwd)+1);
                        if (r->resp == NULL) {
			  /* __pam_free_resp(num_msg, *response); */
                            *response = NULL;
                            return (PAM_CONV_ERR);
                        }
                        (void) strcpy(r->resp, saved_user_passwd);
                        r->resp_retcode=0;
                    }

                    m++;
                    r++;
                    break;


		case PAM_ERROR_MSG:
			if (m->msg != NULL) { 
	    	 	    Debug ("login_conv ERROR: %s\n", m->msg); 
			}
			m++;
			r++;
			break;

		case PAM_TEXT_INFO:
			if (m->msg != NULL) { 
	    	 	    Debug ("login_conv INFO: %s\n", m->msg); 
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
	return (PAM_SUCCESS);
}
