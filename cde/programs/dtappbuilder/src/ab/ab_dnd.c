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
 *	$XConsortium: ab_dnd.c /main/3 1995/11/06 17:13:05 rswiston $
 *
 * @(#)ab_dnd.c	1.19 18 Apr 1995	cde_app_builder/src/
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */


#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/DragDrop.h>
#include <Dt/Dt.h>
#include <Dt/Dts.h>
#include <Dt/Dnd.h>
#include <ab_private/ab.h>
#include <ab_private/ab_dnd.h>
#include <ab_private/ab_bil.h>
#include <ab_private/ui_util.h>
#include <ab_private/proj.h>
#include "palette_ui.h"
#include "proj_ui.h"

/*
 * ab_dnd.c
 */

static void	textTransferCB(
		    Widget	widget,
		    XtPointer	clientData,
    		    XtPointer	callData
		);

static void	textAnimateCB(
		    Widget	widget,
		    XtPointer	clientData,
		    XtPointer	callData
		);

static int	dnd_load_file(
		    STRING	fileName
		);

static int 	dnd_load_buffer(
		    DtDndBuffer dropBuf
		);

/*
 * ab_dnd_register_drop_file
 *
 * Registers widget to accept drops of files.
 * NOTE: DtAppInitialize() or DtDragInitialize()
 * must be called prior to calling this function.
 */
void
ab_dnd_register_drop_file
(
    Widget	dropFile
)
{
    static XtCallbackRec transferCBRec[] = { {textTransferCB, NULL},
						{NULL, NULL} };
    static XtCallbackRec animateCBRec[]  = { {textAnimateCB, NULL},
						{NULL, NULL} };
    Arg			arg[1];

    if (!dropFile)
	return;

    XtSetArg(arg[0], DtNdropAnimateCallback, animateCBRec);

    DtDndDropRegister(dropFile, 
			DtDND_FILENAME_TRANSFER | DtDND_BUFFER_TRANSFER, 
			XmDROP_COPY,
			transferCBRec, 
			arg, 
			1);		/* number of args in argument list */
}

/*
 * textTransferCB
 *
 * Handles transfer of files or text to the text edit. Files are transfered
 * by placing their name in the field, text by inserting the text into the
 * field.
 */
static void
textTransferCB
(
    Widget	widget,
    XtPointer	clientData,
    XtPointer	callData
)
{
    DtDndTransferCallbackStruct *transferInfo = 
			(DtDndTransferCallbackStruct *) callData;
    String		newString = NULL;
    int			iRet = 0;

    switch (transferInfo->dropData->protocol) 
    {
	case DtDND_FILENAME_TRANSFER:
            newString = XtNewString((char *)
                transferInfo->dropData->data.files[0]);
	    if (dnd_load_file(newString) < 0)
		transferInfo->status = DtDND_FAILURE;
	    break;

	case DtDND_BUFFER_TRANSFER:
	    iRet = dnd_load_buffer(transferInfo->dropData->data.buffers[0]);
	    if (iRet < 0)
                transferInfo->status = DtDND_FAILURE;	
		
	    break;

	default:
	    break;
    }
}

static int
dnd_load_buffer(
    DtDndBuffer	dropBuf
)
{
    char	*dataType = NULL;
    FILE	*inFile = NULL;
    int		iRet = 0;
    ABObj	project = proj_get_project();
    STRING	fmtStr = NULL, fileName = NULL, msg = NULL;
    XmString	xm_str = NULL;
    DTB_MODAL_ANSWER	answer = DTB_ANSWER_NONE;

    dataType = (char *) DtDtsBufferToDataType(
		dropBuf.bp, dropBuf.size, dropBuf.name);

    inFile = util_create_tmp_file((char *) dropBuf.bp);
    if (inFile == NULL)
    {
        (void)dtb_show_modal_message(dtb_get_toplevel_widget(),
                &dtb_palette_drop_err_msg, NULL, NULL, NULL);
        iRet = -1;
	goto epilogue;
    }

    fileName = dropBuf.name? dropBuf.name : catgets(Dtb_project_catd, 100, 104, "NoName");
    fileName = XtNewString(fileName);
    
    if ( (strcmp(dataType, BIX) == 0) ||
	 (strcmp(dataType, BIP) == 0) )
    {
	/* Before loading in the new project, check if
	 * there are unsaved edits in the existing project.
	 * If so, post a warning and allow the user to
	 * cancel the operation.
	 */
	if (proj_check_unsaved_edits(project))
	{  
            answer = dtb_show_modal_message(dtb_get_toplevel_widget(),
                &dtb_proj_open_proj_warn_msg,
                NULL, NULL, NULL);
 
            switch (answer)
            {
            	case DTB_ANSWER_ACTION1:
                    /* Go ahead and destroy the old project. */
                    proj_destroy_project(project);
		    iRet = ab_load_project(fileName, inFile, TRUE);
                break;
 
            	case DTB_ANSWER_CANCEL:
		    iRet = -1;
                    break;
            }
	}
	else
	{
	    iRet = ab_load_project(fileName, inFile, TRUE);
	}
    }
    else if (strcmp(dataType, BIL) == 0)
    {
	iRet = ab_import_module(fileName, inFile, FALSE);
    }
    else 
    {
	fmtStr = XtNewString(catgets(Dtb_project_catd, 100, 103,
	"Cannot determine file type for %s.\nDo you want to load the file as a project or a module?"));
	
	msg = (STRING) util_malloc(strlen(fmtStr) + strlen(fileName) + 1);
	sprintf(msg, fmtStr, fileName);
	xm_str = XmStringCreateLocalized(msg);

	answer = dtb_show_modal_message(dtb_get_toplevel_widget(),
		&dtb_proj_ld_as_bip_or_bil_msg, xm_str, NULL, NULL);

	XtFree(fmtStr);
	util_free(msg);
	XmStringFree(xm_str);

	switch (answer)
	{
	    case DTB_ANSWER_ACTION1:	/* Load as Project */
		if (proj_check_unsaved_edits(project))
		{
		    answer = dtb_show_modal_message(dtb_get_toplevel_widget(),
			     &dtb_proj_open_proj_warn_msg,
			     NULL, NULL, NULL);

		    switch (answer)
		    {
			case DTB_ANSWER_ACTION1:
			    /* Go ahead and destroy the old project.  */
			    proj_destroy_project(project);
			    iRet = ab_load_project(fileName, inFile, TRUE);
			    break;

			case DTB_ANSWER_CANCEL:
			    iRet = -1;
			    break;
		    }   
        	}
		else
		{
		    iRet = ab_load_project(fileName, inFile, TRUE);
		}
	 	break;

	    case DTB_ANSWER_ACTION2:	/*Load as Module */
		iRet = ab_import_module(fileName, inFile, FALSE);
		break;

	    case DTB_ANSWER_CANCEL:
		iRet = -1;
		break;
	}
    }
   
    if (fileName)
	XtFree(fileName);

epilogue:
    return (iRet);
}

static int
dnd_load_file(
    STRING	fileName
)
{
    char		*dataType = NULL;
    ABObj		project = proj_get_project();
    STRING      	fmtStr = NULL, msg = NULL;
    XmString    	xm_str = NULL;
    int			iRet = 0;
    DTB_MODAL_ANSWER    answer = DTB_ANSWER_NONE;

    /* A file has been dropped on dtbuilder. */
    if (!util_strempty(fileName)) 
    {
        util_dprintf(2, "Dropped file %s on App Builder\n", fileName);

	dataType = (char *) DtDtsFileToDataType(fileName);
	if ( (strcmp(dataType, BIP) == 0) ||
             (strcmp(dataType, BIX) == 0) )
	{
	    /* Before loading in the new project, check if
             * there are unsaved edits in the existing project.
             * If so, post a warning and allow the user to
             * cancel the operation.
             */
            if (proj_check_unsaved_edits(project))
            {
            	answer = dtb_show_modal_message(dtb_get_toplevel_widget(),
                	&dtb_proj_open_proj_warn_msg,
                	NULL, NULL, NULL);

            	switch (answer)        
            	{
                    case DTB_ANSWER_ACTION1:
                    	/* Go ahead and destroy the old project. */
                    	proj_destroy_project(project);
			iRet = ab_check_and_open_bip(fileName);
                    break;

                    case DTB_ANSWER_CANCEL:
                    	iRet = -1;
                    break;
            	}
            }
            else
            {
		iRet = ab_check_and_open_bip(fileName);
            }
	}
	else if (strcmp(dataType, BIL) == 0)
	{
	    answer = dtb_show_modal_message(dtb_get_toplevel_widget(),
			&dtb_palette_import_by_msg, NULL, NULL, NULL);
	    switch (answer)
	    {
		case DTB_ANSWER_ACTION1:	/* Copy */
		    iRet = ab_check_and_import_bil(fileName, TRUE);
		    break;
	
		case DTB_ANSWER_ACTION2:	/* Reference */
		    iRet = ab_check_and_import_bil(fileName, FALSE);
		    break;

		case DTB_ANSWER_CANCEL:
		    iRet = -1;
		    break;
 	    }
	}
	else
	{
            fmtStr = XtNewString(catgets(Dtb_project_catd, 100, 103,
        	"Cannot determine file type for %s.\nDo you want to load the file as a project or a module?"));
            msg = (STRING) util_malloc(strlen(fmtStr) + strlen(fileName) + 1);
            sprintf(msg, fmtStr, fileName);
            xm_str = XmStringCreateLocalized(msg);
 
            answer = dtb_show_modal_message(dtb_get_toplevel_widget(),
                &dtb_proj_ld_as_bip_or_bil_msg, xm_str, NULL, NULL);
 
            XtFree(fmtStr);
            util_free(msg);
            XmStringFree(xm_str);

	    switch (answer)
            {
            	case DTB_ANSWER_ACTION1:    /* Load as Project */
                    if (proj_check_unsaved_edits(project))
                    {
			answer = dtb_show_modal_message(
				dtb_get_toplevel_widget(),
                             	&dtb_proj_open_proj_warn_msg,
                             	NULL, NULL, NULL);
 
			switch (answer)
                    	{
			    case DTB_ANSWER_ACTION1:
                            	/* Go ahead and destroy the old project.  */
                            	proj_destroy_project(project);
			    	iRet = ab_check_and_open_bip(fileName);
				break;
 
                            case DTB_ANSWER_CANCEL:
                            	iRet = -1;
                            	break;
                    	}
                    }
		    else
                    {
			iRet = ab_check_and_open_bip(fileName);
                    }
                    break;
 
		case DTB_ANSWER_ACTION2:    /*Load as Module */
            	    answer = dtb_show_modal_message(dtb_get_toplevel_widget(),
                        &dtb_palette_import_by_msg, NULL, NULL, NULL);
            	    switch (answer)
            	    {
                	case DTB_ANSWER_ACTION1:        /* Copy */
                    	    iRet = ab_check_and_import_bil(fileName, TRUE);
                    	break;

                	case DTB_ANSWER_ACTION2:        /* Reference */
                    	    iRet = ab_check_and_import_bil(fileName, FALSE);
                    	break;
 
                	case DTB_ANSWER_CANCEL:
                    	    iRet = -1;
                    	break;
            	    }
                    break;
 
            	case DTB_ANSWER_CANCEL:
                    iRet = -1;
                    break;
	    }
	}
    }
    else iRet = -1;

    return (iRet);	
}

/*
 * textAnimateCB
 *
 * Not implemented.
 */
static void
textAnimateCB
(
    Widget	widget,
    XtPointer	clientData,
    XtPointer	callData
)
{
    /* REMIND: Do something! */
}

/*
 * ab_dnd_unregister_drop
 *
 * Unregisters widget to accept drops of files.
 * NOTE: DtAppInitialize() or DtDragInitialize()
 * must be called prior to calling this function.
 */
void
ab_dnd_unregister_drop
(
    Widget	dropFile
)
{
    if (!dropFile)
	return;
    
    DtDndDropUnregister(dropFile);
}
