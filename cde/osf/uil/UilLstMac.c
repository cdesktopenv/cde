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
 *  @OSF_COPYRIGHT@
 *  COPYRIGHT NOTICE
 *  Copyright (c) 1990, 1991, 1992, 1993 Open Software Foundation, Inc.
 *  ALL RIGHTS RESERVED (MOTIF). See the file named COPYRIGHT.MOTIF for
 *  the full copyright text.
*/ 
/* 
 * HISTORY
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: UilLstMac.c /main/15 1997/03/12 15:21:48 dbl $"
#endif
#endif

/*
*  (c) Copyright 1989, 1990, DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASS. */

/*
**++
**  FACILITY:
**
**      User Interface Language Compiler (UIL)
**
**  ABSTRACT:
**
**      This module contain the routines for creating the machine listing.
**
**--
**/


/*
**
**  INCLUDE FILES
**
**/



#include <Mrm/MrmAppl.h>
#include <X11/Xmd.h>		/* For 64-bit architecture dependencies */


#ifdef DXM_V11
#include <DXm/DXmHelpB.h>
#endif

#include "UilDefI.h"


/*
**
**  DEFINE and MACRO DEFINITIONS
**
**/

/*	Holds offset info for machine code listing.	*/

typedef struct _off_info {
	unsigned short int	w_off_type;
	unsigned short int	w_off_offset;
	} off_info_type;

#define		k_name_off		0
#define		k_class_off		1
#define		k_arglist_off		2
#define		k_children_off		3
#define		k_comment_off		4
#define		k_creation_off		5
#define		k_callback_off		6
#define		k_resource_off		7
#define		k_float_off		8
#define		k_unknown_off		9
#define		k_resource_id_off	10
#define		k_child_off		11
#define		k_text_entry_off	12
#define		k_null_text_entry_off	13
#define         k_single_float_off      14 /* single float data type RAP */

#define		k_off_stack_size	300

static off_info_type	off_info_stack [k_off_stack_size];

static int		off_info_cnt = 0;




#define		off_stack_empty		(off_info_cnt == 0)

#define _Offset(type,base,field)    \
        ((unsigned int)(base + XtOffsetOf(type, field)))

#if defined(__STDC__)
#define _OffsetArray(type,base,field)    \
        ((unsigned int)(base + XtOffsetOf(type, field)))
#else
#define _OffsetArray(type,base,field)    \
        ((unsigned int)(base + XtOffsetOf(type, field[0])))
#endif
#define	_Pointer(base,offset) ((char *)(base)+(offset))

#define ASNHEADERLEN 3
#define CSLONGBIT    (unsigned char)(0x80)
#define ASNstrlen(a) (ASNHEADERLEN+ \
		      (((unsigned char)(a)[ASNHEADERLEN]&CSLONGBIT)? \
		       (((unsigned short)((a)[ASNHEADERLEN+1])<<8) | \
			(unsigned short)((a)[ASNHEADERLEN+2]))+3 : \
		       ((unsigned short)((a)[ASNHEADERLEN]))+1))

/*
**
**  EXTERNAL VARIABLE DECLARATIONS
**
**/


/*
**
**  GLOBAL VARIABLE DECLARATIONS
**
**/


/*
**
**  OWN VARIABLE DECLARATIONS
**
**/



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function saves the machine code listing information for widgets.
**
**  FORMAL PARAMETERS:
**
**      widget_entry	Symbol node for this widget
**      az_context	context containing the URM output information.
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      the machine code is saved in the source record structure for
**	the widget.
**
**--
**/

void		save_widget_machine_code ( widget_entry, az_context )

sym_widget_entry_type	* widget_entry;
URMResourceContext	* az_context;

