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
static char rcsid[] = "$TOG: UilData.c /main/14 1999/04/16 10:41:44 mgreess $"
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
**      Global data definitions
**
**--
**/


/*
**
**  INCLUDE FILES
**
**/

#include <setjmp.h>

#include <Dt/Editor.h>
#include <Dt/Help.h>
#include <Dt/HelpDialog.h>
#include <Dt/HelpQuickD.h>
#include <Dt/MenuButton.h>
#include <Dt/Print.h>
#include <Dt/TermPrim.h>
#include <Dt/Term.h>

#include <Mrm/MrmAppl.h>
#include <Mrm/Mrm.h>
#include <Xm/MwmUtil.h> /* For MWM_*_* defines. */

#include "UilDefI.h"
#include "UilSymGen.h"

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

/*    Case sensitivity switch; TRUE if case sensitive.    */



externaldef(uil_comp_glbl) boolean	uil_v_case_sensitive = TRUE;

/*    Location to store error type.    */

externaldef(uil_comp_glbl) jmp_buf	uil_az_error_env_block;
externaldef(uil_comp_glbl) boolean	uil_az_error_env_valid = FALSE;

/*    Debugging switches.    */

#if debug_version
externaldef(uil_comp_glbl) boolean	uil_v_dump_tokens = FALSE;
externaldef(uil_comp_glbl) boolean	uil_v_dump_symbols = FALSE;
#endif

externaldef(uil_comp_glbl) status	uil_l_compile_status = uil_k_success_status;
externaldef(uil_comp_glbl) Uil_compile_desc_type   *Uil_compile_desc_ptr = NULL;
externaldef(uil_comp_glbl) int		Uil_percent_complete = 0;
externaldef(uil_comp_glbl) int		Uil_lines_processed = 0;
externaldef(uil_comp_glbl) char	*Uil_current_file = "";


/*	Define the user-defined object and default character set */
externaldef(uil_sym_glbl) unsigned short int	uil_sym_user_defined_object =
		sym_k_user_defined_object;
externaldef(uil_sym_glbl) unsigned short int	uil_sym_default_charset =
		sym_k_iso_latin1_charset;
externaldef(uil_sym_glbl) unsigned short int	uil_sym_isolatin1_charset =
		sym_k_iso_latin1_charset;

/*	argument types, allowed tables, constraints, related arguments */
#include "UilSymArTy.h"
#include "UilSymRArg.h"
#include "UilSymArTa.h"
#include "UilSymReas.h"
#include "UilSymCtl.h"
#include "UilConst.h"

/* Allowed children and child classes */
#include "UilSymChCl.h"
#include "UilSymChTa.h"

/*	sym_k... to name vectors. */
#include "UilSymNam.h"

/*	Define mapping of sym_k_..._value to data type names. */
externaldef(uil_comp_glbl) int uil_max_value = sym_k_max_value;
externaldef(uil_comp_glbl) char *uil_datatype_names [] = {
    "",	/* NOT USED */
    "any",
    "boolean",
    "string",		/* char 8 */
    "compound_string",
    "float",
    "integer",
    "single_float",
    "localized_string",
    "wchar_string",
    "horizontal_dimension_integer",
    "vertical_dimension_integer",
    "horizontal_dimension_float",
    "vertical_dimension_float",
    "argument",
    "asciz_table",
    "class_rec_name",
    "color",
    "color_table",
    "font",
    "font_table",
    "icon",
    "identifier",
    "integer_table",
    "keysym",
    "pixmap",
    "reason",
    "rgb",
    "string_table",
    "translation_table",
    "widget_ref",
    "xbitmapfile",
    "fontset",
    "child",
};

/*	Enumeration Set/Value tables */
#include "UilSymEnum.h"

/*	Character set attribute tables */
#include "UilSymCSet.h"

/*	Mrm encoding information */
#include "UilUrmClas.h"

/*	Names of tokens in grammar */
#include "UilTokName.h"

/*	The keyword tables */
#include "UilKeyTab.h"


/*
**
**  OWN VARIABLE DECLARATIONS
**
**/

