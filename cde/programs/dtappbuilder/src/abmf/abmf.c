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

/*
 * $XConsortium: abmf.c /main/6 1996/11/21 12:31:02 mustafa $
 * 
 * @(#)abmf.c   3.53 16 Feb 1994      cde_app_builder/src/abmf
 * 
 * RESTRICTED CONFIDENTIAL INFORMATION:
 * 
 * The information in this document is subject to special restrictions in a
 * confidential disclosure agreement between HP, IBM, Sun, USL, SCO and
 * Univel.  Do not distribute this document outside HP, IBM, Sun, USL, SCO,
 * or Univel without Sun's specific written approval.  This document and all
 * copies and derivative works thereof must be returned or destroyed at Sun's
 * request.
 * 
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/times.h>
#include <time.h>
#include <limits.h>
#include <ctype.h>
#include <nl_types.h>
#include <ab_private/util.h>
#include <ab_private/obj.h>
#include <ab_private/objxm.h>
#include <ab_private/bil.h>
#include "dtb_utils.h"
#include "write_codeP.h"
#include "motifdefsP.h"
#include "argsP.h"
#include "lib_func_stringsP.h"
#include "instancesP.h"
#include "ui_header_fileP.h"
#include "abmfP.h"
#include "abmf.h"

/* glibc considers CLK_TCK obsolete */
#if defined(linux) && !defined(CLK_TCK)
#define CLK_TCK CLOCKS_PER_SEC
#endif

#ifdef __cplusplus
extern "C" {
#endif
extern long _sysconf(int name);		/* CLK_TCK uses this */
#ifdef __cplusplus
} // extern "C"
#endif

typedef enum
{
    ABMF_PROFILE_UNDEF = 0,
    ABMF_PROFILE_LOAD,
    ABMF_PROFILE_CONFIG,
    ABMF_PROFILE_TRAVERSALS,
    ABMF_PROFILE_WHAT_NUM_VALUES
} ABMF_PROFILE_WHAT;


typedef struct
{
    BOOL			force_load_all;
    ABMF_PROFILE_WHAT		debug_profile_what;
    ABMF_CGEN_RESTRICTION	cgen_restriction;
    BOOL			dump_tree;
    StringList			files;
    BOOL               		i18n;
    BOOL               		write_main;
    BOOL			show_all_windows;
    BOOL			show_all_windows_set;
    BOOL               		merge_files;
    ISTRING            		proj_file;
    BOOL               		prototype_funcs;
    BOOL               		source_browser;
    BOOL			use_default_project;
    int                		verbosity;
    BOOL               		write_all_resources;
    BOOL               		write_i18n_resources;
} CmdlineArgsRec, *CmdlineArgs;

BOOL	freshenUnchangedFiles = TRUE;

/*
 * Private Functions
 */
static int      abmf_init(void);
static int      abmf_usage(void);
static int      parse_args(int argc, char *argv[], CmdlineArgs args);
static int      find_proj_file(CmdlineArgs args);
static int	mark_modules_to_load_and_write(
			ABObj		project, 
			BOOL		loadAllModules, 
			BOOL		genAllModules, 
			BOOL		genNoModules,
			StringList	fileNames
		);
static int	load_marked_modules(ABObj project);
static int	load_module(ABObj module);
static BOOL     write_required(ABObj tree);
static int      examine_tree(ABObj project);
static int	munge_ensure_win_parent(ABObj win);
static int	munge_liberate_menu(ABObj menu);
static int	dup_all_menu_refs(ABObj project);
static int	abmfP_dup_menu_ref_tree(ABObj obj);
static int	abmfP_do_dup_menu_ref_tree(ABObj obj);
static int	dump_tree(ABObj tree);
static int	print_tree(ABObj root, int indent);
static int	abmfP_prepare_tree(ABObj project);
static int	abmfP_create_obj_data_for_project(ABObj project);
static int	abmfP_create_obj_data_for_module(ABObj objInModule);
static int	ensure_data_for_module_obj(ABObj module);
static BOOL	proj_file_has_modules(
			STRING		projFileName, 
			StringList	fileNames
		);
static int	replace_string_shorter(
			STRING buf, 
			STRING subStr, 
			STRING replaceStr
		);



/*
 * Debugging routines
 */
#ifdef DEBUG
static int	dump_callbacks(ABObj project);
static int	debug_verify_tree(ABObj root);
#endif /* DEBUG */

#ifdef DEBUG	/* performance testing */
static int	time_traversal(ABObj root);
static int	get_cur_time(double *realTimeOut, double *cpuTimeOut);
#endif /* DEBUG */


/* Internationalization defines */
nl_catd Dtb_project_catd;

/* Workaround for XPG4 API compatibility */
#if !defined(NL_CAT_LOCALE)
#define NL_CAT_LOCALE 0
#endif


