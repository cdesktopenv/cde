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
/* $XConsortium: get_level.c /main/4 1995/10/27 16:19:39 rswiston $ */
/*
 * get_level.c 
 * last modified by: 
 * David Dolson June 7/92
 *	- rewrote most of B1 security routines.  Much of it is based on
 *	  parallel routines in login.
 * Ron Voll     July 7/92
 *      - rolled the xdm version of this file into dtlogin.
 */

#ifdef BLS	/* Brackets entire file */
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <termio.h>
#include <errno.h>
#ifdef SEC_NET_TTY
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

#include <sys/security.h>
#include <sys/audit.h>
#include <prot.h>
#include <protcmd.h>
#if defined(TAC3) && !defined(TPLOGIN)
#include <sys/secpolicy.h>
#include <mandatory.h>
#include <fcntl.h>
#endif
#include <stdlib.h>
#include <time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#if 0
#include <strings.h>
#endif
#include <sys/wait.h>
#include <grp.h>

/*
 * Local include file for bls specific definitions.
 * Also defines some of the structures from dm.h for bls usage.
 */
#include "bls.h"

/* drop those privs from the base set which are not needed by xdm */
void
drop_privs(void)
{
	priv_t privs[SEC_SPRIVVEC_SIZE];

	getpriv(SEC_BASE_PRIV, privs);
	RMBIT(privs, SEC_ALLOWNETACCESS);
	RMBIT(privs, SEC_NETPRIVSESSION);
	RMBIT(privs, SEC_NETNOAUTH);
	RMBIT(privs, SEC_MULTILEVELDIR);
	setpriv(SEC_BASE_PRIV, privs);
	return;
}

/* stuff to do at the start */
void
init_security(void)
{
	/* set default file creation mode to be restrictive */
	umask(~SEC_DEFAULT_MODE);
	drop_privs();
}

/* check that the requested security level is valid for this user,
 * return 1 = success, return 0 is fail(fatal)
 */
int
verify_user_seclevel( struct verify_info verify, char *desired_label)
{
	int uid;
	mand_ir_t *dl_ir, *clearance_ir;
	struct pr_passwd *prpwd;
	struct passwd *pwd;

	prpwd = verify->prpwd;
	pwd = verify->pwd;
	uid = verify->uid;

	/* check that desired_label falls within user's range */
	dl_ir = mand_er_to_ir(desired_label);
	if (dl_ir ==NULL) {
	    audit_login(prpwd, pwd, verify->terminal,
		"Unknown clearance level", ES_LOGIN_FAILED);
	    Debug("unable to translate clearance\n");
	    return 0;
	}
	/* get user clearance from prpwd database */
	if (prpwd->uflg.fg_clearance)   
		clearance_ir = &prpwd->ufld.fd_clearance;
	else if (prpwd->sflg.fg_clearance) 
		clearance_ir = &prpwd->sfld.fd_clearance; 
	else 
		clearance_ir = mand_syslo;

	/* make sure clearance dominates or equals desired_label */
	switch(mand_ir_relationship(/* subject */ dl_ir, 
				/* object */ clearance_ir)) {
	    case MAND_ODOM:
	    case MAND_EQUAL:
		/* Within range */
		break;
	    default:
	    	audit_login(prpwd, pwd, verify->terminal,
		    "Security label out of range", ES_LOGIN_FAILED);
		Debug("Invalid clearance for this user\n");
		mand_free_ir(dl_ir);
		return 0;
	}
	verify->clearance_ir = clearance_ir;
	verify->sec_label_ir = dl_ir;

	return 1;
}

/* check the proper structures to determine if the user has a password.
 * If the nullpw field is set, the user does not need one, and this
 * overrides the rest of the checking.
 * return 1 means that a password exists (or is not needed)
 */
int 
password_exists(struct verify_info *verify)
{
    struct pr_passwd *prpwd;
    BOOL nocheck;

    Debug("password_exists()\n");
    prpwd = verify->prpwd;
    if (prpwd->uflg.fg_nullpw)
	nocheck=prpwd->ufld.fd_nullpw;
    else if (prpwd->sflg.fg_nullpw)
	nocheck=prpwd->sfld.fd_nullpw;
    else
	nocheck=FALSE;

    if (!nocheck) { 	/* user needs password */
    	Debug("password required for user\n");
    	if (!prpwd->uflg.fg_encrypt ||
	    prpwd->ufld.fd_encrypt[0] == '\0' ) {
		return 0;
    	}
    }
    return 1;
}


