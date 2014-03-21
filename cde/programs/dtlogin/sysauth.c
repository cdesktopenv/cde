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
/* $XConsortium: sysauth.c /main/7 1996/10/30 11:12:45 drk $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        sysauth.c
 **
 **   Project:     DT
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
 **	__apollo    Domain OS only
 **	__hp_osf    HP OSF/1 OS only
 **     sun         SUN OS only
 **     USL         USL OS only
 **     SVR4        SUN OS and USL
 **     _AIX        AIX only
 **     _POWER      AIX version 4 only
 **	__uxp__     Fujitsu UXP/DS
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

#ifdef PAM
#include	<security/pam_appl.h>
#endif

#ifdef _AIX
#include	<usersec.h>
#include	<login.h>
#include	<sys/access.h>
#include 	<sys/sem.h>
#include 	<sys/stat.h>
#include	<sys/ipc.h>
#include	<sys/audit.h>
#endif

/* necessary for bzero */
#ifdef SVR4
#include        <X11/Xfuncs.h>
#if defined(sun) || defined(__uxp__)
#include        <shadow.h>
#endif
#endif

#if defined(USL) 
#include <iaf.h>
#endif

#include	"dm.h"
#include	"vg.h" 
#include	"vgmsg.h"
#include	"sysauth.h"

/*
 * Define as generic those without platform specific code.
 */
#if !(defined(__hpux)   || \
      defined(__apollo) || \
      defined(_AIX)     || \
      defined(sun) 	|| \
      defined(__uxp__)  || \
      defined(USL))
#define generic
#endif

#ifdef __osf__
#include <sys/sysinfo.h>
#endif

#if defined(linux)
#    include <shadow.h>
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

extern char *globalDisplayName;

/***************************************************************************
 *
 *  External declarations (HPUX)
 *
 ***************************************************************************/

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
static int	PasswordAged( register struct passwd *pw) ;
static void	WriteBtmp( char *name) ;




/***************************************************************************
 *
 *  Global variables (HPUX)
 *
 ***************************************************************************/

