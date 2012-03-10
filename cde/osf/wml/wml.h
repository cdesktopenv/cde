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
/*   $XConsortium: wml.h /main/9 1995/08/29 11:10:33 drk $ */
/*
*  (c) Copyright 1989, 1990, DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASS. */

/*
 * This file contains the structure and literal definitions required
 * by the WML processor
 */

#if defined(__STDC__)
#include <stddef.h>
#else
#include <stdio.h>
#endif

#define TRUE		1
#define FALSE		0
#define SUCCESS		1
#define FAILURE		0

/*
 * Generic object pointer
 */
typedef	char	*ObjectPtr;


/*
 * True and False for attributes, so setting is explicit
 */
#define	WmlAttributeUnspecified		0
#define	WmlAttributeTrue		1
#define	WmlAttributeFalse		2

/*
 * Values of character set direction
 */
#define	WmlCharSetDirectionLtoR		1
#define	WmlCharSetDirectionRtoL		2

/*
 * Values of character set character size
 */
#define	WmlCharSizeOneByte		1
#define	WmlCharSizeTwoByte		2
#define	WmlCharSizeMixed1_2Byte		3


/*
 * Upper case and lower case converters
 */
#define _upper(c)	((c) >= 'a' && (c) <= 'z' ? (c) & 0x5F:(c))
#define _lower(c)	((c) >= 'A' && (c) <= 'Z' ? (c) | 0x20:(c))


/*
 * The Uil token classes which are dealt with WML. Matched to definitions
 * in UilKeyDef.h, although this isn't required.
 */
#define	WmlTokenClassArgument	1
#define	WmlTokenClassCharset	2
#define	WmlTokenClassColor	3
#define	WmlTokenClassEnumval	4
#define	WmlTokenClassFont	5
#define	WmlTokenClassIdentifier	6
#define	WmlTokenClassKeyword	7
#define	WmlTokenClassLiteral	8
#define	WmlTokenClassReason	9
#define	WmlTokenClassReserved	10
#define	WmlTokenClassSpecial	11
#define	WmlTokenClassUnused	12
#define	WmlTokenClassClass	13
#define WmlTokenClassChild	14



/*
 * Structures for WML objects. Two kinds are created for each object
 * recognized and built as a WML description is read:
 *	- A syntactic descriptor, which captures the information parse
 *	  from the input in pretty much its raw form
 *	- A semantically resolved descriptor, in which pointers to
 *	  other descriptors have been resolved, ordering is done, 
 *	  superclass inheritance is complete, etc.
 */

/*
 * Syntactic structures. These are constructred during input parse.
 */


/*
 * A dummy holding only a header. Allows access to the validation field.
 */
typedef struct
    {
    int			validation;	/* a unique validation code */
    ObjectPtr		rslvdef;	/* the resolved object pointer */
    } WmlSynDef, *WmlSynDefPtr;



/*
 * A class resource descriptor. This a subset of a full resource descriptor,
 * which captures those elements which can be overridden in a class declaration
 */
#define WmlClassResDefValid	871253

typedef struct WmlSynClassResDefStruct
    {
    int			validation;	/* WmlClassResDefValid */
    struct WmlSynClassResDefStruct
			*next;		/* next descriptor in chain */
    char		*name;		/* resource name */
    char		*type;		/* override type name */
    char		*dflt;		/* override default value */
    short int		exclude;	/* WmlAttributeTrue if to be excluded */
    } WmlSynClassResDef, *WmlSynClassResDefPtr;

/*
 * A class child descriptor. 
 */
#define WmlClassChildDefValid	780344

typedef struct WmlSynClassChildDefStruct
    {
    int			validation;	/* WmlClassChildDefValid */
    struct WmlSynClassChildDefStruct
			*next;		/* next descriptor in chain */
    char		*name;		/* child name */
    } WmlSynClassChildDef, *WmlSynClassChildDefPtr;


/*
 * A class controls descriptor. It contains elements which can be added
 * to a controls reference.
 */

#define WmlClassCtrlDefValid	7132320

typedef struct WmlSynClassCtrlDefStruct
    {
    int			validation;	/* WmlClassCtrlDefValid */
    struct WmlSynClassCtrlDefStruct
			*next;		/* next descriptor in chain */
    char		*name;		/* controlled class name */
    } WmlSynClassCtrlDef, *WmlSynClassCtrlDefPtr;


/*
 * A class structure. One exists for each Class statement in a WML
 * description.
 */
#define WmlClassDefValid	93741

