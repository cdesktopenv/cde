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
/*
 *      $XConsortium: cgen_utils.c /main/7 1996/10/02 10:56:38 drk $
 *
 *      @(#)cgen_utils.c	1.6 01 Jun 1994
 *
 *      RESTRICTED CONFIDENTIAL INFORMATION:
 *
 *      The information in this document is subject to special
 *      restrictions in a confidential disclosure agreement between
 *      HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *      document outside HP, IBM, Sun, USL, SCO, or Univel without
 *      Sun's specific written approval.  This document and all copies
 *      and derivative works thereof must be returned or destroyed at
 *      Sun's request.
 *
 *      Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */


/*
 * cgen_utils.c - utility functions for Code Generator Window and
 *		  property sheet.
 */

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif

#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <assert.h>
#include <sys/param.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <Xm/Xm.h>
#include <Xm/List.h>
#include <Dt/TermPrim.h>
#include <Dt/Term.h>

#include <ab_private/ab.h>
#include <ab_private/abobj_set.h>
#include <ab_private/proj.h>
#include <ab_private/cgen.h>
#include <ab_private/obj_notify.h>
#include <ab_private/util.h>
#include <ab_private/trav.h>
#include <ab_private/strlist.h>

#include "dtbuilder.h"
#include "cgen_win_ui.h"
#include "palette_ui.h"
#include "cgen_props_ui.h"


#define INVALID_PID 		((pid_t)-1)
#define EXIT_SLEEP_SECONDS	(10)		/* see subprocess_exit() */
#define CHILD_PID_LIST_SIZE	EXIT_SLEEP_SECONDS

/* WARNING: If more dtcodegen options are added, then
 * 	    MAX_CGEN_FIXED_ARGS must be incremented!
 *
 * MAX_CGEN_FIXED_ARGS = dtcodegen, -changed, -nomerge, -v or -s,
 * -main, -p, <project_name>, NULL, +2(extra) = 10
 */
#define MAX_CGEN_FIXED_ARGS	10

typedef enum
{
    CG_STATUS_UNDEF = 0,
    CG_STATUS_ERROR,
    CG_STATUS_EXITED,
    CG_STATUS_SIGNALLED,
    CG_STATUS_STARTED,
    CG_STATUS_NUM_VALUES
} CG_STATUS;


CGenOptions		CodeGenOptions;
StringList	    user_env_vars = NULL;
StringList	    module_list = NULL;
static CG_GOAL      user_goal = CG_GOAL_UNDEF;

static XtInputId    input_proc_id = (XtInputId)-1;
static int          status_pipe_read = -1;
static int          status_pipe_write = -1;
static Widget       output_termWidget = NULL;
static Widget       input_termWidget = NULL;
static Widget       make_run_item = NULL;
static Widget       gen_code_item = NULL;
static Widget       make_item = NULL;
static Widget       run_item = NULL;
static Widget       abort_item = NULL;
static Widget	    cgen_props_item = NULL;
static Widget       gen_code_button = NULL;
static Widget	    make_run_button = NULL;
static Widget	    make_button = NULL;
static Widget	    run_button = NULL;
static Widget	    abort_button = NULL;
static Widget	    cur_dir_text = NULL;
static ISTRING      termSlaveDeviceName = NULL;
static pid_t		child_pid_list[CHILD_PID_LIST_SIZE];
static pid_t		actual_process_pgid = INVALID_PID;
static pid_t		abortingPID = INVALID_PID;

/*
 * Places to find various commands
 */
static STRING	dtcodegenCmdList[] = 
{
    "",				/* exe-dir filled in at run time */
    "",				/* exe-dir/../abmf : filled in at runtime */
    "dtcodegen", 
    "/usr/dt/bin/dtcodegen", 
    NULL
};

static STRING	makeCmdList[] =
{
    "make",
    "/usr/ccs/bin/make",
    "/usr/dist/exe/make",
    "/usr/dist/local/exe/make",
    NULL
};


static BOOL	cgen_inited = FALSE;
int		cgen_init(void);
#define cgen_check_init() (cgen_inited? 0:cgen_init())

static int	util_fdsync(int fd);		/* REMIND: move to libAButil*/
static int      print_to_term(STRING msg);
static int      send_output_to_term(void);
static int      term_execute_command(CG_SUBCOMMAND cmd_code, STRING cmd, STRING argv[]);
static int      get_slave_device_name(void);
static int	print_failure_message(CG_SUBCOMMAND cmd_code, int exit_code);
static int	print_internal_err_message(void);
static int	print_success_message(void);
static int	print_cmd_not_found_message(STRING cmd);
static int	print_death_message(void);
static int	print_exit_message(int exit_code);
static int	print_abort_message(void);
static int	create_status_pipe(void);
static int	destroy_status_pipe(void);
static int	write_to_status_pipe(
			CG_STATUS	status_code, 
			CG_SUBCOMMAND	cmd_code, 
			void		*status_data	
		);
static int	read_from_status_pipe(
			CG_STATUS	*status_code_out,
			CG_SUBCOMMAND	*cmd_code_out,
			void		**status_data_out
		);
static int	careful_kill_group(pid_t pgid);
static int	goto_ready_state(void);
static int	goto_busy_state(void);
static int	cgenP_abort(void);
static int	cgen_set_title(STRING projectName);
static int	cgen_set_project_dir(STRING dir);
static int	cgen_obj_name_changed_cb(ObjEvAttChangeInfo evInfo);
static void	cgen_abort_at_exit(void);
static STRING 	cgenP_get_env_var(STRING varName);
static int	cgenP_put_env_var(STRING varName, STRING varValue);
static int      set_props_proj_name(STRING);
static int      obj_renamedOCB(ObjEvAttChangeInfo);
static int      obj_destroyedOCB(ObjEvDestroyInfo);
static int      obj_updateOCB(ObjEvUpdateInfo info);
static int	do_user_action(
			CG_GOAL		goal,
			CG_SUBCOMMAND	cmd
		);
static int	build_dtcodegen_cmd_list(STRING *cmdList);
static int	build_dtcodegen_arg_list(STRING *argList, int *iInOut);
static int 	check_path(void);
static int 	check_path_to_cmd(STRING *cmdList, BOOL *allowWarnUserInOut);
static int	check_makefile(BOOL *continueOut);
static int 	cgenP_makefile_is_for_project(STRING fileName, ABObj project);
static BOOL	strings_exist_in_file(StringList strings, FILE *file);
static int 	destroy_makefile(void);
static int 	destroy_links_to_file(STRING fileName);
static int	move_file_to_backup(STRING fileName);
static int 	add_obj_file_name(
			StringList	fileNames, 
			ABObj		obj, 
			STRING		suffix
		);
static BOOL	all_files_exist(ABObj project);
static int 	select_command(STRING *cmdList, STRING *cmdOut);
static BOOL 	command_exists(STRING cmd, STRING path);
static int	save_done_cb(int status);
static int	add_child_to_list(pid_t child_pid);
static int	wait_for_child(void);
static STRING	cvt_type_to_ident(
			STRING type, 
			STRING identBuf, 
			int identBufSize
		);
static Boolean	path_is_executable(
    			char 	*path,
    			uid_t	euid,
    			gid_t 	egid
		);

static void     popdown_cgen_window(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );

static void 	pipe_data_ready_proc(
		    XtPointer	client_data,
		    int		*fid,
		    XtInputId	*id
		);

static int	subprocess_exit(int exit_code);

static int          exec_generate_code(void);
static int          exec_make(void);
static int          exec_run(void);

static int          exec_generate_proj(void);
static int          exec_generate_main(void);
static int          exec_generate_specific_files(void);
static int          exec_generate_specific_files_and_main(void);

#define util_fdclose(fd) \
	((fd) < 0? \
	    0 \
	: \
	    ((close(fd) == 0)?  \
		(((fd) = -1),0) \
	    : \
		-1)  \
        )

/*
 * Exit cmd_code and exit_code are the command that was previously run
 * and its exit status.
 */
static int	exec_next_command(
			CG_SUBCOMMAND cmd_code, int exit_code);
static int	exec_next_command_for_gen_code(
			CG_SUBCOMMAND cmd_code, int exit_code);
static int	exec_next_command_for_make(
			CG_SUBCOMMAND cmd_code, int exit_code);
static int	exec_next_command_for_run(
			CG_SUBCOMMAND cmd_code, int exit_code);
static int	exec_next_command_for_build_and_run(
			CG_SUBCOMMAND cmd_code, int exit_code);
static int	exec_first_build_and_run_command(void);

/*
 * This should go at the beginning of all public entry points. It
 * checks to see if the request can be processed.
 */
#define public_entry_point() \
	    if (AB_cgen_win == NULL) {return 0;} else {cgen_check_init();}


/*************************************************************************
 **									**
 **		PUBLIC ENTRY POINTS					**
 **									**
 *************************************************************************/

void
cgen_show_codegen_win(
    void
)
{
    Widget shell = (Widget) NULL;
    cgen_check_init();

    if (AB_cgen_win == (Widget) NULL)
    {
	/*
	 * Module initialization
	 */
        dtbCgenWinMainwindowInfo_clear(&dtb_cgen_win_mainwindow);

	dtb_cgen_win_mainwindow_initialize(
		&dtb_cgen_win_mainwindow, dtb_get_toplevel_widget());
        AB_cgen_win = dtb_cgen_win_mainwindow.mainwindow_mainwin;
        shell = dtb_cgen_win_mainwindow.mainwindow;
 
        /* Set up local handles for important widgets */
	make_run_item    = dtb_cgen_win_mainwindow.menubar_File_item_file_pulldown_items.Make_Run_item;
	gen_code_item    = dtb_cgen_win_mainwindow.menubar_File_item_file_pulldown_items.Generate_Code_item;
	make_item        = dtb_cgen_win_mainwindow.menubar_File_item_file_pulldown_items.Make_item;
	run_item         = dtb_cgen_win_mainwindow.menubar_File_item_file_pulldown_items.Run_item;
	abort_item       = dtb_cgen_win_mainwindow.menubar_File_item_file_pulldown_items.Abort_item;
        cgen_props_item  = dtb_cgen_win_mainwindow.menubar_Options_item_options_pulldown_items.Generator_item;
        cur_dir_text     = dtb_cgen_win_mainwindow.proj_dir;
        gen_code_button  = dtb_cgen_win_mainwindow.gen_code_btn;
        make_button      = dtb_cgen_win_mainwindow.make_btn;
        run_button       = dtb_cgen_win_mainwindow.run_btn;
	make_run_button  = dtb_cgen_win_mainwindow.make_run_btn;
	abort_button 	 = dtb_cgen_win_mainwindow.abort_btn;
        output_termWidget= dtb_cgen_win_mainwindow.output_termp;
        input_termWidget = dtb_cgen_win_mainwindow.input_termp;
 
        /* 
         * Setup window to participate in dtbuilder window protocol
         */  
        ab_register_window(AB_cgen_win, AB_WIN_WINDOW, WindowHidden, 
		AB_toplevel, AB_WPOS_STACK_CENTER,
		popdown_cgen_window, NULL); 

        cgen_notify_new_project(proj_get_project());
        obj_add_rename_callback( cgen_obj_name_changed_cb,
                                "cgen_obj_name_changed_cb"); 

        XtRealizeWidget(shell);
	cgenP_sync_up_dir();
	build_dtcodegen_cmd_list(dtcodegenCmdList);

	/*
	 * Make sure the application being run can find its resource file
	 * By setting this via cgenP_put_env_var(), the user has the
	 * option of resetting it to its original value.
	 */
	if (cgenP_get_env_var("XAPPLRESDIR") == NULL)
	{
	    cgenP_put_env_var("XAPPLRESDIR", ".");
	}

        atexit(cgen_abort_at_exit);     /* make sure children die! */
    }
    ab_show_window(AB_cgen_win);

    /* Turn off the cursor for the output only termpane */
    DtTermDisplaySend(output_termWidget, (unsigned char *)"[?25l", strlen("[?25l"));
}


