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
 *	$XConsortium: objP.h /main/3 1995/11/06 18:33:04 rswiston $
 *
 * @(#)objP.h	3.23 11 Feb 1994	cde_app_builder/src/libABobj
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
 * ABObjP.h - private declarations for libABobj
 *
 * Private file!  Not to be used by library clients!
 *
 * NB - THIS FILE MUST BE INCLUDED BY EVERY FILE IN libABobj.
 *      IT MUST BE INCLUDE *BEFORE* <obj.h> OR ANY OTHER INCLUDE
 *  	FILE THAT MAY INCLUDE <obj.h>
 *      
 */
#ifndef _ABOBJ_ABOBJP_H_
#define _ABOBJ_ABOBJP_H_

#include <stdio.h>
#include <ab_private/AB.h>

/*************************************************************************
**									** 
**	Include obj.h							**
**									**
**	*BEFORE* include obj.h, map Private field names from ugly	**
**	public names to something we can use internally.		**
**									**
*************************************************************************/
#define arg_type_ABOBJPRIVDDDD4810638710	arg_type
#define arg_value_ABOBJPRIVDDDD8231879567	arg_value
#define builtin_ABOBJPRIVDDDD6250806957		builtin
#define func_name_ABOBJPRIVDDDD7560862490	func_name
#define func_type_ABOBJPRIVDDDD7651983840	func_type
#define func_value_ABOBJPRIVDDDD4857490328	func_value
#define code_frag_ABOBJPRIVDDDD9845797105	code_frag
#define sval_ABOBJPRIVDDDD8534647380		sval
#define func_name_suffix_ABOBJPRIVDDDD2467925683 func_name_suffix
#define busy_drop_glyph_ABOBJPRIVDDDD9052574293	busy_drop_glyph
#define filter_pattern_ABOBJPRIVDDDD3579039821	filter_pattern
#define match_glyph_ABOBJPRIVDDDD8495326869	match_glyph
#define match_glyph_mask_ABOBJPRIVDDDD1538296450 match_glyph_mask
#define file_ABOBJPRIVDDDD2382601347		file
#define stubs_file_ABOBJPRIVDDDD6123725490	stubs_file
#define ui_file_ABOBJPRIVDDDD8421231863		ui_file
#define accelerator_ABOBJPRIVDDDD0256230318	accelerator
#define mnemonic_ABOBJPRIVDDDD0903052253	mnemonic
#define file_ABOBJPRIVDDDD2192049045		file
#define stubs_file_ABOBJPRIVDDDD1280681372	stubs_file
#define initial_value_string_ABOBJPRIVDDDD2521740590 initial_value_string
#define process_string_ABOBJPRIVDDDD3271493068	process_string
#define icon_ABOBJPRIVDDDD3403707469		icon
#define icon_mask_ABOBJPRIVDDD93883729879	icon_mask
#define icon_label_ABOBJPRIVDDDD4898216273	icon_label
#define name_ABOBJPRIVDDDD6281986324		name
#define impl_flags_ABOBJPRIVDDDD5707803418	impl_flags
#define impl_dnd_flags_ABOBJPRIVDDD7849439167	impl_dnd_flags
#define user_data_ABOBJPRIVDDDD1031984606	user_data
#define help_volume_ABOBJPRIVDDDD3479759502	help_volume
#define help_location_ABOBJPRIVDDDD1652183835	help_location
#define help_text_ABOBJPRIVDDDD3129281507	help_text
#define drag_cursor_ABOBJPRIVDDDD9147148353	drag_cursor
#define drag_cursor_mask_ABOBJPRIVDDDD7624768024 drag_cursor_mask

#define bg_color_ABOBJPRIVDDDD7609528164	bg_color
#define fg_color_ABOBJPRIVDDDD6392057630	fg_color
#define label_ABOBJPRIVDDDD8127856367		label
#define menu_name_ABOBJPRIVDDDD8412670921	menu_name
#define class_name_ABOBJPRIVDDDD9513018430	class_name


#include <ab_private/obj.h>



/*************************************************************************
**									** 
**	impl_flags flags						**
**									**
**  NOT TO BE USED OUTSIDE libABobj!					**
**									**
*************************************************************************/
typedef unsigned int	IMPL_FLAGS;
#define ObjFlagAlreadyFreedValue	((IMPL_FLAGS)(0xa5a5a5a5))
#define	ObjFlagNone			((IMPL_FLAGS)((0x00U)))
#define	ObjFlagIsReadOnly		((IMPL_FLAGS)((0x01U)<< 1))
#define	ObjFlagBeingDestroyed		((IMPL_FLAGS)((0x01U)<< 2))
#define	ObjFlagDestroyed		((IMPL_FLAGS)((0x01U)<< 3))
#define	ObjFlagWriteMe			((IMPL_FLAGS)((0x01U)<< 4))
#define ObjFlagHeightIsResizable	((IMPL_FLAGS)((0x01U)<< 5))
#define ObjFlagIsDefined		((IMPL_FLAGS)((0x01U)<< 6))
#define ObjFlagIsInitiallyActive	((IMPL_FLAGS)((0x01U)<< 7))
#define ObjFlagIsInitiallyVisible	((IMPL_FLAGS)((0x01U)<< 8))
#define ObjFlagIsSelected		((IMPL_FLAGS)((0x01U)<< 9))
#define ObjFlagWasWritten		((IMPL_FLAGS)((0x01U)<<10))
#define ObjFlagWidthIsResizable		((IMPL_FLAGS)((0x01U)<<11))

/*
 * Internal lists of objects
 */
extern ABObjList	objP_all_objs_list;	/* all existing objs */
int objP_lists_add(ABObj obj);
int objP_lists_remove(ABObj obj);


/*************************************************************************
**									** 
**	Private functions for this code module				**
**									**
*************************************************************************/
int	obj_construct_type_specific_info(ABObj obj);
int	obj_destruct_type_specific_info(ABObj obj);
int	abo_error(STRING message);
int	obj_set_impl_flags(ABObj, IMPL_FLAGS flagsToAdd);
int	obj_clear_impl_flags(ABObj, IMPL_FLAGS flagsToClear);
int	obj_has_impl_flags(ABObj, IMPL_FLAGS flagsToTest);
int	objP_actually_destroy_one(ABObj obj);
int	objP_free_mem(ABObj obj);		/* frees memory */
int	objP_update_verify(ABObj obj);		/* "sometimes" verifies */
int	objP_set_parent(ABObj obj, ABObj newParent);


#define do_return_if_err(_return_code, _return_value) \
	   if ((_return_code) < 0) \
	   { \
	       return_value= (_return_value); \
	       goto epilogue; \
	   }

#define do_return() \
	    goto epilogue
	
#define do_return_code(_return_code) \
	    {return_value= (_return_code); goto epilogue;}



/*************************************************************************
**									** 
**	inline implementation						**
**									**
**									**
*************************************************************************/
#define obj_set_impl_flags(obj, flags)	((obj)->impl_flags |= (flags))
#define obj_clear_impl_flags(obj, flags) ((obj)->impl_flags &= ~(flags))
#define obj_has_impl_flags(obj, flags) (((obj)->impl_flags & (flags)) != 0)

/*
 * Pass-by-reference func
 */
int objP_actually_destroy_one_impl(ABObj *objPtr);
#define objP_actually_destroy_one(obj) (objP_actually_destroy_one_impl(&(obj)))

int objP_free_mem_impl(ABObj *objPtr);
#define objP_free_mem(obj)	(objP_free_mem_impl(&(obj)))


#endif /* _ABOBJ_ABOBJP_H_ */

