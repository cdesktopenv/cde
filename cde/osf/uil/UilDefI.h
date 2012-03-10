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
/*   $TOG: UilDefI.h /main/13 1999/07/07 10:24:15 vipin $ */

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
**      This include file defines the set of definitions used by all of
**	modules of the UIL compiler.
**
**--
**/

#ifndef UilDefI_h
#define UilDefI_h


/*
**
**  INCLUDE FILES
**
**/

#define X_INCLUDE_TIME_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>		/* for _Xctimeparams */

#include <Mrm/MrmAppl.h>
#include <Mrm/Mrm.h>

/*
**                                    
**  Universal Definitions
**
*/

#define _compiler_version "V2.0-000"
#define _compiler_version_int 2
#define _data_version 2
#define _host_compiler "Motif Uil Compiler"

#ifdef TRUE
#undef TRUE
#endif
#define TRUE	 	1

#ifdef FALSE
#undef FALSE
#endif
#define FALSE	  	0

#ifndef debug_version
#define debug_version	FALSE
#endif

#define NOSTRING_DIRECTION 2

#define k_normal  	1
#define k_error   	0

typedef int status;
typedef int boolean;

/*
**  Copy const handling from XmP.h.
*/

#ifndef XmConst
#if defined(__STDC__) || !defined( NO_CONST )
#define XmConst const
#else
#define XmConst
#endif /* __STDC__ */
#endif /* XmConst */

/* Uil will automatically strip this prefix when saving name of automatically
 * created child.  This is used to prevent name conflicts in existing uil 
 * files.  Children names without this prefix will be saved as is.
 */
#define AUTO_CHILD_PREFIX "Xm_"

/*
**
**  Constants controlling limits in the compiler
**
*/

#define src_k_max_source_files		101
#define src_k_max_source_line_length	132
#define lex_k_unprint_sub		'?'
#define sym_k_max_list_count		1000
#define sym_k_max_color_count		256
#define Uil_k_max_include_dir_count	32

/*
**
**  Definition of Compiler Termination Statuses
**
*/

#define uil_k_min_status	Uil_k_min_status
#define uil_k_success_status	Uil_k_success_status
#define uil_k_info_status	Uil_k_info_status
#define uil_k_warning_status	Uil_k_warning_status
#define uil_k_error_status	Uil_k_error_status
#define uil_k_severe_status	Uil_k_severe_status
#define uil_k_max_status	Uil_k_max_status


/*
**
**  Macros to access bit vectors
**
*/

/* MOVED TO DBDef.h */


/*
**
**  Upper and Lower Case Translation Macros
**
*/

#define _upper(c)	((c) >= 'a' && (c) <= 'z' ? (c) & 0x5F:(c))
#define _lower(c)	((c) >= 'A' && (c) <= 'Z' ? (c) | 0x20:(c))


/*
**
**  Assert Macro
**
*/

#if debug_version
#define _assert( __condition, __text )			\
	if (!(__condition))				\
	{  diag_issue_internal_error( (__text)); }
#else
#define _assert( __condition, __text )			\
	if (!(__condition))				\
	{  diag_issue_internal_error( NULL ); }
#endif

/*
**
**  _error and _okay macro
**
*/

#define _error( __status )	\
	((__status & 1)==0)

#define _success( __status )	\
	((__status & 1)==1)


/*
**
**  Memory allocation definitions
**
*/

#define _get_memory XtMalloc

#define _free_memory XtFree

/*
**
**  Move macro
**
*/

#ifdef NO_MEMMOVE
/* For fixing the bug CR 4581 */
extern char* uil_mmove(char*, char*, int);

#define _move uil_mmove
#else
#define _move memmove
#endif /* NO_MEMMOVE */

/*
**
**  Compare macro
**
*/

#define _compare strcmp

/*
**
**  Fill macro
**
*/

#define _fill( __string, __char, __len )	\
    {						\
      register int i = __len; 			\
      while (--i >= 0)				\
	(__string)[i] = __char;			\
    }

/*
**
**  Index macro
**
*/

#define _index( __char, __string, __len ) \
    ((int)((char *)memchr( __string, __char, __len ) - (char *)__string ))

/*
**
**  Debug output macro
**
*/
#if debug_version
#define _debug_output lst_debug_output
#else
#define _debug_output printf
#endif


