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
 * $TOG: args.c /main/5 1999/11/04 18:05:03 mgreess $
 * 
 * @(#)args.c	1.28 16 Feb 1994	cde_app_builder/src/abmf
 * 
 * RESTRICTED CONFIDENTIAL INFORMATION:
 * 
 * The information in this document is subject to special restrictions in a
 * confidential disclosure agreement between HP, IBM, Sun, USL, SCO and
 * Univel.  Do not distribute this document outside HP, IBM, Sun, USL, SCO,
 * or Univel without Sun's specific written approval.  This document and all
 * copies and derivative works thereof must be returned or destroyed at Sun's
 * request.
 * 
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 * 
 */

/*
 * args.c - handles argument printing, et cetera
 */

#include <stdio.h>
#include <assert.h>
#include <Xm/Xm.h>
#include <Xm/XmStrDefs.h>
#include <Dt/SpinBox.h>
#include <Dt/Term.h>
#include <ab_private/objxm.h>
#include <ab_private/abio.h>
#include "abmfP.h"
#include "utilsP.h"
#include "instancesP.h"
#include "motifdefsP.h"
#include "obj_namesP.h"
#include "argsP.h"
#include "msg_cvt.h"

/*************************************************************************
**                                                                      **
**       Constants (#define and const)					**
**                                                                      **
**************************************************************************/

/*************************************************************************
**                                                                      **
**       Private Functions (C declarations and macros)			**
**                                                                      **
**************************************************************************/

static int abmfP_obj_spew_one_class_args(
			    GenCodeInfo		genCodeInfo,
			    ABObj		obj,
			    ABMF_ARG_CLASS	argClass,
			    ABMF_ARG_LIST_FORMAT argFormat
			    );

static int spew_extra_item_strings_args(
                	GenCodeInfo     genCodeInfo,
                	ABObj           first_item,
			int		num_items,
			STRING		strings_res_name,
			STRING		strings_count_res_name
			);

static int spew_extra_choice_args(
			GenCodeInfo 		genCodeInfo, 
			ABObj 			obj,
			ABMF_ARG_CLASS		argClasses,
			ABMF_ARG_LIST_FORMAT	argFormat
			);

static int spew_extra_list_args(
			GenCodeInfo	genCodeInfo, 
			ABObj		obj,
			ABMF_ARG_CLASS	argClasses
			);

static int spew_extra_spinbox_args(
			GenCodeInfo	genCodeInfo, 
			ABObj		obj,
			ABMF_ARG_CLASS	argClasses
			);

static BOOL	get_wclass_and_res_value(
			ABObj		obj, 
			STRING		argName, 
			WidgetClass	*wClassOut,
			XtArgVal	*resValueOut
		);

static BOOL	arg_has_default_value(ABObj obj, STRING argName);

static STRING	fmt_strval_for_res_file(STRING string);

#define safe_c_str(s) (((s) == NULL)? nullCStr:(s))

/*************************************************************************
**                                                                      **
**       Data     	                                                **
**                                                                      **
**************************************************************************/

#ifndef objxm_obj_get_num_args
#define objxm_obj_get_num_args(obj) \
	(objxm_get_num_args(objxm_obj_get_ui_args(obj)))
#endif

static STRING	nullCStr = "NULL";

/* typed args */
StringList	typed_arg_names= NULL;

#ifdef BOGUS
/* L3 related resources of type INT */
static char        *IntArray[] =
{
    "x",
    "y",
    NULL
};

/*
 * L3 related resources for all widgets Arranged in order of freq of access.
 * If we switch to binary search, sort this alphabetically ...
 */
static StringList   i18nl3_resources = NULL;

/*
 * L3 related resources for list pane
 */
static StringList   list_i18nl3_resources = NULL;
#endif /* BOGUS */

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

int
abmfP_args_init(void)
{
    static BOOL         inited = FALSE;
    ISTRING            *istr_entry = NULL;
    if (inited)
    {
	return 0;
    }
    inited = TRUE;

/*
 * Call istr_const so that the string list will not allocate a new
 * string (it is implemented using ISTRINGs).
 */
#define addit(s) (istr_const(s), strlist_add_str(typed_arg_names, (s), NULL))
    typed_arg_names= strlist_create();
    addit(XmNbackground);
    addit(XmNfontList);
    addit(XmNforeground);
    addit(XmNlabelString);
    strlist_shrink_mem_to_fit(typed_arg_names);
#undef addit

#ifdef BOGUS
#define addit(s) (istr_const(s), strlist_add_str(i18nl3_resources, (s), NULL))
    i18nl3_resources = strlist_create();
    addit(XmNlabelString);
    addit(XmNtitleString);
    addit(XmNstring);
    strlist_shrink_mem_to_fit(i18nl3_resources);
#undef addit

#define addit(s) (istr_const(s), strlist_add_str(list_i18nl3_resources, (s), NULL))
    list_i18nl3_resources = strlist_create();
    addit(XmNheight);
    strlist_shrink_mem_to_fit(list_i18nl3_resources);
#undef addit
#endif /* BOGUS */

    return 0;
}


/*
 * dump - dump to resource file
 * write - write to C file
 * spew = either dump or write
 */
/* INT VALUES */
#define dump_int_resource(inst,name,value) \
	(wroteResource = TRUE, \
	 fprintf(fp,"%s.%s:\t%d\n", inst, name, value))

#define write_int_resource(name,value) \
	(wroteResource = TRUE, abio_printf(fp,"%s, %d", name, (int)(value)))

/* STRING values */
#define dump_str_resource(inst,name,value) \
	 wroteResource = TRUE; \
	 fprintf(fp,"%s.%s:", inst, name); \
	 strval2 = fmt_strval_for_res_file((STRING)value); \
	 (strval2 == NULL? fputs(nlstr,fp) :fprintf(fp, "\t%s\n", strval2)); \
	 if (strval2 != ((STRING)value)) {util_free(strval2)}; \
	 strval2 = NULL;

#define write_str_resource(name,value) \
	(wroteResource = TRUE, \
	 abio_printf(fp, "%s, ", name), \
	 (value == NULL? \
	     abio_puts(fp, nullCStr) \
	 : \
	     abio_put_string(fp, (STRING)(value))))


/* ISTRING values */
#define dump_istr_resource(inst, name,value) \
	dump_str_resource(inst, name, istr_string((ISTRING)(value)))

#define write_istr_resource(name,value) \
	write_str_resource(name, istr_string((ISTRING)(value)))


/* LITERAL values (doesn't quote the value string) */
#define write_str_literal_resource(name,value) \
	(wroteResource = TRUE, \
	 abio_printf(fp, "%s, %s", name, (STRING)(value)))

#define write_istr_literal_resource(name,value) \
	write_str_literal_resource(name,istr_string((ISTRING)(value)))

/*
 * Formats and writes a resource.
 * 
 * if dumpResources is TRUE, writes the resource out in resource file format.
 * Otherwise, writes it out in C format.  Does not write out a newline!
 * 
 * obj may be NULL if dumpResources is FALSE
 *
 * These are the values expected for the specific types of arguments:
 * 	- AB_ARG_XMSTRING : ISTRING
 *	- AB_ARG_PIXEL : ISTRING
 * 	- AB_ARG_PIXMAP : ISTRING
 *	- AB_ARG_LITERAL : ISTRING
 *	- AB_ARG_WIDGET : ISTRING (object name, *not* widget name)
 */
int
abmfP_write_arg_val(
		     GenCodeInfo	genCodeInfo,
		     BOOL	dumpResource,	/* put in res. file format */
		     STRING	argName,
		     void	*argValue,
		     ABObj	obj		/* obj the arg belongs to */
)
{
    ISTRING             i_name = objxm_get_res_strname(argName);
    STRING		c_arg_name = istr_string(i_name);
    AB_ARG_TYPE         arg_type = AB_ARG_UNDEF;
    char                dumpWidgetName[MAX_NAME_SIZE];
    char 		strval[MAX_NAME_SIZE+10];
    STRING		strval2 = NULL;
    BOOL		wroteResource= FALSE;
    FILE		*fp= (dumpResource? 
			        (genCodeInfo->resource_file)
			      : (genCodeInfo->code_file));
    *dumpWidgetName = 0;
    *strval = 0;

    if (fp == NULL)
    {
	return ERR_BAD_PARAM;
    }
    if (dumpResource && (obj != NULL))
    {
	ABObj	resourceObj = obj;
        if (   (util_streq(argName, XmNscrollBarDisplayPolicy))
	    && (obj_is_list(obj_get_root(obj)))
	    && (ObjWClassIsScrolledWindow(abmfP_parent(obj))) 
	   )
	{
	    /* this resource was put on the wrong widget */
	    resourceObj = abmfP_parent(obj);
	}
	util_strncpy(dumpWidgetName, 
		abmfP_get_widget_name_for_res_file(resourceObj),
		MAX_NAME_SIZE);
    }

    /* These macros use formal parameters from this specific function */

    arg_type = abmfP_get_res_type(argName, obj);
    switch (arg_type)
    {
    case AB_ARG_INT:
	if (dumpResource)
	{
	    wroteResource = TRUE;
	    dump_int_resource(dumpWidgetName, argName, argValue);
	}
	else 
	{
	    if (obj_is_menu_item(obj) || obj_is_choice_item(obj))
	    {
	    }
	    else
	    {
	        wroteResource = TRUE;
	        abio_printf(fp, "%s,", c_arg_name);
	        abio_printf(fp, " %d", argValue);
	    }
	}
	break;

    case AB_ARG_STRING:
	if (dumpResource)
	{
	    dump_str_resource(dumpWidgetName, argName, argValue);
	}
	else
	{
            /*
             * We try not to call catgets() on a per-resource basis because
             * it might lead to cascaded catgets() calls.  However, not doing 
             * that means we have to be smart enough to have a local variable 
             * to hold the fetched string.  We'll handle all those cases we 
             * know about explicitly, and then fall through to using catgets
             * just to make sure we don't miss something.  Still, we should
             * try and make sure there are local variables for all string-valued
             * resources...
             */

	    /* Local variable case #1: XmNtitle */
            if (util_streq(argName, XmNtitle) 
                && (abmfP_title_str_var_has_value(genCodeInfo)) )
            {
                    write_istr_literal_resource(c_arg_name,
                        abmfP_title_str_var(genCodeInfo));
                    wroteResource= TRUE;
                    break;
            }

	    /* Local variable case #2: XmNvalue (for text objects *only*) */
            if (obj_is_text(obj) && util_streq(argName, XmNvalue) 
                && (abmfP_value_str_var_has_value(genCodeInfo)) )
            {
                    write_istr_literal_resource(c_arg_name,
                        abmfP_value_str_var(genCodeInfo));
                    wroteResource= TRUE;
                    break;
            }

	    /* Local variable case #3: XmNiconName */
            if (util_streq(argName, XmNiconName) 
                && (abmfP_icon_name_str_var_has_value(genCodeInfo)) )
            {
                    write_istr_literal_resource(c_arg_name,
                        abmfP_icon_name_str_var(genCodeInfo));
                    wroteResource= TRUE;
                    break;
            }

            /*
             * TBD: Streamline this and update macros like
             *      write_str_resource to handle i18n transparently
             */
	    /* XmNaccelerator should NOT be localized.  */
            if (genCodeInfo->i18n_method == ABMF_I18N_XPG4_API &&
		!util_streq(argName, XmNaccelerator)) 
            {
                char	*prefix_fmt, *suffix;
                char	*str = (char *)argValue;

                wroteResource = TRUE;

		if (util_streq(argName, XmNtitle) || 
		    util_streq(argName, XmNiconName))
		{
		    prefix_fmt = "%s, XtNewString(%s";
		    suffix = "))";
		}
		else
		{
		    prefix_fmt = "%s, %s";
		    suffix = ")";
		}
		    
                abio_printf(fp, prefix_fmt,   c_arg_name,
                            abmfP_catgets_prefix_str(genCodeInfo, obj, str));
                abio_put_string(fp, str);
                abio_puts(fp, suffix);
            }
            else
            {
                write_str_resource(c_arg_name, argValue);
            }
	}
	break;

    case AB_ARG_VOID_PTR:
	if (util_streq(argName, XmNentryClass))
	{
	    /* widget class */
	    STRING	wclass_name= abmfP_convert_wclass_ptr_to_name(
					(WidgetClass)(argValue));
	    if (wclass_name != NULL)
	    {
		if (dumpResource)
		{
		    dump_str_resource(dumpWidgetName, argName, wclass_name);
		}
		else
		{
		    write_str_literal_resource(c_arg_name, wclass_name);
		}
	    }
	}
    break;

    case AB_ARG_XMSTRING:
	if (dumpResource)
	{
	    dump_istr_resource(dumpWidgetName, argName, argValue);
	}
	else
	{
	    if (util_streq(argName, XmNlabelString))
	    {
		if (abmfP_label_xmstr_var_has_value(genCodeInfo))
	    	{
	            write_istr_literal_resource(c_arg_name, 
			abmfP_label_xmstr_var(genCodeInfo));
	    	    wroteResource= TRUE;
		}
	        else
	    	{
		    if (obj_is_item(obj) &&
			abmfP_label_array_var(genCodeInfo) != NULL)
		    {
		    	sprintf(strval, "%s[%s]",
				istr_string(abmfP_label_array_var(genCodeInfo)),
				istr_string(abmfP_counter_var(genCodeInfo)));
	    	        wroteResource= TRUE;
		    }
		}
	    }
	    else if (util_streq(argName, XmNacceleratorText))
	    {
		if (abmfP_acceltext_xmstr_var_has_value(genCodeInfo))
		{
		    write_istr_literal_resource(c_arg_name,
			abmfP_acceltext_xmstr_var(genCodeInfo));
		    wroteResource = TRUE;
		}
	    }
	    else if (util_streq(argName, XmNokLabelString))
	    {
		if (abmfP_ok_label_xmstr_var_has_value(genCodeInfo))
		{
		    write_istr_literal_resource(c_arg_name,
			abmfP_ok_label_xmstr_var(genCodeInfo));
		    wroteResource = TRUE;
		}
	    }
            else if (util_streq(argName, XmNdirectory))
            {
                if (abmfP_directory_xmstr_var_has_value(genCodeInfo))
                {
                    write_istr_literal_resource(c_arg_name,
                        abmfP_directory_xmstr_var(genCodeInfo));
                    wroteResource = TRUE;
                }
            }
            else if (util_streq(argName, XmNpattern))
            {
                if (abmfP_pattern_xmstr_var_has_value(genCodeInfo))
                {
                    write_istr_literal_resource(c_arg_name,
                        abmfP_pattern_xmstr_var(genCodeInfo));
                    wroteResource = TRUE;
                }
            }
	    if (!wroteResource)
	    {
		if (argValue == NULL)
		{
		    abio_printf(fp, "%s, %s,\n", c_arg_name, nullCStr);
		    wroteResource = TRUE;
		}
		else
		{
		    sprintf(strval, "\"%s\"", 
			istr_string((ISTRING)(argValue)));

	            abio_printf(fp, "XtVaTypedArg, %s,\n", c_arg_name);
		    abio_indent(fp);
		    wroteResource = TRUE;
                    /*
                     * TBD: passing array references for items won't
                     *	 work with catgets: need to fix that
                     */
                    /*
                     * TBD: Streamline this and update macros like
                     *      write_str_resource to handle i18n transparently
                     */
                    if (genCodeInfo->i18n_method == ABMF_I18N_XPG4_API)
                    {
                        char *unq_str_val = istr_string((ISTRING)(argValue));
                        char *str_catg;
                        
                        str_catg = abmfP_catgets_prefix_str(genCodeInfo, obj,
							    unq_str_val);
                    
                        abio_printf(fp, "XmRString, %s%s), strlen(%s%s))+1",
			    str_catg, strval, str_catg, strval);
                    }
                    else
                    {
                        abio_printf(fp, "XmRString, %s, strlen(%s)+1",
                                    strval, strval);
                    }
		    abio_outdent(fp);
		} /* !argValue == NULL */
	    } /* !wroteResource */
	}
	break;
    case AB_ARG_MNEMONIC:
        if (dumpResource)
        {
            wroteResource = TRUE;
            dump_istr_resource(dumpWidgetName, argName, argValue);
        }
        else
        {
            wroteResource = TRUE;
            abio_printf(fp, "%s, ", c_arg_name);
            if (obj_is_item(obj) && 
		abmfP_mnemonic_array_var(genCodeInfo) != NULL)
	    {
                sprintf(strval, "%s[i]", 
			istr_string(abmfP_mnemonic_array_var(genCodeInfo)));
		abio_printf(fp, "%s? (KeySym)XStringToKeysym(%s) : (KeySym)NULL", strval, strval);
	    }
	    else
	    {
                sprintf(strval, "\"%s\"",
                        istr_string((ISTRING)(argValue)));
		abio_printf(fp, "XStringToKeysym(%s)", strval);
	    }
        }
        break;

    case AB_ARG_PIXEL:
	if (dumpResource)
	{
	    dump_istr_resource(dumpWidgetName, argName, argValue);
	}
	else
	{
	    STRING	strval= istr_string((ISTRING)(argValue));
	    STRING	parentWidgetName = 
		abmfP_get_widget_parent_name(genCodeInfo, obj);

	    if (parentWidgetName == NULL)
	    {
		parentWidgetName= abmfP_get_c_name(genCodeInfo, obj);
	    }

	    wroteResource= TRUE;
	    abio_printf(fp, "%s, dtb_cvt_string_to_pixel(%s, ",
		c_arg_name, parentWidgetName);
	    abio_put_string(fp, strval);
	    abio_puts(fp, ")");

	    /*
	    abio_printf(fp, "XtVaTypedArg, %s,\n", c_arg_name);
	    abio_indent(fp);
            abio_printf(fp, "XmRString, \"%s\", strlen(\"%s\")+1",
                        strval, strval);
	    abio_outdent(fp);
	    */
	}
	break;

    case AB_ARG_PIXMAP:
	if (dumpResource)
	{
	}
	else
	{
	    if (util_streq(argName, XmNlabelPixmap))
	    {
	        if (abmfP_label_pixmap_var_has_value(genCodeInfo))
	        {
	            write_str_literal_resource(c_arg_name, 
		        istr_string(abmfP_label_pixmap_var(genCodeInfo)));
		}
	    }
	    else if (util_streq(argName, XmNiconPixmap))
	    {
		if (abmfP_icon_pixmap_var_has_value(genCodeInfo))
		{
	            write_str_literal_resource(c_arg_name, 
		        istr_string(abmfP_icon_pixmap_var(genCodeInfo)));
		}
	    }
	    else if (util_streq(argName, XmNiconMask))
	    {
		if (abmfP_icon_mask_pixmap_var_has_value(genCodeInfo))
		{
	            write_str_literal_resource(c_arg_name, 
		        istr_string(abmfP_icon_mask_pixmap_var(genCodeInfo)));
		}
	    }
	}
	break;

    case AB_ARG_FONT:
	if (dumpResource)
	{
	    dump_str_resource(dumpWidgetName, argName, argValue);
	}
	else
	{
	    wroteResource= TRUE;
	    abio_printf(fp, "XtVaTypedArg, %s,\n", c_arg_name);
	    abio_indent(fp);
	    abio_printf(fp, "XmRString, %s, strlen(%s)+1",
			(STRING)argValue, (STRING) argValue);
	    abio_printf(fp, ",\n");
	    abio_outdent(fp);
	}
	break;

    case AB_ARG_BOOLEAN:
	if (dumpResource)
	{
	    char         *rstring = (argValue? "true" : "false");
	    dump_str_resource(dumpWidgetName, argName, rstring);
	}
	else
	{
	    if (   obj_is_item(obj)
		&& (util_streq(argName, XmNset)) )
	    {
		/* use the "selected" local array */
		wroteResource = TRUE;
	        if (genCodeInfo->i18n_method != ABMF_I18N_XPG4_API)
	           abio_printf(fp, "%s, %s[%s]", c_arg_name,
		       abmfP_get_c_array_of_selected_bools_name(obj),
		       istr_string(abmfP_counter_var(genCodeInfo)));
		else
	           abio_printf(fp, "%s, %s", c_arg_name,
		       obj_is_initially_selected(obj)? "True":"False");
	    }
	    else
	    {
	       wroteResource= TRUE;
	       abio_printf(fp, "%s, %s", c_arg_name,
			   (((Boolean)(unsigned long)argValue)? "True" : "False") );
	    }
	}
	break;

    case AB_ARG_LITERAL:

	if (dumpResource)
	{
	    dump_istr_resource(dumpWidgetName, argName, argValue);
	}
	else if (obj_is_choice_item(obj))
	{
	    break;
	}
	else
	{
	    write_istr_literal_resource(c_arg_name, argValue);
	}
	break;

    case AB_ARG_WIDGET:
    {
	ABObj	module= obj_get_module(obj);
	ABObj	attachObj= NULL;
	if (module != NULL)
	{
	    STRING	attachObjName= istr_string((ISTRING)(argValue));
	    attachObj = obj_scoped_find_by_name(module, attachObjName);
	}
	assert(attachObj != NULL);
	assert(!dumpResource);		/* can't dump widget resources */

	write_str_literal_resource(c_arg_name, 
		abmfP_get_c_name(genCodeInfo, attachObj));
    }
    break;
#ifdef DEBUG
    default:
	util_dprintf(0, "unknown AB_ARG_TYPE %d (argName:%s)\n", 
	    arg_type, util_strsafe(argName));
#endif /* DEBUG */
	/* break; */
    }

    return wroteResource? 0:-1;
}


/*
 * writes out a complete resource-setting line, including
 *     XSetArg(..) or end of a va_list line
 */
int
abmfP_write_arg_val_line(
		    GenCodeInfo	genCodeInfo,
		    BOOL	dumpResource,	/* put in res. file format */
		    STRING	argName,
		    void	*argValue,
		    ABObj	obj,		/* obj the arg belongs to */
		    ABMF_ARG_LIST_FORMAT argFormat
)
{
    File	codeFile = genCodeInfo->code_file;

    if (!dumpResource)
    {
        switch (argFormat)
        {
	    case ABMF_ARGFMT_ARRAY:
		abio_printf(codeFile, "XtSetArg(args[%s], ",
			istr_string(abmfP_arg_counter_var(genCodeInfo)));
	    break;
	    case ABMF_ARGFMT_VA_LIST:
	    break;
        }
    }

    abmfP_write_arg_val(genCodeInfo, dumpResource, argName, argValue, obj);

    if (!dumpResource)
    {
        switch (argFormat)
        {
	    case ABMF_ARGFMT_ARRAY:
		abio_printf(codeFile, ");  ++%s;\n",
			istr_string(abmfP_arg_counter_var(genCodeInfo)));
	    break;
	    case ABMF_ARGFMT_VA_LIST:
		abio_puts(codeFile, ",\n");
	    break;
        }
    }

    return 0;
}


int
abmfP_write_resource(
		     GenCodeInfo	genCodeInfo,
		     BOOL	dumpResource,	/* put in res. file format */
		     Arg	*arg,		/* argument to format/write*/
		     ABObj	obj		/* obj the arg belongs to */
)
{
    return abmfP_write_arg_val(genCodeInfo, 
		dumpResource, arg->name, (void *)(arg->value), obj);
}


/*
 * Writes all arguments, untyped before typed
 *
 * Does *not* print out the i = 0 or XtVaSetValues before spewing
 * the resources! This must have already been done.
 */
int
abmfP_obj_spew_args(
		    GenCodeInfo		genCodeInfo,
		    ABObj		obj,
		    ABMF_ARG_CLASS	argClasses,
		    ABMF_ARG_LIST_FORMAT argFormat
)
{
    int                 return_value = 0;
    int                 iRC = 0;/* int return code */

    if ((argClasses & ABMF_ARGCLASS_OTHER) != 0)
    {
        iRC = abmfP_obj_spew_one_class_args(genCodeInfo, obj,
			ABMF_ARGCLASS_OTHER, argFormat);
        return_if_err(iRC, iRC);
    }

    if ((argClasses & ABMF_ARGCLASS_SENSITIVITY) != 0)
    {
        iRC = abmfP_obj_spew_one_class_args(genCodeInfo, obj,
			ABMF_ARGCLASS_SENSITIVITY, argFormat);
        return_if_err(iRC, iRC);
    }

    if ((argClasses & ABMF_ARGCLASS_TYPED) != 0)
    {
        iRC = abmfP_obj_spew_one_class_args(genCodeInfo, obj,
			ABMF_ARGCLASS_TYPED, argFormat);
        return_if_err(iRC, iRC);
    }

    if ((argClasses & ABMF_ARGCLASS_WIDGET_REF) != 0)
    {
        iRC = abmfP_obj_spew_one_class_args(genCodeInfo, obj,
			ABMF_ARGCLASS_WIDGET_REF, argFormat);
        return_if_err(iRC, iRC);
    }

    switch (obj_get_type(obj))
    {
	case AB_TYPE_LIST:
	    spew_extra_list_args(genCodeInfo, obj, argClasses);
	break;

	case AB_TYPE_CHOICE:
	    spew_extra_choice_args(genCodeInfo, obj, argClasses, argFormat);
	break;

	case AB_TYPE_SPIN_BOX:
            spew_extra_spinbox_args(genCodeInfo, obj, argClasses);
	break;
    }

epilogue:
    return return_value;
}


/*
 * Assumes: argClass has exactly one bit set.
 *
 * If arg is to be written, bug argFormat is ABMF_ARGFMT_UNDEF, the
 * arg is ignored.
 */
static int
abmfP_obj_spew_one_class_args(
			    GenCodeInfo		genCodeInfo,
			    ABObj		obj,
			    ABMF_ARG_CLASS	argClass,
			    ABMF_ARG_LIST_FORMAT argFormat
)
{
    int			return_value = 0;
    int			objCount = 0;
    ABObj		actualObj = obj_get_actual_obj(obj);
    ArgList             argList = NULL;
    Arg                *arg = NULL;
    ABObj               project = obj_get_project(obj);
    ABObj               menu = NULL;


    for (objCount = 0; objCount < 2; ++objCount)
    {
	if (objCount == 0)
	{
	    argList = objxm_obj_get_ui_args(obj);
	}
	else
	{
	    if ((actualObj == NULL) || (actualObj == obj))
	    {
		continue;		/* no reference! */
	    }
	    else
	    {
		/* we have a reference to another obj - dump its resources
		 * as if they were on this obj.
		 */
		argList = objxm_obj_get_ui_args(actualObj);
	    }
	}

      for (arg = (Arg *) argList;
	 (arg != NULL) && (arg->name != NULL); ++arg)
      {
	switch (argClass)
	{
	    case ABMF_ARGCLASS_SENSITIVITY:
		if (!abmfP_arg_is_sensitivity(arg->name))
		{
		    continue;
		}
	    break;

	    case ABMF_ARGCLASS_TYPED:
		if (!abmfP_arg_is_typed(arg->name))
		{
		    continue;
		}
	    break;

	    case ABMF_ARGCLASS_WIDGET_REF:
		if (!abmfP_arg_is_widget_ref(arg->name))
		{
		    continue;
		}
	    break;

	    case ABMF_ARGCLASS_OTHER:
		if (   abmfP_arg_is_sensitivity(arg->name) 
		    || abmfP_arg_is_typed(arg->name)
		    || abmfP_arg_is_widget_ref(arg->name))
		{
		    continue;
		}
	    break;

	    default:
		continue;
	    /* break; */
	}

	/***** IT QUALIFIES - SPEW IT *****/

	if (abmfP_arg_is_dumped(genCodeInfo, arg->name, obj))
	{
	    abmfP_write_resource(genCodeInfo, TRUE, arg, obj);
	}
	else if (abmfP_arg_is_written(genCodeInfo, arg->name, obj))
	{
	    if (   (argFormat == ABMF_ARGFMT_VA_LIST)
	        || (argFormat == ABMF_ARGFMT_ARRAY) )
	    {
	        File	codeFile= genCodeInfo->code_file;

	        if (   (!abmfP_label_pixmap_var_has_value(genCodeInfo))
		    && (   (util_streq(arg->name, XmNlabelPixmap))
		        || (util_streq(arg->name, XmNlabelType)) ))
	        {
		    /* Don't write label resources - a dtb_ convenience
		     * function will be written out that sets the label.
		     */
	        }
	        else switch (argFormat)
	        {
		    case ABMF_ARGFMT_VA_LIST:
	                if (abmfP_write_resource(genCodeInfo, FALSE, arg, obj) >= 0)
		        {
		            abio_printf(codeFile, ",\n");
		        }
		    break;
    
		    case ABMF_ARGFMT_ARRAY:
		        abio_printf(codeFile, "XtSetArg(args[%s], ",
		            istr_string(abmfP_arg_counter_var(genCodeInfo)));
	                abmfP_write_resource(genCodeInfo, FALSE, arg, obj);
		        abio_printf(codeFile, ");  ++%s;\n",
		            istr_string(abmfP_arg_counter_var(genCodeInfo)));
		    break;
	        }
	    }
        } /* abmfP_arg_is_written */
      } /* for arg */
    } /* for objCount */

    return return_value;
}

static int
spew_extra_item_strings_args(
                GenCodeInfo     genCodeInfo,
                ABObj           first_item,
		int		num_items,
		STRING		strings_res_name,
		STRING		strings_count_res_name
)
{
    FILE        *codeFile = genCodeInfo->code_file;

    if (abmfP_arg_counter_var(genCodeInfo) &&
	abmfP_get_c_array_of_xmstrings_name(first_item))
    {
        abio_printf(codeFile, "XtSetArg(args[%s], "
                "%s, (XtArgVal)%s); %s++;\n",
                istr_string(abmfP_arg_counter_var(genCodeInfo)),
		strings_res_name,
                abmfP_get_c_array_of_xmstrings_name(first_item),
                istr_string(abmfP_arg_counter_var(genCodeInfo)));

        abio_printf(codeFile,
                "XtSetArg(args[%s], "
                "%s, (XtArgVal)%d); %s++;\n",
                istr_string(abmfP_arg_counter_var(genCodeInfo)),
		strings_count_res_name,
                num_items,
                istr_string(abmfP_arg_counter_var(genCodeInfo)));
    }
    return 0;
}


static int
spew_extra_choice_args(
		GenCodeInfo	genCodeInfo, 
		ABObj 		obj, 
		ABMF_ARG_CLASS	argClasses,
		ABMF_ARG_LIST_FORMAT	argFormat
)
{
    int		numItems = 0;
    ABObj	wholeObj = obj_get_root(obj);
    ABObj	actualObj = objxm_comp_get_subobj(wholeObj, AB_CFG_OBJECT_OBJ);

    if (   (obj_get_choice_type(wholeObj) != AB_CHOICE_OPTION_MENU)
	|| (actualObj != obj))
    {
	return 0;
    }

    numItems = obj_get_num_items(wholeObj);

    /*
     * subMenuId resource - we don't consider this a widget-ref resource
     */
    if (((argClasses & ABMF_ARGCLASS_OTHER) != 0) && (numItems > 0))
    {
	ABObj menuObj = objxm_comp_get_subobj(wholeObj, AB_CFG_PARENT_OBJ);
	/* this choice has items, which are contained in a menu pane */
	if (menuObj != NULL)
	{
	    #ifdef OBSOLETE
	    abio_printf(genCodeInfo->code_file, "XtSetArg(args[%s], "
	        "XmNsubMenuId, (XtArgVal)%s); %s++;\n",
		istr_string(abmfP_arg_counter_var(genCodeInfo)),
		abmfP_abmfP_get_c_name(genCodeInfo, menuObj),
		istr_string(abmfP_arg_counter_var(genCodeInfo)));
	    #endif /* OBSOLETE */
	    STRING	argName = XmNsubMenuId;
	    ISTRING	objName = istr_dup(obj_get_name_istr(menuObj));

	    abmfP_write_arg_val_line(
		     genCodeInfo,
		     abmfP_arg_is_dumped(genCodeInfo, argName, menuObj),
		     argName,
		     objName,
		     menuObj,
		     argFormat);

	    istr_destroy(objName);
	}
    }

    return 0;
}


static int
spew_extra_list_args(
		GenCodeInfo	genCodeInfo, 
		ABObj		obj, 
		ABMF_ARG_CLASS	argClasses
)
{
    FILE	*codeFile = genCodeInfo->code_file;
    int		num_items= obj_get_num_items(obj);
    ABObj	first_item= obj_get_item(obj, 0);
    ABObj	parent= abmfP_parent(obj);
    ABObj	actual_obj= objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
    BOOL	obj_is_actual_obj= (actual_obj == obj);

    /* 
    * If the parent is not written out, we need to write it's args here
    */
    if (    obj_is_actual_obj
	&& (obj_get_root(obj) == obj_get_root(parent))
	&& (ObjWClassIsScrolledWindow(parent))
	)
    {
        abmfP_obj_spew_args(genCodeInfo, parent, argClasses, ABMF_ARGFMT_ARRAY);

        if ((num_items > 0) && ((argClasses & ABMF_ARGCLASS_OTHER) != 0))
	    spew_extra_item_strings_args(genCodeInfo, first_item, num_items,
		"XmNitems","XmNitemCount");
    }
    return 0;
}

static int
spew_extra_spinbox_args(
                GenCodeInfo     genCodeInfo,
                ABObj           obj,
                ABMF_ARG_CLASS  argClasses
)
{
    FILE        *codeFile = genCodeInfo->code_file;
    int         num_items= obj_get_num_items(obj);
    ABObj       first_item= obj_get_item(obj, 0);

    if (obj == objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ) &&
	obj_get_text_type(obj_get_root(obj)) == AB_TEXT_DEFINED_STRING)
    {
   	if ((num_items > 0) && ((argClasses & ABMF_ARGCLASS_OTHER) != 0))
            spew_extra_item_strings_args(genCodeInfo, first_item, num_items,
		"DtNvalues","DtNnumValues"); 
    }    
    return 0;
}


