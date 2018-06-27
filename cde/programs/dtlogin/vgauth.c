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
/* $XConsortium: vgauth.c /main/4 1996/10/04 16:56:33 drk $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        vgauth.h
 **
 **   Project:     HP Visual User Environment (DT)
 **
 **   Description: Dtgreet user authentication routines
 **
 **                These routines validate the user; checking name, password,
 **		   number of users on the system, password aging, etc.
 **
 **
 **   (c) Copyright 1987, 1988, 1989 by Hewlett-Packard Company
 **
 **
 **	Conditional compiles for HPUX:
 **
 **     OSMAJORVERSION < 8
 **     	    HP-UX 7.0/7.03 restricted license counting algorithms
 **		    are used. Otherwise HP-UX 8.0 and beyond is used
 **
 **	AUDIT       HP C2 security enhancements; checks for existence of
 **                 SECUREPASSWD file and authenticates user against
 **                 password contained in that file. Also performs
 **                 self-auditing of login actions.  Incompatible with 
 **                 #ifdef SecureWare
 **
 **     BLS	    HP BLS B1 simple authentication.
 **
 **     __AFS        AFS 3 authentication mechanism
 **     __KERBEROS   Kerberos authentication mechanism
 **     __PASSWD_ETC Domain/OS Registry from HP-UX authentication mechanism
 **
 **     Platform identification:
 **
 **	__hpux	    HP-UX OS only
 **     sun         SUN OS only
 **     SVR4        SUN OS et al.
 **     _AIX        AIX only
 **
 ****************************************************************************
 ************************************<+>*************************************/


/***************************************************************************
 *
 *  Includes & Defines
 *
 ***************************************************************************/

#include	<stdio.h>
#include	<fcntl.h>
#include	<stdlib.h>
#include	<pwd.h>

/* necessary for bzero */
#ifdef SVR4
#include        <X11/Xfuncs.h>
#ifdef  sun
#include        <shadow.h>
#endif
#endif

#include	"vg.h"
#include	"vgmsg.h"

/*
 * Define as generic those without platform specific code.
 */
#if !(defined(__hpux) || defined(_AIX) || defined(sun))
#define generic
#endif

#ifdef __hpux
/***************************************************************************
 *
 *  Start authentication routines (HPUX)
 *
 ***************************************************************************/

#include 	<sys/param.h>     /* for MAXUID macro */
#include	<sys/types.h>
#include	<sys/utsname.h>
#include	<string.h>
#include	<utmp.h>
#include	<time.h>


#ifdef AUDIT
#    include <sys/audit.h>
#    include <sys/errno.h>
#    include <sys/stat.h>
#    define SECUREPASS	"/.secure/etc/passwd"
#endif

#ifdef BLS
#    include <sys/security.h>
#    include <prot.h>
#endif

#ifdef __AFS
#include	<afs/kautils.h>
#endif /* __AFS */

#ifdef __KERBEROS
#    include <krb.h>
#    define KRBLIFE	255		/* max lifetime */
#endif /* __KERBEROS */

#ifdef __PASSWD_ETC
#    include	"rgy_base.h"
#endif



#define how_to_count ut_exit.e_exit

#ifdef __hp9000s300
    int num_users[] = { 2, 32767 };
#   define MIN_VERSION     'A'
#   define UNLIMITED       'B'
#else
    int num_users[] = { 2, 16, 32, 64 , 8 };
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
 *  External declarations (HPUX)
 *
 ***************************************************************************/

extern Widget focusWidget;		/* login or password text field	   */

extern long	groups[NGROUPS];


#ifdef __PASSWD_ETC
extern boolean rgy_$using_local_registry();
extern struct passwd * getpwnam_full();
extern boolean is_acct_expired();
extern boolean is_passwd_expired();
extern boolean is_passwd_invalid();
extern boolean rgy_$is_des();
#endif



/***************************************************************************
 *
 *  Procedure declarations (HPUX)
 *
 ***************************************************************************/

static void	Audit( struct passwd *p, char *msg, int errnum) ;
static int	CheckPassword( char *name, char	*passwd, struct passwd **ppwd );
static int	CountUsers( int added_users) ;
static int	CountUsersStrict( char *new_user) ;
static int	PasswordAged( struct passwd *pw) ;
static void	WriteBtmp( char *name) ;




/***************************************************************************
 *
 *  Global variables (HPUX)
 *
 ***************************************************************************/

