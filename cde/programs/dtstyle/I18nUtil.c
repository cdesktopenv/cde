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
/* $XConsortium: I18nUtil.c /main/1 1996/03/25 00:52:03 pascale $ */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        I18nEnv.c
 **
 **   Description: Contains utility functions for the Dtstyle I18N component.
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

/*+++++++++++++++++++++++++++++++++++++++*/
/* include files                         */
/*+++++++++++++++++++++++++++++++++++++++*/


#include "I18nUtil.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* include extern functions              */
/*+++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++*/
/* Local #defines                        */
/*+++++++++++++++++++++++++++++++++++++++*/

#define  TAG_END_CHAR            ':'


/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Functions                    */
/*+++++++++++++++++++++++++++++++++++++++*/

static char *trim_line (char * );

/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Variables                    */
/*+++++++++++++++++++++++++++++++++++++++*/
 
	/* ********  file reading  ******** */

static int	tag_line_num = 0;
static char	*tag_linebuf = NULL;
static char	*tag_file = NULL;


void	start_tag_line(
    char  *fname
)
{
    if (fname) {
	if (!tag_linebuf)
	    tag_linebuf = (char *) XtCalloc(BUFSIZ, sizeof(char));
	tag_linebuf[0] = 0;
	tag_file = fname;
    } else {
	if (tag_linebuf) 
	    XtFree(tag_linebuf);
	tag_linebuf = tag_file = 0;
    }
    tag_line_num = 0;
}

int	read_tag_line(
    FILE	*fp,
    char	**tagp, 
    char        **valp
)
{
    char	*lp, *lp2;

    while (fgets(lp = tag_linebuf, BUFSIZ, fp)) {
	tag_line_num++;
	skip_white(lp);		/* lp = trim_line(lp); */
	if (!*lp || *lp == '\n' || is_comment_char(*lp))
	    continue;
	if (!(lp2 = strchr(lp, TAG_END_CHAR))) {
	    continue;
	}
	*lp2++ = 0;
	lp2 = trim_line(lp2);

	*tagp = lp;
	*valp = *lp2 ? lp2 : 0;
	return tag_line_num;
    }
    *tagp = *valp = 0;

    return (ferror(fp)) ? -1 : 0;
}

	/* ********  string manupilation  ******** */

static  char  *
trim_line(
     char *ptr
)
{
    char	*lastp;

    skip_white(ptr);
    for (lastp = ptr + strlen(ptr) - 1;
	lastp >= ptr && (is_white(*lastp) || *lastp == '\n'); lastp--) ;
    *(lastp + 1) = 0;
    return ptr;		/* return lastp > ptr ? ptr : NULL; */
}

int	
str_to_int(
    char *ptr,
    int  *val
)
{
    int	base;
    char *pp;

    /* if (!ptr || !*ptr || !val)	return(False); */
    *val = 0;
    base = ptr[0] == '0' ? (((ptr[1] & 0xdf) == 'X') ? 16 : 8) : 10;
    *val = strtol(ptr, &pp, base);
    if (!pp || *pp)	return(False);
    return(True);
}


Bool	
str_to_bool(
    char  *ptr,
    Bool  def_val
)
{
    if (!ptr || !*ptr)	return def_val;
    skip_white(ptr);

    switch (*ptr) {		/* true/false , 1/0 , yes/no , on/off */
	case '1':
	case 'T': case 't':
	case 'Y': case 'y':
		def_val = True; break;

	case '0':
	case 'F': case 'f':
	case 'N': case 'n':
		def_val = False; break;

	case 'O': case 'o':
		if (ptr[1] == 'N' || ptr[1] == 'n')
		    def_val = True;
		else if (ptr[1] == 'F' || ptr[1] == 'f')
		    def_val = False;
		break;
    }
    return def_val;
}