/* ARGSUSED */
int
main(int argc, STRING *argv)
{
    int			exitValue = 0;
    int                 iRC= 0;         /* int return code */
    CmdlineArgsRec      cmdlineRec;
    CmdlineArgs         cmdline = &cmdlineRec;
    ABObj               project = NULL;
    ABObj               module = NULL;
    int                 num_modules_processed = 0;
    BOOL		genAllFiles = FALSE;
    BOOL		genMain = FALSE;
    BOOL		genMainOnly = FALSE;
    BOOL		useDefaultProject = FALSE;
    BOOL		showAllWindows = FALSE;
    STRING		errmsg = NULL;

#ifdef DEBUG	/* performance testing */
    double		progStartSeconds = 0.0;
    double		progEndSeconds = 0.0;
    double		progSeconds = 0.0;
    double		progStartCPUSeconds = 0.0;
    double		progEndCPUSeconds = 0.0;
    double		progCPUSeconds = 0.0;
    double		configStartSeconds = 0.0;
    double		configEndSeconds = 0.0;
    double		configSeconds = 0.0;
    double		configStartCPUSeconds = 0.0;
    double		configEndCPUSeconds = 0.0;
    double		configCPUSeconds = 0.0;
    double		loadStartSeconds = 0.0;
    double		loadEndSeconds = 0.0;
    double		loadSeconds = 0.0;
    double		loadStartCPUSeconds = 0.0;
    double		loadEndCPUSeconds = 0.0;
    double		loadCPUSeconds = 0.0;
    double		startupStartSeconds = 0.0;
    double		startupEndSeconds = 0.0;
    double		startupSeconds = 0.0;
    double		startupStartCPUSeconds = 0.0;
    double		startupEndCPUSeconds = 0.0;
    double		startupCPUSeconds = 0.0;
    get_cur_time(&progStartSeconds, &progStartCPUSeconds);
    startupStartSeconds = loadStartSeconds = configStartSeconds
				= progStartSeconds;
    startupStartCPUSeconds = loadStartCPUSeconds = configStartCPUSeconds
				= progStartCPUSeconds;
#endif /* DEBUG */

    /*
     * Open the standard message catalog for the project.
     */
    Dtb_project_catd = catopen("dtcodegen", NL_CAT_LOCALE);
    if (Dtb_project_catd == (nl_catd)-1)
    {
        fprintf(stderr, 
	    "WARNING: Could not open message catalog: dtcodegen.cat\n");
    }

    util_init(&argc, &argv);
    dtb_save_command(argv[0]);
    objxm_init(NULL);
    abmf_init();

    if ((iRC = parse_args(argc, argv, cmdline)) < 0)
    {
        exit(1);
    }
    util_set_verbosity(cmdline->verbosity);

    /*********************************************************************
     **									**
     **		 Figure out what the user wants				**
     **									**
     *********************************************************************/

    /*
     * Find the project file
     */
    if (cmdline->use_default_project)
    {
	useDefaultProject = TRUE;
    }
    else if (cmdline->proj_file == NULL)
    {
        int numProjFiles = find_proj_file(cmdline);
	if (cmdline->proj_file == NULL)
	{
	    if (!strlist_is_empty(cmdline->files))
	    {
		useDefaultProject = TRUE;
	        if (!util_be_silent())
	        {
	            util_printf(
			catgets(Dtb_project_catd, 1, 1,
	    "No project file that references module(s) - using defaults\n"));
	        }
	    }
	    else if (numProjFiles == 0)
	    {
		util_printf(
		    catgets(Dtb_project_catd, 1, 2,
		      "No files specified, and no project file found\n"));
		abmf_usage();
	    }
	    else
	    {
		/* assume find_proj_file() complained sufficiently */
	        exit(1);
	    }
	}
    }

    genMain = FALSE;
    genMainOnly = FALSE;
    genAllFiles = FALSE;
    if (strlist_is_empty(cmdline->files))
    {
	if (cmdline->write_main)
	{
	    /* only -main was specified */
	    genMain = TRUE;
	    genMainOnly = TRUE;
	}
	else
	{
	    /* no specific files requested - gen them all */
	    genAllFiles = TRUE;
	    genMain = TRUE;
	}
    }
    else
    {
	/* files specified */
	if (cmdline->write_main)
	{
	    genMain = TRUE;
	    if (useDefaultProject)
	    {
		/* dtcodegen -np module.bil -main : only gen main */
		genMainOnly = TRUE;
	    }
	}
	else
	{
	    if (useDefaultProject)
	    {
		genAllFiles = TRUE;
		genMain = TRUE;
	    }
	}
    }

    if (cmdline->show_all_windows_set)
    {
	showAllWindows = cmdline->show_all_windows;
    }
    else if (useDefaultProject)
    {
	/* user did not override, so we're going to set show_all_windows */
	showAllWindows = TRUE;
    }

    util_dprintf(1, "main:%d mainonly:%d all:%d defaultproj:%d showall:%d\n",
	genMain, genMainOnly, genAllFiles, useDefaultProject, showAllWindows);

    if ((!util_be_silent()) && (cmdline->proj_file != NULL))
    {
	char *proj_file_name = istr_string_safe(cmdline->proj_file);
        util_printf(
	    catgets(Dtb_project_catd, 1, 3, "Reading project %s.\n"),
                proj_file_name);
    }

#ifdef DEBUG	/* performance testing */
    get_cur_time(&loadStartSeconds, &loadStartCPUSeconds);
#endif

    /*********************************************************************
     **									**
     **			Load or create the project			**
     **									**
     *********************************************************************/

    if (cmdline->proj_file != NULL)
    {
	/* project = bil_load_file(istr_string(cmdline->proj_file), NULL, project);
	assert(project != NULL); */

        iRC = bil_load_file_and_resolve_all(
				istr_string(cmdline->proj_file), NULL, &project);
	if (iRC < 0)
	{
	    exitValue = 1;
	    goto epilogue;
	}
    }

    if (useDefaultProject)
    {
	int	i = 0;
	int	numModuleFiles = 0;
	STRING	moduleFileName = NULL;
	ABObj	fileObj = NULL;
	char	*dotPtr = NULL;
	char	projNameBuf[1024];
	*projNameBuf = 0;

	/*
	 * Create the default project
	 */
	if (project != NULL)
	{
	    ABObj oldProject = project;
	    project = obj_create(AB_TYPE_PROJECT, NULL);
	    obj_move_children(project, oldProject);
	    obj_destroy(oldProject);
	}
	else
	{
	    project = obj_create(AB_TYPE_PROJECT, NULL);
	}
	util_strncpy(projNameBuf, 
			strlist_get_str(cmdline->files, 0, NULL), 1024);
	dotPtr = strrchr(projNameBuf, '.');
	if (dotPtr != NULL)
	{
	    *dotPtr = 0;
	}
	obj_set_name(project, projNameBuf);
	obj_set_file(project, strlist_get_str(cmdline->files, 0, NULL));
	obj_set_is_default(project, TRUE);

	/*
	 * Create "files" to get converted to undefined modules
	 */
	numModuleFiles = strlist_get_num_strs(cmdline->files);
	for (i = 0; i < numModuleFiles; ++i)
	{
	    moduleFileName = strlist_get_str(cmdline->files, i, NULL);
	    fileObj = obj_create(AB_TYPE_FILE, project);
	    obj_set_file(fileObj, moduleFileName);
	}
    }

    /*
     * Load the appropriate files
     */
    if (genMain)
    {
	mfobj_set_flags(project, CGenFlagLoadMe|CGenFlagIsReferenced);
	obj_set_write_me(project, TRUE);
    }
    mark_modules_to_load_and_write(
	project, cmdline->force_load_all, genAllFiles, genMainOnly, cmdline->files);
    load_marked_modules(project);

#ifdef DEBUG	/* performance testing */
    get_cur_time(&loadEndSeconds, &loadEndCPUSeconds);
    loadSeconds = loadEndSeconds - loadStartSeconds;
    loadCPUSeconds = loadEndCPUSeconds - loadStartCPUSeconds;
    printf("load real:%lg  CPU:%lg\n", loadSeconds, loadCPUSeconds);
#endif /* DEBUG */

    if (iRC < 0)
    {
	char *prog_name_string = util_get_program_name();

        fprintf(stderr, 
	    catgets(Dtb_project_catd, 1, 4,
	        "%s: exiting due to error loading project.\n"), 
		prog_name_string);
        exit(1);
    }
    if (cmdline->debug_profile_what == ABMF_PROFILE_LOAD)
    {
	exit(0);
    }

    /*
     * See if we actually have anything to write
     */
    if (!write_required(project))
    {
	char *prog_name_string = util_get_program_name();

        fprintf(stderr, 
	    catgets(Dtb_project_catd, 1, 5,
	      "%s: Nothing to do!\n"), prog_name_string);
        exit(1);
    }

    /*
     * Configure the raw objects into AB objects
     */
    if (showAllWindows)
    {
	ABObj	window = NULL;
	AB_TRAVERSAL	trav;
	for (trav_open(&trav, project, AB_TRAV_WINDOWS);
	    (window = trav_next(&trav)) != NULL; )
	{
	    obj_set_is_initially_visible(window, TRUE);
	}
	trav_close(&trav);
    }

#ifdef DEBUG	/* performance testing */
    get_cur_time(&configStartSeconds, &configStartCPUSeconds);
#endif

    examine_tree(project);

#ifdef DEBUG	/* performance testing */
    get_cur_time(&configEndSeconds, &configEndCPUSeconds);
    configSeconds = configEndSeconds - configStartSeconds;
    configCPUSeconds = configEndCPUSeconds - configStartCPUSeconds;
    get_cur_time(&startupEndSeconds, &startupEndCPUSeconds);
    startupSeconds = startupEndSeconds - startupStartSeconds;
    startupCPUSeconds = startupEndCPUSeconds - startupStartCPUSeconds;

    /*
     * Print out startup stats (if debugging build)
     */
    util_printf("Startup real time: %lg s (%lg s load, %lg s config)\n",
		startupSeconds, loadSeconds, configSeconds);
    util_printf("Startup CPU time: %lg s (%lg s load, %lg s config)\n",
		startupCPUSeconds, loadCPUSeconds, configCPUSeconds);
#endif /* DEBUG */

#ifdef DEBUG
    util_dprintf(1, "after configure_tree\n");
    debug_verify_tree(project);
#endif /* DEBUG */

    /*
     * The tree has been loaded, examined, mangled, and generally messed with.
     * Now, do what the user has requested.
     */

#ifdef DEBUG
    /* make sure we didn't muss anything up */
    if (debugging())
    {
	int	numObjs = trav_count(project, AB_TRAV_ALL);
	obj_tree_verify(project);
        if (   ((debug_level() >= 10) || ((25*debug_level()) >= numObjs))
	    && (!cmdline->dump_tree))
        {
            objxm_tree_print(project);
        }
    }
#endif /* DEBUG */

    if (cmdline->dump_tree)
    {
	dump_tree(project);
    }
    else
    {
	AB_ARG_CLASS_FLAGS dumpedRes = obj_get_res_file_arg_classes(project);
	ABMF_I18N_METHOD   i18n_method = ABMF_I18N_NONE;

	if (obj_get_i18n_enabled(project))
	    i18n_method = ABMF_I18N_XPG4_API;

        /*
         * Generate the code!
         */
        iRC = abmf_generate_code(
                         project,
		         cmdline->cgen_restriction,
                         cmdline->merge_files,
                         i18n_method,
                         cmdline->prototype_funcs,
			 dumpedRes
            );
	if (iRC < 0)
	{
	    exitValue = 1;
	}
    }

#ifdef DEBUG	/* performance testing */
    get_cur_time(&progEndSeconds, &progEndCPUSeconds);
    progSeconds = progEndSeconds - progStartSeconds;
    progCPUSeconds = progEndCPUSeconds - progStartCPUSeconds;
    util_printf("Total real time: %lg s   CPU time: %lg s\n",
	progSeconds, progCPUSeconds);
#endif /* DEBUG */

epilogue:
#ifdef DEBUG
    debug_verify_tree(project);
#endif /* DEBUG */

    return exitValue;
}


