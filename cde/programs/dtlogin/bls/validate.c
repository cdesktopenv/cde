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
/* $XConsortium: validate.c /main/4 1995/10/27 16:19:47 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        validate.c
 **
 **   Project:     HP Visual User Environment (DT)
 **
 **   Description: Dtgreet BLS user authentication routines
 **
 **                These routines validate the user; checking name, password,
 **		   number of users on the system, password aging, etc.
 **
 **
 **   (c) Copyright 1987, 1988, 1989 by Hewlett-Packard Company
 **
 **
 **	Conditional compiles:
 **
 **     OSMAJORVERSION < 8
 **     	    HP-UX 7.0/7.03 restricted license counting algorithms
 **		    are used. Otherwise HP-UX 8.0 and beyond is used
 **
 **     BLS	    HP BLS B1 simple authentication.
 **
 **	__hpux	    HP-UX OS only
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifdef	BLS

/***************************************************************************
 *
 *  Includes & Defines
 *
 ***************************************************************************/

#include	<stdio.h>
#include	<fcntl.h>
#include	<stdlib.h>
#include	<pwd.h>

#include	"../vg.h"



/***************************************************************************
 *
 *  HP-UX BLS authentication routines
 *
 ***************************************************************************/

#include 	<sys/param.h>     /* for MAXUID macro */
#include	<sys/types.h>
#include	<sys/utsname.h>
#include	<string.h>
#include	<utmp.h>
#include	<time.h>
#include	<grp.h>


/*  BLS only headers */
#    include <sys/security.h>
#    include <prot.h>
#    include "bls.h"


#define how_to_count ut_exit.e_exit

#ifdef __hp9000s300
static    int num_users[] = { 2, 32767 };
#   define MIN_VERSION     'A'
#   define UNLIMITED       'B'
#else
static    int num_users[] = { 2, 16, 32, 64 , 8 };
#   define MIN_VERSION 	'A'
#   define UNLIMITED	'U'
#endif

/* Maximum number of users allowed with restricted license */
#if OSMAJORVERSION < 8
#    define MAX_STRICT_USERS 2	
#else
#    define MAX_STRICT_USERS 8
#endif

#define NUM_VERSIONS 	(sizeof(num_users)/sizeof(num_users[0])) - 1



/***************************************************************************
 *
 *  External declarations
 *
 ***************************************************************************/

extern Widget focusWidget;		/* login or password text field	   */

extern long	groups[NGROUPS];


/***************************************************************************
 *
 *  Procedure declarations
 *
 ***************************************************************************/

static int	CheckPassword( char *name, char	*passwd );
static int	CountUsers( int added_users) ;
static int	CountUsersStrict( char *new_user) ;
static void	WriteBtmp( char *name) ;




/***************************************************************************
 *
 *  Global variables
 *
 ***************************************************************************/

/* BLS only data */
   struct pr_passwd	*b1_pwd;
   struct verify_info	verify_data;
   struct verify_info	*verify = &verify_data;
   struct greet_info	greet_data;
   struct greet_info	*greet = &greet_data;
   static int		UserHasPassword = 1;

 
/***************************************************************************
 *
 *  CountUsers
 *
 *  see if new user has exceeded the maximum.
 ***************************************************************************/

#define NCOUNT 16

static int 
CountUsers( int added_users )
{
    int count[NCOUNT], nusers, i;
    struct utmp *entry;

    for (i=0; i<NCOUNT; i++)
	count[i] = 0;

    count[added_users]++;

    while ( (entry = getutent()) != NULL) {
	if (entry->ut_type == USER_PROCESS) {
	    i = entry->how_to_count;
	    if (i < 0 || i >= NCOUNT)
		i = 1;          /* if out of range, then count */
				/* as ordinary user */
	    count[i]++;
	}
    }
    endutent();

    /*
     * KEY:
     *  [0]     does not count at all
     *  [1]     counts as real user
     *  [2]     logins via a pty which have not gone trough login.  These 
     *		collectively count as 1 user IF count[3] is 0, otherwise,
     *		they are not counted. Starting with HP-UX 8.0 they are
     *		no longer counted at all.
     *  [3]     logins via a pty which have been logged through login (i.e.
     *		rlogin and telnet).  these count as 1 "real" user per
     *		unique user name.
     *  [4-15]  may be used for groups of users which collectively
     *          count as 1
     */
    nusers = count[1];

#if OSMAJORVERSION < 8
    for (i=2; i<NCOUNT; i++)
#else
    for (i=3; i<NCOUNT; i++)
#endif
	if (count[i] > 0)
	    nusers++;

    return(nusers);
}




