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
/*   $XConsortium: UilSymDef.h /main/12 1995/07/14 09:38:52 drk $ */

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
**      This include file defines the interface to the UIL symbol table.
**
**--
**/

#ifndef UilSymDef_h
#define UilSymDef_h

/*
**
**  INCLUDE FILES
**
**/

#include <Mrm/MrmPublic.h>
#include <Xm/Xm.h>

/*
**  constraint check access macro
*/
#define _constraint_check(arg) \
 (constraint_tab[((arg)-1)/8] & (1 << ((arg)-1)%8))

/*
**  Hash table size
*/

/* MOVED TO DBDef.h */

/* 
**  Symbol tags.  These values are used in the tag field of every object header
**  to identify the type of the entry.
*/

/* MOVED TO DBDef.h */


/*
**    Common attribute masks -- These values are used in the b_flags field of
**    the object_header.
*/

/* MOVED TO DBDef.h */


/*
**	Output states - order is important
*/

/* MOVED TO DBDef.h */



/*
**	Constants to define compiler-recognized data types. It is
**	important that the names used in .wml language descriptions
**	match these names. These values were once automatically generated
**	by WML. They are now maintained by hand to reduce compiler
**	dependence on WML artifacts.
*/
/* MOVED TO DBDef.h */

 

/* 
**  Header for each symbol entry
*/

typedef struct
{
    char                b_tag;
	/* type of entry The values of this field ia a sym_k_XXX_entry as   */
	/* defined above.						    */
    char		b_type;
	/*  generic subtype field usage depends on value of b_tag field	    */
	/*  above (sym_k_value_entry -> b_type is a sym_k_XXX_value as	    */
	/*  defined in UilSymGen.h, sym_k_list_entry -> b_type is a	    */
	/*  sym_k_XXX_list as defined below, sym_k_widget_entry -> b_type   */
	/*  is a sym_k_XXX_object as defined in UilSymGen.h, 		    */
	/*  sym_k_child_entry -> b_type is a sym_k_XXX_child as defined in  */
        /*  UilSymGen.h, sym_k_section_entry -> b_type is a 		    */
	/*  sym_k_XXX_section as defined below).			    */
	/*								    */
    unsigned short	w_node_size;
	/*  size of the node in longwords				    */
    int			user_data;
	/* not used directly by the compiler.  It is for use by		    */
	/* applications making use of callable UIL only.		    */

    /*
    ** Fields to save source information about the symbol:  We currently save
    ** five things.  The source record where the symbol "begins" (begins is in
    ** quotes because this may point to just white space before the actual
    ** symbol), the position (character) within that record where the symbol
    ** begins, the source record where the symbol ends, the position within
    ** that record that symbol really begins (i.e. no white space or anything),
    ** and the position where the symbol definition ends.
    */

    struct _src_source_record_type  *az_src_rec;
	/* Source record where symbol defined			    */
    unsigned char	b_src_pos;
	/* Starting position within the source record		    */
    unsigned char	b_end_pos;
	/* Ending position within the source record		    */

} sym_entry_header_type;



/*
**  Common view of a symbol entry
*/

typedef struct  
{
    sym_entry_header_type   header;
	/* common part of all symbol entries.				    */
    char                    b_value[ 1 ];
	/* information specific to each different entry type.		    */
} sym_entry_type;



/*
**  Name entry 
*/

/*	Flags for name entries						    */

#define		sym_m_referenced 	(1 << 0)
	/* Used in the b_flags field of a name entry.  Set if this name is   */
	/* reference elsewhere in this UIL source module.		    */
#define		sym_m_cycle_checked	(1 << 1)
	/* set if the widget entry for this name has been checked for	*/
	/* cyclic references.						*/
#define		sym_m_has_cycle		(2 << 1)
	/* set if a cycle was detected for a widget entry		*/
#define		sym_m_charset		(1 << 3)
       /* set if symbol being used as charset name. */