#ifdef AUDIT
  struct s_passwd *s_pwd;
  struct stat	s_pfile;
  int		secure;	/* flag to denote existence of secure passwd file */
  Boolean	selfAuditOn;
#endif

#ifdef BLS
   struct pr_passwd	*b1_pwd;
#endif

#ifdef __PASSWD_ETC
rgy_$acct_admin_t admin_part;
rgy_$policy_t     policy;
rgy_$acct_user_t  user_part;
#endif



/***************************************************************************
 *
 *  Audit (HPUX)
 *
 *  Construct self audit record for event and write to the audit trail. 
 *  This routine assumes that the effective uid is currently 0. If auditing
 *  is not defined, this routine does nothing.
 ***************************************************************************/

static void 
Audit( struct passwd *p, char *msg, int errnum )
{


#ifdef AUDIT
    char *txtptr;
    struct self_audit_rec audrec;
    int status;
#endif

    /*
     * make sure program is back to super-user...
     */

    setresuid(0,0,0);


#ifdef AUDIT
    if (selfAuditOn) {
	txtptr = (char *)audrec.aud_body.text;
	sprintf(txtptr, "User= %s uid=%ld audid=%ld%s", p->pw_name,
		(long)p->pw_uid, (long)p->pw_audid, msg);
	audrec.aud_head.ah_pid = getpid();
	audrec.aud_head.ah_error = errnum;
	audrec.aud_head.ah_event = EN_LOGINS;
	audrec.aud_head.ah_len = strlen (txtptr);
	status = audwrite(&audrec);

	if ( status != 0 ) {
	    switch (errno) {
	    
	    case EPERM:
		LogError(ReadCatalog(
			MC_LOG_SET,MC_LOG_NOT_SUSER,MC_DEF_LOG_NOT_SUSER));
		break;
	    
	    case EINVAL:
		LogError(ReadCatalog(
			MC_LOG_SET,MC_LOG_INV_EVENT,MC_DEF_LOG_INV_EVENT));
		break;
	    
	    default:
		LogError(ReadCatalog(
			MC_LOG_SET,MC_LOG_ERR_ERRNO,MC_DEF_LOG_ERR_ERRNO),
			errno);
		break;
	    }
	}
    }

#else
    return;
#endif
}





 
/***************************************************************************
 *
 *  CountUsers (HPUX only)
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
 *  CountUsersStrict (HPUX only)
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
 *  PasswordAged (HPUX)
 *
 *  see if password has aged
 ***************************************************************************/
#define SECONDS_IN_WEEK		604800L

static int 
PasswordAged( struct passwd *pw )
{
    long change_week;	/* week password was changed (1/1/70 = Week 0) */
    long last_week;	/* week after which password must change */
    long first_week;	/* week before which password can't change */
    long this_week;	/* this week derived from time() */

    char *passwdAge;	/* password aging time */

#ifdef __PASSWD_ETC
    boolean lrgy;
#endif
    

    passwdAge = pw->pw_age;
    
#ifdef AUDIT
    if (secure)
	passwdAge = s_pwd->pw_age;
#endif


#ifdef __PASSWD_ETC
    /* Account validity checks:  If we were able to connect to the network
     * registry, then we've acquired account and policy data and can perform
     * account/password checking
     */

    lrgy = rgy_$using_local_registry();
    if ( !lrgy ) { 

	/* Check for password expiration or invalidity */
	if ( rgy_$is_passwd_expired(&user_part, &policy ) != 0 ) {
	    return TRUE;
	}
    }
   return FALSE;

#else

    if (*passwdAge == NULL)
	return(0);

    first_week = last_week = change_week = (long) a64l(passwdAge);
    last_week &= 0x3f;				/* first six bits */
    first_week = (first_week >> 6) & 0x3f;	/* next six bits */
    change_week >>= 12;				/* everything else */

    this_week = (long) time((long *) 0) / SECONDS_IN_WEEK;

/*
**	Password aging conditions:
**	*   if the last week is less than the first week (e.g., the aging
**	    field looks like "./"), only the superuser can change the
**	    password.  We don't request a new password.
**	*   if the week the password was last changed is after this week,
**	    we have a problem, and request a new password.
**	*   if this week is after the specified aging time, we request
**	    a new password.
*/
    if (last_week < first_week)
	return(0);

    if (change_week <= this_week && this_week <= (change_week + last_week))
	return(0);

    return(1);

#endif
}