/*
**
** Common includes needed by most modules
**
*/
#include "Uil.h"
#include "UilDBDef.h"                      /* for binary database */
#include "UilMess.h"
#include "UilSymDef.h"			/* must come before UilKeyDef.h */
#include "UilSrcDef.h"			/* must come before UilIODef.h */
#include "UilIODef.h"
#include "UilDiagDef.h"
#include "UilSarDef.h"
#if defined(linux)
#define YYSTYPE yystype
#endif
#include "UilLexPars.h"
#include "UilCmdDef.h"
#include "UilLexDef.h"

/*
** Global declarations
*/
#include "UilSymGl.h"
#include "UilCompGl.h"

/* Needed for following macro. */
#include "UilSymGen.h"

#define DEFAULT_TAG uil_charset_names[sym_k_XmFONTLIST_DEFAULT_TAG_charset]

/*
** Function declarations not defined elsewhere
*/
#define _ARGUMENTS(arglist) arglist

/* uilcmd.c */

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern void diag_issue_diagnostic  
	_ARGUMENTS(( int d_message_number , 
		    src_source_record_type *az_src_rec ,
		    int l_start_column, ... ));
extern void lst_debug_output  _ARGUMENTS(( char *format, ... ));


void cmd_decode_command_line  _ARGUMENTS(( int l_arg_count , char *rac_arg_value []));

/* uildb.c */
extern void db_incorporate _ARGUMENTS(( void ));
extern void db_read_ints_and_string _ARGUMENTS((_db_header_ptr header));
extern void db_read_char_table _ARGUMENTS((_db_header_ptr header));
extern void db_read_length_and_string _ARGUMENTS((_db_header_ptr header));
extern void db_read_int_and_shorts _ARGUMENTS((_db_header_ptr header));
extern void db_open_file _ARGUMENTS(( void ));
extern String get_root_dir_name _ARGUMENTS(( void ));
extern String init_wmd_path _ARGUMENTS((String filename));

/* uildiags.c */
extern void diag_issue_summary  _ARGUMENTS(( void ));
extern char *diag_tag_text  _ARGUMENTS(( int XmConst b_tag ));
extern char *diag_object_text  _ARGUMENTS(( int XmConst b_type ));
extern char *diag_value_text  _ARGUMENTS(( int XmConst b_type ));
extern char *diag_charset_text  _ARGUMENTS(( int XmConst b_type ));
extern void diag_initialize_diagnostics  _ARGUMENTS(( void ));
extern void diag_restore_diagnostics  _ARGUMENTS(( void ));
extern void diag_reset_overflow_handler  _ARGUMENTS(( void ));
extern void diag_handler  _ARGUMENTS(( int l_error ));
extern void diag_issue_internal_error  _ARGUMENTS(( char *error_text ));
extern void write_msg_to_standard_error  _ARGUMENTS(( XmConst int message_number , XmConst char *src_buffer , XmConst char *ptr_buffer , XmConst char *msg_buffer , XmConst char *loc_buffer ));
extern char XmConst *diag_get_message_abbrev  _ARGUMENTS(( int d_message_number ));
extern void diag_report_status  _ARGUMENTS(( void ));

/* uilkeytab.c */
extern key_keytable_entry_type *key_find_keyword  _ARGUMENTS(( unsigned int symbol_length , char *symbol_ptr ));
extern void key_initialize  _ARGUMENTS(( void ));

/* uillexana.c */
extern int yylex  _ARGUMENTS(( void ));
extern void lex_initialize_analyzer  _ARGUMENTS(( void ));
extern void Uil_lex_cleanup_analyzer  _ARGUMENTS(( void ));
extern void lex_issue_error  _ARGUMENTS(( int restart_token ));
extern void issue_control_char_diagnostic  _ARGUMENTS(( unsigned char c_char ));
extern void lex_filter_unprintable_chars  _ARGUMENTS(( unsigned char *buffer , int length , unsigned long flags ));
extern long cvt_ascii_to_long  _ARGUMENTS(( unsigned char XmConst *c_text ));
extern sym_value_entry_type *create_str_entry  _ARGUMENTS(( int l_size , int l_charset , sym_value_entry_type *az_charset_entry ));