/* check that the requested security level can be used on this X terminal,
 * and that it is not locked.
 * Currently there is no support for locking xterms like there is for 
 * /dev/tty* terminals.
 */
int
verify_sec_xterm(struct verify_info *verify, char *desired_label)
{
	return 1;
}


/* set clearance and label for the user.  Audit all failures.
 * return 0=fail, 1=pass
 */
int
set_sec_label(struct verify_info *verify)
{
	struct pr_passwd *prpwd;
	struct passwd *pwd;
	/* set clearance */
	prpwd = verify->prpwd;
	pwd = verify->pwd;

	if (setclrnce(verify->sec_label_ir)==-1) {
	  switch(errno) {
	    case EPERM:
	    	audit_login(prpwd, pwd, verify->terminal,
		    "Insufficient privs to set clearance", ES_LOGIN_FAILED);
		Debug ("login failed: EPERM on setclrnce()\n");
		break;
	    case EFAULT:
		/* audit:login failed: xdm memory fault */
	    default:
	    	audit_login(prpwd, pwd, verify->terminal,
		    "Unable to set clearance", ES_LOGIN_FAILED);
		Debug ("setclrnce failed: error: %d\n", errno);
		break;
	  }
	  return 0;
	}
	/* set label */
	if (setslabel(verify->sec_label_ir)==-1) {
	  switch(errno) {
	    case EPERM:
	    	audit_login(prpwd, pwd, verify->terminal,
		    "Insufficient privs to set sec label", ES_LOGIN_FAILED);
		Debug ("login failed: insufficient priv. to setslabel()\n");
		break;
	    case EFAULT:
		/* audit:login failed: xdm memory fault */
	    default:
	    	audit_login(prpwd, pwd, verify->terminal,
		    "Unable to set sec label", ES_LOGIN_FAILED);
		Debug ("setslabel() failed: error: %d\n", errno);
		break;
	  }
	  return 0;
	}
	return 1;
}

/* set the effective, base, and maximum priv vectors for the 
 * new process, based on values from the pr_passwd entry.
 * Inability to find either user priv's or default priv's
 * results in failure.  One or the other must be there.
 * Function returns 1 for success, 0 for failure.
 * A failure of this function should be considered fatal.
 */