#define WmlClassTypeMetaclass	1
#define	WmlClassTypeWidget	2
#define	WmlClassTypeGadget	3

typedef struct WmlSynClassDefStruct
    {
    int			validation;	/* WmlClassDefValid */
    struct WmlClassDefStruct
			*rslvdef;	/* resolved definition */
    short int		type;		/* from WmlClassType... */
    short int		dialog;		/* TRUE if a dialog class (has
					   window manager decoration, etc) */
    char		*name;		/* class name */
    char		*superclass;	/* superclass name */
    char		*parentclass;	/* parentclass name */
    char		*widgetclass;	/* widget class name */
    char		*int_lit;	/* internal literal name */
    char		*convfunc;	/* convenience function name */
    char		*docname;	/* name for docs */
    char		*ctrlmapto;	/* resource controls map to */
    WmlSynClassCtrlDefPtr
			controls;	/* list of controlled classes */
    WmlSynClassResDefPtr
			resources;	/* list of resources from input */
    WmlSynClassChildDefPtr
                        children;	/* list of automatic children */
    } WmlSynClassDef, *WmlSynClassDefPtr;


/*
 * A controls list structure. One is created for each ControlsList statement
 * in a WML description.
 */
#define WmlCtrlListDefValid	621298

typedef struct WmlSynCtrlListDefStruct
    {
    int			validation;	/* WmlCtrlListDefValid */
    struct WmlCtrlListDefStruct
			*rslvdef;	/* resolved definition */
    char		*name;		/* controls list name */
    WmlSynClassCtrlDefPtr
			controls;	/* list of controlled classes */
    } WmlSynCtrlListDef, *WmlSynCtrlListDefPtr;


/*
 * A resource descriptor. One is created for each Resource statement
 * in a WML description
 */
#define WmlResourceDefValid	310538

#define WmlResourceTypeArgument		1
#define	WmlResourceTypeReason		2
#define	WmlResourceTypeConstraint	3
#define	WmlResourceTypeSubResource	4

typedef struct WmlSynResourceDefStruct
    {
    int			validation;	/* WmlResourceDefValid */
    struct WmlResourceDefStruct
			*rslvdef;	/* resolved definition */
    short int		type;		/* from WmlResourceType... */
    char		*name;		/* resource name */
    char		*datatype;	/* resource data type */
    char		*int_lit;	/* internal literal name */
    char		*resliteral;	/* resource name literal */
    char		*enumset;	/* enumeration set name */
    char		*docname;	/* name for docs */
    char		*related;	/* names related resource */
    char		*dflt;		/* default value */
    short int		xrm_support;	/* WmlAttributeTrue if can be
					   Xrm resource */
    short int		alias_cnt;	/* alias count */
    char		**alias_list;	/* vector of aliass */
    } WmlSynResourceDef, *WmlSynResourceDefPtr;

/*
 * A child descriptor. One is created for each Child statement
 * in a WML description
 */
#define WmlChildDefValid	229629

typedef struct WmlSynChildDefStruct
    {
    int			validation;	/* WmlResourceDefValid */
    struct WmlChildDefStruct
			*rslvdef;	/* resolved definition */
    char		*name;		/* child name */
    char		*class;		/* child class */
    } WmlSynChildDef, *WmlSynChildDefPtr;


/*
 * A datatype descriptor
 */
#define WmlDataTypeDefValid	714210

typedef struct WmlSynDataTypeDefStruct
    {
    int			validation;	/* WmlDataTypeDefValid */
    struct WmlDataTypeDefStruct
			*rslvdef;	/* resolved definition */
    char		*name;		/* data type name */
    char		*int_lit;	/* internal literal name */
    char		*docname;	/* name for docs */
    short int		xrm_support;	/* WmlAttributeTrue if can be
					   Xrm resource */
    } WmlSynDataTypeDef, *WmlSynDataTypeDefPtr;


/*
 * An enumeration set values descriptor, as it occurs in the list for
 * an enumeration set descriptor.
 */
typedef struct WmlSynEnumSetValDefStruct
    {
    int			validation;	/* WmlEnumValueDefValid */
    struct WmlSynEnumSetValDefStruct
			*next;		/* next descriptor in chain */
    char		*name;		/* enumeration value name */
    } WmlSynEnumSetValDef, *WmlSynEnumSetValDefPtr;


/*
 * An enumeration set descriptor
 */
#define	WmlEnumSetDefValid	931184