int
cgen_init(void)
{
    int		i = 0;

    cgen_inited = TRUE;

    for (i = 0; i < XtNumber(child_pid_list); ++i)
    {
	child_pid_list[i] = INVALID_PID;
    }
    return 0;
}


static int
build_dtcodegen_cmd_list(STRING *cmdList)
{
    char	exeDirCmd[MAXPATHLEN+1];
    STRING	exeDir = NULL;
    *exeDirCmd = 0;

    exeDir = dtb_get_exe_dir();
    if (exeDir == NULL)
    {
	return ERR_INTERNAL;
    }

    if (*(cmdList[0]) == 0)
    {
        sprintf(exeDirCmd, "%s/dtcodegen", exeDir);
        cmdList[0] = strdup(exeDirCmd);
    }

    if (*(cmdList[1]) == 0)
    {
        sprintf(exeDirCmd, "%s/../abmf/dtcodegen", exeDir);
        cmdList[1] = strdup(exeDirCmd);
    }

    return 0;
}


static void
popdown_cgen_window(
    Widget      widget,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    /* If its secondary dialogs are open, close them first to
     * ensure their state is recorded properly.
     */
    if (AB_cgen_prop_dialog && ab_window_is_open(AB_cgen_prop_dialog))
	ui_win_show(AB_cgen_prop_dialog, False, XtGrabNone);
    if (AB_cgen_env_dialog && ab_window_is_open(AB_cgen_env_dialog))
	ui_win_show(AB_cgen_env_dialog, False, XtGrabNone);

    ui_win_show(widget, False, XtGrabNone);
}


int	
cgen_notify_new_project(ABObj project)
{
    public_entry_point(); 	/* see if we'll allow this entry */
    cgen_set_title(project == NULL? NULL : obj_get_name(project));
    cgen_set_project_dir(NULL);
    return 0;
}


int
cgen_notify_new_directory(STRING dir)
{
    public_entry_point();
    cgen_set_project_dir(dir);
    cgenP_sync_up_dir();	/* NOTE: assuming dir = project dir */
    return 0;
}


int
cgen_abort(void)
{
    public_entry_point();
    return cgenP_abort();
}


static void
cgen_abort_at_exit(void)
{
    cgen_abort();
}

int
cgen_notify_props_new_proj(
    ABObj project
)
{
    int         ret = 0;

    ret = set_props_proj_name(project == NULL? NULL : obj_get_name(project));
    return ret;
}
 
void
cgenP_init_props_module_list(
    Widget      mod_list
)
{
    ABObj       	proj = proj_get_project();
    ABObj       	obj = NULL;
    AB_TRAVERSAL	trav;
 
    for (trav_open(&trav, proj, AB_TRAV_MODULES);
        (obj= trav_next(&trav)) != NULL; )
    {
	if (obj_is_defined(obj))
	{
            ui_list_add_item(mod_list, obj_get_name(obj), 0);
	}
    }
    trav_close(&trav);
}    
 
/*
 * Initialize Code Generator Props:
 *      Add callbacks for object rename & destroy
 */
void
cgenP_prop_init(void)
{
    obj_add_rename_callback(obj_renamedOCB, "cgen_prop_init");
    obj_add_update_callback(obj_updateOCB, "cgen_prop_init");
    obj_add_destroy_callback(obj_destroyedOCB, "cgen_prop_init");
}
 
void
cgen_gen_code(
    CG_SUBCOMMAND       cmd
)
{
    do_user_action(CG_GOAL_GEN_CODE, cmd);
}

void
cgen_make(
    CG_SUBCOMMAND       cmd
)
{
    do_user_action(CG_GOAL_MAKE , cmd);
}

void
cgen_run(
    CG_SUBCOMMAND       cmd
)
{
    do_user_action(CG_GOAL_RUN, cmd);
}

void
cgen_make_run(
    CG_SUBCOMMAND       cmd
)
{
    do_user_action(CG_GOAL_MAKE_AND_RUN, cmd);
}


/*************************************************************************
 *************************************************************************
 **                                                                     **
 **      PRIVATE ENTRY POINTS						**
 **                                                                     **
 *************************************************************************
 *************************************************************************/


/*************************************************************************
**                                                                      **
**      Handle the terminal emulator                                    **
**                                                                      **
*************************************************************************/

/*
 * Prints the string to the terminal. Always flushes.
 */
static int
print_to_term(STRING msg)
{
    int                 rc = 0;
    FILE               *fp = NULL;

    if ((rc = get_slave_device_name()) < 0)
    {
        return rc;
    }
    fp = util_fopen_locked(istr_string(termSlaveDeviceName), "w");
    if (fp == NULL)
    {
        return -1;
    }
    fprintf(fp, "%s", msg);
    util_fclose(fp);
    return 0;
}

/*
 * Sends stdout and stderr to term window
 */
static int
send_output_to_term(void)
{
    static BOOL         done = FALSE;
    int                 rc = 0;

    if (done)
    {
        return 0;
    }
    if ((rc = get_slave_device_name()) < 0)
    {
        return rc;
    }

    done = TRUE;
    freopen(istr_string(termSlaveDeviceName), "w", stdout);
    freopen(istr_string(termSlaveDeviceName), "w", stderr);

    return 0;
}


static int
term_execute_command(CG_SUBCOMMAND cmd_code, STRING cmd, STRING argv[])
{
    CG_STATUS		status_code = CG_STATUS_UNDEF;
    int                 exit_code = 0;
    int                 rc = 0;
    pid_t		rc_pid = INVALID_PID;
    pid_t		watchdog_pid = INVALID_PID;
    int                 child_status = 0;
    STRING		msg = NULL;
    int			i= 0, msg_size = 0;

    if (actual_process_pgid != INVALID_PID)
    {
	return -1;
    }
    if ((rc = get_slave_device_name()) < 0)
    {
        return rc;
    }
    if (cmd == NULL)
    {
        return 0;
    }

    msg_size = strlen("====> Running: ");
    for (i = 0; argv[i] != NULL; ++i)
    {
        msg_size += strlen(argv[i]) + 1;
    }    
    msg = (STRING) XtMalloc(msg_size + 1);  /* Add 1 for NULL */
    strcpy(msg, "====> Running: ");
    strcat(msg, cmd);
    for (i= 1; argv[i] != NULL; ++i)
    {
	strcat(msg, " ");
	strcat(msg, argv[i]);
    }
    strcat(msg, "\n");
    print_to_term(msg);

    /*
     * Clean up (possible) zombie watchdog process(es)
     */
    wait_for_child();

    create_status_pipe();
    watchdog_pid = fork();
    if (watchdog_pid == (pid_t)-1)
    {
        /* error occurred! */
	util_printf_err("Could not create subprocess: %s\n",
		strerror(errno));
	watchdog_pid = INVALID_PID;
	write_to_status_pipe(CG_STATUS_ERROR, cmd_code, (void*)0);
    }
    else if (watchdog_pid == 0)
    {
        /* child - "watchdog" process */
        int            	rc = 0;
	pid_t		actual_process_pid = INVALID_PID;

	/*
	 * For some reason, if the SIGCHLD signal is not blocked,
	 * waitpid() returns -1, with errno == EINTR. This is exactly
	 * the opposite of the behavior I would expect, but it seems
	 * to work fine.
	 */
	{
	    sigset_t	signals;
	    sigemptyset(&signals);
	    sigaddset(&signals, SIGCHLD);
	    sigprocmask(SIG_BLOCK, &signals, NULL);
	}

	util_fdclose(status_pipe_read);

        send_output_to_term();
	actual_process_pid = fork();
	if (actual_process_pid == INVALID_PID)
	{
	    /* error! */
	    fprintf(stderr, "Could not create subprocess: %s\n",
		strerror(errno));
	    write_to_status_pipe(CG_STATUS_ERROR, cmd_code, (void*)0);
	    subprocess_exit(1);
	}
	else if (actual_process_pid == 0)
	{
	    /* grandchild - becomes the program we are actually running */
	    int 	num_env_vars = strlist_get_num_strs(user_env_vars);
	    int	i = 0;
	    STRING	var_name = NULL;
	    STRING	var_value = NULL;
	    STRING	putenv_var = NULL;
	    int		putenv_var_size = 0;

	    setpgid(0,0);		/* make process group leader */
	    write_to_status_pipe(CG_STATUS_STARTED, cmd_code, (void*)(intptr_t) getpgrp());

	    for (i = 0; i < num_env_vars; ++i)
	    {
	        var_value = NULL;
	        var_name = strlist_get_str(user_env_vars,i, (void **)&var_value);
		if (!util_strempty(var_value))
	  	{
	            putenv_var_size = strlen(var_name) + strlen(var_value) + 2;
						/* +2 for "=" and NULL */
	            putenv_var = (STRING)util_malloc(putenv_var_size);
	            sprintf(putenv_var, "%s=%s", var_name, var_value);
		}
	 	else
		{
		    putenv_var_size = strlen(var_name) + 2;
	            putenv_var = (STRING)util_malloc(putenv_var_size);
	            sprintf(putenv_var, "%s=", var_name);
		}
	        util_putenv(putenv_var);
	    }

	    /*
	     * Close status pipe and start command!
	     */
	    util_fdclose(status_pipe_read);
	    util_fdclose(status_pipe_write);
	    if (execvp(cmd, argv) == -1)
	    {
		perror(cmd);
	    }
	    subprocess_exit(1);
	} /* grandchild (actual work process) */
	else
	{
	    /* child ("watchdog" process) */ 
	    int		grandchild_status = 0;
	    pid_t	rc_pid = INVALID_PID;	/* returned pid */

	    /*util_dprintf(3, "process launched: %ld\n", (long)actual_process_pid);*/

	    rc_pid = waitpid(actual_process_pid, &grandchild_status, 0);
	    /*printf("frontline: child done!\n");*/
	    if (rc_pid == INVALID_PID)
	    {
		/* damn! an error occurred... */
	        /*printf("frontline: error waiting for child! (%s)\n",
			util_strsafe(strerror(errno)));*/
		status_code = CG_STATUS_ERROR;
		exit_code= -1;
	    }
	    else
	    {
		exit_code= -1;
		if (WIFEXITED(grandchild_status))
		{
		    /* grandchild exited */
		    /* printf("frontline: grandchild exited(%d)\n", 
			WEXITSTATUS(grandchild_status));*/
		    status_code = CG_STATUS_EXITED;
		    exit_code = WEXITSTATUS(grandchild_status);
		}
		else if (WIFSIGNALED(grandchild_status))
		{
		    /* child was killed by uncaught signal */
		    int		kill_signal = WTERMSIG(grandchild_status);
		    STRING	signalName = NULL;
		    time_t	signalTime = time(NULL);
		    BOOL	coreDumped = FALSE;
		    struct stat	fileInfo;
		    /*printf("frontline: child signalled(%d)\n", kill_signal);*/

		    status_code = CG_STATUS_SIGNALLED;
		    exit_code = kill_signal;

		    if (stat("core", &fileInfo) == 0)
		    {
			if (labs(difftime(signalTime, fileInfo.st_mtime)) < 5)
			{
			    coreDumped = TRUE;
			}
		    }

		    switch (kill_signal)
		    {
			case SIGABRT: signalName = "Aborted";
			break;
			case SIGALRM: signalName = "Uncaught alarm";
			break;
			#ifdef SIGBUS	/* SIGBUS is not POSIX */
			case SIGBUS: signalName = "Bus error";
			break;
			#endif
			case SIGFPE: signalName = "Arithmetic exception";
			break;
			case SIGHUP: signalName = "Hangup";
			break;
			case SIGILL: signalName = "Illegal instruction";
			break;
			case SIGINT: signalName = "Interrupted";
			break;
			case SIGKILL: signalName = "Killed";
			break;
			case SIGPIPE: signalName = "Write to bad pipe";
			break;
			case SIGQUIT: signalName = "Quit";
			break;
			case SIGSEGV: signalName = "Segmentation fault";
			break;
			case SIGTERM: signalName = "Terminated";
			break;
			default:
			{
			    static char sigmsg[30];
			    sprintf(sigmsg, "Uncaught signal %d", kill_signal);
			}
			break;
		    }

		    fprintf(stderr, "====> %s", signalName);
		    if (coreDumped)
		    {
		        fprintf(stderr, " (Core dumped)");
		    }
		    fprintf(stderr, "\n");
		}
	    }

	    if (status_pipe_write >= 0)
	    {
	        write_to_status_pipe(status_code, cmd_code, (void*)(intptr_t) exit_code);
                util_fdclose(status_pipe_write);
	    }
            subprocess_exit(exit_code);
	} /* child (watchdog) */

	/* This block should never execute */
	assert(("Bad block executed",TRUE));
	subprocess_exit(1);
    }
    else
    {
	/* parent */
	add_child_to_list(watchdog_pid);
	util_fdclose(status_pipe_write);
    }

    return 0;
}


