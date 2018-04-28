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
/* $XConsortium: login.h /main/3 1995/10/27 16:19:16 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*  LOGIN.INS.C, /us/ins, rod, 12/09/88 */

/* changes:
    12/21/88 gilbert Modifications for C++.
    12/09/88 betsy  add login_$logins_disabled 
    04/19/88 betsy  cast values for login_$opt_set_t  ( to be consistent )
    03/30/88 betsy  add login_$set_host
    03/21/88 betsy  add options login_$addut & login_$chown_tty
	01/04/88 brian	Fixed param decl of login_$chk_login
	12/31/87 brian	Converted to ANSII from /us/ins/login.ins.c/6
*/

#define login_$mod 2

#define login_$err_shut        0x07020001  /*user wants to caller to shut down*/
#define login_$err_exit        0x07020002  /*user wants to caller to exit*/
#define login_$no_room         0x07020003  /*no room for rep at open */
#define login_$bad_passwd      0x07020004  /*passwd check failed */
#define login_$inv_acct        0x07020005  /* account expired */
#define login_$logins_disabled 0x07020006  /* logins disabled */

#define login_$log_stream_index 4    /* 4th stream in streams array is the open log */


typedef void *login_$ptr;

typedef short enum { login_$read, login_$update } login_$mode_t;

typedef short enum {
    login_$log_events,      /* log all events */
    login_$log_inv_only,    /* log only unsuccessful attempts to log in */
    login_$no_passwd,       /* don't require password (!) */
    login_$no_setsid,       /* implies no_setpm, but allows set of wd/nd */
    login_$no_setpm,        /* not done unless setsid successfully done */
    login_$no_setwd,        /* not done if setsid attempted and failed */
    login_$fail_ret,        /* return if failure logged */
    login_$no_prompt_name,  /* no prompt for name */
    login_$no_prompt_pass,  /* no prompt for password */
    login_$preserve_sub,    /* preserve subsystem */
    login_$leave_log_open,  /* leave log file open on return */
    login_$addut,           /* add utmp/wtmp/lastlog entry */
    login_$chown_tty,       /* chown the login device */
    login_$extra14,         /* pad to 32-bit set */
    login_$extra15, 
    login_$extra16, 
    login_$extra17          /* just over the border */
} login_$opt_t;

typedef long login_$opt_set_t;
#define login_$log_events_sm      ((login_$opt_set_t) (1 << (int) login_$log_events))
#define login_$log_inv_only_sm    ((login_$opt_set_t) (1 << (int) login_$log_inv_only))
#define login_$no_passwd_sm       ((login_$opt_set_t) (1 << (int) login_$no_passwd))
#define login_$no_setsid_sm       ((login_$opt_set_t) (1 << (int) login_$no_setsid))
#define login_$no_setpm_sm        ((login_$opt_set_t) (1 << (int) login_$no_setpm))
#define login_$no_setwd_sm        ((login_$opt_set_t) (1 << (int) login_$no_setwd))
#define login_$fail_ret_sm        ((login_$opt_set_t) (1 << (int) login_$fail_ret))
#define login_$no_prompt_name_sm  ((login_$opt_set_t) (1 << (int) login_$no_prompt_name))
#define login_$no_prompt_pass_sm  ((login_$opt_set_t) (1 << (int) login_$no_prompt_pass))
#define login_$preserve_sub_sm    ((login_$opt_set_t) (1 << (int) login_$preserve_sub))
#define login_$leave_log_open_sm  ((login_$opt_set_t) (1 << (int) login_$leave_log_open))
#define login_$addut_sm           ((login_$opt_set_t) (1 << (int) login_$addut))
#define login_$chown_tty_sm       ((login_$opt_set_t) (1 << (int) login_$chown_tty))

typedef char login_$string_t[256];
typedef ios_$id_t login_$streams_t[4];

typedef char (*login_$open_log_p)(
		char		* log_file,
		short		& log_flen,
		stream_$id_t    * logstr
);