typedef struct WmlSynEnumSetDefStruct
    {
    int			validation;	/* WmlEnumSetDefValid */
    struct WmlEnumSetDefStruct
			*rslvdef;	/* resolved definition */
    char		*name;		/* enumeration set name */
    char		*datatype;	/* enumeration set data type */
    WmlSynEnumSetValDefPtr
			values;		/* list of values in set */
    } WmlSynEnumSetDef, *WmlSynEnumSetDefPtr;



/*
 * An enumeration value descriptor
 */
#define	WmlEnumValueDefValid	172938

typedef struct WmlSynEnumValueDefStruct
    {
    int			validation;	/* WmlEnumValueDefValid */
    struct WmlEnumValueDefStruct
			*rslvdef;	/* resolved definition */
    char		*name;		/* enumeration value name */
    char		*enumlit;	/* name of defining literal */
    } WmlSynEnumValueDef, *WmlSynEnumValueDefPtr;


/*
 * A charset descriptor
 */
#define WmlCharSetDefValid	110983

typedef struct WmlSynCharSetDefStruct
    {
    int			validation;	/* WmlCharSetDefValid */
    struct WmlCharSetDefStruct
			*rslvdef;	/* resolved definition */
    char		*name;		/* data type name */
    char		*int_lit;	/* internal literal name */
    char		*xms_name;	/* identifying XmString name */
    short int		direction;	/* WmlCharSetDirection... */
    short int		parsedirection;	/* WmlCharSetDirection... */
    short int		charsize;	/* WmlCharSize... */
    short int		alias_cnt;	/* alias count */
    char		**alias_list;	/* vector of aliases */
    } WmlSynCharSetDef, *WmlSynCharSetDefPtr;



/*
 * Data structures constructed during semantic validation. Each points
 * as required to syntactic data structures. These are typically accessed
 * from ordered pointer vectors.
 */


/*
 * A datatype descriptor. It extends the syntactic element by assigning
 * a code to the descriptor.
 */
typedef struct WmlDataTypeDefStruct
    {
    struct WmlSynDataTypeDefStruct
			*syndef;	/* syntactic definition */
    char		*tkname;	/* name for generating literals */
    } WmlDataTypeDef, *WmlDataTypeDefPtr;


/*
 * A charset descriptor. It extends the syntactic element by assigning
 * a code to the descriptor.
 */
typedef struct WmlCharSetDefStruct
    {
    struct WmlSynCharSetDefStruct
			*syndef;	/* syntactic definition */
    char		*tkname;	/* name for generating literals */
    short int		sym_code;	/* code value for literals */
    } WmlCharSetDef, *WmlCharSetDefPtr;

/*
 * A child descriptor. It extends the syntactic element by assigning
 * a code and a pointer to the class to the descriptor.
 */
typedef struct WmlChildDefStruct
    {
    struct WmlSynChildDefStruct
			*syndef;	/* syntactic definition */
    char		*tkname;	/* name for generating literals */
    short int		sym_code;	/* code value for literals */
    struct WmlClassDefStruct
                        *class;		/* class structure */
    struct WmlClassChildDefStruct
			*ref_ptr;	/* used dynamically for search */
    } WmlChildDef, *WmlChildDefPtr;

/*
 * An element in the values list of an enumeration set. These elements have
 * separate lists in order to deal with the possibility of an enumeration
 * value which is a member of more than one list.
 */
typedef struct WmlEnumSetValDefStruct
    {
    struct WmlEnumSetValDefStruct
			*next;		/* next value in list */
    struct WmlEnumValueDefStruct
			*value;		/* value descriptor for element */
    } WmlEnumSetValDef, *WmlEnumSetValDefPtr;


/*
 * A resolved enumeration set descriptor
 */
typedef struct WmlEnumSetDefStruct
    {
    struct WmlSynEnumSetDefStruct
			*syndef;	/* syntactic definition */
    char		*tkname;	/* name for generating literals */
    short int		sym_code;	/* code value for literals */
    WmlDataTypeDefPtr	dtype_def;	/* data type */
    short int		values_cnt;	/* count of # of values in set */
    WmlEnumSetValDefPtr
			values;		/* list of values in set */
    } WmlEnumSetDef, *WmlEnumSetDefPtr;


/*
 * A resolved enumeration value descriptor
 */
typedef struct WmlEnumValueDefStruct
    {
    struct WmlSynEnumValueDefStruct
			*syndef;	/* syntactic definition */
    short int		sym_code;	/* code value for literals */
    } WmlEnumValueDef, *WmlEnumValueDefPtr;


