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
static char rcsid[] = "$XConsortium: wmlresolve.c /main/9 1995/08/29 11:11:05 drk $"
#endif
#endif
/*
*  (c) Copyright 1989, 1990, DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASS. */

/*
 * This module contains all routines which perform semantic analysis of
 * the parsed WML specification. It is responsible for building all
 * ordered structures which can be directly translated into literal
 * code values for the various .h files. It is responsible for performing
 * inheritance of resources for all classes.
 *
 * Input:
 *	the ordered list of syntactic objects in wml_synobj_ptr
 *
 * Output:
 *
 */


#include "wml.h"

#if defined(__STDC__)
#include <stdlib.h>
#endif
#include <stdio.h>

/*
 * Routines used only in this module
 */
void wmlResolveGenerateSymK ();
void wmlResolveValidateClass ();
void wmlResolvePrintReport ();

void wmlResolveSymKDataType ();
void wmlResolveSymKChild ();
void wmlResolveSymKEnumVal ();
void wmlResolveSymKEnumSet ();
void wmlResolveSymKReason ();
void wmlResolveSymKArgument ();
void wmlResolveSymKRelated ();
void wmlResolveSymKClass ();
void wmlResolveSymKCtrlList ();
void wmlResolveSymKCharSet ();

void wmlResolveClassInherit ();
void wmlResolveClearRefPointers ();
void wmlResolveInitRefObj ();
void wmlResolveInitChildRefObj ();

void wmlResolvePrintClass ();
void wmlResolvePrintClassArgs ();
void wmlResolvePrintClassReasons ();

ObjectPtr wmlResolveFindObject ();
void wmlIssueReferenceError ();
void wmlIssueIllegalReferenceError ();
void wmlIssueError ();



/*
 * The control routine for semantic analysis. It calls the various phases.
 */

void wmlResolveDescriptors ()

{

/*
 * Perform the code assignment pass. This results in assignment of sym_k_...
 * codes to all entities. Also, all objects and cross-linking are validated.
 */
wmlResolveGenerateSymK ();
printf ("\nInitial validation and reference resolution complete");

/*
 * Perform class inheritance and validation
 */
wmlResolveValidateClass ();
printf ("\nClass validation and inheritance complete");

/*
 * Print a report
 */
if ( wml_err_count > 0 ) return;
wmlResolvePrintReport ();

}



/*
 * Routine to linearize and assign sym_k... literals for objects. Simply
 * a dispatching routine.
 */

void wmlResolveGenerateSymK ()

{

/*
 * Process the datatype objects
 */
wmlResolveSymKDataType ();

/*
 * Process the enumeration value and enumeration sets
 */
wmlResolveSymKEnumVal ();
wmlResolveSymKEnumSet ();

/*
 * Process the resources, producing argument and reason vectors.
 */
wmlResolveSymKReason ();
wmlResolveSymKArgument ();

/*
 * Bind related arguments
 */
wmlResolveSymKRelated ();

/*
 * Process the class definitions
 */
wmlResolveSymKClass ();

/*
 * Process the controls list definitions
 */
wmlResolveSymKCtrlList ();

/*
 * Process the charset objects
 */
wmlResolveSymKCharSet ();

/* Process the child definitions. */
wmlResolveSymKChild();

}



/*
 * Routine to linearize data types
 *
 * - Generate the wml_obj_datatype... vector of resolved data type objects,
 *   ordered lexicographically.
 *   Do name processing, and acquire links to any other objects named in
 *   the syntactic descriptor.
 */

void wmlResolveSymKDataType ()

{

WmlSynDataTypeDefPtr	cursyn;		/* current syntactic object */
WmlDataTypeDefPtr	newobj;		/* new resolved object */
int			ndx;		/* loop index */

/*
 * Initialize the object vector. Then process the syntactic vector,
 * processing each datatype object encountered (the vector is ordered).
 * create and append a resolved object for each one encountered. This
 * will be ordered as well.
 */
wmlInitHList (wml_obj_datatype_ptr, 50, TRUE);
for ( ndx=0 ; ndx<wml_synobj_ptr->cnt ; ndx++ )
    {
    cursyn = (WmlSynDataTypeDefPtr) wml_synobj_ptr->hvec[ndx].objptr;
    if ( cursyn->validation != WmlDataTypeDefValid ) continue;

/*
 * Create and initialize new object. Append to resolved object vector.
 */
    newobj = (WmlDataTypeDefPtr) malloc (sizeof(WmlDataTypeDef));
    newobj->syndef = cursyn;
    cursyn->rslvdef = newobj;
    if ( cursyn->int_lit != NULL )
	newobj->tkname = cursyn->int_lit;
    else
	newobj->tkname = cursyn->name;
    wmlInsertInHList (wml_obj_datatype_ptr, newobj->tkname, (ObjectPtr)newobj);

/*
 * Validate any object references in the syntactic object
 */

    }

}


/*
 * Routine to linearize children
 *
 * - Generate the wml_obj_child... vector of resolved child objects,
 *   ordered lexicographically.  Assign sym_k_... values while doing so.
 *   Link child to its class. 
 */

void wmlResolveSymKChild ()

{

WmlSynChildDefPtr	cursyn;		/* current syntactic object */
WmlChildDefPtr		newobj;		/* new resolved object */
int			code;		/* assigned sym_k code value */
int			ndx;		/* loop index */

/*
 * Initialize the object vector. Then process the syntactic vector,
 * processing each child object encountered (the vector is ordered).
 * create and append a resolved object for each one encountered. This
 * will be ordered as well.
 */
wmlInitHList (wml_obj_child_ptr, 50, TRUE);
for ( ndx=0 ; ndx<wml_synobj_ptr->cnt ; ndx++ )
    {
    cursyn = (WmlSynChildDefPtr) wml_synobj_ptr->hvec[ndx].objptr;
    if ( cursyn->validation != WmlChildDefValid ) continue;

/*
 * Create and initialize new object. Append to resolved object vector.
 */
    newobj = (WmlChildDefPtr) malloc (sizeof(WmlChildDef));
    newobj->syndef = cursyn;
    cursyn->rslvdef = newobj;
    newobj->tkname = cursyn->name;
    wmlInsertInHList (wml_obj_child_ptr, newobj->tkname, (ObjectPtr)newobj);

/* Link class to the resolved object. */
if (cursyn->class != NULL)
  newobj->class = (WmlClassDefPtr)
    wmlResolveFindObject(cursyn->class, WmlClassDefValid, cursyn->name);
  }

/*
 * All objects are in the vector. The order is the code order, so
 * process it again and assign codes to each object
 */
code = 1;
for ( ndx=0 ; ndx<wml_obj_child_ptr->cnt ; ndx++ )
    {
    newobj = (WmlChildDefPtr) wml_obj_child_ptr->hvec[ndx].objptr;
    newobj->sym_code = code;
    code += 1;
    }

}



/*
 * Routine to linearize and assign sym_k values to enumeration values
 *
 * - Generate the wml_obj_datatype... vector of resolved data type objects,
 *   ordered lexicographically. No sym_k_... values are needed for
 *   enumeration values, so don't assign any.
 */

void wmlResolveSymKEnumVal ()