/* uillstlst.c */
extern void lst_open_listing  _ARGUMENTS(( void ));
extern void Uil_lst_cleanup_listing  _ARGUMENTS(( void ));
extern status create_listing_file  _ARGUMENTS(( uil_fcb_type *az_fcb ));
extern void lst_output_line  _ARGUMENTS(( char *ac_line , boolean v_new_page ));
extern char *current_time  _ARGUMENTS(( _Xctimeparams *ctime_buf ));
extern void lst_output_listing  _ARGUMENTS(( void ));
extern void lst_output_messages  _ARGUMENTS(( src_message_item_type *az_message_item ));
extern void lst_output_machine_code  _ARGUMENTS(( src_source_record_type *az_src_rec ));
extern void lst_output_message_ptr_line  _ARGUMENTS(( src_source_record_type *az_src_rec , char *src_buffer ));

/* uillstmac.c */
extern void save_widget_machine_code  _ARGUMENTS(( sym_widget_entry_type *widget_entry , URMResourceContext *az_context ));
extern void unload_stack  _ARGUMENTS(( char *rec , int rec_size , src_source_record_type *az_src_rec ));
extern void save_value_machine_code  _ARGUMENTS(( sym_value_entry_type *value_entry , URMResourceContext *az_context ));
extern void save_module_machine_code  _ARGUMENTS(( src_source_record_type *az_src_rec , URMResourceContext *az_context ));
extern void off_put  _ARGUMENTS(( unsigned short off_type , unsigned short off_offset ));
extern void off_get  _ARGUMENTS(( unsigned short *off_type , unsigned short *off_offset ));
extern char *type_from_code  _ARGUMENTS(( MrmType type_code ));
extern char *access_from_code  _ARGUMENTS(( MrmFlag access_code ));
extern char *group_from_code  _ARGUMENTS(( MrmGroup group_code ));
extern void format_arg_value  _ARGUMENTS(( RGMArgValuePtr argval_ptr , char *buffer ));
extern char *class_name_from_code  _ARGUMENTS(( MrmCode mrm_class ));
extern char *resource_name_from_code  _ARGUMENTS(( MrmCode resource ));

/* uilmain.c */
extern void uil_exit  _ARGUMENTS(( int severity ));
extern Uil_status_type Uil _ARGUMENTS((Uil_command_type
*comand_desc,Uil_compile_desc_type *compile_desc,Uil_continue_type
(*message_cb)(), char *message_data, Uil_continue_type (*status_cb)(),
char *status_data));


/* uilp2out.c */
extern void sem_output_uid_file  _ARGUMENTS(( void ));
extern void push  _ARGUMENTS(( sym_entry_type *sym_entry ));
extern sym_entry_type *pop  _ARGUMENTS(( void ));
extern void out_emit_widget  _ARGUMENTS(( sym_widget_entry_type *widget_entry ));
extern void extract_subtree_control  _ARGUMENTS(( sym_list_entry_type *list_entry , sym_control_entry_type **menu_entry , int *count ));
extern void extract_create_callback  _ARGUMENTS(( sym_list_entry_type *list_entry , sym_callback_entry_type **create_entry ));
extern void process_all_callbacks  _ARGUMENTS(( sym_list_entry_type *list_entry , int *arglist_index ));
extern void process_all_arguments  _ARGUMENTS(( sym_list_entry_type *list_entry , int *arglist_index , int *related_count ));
extern void process_all_controls  _ARGUMENTS(( sym_list_entry_type *list_entry , int *widget_index ));
extern void out_emit_value  _ARGUMENTS(( sym_value_entry_type *value_entry ));
extern void emit_callback  _ARGUMENTS(( sym_callback_entry_type *callback_entry , int *arglist_index , boolean emit_create ));
extern void emit_callback_procedures _ARGUMENTS(( sym_proc_ref_entry_type *proc_ref_entry_next, int *proc_ref_index, MrmOffset callback_offset ));
extern int count_proc _ARGUMENTS(( sym_list_entry_type *proc_list, int count));
extern void emit_argument  _ARGUMENTS(( sym_argument_entry_type *argument_entry , int arglist_index , int *related_arg_count ));
extern void emit_control  _ARGUMENTS(( sym_control_entry_type *control_entry , int control_offset ));
extern MrmCode ref_value  _ARGUMENTS(( sym_value_entry_type *value_entry , MrmCode *arg_type , long *arg_value , MrmCode *arg_access , char **arg_index , MrmResource_id *arg_id , MrmCode *arg_group ));
extern MrmCode ref_control  _ARGUMENTS(( sym_control_entry_type *control_entry , MrmCode *access , char **index , MrmResource_id *id ));
extern void issue_urm_error  _ARGUMENTS(( char *problem ));
extern MrmCode Urm_code_from_uil_type  _ARGUMENTS(( int uil_type ));
extern int compute_color_table_size  _ARGUMENTS(( sym_value_entry_type *table_entry ));
extern void create_color_table  _ARGUMENTS(( sym_value_entry_type *table_entry , char *buffer ));
extern int compute_icon_size  _ARGUMENTS(( sym_value_entry_type *icon_entry ));
extern void create_icon  _ARGUMENTS(( sym_value_entry_type *icon_entry , char *buffer ));
extern int compute_list_size  _ARGUMENTS(( sym_list_entry_type *list_entry , int type ));
extern void create_int_compression_codes  _ARGUMENTS(( void ));
extern void create_ext_compression_codes  _ARGUMENTS(( void ));