/*
 * Formats a string value for a resource file.
 *
 * If the string is OK, returns string, otherwise, mallocs() a new
 * string. 
 * 
 * Be sure to free the returned value, if it is NOT == string.
 */
static STRING
fmt_strval_for_res_file(STRING string)
{
    BOOL	modified = FALSE;
    int		i = 0;
    int		len = 0;
    char	*newStr = NULL;
    int		newStrI = 0;
    int		newStrLen = 0;
    
    if (string == NULL)
    {
	return NULL;
    }

    len = strlen(string);
    newStrLen = len;
    for (i = 0; i < len; ++i)
    {
	if (string[i] == '\n')
	{
	    modified = TRUE;
	    ++newStrLen;
	}
    }

    /* 
     * Duplicate it.
     */
    if (!modified)
    {
	newStr = string;
    }
    else
    {
	if ((newStr = (STRING)util_malloc((newStrLen+1)*sizeof(char))) == NULL)
	{
	    return NULL;
	}
	newStrI = 0;
	for (i = 0; i < len; ++i)
	{
	    if (string[i] == '\n')
	    {
		newStr[newStrI++] = '\\';
		newStr[newStrI++] = 'n';
	    }
	    else
	    {
		newStr[newStrI++] = string[i];
	    }
	}
	newStr[newStrLen] = 0;
    }

    return newStr;
}


