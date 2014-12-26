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
/* $XConsortium: account.c /main/6 1996/10/30 11:12:13 drk $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*
 *  account.c
 *
 *    routines for handling accounting
 *
 *    ex. utmp/wtmp/btmp, user-license restrictions,
 *
 */


/***************************************************************************
 *
 *  Includes
 *
 ***************************************************************************/


#ifdef __apollo
#include <X11/apollosys.h>        /* for pid_t struct in hp-ux sys/types.h */
#include	<sys/types.h>
#else
#include	<sys/types.h>
#include	<sys/stat.h>
#endif

#ifdef AIXV3
#include	<usersec.h>
#endif

#include	<fcntl.h>
#if defined(__FreeBSD__) && OSMAJORVERSION > 8
#include	<utmpx.h>
#else
#include	<utmp.h>
#endif

#include	<time.h>
#include	"dm.h"

#ifdef __PASSWD_ETC
#include	"rgy_base.h"
#endif


/***************************************************************************
 *
 *  External declarations
 *
 ***************************************************************************/
#if defined(AIXV3) && !defined(_POWER)
extern void
GetLoginInfo(struct display *d, int *loginType, char *ttyName, char **hostname);
#endif



/***************************************************************************
 *
 *  Procedure declarations
 *
 ***************************************************************************/
#if defined(AIXV3) && !defined(_POWER)
void dt_lastlogin ( char * user, struct lastlogin * llogin);
#endif




/***************************************************************************
 *
 *  Global variables
 *
 ***************************************************************************/

#ifdef __PASSWD_ETC
struct rtmp {
         char              rt_line[8];       /* tty name */
         rgy_$login_name_t rt_name;          /* user id (full SID) */
         char              rt_host[16];      /* hostname, if remote */
         long              rt_time;          /* time on */
};
#endif

#ifdef AIXV3
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


static struct lastlogin last_login;
#endif



/***************************************************************************
 *
 *  Account
 *
 *  update utmp/wtmp files.  
 ***************************************************************************/
