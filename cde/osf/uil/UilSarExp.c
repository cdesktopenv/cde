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
static char rcsid[] = "$XConsortium: UilSarExp.c /main/11 1995/07/14 09:37:03 drk $"
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
**      This module supports value expressions in UIL.  
**
**--
**/


/*
**
**  INCLUDE FILES
**
**/

#include "UilDefI.h"


/*
**
**  DEFINE and MACRO DEFINITIONS
**
**/


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
**      This function processes a binary arithmetic operator in an expression
**
**  FORMAL PARAMETERS:
**
**      operator_frame: [in-out]    frame holding the operator and the
**				    target frame
**	op1_frame: [in]		    frame holding operand 1
**	op2_frame: [in]		    frame holding operand 2
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
**      op1 or op2 symbol entry may be freed
**
**--
**/
void	sar_binary_op(operator_frame, op1_frame, op2_frame)

yystype	    *operator_frame;
yystype	    *op1_frame;
yystype	    *op2_frame;

{
    sym_value_entry_type    *res_entry;
    int	    		    operator;

    _assert( operator_frame->b_tag == sar_k_token_frame,
	     "operator missing" );

	res_entry = (sym_value_entry_type *)
			sem_allocate_node( sym_k_value_entry,
				       sym_k_value_entry_size );

	res_entry->obj_header.b_flags = sym_m_builtin | sym_m_private;
	res_entry->b_type = sym_k_any_value;

/*	res_entry->az_source_rec = op1_frame->az_source_record; */

	_sar_save_source_pos (&res_entry->header, op1_frame);

    /*
    **	Determine the operator from the operator frame
    */

    switch (operator_frame->b_type)
    {
    case AND:
	    operator = sym_k_and_op;
	break;

    case PLUS:
	operator = sym_k_add_op;
	break;

    case MINUS:
	operator = sym_k_subtract_op;
	break;

    case MULTIPLY:
	operator = sym_k_multiply_op;
	break;

    case DIVIDE:
	operator = sym_k_divide_op;
	break;

    case LEFT_SHIFT:
	operator = sym_k_left_shift_op;
	break;

    case RIGHT_SHIFT:
	operator = sym_k_right_shift_op;
	break;

    case OR:
	operator = sym_k_or_op;
	break;

    case XOR:
	operator = sym_k_or_op;
	break;

    default:
	_assert( FALSE, "unknown binary operator" );
    }

    res_entry->b_expr_opr = operator;

    /*
    ** If the value is a forward reference, we'll patch in the
    ** address of the the referenced value between passes.  Otherwise,
    ** just point to the referenced value node.
    */
    if ((op1_frame->b_flags & sym_m_forward_ref) != 0)
	sym_make_value_forward_ref (op1_frame, 
	(char*)&(res_entry->az_exp_op1), sym_k_patch_add);
    else    
	res_entry->az_exp_op1 =
	    (sym_value_entry_type *) op1_frame->value.az_symbol_entry;

    if ((op2_frame->b_flags & sym_m_forward_ref) != 0)
	sym_make_value_forward_ref (op2_frame, 
	(char*)&(res_entry->az_exp_op2), sym_k_patch_add);
    else    
	res_entry->az_exp_op2 =
	    (sym_value_entry_type *) op2_frame->value.az_symbol_entry;

    operator_frame->b_tag = sar_k_value_frame;
    operator_frame->b_type = res_entry->b_type;
    operator_frame->b_flags = res_entry->obj_header.b_flags;
    operator_frame->value.az_symbol_entry =
		(sym_entry_type *) res_entry;

    return;
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function processes a unary arithmetic operator in an expression
**
**  FORMAL PARAMETERS:
**
**      operator_frame: [in-out]    frame holding the operator and the
**				    target frame
**	op1_frame: [in]		    frame holding operand 1
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
**      op1 symbol entry may be freed
**
**--
**/
void	sar_unary_op(operator_frame, op1_frame)

yystype	    *operator_frame;
yystype	    *op1_frame;

{
    sym_value_entry_type    *res_entry;
    int			    operator;
    int			    res_type;


    _assert( operator_frame->b_tag == sar_k_token_frame,
	     "operator missing" );

    /*
    **	Determine the operator from the operator frame
    */

    switch (operator_frame->b_type)
    {
    case PLUS:
	operator = sym_k_unary_plus_op;
	break;

    case MINUS:
	operator = sym_k_unary_minus_op;
	break;

    case NOT:
	operator = sym_k_not_op;
	break;

    case INTEGER:
	res_type = sym_k_integer_value;
	operator = sym_k_coerce_op;
        break;
    case FLOAT:
	res_type = sym_k_float_value;
	operator = sym_k_coerce_op;
        break;
    case SINGLE_FLOAT:
	res_type = sym_k_single_float_value;
	operator = sym_k_coerce_op;
        break;
    case KEYSYM:
	res_type = sym_k_keysym_value;
	operator = sym_k_coerce_op;
        break;


    default:
	_assert( FALSE, "unknown unary operator" );
    }

    /*
    **	Create the result
    */

    res_entry = (sym_value_entry_type *)
	sem_allocate_node (sym_k_value_entry, sym_k_value_entry_size);
    
    res_entry->b_expr_opr = operator;
    /* Begin fixing OSF CR 5691 */
    res_entry->b_type = operator; 
    /* End fixing OSF CR 5691 */    
    res_entry->az_exp_op1 =
	(sym_value_entry_type *) op1_frame->value.az_symbol_entry;
    res_entry->obj_header.b_flags = sym_m_builtin | sym_m_private;
    if (operator == sym_k_coerce_op)
	res_entry->b_type = res_type;
    
    _sar_save_source_pos (&res_entry->header, op1_frame );

    operator_frame->b_tag = sar_k_value_frame;
    operator_frame->b_type = res_entry->b_type;
    operator_frame->b_flags = res_entry->obj_header.b_flags;
    operator_frame->value.az_symbol_entry = (sym_entry_type *) res_entry;
    
    return;

}