static int
parse_args(int argc, char *argv[], CmdlineArgs cmdline)
{
    int                 iReturn = 0;
    int                 argCount = 0;

    /*
     * Set default values
     */
    cmdline->force_load_all = TRUE;
    cmdline->debug_profile_what = ABMF_PROFILE_UNDEF;
    cmdline->cgen_restriction = ABMF_CGEN_ALL;
    cmdline->dump_tree = FALSE;
    cmdline->files = strlist_create();
    cmdline->i18n = FALSE;
    cmdline->show_all_windows = FALSE;
    cmdline->show_all_windows_set = FALSE;
    cmdline->write_main = FALSE;
    cmdline->merge_files = TRUE;
    cmdline->proj_file = NULL;
    cmdline->prototype_funcs = TRUE;
    cmdline->source_browser = FALSE;
    cmdline->use_default_project = FALSE;
    cmdline->verbosity = util_get_verbosity();
    cmdline->write_all_resources = FALSE;
    cmdline->write_i18n_resources = FALSE;

    for (argCount = 1; argCount < argc; ++argCount)
    {
        char               *arg = argv[argCount];
        if (*arg == '-')
        {
            if (strcmp(arg, "-s") == 0 ||
                strcmp(arg, "-silent") == 0)
            {
                cmdline->verbosity = 0;
            }
            else if (strcmp(arg, "-a") == 0 ||
                     strcmp(arg, "-ansi") == 0)
            {
                cmdline->prototype_funcs = TRUE;
            }
	    else if (util_streq(arg, "-freshen"))
	    {
		freshenUnchangedFiles = TRUE;
	    }
	    else if (util_streq(arg, "-nofreshen"))
	    {
		freshenUnchangedFiles = FALSE;
	    }
#ifdef DEBUG
	    else if (util_streq(arg, "-tree"))
	    {
		cmdline->dump_tree = TRUE;
	    }
#endif /* DEBUG */
	    else if (util_streq(arg, "-changed"))
	    {
		cmdline->cgen_restriction = ABMF_CGEN_BY_DATE;
	    }
            else if (strcmp(arg, "-k") == 0 ||
                     strcmp(arg, "-kandr") == 0)
            {
                cmdline->prototype_funcs = FALSE;
            }
            else if (strcmp(arg, "-main") == 0)
            {
                cmdline->write_main = TRUE;
            }
	    else if (util_streq(arg, "-showall"))
	    {
		cmdline->show_all_windows = TRUE;
		cmdline->show_all_windows_set = TRUE;
	    }
	    else if (util_streq(arg, "-noshowall"))
	    {
		cmdline->show_all_windows = FALSE;
		cmdline->show_all_windows_set = TRUE;
	    }
            else if (util_streq(arg, "-merge"))
            {
                cmdline->merge_files = TRUE;
            }
            else if (util_streq(arg, "-nomerge"))
            {
                cmdline->merge_files = FALSE;
            }
            else if (strcmp(arg, "-P") == 0 ||
                     strcmp(arg, "-p") == 0 ||
                     strcmp(arg, "-project") == 0)
            {
                if ((argCount + 1) >= argc)
                {
                    util_error( catgets(Dtb_project_catd, 1, 6,
		    "Missing project name for -p option"));
                    abmf_usage();
                }
                else
                {

                    /*
                     * get the project file name
                     */
                    STRING      projArg;
                    ++argCount;
                    projArg= argv[argCount];
                    if (   util_file_name_has_extension(projArg, "bip")
                        || util_file_name_has_extension(projArg, "bix"))
                    {
                        cmdline->proj_file = istr_const(projArg);
                    }
                    else
                    {
                        char                projFile[MAX_PATH_SIZE];
                        sprintf(projFile, "%s.bip", projArg);
			if (!util_file_exists(projFile))
			{
			    char	encapsFile[MAXPATHLEN];
			    sprintf(encapsFile, "%s.bix", projArg);
			    if (util_file_exists(encapsFile))
			    {
				strcpy(projFile, encapsFile);
			    }
			}
                        cmdline->proj_file = istr_create(projFile);
                    }
                }
            }
	    else if (   util_streq(arg, "-np")
		     || util_streq(arg, "-noproject"))
	    {
	        cmdline->use_default_project = TRUE;
	    }
            else if (strcmp(arg, "-help") == 0)
            {
                abmf_usage();
            }
            else if (strcmp(arg, "-i") == 0 ||
                     strcmp(arg, "-i18n") == 0)
            {
                cmdline->i18n = TRUE;
            }
            else if (strcmp(arg, "-r") == 0 ||
                     strcmp(arg, "-resources") == 0)
            {
                cmdline->write_all_resources = TRUE;
            }
            else if (strcmp(arg, "-sb") == 0)
            {
                cmdline->source_browser = TRUE;
            }
            else if (strncmp(arg, "-v", 2) == 0)
            {
		cmdline->verbosity = 2;
            }

	    /* 
	     * debugging options 
	     */
#ifdef DEBUG
	    else if (strncmp(arg, "-V", 2) == 0)
	    {
                int                 i;
                for (i = 1; arg[i] == 'V'; ++i) 
		    { /* empty */ }
                --i;
                cmdline->verbosity = i;
	    }
	    else if (strncmp(arg, "-time", 5) == 0)
	    {
		if (argCount < (argc-1))
		{
		    arg = argv[++argCount];
		    if (util_streq(arg, "load"))
		    {
			cmdline->debug_profile_what = ABMF_PROFILE_LOAD;
		    }
		}
	    }
	    else if (strncmp(arg, "-loadall", 8) == 0)
	    {
		cmdline->force_load_all = TRUE;
	    }
#endif /* DEBUG */
            else
            {
		char *prog_name_string = util_get_program_name();

                fprintf(stderr, catgets(Dtb_project_catd, 1, 7,
		    "%s: Illegal option \"%s\"\n\n"), prog_name_string, arg);
                abmf_usage();
            }

        }                       /* arg == '-' */
        else
        {

            /*
             * It's a file name
             */
            if (   util_file_name_has_extension(arg, "bip")
                || util_file_name_has_extension(arg, "bix") )
            {
                if (cmdline->proj_file != NULL)
                {
                    util_error(
			catgets(Dtb_project_catd, 1, 8,
		    "Only one project file may be specified."));
                    abmf_usage();
                }
                else
                {
                    cmdline->proj_file = istr_const(arg);
                }
            }
            else
            {
                ISTRING             newFile = NULL;

                if (util_file_name_has_ab_extension(arg))
                {
                    newFile = istr_const(arg);
                }
                else
                {
                    char                fileName[MAX_PATH_SIZE];
                    strcpy(fileName, arg);
                    strcat(fileName, ".bil");
                    newFile = istr_create(fileName);
                }

		strlist_add_istr(cmdline->files, newFile, NULL);
            }                   /* not project file */
        }                       /* ! flag arg */
    } /* for argCount */


    /*
     * Make the comand-line args consistent
     */
    if (cmdline->use_default_project && (cmdline->proj_file != NULL))
    {
	char *proj_file_name = istr_string_safe(cmdline->proj_file);

        util_printf_err(
	  catgets(Dtb_project_catd, 1, 9,
          "ignoring -noproject (-np) because project file was specified: %s\n"),
	       proj_file_name);
	cmdline->use_default_project = FALSE;
    }

    return iReturn;
}