/***************************************************************************
 *
 *  CountUsersStrict
 *
 *  see if new user has exceeded the maximum.
 ***************************************************************************/

static int 
CountUsersStrict( char *new_user )
{
    char pty_users[MAX_STRICT_USERS][8];
    int count[NCOUNT], nusers, i, cnt, pty_off = -1, uname_off;
    struct utmp *entry;

    /*
     *  Initialize count array...
     */
    for (i = 0; i < NCOUNT; i++)
	count[i] = 0;

    /*
     *  Add in the new user (we know it's not a pty)...
     */
    count[1]++;

    while ( (entry = getutent()) != NULL ) {
	if (entry->ut_type == USER_PROCESS) {
	    i = entry->how_to_count;

	    /* if out of range, then count as ordinary user logged in 
	       via a tty */
	    if (i == 1 || (i < 0 || i >= NCOUNT))
		count[1]++;
	    /* See if it is a pty login granted by login program */
	    else if (i == 3) {
		count[3]++;
	        /* See if user is already logged in via login pty */
		uname_off = -1;
		for (cnt = 0; cnt <= pty_off; cnt++)
			if (strncmp(pty_users[cnt], entry->ut_user, 8) == 0)
				uname_off = cnt;

		if (uname_off == -1) { /* user is not logged in via pty yet */
			
		    if (pty_off >= MAX_STRICT_USERS)  /* cannot add any
		    					 more users */
		    	return(MAX_STRICT_USERS + 1);
		    /* add the user name to the array of pty users */
		    else
			strncpy(pty_users[++pty_off], entry->ut_user, 8);
		}
	    } /* end if (i == 3) */
	    else
		count[i]++;
	} /* end if entry->ut_type == USER_PROCESS */
    } /* end while (entry = getutent()) */

    endutent();
    /*
     * KEY:
     *  [0]	does not count at all
     *  [1]	counts as "real" user
     *  [2]     logins via a pty which have not gone trough login.  These 
     *		collectively count as 1 user IF count[3] is 0, otherwise,
     *		they are not counted. Starting with HP-UX 8.0 they are
     *		no longer counted at all.
     *  [3]     logins via a pty which have been logged through login (i.e.
     *		rlogin and telnet).  these count as 1 "real" user per
     *		unique user name.
     *  [4-15]  may be used for groups of users which collectively count 
     *          as 1
     */

     nusers = pty_off + 1 + count[1];  /* Current number of users is sum of
					  users logged in via tty + the
					  number of unique users logged in 
					  via pty which have gone through
					  login */

#if OSMAJORVERSION < 8
    if ((count[3] == 0) && (count[2] != 0))
	nusers++;			/* Add 1 user for all pty logins IF
					   none of pty logins have been 
					   granted by the login program */
#else
    /*
     * Don't count any hpterm logins (exit status of 2).  We already
     * counted all pty logins granted by the login program.
     */
#endif

    for (i = 4; i < NCOUNT; i++)
        if (count[i] > 0)
	    nusers++;
    return(nusers);
}



/***************************************************************************
 *
 *  CheckPassword
 *
 *  Check validity of user password. 
 *
 ***************************************************************************/

