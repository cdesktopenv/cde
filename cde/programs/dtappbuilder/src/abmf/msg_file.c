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
 * $XConsortium: msg_file.c /main/4 1996/11/21 12:31:25 mustafa $
 * 
 * @(#)msg_file.c	1.1 15 Jul 1994	cde_app_builder/src/abmf
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
 * msg_file.c - ADT implementation for message file [re]generation
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <ab_private/abio.h>
#include "msg_fileP.h"

#define INIT_ALLOCED_SLOTS	16


static BOOL	msgFile_load(
		    MsgFile	that,
		    char	*filename
		);
static BOOL	msgFile_read_top_comment(
		    MsgFile	that
		);
static BOOL	msgFile_read_msg_set(
		    MsgFile	that
		);
static MsgSet	msgFile_get_msg_set_by_num(
		    MsgFile	that,
		    int		set_num
		);
static MsgSet	msgFile_add_msg_set(
		    MsgFile	that,
		    char	*module_name,
		    int		set_number,
		    BOOL	is_generated
		);
static int	msgFile_max_set_number(
		    MsgFile	that
		);
static MsgSetRec *msgSet_create_new(
		    int		set_number,
		    char	*name,
		    char	*comment,
		    BOOL	is_generated
		 );
static char	*msgSet_get_name(
		    MsgSet that
		);
static BOOL	msgSet_is_generated(
		    MsgSet that
		);
static void	msgSet_output(
		    MsgSet	that,
		    FILE	*fp
		);
static Msg	msgSet_add_msg(
		    MsgSet	that,
		    char	*string,
		    char	*comment,
		    int		msg_num
		);
static int	msgSet_max_msg_number(
		    MsgSet	that
		);
static Msg	msg_create(
		    char	*string,
		    char	*comment,
		    int		number
		);
static void	msg_destroy(
		    Msg		that
		);
static char *	msg_get_string(
		    Msg		that
		);
static int	msg_get_number(
		    Msg		that
		);
static void	msg_set_is_referenced(
		    Msg		that,
		    BOOL	is_referenced
		);
static BOOL	msg_is_referenced(
		    Msg		that
		);
static void	msg_output(
		    Msg		that,
		    FILE	*fp
		);

static void	out_str_puts(
		    char	*string,
		    FILE	*fp
		);
static char *	in_str_dup(
		    char	*string
		);

static BOOL	get_set_number_and_name(
		    FILE	*fp,
		    int		*set_num,
		    char	**set_name
		);
static char *	get_comment(
		    FILE	*fp
		);
static BOOL	get_msg_and_comment(
		    FILE	*fp,
		    int		*msg_num,
		    char	**msg_str,
		    char	**msg_cmt
		);
static BOOL	get_message(
		    FILE	*fp,
		    int		*msg_num,
		    char	**msg_str
		);
static char *	extract_bil_name(
		    char	*set_line_comment
		);
static char *	jump_backslash_gets(
		    FILE	*fp
		);
static void	jump_backslash_ungets(
		    char	*buf
		);

/* These are obsolete functions and should go away eventually */
static char *	nl_dup(
		    char	*string
		);
static void	nl_puts(
		    char	*string,
		    FILE	*fp
		);

/* MsgSet Methods */

MsgSet
MsgSet_create(
    int		set_number,
    char	*name
    )
{
    MsgSetRec	*ret_val = (MsgSetRec *)NULL;

    if (set_number <= 0)
        goto cret;

    ret_val = msgSet_create_new(set_number, name, NULL, TRUE);

cret:
    return(ret_val);
}


int
MsgSet_destroy(
    MsgSet that
    )
{
    if (that == (MsgSet)NULL)
        goto cret;

    if (that->is_generated && that->msgs != (Msg *)NULL)
    {
        int	i;

        for (i = 0; i < that->num_msgs; i++)
            msg_destroy(that->msgs[i]);

        free(that->msgs);
    }
    free(that);

cret:
    return 0;
}


int
MsgSet_get_number(
    MsgSet that
    )
{
    int	ret_val = -1;

    if (that == (MsgSet)NULL)
        goto cret;

    ret_val = that->set_number;

cret:    
    return(ret_val);
}


int
MsgSet_set_allow_msg_delete(MsgSet msgSet, BOOL allow_delete)
{
    msgSet->allow_msg_delete = allow_delete;
    return 0;
}


BOOL
MsgSet_get_allow_msg_delete(MsgSet msgSet)
{
   return  msgSet->allow_msg_delete;
}


int
MsgSet_just_find_msg(
    MsgSet	that,
    char	*string
    )
{
    int			ret_val = -1;
    int	i;

    if (that == NULL || string == (char *)NULL)
        goto cret;
    
    for (i = 0; i < that->num_msgs; i++)
        if (!strcmp(string, msg_get_string(that->msgs[i])))
            break;

    if (i == that->num_msgs)
        goto cret;

    ret_val = msg_get_number(that->msgs[i]);

cret:    
    return(ret_val);
}


int
MsgSet_sure_find_msg(	/* adds it, if not already there */
    MsgSet	that,
    char	*string
    )
{
    int	ret_val = -1;
    Msg	msg;

    if (that == (MsgSet)NULL)
        goto cret;

    if ((ret_val = MsgSet_just_find_msg(that, string)) != -1)
        goto cret;

    if ((msg = msgSet_add_msg(that, string, NULL,
                              msgSet_max_msg_number(that)+1)) == (Msg)NULL)
        goto cret;
    
    ret_val = msg_get_number(msg);

cret:    
    return(ret_val);
}


int
MsgSet_set_is_referenced(MsgSet msgSet, BOOL set_is_referenced)
{
    msgSet->is_referenced = set_is_referenced;
    return 0;
}


int
MsgSet_set_msg_is_referenced(
    MsgSet	that,
    int		msg_num,
    BOOL	msg_is_referenced
)
{
    int	i;

    if (that == (MsgSet)NULL)
        goto cret;
    
    for (i = 0; i < that->num_msgs; i++)
	if (that->msgs[i]->msg_number == msg_num)
	    break;

    if (i == that->num_msgs)
	goto cret;

    if (msg_is_referenced && (!that->is_referenced))
    {
	that->is_referenced = (BOOL)TRUE;
    }

    msg_set_is_referenced(that->msgs[i], msg_is_referenced);

cret:
    return 0;
}


/* MsgFile Methods */

MsgFile
MsgFile_create(
    char	*project_name,
    char	*file_name	/* message source file name */
    )
{
    MsgFileRec	*ret_val = (MsgFileRec *)NULL;
    MsgFileRec	*new_msg_file;
    char	proj_name_suffixed[BUFSIZ];

    if (project_name == NULL)
        goto cret;
    
    if ((new_msg_file = (MsgFileRec *)malloc(sizeof(MsgFileRec))) == NULL)
        goto cret;

    new_msg_file->fp = (FILE *)NULL;

    new_msg_file->file_comment = (char *)NULL;
    
    new_msg_file->msg_sets = (MsgSet *)NULL;
    new_msg_file->num_msg_sets = 0;
    new_msg_file->alloced_slots = INIT_ALLOCED_SLOTS;

    if ((new_msg_file->msg_sets = (MsgSet *)malloc(sizeof(MsgSet)*
                                                   new_msg_file->alloced_slots))
        == (MsgSet *)NULL)
    {
        MsgFile_destroy(new_msg_file);
        ret_val = (MsgFile)NULL;
        goto cret;
    }

    
    ret_val = new_msg_file;

    if (file_name == (char *)NULL)
        goto cret;

    if (msgFile_load(ret_val, file_name) == FALSE)
    {
        MsgFile_destroy(ret_val);
        ret_val = (MsgFile)NULL;
    }

    snprintf(proj_name_suffixed, sizeof(proj_name_suffixed), "%s_project", project_name);
    
    if (MsgFile_sure_find_msg_set(new_msg_file, strdup(proj_name_suffixed))
        == (MsgSet)NULL)
    {
        MsgFile_destroy(new_msg_file);
        ret_val = (MsgFile)NULL;
        goto cret;
    }
    
cret:    
    return(ret_val);
}


int
MsgFile_destroy(
    MsgFile that
    )
{
    if (that == (MsgFile)NULL)
        goto cret;
    
    if (that->msg_sets != (MsgSet *)NULL)
    {
        int	i;

        for (i = 0; i < that->num_msg_sets; i++)
            MsgSet_destroy(that->msg_sets[i]);

        free(that->msg_sets);
    }
    
    if (that->fp != (FILE *)NULL)
       fclose(that->fp);

    free(that);

cret:
    return 0;
}


int
MsgFile_save(
    MsgFile	that,
    FILE	**msgFileOutPtr
    )
{
    int			ret_val = 0;
    FILE		*fp = NULL;
    int	i;

    *msgFileOutPtr = NULL;

    if (that == (MsgFile)NULL)
    {
	ret_val = ERR_CLIENT;
        goto cret;
    }
    
    if ((fp = tmpfile()) == NULL)
    {
	ret_val = ERR_OPEN;
        goto cret;
    }

    if (that->file_comment == (char *)NULL)
    {
        fputs(
         "$ ==================================================================\n",
         fp);
        fputs(
         "$ = Project Message Source File: generated by 'dtcodegen'\n",
         fp);
        fputs(
	 "$ = Generated message sets have \'", fp);
	    fputs(DTB_GEN_MSG_SET_PREFIX, fp);
	    fputs("<name>\' as their comment\n",
	     fp);
        fputs(
         "$ = PLEASE READ SET COMMENTS ON HOW TO EDIT GENERATED MESSAGE SETS \n",
         fp);
        fputs(
         "$ ==================================================================\n\n",
         fp);

        /* Output the $quote directive */
        fputs("$quote \"\n\n",fp);
    }
    else
    {
        fputs(that->file_comment, fp);
    }

    for (i = 0; i < that->num_msg_sets; i++)
    {
        msgSet_output(that->msg_sets[i], fp);
    }
    
    ret_val = (BOOL)TRUE;
    
cret:    
    *msgFileOutPtr = fp;
    return(ret_val);
}


MsgSet
MsgFile_obj_sure_find_msg_set(MsgFile msgFile, ABObj obj)
{
    ABObj	msgSetObj = NULL;
    STRING	msgSetName = NULL;
    char	msgSetNameBuf[1024];
    MsgSet	msgSet = NULL;
    *msgSetNameBuf = 0;
    
    /*
     * Find the object that defines the message set
     */
    msgSetObj = obj_get_module(obj);
    if (msgSetObj == NULL)
    {
	msgSetObj = obj_get_project(obj);
    }
    if (msgSetObj == NULL)
    {
	return NULL;
    }

    /*
     * Determine the set's name
     */
    if (obj_is_project(msgSetObj))
    {
	sprintf(msgSetNameBuf, "%s_project", obj_get_name(msgSetObj));
	msgSetName = msgSetNameBuf;
    }
    else
    {
	msgSetName = obj_get_name(msgSetObj);
    }

    /*
     * Find the set
     */
    msgSet = MsgFile_sure_find_msg_set(msgFile, msgSetName);
    return msgSet;
}



MsgSet
MsgFile_just_find_msg_set(
    MsgFile	that,
    STRING	set_name
    )
{
    MsgSet		ret_val = (MsgSet)NULL;
    int	i;

    if (that == (MsgFile)NULL)
        goto cret;
    
    for (i = 0; i < that->num_msg_sets; i++)
    {
        char	*msg_set_name;
        
        if (!msgSet_is_generated(that->msg_sets[i]))
            continue;

        msg_set_name = msgSet_get_name(that->msg_sets[i]);

        if (msg_set_name == NULL)
            continue;
        
        if (!strcmp(set_name, msgSet_get_name(that->msg_sets[i])))
            break;
    }
    if (i == that->num_msg_sets)
        goto cret;

    ret_val = that->msg_sets[i];
            
cret:    
    return(ret_val);
}


MsgSet
MsgFile_sure_find_msg_set(	/* adds one, if not already there */
    MsgFile	that,
    char	*module_name
    )
{
    MsgSet	ret_val = (MsgSet)NULL;

    if (that == (MsgFile)NULL)
        goto cret;

    if ((ret_val = MsgFile_just_find_msg_set(that, module_name)) != (MsgSet)NULL)
        goto cret;

    ret_val = msgFile_add_msg_set(that, module_name,
                                  msgFile_max_set_number(that)+1, TRUE);

cret:    
    return(ret_val);
}



/* Functions private to this implementation */

static BOOL
msgFile_load(
    MsgFile	that,
    char	*filename
    )
{
    BOOL	ret_val = (BOOL)FALSE;

    if (that->fp != (FILE *)NULL)
        goto cret;

    /* Open the message file */
    if ((that->fp = util_fopen_locked(filename, "r")) == (FILE *)NULL)
    {
        /* If there is no existing msg source file,
           load should succeed:
           because it loaded whatever was there which
           is nothing
         */
        if (errno == ENOENT)
            ret_val = (BOOL)TRUE;
        
        goto cret;
    }

    if (util_get_verbosity() >= 1)
	util_printf("reading existing %s\n", filename);

    /* Read comment till first set */
    if (msgFile_read_top_comment(that) == FALSE)
    {
        fclose(that->fp);
        that->fp = NULL;
        goto cret;
    }

    /* Read in each message set */
    while (msgFile_read_msg_set(that) == TRUE)
        ;
    fclose(that->fp);

    ret_val = (BOOL)TRUE;

cret:
    return(ret_val);
}


static BOOL
msgFile_read_top_comment(
    MsgFile	that
)
{
    BOOL	ret_val = (BOOL)FALSE;

    if (that->fp == (FILE *)NULL)
        goto cret;

    that->file_comment = get_comment(that->fp);
    
    ret_val = (BOOL)TRUE;
cret:
    return (ret_val);
}

static BOOL
msgFile_read_msg_set(
    MsgFile	that
)
{
    BOOL	ret_val = (BOOL)FALSE;
    int		set_number;
    char	*set_line_cmt;
    char	*bil_name;
    MsgSet	msg_set;

    int		msg_num;
    char	*msg_str = (char *)NULL;
    char	*msg_cmt = (char *)NULL;
    

    if (that->fp == (FILE *)NULL)
        goto cret;
    if (get_set_number_and_name(that->fp, &set_number, &set_line_cmt) == FALSE)
        goto cret;

    if ((msg_set = msgFile_get_msg_set_by_num(that, set_number)) == NULL)
    {
        BOOL	is_generated = (BOOL)TRUE;
        
        if ((bil_name = extract_bil_name(set_line_cmt)) == (char *)NULL)
            is_generated = (BOOL)FALSE;
        else
            free(set_line_cmt);

        if ((msg_set = msgFile_add_msg_set(that, 
                                (is_generated)?bil_name:set_line_cmt,
                                set_number,
                                is_generated))
             == (MsgSet)NULL)
            goto cret;
    }
    msg_set->set_comment = get_comment(that->fp);
    
    while (get_msg_and_comment(that->fp, &msg_num, &msg_str, &msg_cmt))
    {
        if (msgSet_add_msg(msg_set, msg_str, msg_cmt, msg_num) == (Msg)NULL)
            goto cret;
    }
    
    ret_val = (BOOL)TRUE;
    
cret:
    return(ret_val);
}

static MsgSet
msgFile_get_msg_set_by_num(
    MsgFile	that,
    int		set_num
    )
{
    MsgSet		ret_val = (MsgSet)NULL;
    int	i;

    if (that == NULL)
        goto cret;

    for (i = 0; i < that->num_msg_sets; i++)
    {
        int	num = MsgSet_get_number(that->msg_sets[i]);

        if (num == set_num) 
        {
            ret_val = that->msg_sets[i];
            break;
        }
    }
    
cret:
    return(ret_val);
}


static MsgSet
msgFile_add_msg_set(
    MsgFile	that,
    char	*module_name,
    int		set_num,
    BOOL	is_generated
    )
{
    MsgSet	ret_val = (MsgSet)NULL;
    MsgSet	new_set;

    if (that == (MsgFile)NULL)
        goto cret;

    if (that->num_msg_sets == that->alloced_slots)
    {
        MsgSet	*new_sets;
        
        that->alloced_slots += INIT_ALLOCED_SLOTS;
        if ((new_sets = (MsgSet*)realloc((void *)that->msg_sets,
                                sizeof(MsgSet)*that->alloced_slots))
            == NULL)
        {
            that->alloced_slots -= INIT_ALLOCED_SLOTS;
            goto cret;
        }
        
        that->msg_sets = new_sets;
    }
    
    new_set = msgSet_create_new(set_num, module_name, NULL, is_generated);

    if (new_set == (MsgSet)NULL)
        goto cret;
    
    that->msg_sets[that->num_msg_sets++] = ret_val = new_set;
    
cret:
    return(ret_val);
}


static int
msgFile_max_set_number(
    MsgFile	that
    )
{
    int			ret_val = 0;
    int	i;

    if (that == NULL)
        goto cret;

    for (i = 0; i < that->num_msg_sets; i++)
    {
        int	num = MsgSet_get_number(that->msg_sets[i]);

        if (num > ret_val)
            ret_val = num;
    }
    
cret:
    return(ret_val);
}


static MsgSetRec *
msgSet_create_new(
    int		set_number,
    char	*name,
    char	*comment,
    BOOL	is_generated
    )
{
    MsgSetRec	*ret_val = (MsgSetRec *)NULL;
    MsgSetRec	*new_msg_set;

    if ((new_msg_set = (MsgSetRec *)malloc(sizeof(MsgSetRec))) == NULL)
        goto cret;

    new_msg_set->set_number = set_number;
    new_msg_set->set_name = name;
    new_msg_set->allow_msg_delete = FALSE;
    new_msg_set->is_generated = is_generated;
    new_msg_set->is_referenced = FALSE;
    new_msg_set->set_comment = NULL;
    if (comment != NULL)
    {
        new_msg_set->set_comment = strdup(comment);
    }
    new_msg_set->msgs = (Msg *)NULL;
    new_msg_set->num_msgs = 0;
    
    if (!is_generated)
    {
        new_msg_set->alloced_slots = 0;
    }
    else
    {
        new_msg_set->alloced_slots = INIT_ALLOCED_SLOTS;
        if ((new_msg_set->msgs = (Msg *)malloc(sizeof(Msg)*
                                               new_msg_set->alloced_slots))
            == (Msg *)NULL)
        {
            MsgSet_destroy(new_msg_set);
            ret_val = (MsgSet)NULL;
            goto cret;
        }
    }

    ret_val = new_msg_set;

cret:    
    return(ret_val);
}


static char *
msgSet_get_name(
    MsgSet that
    )
{
    char	*ret_val = (char *)NULL;

    if (that == (MsgSet)NULL)
        goto cret;

    ret_val = that->set_name;

cret:
    return(ret_val);
}


static BOOL
msgSet_is_generated(
    MsgSet that
    )
{
    BOOL	ret_val = FALSE;

    if (that == (MsgSet)NULL)
        goto cret;

    ret_val = that->is_generated;

cret:
    return(ret_val);
    
}

static void
msgSet_output(
    MsgSet	that,
    FILE	*fp
    )
{
    char	*set_name;
    int	i;
        
    if (that == (MsgSet)NULL)
        goto cret;

    set_name = msgSet_get_name(that);
    if (set_name == (char *)NULL)
        set_name = "";

    if (strcmp(set_name, "appfw") == 0)
        that->allow_msg_delete = FALSE;
    
    fprintf(fp, "$set %d\t", MsgSet_get_number(that));

    if (msgSet_is_generated(that))
        fprintf(fp, "%s%s\n", DTB_GEN_MSG_SET_PREFIX, set_name);
    else
        fputs(set_name, fp);

    if (that->set_comment != (char *)NULL)
        fputs(that->set_comment, fp);
    else if (msgSet_is_generated(that))
    {
        fprintf(fp, "$\t** Generated messages for '%s'.\n", set_name);
        fputs("$\t** DO NOT EDIT MESSAGES OR THE $set LINE ABOVE.\n", fp);
        fputs("$\t** DO NOT USE THESE MESSAGES IN YOUR APPLICATION CODE.\n",
              fp);
        fputs("$\t** You can edit this comment and add comments below each message.\n", fp);
    }

    if ((!that->is_referenced) || (!that->allow_msg_delete))
    {
	for (i = 0; i < that->num_msgs; i++)
	    msg_output(that->msgs[i], fp);
    }
    else
    {
	for (i = 0; i < that->num_msgs; i++)
	{
	    if (msg_is_referenced(that->msgs[i]))
		msg_output(that->msgs[i], fp);
	}
    }

cret:
    return;
}


static Msg
msgSet_add_msg(
    MsgSet	that,
    char	*string,
    char	*comment,
    int		msg_num
    )
{
    Msg	ret_val = (Msg)NULL;
    Msg	new_msg;

    if (that == (MsgSet)NULL)
        goto cret;

    if (that->num_msgs == that->alloced_slots)
    {
        Msg	*new_msgs;
        
        that->alloced_slots += INIT_ALLOCED_SLOTS;
        if ((new_msgs = (Msg*)realloc((void *)that->msgs,
                                sizeof(Msg)*that->alloced_slots))
            == NULL)
        {
            that->alloced_slots -= INIT_ALLOCED_SLOTS;
            goto cret;
        }
        
        that->msgs = new_msgs;
    }
    
    new_msg = msg_create(string, comment, msg_num);

    if (new_msg == (Msg)NULL)
        goto cret;
    
    that->msgs[that->num_msgs++] = ret_val = new_msg;
    
cret:
    return(ret_val);
}


static int
msgSet_max_msg_number(
    MsgSet	that
    )
{
    int			ret_val = 0;
    int	i;

    if (that == NULL)
        goto cret;

    for (i = 0; i < that->num_msgs; i++)
    {
        int	num = msg_get_number(that->msgs[i]);

        if (num > ret_val)
            ret_val = num;
    }
    
cret:
    return(ret_val);
}


static Msg
msg_create(
    char	*string,
    char	*comment,
    int		number
    )
{
    Msg		ret_val = (Msg)NULL;
    MsgRec	*new_msg;

    if ((new_msg = (MsgRec *)malloc(sizeof(MsgRec))) == NULL)
        goto cret;

    new_msg->msg_string = istr_create(string);
    new_msg->msg_comment = istr_create(comment);
    new_msg->msg_number = number;
    new_msg->is_referenced = (BOOL)FALSE;
    
    ret_val = new_msg;
    
cret:
    return(ret_val);
}


static void
msg_destroy(
    Msg	that
    )
{
    if (that == (Msg)NULL)
        goto cret;

    free((void *)that);

cret:
    return;
}


static char *
msg_get_string(
    Msg		that
    )
{
    char	*ret_val = (char *)NULL;

    if (that == NULL)
        goto cret;

    ret_val = istr_string(that->msg_string);
cret:
    return(ret_val);
}


static int
msg_get_number(
    Msg		that
    )
{
    int	ret_val = -1;
    
    if (that == NULL || istr_string(that->msg_string) == NULL)
        goto cret;

    ret_val = that->msg_number;
cret:
    return(ret_val);
}


static void
msg_set_is_referenced(
    Msg		that,
    BOOL	msg_is_referenced
)
{
    if (that == (Msg)NULL)
	goto cret;

    if (util_xor(that->is_referenced, msg_is_referenced))
    {
	that->is_referenced = msg_is_referenced;
    }

cret:
    return;
}

static BOOL
msg_is_referenced(
    Msg		that
)
{
    BOOL	ret_val = (BOOL)FALSE;

    if (that == (Msg)NULL)
	goto cret;

    ret_val = that->is_referenced;

cret:
    return(ret_val);
}


static void
msg_output(
    Msg		that,
    FILE	*fp
    )
{
    char *comment;

    if (that == NULL || istr_string(that->msg_string) == NULL)
        goto cret;

    fprintf(fp, "%d \"", that->msg_number);
    out_str_puts(istr_string(that->msg_string), fp);
    fprintf(fp,"\"\n");

    if ( (comment=istr_string(that->msg_comment)) != (char *)NULL)
        fputs(comment, fp);

cret:
    return;
}



/* 
** Convert a string from the external (gencat) format to the literal
** internal (byte-correct) format.
**
** Note that this function allocates space and expects the caller to free it.
*/
static char *
in_str_dup(
    char	*string
)
{
    int		inc = 256;			/* incremental allocation */
    char	*dup_str = (char *) NULL;	/* storage buffer */
    char	*c;
    int		buf_sz, len;

    buf_sz = inc;
    dup_str = (char *)malloc(buf_sz);
    for(c = string,len=0;*c!=0;c++) {
        if(len+2 >= buf_sz) {
    	    buf_sz += inc;
    	    dup_str = (char *)realloc(dup_str, buf_sz);
        }
	/* Transform "\n" into '\n' and "\"" into '"' */
	if(*c == '\\') {
	    switch(*(c+1)) {
		case 'n': dup_str[len++] = '\n'; c++; break;
		case '"': dup_str[len++] = '"' ; c++; break;
		default : dup_str[len++] = '\\';
			  dup_str[len++] = *(c+1); 
			  c++;
			  break;
	    }
	}
	else dup_str[len++] = *c;
    }
    dup_str[len] = '\0';

    return(dup_str);
}

/* 
** Transform a literal internal (byte-correct) string into a form acceptable
** to gencat 
*/
static void
out_str_puts(
    char	*string,
    FILE	*fp
)
{
    char	*c;
    for(c=string;*c!=0;c++) {
	if(*c == '"') {
	    fputc('\\',fp);
	    fputc(*c,fp);
	}
	else if(*c == '\n') {
	    fputs("\\n",fp);
	}
	else {
	    fputc(*c,fp);
	}
    }
}


/* 
** NOTE: OBSOLETE FUNCTION -- do not use (for reference only)!
**
** Go through a string transforming the to character sequence '\' + 'n' into 
** the single character '\n'.  All other character combinations will be
** unaltered.
*/
static char *
nl_dup(
    char	*string
)
{
    char	*ret_val = (char *)NULL;
    char	*trav;
    char	*start;
    char	*end;
    char	*dup_str = (char *)NULL;
    int		cur_len = 0;
    BOOL	done = (BOOL)FALSE;

    if (string == (char *)NULL)
	goto cret;

    start = trav = string;

    while (!done)
    {
	int	len;
	int	offs = 1;

	if ((end = strchr(trav, '\\')) == NULL)
	    done = TRUE;
	else
	{
	    if (*(end+1) != 'n')
	    {
		trav = end + 1;
		continue;
	    }
	    else
	    {
		*end = '\0';
		offs = 2;
	    }
	}

	len = strlen(trav);
	cur_len += len+offs;

	if ((dup_str = (char *)realloc(dup_str, cur_len)) == NULL)
	    goto cret;

	if (cur_len == len+offs)
	    strcpy(dup_str, start);
	else
	    strcat(dup_str, trav);
	if (offs == 2)
	    strcat(dup_str, "\n");
	start = trav = end + offs;
	offs = 1;
    }

    ret_val = dup_str;

cret:
    return(ret_val);
}

/*
** NOTE: OBSOLETE FUNCTION -- do not use (for reference only)!
*/
static void
nl_puts(
    char	*string,
    FILE	*fp
)
{
    char	*trav = string;
    char	*end;

    while ((end = strchr(trav, '\n')) != NULL)
    {
	*end = '\0';
	fputs(trav, fp);
	fputs("\\n", fp);
	*end = '\n';
	trav = end+1;
    }
    fputs(trav, fp);
}

static BOOL
get_set_number_and_name(
    FILE	*fp,
    int		*set_num,
    char	**set_name
)
{
    BOOL	ret_val = (BOOL)FALSE;
    char	*buf;
    char	*trav;
    char	*sn;
    char	comment[BUFSIZ];
    int		num;

    if ((buf = jump_backslash_gets(fp)) == NULL)
        goto cret;

    if (strncmp(buf, "$set", 4))
        goto fret;

    trav = buf + 4;

    if (*trav != ' ' && *trav != '\t')
        goto fret;

    sn = ++trav;

    if (!isdigit(*trav))
        goto fret;
    
    do 
    {
        trav++;
    } while (isdigit(*trav) &&
             *trav != ' ' && *trav != '\t' &&
             *trav != '\0');

    if (*trav != ' ' && *trav != '\t')
        goto fret;
    *trav = '\0';

    if ((num = atoi(sn)) < 1)
        goto fret;

    *set_num = num;
    
    *set_name = strdup(++trav);

    ret_val = (BOOL)TRUE;

fret:
    free(buf);

cret:
    return(ret_val);
}

static char *
get_comment(
    FILE	*fp
)
{
    char	*ret_val = (char *)NULL;
    int		cur_len = 0;
    char	*cmt_str = (char *)NULL;
    char	*buf;
    
    while ((buf = jump_backslash_gets(fp)) != NULL)
    {
        int	new_len = strlen(buf);
        
        if (strncmp(buf, "$ ", 2) != 0 &&
            strncmp(buf, "$\t", 2) != 0 &&
            strncmp(buf, "$delset", 7) != 0 &&
            strncmp(buf, "$quote", 6) != 0 &&
            strncmp(buf, "\n", 1) != 0)
        {
            jump_backslash_ungets(buf);
            break;
        }

        cur_len += new_len+1;
        if ((cmt_str = (char *)realloc(cmt_str, cur_len)) == NULL)
            goto cret;

        if (cur_len == new_len+1)
            strcpy(cmt_str, buf);
        else
            strcat(cmt_str, buf);

        free(buf);
    }
    
    ret_val = cmt_str;
cret:
    return(ret_val);
}

static BOOL
get_msg_and_comment(
    FILE	*fp,
    int		*msg_num,
    char	**msg_str,
    char	**msg_cmt
)
{
    BOOL	ret_val = (BOOL)FALSE;
    
    if (!get_message(fp, msg_num, msg_str))
        goto cret;
    *msg_cmt = get_comment(fp);
    
    ret_val = (BOOL)TRUE;
cret:
    return(ret_val);
}

/*
** This function parses a line from the message source text file to get the
** message number and the message string.  The expected syntax for a message
** line is:
**   [0-9]*[ \t]*"STRING"$
**
** Note that because we're using double quotes to delimit messages we have to
** strip them off the string, plus we have to recognize that there may be
** more than one white-space character between message number and string.
*/

static BOOL
get_message(
    FILE	*fp,
    int		*msg_num,
    char	**msg_str
)
{
    BOOL	ret_val = (BOOL)FALSE;
    char	*buf, *c;
    char	*trav;
    int		num;
    int		len;

    if ((buf = jump_backslash_gets(fp)) == NULL)
        goto cret;

    trav = buf;

    if (!isdigit(*trav))
    {
        jump_backslash_ungets(buf);
        goto cret;
    }
    
    /* 
    ** Step through the line as long as we find digits and don't see 
    ** whitespace or null.
    */
    do 
    {
        trav++;
    } while (isdigit(*trav) &&
             *trav != ' ' && *trav != '\t' &&
             *trav != '\0');

    if (*trav != ' ' && *trav != '\t')
        goto fret;

    *trav = '\0';

    num = atoi(buf);
    if (num < 1)
        goto fret;

    *msg_num = num;

    /* Now step through whitespace until we run out (or find null) */
    do
    {
	trav++;
    } while ( (*trav == ' ' || *trav == '\t') && (*trav != '\0') );


    if (*trav == '\0') goto fret;

    /* What's left is the message string. */
    len = strlen(trav) - 1;
    trav[len] = '\0';         /* Nuke the last newline */

    /* If the string appears to be embedded in double quotes, strip them off */
    if(trav[0] == '\"' && ((c=strrchr(trav,'\"')) != trav) ) {
	trav++;
	*c = '\0';
    }
    
    /* dup after converting \ n sequence into newline */
    *msg_str = in_str_dup(trav);	

    ret_val = TRUE;
fret:
    free(buf);
cret:
    return(ret_val);
}


static char *
extract_bil_name(
    char	*set_line_comment
)
{
    char	*ret_val = (char *)NULL;
    char	*trav;
    int		len;
    char	*bil_name;

    if (set_line_comment == NULL)
        goto cret;

    if (strncmp(set_line_comment,
		    DTB_GEN_MSG_SET_PREFIX,
		    sizeof(DTB_GEN_MSG_SET_PREFIX)-1))
        goto cret;

    trav = set_line_comment + sizeof(DTB_GEN_MSG_SET_PREFIX)-1;
    len = strlen(trav);
    trav[len-1] = '\0';           /* Nuke the last new-line */
    
    if (!ab_ident_is_ok(trav))
        goto cret;

    bil_name = strdup(trav);

    trav[len-1] = '\n';           /* Restore the last new-line */

    ret_val = bil_name;
    
cret:
    return(ret_val);
}


static char	*ungets_buf = (char *)NULL;

static char *
jump_backslash_gets(
    FILE	*fp
)
{
    static char	tmp[BUFSIZ];
    char	*ret_val = (char *)NULL;
    char	*new_str = (char *)NULL;
    int		cur_len = 0;
    BOOL	done = (BOOL)FALSE;

    if (ungets_buf != (char *)NULL)
    {
        ret_val = ungets_buf;
        ungets_buf = (char *)NULL;
        goto cret;
    }
    
    while (done == (BOOL)FALSE)
    {
        int	len;
        
        if (fgets(tmp, BUFSIZ, fp) == NULL)
            goto cret;

        len = strlen(tmp);
    
        if (len < 2 || tmp[len-2] != '\\')
            done = (BOOL)TRUE;
        cur_len += len+1;
        if ((new_str = (char *) realloc(new_str, cur_len)) == NULL)
            goto cret;

        if (cur_len == len+1)
            strcpy(new_str, tmp);
        else
            strcat(new_str, tmp);
    }
    ret_val = new_str;
    
cret:
    return (ret_val);
}

static void
jump_backslash_ungets(
    char	*buf
)
{
    ungets_buf = buf;
}