typedef struct _sym_name_entry_type
{
    sym_entry_header_type	header;
	/* common header						*/
    sym_entry_type 		*az_object;
	/* pointer to the object entry with this name			*/
    struct _sym_name_entry_type	*az_next_name_entry;
	/* next name on hash chain					*/
    struct _sym_name_entry_type *az_prev_name_entry;
	/* prev name on hash chain					*/
    int				az_cycle_id;
	/* unique id for each cycle check				*/
    unsigned char		b_flags;
	/* flags (possible value is sym_m_referenced as declared above)	*/
    char			c_text[ 1 ];
	/* text of the name						*/
} sym_name_entry_type;

/*
**  Size of the fixed part of a name entry
*/

#define sym_k_name_entry_size (sizeof( sym_name_entry_type)- \
			       sizeof( char[ 1 ]) )


/*
**  Value entry 
*/

/*
**  Types of values are included in UilSymDef.h
*/

/*
**  Charsets supported by UIL are defined via WML in UilSymGen.h
*/



/* 
**  Common header for each "object" entry.  This header is shared by all
**  widget, gadget, list, argument, callback, and control node types.  It
**  always appears immediately following the standard entry header field.  Also
**  defined here is a prototype node for these types of "object" entries.
*/

typedef struct
{
    sym_name_entry_type		* az_name;
	/* pointer to the name entry that holds the object name.	  */
    sym_entry_type		* az_reference;
	/* pointer to the entry that references this object.		  */
    sym_entry_type		* az_next;
	/* pointer to next entry in a list, if this entry is linked under */
        /* a list entry.						  */
    char                    	* az_comment;
	/* pointer to comment entry associated with this object.          */
    unsigned int		b_flags;
	/* object flags.						  */
} sym_obj_header_type;

typedef struct
{
    sym_entry_header_type	header;
	/* common entry header						  */
    sym_obj_header_type		obj_header;
	/* common object header						  */
} sym_obj_entry_type;



/*
**  This structure defines an element in a color table.  A color table contains
**  an array of such elements.
*/

typedef struct	
{
    unsigned char	b_letter;
	/* letter for this color					  */
    unsigned char	b_index;
	/* index for this color						  */
    unsigned short	w_desc_offset;
	/* descriptor offset at in context				  */
    struct _sym_value_entry_type    *az_color;
	/* pointer to the value entry for this color			  */

} sym_color_element;


/*
**  This structure defines the data for an icon.
*/

typedef struct	
{
    unsigned short	w_height;
	/* height in pixels of the icon					  */
    unsigned short	w_width;
	/* width in pixels of the icon					  */
    struct _sym_value_entry_type    *az_color_table;
	/* pointer to the value entry for the color table		  */
    struct _sym_value_entry_type    *az_rows;
	/* pointer to the list of icon rows				  */

} sym_icon_element;

/*
**  This structure define an enumeration set entry
*/

/* MOVED TO DBDef.h */


/*
**	Auxiliary sym_k_flags for values stored in b_aux_flags field
*/

#define		sym_m_table_entry	(1 << 0)
    /* This item is a component of table				  */
#define		sym_m_separate		(1 << 2)
    /* add CDA separator to end of string				  */
#define		sym_m_sixteen_bit	sym_m_separate
    /* character set is 16-bit						  */
#define		sym_m_exp_eval		(1 << 3)
    /* if set, indicates that expression has already been evaluated	  */

/*
**	Types of colors - stored in b_arg_type
*/

#define	sym_k_unspecified_color	0
#define	sym_k_background_color	1
       /* This color should displayed in the background on a monochrome	  */
       /* display.							  */
#define	sym_k_foreground_color	2
       /* This color should displayed in the foreground on a monochrome	  */
       /* display.							  */

/*
**	value sets defining expression operators
*/

/* MOVED TO DBDef.h */