static int
mark_modules_to_load_and_write(
			ABObj		project, 
			BOOL		loadAllModules, 
			BOOL		genAllModules,
			BOOL		genNoModules,
			StringList	fileNames
)
{
    int			return_value = 0;
    AB_TRAVERSAL	fileTrav;
    ABObj		file = NULL;
    ISTRING		fileName = NULL;
    char		newModuleName[1024];
    STRING		dotPtr = NULL;
    AB_TRAVERSAL	allTrav;
    ABObj		obj = NULL;
    ABObj		obj1 = NULL;
    ABObj		obj2 = NULL;
    AB_TRAVERSAL	moduleTrav;
    ABObj		module = NULL;
    ABObj		winParent = NULL;
    *newModuleName = 0;

    /*
     * Convert remaining files into undefined modules
     */
    for (trav_open(&fileTrav, project, AB_TRAV_FILES | AB_TRAV_MOD_SAFE);
	(file = trav_next(&fileTrav)) != NULL; )
    {
	fileName = istr_create(obj_get_file(file));
	util_get_file_name_from_path(
			istr_string(fileName), newModuleName, 1024);
	dotPtr = strrchr(newModuleName, '.');
	if (dotPtr != NULL)
	{
	    *dotPtr = 0;
	}

	module = obj_find_module_by_name(project, newModuleName);
	if (module != NULL)
	{
	    /* this module already exists, probably because of a forward */
	    /* reference */
	    obj_destroy(file);
	}
	else
	{
	    obj_set_type(file, AB_TYPE_MODULE);
	    module = file; file = NULL;
	    obj_set_name(module, newModuleName);
	    obj_set_is_defined(module, FALSE);
	    mfobj_set_flags(module, CGenFlagLoadMe|CGenFlagIsReferenced);
	}

	/*
	 * Objects/modules created as a forward reference will
	 * not have their file names set, so we're going to
	 * check it.
	 */
	if (obj_get_file(module) == NULL)
	{
	    obj_set_file(module, istr_string(fileName));
	}
	istr_destroy(fileName);

	load_module(module);
    }
    trav_close(&fileTrav);


    /*
     * Mark the modules that are to be written
     */
    {
	char	explodedModuleFile[MAXPATHLEN+1];
	*explodedModuleFile = 0;

        for (trav_open(&moduleTrav, project, AB_TRAV_MODULES|AB_TRAV_MOD_SAFE);
	    (module = trav_next(&moduleTrav)) != NULL; )
        {
	    sprintf(explodedModuleFile, "%s.bil", obj_get_name(module));

	    if ((genAllModules && obj_is_defined(module)) ||
                (strlist_str_exists(fileNames, explodedModuleFile) && (!genNoModules)))
	    {
		if (obj_is_defined(module))	/* must be defined!! */
		{
	            strlist_set_str_data(fileNames, explodedModuleFile, (void*)TRUE);
                    obj_set_write_me(module, TRUE);
		}
            }
        }

        trav_close(&moduleTrav);
    }

    /*
     * Make sure all the modules on the command line were found
     */
    {
	int	i = 0;
	int	numFiles = strlist_get_num_strs(fileNames);
	STRING	fileName = NULL;
	BOOL	wasFound = FALSE;
	void	*wasFoundVoidPtr;
	BOOL	errOccurred = FALSE;
        for (i = 0; i < numFiles; ++i)
	{
	    fileName = strlist_get_str(fileNames, i, &wasFoundVoidPtr);
	    assert(fileName != NULL);
	    wasFound = (BOOL)(unsigned long)wasFoundVoidPtr;
	    if (!wasFound)
	    {
		util_printf_err("module not found in project: %s\n", fileName);
		errOccurred = TRUE;
	    }
	}
	if (errOccurred)
	{
	    exit(1);
	}
    }

    /*
     * Determine dependencies necessary to write the requested modules
     */
    /* if loadAll... */
    {
        for (trav_open(&allTrav, project, AB_TRAV_ALL | AB_TRAV_MOD_SAFE);
	    (obj = trav_next(&allTrav)) != NULL; )
        {
	    switch (obj_get_type(obj))
	    {
	    case AB_TYPE_MODULE:
	        if (obj_get_write_me(obj))
	        {
		    mfobj_set_flags(obj, CGenFlagIsReferenced);
	        }
	    break;
    
	    /*
	     * win-parent can be in another module
	     */
	    case AB_TYPE_DIALOG:
	    case AB_TYPE_BASE_WINDOW:
	        if (   obj_is_defined(obj)
		    && obj_is_popup(obj)
		    && obj_get_write_me(obj_get_module(obj)))
	        {
		    if ((winParent = obj_get_win_parent(obj)) != NULL)
		    {
		        mfobj_set_flags(obj_get_module(winParent), 
					CGenFlagIsReferenced);
		    }
	        }

		/* visible windows must be loaded to write main */
		if (   obj_get_write_me(project)
		    && obj_is_defined(obj) && obj_is_initially_visible(obj))
		{
		    mfobj_set_flags(obj_get_module(obj), 
					CGenFlagIsReferenced);
		}
	    break;
    
	    /*
	     *
	     */
	    case AB_TYPE_ACTION:
		obj1 = obj_get_from(obj);
		if (obj_is_project(obj1))
		    break;
		obj2 = obj_get_to(obj);
		if (  (   (obj1 != NULL) 
		       && obj_is_defined(obj1)
		       && obj_get_write_me(obj_get_module(obj1)))
		    ||(   (obj2 != NULL) 
		       && obj_is_defined(obj2)
		       && obj_get_write_me(obj_get_module(obj2)))
		   )
		{
		        /*
		         * At least one object will be written, make sure
		         * both have data.
		         */
			if (obj1 != NULL)
			{
		            mfobj_set_flags((obj_get_module(obj1)), 
					CGenFlagIsReferenced);
			}
			if (obj2 != NULL)
			{
		            mfobj_set_flags((obj_get_module(obj2)), 
			   		 CGenFlagIsReferenced);
			}
		}

		/*
		 * to/from of cross-module conns must be available to write
		 * main
		 */
		if (   obj_get_write_me(project)
		    && obj_is_defined(obj)
		    && obj_is_cross_module(obj))
		{
			if ((obj1 != NULL) && obj_is_defined(obj1))
			{
		            mfobj_set_flags((obj_get_module(obj1)), 
					CGenFlagIsReferenced);
			}
			if ((obj2 != NULL) && obj_is_defined(obj2))
			{
		            mfobj_set_flags((obj_get_module(obj2)), 
			   		 CGenFlagIsReferenced);
			}
		}
	    break;
	    }
        }
        trav_close(&allTrav);
    }

    return 0;
}


static int
load_marked_modules(ABObj project)
{
    ABObj		module = NULL;
    AB_TRAVERSAL	moduleTrav;
    char	fileName[MAXPATHLEN+1] = "";

    for (trav_open(&moduleTrav, project, AB_TRAV_MODULES|AB_TRAV_MOD_SAFE);
	(module = trav_next(&moduleTrav)) != NULL; )
    {
	if (   mfobj_has_flags(module, CGenFlagLoadMe) 
	    && (!obj_is_defined(module)))
	{
	    load_module(module);
	}
    }
    trav_close(&moduleTrav);

    return 0;
}


static int
load_module(ABObj module)
{
    char	fileName[MAXPATHLEN+1];
    ABObj	newProject = NULL;
    ABObj	project = obj_get_project(module);
    *fileName = 0;

	if (obj_get_file(module) != NULL)
	{
	    strcpy(fileName, obj_get_file(module));
	}
	else
	{
	    strcpy(fileName, obj_get_name(module));
	    if (!util_file_name_has_ab_extension(fileName))
	    {
	        strcat(fileName, ".bil");
	    }
	}

	newProject = bil_load_file(fileName, NULL, project, NULL);
	if (newProject == NULL)
	{
	    /* a failure ocurred. The error has already been reported */
	    exit(1);
	}
	else if (newProject != project)
	{
	    util_printf_err(
	    catgets(Dtb_project_catd, 1, 10,
	             "Unexpected project in file %s. Aborting\n"), fileName);
	    exit(1);
	}
	if (!obj_is_defined(module))
	{
	    char *module_name_string = obj_get_name(module);
    
	    util_printf_err(
		    catgets(Dtb_project_catd, 1, 11,
		        "Module %s not found in file %s. Aborting\n"),
		        module_name_string, fileName);
	    exit(1);
	}

    return 0;
}


/*
 * Print usage message
 */