{

WmlSynEnumSetDefPtr	cures;		/* current enumeration set */
WmlSynEnumSetValDefPtr	curesv;		/* current enum set value */
WmlSynEnumValueDefPtr	cursyn;		/* current syntactic object */
WmlEnumValueDefPtr	newobj;		/* new resolved object */
int			ndx;		/* loop index */
int			code;		/* sym_k_... code */


/*
 * Perform defaulting. Process all the enumeration sets, and define a
 * syntactic object for every enumeration value named in an enumeration set
 * which has no syntactic entry. If there is an error in a name, then
 * this error won't be detected until we attempt to compile the output .h files.
 */
for ( ndx=0 ; ndx<wml_synobj_ptr->cnt ; ndx++ )
    {
    cures = (WmlSynEnumSetDefPtr) wml_synobj_ptr->hvec[ndx].objptr;
    if ( cures->validation != WmlEnumSetDefValid ) continue;
    for (curesv=cures->values ; curesv!=NULL ; curesv=curesv->next)
	if ( wmlFindInHList(wml_synobj_ptr,curesv->name) < 0 )
	    wmlCreateEnumValue (curesv->name);
    }

/*
 * Initialize the object vector. Then process the syntactic vector,
 * processing each enumeration value object encountered (the vector is ordered).
 * create and append a resolved object for each one encountered. This
 * will be ordered as well.
 */
wmlInitHList (wml_obj_enumval_ptr, 50, TRUE);
for ( ndx=0 ; ndx<wml_synobj_ptr->cnt ; ndx++ )
    {
    cursyn = (WmlSynEnumValueDefPtr) wml_synobj_ptr->hvec[ndx].objptr;
    if ( cursyn->validation != WmlEnumValueDefValid ) continue;

/*
 * Create and initialize new object. Append to resolved object vector.
 */
    newobj = (WmlEnumValueDefPtr) malloc (sizeof(WmlEnumValueDef));
    newobj->syndef = cursyn;
    cursyn->rslvdef = newobj;
    wmlInsertInHList (wml_obj_enumval_ptr, cursyn->name, (ObjectPtr)newobj);
    }

/*
 * All objects are in the vector. That order is the code order, so
 * process it again and assign codes to each object
 */
code = 1;
for ( ndx=0 ; ndx<wml_obj_enumval_ptr->cnt ; ndx++ )
    {
    newobj = (WmlEnumValueDefPtr) wml_obj_enumval_ptr->hvec[ndx].objptr;
    newobj->sym_code = code;
    code += 1;
    }

}



/*
 * Routine to linearize and assign sym_k values to enumeration sets
 *
 * - Generate the wml_obj_datatype... vector of resolved data type objects,
 *   ordered lexicographically. No sym_k_... values are needed for
 *   enumeration values, so don't assign any.
 */

void wmlResolveSymKEnumSet ()

{

WmlSynEnumSetDefPtr	cursyn;		/* current syntactic object */
WmlEnumSetDefPtr	newobj;		/* new resolved object */
int			ndx;		/* loop index */
int			code;		/* sym_k_... code */
WmlSynEnumSetValDefPtr	esvelm;		/* current syntactic list element */
WmlEnumValueDefPtr	evobj;		/* current enumeration value */
WmlEnumSetValDefPtr	esvobj;		/* current list element */

/*
 * Initialize the object vector. Then process the syntactic vector,
 * processing each enumeration set object encountered (the vector is ordered).
 * create and append a resolved object for each one encountered. This
 * will be ordered as well.
 */
wmlInitHList (wml_obj_enumset_ptr, 20, TRUE);
for ( ndx=0 ; ndx<wml_synobj_ptr->cnt ; ndx++ )
    {
    cursyn = (WmlSynEnumSetDefPtr) wml_synobj_ptr->hvec[ndx].objptr;
    if ( cursyn->validation != WmlEnumSetDefValid ) continue;

/*
 * Create and initialize new object. Append to resolved object vector.
 */
    newobj = (WmlEnumSetDefPtr) malloc (sizeof(WmlEnumSetDef));
    newobj->syndef = cursyn;
    cursyn->rslvdef = newobj;
    newobj->tkname = cursyn->name;
    newobj->dtype_def = (WmlDataTypeDefPtr)
	wmlResolveFindObject (cursyn->datatype,
			      WmlDataTypeDefValid,
			      cursyn->name);
    wmlInsertInHList (wml_obj_enumset_ptr, newobj->tkname, (ObjectPtr)newobj);
    }

/*
 * All objects are in the vector. That order is the code order, so
 * process it again and assign codes to each object. Simultaneously construct
 * a vector of resolved enumeration values, and count them.
 */
code = 1;
for ( ndx=0 ; ndx<wml_obj_enumset_ptr->cnt ; ndx++ )
    {
    newobj = (WmlEnumSetDefPtr) wml_obj_enumset_ptr->hvec[ndx].objptr;
    newobj->sym_code = code;
    code += 1;

/*
 * Validate and construct a resolved enumeration value list
 */
    cursyn = newobj->syndef;
    newobj->values_cnt = 0;
    newobj->values = NULL;
    for ( esvelm=cursyn->values ; esvelm!=NULL ; esvelm=esvelm->next )
	{
	evobj = (WmlEnumValueDefPtr)
	    wmlResolveFindObject (esvelm->name,
				  WmlEnumValueDefValid,
				  cursyn->name);
	if ( evobj == NULL ) continue;
	esvobj = (WmlEnumSetValDefPtr) malloc (sizeof(WmlEnumSetValDef));
	esvobj->value = evobj;
	esvobj->next = newobj->values;
	newobj->values = esvobj;
	newobj->values_cnt += 1;
	}
    }

}



/*
 * Routine to linearize and assign sym_k values to reasons.
 *
 * - Generate the wml_obj_reason... vector of resolved reason objects,
 *   ordered lexicographically. Assign a sym_k_... value as this is done.
 *   Do name processing, and acquire links to any other objects named in
 *   the syntactic descriptor.
 */

void wmlResolveSymKReason ()

{

WmlSynResourceDefPtr	cursyn;		/* current syntactic object */
WmlResourceDefPtr	newobj;		/* new resolved object */
int			ndx;		/* loop index */
int			code;		/* assigned sym_k code value */
char			errmsg[300];

/*
 * Initialize the object vector. Then process the syntactic vector,
 * processing each reason resource object encountered (the vector is ordered).
 * create and append a resolved object for each one encountered. This
 * will be ordered as well.
 */
wmlInitHList (wml_obj_reason_ptr, 100, TRUE);
for ( ndx=0 ; ndx<wml_synobj_ptr->cnt ; ndx++ )
    {
    cursyn = (WmlSynResourceDefPtr) wml_synobj_ptr->hvec[ndx].objptr;
    if ( cursyn->validation != WmlResourceDefValid ) continue;
    if ( cursyn->type != WmlResourceTypeReason ) continue;
    newobj = (WmlResourceDefPtr) malloc (sizeof(WmlResourceDef));

/*
 * Create and initialize new object. Append to resolved object vector.
 */
    newobj->syndef = cursyn;
    cursyn->rslvdef = newobj;
    if ( cursyn->int_lit != NULL )
	newobj->tkname = cursyn->int_lit;
    else
	newobj->tkname = cursyn->name;
    newobj->dtype_def = NULL;
    newobj->enumset_def = NULL;
    newobj->related_code = 0;
    wmlInsertInHList (wml_obj_reason_ptr, newobj->tkname, (ObjectPtr)newobj);

/*
 * Validate any object references in the syntactic object
 * Reason can't bind to some objects.
 */
    if ( cursyn->datatype != NULL )
	wmlIssueIllegalReferenceError (cursyn->name, "DataType");

    }

/*
 * All objects are in the vector. That order is the code order, so
 * process it again and assign codes to each object
 */
code = 1;
for ( ndx=0 ; ndx<wml_obj_reason_ptr->cnt ; ndx++ )
    {
    newobj = (WmlResourceDefPtr) wml_obj_reason_ptr->hvec[ndx].objptr;
    newobj->sym_code = code;
    code += 1;
    }

}



