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
 *	$XConsortium: load.c /main/3 1995/11/06 18:29:57 rswiston $
 *
 *	@(#)load.c	1.18 03 Apr 1995	cde_app_builder/src/libABil
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


/*
 *  load.c - load c file.
 */

#include <string.h>
#include <stdio.h>
#include "loadP.h"
#include "bilP.h"

#define MAX_NAME_LEN	256

static char	Current_Object[MAX_NAME_LEN]= "";
static char	Attribute[MAX_NAME_LEN]= "";
static char	Action_Attr[MAX_NAME_LEN]= "";
static char	File[MAX_NAME_LEN]= "";
static AbilErrorMessage Errormsg[BIL_ERRMSG_COUNT];
static AbilGetLineNumberCallback	getLineNumberCB = NULL;

int	abilP_err_line_number = -1;

static int abil_load_errmsg_table();

/*
 * Build a status message including the current object, the current
 * attribute, and the specified message.
 *
 * The userMessage argument passed is is an index into the internal
 * error message table.
 */
STRING
abil_loadmsg(STRING userMessage)
{
	static char     msg[256];
	char		tmpMsg[256];
	int		lineNumber = abil_loadmsg_get_line_number();
	*msg= 0;
	*tmpMsg= 0;

	if (*File != 0)
	{
	    sprintf(tmpMsg, 
		catgets(ABIL_MESSAGE_CATD, ABIL_MESSAGE_SET, 27, "file: %s"), 
		File);
	    strcat(msg, tmpMsg);
	}
	if (lineNumber > 0)
	{
	    sprintf(tmpMsg, 
		catgets(ABIL_MESSAGE_CATD, ABIL_MESSAGE_SET, 28, ", line: %d"),
		lineNumber);
	    strcat(msg, tmpMsg);
	}
	if (*Current_Object != 0)
	{
	    sprintf(tmpMsg, 
		catgets(ABIL_MESSAGE_CATD, ABIL_MESSAGE_SET,29, ", object: %s"),
		Current_Object);
	    strcat(msg, tmpMsg);
	}
	if (*Attribute != 0)
	{
	    sprintf(tmpMsg, 
		catgets(ABIL_MESSAGE_CATD, ABIL_MESSAGE_SET, 30,
		  ", attribute: %s"),
		Attribute);
	    strcat(msg, tmpMsg);
	}
	if (*Action_Attr != 0)
	{
	    sprintf(tmpMsg, 
		catgets(ABIL_MESSAGE_CATD, ABIL_MESSAGE_SET, 31, 
		   ", action-attribute: %s"),
		Action_Attr);
	    strcat(msg, tmpMsg);
	}

	strcat(msg, catgets(ABIL_MESSAGE_CATD, ABIL_MESSAGE_SET, 32, ", "));
	if ( (userMessage == 0) || (*userMessage == 0) )
	{
	    strcat(msg, catgets(ABIL_MESSAGE_CATD, ABIL_MESSAGE_SET, 33, 
		"syntax error"));
	}
	else
	{
	    strcat(msg, userMessage);
	}
	strcat(msg, catgets(ABIL_MESSAGE_CATD, ABIL_MESSAGE_SET, 34, "\n"));

	return msg;
}

static int
move_string(STRING to, STRING from)
{
	if (from == NULL)
	{
		*to= 0;
	}
	else
	{
		strcpy(to, from);
	}
	return 0;
}

int
abil_loadmsg_clear(void)
{
	move_string(Current_Object, NULL);
	move_string(Attribute, NULL);
	move_string(Action_Attr, NULL);
	move_string(File, NULL);
	return 0;
}

int
abil_loadmsg_set_object(STRING objname)
{
	return move_string(Current_Object, objname);
}

int
abil_loadmsg_set_att(STRING attname)
{
	return move_string(Attribute, attname);
}

int
abil_loadmsg_set_action_att(STRING actattname)
{
	return move_string(Action_Attr, actattname);
}

int
abil_loadmsg_set_file(STRING filename)
{
	return move_string(File, filename);
}

BOOL
abil_loadmsg_err_printed(void)
{
    int	cur_err_line = bilP_load_get_line_number();

    if ((abilP_err_line_number == cur_err_line) ||
	(cur_err_line == -1))
	return TRUE;
    else
	return FALSE;
}

int
abil_loadmsg_set_err_printed(BOOL printed)
{
    /* If the yacc error has been printed out, then
     * set the global error line number to the current
     * line number in the bil file, else reset it.
     */
    if (printed)
	abilP_err_line_number = bilP_load_get_line_number();
    else
	abilP_err_line_number = -1;

    return 0;
}

/*
** This routine prints one of the standard BIL load error messages by 
** looking it up in the error message table, fetching the right string
** from the message catalog, and passing it to abil_loadmsg() to be output.
**
** All the I18N has to happen here (before abil_loadmsg() is called)...
*/
int
abil_print_load_err(int errmsg)
{
    static BOOL	errmsg_tbl_init = FALSE;
    char	msg[1024] = "";
    int		msgLen = 0;

    /*
    ** Load up the error message table if this is the first time we've 
    ** needed to output an error message.
    */
    if(errmsg_tbl_init == FALSE) {
	abil_load_errmsg_table();
	errmsg_tbl_init = TRUE;
    }

    if ((errmsg == ERR_NOT_IMPL) && (!util_be_verbose()))
    {
	return 0;
    }

    if (!abil_loadmsg_err_printed())
    {
	if(Errormsg[errmsg].msg_id < 0) {
	    snprintf(msg, sizeof(msg), "%s", catgets(ABIL_MESSAGE_CATD, ABIL_MESSAGE_SET, 33, 
		"syntax error"));
	}
	else {
	    snprintf(msg, sizeof(msg), "%s", 
		catgets(ABIL_MESSAGE_CATD, ABIL_MESSAGE_SET, 
		Errormsg[errmsg].msg_id,
		Errormsg[errmsg].def_str));
	}

	if ((msgLen = strlen(msg)) > 0)
	{
	    if (msg[msgLen-1] == '\n')
	    {
	        msg[msgLen-1] = 0;
	    }
	}
	util_error(abil_loadmsg(msg));
	abil_loadmsg_set_err_printed(TRUE);
    }
    return 0;
}

/*
** This routine prints a custom error message rather than one of the standard
** ones from the BIL error message table.  It takes the message string as
** an argument and then passes it off to abil_loadmsg() to be output.
**
** This routine assumes the custom error message has been I18Nized beforehand.
*/
int
abil_print_custom_load_err(STRING errmsgstr)
{
    if (!abil_loadmsg_err_printed())
    {
	util_error(abil_loadmsg(errmsgstr));
	abil_loadmsg_set_err_printed(TRUE);
    }
    return 0;
}


/* Macro to make it easier to do error message table initialization */
#define blet(i,m,s)	\
    Errormsg[i].msg_id  = m; \
     Errormsg[i].def_str = s

/* Load the BIL error message table with message set and default text */
static int
abil_load_errmsg_table(void)
{
    int i;

    /* "Clear" out the error message table by setting message id to -1 */
    for(i=0;i<BIL_ERRMSG_COUNT;i++) Errormsg[i].msg_id = -1; 

    blet(ERR_BAD_ANCHOR         , 1, "cannot find anchor object");
    blet(ERR_BAD_ATT_FOR_OBJ    , 2, "no such attribute for this object");
    blet(ERR_BAD_IDENTIFIER     , 3, "not a valid C identifier");
    blet(ERR_EOF                , 4, "unexpected end-of-file");
    blet(ERR_FILE_FORMAT        , 5, "unknown file format");
    blet(ERR_NOT_IMPL           , 6, "currently not implemented");
    blet(ERR_NOT_IMPL_IGNOR     , 7, "currently not implemented - ignoring object and its attributes");
    blet(ERR_NO_MEMORY          , 8, "out of memory");
    blet(ERR_UNKNOWN            , 9, "unknown value");
    blet(ERR_UNKNOWN_ATTR       ,10, "unknown attribute");
    blet(ERR_UNKNOWN_MENU_TYPE  ,11, "unknown menu type - defaulting to Command Menu");
    blet(ERR_UNKNOWN_OBJ        ,12, "cannot find object");
    blet(ERR_UNKNOWN_OBJECT     ,13, "unknown object");
    blet(ERR_UNKNOWN_WHEN       ,14, "bad value - ignoring this 'Connection'");
    blet(ERR_WANT_ARG           ,15, "cannot find function argument");
    blet(ERR_WANT_BOOLEAN       ,16, "cannot find boolean");
    blet(ERR_WANT_FULL_NAME     ,17, "incorrect name syntax");
    blet(ERR_WANT_INTEGER       ,18, "cannot find integer");
    blet(ERR_WANT_KEYWORD       ,19, "cannot find keyword");
    blet(ERR_WANT_LIST          ,20, "cannot find start of list");
    blet(ERR_WANT_MENU_NAME     ,21, "cannot find menu for button");
    blet(ERR_WANT_NAME          ,22, "cannot find start of name");
    blet(ERR_WANT_OBJECT        ,23, "cannot find start of object");
    blet(ERR_WANT_STRING        ,24, "cannot find string");
    blet(WARN_BAD_HANDLER       ,25, "WARNING not a C identifier, ignored");
    blet(WARN_NO_MEMBERS        ,26, "WARNING: empty group, skipping...");
    blet(ERR_NOT_PROJECT        ,46, "not a project file");
    blet(ERR_NOT_MODULE         ,47, "not a module file");
    return 0;	
}


int
abil_loadmsg_get_line_number(void)
{
    return ((getLineNumberCB == NULL)? -1:getLineNumberCB());
}

int	
abil_loadmsg_set_line_number_callback(AbilGetLineNumberCallback cb)
{
    getLineNumberCB = cb;
    return 0;
}

