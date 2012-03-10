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
static char rcsid[] = "$TOG: wml.c /main/8 1999/04/16 09:41:47 mgreess $"
#endif
#endif
/*
*  (c) Copyright 1989, 1990, DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASS. */

/*
 * This is the main program for WML. It declares all global data structures
 * used during a compilation, and during output.
 */

/*
 * WML is a semi-standard Unix application. It reads its input from
 * stdin, which is expected to be a stream containing a WML description
 * of a UIL language. If this stream is successfully parsed and semantically
 * validated, then WML writes a series of standard .h and .dat files into
 * the user directory. The .h files will be used directly to construct
 * the UIL compiler. The .dat files are used by other phases of UIL
 * table generation.
 *
 * The files created by WML are:
 *
 *	.h files:
 *		UilSymGen.h
 *		UilSymArTy.h
 *		UilSymRArg.h
 *		UilDrmClas.h
 *		UilConst.h
 *		UilSymReas.h
 *		UilSymArTa.h
 *		UilSymCtl.h
 *		UilSymNam.h
 *	.dat files
 *		argument.dat
 *		reason.dat
 *		grammar.dat
 *	.mm files
 *		wml-uil.mm
 */

#include "wml.h"

#if defined(SYSV) || defined(SVR4)
#include <fcntl.h>
#else
#include <sys/file.h>
#endif
#include <stdio.h>


/*
 * Globals used during WML parsing.
 *
 * WML uses globals exclusively to communicate data during parsing. The
 * current object being constructed is held by these globals, and all
 * routines called from the parse assume correct setting of these globals.
 * This simplisitic approach is possible since the WML description language
 * has no recursive constructs requiring a frame stack.
 */

/*
 * Error and other counts
 */
int		wml_err_count = 0;	/* total errors */
int		wml_line_count = 0;	/* lines read from input */

/*
 * Dynamic ordered vector of all objects encountered during parse. This
 * is used to detect name collisions, and is the primary order vector
 * used for all other vectors constructed curing the semantic resolution
 * phase of processing.
 */
DynamicHandleListDef	wml_synobj;
DynamicHandleListDefPtr	wml_synobj_ptr = &wml_synobj;


/*
 * Dynamic vectors of vectors partitioned and ordered
 * as required by the semantic processing and output routines. All
 * point to resolved objects rather than syntactic objects.
 */
DynamicHandleListDef	wml_obj_datatype;	/* datatype objects */
DynamicHandleListDefPtr	wml_obj_datatype_ptr = &wml_obj_datatype;

DynamicHandleListDef	wml_obj_enumval;	/* enumeration value objects */
DynamicHandleListDefPtr	wml_obj_enumval_ptr = &wml_obj_enumval;

DynamicHandleListDef	wml_obj_enumset;	/* enumeration set objects */
DynamicHandleListDefPtr	wml_obj_enumset_ptr = &wml_obj_enumset;

DynamicHandleListDef	wml_obj_reason;	/* reason resource objects */
DynamicHandleListDefPtr	wml_obj_reason_ptr = &wml_obj_reason;

DynamicHandleListDef	wml_obj_arg;	/* argument resource objects */
DynamicHandleListDefPtr	wml_obj_arg_ptr = &wml_obj_arg;

DynamicHandleListDef	wml_obj_child;	/* argument resource objects */
DynamicHandleListDefPtr	wml_obj_child_ptr = &wml_obj_child;

DynamicHandleListDef	wml_obj_allclass;	/* metaclass, widget, gadget */
DynamicHandleListDefPtr	wml_obj_allclass_ptr = &wml_obj_allclass;

DynamicHandleListDef	wml_obj_class;		/* widget & gadget objects */
DynamicHandleListDefPtr	wml_obj_class_ptr = &wml_obj_class;

DynamicHandleListDef	wml_obj_ctrlist;	/* controls list objects */
DynamicHandleListDefPtr	wml_obj_ctrlist_ptr = &wml_obj_ctrlist;

DynamicHandleListDef	wml_obj_charset;	/* charset objects */
DynamicHandleListDefPtr	wml_obj_charset_ptr = &wml_obj_charset;

DynamicHandleListDef	wml_tok_sens;		/* case-sensitive tokens */
DynamicHandleListDefPtr	wml_tok_sens_ptr = &wml_tok_sens;

DynamicHandleListDef	wml_tok_insens;		/* case-insensitive tokens */
DynamicHandleListDefPtr	wml_tok_insens_ptr = &wml_tok_insens;


/*
 * Routines only accessible in this module
 */
void wmlInit ();

/*
 * External variables
 */
extern	int	yyleng;




/*
 * The WML main routine:
 *
 *	1. Initialize global storage
 *	2. Open the input file if there is one
 *	3. Parse the WML description in stdin. Exit on errors
 *	4. Perform semantic validation and resolution. Exit on errors.
 *	5. Output files
 */

int main (argc, argv)
    int		argc;
    char	**argv;

{

int		done;
int		fd;		/* input file descriptor */

/*
 * Initialize storage
 */
wmlInit ();

/*
 * Assume that anything in argv must be an input file. Open it, and
 * dup it to stdin
 */
if ( argc > 1 )
    {
    if ( (fd=open(argv[1],O_RDONLY)) == -1 )
	printf ("\nCouldn't open file %s", argv[1]);
    else
	dup2 (fd, 0);
    }

/*
 * Process the input
 */
done = 0;
while (!done)
    {
    
    /*
     * Parse the input stream
     */
    yyleng = 0;		/* initialization safety */
    yyparse ();
    if ( wml_err_count > 0 ) break;
    printf ("\nParse of WML input complete");
    
    /*
     * Perform semantic validation, and construct resolved data structures
     */
    wmlResolveDescriptors ();
    if ( wml_err_count > 0 ) break;
    printf ("\nSemantic validation and resolution complete");
    
    /*
     * Output 
     */
    wmlOutput ();
    if ( wml_err_count > 0 ) break;
    printf ("\nWML Uil*.h and wml-uil.mm file creation complete\n");
    
    done = 1;
    }

/*
 * Report inaction on errors
 */
if ( wml_err_count > 0 )
    {
    printf ("\nWML found %d errors, no or incomplete output produced\n",
	    wml_err_count);
     /* Begin fixing the bug CR 4748 */
     exit(1);
     /* End fixing the bug CR 4748 */
    }

return (0);

}


/*
 * Routine to initialize WML.
 *
 * The main job is to dynamically allocate any dynamic lists to a reasonable
 * initial state.
 */
void wmlInit ()

{

/*
 * Initialize the list of all syntactic objects
 */
wmlInitHList (wml_synobj_ptr, 1000, TRUE);

}

yywrap()
{
return(1);
}