/*
 * Routine to linearize and assign sym_k values to arguments.
 *
 * - Generate the wml_obj_arg... vector of resovled reason objects,
 *   ordered lexicographically. Assign a sym_k_... values while doing so.
 *   validate the data type for each argument, and link it to its data type
 *   object.
 *   Do name processing, and acquire links to any other objects named in
 *   the syntactic descriptor.
 */

void wmlResolveSymKArgument ()

{


WmlSynResourceDefPtr	cursyn;		/* current syntactic object */
WmlResourceDefPtr	newobj;		/* new resolved object */
int			ndx;		/* loop index */
int			code;		/* assigned sym_k code value */
char			errmsg[300];

/*
 * Initialize the object vector. Then process the syntactic vector,
 * processing each reason resource object encountered (the vector is ordered).
 * create and append a resolved object for each one encountered. This
 * will be ordered as well.
 */
wmlInitHList (wml_obj_arg_ptr, 500, TRUE);
for ( ndx=0 ; ndx<wml_synobj_ptr->cnt ; ndx++ )
    {
    cursyn = (WmlSynResourceDefPtr) wml_synobj_ptr->hvec[ndx].objptr;
    if ( cursyn->validation != WmlResourceDefValid ) continue;
    if ( cursyn->type == WmlResourceTypeReason ) continue;
    newobj = (WmlResourceDefPtr) malloc (sizeof(WmlResourceDef));

/*
 * Create and initialize new object. Append to resolved object vector.
 */
    newobj->syndef = cursyn;
    cursyn->rslvdef = newobj;
    if ( cursyn->int_lit != NULL )
	newobj->tkname = cursyn->int_lit;
    else
	newobj->tkname = cursyn->name;
    newobj->related_code = 0;
    newobj->enumset_def = NULL;
    wmlInsertInHList (wml_obj_arg_ptr, newobj->tkname, (ObjectPtr)newobj);

/*
 * Validate any object references in the syntactic object
 */
    newobj->dtype_def = (WmlDataTypeDefPtr)
	wmlResolveFindObject (cursyn->datatype,
			      WmlDataTypeDefValid,
			      cursyn->name);
    if ( cursyn->enumset != NULL )
	newobj->enumset_def = (WmlEnumSetDefPtr)
	    wmlResolveFindObject (cursyn->enumset,
				  WmlEnumSetDefValid,
				  cursyn->name);

    }

/*
 * All objects are in the vector. The order is the code order, so
 * process it again and assign codes to each object
 */
code = 1;
for ( ndx=0 ; ndx<wml_obj_arg_ptr->cnt ; ndx++ )
    {
    newobj = (WmlResourceDefPtr) wml_obj_arg_ptr->hvec[ndx].objptr;
    newobj->sym_code = code;
    code += 1;
    }

}



/*
 * Routine to resolve related argument references.
 *
 * Search the argument vector for any argument with its related
 * argument set. Find the related argument, and bind the relation.
 * The binding only goes one way.
 */

void wmlResolveSymKRelated ()

{

WmlResourceDefPtr	srcobj;		/* object with related reference */
WmlResourceDefPtr	dstobj;		/* other object in binding */
WmlSynResourceDefPtr	srcsynobj;	/* source syntactic object */
int			ndx;		/* loop index */


/*
 * Scan all arguments for related argument bindings.
 */
for ( ndx=0 ; ndx<wml_obj_arg_ptr->cnt ; ndx++ )
    {
    srcobj = (WmlResourceDefPtr) wml_obj_arg_ptr->hvec[ndx].objptr;
    srcsynobj = srcobj->syndef;
    if ( srcsynobj->related != NULL )
	{
	dstobj = (WmlResourceDefPtr)
	    wmlResolveFindObject (srcsynobj->related,
				  WmlResourceDefValid,
				  srcsynobj->name);
	if ( dstobj != NULL )
	    srcobj->related_code = dstobj->sym_code;
	}
    }
}

 

/*
 * Routine to linearize and assign sym_k values to classes
 *
 * There are two linearizations of classes:
 *	- all classes in wml_obj_allclass...
 *	- all widgets and gadgets in wml_obj_class...
 * Create and linearize all class objects into these vectors. Assign sym_k
 * codes. Link all subclasses to their superclasses. Perform name processing
 * and link to any other named object.
 *
 * Resources are not inherited and linked at this time.
 */

void wmlResolveSymKClass ()

{

WmlSynClassDefPtr	cursyn;		/* current syntactic object */
WmlClassDefPtr		newobj;		/* new resolved object */
int			ndx;		/* loop index */
int			code;		/* assigned sym_k code value */
char			errmsg[300];


/*
 * Initialize the object vectors. Then process the syntactic vector,
 * processing each class object encountered (the vector is ordered).
 * create and append a resolved object for each one encountered. This
 * will be ordered as well.
 */
wmlInitHList (wml_obj_allclass_ptr, 200, TRUE);
wmlInitHList (wml_obj_class_ptr, 200, TRUE);
for ( ndx=0 ; ndx<wml_synobj_ptr->cnt ; ndx++ )
    {
    cursyn = (WmlSynClassDefPtr) wml_synobj_ptr->hvec[ndx].objptr;
    if ( cursyn->validation != WmlClassDefValid ) continue;

/*
 * Create and initialize new object. Append to resolved object vector.
 */
    newobj = (WmlClassDefPtr) malloc (sizeof(WmlClassDef));
    newobj->syndef = cursyn;
    cursyn->rslvdef = newobj;
    newobj->superclass = NULL;
    newobj->parentclass = NULL;
    if ( cursyn->int_lit != NULL )
	newobj->tkname = cursyn->int_lit;
    else
	newobj->tkname = cursyn->name;
    newobj->inherit_done = FALSE;
    newobj->arguments = NULL;
    newobj->reasons = NULL;
    newobj->controls = NULL;
    newobj->children = NULL;
    newobj->variant = NULL;
    newobj->nondialog = NULL;
    newobj->ctrlmapto = NULL;
    switch ( cursyn->type )
	{
	case WmlClassTypeMetaclass:
	    wmlInsertInHList
		(wml_obj_allclass_ptr, newobj->tkname, (ObjectPtr)newobj);
	    break;
	case WmlClassTypeGadget:
	case WmlClassTypeWidget:
	    wmlInsertInHList
		(wml_obj_allclass_ptr, newobj->tkname, (ObjectPtr)newobj);
	    wmlInsertInHList
		(wml_obj_class_ptr, newobj->tkname, (ObjectPtr)newobj);
	    break;
	}

/*
 * Require a convenience function name
 */
    if ( cursyn->type != WmlClassTypeMetaclass )
	if ( cursyn->convfunc == NULL )
	    {
	    sprintf (errmsg, "Class %s does not have a convenience function",
		     cursyn->name);
	    wmlIssueError (errmsg);
	    }	

/*
 * Validate any object references in the syntactic object
 */
    if ( cursyn->ctrlmapto != NULL )
	newobj->ctrlmapto = (WmlResourceDefPtr)
	    wmlResolveFindObject (cursyn->ctrlmapto,
				  WmlResourceDefValid,
				  cursyn->name);

    }

/*
 * All objects are in the vector. That order is the code order, so
 * process it again and assign codes to each object
 */
code = 1;
for ( ndx=0 ; ndx<wml_obj_class_ptr->cnt ; ndx++ )
    {
    newobj = (WmlClassDefPtr) wml_obj_class_ptr->hvec[ndx].objptr;
    newobj->sym_code = code;
    code += 1;
    }

}