/* uilp2reslv.c */
extern void sem_resolve_forward_refs  _ARGUMENTS(( void ));

/* uilsarcomp.c */
extern sym_value_entry_type *sem_create_cstr  _ARGUMENTS(( void ));
extern sym_value_entry_type *sem_create_wchar_str  _ARGUMENTS(( void ));
extern void sem_append_str_to_cstr  _ARGUMENTS(( sym_value_entry_type *az_cstr_entry , sym_value_entry_type *az_str_entry , boolean op2_temporary ));
extern void sem_append_cstr_to_cstr  _ARGUMENTS(( sym_value_entry_type *az_cstr1_entry , sym_value_entry_type *az_cstr2_entry , boolean op2_temporary ));
extern sym_value_entry_type *sem_cat_str_to_str  _ARGUMENTS(( sym_value_entry_type *az_str1_entry , boolean op1_temporary , sym_value_entry_type *az_str2_entry , boolean op2_temporary ));
extern int sem_map_subclass_to_charset  _ARGUMENTS(( int charset_as_subclass ));
extern void sar_charset_verify  _ARGUMENTS(( yystype *charset_frame ));
extern void sar_make_fallback_charset _ARGUMENTS((yystype *name_frame)); 
extern void sar_chk_charset_attr  _ARGUMENTS(( yystype *target_frame , yystype *value_frame , yystype *prior_value_frame ));
extern void sar_make_charset  _ARGUMENTS(( yystype *target_frame , yystype *value_frame , yystype *attr_frame , yystype *keyword_frame ));

/* uilsarexp.c */
extern void sar_binary_op  _ARGUMENTS(( yystype *operator_frame , yystype *op1_frame , yystype *op2_frame ));
extern void sar_unary_op  _ARGUMENTS(( yystype *operator_frame , yystype *op1_frame ));

/* uilsarinc.c */
extern void sar_include_file  _ARGUMENTS(( yystype *file_frame , yystype *include_frame , yystype *semi_frame ));

/* uilsarmod.c */
extern void sar_initialize  _ARGUMENTS(( void ));
extern void sar_create_root  _ARGUMENTS(( yystype *root_frame ));
extern void sar_create_module  _ARGUMENTS(( yystype *target_frame , yystype *id_frame , yystype *module_frame ));
extern void sar_process_module_version  _ARGUMENTS(( yystype *value_frame , yystype *start_frame ));
extern void sar_process_module_sensitivity  _ARGUMENTS(( yystype *token_frame , yystype *start_frame ));
extern void sar_process_module_charset  _ARGUMENTS(( yystype *token_frame , yystype *start_frame ));
extern void sar_save_module_source  _ARGUMENTS(( void ));
extern void sar_make_def_obj  _ARGUMENTS(( yystype *object_frame ));
extern void sar_process_module_variant  _ARGUMENTS(( yystype *obj_type_frame , yystype *variant_frame ));
extern void sar_save_section_source  _ARGUMENTS(( yystype *header_frame , int section_type ));