/***************************************************************************
 *
 *  CheckPassword (HPUX only)
 *
 *  Check validity of user password. One of several authentication schemes
 *  can be used, including Kerberos, AFS 3, HP BLS and traditional 
 *  /etc/passwd. These are selectable via a resource in Dtlogin.
 *  
 *  Domain registry authentication (PasswdEtc) can also be compiled in as
 *  the only authentication scheme used.
 *
 ***************************************************************************/

static int
CheckPassword( char *name, char	*passwd, struct passwd **ppwd )
{

    char		*crypt();
    struct passwd	*p;

    char		*reason;
    
#ifdef __KERBEROS
    char		realm[REALM_SZ];
    int			kerno;
#endif /* __KERBEROS */


#ifdef AUDIT
    /*
     *  validate that user has an entry in the shadow password file on an 
     *  HP-UX C2 trusted system. Keep info in a global structure.
     */
    if (secure) {
	setspwent();
	s_pwd = getspwnam(name);
	endspwent();

	if (s_pwd == NULL)
	    return(FALSE);	
    }
#endif


    /*
     *  look up user's regular account information...
     */

#ifdef __PASSWD_ETC
    /*
     *  look up entry from registry...
     *
     *  need getpwnam_full to get policy data for passwd expiration 
     *  or invalidity...
     */
    p = getpwnam_full(name, &user_part, &admin_part, &policy);
#else
    p = getpwnam(name);
#endif

    endpwent();
    *ppwd = p;
    
    if ( p == NULL || strlen(name) == 0 ) 
	return(FALSE);


#ifdef __AFS

    /*
     *  AFS password authentication...
     */
     
    if ( IsVerifyName(VN_AFS) ) {

	if (focusWidget == login_text)
	    return(FALSE);

	if ( ka_UserAuthenticateGeneral(
		KA_USERAUTH_VERSION + KA_USERAUTH_DOSETPAG, 
		p->pw_name,		/* kerberos name */
		(char *)0,		/* instance */
		(char *)0,		/* realm */
		passwd,			/* password */
		0,			/* default lifetime */
		0,			/* spare 1 */
		0,			/* spare 2 */
		&reason) == 0 ) {	/* error string */

	    if (strcmp(p->pw_passwd, "*") == 0)
		return(FALSE);
	    else
		return(TRUE);
	}

	else {
	    LogError(ReadCatalog(
		MC_LOG_SET,MC_LOG_AFS_FAILATH,MC_DEF_LOG_AFS_FAILATH),reason);
	}
    }    	
#endif /* __AFS */


#ifdef __KERBEROS

    /*
     *  Kerberos password authentication...
     */
     
    if ( IsVerifyName(VN_KRB) ) {

	if (focusWidget == login_text)
	    return(FALSE);
	
	(void)krb_get_lrealm(realm, 1);

	setresuid(p->pw_uid, p->pw_uid, -1);
	kerno = krb_get_pw_in_tkt(p->pw_name, 
				  "", 
				  realm, 
				  "krbtgt", 
				  realm, 
				  KRBLIFE, 
				  passwd);

	setresuid(0, 0, -1);

	if (kerno == KSUCCESS) 
	    if (strcmp(p->pw_passwd, "*") == 0)
		return(FALSE);
	    else
		return(TRUE);

    }
#endif /* __KERBEROS */



    /*
     *  traditional password verification...
     */
     
    if (strcmp (crypt (passwd, p->pw_passwd), p->pw_passwd) == 0)
	return(TRUE);
	

#ifdef __PASSWD_ETC
    /*
     *   If regular passwd check fails, try old-style SR
     */

    if (rgy_$is_des(passwd, strlen(passwd), p->pw_passwd) == TRUE)
	return (TRUE);
#endif


    /*
     *  all password checks failed...
     */
     
    return (FALSE);

}




/***************************************************************************
 *
 *  Verify (HPUX)
 *
 *  verify the user
 *
 *  return codes indicate authentication results.
 ***************************************************************************/

#define MAXATTEMPTS	3

struct  passwd nouser = {"", "nope"};	/* invalid user password struct	   */