typedef struct _sym_value_entry_type
{
    sym_entry_header_type   header;
	/* common header						  */
    sym_obj_header_type     obj_header;
	/* common header for objects					  */
    unsigned char	    b_type;
	/* b_type distinguishes usage.  Possible values are sym_k_XXX_value */
	/* as defined in UilSymGen.h					  */
    unsigned short          w_length;
	/* length for variable length types				  */
    char		    output_state;
	/* output_state (only byte is needed)				  */
    unsigned char	    b_table_count;
	/* number of strings in table, colors in a color table, or fonts in */
	/* a font table.						  */
    unsigned char	    b_aux_flags;
	/* auxiliary flags						  */
    unsigned char	    b_arg_type;
	/* type of argument value or monochrome treatment of a color	  */
    unsigned char	    b_data_offset;
	/* offset of bits in context for an icon			  */
    unsigned char	    b_pixel_type;
	/* pixel size encoding of an icon				  */
    unsigned char	    b_charset;
	/* character set of a string					  */
    unsigned char	    b_direction;
	/* writing direction of a string, from XmSTRING_DIRECTION_...	  */
    unsigned char	    b_max_index;
	/* max index in a color table					  */
    unsigned char	    b_expr_opr;
	/* operator for an expression value node			  */
    unsigned int	    l_circular_ref_chk;
	/* field to mark when checking for circular references		  */
    unsigned short int	    b_enumeration_value_code;
        /* if non-0, the node takes its value from the value of the       */
        /* sym_k_..._enumval code in this field (currently integer)       */
    MrmResource_id    	resource_id; 
	/* 
resource id for private values.				  */

/* Structure for different kinds of nodes pointing to other value nodes	  */
    struct _sym_value_entry_type    * az_charset_value;
        /* pointer to value entry describing a userdefined charset. */
        /* Valid only when b_charset == sym_k_userdefined_charset   */
    struct _sym_value_entry_type    * az_next_table_value;
	/* next element for tables.				    */
    struct _sym_value_entry_type    *az_first_table_value;
        /* pointer to first table entry (font, string, etc.) if     */
        /* value is a table.					    */
    struct _sym_value_entry_type	* az_exp_op1;
    struct _sym_value_entry_type	* az_exp_op2;
        /* pointer to the one or two value operands of an	    */
        /* expression value node. Special operands have an          */
        /* explicitly state semantic.                               */

/* Union for the actual value of a value node				*/
    union _sym_value_entry_value_union
    {
	/*  must be capable of holding a pointer, chs */
	long		l_integer;
	    /* value if an integer.					*/
        double		d_real;
	    /* value if a double float.					*/
	char		*c_value;
	    /* value of ASCIZ string		  			*/
	XmString	xms_value;
	   /* value of XmString						*/
	float           single_float;
	    /* value if a single float					*/
	sym_color_element	*z_color;	
	    /* value if color table.					*/
	sym_icon_element	*z_icon;
	    /* value if icon						*/
	char		*az_data;
	    /* pointer to any other kind of allocated data		*/
    } value;
} sym_value_entry_type;


/*
**  Define the fixed size of the value entry structure
*/   

#define sym_k_value_entry_size (sizeof(sym_value_entry_type))


/*
**  List entry
*/

/*
**  The types of lists, values for the header.b_type field of a list entry.
*/

#define 	sym_k_error_list		sym_k_error_entry
#define 	sym_k_callback_list		sym_k_callback_entry
#define 	sym_k_argument_list		sym_k_argument_entry
#define 	sym_k_control_list		sym_k_control_entry
#define		sym_k_proc_ref_list		sym_k_proc_ref_entry

typedef struct _sym_list_entry_type
{
    sym_entry_header_type   header;
	/* common header						    */
    sym_obj_header_type     obj_header;
	/* common header for objects					    */
    unsigned short	    w_count;
	/* number of elements in the list.				    */
    unsigned short	    w_gadget_count;
	/* number of gadgets in the list.				    */
} sym_list_entry_type;

