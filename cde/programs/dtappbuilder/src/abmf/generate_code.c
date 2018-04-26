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
 * $XConsortium: generate_code.c /main/4 1996/08/27 16:16:30 mustafa $
 * 
 * @(#)generate_code.c	1.36 04 May 1995	cde_app_builder/src/abmf
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


/*
 * generate_code.c - writes all files necessary for the project
 */

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utime.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>
#include <ab_private/util.h>
#include <ab_private/abio.h>
#include "dtb_utils.h"
#include "msg_file.h"
#include "obj_namesP.h"
#include "write_codeP.h"
#include "ui_header_fileP.h"
#include "resource_fileP.h"
#include "ui_c_fileP.h"
#include "stubs_c_fileP.h"
#include "proj_c_fileP.h"
#include "proj_header_fileP.h"
#include "make_fileP.h"
#include "connectP.h"
#include "merge_cP.h"
#include "instancesP.h"
#include "utils_header_fileP.h"
#include "utils_c_fileP.h"
#include "abmf.h"
#include "msg_cvt.h"

/*************************************************************************
**                                                                      **
**       Constants (#define and const)					**
**                                                                      **
**************************************************************************/

typedef struct
{
    ISTRING             file_name;
    time_t              mod_time;
    BOOL                merged;
}                   GenLogEntryRec, *GenLogEntry;

typedef struct
{
    ISTRING             log_file;
    int                 num_entries;
    int                 entries_size;
    GenLogEntry         entries;
}                   GenLogRec, *GenLog;

static int          log_construct(GenLog);
static int          log_destruct(GenLog);
static int          log_sort_by_date(GenLog);
static GenLogEntry  log_find_entry_by_name(GenLog, STRING file_name);

static int		log_add_entry(
	      			GenLog,
	      			STRING file_name,
	      			time_t mod_date,
	      			BOOL merged
			);

static int          log_dump(GenLog);
/* private methods */
static int          logP_release_data(GenLog);
static int          logP_read(GenLog);
static int          logP_write(GenLog);
static int          logP_get_int_from_string(STRING * ptr_ptr);

/*************************************************************************
**                                                                      **
**       Private Functions (C declarations and macros)			**
**                                                                      **
**************************************************************************/

typedef enum
{
    ABMF_SKIP_UNDEF = 0,
    ABMF_SKIP_UP_TO_DATE,
    ABMF_SKIP_NO_CHANGES,
    ABMF_SKIP_ERR_OCCURRED,
    ABMF_SKIP_WHY_NUM_VALUES	/* must be last */
} ABMF_SKIP_WHY;

static int	write_make_file(
		    GenCodeInfo genCodeInfo,
		    GenLog genLog,
		    ABObj project,
		    BOOL merge_files,
		    BOOL check_dates,
		    BOOL * makeFileChangedPtr
		);

static int	write_project_files(
		    GenCodeInfo genCodeInfo,
		    GenLog genLog,
		    ABObj project,
		    BOOL merge_files,
		    BOOL check_dates,
		    BOOL force_write_c_file,
		    BOOL * headerFileChangedPtr,
		    BOOL * stubsCFileChangedPtr
		);

static int	write_module_files(
		   GenCodeInfo genCodeInfo,
		   GenLog genLog,
		   ABObj module,
		   BOOL merge_files,
		   BOOL check_dates,
		   STRING resFileName,
		   BOOL * headerFileChangedPtr,
		   BOOL * uiCFileChangedPtr,
		   BOOL * stubsCFileChangedPtr
		);

static int	write_utils_files(
			GenCodeInfo	genCodeInfo, 
			GenLog		genLog, 
			ABObj		project,
			BOOL		check_dates,
		        BOOL		*headerFileChangedPtr,
		        BOOL 		*CFileChangedPtr
		);

static int	write_app_resource_file(
			GenCodeInfo	genCodeInfo,
			GenLog		genLog,
			ABObj		project,
			BOOL		mergeFiles,
			BOOL		checkDates,
			BOOL		*fileChangedOut
		);
static int	write_msg_file(
			GenCodeInfo	genCodeInfo,
			GenLog		genLog,
			ABObj		project,
			BOOL		checkDates,
			BOOL		*msgFileChangedOut
		);
static int	print_progress_message(
			int	verbosity,
			STRING	message,
			STRING	fileName
		);
static int	print_backing_up_message(
			STRING fromFile, 
			STRING toFile
		);
static int	print_skipping_message(
			STRING		fileName, 
			ABMF_SKIP_WHY	why
		);
static int          count_possible_substruct_fields(ABObj obj);

static int compare_file_times(
		   STRING input_file,
		   STRING output_file,
		   GenLog genLog
);

static int 	should_write_file(
		  STRING output_file,
		  STRING input_file,
		  GenLog genLog,
		  BOOL merging,
		  BOOL check_dates
);

static int	check_and_merge_c_file(
			BOOL		needMerge,
			File		*codeFileInOut,
			STRING		codeFileName,
			ABMF_SKIP_WHY	*fileSkipReasonInOut,
			File		*deltaFileOut
		);
static int	check_and_replace_file(
			STRING		fileName,
			GenLog		genLog,
			File		newFile, 
			BOOL		check_for_changes,
			BOOL		fileWasMerged,
			BOOL		*fileChangedOutPtr,
			ABMF_SKIP_WHY	*fileSKippedReasonOutPtr
		);
static BOOL	file_changed_since_last_log(GenLog genLog, STRING fileName);
static time_t	get_file_mod_time(STRING fileName);
static BOOL	source_files_equal(FILE *file1, FILE *file2);
static int	move_file(STRING existingName, STRING newName, BOOL force);
static int	replace_file(STRING fileName, File fromFile, BOOL rewindFiles);
static time_t   mkgmtime(struct tm * tloc);
static int	touch_file(STRING fileName);

static int	update_msg_set(MsgFile msgFile, ABObj obj);

/*
 * Various types of progress messages
 */
#define print_comparing_message(_fname) \
	 (print_progress_message(3, "comparing", _fname))

#define print_processing_message(_fname) \
	 (print_progress_message(2, "processing", _fname))

#define print_merging_message(_fname) \
	 (print_progress_message(2, "merging", _fname))

#define print_writing_message(_fname) \
	 {STRING progress_msg; \
	 progress_msg=XtNewString(catgets(Dtb_project_catd, 1, 52, "writing"));\
	 print_progress_message(1,progress_msg,_fname);\
	 XtFree(progress_msg);}


/*************************************************************************
**                                                                      **
**       Data     	                                                **
**                                                                      **
**************************************************************************/

extern BOOL	freshenUnchangedFiles;
static BOOL	appResFileComplete = FALSE;

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/



/*
 * Write all the code that the user wants written.
 */
int
abmf_generate_code(
		   ABObj 		project,
		   ABMF_CGEN_RESTRICTION restriction,
		   BOOL 		merge_files,
		   ABMF_I18N_METHOD 	i18n_method_in,
		   BOOL 		prototype_functions_in,
		   AB_ARG_CLASS_FLAGS	dumped_resources_in
)
{
    int                 return_value = 0;
    int                 iRC = 0;/* int return code */
    AB_TRAVERSAL        trav;
    ABObj               module = NULL;
    char                resFileName[MAX_PATH_SIZE];
    BOOL                writeAll = FALSE;
    GenCodeInfoRec      genCodeInfoRec;
    GenCodeInfo         genCodeInfo = &genCodeInfoRec;
    GenLogRec           genLogRec,
                       *genLog = &(genLogRec);
    BOOL                forceWriteProjectCFile = FALSE;
    BOOL                headerFileChanged = FALSE;
    BOOL                uiCFileChanged = FALSE;
    BOOL                stubsCFileChanged = FALSE;
    BOOL                makeFileChanged = FALSE;
    BOOL		msgFileChanged = FALSE;
    BOOL		resFileChanged = FALSE;
    BOOL                check_dates = FALSE;
    *resFileName = 0;
    abmfP_gencode_construct(&genCodeInfoRec);
    log_construct(&genLogRec);

    check_dates = ((restriction == ABMF_CGEN_BY_DATE) ||
		   (restriction == ABMF_CGEN_BY_DATE_AND_CONTENTS));

    /*
     * Set up GenCodeInfo data
     */
    genCodeInfo->prototype_funcs = prototype_functions_in;
    genCodeInfo->dumped_resources = dumped_resources_in;
    genCodeInfo->i18n_method = i18n_method_in;
    appResFileComplete = FALSE;

    /*
     * Determine if we're writing EVERYTHING
     */
    if (obj_get_write_me(project))
    {
	writeAll = TRUE;
	for (trav_open(&trav, project, AB_TRAV_MODULES);
	     (module = trav_next(&trav)) != NULL;)
	{
	    if (!obj_get_write_me(module))
	    {
		writeAll = FALSE;
		break;
	    }
	}
	trav_close(&trav);
    }

    abmfP_tree_set_action_names(project);

    /*
     * Open message source file if we have i18n enabled
     */
    if (genCodeInfo->i18n_method == ABMF_I18N_XPG4_API)
    {
	if (!abmfP_initialize_msg_file(genCodeInfo, project))
	{
	    util_dprintf(0, "Cannot generate message source file: %s\n",
			    genCodeInfo->msg_src_file_name);
	    return_value = -1;
	    goto epilogue;
	}
    }

    /*
     * Write out the modules
     */
    for (trav_open(&trav, project, AB_TRAV_MODULES);
	 (module = trav_next(&trav)) != NULL;)
    {
	if (!obj_get_write_me(module) 
	    || !obj_is_defined(module))
	{
	    continue;
	}
        
	/*
         * Open resource file (if necessary)
         */
	abmfP_get_intermediate_res_file_name(
		module, resFileName, MAX_PATH_SIZE);
        if (genCodeInfo->dumped_resources == AB_ARG_CLASS_FLAGS_NONE)
	{
	    unlink(resFileName);
	}
	else
        {
	    abmfP_get_intermediate_res_file_name(
		module, resFileName, MAX_PATH_SIZE);
	    genCodeInfo->resource_file = 
		abmfP_res_file_open(resFileName, resFileName, module, TRUE);
	    if (genCodeInfo->resource_file == NULL)
	    {
	        exit(1);
	    }
        }

	iRC = write_module_files(genCodeInfo, genLog,
			 module, merge_files, check_dates, resFileName,
		   &headerFileChanged, &uiCFileChanged, &stubsCFileChanged);
	return_if_err(iRC, iRC);
	if (headerFileChanged)
	{
	    forceWriteProjectCFile = TRUE;
	}
	abmfP_res_file_close(genCodeInfo->resource_file);
    }
    trav_close(&trav);
    if (return_value < 0)
    {
	goto epilogue;
    }


    /*
     * Write project/main files.
     */
    if (obj_get_write_me(project))
    {
	abmfP_get_intermediate_res_file_name(
		project, resFileName, MAX_PATH_SIZE);
        if (genCodeInfo->dumped_resources == AB_ARG_CLASS_FLAGS_NONE)
	{
	    unlink(resFileName);
	}
	else
        {
	    abmfP_get_intermediate_res_file_name(
		project, resFileName, MAX_PATH_SIZE);
	    genCodeInfo->resource_file = 
		abmfP_res_file_open(resFileName, resFileName, project, TRUE);
	    if (genCodeInfo->resource_file == NULL)
	    {
		exit(1);
	    }
	}

	iRC = write_project_files(genCodeInfo, genLog, project,
			    merge_files, check_dates, forceWriteProjectCFile,
			    &headerFileChanged, &stubsCFileChanged);
        return_if_err(iRC, iRC);
	abmfP_res_file_close(genCodeInfo->resource_file);
    }

    /*
     * Build the application resource file
     */
    iRC = write_app_resource_file(
			genCodeInfo, 
			genLog, 
			project, 
			merge_files, 
			check_dates, 
			&resFileChanged);
    return_if_err(iRC,iRC);

    /*
     * Write more project files
     */
    if (obj_get_write_me(project))
    {
	iRC = write_utils_files(genCodeInfo, genLog, project, check_dates,
				&headerFileChanged, &stubsCFileChanged);
        return_if_err(iRC, iRC);

        iRC = write_make_file(genCodeInfo, genLog,
		       project, merge_files, check_dates, &makeFileChanged);
        return_if_err(iRC, iRC);
    }

    iRC = write_msg_file(	genCodeInfo, 
			genLog,
			project, 
			check_dates, 
			&msgFileChanged);
    return_if_err(iRC, iRC);

    if (!appResFileComplete)
    {
	/* 
	 * REMIND: This should be a warning message the user sees. We
	 *         need permission to change the msg catalog
	 */
	if (debugging())
	{
            util_dprintf(1,
"Warning: Application resource file incomplete, because some modules\n");
            util_dprintf(1,
"         have not yet been processed.\n");
	}
    }

epilogue:
    if (return_value < 0)
    {
	util_puts_err("\n");
	util_puts_err(catgets(Dtb_project_catd, 1, 53,
	    "** Aborting due to errors **\n"));
    }
    abmfP_res_file_close(genCodeInfo->resource_file);	/* NULL OK */

    log_destruct(&genLogRec);
    abmfP_gencode_destruct(&genCodeInfoRec);
    return return_value;
}


/*
 * Write code for one module
 */
static int
write_module_files(
		   GenCodeInfo genCodeInfo,
		   GenLog genLog,
		   ABObj module,
		   BOOL merge_files,
		   BOOL check_dates,
		   STRING resFileName,
		   BOOL * headerFileChangedPtr,
		   BOOL * uiCFileChangedPtr,
		   BOOL * stubsCFileChangedPtr
)
{
#define headerFileChanged (*headerFileChangedPtr)
#define uiCFileChanged (*uiCFileChangedPtr)
#define stubsCFileChanged (*stubsCFileChangedPtr)
#define codeFile (genCodeInfo->code_file)
    int                 return_value = 0;
    int                 iRC = 0;/* int return code */
    STRING              errmsg = NULL;
    char                uiHeaderFileName[MAX_PATH_SIZE];
    char                uiHeaderDefineName[MAX_PATH_SIZE];
    char                uiCFileName[MAX_PATH_SIZE];
    char                stubsFileName[MAX_PATH_SIZE];
    char                stubsBakFileName[MAX_PATH_SIZE];
    BOOL                needStubsMerge = FALSE;
    STRING              curFileName = NULL;
    BOOL                curFileSkipped = FALSE;
    ABMF_SKIP_WHY	curFileSkipReason = ABMF_SKIP_UNDEF;
    *uiHeaderFileName = 0;
    *uiHeaderDefineName = 0;
    *uiCFileName = 0;
    *stubsFileName = 0;
    *stubsBakFileName = 0;

    headerFileChanged = FALSE;
    uiCFileChanged = FALSE;
    stubsCFileChanged = FALSE;

    if (!ab_c_ident_is_ok(obj_get_name(module)))
    {
	char *prog_name_string = util_get_program_name();
	char *module_name_string = obj_get_name(module);

	fprintf(stderr, catgets(Dtb_project_catd, 1, 54,
	    "%s: module filename must contain only letters\n"), 
	    prog_name_string);
	fprintf(stderr, catgets(Dtb_project_catd, 1, 55,
	    "and digits.  %s will generate C variables based\n"), 
	    prog_name_string);
	fprintf(stderr, catgets(Dtb_project_catd, 1, 56,
	    "on the module filename.  Please rename %s\n"), module_name_string);
	exit(1);
    }

    /*
     * Determine the file names
     */
    sprintf(uiCFileName, "%s_ui.c", obj_get_name(module));
    sprintf(uiHeaderFileName, "%s_ui.h", obj_get_name(module));
    strcpy(uiHeaderDefineName,
	   abmfP_get_define_from_file_name(uiHeaderFileName));
    sprintf(stubsFileName, "%s_stubs.c", obj_get_name(module));
    sprintf(stubsBakFileName, "%s.BAK", stubsFileName);

    /*
     * ***** WRITE UI HEADER FILE *****
     */
    curFileName = uiHeaderFileName;
    curFileSkipReason = ABMF_SKIP_UNDEF;
    if (!should_write_file(
	uiHeaderFileName, obj_get_file(module), genLog, FALSE, check_dates))
    {
	curFileSkipReason = ABMF_SKIP_UP_TO_DATE;
    }
    else
    {
	print_processing_message(uiHeaderFileName);
	if ((errmsg = abio_open_output(NULL, &codeFile)) != NULL)
	{
	    util_printf_err("%s\n", errmsg);
	    return_code(ERR_OPEN);
	}
	iRC = abmfP_write_ui_header_file(genCodeInfo,
					 module,
					 uiHeaderFileName,
					 uiHeaderDefineName);
	return_if_err(iRC,iRC);

	iRC = check_and_replace_file(
			curFileName,
			genLog,
			codeFile,
			check_dates,
			FALSE,
			&headerFileChanged,
			&curFileSkipReason);
	return_if_err(iRC,iRC);

	abio_close_output(codeFile);
    }				/* date check */
    if (curFileSkipReason != ABMF_SKIP_UNDEF)
    {
	print_skipping_message(curFileName, curFileSkipReason);
	if (freshenUnchangedFiles)
	{
	    touch_file(curFileName);
	}
    }

    /*
     * ***** WRITE UI.C FILE *****
     */
    curFileName = uiCFileName;
    curFileSkipReason = ABMF_SKIP_UNDEF;
    if (!should_write_file(uiCFileName, obj_get_file(module), genLog,
			   FALSE, check_dates))
    {
	curFileSkipReason = ABMF_SKIP_UP_TO_DATE;
    }
    else
    {
	print_processing_message(curFileName);
	if (!util_be_silent())
	{
	    if ((genCodeInfo->resource_file != NULL) && (resFileName != NULL))
	    {
		char *prog_name_string = util_get_program_name();
		fprintf(stderr, catgets(Dtb_project_catd, 1, 57,
			"%s: writing resources for %s into %s\n"),
			prog_name_string,
			uiCFileName,
			resFileName);
	    }
	}
	if ((errmsg = abio_open_output(NULL, &codeFile)) != NULL)
	{
	    util_printf_err("%s\n", errmsg);
	    return_code(ERR_OPEN);
	}
	iRC = abmfP_write_ui_c_file(genCodeInfo, uiCFileName, module);
	return_if_err(iRC, iRC);

	iRC = check_and_replace_file(
			curFileName, 
			genLog,
			codeFile, 
			check_dates,
			FALSE,
			&uiCFileChanged,
			&curFileSkipReason);
	return_if_err(iRC, iRC);

	abio_close_output(codeFile);
    }				/* date check */
    if (curFileSkipReason != ABMF_SKIP_UNDEF)
    {
	print_skipping_message(curFileName, curFileSkipReason);
	if (freshenUnchangedFiles)
	{
	    touch_file(curFileName);
	}
    }


    /*****   WRITE STUBS.C FILE *****/

    /*
     * If the stubs file is already present, make a backup of it.
     */
    curFileName = stubsFileName;
    curFileSkipReason = ABMF_SKIP_UNDEF;
    if (!should_write_file(stubsFileName, obj_get_file(module), genLog,
			   merge_files, check_dates))
    {
	curFileSkipReason = ABMF_SKIP_UP_TO_DATE;
    }
    else
    {
	needStubsMerge = merge_files;
	if (!util_file_exists(stubsFileName))
	{
	    needStubsMerge = FALSE;
	}

	if (curFileSkipReason == ABMF_SKIP_UNDEF)
	{
	    /*
	     * Open the output (temp) file.
	     */
	    File	deltaFile = NULL;

	    errmsg = abio_open_output(NULL, &codeFile);	/* tmp file */
	    if (errmsg != NULL)
	    {
		util_printf_err("%s\n", errmsg);
		return_code(ERR_OPEN);
	    }

	    /*
	     * Write the output file
	     */
	    print_processing_message(stubsFileName);
	    iRC = abmfP_write_stubs_c_file(genCodeInfo, stubsFileName, module);
	    return_if_err(iRC, iRC);

	    /*
	     * Merge the file
	     */
	    iRC = check_and_merge_c_file(
			needStubsMerge,
			&codeFile,
			curFileName,
			&curFileSkipReason,
			&deltaFile);
	    return_if_err(iRC,iRC);
			
	    /*
	     * Replace the file with the new version
	     */
	    iRC = check_and_replace_file(
			curFileName, 
			genLog,
			codeFile, 
			check_dates,
			needStubsMerge,
			&stubsCFileChanged,
			&curFileSkipReason);
	    return_if_err(iRC,iRC);

	    if ((stubsCFileChanged) && (deltaFile != NULL))
	    {
		char	deltaFileName[MAXPATHLEN];
		sprintf(deltaFileName, "%s.delta", stubsFileName);
	        replace_file(deltaFileName, deltaFile, TRUE);
	    }

    	    util_fclose(codeFile);
    	    util_fclose(deltaFile);
	} /* !curFileSkipped */
    } /* date check */
    if (curFileSkipReason != ABMF_SKIP_UNDEF)
    {
	print_skipping_message(curFileName, curFileSkipReason);
	if (freshenUnchangedFiles)
	{
	    touch_file(curFileName);
	}
    }

epilogue:
    abio_close_output(codeFile);
    return return_value;
#undef headerFileChanged
#undef uiCFileChanged
#undef uiStubsFileChanged
#undef codeFile
}


/*
 * Write the code for the main files of a project (project.h, project.c).
 */
static int
write_project_files(
		    GenCodeInfo genCodeInfo,
		    GenLog genLog,
		    ABObj project,
		    BOOL merge_files,
		    BOOL check_dates,
		    BOOL force_write_c_file,
		    BOOL * headerFileChangedPtr,
		    BOOL * stubsCFileChangedPtr
)
{
#define headerFileChanged (*headerFileChangedPtr)
#define stubsCFileChanged (*stubsCFileChangedPtr)
#define codeFile (genCodeInfo->code_file)
    int                 return_value = 0;
    int                 iRC = 0;/* int return code */
    STRING              errmsg = NULL;
    BOOL                needHeaderMerge = FALSE;
    BOOL                needStubsMerge = FALSE;
    char                headerFileName[MAX_PATH_SIZE];
    char                headerDefineName[MAX_PATH_SIZE];
    char                headerBakFileName[MAX_PATH_SIZE];
    char                stubsFileName[MAX_PATH_SIZE];
    char                stubsBakFileName[MAX_PATH_SIZE];
    STRING              curFileName = NULL;
    ABMF_SKIP_WHY	curFileSkipReason = ABMF_SKIP_UNDEF;
    File		orgFile = NULL;
    File		mergedFile = NULL;
    File		deltaFile = NULL;

    headerFileChanged = FALSE;
    stubsCFileChanged = FALSE;

    sprintf(headerFileName, "%s.h", obj_get_name(project));
    strcpy(headerDefineName, abmfP_get_define_from_file_name(headerFileName));
    sprintf(headerBakFileName, "%s.BAK", headerFileName);
    sprintf(stubsFileName, "%s.c", obj_get_name(project));
    sprintf(stubsBakFileName, "%s.BAK", stubsFileName);


    /***** PROJECT HEADER FILE *****/

    curFileName = headerFileName;
    curFileSkipReason = ABMF_SKIP_UNDEF;
    needHeaderMerge = (merge_files && util_file_exists(curFileName));
    if (!should_write_file(
			   headerFileName, obj_get_file(project), genLog,
			   needHeaderMerge, check_dates))
    {
	curFileSkipReason = ABMF_SKIP_UP_TO_DATE;
    }
    else
    {
	/*
	 * Write the beastie
	 */
	print_processing_message(headerFileName);
	if ((errmsg = abio_open_output(NULL, &codeFile)) != NULL)
	{
	    util_printf_err("%s\n", errmsg);
	    return ERR_OPEN;
	}
	iRC = abmfP_write_project_header_file(
					      genCodeInfo,
					      project,
					      headerFileName);
	return_if_err(iRC, iRC);

  	/*
         * Merge the file
         */
	iRC = check_and_merge_c_file(
			needHeaderMerge,
			&codeFile,
			curFileName,
			&curFileSkipReason,
			&deltaFile);
	return_if_err(iRC,iRC);
			
	/*
	 * Replace the old version with the new
	 */
	iRC = check_and_replace_file(
			curFileName, 
			genLog,
			codeFile, 
			check_dates,
			needHeaderMerge,
			&headerFileChanged,
			&curFileSkipReason);
	return_if_err(iRC, iRC);
    }				/* compare_file_times */
    if (curFileSkipReason != ABMF_SKIP_UNDEF)
    {
	print_skipping_message(curFileName, curFileSkipReason);
	if (freshenUnchangedFiles)
	{
	    touch_file(curFileName);
	}
    }
    abio_close_output(codeFile);
    util_fclose(orgFile);
    util_fclose(mergedFile);
    util_fclose(deltaFile);


    /*
     * ***** PROJECT .C  *****
     */
    curFileName = stubsFileName;
    curFileSkipReason = ABMF_SKIP_UNDEF;
    needStubsMerge = (merge_files && util_file_exists(curFileName));
    if (!(force_write_c_file ||
	  should_write_file(stubsFileName, obj_get_file(project),
			    genLog, merge_files, check_dates)))
    {
	curFileSkipReason = ABMF_SKIP_UP_TO_DATE;
    }
    else
    {
	if (util_be_verbose())
	{
	    if (check_dates && force_write_c_file)
	    {
		util_printf(catgets(Dtb_project_catd, 1, 58,
		    "generating %s because of changes in modules\n"),
		    stubsFileName);
	    }
	}

	print_processing_message(stubsFileName);
	errmsg = abio_open_output(NULL, &codeFile);
	if (errmsg != NULL)
	{
	    return_value = ERR_OPEN;
	    util_printf_err("%s\n", errmsg);
	    goto epilogue;
	}
	iRC = abmfP_write_project_c_file(
		       genCodeInfo, stubsFileName, needStubsMerge, project);
	return_if_err(iRC, iRC);

  	/*
         * Merge the file
         */
	iRC = check_and_merge_c_file(
			needStubsMerge,
			&codeFile,
			curFileName,
			&curFileSkipReason,
			&deltaFile);
	return_if_err(iRC,iRC);
			
	/*
	 * Replace the old version with the new
	 */
	iRC = check_and_replace_file(
			curFileName, 
			genLog,
			codeFile, 
			check_dates,
			needStubsMerge,
			&stubsCFileChanged,
			&curFileSkipReason);
	return_if_err(iRC, iRC);

    }				/* compare_file_times */
    if (curFileSkipReason != ABMF_SKIP_UNDEF)
    {
	print_skipping_message(curFileName, curFileSkipReason);
	if (freshenUnchangedFiles)
	{
	    touch_file(curFileName);
	}
    }

epilogue:
    abio_close_output(codeFile);
    util_fclose(orgFile);
    util_fclose(mergedFile);
    util_fclose(deltaFile);
    return return_value;
#undef headerFileChanged
#undef stubsCFileChanged
#undef codeFile
}


static int
write_make_file(
		GenCodeInfo	genCodeInfo,
		GenLog		genLog,
		ABObj		project,
		BOOL		merge_files,
		BOOL		check_dates,
		BOOL		*makeFileChangedPtr
)
{
#define makeFileChanged (*makeFileChangedPtr)
#define makeFile (genCodeInfo->code_file)
    int                 return_value = 0;
    int                 rc = 0;
    STRING              errmsg = NULL;
    int                 intOSMin = ((int) AB_OS_UNDEF) + 1;
    int                 intOSMax = ((int) AB_OS_TYPE_NUM_VALUES) - 1;
    AB_OS_TYPE          actualOSType = util_get_os_type();
    AB_OS_TYPE          curOSType = AB_OS_UNDEF;
    STRING              curOSTypeString = NULL;
    char                curMakeFileName[256] = "";
    BOOL		curMakeFileIsActual = FALSE;
    BOOL		writeCurMakeFile = FALSE;
    STRING              actualMakeFileName = "Makefile";
    BOOL		actualMakeFileModified = FALSE;
    BOOL		actualMakeFileIsOurs = FALSE;
    int                 i = 0;
    merge_files = merge_files;	/* ignored, for now */
    check_dates = check_dates;

    makeFileChanged = FALSE;

    /* if it's not there, we own it */
    actualMakeFileIsOurs = !util_file_exists(actualMakeFileName);

    for (i = intOSMin; i <= intOSMax; ++i)
    {
	writeCurMakeFile = TRUE;
	curMakeFileIsActual = FALSE;
	curOSType = (AB_OS_TYPE) i;
	curOSTypeString = util_os_type_to_ident(curOSType);
	if (curOSTypeString == NULL)
	{
	    continue;
	}
	sprintf(curMakeFileName, "%s.%s", actualMakeFileName, curOSTypeString);

	if (util_paths_are_same_file(curMakeFileName, actualMakeFileName))
	{
	    curMakeFileIsActual = TRUE;
	    actualMakeFileIsOurs = TRUE;
	}
        if (file_changed_since_last_log(genLog, curMakeFileName))
	{
	    writeCurMakeFile = FALSE;
	    if (curMakeFileIsActual)
	    {
		actualMakeFileModified = TRUE;
	    }
	    if (util_be_verbose())
	    {
		char *prog_name_string = util_get_program_name();
		fprintf(stderr, catgets(Dtb_project_catd, 1, 59,
		    "%s: Skipping user-defined %s\n"),
		    prog_name_string, curMakeFileName);
	    }
	}

	if (writeCurMakeFile)
	{
	    print_writing_message(curMakeFileName);
	    if ((errmsg = abio_open_output(curMakeFileName, &makeFile)) != NULL)
	    {
	        util_printf_err("%s\n", errmsg);
	        goto epilogue;
	    }
	    abmfP_write_make_file(genCodeInfo, project, curOSType, FALSE);
	    abio_close_output(makeFile);
	    makeFileChanged = TRUE;
	    log_add_entry(genLog, curMakeFileName, -1, FALSE);
	}
    }				/* for osType */

    if (actualOSType == AB_OS_UNDEF)
    {
	util_printf_err(catgets(Dtb_project_catd, 1, 60,
	    "Could not determine OS type of this machine\n"));
	goto epilogue;
    }

    sprintf(curMakeFileName, "%s.%s",
	    actualMakeFileName, util_os_type_to_ident(actualOSType));
    if (! (actualMakeFileIsOurs && (!actualMakeFileModified)) )
    {
	/* The user has modified this file since we wrote it last */
	if (util_be_verbose())
	{
	    util_printf(catgets(Dtb_project_catd, 1, 61,
	       "Skipping user-defined %s\n"), actualMakeFileName);
	}
    }
    else
    {
	if (!util_be_silent())
	{
	    util_printf(catgets(Dtb_project_catd, 1, 62,
		 "linking %s => %s\n"), curMakeFileName, actualMakeFileName);
	}
	rc = unlink(actualMakeFileName);
	if (util_file_exists(actualMakeFileName))
	{
	    util_printf_err(catgets(Dtb_project_catd, 1, 63,
		"Could not remove %s\n"), actualMakeFileName);
	    return_code(-1);
	}
	rc = link(curMakeFileName, actualMakeFileName);
	if (rc != 0)
	{
	    util_printf_err(catgets(Dtb_project_catd, 1, 64,
		"Could not create link to %s\n"), curMakeFileName);
	    return_code(-1);
	}
	log_add_entry(genLog, actualMakeFileName, -1, FALSE);
    }

epilogue:
    abio_close_output(makeFile);
    return return_value;
#undef makeFileChanged
#undef makeFile
}


static int
write_utils_files(
			GenCodeInfo	genCodeInfo, 
			GenLog		genLog, 
			ABObj		project,
			BOOL		check_dates,
		        BOOL		*headerFileChangedPtr,
		        BOOL 		*CFileChangedPtr
)
{
#define headerFileChanged (*headerFileChangedPtr)
#define CFileChanged (*CFileChangedPtr)
#define codeFile (genCodeInfo->code_file)
    int			return_value = 0;
    int			iRC = 0;		/* int return code */
    STRING		curFileName = NULL;
    ABMF_SKIP_WHY	curFileSkipReason = ABMF_SKIP_UNDEF;
    STRING	utilHFileName = abmfP_get_utils_header_file_name(project);
    STRING 	utilCFileName = abmfP_get_utils_c_file_name(project);
    STRING		errmsg = NULL;

    /***** UTILITY HEADER FILE *****/

    curFileName = utilHFileName;
    curFileSkipReason = ABMF_SKIP_UNDEF;
    /*
     * Write the beastie
     */
    print_processing_message(curFileName);
    if ((errmsg = abio_open_output(NULL, &codeFile)) != NULL)
    {
        util_printf_err("%s\n", errmsg);
        return ERR_OPEN;
    }
    iRC = abmfP_write_utils_header_file(genCodeInfo, curFileName, project);
    return_if_err(iRC, iRC);

    /*
     * Replace the old version with the new
     */
    iRC = check_and_replace_file(
		curFileName, 
		genLog,
		codeFile, 
		check_dates,
		FALSE,
		&headerFileChanged,
		&curFileSkipReason);
    return_if_err(iRC, iRC);
    if (curFileSkipReason != ABMF_SKIP_UNDEF)
    {
	print_skipping_message(curFileName, curFileSkipReason);
	if (freshenUnchangedFiles)
	{
	    touch_file(curFileName);
	}
    }
    abio_close_output(codeFile);


    /*
     * ***** PROJECT .C  *****
     */
    curFileName = utilCFileName;
    curFileSkipReason = ABMF_SKIP_UNDEF;
    {
	print_processing_message(curFileName);
	errmsg = abio_open_output(NULL, &codeFile);
	if (errmsg != NULL)
	{
	    return_value = ERR_OPEN;
	    util_printf_err("%s\n", errmsg);
	    goto epilogue;
	}
        iRC = abmfP_write_utils_c_file(genCodeInfo, curFileName, project);
	return_if_err(iRC, iRC);

	/*
	 * Replace the old version with the new
	 */
	iRC = check_and_replace_file(
			curFileName, 
			genLog,
			codeFile, 
			check_dates,
			FALSE,
			&CFileChanged,
			&curFileSkipReason);
	return_if_err(iRC, iRC);

    }				/* compare_file_times */
    if (curFileSkipReason != ABMF_SKIP_UNDEF)
    {
	print_skipping_message(curFileName, curFileSkipReason);
	if (freshenUnchangedFiles)
	{
	    touch_file(curFileName);
	}
    }

epilogue:
    abio_close_output(codeFile);
    return return_value;
#undef headerFileChanged
#undef CFileChanged
#undef codeFile
}


#ifdef BOGUS
{
#define codeFile (genCodeInfo->code_file)
    int		return_value = 0;
    int		rc = 0;			/* return code */
    STRING	errmsg = NULL;
    STRING	utilHFileName = NULL;
    STRING	utilCFileName = NULL;
    genLog = genLog;	/* avoid warning */

    utilHFileName = abmfP_get_utils_header_file_name(project);
    utilCFileName = abmfP_get_utils_c_file_name(project);

    errmsg = abio_open_output(utilHFileName, &codeFile);
    if (errmsg != NULL)
    {
	util_printf_err("%s\n", errmsg);
    }
    print_processing_message(utilHFileName);
    rc = abmfP_write_utils_header_file(genCodeInfo, utilHFileName, project);
    return_if_err(rc,rc);
    abio_close_output(codeFile);

    errmsg = abio_open_output(utilCFileName, &codeFile);
    if (errmsg != NULL)
    {
	util_printf_err("%s\n", errmsg);
    }
    print_writing_message(utilCFileName);
    rc = abmfP_write_utils_c_file(genCodeInfo, utilCFileName, project);
    return_if_err(rc,rc);
    abio_close_output(codeFile);

epilogue:
    abio_close_output(codeFile);
    return return_value;
#undef codeFile
}
#endif /* BOGUS */


static int	
write_app_resource_file(
			GenCodeInfo	genCodeInfo,
			GenLog		genLog,
			ABObj		project,
			BOOL		merge_files,
			BOOL		check_dates,
			BOOL		*fileChangedOut
)
{
    int		return_value = 0;
    int		rc = 0;			/* return code */
    File	newResFile = NULL;
    char	appResFileName[MAXPATHLEN+1];
    STRING	curFileName = NULL;
    ABMF_SKIP_WHY	curFileSkipReason = ABMF_SKIP_UNDEF;
    *appResFileName = 0;

    /*
     * Write new vanilla resource file
     */
    abmfP_get_app_res_file_name(project, appResFileName, MAXPATHLEN+1);
    print_processing_message(appResFileName);
    newResFile = abmfP_res_file_open(NULL, appResFileName, project, FALSE);
    curFileName = appResFileName;
    abio_printf(newResFile, 
	"! All CDE applications should include the standard Dt resource file\n"
	"#include \"Dt\"\n"
	"\n");
    rc = abmfP_write_app_res_file(newResFile, project, appResFileName);
    if (rc < 0)
    {
	/* error */
    }
    else if (rc == 0)
    {
	appResFileComplete = TRUE;
    }
    else if (rc == 1)
    {
	/* file built successfully, but is incomplete */
	if (!util_be_silent())
	{
	    /*
	     * We don't want to merge this file. Its pointless, since it's
	     * incomplete, and it may make us overwrite an existing  
	     * complete resource file.
	     */
	    abmfP_res_file_close(newResFile);
	    goto epilogue;
	}
    }

    /*
     * Merge the files, if necessary
     */
    if (merge_files)
    {
	File	oldResFile = util_fopen_locked(appResFileName, "r");
	File	mergedResFile = NULL;
	int	rc = 0;

	if (oldResFile != NULL)
	{
	    print_merging_message(curFileName);
	    abmfP_res_file_merge(newResFile, oldResFile, &mergedResFile);
	    assert(rc >= 0);
	    if (mergedResFile != NULL)
	    {
	        util_fclose(newResFile);
	        newResFile = mergedResFile;
	        mergedResFile = NULL;
	    }
	}
    }

    /*
     * See if anything changed
     */
    rc = check_and_replace_file(
		appResFileName,
		genLog,
		newResFile,
		check_dates,
		merge_files,
		fileChangedOut,
		&curFileSkipReason);
    return_if_err(rc,rc);
    if (curFileSkipReason != ABMF_SKIP_UNDEF)
    {
	print_skipping_message(curFileName, curFileSkipReason);
	if (freshenUnchangedFiles)
	{
	    touch_file(curFileName);
	}
    }

epilogue:
    return return_value;
}


static int
write_msg_file(
			GenCodeInfo	genCodeInfo,
			GenLog		genLog,
			ABObj		project,
			BOOL		check_for_changes,
			BOOL		*msgFileChangedOutPtr
)
{
    int			return_value = 0;
    int			rc = 0;			/* return code */
    File		newMsgFile = NULL;
    ABMF_SKIP_WHY	msgFileSkipReason = ABMF_SKIP_UNDEF;
    STRING		curFileName = genCodeInfo->msg_src_file_name;
    AB_TRAVERSAL	trav;
    ABObj		module = NULL;

    update_msg_set(genCodeInfo->msg_file_obj, project);
    for (trav_open(&trav, project, AB_TRAV_MODULES);
	(module = trav_next(&trav)) != NULL; )
    {
	update_msg_set(genCodeInfo->msg_file_obj, module);
    }
    trav_close(&trav);

    /*
     * Build the message catalog
     */
    if (genCodeInfo->i18n_method == ABMF_I18N_XPG4_API)
    {
	print_processing_message(curFileName);
	rc = MsgFile_save(genCodeInfo->msg_file_obj, &newMsgFile);
	return_if_err(rc,rc);

   	rc = check_and_replace_file(
			curFileName,
			genLog,
			newMsgFile,
			check_for_changes,
			TRUE,
			msgFileChangedOutPtr,
			&msgFileSkipReason);
	return_if_err(rc,rc);
    }
    if (msgFileSkipReason != ABMF_SKIP_UNDEF)
    {
	print_skipping_message(curFileName, msgFileSkipReason);
	if (freshenUnchangedFiles)
	{
	    touch_file(curFileName);
	}
    }

epilogue:
    util_fclose(newMsgFile);
    return return_value;
}


/*
 * Ensures that all the messages for the given object have been 
 * created, if the object is being generated.
 *
 * For objects that are not being code-generated, makes sure that no
 * messages will be deleted.
 */
static int
update_msg_set(MsgFile msgFile, ABObj obj)
{
#define do_string(_s) \
	    if ((string = (_s)) != NULL) \
	    { \
		MsgSet_sure_find_msg(msgSet, string); \
	    }

    int			return_value = 0;
    STRING		string = NULL;
    MsgSet		msgSet = NULL;
    ABObj		stringObj = NULL;
    BOOL		objIsProject = obj_is_project(obj);
    AB_TRAVERSAL	trav;
    assert(obj_is_module(obj) || obj_is_project(obj));

    msgSet = MsgFile_obj_sure_find_msg_set(msgFile, obj);
    if (msgSet == NULL)
    {
	return ERR_INTERNAL;
    }

    if (obj_get_write_me(obj))
    {
	MsgSet_set_is_referenced(msgSet, TRUE);
	MsgSet_set_allow_msg_delete(msgSet, TRUE);
	for (trav_open(&trav, obj, AB_TRAV_ALL);
		(stringObj = trav_next(&trav)) != NULL; )
	{
	    if (objIsProject && (obj_get_module(stringObj) != NULL))
	    {
		continue;
	    }
	    do_string(obj_get_arg_string(stringObj));
	    do_string(obj_get_menu_title(stringObj));
	    do_string(obj_get_ok_label(stringObj));
	    do_string(obj_get_msg_string(stringObj));
	    do_string(obj_get_action1_label(stringObj));
	    do_string(obj_get_action2_label(stringObj));
	    do_string(obj_get_initial_value_string(stringObj));
	    do_string(obj_get_icon_label(stringObj));
	    do_string(obj_get_label(stringObj));
	}
	trav_close(&trav);
    }
    else
    {
	MsgSet_set_allow_msg_delete(msgSet, FALSE);
    }

    return return_value;
#undef do_string
}


/*
 * Returns TRUE if the output file should be written
 */
static int
should_write_file(
		  STRING output_file,
		  STRING input_file,
		  GenLog genLog,
		  BOOL merge_files,
		  BOOL check_times
)
{
    static BOOL			initialized = FALSE;
    static time_t		exeDate = (time_t)-1;
    GenLogEntry         	logEntry = NULL;
    struct stat			fileInfo;

/*
    return TRUE;

 * This is supposed to check to see generation parameters have changed
 * that should cause the file to be regenerated. Unfortunately, it's gotten
 * out-of-date and does not catch all cases. 
 *
 * Don't delete this, it can be revamped and re-used later.
 */

/* #ifdef BOGUS		save this - we will need it again */

    if (!initialized)
    {
	char	exePath[MAX_PATH_SIZE];
	STRING	exeDir = NULL;
	STRING	exeFile = NULL;
	*exePath = 0;
	initialized = TRUE;

	exeDir = dtb_get_exe_dir();
	exeFile = util_get_program_name();
	if (exeDir != NULL)
	{
	    strcpy(exePath, exeDir);
	}
	if (exeFile != NULL)
	{
	    strcat(exePath, "/");
	    strcat(exePath, exeFile);
	}

        exeDate = (time_t)0;
        if (stat(exePath, &fileInfo) == 0)
	{
	    exeDate = fileInfo.st_mtime;
	}
    }

    if (!check_times)
    {
	return TRUE;
    }

    /*
     * See if the generation parameters have changed.
     */
    logEntry = log_find_entry_by_name(genLog, output_file);
    if ((logEntry != NULL) && (util_xor(logEntry->merged, merge_files)))
    {
	return TRUE;
    }

    /*
     * See if the code generator executable is newer than this file
     */
    if (stat(output_file, &fileInfo) == 0)
    {
        if (exeDate > fileInfo.st_mtime)
        {
	    return TRUE;
        }
    }

    /* 
     * Check input vs. output timestamps
     */
    return (compare_file_times(input_file, output_file, genLog) > 0);
  /*#endif BOGUS */
}


static int
check_and_merge_c_file(
			BOOL		needMerge,
			File		*codeFileInOut,
			STRING		codeFileName,
			ABMF_SKIP_WHY	*fileSkipReasonInOut,
			File		*deltaFileOut
)
{
    int		return_value = 0;
    int		rc = 0;			/* return code */
    File	codeFile = *codeFileInOut;
    File	mergedFile = NULL;
    File	deltaFile = NULL;
    File	*deltaFilePtr = NULL;
    File	orgFile = NULL;

    #ifdef DEBUG
    if (debugging())
    {
	deltaFilePtr = &deltaFile;	/* delta file is primarily debugging */
    }
    #endif /* DEBUG */

    *deltaFileOut = NULL;

    if (   ((*fileSkipReasonInOut) != ABMF_SKIP_UNDEF)
	|| (!needMerge) )
    {
	return 0;
    }

    print_merging_message(codeFileName);
    orgFile = util_fopen_locked(codeFileName, "r");
    if (orgFile == NULL)
    {
        *fileSkipReasonInOut = ABMF_SKIP_ERR_OCCURRED;
        util_printf_err("%s: %s\n", codeFileName, strerror(errno));
        return_code(-1);
    }
    rewind(codeFile);
    rewind(orgFile);
    rc = abmfP_merge_c_files(
	    codeFile, "new file",
	    orgFile, codeFileName,
	    &mergedFile, deltaFilePtr);
    if (rc < 0)
    {
	*fileSkipReasonInOut = ABMF_SKIP_ERR_OCCURRED;
        return_code(-1);
    }
    
    if (mergedFile == NULL)
    {
        *fileSkipReasonInOut = ABMF_SKIP_NO_CHANGES;
    }
    else
    {
        /* changes were detected and merged */
        util_fclose(codeFile);
        codeFile = mergedFile; mergedFile = NULL;
    }

epilogue:
    util_fclose(orgFile);
    *deltaFileOut = deltaFile;
    *codeFileInOut = codeFile;
    return return_value;
}


/*
 * genLog may be NULL
 */
static int
check_and_replace_file(
			STRING		fileName,
			GenLog		genLog,
			File		newFile, 
			BOOL		check_for_changes,
			BOOL		fileWasMerged,
			BOOL		*fileChangedOutPtr,
			ABMF_SKIP_WHY	*fileSkippedReasonOutPtr
)
{
#define fileSkippedReasonOut (*fileSkippedReasonOutPtr)
    int		return_value = 0;
    int		rc = 0;			/* return code */
    BOOL	fileChanged = FALSE;
    BOOL	updateLog = FALSE;

    fileSkippedReasonOut = ABMF_SKIP_UNDEF;

    if (check_for_changes && util_file_exists(fileName))
    {
        File orgFile = util_fopen_locked(fileName, "r");
        if (orgFile == NULL)
        {
	    fileSkippedReasonOut = ABMF_SKIP_ERR_OCCURRED;
	    util_printf_err(catgets(Dtb_project_catd, 1, 65, "%s: %s\n"),
		fileName, strerror(errno));
	    return_code(-1);
        }
        print_comparing_message(fileName);
        fileChanged = !source_files_equal(newFile, orgFile);
        util_fclose(orgFile);
    }
    else
    {
        fileChanged = TRUE;
    }
    
    if (fileChanged)
    {
	print_writing_message(fileName);

	/*
	 * Check write permission on file
	 */
	if (util_file_exists(fileName))
	{
	    File	file = util_fopen_locked(fileName, "a+");
	    if (file == NULL)
	    {
	        fileSkippedReasonOut = ABMF_SKIP_ERR_OCCURRED;
	        util_printf_err(catgets(Dtb_project_catd, 1, 65, "%s: %s\n"),
		    fileName, strerror(errno));
		return_code(ERR_OPEN);
	    }
	    util_fclose(file);
	}

	/*
	 * Replace the old with the new
	 */
        rc = replace_file(fileName, newFile, TRUE);
	if (rc < 0)
	{
	    fileChanged = FALSE;
	    fileSkippedReasonOut = ABMF_SKIP_ERR_OCCURRED;
	    return_code(rc);
	}
	else
	{
	    /* successful creation of new file */
	    updateLog = TRUE;
	}
    }
    else
    {
	/* no changes - it's up-to-date */
	fileSkippedReasonOut = ABMF_SKIP_NO_CHANGES;
	if (freshenUnchangedFiles)
	{
	    touch_file(fileName);		/* update the timestamp */
	}
	updateLog = TRUE;
    }

epilogue:
    if (updateLog && (genLog != NULL))
    {
        log_add_entry(genLog, fileName, -1, fileWasMerged);
    }
    *fileChangedOutPtr = fileChanged;
    return return_value;
#undef fileSkippedReasonOut
} /* check_and_replace_file */


static int
move_file(STRING fileName, STRING newFileName, BOOL force)
{
    int		rc = 0;

    if (force)
    {
	unlink(newFileName);
    }

    rc = link(fileName, newFileName);
    if (rc != 0)
    {
	return -1;
    }

    rc = unlink(fileName);
    if (rc != 0)
    {
	return -1;
    }

    return 0;
}


static int
replace_file(STRING fileName, File fromFile, BOOL rewindFiles)
{
    int		c;
    char	bakFileName[MAXPATHLEN];
    File	toFile = NULL;


    if (rewindFiles)
    {
	rewind(fromFile);
    }

    if (util_file_exists(fileName))
    {
        sprintf(bakFileName, "%s.BAK", fileName);
        print_backing_up_message(fileName, bakFileName);
        move_file(fileName, bakFileName, TRUE);
    }

    toFile = util_fopen_locked(fileName, "w");
    if (toFile == NULL)
    {
	util_printf_err(catgets(Dtb_project_catd, 1, 65, "%s: %s\n"),
	    fileName, strerror(errno));
	return ERR_OPEN;
    }

    while ((c = fgetc(fromFile)) != EOF)
    {
	fputc(c, toFile);
    }

    util_fclose(toFile);
    return 0;
}


static int
print_progress_message(int verbosity, STRING message, STRING fileName)
{
    if (util_get_verbosity() >= verbosity)
    {
	util_printf_err(catgets(Dtb_project_catd, 1, 67, "%s %s\n"),
		message, fileName);
    }
    return 0;
}


static int
print_skipping_message(STRING fileName, ABMF_SKIP_WHY why)
{
    if (util_be_verbose())
    {
	switch (why)
	{
	    case ABMF_SKIP_NO_CHANGES: 
		util_printf(catgets(Dtb_project_catd, 1, 68,
		    "skipping (no changes) %s\n"), fileName);
		break;
	    case ABMF_SKIP_UP_TO_DATE: 
		util_printf(catgets(Dtb_project_catd, 1, 69,
		    "skipping (up-to-date) %s\n"), fileName);
		break;
	    case ABMF_SKIP_ERR_OCCURRED: 
		util_printf(catgets(Dtb_project_catd, 1, 70,
		    "skipping due to errors %s\n"), fileName);
		break;
	}
    }
    return 0;
}


static int
print_backing_up_message(STRING fileFromName, STRING fileToName)
{
    if (util_be_verbose())
    {
	util_printf(catgets(Dtb_project_catd, 1, 71, 
	  "saving previous %s to %s\n"), fileFromName, fileToName);
    }
    return 0;
}


/*
 * Returns: < 0 if file1 < file2 (file1 is older) 0 if file1 == file2 (files
 * have same mod. time) > 0 if file1 > file2 (file1 is newer)
 */
static int
compare_file_times(STRING input_file, STRING output_file, GenLog genLog)
{
    int                 return_value = 0;
    GenLogEntry         logEntry = log_find_entry_by_name(genLog, output_file);
    struct stat         fileInfo;

    if (logEntry == NULL)
    {
	/* we don't have any record of output file, so input file is newer */
	return 1;
    }
    if (!util_file_exists(output_file))
    {
	/* output file doesn't exit - input file is newer */
	return 1;
    }

    if (stat(input_file, &fileInfo) != 0)
    {
	/* the input file doesn't exist? return 1 to force the issue */
	return 1;
    }

    return_value = 0;
    if (fileInfo.st_mtime > logEntry->mod_time)
    {
	return_value = 1;
    }
    else if (fileInfo.st_mtime < logEntry->mod_time)
    {
	return_value = -1;
    }

    return return_value;
} 


/*
 * See if the user has fiddled with this file
 */
static BOOL
file_changed_since_last_log(GenLog genLog, STRING fileName)
{
    BOOL	fileWasModified = FALSE;
    GenLogEntry	logEntry = NULL;
    time_t	lastLogWriteTime = 0;

    logEntry = log_find_entry_by_name(genLog, fileName);
    lastLogWriteTime = (logEntry == NULL? 0: logEntry->mod_time);
    fileWasModified = (get_file_mod_time(fileName) > lastLogWriteTime);

    return fileWasModified;
}


/*
 * Returns (time_t)-1 if file not found, or other error
 */
static time_t
get_file_mod_time(STRING fileName)
{
    time_t	fileModTime = (time_t)-1;
    struct stat	fileInfo;
    if (stat(fileName, &fileInfo) == 0)
    {
	fileModTime = fileInfo.st_mtime;
    }
    return fileModTime;
}


/*
 * POSIX gives us struct -> local time, but no struct -> UTC, so we've to to
 * write our own.
 */
static              time_t
mkgmtime(struct tm * localTimeStruct)
{
    static long         timeDiff = 0;
    time_t              gmTime = (time_t) - 1;
    time_t              curTime = 0;
    time_t              localTime = 0;
    time_t              adjustedTime = 0;
    struct tm          *gmTimeStruct = NULL;
    int                 localTimeIsDST = 0;

    /*
     * Determine time difference between localtime and gmtime
     */
    curTime = time(NULL);
    localTimeIsDST = localTimeStruct->tm_isdst;
    if (localTimeIsDST < 0)
    {
	/* we're supposed to figure out DST */
	struct tm *curTimeStruct = localtime(&curTime);
	localTimeIsDST = curTimeStruct->tm_isdst;
    }
    gmTimeStruct = gmtime(&curTime);
    if (gmTimeStruct == NULL)
    {
	goto epilogue;
    }
    gmTimeStruct->tm_isdst = localTimeIsDST;
    adjustedTime = mktime(gmTimeStruct);
    if (adjustedTime == (time_t) - 1)
    {
	goto epilogue;
    }
    timeDiff = ((long) curTime) - ((long) adjustedTime);

    /*
     * Convert given time as a local time and adjust
     */
    localTime = mktime(localTimeStruct);
    if (localTime == (time_t) - 1)
    {
	goto epilogue;
    }
    gmTime = (time_t) (((long) localTime) + ((long) timeDiff));

epilogue:
    return gmTime;
}


static int
touch_file(STRING fileName)
{
    int rc = utime(fileName, NULL);
    return (rc == 0? OK:ERR);
}


/*
 * Modifies: current position on file1,file2
 * parameters may be NULL
 */
static BOOL
source_files_equal(FILE *file1, FILE *file2)
{
    BOOL	filesSame = TRUE;
    BOOL	done = FALSE;
    int		file1Char = ' ';
    int		file2Char = ' ';

    if (file1 == file2)
    {
	return TRUE;
    }
    else if ((file1 == NULL) || (file2 == NULL))
    {
	/* one is NULL, the other is not */
	return FALSE;
    }

    rewind(file1);
    rewind(file2);

    while (!done)
    {
	if (file1Char != EOF)
	{
	    file1Char = fgetc(file1); 
	}
	if (file2Char != EOF)
	{
	    file2Char = fgetc(file2); 
	}

	filesSame = (file1Char == file2Char);
	done =  ((!filesSame) || ((file1Char == EOF) && (file2Char == EOF)));
    }

    return filesSame;
}


/*************************************************************************
 **									**
 **			GenLog						**
 **									**
 *************************************************************************/

static int
log_construct(GenLog log)
{
    log->log_file = NULL;
    log->num_entries = 0;
    log->entries_size = 0;
    log->entries = NULL;
    return logP_read(log);
}


static int
log_destruct(GenLog log)
{
    logP_write(log);
    logP_release_data(log);
    return 0;
}


static int
logP_release_data(GenLog log)
{
    int                 i;

    istr_destroy(log->log_file);

    for (i = 0; i < log->entries_size; ++i)
    {
	istr_destroy(log->entries[i].file_name);
	log->entries[i].mod_time = (time_t) 0;
    }
    log->num_entries = 0;
    log->entries_size = 0;

    util_free(log->entries);

    return 0;
}

static              GenLogEntry
log_find_entry_by_name(GenLog log, STRING file_name)
{
    GenLogEntry         return_value = NULL;
    ISTRING             istr_file_name = istr_dup_existing(file_name);
    int                 i = 0;

    if (istr_file_name == NULL)
    {
	return_code(NULL);
    }

    for (i = 0; i < log->num_entries; ++i)
    {
	if (istr_equal(istr_file_name, log->entries[i].file_name))
	{
	    return_value = &(log->entries[i]);
	    break;
	}
    }


epilogue:
    istr_destroy(istr_file_name);
    return return_value;
}


/*
 * If mod_time is (time_t)-1, the file itself is checked for modification
 * time.
 */
static int
log_add_entry(GenLog log, STRING file_name, time_t mod_time, BOOL merged)
{
    int                 return_value = 0;
    GenLogEntry         entry = log_find_entry_by_name(log, file_name);
    int                 new_entries_size = 0;
    int                 new_num_entries = 0;
    GenLogEntry         new_entries = NULL;

    if (mod_time == (time_t)-1)
    { 
	mod_time = get_file_mod_time(file_name);
    }

    if (entry != NULL)
    {
	entry->mod_time = mod_time;
	entry->merged = merged;
	return_code(0);
    }

    /*
     * Allocate space to append
     */
    new_entries_size = log->entries_size;
    new_num_entries = log->num_entries + 1;
    new_entries_size = util_max(new_num_entries, new_entries_size);
    new_entries = (GenLogEntry) realloc(log->entries,
				 new_entries_size * sizeof(GenLogEntryRec));

    if (new_entries == NULL)
    {
	return_code(ERR_NO_MEMORY);
    }

    log->entries = new_entries;
    log->entries_size = new_entries_size;
    log->num_entries = new_num_entries;

    /*
     * Fill in the new data
     */
    entry = &(log->entries[(log->num_entries) - 1]);
    entry->file_name = istr_create(file_name);
    entry->mod_time = mod_time;
    entry->merged = merged;

epilogue:
    return return_value;
}



static int
log_dump(GenLog log)
{
#ifndef DEBUG
    return 0;
#else
    int                 i = 0;
    GenLogEntry         entry = NULL;

    if (log == NULL)
    {
	util_dprintf(0, "NULL log\n");
	return 0;
    }
    if (log->num_entries == 0)
    {
	util_dprintf(0, "Empty log file:%s\n", istr_string_safe(log->log_file));
	return 0;
    }
    util_dprintf(0, "===== Gen Log (file %s) =====\n",
		 istr_string_safe(log->log_file));
    for (i = 0; i < log->num_entries; ++i)
    {
	entry = &(log->entries[i]);
	util_dprintf(0, "%s %ld merged:%d\n",
	       istr_string_safe(entry->file_name), (long) (entry->mod_time),
		     entry->merged);
    }
    util_dprintf(0, "===================\n");
    return 0;
#endif				/* DEBUG */
}


static int
logP_read(GenLog log)
{
    int                 return_value = 0;
    char                log_file_name[MAXPATHLEN] = "";
    char                line_buf[MAXPATHLEN] = "";
    GenLogEntry         next_entry = NULL;
    int                 i = 0;
    int                 num_lines = 0;
    File                file = NULL;
    char               *token_start = NULL;
    char               *line_ptr = NULL;
    char               *line_tmp_ptr = NULL;
    int                 c = 0;
    int                 year = 0;
    int                 month = 0;
    int                 day = 0;
    int                 hour = 0;
    int                 minute = 0;
    int                 second = 0;
    struct tm           time_struct;

    logP_release_data(log);	/* re-init */

    sprintf(log_file_name, ".%s.log", util_get_program_name());
    log->log_file = istr_create(log_file_name);
    file = util_fopen_locked(log_file_name, "rt");
    if (file == NULL)
    {
	return_code(0);
    }

    num_lines = 0;
    while ((c = getc(file)) != EOF)
    {
	if ((c == '\n') || (c == '\r'))
	{
	    ++num_lines;
	}
    }

    if (num_lines < 1)
    {
	return_code(0);
    }

    log->entries_size = num_lines;
    log->entries = (GenLogEntry) util_malloc(
				log->entries_size * sizeof(GenLogEntryRec));
    if (log->entries == NULL)
    {
	log->entries_size = 0;
	return_code(ERR_NO_MEMORY);
    }
    for (i = 0; i < log->entries_size; ++i)
    {
	next_entry = &(log->entries[i]);
	next_entry->file_name = NULL;
	next_entry->mod_time = 0;
	next_entry->merged = FALSE;
    }

    rewind(file);

    for (; (!feof(file)) && (log->num_entries < log->entries_size);)
    {
	next_entry = &(log->entries[(log->num_entries)]);

	*line_buf = 0;
	fgets(line_buf, MAXPATHLEN, file);
	line_ptr = line_buf;

	/*
	 * file name
	 */
	token_start = line_ptr;
	line_ptr = strchr(line_ptr, ' ');
	if (line_ptr == NULL)
	{
	    continue;		/* error - skip this one */
	}
	*line_ptr = 0;
	if (strlen(token_start) <= (size_t) 0)
	{
	    continue;		/* an error - skip this entry */
	}
	next_entry->file_name = istr_create(token_start);
	*line_ptr = ' ';

	/*
	 * Generation time
	 */
	year = logP_get_int_from_string(&line_ptr);
	month = logP_get_int_from_string(&line_ptr);
	day = logP_get_int_from_string(&line_ptr);
	hour = logP_get_int_from_string(&line_ptr);
	minute = logP_get_int_from_string(&line_ptr);
	second = logP_get_int_from_string(&line_ptr);
	time_struct.tm_year = year - 1900;
	time_struct.tm_mon = month - 1;
	time_struct.tm_mday = day;
	time_struct.tm_hour = hour;
	time_struct.tm_min = minute;
	time_struct.tm_sec = second;
	time_struct.tm_isdst = -1;	/* let sys determine DST or no */

	if (line_ptr == NULL)
	{
	    continue;		/* an error - skip this line */
	}

	next_entry->mod_time = mkgmtime(&time_struct);
	if (next_entry->mod_time == (time_t) - 1)
	{
	    continue;
	}

	/*
	 * merged
	 */
	line_tmp_ptr = strstr(line_ptr, "merged:");
	if (line_tmp_ptr == NULL)
	{
	    continue;		/* an error - skip this entry */
	}
	else
	{
	    line_tmp_ptr += strlen("merged:");
	    next_entry->merged = (tolower(*line_tmp_ptr) == 'y');
	    if ((*line_tmp_ptr) != 0)
	    {
		++line_tmp_ptr;
	    }
	    line_ptr = line_tmp_ptr;
	}

	++(log->num_entries);
    }

epilogue:
    util_fclose(file);
    /* util_dprintf(0, "After read "); log_dump(log); */
    return return_value;
}


static int
logP_write(GenLog log)
{
    int                 return_value = 0;
    STRING              log_file_name = istr_string(log->log_file);
    File                file = NULL;
    GenLogEntry         entry = NULL;
    int                 i = 0;
    struct tm          *time_struct = NULL;
    STRING              file_name = NULL;

    if (log_file_name == NULL)
    {
	return_code(0);
    }
    file = util_fopen_locked(log_file_name, "wt");
    if (file == NULL)
    {
	return_code(ERR);
    }

    for (i = 0; i < log->num_entries; ++i)
    {
	entry = &(log->entries[i]);
	file_name = istr_string(entry->file_name);

	/* this is mostly in case we mucked things up. */
	if ((file_name == NULL) || (!util_file_exists(file_name)))
	{
	    continue;
	}

	time_struct = gmtime(&(entry->mod_time));
	fprintf(file, "%s %04d.%02d.%02d.%02d.%02d.%02d",
		file_name,
		time_struct->tm_year + 1900,
		time_struct->tm_mon + 1,
		time_struct->tm_mday,
		time_struct->tm_hour,
		time_struct->tm_min,
		time_struct->tm_sec);

	fprintf(file, " merged:%s", (entry->merged ? "y" : "n"));

	fprintf(file, "\n");
    }

epilogue:
    util_fclose(file);
    return return_value;
}


static int
logP_get_int_from_string(STRING * ptr_ptr)
{
#define ptr (*ptr_ptr)
    int                 int_value = 0;
    char               *tok_start = NULL;

    if (ptr == NULL)
    {
	return 0;
    }

    tok_start = ptr;
    while (((*tok_start) != 0) && (!isdigit(*tok_start)))
    {
	++tok_start;
    }

    if ((*tok_start) != 0)
    {
	int_value = atoi(tok_start);

	for (ptr = tok_start; ((*ptr) != 0) && isdigit(*ptr); ++ptr)
	{
	}
    }

    return int_value;
#undef ptr
}