static int
abmf_usage(void)
{
    char *program_name_string = util_get_program_name();

    fprintf(stderr,"\n");
    fprintf(stderr,
       catgets(Dtb_project_catd, 1, 12,
"Usage: %s [options] [project-file] [module-file [module-file] ...]\n\n"), 
	program_name_string);

    fprintf(stderr, "%s", catgets(Dtb_project_catd, 1, 13,
"Code is generated for each module specified on the command line, or for\n"));
    fprintf(stderr, "%s", catgets(Dtb_project_catd, 1, 14,
"all modules in the project, if no modules are specified. If no project\n"));
    fprintf(stderr, "%s", catgets(Dtb_project_catd, 1, 15,
"file is specified, a project file containing the specified module(s) is\n"));
    fprintf(stderr, "%s", catgets(Dtb_project_catd, 1, 16,
"searched for in the current directory.\n\n"));

    fprintf(stderr, "%s", catgets(Dtb_project_catd, 1, 17,
"Files with extension .bip are assumend to be BIL project files, files with\n"));
    fprintf(stderr, "%s", catgets(Dtb_project_catd, 1, 18,
".bix extenstion are assumed to be encapsulated BIL files, and files\n"));
    fprintf(stderr, "%s", catgets(Dtb_project_catd, 1, 19,
"With a .bil extension are assumed to be BIL module files.\n\n"));

    fprintf(stderr, "%s", catgets(Dtb_project_catd, 1, 20,
"Options (* = default, + = default with no project file):\n"));
    fprintf(stderr, "%s", catgets(Dtb_project_catd, 1, 21,
"  -help (-h)        Print out this help message\n"));
    fprintf(stderr, "%s", catgets(Dtb_project_catd, 1, 22,
"  -main             Write file containing main()\n"));
    fprintf(stderr, "%s", catgets(Dtb_project_catd, 1, 23,
"  -changed          Only generate files that have changed\n"));
    fprintf(stderr, "%s", catgets(Dtb_project_catd, 1, 24,
"* -merge	     Merge generated _stubs.c files with previous version\n"));
    fprintf(stderr, "%s", catgets(Dtb_project_catd, 1, 25,
"  -nomerge          Don't merge existing and new stubs file\n"));
    fprintf(stderr, "%s", catgets(Dtb_project_catd, 1, 26,
"* -project (-p)     Specify a project to generate code for\n"));
    fprintf(stderr, "%s", catgets(Dtb_project_catd, 1, 27,
"  -noproject (-np)  Use default project settings, ignore project file\n"));
    fprintf(stderr, "%s", catgets(Dtb_project_catd, 1, 28,
"+ -showall          Application shows (maps) all windows at startup\n"));
    fprintf(stderr, "%s", catgets(Dtb_project_catd, 1, 29,
"* -noshowall        Application shows (maps) only initially-visible windows\n"));
    fprintf(stderr, "%s", catgets(Dtb_project_catd, 1, 30,
"  -silent (-s)      Silent mode, no messages written\n"));
    fprintf(stderr, "%s", catgets(Dtb_project_catd, 1, 31,
"  -verbose (-v)     Verbose mode, detailed progress messages\n\n"));


/* "-a (-ansi)      Write ANSI C code\n" */
/* "-k (-kandr)     Write K&R C code, no function prototypes\n" */
/* "-i (-i18n)      Create resource file for I18N\n" */
/* "-r (-resources) Write all resources into a resource file\n" */

    exit(1);
    return ERR_INTERNAL;
}


/*
 * Searches the current directory for a project file
 * Sets the cmdline->proj_file member
 *
 * Returns the total number of project files found
 *    
 */
static int
find_proj_file(CmdlineArgs cmdline)
{
    DIR                 *dir= NULL;
    struct dirent       *dirEntry= NULL;
    int			numProjectFiles = 0;
    StringListRec	projFiles;
    strlist_construct(&projFiles);

    dir= opendir(".");
    if (dir == NULL)
    {
        perror(catgets(Dtb_project_catd,1,32,"Couldn't open '.'"));
        return -1;
    }

    while ((dirEntry= readdir(dir)) != NULL)
    {
        if (util_file_name_has_extension(dirEntry->d_name, "bip"))
        {
            /* found a project file! */
	    strlist_add_str(&projFiles, dirEntry->d_name, NULL);
        }
    }

    numProjectFiles = strlist_get_num_strs(&projFiles);
    if (numProjectFiles > 0)
    {
	int	num_strings = strlist_get_num_strs(&projFiles);
	int	i = 0;
	STRING	fileName = NULL;

	for (i = 0; i < strlist_get_num_strs(&projFiles); ++i)
	{
	    fileName = strlist_get_str(&projFiles, i, NULL);
	    if (!proj_file_has_modules(fileName, cmdline->files))
	    {
		strlist_remove_index(&projFiles, i);
		--i;
	    }
	}
    }

    if (strlist_get_num_strs(&projFiles) == 1)
    {
	cmdline->proj_file = istr_create(strlist_get_str(&projFiles, 0, NULL));
    }
    else if (strlist_get_num_strs(&projFiles) > 1)
    {
	int	i = 0;
	char    *prog_name_string = util_get_program_name();

	fprintf(stderr, catgets(Dtb_project_catd, 1, 33,
	    "%s: Please specify project file (e.g."), prog_name_string);
  	for (i = 0; i < strlist_get_num_strs(&projFiles); ++i)
	{
	    fprintf(stderr, ", %s", strlist_get_str(&projFiles, i, NULL));
	}
	fprintf(stderr, "%s", catgets(Dtb_project_catd, 1, 34, ")\n"));
	if (!strlist_is_empty(cmdline->files))
	{
	    fprintf(stderr,
		catgets(Dtb_project_catd, 1, 35,
    		"%s: More than one project contains specified module(s)\n"),
		prog_name_string);
	}
    }

    closedir(dir); dir= NULL;

    strlist_destruct(&projFiles);
    return numProjectFiles;
}


static BOOL
proj_file_has_modules(STRING projFileName, StringList moduleFileNames)
{
    BOOL	hasModules = FALSE;
    char	word[1024] = "";
    int		modulesFound = 0;
    int		wordLen = 0;
    File	file = NULL;
    int		c = 0;
    fpos_t	fpos;
    int		numFiles = strlist_get_num_strs(moduleFileNames);

    if (numFiles < 1)
    {
	return TRUE;
    }

    file = util_fopen_locked(projFileName, "r");
    if (file == NULL)
    {
	perror(projFileName);
	return FALSE;
    }

    while ((c = fgetc(file)) != EOF)
    {
	if (c == ':')
	{
	    fgetpos(file, &fpos);
	    *word = 0;
	    if (   (fgets(word, 6, file) != NULL) 
		&& (strncmp(word, "files", 5) == 0))
	    {
		/* :files <file-list> */
		BOOL	filesDone = FALSE;
		while (((c = fgetc(file)) != EOF) && (c != '(')) {}
		filesDone = (c == EOF);
		while (!filesDone)
		{
		    while (((c = fgetc(file)) != EOF) && isspace(c)) {}
		    ungetc(c, file);
		    *word = 0;
		    wordLen = 0;
		    while (((c = fgetc(file)) != EOF) && (!isspace(c)))
		    {
			if (c == ')')
			{
			    filesDone = TRUE;
			    break;
			}
			word[wordLen++] = c;
			word[wordLen] = 0;
		    }
		    if (c == EOF)
		    {
			filesDone = TRUE;
		    }

		    if (*word != 0)
		    {
			int	i = 0;
			ISTRING	temp_istr = istr_create(word);
			for (i = 0; i < numFiles; ++i)
			{
			    if (istr_equal(temp_istr, 
				    strlist_get_istr(moduleFileNames, i, NULL)))
			    {
				++modulesFound;
				break;
			    }
			}
			istr_destroy(temp_istr);
		    }
		}
	    }
	    else if (   (strncmp(word, "modul", 5) == 0)
		     && (fgets(word, 2, file) != NULL)
		     && (strncmp(word, "e", 1) == 0) )
	    {
		/* :module <module-name */
		/* REMIND: we need to have this for .bix files to work! */
	    }
	    else
	    {
		fsetpos(file, &fpos);
	    }
	} /* c == ':' */
    } /* while c != EOF */

    util_fclose(file);
    hasModules = (modulesFound >= numFiles);

    return hasModules;
}