/*
**  Define the size of the list structure
*/   

#define sym_k_list_entry_size (sizeof(sym_list_entry_type))



/*
**  Nested List entry
*/

/*
**  A nested list entry appears in a list as the entry specifying a reference
**  to another list (a nested list reference). It has its b_type field set
**  to the list type (necessarily the same type as the list it is contained
**  in, and the list it points to). The az_next field locates the next list
**  element, as usual. The az_list field points to the list entry for the
**  list which is referenced.
*/

typedef struct _sym_nested_list_entry_type
{
    sym_entry_header_type   header;
	/* common header						    */
    sym_obj_header_type     obj_header;
	/* common header for objects					    */
    sym_list_entry_type	    *az_list;
	/* pointer to the referenced list                                   */
} sym_nested_list_entry_type;

/*
**  Define the size of the nested_list structure
*/   

#define sym_k_nested_list_entry_size (sizeof(sym_nested_list_entry_type))



/*
**  Argument entry
*/

typedef struct _sym_argument_entry_type
{
    sym_entry_header_type	header;
	/* common entry header						    */
    sym_obj_header_type		obj_header;
	/* common object header						    */
    sym_value_entry_type	* az_arg_name;
	/* pointer to the value entry containing the argument name.	    */
    sym_value_entry_type	* az_arg_value;
	/* pointer to the value entry containing the argument value.	    */
} sym_argument_entry_type;

#define sym_k_argument_entry_size (sizeof(sym_argument_entry_type))


/*
**  Procedure Definition Entry
*/

typedef struct
{
    sym_entry_header_type	header;
	/* common header.						    */
    sym_obj_header_type		obj_header;
	/* common header for objects					    */
    unsigned char		v_arg_checking;
	/* should argument type be checked.				    */
    unsigned char		b_arg_count;
	/* number of arguments.						    */
    unsigned char		b_arg_type;
	/* The type of single argument (tag) for this procedure.  It should */
	/* be a sym_k_XXX_value as defined in UilDBDef.h.		    */
    unsigned char		b_widget_type;
       /* If b_arg_type is sym_k_widget_ref_value, then this field should   */
       /* be a sym_k_XXX_object as defined in UilSymGen.h or 		    */
       /* (uil_max_object + 1) if the type is unspecified.		    */
} sym_proc_def_entry_type;

#define sym_k_proc_def_entry_size (sizeof( sym_proc_def_entry_type))


/*
**  Procedure Reference Entry
*/

typedef struct
{
    sym_entry_header_type	header;
	/* common header.						    */
    sym_obj_header_type		obj_header;
	/* common object header.					    */
    sym_proc_def_entry_type	*az_proc_def;
	/* pointer to the procedure definition entry for the procedure to   */
	/* be called.							    */
    sym_value_entry_type	*az_arg_value;
	/* pointer to the value entry for the argument to be passed as the  */
	/* callback tag to the procedure.				    */
} sym_proc_ref_entry_type;

#define sym_k_proc_ref_entry_size (sizeof( sym_proc_ref_entry_type))


/*
**  These values are passed from the grammar to the semantic action routines
**  (SAR) to identify how the procedure is used.
*/
#define sym_k_callback_proc	1
    /*	The procedure was used as a callback routine.			    */
#define sym_k_object_proc	2
    /*	The procedure was used as a creation routine for a user defined	    */
    /*	widget								    */


/*
**  Callback entry
*/

typedef struct _sym_callback_entry_type
{
    sym_entry_header_type	header;
	/* common entry header.						    */
    sym_obj_header_type		obj_header;
	/* common object header.					    */
    sym_value_entry_type	* az_call_reason_name;
	/* pointer to the value entry containing the reason (callback) name */
    sym_proc_ref_entry_type	* az_call_proc_ref;
	/* pointer to the procedure reference to be invoked for this	    */
	/* callback reason.						    */
    sym_list_entry_type		* az_call_proc_ref_list;
	/* list entry of procedure references when multiple callbacks are   */
	/* used for the callback.					    */
} sym_callback_entry_type;

