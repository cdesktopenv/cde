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
/* $XConsortium: vgapollo.c /main/4 1995/10/27 16:17:06 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        vgapollo.c
 **
 **   Project:     HP Visual User Environment (DT)
 **
 **   Description: Dtgreet user authentication routines for Domain/OS 10.4
 **
 **                These routines validate the user; checking name, password,
 **		   home directory, password aging, etc.
 **
 **
 **   (c) Copyright 1987, 1988, 1989 by Hewlett-Packard Company
 **
 **
 **	Conditional compiles:
 **
 **	__apollo    Domain OS only
 **
 ****************************************************************************
 ************************************<+>*************************************/


#include	<stdio.h>		/* placed here so file isn't empty  */
#include	"vgmsg.h"

#ifdef __apollo

/***************************************************************************
 *
 *  Includes & Defines
 *
 ***************************************************************************/

#include	<pwd.h>

#include	<apollo/base.h>
#include	<apollo/error.h>

#include	"apollo/passwd.h"	/* copy of <apollo/sys/passwd.h>   */
#include	"apollo/login.h"	/* copy of <apollo/sys/login.h>	   */
#include	"apollo/rgy_base.h"

#include	"vg.h"

      
#define	SCMPN(a, b)	strncmp(a, b, sizeof(a))
#define	SCPYN(a, b)	strncpy(a, b, sizeof(a))
#define eq(a,b)		!strcmp(a,b)

#define NMAX	strlen(name)
#define HMAX	strlen(host)

#define STRING(str)	(str), (short) strlen(str)
#define STRNULL(s, l)	((s)[(l)] = '\0')
#define ISTRING(str)	(str), (int) strlen(str)



/***************************************************************************
 *
 *  External declarations
 *
 ***************************************************************************/


/***************************************************************************
 *
 *  Procedure declarations
 *
 ***************************************************************************/


static boolean 	CheckLogin( char *user, char *passwd, char *host, 
			    status_$t *status)
static boolean	CheckPassword( char *user, char *passwd) ;
static int	PasswordAged( register struct passwd *pw) ;




/***************************************************************************
 *
 *  Global variables
 *
 ***************************************************************************/

rgy_$policy_t       policy;
rgy_$acct_user_t user_part;
rgy_$acct_admin_t admin_part;     
extern struct passwd * getpwnam_full();



/***************************************************************************
 *
 *  Stub routines
 *
 ***************************************************************************/




/***************************************************************************
 *
 *  CheckLogin
 *
 *  check validity of user name, password and other login parameters
 *
 ***************************************************************************/

static boolean
CheckLogin( char *user, char *passwd, char *host, status_$t *status)
{
    ios_$id_t		logid;
    login_$opt_set_t	opts;
      
    login_$set_host(host, strlen(host));

    opts = login_$no_setsid_sm |
	   login_$no_setwd_sm  |
	   login_$no_prompt_pass;

    if ( !login_$chk_login(opts,
			STRING(user),
    			STRING(passwd), 
    			(login_$open_log_p) NULL,
    			STRING(""),
    			&logid,
    			status)) {

    	return(false);

    } else
    	return(true);
}     




/***************************************************************************
 *
 *  CheckPassword
 *
 *  check validity of just user name and password
 ***************************************************************************/

static boolean 
CheckPassword( char *user, char *passwd )
{
    login_$ptr	lptr;
    status_$t	status;
             
    login_$open((login_$mode_t) 0, &lptr, &status);
    if (status.all == status_$ok)
	login_$set_ppo(lptr, STRING(user), &status);
    if (status.all == status_$ok)
	login_$ckpass(lptr, STRING(passwd), &status);

    return (status.all == status_$ok);
}     




/***************************************************************************
 *
 *  PasswordAged
 *
 *  see if password has aged
 ***************************************************************************/

static int 
PasswordAged( register struct passwd *pw )
{

    boolean lrgy;

    /* Account validity checks:  If we were able to connect to the network
     * registry, then we've acquired account and policy data and can perform
     * account/password checking
     */

    lrgy = rgy_$using_local_registry();
    if ( !lrgy ) { 

	/* Check for password expiration or invalidity */
	if (rgy_$is_passwd_expired(&user_part, &policy ) == true  ||
	    rgy_$is_passwd_invalid(&user_part) == true)  {

	    return TRUE;
	}
    }
   return FALSE;
}


    

/***************************************************************************
 *
 *  Verify
 *
 *  verify the user
 *
 *  return codes indicate authentication results.
 ***************************************************************************/

#define MAXATTEMPTS	5

extern Widget focusWidget;		/* login or password text field	   */
struct  passwd nouser = {"", "nope"};	/* invalid user password struct	   */

int 
Verify( char *name, char *passwd )
{

    static int		login_attempts = 0; /* # failed authentications	   */
    
    struct passwd	*p;	/* password structure			   */
    char 		*host;	/* host that login is coming in from	   */
    status_$t		status;	/* status code returned by CheckLogin	   */

    int			n;

    host = dpyinfo.name;
    

    /*
     *  look up entry from registry...
     *
     *  need getpwnam_full to get policy data for passwd expiration 
     *  or invalidity...
     */
    p = getpwnam_full(name, &user_part, &admin_part, &policy);
/*    p = getpwnam(name);*/
    
    if (!p || strlen(name) == 0 || p->pw_name == NULL )
	p = &nouser;


    /*
     *  validate user/password...
     */

    if (!CheckLogin(name, passwd, host, &status)) {

	/*
	 *  if verification failed, but was just a name check, prompt for
	 *  password...
	 */

	if ( focusWidget != passwd_text ) 
	    return (VF_INVALID);


	/*
	 *  if maximum number of attempts exceeded, log failure...
	 */

	if ((++login_attempts % MAXATTEMPTS) == 0 ) {

#ifdef peter
		syslog(LOG_CRIT,
		    "REPEATED LOGIN FAILURES ON %s FROM %.*s, %.*s",
		    "??", HMAX, host, NMAX, name);
#endif
	}
	

	/*
	 *  check status codes from verification...
 	 */
     
	switch (status.all) {

	case login_$logins_disabled:	/* logins are disabled		   */
	    if (p->pw_uid != 0)
		return(VF_NO_LOGIN);
	    else
		if (!CheckPassword(name,passwd))
		    return(VF_INVALID);
		    
	    break;

	case login_$inv_acct:		/* invalid account		   */
	    if ( PasswordAged(p) ) 
		return(VF_PASSWD_AGED);
	    else
		return(VF_INVALID);
	    break;

	default:			/* other failed verification	   */
	    return(VF_INVALID);
	    break;

	}
    }

         

    /*
     *  verify home directory exists...
     */

    if (chdir(p->pw_dir) < 0) {
	if (chdir("/") < 0)
	    return(VF_HOME);
	else 
	    LogError(ReadCatalog(
		MC_LOG_SET,MC_LOG_NO_HMDIR,MC_DEF_LOG_NO_HMDIR),
		p->pw_dir, name);
    }


    /*
     *  validate uid and gid...
     */

    if ((p->pw_gid < 0)      || 
	(setgid(p->pw_gid) == -1)) {
	return(VF_BAD_GID);
    }

    if ((p->pw_uid < 0)      || 
	(seteuid(p->pw_uid) == -1)) {
	return(VF_BAD_UID);
    }


    /*
     * verify ok...
     */

    return(VF_OK);
}

#endif
