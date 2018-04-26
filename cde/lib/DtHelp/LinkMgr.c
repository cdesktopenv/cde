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
/* $XConsortium: LinkMgr.c /main/10 1996/11/01 10:10:59 drk $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        LinkMgr.c
 **
 **   Project:     Cde Help System
 **
 **   Description: Hypertext manager for the core engine of the help
 **                system.  Processes requests from the UI to move, turn
 **                on, or turn off the traversal indicator, to return
 **                information about a hypertext link and to determine
 **                if a spot selected contains a hypertext link.
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/

/*
 * system includes
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xos.h>
#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

/*
 * Canvas Engine Includes
 */
#include "CanvasP.h"

/*
 * private includes
 */
#include "CanvasOsI.h"
#include "CvStringI.h"
#include "LinkMgrI.h"
#include "LinkMgrP.h"


#ifdef NLS16
#endif

/********    Private Function Declarations    ********/
/********    End Private Function Declarations    ********/

/*****************************************************************************
 *		Private Defines
 *****************************************************************************/
#define	GROW_SIZE	3

/*****************************************************************************
 *		Private Variables
 *****************************************************************************/
const static struct _dtCvLinkDb DefLinkDb = { 0, NULL };

/*****************************************************************************
 *		Private Functions
 *****************************************************************************/
/*****************************************************************************
 * Function: FindLink (
 *
 * Purpose:
 *****************************************************************************/
static	int
FindLink (
    _DtCvLinkDb  link_db,
    char	*id,
    int		*ret_no,
    int		*ret_hint)
{
    int result = -1;
    int i;

    for (i = 0; -1 == result && i < link_db->max; i++)
      {
	if (_DtLinkTypeNone != link_db->list[i].type
			&& NULL != link_db->list[i].id
			&& 0 == _DtCvStrCaseCmpLatin1(link_db->list[i].id, id))
	  {
	    /*
	     * return a valid index
	     */
	    *ret_no = i;

	     /*
	      * check to see if the hint changes
	      */
	     if (_DtLinkTypeLink == link_db->list[i].type &&
		_DtCvWindowHint_Original != link_db->list[i].info.link.hint)
		*ret_hint = link_db->list[i].info.link.hint;

	     /*
	      * clear the error return
	      */
	     result = 0;
          }
      }

    return result;
}

/*****************************************************************************
 * Function: ResolveSwitch (_DtCvLinkDb link_db, int *ret_link)
 *
 * Purpose:
 *****************************************************************************/
static	int
ResolveSwitch (
    _DtCvLinkDb	 link_db,
    int		 link_no,
    int		 iterations,
    int		(*filter)(),
    void	*client_data,
    int		*ret_link,
    int		*ret_hint)
{
    int          result = -1;
    char	*valueData = NULL;

    if (iterations < 100)
      {
	result = _DtCvRunInterp(filter, client_data,
				link_db->list[link_no].info.swtch.interp,
				link_db->list[link_no].info.swtch.cmd,
				&valueData);

	if (result == 0)
	  {
	    int   value = atoi(valueData);
	    char *idRefs = link_db->list[link_no].info.swtch.branches;
	    char *rid;

	    iterations++;

	    while (value > 0 && *idRefs != '\0')
	      {
		/*
		 * skip this id.
		 */
		while(*idRefs != ' ' && *idRefs != '\0')
		    idRefs++;

		/*
		 * skip the spaces
		 */
		while(*idRefs == ' ')
		    idRefs++;

		/*
		 * decrement the index.
		 */
		value--;
	      }

	    /*
	     * if the value returned is more than the id list or we hit
	     * the end, back up to the first idref
	     */
	    if (value > 0 || *idRefs == '\0')
		idRefs = link_db->list[link_no].info.swtch.branches;

	    /*
	     * duplicate the id and null out the
	     * extraneous ids.
	     */
	    rid    = strdup (idRefs);
	    idRefs = rid;
	    while(*idRefs != ' ' && *idRefs != '\0')
		idRefs++;
	    *idRefs = '\0';

	    result = FindLink(link_db, rid, &link_no, ret_hint);
	    if (0 == result && _DtLinkTypeSwitch == link_db->list[link_no].type)
	        result = ResolveSwitch (link_db,  link_no, iterations, filter,
					client_data, &link_no, ret_hint);
	    free (rid);
	  }

	if (valueData != NULL)
	    free(valueData);
      }

    *ret_link = link_no;
    return result;
}

/*****************************************************************************
 * Function: GetNextEntry (
 *
 * Purpose:
 *****************************************************************************/
static	int
GetNextEntry (
    _DtCvLinkDb link_db)
{
    int    i = 0;

    while (i < link_db->max && _DtLinkTypeNone != link_db->list[i].type)
	i++;

    if (i >= link_db->max)
      {
	link_db->max += GROW_SIZE;
	if (link_db->list != NULL)
	    link_db->list = (_DtCvLinkEntry *) realloc(link_db->list,
				(sizeof (_DtCvLinkEntry) * link_db->max));
	else
	    link_db->list = (_DtCvLinkEntry *) malloc(
				sizeof (_DtCvLinkEntry) * link_db->max);

	if (link_db->list == NULL)
	    return -1;

	while (i < link_db->max)
	  {
	    link_db->list[i  ].id   = NULL;
	    link_db->list[i++].type = _DtLinkTypeNone;
	  }

	i -= GROW_SIZE;
      }

    return i;
}