/*
 * A resource descriptor
 */
typedef struct WmlResourceDefStruct
    {
    struct WmlSynResourceDefStruct
			*syndef;	/* syntactic definition */
    char		*tkname;	/* name for generating literals */
    short int		sym_code;	/* code value for literals */
    WmlDataTypeDefPtr	dtype_def;	/* data type for base definition */
    WmlEnumSetDefPtr	enumset_def;	/* enumeration set if specified */
    short int		related_code;	/* if non-0, the sym_code for the
					   related (count) argument */
    short int		xrm_support;	/* if AttributeTrue, then the resource
					   can be set in Xrm. Resolved
					   from either explicit setting or
					   data type setting */
    struct WmlClassDefStruct
			*ref_class;	/* used for membership sort */
    struct WmlClassResDefStruct
			*ref_ptr;	/* used dynamically for search */
    } WmlResourceDef, *WmlResourceDefPtr;


/*
 * A resource descriptor with overrides which is a member of the resource
 * list of a class descriptor.
 */
typedef struct WmlClassResDefStruct
    {
    struct WmlClassResDefStruct
			*next;		/* next resource in list */
    WmlResourceDefPtr	act_resource;	/* actual resource descriptor */
    WmlDataTypeDefPtr	over_dtype;	/* overriding data type */
    char		*dflt;		/* overriding default */
    int			exclude;	/* WmlAttributeTrue if to be excluded */
    } WmlClassResDef, *WmlClassResDefPtr;

/*
 * A child descriptor which is a member of the children
 * list of a class descriptor.
 */
typedef struct WmlClassChildDefStruct
    {
    struct WmlClassChildDefStruct
			*next;		/* next child in list */
    WmlChildDefPtr	act_child;	/* actual child descriptor */
  } WmlClassChildDef, *WmlClassChildDefPtr;


/*
 * An element for the controls list of a resolved class descriptor
 */
typedef struct WmlClassCtrlDefStruct
    {
    struct WmlClassCtrlDefStruct
			*next;		/* next control in list */
    struct WmlClassDefStruct
			*ctrl;		/* class being controlled */
    } WmlClassCtrlDef, *WmlClassCtrlDefPtr;
    
    

/*
 * A resolved class descriptor. It has a pointer to its superclass, and
 * a resource list consisting of its inherited resources followed by
 * its own resources.
 */
typedef struct WmlClassDefStruct
    {
    struct WmlSynClassDefStruct
			*syndef;	/* syntactic definition */
    struct WmlClassDefStruct
			*superclass;	/* superclass structure */
    struct WmlClassDefStruct
			*parentclass;	/* parentclass structure */
    char		*tkname;	/* name to be used in literals.
					   int_lit or name if no int_lit */
    short int		sym_code;	/* code value for literals */
    short int		inherit_done;	/* TRUE when inheritance complete */
    WmlClassResDefPtr	arguments;	/* linked argument list */
    WmlClassResDefPtr	reasons;	/* lined reason list */
    WmlClassCtrlDefPtr	controls;	/* list of controlled classes.
					   Controls list references will
					   be expanded into this list. */
    WmlClassChildDefPtr	children;	/* list of automatic children */
    struct WmlClassDefStruct
			*variant;	/* the gadget class for a widget */
    struct WmlClassDefStruct
			*nondialog;	/* the non-dialog ancestor of a
					   dialog widget */
    WmlResourceDefPtr	ctrlmapto;	/* the resource controls map to */
    struct WmlClassCtrlDefStruct
			*ref_ptr;	/* used dynamically for search */
    } WmlClassDef, *WmlClassDefPtr;


/*
 * A resolved controls list descriptor.
 */
typedef struct WmlCtrlListDefStruct
   {
   struct WmlSynCtrlListDefStruct
			*syndef;	/* syntactic definition */
   WmlClassCtrlDefPtr	controls;	/* list of controlled classes */
   } WmlCtrlListDef, *WmlCtrlListDefPtr;



/*
 * Data structures used to locate and order objects in various ways.
 */

/*
 * Token structure used to create ordered token lists for generation of
 * UilKeyTab.h. The token string is in the order vector.
 */
typedef struct WmlKeyWTokenStruct
    {
    int			class;		/* token class, WmlTokenClass... */
    ObjectPtr		objdef;		/* object definition (resolved) */
    } WmlKeyWToken, *WmlKeyWTokenPtr;


/*
 * A grammar token as obtained from the UIL grammar file (Uil.y)
 */