/*
 * Routine to validate controls lists
 *
 * Construct and linearize resolved controls lists. The linearized list
 * is used to resolve references.
 */

void wmlResolveSymKCtrlList ()

{

WmlSynCtrlListDefPtr	cursyn;		/* current syntactic object */
WmlCtrlListDefPtr	newobj;		/* new resolved object */
WmlSynClassCtrlDefPtr	refptr;		/* current controls reference */
WmlClassCtrlDefPtr	ctrlobj;	/* resolved control reference */
WmlClassDefPtr		classobj;	/* the controlled class */
int			ndx;		/* loop index */


/*
 * Process each control list. Construct a resolved control list for each
 */
wmlInitHList (wml_obj_ctrlist_ptr, 20, TRUE);
for ( ndx=0 ; ndx<wml_synobj_ptr->cnt ; ndx++ )
    {
    cursyn = (WmlSynCtrlListDefPtr) wml_synobj_ptr->hvec[ndx].objptr;
    if ( cursyn->validation != WmlCtrlListDefValid ) continue;

/*
 * Create and initialize new object. Append to resolved object vector.
 */
    newobj = (WmlCtrlListDefPtr) malloc (sizeof(WmlCtrlListDef));
    newobj->syndef = cursyn;
    cursyn->rslvdef = newobj;
    newobj->controls = NULL;
    wmlInsertInHList (wml_obj_ctrlist_ptr, cursyn->name, (ObjectPtr)newobj);

/*
 * Validate and construct a resolved controls reference list.
 */
    for ( refptr=cursyn->controls ; refptr!=NULL ; refptr=refptr->next )
	{
	classobj = (WmlClassDefPtr)
	    wmlResolveFindObject (refptr->name,
				  WmlClassDefValid,
				  cursyn->name);
	if ( classobj == NULL ) continue;
	ctrlobj = (WmlClassCtrlDefPtr) malloc (sizeof(WmlClassCtrlDef));
	ctrlobj->next = newobj->controls;
	newobj->controls = ctrlobj;
	ctrlobj->ctrl = classobj;
	}
    }

}



/*
 * Routine to linearize and assign sym_k values to character sets
 *
 * - Generate the wml_obj_charset... vector of resolved data type objects,
 *   ordered lexicographically. Assign a sym_k... value as this is done.
 *   Do name processing, and acquire links to any other objects named in
 *   the syntactic descriptor.
 */

void wmlResolveSymKCharSet ()

{

WmlSynCharSetDefPtr	cursyn;		/* current syntactic object */
WmlCharSetDefPtr	newobj;		/* new resolved object */
int			ndx;		/* loop index */
int			code;		/* assigned sym_k code value */
char			errmsg[300];


/*
 * Initialize the object vector. Then process the syntactic vector,
 * processing each charset object encountered (the vector is ordered).
 * create and append a resolved object for each one encountered. This
 * will be ordered as well.
 */
wmlInitHList (wml_obj_charset_ptr, 50, TRUE);
for ( ndx=0 ; ndx<wml_synobj_ptr->cnt ; ndx++ )
    {
    cursyn = (WmlSynCharSetDefPtr) wml_synobj_ptr->hvec[ndx].objptr;
    if ( cursyn->validation != WmlCharSetDefValid ) continue;

/*
 * Create and initialize new object. Append to resolved object vector.
 */
    newobj = (WmlCharSetDefPtr) malloc (sizeof(WmlCharSetDef));
    newobj->syndef = cursyn;
    cursyn->rslvdef = newobj;
    if ( cursyn->int_lit != NULL )
	newobj->tkname = cursyn->int_lit;
    else
	newobj->tkname = cursyn->name;
    wmlInsertInHList (wml_obj_charset_ptr, newobj->tkname, (ObjectPtr)newobj);

/*
 * Parsing direction defaults to writing direction if unspecified
 */
    if ( cursyn->parsedirection == WmlAttributeUnspecified )
	cursyn->parsedirection = cursyn->direction;

/*
 * Require StandardsName attribute for character set
 */
    if ( cursyn->xms_name == NULL )
	{
	sprintf (errmsg, "CharacterSet %s does not have a StandardsName",
		 cursyn->name);
	wmlIssueError (errmsg);
	}	

    }

/*
 * All objects are in the vector. That order is the code order, so
 * process it again and assign codes to each object. We start at code
 * 2 since 1 is reserved for sym_k_userdefined_charset
 */
code = 2;
for ( ndx=0 ; ndx<wml_obj_charset_ptr->cnt ; ndx++ )
    {
    newobj = (WmlCharSetDefPtr) wml_obj_charset_ptr->hvec[ndx].objptr;
    newobj->sym_code = code;
    code += 1;
    }

}



/*
 * Routine to perform class inheritance and validation.
 *
 * This routine has two major phases:
 *	- Complete resolution of class references, and expand out
 *	  controls list.
 *	- Perform inheritance of resources, partitioning them into
 *	  into arguments and reasons. When complete, the class has
 *	  a list of all its resources, including copies from a 
 *	  superclass and possibly a parentclass.
 *	  Excluded resources remain in the list, and are simply marked.
 */

void wmlResolveValidateClass ()