/*
 * TRUE if the arg is dumped to the resource file, OR written to the
 * C file. If this routine returns FALSE, the argument is ignored and
 * not stored anywhere.
 */
BOOL
abmfP_arg_is_spewed(GenCodeInfo genCodeInfo, STRING argName, ABObj obj)
{
    /*
     * We set resources on items in a loop, but we don't check properly to
     * see if we can remove default values. This is a bug! Unfortunately, 
     * we can't fix it, at this point - the easiest thing to do is to always
     * generate resources for all items.
     */
    return (    obj_is_item(obj)
	    || (!arg_has_default_value(obj, argName)) );
}


/*
 * TRUE if the arg is dumped to the resource file
 */
BOOL
abmfP_arg_is_dumped(GenCodeInfo genCodeInfo, STRING argName, ABObj obj)
{
    AB_ARG_CLASS	argClass = objxm_get_res_class(argName);
    AB_ARG_CLASS_FLAGS	argClassFlag = util_arg_class_to_flag(argClass);

    /* widget-ref resources are never dumped (Xt doesn't deal with it
     * There is no conversion proc for slidingMode
     * There is no conversion proc for XmNtearOffModel
     * There is no conversion proc for DtNarrowLayout
     * There is no conversion proc for DtNspinBoxChildType
     * subMenuId should probably be considered a widgetRef resource, but
     *		it's not, for other reasons. Nonetheless, it CANNOT go
     *		in the resource file!
     */
    return (   ((genCodeInfo->dumped_resources & argClassFlag) != 0)
	    && (!abmfP_arg_is_widget_ref(argName))
	    && (!util_streq(argName, "slidingMode"))
	    && (!util_streq(argName, XmNsubMenuId))
	    && (!util_streq(argName, XmNtearOffModel))
	    && (!util_streq(argName, DtNarrowLayout))
	    && (!util_streq(argName, DtNspinBoxChildType))
	    && (abmfP_arg_is_spewed(genCodeInfo, argName, obj))
	   );
}