static int
CheckPassword( char *name, char	*passwd )
{

    char		*crypt();
    struct passwd	*p;
    char		*reason;

    /*
     *  HP BLS B1 password authentication...
     */

    if ( ISSECURE ) {
	b1_pwd = getprpwnam(name);
    
	if ( b1_pwd == NULL || strlen(name) == 0 ) {
	    Debug("unknown user '%s'\n", name);
	    audit_login((struct pr_passwd *)0, (struct passwd *)0,
		dpyinfo.name, "No entry in protected password db",
		ES_LOGIN_FAILED);
	    return(FALSE);
	}
    
    /*
     *  look up user's regular account information...
     */

    p = getpwnam(name);
	
    if ( p == NULL || strlen(name) == 0 ) {
	Debug("unknown user '%s'\n", name);
	audit_login((struct pr_passwd *)0, (struct passwd *)0,
	    dpyinfo.name, "No entry in password file",
	    ES_LOGIN_FAILED);
	return(FALSE);
    }
		
    /*  verify_info has become a catchall for info needed later */
	verify->user_name = name;
	verify->prpwd = b1_pwd;
	verify->pwd = p;
	strncpy(verify->terminal, dpyinfo.name, 15);
	verify->terminal[15]='\0';

    }     

    Debug("Verify %s \n",name);

    /* if the password doesn't exists, we can't check it, but
     * the user will be forced to change it later */
    if ( (UserHasPassword = password_exists(verify)) != 0 )
        if ( strcmp(bigcrypt(passwd,b1_pwd->ufld.fd_encrypt),
		    b1_pwd->ufld.fd_encrypt) ) {
	    Debug("verify failed\n");
            audit_login( b1_pwd, p ,dpyinfo.name,
			"Password incorrect",
			ES_LOGIN_FAILED);
	    return(FALSE);
        } else {
	    Debug ("username/password verify succeeded\n");
	    return(TRUE);
	}    
    /*
     *  all password checks failed...
     */
     
    return (FALSE);

}




/***************************************************************************
 *
 *  BLS_Verify
 *
 *  verify the user
 *
 *  return codes indicate authentication results.
 ***************************************************************************/

#define MAXATTEMPTS	3

static struct  passwd nouser = {"", "nope"};	/* invalid user password struct	   */