/*****************************************************************************
 *		Semi-Public Functions
 *****************************************************************************/
/******************************************************************************
 * Function:    int _DtLinkDbGetLinkType (link_index)
 *
 * Parameters:
 *
 * Returns:
 *
 * errno Values:
 *
 * Purpose:     Return the hypertext link type.
 *
 *****************************************************************************/
int
_DtLinkDbGetLinkType (
    _DtCvLinkDb link_db,
    int		link_index )
{
    int  value = -1;

    if (link_index < link_db->max &&
			_DtLinkTypeNone != link_db->list[link_index].type)
	value = link_db->list[link_index].info.link.lnk_type;

    return value;
}

/******************************************************************************
 * Function:    int _DtLinkDbGetHint (link_index)
 *
 * Parameters:
 *
 * Returns:
 *
 * errno Values:
 *
 * Purpose:     Return the hypertext link type.
 *
 *****************************************************************************/
int
_DtLinkDbGetHint (
    _DtCvLinkDb link_db,
    int		 link_index )
{
    int  value = -1;

    if (link_index < link_db->max &&
			_DtLinkTypeNone != link_db->list[link_index].type)
	value = link_db->list[link_index].info.link.hint;

    return value;
}

/******************************************************************************
 * Function:    char *_DtLinkDbGetLinkSpec (link_index)
 *
 * Parameters:
 *
 * Returns:
 *
 * errno Values:
 *
 * Purpose:     Return the hypertext link type.
 *
 *****************************************************************************/
char *
_DtLinkDbGetLinkSpec (
    _DtCvLinkDb link_db,
    int		 link_index )
{
    char  *ptr = NULL;

    if (link_index < link_db->max &&
			_DtLinkTypeNone != link_db->list[link_index].type)
	ptr = link_db->list[link_index].info.link.spec;

    return ptr;
}

/******************************************************************************
 * Function:    int _DtLinkDbGetLinkInfo (link_index, ret_info)
 *
 * Parameters:
 *
 * Returns:
 *
 * errno Values:
 *
 * Purpose:     Return the hypertext link type.
 *
 *****************************************************************************/
int
_DtLinkDbGetLinkInfo (
    _DtCvLinkDb		 link_db,
    int			 link_index,
    int			(*filter)(),
    void		*client_data,
    _DtCvLinkInfo	*ret_info)
{
    int    hint;
    int    result = -1;
    char  *spec;

    if (link_index < link_db->max &&
			_DtLinkTypeNone != link_db->list[link_index].type)
      {
	result = 0;

	spec = link_db->list[link_index].info.link.spec;
	hint = link_db->list[link_index].info.link.hint;

	/*
	 * is this a link to a switch? Check the switches for
	 * the spec (id) and resolve to another link if so.
	 */
	if (_DtLinkTypeSwitch == link_db->list[link_index].type ||
		(0 == FindLink(link_db, spec, &link_index, &hint)
			&& _DtLinkTypeSwitch == link_db->list[link_index].type))
	    result = ResolveSwitch(link_db, link_index, 0, filter, client_data,
							&link_index, &hint);

	if (0 == result)
	  {
	    ret_info->win_hint   = hint;
	    ret_info->hyper_type =
				link_db->list[link_index].info.link.lnk_type;
	    ret_info->specification =
				link_db->list[link_index].info.link.spec;
	    ret_info->description =
				link_db->list[link_index].info.link.descrip;
          }
      }

    return result;
}

/*****************************************************************************
 *		Public Functions
 *****************************************************************************/
/*****************************************************************************
 * Function:	_DtCvLinkDb _DtLinkDbCreate ()
 *
 * Parameters:
 *
 * Returns:	A link data base handle.
 *
 * Purpose:	Create a link data base.
 *
 *****************************************************************************/
_DtCvLinkDb
_DtLinkDbCreate (void)
{
    _DtCvLinkDb newDb = (_DtCvLinkDb) malloc (sizeof(struct _dtCvLinkDb));

    if (NULL != newDb)
	*newDb = DefLinkDb;

    return (newDb);
}

/*****************************************************************************
 * Function:	void _DtLinkDbDestroy (_DtCvLinkDb link_db)
 *
 * Parameters:
 *		canvas		Specifies the handle for the canvas.
 *
 * Returns:	A handle to the canvas or NULL if an error occurs.
 *
 * Purpose:
 *
 *****************************************************************************/