typedef struct {
        int			(*help)(stream_$id_t	& stream);
        login_$open_log_p	open_log;
	/* A note to you:                                                     aug-87
         These next two routines are here only for callers that cannot provide streams
         for their i/o (i.e. the dm. It was just too hard, esp since he is 'going away'.)
         To have them used, pass the number -2 as your first stream in login_$streams_t.
         A bit of a hack, true, but as long as we all understand each other, it will work.
         Login_$login promises NOT to ref these routines if you pass real streams. (Note
         that the fillbuf stuff is gone (since changing home dir is no longer supported.)
	*/ 
        short 			(*pread)(
					stream_$id_t	& stream,
					char		* inbuf,
					short		& inlen,
					char		* pstr,
					short		& plen,
					boolean		& echo,
					char		* fillbuf, /* pre-fill buffer with string */
					short		& fillbuflen
				);
        int			(*pwrite)(
					stream_$id_t	& stream,
					char		* pstr,
					short		& plen
				);
} login_$proc_rec_t;


/*  Required for C++ V2.0  */
#ifdef  __cplusplus
    extern "C" {
#endif

/* FULL_LOGIN --- performs the login function with prompting, implementing system policy,
            setting wd, nd, pm_$sid_text, pm_$home_txt and default shell (unless 
            already set), prints /etc/dmmsg if exists.  Supports 'shut, 'exit' and 
            'help' and all options EXCEPT login_$no_setsid.  Takes optional strings
            for pre-read user name [passwd]. */
extern boolean 
login_$full_login(
	login_$opt_set_t	& opts,
	login_$streams_t	* streams,	/* in, out, err, log_stream  */
	char			* login_name,	/* login string - iff login_$no_prompt_name */
	short			& login_nlen,
	char			* pass,		/* passwd string - iff login_$no_prompt_pass */
	short			& passlen,
	login_$proc_rec_t	& ioprocs,	/* procedure vectors */
	char			* log_file,	/* used iff login_$log_events/inv in opts */
	short			& log_flen,
	status_$t		* status
);

/* CHK_LOGIN --- performs the login function based on the strings passed in with no 
                prompting.  DOES NOT fully implement system policy. i.e. does nothing
                if passwd needs to be changed, but returns bad status if expired or
                invalid accounts.  Gives no error msgs. */
extern boolean
login_$chk_login (
	login_$opt_set_t	& opts,
	char			* login_name,	/* login string - iff login_$no_prompt_name */
	short			& login_nlen,
	char			* pass,		/* passwd string - iff login_$no_prompt_pass */
	short			& passlen,
	login_$open_log_p	& open_log,	/* used iff login_$log_events/inv in opts */
	char			* log_file,	/* used iff login_$log_events/inv in opts */
	short			& log_flen,	/* used iff login_$log_events/inv in opts */
	ios_$id_t		* log_stream,	/* stream for open log file iff login_$leave_log_open */
	status_$t		* status
);

/*-----------------------------------------------------------------------------
  LOGIN_$SET_HOST - sets the host name to be used in the addut on a subsequent 
                    login_$full_login or login_$chk_login call                 */
extern void 
login_$set_host(
   char        * host_name,  /* max len is 64 */
   int         & host_len
);

/* O P E N --- prepare to do login type stuff */
extern void
login_$open(
	login_$mode_t	& lmode,
	login_$ptr	* lptr,
	status_$t	* st
);

/* S E T _ P P O -- set the PPO to operate on 
   if ppo length is zero, use currently logged in user's PPO
   if set_ppo not call before chpass, ckpass or chhdir called, one
   will automatically be done (for current user)
*/
extern void
login_$set_ppo(
	login_$ptr	& lptr,
	char		* ppo,
	short		& plen,
	status_$t	* st
);

/* C K P A S S -- check password 
   checks the supplied password against that of the set PPO 
*/        
extern void
login_$ckpass(
	login_$ptr	& lptr,
	char		* pass,
	short		& plen,
	status_$t	* st
);                              

/* C H P A S S --- change password in the registry */
extern void 
login_$chpass(
	login_$ptr	& lptr,
	char		* pass,
	short		& plen,
	status_$t	* st
);

/* C H H D I R --- change home directory in the registry */
extern void 
login_$chhdir(
	login_$ptr	& lptr,
	char		* hdir,
	short		& hlen,
	status_$t	* st
);

/* E R R _ C O N T E X T --- file name that had problems */
extern void 
login_$err_context(
	login_$ptr	& lptr,
	status_$t	* errst,
	char		* badf,
	short		* blen,
	status_$t	* st
);

/* C L O S E --- release all acquired resources */
    /* NOTE --- call even if open FAILS! */
extern void 
login_$close(
	login_$ptr	& lptr,
	status_$t	* st
);


#ifdef  __cplusplus
    }
#endif