/* uilsarobj.c */
extern void sar_assoc_comment  _ARGUMENTS(( sym_obj_entry_type    *object ));
extern void sar_create_object  _ARGUMENTS(( yystype *object_frame , unsigned char object_type ));
extern void sar_create_child  _ARGUMENTS(( yystype *object_frame ));
extern void sar_link_section  _ARGUMENTS(( yystype *id_frame ));
extern void sar_save_src_semicolon_pos  _ARGUMENTS(( yystype *semi_frame ));
extern void sar_save_list_end  _ARGUMENTS(( yystype *close_frame ));
extern void sar_save_src_entry_end  _ARGUMENTS(( yystype *close_frame , yystype *entry_frame ));
extern void sar_set_object_flags  _ARGUMENTS(( yystype *current_frame , unsigned char mask ));
extern void sar_unset_object_flags  _ARGUMENTS(( yystype *current_frame , unsigned char mask ));
extern void sar_set_list_type  _ARGUMENTS(( yystype *current_frame ));
extern void sar_set_object_class  _ARGUMENTS(( yystype *current_frame ));
extern void sar_set_object_variant  _ARGUMENTS(( yystype *current_frame ));
extern yystype *sem_find_object  _ARGUMENTS(( yystype *current_frame ));
extern void sar_object_reference  _ARGUMENTS(( yystype *ref_frame ));
extern void sar_update_parent_list  _ARGUMENTS(( yystype *control_list_frame ));
extern void parent_list_traverse  _ARGUMENTS(( sym_widget_entry_type *widget_entry , sym_list_entry_type *control_list_entry ));
extern void sar_save_feature  _ARGUMENTS(( yystype *feature_frame ));
extern void sar_save_argument_pair  _ARGUMENTS(( yystype *argument_frame , yystype *value_frame , yystype *equals_frame ));
extern void sar_save_reason_binding  _ARGUMENTS(( yystype *reason_frame , yystype *proc_ref_frame , yystype *equals_frame ));
extern void sar_save_control_item  _ARGUMENTS(( yystype *managed_frame , yystype *item_frame ));
extern void sar_save_control_widget  _ARGUMENTS(( yystype *control_frame , yystype *item_frame ));
extern void sar_save_user_proc_ref_src  _ARGUMENTS(( yystype *procedure_frame , yystype *proc_id_frame , yystype *proc_arg_frame ));
extern void sar_process_proc_ref  _ARGUMENTS(( yystype *proc_id_frame , yystype *proc_arg_frame , int context ));
extern void sar_add_list_entry  _ARGUMENTS(( yystype *entry_frame ));
extern void sar_add_forward_list_entry  _ARGUMENTS(( yystype *entry_frame ));
extern void sar_verify_object  _ARGUMENTS(( yystype *current_frame ));
extern sym_entry_type *sem_allocate_node  _ARGUMENTS(( unsigned char node_tag , unsigned short node_size ));
extern void sem_free_node  _ARGUMENTS(( sym_entry_type *node_ptr ));

/* uilsarproc.c */
extern void sar_create_procedure  _ARGUMENTS(( XmConst yystype *id_frame , XmConst yystype *param_frame , XmConst yystype *class_frame , XmConst yystype *semi_frame ));
extern sym_proc_ref_entry_type *sem_reference_procedure  _ARGUMENTS(( yystype *id_frame , XmConst yystype *value_frame , XmConst int context ));
extern sym_entry_type *sem_ref_name  _ARGUMENTS(( yystype *id_frame , XmConst int tag ));

