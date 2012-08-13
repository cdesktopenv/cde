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
 *	$XConsortium: gil_loadobj.c /main/3 1995/11/06 18:29:29 rswiston $
 */

#include <ab_private/util.h>
#include <ab_private/abio.h>
#include "gilP.h"
#include "gil_loadattP.h"
#include "load.h"
#include "loadP.h"


/*
 * Loads one gil object into a parent.  The object is created by this
 * routine.
 */
int
gilP_load_object(FILE * inFile, ABObj parent)
{
    ABObj obj = obj_create(AB_TYPE_UNDEF, parent);
    abil_loadmsg_set_object(NULL);
    abil_loadmsg_set_att(NULL);
    abil_loadmsg_set_action_att(NULL);
    if (obj == NULL)
    {
        abil_print_load_err(ERR_NO_MEMORY);
    }
    return gilP_load_object2(inFile, obj, parent);
}


/*
 * Loads one gil object, using the object passed in to start with.  This way,
 * arbitrary attributes may be set on an object before the load process
 * begins.
 * 
 * This is necessary because some incredible bonehead neglegted to put :type and
 * :name keys into the parent object.  Since the normal load won't set the
 * type properly, we have to do it up front.
 */
int
gilP_load_object2(FILE * inFile, ABObj obj, ABObj parent)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    AB_GIL_ATTRIBUTE    attr;
    ISTRING             keyword;
    BOOL                object_read = FALSE;

    while (!object_read)
    {
        if (abio_get_eof(inFile))
        {
            abil_print_load_err(ERR_EOF);
            goto epilogue;
        }
        if (abio_gil_get_object_end(inFile))
        {
            /*** The whole object has been read! ***/
            object_read = TRUE;
            if (obj->type == AB_TYPE_LAYERS)
            {
                if (obj_get_num_children(obj) == 0)
                {
                    obj_destroy(obj);
                    obj = NULL;
                    goto epilogue;
                }
            }
            if (obj != NULL)
            {

                /*
                 * The root node may have itself as it's parent
                 */
                if ((!obj_has_parent(obj))
                    && (obj != parent))
                {
                    obj_append_child(parent, obj);
                }
            }
        }
        else
        {
            /*** More attributes to go... ***/
	    abil_loadmsg_set_err_printed(FALSE);
            if (!abio_get_keyword(inFile, &keyword))
            {
                abil_print_load_err(ERR_WANT_KEYWORD);
                goto epilogue;
            }
            abil_loadmsg_set_att(istr_string(keyword));

            attr = gilP_string_to_att(istr_string(keyword));
            if ((rc = gilP_load_attribute_value(
                                        inFile, obj, attr, parent)) != 0)
            {
                return_value = rc;
                goto epilogue;
            }
        }
    }                           /* while TRUE */

epilogue:
    return return_value;
}