/*
 * Gets the slave device name and puts it into the file variable
 */
static int
get_slave_device_name(void)
{
    String              deviceName = NULL;

    if (termSlaveDeviceName != NULL)
    {
        return 0;
    }

    if (termSlaveDeviceName == NULL)
    {
        XtVaGetValues(output_termWidget, DtNtermSlaveName, &deviceName, NULL);
        if (deviceName != NULL)
        {
	    termSlaveDeviceName= istr_create(deviceName);
            /*util_dprintf(2,"slavename: '%s'\n", util_strsafe(deviceName));*/
        }
    }   /* deviceName == NULL */

    return (termSlaveDeviceName == NULL ? -1 : 0);
}


/*************************************************************************
**                                                                      **
**      Implement the functions                                         **
**                                                                      **
*************************************************************************/

static int
exec_generate_code(void)
{
    ABObj	project= proj_get_project();
    STRING	project_name= NULL;

    if (project == NULL)
    {
	return -1;
    }
    project_name = obj_get_name(project);
    if (project_name == NULL)
    {
	return -1;
    }

    switch (CodeGenOptions.cmd_flag)
    {
	case CG_GEN_SPECIFIC_FILES_FLAG:
            exec_generate_specific_files();
        break;
 
        case CG_GEN_SPECIFIC_FILES_AND_MAIN_FLAG:
            exec_generate_specific_files_and_main();
        break;
 
        case CG_GEN_MAIN_FLAG:
            exec_generate_main();
        break;

        case CG_GEN_PROJ_FLAG:
            exec_generate_proj();
        break;
 
        default:
        break;
    }
    return 0;
}

static int
exec_generate_main(void)
{
    int		rc = 0;		/* return code */
    ABObj       project= proj_get_project();
    STRING      project_name= NULL;
    STRING      argv[MAX_CGEN_FIXED_ARGS];
    int         i = 0;
    STRING	cmd = NULL;

    if (project == NULL)
    {
        return -1;
    }
    project_name = obj_get_name(project);
    if (project_name == NULL)
    {
        return -1;
    }

    for (i = 0; i < MAX_CGEN_FIXED_ARGS; i++)
        argv[i] = NULL;

    i = 0;
    if ((rc = build_dtcodegen_arg_list(argv, &i)) < 0)
    {
	return rc;
    }

    argv[i]= "-main";		i++;
    argv[i]= "-p";		i++;
    argv[i]= project_name;	i++;
    argv[i]= NULL;

    term_execute_command(CG_CMD_GEN_CODE, argv[0], argv);
    return 0;
}
 
static int
exec_generate_proj(void)
{
    int		rc = 0;		/* return code */
    ABObj       project= proj_get_project();
    STRING      project_name= NULL;
    STRING      argv[MAX_CGEN_FIXED_ARGS];
    int         i = 0;
    STRING	cmd = NULL;
 
    if (project == NULL)
    {
        return -1;
    }
    project_name= obj_get_name(project);
    if (project_name == NULL)
    {
        return -1;
    }
 
    for (i = 0; i < MAX_CGEN_FIXED_ARGS; i++) 
        argv[i] = NULL;

    i = 0;
    if ((rc = build_dtcodegen_arg_list(argv, &i)) < 0)
    {
	return rc;
    }
       
    argv[i]= "-p";              i++;
    argv[i]= project_name;      i++;
    argv[i]= NULL;
    term_execute_command(CG_CMD_GEN_CODE, argv[0], argv);
 
    return 0;
}
 
static int
exec_generate_specific_files(void)
{
    int		returnValue = 0;
    int		rc = 0;		/* return code */
    ABObj       project= proj_get_project();
    STRING      project_name= NULL;
    STRING      *argv;
    int         argv_size = MAX_CGEN_FIXED_ARGS;
    int         i, n, num_args, arg_count, num_mods;
 
    if (project == NULL)
    {
        return -1;
    }
    project_name= obj_get_name(project);
    if (project_name == NULL)
    {
        return -1;
    }

    num_mods = strlist_get_num_strs(CodeGenOptions.module_list);
    argv_size += num_mods;
    argv = (STRING *)util_malloc(argv_size * sizeof(STRING));
    for (i = 0; i < argv_size; i++)
	argv[i] = NULL;
 
    arg_count = 0;
    if ((rc = build_dtcodegen_arg_list(argv, &arg_count)) < 0)
    {
	returnValue = rc;
	goto epilogue;
    }

    argv[arg_count]= "-p";              arg_count++;
    argv[arg_count]= project_name;      arg_count++;
 
    num_args = arg_count + num_mods;
    for (i = arg_count, n = 0; i < num_args; i++, n++)
    {
        argv[i] = strlist_get_str(CodeGenOptions.module_list, n, (void **)NULL);
    }
    argv[num_args] = NULL;
     
    term_execute_command(CG_CMD_GEN_CODE, argv[0], argv);

epilogue:
    util_free(argv);
    return returnValue;
}
 
static int
exec_generate_specific_files_and_main(void)
{
    int		returnValue = 0;
    int		rc = 0;		/* return code */
    ABObj       project= proj_get_project();
    STRING      project_name= NULL;
    STRING      *argv;
    int         argv_size = MAX_CGEN_FIXED_ARGS;
    int         i, n, num_args, arg_count, num_mods;
 
    if (project == NULL)
    {
        return -1;
    }
    project_name= obj_get_name(project);
    if (project_name == NULL)
    {
        return -1;
    }

    num_mods = strlist_get_num_strs(CodeGenOptions.module_list);
    argv_size += num_mods;
    argv = (STRING *)util_malloc(argv_size * sizeof(STRING));
    for (i = 0; i < argv_size; i++) 
        argv[i] = NULL;

    arg_count = 0;
    if ((rc = build_dtcodegen_arg_list(argv, &arg_count)) < 0)
    {
	returnValue = rc;
	goto epilogue;
    }

    argv[arg_count]= "-main";           arg_count++;
    argv[arg_count]= "-p";              arg_count++;
    argv[arg_count]= project_name;      arg_count++;
 
    num_args = arg_count + num_mods;
    for (i = arg_count, n = 0; i < num_args; i++, n++)
    {
        argv[i] = strlist_get_str(CodeGenOptions.module_list, n, (void **)NULL);
    }
    argv[num_args] = NULL;
     
    term_execute_command(CG_CMD_GEN_CODE, argv[0], argv);

epilogue:
    util_free(argv);
    return returnValue;
}


/*
 * Builds the code generator options that are common to all the code
 * generation goals.
 *
 * argList[0] = the executable
 *
 * The individual strings returned in argList are pointers to static
 * storage and should not be freed by the caller.
 *
 * Returns the number of arguments that were added.
 *
 * ASSUMES: the argList array is large enough to handle all args, plus a NULL
 */