void
_DtLinkDbDestroy (
    _DtCvLinkDb	link_db)
{
    int i;

    if (NULL != link_db)
      {
        for (i = 0; i < link_db->max; i++)
          {
	    if (_DtLinkTypeLink == link_db->list[i].type)
	      {
	        if (NULL != link_db->list[i].id)
	            free(link_db->list[i].id);
		free(link_db->list[i].info.link.spec);
	        if (NULL != link_db->list[i].info.link.descrip)
		    free(link_db->list[i].info.link.descrip);
	      }
	    else if (_DtLinkTypeSwitch == link_db->list[i].type)
	      {
	        free(link_db->list[i].id);
		free(link_db->list[i].info.swtch.interp);
		free(link_db->list[i].info.swtch.cmd);
		free(link_db->list[i].info.swtch.branches);
	      }
          }
	if (NULL != link_db->list)
	    free(link_db->list);

	free(link_db);
      }
}

/******************************************************************************
 * Function:    int _DtLinkDbAddLink (link_data, char *link, int type,
 *							char *description)
 *
 * Parameters:
 *              link	Specifies the hypertext link specification.
 *              type	Specifies the type of hypertext link.
 *              link	Specifies the hypertext link description.
 *
 * Returns      The index into the list of hypertext links.
 *		-1 for errors.
 *
 * errno Values:
 *		DtErrorMalloc
 *
 * Purpose:     Place a hypertext link into an array.
 *
 * Note:	The link and description pointers are hereafter owned by
 *		the hypertext list. The caller should not free or realloc
 *		these pointers.
 *
 *****************************************************************************/
int
_DtLinkDbAddLink (
    _DtCvLinkDb link_db,
    char	 *id,
    char	*spec,
    int		 type,
    int		 hint,
    char	*description)
{
    int    i = GetNextEntry(link_db);

    if (spec == NULL || 0 > i)
	return -1;

    /*
     * copy the information
     */
    if (NULL != id)
      {
	id = strdup(id);
	if (NULL == id)
	    return -1;
      }

    spec = strdup(spec);
    if (NULL == spec)
      {
	if (NULL != id) free (id);
	return -1;
      }

    if (NULL != description)
      {
	description = strdup(description);
	if (NULL == description)
	  {
	    if (NULL != id) free (id);
	    free(spec);
	    return -1;
	  }
      }

    /*
     * assign the link
     */
    link_db->list[i].type = _DtLinkTypeLink;
    link_db->list[i].id   = id;

    link_db->list[i].info.link.spec     = spec;
    link_db->list[i].info.link.lnk_type = type;
    link_db->list[i].info.link.hint     = hint;
    link_db->list[i].info.link.descrip  = description;

    return i;
}

/******************************************************************************
 * Function:    void _DtLinkDbRemoveLink (link_index)
 *
 * Parameters:
 *
 * Returns:
 *
 * errno Values:
 *
 * Purpose:     Remove a hypertext link from the array.
 *
 *****************************************************************************/
void
_DtLinkDbRemoveLink (
    _DtCvLinkDb link_db,
    int		link_index )
{
    if (link_index < link_db->max &&
			_DtLinkTypeNone != link_db->list[link_index].type)
      {
	if (NULL != link_db->list[link_index].id)
	    free(link_db->list[link_index].id);

	if (_DtLinkTypeLink == link_db->list[link_index].type)
	  {
	    free(link_db->list[link_index].info.link.spec);
	    if (NULL != link_db->list[link_index].info.link.descrip)
		free(link_db->list[link_index].info.link.descrip);
	  }
	else if (_DtLinkTypeSwitch == link_db->list[link_index].type)
	  {
	    free(link_db->list[link_index].info.swtch.interp);
	    free(link_db->list[link_index].info.swtch.cmd);
	    free(link_db->list[link_index].info.swtch.branches);
	  }

	link_db->list[link_index].type = _DtLinkTypeNone;
	link_db->list[link_index].id   = NULL;
      }
}

/******************************************************************************
 * Function:    int _DtLinkDbAddSwitch (
 *
 * Parameters:
 *
 * Returns:
 *
 * errno Values:
 *
 * Purpose:     Add a switch to the link database
 *
 *****************************************************************************/
int
_DtLinkDbAddSwitch (
    _DtCvLinkDb link_db,
    char	*id,
    char	*interp,
    char	*cmd,
    char	*branches)
{
    int    i = GetNextEntry(link_db);

    if (NULL == id || NULL == interp || NULL == cmd
						|| NULL == branches || 0 > i)
	return -1;

    /*
     * copy the information
     */
    id     = strdup(id);
    interp = strdup(interp);
    cmd    = strdup(cmd);
    branches = strdup(branches);

    if (NULL == id || NULL == interp || NULL == cmd || NULL == branches)
      {
	if (NULL != id)       free(id);
	if (NULL != interp)   free(interp);
	if (NULL != cmd)      free(cmd);
	if (NULL != branches) free(branches);
	return -1;
      }

    link_db->list[i].type = _DtLinkTypeSwitch;
    link_db->list[i].id   = id;

    link_db->list[i].info.swtch.interp   = interp;
    link_db->list[i].info.swtch.cmd      = cmd;
    link_db->list[i].info.swtch.branches = branches;

    return i;
}
