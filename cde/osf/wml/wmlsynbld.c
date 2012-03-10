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
static char rcsid[] = "$XConsortium: wmlsynbld.c /main/9 1995/08/29 11:11:12 drk $"
#endif
#endif
/*
*  (c) Copyright 1989, 1990, DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASS. */

/*
 * This module contains the programs which construct the syntactic
 * representation of the WML input. All the routines are called as
 * actions of the grammar productions.
 *
 * Since WML is so simple, no stack frame technology is used. Instead,
 * context is maintained by global pointers and vectors which contain
 * the intermediate results of parsing a statement. At most, these
 * contain an object being constructed (for instance a class descriptor)
 * and a subobject (for instance a resource reference in a class).
 *
 * Results are communicated back using the global error count
 * wml_err_count, and the ordered handle list wml_synobj_ptr.
 */


#include "wml.h"
#include "wmlparse.h"

#if defined(__STDC__)
#include <stdlib.h>
#endif
#include <stdio.h>


/*
 * Globals used during WML parsing.
 */

/*
 * Character arrays and other variables to hold lexemes
 * are defined in wmllex.l
 */

/*
 * Current principal object being constructed
 * Current subobject
 */
ObjectPtr	wml_cur_obj;
ObjectPtr	wml_cur_subobj;



/*
 * Routine to create a class descriptor. The result is placed in both
 * wml_cur_obj and wml_synobj.
 *
 *	name		the class name
 *	ctype		class type, one of METACLASS | WIDGET | GADGET
 */

void wmlCreateClass (name, ctype)
    char		*name;
    int			ctype;

{

WmlSynClassDefPtr	cdesc;		/* new class descriptor */


/*
 * Initialize the new class descriptor. Enter it in the object list.
 * Set the current object global to the descriptor.
 */
cdesc = (WmlSynClassDefPtr) malloc (sizeof(WmlSynClassDef));
cdesc->validation = WmlClassDefValid;
cdesc->rslvdef = NULL;
switch ( ctype )
    {
    case METACLASS:
        cdesc->type = WmlClassTypeMetaclass;
	break;
    case WIDGET:
        cdesc->type = WmlClassTypeWidget;
	break;
    case GADGET:
        cdesc->type = WmlClassTypeGadget;
	break;
    default:
	printf ("\nwmlCreateClass: unknown class type %d", ctype);
	return;
	break;
    }
cdesc->dialog = FALSE;
cdesc->name = wmlAllocateString (name);
cdesc->superclass = NULL;
cdesc->parentclass = NULL;
cdesc->widgetclass = NULL;
cdesc->int_lit = NULL;
cdesc->convfunc = NULL;
cdesc->docname = NULL;
cdesc->ctrlmapto = NULL;
cdesc->controls = NULL;
cdesc->resources = NULL;
cdesc->children = NULL;

if ( wmlFindInHList(wml_synobj_ptr,name) >= 0 )
    {
    printf ("\nDuplicate name %s found", name);
    return;
    }
wmlInsertInHList (wml_synobj_ptr, name, (ObjectPtr)cdesc);
wml_cur_obj = (ObjectPtr) cdesc;
wml_cur_subobj = NULL;

return;

}



/*
 * Routine to create a resource descriptor. The result is placed in both
 * wml_cur_obj and wml_synobj.
 *
 *	name		the resource name
 *	rtype		resource type, one of
 *			ARGUMENT | REASON | CONSTRAINT | SUBRESOURCE
 */

void wmlCreateResource (name, rtype)
    char		*name;
    int			rtype;

