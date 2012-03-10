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
/*   $XConsortium: UilSarDef.h /main/10 1995/07/14 09:36:55 drk $ */

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
**      This include file defines the interface to the UIL parser.
**	UIL uses YACC as its parsing tool.
**
**--
**/

#ifndef UilSarDef_h
#define UilSarDef_h



/*
**  Format of a value on YACC value stack.  This is also the form of a
**  token created by the lexical analyzer.
*/

#define	    sar_k_null_frame	0	/* tag for an epsilon production */
#define	    sar_k_token_frame	1	/* tag for a token frame */
#define	    sar_k_value_frame	2	/* tag for a value frame */
#define	    sar_k_module_frame	3	/* tag for module frame */
#define     sar_k_object_frame	4	/* tag for object frame */
#define     sar_k_root_frame    5	/* tag for root frame */

typedef struct
{
    src_source_record_type  *az_source_record;	/* actual record where token exists */
    unsigned char	    b_source_pos;	/* the character in az_source_record
						   where this token begins */
    unsigned char	    b_source_end;	/* the character in az_source_record
						   where this token ends */
    unsigned char	    b_tag;		/* tag of stack frame */
    unsigned char	    b_type;		/* for tokens - token number
						   for value - the data type */
    unsigned short	    b_flags;		/* used by value */
    unsigned char	    b_direction;	/* used by value */
    unsigned char	    b_charset;		/* used by value */
    union
    {
    /* must be capable of holding a pointer */
	long		    l_integer;		/* integer value*/
	sym_entry_type	    *az_symbol_entry;	/* symbol entry */
	key_keytable_entry_type
			    *az_keyword_entry;	/* keyword entry */
    }	value;
} yystype;


/*
**  Macros for moving source information to and from parse stack frames
*/

#define    _sar_move_source_info( _target, _source )			\
	   {								\
		yystype	    *__target;					\
		yystype	    *__source;					\
									\
		__target = (_target);  __source = (_source);		\
		__target->az_source_record = __source->az_source_record;\
		__target->b_source_pos = __source->b_source_pos;	\
		__target->b_source_end = __source->b_source_end;	\
	   }

#define    _sar_move_source_info_2( _target, _source )		\
   {								\
	sym_entry_header_type	*__target;			\
	sym_entry_header_type	*__source;			\
								\
	__target = (_target);  __source = (_source);		\
								\
	__target->az_src_rec = __source->az_src_rec;		\
	__target->b_src_pos = __source->b_src_pos;		\
	__target->b_end_pos = __source->b_end_pos;		\
   }


#define _sar_save_source_info( _target, _src_beg, _src_end )	\
   {								\
	sym_entry_header_type	*__target;			\
	XmConst yystype	    		*__src_end;		\
								\
	__target = (_target); 					\
	__src_end = (_src_end);					\
								\
	__target->az_src_rec	= __src_end->az_source_record;	\
	__target->b_src_pos	= __src_end->b_source_pos;	\
	__target->b_end_pos	= __src_end->b_source_end;	\
   }

#define _sar_save_source_pos( _target, _src )			\
   {								\
	sym_entry_header_type	*__target;			\
	XmConst yystype		*__src;				\
								\
	__target = (_target);	__src = (_src);			\
								\
	__target->az_src_rec	= __src->az_source_record;	\
	__target->b_src_pos	= __src->b_source_pos;		\
	__target->b_end_pos	= __src->b_source_end;		\
   }

#define    _sar_source_position( _source )			\
		_source->az_source_record, 			\
		_source->b_source_pos

#define    _sar_source_pos2( _source )				\
		_source->header.az_src_rec, 			\
		_source->header.b_src_pos



#endif /* UilSarDef_h */
/* DON'T ADD STUFF AFTER THIS #endif */