int 
Verify( char *name, char *passwd )
{

    static int		login_attempts = 0; /* # failed authentications	   */
    
    struct passwd	*p;		/* password structure */

    struct utsname	utsnam;
    int			n;

    

#ifdef AUDIT
    /* 
     * turn on self auditing...
     */

    if (audswitch(AUD_SUSPEND) == -1)
    	selfAuditOn = FALSE;
    else
    	selfAuditOn = TRUE;

    /*
     *  set the secure flag if SECUREPASS exists. If so, we
     *  are using it for authentication instead of /etc/passwd...
     */

    secure = (stat(SECUREPASS, &s_pfile) < 0) ? 0:1;


    /*
     * set the audit process flag unconditionally on since we want 
     * to log all logins regardless of whether the user's audit 
     * flag is set...
     */

    if (secure)
    	setaudproc(AUD_PROC);

#endif /* AUDIT */


    /*
     *  validate password...
     */
     
    if ( CheckPassword(name, passwd, &p) == FALSE) {
	if ( focusWidget == passwd_text ) {
	
	    WriteBtmp(name);

	    if ((++login_attempts % MAXATTEMPTS) == 0 ) {

		if (p->pw_name == NULL )
		    p = &nouser;

		Audit(p, " Failed login (bailout)", 1);

	    }
	}
	
	return(VF_INVALID);
    }


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
		Audit(p,
		      " attempted to login - too many users on the system",
		      20);

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
		    Audit(p,
		          " attempted to login - too many users on the system",
			  20);

		    return(VF_MAX_USERS);
		}
	    }
	}
    }

#ifdef __PASSWD_ETC
    /*
     * Check for account validity. Unfortunately, we have no graphical
     * dialog for this at this time so the best we can do is log an
     * error message and hope the system administrator sees it.
     */

    if ( !rgy_$using_local_registry() ) {
	if (rgy_$is_acct_expired(&admin_part) != 0 ) {
	    LogError(ReadCatalog(MC_LOG_SET,MC_LOG_ACC_EXP,MC_DEF_LOG_ACC_EXP), 
		      name);
	    return(VF_INVALID);
	}
    }
#endif

    /*
     *  check password aging...
     */

     if ( PasswordAged(p) ) return(VF_PASSWD_AGED);
         

    /*
     *  verify home directory exists...
     */

    if(chdir(p->pw_dir) < 0) {
	Audit(p, " attempted to login - no home directory", 1);
        return(VF_HOME);
    }


#ifdef AUDIT
    /*
     *  check audit flag and id...
     */

    setresuid(0,0,0);

    if (secure && (p->pw_audflg > 1 || p->pw_audflg < 0)) {
	Audit(p, " attempted to login - bad audit flag", 1);
 	return(VF_BAD_AFLAG);
    }

    if (secure && (setaudid(p->pw_audid) == -1 )) {
	Audit(p, " attempted to login - bad audit id", 1);
	return(VF_BAD_AID);
    }
#endif /* AUDIT */


    /*
     *  validate uid and gid...
     */

    if ((p->pw_gid < 0)      || 
        (p->pw_gid > MAXUID) ||
	(setgid(p->pw_gid) == -1)) {

	Audit(p, " attempted to login - bad group id", 1);
	return(VF_BAD_GID);
    }


#ifdef __AFS
    /*
     *  ka_UserAuthenticateGeneral() sets the group access of this process
     *  to the proper PAG. Pick up these values and pass them back to
     *  Dtlogin to be put into the user's environment...
     */
     
    if ( IsVerifyName(VN_AFS) ) {
	groups[0] = groups[1] = 0;
	getgroups(NGROUPS, groups);
    }

#endif /* __AFS */


    if ((p->pw_uid < 0)      || 
        (p->pw_uid > MAXUID) ||
	(setresuid(p->pw_uid, p->pw_uid, 0) == -1)) {

	Audit(p, " attempted to login - bad user id", 1);
	return(VF_BAD_UID);
    }




    /*
     * verify ok...
     */

    Audit(p, " Successful login", 0);
    return(VF_OK);
}




/***************************************************************************
 *
 *  WriteBtmp (HPUX)
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
#ifndef SVR4
	strncpy(utmp.ut_host, dpyinfo.name, sizeof(utmp.ut_host));
#endif
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
 *  End authentication routines (HPUX)
 *
 ***************************************************************************/
#endif /* __hpux */



/***************************************************************************
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************/
 

#ifdef sun
/***************************************************************************
 *
 *  Start authentication routines (SUN)
 *
 ***************************************************************************/




/***************************************************************************
 *
 *  External declarations (SUN)
 *
 ***************************************************************************/




/***************************************************************************
 *
 *  Procedure declarations (SUN)
 *
 ***************************************************************************/