{

WmlSynResourceDefPtr	rdesc;		/* new resource descriptor */


/*
 * Initialize the new resource descriptor. Enter it in the object list.
 * Set the current object global to the descriptor.
 */
rdesc = (WmlSynResourceDefPtr) malloc (sizeof(WmlSynResourceDef));
rdesc->validation = WmlResourceDefValid;
rdesc->rslvdef = NULL;
switch ( rtype )
    {
    case ARGUMENT:
        rdesc->type = WmlResourceTypeArgument;
	rdesc->xrm_support = WmlAttributeTrue;
	break;
    case REASON:
        rdesc->type = WmlResourceTypeReason;
	rdesc->xrm_support = WmlAttributeFalse;
	break;
    case CONSTRAINT:
        rdesc->type = WmlResourceTypeConstraint;
	rdesc->xrm_support = WmlAttributeTrue;
	break;
    case SUBRESOURCE:
        rdesc->type = WmlResourceTypeSubResource;
	rdesc->xrm_support = WmlAttributeTrue;
	break;
    default:
	printf ("\nwmlCreateResource: unknown resource type %d", rtype);
	return;
	break;
    }
rdesc->name = wmlAllocateString (name);
rdesc->datatype = NULL;
rdesc->int_lit = NULL;
rdesc->resliteral = wmlAllocateString (name);	/* default to name */
rdesc->enumset = NULL;
rdesc->docname = NULL;
rdesc->related = NULL;
rdesc->dflt = NULL;
rdesc->alias_cnt = 0;
rdesc->alias_list = NULL;

if ( wmlFindInHList(wml_synobj_ptr,name) >= 0 )
    {
    printf ("\nDuplicate name %s found", name);
    return;
    }
wmlInsertInHList (wml_synobj_ptr, name, (ObjectPtr)rdesc);
wml_cur_obj = (ObjectPtr) rdesc;
wml_cur_subobj = NULL;

return;

}



/*
 * Routine to create a datatype descriptor. The result is placed in both
 * wml_cur_obj and wml_synobj.
 *
 *	name		the datatype name
 */

void wmlCreateDatatype (name)
    char		*name;

{

WmlSynDataTypeDefPtr	ddesc;		/* new datatype descriptor */


/*
 * Initialize the new datatype descriptor. Enter it in the object list.
 * Set the current object global to the descriptor.
 */
ddesc = (WmlSynDataTypeDefPtr) malloc (sizeof(WmlSynDataTypeDef));
ddesc->validation = WmlDataTypeDefValid;
ddesc->rslvdef = NULL;
ddesc->name = wmlAllocateString (name);
ddesc->int_lit = NULL;
ddesc->docname = NULL;
ddesc->xrm_support = WmlAttributeTrue;

if ( wmlFindInHList(wml_synobj_ptr,name) >= 0 )
    {
    printf ("\nDuplicate name %s found", name);
    return;
    }
wmlInsertInHList (wml_synobj_ptr, name, (ObjectPtr)ddesc);
wml_cur_obj = (ObjectPtr) ddesc;
wml_cur_subobj = NULL;

return;

}


/*
 * Routine to create a child descriptor. The result is placed in both
 * wml_cur_obj and wml_synobj.
 *
 *	name		the child name
 *	class		the class name
 */

void wmlCreateChild (name, class)
     char		*name;
     char		*class;
{

WmlSynChildDefPtr	chdesc;		/* new child descriptor */


/*
 * Initialize the new child descriptor. Enter it in the object list.
 * Set the current object global to the descriptor.
 */
chdesc = (WmlSynChildDefPtr) malloc (sizeof(WmlSynChildDef));
chdesc->validation = WmlChildDefValid;
chdesc->rslvdef = NULL;
chdesc->name = wmlAllocateString (name);
chdesc->class = wmlAllocateString (class);

if ( wmlFindInHList(wml_synobj_ptr,name) >= 0 )
    {
    printf ("\nDuplicate name %s found", name);
    return;
    }
wmlInsertInHList (wml_synobj_ptr, name, (ObjectPtr)chdesc);
wml_cur_obj = (ObjectPtr) chdesc;
wml_cur_subobj = NULL;

return;

}



/*
 * Routine to create a controls list descriptor. The result is placed in both
 * wml_cur_obj and wml_synobj.
 *
 *	name		the controls list name
 */

void wmlCreateOrAppendCtrlList (name)
    char		*name;

{
int			idx;
WmlSynCtrlListDefPtr	cdesc;		/* new CtrlList descriptor */

idx = wmlFindInHList(wml_synobj_ptr,name);

if (idx < 0 ) {
  /* Didn't find list */

  /*
   * Initialize the new CtrlList descriptor. Enter it in the object list.
   * Set the current object global to the descriptor.
   */
  cdesc = (WmlSynCtrlListDefPtr) malloc (sizeof(WmlSynCtrlListDef));
  cdesc->validation = WmlCtrlListDefValid;
  cdesc->rslvdef = NULL;
  cdesc->name = wmlAllocateString (name);
  cdesc->controls = NULL;

  wmlInsertInHList (wml_synobj_ptr, name, (ObjectPtr)cdesc);
} else {
  cdesc = (WmlSynCtrlListDefPtr) wml_synobj_ptr -> hvec[idx].objptr;
  printf ("\nAppending to list name %s", name);
}

wml_cur_obj = (ObjectPtr) cdesc;
wml_cur_subobj = NULL;

return;

}