int 
BLS_Verify( char *name, char *passwd )
{

    static int		login_attempts = 0; /* # failed authentications	   */
    
    struct passwd	*p;		/* password structure */
    struct pr_passwd *prpwd;

    struct utsname	utsnam;
    int			n;
    int			uid;

    /*
     * Desparate maneuvre to give dtgreet the privledges it needs
     */
     if ( login_attempts == 0 ) {
	Debug("Setting luid for dtgreet\n");
	if ( getluid() == -1 )
		setluid(getuid());
     }
    
    /*
     *  validate password...
     */
    
    if ( CheckPassword(name, passwd) == FALSE) {
        p = verify->pwd;
	if ( focusWidget == passwd_text ) {
	
	    WriteBtmp(name);

	    if ((++login_attempts % MAXATTEMPTS) == 0 ) {

		if (p->pw_name == NULL )
		    p = &nouser;

		audit_login( b1_pwd, p ,dpyinfo.name,
			"Failed login(bailout)",
			ES_LOGIN_FAILED);
	    }
	    
	} else if ( !UserHasPassword ) {
		/*
		 * The user has not password -- this must be the initial login for this
		 * user.  Treat it like an expired password.  This should invoke the
		 * password program on behalf of the user.
		 */
		UserHasPassword = 1;
		return  VF_PASSWD_AGED;
	}
	
	return(VF_INVALID);
    }
    prpwd = verify->prpwd;
    p = verify->pwd;
    
    /* check that the uid of both passwd and pr_passwd struct's agree */
    uid = p->pw_uid;
    if (uid != prpwd->ufld.fd_uid) {
	audit_login(prpwd, p, verify->terminal, 
	    "User id's inconsistent across password database\n",
	    ES_LOGIN_FAILED);
	Debug("login failed - uid's do not match\n");
	return VF_BAD_UID;
    }
    verify->uid = uid;

    /* check if user's account is locked 
     * This can be by dead password (lifetime exceeded),
     * fd_lock is set, or fd_max_tries is exceeded.
     * locked_out is from libsec, but is poorly documented.
     */
    if (locked_out(prpwd)) {
        Debug("Account locked\n");
        audit_login(prpwd, p, verify->terminal,
	    "Account locked", ES_LOGIN_FAILED);
	return VF_INVALID;
    }
    /* can user log in at this time?
     * time_lock is in libsec, but poorly documented
     */
    if (time_lock(prpwd)) {
        Debug("Account time-locked\n");
        audit_login(prpwd, p, verify->terminal,
            "Account time-locked", ES_LOGIN_FAILED);
        return VF_INVALID;
    }
    
    /****************************************************
      xdm checks the security level here using
      verify_sec_user
      We do it later from the dtgreet callback rountine
      VerifySensitivityLevel()
    ****************************************************/

#if 0 
    /*
     *  check restricted license...
     *
     *  Note: This only applies to local displays. Foreign displays
     *	      (i.e. X-terminals) apparently do not count.
     */

    /* Get the version info via uname.  If it doesn't look right,
     * assume the smallest user configuration
     */

    if (getenv(LOCATION) != NULL) {
	if (uname(&utsnam) < 0)
	    utsnam.version[0] = MIN_VERSION;

	/*
	 * Mappings:
	 *    834 -> 834
	 *    844 -> 844
	 *    836 -> 635
	 *    846 -> 645
	 *    843 -> 642
	 *    853 -> 652
	 */

	if ((!strncmp(utsnam.machine, "9000/834", UTSLEN)) ||
	    (!strncmp(utsnam.machine, "9000/844", UTSLEN)) ||
	    (!strncmp(utsnam.machine, "9000/836", UTSLEN)) ||
	    (!strncmp(utsnam.machine, "9000/846", UTSLEN)) ||
	    (!strncmp(utsnam.machine, "9000/843", UTSLEN)) ||
	    (!strncmp(utsnam.machine, "9000/853", UTSLEN))) {

/*	    strict_count = 1;*/
	    if (CountUsersStrict(name) > MAX_STRICT_USERS) {
		audit_login( b1_pwd, p ,dpyinfo.name,
			"Attempted to login - too many users on the system",
			ES_LOGIN_FAILED);
		return(VF_MAX_USERS);
	    }
	}
	else {
	    if (utsnam.version[0] != UNLIMITED) {
		if ((utsnam.version[0]-'A' < 0) ||
	     	    (utsnam.version[0]-'A' > NUM_VERSIONS))
		    utsnam.version[0] = MIN_VERSION;

		n = (int) utsnam.version[0] - 'A';
		if (CountUsers(1) > num_users[n]) {
			audit_login( b1_pwd, p ,dpyinfo.name,
				"Attempted to login - too many users on the system",
				ES_LOGIN_FAILED);
		    return(VF_MAX_USERS);
		}
	    }
	}
    }

#endif	/* 0 */

    /*
     *  check password aging...
     */

    if ( passwordExpired(verify)) {
	audit_login( b1_pwd, p ,dpyinfo.name,
		"Password expired",
		ES_LOGIN_FAILED);
	return(VF_PASSWD_AGED);
    }
         

    /*
     *  verify home directory exists...
     */

    if(chdir(p->pw_dir) < 0) {
	Debug("Attempted to login -- no home directory\n");
	audit_login( b1_pwd, p ,dpyinfo.name,
		" Attempted to login - no home directory",
		ES_LOGIN_FAILED);
        return(VF_HOME);
    }

    /*
     *  validate uid and gid...
     */
#ifdef NGROUPS
    getGroups(greet->name, verify, p->pw_gid);
#else
    verify->gid = pwd->pw_gid;
    
    if ((p->pw_gid < 0)      || 
        (p->pw_gid > MAXUID) ||
	(setgid(p->pw_gid) == -1)) {

	Debug("Attempted to login -- bad group id");
	audit_login( b1_pwd, p ,dpyinfo.name,
		"Attempted to login - bad group id",
		ES_LOGIN_FAILED);
	return(VF_BAD_GID);
    }
#endif	/* NGROUPS */

    if ((p->pw_uid < 0)      || 
        (p->pw_uid > MAXUID) ||
	(setresuid(p->pw_uid, p->pw_uid, 0) == -1)) {

	Debug("Attempted to login -- bad user id\n");
	audit_login( b1_pwd, p ,dpyinfo.name,
		"Attempted to login - bad user id",
		ES_LOGIN_FAILED);
	return(VF_BAD_UID);
    }


    /*
     * verify ok...
     */

    Debug ("Successful login\n");
    audit_login( b1_pwd, p ,dpyinfo.name,
	"Successful login",
	ES_LOGIN_REMOTE);
    return(VF_OK);
}




/***************************************************************************
 *
 *  WriteBtmp
 *
 *  log bad login attempts
 *  
 ***************************************************************************/