void 
Account( struct display *d, char *user, char *line, pid_t pid,
#if NeedWidePrototypes
        int type,
#else
        short type,
#endif /* NeedWidePrototypes */
        waitType exitcode )
{
#if !defined(CSRG_BASED) /* we cannot do this on BSD ... */
    struct utmp utmp;		/* local struct for new entry	   	   */
    struct utmp *u;		/* pointer to entry in utmp file	   */
    int	fd;
    char	buf[32];
    char* user_str = user ? user : "NULL";
    char* line_str = line ? line : "NULL";
            
#ifdef __PASSWD_ETC
    struct rtmp rtmp;
    struct rtmp *r;
    int tty_slot;
    int rtmp_fd;
#endif

    if (d->utmpId == NULL) return;
    
    switch (type) {
    
    case INIT_PROCESS:	strcpy(buf, "INIT_PROCESS");	break;
    case LOGIN_PROCESS:	strcpy(buf, "LOGIN_PROCESS");	break;
    case USER_PROCESS:	strcpy(buf, "USER_PROCESS");	break;
    case DEAD_PROCESS:	strcpy(buf, "DEAD_PROCESS");	break;
    default:		strcpy(buf, "UNKNOWN");		break;
    }

    Debug("Account: id=%s, user=%s, line=%s, pid=%d, type=%s\n",
	   d->utmpId, user_str, line_str, pid, buf);

#ifdef PAM
    PamAccounting("dtlogin", d->name, d->utmpId, user, 
		        line, pid, type, exitcode);
#else
#   ifdef SUNAUTH
       solaris_accounting("dtlogin", d->name, d->utmpId, user, 
		           line, pid, type, exitcode);
#   endif
#endif

#ifdef sun
    return;
#else
    bzero(&utmp, sizeof(struct utmp));

    strncpy(utmp.ut_id, d->utmpId, sizeof(u->ut_id) - 1);
    utmp.ut_type = LOGIN_PROCESS;
    
    setutent();
    if ( (u = getutid(&utmp)) == NULL ) u = &utmp;

    /*
     *  make sure process ID's match if this is DEAD_PROCESS...
     *  don't update an already DEAD_PROCESS...
     */

    if ((type == DEAD_PROCESS && pid != 0 && u->ut_pid != pid) ||
        (type == DEAD_PROCESS && u->ut_type == DEAD_PROCESS)	) {

	endutent();
	return;
    }


    /*
     *  fill in required fields of utmp structure...
     *
     *  Note: for USER_PRCESS the "e_exit" field is overloaded to contain
     *        the method for counting this user. This is used later to
     *        determine if restricted user licenses have been exceeded.
     *	      Currently, an unlimited number of foreign displays can log in.
     */
     
    if (user) strncpy(u->ut_user, user, sizeof(u->ut_user));
    if (line) {
#ifdef _AIX
/*
      For AIX the Init process writes the exact mapped device name for console
      to the utmp file (like hft/0), if a getty on /dev/console record exists
      in the Inittab file.Hitherto, we need to have a similar logic to make
      sure for having the correct entry in the utmp file in order for the correct
      operation of the GettyRunning function. It should be noted that by having
      the correct value in the d->gettyLine field, the utmp file eventuallly
      updated by the Account function in dm.c will have the right value. And
      thus the GettyRunning function returns the appropriate value. So, it
      is important that the following logic be included here for AIX platform
      only.
      Raghu Krovvidi         07.06.93
 */

        if (!strcmp(line,"console")) {
             char *ttynm;
             int fd=0;

             fd = open("/dev/console",O_RDONLY);
             ttynm = ttyname(fd);
             ttynm += 5;
             strcpy(u->ut_line,ttynm);
             close(fd);
        }
	else
             strncpy(u->ut_line, line, sizeof(u->ut_line));
	     
#else
             strncpy(u->ut_line, line, sizeof(u->ut_line));
#endif
    }
    if (pid ) u->ut_pid = pid;
    if (type) {
	u->ut_type = type;
	if (type == DEAD_PROCESS) {
	    u->ut_exit.e_termination = waitSig(exitcode);
	    u->ut_exit.e_exit = waitCode(exitcode);
#ifndef SVR4
	    (void) memset((char *) u->ut_host, '\0', sizeof(u->ut_host));
#endif
	}

 	if (type == LOGIN_PROCESS && d->displayType.location != Local ) {
#ifndef SVR4
 	    strncpy(u->ut_host, d->name, sizeof(u->ut_host));
#endif
#ifdef __hpux
 	    u->ut_addr = 0;
#endif
 	}
 		    
	if (type == USER_PROCESS)
	    u->ut_exit.e_exit = (d->displayType.location == Local ? 1 : 0 );
    }	

    (void) time(&u->ut_time);

    /* 
     * write to utmp...  
     *
     * (Do not close utmp yet. If "u" points to the static structure, it is
     *  cleared upon close. This does not bode well for the following write
     *  to wtmp!)
     */

    pututline(u);


    /*
     *  write the same entry to wtmp...
     */

    if ((fd = open(WTMP_FILE, O_WRONLY | O_APPEND)) >= 0) {
	if(-1 == write(fd, u, sizeof(utmp))) {
          perror(strerror(errno));
        }
	close(fd);
    }


    /*
     *  close utmp...
     */
     
    endutent();

#ifdef __PASSWD_ETC
    /* Now fill in the "rgy utmp" struct */
    if (line) strncpy(rtmp.rt_line, u->ut_line, sizeof(u->ut_line));
    bzero(rtmp.rt_host, sizeof(rtmp.rt_host));
    rtmp.rt_time = u->ut_time;
    r = &rtmp;

    /* Write entry to rtmp */
    tty_slot = ttyslot();

    if (tty_slot > 0 && (rtmp_fd = open("/etc/rtmp", O_WRONLY|O_CREAT, 0644)) >= 0) {
         lseek(rtmp_fd, (long) (tty_slot * sizeof(struct rtmp)), 0);
         write(rtmp_fd, (char *) r, sizeof(struct rtmp));
         close(rtmp_fd);
    }
#endif
#if defined(AIXV3) && !defined(_POWER)
	/* Log the lastlogin data ..    RK  09.13.93  */
	/** in AIX 4.1 this is taken care of during authentication **/
    if(type == USER_PROCESS) {
	int loginType;
	char tempTtyName[128];
	char *hostname;

	GetLoginInfo(d, &loginType, tempTtyName, &hostname);
	time(&last_login.stime);

	if(line) {
		Debug("tty_last_login is (line=%s)\n",line);
		last_login.stty = (char *)malloc(strlen(line) + 1);
		strcpy(last_login.stty,line);
	} else {
	    last_login.stty = (char *)malloc(strlen(tempTtyName) + 1);
	    strcpy(last_login.stty,tempTtyName);
	}

        last_login.shost = (char *) malloc (MAXHOSTNAMELEN);
	if (hostname == NULL) {
            gethostname (last_login.shost , MAXHOSTNAMELEN);
	} else {
	    strncpy(last_login.shost, hostname, MAXHOSTNAMELEN);
	    last_login.shost[MAXHOSTNAMELEN -1] = '\0';
	}

	Debug("logging lastlogin entry (user=%s)\n",user);
	dt_lastlogin(user,&last_login); 
	free(last_login.stty);
	free(last_login.shost);
    }
#endif

#endif /* !sun */
#endif /* !CSRG_BASED */
}