static int
build_dtcodegen_arg_list(STRING *argList, int *iInOut)
{
    int		returnValue = 0;
    int		rc = 0;
    int		i = (*iInOut);
    int		numArgsAdded = 0;
    STRING	dtcodegenCmd = NULL;

    if ((rc = select_command(dtcodegenCmdList, &dtcodegenCmd)) < 0)
    {
	print_cmd_not_found_message("dtcodegen");
	returnValue = rc;
	goto epilogue;
    }

    argList[i++]= dtcodegenCmd;
    argList[i++]= "-changed";		/* everybody gets this one */

    if (CodeGenOptions.no_merge)     
    {
        argList[i++] = "-nomerge";
	++numArgsAdded;
    }
 
    switch (CodeGenOptions.verbosity)
    {
        case CG_VERBOSITY_SILENT:
            argList[i++] = "-s";
	    ++numArgsAdded;
        break;
         
        case CG_VERBOSITY_VERBOSE:
            argList[i++] = "-v";
	    ++numArgsAdded;
        break;
    }

    argList[i] = NULL;

epilogue:
    if (returnValue >= 0)
    {
	/* successful - return i */
	returnValue = numArgsAdded;
	(*iInOut) = i;
    }
    return returnValue;
}


static int
exec_make(void)
{
    int			rc = 0;		/* return code */
    STRING		cmd = NULL;
    STRING		argv[5];
    BOOL		continueMake = FALSE;

    rc= check_makefile(&continueMake);
    if ((rc < 0) || (!continueMake))
    {
	return rc;
    }

    if ((rc = select_command(makeCmdList, &cmd)) < 0)
    {
	print_cmd_not_found_message(makeCmdList[0]);
	return rc;
    }

    argv[0] = cmd;
    argv[1] = CodeGenOptions.make_args;
    argv[2] = NULL;
    term_execute_command(CG_CMD_MAKE, argv[0], argv);
    return 0;
}


static int
exec_run(void)
{
    ABObj	project= NULL;
    char	executable_name[1024];
    char	cmd[1024];
    STRING	argv[5];
    *executable_name = 0;
    *cmd = 0;

    project= proj_get_project();
    if ((project == NULL) || (obj_get_name(project) == NULL))
    {
	return -1;
    }

    cvt_type_to_ident(obj_get_name(project), executable_name, 1024);
    sprintf(cmd, "./%s", executable_name);
    argv[0] = cmd;
    argv[1] = CodeGenOptions.run_args;
    argv[2] = NULL;
    term_execute_command(CG_CMD_RUN, argv[0], argv);

    return 0;
}


/*
 * cmd_code is the code of the command that just finished.  Executes
 * the next command necessary to achieve the user's goal.
 *
 * If cmd_code is CG_CMD_UNDEF, assumes that no commands have been
 * issued, and issues the first command to achieve the goal.
 */
static int
exec_next_command(CG_SUBCOMMAND cmd_code, int exit_code)
{
    int return_value= 0;

    if (cmd_code == CG_CMD_UNDEF)
    {
	print_to_term("\n\n");
	goto_busy_state();
    }

    switch (user_goal)
    {
        case CG_GOAL_GEN_CODE:
            return_value= exec_next_command_for_gen_code(cmd_code, exit_code);
        break;

        case CG_GOAL_MAKE:
            return_value= exec_next_command_for_make(cmd_code, exit_code);
        break;

        case CG_GOAL_RUN:
            return_value= exec_next_command_for_run(cmd_code, exit_code);
        break;

	case CG_GOAL_MAKE_AND_RUN:
	    return_value = exec_next_command_for_build_and_run(cmd_code, exit_code);
	break;

        default:
	    goto_ready_state();
            return_value= ERR_INTERNAL;
        break;
    }

    if (return_value < 0)
    {
	goto_ready_state();
    }

    return return_value;
}

static int
exec_next_command_for_gen_code(CG_SUBCOMMAND cmd_code, int exit_code)
{
    int		rc= 0;	/* return code */
    exit_code = exit_code;	/* avoid warning */

    switch (cmd_code)
    {
        case CG_CMD_UNDEF:
            rc= exec_generate_code();
        break;

        case CG_CMD_GEN_CODE:
            print_success_message();
        break;

        default:
        break;
    }
    return 0;
}


static int
exec_next_command_for_make(CG_SUBCOMMAND cmd_code, int exit_code)
{
    exit_code = exit_code;	/* avoid warning */

    switch (cmd_code)
    {
        case CG_CMD_UNDEF:
	case CG_CMD_GEN_CODE:	/* was run to get Makefile */
            exec_make();
        break;

        case CG_CMD_MAKE:
            print_success_message();
        break;

        default:
        break;
    }

    return 0;
}

static int
exec_next_command_for_run(CG_SUBCOMMAND cmd_code, int exit_code)
{
    switch (cmd_code)
    {
        case CG_CMD_UNDEF:
            exec_run();
        break;

        case CG_CMD_RUN:
            print_exit_message(exit_code);
        break;

        default:
        break;
    }

    return 0;
}

static int
exec_next_command_for_build_and_run(CG_SUBCOMMAND cmd_code, int exit_code)
{
    switch (cmd_code)
    {
        case CG_CMD_UNDEF:
            /* this may do a generate and/or a make */
            exec_first_build_and_run_command();
        break;

        case CG_CMD_GEN_CODE:
            exec_make();
        break;

        case CG_CMD_MAKE:
            exec_run();
        break;

        case CG_CMD_RUN:
            print_exit_message(exit_code);
        break;

        default:
        break;
    } /* cmd_code */

    return 0;
}


static int
exec_first_build_and_run_command(void)
{
    int	return_value= 0;
    if (!util_file_exists("Makefile"))
    {
	return_value= exec_generate_proj();
    }
    else
    {
	return_value= exec_make();
    }
    return return_value;
}


static int
wait_for_child(void)
{
    pid_t	childPid = INVALID_PID;
    pid_t	rcPid = INVALID_PID;
    int		i = 0;
    int		numExited = 0;

    for (i = 0; i < XtNumber(child_pid_list); ++i)
    {
	if ((childPid = child_pid_list[i]) == INVALID_PID)
	{
	    continue;
	}
        if (   ((rcPid = waitpid(childPid, (int*)0, WNOHANG)) == childPid)
	    || (kill(childPid,0) == -1))
        {
            child_pid_list[i] = INVALID_PID;
	    ++numExited;
        }
    }

    return numExited;
}


static int
add_child_to_list(pid_t childPid)
{
    int		i = 0;
    BOOL	added = FALSE;
    int		attemptCount = 0;
    int		maxAttempts = EXIT_SLEEP_SECONDS+2;

    for (attemptCount = 0; (!added) && (attemptCount < maxAttempts);
	++attemptCount)
    {
	if (attemptCount > 0)
	{
	    sleep(1);		/* wait for child to exit, making room */
	}
	wait_for_child();	/* make some room */

        for (i = 0; i < XtNumber(child_pid_list); ++i)
        {
	    if (child_pid_list[i] == INVALID_PID)
	    {
	        child_pid_list[i] = childPid;
	        added = TRUE;
	        break;
	    }
        }
    }

    return added?0:-1;
}

static int
subprocess_exit(int exit_code)
{
    if (status_pipe_write >= 0)
    {
	util_fdsync(status_pipe_write);
    }
    util_fdclose(status_pipe_write);
    util_fdclose(status_pipe_read);

    /*
     * On AIX, writing to a pipe and immediately exiting seems to guarantee
     * that not all the data will reach the receiving end of the pipe.
     * closing the pipe and sleeping for a few seconds seems to work well.
     */
    if (util_get_os_type() == AB_OS_AIX)
    {
	sleep(EXIT_SLEEP_SECONDS);
    }
    _exit(exit_code);
    return -1;
}


static int
print_internal_err_message(void)
{
    print_to_term("****> UNSUCCESSFUL (Internal failure occurred).\n");
    return 0;
}


static int
print_failure_message(CG_SUBCOMMAND cmd_code, int exit_code)
{
    char	msg[256];
    cmd_code= cmd_code;
    sprintf(msg, "****> UNSUCCESSFUL (Command exited with code %d).\n",
		exit_code);
    print_to_term(msg);
    user_goal= CG_GOAL_UNDEF;
    goto_ready_state();
    return 0;
}


static int
print_success_message(void)
{
    print_to_term("====> Completed successfully.\n");
    user_goal= CG_GOAL_UNDEF;
    goto_ready_state();
    return 0;
}


static int
print_cmd_not_found_message(STRING cmd)
{
    char	msg[1024];
    *msg = 0;

    sprintf(msg, 
    "****> ERROR - Could not find command '%s'.\n"
    "****>         Please check your PATH variable (This can be\n"
    "****>         done via the Options->Environment menu).\n",
	cmd);
    print_to_term(msg);

    user_goal= CG_GOAL_UNDEF;
    goto_ready_state();
    return 0;
}


static int
print_death_message(void)
{
    char msg[256];
    sprintf(msg, 
"****> Program died a horrible, unnatural death, due to an uncaught signal\n");
    print_to_term(msg);
    user_goal = CG_GOAL_UNDEF;
    return goto_ready_state();
}


static int
print_exit_message(int exitCode)
{
    char msg[256];
    sprintf(msg, "====> Program exited (exit code %d)\n", exitCode);
    print_to_term(msg);
    user_goal= CG_GOAL_UNDEF;
    goto_ready_state();
    return 0;
}


static int
print_abort_message(void)
{
    print_to_term("\n====> Command aborted.\n");
    user_goal= CG_GOAL_UNDEF;
    goto_ready_state();
    return 0;
}