/*
 * TRUE if the arg is written to the C file
 */
BOOL
abmfP_arg_is_written(GenCodeInfo genCodeInfo, STRING argName, ABObj obj)
{
    return (   (abmfP_arg_is_spewed(genCodeInfo, argName, obj))
	    && (!abmfP_arg_is_dumped(genCodeInfo, argName, obj))
	   );
	    
}


/*
 * Determines whether a resource is has a widget reference for its value
 */
BOOL
abmfP_arg_is_widget_ref(STRING argName)
{
    /* we don't consider subMenuId a widget-ref resource because 
     * it gets handled more like other resources, and doesn't affect
     * layout.
     */
    return 
	(   (abmfP_get_res_type(argName, NULL) == AB_ARG_WIDGET)
	 && (!util_streq(argName, XmNsubMenuId)) );
}


/*
 * Determine whether a resource specifies a callback.
 */
BOOL
abmfP_arg_is_callback(STRING argName)
{
    return (abmfP_get_res_type(argName, NULL) == AB_ARG_CALLBACK);
}


BOOL
abmfP_arg_is_sensitivity(STRING argName)
{
    return util_streq(argName, XmNsensitive);
}


/*
 * Determine if this is a typed arg
 */
BOOL
abmfP_arg_is_typed(STRING argName)
{
    BOOL	isIt = FALSE;
    AB_ARG_TYPE	arg_type = abmfP_get_res_type(argName, NULL);

    switch (arg_type)
    {
        case AB_ARG_PIXEL:
        case AB_ARG_XMSTRING:
        case AB_ARG_FONT:
	    isIt = TRUE;
	    break;
    }

    return isIt;
}


