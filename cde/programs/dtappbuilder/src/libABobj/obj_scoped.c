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
 *	$XConsortium: obj_scoped.c /main/3 1995/11/06 18:38:33 rswiston $
 *
 *	@(#)obj_scoped.c	1.10 13 Feb 1994	
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
 *  scoped.c - handled scoped naming and objects
 *
 *  A scoped name has the form <name>, <module-name>.<name>, or
 *  <module-name>::<name>.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <ab_private/obj.h>

/*
 * The objName parameter can be a simple name, or can be of the form
 * module.name or module::name.  The separator can have an arbitrary
 * amount of white space on either side of it.  For example, these are
 * all legal names:
 *
 *	"my_object"
 *	"my_module::your_object"
 *	"your_module.your_object
 *	"your_module .  her_object"
 *	"his_module   ::   that_object"
 *
 * If the object name is simple (without "." or "::"), searchFirstObj 
 * is searched, and then the module is searched for an object with the 
 * given name.  
 *
 * If the name is complex, the module and object name 
 * are found, relative to the project.
 */
ABObj
obj_scoped_find_by_name(ABObj searchFirstObj, STRING objName)
{
    ABObj	obj= NULL;
    char	*dotPtr= NULL;
    char	*moduleNamePtr= NULL;
    char	*objNamePtr= objName;
    char	*moduleNameEndPtr = NULL;
    int		moduleNameEndChar = -1;
    char	nameBuf[1024];
    /* printf("obj_scoped_find_by_name(%s[%s])\n",
	obj_get_safe_name(searchFirstObj, nameBuf, 1024), 
	util_strsafe(objName)); */

    if ((dotPtr= strchr(objName, '.')) != NULL)
    {
	moduleNamePtr= objName;
	objNamePtr= dotPtr+1;
    }
    else if ((dotPtr= strstr(objName, "::")) != NULL)
    {
	moduleNamePtr= objName;
	objNamePtr= dotPtr+2;
    }

    /*
     *  Remove white space on either side of the separator
     */
    if (dotPtr != NULL)
    {
        /* put a 0 at the end of the module name */
	for (moduleNameEndPtr = dotPtr-1;
		((moduleNameEndPtr > objName) && isspace(*moduleNameEndPtr)); )
	{
	    --moduleNameEndPtr;
	}
	if (moduleNameEndPtr < dotPtr)
	{
	    ++moduleNameEndPtr; /* point one *after* last char in name */
	}
	moduleNameEndChar = *moduleNameEndPtr;
	*moduleNameEndPtr = 0;
    }
    while (((*objNamePtr) != 0) && (isspace(*objNamePtr)))
    {
	++objNamePtr;
    }

    /*
     * We now have the module and object name, so find the object.
     */
    if (moduleNamePtr == NULL)
    {
	/*
	 * This is a simple name
	 */
	obj= obj_find_by_name(searchFirstObj, objNamePtr);

	/*
	 * It doesn't exist in the scoping object, so look at the module
	 */
	if ((obj == NULL) && (!obj_is_module(searchFirstObj)))
	{
	    ABObj module = obj_get_module(searchFirstObj);
	    if (module != NULL)
	    {
		obj = obj_find_by_name(module, objNamePtr);
	    }
	}
    }
    else
    {
	/*
	 * Module name has been specified
	 */
	ABObj	objProject= obj_get_project(searchFirstObj);
	ABObj	objModule= NULL;
	
	if (objProject != NULL)
	{
	    objModule = obj_find_module_by_name(objProject, moduleNamePtr);
	}
	if (objModule != NULL)
	{
	    obj= obj_find_by_name(objModule, objNamePtr);
	}
    }

epilogue:
    /* replace the 0 we inserted with the char that was there */
    if (moduleNameEndPtr != NULL)
    {
	*moduleNameEndPtr= moduleNameEndChar;
    }
    return obj;
}

