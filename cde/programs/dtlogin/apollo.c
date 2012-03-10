/* $XConsortium: apollo.c /main/4 1995/10/27 16:11:14 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        apollo.c
 **
 **   Project:     HP Visual User Environment (DT)
 **
 **   Description: Dtlogin user setup routines for Domain/OS 10.4
 **
 **                These routines set up the user's state before starting
 **		   the DT session.
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


#include	<stdio.h>		/* just so file isn't empty	   */

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

#include	"dm.h"
#include	"vgmsg.h"

      
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




/***************************************************************************
 *
 *  Global variables
 *
 ***************************************************************************/




/***************************************************************************
 *
 *  Stub routines
 *
 ***************************************************************************/




/***************************************************************************
 *
 *  DoLogin
 *
 *  set up user environment
 ***************************************************************************/

int
DoLogin( char *user, char *passwd, char *host )
{
	ios_$id_t		logid;
	login_$opt_set_t	opts;
		status_$t		status;
        error_$string_t         subsystem, module, error;
        short                   sslen, modlen, errlen;
	char			buf[BUFSIZ];
      
	Debug("DoLogin(): for %s\n", user);
	login_$set_host(host, strlen(host));

	opts = login_$no_prompt_name_sm | 
	       login_$no_prompt_pass_sm | 
	       login_$addut_sm		|
	       login_$chown_tty_sm;

	Debug("DoLogin(): before calling login_$chk_login()\n");
	if ((!login_$chk_login(opts, STRING(user), STRING(passwd),
	                       (login_$open_log_p) NULL, STRING(""),
			       &logid, &status)) 
	    && (status.all != status_$ok)) {

	    Debug("DoLogin(): login_$chk_login() failed\n");
       	    error_$get_text(status, subsystem, &sslen, module, &modlen, 
	    		    error, &errlen);
            STRNULL(error, errlen);
	    STRNULL(module, modlen);
       	    STRNULL(subsystem, sslen);
            sprintf(buf,ReadCatalog(MC_LOG_SET,MC_LOG_DOMAIN,MC_DEF_LOG_DOMAIN),
		error, subsystem, module);
	    LogError (buf);

	    bzero (passwd, strlen(passwd)); 

	    return (false);
	}

	Debug("DoLogin(): login_$chk_login() succeeded\n");

	/* 
	 * clear password...
	 */
	bzero (passwd, strlen(passwd)); 

	return (true);
}     
#endif
