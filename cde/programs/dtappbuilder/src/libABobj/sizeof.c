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
 *	$XConsortium: sizeof.c /main/4 1996/10/02 15:41:55 drk $
 *
 *	@(#)sizeof.c	3.5 11 Feb 1994	
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
 * sizeof.c
 */

#include <stdio.h>
#include <ab_private/obj.h>

main ()
{
	{
	AB_OBJ			obj;
	AB_OBJ_EXTRA_INFO	*info= &(obj.info);
	printf("sizeof:\n");
	printf("	ABObj:%ld (%ld + %ld)\n", 
			(long)sizeof(obj), 
			(long)sizeof(obj)-(long)sizeof(obj.info),
			(long)sizeof(obj.info));
	printf("	info:%ld\n", (long)sizeof(obj.info));
	printf("	action:%ld\n", (long)sizeof(info->action));
	printf("	action_list:%ld\n", (long)sizeof(info->action_list));
	printf("	button:%ld\n", (long)sizeof(info->button));
	printf("	choice:%ld\n", (long)sizeof(info->choice));
        printf("        file_chooser:%ld\n", (long)sizeof(info->file_chooser));
/*        printf("        message_box:%ld\n", (long)sizeof(info->msg_box));     */
	printf("	container:%ld\n", (long)sizeof(info->container));
	printf("	drawing_area:%ld\n", (long)sizeof(info->drawing_area));
/*	printf("	drop_target:%ld\n", (long)sizeof(info->drop_target)); */
	/*printf("	glyph:%ld\n", (long)sizeof(info->glyph)); */
	printf("	item:%ld\n", (long)sizeof(info->item));
	printf("	label:%ld\n", (long)sizeof(info->label));
	printf("	list:%ld\n", (long)sizeof(info->list));
	printf("	menu:%ld\n", (long)sizeof(info->menu));
/*	printf("	menu_ref:%ld\n", (long)sizeof(info->menu_ref)); */
	printf("	module:%ld\n", (long)sizeof(info->module));
	printf("	project:%ld\n", (long)sizeof(info->project));
	printf("	slider:%ld\n", (long)sizeof(info->scale));
	printf("	term:%ld\n", (long)sizeof(info->term));
	printf("	text:%ld\n", (long)sizeof(info->text));
	printf("	window:%ld\n", (long)sizeof(info->window));
	/* printf("	:%ld\n", (long)sizeof()); */
	}
}