static void Audit( struct passwd *p, char *msg, int errnum) ;
static int  PasswordAged( struct passwd *pw) ;
static void WriteBtmp( char *name) ;




/***************************************************************************
 *
 *  Global variables (SUN)
 *
 ***************************************************************************/




/***************************************************************************
 *
 *  Audit (SUN)
 *
 ***************************************************************************/

static void 
Audit( struct passwd *p, char *msg, int errnum )
{

    /*
     * make sure program is back to super-user...
     */

    seteuid(0);

    return;
}




/***************************************************************************
 *
 *  WriteBtmp (SUN)
 *
 *  log bad login attempts
 *  
 ***************************************************************************/

static void 
WriteBtmp( char *name )
{
    return;
}




/***************************************************************************
 *
 *  PasswordAged (SUN)
 *
 *  see if password has aged
 ***************************************************************************/
#define SECONDS_IN_WEEK		604800L

static int 
PasswordAged( struct passwd *pw )
{
    long change_week;	/* week password was changed (1/1/70 = Week 0) */
    long last_week;	/* week after which password must change */
    long first_week;	/* week before which password can't change */
    long this_week;	/* this week derived from time() */
    char *file;		/* help file name */
    char *command;	/* the /bin/passwd command string */

    if (*pw->pw_age == NULL)
	return(0);

    first_week = last_week = change_week = (long) a64l(pw->pw_age);
    last_week &= 0x3f;				/* first six bits */
    first_week = (first_week >> 6) & 0x3f;	/* next six bits */
    change_week >>= 12;				/* everything else */

    this_week = (long) time((long *) 0) / SECONDS_IN_WEEK;

/*
**	Password aging conditions:
**	*   if the last week is less than the first week (e.g., the aging
**	    field looks like "./"), only the superuser can change the
**	    password.  We don't request a new password.
**	*   if the week the password was last changed is after this week,
**	    we have a problem, and request a new password.
**	*   if this week is after the specified aging time, we request
**	    a new password.
*/
    if (last_week < first_week)
	return(0);

    if (change_week <= this_week && this_week <= (change_week + last_week))
	return(0);

    return(1);
}


    

/***************************************************************************
 *
 *  Verify (SUN)
 *
 *  verify the user
 *
 *  return codes indicate authentication results.
 ***************************************************************************/

#define MAXATTEMPTS	3

extern Widget focusWidget;		/* login or password text field	   */
struct  passwd nouser = {"", "nope"};	/* invalid user password struct	   */

int 
Verify( char *name, char *passwd )
{

    static int		login_attempts = 0; /* # failed authentications	   */
    
    struct passwd	*p;		/* password structure */
    struct spwd         *sp;            /* shadow info */
    char 		*crypt();

    int			n;

    p = getpwnam(name);
    sp = getspnam(name);
    
    if (!p || strlen(name) == 0 ||
        strcmp (crypt (passwd, sp->sp_pwdp), sp->sp_pwdp)) {

	if ( focusWidget == passwd_text ) {
	
	    WriteBtmp(name);

	    if ((++login_attempts % MAXATTEMPTS) == 0 ) {

		if (p->pw_name == NULL )
		    p = &nouser;

		Audit(p, " Failed login (bailout)", 1);

	    }
	}
	
	return(VF_INVALID);
    }


    /*
     *  check password aging...
     */

     if ( PasswordAged(p) ) return(VF_PASSWD_AGED);

    /*
     *  verify home directory exists...
     */

    if(chdir(p->pw_dir) < 0) {
	Audit(p, " attempted to login - no home directory", 1);
        return(VF_HOME);
    }


    /*
     *  validate uid and gid...
     */


    if ((p->pw_gid < 0)      || 
	(setgid(p->pw_gid) == -1)) {

	Audit(p, " attempted to login - bad group id", 1);
	return(VF_BAD_GID);
    }

    if ((p->pw_uid < 0)      || 
	(seteuid(p->pw_uid) == -1)) {

	Audit(p, " attempted to login - bad user id", 1);
	return(VF_BAD_UID);
    }



    /*
     * verify ok...
     */

    Audit(p, " Successful login", 0);
    return(VF_OK);
}



/***************************************************************************
 *
 *  End authentication routines (SUN)
 *
 ***************************************************************************/
#endif /* sun */

/***************************************************************************
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************/