/*
 * Routine to create an enumeration set descriptor. The result is placed in both
 * wml_cur_obj and wml_synobj.
 *
 *	name		the enumeration set name
 *	type		data type, must match a data type name
 */

void wmlCreateEnumSet (name, dtype)
    char		*name;
    char		*dtype;

{

WmlSynEnumSetDefPtr	esdesc;		/* new enumeration set descriptor */


/*
 * Initialize the new resource descriptor. Enter it in the object list.
 * Set the current object global to the descriptor.
 */
esdesc = (WmlSynEnumSetDefPtr) malloc (sizeof(WmlSynEnumSetDef));
esdesc->validation = WmlEnumSetDefValid;
esdesc->rslvdef = NULL;
esdesc->name = wmlAllocateString (name);
esdesc->datatype = wmlAllocateString (dtype);
esdesc->values = NULL;

if ( wmlFindInHList(wml_synobj_ptr,name) >= 0 )
    {
    printf ("\nDuplicate name %s found", name);
    return;
    }
wmlInsertInHList (wml_synobj_ptr, name, (ObjectPtr)esdesc);
wml_cur_obj = (ObjectPtr) esdesc;
wml_cur_subobj = NULL;

return;

}



/*
 * Routine to create an enumeration value descriptor. The result is placed in both
 * wml_cur_obj and wml_synobj.
 *
 *	name		the enumeration value name
 */

void wmlCreateEnumValue (name)
    char		*name;

{

WmlSynEnumValueDefPtr	evdesc;		/* new enumeration value descriptor */


/*
 * Initialize the new resource descriptor. Enter it in the object list.
 * Set the current object global to the descriptor.
 */
evdesc = (WmlSynEnumValueDefPtr) malloc (sizeof(WmlSynEnumValueDef));
evdesc->validation = WmlEnumValueDefValid;
evdesc->rslvdef = NULL;
evdesc->name = wmlAllocateString (name);
evdesc->enumlit = wmlAllocateString (name);	/* defaults to name */

if ( wmlFindInHList(wml_synobj_ptr,name) >= 0 )
    {
    printf ("\nDuplicate name %s found", name);
    return;
    }
wmlInsertInHList (wml_synobj_ptr, name, (ObjectPtr)evdesc);
wml_cur_obj = (ObjectPtr) evdesc;
wml_cur_subobj = NULL;

return;

}



/*
 * Routine to create a charset descriptor. The result is placed in both
 * wml_cur_obj and wml_synobj.
 *
 *	name		the charset name
 */

void wmlCreateCharset (name)
    char		*name;

{

WmlSynCharSetDefPtr	ddesc;		/* new charset descriptor */


/*
 * Initialize the new charset descriptor. Enter it in the object list.
 * Set the current object global to the descriptor.
 */
ddesc = (WmlSynCharSetDefPtr) malloc (sizeof(WmlSynCharSetDef));
ddesc->validation = WmlCharSetDefValid;
ddesc->rslvdef = NULL;
ddesc->name = wmlAllocateString (name);
ddesc->int_lit = NULL;
ddesc->xms_name = NULL;
ddesc->direction = WmlCharSetDirectionLtoR;
ddesc->parsedirection = WmlAttributeUnspecified;
ddesc->charsize = WmlCharSizeOneByte;
ddesc->alias_cnt = 0;
ddesc->alias_list = NULL;

if ( wmlFindInHList(wml_synobj_ptr,name) >= 0 )
    {
    printf ("\nDuplicate name %s found", name);
    return;
    }
wmlInsertInHList (wml_synobj_ptr, name, (ObjectPtr)ddesc);
wml_cur_obj = (ObjectPtr) ddesc;
wml_cur_subobj = NULL;

return;

}