/*
 * Get the number of args to be written for this beastie
 */
int
abmfP_get_num_args_of_classes(ABObj obj, ABMF_ARG_CLASS argClasses)
{
    int	num_args= 0;
    ABObj	actualObj = obj_get_actual_obj(obj);
    BOOL	countSensitivity = 
			((argClasses & ABMF_ARGCLASS_SENSITIVITY) != 0);
    BOOL	countTyped= ((argClasses & ABMF_ARGCLASS_TYPED) != 0);
    BOOL	countWidgetRef= ((argClasses & ABMF_ARGCLASS_WIDGET_REF) != 0);
    BOOL	countOther= ((argClasses & ABMF_ARGCLASS_OTHER) != 0);

    if (argClasses == ABMF_ARGCLASS_ALL)
    {
        num_args= objxm_obj_get_num_ui_args(obj);
    }
    else
    {
	Arg	*arg= NULL;
	for (arg= objxm_obj_get_ui_args(obj); 
		(arg != NULL) && (arg->name != NULL); ++arg)
	{
	    if (countSensitivity && abmfP_arg_is_sensitivity(arg->name))
	    {
		++num_args;
		continue;
	    }

	    if (countTyped && abmfP_arg_is_typed(arg->name))
	    {
		++num_args;
		continue;
	    }

	    if (countWidgetRef && abmfP_arg_is_widget_ref(arg->name))
	    {
		++num_args;
		continue;
	    }

	    if (   countOther
		&& (! (   abmfP_arg_is_sensitivity(arg->name)
		       || abmfP_arg_is_typed(arg->name)
		       || abmfP_arg_is_widget_ref(arg->name) )))
	    {
		++num_args;
		continue;
	    }
	} /* for arg */
    }

    if (countOther && obj_is_list(obj))
    {
	if (obj_get_num_items(obj) > 0)
	{
	    num_args += 2;	/* itemCount, itemArray */
	}
    }
    if (countOther && obj_is_term_pane(obj))
    {
	if (objxm_comp_get_subobj(obj, AB_CFG_SCROLLBAR_OBJ) != NULL)
	{
	    ++num_args;
	}
    }

    /*
     * The arguments for the obj this one references will get spit out
     * as part of this obj, as well.
     */
    if (actualObj != obj)
    {
        num_args += abmfP_get_num_args_of_classes(actualObj, argClasses);
    }

    return num_args;
}