int
set_sec_privs(struct verify_info *verify)
{

	priv_t *maxprivs, *bprivs;
	priv_t curr_bprivs[SEC_SPRIVVEC_SIZE];
	priv_t curr_sprivs[SEC_SPRIVVEC_SIZE];

	struct pr_passwd *prpwd;
	struct passwd *pwd;
	int bit;

	prpwd = verify->prpwd;
	pwd = verify->pwd;

	/* kernel authorizations */
	if (prpwd->uflg.fg_sprivs) {
	    maxprivs = &prpwd->ufld.fd_sprivs[0];
	}else if(prpwd->sflg.fg_sprivs) { 
	    maxprivs = &prpwd->sfld.fd_sprivs[0];
	    Debug("Using default kernel priv's\n");
	}else {
	    audit_login(prpwd, pwd, verify->terminal, 
		"Unable to find kernel priv set for user",
		ES_LOGIN_FAILED);
	    Debug("Can't find max. priv set for user-quitting\n");
	    return 0;
	}

	/* base priv's and initial effective priv's */
	if (verify->prpwd->uflg.fg_bprivs) {
	    bprivs = &verify->prpwd->ufld.fd_bprivs[0];
	}else if (verify->prpwd->sflg.fg_bprivs) {  /* use system defaults */
	    bprivs = &verify->prpwd->sfld.fd_bprivs[0];
	    Debug("Using default base priv's\n");
	}else{
	    audit_login(prpwd, pwd, verify->terminal, 
		"Unable to find base priv set for user",
		ES_LOGIN_FAILED);
	    Debug("Can't find base priv set for user-quitting\n");
	    return 0;
	}

	getpriv(SEC_MAXIMUM_PRIV, curr_sprivs);
	getpriv(SEC_BASE_PRIV, curr_bprivs);

	/* remove those privs which the current process does not have,
	 * to avoid any error in the following system calls 
	 */
	for (bit=0; bit<=SEC_MAX_SPRIV; bit++) {
		if (!ISBITSET(curr_sprivs, bit)) 
			RMBIT(maxprivs, bit);
		if (!ISBITSET(curr_bprivs, bit))
			RMBIT(bprivs, bit);
	}
	
	/* login removes those bits from maxprivs which the current process
	 * does not have. - This program assumes the system config
	 * utilities will enforce the rules for setpriv(3).  Any failure
	 * of setpriv will indicate a corrupt database.
	 */

	if (setpriv(SEC_MAXIMUM_PRIV, maxprivs)==-1) {
	  switch(errno) {
	    case EPERM:
		Debug("setpriv (max) failed: EPERM\n");
		break;
	    case EINVAL:
		Debug("setpriv (max) failed: EINVAL\n");
		break;
	    case EFAULT:
		Debug("setpriv (max) failed: EFAULT\n");
		break;
	    default:
		Debug("setpriv (max) failed for unknown error: %d\n",errno);
		break;
	  }
	  audit_login(prpwd, pwd, verify->terminal,
		"Unable to set Kernel privs", ES_LOGIN_FAILED);
	  Debug("Unable to set Kernel privs (error %d): aborting\n",errno);
	  return 0;
	}

	if (setpriv(SEC_BASE_PRIV, bprivs)==-1) {
	  switch(errno) {
	    case EPERM:
		Debug("setpriv (base) failed: EPERM\n");
		break;
	    case EINVAL:
		Debug("setpriv (base) failed: EINVAL\n");
		break;
	    case EFAULT:
		Debug("setpriv (base) failed: EFAULT\n");
		break;
	    default:
		Debug("setpriv (base) failed for unknown error: %d\n",errno);
		break;
	  }
	  audit_login(prpwd, pwd, verify->terminal,
		"Unable to set base privs", ES_LOGIN_FAILED);
	  return 0;
	}

	if (setpriv(SEC_EFFECTIVE_PRIV, bprivs)==-1) {
	  switch(errno) {
	    case EPERM:
		Debug("setpriv (effective) failed: EPERM\n");
		break;
	    case EINVAL:
		Debug("setpriv (effective) failed: EINVAL\n");
		break;
	    case EFAULT:
		Debug("setpriv (effective) failed: EFAULT\n");
		break;
	    default:
		Debug("setpriv (effective) failed for unknown error: %d\n",
			errno);
		break;
	  }
	  audit_login(prpwd, pwd, verify->terminal,
		"Unable to set effective privs", ES_LOGIN_FAILED);
	  Debug("Unable to set effective privs (error %d): aborting\n",errno);
	  return 0;
	}
	return 1;

}


/* change the current process over to be owned by the user verify->uid.
 * Also properly set the privs, sec label, etc.
 * Also audits failures.
 * return=1 for success, 0 for fail.  A failure should be considered fatal.
 */