{

int			ndx;		/* loop index */
int			max;		/* maximum code value */
WmlClassDefPtr		clsobj;		/* current class object */
WmlSynClassDefPtr	synobj;		/* syntactic class object */
WmlClassDefPtr		superobj;	/* superclass */
WmlClassDefPtr		parentobj;	/* parentclass */
WmlClassDefPtr		widgobj;	/* gadget's widget class */
WmlSynClassCtrlDefPtr	refptr;		/* current controls reference */
WmlClassCtrlDefPtr	ctrlobj;	/* resolved control reference */
int			refndx;		/* index in vector */
WmlClassDefPtr		refcls;		/* referenced class object */
WmlCtrlListDefPtr	reflist;	/* controls list object */
WmlClassCtrlDefPtr	listelem;	/* control reference in list */
char			errmsg[300];


/*
 * Acquire the superclass pointer for each widget and gadget class
 */
for ( ndx=0 ; ndx<wml_obj_allclass_ptr->cnt ; ndx++ )
    {
    clsobj = (WmlClassDefPtr) wml_obj_allclass_ptr->hvec[ndx].objptr;
    synobj = clsobj->syndef;
    if ( synobj->superclass != NULL )
	{
	superobj = (WmlClassDefPtr)
	    wmlResolveFindObject (synobj->superclass,
				  WmlClassDefValid,
				  synobj->name);
	if ( superobj == NULL ) continue;
	clsobj->superclass = superobj;
	}
    }

/*
 * Acquire the parentclass pointer (if one exists),
 * for each widget and gadget class
 */
for ( ndx=0 ; ndx<wml_obj_allclass_ptr->cnt ; ndx++ )
    {
    clsobj = (WmlClassDefPtr) wml_obj_allclass_ptr->hvec[ndx].objptr;
    synobj = clsobj->syndef;
    if ( synobj->parentclass != NULL )
	{
	parentobj = (WmlClassDefPtr)
	    wmlResolveFindObject (synobj->parentclass,
				  WmlClassDefValid,
				  synobj->name);
	if ( parentobj == NULL ) continue;
	clsobj->parentclass = parentobj;
	}
    }

/*
 * Link each gadget class with its widget class (both ways).
 * Link any class with a non-dialog version to the non-dialog class.
 */
for ( ndx=0 ; ndx<wml_obj_class_ptr->cnt ; ndx++ )
    {
    clsobj = (WmlClassDefPtr) wml_obj_class_ptr->hvec[ndx].objptr;
    synobj = clsobj->syndef;
    if ( synobj->type == WmlClassTypeGadget )
	{
	if ( synobj->widgetclass == NULL )
	    {
	    sprintf (errmsg, "Gadget class %s has no widgetclass reference",
		     synobj->name);
	    wmlIssueError (errmsg);
	    }
	else
	    {
	    widgobj = (WmlClassDefPtr)
		wmlResolveFindObject
		    (synobj->widgetclass,
		     WmlClassDefValid,
		     synobj->name);
	    if ( widgobj != NULL )
		{
		clsobj->variant = widgobj;
		widgobj->variant = clsobj;
		}
	    }
	}
    if ( synobj->dialog == TRUE )
	{
	clsobj->nondialog = clsobj->superclass;
	while ( clsobj->nondialog->syndef->dialog == TRUE )
	    clsobj->nondialog = clsobj->nondialog->superclass;
	}
    else
	if ( clsobj->superclass != NULL )
	    {
	    synobj->dialog = clsobj->superclass->syndef->dialog;
	    clsobj->nondialog = clsobj->superclass->nondialog;
	    }
    }

/*
 * Construct the list of resolved controls. Control lists are expanded
 * in place.
 */
for ( ndx=0 ; ndx<wml_obj_class_ptr->cnt ; ndx++ )
    {
    clsobj = (WmlClassDefPtr) wml_obj_class_ptr->hvec[ndx].objptr;
    synobj = clsobj->syndef;
    for ( refptr=synobj->controls ; refptr!= NULL ; refptr=refptr->next )
	{
	refndx = wmlFindInHList (wml_obj_class_ptr, refptr->name);
	if ( refndx >= 0 )
	    {
	    refcls = (WmlClassDefPtr) wml_obj_class_ptr->hvec[refndx].objptr;
	    ctrlobj = (WmlClassCtrlDefPtr) malloc (sizeof(WmlClassCtrlDef));
	    ctrlobj->next = clsobj->controls;
	    clsobj->controls = ctrlobj;
	    ctrlobj->ctrl = refcls;
	    continue;
	    }
	refndx = wmlFindInHList (wml_obj_ctrlist_ptr, refptr->name);
	if ( refndx >= 0 )
	    {
	    reflist = (WmlCtrlListDefPtr)
		wml_obj_ctrlist_ptr->hvec[refndx].objptr;
	    for ( listelem=reflist->controls ;
		  listelem!=NULL ;
		  listelem=listelem->next)
		{
		ctrlobj = (WmlClassCtrlDefPtr)
		    malloc (sizeof(WmlClassCtrlDef));
		ctrlobj->next = clsobj->controls;
		clsobj->controls = ctrlobj;
		ctrlobj->ctrl = listelem->ctrl;
		}
	    continue;
	    }
	wmlIssueReferenceError (synobj->name, refptr->name);
	continue;
	}
    }

/*
 * Perform resource inheritance for each class. This constructs the
 * arguments and reasons reference vectors.
 */
for ( ndx=0 ; ndx<wml_obj_allclass_ptr->cnt ; ndx++ )
    {
    clsobj = (WmlClassDefPtr) wml_obj_allclass_ptr->hvec[ndx].objptr;
    wmlResolveClassInherit (clsobj);
    }

}



/*
 * Routine to perform resource inheritance for a class.
 *
 * This routine constructs the argument and reason resource and child reference
 * vectors for a class. It first ensures the superclass (if any) has
 * been inited. It then makes a copy of the superclass lists. It repeats this
 * procedure for the parentclass (if any.) Finally, it
 * merges in the resources from the syntactic object. It uses the
 * resolved resource or child object to point to the matching reference object 
 * in the list being created as an aid to search doing overrides. This also
 * detects whether a resource or child is already in the list (if so, it is 
 * assumed to be inherited).
 */

void wmlResolveClassInherit (clsobj)
    WmlClassDefPtr		clsobj;