static BOOL
write_required(ABObj tree)
{
    BOOL        writeSomething= FALSE;

    if (obj_get_write_me(tree))
    {
        writeSomething= TRUE;
    }
    else
    {
        AB_TRAVERSAL    trav;
        ABObj           module= NULL;

        for(trav_open(&trav, tree, AB_TRAV_MODULES);
            (module= trav_next(&trav)) != NULL; )
        {
            if (obj_get_write_me(module))
            {
                writeSomething= TRUE;
                break;
            }
        }
        trav_close(&trav);
    }

    return writeSomething;
}


/*
 * Initializes everything in abmf.
 */
static int
abmf_init(void)
{
    STRING	templateFuncName = NULL;

    abmfP_motifdefs_init();
    abmfP_ui_header_file_init();
    abmfP_args_init();

    /*
     * Turn "template" library functions into printf format strings
     */
    templateFuncName = "dtb_default_dragCB";
    replace_string_shorter(abmfP_lib_default_dragCB->proto, 
				templateFuncName, "%s");
    replace_string_shorter(abmfP_lib_default_dragCB->def, 
				templateFuncName, "%s");

    templateFuncName = "dtb_default_dropCB";
    replace_string_shorter(abmfP_lib_default_dropCB->proto, 
				templateFuncName, "%s");
    replace_string_shorter(abmfP_lib_default_dropCB->def, 
				templateFuncName, "%s");

    return 0;
}


/*
 * Replaces all occurences of substr with replacestr. 
 * replaceStr *must* be shorter than or the same length as subStr
 */
static int
replace_string_shorter(STRING buf, STRING subStr, STRING replaceStr)
{
    int		numReplaced = 0;
    char	*bufPtr = buf;
    char	*bufEnd = buf + strlen(buf);
    int		subStrLen = strlen(subStr);
    char	*subStrPtr = subStr;
    int		replaceStrLen = strlen(replaceStr);
    int		replaceDiffLen = subStrLen - replaceStrLen;

    assert(strlen(subStr) >= (size_t)strlen(replaceStr));

    while ((subStrPtr = strstr(bufPtr, subStr)) != NULL)
    {
	memmove(subStrPtr+replaceStrLen, 
		subStrPtr+subStrLen,
		((int)(bufEnd - (subStrPtr+subStrLen))) + 1);
	strncpy(subStrPtr, replaceStr, replaceStrLen);	/* no NULL! */
	bufPtr = subStrPtr+replaceStrLen;
	bufEnd -= replaceDiffLen;
	++numReplaced;
    }
    return numReplaced;
}


/* 
 *
 */
static int
examine_tree(ABObj project)
{
    int                 returnValue = 0;
    AB_TRAVERSAL        moduleTrav;
    AB_TRAVERSAL        uiTrav;
    ABObj               module = NULL;
    ABObj               obj = NULL;
    ABObj		parent = NULL;
    StringList		proj_callbacks = NULL;
    ABObj		callbackScopeObj = NULL;
    STRING		funcName = NULL;

    objxm_obj_configure(project, OBJXM_CONFIG_CODEGEN, TRUE);

    for (trav_open(&moduleTrav, project, AB_TRAV_MODULES);
        (module = trav_next(&moduleTrav)) != NULL; )
    {
	if (!mfobj_has_flags(module, CGenFlagIsReferenced))
	{
	    continue;
	}

#ifdef DEBUG
	if (debugging())
	{
	    char	name[256];
  	    util_dprintf(1, "Configuring %s\n", 
		obj_get_safe_name(module, name, 256));
	}
#endif /* DEBUG */
	objxm_tree_configure(module, OBJXM_CONFIG_CODEGEN);
        dup_all_menu_refs(module);	/* this can create new objs */

        for (trav_open(&uiTrav, module, 
                AB_TRAV_UI | AB_TRAV_MOD_PARENTS_FIRST | AB_TRAV_MOD_SAFE);
            (obj = trav_next(&uiTrav)) != NULL; )
        {
            if (   (obj_get_class_name(obj) == NULL)
                && (!obj_has_flag(obj, NoCodeGenFlag)) ) 
            { 
                #ifdef DEBUG
		if (debugging())
		{
		    if (!(obj_is_item(obj) || obj_is_menu(obj) || obj_is_message(obj)))
                    {
                    char name[256] = "";
                    util_dprintf(0, 
                        "WARNING: "
                      "Object has no class name (NoCodeGenFlag not set): %s\n",
                        obj_get_safe_name(obj, name, 256));
                    }
		}
                #endif /* DEBUG */
            };

	    munge_ensure_win_parent(obj);
	    munge_liberate_menu(obj);
	}
        trav_close(&uiTrav);
    } /* trav modules */
    trav_close(&moduleTrav);

    abmfP_prepare_tree(project);	/* creates extra data for each obj */

    return returnValue;
}


static int
munge_ensure_win_parent(ABObj win)
{
    ABObj	root_window = abmfP_get_root_window(obj_get_project(win));
    if (!obj_is_window(win))
    {
	return 0;
    }

    if ((obj_get_win_parent(win) == NULL) && (win != root_window))
    {
	obj_set_win_parent(win, root_window);
    }

    return 0;
}


static int
munge_liberate_menu(ABObj menu)
{
    if (!obj_is_menu(menu))
    {
	return -1;
    }
    {
	ABObj		compRoot = obj_get_root(menu);
	AB_OBJECT_TYPE	compType = AB_TYPE_UNDEF;
	
	if (compRoot != NULL)
	{
	    compType = obj_get_type(compRoot);
	}
	if (   (compRoot != menu)
	    && (   (compType == AB_TYPE_ITEM)
	        || (compType == AB_TYPE_BUTTON)) )
	{
	    menu->part_of = NULL;
	}
    }

    return 0;
}


static int
dup_all_menu_refs(ABObj project)
{
    AB_TRAVERSAL	moduleTrav;
    AB_TRAVERSAL	menuTrav;
    ABObj		module = NULL;
    ABObj		menu = NULL;

    for (trav_open(&moduleTrav, project, AB_TRAV_MODULES);
	(module = trav_next(&moduleTrav)) != NULL; )
    {
	if (!obj_get_write_me(module))
	{
	    continue;
	}
        for (trav_open(&menuTrav, module, AB_TRAV_MENUS);
	    (menu = trav_next(&menuTrav)) != NULL; )
        {
    	    if (obj_is_ref(menu))
	    {
		abmfP_dup_menu_ref_tree(menu);
		if (obj_is_popup(menu))
		{
		   STRING title = obj_get_menu_title(obj_get_parent(menu));
		   if (!util_strempty(title))
		    	objxm_create_popup_menu_title(menu, OBJXM_CONFIG_CODEGEN,
				title);
		}
	    }
        }
        trav_close(&menuTrav);
    }
    trav_close(&moduleTrav);

    return 0;
}


static int
abmfP_dup_menu_ref_tree(ABObj obj)
{
    int		rc = 0;		/* return code */
    int			return_value = 0;
    ABObj	refObj = NULL;
    AB_TRAVERSAL	trav;

    if (!obj_is_menu_ref(obj))
    {
	return 0;
    }
    rc = abmfP_do_dup_menu_ref_tree(obj);
    if (rc < 0)
    {
	return rc;
    }

    /*
     * We can't handle multi-layered references, so we are going to collapse
     * any reference lists to only reference the final (actual) object.
     */
    for (trav_open(&trav, obj, AB_TRAV_UI);
	(obj = trav_next(&trav)) != NULL; )
    {
	refObj = obj_get_actual_obj(obj);
	if (refObj != NULL)
	{
	    while (obj_is_ref(refObj))
	    {
	        refObj = obj_get_actual_obj(refObj);
	    }
	    obj_cvt_to_ref(obj, refObj);
	}
    }
    trav_close(&trav);
    return return_value;
}