#ifdef _AIX 
/***************************************************************************
 *
 *  Start authentication routines (AIX)
 *
 ***************************************************************************/

#include	<time.h>
#include	<sys/types.h>
#include	<usersec.h>
#include	<userpw.h>
#include	<userconf.h>

/***************************************************************************
 *
 *  External declarations (AIX)
 *
 ***************************************************************************/




/***************************************************************************
 *
 *  Procedure declarations (AIX)
 *
 ***************************************************************************/

static void Audit( struct passwd *p, char *msg, int errnum) ;
static int  PasswordAged(char *name, struct passwd *pw) ;
static void WriteBtmp( char *name) ;




/***************************************************************************
 *
 *  Global variables (AIX)
 *
 ***************************************************************************/




/***************************************************************************
 *
 *  Audit (AIX)
 *
 ***************************************************************************/

static void 
Audit( struct passwd *p, char *msg, int errnum )
{

    /*
     * make sure program is back to super-user...
     */

    seteuid(0);

    return;
}




/***************************************************************************
 *
 *  WriteBtmp (AIX)
 *
 *  log bad login attempts
 *  
 ***************************************************************************/

static void 
WriteBtmp( char *name )
{
    return;
}




/***************************************************************************
 *
 *  PasswordAged (AIX)
 *
 *  see if password has aged
 ***************************************************************************/
#define SECONDS_IN_WEEK		604800L

static int 
PasswordAged(char *name, struct passwd *pw )
{
  struct userpw *pupw; /* authentication information from getuserpw() */
  struct userpw  upw;  /* working authentication information */
  int err;             /* return code from getconfattr() */
  ulong maxage;        /* maximun age from getconfattr() */
  ulong now;           /* time now */

 /*
  * Determine user password aging criteria. Note that only
  * the 'lastupdate' and 'flags' fields are set by this operation.
  */
  setpwdb(S_READ);
  if ((pupw = getuserpw(name)) != NULL)
  {
    upw.upw_lastupdate = pupw->upw_lastupdate;
    upw.upw_flags = pupw->upw_flags;
  }
  else
  {
    upw.upw_lastupdate = 0;
    upw.upw_flags = 0;
  }
  endpwdb();

 /*
  * Consider password as having not expired if nocheck set.
  */
  if (upw.upw_flags & PW_NOCHECK) return(FALSE);

 /*
  * Get system password aging criteria.
  */
  err = getconfattr (SC_SYS_PASSWD, SC_MAXAGE, (void *)&maxage, SEC_INT);
  if (!err && maxage)
  {
   /*
    * Change from weeks to seconds
    */
    maxage = maxage * SECONDS_IN_WEEK;
    now = time ((long *) 0);

    if ((upw.upw_lastupdate + maxage) >= now)
    {
     /*
      * Password has not expired.
      */
      return(FALSE);
    }
  }
  else
  {
   /*
    * Could not retrieve system password aging info or maxage set to
    * zero. In either case, consider password has having not expired.
    */
    return(FALSE);
  }

 /* 
  * We haven't returned by now, so indicate password has expired.
  */
  return(TRUE);
}

    

/***************************************************************************
 *
 *  Verify (AIX)
 *
 *  verify the user
 *
 *  return codes indicate authentication results.
 ***************************************************************************/

#define MAXATTEMPTS	3

extern Widget focusWidget;		/* login or password text field	   */
struct  passwd nouser = {"", "nope"};	/* invalid user password struct	   */

int 
Verify( char *name, char *passwd )
{

    static int		login_attempts = 0; /* # failed authentications	   */
    
    struct passwd	*p;		/* password structure */
    char 		*crypt();

    int			n;

    p = getpwnam(name);
    
    if (!p || strlen(name) == 0 ||
        strcmp (crypt (passwd, p->pw_passwd), p->pw_passwd)) {

	if ( focusWidget == passwd_text ) {
	
	    WriteBtmp(name);

	    if ((++login_attempts % MAXATTEMPTS) == 0 ) {

		if (p->pw_name == NULL )
		    p = &nouser;

		Audit(p, " Failed login (bailout)", 1);

	    }
	}
	
	return(VF_INVALID);
    }


    /*
     *  check password aging...
     */

     if ( PasswordAged(name,p) ) return(VF_PASSWD_AGED);

    /*
     *  verify home directory exists...
     */

    if(chdir(p->pw_dir) < 0) {
	Audit(p, " attempted to login - no home directory", 1);
        return(VF_HOME);
    }


    /*
     *  validate uid and gid...
     */


    if ((p->pw_gid < 0)      || 
	(setgid(p->pw_gid) == -1)) {

	Audit(p, " attempted to login - bad group id", 1);
	return(VF_BAD_GID);
    }

    if ((p->pw_uid < 0)) {
	Audit(p, " attempted to login - bad user id", 1);
	return(VF_BAD_UID);
    }



    /*
     * verify ok...
     */

    Audit(p, " Successful login", 0);
    return(VF_OK);
}