{

WmlClassDefPtr		superobj;	/* superclass object */
WmlClassDefPtr		parentobj;	/* parentclass object */
int			ndx;		/* loop index */
WmlResourceDefPtr	resobj;		/* current resource object */
WmlClassResDefPtr	refobj;		/* current resource reference */
WmlClassResDefPtr	srcref;		/* source of copy */
WmlChildDefPtr		childobj;	/* current child object */
WmlClassChildDefPtr	crefobj;	/* current child reference */
WmlClassChildDefPtr	csrcref;	/* child source of copy */
WmlSynClassDefPtr	synobj;		/* this class' syntactic object */
WmlSynClassResDefPtr	refptr;		/* syntactic resource reference */
WmlSynClassChildDefPtr	crefptr;	/* syntactic child reference */


/*
 * Done if inheritance previously performed. Ensure the superclass is
 * done.
 */
if ( clsobj == NULL ) return;
if ( clsobj->inherit_done ) return;
superobj = clsobj->superclass;
wmlResolveClassInherit (superobj);
parentobj = clsobj->parentclass;
wmlResolveClassInherit (parentobj);
synobj = clsobj->syndef;

/*
 * Clear the active reference pointer in the resolved resource objects.
 */
wmlResolveClearRefPointers ();

/*
 * Copy the superclass resources, setting the reference pointer as we go.
 */
if ( superobj != NULL )
    {
    for ( srcref=superobj->arguments ; srcref!=NULL ; srcref=srcref->next )
	{
	refobj = (WmlClassResDefPtr) malloc (sizeof(WmlClassResDef));
	refobj->next = clsobj->arguments;
	clsobj->arguments = refobj;
	wmlResolveInitRefObj (refobj, srcref);
	}
    for ( srcref=superobj->reasons ; srcref!=NULL ; srcref=srcref->next )
	{
	refobj = (WmlClassResDefPtr) malloc (sizeof(WmlClassResDef));
	refobj->next = clsobj->reasons;
	clsobj->reasons = refobj;
	wmlResolveInitRefObj (refobj, srcref);
	}
    for (csrcref = superobj->children ; csrcref!=NULL ; csrcref=csrcref->next)
      {
	crefobj = (WmlClassChildDefPtr) malloc (sizeof(WmlClassChildDef));
	crefobj->next = clsobj->children;
	clsobj->children = crefobj;
	wmlResolveInitChildRefObj (crefobj, csrcref);
      }
    }

/*
 * Copy the parentclass resources, setting the reference pointer as we go.
 */
if ( parentobj != NULL )
    {
    for ( srcref=parentobj->arguments ; srcref!=NULL ; srcref=srcref->next )
	{
	  if (srcref->act_resource->ref_ptr == NULL)
	    {
	      refobj = (WmlClassResDefPtr) malloc (sizeof(WmlClassResDef));
	      refobj->next = clsobj->arguments;
	      clsobj->arguments = refobj;
	      wmlResolveInitRefObj (refobj, srcref);
	    }
	}
    for ( srcref=parentobj->reasons ; srcref!=NULL ; srcref=srcref->next )
	{
	  if (srcref->act_resource->ref_ptr == NULL)
	    {
	      refobj = (WmlClassResDefPtr) malloc (sizeof(WmlClassResDef));
	      refobj->next = clsobj->reasons;
	      clsobj->reasons = refobj;
	      wmlResolveInitRefObj (refobj, srcref);
	    }
	}
    for (csrcref = parentobj->children ; csrcref!=NULL ; csrcref=csrcref->next)
      {
	if (csrcref->act_child->ref_ptr == NULL)
	  {
	    crefobj = (WmlClassChildDefPtr) malloc (sizeof(WmlClassChildDef));
	    crefobj->next = clsobj->children;
	    clsobj->children = crefobj;
	    wmlResolveInitChildRefObj (crefobj, csrcref);
	  }
      }
    }

/*
 * Process the resources belonging to this class. They may either be
 * new resources, or override ones already in the list. Partition them
 * into arguments and reasons.
 */
for ( refptr=synobj->resources ; refptr!=NULL ; refptr=refptr->next )
    {
    resobj = (WmlResourceDefPtr) wmlResolveFindObject (refptr->name,
						       WmlResourceDefValid,
						       synobj->name);
    if ( resobj == NULL ) continue;

    /*
     * Acquire the resolved resource object, and the resource reference.
     * New references are linked in to the proper list, and have their
     * defaults set.
     */
    if ( resobj->ref_ptr != NULL )
	refobj = resobj->ref_ptr;
    else
	{
	refobj = (WmlClassResDefPtr) malloc (sizeof(WmlClassResDef));
	refobj->act_resource = resobj;
	resobj->ref_ptr = refobj;
	refobj->over_dtype = NULL;
	refobj->dflt = NULL;
	refobj->exclude = WmlAttributeUnspecified;
	if ( resobj->syndef->type == WmlResourceTypeReason )
	    {
	    refobj->next = clsobj->reasons;
	    clsobj->reasons = refobj;
	    }
	else
	    {
	    refobj->next = clsobj->arguments;
	    clsobj->arguments = refobj;
	    }
	}

    /*
     * Override any values in the reference which are explicit in the
     * syntactic reference.
     */
    if ( refptr->type != NULL )
	refobj->over_dtype = (WmlDataTypeDefPtr)
	    wmlResolveFindObject (refptr->type,
				  WmlDataTypeDefValid,
				  synobj->name);
    if ( refptr->dflt != NULL )
	refobj->dflt = refptr->dflt;
    if ( refptr->exclude != WmlAttributeUnspecified )
	refobj->exclude = refptr->exclude;
    }

/*
 * Process the children belonging to this class. 
 */
for ( crefptr = synobj->children ; crefptr!=NULL ; crefptr = crefptr->next )
  {
    childobj = (WmlChildDefPtr) wmlResolveFindObject (crefptr->name,
						     WmlChildDefValid,
						     synobj->name);
    if ( childobj == NULL ) continue;

    /*
     * Acquire the resolved child object, and the child reference.
     * New references are linked in to the proper list, and have their
     * defaults set.
     */
    if ( childobj->ref_ptr != NULL )
	crefobj = childobj->ref_ptr;
    else
      {
	crefobj = (WmlClassChildDefPtr) malloc (sizeof(WmlClassChildDef));
	crefobj->act_child = childobj;
	childobj->ref_ptr = crefobj;
	crefobj->next = clsobj->children;
	clsobj->children = crefobj;
      }
  }

/*
 * inheritance complete
 */
clsobj->inherit_done = TRUE;

}



/*
 * Routine to copy a resource reference
 */

void wmlResolveInitRefObj (dstobj, srcobj)
    WmlClassResDefPtr		dstobj;
    WmlClassResDefPtr		srcobj;

{

WmlResourceDefPtr	resobj;


resobj = srcobj->act_resource;
dstobj->act_resource = resobj;
resobj->ref_ptr = dstobj;
dstobj->over_dtype = srcobj->over_dtype;
dstobj->dflt = srcobj->dflt;
dstobj->exclude = srcobj->exclude;

}


/*
 * Routine to copy a child reference
 */

void wmlResolveInitChildRefObj (dstobj, srcobj)
    WmlClassChildDefPtr		dstobj;
    WmlClassChildDefPtr		srcobj;

{

WmlChildDefPtr	childobj;

childobj = srcobj->act_child;
dstobj->act_child = childobj;
childobj->ref_ptr = dstobj;

}



/*
 * Routine to print a report in a file.
 *
 * This routine dumps the developed database into the file 'wml.report'
 */

void wmlResolvePrintReport ()

{

FILE			*outfil;	/* output file */
int			ndx;		/* loop index */
WmlClassDefPtr		clsobj;		/* current class */


/*
 * Open the output file.
 */
outfil = fopen ( "wml.report", "w");
if ( outfil == NULL )
    {
    printf ("\nCouldn't open wml.report");
    return;
    }

/*
 * Go through all classes. Print basic information, then dump their
 * resources. The main purpose of this report is to show the actual
 * resources and controls for the class.
 */

for ( ndx=0 ; ndx<wml_obj_allclass_ptr->cnt ; ndx++ )
    {
    clsobj = (WmlClassDefPtr) wml_obj_allclass_ptr->hvec[ndx].objptr;
    wmlMarkReferencePointers (clsobj);
    wmlResolvePrintClass (outfil, clsobj);
    }


/*
 * close the output file
 */
fprintf (outfil, "\n\n");
printf ("\nCreated report file wml.report");
fclose (outfil);

}



/*
 * Print the information for a class
 */

void wmlResolvePrintClass (outfil, clsobj)
    FILE			*outfil;
    WmlClassDefPtr		clsobj;