/* uilsarval.c */
extern void sar_map_keyword_to_name  _ARGUMENTS(( yystype *target_frame , yystype *keyword_frame ));
extern void sar_process_id  _ARGUMENTS(( yystype *target_frame , yystype *id_frame ));
extern void sar_process_id_ref  _ARGUMENTS(( yystype *id_frame ));
extern void sar_make_private_value  _ARGUMENTS(( yystype *value_frame , yystype *token_frame , int value_type , yystype *keyword_frame , int arg_type ));
extern void sar_make_rgb_private_value  _ARGUMENTS(( yystype *value_frame , yystype *token_frame , int value_type , yystype *keyword_frame , int arg_type ));
extern void sar_append_table_value  _ARGUMENTS(( yystype *value_frame , yystype *table_frame , int table_type , yystype *comma_frame ));
extern void sar_value_not_implemented  _ARGUMENTS(( yystype *value_frame , yystype *token_frame , char *error_text ));
extern void sar_cat_value  _ARGUMENTS(( yystype *operator_frame , yystype *op1_frame , yystype *op2_frame ));
extern void sar_chk_comp_str_attr  _ARGUMENTS(( yystype *target_frame , yystype *value_frame , yystype *prior_value_frame ));
extern void sar_make_comp_str  _ARGUMENTS(( yystype *target_frame , yystype *value_frame , yystype *attr_frame , yystype *keyword_frame ));
extern void sar_make_comp_str_comp _ARGUMENTS((yystype *target_frame, yystype *type_frame, yystype *value_frame, yystype *keyword_frame));
extern void sar_make_wchar_str  _ARGUMENTS(( yystype *target_frame , yystype *value_frame , yystype *attr_frame , yystype *keyword_frame ));
extern void sar_value_type_error  _ARGUMENTS(( yystype *value_frame , int expected_type ));
extern void sar_private_error  _ARGUMENTS(( yystype *value_frame ));
extern void sar_import_value_entry  _ARGUMENTS(( yystype *target_frame , yystype *token_frame ));
extern void sar_bind_value_name  _ARGUMENTS(( yystype *id_frame , yystype *value_frame , yystype *semi_frame ));
extern sym_name_entry_type *sem_dcl_name  _ARGUMENTS(( XmConst yystype *id_frame ));
extern sym_value_entry_type *sem_create_value_entry  _ARGUMENTS(( char *value , int length , int value_type ));
extern void sar_create_identifier  _ARGUMENTS(( XmConst yystype *id_frame , XmConst yystype *semi_frame ));
extern void sar_make_font_table  _ARGUMENTS(( yystype *target_frame , yystype *font_frame , yystype *prior_target_frame , yystype *keyword_frame ));
extern void sar_make_font_item  _ARGUMENTS(( yystype *target_frame , yystype *charset_frame , yystype *font_frame ));
extern void sar_make_font  _ARGUMENTS(( yystype *target_frame , yystype *charset_frame , yystype *value_frame , yystype *keyword_frame ));
extern void sar_make_fontset  _ARGUMENTS(( yystype *target_frame , yystype *charset_frame , yystype *value_frame , yystype *keyword_frame ));
extern void sar_make_color_item  _ARGUMENTS(( yystype *target_frame , yystype *color_frame , yystype *letter_frame ));
extern void sar_append_color_item  _ARGUMENTS(( yystype *target_frame , yystype *item_frame , yystype *prior_target_frame ));
extern void sar_make_color_table  _ARGUMENTS(( yystype *target_frame , yystype *list_frame , yystype *keyword_frame ));
extern void sar_make_color  _ARGUMENTS(( yystype *target_frame , yystype *color_frame , yystype *mono_frame , yystype *keyword_frame ));
extern void sar_make_icon  _ARGUMENTS(( yystype *target_frame , yystype *list_frame , yystype *table_frame , yystype *keyword_frame ));

/* uilsemcset.c */
extern char *sem_charset_name  _ARGUMENTS(( int l_charset , sym_value_entry_type *az_charset_entry ));
extern void sem_charset_info  _ARGUMENTS(( int l_charset , sym_value_entry_type *az_charset_entry , int *write_direction , int *parse_direction , int *sixteen_bit ));
extern int sem_charset_lang_name  _ARGUMENTS(( char *lang_charset ));

/* uilsemval.c */
typedef	union  	{
	    boolean	boolean_value;
	    int		integer_value;
	    double	real_value;
	    char        character_value;   /* character data type RAP */
	    float       single_float_value; /* single float data type RAP */
	} data_value_type;