#define sym_k_callback_entry_size (sizeof(sym_callback_entry_type))


/*
**  Parent list for widgets and gadgets
**
**  This list contains pointers to each of the widgets and gadgets that
**  reference the current widget in their control list.  This information is
**  required in order to check constraint arguments.
*/

typedef struct _sym_parent_list_type
{
    sym_entry_header_type           header;
	/* common entry header						    */
    struct _sym_widget_entry_type   * parent;
	/* pointer to a widget entry that references the originating widget */
	/* in its controls list.					    */
    struct _sym_parent_list_type    * next;
	/* pointer to next parent list entry.				    */
} sym_parent_list_type;

#define sym_k_parent_list_size (sizeof(sym_parent_list_type))



/*
**  Widget entry and Gadget entry
**
**	Gadgets have the same form as a widget.  They are distinguished
**	simply by the setting of sym_m_obj_is_gadget in obj_header.b_flags .
*/



/*    widget and gadget node structure.					    */

typedef struct _sym_widget_entry_type
{
    sym_entry_header_type	header;
	/* common entry header						    */
    sym_obj_header_type		obj_header;
	/* common object header						    */
    sym_list_entry_type		* az_callbacks;
	/* pointer to a list entry of type sym_k_callback_list which	    */
	/* contains the callbacks for this widget.			    */
    sym_list_entry_type		* az_arguments;
	/* pointer to a list entry of type sym_k_argument_list which	    */
	/* contains the arguments for this widget.			    */
    sym_list_entry_type		* az_controls;
	/* pointer to a list entry of type sym_k_controls_list which	    */
	/* contains the widgets that are children (are controlled by) this  */
	/* widget.							    */
    sym_proc_ref_entry_type	* az_create_proc;
	/* for user_defined widgets this points to the procedure reference  */
	/* entry that specifies the creation routine for the widget.	    */
    sym_parent_list_type	* parent_list;
	/* pointer to a parent list entry that links all objects which	    */
	/* reference this object in there controls lists.  It is used to    */
	/* traverse through the parents to verify that all contraint	    */
	/* arguments specified on this widget are really inherited from one */
	/* of its parents.						    */
    int				output_state;
	/* Used when generating the UID file.  Possible values:		    */
	/* sym_k_queued, sym_k_emitted, sym_k_not_processed. (only a byte   */
	/* is needed).							    */
    MrmResource_id			resource_id;
	/* The resource id if the object is private.  This is used when	    */
	/* generating the UID file.					    */
} sym_widget_entry_type;

#define sym_k_widget_entry_size (sizeof(sym_widget_entry_type))



/*
**  Control entry
*/

/*
**  Control flags -- These are additional values that may occur in the b_flags
**  field for control entries. NOTE: Must not overlap common flags defined
**  above.
*/	

#define	sym_m_def_in_progress	(1 << 6)
    /*	used in all lists.						    */
#define	sym_m_managed		(1 << 7)
    /*	This bit is set when the controlled object is to be managed.	    */
#define sym_m_obj_is_reference  (1 << 8)
#define sym_m_forward_ref	(1 << 9)
    /*  This bit is set if the item is a forward reference not yet resolved*/
#define sym_m_validated		(1 << 10)
    /*	Used for widgets to indicate if it has been validated */


/*    Control node structure.						    */

typedef struct _sym_control_entry_type
{
    sym_entry_header_type	header;
	/* common entry header						    */
    sym_obj_header_type		obj_header;
	/* common object header						    */
    sym_widget_entry_type	* az_con_obj;
	/* pointer to the widget entry that is controlled by this widget.   */
} sym_control_entry_type;