{

int			ndx;		/* loop index */
WmlSynClassDefPtr	synobj;		/* syntactic object */
WmlClassCtrlDefPtr	ctrlref;	/* controls reference */
WmlClassDefPtr		ctrlobj;	/* current class in control */


synobj = clsobj->syndef;
fprintf (outfil, "\n\n\nClass %s:", synobj->name);
switch ( synobj->type )
    {
    case WmlClassTypeMetaclass:
        fprintf (outfil, "\n  Type: Metaclass\t");
	if ( synobj->superclass != NULL )
	    fprintf (outfil, "Superclass: %s\t", synobj->superclass);
	if ( synobj->parentclass != NULL )
	    fprintf (outfil, "Parentclass: %s\t", synobj->parentclass);
	break;
    case WmlClassTypeWidget:
	fprintf (outfil, "\n  Type: Widget\t");
	if ( synobj->superclass != NULL )
	    fprintf (outfil, "Superclass: %s\t", synobj->superclass);
	if ( synobj->parentclass != NULL )
	    fprintf (outfil, "Parentclass: %s\t", synobj->parentclass);
	if ( clsobj->variant != NULL )
	    fprintf (outfil, "\n  Associated gadget class: %s\t",
		     clsobj->variant->syndef->name);
	if ( synobj->convfunc != NULL )
	    fprintf (outfil, "Convenience function: %s", synobj->convfunc);
	break;
    case WmlClassTypeGadget:
	fprintf (outfil, "\n  Type: Gadget\t");
	if ( synobj->superclass != NULL )
	    fprintf (outfil, "Superclass: %s\t", synobj->superclass);
	if ( synobj->parentclass != NULL )
	    fprintf (outfil, "Parentclass: %s\t", synobj->parentclass);
	if ( clsobj->variant != NULL )
	    fprintf (outfil, "\n  Associated widget class: %s\t",
		     clsobj->variant->syndef->name);
	if ( synobj->convfunc != NULL )
	    fprintf (outfil, "Convenience function: %s", synobj->convfunc);
	break;
    }

/*
 * Print associated non-dialog class
 */
if ( clsobj->nondialog != NULL )
    fprintf (outfil, "\n  DialogClass: True\tNon-dialog ancestor: %s\t",
	     clsobj->nondialog->syndef->name);

/*
 * Print the arguments valid in the class. First the new resources for the
 * class are printed, then each ancestor's contribution is printed. This
 * is intended to match the way resources are printed in the toolkit manual,
 * so that checking is as easy as possible.
 */
fprintf (outfil, "\n  Arguments:");
wmlResolvePrintClassArgs (outfil, clsobj);

/*
 * Print the reasons valid in the class
 */
fprintf (outfil, "\n  Reasons:");
wmlResolvePrintClassReasons (outfil, clsobj);

/*
 * Print the controls valid in the class
 */
fprintf (outfil, "\n  Controls:");
for ( ndx=0 ; ndx<wml_obj_class_ptr->cnt ; ndx++ )
    {
    ctrlobj = (WmlClassDefPtr) wml_obj_class_ptr->hvec[ndx].objptr;
    if ( ctrlobj->ref_ptr == NULL ) continue;
    fprintf (outfil, "\n    %s", ctrlobj->syndef->name);
    }

}



/*
 * Routine to print the arguments for a class
 *
 * This routine prints out the currently marked arguments which are
 * present in this class. Each argument which is printed is remarked
 * so that it won't be printed again. This routine first prints the
 * superclass arguments, so that the printing order becomes the top-down
 * inheritance order.
 */

void wmlResolvePrintClassArgs (outfil, clsobj)
    FILE			*outfil;
    WmlClassDefPtr		clsobj;

{

int			prthdr = TRUE;	/* print header line */
int			ndx;		/* loop index */
WmlSynClassDefPtr	synobj;		/* syntactic object */
WmlClassResDefPtr	resref;		/* resource reference */
int			constr = FALSE;	/* check for constraints */
WmlResourceDefPtr	resobj;		/* current resource */
WmlSynResourceDefPtr	synres;		/* syntactic resource object */


/*
 * Print the superclass arguments
 */
if ( clsobj->superclass != NULL )
    wmlResolvePrintClassArgs (outfil, clsobj->superclass);

/*
 * Print the parentclass arguments
 */
if ( clsobj->parentclass != NULL )
    wmlResolvePrintClassArgs (outfil, clsobj->parentclass);

/*
 * Print the arguments for this class. Unmark the reference so it won't
 * be printed again.
 */
synobj = clsobj->syndef;
for ( ndx=0 ; ndx<wml_obj_arg_ptr->cnt ; ndx++ )
    {
    resobj = (WmlResourceDefPtr) wml_obj_arg_ptr->hvec[ndx].objptr;
    resref = resobj->ref_ptr;
    if ( resref == NULL ) continue;
    if ( wmlResolveResIsMember(resobj,clsobj->arguments) == NULL ) continue;
    synres = resobj->syndef;
    switch ( synres->type )
	{
	case WmlResourceTypeArgument:
	case WmlResourceTypeSubResource:
	    break;
	case WmlResourceTypeConstraint:
	    constr = TRUE;
	    break;
	default:
	    continue;
	    break;
	}
    if ( prthdr )
	{
	fprintf (outfil, "\n    %s argument set:", synobj->name);
	prthdr = FALSE;
	}
	
    fprintf (outfil, "\n      %s", synres->name);
    fprintf (outfil, "\n\tType = %s", resobj->dtype_def->syndef->name);
    if ( strcmp(synres->name,synres->resliteral) != 0 )
	fprintf (outfil, "\tResourceLiteral = %s", synres->resliteral);
    switch ( resref->exclude )
	{
	case WmlAttributeTrue:
	    fprintf (outfil, "\n\tExclude = True;");
	    break;
	case WmlAttributeFalse:
	    fprintf (outfil, "\n\tExclude = False;");
	    break;
	}
    if ( resref->dflt != NULL )
	fprintf (outfil, "\n\tDefault = \"%s\"", resref->dflt);
    else
	if ( synres->dflt != NULL )
	    fprintf (outfil, "\n\tDefault = \"%s\"", synres->dflt);
    resobj->ref_ptr = NULL;
    }

/*
 * Print the constraints valid in the class
 */
if ( constr )
    {
    prthdr = TRUE;
    for ( ndx=0 ; ndx<wml_obj_arg_ptr->cnt ; ndx++ )
	{
	resobj = (WmlResourceDefPtr) wml_obj_arg_ptr->hvec[ndx].objptr;
	resref = resobj->ref_ptr;
	if ( resref == NULL ) continue;
	if ( wmlResolveResIsMember(resobj,clsobj->arguments) == NULL ) continue;
	synres = resobj->syndef;
	switch ( synres->type )
	    {
	    case WmlResourceTypeConstraint:
	        break;
	    default:
		continue;
		break;
	    }
	if ( prthdr )
	    {
	    fprintf (outfil, "\n    %s constraint set:", synobj->name);
	    prthdr = FALSE;
	    }
	fprintf (outfil, "\n      %s", synres->name);
	if ( strcmp(synres->name,synres->resliteral) != 0 )
	    fprintf (outfil, "\tResourceLiteral = %s", synres->resliteral);
	switch ( resref->exclude )
	    {
	    case WmlAttributeTrue:
	        fprintf (outfil, "\n\tExclude = True;");
		break;
	    case WmlAttributeFalse:
		fprintf (outfil, "\n\tExclude = False;");
		break;
	    }
	if ( resref->dflt != NULL )
	    fprintf (outfil, "\n\tDefault = \"%s\"", resref->dflt);
	else
	    if ( synres->dflt != NULL )
		fprintf (outfil, "\n\tDefault = \"%s\"", synres->dflt);
	resobj->ref_ptr = NULL;
	}
    }

}