static int
abmfP_do_dup_menu_ref_tree(ABObj obj)
{
    int		return_value = 0;
    int		rc = 0;
    ABObj	actualMenu = NULL;
    if (!obj_is_menu_ref(obj))
    {
	return 0;
    }
    actualMenu = obj_get_actual_obj(obj);
    if (actualMenu == NULL)
    {
	return -1;
    }

    if (obj_get_num_items(obj) != obj_get_num_items(actualMenu))
    {
	ABObj dupTree = obj_tree_create_ref(actualMenu);
	if (dupTree == NULL)
	{
	    return_value = -1;
	    goto epilogue;
	}
	rc = obj_move_children(obj, dupTree);
	if (rc < 0)
	{
	    return_value = rc;
	}
	obj_destroy(dupTree);

	{
	    AB_TRAVERSAL	trav;
	    ABObj		descendant = NULL;

	    for (trav_open(&trav, obj, 
			AB_TRAV_UI|AB_TRAV_MOD_PARENTS_FIRST|AB_TRAV_MOD_SAFE);
		(descendant = trav_next(&trav)) != NULL; )
	    {
		if (descendant == obj)
		{
		    continue;
		}
		if (obj_is_menu_ref(descendant))
		{
		    abmfP_do_dup_menu_ref_tree(descendant);
		}
	    }
	    trav_close(&trav);
	}
    }
epilogue:
    return return_value;
}


static int
dump_tree(ABObj tree)
{
    int		return_value = 0;
    int		old_verbosity = util_get_verbosity();

    switch (old_verbosity)
    {
	case 0: /* FALLTHROUGH */
	case 1: util_set_verbosity(3);
	break;

	case 2: util_set_verbosity(4); 
	break;
    }

    print_tree(tree, 0);

epilogue:
    util_set_verbosity(old_verbosity);
    return return_value;
}


static int
print_tree(ABObj root, int indent)
{
    AB_TRAVERSAL	trav;
    ABObj		child = NULL;
    int			numPrinted = 0;
    int			travType = AB_TRAV_SALIENT_CHILDREN;

    if (util_get_verbosity() >= 4)
    {
	travType = AB_TRAV_CHILDREN;
    }

    if (   (travType == AB_TRAV_CHILDREN)
	|| (obj_is_salient(root)) )
    {
        objxm_print_indented(root, indent, util_get_verbosity());
        ++numPrinted;
    }
    
    if (obj_is_module(root) && (!obj_get_write_me(root)))
    {
	goto epilogue;
    }
    for (trav_open(&trav, root, travType);
	(child = trav_next(&trav)) != NULL; )
    {
	numPrinted += print_tree(child, indent+4);
    }
    trav_close(&trav);

    /*
     * Look for non-salient children that need their children printed
     */
    if (travType == AB_TRAV_SALIENT_CHILDREN)
    {
	for (trav_open(&trav, root, AB_TRAV_CHILDREN);
	    (child = trav_next(&trav)) != NULL; )
	{
	    if ((!obj_is_salient(child)) && (obj_get_root(child) != root))
	    {
		numPrinted += print_tree(child, indent);
	    }
	}
    }

epilogue:
    return numPrinted;
}


#ifdef DEBUG
static int
time_traversal(ABObj root)
{
	int		return_value = 0;
	AB_TRAVERSAL	trav;
	ABObj		obj = NULL;
	int		i = 0;
	double		startTime = 0;
	double		endTime = 0;
	double		totalTravTime = 0;
	double		oneTravTime = 0;
	int		numTravs = 1000;
	struct tms	timeBuf;
	long		ticks_per_second = sysconf(_SC_CLK_TCK);
	int		oldVerbosity = util_get_verbosity();

	if (ticks_per_second <= 0)
	{
	    util_dprintf(0, "Couldn't get the value of _SC_CLK_TCK!\n");
	    return_code(ERR_INTERNAL);
	}
	
	util_set_verbosity(0);		/* no expensive error-checking */

	/*
	 *  all
	 */
	util_printf("Beginning trav test\n");
	startTime = (times(&timeBuf) *1.0) / ticks_per_second;
	for (i = 0; i < numTravs; ++i)
	{
	    for (trav_open(&trav, root, AB_TRAV_ALL);
		(obj = trav_next(&trav)) != NULL; )
	    {
	    }
	    trav_close(&trav);
	}
	endTime = (times(&timeBuf) *1.0) / ticks_per_second;
	util_printf("end of trav test\n");
	totalTravTime = (endTime - startTime);
	oneTravTime = totalTravTime/numTravs;
	util_printf("one ALL traversal time: %g\n", oneTravTime);

	/*
	 * salient
	 */
	startTime = (times(&timeBuf) *1.0) / ticks_per_second;
	for (i = 0; i < numTravs; ++i)
	{
	    for (trav_open(&trav, root, AB_TRAV_SALIENT);
		(obj = trav_next(&trav)) != NULL; )
	    {
	    }
	    trav_close(&trav);
	}
	endTime = (times(&timeBuf) *1.0) / ticks_per_second;
	util_printf("end of trav test\n");
	totalTravTime = (endTime - startTime);
	oneTravTime = totalTravTime/numTravs;
	util_printf("one SALIENT traversal time: %lg\n", oneTravTime);

epilogue:
    util_set_verbosity(oldVerbosity);
    return return_value;
}
#endif /* DEBUG */


/*
 * Actually sets the tree up for code generation.
 */
static int
abmfP_prepare_tree(ABObj project)
{
    ABObj		module = NULL;
    AB_TRAVERSAL	trav;
    StringList		proj_callbacks = NULL;
    ABObj		obj = NULL;
    AB_TRAVERSAL	allTrav;
    ABObj		callbackScopeObj;
    STRING		funcName = NULL;
    ABObj		parent = NULL;

    /*
     * Create extra cgen data for each object
     */
    abmfP_create_obj_data_for_project(project);
    for(trav_open(&trav, project, AB_TRAV_MODULES);
	(module = trav_next(&trav)) != NULL; )
    {
	abmfP_create_obj_data_for_module(module);
    }
    trav_close(&trav);

    proj_callbacks = mfobj_get_proj_data(project)->callbacks;

    for (trav_open(&allTrav, project, 
                AB_TRAV_ALL | AB_TRAV_MOD_PARENTS_FIRST | AB_TRAV_MOD_SAFE);
	(obj = trav_next(&allTrav)) != NULL; )
    {
	module = obj_get_module(obj);

	if (obj_is_action(obj) && (obj_get_func_type(obj) == AB_FUNC_USER_DEF))
	{
	    /* 
	     * we have a user-defined callback. decide where to put it 
	     */
	    funcName = obj_get_func_name(obj);
	    callbackScopeObj = abmfP_find_callback_scope(project, funcName);
	    if (callbackScopeObj == NULL)
	    {
		/* doesn't exist */
		if (obj_is_intra_module(obj) &&
		    !obj_is_project(obj_get_from(obj)))
		{
		    strlist_add_str(mfobj_get_module_data(module)->callbacks, 
			funcName, (void *)obj);
		}
		else
		{
		    strlist_add_str(mfobj_get_proj_data(project)->callbacks,
			funcName, (void *)obj);

		    if (obj_is_project(obj_get_from(obj)))
		    	mfobj_set_flags(obj, CGenFlagWriteDefToProjFile);
		}
	    }
	    else
	    {
		/*
		 * It exists - see if it's in the right place...
		 */
		if (   obj_is_module(callbackScopeObj) 
		    && (callbackScopeObj != module))
		{
		    /*
		     * it's in more than one module - put it in the project
		     */
		    int		str_index = 0;
		    ABObj	originalConn = NULL;
		    StringList	moduleCallbacks = 
				    mfobj_get_module_data(callbackScopeObj)
					->callbacks;

		    str_index = 
			strlist_get_str_index(moduleCallbacks, funcName);
		    strlist_get_str(moduleCallbacks, str_index, 
			(void **)&originalConn);
		    if (originalConn != NULL)
		    {
			mfobj_set_flags(originalConn, 
				CGenFlagWriteDefToProjFile);
		    }
		    strlist_remove_index(moduleCallbacks, str_index);
		    strlist_add_str(
			mfobj_get_proj_data(project)->callbacks,
			funcName, (void *)originalConn);
		    mfobj_set_flags(obj, CGenFlagWriteDefToProjFile);
		}

		/*
		 * Mark this as a duplicate, so we only write it once
		 */
		mfobj_set_flags(obj, CGenFlagIsDuplicateDef);
	    }
	} /* action */
	else if ((module != NULL) && (obj_get_write_me(module)))
        {
	    /*
	     * For some reason, menu titles are of type menu and not label
	     */
	    if (ObjWClassIsLabel(obj))
	    {
		obj->type = AB_TYPE_LABEL;
		parent = abmfP_parent(obj);
		if (   obj_is_menu(parent)
		    || obj_is_menu_ref(parent))
		{
		    obj->part_of = abmfP_parent(obj);
		    parent->part_of = parent;
		}
	    }

	    if (   (obj_is_menu(obj) || (obj_is_menu_ref(obj)))
		&& (obj_get_class_name(obj) == NULL))
	    {
		obj_set_class_name(obj, istr_string(abmfP_xmMenuShell));
	    }
	} /* obj_get_write_me() */
	if ((!(mfobj_get_proj_data(project)->has_ui_obj)) && obj_is_ui(obj))
	{
	    mfobj_get_proj_data(project)->has_ui_obj = TRUE;
	}
	if (obj_get_type(obj) == AB_TYPE_TERM_PANE)
	{
	    mfobj_get_proj_data(project)->has_terminal = TRUE;
	}
    } /* trav modules */
    trav_close(&allTrav);

    return 0;
}