{

    RGMWidgetRecordPtr		w_rec;
    char			buffer [132];
    unsigned short int		w_rec_size;
    src_source_record_type	* az_src_rec;

#define	_WRPointer(w_rec,offset) ((char *)(w_rec)+(offset))

#define _WROffset(field)    (_Offset(RGMWidgetRecord,0,field))

    az_src_rec = widget_entry->header.az_src_rec;

    if (widget_entry->resource_id != 0 ) {
	sprintf (buffer, "Resource ID: %08X", widget_entry->resource_id);
	src_append_machine_code (
		az_src_rec,
		0,
		0, 0,
		buffer );
    } else if (widget_entry->obj_header.az_name != NULL) {
	sprintf (buffer, "Resource index: %s",
		 widget_entry->obj_header.az_name->c_text);
	src_append_machine_code (
		az_src_rec,
		0,
		0, 0,
		buffer );
    }

    w_rec = (RGMWidgetRecordPtr) UrmRCBuffer (az_context);

    src_append_machine_code (
		az_src_rec,
		_WROffset (validation),
		sizeof (w_rec->validation), (char*)& w_rec->validation,
		"widget record" );

    sprintf (buffer, "size: %d", w_rec->size);

    src_append_machine_code (
		az_src_rec,
		_WROffset (size),
		sizeof (w_rec->size), (char*)& w_rec->size,
		buffer );

    w_rec_size = w_rec->size;

    sprintf (buffer, "access: %s",
			access_from_code (w_rec->access) );

    src_append_machine_code (
		az_src_rec,
		_WROffset (access),
		sizeof (w_rec->access), (char*)& w_rec->access,
		buffer );

    sprintf (buffer, "locked: ");

    if (w_rec->lock) {
	strcat (buffer, "true");
    } else {
	strcat (buffer, "false");
    }

    src_append_machine_code (
		az_src_rec,
		_WROffset (lock),
		sizeof (w_rec->lock), (char*)& w_rec->lock,
		buffer );

    sprintf (buffer, "type: ");

    strcat (buffer, class_name_from_code (w_rec->type));

    src_append_machine_code (
		az_src_rec,
		_WROffset (type),
		sizeof (w_rec->type), (char*)& w_rec->type,
		buffer );

    if (w_rec->name_offs == (MrmOffset) 0) {
	sprintf (buffer, "no name specified");
    } else {
	sprintf (buffer, "name: offset %X (hex)",
		 w_rec->name_offs);
	off_put (k_name_off, w_rec->name_offs);
    }

    src_append_machine_code (
		az_src_rec,
		_WROffset (name_offs),
		sizeof (w_rec->name_offs),
		(char*)& w_rec->name_offs,
		buffer );

    if (w_rec->class_offs == (MrmOffset) 0) {
	sprintf (buffer, "class: builtin");
    } else {
	sprintf (buffer, "class: offset %X (hex)",
		 w_rec->class_offs);
	off_put (k_class_off, w_rec->class_offs);
    }

    src_append_machine_code (
		az_src_rec,
		_WROffset (class_offs),
		sizeof (w_rec->class_offs),
		(char*)& w_rec->class_offs,
		buffer );

    if (w_rec->arglist_offs == (MrmOffset) 0) {
	sprintf (buffer, "no argument list");
    } else {
	sprintf (buffer, "argument list offset: %X (hex)", w_rec->arglist_offs);
	off_put (k_arglist_off, w_rec->arglist_offs);
    }

    src_append_machine_code (
		az_src_rec,
		_WROffset (arglist_offs),
		sizeof (w_rec->arglist_offs),
		(char*)& w_rec->arglist_offs,
		buffer );

    if (w_rec->children_offs == (MrmOffset) 0) {
	sprintf (buffer, "no children");
    } else {
	sprintf (buffer, "children list offset: %X (hex)",
		 w_rec->children_offs);
	off_put (k_children_off, w_rec->children_offs);
    }

    src_append_machine_code (
		az_src_rec,
		_WROffset (children_offs),
		sizeof (w_rec->children_offs),
		(char*)& w_rec->children_offs,
		buffer );

    if (w_rec->comment_offs == (MrmOffset) 0) {
	sprintf (buffer, "no comment specified");
    } else {
	sprintf (buffer, "comment: offset %X (hex)",
		 w_rec->comment_offs);
	off_put (k_comment_off, w_rec->comment_offs);
    }

    src_append_machine_code (
		az_src_rec,
		_WROffset (comment_offs),
		sizeof (w_rec->comment_offs),
		(char*)& w_rec->comment_offs,
		buffer );

    if (w_rec->creation_offs == (MrmOffset) 0) {
	sprintf (buffer, "no creation callback");
    } else {
	sprintf (buffer, "creation callback offset: %X (hex)",
		 w_rec->creation_offs);
	off_put (k_creation_off, w_rec->creation_offs);
    }

    src_append_machine_code (
		az_src_rec,
		_WROffset (creation_offs),
		sizeof (w_rec->creation_offs),
		(char*)& w_rec->creation_offs,
		buffer );

    unload_stack(( char *) w_rec, w_rec_size, az_src_rec );

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function unloads entries placed on a stack and outputs them
**	into the listing.  The routine is used by both widgets and gadgets
**	to output their argument, control, and callback lists.
**
**  FORMAL PARAMETERS:
**
**      rec	    base of the record being annotated
**      rec_sizr    size of the record being annotated
**      az_src_rec  address of the source for record being annotated
**
**  IMPLICIT INPUTS:
**
**
**  IMPLICIT OUTPUTS:
**
**
**  SIDE EFFECTS:
**
**
**--
**/
void	unload_stack( rec, rec_size, az_src_rec )

char			*rec;
int			rec_size;
src_source_record_type	*az_src_rec;

{

    char			buffer [132], * ptr;
    int				child_index;

    while ( ! off_stack_empty ) {

	unsigned short		off_type, off_offset;

	off_get (& off_type, & off_offset);

	switch (off_type) {

	    case k_name_off :
	    case k_class_off :
	    case k_comment_off : {

		ptr = (char *) _Pointer(rec, off_offset);
		src_append_machine_code (
			az_src_rec, off_offset,
			strlen (ptr) + 1, ptr, NULL );
		break;
	    }
	    
	    case k_unknown_off : {

		unsigned short int	len;

/*	Write out everything up to the next offset or the end of the
	record.	*/

		if ( ! off_stack_empty ) {
		    unsigned short int	next_type, next_offset;

		    off_get (& next_type, & next_offset);
		    off_put (next_type, next_offset);
		    len = next_offset - off_offset;
		} else {
		    len = rec_size - off_offset;
		}

		ptr = (char *) _Pointer(rec, off_offset);
		src_append_machine_code (
			az_src_rec, off_offset,
			len, ptr, NULL );
		break;
	    }
	    
	    case k_arglist_off : {

		RGMArgListDescPtr	argdesc;
		RGMArgumentPtr		arg_ptr;
		int			j;

		argdesc = (RGMArgListDescPtr) _Pointer(rec, off_offset);

		sprintf (buffer, "argument count: %d", argdesc->count);

		src_append_machine_code (
			az_src_rec,
			_Offset (RGMArgListDesc, off_offset, count),
			sizeof (argdesc->count),
			(char*)& argdesc->count,
			buffer );

		sprintf (buffer, "related argument count: %d", argdesc->extra);

		src_append_machine_code (
			az_src_rec,
			_Offset (RGMArgListDesc, off_offset, extra),
			sizeof (argdesc->extra),
			(char*)& argdesc->extra,
			buffer );

		off_offset = _OffsetArray (RGMArgListDesc, off_offset, args);

		for (j = 0, arg_ptr = argdesc->args;
		     j < argdesc->count;
		     j++, arg_ptr++, off_offset += sizeof (RGMArgument)) {

		    if (arg_ptr->tag_code != UilMrmUnknownCode )
			{
			sprintf (buffer,
				 "(%d) arg type: %s",
				 j,
				 resource_name_from_code(arg_ptr->tag_code));
			}
		    else
			{
			ptr = (char *)
			    _Pointer(rec,arg_ptr->stg_or_relcode.tag_offs);
			sprintf (buffer,
				 "(%d) arg type: %s (user defined)",
				 j,
				 ptr);
			}

		    src_append_machine_code (
			az_src_rec,
			_Offset (RGMArgument, off_offset, tag_code),
			sizeof (arg_ptr->tag_code),
			(char*)& arg_ptr->tag_code,
			buffer );

		    if (arg_ptr->stg_or_relcode.tag_offs == (MrmOffset) 0) {
			sprintf (buffer, "(%d) no tag offset", j);
		    } else {
			sprintf (buffer, "(%d) tag offset: %X (hex)",
					j, arg_ptr->stg_or_relcode.tag_offs);
			off_put (k_name_off, arg_ptr->stg_or_relcode.tag_offs);
		    }

		    src_append_machine_code (
			az_src_rec,
			_Offset (RGMArgument, off_offset, stg_or_relcode.tag_offs),
			sizeof (arg_ptr->stg_or_relcode.tag_offs),
			(char*)& arg_ptr->stg_or_relcode.tag_offs,
			buffer );

		    sprintf (buffer, "(%d) type: %s",
			j, type_from_code (arg_ptr->arg_val.rep_type) );

		    src_append_machine_code (
			az_src_rec,
			_Offset (RGMArgument, off_offset,
							arg_val.rep_type),
			sizeof (arg_ptr->arg_val.rep_type),
			(char*)& arg_ptr->arg_val.rep_type,
			buffer );

		    sprintf (buffer, "(%d) ", j);

		    format_arg_value (& arg_ptr->arg_val,
				& buffer [strlen (buffer)]);

		    src_append_machine_code (
			az_src_rec,
			_Offset (RGMArgument, off_offset,
							arg_val.datum),
			sizeof (arg_ptr->arg_val.datum),
			(char*)& arg_ptr->arg_val.datum,
			buffer );

		}

		break;
	    }

	    case k_float_off : {

		double	* dptr;

		dptr = (double *) _Pointer (rec, off_offset);
		sprintf (buffer, "floating point value: %g", (* dptr));
		src_append_machine_code (
			az_src_rec,
			off_offset,
			sizeof (double), (char*)dptr, buffer );
		break;
	    }

/* single float data type RAP */
	    case k_single_float_off :
	      {
		float *fptr;

		fptr = (float *) _Pointer(rec, off_offset);
		sprintf(buffer, "Single float value: %g", (* fptr));
		src_append_machine_code (
					 az_src_rec,
					 off_offset,
					 sizeof (float), (char*)fptr, buffer);
		break;
	      }


	    case k_creation_off :
	    case k_callback_off : {

		RGMCallbackDescPtr	cb_desc_ptr;
		RGMCallbackItemPtr	cb_item_ptr;
		int			j;

		cb_desc_ptr =
			(RGMCallbackDescPtr) _Pointer (rec, off_offset);

		src_append_machine_code (
			az_src_rec,
			_Offset (RGMCallbackDesc, off_offset, validation),
			sizeof (cb_desc_ptr->validation),
			(char*)& cb_desc_ptr->validation,
			"callback descriptor" );

		sprintf (buffer, "callback count: %d", cb_desc_ptr->count);

		src_append_machine_code (
			az_src_rec,
			_Offset (RGMCallbackDesc, off_offset, count),
			sizeof (cb_desc_ptr->count),
			(char*)& cb_desc_ptr->count,
			buffer );

		off_offset =
			_OffsetArray (RGMCallbackDesc, off_offset, item);

		for (j = 0, cb_item_ptr = cb_desc_ptr->item;
		     j < cb_desc_ptr->count;
		     j++, cb_item_ptr++,
				off_offset += sizeof (RGMCallbackItem)) {

		    sprintf (buffer,
				"(%d) routine name offset: %X (hex)",
				j, cb_item_ptr->cb_item.routine);

		    src_append_machine_code (
			az_src_rec,
			_Offset (RGMCallbackItem, off_offset,
				 cb_item.routine),
#ifndef WORD64
			sizeof (cb_item_ptr->cb_item.routine),
			(char*)& cb_item_ptr->cb_item.routine,
#else /* 64-bit architecture */
			4,
			(char*)& cb_item_ptr->cb_item,
#endif /* WORD64 */
			buffer );

		    off_put (k_name_off, cb_item_ptr->cb_item.routine);

		    sprintf (buffer, "(%d) routine tag type: %s",
			j, type_from_code (
				cb_item_ptr->cb_item.rep_type) );

		    src_append_machine_code
			(az_src_rec,
			 _Offset (RGMCallbackItem, off_offset,
				  cb_item.rep_type),
#ifndef WORD64
			 sizeof (cb_item_ptr->cb_item.rep_type),
			 (char*)& cb_item_ptr->cb_item.rep_type,
#else /* 64-bit architecture */
			4,
			(char*)& cb_item_ptr->cb_item + 4,
#endif /* WORD64 */
			 buffer );

		    sprintf (buffer, "(%d) value: ", j);
		    src_append_machine_code
			(az_src_rec,
			 _Offset (RGMCallbackItem, off_offset,
				  cb_item.datum),
			 sizeof (cb_item_ptr->cb_item.datum),
			 (char*)& cb_item_ptr->cb_item.datum,
			 buffer );

		}	/* for */

		break;
	    }

	    case k_resource_off : {

		RGMResourceDescPtr	r_desc_ptr;

		r_desc_ptr =
			(RGMResourceDescPtr) _Pointer (rec, off_offset);

		sprintf (buffer, "resource descriptor, size: %d",
			 r_desc_ptr->size);

		src_append_machine_code (
			az_src_rec,
			_Offset (RGMResourceDesc, off_offset, size),
			sizeof (r_desc_ptr->size),
			(char*)& r_desc_ptr->size,
			buffer );

		sprintf (buffer, "access: %s",
				access_from_code (r_desc_ptr->access) );

		src_append_machine_code (
			az_src_rec,
			_Offset (RGMResourceDesc, off_offset, access),
			sizeof (r_desc_ptr->access),
			(char*)& r_desc_ptr->access,
			buffer );

		switch (r_desc_ptr->type) {
		    case URMrIndex:
			sprintf (buffer, "index, offset: %X (hex)",
				_Offset (RGMResourceDesc, off_offset, key) );

			off_put (k_name_off,
				_Offset (RGMResourceDesc, off_offset, key) );
			break;

		    case URMrRID:

			sprintf (buffer, "resource ID, offset: %X (hex)",
				_Offset (RGMResourceDesc, off_offset, key) );

			off_put (k_resource_id_off,
				_Offset (RGMResourceDesc, off_offset, key) );
			break;

		    default:
			sprintf (buffer, "unknown resource type");
			break;
		}

		src_append_machine_code (
			az_src_rec,
			_Offset (RGMResourceDesc, off_offset, type),
			sizeof (r_desc_ptr->type),
			(char*)& r_desc_ptr->type,
			buffer );

		sprintf (buffer, "resource group: %s",
			 group_from_code (r_desc_ptr->res_group) );

		src_append_machine_code (
			az_src_rec,
			_Offset (RGMResourceDesc, off_offset, res_group),
			sizeof (r_desc_ptr->res_group),
			(char*)& r_desc_ptr->res_group,
			buffer );

		sprintf (buffer, "resource type: %s",
			(r_desc_ptr->res_group == URMgLiteral) ? 
				type_from_code (r_desc_ptr->cvt_type) :
				class_name_from_code (r_desc_ptr->cvt_type) );

		src_append_machine_code (
			az_src_rec,
			_Offset (RGMResourceDesc, off_offset, cvt_type),
			sizeof (r_desc_ptr->cvt_type),
			(char*)& r_desc_ptr->cvt_type,
			buffer );

		break;
	    }

	    case k_resource_id_off : {

		sprintf (buffer, "resource id");

		src_append_machine_code (
			az_src_rec,
			off_offset,
			sizeof (MrmResource_id),
			(char *) _Pointer (rec, off_offset),
			buffer );

		break;
	    }

	    case k_children_off : {

		RGMChildrenDescPtr	c_desc_ptr;
		int			j;

		c_desc_ptr = (RGMChildrenDescPtr) _Pointer(rec, off_offset);

		sprintf (buffer, "Children count: %d",
			 c_desc_ptr->count);

		src_append_machine_code (
			az_src_rec,
			_Offset (RGMChildrenDesc, off_offset, count),
			sizeof (c_desc_ptr->count),
			(char*)& c_desc_ptr->count,
			buffer );

		off_offset =
			_OffsetArray (RGMChildrenDesc, off_offset, child);

		for (j = 0; j < c_desc_ptr->count;
			     j++, off_offset += sizeof (RGMChildDesc)) {

		    off_put (k_child_off, off_offset);
		}	/* for */

		child_index = 0;

		break;
	    }

	    case k_child_off : {

		RGMChildDescPtr		c_ptr;
		int			j;

		c_ptr = (RGMChildDescPtr) _Pointer(rec, off_offset);

		j = child_index;

		child_index++;

		if (c_ptr->manage) {
		    sprintf (buffer, "(%d) managed", j);
		} else {
		    sprintf (buffer, "(%d) unmanaged", j);
		}

		src_append_machine_code (
			az_src_rec,
			_Offset (RGMChildDesc, off_offset, manage),
			sizeof (c_ptr->manage),
			(char*)& c_ptr->manage,
			buffer );

		sprintf (buffer, "(%d) access: %s", j,
				access_from_code (c_ptr->access) );

		src_append_machine_code (
			az_src_rec,
			_Offset (RGMChildDesc, off_offset, access),
			sizeof (c_ptr->access),
			(char*)& c_ptr->access,
			buffer );

		switch (c_ptr->type) {
		    case URMrIndex:
			sprintf (buffer, "(%d) index, offset: %X (hex)",
				j, c_ptr->key.index_offs);

			src_append_machine_code (
				az_src_rec,
				_Offset (RGMChildDesc, off_offset, type),
				sizeof (c_ptr->type) + sizeof (c_ptr->key),
				(char*)& c_ptr->type,
				buffer );

			off_put (k_name_off, c_ptr->key.index_offs);

			break;

		    case URMrRID:
			sprintf (buffer,
				"(%d) resource ID, offset: %X (hex)",
				j, _Offset (RGMChildDesc, off_offset, key) );

			src_append_machine_code (
				az_src_rec,
				_Offset (RGMChildDesc, off_offset, type),
				sizeof (c_ptr->type),
				(char*)& c_ptr->type,
				buffer );

			off_put (k_resource_id_off,
				_Offset (RGMChildDesc, off_offset, key) );
			break;

		    default:

			sprintf (buffer, "(%d) unknown class", j);

			src_append_machine_code (
				az_src_rec,
				_Offset (RGMChildDesc, off_offset, type),
				sizeof (c_ptr->type),
				(char*)& c_ptr->type,
				buffer );

			break;
		}

		break;
	    }

	    default : {

		break;
	    }
        }	/* switch	*/
    }		/*  for		*/

    src_append_machine_code ( az_src_rec, 0, 0, 0, 0 );

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function saves the machine code listing information for values.
**
**  FORMAL PARAMETERS:
**
**      value_entry	Symbol node for this value
**      az_context	context containing the URM output information.
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      the machine code is saved in the source record structure for this
**	value.
**
**--
**/

void		save_value_machine_code ( value_entry, az_context )

sym_value_entry_type	* value_entry;
URMResourceContext	* az_context;

{
    src_source_record_type	* az_src_rec;
    char			buffer [132], * rc_buffer;
    unsigned short int		rc_size;

    az_src_rec = value_entry->header.az_src_rec;

    if (value_entry->resource_id != 0) {
	sprintf (buffer, "Resource ID: %08X", value_entry->resource_id);
	src_append_machine_code (
		az_src_rec,
		0,
		0, 0,
		buffer );
    } else if (value_entry->obj_header.az_name != NULL) {
	sprintf (buffer, "Resource index: %s",
		 value_entry->obj_header.az_name->c_text);
	src_append_machine_code
	    (az_src_rec,
	     0,
	     0, 0,
	     buffer);
    }

    sprintf (buffer, "size: %d, group: %s",
	     UrmRCSize (az_context),
	     group_from_code (UrmRCGroup (az_context)) );
    src_append_machine_code ( az_src_rec, 0, 0, 0, buffer );

    sprintf (buffer, "type: %s, access: %s, locked: ",
	     type_from_code (UrmRCType (az_context)),
	     access_from_code (UrmRCAccess (az_context)) );

    if (UrmRCLock (az_context)) {
	strcat (buffer, "true");
    } else {
	strcat (buffer, "false");
    }

    src_append_machine_code ( az_src_rec, 0, 0, 0, buffer );

    rc_buffer = UrmRCBuffer (az_context);
    rc_size = UrmRCSize (az_context);

    /* 
    **	Case on the type of literal.
    */

    switch (value_entry->b_type)
	{
	case sym_k_bool_value:

	    if ( * (unsigned long *) rc_buffer )
		sprintf (buffer, "value: true");
	    else
		sprintf (buffer, "value: false");
	    src_append_machine_code (
		az_src_rec, 0, rc_size, rc_buffer, buffer );
	    break;

	case sym_k_integer_value:
	case sym_k_horizontal_integer_value:
	case sym_k_vertical_integer_value:

	    sprintf (buffer, "value: %d",
		     (* (unsigned long *) rc_buffer) );
	    src_append_machine_code (
		az_src_rec, 0, rc_size, rc_buffer, buffer );
	    break;

	case sym_k_integer_table_value: 
	case sym_k_rgb_value:  /* rgb data type RAP */
	    {
	    int index = 0;
	    int offset = 0;

	    for (offset = 0; offset < (int)rc_size; offset += sizeof(int), index++)
		{
		sprintf (buffer, "value[%d]: %d",
			 index, ((unsigned long *) rc_buffer)[index] );
		
		src_append_machine_code
		    (az_src_rec, 0, sizeof(int), 
		     (char*)&(((unsigned long *)rc_buffer)[index]), buffer );
		}
	    break;
	    }

	case sym_k_float_value:
	case sym_k_horizontal_float_value:
	case sym_k_vertical_float_value:
	    sprintf (buffer, "value: %g",
		     (* (double *) rc_buffer) );
	    src_append_machine_code (
		az_src_rec, 0, rc_size, rc_buffer, buffer );
	    break;

	case sym_k_single_float_value:
	    sprintf (buffer, "value: %g",
		     (* (float *) rc_buffer));
	    src_append_machine_code
		(az_src_rec, 0, rc_size, rc_buffer, buffer );
            break;

	case sym_k_char_8_value:
	case sym_k_color_value:
	case sym_k_color_table_value:
	case sym_k_icon_value:
	case sym_k_font_value:
	case sym_k_fontset_value:
	case sym_k_font_table_value:
	case sym_k_reason_value:
	case sym_k_argument_value:
	case sym_k_identifier_value:
	case sym_k_compound_string_value:
	case sym_k_xbitmapfile_value:
	case sym_k_keysym_value:
	    src_append_machine_code (
		az_src_rec,
		0,
		rc_size, rc_buffer, 0 );
	    break;

	case sym_k_asciz_table_value:
	case sym_k_string_table_value: {
	    RGMTextVectorPtr	tv_ptr;
	    unsigned short	off_type, off_offset, off_cnt;
	    int			i;

	    off_offset = 0;
	    tv_ptr = (RGMTextVectorPtr) rc_buffer;
	    src_append_machine_code (
			az_src_rec,
			_Offset (RGMTextVector, off_offset, validation),
			sizeof (tv_ptr->validation),
			(char*)& tv_ptr->validation,
			"text vector" );

	    sprintf (buffer, "count: %d", tv_ptr->count);
	    src_append_machine_code (
			az_src_rec,
			_Offset (RGMTextVector, off_offset, count),
			sizeof (tv_ptr->count), (char*)& tv_ptr->count,
			buffer );

	    off_offset = _OffsetArray (RGMTextVector, off_offset, item);
	    for (i = 0; i < tv_ptr->count;
		 i++, off_offset += sizeof (RGMTextEntry)) {

		off_put (k_text_entry_off, off_offset);
	    }

	    off_put (k_null_text_entry_off, off_offset);
	    off_cnt = 0;
	    while ( ! off_stack_empty ) {

		RGMTextEntryPtr te_ptr;
		off_get (& off_type, & off_offset);
		switch (off_type) {
		    case k_name_off :

		      {
			char *stringPtr = (char *) _Pointer (rc_buffer,off_offset);
			src_append_machine_code (az_src_rec, off_offset,
						 ASNstrlen( stringPtr ),
						 stringPtr, NULL );	    
		      }
			break;

		    case k_null_text_entry_off :

			src_append_machine_code (
				az_src_rec, off_offset,
				sizeof (RGMTextEntry),
				(char*)_Pointer (rc_buffer, off_offset),
				"end of text vector" );

			break;
	    
		    case k_text_entry_off :

			te_ptr = (RGMTextEntryPtr)
					_Pointer (rc_buffer, off_offset);

			sprintf (buffer, "(%d) type: %s, offset: %X (hex)",
			    off_cnt,
			    type_from_code (te_ptr->text_item.rep_type),
			    te_ptr->text_item.offset );

			src_append_machine_code (
				az_src_rec,
				_Offset (RGMTextEntry, off_offset,
								text_item),
				sizeof (te_ptr->text_item),
				(char*)& te_ptr->text_item,
				buffer );

			off_cnt++;

			off_put (k_name_off, te_ptr->text_item.offset);

			break;

		    default:

			sprintf (buffer, "(%d) unknown text entry", i);

			src_append_machine_code (
				az_src_rec,
				off_offset,
				0, 0, buffer );

			break;
		    }
		}

	    break;
	}

	case sym_k_trans_table_value:

	    src_append_machine_code (
		az_src_rec, 0, rc_size, rc_buffer, NULL );

	    break;
	    
	default: {

	    sprintf (buffer, "unknown value");

	    src_append_machine_code (
		az_src_rec,
		0,
		0, 0, buffer );

	    break;
	}
    }

    src_append_machine_code ( az_src_rec, 0, 0, 0, 0 );

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function saves the machine code listing information for modules.
**
**  FORMAL PARAMETERS:
**
**      az_src_rec	Source record for this machine code info.
**      az_context	context containing the URM output information.
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      the machine code is saved in the source record structure
**
**--
**/

void		save_module_machine_code ( az_src_rec, az_context )

src_source_record_type	* az_src_rec;
URMResourceContext	* az_context;

{

    RGMModuleDescPtr		m_rec;
    RGMTopmostDescPtr		t_rec;
    char			buffer [132];
    unsigned short		off_offset;
    int				i;

    off_offset = 0;

    m_rec = (RGMModuleDescPtr) UrmRCBuffer (az_context);

    src_append_machine_code (
		az_src_rec,
		_Offset (RGMModuleDesc, off_offset, validation),
		sizeof (m_rec->validation), (char*)& m_rec->validation,
		"module record" );

    sprintf (buffer, "topmost count: %d", m_rec->count);

    src_append_machine_code (
		az_src_rec,
		_Offset (RGMModuleDesc, off_offset, count),
		sizeof (m_rec->count), (char*)& m_rec->count,
		buffer );

    off_offset = _OffsetArray (RGMModuleDesc, off_offset, topmost);

    for (i = 0, t_rec = m_rec->topmost;
	 i < m_rec->count;
	 i++, t_rec++, off_offset += sizeof (RGMTopmostDesc)) {

	sprintf (buffer, "(%d) %s", i, t_rec->index);

	src_append_machine_code (
		az_src_rec,
		_OffsetArray (RGMTopmostDesc, off_offset, index),
		strlen(t_rec->index), t_rec->index,
		buffer );

    }	/* for */

    src_append_machine_code ( az_src_rec, 0, 0, 0, 0 );

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function adds an offset entry to the offset stack for
**	machine code.  The entry is inserted in the proper place in
**	the stack depending on the offset value.
**
**  FORMAL PARAMETERS:
**
**      off_type	type of offset entry
**	off_offset	offset value for this offset entry
**
**  IMPLICIT INPUTS:
**
**	off_info_cnt		number of offset stack entries
**      off_info_stack	stack containing offset entries
**
**  IMPLICIT OUTPUTS:
**
**	off_info_cnt		number of offset stack entries
**      off_info_stack	stack containing offset entries
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      the offset info is saved in the offset stack
**
**--
**/

void	off_put 

	(unsigned short off_type, unsigned short off_offset)

{
    off_info_type	off_info, * off_info_ptr;
    int			i;

    if (off_info_cnt >= k_off_stack_size) {
	diag_issue_internal_error ("stack overflow in machine listing");
    }

    off_info.w_off_type = off_type;
    off_info.w_off_offset = off_offset;

    /*   Insert on the stack before the first entry with a lower offset
	 value	*/

    for (i = 0, off_info_ptr = off_info_stack;
			 i < off_info_cnt; i++, off_info_ptr++) {
	if (off_info.w_off_offset > off_info_ptr->w_off_offset) {
	    break;
	}
    }

    _move ( & off_info_ptr [1], off_info_ptr,
	    (off_info_cnt - i) * sizeof (off_info_type) );

    (* off_info_ptr) = off_info;
    off_info_cnt++;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function removes an offset entry to the offset stack for
**	machine code.
**
**  FORMAL PARAMETERS:
**
**      off_type	address to receive the type of offset entry
**	off_offset	address to receive the  offset value for this
**			offset entry
**
**  IMPLICIT INPUTS:
**
**	off_info_cnt		number of offset stack entries
**      off_info_stack	stack containing offset entries
**
**  IMPLICIT OUTPUTS:
**
**	off_info_cnt		number of offset stack entries
**      off_info_stack	stack containing offset entries
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      the offset info is retrieved from the offset stack
**
**--
**/

void	off_get (off_type, off_offset)

unsigned short		* off_type;
unsigned short		* off_offset;

{
    off_info_type	* off_info_ptr;

    if (off_info_cnt <= 0) {
	diag_issue_internal_error ("stack underflow in machine listing");
    }

    /*   Remove from the stack the entry with the lowest offset value	*/

    off_info_ptr = & off_info_stack [off_info_cnt - 1];

    (* off_type) = off_info_ptr->w_off_type;
    (* off_offset) = off_info_ptr->w_off_offset;

    off_info_cnt--;

}

char * type_from_code 

	(MrmType type_code)

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This procedure returns the string corresponding to a URM type code
 *
 *  FORMAL PARAMETERS:
 *
 *	type 		type code from RGMrType...
 *
 *  IMPLICIT INPUTS:
 *
 *      none
 *
 *  IMPLICIT OUTPUTS:
 *
 *      none
 *
 *  FUNCTION VALUE:
 *
 *	The string corresponding to the type code
 *
 *  SIDE EFFECTS:
 *
 *      none
 *
 *--
 */

{

    switch (type_code)
	{
	case MrmRtypeInteger:
	    return uil_datatype_names[sym_k_integer_value];
	case MrmRtypeHorizontalInteger:
	    return uil_datatype_names[sym_k_horizontal_integer_value];
	case MrmRtypeVerticalInteger:
	    return uil_datatype_names[sym_k_vertical_integer_value];
	case MrmRtypeBoolean:
	    return uil_datatype_names[sym_k_bool_value];
	case MrmRtypeChar8:
	    return uil_datatype_names[sym_k_char_8_value];
	case MrmRtypeChar8Vector:
	    return uil_datatype_names[sym_k_asciz_table_value];
	case MrmRtypeCString:
	    return uil_datatype_names[sym_k_compound_string_value];
	case MrmRtypeCStringVector:
	    return uil_datatype_names[sym_k_string_table_value];
	case MrmRtypeFloat:
	    return uil_datatype_names[sym_k_float_value];
	case MrmRtypeHorizontalFloat:
	    return uil_datatype_names[sym_k_horizontal_float_value];
	case MrmRtypeVerticalFloat:
	    return uil_datatype_names[sym_k_vertical_float_value];
	case MrmRtypeSingleFloat: /* single float data type RAP */
	    return uil_datatype_names[sym_k_single_float_value];
	case MrmRtypeCallback:
	    return "callback";
	case MrmRtypePixmapImage:
	    return "pixmap image";
	case MrmRtypePixmapDDIF:
	    return "pixmap DDIF";
	case MrmRtypeResource:
	    return "resource";
	case MrmRtypeAddrName:
	    return "addr name";
	case MrmRtypeIconImage:
	    return uil_datatype_names[sym_k_icon_value];
	case MrmRtypeFont:
	    return uil_datatype_names[sym_k_font_value];
	case MrmRtypeFontSet:
	    return uil_datatype_names[sym_k_fontset_value];
	case MrmRtypeFontList:
	    return uil_datatype_names[sym_k_font_table_value];
	case MrmRtypeColor:
	    return uil_datatype_names[sym_k_color_value];
	case MrmRtypeColorTable:
	    return uil_datatype_names[sym_k_color_table_value];
	case MrmRtypeAny:
	    return uil_datatype_names[sym_k_any_value];
	case MrmRtypeTransTable:
	    return uil_datatype_names[sym_k_trans_table_value];
	case MrmRtypeClassRecName:
	    return uil_datatype_names[sym_k_class_rec_name_value];
	case MrmRtypeIntegerVector:
	    return uil_datatype_names[sym_k_integer_table_value];
	case MrmRtypeXBitmapFile:
	    return uil_datatype_names[sym_k_xbitmapfile_value];
	  case MrmRtypeKeysym:
	    return uil_datatype_names[sym_k_keysym_value];
	default:
	    return "unknown";
    }

}

char * access_from_code 

	(MrmFlag access_code)

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This procedure returns the string corresponding to a URM access code
 *
 *  FORMAL PARAMETERS:
 *
 *	access_code		access code
 *
 *  IMPLICIT INPUTS:
 *
 *      none
 *
 *  IMPLICIT OUTPUTS:
 *
 *      none
 *
 *  FUNCTION VALUE:
 *
 *	The string corresponding to the access code
 *
 *  SIDE EFFECTS:
 *
 *      none
 *
 *--
 */

{

    switch (access_code) {
	case URMaPublic:	return "public"; 
	case URMaPrivate:	return "private"; 
	default:		return "unknown"; 
    }

}

char * group_from_code 

	(MrmGroup group_code)

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This procedure returns the string corresponding to a URM group code
 *
 *  FORMAL PARAMETERS:
 *
 *	group_code		group code
 *
 *  IMPLICIT INPUTS:
 *
 *      none
 *
 *  IMPLICIT OUTPUTS:
 *
 *      none
 *
 *  FUNCTION VALUE:
 *
 *	The string corresponding to the group code
 *
 *  SIDE EFFECTS:
 *
 *      none
 *
 *--
 */

{

    switch (group_code) {
	case URMgWidget:	return "widget"; 
	case URMgLiteral:	return "literal"; 
	case URMgResourceSet:	return "resource set"; 
	default:		return "unknown"; 
    }

}


void format_arg_value (argval_ptr, buffer)

RGMArgValuePtr		argval_ptr;
char			* buffer;

/*
 *++
 *
 *  FUNCTION PROTOTYPE
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine formats a value descriptor into the buffer.
 *
 *  FORMAL PARAMETERS:
 *
 *	argval_ptr	pointer to the argument value descriptor
 *	buffer		buffer to receive the formatted value
 *
 *  IMPLICIT INPUTS:
 *
 *
 *  IMPLICIT OUTPUTS:
 *
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *
 *--
 */

{

    switch ( argval_ptr->rep_type ) {

	case MrmRtypeInteger:
        case MrmRtypeHorizontalInteger:
        case MrmRtypeVerticalInteger:
	    sprintf (buffer, "value: %d", argval_ptr->datum.ival);
	    break;

	case MrmRtypeBoolean:
	    if (argval_ptr->datum.ival) {
		sprintf (buffer, "value: true");
	    } else {
		sprintf (buffer, "value: false");
	    }
	    break;

	case MrmRtypeChar8:
	case MrmRtypeCStringVector:
	case MrmRtypePixmapImage:
	case MrmRtypePixmapDDIF:
	case MrmRtypeCString:
	case MrmRtypeAddrName:
        case MrmRtypeKeysym:
	    sprintf (buffer, "offset: %X (hex)", argval_ptr->datum.offset);
	    off_put (k_unknown_off, argval_ptr->datum.offset);
	    break;

	case MrmRtypeFloat:
        case MrmRtypeHorizontalFloat:
        case MrmRtypeVerticalFloat:
	    sprintf (buffer, "offset: %X (hex)", argval_ptr->datum.offset);
	    off_put (k_float_off, argval_ptr->datum.offset);
	    break;

	  case MrmRtypeSingleFloat: /* single float data type RAP */
	    sprintf (buffer, "offset: %X (hex)", argval_ptr->datum.offset);
            off_put (k_single_float_off, argval_ptr->datum.offset);
            break;

	case MrmRtypeCallback:
	    sprintf (buffer, "offset: %X (hex)", argval_ptr->datum.offset);
	    off_put (k_callback_off, argval_ptr->datum.offset);
	    break;

	case MrmRtypeResource:
	    sprintf (buffer, "offset: %X (hex)", argval_ptr->datum.offset);
	    off_put (k_resource_off, argval_ptr->datum.offset);
	    break;

	default:
	    sprintf (buffer, "value unknown");
	    break;
    }

}


char * class_name_from_code 

	(MrmCode class)

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This procedure returns the string corresponding to a URM class code
 *
 *  FORMAL PARAMETERS:
 *
 *	class		class code from URMwc...
 *
 *  IMPLICIT INPUTS:
 *
 *      none
 *
 *  IMPLICIT OUTPUTS:
 *
 *      none
 *
 *  FUNCTION VALUE:
 *
 *	The string corresponding to the class code
 *
 *  SIDE EFFECTS:
 *
 *      none
 *
 *--
 */

{

int		sym_code;


/*
 * Find the sym_k code for the class, then return its name
 */
for ( sym_code=sym_k_error_object+1;
      sym_code<=uil_max_object;
      sym_code++ )
    if ( uil_widget_compr[sym_code] == class )
	return uil_widget_names[sym_code];
return "unknown";

}


char * resource_name_from_code 

	(MrmCode resource)

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This procedure returns the string corresponding to a compression code
 *
 *  FORMAL PARAMETERS:
 *
 *	resource	compression code for argument or reason
 *
 *  IMPLICIT INPUTS:
 *
 *      none
 *
 *  IMPLICIT OUTPUTS:
 *
 *      none
 *
 *  FUNCTION VALUE:
 *
 *	The string corresponding to the resource code
 *
 *  SIDE EFFECTS:
 *
 *      none
 *
 *--
 */

{

int		sym_code;


/*
 * Find the sym_k code for the resource, then return its name
 */
for ( sym_code=0 ; sym_code<=uil_max_arg ; sym_code++ )
    if ( uil_arg_compr[sym_code] == resource )
	return uil_argument_names[sym_code];
for ( sym_code=0 ; sym_code<=uil_max_reason ; sym_code++ )
    if ( uil_reas_compr[sym_code] == resource )
	return uil_reason_names[sym_code];
for ( sym_code=0 ; sym_code<=uil_max_child ; sym_code++ )
    if ( uil_child_compr[sym_code] == resource )
	return uil_child_names[sym_code];
return "unknown";

}