int 
change_to_user(struct verify_info *verify)
{
	struct pr_passwd *prpwd;
	struct passwd *pwd;
	int new_nice;

	prpwd = verify->prpwd;
	pwd = verify->pwd;

	Debug("change_to_user()\n");
	/* 1. set the login user id - settable only once */
	if (setluid(verify->uid)==-1) {
	  switch(errno) {
	    case EPERM:
		Debug("Unable to set luid - EPERM\n");
	  	audit_login(prpwd, pwd, verify->terminal, 
		    "Unable to set luid - insufficient privs", 
		    ES_LOGIN_FAILED);
		break;
	    case EINVAL:
		Debug("Unable to set luid - suspicious of pwd db.\n");
	  	audit_login(prpwd, pwd, verify->terminal, 
		    "Unable to set luid - out of range", ES_LOGIN_FAILED);
		break;
	    default:
		Debug("Can't set luid-Unknown error %d\n",errno);
	  	audit_login(prpwd, pwd, verify->terminal, 
		    "Unable to set luid-unknown error", ES_LOGIN_FAILED);
		break;
	  }
	  return 0;
	}

	/*
	 * Set the 'nice' priority if necessary.  Since the return value
	 * of nice(2) can normally be -1 from the documentation, and
	 * -1 is the error condition, we key off of errno, not the
	 * return value to find if the change were successful.
	 * Note we must do this before the setuid(2) below.
	 */
	errno = 0;
	prpwd = verify->prpwd;
	if (prpwd->uflg.fg_nice)
		new_nice = prpwd->ufld.fd_nice;
	else if (prpwd->sflg.fg_nice)
		new_nice = prpwd->sfld.fd_nice;

	if (prpwd->uflg.fg_nice || prpwd->sflg.fg_nice)  {
		(void) nice(new_nice);
		if (errno != 0)  {
			audit_login(prpwd, verify->pwd, NULL,
		    	     "bad 'nice' setting", ES_LOGIN_FAILED);
                        Debug("Bad priority setting.\n");
			return 0;
		}
	}


	/*  2. set the group(s) id and
	 *  3. set the regular user id */

#ifdef NGROUPS

	/* setgroups (verify->ngroups, verify->groups);
	*/
	if(setgid (verify->groups[0])) {
	  switch(errno) {
		case EPERM: 
			Debug("setgid EPERM\n");
			break;
		case EINVAL: 
			Debug("setgid EINVAL\n");
			break;
		default: 
			Debug("setgid unknown error: %d\n",errno);
			break;
	  }
	  return 0;
	}
	initgroups(verify->user_name, verify->groups[0]);
#else
	if(setgid (verify->gid)) {
	  switch(errno) {
		case EPERM: Debug("setgid EPERM\n");break;
		case EINVAL: Debug("setgid EINVAL\n");break;
		default: Debug("setgid unknown error\n");break;
	  }
	  return 0;
	}
#endif

	if(setuid (verify->uid)) {
	  switch(errno) {
		case EPERM: Debug("setgid EPERM\n");break;
		case EINVAL: Debug("setgid EINVAL\n");break;
		default: Debug("setgid unknown error\n");break;
	  }
	  return 0;
	}

	/* 4. set security clearance and label for the new process */
	if (!set_sec_label(verify)) 
		return 0;

	/* 5. set audit parameters */
	audit_adjust_mask(prpwd);

	/* 6. set privlege levels - maximum, base, and effective */
	if (!set_sec_privs(verify))
		return 0;

	return 1;
}


/* 
 * Try to read back everything, and print it.  If a fatal error occurs,
 * return code is 0.  1=success.
 */
int
dump_sec_debug_info(struct verify_info *verify)
{
	mand_ir_t *level_ir;
	priv_t privs[SEC_SPRIVVEC_SIZE];

	Debug ("luid: %d, real uid: %d, effective uid:%d,\n",
		getluid(),getuid(),geteuid());
	Debug ("real gid:%d, effective gid: %d\n", getgid(),getegid());
	level_ir = mand_alloc_ir();
	if (getclrnce(level_ir)==-1) {
	  switch(errno) {
		case EFAULT: Debug("getclrnce EFAULT\n");break;
		case EINVAL: Debug("getclrnce EINVAL\n");break;
		default: Debug("getclrnce unknown error:%d\n",errno);break;
	  }
	  return 0;
	}else Debug ("Clearance: %s\n", mand_ir_to_er(level_ir) );
	if (getslabel(level_ir)==-1) {
	  switch(errno) {
		case EFAULT: Debug("getslabel EFAULT\n");break;
		case EINVAL: Debug("getslabel EINVAL\n");break;
		default: Debug("getslabel unknown error:%d\n",errno);break;
	  }
	  return 0;
	}else Debug ("Level: %s\n", mand_ir_to_er(level_ir));
	mand_free_ir(level_ir);
	if(getpriv(SEC_MAXIMUM_PRIV, privs)==-1) {
	  switch(errno) {
		case EFAULT: Debug("getpriv max EFAULT\n");break;
		case EINVAL: Debug("getpriv max EINVAL\n");break;
		default: Debug("getpriv max unknown error:%d\n",errno);
			break;
	  }
	  return 0;
	}else Debug ("max priv: %x.%x\n", privs[0],privs[1]);
	if(getpriv(SEC_EFFECTIVE_PRIV, privs)==-1) {
	  switch(errno) {
		case EFAULT: Debug("getpriv eff EFAULT\n");break;
		case EINVAL: Debug("getpriv eff EINVAL\n");break;
		default: Debug("getpriv eff unknown error:%d\n",errno);
			break;
	  }
	  return 0;
	}else Debug ("eff priv: %x.%x\n", privs[0],privs[1]);
	if(getpriv(SEC_BASE_PRIV, privs)==-1) {
	  switch(errno) {
		case EFAULT: Debug("getpriv base EFAULT\n");break;
		case EINVAL: Debug("getpriv base EINVAL\n");break;
		default: Debug("getpriv base unknown error:%d\n",errno);
			break;
	  }
	  return 0;
	}else Debug ("base priv: %x.%x\n", privs[0],privs[1]);
	return 1;
}