/*
 * If obj is NULL, returns type if arg type is the same for all objects
 */
AB_ARG_TYPE
abmfP_get_res_type(STRING res_xmname, ABObj obj)
{
    AB_ARG_TYPE	arg_type = objxm_get_res_type(res_xmname);
    if ((arg_type != AB_ARG_VARIABLE) || (obj == NULL))
    {
	return arg_type;
    }

    /*
     * The arg type may depend on the object
     */
    if (util_streq(res_xmname, XmNvalue))
    {
	arg_type = AB_ARG_INT;
	switch (obj_get_type(obj))
        {
	    case AB_TYPE_TEXT_FIELD:
	    case AB_TYPE_TEXT_PANE:
	        arg_type = AB_ARG_STRING;
	    break;
        }
    }

    return arg_type;
}


int	
abmfP_xt_va_list_open(GenCodeInfo genCodeInfo)
{
    abio_indent(genCodeInfo->code_file);
    return 0;
}


int	
abmfP_xt_va_list_open_setvalues(GenCodeInfo genCodeInfo, ABObj obj)
{
    abio_printf(genCodeInfo->code_file, "XtVaSetValues(%s,\n", 
		abmfP_get_c_name(genCodeInfo, obj));
    abio_indent(genCodeInfo->code_file);
    return 0;
}


