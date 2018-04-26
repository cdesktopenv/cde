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
 *	$XConsortium: obj_trigger.c /main/3 1995/11/06 18:39:23 rswiston $
 *
 * %W% %G%	cde_app_builder/src/libABobj
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
 *  trigger.c - Action triggers
 */

#include <stdio.h>
#include <stdlib.h>
#include <ab_private/util.h>
#include "objP.h"

/*
 * Creates and inits a trigger.
 */
ABTrigger
trigger_create(void)
{
	ABTrigger	trigger= NULL;

	trigger= (ABTrigger)malloc(sizeof(*trigger));
	trigger_init(trigger);
	return trigger;
}


int
trigger_destroy(ABTrigger trigger)
{
	trigger_release(trigger);
	util_free(trigger);
	return 0;
}


/*
 * Sets default values for all the fields
 */
int
trigger_init(ABTrigger trigger)
{
	trigger->next= NULL;
	trigger->prev= NULL;
	trigger->owner= NULL;
	trigger->from= NULL;
	trigger->when= AB_WHEN_UNDEF;
	return 0;
}


/*
 * Releases all resources and links the trigger may have.  It is safe
 * to delete the object after it is released.
 */
int
trigger_release(ABTrigger trigger)
{
	return trigger_unlink(trigger);
}


/*
 * Removes the trigger completely from the network of data structures.
 */
int 
trigger_unlink(ABTrigger trigger)
{
	if (trigger->owner != NULL)
	{
		obj_action_remove_trigger(trigger->owner, trigger);
	}
	return 0;
}