#ifdef AUDIT
  struct s_passwd *s_pwd;
  struct stat	s_pfile;
  int		secure;	/* flag to denote existance of secure passwd file */
  int    	selfAuditOn;
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
PasswordAged( register struct passwd *pw )
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
     
    if ( vg_IsVerifyName(VN_AFS) ) {

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
     
    if ( vg_IsVerifyName(VN_KRB) ) {

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
     *   If regular passwd check fails, try old-style Apollo SR
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
 *  Authenticate (HPUX)
 *
 *  verify the user
 *
 *  return codes indicate authentication results.
 ***************************************************************************/

#define MAXATTEMPTS	3

struct  passwd nouser = {"", "nope"};	/* invalid user password struct	   */

int 
Authenticate( struct display *d, char *name, char *passwd, char **msg )
{

    static int		login_attempts = 0; /* # failed authentications	   */
    
    struct passwd	*p = NULL;	/* password structure */

    struct utsname	utsnam;
    int			n;

    char               *origpw;

   /*
    * Nothing to do if no name provided.
    */
    if (!name)
      return(VF_INVALID); 
 
#ifdef SIA

    if (strlen(name) == 0)
      return(VF_INVALID);

    if (!passwd)
      return(VF_CHALLENGE);

    return(VF_OK);

#endif
  
   /*
    * Save provided password.
    */
    origpw = passwd; 
    if (!passwd) passwd = "";
    
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
	
	    WriteBtmp(name);

	    if ((++login_attempts % MAXATTEMPTS) == 0 ) {

		if (p == NULL )
		    p = &nouser;

		Audit(p, " Failed login (bailout)", 1);

	    }
	
	return(origpw ? VF_INVALID : VF_CHALLENGE);
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
     
    if ( vg_IsVerifyName(VN_AFS) ) {
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

    int found=FALSE;

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
	strncpy(utmp.ut_line, globalDisplayName, sizeof(utmp.ut_line));
	utmp.ut_type = LOGIN_PROCESS;
#ifndef SVR4
	strncpy(utmp.ut_host, globalDisplayName, sizeof(utmp.ut_host));
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


#if defined(__uxp__) 

/***************************************************************************
 *
 *  Start authentication routines (UXP)
 *
 ***************************************************************************/




/***************************************************************************
 *
 *  External declarations (UXP)
 *
 ***************************************************************************/




/***************************************************************************
 *
 *  Procedure declarations (UXP)
 *
 ***************************************************************************/

static void Audit( struct passwd *p, char *msg, int errnum) ;
static int  PasswordAged( register struct passwd *pw) ;
static void WriteBtmp( char *name) ;




/***************************************************************************
 *
 *  Global variables (UXP)
 *
 ***************************************************************************/




/***************************************************************************
 *
 *  Audit (UXP)
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
 *  WriteBtmp (UXP)
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
 *  PasswordAged (UXP)
 *
 *  see if password has aged
 ***************************************************************************/
#define SECONDS_IN_WEEK		604800L

static int 
PasswordAged( register struct passwd *pw )
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
 *  Authenticate (UXP)
 *
 *  verify the user
 *
 *  return codes indicate authentication results.
 ***************************************************************************/

#define MAXATTEMPTS	3

struct  passwd nouser = {"", "nope"};	/* invalid user password struct	   */

int 
Authenticate( struct display *d, char *name, char *passwd, char **msg )
{

    static int		login_attempts = 0; /* # failed authentications	   */
    
    struct passwd	*p;		/* password structure */
    struct spwd         *sp;            /* shadow info */
    char 		*crypt();

    int			n;

    char               *origpw;

   /*
    * Nothing to do if no name provided.
    */
    if (!name)
      return(VF_INVALID);

   /*
    * Save provided password.
    */
    origpw = passwd;
    if (!passwd) passwd = "";


    p = getpwnam(name);
    sp = getspnam(name);
    
    if (!p || strlen(name) == 0 ||
        strcmp (crypt (passwd, sp->sp_pwdp), sp->sp_pwdp)) {

	    WriteBtmp(name);

	    if ((++login_attempts % MAXATTEMPTS) == 0 ) {

		if (p == NULL )
		    p = &nouser;

		Audit(p, " Failed login (bailout)", 1);

	    }
	
	return(origpw ? VF_INVALID : VF_CHALLENGE);
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
 *  End authentication routines (UXP)
 *
 ***************************************************************************/
#endif

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

#include "solaris.h"
#ifdef SUNAUTH
#include <security/ia_appl.h>
#endif


/***************************************************************************
 *
 *  Procedure declarations (SUN)
 *
 ***************************************************************************/

static void Audit( struct passwd *p, char *msg, int errnum) ;



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

    Debug("Audit: %s\n", msg);

    return;
}


/***************************************************************************
 *
 *  Authenticate (SUN)
 *
 *  verify the user
 *
 *  return codes indicate authentication results.
 ***************************************************************************/

int 
Authenticate( struct display *d, char *name, char *passwd, char **msg )
{
   int	status;
   char* ttyLine = d->gettyLine;

   /*
    * Nothing to do if no name provided.
    */
    if (!name) {
      return(VF_INVALID);
    }

   /*
    * Construct device line
    */
#ifdef DEF_NETWORK_DEV
            /*
             * If location is not local (remote XDMCP dtlogin) and
             * remote accouting is enabled (networkDev start with /dev/...)
             * Set tty line name to match network device for accouting.
             * Unless the resource was specifically set, default is value
             * of DEF_NETWORK_DEV define (/dev/dtremote)
             */

            if ( d->displayType.location != Local &&
                 networkDev && !strncmp(networkDev,"/dev/",5)) {
                ttyLine = networkDev+5;
            }
#endif

   /*
    * Authenticate user and return status
    */

#ifdef PAM 
    status = PamAuthenticate("dtlogin", d->name, passwd, name, ttyLine);

    switch(status) {
        case PAM_SUCCESS:
            return(VF_OK);

        case PAM_NEW_AUTHTOK_REQD:
	    return(VF_PASSWD_AGED);

	default:
	    return(passwd ? VF_INVALID : VF_CHALLENGE);
    }
#else
    status = solaris_authenticate("dtlogin", d->name, passwd, name, ttyLine);

    switch(status) {
        case IA_SUCCESS:
            return(VF_OK);

        case IA_NEWTOK_REQD:
	    return(VF_PASSWD_AGED);

	default:
	    return(passwd ? VF_INVALID : VF_CHALLENGE);
    }
#endif /* !PAM */
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
 ***************************************************************************/
#ifdef _AIX 
/***************************************************************************
 *
 *  GetLoginInfo
 *
 *  get the information from the display about local/remote login and
 *  create a dummy tty name for loginrestrictions.
 *
 ***************************************************************************/
void
GetLoginInfo(struct display *d, int *loginType, char *ttyName, char **hostname)
{
  char workarea[128];

  CleanUpName(d->name, workarea, 128);
  sprintf(ttyName, "/dev/dtlogin/%s", workarea);

  if (d->displayType.location == Foreign) {
    *loginType = S_RLOGIN;
    *hostname = d->name;
    Debug("Login Info - Remote user on tty=%s.\n", ttyName);
  } else {
    *loginType = S_LOGIN;
    *hostname = NULL;
    Debug("Login Info - Local user on tty=%s.\n", ttyName);
  }
}
#ifdef _POWER
/***************************************************************************
 *
 *  Authenticate (AIX version 4.1)
 *
 *  verify the user
 *
 *  return codes indicate authentication results.
 ***************************************************************************/
Authenticate( struct display *d, char *name, char *passwd, char **msg )
{
  int arc;
  int rc;
  int reenter;
  static int unknown;
  static char *msgpending;
  int loginType;
  char tty[128];
  char *hostname;

  GetLoginInfo(d, &loginType, tty, &hostname);

  if (name == NULL)
  {
    unknown = 0;
  }

  if (unknown)
  {
 /*
    * No more challenges. User failed login.
    */
    unknown = 0;
    loginfailed(name, hostname, tty);
    return(VF_INVALID);
  }

 /*
  * Authenticate with response to last challenge.
  */
  rc = authenticate(name, passwd, &reenter, msg);

  if (reenter)
  {
   /*
    * System has presented user with new challenge.
    */
    return(VF_CHALLENGE);
  }

  if (rc && errno == ENOENT)
  { 
   /*
    * User is unknown to the system. Simulate a password
    * challenge, but save message for display for next call.
    */
    unknown = 1;
    return(VF_CHALLENGE);
  }

  if (rc)
  {
   /*
    * No more challenges. User failed login.
    */
    loginfailed(name, hostname, tty);
    return(VF_INVALID);
  }

 /*
  * User authenticated. Check login restrictions.
  */
  rc = loginrestrictions(name, loginType, tty, msg);

  if (rc)
  {
   /* 
    * Login restrictions disallow login.
    */
    loginfailed(name, hostname, tty);
    return(VF_MESSAGE);
  }

 /*
  * Check password expiration.
  */
  rc = passwdexpired(name, msg);

  if (rc)
  {
   /*
    * Login succeeded, but password expired.
    */
    return(VF_PASSWD_AGED);
  }

 /*
  * Login succeeded.
  */
  loginsuccess(name, hostname, tty, msg);
  return(VF_OK);
}



#else /* !_POWER */
/***************************************************************************
 *
 *  Start authentication routines (AIX)
 *
 ***************************************************************************/

#include	<time.h>
#include	<sys/types.h>
#include	<sys/errno.h>
#include	<usersec.h>
#include	<userpw.h>
#include	<userconf.h>
#include	<utmp.h>
#include	<time.h>

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
static int  PasswordAged(char *name, register struct passwd *pw) ;
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
    if ( (auditwrite ("USER_Login", AUDIT_OK,
            p->pw_name, strlen (p->pw_name) + 1,
            msg, strlen (msg) + 1, NULL))  == -1 )
          Debug(" Could not do Auditing\n");

}




/***************************************************************************
 *
 *  WriteBtmp (AIX)
 *
 *  log bad login attempts to /etc/security/failedlogin file
 *  
 *  RK	09.13.93
 ***************************************************************************/

static void 
WriteBtmp( char *name )
{
	int	fd;
	struct  utmp	ut;

	if( (fd = open("/etc/security/failedlogin",O_CREAT|O_RDWR,0644)) != -1) {
		bzero(&ut,sizeof(struct utmp));
		if(name)
			strncpy(ut.ut_user, name, sizeof ut.ut_user);
		ut.ut_type = USER_PROCESS;
		ut.ut_pid = getpid();
		ut.ut_time = time((time_t *)0);
		write(fd, (char *)&ut, sizeof(struct utmp));
		close(fd);
	}
}




/***************************************************************************
 *
 *  PasswordAged (AIX)
 *
 *  see if password has aged
 ***************************************************************************/
#define SECONDS_IN_WEEK		604800L

static int 
PasswordAged(char *name, register struct passwd *pw )
{
  struct userpw *pupw; /* authentication information from getuserpw() */
  struct userpw  upw;  /* working authentication information */
  int err;             /* return code from getconfattr() */
  ulong maxage;        /* maximun age from getconfattr() */
  ulong now;           /* time now */

#ifdef	_POWER
  return(FALSE);
#else	/* _POWER */
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
#endif	/* _POWER */
}

/***************************************************************************
 *  dt_failedlogin (AIX)
 *
 * log failed login in /etc/security/lastlog
 ***************************************************************************/
struct  lastlogin {
        time_t  ftime;
        time_t  stime;
        int     fcount;
        char    user[32];
        char    *stty;
        char    *ftty;
        char    *shost;
        char    *fhost;
};
extern void 
	dt_lastlogin ( char * user, struct lastlogin * llogin);

void 
dt_failedlogin(char *name, char *ttyName, char *hostName)
{
	struct lastlogin last_login;

	last_login.stime = 0;

        time(&last_login.ftime);

        last_login.ftty = ttyName;

        last_login.fhost = (char *) malloc (MAXHOSTNAMELEN);
        if (hostName == NULL) {
            gethostname (last_login.fhost , MAXHOSTNAMELEN);
        } else {
            strncpy(last_login.fhost, hostName, MAXHOSTNAMELEN);
            last_login.fhost[MAXHOSTNAMELEN -1] = '\0';
        }

        Debug("logging failed lastlogin entry (user=%s)\n",name);
        dt_lastlogin(name, &last_login);
        free(last_login.fhost);
}

    

/***************************************************************************
 *
 *  Authenticate (AIX)
 *
 *  verify the user
 *
 *  return codes indicate authentication results.
 ***************************************************************************/

#define MAXATTEMPTS	3

struct  passwd nouser = {"", "nope"};	/* invalid user password struct	   */

int 
Authenticate( struct display *d, char *name, char *passwd, char **msg )
{

    static int		login_attempts = 0; /* # failed authentications	   */
    struct passwd	*p;		/* password structure */
    char 		*crypt();
    char               *origpw;
    int loginType;
    char tty[128];
    char *hostname;

   /*
    * Nothing to do if no name provided.
    */
    if (!name)
      return(VF_INVALID);

   /*
    * Save provided password.
    */
    origpw = passwd;
    if (!passwd) passwd = "";

    if(strlen(name) > S_NAMELEN)
         return(VF_INVALID);

    GetLoginInfo(d, &loginType, tty, &hostname);

    p = getpwnam(name);
     
    if (!p  || strcmp (crypt (passwd, p->pw_passwd), p->pw_passwd)) {

	WriteBtmp(name);  

	if ((++login_attempts % MAXATTEMPTS) == 0 ) {

	    if (p == NULL )
		p = &nouser;

	    Audit(p, " Failed login (bailout)", 1);
	}

	if (origpw) {
	    dt_failedlogin(name, tty, hostname);
	    return (VF_INVALID);
	} else
	    return(VF_CHALLENGE);
    }

    /* Note: The password should be checked if it is the first time
             the user is logging in or whether the sysadm has changed
             the password for the user. Code should be added here if
             this functionality should be supported. The "upw_flags"
             of the password structure gets set to PW_ADMCHG in this
             case.				RK 09.13.93.
    */

    /*
     *  check password aging...
     */

    if ( PasswordAged(name,p) ) return(VF_PASSWD_AGED);


    /* Validate for User Account  RK 09.13.93 */
    if(ckuseracct(name, loginType, tty) == -1)  {
	dt_failedlogin(name, tty, hostname);
	return(VF_INVALID);
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

    /* Check for max number of logins  RK 09.13.93 */
    if (tsm_check_login(p->pw_uid) == -1) {
	dt_failedlogin(name, tty, hostname);
        return(VF_INVALID);
    }

    /* Check for /etc/nologin file   RK 09.13.93 */
    if ( (access("/etc/nologin",R_OK) == 0) && (p->pw_uid != 0) ) {
	dt_failedlogin(name, tty, hostname);
	return(VF_INVALID);
    }

    /*
     *  verify home directory exists...
     */

    if(chdir(p->pw_dir) < 0) {
	Audit(p, " attempted to login - no home directory", 1);
        return(VF_HOME);
    }

    /*
     * verify ok...
     */

    Audit(p, " Successful login", 0);
    return(VF_OK);
}

/**************************************************************************
* 
* tsm_check_login() 
*
* Checks for max number of logins on the system. If the new user trying to
* login exceeds the max limit then the user is not allowed to login.
*
* RK 09.13.93
**************************************************************************/

/**************************************************************************
* 
* tsm_check_login() 
*
* Checks for max number of logins on the system. If the new user trying to
* login exceeds the max limit then the user is not allowed to login.
*
* RK 09.13.93
**************************************************************************/

int
tsm_check_login(uid_t uid)
{

	key_t	key;
	char    *buffer;
	int	semid;
	int 	fd;
	struct  stat stat_buf;
	static	struct	sembuf	sop = { 0, -1, (SEM_UNDO|IPC_NOWAIT) };
	static	struct	sembuf	initsop = { 0, 0, (IPC_NOWAIT) };

	/*
	 * The login counter semaphore may not be set yet.  See if it exists
	 * and try creating it with the correct count if it doesn't.  An
	 * attempt is made to create the semaphore.  Only if that attempt fails
	 * is the semaphore set to maxlogins from login.cfg.
	 */


	/*
	 * Don't Check if the user is already logged. ie running login
	 * from a shell
	 */
		/*
		 * Generate the semaphore key from the init program.
		 */
		Debug("Start of maxlogin check\n");
		if ((key = ftok (CDE_INSTALLATION_TOP "/bin/dtlogin", 1)) != (key_t) -1) {
			Debug("key created\n");
			if ((semid = semget (key, 1, IPC_CREAT|IPC_EXCL|0600)) != -1) {
				int	i;
				Debug("Completed IPCkey\n");
				if (! getconfattr ("usw", "maxlogins", &i, SEC_INT)) {
                                        Debug("Max logins from login.cfg is :%d\n",i);
					if (i <= 0)
						i = 10000; /* a very large number */

					initsop.sem_op = i;
					if (semop (semid, &initsop, 1))
						{
                                                    Debug("failed while decrementing\n");
						return(-1);	
						}
				} else {
					semctl (semid, 1, IPC_RMID, 0);
				}
			}

			/*
			 * Only 'n' login sessions are allowed on the system. 
			 * This code block decrements a semaphore.
			 * The semundo value will be set to adjust the
			 * semaphore when tsm exits.
			 *
			 * This code will be ignored if the appropriate
			 * semaphore set does not exist.
			 */

			if ((semid = semget (key, 1, 0)) != -1) {
                                  Debug("getting key for maxlogins\n");
				/*
				 * If the semaphore is zero and we are not
				 * root, then we fail as there are already the
				 * allotted number of login sessions on the
				 * system.
				 */
				if ((semop (semid, &sop, 1)  == -1) && uid) {
                                                    Debug("reached MAXLOGINS limit\n");
					errno = EAGAIN;
					return(-1);
				}
			}
		}

}
#endif /* !_POWER */
#endif /* _AIX */
/***************************************************************************
 *
 *  End authentication routines (AIX)
 *
 ***************************************************************************/

/***************************************************************************
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************/


#if defined(USL) 
/***************************************************************************
 *
 *  Start authentication routines (USL)
 *
 ***************************************************************************/


#define LOGIN_SUCCESS   0
#define LOGIN_FAIL      1
#define INACTIVE        2
#define EXPIRED         3
#define IDLEWEEKS       4
#define MANDATORY       5
#define AGED            6
#define BADSHELL        7
#define NOHOME          8
#define PFLAG           11

/***************************************************************************
 *
 *  External declarations (USL)
 *
 ***************************************************************************/



/***************************************************************************
 *
 *  Procedure declarations (USL)
 *
 ***************************************************************************/


static void Audit( struct passwd *p, char *msg, int errnum) ;
static int  PasswordAged( register struct passwd *pw) ;
static void WriteBtmp( char *name) ;



/***************************************************************************
 *
 *  Global variables (USL)
 *
 ***************************************************************************/

extern int      Slaveptty_fd;


/***************************************************************************
 *
 *  Audit (USL)
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
 *  WriteBtmp (USL)
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
 *  Authenticate (USL)
 *
 *  verify the user
 *
 *  return codes indicate authentication results.
 ***************************************************************************/

#define MAXATTEMPTS	3

int 
Authenticate( struct display *d, char *name, char *passwd, char **msg )
{

    int			ret;
    char                **ava, *tty;
    char                *p;
    char                xlogname[137];
    char                xpasswd[137];
    char                xtty[137];

    char               *origpw;

   /*
    * Nothing to do if no name provided.
    */
    if (!name)
      return(VF_INVALID);

   /*
    * Save provided password.
    */
    origpw = passwd;
    if (!passwd) passwd = "";

	/*
	 * Put the logname and the passwd on the ava stream
	 */
	if ((ret = dup2 (Slaveptty_fd, 0)) == -1)
		Debug ("Problem with dup2\n");

	ava = retava(Slaveptty_fd);

	(void) bzero (xlogname, strlen (xlogname));
	(void) sprintf (xlogname, "XLOGNAME=%s", name);
	if ((ava = putava (xlogname, ava)) == NULL)
		{
		Debug ("Could not set logname ava\n");
		}

	(void) bzero (xpasswd, strlen (xpasswd));
	(void) sprintf (xpasswd, "XPASSWD=%s", passwd);
	if ((ava = putava (xpasswd, ava)) == NULL)
		{
		Debug ("Could not set passwd ava\n");
		}

	(void) bzero (xtty, strlen (xtty));
	(void) sprintf (xtty, "XTTY=%s", ttyname(Slaveptty_fd));
	if ((ava = putava (xtty, ava)) == NULL)
		{
		Debug ("Could not set tty ava\n");
		}

	if (setava (Slaveptty_fd, ava) != 0)
		{
		Debug ("Could not do setava\n");
		}

	/*
	 * invoke identification and authorizarion scheme
	 */

	switch (ret = invoke (Slaveptty_fd, "login"))
		{

		case LOGIN_SUCCESS:
			Audit(p, " Successful login", 0);
    			return(VF_OK);
		case LOGIN_FAIL:
			return(origpw ? VF_INVALID : VF_CHALLENGE);

		/* The next three cases need to define special return values */
		/* for the aged passwords and accounts.			     */

		case INACTIVE:
		case IDLEWEEKS:
			/* PasswdAged (linfo); */
			return(VF_PASSWD_AGED);
		case EXPIRED:  /* SS */
			/* AccountAged (linfo); */
			return(VF_PASSWD_AGED);

		/* These 3 cases should allow user to select a new password */
		/* after displaying a warrning, but current implementation  */
		/* only displays the warning.				    */

		case MANDATORY:
		case PFLAG:
		case AGED:
			return(VF_PASSWD_AGED);
		default:
			return(VF_INVALID);  /* SS */
		}
}


/***************************************************************************
 *
 *  End authentication routines (USL)
 *
 ***************************************************************************/
#endif /* USL */

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
static int  PasswordAged( register struct passwd *pw) ;
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
PasswordAged( register struct passwd *pw )
{
  return(FALSE);
}

    

/***************************************************************************
 *
 *  Authenticate (generic)
 *
 *  verify the user
 *
 *  return codes indicate authentication results.
 ***************************************************************************/

#define MAXATTEMPTS	3

struct  passwd nouser = {"", "nope"};	/* invalid user password struct	   */

int 
Authenticate( struct display *d, char *name, char *passwd, char **msg )
{

    static int		login_attempts = 0; /* # failed authentications	   */
    
    struct passwd	*p;		/* password structure */
    char 		*crypt();

    int			n;

    char               *origpw;

   /*
    * Nothing to do if no name provided.
    */
    if (!name)
      return(VF_INVALID);

   /*
    * Save provided password.
    */
    origpw = passwd;
    if (!passwd) passwd = "";

    p = getpwnam(name);
    
#if defined(linux)
    /*
     * Use the Linux Shadow Password system to get the crypt()ed password
     */
    if(p) {
        struct spwd *s = getspnam(name);
	if(s) {
            p->pw_passwd = s->sp_pwdp;
        }
    }
#endif

    if (!p || strlen(name) == 0 ||
        strcmp (crypt (passwd, p->pw_passwd), p->pw_passwd)) {

	    WriteBtmp(name);

	    if ((++login_attempts % MAXATTEMPTS) == 0 ) {

		if (p == NULL )
		    p = &nouser;

		Audit(p, " Failed login (bailout)", 1);

	    }
	
	return(origpw ? VF_INVALID : VF_CHALLENGE);
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