static void
pipe_data_ready_proc(
    XtPointer	client_data,
    int		*fid,
    XtInputId	*id
)
{
    BOOL		aborted = FALSE;
    CG_SUBCOMMAND	cmd_code = CG_CMD_UNDEF;
    CG_STATUS		status_code = CG_STATUS_UNDEF;
    void		*status_data = NULL;
    int			int_status_code = 0;
    int			int_cmd_code = 0;
    int			exit_code = 0;
    int			kill_signal = 0;
    pid_t		rc_pid = INVALID_PID;
    id = id;	/* avoid warning */

    /*util_dprintf(3, "rcv - data ready on pipe...\n");*/
    if (read_from_status_pipe(&status_code, &cmd_code, &status_data) < 0)
    {
	status_code = CG_STATUS_ERROR;
	goto epilogue;
    }
    aborted = (   (abortingPID != INVALID_PID)
	       && (abortingPID == actual_process_pgid) );

    switch (status_code)
    {
	case CG_STATUS_STARTED:
	    actual_process_pgid = (pid_t)(intptr_t) status_data;
	    /*util_dprintf(2,"rcv started: %ld\n", (long)actual_process_pgid);*/
	break;

	case CG_STATUS_EXITED:
	    exit_code = (int)(intptr_t) status_data;
	    actual_process_pgid = INVALID_PID;
	    /*util_dprintf(2,"rcv exit(%d)\n", exit_code);*/
	    if (aborted)
	    {
		/* message gets printed below */
	    }
            else if ((exit_code != 0) && (cmd_code != CG_CMD_RUN))
            {
	        print_failure_message(cmd_code, exit_code);
	        goto_ready_state();
            }
            else
            {
	        exec_next_command(cmd_code, exit_code);
	    }
	break;

	case CG_STATUS_SIGNALLED:
	    kill_signal = (int)(intptr_t) status_data;
	    /*util_dprintf(2,"rcv signalled(%d)\n", kill_signal);*/
	    actual_process_pgid = INVALID_PID;
	    goto_ready_state();
	break;

	case CG_STATUS_ERROR:
	    print_internal_err_message();
	    if (actual_process_pgid != INVALID_PID)
	    {
	        if (careful_kill_group(actual_process_pgid) >= 0)
		{
	    	    actual_process_pgid = INVALID_PID;
		}
	    }
	    goto_ready_state();
	break;

	default:
	    if (actual_process_pgid != INVALID_PID)
	    {
		if (careful_kill_group(actual_process_pgid) >= 0)
		{
		    actual_process_pgid = INVALID_PID;
		}
		goto_ready_state();
	    }
	break;
    }

    if (aborted)
    {
	print_abort_message();
	goto_ready_state();
    }

    if (actual_process_pgid == INVALID_PID)
    {
        abortingPID = INVALID_PID;
    }

epilogue:
    wait_for_child();	/* clean up (possible) zombie watchdog processes */
    return;
}


/*
 * We are running a process - desensitize most buttons.
 */
static int
goto_busy_state(void)
{
    XtSetSensitive(abort_button, True);
    XtSetSensitive(abort_item, True);

    XtSetSensitive(cgen_props_item, False);
    XtSetSensitive(gen_code_button, False);
    XtSetSensitive(gen_code_item, False);
    XtSetSensitive(make_button, False);
    XtSetSensitive(make_item, False);
    XtSetSensitive(make_run_button, False);
    XtSetSensitive(make_run_item, False);
    XtSetSensitive(run_button, False);
    XtSetSensitive(run_item, False);
    return 0;
}


/*
 * We are waiting for user input
 */
static int
goto_ready_state(void)
{
    XtSetSensitive(abort_button, False);
    XtSetSensitive(abort_item, False);

    XtSetSensitive(cgen_props_item, True);
    XtSetSensitive(gen_code_button, True);
    XtSetSensitive(gen_code_item, True);
    XtSetSensitive(make_button, True);
    XtSetSensitive(make_item, True);
    XtSetSensitive(make_run_button, True);
    XtSetSensitive(make_run_item, True);
    XtSetSensitive(run_button, True);
    XtSetSensitive(run_item, True);

    return 0;
}


static int
create_status_pipe(void)
{
    BOOL	pipeOpen = TRUE;
    /*util_dprintf(2,"create_status_pipe()\n");*/

    if ((status_pipe_read < 0) || (status_pipe_write < 0))
    {
        int                 fds[2];

	destroy_status_pipe();
        if (pipe(fds) == 0)
        {
	    pipeOpen = TRUE;
            status_pipe_read = fds[0]; fds[0] = -1;
            status_pipe_write = fds[1]; fds[1] = -1;
        }

        if (   (input_proc_id == -1) 
	    && (status_pipe_read >= 0)
	    && (output_termWidget != NULL))
        {
	    XtAppContext	app_context= XtWidgetToApplicationContext(output_termWidget);
	    input_proc_id = 
	        XtAppAddInput(
		    app_context, 
		    status_pipe_read,
		    (XtPointer)XtInputReadMask,
		    pipe_data_ready_proc,
		    NULL
		    );
        }
    }


#ifdef DEBUG
    if (!pipeOpen)
    {
	util_dprintf(1, "CGEN WINDOW: COULD NOT CREATE STATUS PIPE\n");
    }
#endif /* DEBUG */
    return pipeOpen?0:-1;
}


static int
destroy_status_pipe(void)
{
    /*util_dprintf(2,"destroy_status_pipe()\n");*/
    util_fdclose(status_pipe_read);
    util_fdclose(status_pipe_write);

    if (input_proc_id != -1)
    {
        XtRemoveInput(input_proc_id); input_proc_id = -1;
    }

    return 0;
}


static int
write_to_status_pipe(
			CG_STATUS	status_code, 
			CG_SUBCOMMAND	cmd_code, 
			void		*status_data
)
{
    int		int_status_code = (int)status_code;
    int		int_cmd_code= (int)cmd_code;

    /*printf("write to pipe: %d %d %d\n", 
		(int)status_code, (int)cmd_code, (int)status_data);*/
    write(status_pipe_write, (void*)&int_status_code, sizeof(int));
    write(status_pipe_write, (void*)&int_cmd_code, sizeof(int));
    write(status_pipe_write, (void*)&status_data, sizeof(void*));

    /*
     * The parent always keeps the write file descriptor open, and
     * on HP and IBM, each write may not get flushed. Force it.
     */
    util_fdsync(status_pipe_write);

    return 0;
}

static int	
read_from_status_pipe(
			CG_STATUS	*status_code_out,
			CG_SUBCOMMAND	*cmd_code_out,
			void		**status_data_out
)
{
    int		return_value = 0;
    int		int_status_code = 0;
    int		int_cmd_code = 0;
    void	*status_data = NULL;
    int		nread = 0;

    /* util_dprintf(2,"Data ready\n");*/
    if (status_pipe_write >= 0)
    {
	util_fdsync(status_pipe_write);
    }
    nread += read(status_pipe_read, (void *)&int_status_code, sizeof(int));
    nread += read(status_pipe_read, (void *)&int_cmd_code, sizeof(int));
    nread += read(status_pipe_read, (void *)&status_data, sizeof(void*));
    (*status_code_out) = (CG_STATUS)int_status_code;
    (*cmd_code_out) = (CG_SUBCOMMAND)int_cmd_code;
    (*status_data_out) = status_data;

    return_value = nread;
    if (nread < 1)
    {
	/* The write end of the pipe is apparently closed */
	destroy_status_pipe();
	return_value = -1;
    }

    /*printf("rcv (fd:%d bytes:%d) read from pipe: %d %d %d\n", 
	status_pipe_read, nread,
	(int)(*status_code_out), 
	(int)(*cmd_code_out), 
	(int)(*status_data_out));*/

    return return_value;
}


/*
 * Tries to kill the process in a "friendly" way.  Sends SIGTERM first,
 * waits 5 seconds, and then sends SIGKILL.
 *
 * Returns >= 0 if successfully killed, <0 otherwise
 */
static int
careful_kill_group(pid_t pgid)
{
    Bool	killed = False;
    int		rc = 0;
    pid_t	leader_pid = pgid;
    int		child_status = 0;
    /* pid_t	pid_done = INVALID_PID; */
    int		waitcount = 0;
    long	kill_pgrp_id = (long)(-1 * pgid); /* negative pid = group id */

    /*util_dprintf(2, "careful_kill_group(%ld)\n", (long)pgid);*/

    /*
     * Try SIGTERM
     */
    kill(kill_pgrp_id, SIGTERM);
    for (waitcount= 0; (!killed) && (waitcount < 5); ++waitcount)
    {
	if (kill(kill_pgrp_id, 0) == -1)	/* sig 0 checks pid only */
	{
	    /* kill failed, so group doesn't exist, any more */
	    killed = TRUE;
	}
	if (!killed)
	{
	    sleep(1);
	}
    }

    /* 
     * if SIGTERM was ignored, NUKE IT! 
     */
    if (!killed)
    {
	kill(kill_pgrp_id, SIGKILL);	/* can't ignore this, turkey! */
        for (waitcount= 0; (!killed) && (waitcount < 5); ++waitcount)
        {
	    if (kill(kill_pgrp_id, 0) == -1)	/* sig 0 checks pid only */
	    {
	        /* kill failed, so group doesn't exist, any more */
	        killed = TRUE;
	    }
	    if (!killed)
	    {
	        sleep(1);
	    }
	}
    }

    /*util_dprintf(2,"%s: %ld\n", (killed? "Killed":"COULD NOT KILL"), (long)pgid);*/

    return killed? 0:-1;
}


/*
 * projectName may be NULL (signifies no project)
 */
static int
cgen_set_title(STRING projectName)
{
    char	newTitle[256];
    strcpy(newTitle, "Code Generator ");
    if (projectName == NULL)
    {
	strcat(newTitle, "(No Project)");
    }
    else
    {
	sprintf(newTitle+strlen(newTitle), " - Project %s.bip",
		projectName);
    }

    XtVaSetValues(XtParent(AB_cgen_win),
	XmNtitle,	newTitle,
	NULL);

    return 0;
}


/*
 * Sets the project dir: field to be the current directory
 * If dir is NULL, looks at CWD.
 */
static int
cgen_set_project_dir(STRING dir)
{
    STRING	newDir= NULL;
    XmString	xmlabel= NULL;

    if (dir != NULL)
    {
	newDir= dir;
    }
    else
    {
	newDir= ab_get_cur_dir();
    }
    xmlabel = XmStringCreateLocalized(newDir);
    XtVaSetValues(cur_dir_text,
		XmNlabelString, xmlabel,
		NULL);
    XmStringFree(xmlabel); xmlabel= NULL;
    return 0;
}


static int
cgen_obj_name_changed_cb(ObjEvAttChangeInfo evInfo)
{
    ABObj	project= evInfo->obj;
    char	newTitle[256];

    if (   (obj_is_project(project))
	&& ((evInfo->atts & OBJEV_ATT_NAME) != 0) 
	&& (proj_get_project() == project) 
       )
    {
	/* the project's name changed.  Update the title bar */
	cgen_set_title(obj_get_name(project));
    }

    return 0;
}


static int
cgenP_abort(void)
{
    /*util_dprintf(2, "GUI: aborting(%ld)\n", (long)actual_process_pgid);*/
    if (   (actual_process_pgid != INVALID_PID)
	&& (abortingPID != actual_process_pgid) )
    {
        /* the process still exists, and it is not in the
         * process of being aborted. Abort it!
         */
        XtSetSensitive(abort_button, False);
        abortingPID = actual_process_pgid;	/* do first!, so we know.. */
        careful_kill_group(abortingPID);
        XtSetSensitive(abort_button, True);
    }

    if (   (actual_process_pgid == INVALID_PID)
	&& (abortingPID == INVALID_PID) )
    {
	/* There is no process to be aborted */
	goto_ready_state();
    }

    return 0;
}


/*
 * projectName may be NULL (signifies no project)
 */