/*
 * Routine to set an attribute in a class descriptor.
 *
 * This routine sets the given attribute in the current object, which
 * must be a class descriptor. The current object and subobject do not
 * change.
 *
 *	attrid		oneof SUPERCLASS | INTERNALLITERAL | DOCNAME |
 *			CONVFUNC | WIDGETCLASS | DIALOGCLASS |
 *			CTRLMAPSRESOURCE
 *	val		value of the attribute, usually a string
 */

void wmlAddClassAttribute (attrid, val)
    int			attrid;
    char		*val;

{

WmlSynClassDefPtr	cdesc;		/* the class descriptor */


/*
 * Acquire the current class descriptor
 */
if ( wml_cur_obj == NULL )
    {
    printf ("\nwmlAddClassAttribute: NULL current object");
    return;
    }
cdesc = (WmlSynClassDefPtr) wml_cur_obj;
if ( cdesc->validation != WmlClassDefValid )
    {
    printf ("\nwmlAddClassAttribute: %d not a class descriptor",
	    cdesc->validation);
    return;
    }

/*
 * Set the appropriate resource
 */
switch ( attrid )
    {
    case SUPERCLASS:
        cdesc->superclass = wmlAllocateString (val);
	break;	
    case PARENTCLASS:
        cdesc->parentclass = wmlAllocateString (val);
	break;	
    case INTERNALLITERAL:
        cdesc->int_lit = wmlAllocateString (val);
	break;	
    case CONVFUNC:
        cdesc->convfunc = wmlAllocateString (val);
	break;	
    case DOCNAME:
        cdesc->docname = wmlAllocateString (val);
	break;	
    case WIDGETCLASS:
        cdesc->widgetclass = wmlAllocateString (val);
	break;	
    case DIALOGCLASS:
	switch ( (long)val )
	    {
	    case ATTRTRUE:
	        cdesc->dialog = TRUE;
		break;
	    }
	break;	
    case CTRLMAPSRESOURCE:
        cdesc->ctrlmapto = wmlAllocateString (val);
	break;	
    }

return;
}



/*
 * Routine to add a control specification to the current class.
 * The current object must be a class descriptor. The entry name
 * is added to the controls list. The control specification becomes the
 * current subobject.
 *
 *	name		the name of the controlled class
 */

void wmlAddClassControl (name)
    char			*name;

{
    
WmlSynClassDefPtr	cdesc;		/* the class descriptor */
WmlSynClassCtrlDefPtr	ctrlelm;	/* controls element */


/*
 * Acquire the current class descriptor
 */
if ( wml_cur_obj == NULL )
    {
    printf ("\nwmlAddClassControl: NULL current object");
    return;
    }
cdesc = (WmlSynClassDefPtr) wml_cur_obj;
if ( cdesc->validation != WmlClassDefValid )
    {
    printf ("\nwmlAddClassControl: %d not a class descriptor",
	    cdesc->validation);
    return;
    }

/*
 * Add the control to the control list
 */
ctrlelm = (WmlSynClassCtrlDefPtr) malloc (sizeof(WmlSynClassCtrlDef));
ctrlelm->validation = WmlClassCtrlDefValid;
ctrlelm->next = cdesc->controls;
cdesc->controls = ctrlelm;
ctrlelm->name = wmlAllocateString (name);

/*
 * This becomes the current subobject
 */
wml_cur_subobj = (ObjectPtr) ctrlelm;

return;

}



/*
 * Add a resource descriptor to a class.
 * The current object must be a class descriptor. Create and add a
 * resource descriptor, which becomes the current subobject. It is not
 * entered in the named object list.
 *
 *	name		the resource name
 */

void wmlAddClassResource (name)
    char			*name;

{
    
WmlSynClassDefPtr	cdesc;		/* the class descriptor */
WmlSynClassResDefPtr	rdesc;		/* the resource reference descriptor */


/*
 * Acquire the current class descriptor
 */
if ( wml_cur_obj == NULL )
    {
    printf ("\nwmlAddClassResource: NULL current object");
    return;
    }
cdesc = (WmlSynClassDefPtr) wml_cur_obj;
if ( cdesc->validation != WmlClassDefValid )
    {
    printf ("\nwmlAddClassResource: %d not a class descriptor",
	    cdesc->validation);
    return;
    }

/*
 * Add the resource to the resource list
 */
rdesc = (WmlSynClassResDefPtr) malloc (sizeof(WmlSynClassResDef));
rdesc->validation = WmlClassResDefValid;
rdesc->name = wmlAllocateString (name);
rdesc->type = NULL;
rdesc->dflt = NULL;
rdesc->exclude = WmlAttributeUnspecified;

rdesc->next = cdesc->resources;
cdesc->resources = rdesc;

/*
 * This becomes the current subobject
 */
wml_cur_subobj = (ObjectPtr) rdesc;

return;

}