/*
 * Routine to print reasons in a class.
 *
 * Like printing arguments, only reasons instead.
 */

void wmlResolvePrintClassReasons (outfil, clsobj)
    FILE			*outfil;
    WmlClassDefPtr		clsobj;

{

int			prthdr = TRUE;	/* print header flag */
int			ndx;		/* loop index */
WmlSynClassDefPtr	synobj;		/* syntactic object */
WmlClassResDefPtr	resref;		/* resource reference */
WmlResourceDefPtr	resobj;		/* current resource */
WmlSynResourceDefPtr	synres;		/* syntactic resource object */


/*
 * Print the superclass reasons
 */
if ( clsobj->superclass != NULL )
    wmlResolvePrintClassReasons (outfil, clsobj->superclass);

/*
 * Print the parentclass reasons
 */
if ( clsobj->parentclass != NULL )
    wmlResolvePrintClassReasons (outfil, clsobj->parentclass);

/*
 * Print the reasons for this class. Unmark the reference so it won't
 * be printed again.
 */
synobj = clsobj->syndef;
for ( ndx=0 ; ndx<wml_obj_reason_ptr->cnt ; ndx++ )
    {
    resobj = (WmlResourceDefPtr) wml_obj_reason_ptr->hvec[ndx].objptr;
    resref = resobj->ref_ptr;
    if ( resref == NULL ) continue;
    if ( wmlResolveResIsMember(resobj,clsobj->reasons) == NULL ) continue;
    synres = resobj->syndef;
    if ( prthdr )
	{
	fprintf (outfil, "\n    %s reason set:", synobj->name);
	prthdr = FALSE;
	}
    fprintf (outfil, "\n      %s", synres->name);
    if ( strcmp(synres->name,synres->resliteral) != 0 )
	fprintf (outfil, "\tResourceLiteral = %s", synres->resliteral);
    switch ( resref->exclude )
	{
	case WmlAttributeTrue:
	    fprintf (outfil, "\n\tExclude = True;");
	    break;
	case WmlAttributeFalse:
	    fprintf (outfil, "\n\tExclude = False;");
	    break;
	}
    resobj->ref_ptr = NULL;
    }
}



/*
 * Routine to mark reference pointers for a class
 *
 * This routine clears all reference pointers, then marks the class and
 * resource objects to flag those contained in the current class. This
 * allows processing of the widget and resource vectors in order to produce
 * bit masks or reports.
 */

void wmlMarkReferencePointers (clsobj)
    WmlClassDefPtr		clsobj;

{

int			ndx;		/* loop index */
WmlClassDefPtr		mrkcls;		/* class object to mark */
WmlResourceDefPtr	mrkres;		/* resource object to mark */
WmlClassResDefPtr	resref;		/* resource reference */
WmlClassCtrlDefPtr	ctrlref;	/* controls reference */


/*
 * Clear the reference pointers. Then go through the arguments, reasons,
 * and controls lists, and mark the referenced classes.
 */
wmlResolveClearRefPointers ();
for ( resref=clsobj->arguments ; resref!= NULL ; resref=resref->next )
    resref->act_resource->ref_ptr = resref;
for ( resref=clsobj->reasons ; resref!= NULL ; resref=resref->next )
    resref->act_resource->ref_ptr = resref;
for ( ctrlref=clsobj->controls ; ctrlref!=NULL ; ctrlref=ctrlref->next )
    ctrlref->ctrl->ref_ptr = ctrlref;

}



/*
 * Routine to clear reference pointers
 */

void wmlResolveClearRefPointers ()

{

int			ndx;		/* loop index */
WmlClassDefPtr		mrkcls;		/* class object to mark */
WmlResourceDefPtr	mrkres;		/* resource object to mark */
WmlChildDefPtr		mrkcld;		/* child object to mark */

for ( ndx=0 ; ndx<wml_obj_allclass_ptr->cnt ; ndx++ )
    {
    mrkcls = (WmlClassDefPtr) wml_obj_allclass_ptr->hvec[ndx].objptr;
    mrkcls->ref_ptr = NULL;
    }
for ( ndx=0 ; ndx<wml_obj_reason_ptr->cnt ; ndx++ )
    {
    mrkres = (WmlResourceDefPtr) wml_obj_reason_ptr->hvec[ndx].objptr;
    mrkres->ref_ptr = NULL;
    }
for ( ndx=0 ; ndx<wml_obj_arg_ptr->cnt ; ndx++ )
    {
    mrkres = (WmlResourceDefPtr) wml_obj_arg_ptr->hvec[ndx].objptr;
    mrkres->ref_ptr = NULL;
    }
for ( ndx=0 ; ndx<wml_obj_child_ptr->cnt ; ndx++ )
    {
    mrkcld = (WmlChildDefPtr) wml_obj_child_ptr->hvec[ndx].objptr;
    mrkcld->ref_ptr = NULL;
    }

}



/*
 * Routine to find an object for binding. The name is always looked
 * in the syntactic object list, since all references made by the
 * user are in that list (resolved objects may be entered under
 * an internal literal, and not be found). This routine always attempts to
 * return a resolved object (which depends on object type). It also guarantees
 * that the object it finds matches the given type.
 *
 *	name		the object to be found
 *	type		type the object should match
 *	requester	requester name, for error messages
 *
 * Returns:	pointer to the object found
 */

ObjectPtr wmlResolveFindObject (name, type, requester)
    char			*name;
    int				type;
    char			*requester;

{

int			objndx;		/* the object index in the list */
WmlSynDefPtr		synobj;		/* syntactic object */
char			errmsg[300];


objndx = wmlFindInHList (wml_synobj_ptr, name);
if ( objndx < 0 )
    {
    wmlIssueReferenceError (requester, name);
    return NULL;
    }
synobj = (WmlSynDefPtr) wml_synobj_ptr ->hvec[objndx].objptr;
if ( synobj->validation != type )
    {
    sprintf (errmsg,
	     "Object %s references object %s\n\tin a context where a different type of object is required",
	    requester, name);
    wmlIssueError (errmsg);
    return NULL;
    }
switch ( synobj->validation )
    {
    case WmlClassDefValid:
    case WmlResourceDefValid:
    case WmlDataTypeDefValid:
    case WmlCtrlListDefValid:
    case WmlEnumSetDefValid:
    case WmlEnumValueDefValid:
    case WmlChildDefValid:
        return (ObjectPtr) synobj->rslvdef;
	break;
    default:
	return (ObjectPtr) synobj;
    }

}


/*
 * Report an object reference error
 *
 *	srcname		the object making the reference
 *	badname		the missing object
 */

void wmlIssueReferenceError (srcname, badname)
    char			*srcname;
    char			*badname;

{

printf ("\nObject %s references undefined object %s", srcname, badname);
wml_err_count += 1;

}


/*
 * Report an attempt to make a reference which is not supported.
 */

void wmlIssueIllegalReferenceError (srcname, badname)
    char			*srcname;
    char			*badname;

{

printf ("\nObject %s cannot reference a %s object", srcname, badname);
wml_err_count += 1;

}


/*
 * Report an error string.
 */
void wmlIssueError (errstg)
    char			*errstg;

{

printf ("\n%s", errstg);
wml_err_count += 1;

}