/***************************************************************************
 *
 *  End authentication routines (AIX)
 *
 ***************************************************************************/
#endif /* _AIX */


/***************************************************************************
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************/

#ifdef generic
/***************************************************************************
 *
 *  Start authentication routines (generic)
 *
 ***************************************************************************/


/***************************************************************************
 *
 *  These are a set of routine to do simple password, home dir, uid, and gid
 *  validation. They can be used as a first pass validation for future
 *  porting efforts.
 *
 *  When platform specific validation is developed, those routines should be
 *  included in their own section and the use of these routines discontinued.
 *
 ***************************************************************************/




/***************************************************************************
 *
 *  External declarations (generic)
 *
 ***************************************************************************/




/***************************************************************************
 *
 *  Procedure declarations (generic)
 *
 ***************************************************************************/

static void Audit( struct passwd *p, char *msg, int errnum) ;
static int  PasswordAged( struct passwd *pw) ;
static void WriteBtmp( char *name) ;




/***************************************************************************
 *
 *  Global variables (generic)
 *
 ***************************************************************************/




/***************************************************************************
 *
 *  Audit (generic)
 *
 ***************************************************************************/

static void 
Audit( struct passwd *p, char *msg, int errnum )
{

    /*
     * make sure program is back to super-user...
     */

    seteuid(0);

    return;
}




/***************************************************************************
 *
 *  WriteBtmp (generic)
 *
 *  log bad login attempts
 *  
 ***************************************************************************/

static void 
WriteBtmp( char *name )
{
    return;
}




/***************************************************************************
 *
 *  PasswordAged (Generic)
 *
 *  see if password has aged
 ***************************************************************************/
#define SECONDS_IN_WEEK		604800L

static int 
PasswordAged( struct passwd *pw )
{
  return(FALSE);
}

    

/***************************************************************************
 *
 *  Verify (generic)
 *
 *  verify the user
 *
 *  return codes indicate authentication results.
 ***************************************************************************/

#define MAXATTEMPTS	3

extern Widget focusWidget;		/* login or password text field	   */
struct  passwd nouser = {"", "nope"};	/* invalid user password struct	   */

int 
Verify( char *name, char *passwd )
{

    static int		login_attempts = 0; /* # failed authentications	   */
    
    struct passwd	*p;		/* password structure */
    char 		*crypt();

    int			n;

    p = getpwnam(name);
    
    if (!p || strlen(name) == 0 ||
        strcmp (crypt (passwd, p->pw_passwd), p->pw_passwd)) {

	if ( focusWidget == passwd_text ) {
	
	    WriteBtmp(name);

	    if ((++login_attempts % MAXATTEMPTS) == 0 ) {

		if (p->pw_name == NULL )
		    p = &nouser;

		Audit(p, " Failed login (bailout)", 1);

	    }
	}
	
	return(VF_INVALID);
    }


    /*
     *  check password aging...
     */

     if ( PasswordAged(p) ) return(VF_PASSWD_AGED);
         

    /*
     *  verify home directory exists...
     */

    if(chdir(p->pw_dir) < 0) {
	Audit(p, " attempted to login - no home directory", 1);
        return(VF_HOME);
    }


    /*
     *  validate uid and gid...
     */


    if ((p->pw_gid < 0)      || 
	(setgid(p->pw_gid) == -1)) {

	Audit(p, " attempted to login - bad group id", 1);
	return(VF_BAD_GID);
    }

    if ((p->pw_uid < 0)      || 
	(seteuid(p->pw_uid) == -1)) {

	Audit(p, " attempted to login - bad user id", 1);
	return(VF_BAD_UID);
    }



    /*
     * verify ok...
     */

    Audit(p, " Successful login", 0);
    return(VF_OK);
}




/***************************************************************************
 *
 *  End authentication routines (generic)
 *
 ***************************************************************************/
#endif /* generic */



/***************************************************************************
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************/