/*
 * Add a child descriptor to a class.
 * The current object must be a class descriptor. Create and add a
 * child descriptor, which becomes the current subobject. It is not
 * entered in the named object list.
 *
 *	name		the resource name
 */

void wmlAddClassChild (name)
    char			*name;

{
    
WmlSynClassDefPtr	cdesc;		/* the class descriptor */
WmlSynClassChildDefPtr	chdesc;		/* the child reference descriptor */


/*
 * Acquire the current class descriptor
 */
if ( wml_cur_obj == NULL )
    {
    printf ("\nwmlAddClassResource: NULL current object");
    return;
    }
cdesc = (WmlSynClassDefPtr) wml_cur_obj;
if ( cdesc->validation != WmlClassDefValid )
    {
    printf ("\nwmlAddClassResource: %d not a class descriptor",
	    cdesc->validation);
    return;
    }

/*
 * Add the child to the child list
 */
chdesc = (WmlSynClassChildDefPtr) malloc (sizeof(WmlSynClassChildDef));
chdesc->validation = WmlClassChildDefValid;
chdesc->name = wmlAllocateString (name);

chdesc->next = cdesc->children;
cdesc->children = chdesc;

/*
 * This becomes the current subobject
 */
wml_cur_subobj = (ObjectPtr) chdesc;

return;

}



/*
 * This routine sets an attribute in the current class resource descriptor.
 * The current subobject must be a class resource descriptor. The
 * named attribute is set.
 *
 *	attrid		one of TYPE | DEFAULT | EXCLUDE
 *	val		attribute value, usually a string. Must be
 *			ATTRTRUE | ATTRFALSE for EXCLUDE.
 */
void wmlAddClassResourceAttribute (attrid, val)
    int			attrid;
    char		*val;

{

WmlSynClassResDefPtr	rdesc;		/* current class resource descriptor */
long			excval;		/* EXCLUDE value */


/*
 * Acquire the descriptor from the current subobject.
 */
if ( wml_cur_subobj == NULL )
    {
    printf ("\nwmlAddClassResourceAttribute: NULL current subobject");
    return;
    }
rdesc = (WmlSynClassResDefPtr) wml_cur_subobj;
if ( rdesc->validation != WmlClassResDefValid )
    {
    printf
	("\nwmlAddClassResourceAttribute: %d not a class resource descriptor",
	 rdesc->validation);
    return;
    }

switch ( attrid )
    {
    case TYPE:
        rdesc->type = wmlAllocateString (val);
	break;
    case DEFAULT:
        rdesc->dflt = wmlAllocateString (val);
	break;
    case EXCLUDE:
	excval = (long) val;
	switch ( excval )
	    {
	    case ATTRTRUE:
	        rdesc->exclude = WmlAttributeTrue;
		break;
	    case ATTRFALSE:
		rdesc->exclude = WmlAttributeFalse;
		break;
	    default:
		printf ("\nwmlAddClassResourceAttribute: bad EXCLUDE value %d",
			excval);
		return;
		break;
	    }
	break;
    default:
	printf ("\nwmlAddClassResourceAttribute: unknown attrid %d", attrid);
	return;
	break;
    }

return;

}




/*
 * Routine to set an attribute in a resource descriptor.
 *
 * This routine sets the given attribute in the current object, which
 * must be a resource descriptor. The current object and subobject do not
 * change.
 *
 *	attrid		oneof TYPE | RESOURCELITERAL | INTERNALLITERAL |
 *			RELATED | DOCNAME | DEFAULT | XRMRESOURCE | ALIAS |
 *			ENUMERATIONSET
 *	val		value of the attribute, usually a string
 */

void wmlAddResourceAttribute (attrid, val)
    int			attrid;
    char		*val;