/***************************************************************************
 *
 *  UtmpIdOpen
 *
 *  see if a particular utmp ID is available
 *
 * return codes:  0 = ID is in use
 *		  1 = ID is open 
 ***************************************************************************/

int 
UtmpIdOpen( char *utmpId )
{
    struct utmp *u;		/* pointer to entry in utmp file	   */
    int    status = 1;		/* return code				   */

#if !defined(CSRG_BASED)
    while ( (u = getutent()) != NULL ) {

	if ( (strncmp(u->ut_id, utmpId, 4) == 0 ) &&
	     u->ut_type != DEAD_PROCESS ) {

	    status = 0;
	    break;
	}
    }
    
    endutent();
#endif
    return (status);
}



#if defined(AIXV3) && !defined(_POWER)
void dt_lastlogin ( user, llogin)
char * user;
struct lastlogin * llogin;
{
	char *tmp_char;
	char *tmp_int;
	/*
	 * We are loading all the lastlogin info into a struct and then dealing 
	 * with that so if the technique of storing the values is redone it 
	 * will be easy
	 */
	/* set id back to root */
	seteuid(0);
		/*
		 * Open up the user data base
		 */
		setuserdb(S_READ | S_WRITE);

		/*
		 * setting new unsuccessful login attempts times
		 */
		if (llogin->ftime) {
			/*
			 * Get the old Failure count and increment it
			 */
			if (getuserattr(user, S_ULOGCNT, &tmp_int, SEC_INT) != 0)
				llogin->fcount = 0;
			else
				llogin->fcount = (int)tmp_int;

			/*
			 * put all the new data
			 */
			putuserattr(user, S_ULASTTTY, llogin->ftty, SEC_CHAR);
			llogin->fcount++;
			putuserattr(user, S_ULOGCNT, (void *)llogin->fcount,
				    SEC_INT);
			putuserattr(user, S_ULASTTIME, (void *)llogin->ftime,
				    SEC_INT);
			putuserattr(user, S_ULASTHOST, llogin->fhost, SEC_CHAR);
			putuserattr(user, NULL, NULL, SEC_COMMIT);

			/*
			 * Close data base and zero out indicator fields
			 */
			llogin->ftime = 0;
			llogin->fcount = 0;
		}

		/*
		 * New Successful login data
		 */
		if (llogin->stime) {
			putuserattr(user, S_LASTTIME, (void *)llogin->stime,
				    SEC_INT);
			putuserattr(user, S_LASTTTY, llogin->stty, SEC_CHAR);
			Debug("hostname logged is %s\n",llogin->shost);
			putuserattr(user, S_LASTHOST, llogin->shost, SEC_CHAR);
			putuserattr(user, S_ULOGCNT, 0, SEC_INT);
			putuserattr(user, NULL, NULL, SEC_COMMIT);
		}
	enduserdb();
}
#endif