int	
abmfP_xt_va_list_close(GenCodeInfo genCodeInfo)
{
    abio_printf(genCodeInfo->code_file, "NULL);\n");
    abio_outdent(genCodeInfo->code_file);
    return 0;
}


/*************************************************************************
 **									**
 **		COMPOSITE OBJECTS 					**
 **									**
 *************************************************************************/

int
abmfP_comp_get_num_args_of_classes(ABObj obj, ABMF_ARG_CLASS argClasses)
{
    int			num_args = 0;
    AB_TRAVERSAL	compTrav;
    ABObj		subobj = NULL;
    for (trav_open(&compTrav, obj, AB_TRAV_COMP_SUBOBJS);
	(subobj = trav_next(&compTrav)) != NULL; )
    {
	num_args += abmfP_get_num_args_of_classes(obj, argClasses);
    }
    trav_next(&compTrav);

    return num_args;
}


static BOOL
get_wclass_and_res_value(
			ABObj		obj, 
			STRING		argName, 
			WidgetClass	*wClassOut,
			XtArgVal	*resValueOut
)
{
    WidgetClass	wClass = objxm_get_class_ptr(obj_get_class_name(obj));
    Arg		*arg = NULL;
    BOOL	foundEm = FALSE;

    *wClassOut = NULL;
    *resValueOut = 0;

    if (wClass == NULL)
    {
	goto epilogue;
    }
    (*wClassOut) = wClass;

    for (arg = objxm_obj_get_ui_args(obj);
		((arg != NULL) && (arg->name != NULL)); ++arg)
    {
	if (util_streq(argName, arg->name))
	{
	    (*resValueOut) = arg->value;
	    foundEm = TRUE;
	    break;
	}
    }

epilogue:
    return foundEm? 0:-1;
}


static BOOL
arg_has_default_value(ABObj obj, STRING argName)
{
    WidgetClass		wClass = NULL;
    XtArgVal		resValue = (XtArgVal)NULL;
    ABObj		compRoot = obj_get_root(obj);

    return FALSE;
    if (get_wclass_and_res_value(obj, argName, &wClass, &resValue) < 0)
    {
	return FALSE;
    }

    /*
     * REMIND: move this functionality to libABObjXm
     */
    if (util_streq(argName, XmNradioBehavior))
    {
        if (obj_is_choice(compRoot))
	{
	    /* return TRUE, if the boolean values are logically equal */
	    return (!util_xor(TRUE, obj_get_exclusive(compRoot)));
	}
    }

    return objxm_res_value_is_default(argName, wClass, resValue);
}