{

WmlSynResourceDefPtr	rdesc;		/* the resource descriptor */
long			xrmval;		/* XRMRESOURCE value */
char			**synlist;	/* ALIAS pointer list */


/*
 * Acquire the current resource descriptor
 */
if ( wml_cur_obj == NULL )
    {
    printf ("\nwmlAddResourceAttribute: NULL current object");
    return;
    }
rdesc = (WmlSynResourceDefPtr) wml_cur_obj;
if ( rdesc->validation != WmlResourceDefValid )
    {
    printf ("\nwmlAddResourceAttribute: %d not a resource descriptor",
	    rdesc->validation);
    return;
    }

/*
 * Set the appropriate resource
 */
switch ( attrid )
    {
    case TYPE:
        rdesc->datatype = wmlAllocateString (val);
	break;	
    case INTERNALLITERAL:
        rdesc->int_lit = wmlAllocateString (val);
	break;	
    case RESOURCELITERAL:
        rdesc->resliteral = wmlAllocateString (val);
	break;	
    case ENUMERATIONSET:
        rdesc->enumset = wmlAllocateString (val);
	break;	
    case DOCNAME:
        rdesc->docname = wmlAllocateString (val);
	break;	
    case RELATED:
        rdesc->related = wmlAllocateString (val);
	break;	
    case DEFAULT:
        rdesc->dflt = wmlAllocateString (val);
	break;	
    case XRMRESOURCE:
	xrmval = (long) val;
	switch ( xrmval )
	    {
	    case ATTRTRUE:
	        rdesc->xrm_support = WmlAttributeTrue;
		break;
	    case ATTRFALSE:
		rdesc->xrm_support = WmlAttributeFalse;
		break;
	    default:
		printf
		    ("\nwmlAddResourceAttribute: bad XRMRESOURCE value %d",
		     xrmval);
		return;
		break;
	    }
	break;
    case ALIAS:
	if ( rdesc->alias_cnt == 0 )
	    synlist = (char **) malloc (sizeof(char *));
	else
	    synlist = (char **)
		realloc (rdesc->alias_list,
			 (rdesc->alias_cnt+1)*sizeof(char **));
	synlist[rdesc->alias_cnt] = wmlAllocateString (val);
	rdesc->alias_cnt += 1;
	rdesc->alias_list = synlist;
	break;
    default:
	printf ("\nwmlAddResourceAttribute: unknown attrid %d", attrid);
	return;
	break;
    }

return;
}



/*
 * Routine to set an attribute in a datatype descriptor.
 *
 * This routine sets the given attribute in the current object, which
 * must be a datatype descriptor. The current object and subobject do not
 * change.
 *
 *	attrid		oneof INTERNALLITERAL | DOCNAME | XRMRESOURCE
 *	val		value of the attribute, usually a string
 */

void wmlAddDatatypeAttribute (attrid, val)
    int			attrid;
    char		*val;

{

WmlSynDataTypeDefPtr	ddesc;		/* the datatype descriptor */
long			xrmval;		/* XRMRESOURCE value */


/*
 * Acquire the current datatype descriptor
 */
if ( wml_cur_obj == NULL )
    {
    printf ("\nwmlAddDatatypeAttribute: NULL current object");
    return;
    }
ddesc = (WmlSynDataTypeDefPtr) wml_cur_obj;
if ( ddesc->validation != WmlDataTypeDefValid )
    {
    printf ("\nwmlAddDatatypeAttribute: %d not a datatype descriptor",
	    ddesc->validation);
    return;
    }

/*
 * Set the appropriate slot
 */
switch ( attrid )
    {
    case INTERNALLITERAL:
        ddesc->int_lit = wmlAllocateString (val);
	break;	
    case DOCNAME:
        ddesc->docname = wmlAllocateString (val);
	break;	
    case XRMRESOURCE:
	xrmval = (long) val;
	switch ( xrmval )
	    {
	    case ATTRTRUE:
	        ddesc->xrm_support = WmlAttributeTrue;
		break;
	    case ATTRFALSE:
		ddesc->xrm_support = WmlAttributeFalse;
		break;
	    default:
		printf
		    ("\nwmlAddDatatypeAttribute: bad XRMRESOURCE value %d",
		     xrmval);
		return;
		break;
	    }
	break;
    default:
	printf ("\nwmlAddDatatypeAttribute: unknown attrid %d", attrid);
	return;
	break;
    }

return;
}