#define sym_k_control_entry_size (sizeof(sym_control_entry_type))



/*
**  External Definition Entry
*/

typedef struct _sym_external_def_entry_type
{
    sym_entry_header_type	header;	
	/* common header						    */
    struct _sym_external_def_entry_type	*az_next_object;
	/* pointer next external definition entry.			    */
    sym_name_entry_type		*az_name;
	/* pointer to the name entry for the external object.		    */
} sym_external_def_entry_type;

#define sym_k_external_def_entry_size (sizeof( sym_external_def_entry_type))


/*
**  Forward Reference Entry
*/

typedef struct _sym_forward_ref_entry_type
{
    sym_entry_header_type	header;
	/* common header.  The b_type field holds the object type.	    */
    struct _sym_forward_ref_entry_type	*az_next_ref;
	/* pointer to the next forward reference entry.			    */
    char			*a_update_location;
	/* address of the longword field in the referencing entry that	    */
	/* needs to be updated with the address of the widget entry that    */
	/* this forward reference entry was created for.  When this forward */
	/* reference entry is processed in UilP2Reslv.c, then the widget    */
	/* named in the az_name field will be defined and it can be	    */
	/* assigned directly into the a_update_location.		    */
    sym_name_entry_type		*az_name;
	/* pointer to the name entry for the widget being referenced before */
	/* it has been defined.						    */
    sym_widget_entry_type	*parent;
	/* pointer to the widget entry of the parent of the forward	    */
	/* referenced object.						    */
} sym_forward_ref_entry_type;

#define sym_k_forward_ref_entry_size (sizeof( sym_forward_ref_entry_type))


/*
**  Forward Reference Entry
*/

#define sym_k_patch_add		1
#define sym_k_bind_value_name	2
#define sym_k_patch_list_add	3

typedef struct _sym_val_forward_ref_entry_type
{
    sym_entry_header_type	header;
	/* common header.  The b_type field holds the expected value type.  */
    struct _sym_val_forward_ref_entry_type	*az_next_ref;
	/* pointer to the next forward reference entry.			    */
    char			*a_update_location;
	/* address of the longword field in the referencing entry that	    */
	/* needs to be updated with the address of the value entry that     */
	/* this forward reference entry was created for.  When this forward */
	/* reference entry is processed in UilP2Reslv.c, then the value     */
	/* named in the az_name field will be defined and it can be	    */
	/* assigned directly into the a_update_location.		    */
    sym_name_entry_type		*az_name;
	/* pointer to the name entry for the value being referenced before  */
	/* it has been defined.						    */
    unsigned char		fwd_ref_flags;
	/* flags used to indicate how to process entry on forward reference */
	/* chain.							    */
} sym_val_forward_ref_entry_type;

#define sym_k_val_forward_ref_entry_size (sizeof( sym_val_forward_ref_entry_type))


/*
**  Default object entry.  Pointed to from the module entry.  The first one in
**  the list contains source information for the "OBJECTS = " text.  The 'next'
**  field is a pointer to a list which contains the data on the actual clauses.
*/

typedef struct _sym_def_obj_entry_type
{
    sym_entry_header_type	header;
	/* common header						    */
    struct _sym_def_obj_entry_type  *next;
	/* pointer to next default object entry in the list.		    */
    char			b_object_info;
    char			b_variant_info;
} sym_def_obj_entry_type;

#define sym_k_def_obj_entry_size (sizeof( sym_def_obj_entry_type))



/*
**  Module entry (only 1 per compilation)
*/