static BOOL
abmfP_obj_has_no_module(ABObj obj)
{
    return ((obj == NULL) || (obj_get_module(obj) == NULL));
}


/*
 * Creates data ONLY for project-specific things that are not covered
 * by the individual modules.
 *
 * Assumes: project is a project
 */
static int
abmfP_create_obj_data_for_project(ABObj project)
{
    AB_TRAVERSAL	trav;
    ABObj		obj = NULL;
    CGenProjData	projData = NULL;
    CGenData		objData = NULL;
    size_t		objDataNumBytes = 0;
    int			numObjects = 0;
    int			objCount = 0;

    numObjects = 0;
    for (trav_open_cond(&trav, project, AB_TRAV_ALL|AB_TRAV_MOD_SAFE, 
						abmfP_obj_has_no_module);
	(obj = trav_next(&trav)) != NULL; )
    {
	++numObjects;
    }
    /* don't close trav, yet */

    /*
     * Create data space for all children
     */
    objDataNumBytes = numObjects * sizeof(CGenDataRec);
    objData= (CGenData)util_malloc(objDataNumBytes);
    memset(objData, 0, objDataNumBytes);

    /*
     * Assign each obj it's own struct
     */
    for (trav_reset(&trav), objCount = 0; 
	(obj = trav_next(&trav)) != NULL; )
    {
	obj->cgen_data = (CGenData)&(objData[objCount++]);
	mfobj_set_flags(obj, CGenFlagDataIsObj);
    }
    trav_close(&trav);	/* close it, now */
    assert(objCount == numObjects);

    /*
     * Create project-specific data
     */
    projData = (CGenProjData)calloc(1, sizeof(CGenProjDataRec));
    projData->callbacks = strlist_create();
    projData->children_data = objData;
    mfobj_set_proj_data(project, projData);

    return 0;
}

/*
 * Accepts any value (NULL, obj w/o module, ...)
 */
static int
abmfP_create_obj_data_for_module(ABObj objInModule)
{
    ABObj		module = NULL;
    int			numObjects = 0;
    CGenData		objData = NULL;
    size_t		objDataBytes = 0;
    ABObj		obj = NULL;
    AB_TRAVERSAL	trav;
    int			objCount = 0;

    if (   (objInModule == NULL) 
	|| (mfobj_has_data(objInModule))
	|| ((module = obj_get_module(objInModule)) == NULL)
       )
    {
	return 0;
    }


    /*
     * object data is an array of all data for the module.
     */
    numObjects = trav_count(module, AB_TRAV_ALL);
    objDataBytes = numObjects * sizeof(CGenDataRec);
    objData = (CGenData)util_malloc(objDataBytes);
    memset(objData, 0, objDataBytes);
    if (objData == NULL)
    {
	util_printf_err(catgets(Dtb_project_catd, 1, 36,
	    "Could not allocate cgen data\n"));
	return -1;
    }

    /*
     * Assign a structure to each object
     */
    for (trav_open(&trav, module, AB_TRAV_ALL), objCount = 0;
	(obj = trav_next(&trav)) != NULL; )
    {
	if (obj->cgen_data == NULL)
	{
	    obj->cgen_data = (CGenData)&(objData[objCount++]);
	    mfobj_set_flags(obj, CGenFlagDataIsObj);
	}
    }
    trav_close(&trav);
    assert(objCount <= numObjects);

    /*
     * Create module-specific data
     */
    ensure_data_for_module_obj(module);
    mfobj_get_module_data(module)->children_data = objData;

    return 0;
}


/*
 * Makes sure the module has a cgen data associated with it. Will load
 * the objects in the module if necessary, if loadFile is TRUE.
 */
static int
ensure_data_for_module_obj(ABObj module)
{
    CGenModuleData	moduleData = NULL;
    ABObj		project = NULL;
    ABObj		newProject = NULL;

    if (!obj_is_module(module))
    {
	return -1;
    }
    project = obj_get_project(module);
    if (module->cgen_data == NULL)
    {
        module->cgen_data = (CGenData)calloc(sizeof(CGenDataRec), 1);
    }

    if (mfobj_get_module_data(module) == NULL)
    {
	moduleData = (CGenModuleData)calloc(sizeof(CGenModuleDataRec), 1);
        moduleData->callbacks = strlist_create();
        mfobj_set_module_data(module, moduleData);
    }

    return 0;
}


#ifdef DEBUG

static int
dump_callbacks(ABObj project)
{
    ABObj		module = NULL;
    AB_TRAVERSAL	moduleTrav;
    StringList		callbacks = NULL;

    util_dprintf(0, "\n***** CALLBACKS *****\n");
    util_dprintf(0, "project callbacks\n");
    callbacks = mfobj_get_proj_data(project)->callbacks;
    if (callbacks != NULL)
    {
        strlist_dump(callbacks);
    }
    for (trav_open(&moduleTrav, project, AB_TRAV_MODULES);
	(module = trav_next(&moduleTrav)) != NULL; )
    {
	util_dprintf(0, "module '%s' callbacks\n", util_strsafe(obj_get_name(module)));
	callbacks = mfobj_get_module_data(module)->callbacks;
        if (callbacks != NULL)
        {
            strlist_dump(callbacks);
        }
    }
    trav_close(&moduleTrav);
    util_dprintf(0,"\n");
    return 0;
}

/*
 * Checks tree and aborts if an error is found.
 */
static int
debug_verify_tree(ABObj root)
{
    AB_TRAVERSAL	trav;
    char		name[256];
    ABObj		obj = NULL;
    *name = 0;

    if (!debugging())
    {
	return 0;
    }

    util_dprintf(1, "Examining tree for defects.\n");

    /*
     * Do the standard verification
     */
    if (obj_tree_verify(root) < 0)
    {
        util_dprintf(1, "Project tree is corrupt! Aborting.\n");
	abort();
    }

    /*
     * Perform more code-generator specific checks
     */
    for (trav_open(&trav, root, AB_TRAV_ALL);
	(obj = trav_next(&trav)) != NULL; )
    {
	if (obj_is_ui(obj))
	{
	    if (   (obj_get_write_me(obj_get_module(obj)))
		&& (obj_get_name(obj) == NULL) )
	    {
		util_dprintf(1, "Object has no name: %s\n",
			obj_get_safe_name(obj, name, 256));
		abort();
	    }
	}
    }
    trav_close(&trav);

    return 0;
}

#endif /* DEBUG */


#ifdef DEBUG	/* performance testing */
static int	
get_cur_time(double *realTimeOut, double *cpuTimeOut)
{
	long		ticks_per_second = sysconf(_SC_CLK_TCK);
    struct tms	timeInfo;
    double	realTime = times(&timeInfo);
    double	cpuTime = timeInfo.tms_utime + timeInfo.tms_stime
			+ timeInfo.tms_cutime + timeInfo.tms_cstime;
    *realTimeOut  = realTime / CLK_TCK;
    *cpuTimeOut = cpuTime / CLK_TCK;

    return 0;
}

#endif /* DEBUG */