static int
set_props_proj_name(STRING projectName)
{
    char        newProj[256];

    if (projectName == NULL)
    {
        strcpy(newProj, "(No Project)");
    }
    else
    {
	sprintf(newProj, "%s.bip", projectName);
    }

    XtVaSetValues(dtb_cgen_props_cgen_props_dlg.proj_name,
        XtVaTypedArg, XmNlabelString, XtRString,
        newProj, strlen(newProj)+1,
        NULL);

    return 0;  
}
 
/*
 * obj-callback: object name has changed - update Prop Dialog lists
 *               Or project name has changed - update the Prop
 *               proj name.
 */
static int
obj_renamedOCB(
    ObjEvAttChangeInfo    info
)
{
    Widget      list = NULL;
    STRING      mod_name = NULL;
 
    if (AB_cgen_prop_dialog != NULL)
    {
        if (!obj_is_module(info->obj) && !obj_is_project(info->obj))
	{
            return 0;
	}
 
        if (   (obj_is_project(info->obj))
                && ((info->atts & OBJEV_ATT_NAME) != 0))
        {
            /* the project's name changed.  Update prop sheet proj_name */
            set_props_proj_name(obj_get_name(info->obj));
        }
        else
        {
            mod_name = obj_get_name(info->obj);
            if (mod_name == NULL)
                return -1;
 
            XtVaGetValues(AB_cgen_prop_dialog, XmNuserData, &list, NULL);
 
            /* A new module was created and named */
            if (info->old_name != NULL)
	    {
                ui_list_replace_item(list, istr_string(info->old_name), mod_name);
	    }
        }
    }

    return 0;
}    
 
/*
 * obj-callback: object is being destroyed - remove from CGen prop
 *              sheet list.
 */
static int
obj_destroyedOCB(
    ObjEvDestroyInfo    info
)
{
    Widget      list = NULL;
    STRING      mod_name = NULL;
 
    if (AB_cgen_prop_dialog != NULL)
    {
        if (!obj_is_module(info->obj))
            return 0;

        mod_name = obj_get_name(info->obj);
        if (mod_name == NULL)
            return -1;
 
        XtVaGetValues(AB_cgen_prop_dialog, XmNuserData, &list, NULL);
        ui_list_delete_item(list, mod_name);
    }
 
    return 0;    
}

/* 
 * obj-callback: Called when a new project is opened.
 */
static int
obj_updateOCB(
    ObjEvUpdateInfo     info
)
{
    Widget	list = NULL;

    if (AB_cgen_prop_dialog != NULL)
    {
        if ( !obj_is_project(info->obj) &&
	     !obj_is_module(info->obj)
	   )
	{
            return 0;
	}

	if (obj_is_project(info->obj))
	{
	    /* the project's name changed.  Update prop sheet proj_name */
	    set_props_proj_name(obj_get_name(info->obj));
	}
	
	XtVaGetValues(AB_cgen_prop_dialog, XmNuserData, &list, NULL);
	XmListDeleteAllItems(list);
	cgenP_init_props_module_list(list);
	
	/* Initialize the selected modules lists */
	if (CodeGenOptions.module_list != NULL)  /* the list that is applied */
	{
	    strlist_destroy(CodeGenOptions.module_list);
	    CodeGenOptions.module_list = NULL;
	}
	if (module_list != NULL)	/* the list that reflects what
					 * is currently selected */
	{
	    strlist_destroy(module_list);
            module_list = strlist_create(); 
	}
    }

    return 0;
}

int
cgenP_sync_up_dir(void)
{
    STRING 	cmd = NULL;
    STRING 	request_dir = NULL;
    int		cmd_size = 0;

    request_dir = ab_get_cur_dir();
    if (!util_strempty(request_dir))
    {
	cmd_size = strlen("cd ") + strlen(request_dir) + 2;
	cmd = (STRING) XtMalloc(cmd_size);
	strcpy(cmd, "cd ");
	strcat(cmd, request_dir);
	strcat(cmd, "\n");
	DtTermSubprocSend(input_termWidget, (unsigned char*)cmd, strlen(cmd));
    }
    return 0;
}


static int
do_user_action(CG_GOAL goal, CG_SUBCOMMAND cmd)
{
    int			return_value = 0;
    int			rc = 0;				/* return code */
    BOOL		doAction = TRUE;
    DTB_MODAL_ANSWER    answer = DTB_ANSWER_NONE;
    ABObj		project = proj_get_project();
    Widget		dlg = NULL;

    user_goal = goal;
    assert(cmd == CG_CMD_UNDEF);	/* only startup implemented, here */

    /***** SEE IF cc IS ON THE PATH *****/

    if ((rc = check_path()) < 0)
    {
	return_value = rc;
	goto epilogue;
    }


    /*
     * See if we need to build the executable
     */
    if (goal == CG_GOAL_RUN)
    {
	char	exeName[MAXPATHLEN+1];
	ABObj	project = proj_get_project();
	STRING	projName = NULL;
	if ((project != NULL) && ((projName = obj_get_name(project)) != NULL))
	{
	    sprintf(exeName, "./%s", projName);
	    if (!util_file_exists(exeName))
	    {
		goal = CG_GOAL_UNDEF;	/* can't run it - it don't exist! */
		dtb_cgen_win_no_exe_msg_initialize(
			&dtb_cgen_win_no_exe_msg);
	        answer = dtb_show_modal_message(AB_cgen_win,
			&dtb_cgen_win_no_exe_msg, 
			NULL, NULL, &dlg);
		if (answer == DTB_ANSWER_ACTION1)	/* Build It */
		{
		    goal = CG_GOAL_MAKE_AND_RUN;
		}
	    }
	}
    }

    if (goal == CG_GOAL_UNDEF)
    {
	return -1;
    }
    user_goal = goal;


    /***** SEE IF THERE ARE UNSAVED EDITS *****/

        if ( (goal != CG_GOAL_RUN) && proj_check_unsaved_edits(project) )
        {
	    BOOL	doSave = FALSE;

	    if (all_files_exist(project))
	    {
		dtb_cgen_win_query_save_or_gen_old_msg_initialize(
			&dtb_cgen_win_query_save_or_gen_old_msg);
	        answer = dtb_show_modal_message(AB_cgen_win,
			&dtb_cgen_win_query_save_or_gen_old_msg, 
			NULL, NULL, &dlg);
		switch (answer)
		{
		    case DTB_ANSWER_ACTION1:	/* save */
			doSave = TRUE;
		    break;

		    case DTB_ANSWER_ACTION2:	/* gen old */
		    break;

		    default:
			doAction = FALSE;
		    break;
		}
	    }
	    else
	    {
		dtb_cgen_win_query_save_or_abort_msg_initialize(
			&dtb_cgen_win_query_save_or_abort_msg);
	        answer = dtb_show_modal_message(AB_cgen_win,
			&dtb_cgen_win_query_save_or_abort_msg, 
			NULL, NULL, &dlg);
		switch (answer)
		{
		    case DTB_ANSWER_ACTION1:	/* save */
			doSave = TRUE;
		    break;

		    default:
			doAction = FALSE;
		    break;
		}
	    }

	    if (doSave)
	    {
		doAction = FALSE;	/* will be done by save_done_cb */
		proj_save_needed(save_done_cb);
	    }
        } /* proj_check_unsaved_edits() */

    if (doAction)
    {
	exec_next_command(CG_CMD_UNDEF, 0);
    }

epilogue:
    return return_value;
}


static int
save_done_cb(int status)
{
    if (status < 0)
    {
	return 0;
    }

    exec_next_command(CG_CMD_UNDEF, 0);
    return 0;
}


static int
check_path(void)
{
    int		return_value = 0;
    int		rc = 0;		/* return code */
    BOOL	keepAsking = TRUE;

    if (keepAsking)
    {
	static BOOL	allowWarnUserAboutCc = TRUE;
        static STRING 	ccCmdList[] =
        {
	    "cc", 
	    "/opt/SUNWspro/bin/cc",
	    "/usr/dist/exe/cc", 
	    "/usr/dist/local/exe/cc",
	    NULL
        };
        if ((rc = check_path_to_cmd(ccCmdList, &allowWarnUserAboutCc)) < 0)
	{
	    keepAsking = FALSE;
	}
    }
    
    if (keepAsking)
    {
	static BOOL		allowWarnUserAboutSh = TRUE;
	static STRING	shCmdList[] =
	{
	    "sh",
	    "/bin/sh",
	    "/usr/bin/sh",
	    "/sbin/sh",
	    "/usr/dist/exe/sh",
	    "/usr/dist/local/exe/sh",
	    NULL
	};
        if ((rc = check_path_to_cmd(shCmdList, &allowWarnUserAboutSh)) < 0)
	{
	    keepAsking = FALSE;
	}
    }

    if (keepAsking)
    {
	static BOOL		allowWarnUserAboutRm = TRUE;
	static STRING	rmCmdList[] =
	{
	    "rm",
	    "/bin/rm",
	    "/usr/bin/rm",
	    NULL
	};
        if ((rc = check_path_to_cmd(rmCmdList, &allowWarnUserAboutRm)) < 0)
        {
	    keepAsking = FALSE;
	}
    }

    if ((return_value >= 0) && (!keepAsking))
    {
	return_value = -1;
    }

    return return_value;
}


/*
 * Returns <0 if the user cancelled
 */
static int
check_path_to_cmd(STRING *cmdList, BOOL *allowWarnUserInOut)
{
#define allowWarnUser (*allowWarnUserInOut)
    int			return_value = 0;
    BOOL		userCancelled = FALSE;
    int			rc = 0;		/* return code */
    STRING		foundCmd = NULL;
    DTB_MODAL_ANSWER    answer = DTB_ANSWER_NONE;
    Widget		dlg = NULL;
    XmString		xmMsg = NULL;
    STRING		nopathCmd = NULL;	/* cmd with no path */
    
    nopathCmd = strrchr(cmdList[0], '/');
    if (nopathCmd == NULL)
    {
	nopathCmd = cmdList[0];
    }

    if (allowWarnUser)
    {
        if ((rc = select_command(cmdList, &foundCmd)) < 0)
        {
	    /* command not found, anywhere! */
	    return rc;
        }

        if (!util_streq(foundCmd, nopathCmd))
        {
	    /* not on path! */
	    char	dirName[MAXPATHLEN+1];
	    char	*slashPtr = strrchr(foundCmd, '/');
	    int		dirNameLen = 0;
	    char	buffer[16384];
	    STRING	oldPath = NULL;
	    *buffer = 0;

	    oldPath = cgenP_get_env_var("PATH");
	    if (oldPath == NULL)
	    {
		oldPath = "";
	    }

	    if (slashPtr != NULL)
	    {
		dirNameLen = (int)(slashPtr - foundCmd);
		util_strncpy(dirName, foundCmd, dirNameLen+1);
		/*util_dprintf(2, "directory: '%s'\n", dirName);*/

        	sprintf(buffer, catgets(Dtb_project_catd, 100, 52,
                    "Your PATH does not contain the command %s.\n"
		    "In order to access this command, may I append this\n"
		    "directory to your path?:\n"
		    "\n"
		    "    %s"),
		    	nopathCmd, dirName);
		    
		xmMsg = XmStringCreateLocalized(buffer);
		dtb_cgen_win_modify_path_msg_initialize(
			&dtb_cgen_win_modify_path_msg);
	        answer = dtb_show_modal_message(AB_cgen_win,
			&dtb_cgen_win_modify_path_msg, 
			xmMsg, NULL, &dlg);
		XmStringFree(xmMsg); xmMsg = NULL;

		switch (answer)
		{
		    case DTB_ANSWER_ACTION1:	/* Yes */
			sprintf(buffer, "%s:%s", oldPath, dirName);
			cgenP_put_env_var("PATH", buffer);
		    break;

		    case DTB_ANSWER_ACTION2:	/* No */
			allowWarnUser = FALSE;
		    break;

		    case DTB_ANSWER_CANCEL:
			userCancelled = TRUE;
		    break;
		}
	    }
        }
    }

    if ((return_value >= 0) && userCancelled)
    {
	return_value = -1;
    }
    return return_value;
#undef allowWarnUser
}


