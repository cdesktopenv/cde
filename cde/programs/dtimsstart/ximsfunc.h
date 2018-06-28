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
/* @(#)$XConsortium: ximsfunc.h /main/2 1996/05/07 14:02:55 drk $ */

#ifndef	_XIMSFUNC_H_
#define	_XIMSFUNC_H_	1


    /* External Functions */

	/* main.c */
extern	void	sigExit(/* sig */);
extern	void	Exit(/* err_code */);
extern	int	NotifyErrCode(/* err_code */);
extern	int	ErrFilePathNeeded(/* error */);
extern	int	InWaitingState(/*  */);
extern	void	ximsMain(/*  */);
extern	void	ximsFinish(/*  */);
extern	char	*xims_errmsg(/* err_num, arg1, arg2, arg3 */);

	/* env.c */
extern	int	set_cmd_env(/*  */);
extern	int	get_user_environ(/*  */);
extern	int	expand_string(/* in_str, out_str, out_len, ims */);
extern	int	make_new_environ(/* oenv, sel */);
extern	int	put_new_environ(/* oenv */);
extern	int	set_remote_env(/* ptr, env_pass */);
extern	char	*xhp_xinput_name(/* locale */);
extern	char	*get_real_locale(/* locale, aliases */);

	/* select.c */
extern	void	ximsSelect(/*  */);
extern	int	update_user_selection(/* sel, list, idx, host, host_type */);
extern	int	get_ims_idx(/* list, name */);
extern	int	set_ims_status(/* ent */);
extern	int	get_ims_list(/* listp, locale, fill_ent */);

	/* start.c */
extern	void	ximsStart(/*  */);
extern	void	ximsWait(/*  */);
extern	void	ximsWaitDone(/*  */);
extern	int	is_waiting(/*  */);
extern	void	set_sig_chld(/* enable */);
extern	int	im_mod_available(/* renv */);
extern	int	mk_ims_option(/* ptr, sel */);
extern	int	load_resources(/*  */);
extern	int	restore_resources(/*  */);

	/* remote.c */
extern	int	put_remote_conf(/* locale, ims_name */);
extern	int	get_remote_conf(/* listp, hostname, locale, ims_name */);
extern	int	exec_remote_ims(/* sel */);
extern	int	check_hostname(/* hostname */);
extern	int	set_remote_confdata(/* confbuf, conflen */);
extern	int	read_remote_confdata(/* confbuf, conflen */);
extern	int	get_window_status(/*  */);
extern	int	change_window_status(/* status */);
extern	int	set_window_data(/* ac, av */);
extern	int	get_window_data(/* acp, avp */);

	/* action.c */
extern	int	invoke_action(/* action, host */);
extern	void	send_dtims_msg(/* msg, errcode */);
extern	void	dtims_msg_proc(/* w, cd, event, continue_dispatch */);

	/* file.c */
extern	int	create_xims_dir(/*  */);
extern	int	init_log_file(/* org_path, check_size */);
extern	int	set_errorlog(/* path */);
extern	int	read_cmd_conf(/*  */);
extern	int	expand_cmd_conf(/*  */);
extern	int	read_imsconf(/* conf, ims_name, ims_fname */);
extern	int	check_ims_conf(/* ims, ims_name */);
extern	int	read_localeconf(/* list, locale_name */);
extern	int	read_user_selection(/* fselp, locale_name */);
extern	int	read_selection_file(/* fsel, fp */);
extern	int	save_user_selection(/* sel, locale_name */);
extern	int	get_select_mode(/*  */);
extern	int	set_select_mode(/* cur_mode, new_mode */);
extern	int	parse_protolist(/* valp */);
extern	int	default_protocol(/* conf */);

	/* win.c */
extern	int	open_display(/*  */);
extern	void	close_display(/*  */);
extern	int	window_env_ok(/*  */);
extern	int	init_window_env(/*  */);
extern	void	end_window_env(/*  */);
extern	int	clear_cmd_property(/* win */);
extern	int	save_RM(/*  */);
extern	int	merge_RM(/* res1, res2 */);
extern	int	restore_RM(/*  */);
extern	int	start_selection_window(/*  */);
extern	int	start_mode_window(/* cur_mode */);
extern	void	stop_help(/*  */);
extern	void	ximsHelp(/* help_type */);
extern	int	put_msg_win(/* type, msg */);
extern	void	xevent_loop(/*  */);
extern	void	xt_start_waiting(/*  */);
extern	void	xt_stop_waiting(/*  */);

	/* util.c */
		/* string */
extern	char	*strcpyx(/* dest, src */);
extern	char	*strcpy2(/* dest, src1, src2 */);
extern	char	*newstradded(/* src1, src2, src3 */);
extern	int	str_to_int(/* ptr, val */);
extern	bool	str_to_bool(char *ptr, bool def_val);
extern	char	*trim_line(/* ptr */);
extern	char	**parse_strlist(register char *ptr, char sep_ch);
extern	int	pack_strlist(/* ptr, listp, sep_ch */);
extern	void	free_strlist(/* pp */);
extern	bool	parse_dpy_str(/* display_str, host, dnum, snum, dnet */);
extern	char	*std_dpy_str(/* display_str, snum */);
		/* file & dir */
extern	int	make_user_dir(/* path */);
extern	char	*dirname(/* path */);
extern	int	is_directory(/* path, must_writable */);
extern	int	is_regularfile(/* path */);
extern	int	is_emptyfile(/* path */);
extern	int	is_executable(/* path */);
extern	int	is_writable(/* path */);
extern	int	is_readable(/* path, allow_empty */);
		/* file read */
extern	int	start_tag_line(/* fname */);
extern	int	read_tag_line(/* fp, tagp, valp */);
		/* error messges */