/*
 * Routine to add a control specification to the current controls list.
 * The current object must be a controls list descriptor. The entry name
 * is added to the controls list. The new element becomes the current
 * subobject.
 *
 *	name		the name of the controlled class
 */

void wmlAddCtrlListControl (name)
    char			*name;

{
    
WmlSynCtrlListDefPtr	cdesc;		/* the controls list descriptor */
WmlSynClassCtrlDefPtr	ctrlelm;	/* controls element */


/*
 * Acquire the current controls list descriptor
 */
if ( wml_cur_obj == NULL )
    {
    printf ("\nwmlAddCtrlListControl: NULL current object");
    return;
    }
cdesc = (WmlSynCtrlListDefPtr) wml_cur_obj;
if ( cdesc->validation != WmlCtrlListDefValid )
    {
    printf ("\nwmlAddCtrlListControl: %d not a controls list descriptor",
	    cdesc->validation);
    return;
    }

/*
 * Add the control to the control list
 */
ctrlelm = (WmlSynClassCtrlDefPtr) malloc (sizeof(WmlSynClassCtrlDef));
ctrlelm->validation = WmlClassCtrlDefValid;
ctrlelm->next = cdesc->controls;
cdesc->controls = ctrlelm;
ctrlelm->name = wmlAllocateString (name);

/*
 * This becomes the current subobject
 */
wml_cur_subobj = (ObjectPtr) ctrlelm;

return;

}



/*
 * Routine to add an enumeration value to the current enumeration set
 * The current object must be an enumeration set descriptor. The entry name
 * is added to the the enumeration value list.
 *
 *	name		the name of the enumeration value
 */
void wmlAddEnumSetValue (name)
    char		*name;

{

WmlSynEnumSetDefPtr	esdesc;		/* the enumeration set descriptor */
WmlSynEnumSetValDefPtr	evelm;		/* EnumSet EnumValue element */

/*
 * Acquire the current enumeration set descriptor
 */
if ( wml_cur_obj == NULL )
    {
    printf ("\nwmlAddEnumSetValue: NULL current object");
    return;
    }
esdesc = (WmlSynEnumSetDefPtr) wml_cur_obj;
if ( esdesc->validation != WmlEnumSetDefValid )
    {
    printf ("\nwmlAddEnumSetValue: %d not an enumeration set descriptor",
	    esdesc->validation);
    return;
    }

/*
 * Add the value to the set
 */
evelm = (WmlSynEnumSetValDefPtr) malloc (sizeof(WmlSynEnumSetValDef));
evelm->validation = WmlEnumValueDefValid;
evelm->next = esdesc->values;
esdesc->values = evelm;
evelm->name = wmlAllocateString (name);

/*
 * Becomes current subobject
 */
wml_cur_subobj = (ObjectPtr) evelm;

}



/*
 * Routine to set an attribute in an enumeration value
 *
 * This routine sets the given attribute in the current object, which must
 * be an enumeration value descriptor. The current object does not change.
 *
 *	attrid		oneof ENUMLITERAL
 *	val		value of the attribute, usually a string
 */
void wmlAddEnumValueAttribute (attrid, val)
    int			attrid;
    char		*val;

{

WmlSynEnumValueDefPtr	evdesc;		/* the enumeration value descriptor */


/*
 * Acquire the current enumeration value descriptor
 */
if ( wml_cur_obj == NULL )
    {
    printf ("\nwmlAddEnumValueAttribute: NULL current object");
    return;
    }
evdesc = (WmlSynEnumValueDefPtr) wml_cur_obj;
if ( evdesc->validation != WmlEnumValueDefValid )
    {
    printf ("\nwmlAddEnumValueAttribute: %d not an enumeration value descriptor",
	    evdesc->validation);
    return;
    }

/*
 * Set the appropriate slot
 */
switch ( attrid )
    {
    case ENUMLITERAL:
        evdesc->enumlit = wmlAllocateString (val);
	break;	
    default:
	printf ("\nwmlAddEnumValueAttribute: unknown attrid %d", attrid);
	return;
	break;
    }

return;

}