/*
 * Makes sure that all of the files for the project at least exist on 
 * the disk.
 */
static BOOL
all_files_exist(ABObj project)
{
    BOOL		allFilesExist = TRUE;
    STRING		fileName = NULL;
    AB_TRAVERSAL	trav;
    ABObj		module = NULL;

    if (   ((fileName= obj_get_file(project)) == NULL)
	|| (!util_file_exists(fileName)) )
    {
	allFilesExist = FALSE;
	abobj_set_save_needed(project, TRUE);
    }

    for (trav_open(&trav, project, AB_TRAV_MODULES); 
	 ((module = trav_next(&trav)) != NULL); )
    {
        if (   ((fileName= obj_get_file(module)) == NULL)
	    || (!util_file_exists(fileName)) )
	{
	    allFilesExist = FALSE;
	    abobj_set_save_needed(module, TRUE);
	}
    }
    trav_close(&trav);

    return allFilesExist;
}


static STRING
cgenP_get_env_var(STRING varName)
{
    STRING value = NULL;
    
    if (user_env_vars != NULL)
    {
        value = (STRING)strlist_get_str_data(user_env_vars, varName);
    }
    if (value == NULL)
    {
	value = getenv(varName);
    }
    return value;
}


/*
 * Creates a duplicate of the value.
 */
static int
cgenP_put_env_var(STRING varName, STRING varValue)
{
    int		strIndex = -1;
    STRING	oldValue = NULL;
    STRING	newValue = NULL;

    if (user_env_vars == NULL)
    {
	user_env_vars = strlist_create();
    }

    strIndex = strlist_get_str_index(user_env_vars, varName);
    if (strIndex >= 0)
    {
	strlist_get_str(user_env_vars, strIndex, (void **)&oldValue);
	if (oldValue != NULL)
	{
	    util_free(oldValue);
	}
	strlist_remove_index(user_env_vars, strIndex); strIndex = -1;
    }

    newValue = strdup(varValue);
    strlist_add_str(user_env_vars, varName, newValue);
    return 0;
}


static int
check_makefile(BOOL *continueOutPtr)
{
    static BOOL	allowDestroyMakefile = TRUE;
    int		return_value = 0;
    int		rc = 0;			/* return code */
    BOOL	makefileExists = FALSE;
    BOOL	makefileIsOK = FALSE;
    DTB_MODAL_ANSWER    answer = DTB_ANSWER_NONE;
    Widget		dlg = NULL;
    BOOL		doDestroyMakefile = FALSE;
    BOOL		doGenMakefile = FALSE;
    BOOL		makeStarted = FALSE;

    *continueOutPtr = TRUE;

    /*
     * Look for makefile
     */
    rc = cgenP_makefile_is_for_project("makefile", proj_get_project());
    if (rc >= 0)
    {
	makefileExists = makefileIsOK = TRUE;
    }
    else if (rc  == ERR_OPEN)
    {
        makefileExists = FALSE;
        makefileIsOK = FALSE;
    }
    else
    {
	makefileExists = TRUE;
	makefileIsOK = FALSE;
    }
    if (makefileIsOK)
    {
	goto epilogue;
    }


    /*
     * Look for Makefile
     */
    if (!makefileExists)
    {
        rc = cgenP_makefile_is_for_project("Makefile", proj_get_project());
        if (rc >= 0)
        {
            makefileExists = makefileIsOK = TRUE;
        }
        else if (rc  == ERR_OPEN)
        {
            makefileExists = FALSE;
            makefileIsOK = FALSE;
        }
        else
        {
	    makefileExists = TRUE;
	    makefileIsOK = FALSE;
        }
        if (makefileIsOK)
        {
	    goto epilogue;
        }
    }

    /*
     * Display a warning dialog
     */
    if (!makefileExists)
    {
	dtb_cgen_win_no_makefile_msg_initialize(&dtb_cgen_win_no_makefile_msg);
	answer = dtb_show_modal_message(AB_cgen_win,
	&dtb_cgen_win_no_makefile_msg, 
	    NULL, NULL, &dlg);
	switch (answer)
	{
	    case DTB_ANSWER_ACTION1:	/* Yes */
		doGenMakefile = TRUE;
	    break;

	    case DTB_ANSWER_ACTION2:	/* No */
		doGenMakefile = FALSE;
	    break;

	    case DTB_ANSWER_CANCEL:
		doGenMakefile = FALSE;
		*continueOutPtr = FALSE;
	    break;
	}
    }
    else if ((!makefileIsOK) && (allowDestroyMakefile))
    {
	dtb_cgen_win_wrong_makefile_msg_initialize(
		&dtb_cgen_win_wrong_makefile_msg);
	answer = dtb_show_modal_message(AB_cgen_win,
		&dtb_cgen_win_wrong_makefile_msg, 
		NULL, NULL, &dlg);
	switch (answer)
	{
	    case DTB_ANSWER_ACTION1:	/* Yes */
		doDestroyMakefile = TRUE;
		doGenMakefile = TRUE;
	    break;

	    case DTB_ANSWER_ACTION2:	/* No */
		doGenMakefile = FALSE;
	    break;

	    case DTB_ANSWER_ACTION3:	/* Never */
		doGenMakefile = FALSE;
		allowDestroyMakefile = FALSE;
	    break;

	    case DTB_ANSWER_CANCEL:
		doGenMakefile = FALSE;
		*continueOutPtr = FALSE;
	    break;
	}
    }

    /*
     * Perform the actions specified by the user
     */
    if (doDestroyMakefile)
    {
	destroy_makefile();
    }
    if (doGenMakefile)
    {
	*continueOutPtr = FALSE;	/* will restart when dtcodegen done */
	if ((rc = exec_generate_main()) >= 0)
	{
	    makeStarted = TRUE;
	    return_value = rc;
	    goto epilogue;
	}
    }


epilogue:
    if (! (makeStarted || (*continueOutPtr)) )
    {
	/* we're not doing anything */
	goto_ready_state();
    }

    return return_value;
}


static int
destroy_makefile(void)
{
    destroy_links_to_file("makefile");
    destroy_links_to_file("Makefile");
    return 0;
}


static int
destroy_links_to_file(STRING fileName)
{
    int			return_value = 0;
    struct stat		doomedFileInfo;
    struct stat		curFileInfo;
    DIR                 *dir = NULL;
    struct dirent       *dirEntry = NULL;
    StringList		doomedFiles = strlist_create();
    int			i = 0;
    int			numFiles = 0;

    strlist_add_str(doomedFiles, fileName, NULL);
    if (stat(fileName, &doomedFileInfo) != 0)
    {
	util_free(doomedFiles);
	return ERR_OPEN;
    }

    dir = opendir(".");
    if (dir == NULL)
    {
	util_free(doomedFiles);
        return ERR_INTERNAL;
    }

    while ((dirEntry= readdir(dir)) != NULL)
    {
        if (stat(dirEntry->d_name, &curFileInfo) != 0)
	{
	    return_value = -1;
	    break;
	}
	if (   (doomedFileInfo.st_dev == curFileInfo.st_dev)
	    && (doomedFileInfo.st_ino == curFileInfo.st_ino) )
	{
	    /* files are the same! */
	    strlist_add_str(doomedFiles, dirEntry->d_name, NULL);
	}
    }

    closedir(dir);

    /*
     * We've built a list of all filenames in the current directory that
     * refer to the given file.
     */
    numFiles = strlist_get_num_strs(doomedFiles);
    for (i = 0; i < numFiles; ++i)
    {
	move_file_to_backup(strlist_get_str(doomedFiles, i, NULL));
    }

    if (dir != NULL)
    {
	closedir(dir); dir = NULL;
    }
    strlist_destroy(doomedFiles);
    return return_value;
}


static int
move_file_to_backup(STRING fileName)
{
    char	bakNameBuf[MAXPATHLEN+1];
    int		fileNameLen = strlen(fileName);

    if ((fileNameLen >= 4) && (strcmp(fileName+fileNameLen-4, ".BAK") == 0))
    {
	/* Don't make a .BAK.BAK file */
	return 0;
    }

    sprintf(bakNameBuf, "%s.BAK", fileName);
    unlink(bakNameBuf);
    return rename(fileName, bakNameBuf);
}


static int
cgenP_makefile_is_for_project(STRING fileName, ABObj project)
{
    int			return_value = 0;
    FILE		*makeFile = NULL;
    StringList		genFileNames = strlist_create();
    ABObj		module = NULL;
    int			i = 0;
    int			numFiles = 0;
    AB_TRAVERSAL	trav;
    assert((project == NULL) || obj_is_project(project));

    if (project == NULL)
    {
	goto epilogue;
    }

    makeFile = util_fopen_locked(fileName, "r");
    if (makeFile == NULL)
    {
	return_value = ERR_OPEN;
	goto epilogue;
    }

    /*
     * Create list of file names
     */
    add_obj_file_name(genFileNames, project, NULL);
    for (trav_open(&trav, project, AB_TRAV_MODULES);
	(module = trav_next(&trav)) != NULL; )
    {
	add_obj_file_name(genFileNames, module, "_ui");
    }
    trav_close(&trav);

    if (!strings_exist_in_file(genFileNames, makeFile))
    {
	return_value = ERR;
    }

epilogue:
    util_fclose(makeFile);
    strlist_destroy(genFileNames);
    return return_value;
}