/*
 * writeLoginInfo
 *	Input:	file name string (ex. $HOME/.dtlogininfo)
 *		verify structure with password stuff
 *  Write login information to a file to be displayed later, after a
 *  successful login.
 *
 *  Xsession will need to be modified something like this...
 *     DTHELLO="$DTDIR/bin/dthello -f /etc/copyright -f $HOME/.dtlogininfo"
 */
   
int
writeLoginInfo( char *filename, struct verify_info *verify)
{
	char *s1="Last   successful login: %s";
	char *s2="Last unsuccessful login: %s";
	char *s3;
	char s[80];
	char term[15];
	char *label;
	char *message="Sensitivity level for process: ";
	int i;
	int nl;
	time_t slogin, ulogin;
	char *slabel;
	char *uterminal, *sterminal;
	
	FILE *fp;
	
	Debug("Writing login info\n");
	if ((fp = fopen (filename, "w")) == 0 )
		return 0;

	if (verify->prpwd->uflg.fg_slogin)
		slogin=verify->prpwd->ufld.fd_slogin;
	else
		slogin=(time_t)0;

	if (verify->prpwd->uflg.fg_ulogin)
		ulogin=verify->prpwd->ufld.fd_ulogin;
	else
		ulogin=(time_t)0;

	if (verify->prpwd->uflg.fg_suctty)
		sterminal=verify->prpwd->ufld.fd_suctty;
	else
		sterminal="UNKNOWN";

	if (verify->prpwd->uflg.fg_unsuctty)
		uterminal=verify->prpwd->ufld.fd_unsuctty;
	else
		uterminal="UNKNOWN";

	slabel = mand_ir_to_er(verify->sec_label_ir);

	fprintf(fp,"-----------------------------------\n");
	fprintf(fp,"\nPrevious login information:\n\n");

	/* tricky formatting */
	if (slogin != 0) {
		sprintf(s, s1, ctime(&slogin));
		nl=strlen(s)-1;
		s[nl]='\0';  /* remove new-line */
	}else{
		sprintf(s,s1,"NEVER");
	}
	strcat(s, " from ");
	strncpy(term, sterminal, 14);
	term[14]='\0';
	strcat(s, term);
	fprintf(fp,"%s\n",s);

	if (ulogin != 0) {
		sprintf(s, s2, ctime(&ulogin));
		nl=strlen(s)-1;
		s[nl]='\0';  /* remove new-line */
	}else{
		sprintf(s,s2,"NEVER");
	}
	strcat(s, " from ");
	strncpy(term, uterminal, 14);
	term[14]='\0';
	strcat(s, term);
	fprintf(fp,"%s\n",s);

	label = (char*)malloc(strlen(message)+strlen(slabel)+1);
	sprintf(label, "%s%s", message, slabel);
	if (strlen (label) > 77) {
	    for(i=75; label[i]!=',' && i>0; i--);
	    if (i==0) for(i=75; label[i]!=' ' && i>0; i--);
	    if (i==0) i=75;
	    strncpy(s, label, i+1);
	    s[i+1]='\0';
	    fprintf(fp,"%s\n",s);
	    strncpy(s, &label[i+1], 75);
	    s[75]='\0';
	    fprintf(fp,"%s\n",s);
	}else{
	    fprintf(fp,"%s\n",label);
	}
	
	fclose(fp);
	return 1;
}

#endif	/* BLS */