typedef struct _sym_module_entry_type
{
    sym_entry_header_type	header;
	/* common header						    */
    sym_obj_header_type		obj_header;
	/* common header for objects					    */
    sym_value_entry_type 	*az_version;
	/* pointer to the value entry containing version string for the	    */
	/* module (version clause).					    */
    sym_value_entry_type	*az_character_set;
	/* pointer to the value entry containing the default character set  */
	/* information for the module (character set clause).		    */
    sym_value_entry_type	*az_case_sense;
	/* pointer to the value entry containing the value of the module    */
	/* case sensitivity clause.					    */
    sym_def_obj_entry_type	*az_def_obj;
	/* pointer to the default object type specified in the module	    */
	/* default object variant clause.				    */
} sym_module_entry_type;

#define sym_k_module_entry_size (sizeof( sym_module_entry_type))


/*
**	Section entry
*/

/*
** Posiblilities for header.b_type when header.b_tag is sym_k_section_entry.
*/

#define sym_k_list_section	1
	/* list section							    */
#define sym_k_procedure_section	2
	/* procedure section						    */
#define sym_k_value_section	3
	/* value section						    */
#define sym_k_identifier_section 4
	/* identifier section						    */
#define sym_k_object_section	5
	/* widget/gadget section					    */
#define sym_k_include_section	6
	/* include directive						    */
#define sym_k_section_tail	7
	/* This section has no corresponding UIL construct and exists only  */
	/* to aid in saving source information				    */



typedef struct _sym_section_entry_type
{
    sym_entry_header_type	header;
	/* common header						    */
    struct _sym_section_entry_type *prev_section;
	/* previous section "LIST".					    */
    sym_entry_type		*next;
	/* pointer to the entry for the next section _OR_ next child.	    */
    sym_entry_type		*entries;
	/* points to entries in the section _OR_ the entry in the symbol    */
	/* table for this child.					    */
} sym_section_entry_type;

#define sym_k_section_entry_size (sizeof (sym_section_entry_type))


/*
**  Include file entry
*/

typedef struct _sym_include_file_entry_type
{
    sym_entry_header_type	header;
	/* common header						    */
    sym_section_entry_type	*sections;
	/* pointer to a section list; this list is all of the sections that */
	/* exist in this include file.					    */
    char			file_name[255];
	/* the file name as specified in the include statement in the UIL   */
	/* source.							    */
    char			full_file_name[255];
	/* the expanded name for the include file actually opened.	    */
} sym_include_file_entry_type;

#define sym_k_include_file_entry_size (sizeof( sym_include_file_entry_type))


/*
**  Symbol table root entry (root of symbol table tree)
*/

typedef struct _sym_root_entry_type
{
    sym_entry_header_type	header;
	/* common header						    */
    struct _src_source_record_type	*src_record_list;
	/* pointer to a list of source records.				    */
    char			file_name[255];
	/* the main UIL file name as specified on the command line.	    */
    char			full_file_name[255];
	/* the expanded name for the main UIL file that was actually	    */
	/* opened.							    */
    sym_section_entry_type	*sections;
	/* pointer to section list entry.				    */
    sym_module_entry_type	*module_hdr;
	/* pointer to module header entry.				    */
    
} sym_root_entry_type;

#define sym_k_root_entry_size (sizeof( sym_root_entry_type))


/*
**  Color Item Entry
*/

typedef struct _sym_color_item_entry_type
{
    sym_entry_header_type	header;
	/* common header						    */
    struct _sym_color_item_entry_type	*az_next;
	/* next item							    */
    unsigned char		b_letter;
	/* letter used for color					    */
    unsigned char		b_index;
	/* color table index						    */
    unsigned char		b_spare1;
    sym_value_entry_type	*az_color;
	/* name or value entry for the color.				    */
	
} sym_color_item_entry_type;

#define sym_k_color_item_entry_size (sizeof( sym_color_item_entry_type))



/*
**  This macro copies the contents of one entry to another without disturbing
**  the the linked list of entries used to connect all symbol table entries.
*/
#define _sym_copy_entry(__dest, __src, __size) _move (__dest, __src, __size)


#endif /* UilSymDef_h */
/* DON'T ADD STUFF AFTER THIS #endif */