static int
add_obj_file_name(StringList fileNames, ABObj obj, STRING suffix)
{
    STRING	objName = NULL;
    char	fileName[MAXPATHLEN+1];
    *fileName = 0;

    if (obj == NULL)
    {
	return -1;
    }
    if ((objName = obj_get_name(obj)) == NULL)
    {
	return -1;
    }

    if (suffix == NULL)
    {
	suffix = Util_empty_string;
    }
    sprintf(fileName, "%s%s", objName, suffix);
    strlist_add_str(fileNames, fileName, NULL);
    return 0;
}


static BOOL
strings_exist_in_file(StringList strings, FILE *file)
{
#define fast_strneq(s1,s2,n) \
	    (((*(s1)) == (*(s2))) && (strncmp(s1,s2,n) == 0))
    BOOL	stringsExist = FALSE;
    STRING	*stringsArray = NULL;
    int		*stringsLenArray = NULL;
    int		maxFileNameLen = 0;
    int		curFileNameLen = 0;
    STRING	curFileName = NULL;
    char	buf[8193];
    int		numStrings = 0;
    int		numStringsFound = 0;
    int		i = 0;
    int		maxBufLen = sizeof(buf)-1;
    int		c = 0;
    int		bufLen = 0;
    BOOL	*stringExists = NULL;
    char	*stringStart = NULL;
    *buf = 0;

    numStrings = strlist_get_num_strs(strings);

    /*
     * Convert strings list to arrays, to avoid 750,000 calls to istr_string()
     */
    stringsArray = (STRING*)util_malloc(numStrings * sizeof(STRING));
    stringsLenArray = (int*)util_malloc(numStrings * sizeof(int));
    stringExists = (BOOL*)util_malloc(numStrings * sizeof(BOOL));
    if (   (stringsArray == NULL) 
	|| (stringsLenArray == NULL) 
	|| (stringExists == NULL) )
    {
	goto epilogue;
    }
    for (i = 0; i < numStrings; ++i)
    {
	stringsArray[i] = strlist_get_str(strings, i, NULL);
	stringsLenArray[i] = strlen(stringsArray[i]);
	stringExists[i] = FALSE;
    }


    /*
     * Determine the longest file name
     */
    for (i = 0; i < numStrings; ++i)
    {
	curFileName = stringsArray[i];
	curFileNameLen = stringsLenArray[i];
	maxFileNameLen = util_max(maxFileNameLen, curFileNameLen);
    }
    assert(maxFileNameLen < sizeof(buf));

    /*
     * scan the file
     */
    rewind(file);
    bufLen = fread((void *)buf, 1, maxFileNameLen -1, file);
    while ((c = fgetc(file)) != EOF)
    {
	if (bufLen >= maxBufLen)
	{
	    /* we've reached the end of the buffer */
	    memmove(buf, buf + maxBufLen - maxFileNameLen, maxFileNameLen);
	    bufLen = maxFileNameLen;
	}
	buf[bufLen++] = c;

	for (i = 0; i < numStrings; ++i)
	{
	    if (stringExists[i])
	    {
		continue;
	    }
	    stringStart = buf + bufLen - stringsLenArray[i];
	    if (fast_strneq(stringStart, stringsArray[i], stringsLenArray[i]))
	    {
		stringExists[i] = TRUE;
		if (++numStringsFound >= numStrings)
		{
		    /*
		     * Instead of a loop-control variable that must get
		     * checked on each iteration, we use a goto to speed
		     * things up, dramatically.
		     */
		    goto exit_file_loop;
		    break;
		}
	    }
	}
    }
exit_file_loop:

    /*
     * See if they were all found
     */
    stringsExist = TRUE;
    for (i = 0; i < numStrings; ++i)
    {
	if (!stringExists[i])
	{
	    stringsExist = FALSE;
	    if (!debugging())
	    {
	        break;
	    }
#ifdef DEBUG
	    util_dprintf(1, "Not in makefile: '%s'\n", stringsArray[i]);
#endif /* DEBUG */
	}
    }

epilogue:
    util_free(stringsArray);
    util_free(stringsLenArray);
    util_free(stringExists);
    return stringsExist;
#undef fast_strneq
}


static int
select_command(STRING *cmdList, STRING *cmdOutPtr)
{
    int		i = 0;
    int		cmdIndex = -1;
    STRING	path = NULL;

    *cmdOutPtr = NULL;
    path = cgenP_get_env_var("PATH");

    for (i = 0; (cmdIndex < 0) && (cmdList[i] != NULL); ++i)
    {
	if ((strlen(cmdList[i]) > 0) && (command_exists(cmdList[i], path)))
	{
	    cmdIndex = i;
	    break;
	}
    }

    if (cmdIndex >= 0)
    {
	*cmdOutPtr = cmdList[cmdIndex];
    }
    return cmdIndex;
}


static BOOL
command_exists(STRING cmd, STRING path)
{
    static uid_t	euid = (uid_t)-1;
    static uid_t	egid = (uid_t)-1;
    BOOL	cmdExists = FALSE;
    char	szCurrentPath[MAXPATHLEN+1];
    int		iCurrentPathStart = -1;
    int		iCurrentPathLen = -1;
    int		iPathLen = -1;
    int		i = 0;
    int		iExeNameLen = strlen(cmd);
    BOOL	moreDirs = FALSE;
    *szCurrentPath = 0;

    if (euid == (uid_t)-1)
    {
	euid = geteuid();
	egid = getegid();
    }

    /*
     * Check for abolute path to command
     */
    if (   (strncmp(cmd, "/", 1) == 0)
	|| (strncmp(cmd, "./", 2) == 0)
	|| (strncmp(cmd, "../", 3) == 0)
       )
    {
	/* an absolute path */
	cmdExists = path_is_executable(cmd, euid, egid);
	goto epilogue;
    }

    /*
     * Search path for command
     */
    iCurrentPathStart = 0;
    iCurrentPathLen = 0;
    iPathLen = strlen(path);
    moreDirs = TRUE;

    while ((!cmdExists) && (moreDirs))
    {
	/* find beginning of dir name (skip ':') */
	while (   (iCurrentPathStart < iPathLen) 
	       && (path[iCurrentPathStart] == ':'))
	{
	    ++iCurrentPathStart;	/* skip : */
	}
	if (iCurrentPathStart >= iPathLen)
	{
	    moreDirs = FALSE;
	    continue;
	}

        /* find end of dir name */
        for (i= iCurrentPathStart; (i < iPathLen) && (path[i] != ':'); )
        {
            ++i;
        }
        iCurrentPathLen= i - iCurrentPathStart;

	/* make sure path to executable is not too long */
	if ((iCurrentPathLen + iExeNameLen + 2) > MAXPATHLEN)
	{
	    iCurrentPathLen= MAXPATHLEN - (iExeNameLen + 2);
	}

	/* create a possible path to the executable */
	util_strncpy(szCurrentPath, &path[iCurrentPathStart], 
				iCurrentPathLen+1);
	strcat(szCurrentPath, "/");
	strcat(szCurrentPath, cmd);

	/* see if the executable exists (and we can execute it) */
	if (path_is_executable(szCurrentPath, euid, egid))
	{
	    cmdExists = True;
	}

	/* skip past the current directory name */
	iCurrentPathStart += iCurrentPathLen;
    } /* while !cmdExists */

epilogue:
    return cmdExists;
}

/*
 * returns False is path does not exist or is not executable
 */
static Boolean
path_is_executable(
    char 	*path,
    uid_t	euid,
    gid_t 	egid
)
{
    Boolean	bExecutable= False;
    struct stat	sStat;

    /* util_dprintf(3, "path_is_executable(%s)\n", path); */
    if (stat(path, &sStat) == 0)
    {
	Boolean	bDetermined= False;

	if (!bDetermined)
	{
	    if (!S_ISREG(sStat.st_mode))
	    {
		/* not a regular file */
		bDetermined= True;
		bExecutable= False;
	    }
	}

	if (!bDetermined)
	{
	    if (   (euid == 0) 
	        && (   ((sStat.st_mode & S_IXOTH) != 0)
		    || ((sStat.st_mode & S_IXGRP) != 0)
		    || ((sStat.st_mode & S_IXUSR) != 0) )
	       )
	    {
		bDetermined= True;
		bExecutable= True;
	    }
	}

	if (!bDetermined)
	{
	    if (   (((sStat.st_mode & S_IXOTH) != 0)    )
		|| (((sStat.st_mode & S_IXGRP) != 0) && (sStat.st_gid == egid))
		|| (((sStat.st_mode & S_IXUSR) != 0) && (sStat.st_gid == euid))
	       )
	    {
		bDetermined= True;
	        bExecutable= True;
	    }
	}
    } /* if stat */

    return bExecutable;
}


/*
 * This routine must be identical to that in src/abmf/obj_names.c. If
 * either this one or the other one is changed, copy the routine to the
 * other file.
 */
static STRING
cvt_type_to_ident(STRING type, STRING identBuf, int identBufSize)
{
    int		typeOff = 0;
    int		identOff = 0;
    int		typeChar = -1;
    int		typeLen = util_strlen(type);
    int		identMaxLen = identBufSize-1;
    int		lastIdentChar = -1;
    BOOL	lastTypeCharWasUpper = FALSE;

    for (typeOff = 0; 
	    (typeOff < typeLen) && (identOff < (identMaxLen-1)); ++typeOff)
    {
	typeChar = type[typeOff];
	if (isupper(typeChar))
	{
	    if (   (lastIdentChar != '_') 
		&& (!lastTypeCharWasUpper)
		&& (lastIdentChar != -1)
	       )
	    {
		lastIdentChar = identBuf[identOff++] = '_';
	    }
	    if (identOff < (identMaxLen-1))
	    {
	        lastIdentChar = identBuf[identOff++] = tolower(typeChar);
	    }
	    lastTypeCharWasUpper = TRUE;
	}
	else
	{
	    lastIdentChar = identBuf[identOff++] = typeChar;
	    lastTypeCharWasUpper = FALSE;
	}
    }
    identBuf[identOff] = 0;

    return identBuf;
}


/*
 * util_fdsync() syncs the data and IO pending on an open file descriptor
 * out to the physical device, pipe, stream, or whatever.
 *
 * REMIND: move this to libAButil
 */
#ifdef __cplusplus
extern "C" {
#endif

extern int fsync(int fd);		/* non-POSIX function */

#ifdef __cplusplus
} //extern "C"
#endif

static int
util_fdsync(int fd)
{
    BOOL	ok = FALSE;	/* OK if either sync or datasync works */
    if (fsync(fd) == 0)
    {
	ok = TRUE;
    }

    return ok?0:-1;
}