extern void sem_validation  _ARGUMENTS(( void ));
extern void sem_validate_node  _ARGUMENTS(( sym_entry_type *node ));
extern sym_value_entry_type *sem_validate_value_node  _ARGUMENTS(( sym_value_entry_type *value_node ));
extern void sem_validate_widget_node  _ARGUMENTS(( sym_widget_entry_type *widget_node ));
extern void sem_validate_argument_list  _ARGUMENTS(( sym_widget_entry_type *widget_node , unsigned int widget_type , sym_list_entry_type *list_entry , sym_argument_entry_type **seen ));
extern void sem_validate_argument_entry  _ARGUMENTS(( sym_widget_entry_type *widget_node , unsigned int widget_type , sym_list_entry_type *list_entry , sym_argument_entry_type *argument_entry , sym_argument_entry_type **seen ));
extern void sem_validate_argument_enumset  _ARGUMENTS(( sym_argument_entry_type *argument_entry , int arg_code , sym_value_entry_type *arg_value_entry ));
extern void sem_validate_constraint_entry  _ARGUMENTS(( sym_widget_entry_type *widget_node , sym_argument_entry_type *argument_entry, unsigned int widget_type ));
extern void sem_validate_callback_list  _ARGUMENTS(( sym_widget_entry_type *widget_node , unsigned int widget_type , sym_list_entry_type *list_entry , sym_callback_entry_type **seen ));
extern void sem_validate_callback_entry  _ARGUMENTS(( sym_widget_entry_type *widget_node , unsigned int widget_type , sym_list_entry_type *list_entry , sym_callback_entry_type *callback_entry , sym_callback_entry_type **seen ));
extern void sem_validate_control_list  _ARGUMENTS(( sym_widget_entry_type *widget_node , unsigned int widget_type , sym_list_entry_type *list_entry , int *count ));
extern void sem_validate_control_entry  _ARGUMENTS(( sym_widget_entry_type *widget_node , unsigned int widget_type , sym_list_entry_type *list_entry , sym_control_entry_type *control_entry , int *gadget_count ));
extern void sem_validate_widget_cycle  _ARGUMENTS(( sym_list_entry_type *list_entry , sym_name_entry_type *cycle_name ));
extern boolean sem_validate_widget_cycle_aux  _ARGUMENTS(( sym_list_entry_type *list_entry , sym_name_entry_type *cycle_name ));
extern boolean sem_validate_verify_cycle  _ARGUMENTS(( sym_widget_entry_type *cycle_obj , sym_list_entry_type *list_entry ));
extern void sem_validate_procref_list  _ARGUMENTS(( sym_list_entry_type *list_entry ));
extern void sem_validate_procref_entry  _ARGUMENTS(( sym_proc_ref_entry_type *procref_entry ));
extern boolean sem_argument_allowed  _ARGUMENTS(( unsigned int arg_code , unsigned int class_code ));
extern boolean sem_reason_allowed  _ARGUMENTS(( unsigned int rsn_code , unsigned int class_code ));
extern boolean sem_control_allowed  _ARGUMENTS(( unsigned int ctl_code , unsigned int class_code ));
extern boolean sem_child_allowed  _ARGUMENTS(( unsigned int ctl_code , unsigned int class_code ));
extern sym_value_entry_type *sem_evaluate_value  _ARGUMENTS(( sym_value_entry_type *val_entry ));
extern sym_value_entry_type *sem_evaluate_value_cs  _ARGUMENTS(( sym_value_entry_type *csval_entry ));
/* BEGIN OSF Fix CR 4859 */
/* END OSF Fix CR 4859 */
extern sym_value_entry_type *sem_evaluate_value_expr  _ARGUMENTS(( sym_value_entry_type *value_entry ));
extern int validate_arg  _ARGUMENTS(( sym_value_entry_type *operand_entry , int v_operator ));
extern int sem_convert_to_float  _ARGUMENTS(( sym_value_entry_type *operand_entry , data_value_type *data_value ));
extern int sem_convert_to_integer  _ARGUMENTS(( sym_value_entry_type *operand_entry , data_value_type *data_value ));
extern int sem_convert_to_single_float  _ARGUMENTS(( sym_value_entry_type *operand_entry , data_value_type *data_value ));
extern int sem_convert_to_error  _ARGUMENTS(( sym_value_entry_type *operand_entry , data_value_type *data_value ));
extern void sar_cat_value_entry  _ARGUMENTS(( sym_value_entry_type **target_entry , sym_value_entry_type *op1_entry , sym_value_entry_type *op2_entry ));

/* uilsrcsrc.c */
extern void src_initialize_source  _ARGUMENTS(( void ));
extern void Uil_src_cleanup_source  _ARGUMENTS(( void ));
extern void src_open_file  _ARGUMENTS(( XmConst char *c_file_name , char *full_file_name ));
extern status src_get_source_line  _ARGUMENTS(( void ));
extern status open_source_file  _ARGUMENTS(( XmConst char *c_file_name , uil_fcb_type *az_fcb , src_source_buffer_type *az_source_buffer ));
extern status close_source_file  _ARGUMENTS(( uil_fcb_type *az_fcb ));
extern status get_line  _ARGUMENTS(( uil_fcb_type *az_fcb ));
extern boolean reget_line  _ARGUMENTS(( uil_fcb_type *az_fcb , char *c_buffer , XmConst z_key *z_access_key ));
extern char *src_get_file_name  _ARGUMENTS(( XmConst src_source_record_type *az_src_rec ));
extern boolean src_retrieve_source  _ARGUMENTS(( XmConst src_source_record_type *az_src_rec , char *c_buffer ));
extern void src_append_diag_info  _ARGUMENTS(( XmConst src_source_record_type *az_src_rec , XmConst int l_src_pos , XmConst char *c_msg_text , XmConst int l_msg_number ));
extern void src_append_machine_code  _ARGUMENTS(( src_source_record_type *az_src_rec , XmConst int l_offset , XmConst int l_code_len , XmConst char *c_code , XmConst char *c_text_arg ));