static void 
WriteBtmp( char *name )
{
    int fd;
    struct utmp utmp, *u;

    Boolean found=FALSE;

    bzero(&utmp, sizeof(struct utmp));

    utmp.ut_pid = getppid();
    while ((u = getutent()) != NULL) {
    	if ( (u->ut_type == INIT_PROCESS  ||
    	      u->ut_type == LOGIN_PROCESS ||
    	      u->ut_type == USER_PROCESS) && 
    	      u->ut_pid == utmp.ut_pid ) {

		found = TRUE;
    		break;
    	}
    }


    /*
     *  if no utmp entry, this may be an X-terminal. Construct a utmp
     *  entry for it...
     */

    if ( ! found ) {
	strncpy(utmp.ut_id,   "??", sizeof(utmp.ut_id));
	strncpy(utmp.ut_line, dpyinfo.name, sizeof(utmp.ut_line));
	utmp.ut_type = LOGIN_PROCESS;
	strncpy(utmp.ut_host, dpyinfo.name, sizeof(utmp.ut_host));
	u = &utmp;
    }
    
    
    /*
     * If btmp exists, then record the bad attempt
     */
    if ( (fd = open(BTMP_FILE,O_WRONLY|O_APPEND)) >= 0) {
	strncpy(u->ut_user, name, sizeof(u->ut_user));
	(void) time(&u->ut_time);
	write(fd, (char *)u, sizeof(utmp));
	(void) close(fd);
    }

    endutent();		/* Close utmp file */
}


/***************************************************************************
 *
 *  VerifySensitivityLevel
 *
 *  verify B1 Sensitivity Level
 **************************************************************************/
extern   char	*sensitivityLevel;

int
VerifySensitivityLevel( void)
{

    int i;

    greet->b1security = sensitivityLevel =
	(char *) XmTextFieldGetString(passwd_text);

    /* new functions: (side effects: auditing, change verify) */
    if (verify_user_seclevel(verify, sensitivityLevel)
	    && verify_sec_xterm(verify, sensitivityLevel)) {

	    Debug("verify_user_seclevel succeeded.\n");
	    return VF_OK;
    }

    Debug("verify_user_seclevel failed\n");
    return (VF_BAD_SEN_LEVEL);
}


#ifdef NGROUPS
groupMember ( char *name, char **members )
{
	while (*members) {
		if (!strcmp (name, *members))
			return 1;
		++members;
	}
	return 0;
}

getGroups ( char *name, struct verify_info *verify, int gid)
{
	int		ngroups;
	struct group	*g;
	int		i;

	ngroups = 0;
	verify->groups[ngroups++] = gid;
	setgrent ();
	while (g = getgrent()) {
		/*
		 * make the list unique
		 */
		for (i = 0; i < ngroups; i++)
			if (verify->groups[i] == g->gr_gid)
				break;
		if (i != ngroups)
			continue;
		if (groupMember (name, g->gr_mem)) {
			if (ngroups >= NGROUPS)
				LogError ("%s belongs to more than %d groups, %s ignored\n",
					name, NGROUPS, g->gr_name);
			else
				verify->groups[ngroups++] = g->gr_gid;
		}
	}
	verify->ngroups = ngroups;
	endgrent ();
}
#endif

/* check whether the password has expired or not.
 * return 1 means that the password has expired. 
 */
int 
passwordExpired( struct verify_info *verify)
{
	struct pr_passwd *pr;
	time_t expiration;
	time_t last_change;
	time_t expiration_time;
	time_t now;
	int passwd_status;
	struct pr_passwd save_data;
	struct pr_default *df;
	char *ttime;
	char ptime[64];

	pr = verify->prpwd;

	/* 
	 * If null password, do not check expiration.
	 */

	if (!pr->uflg.fg_encrypt || (pr->ufld.fd_encrypt[0] == '\0'))
		return 0;

	now = time((long *) 0);

	if (pr->uflg.fg_schange)
		last_change = pr->ufld.fd_schange;
	else
		last_change = (time_t) 0;

	if (pr->uflg.fg_expire)
		expiration = pr->ufld.fd_expire;
	else if (pr->sflg.fg_expire)
		expiration = pr->sfld.fd_expire;
	else
		expiration = (time_t) 0;

	df = getprdfnam(AUTH_DEFAULT);

	/*
	 * A 0 or missing expiration field means there is no
	 * expiration.
	 */
	expiration_time = expiration ? last_change + expiration : 0;
	
	if (expiration_time && now > expiration_time ) {
		/*
		 * The password has expired
		 */
		Debug("The password is expired\n");
		return 1;
	}

	Debug("The password is not expired\n");
	return 0;
}


/***************************************************************************
 *
 *  end HP-UX authentication routines
 *
 ***************************************************************************/
#endif	/* BLS */