typedef struct WmlGrammarTokenStruct
    {
    int			class;		/* token class, WmlTokenClass... */
    char		*token;		/* token name (literal) */
    int			val;		/* token id as value */
    } WmlGrammarToken, *WmlGrammarTokenPtr;


/*
 * A list element which allows association of a name with an object.
 * Typically used to construct ordered lists.
 */
typedef struct
    {
    char		*objname;	/* object name */
    ObjectPtr		objptr;		/* the object */
    } ObjectHandleDef, *ObjectHandleDefPtr;

/*
 * A dynamic handle element list, extensible by malloc'ing more space.
 */
typedef struct
    {
    int			cnt;		/* # entries in use */
    int			max;		/* max # entries available */
    int			ordered;	/* TRUE if list is lexicographically
					   ordered by object name */
    ObjectHandleDefPtr	hvec;		/* vector of handle entries */
    } DynamicHandleListDef, *DynamicHandleListDefPtr;



/*
 * Global declarations
 */

/*
 * Defined in wml.c
 */
extern int		wml_err_count;		/* total errors */
extern int		wml_line_count;		/* lines read from input */
extern DynamicHandleListDefPtr
			wml_synobj_ptr;		/* syntactic object list */

extern DynamicHandleListDefPtr	wml_obj_datatype_ptr;
extern DynamicHandleListDefPtr	wml_obj_enumval_ptr;
extern DynamicHandleListDefPtr	wml_obj_enumset_ptr;
extern DynamicHandleListDefPtr	wml_obj_reason_ptr;
extern DynamicHandleListDefPtr	wml_obj_arg_ptr;
extern DynamicHandleListDefPtr	wml_obj_child_ptr;
extern DynamicHandleListDefPtr	wml_obj_allclass_ptr;
extern DynamicHandleListDefPtr	wml_obj_class_ptr;
extern DynamicHandleListDefPtr	wml_obj_ctrlist_ptr;
extern DynamicHandleListDefPtr	wml_obj_charset_ptr;

extern DynamicHandleListDefPtr	wml_tok_sens_ptr;
extern DynamicHandleListDefPtr	wml_tok_insens_ptr;




/*
 * Defined in wmlutils.c
 */
extern char *wmlAllocateString ();		/* dynamic string copy */
extern void wmlUpperCaseString ();		/* convert to upper case */
extern void wmlInitHList ();			/* init dynamic list */
extern void wmlResizeHList ();			/* resize a list */
extern void wmlClearHList ();			/* clear a list for reuse */
extern int wmlFindInHList ();			/* find name in list */
extern void wmlInsertInHList ();		/* generic list insert */
extern void wmlInsertInKeyList ();		/* specialized list insert */
extern WmlClassResDefPtr wmlResolveResIsMember ();
						/* is resource in class? */
extern WmlClassChildDefPtr wmlResolveChildIsMember ();
						/* is child in class? */

/*
 * Defined in wmlsynbld.c
 */
extern char		yystringval[];		/* any string value */
extern char		yynameval[];		/* any name (identifier) */
extern int		yytknval1;		/* terminal token value 1 */
extern int		yytknval2;		/* terminal token value 2 */
extern ObjectPtr	wml_cur_obj;		/* object being constructed */
extern ObjectPtr	wml_cur_subobj;		/* current subobject */
extern void wmlCreateClass ();
extern void wmlAddClassAttribute ();
extern void wmlAddClassResource ();
extern void wmlAddClassResourceAttribute ();
extern void wmlAddClassControl ();
extern void wmlAddCtrList ();
extern void wmlCreateResource ();
extern void wmlCreateDatatype ();
extern void wmlAddResourceAttribute ();
extern void wmlAddDatatypeAttribute ();
extern void wmlAddCtrListControl ();
extern void wmlCreateEnumSet ();
extern void wmlAddEnumSetValue ();
extern void wmlCreateEnumValue ();
extern void wmlAddEnumValueAttribute ();
extern void wmlCreateCharset ();
extern void wmlAddCharsetAttribute ();
extern void LexIssueError ();


/*
 * Defined in wmlresolve.c
 */
extern void wmlResolveDescriptors ();
extern void wmlMarkReferencePointers ();


/*
 * Defined in wmlouth.c
 */
extern void wmlOutputHFiles ();


/*
 * Defined in wmloutdat.c
 */
extern void wmlOutputDatFiles ();

/*
 * Define in wmloutp1 or wmloutp2
 */
extern void wmlOutput ();