/* uilsymnam.c */
extern sym_name_entry_type *sym_find_name  _ARGUMENTS(( int l_length , char *c_text ));
extern sym_name_entry_type *sym_insert_name  _ARGUMENTS(( int l_length , char *c_text ));
extern int hash_function  _ARGUMENTS(( int l_length , char *c_value ));
#if debug_version
extern void sym_dump_hash_table  _ARGUMENTS(( void ));
#endif

/* uilsymstor.c */
extern void sym_initialize_storage  _ARGUMENTS(( void ));
extern void Uil_sym_cleanup_storage  _ARGUMENTS(( boolean freealloc ));
extern void sym_make_external_def  _ARGUMENTS(( XmConst sym_name_entry_type *az_name ));
extern void sym_make_forward_ref  _ARGUMENTS(( XmConst yystype *az_id_frame , XmConst int l_widget_type , XmConst char *a_location ));
extern void sym_make_value_forward_ref  _ARGUMENTS(( XmConst yystype *az_value_frame , XmConst char *a_location , XmConst unsigned char fwd_ref_flags ));
extern void UilDumpSymbolTable  _ARGUMENTS(( sym_entry_type *node_entry ));
extern void sym_dump_symbols  _ARGUMENTS(( void ));
extern void sym_dump_symbol  _ARGUMENTS(( sym_entry_type *az_symbol_entry ));
extern void sym_dump_widget  _ARGUMENTS(( XmConst sym_widget_entry_type *az_widget_entry ));
extern void sym_dump_argument  _ARGUMENTS(( XmConst sym_argument_entry_type *az_argument_entry ));
extern void sym_dump_control  _ARGUMENTS(( XmConst sym_control_entry_type *az_control_entry ));
extern void sym_dump_callback  _ARGUMENTS(( XmConst sym_callback_entry_type *az_callback_entry ));
extern void sym_dump_list  _ARGUMENTS(( XmConst sym_list_entry_type *az_list_entry ));
extern void sym_dump_name  _ARGUMENTS(( XmConst sym_name_entry_type *az_name_entry ));
extern void sym_dump_module  _ARGUMENTS(( XmConst sym_module_entry_type *az_module_entry ));
extern void sym_dump_color_item  _ARGUMENTS(( XmConst sym_color_item_entry_type *az_color_item_entry ));
extern void sym_dump_parent_list_item  _ARGUMENTS(( XmConst sym_parent_list_type *az_parent_list_item ));
extern void sym_dump_external_def  _ARGUMENTS(( XmConst sym_external_def_entry_type *az_external_def_entry ));
extern void sym_dump_proc_def  _ARGUMENTS(( XmConst sym_proc_def_entry_type *az_proc_def_entry ));
extern void sym_dump_proc_ref  _ARGUMENTS(( XmConst sym_proc_ref_entry_type *az_proc_ref_entry ));
extern void sym_dump_forward_ref  _ARGUMENTS(( XmConst sym_forward_ref_entry_type *az_forward_ref_entry ));
extern void sym_dump_value  _ARGUMENTS(( XmConst sym_value_entry_type *az_value_entry ));
extern void output_text  _ARGUMENTS(( XmConst int length , XmConst char *text ));
extern void sym_dump_source_info  _ARGUMENTS(( sym_entry_header_type *hdr ));
extern void sym_dump_obj_header  _ARGUMENTS(( XmConst sym_obj_entry_type *az_obj_entry ));
extern void sym_dump_include_file  _ARGUMENTS(( sym_include_file_entry_type *az_symbol_entry ));
extern void sym_dump_section  _ARGUMENTS(( sym_section_entry_type *az_symbol_entry ));
extern void sym_dump_object_variant  _ARGUMENTS(( sym_def_obj_entry_type *az_symbol_entry ));
extern void sym_dump_root_entry  _ARGUMENTS(( sym_root_entry_type *az_symbol_entry ));
extern char *sym_section_text  _ARGUMENTS(( int b_type ));
extern void dump_free_list  _ARGUMENTS(( void ));

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /* UilDefI_h */
/* DON'T ADD STUFF AFTER THIS #endif */