/*
 * Routine to set an attribute in a charset descriptor.
 *
 * This routine sets the given attribute in the current object, which
 * must be a charset descriptor. The current object and subobject do not
 * change.
 *
 *	attrid		oneof INTERNALLITERAL | ALIAS | XMSTRINGCHARSETNAME |
 *			DIRECTION | PARSEDIRECTION | CHARACTERSIZE
 *	val		value of the attribute, usually a string
 */

void wmlAddCharsetAttribute (attrid, val)
    int			attrid;
    char		*val;

{

WmlSynCharSetDefPtr	ddesc;		/* the charset descriptor */
char			**synlist;	/* ALIAS pointer list */
long			atrval;		/* attribute value */


/*
 * Acquire the current charset descriptor
 */
if ( wml_cur_obj == NULL )
    {
    printf ("\nwmlAddCharSetAttribute: NULL current object");
    return;
    }
ddesc = (WmlSynCharSetDefPtr) wml_cur_obj;
if ( ddesc->validation != WmlCharSetDefValid )
    {
    printf ("\nwmlAddCharsetAttribute: %d not a CharSet descriptor",
	    ddesc->validation);
    return;
    }

/*
 * Set the appropriate slot
 */
switch ( attrid )
    {
    case INTERNALLITERAL:
        ddesc->int_lit = wmlAllocateString (val);
	break;	
    case ALIAS:
	if ( ddesc->alias_cnt == 0 )
	    synlist = (char **) malloc (sizeof(char *));
	else
	    synlist = (char **)
		realloc (ddesc->alias_list,
			 (ddesc->alias_cnt+1)*sizeof(char **));
	synlist[ddesc->alias_cnt] = wmlAllocateString (val);
	ddesc->alias_cnt += 1;
	ddesc->alias_list = synlist;
	break;
    case XMSTRINGCHARSETNAME:
        ddesc->xms_name = wmlAllocateString (val);
	break;	
    case DIRECTION:
	atrval = (long) val;
	switch ( atrval )
	    {
	    case LEFTTORIGHT:
	        ddesc->direction = WmlCharSetDirectionLtoR;
		break;
	    case RIGHTTOLEFT:
	        ddesc->direction = WmlCharSetDirectionRtoL;
		break;
	    default:
		printf
		    ("\nwmlAddCharsetAttribute: bad DIRECTION value %d",
		     atrval);
		return;
		break;
	    }
	break;	
    case PARSEDIRECTION:
	atrval = (long) val;
	switch ( atrval )
	    {
	    case LEFTTORIGHT:
	        ddesc->parsedirection = WmlCharSetDirectionLtoR;
		break;
	    case RIGHTTOLEFT:
	        ddesc->parsedirection = WmlCharSetDirectionRtoL;
		break;
	    default:
		printf
		    ("\nwmlAddCharsetAttribute: bad PARSEDIRECTION value %d",
		     atrval);
		return;
		break;
	    }
	break;	
    case CHARACTERSIZE:
	atrval = (long) val;
	switch ( atrval )
	    {
	    case ONEBYTE:
	        ddesc->charsize = WmlCharSizeOneByte;
		break;
	    case TWOBYTE:
	        ddesc->charsize = WmlCharSizeTwoByte;
		break;
	    case MIXED1_2BYTE:
	        ddesc->charsize = WmlCharSizeMixed1_2Byte;
		break;
	    default:
		printf
		    ("\nwmlAddCharsetAttribute: bad CHARACTERSIZE value %d",
		     atrval);
		return;
		break;
	    }
	break;	
    default:
	printf ("\nwmlAddCharsetAttribute: unknown attrid %d", attrid);
	return;
	break;
    }

return;
}



/*
 * The error reporting routine.
 *
 * For now, issue a very simple error message
 */

void LexIssueError (tkn)
    int			tkn;

{

switch ( tkn )
    {
    case SEMICOLON:
        printf ("\n Syntax error: expected a semicolon");
	break;
    case RBRACE:
        printf ("\n Syntax error: expected a right brace");
	break;
    case 0:
	printf ("\nSyntax error: Couldn't recognize a section name, probably fatal");
	break;
    }
printf ("\n\tnear name='%s', value='%s', line %d",
	yynameval, yystringval, wml_line_count);

wml_err_count += 1;

return;

}