extern	void	put_xims_msg(/* msg_type, err_num, arg1, arg2, arg3 */);
extern	void	put_xims_errmsg(/* err_num, arg1, arg2, arg3 */);
extern	void	put_xims_warnmsg(/* err_num, arg1, arg2, arg3 */);
extern	void	print_msg(/* fmt, arg1, arg2, arg3, arg4, arg5 */);
extern	int	put_xims_log(/* fmt, arg1, arg2, arg3 */);
		/* clear structure */
extern	void	clear_ImsConf(/* ims */);
extern	void	clear_ImsEnt(/* ent */);
extern	void	clear_ImsList(/* list */);
extern	void	clear_FileSel(/* fsel */);
extern	void	clear_UserSelection(/* sel */);
extern	void	clear_RunEnv(/* renv */);
extern	void	clear_OutEnv(/* oenv */);
extern	void	clear_UserEnv(/*  */);
extern	void	clear_CmdConf(/*  */);
extern	void	clear_WinEnv(/*  */);
extern	void	clear_CmdOpt(/*  */);
extern	void	clear_All(/*  */);
# ifdef	DEBUG
		/* print structure */
extern	void	pr_FileSel(/* fsel */);
extern	void	pr_UserSelection(/* sel */);
extern	void	pr_ImsConf(/* conf, ims_name */);
extern	void	pr_ImsEnt(/* ent, idx */);
extern	void	pr_ImsList(/* list */);
extern	void	pr_WinEnv(/* wenv */);
extern	void	pr_RunEnv(/* renv */);
extern	void	pr_UserEnv(/*  */);
extern	void	pr_OutEnv(/* oenv */);
extern	void	pr_CmdConf(/*  */);
extern	void	pr_CmdOpt(/*  */);
extern	void	pr_OpModeFlag(/*  */);
extern	char	*StateName(/*  */);
extern	char	*error_name(/* error */);
extern	char	*proto_name(/* proto_idx */);
extern	char	*sig_name(/* sig */);
extern	void	pr_brk(/* msg */);
# endif	/* DEBUG */


    /* Macros */
	/* memory */
#define	CLR(p, st)		memset((void *) p, 0, sizeof(st))
#define	COPY(d, s, n, st)	memcpy((void *)(d), (void *)(s), (n) * sizeof(st))

# ifdef	DEBUG_MEM
void	*_mp_;

#define	ALLOC(n, st)		(_mp_ = XtCalloc(n, sizeof(st)), \
    print_msg("ALLOC(%d, " #st "): _mp_=%p sz=%d\n", n, _mp_, sizeof(st)), (st *) _mp_)
#define	REALLOC(p, n, st)	( _mp = XtRealloc((char *) (p), (n) * sizeof(st)), \
    print_msg("REALLOC(%p, %d, " #st "): _mp_=%p\n", p, n, _mp_), (st *) _mp_)
#define	NEWSTR(s)		(_mp_ = (s) && *(s) ? XtNewString(s) : NULL, \
    print_msg("NEWSTR(%s): _mp_=%p\n", s, _mp_), (char *) _mp_)
#define	FREE(p)			(print_msg("FREE(%p)\n", p), XtFree((char *) p), (p) = 0)
#define	RENEWSTR(p, s)		(_mp_ = (s) && *(s) ? XtNewString(s) : NULL, \
    print_msg("RENEWSTR(" #p "): %p(%s) => %p(%s)\n", p, p, _mp_, s), \
    XtFree((char *) p), (p) = (char *) _mp_)
# else	/* DEBUG_MEM */
#define	ALLOC(n, st)		(st *) XtCalloc(n, sizeof(st))
#define	REALLOC(p, n, st)	(st *) XtRealloc((char *) (p), (n) * sizeof(st))
#define	NEWSTR(s)		((s) && *(s)) ? XtNewString(s) : NULL
#define	FREE(p)			XtFree((char *) p), (p) = 0
#define	RENEWSTR(p, s)		XtFree((char *) p), (p) = NEWSTR(s)
# endif	/* DEBUG_MEM */

#define	FREE_LIST(pp)		free_strlist(pp), (pp) = 0

	/* string */
#define	is_default_name(p)	(*(p) == NAME_DEFAULT_CHAR)
#define	to_upper_ch(c)		((c) & ~0x20)
#define	to_lower_ch(c)		((c) | 0x20)
#define	to_upper_str(p)		while (*(p)) 	*(p)++ &= ~0x20
#define	to_lower_str(p)		while (*(p)) 	*(p)++ |= 0x20
#define	is_comment_char(c)	((c) == COMMENT_CHAR || (c) == COMMENT_CHAR2)
#define	is_white(c)		((c) == ' ' || (c) == '\t' || (c) == '\n')
#define	skip_white(p)		while (is_white(*(p)))	(p)++
#define	next_field(p)	\
	for ((p)++; *(p) && !is_white(*(p)); (p)++) ; skip_white(p)
#define	cut_field(p)	\
	for ((p)++; *(p) && !is_white(*(p)); (p)++) ; *(p)++ = 0; skip_white(p)

	/* misc */
#define Max(a, b)	((a) < (b) ? (b) : (a))
#define Min(a, b)	((a) > (b) ? (b) : (a))

	/* for DEBUG */
# ifdef	DEBUG
#define	DPR(msg)		if (DebugLvl >= 1)	print_msg msg
#define	DPR2(msg)		if (DebugLvl >= 2)	print_msg msg
#define	DPR3(msg)		if (DebugLvl >= 3)	print_msg msg
# else
#define	DPR(msg)
#define	DPR2(msg)
#define	DPR3(msg)
# endif	/* DEBUG */

#endif	/* _XIMSFUNC_H_ */
