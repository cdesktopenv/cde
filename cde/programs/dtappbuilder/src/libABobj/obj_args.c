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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */

/*
 *	$XConsortium: obj_args.c /main/3 1995/11/06 18:33:49 rswiston $
 *
 * @(#)obj_args.c	3.23 17 Feb 1994	cde_app_builder/src/libABobj
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */


/*
 *  args.c - handles arguments
 */

#ifdef BOGUS /* we don't use ABArgs, any more. The front end uses */
	     /* Xt args, independent from us */

#include <stdlib.h>
#include <string.h>
#include <ab_private/util.h>
#include "objP.h"

#undef obj_set_arg
#define obj_set_arg obj_priv_set_arg

#undef obj_set_arg_if_new
#define obj_set_arg_if_new obj_priv_set_arg_if_new


static ABArgList	MergeArgLists(ABArgList args1, int num_args1, 
				ABArgList args2, int num_args2);
static ABTypedArg	MergeTypedArgLists(ABTypedArg args1, int num_args1,
				ABTypedArg args2, int num_args2);

/* 
 * To this routine just pass in 'Arg *' and the obj of type ABObj.
 * it will merge with existing args for obj and stick it back in 
 * obj->args.
 *
 */
int
obj_set_arg(ABObj obj, AB_ARG_TYPE type, STRING name, void *value)
{
	ABArgList	merged_list= NULL;
	int 		nargs= 0;
	ABArg		oldarg;
	AB_ARG		newarg;

	if ((type == AB_ARG_STRING) || 
	    (type == AB_ARG_XMSTRING) ||
	    (type == AB_ARG_LITERAL))
	{
		ABSetArg(&newarg, type, istr_create(name),
				istr_create((STRING)(value)));
	}
	else
	{
		ABSetArg(&newarg, type, istr_create(name), value);
	}

	if ((oldarg= obj_get_arg(obj, name)) != NULL)
	{
		/* replace existing arg */
		*oldarg= newarg;
		return 0;
	}

	/*
	 * Doesn't exist - create and add it
	 */
	if ((nargs = obj_get_num_args(obj)) == 0)
	{			/* first time ever */
		merged_list= MergeArgLists(&newarg, 1, (ABArgList)NULL, 0);
		obj->args = merged_list;
	}
	else
	{			/* merge with existing args */
		merged_list= MergeArgLists(&newarg, 1, obj->args, nargs);
		(void) free(obj->args);
		obj->args = merged_list;
	}
	return 0;
}

/*
 *  sets the argument only if it doesn't already exist in the object
 *  returns 0 if the arg is set, negative if not
 */
int
obj_set_arg_if_new(ABObj obj, AB_ARG_TYPE type, STRING name, void *value)
{
	ABArg	old_arg= obj_get_arg(obj, name);
	if (old_arg != NULL)
	{
		return 1;
	}
	return obj_set_arg(obj, type, name, value);
}


void
obj_remove_all_args(ABObj obj)
{
	util_free(obj->args);
}


void
obj_remove_all_typed_args(ABObj obj)
{
	util_free(obj->typed_args);
}


/* 
 * To this routine just pass in 'XtTypedArg *' and the obj of type ABObj,
 * it will merge with existing args for obj and stick it back in 
 * obj->typed_args
 *
 */
int
obj_set_typed_arg(
    ABObj	obj, 
    AB_ARG_TYPE	type,
    STRING	name,
    STRING	value_type,
    int		value_size,
    void	*value
)
{
	AB_TYPED_ARG	newarg;
	ABTypedArg	merged_list= NULL;
	int		nargs= 0;

	ABSetTypedArg(&newarg, type, istr_create(name),
		istr_create(value_type), value_size, value);

	if ((nargs= obj_get_num_args(obj)) == 0)
	{			/* first time ever */
		merged_list= MergeTypedArgLists(&newarg, 1, NULL, 0);
		obj->typed_args = merged_list;
	}
	else
	{			/* merge with existing args */
		merged_list= MergeTypedArgLists(&newarg, 1, 
				obj->typed_args, nargs);
		(void) free((STRING )obj->typed_args); 
		obj->typed_args = merged_list;
	}
	return 0;
}


/*
 *  sets the typed argument only if it doesn't already exist in the object
 *  returns 0 if the arg is set, negative if not.
 */
int
obj_set_typed_arg_if_new(
    ABObj	obj, 
    AB_ARG_TYPE	type,
    STRING	name,
    STRING	value_type,
    int		value_size,
    void	*value
)
{
	ABTypedArg	old_targ= obj_get_typed_arg(obj, name);
	if (old_targ != NULL)
	{
		return -1;
	}

	return obj_set_typed_arg(obj, 
			type, name, value_type, value_size, value);
}

/*
 * This routine finds the named arg from the given obj's typed_args and 
 * returns an index to it. If unable to find it returns ERROR(-1).
 */
ABTypedArg 
obj_get_typed_arg(ABObj obj, char *name)
{
	int		num_args;
	int		i;
	ABTypedArg	arg_list= NULL;
	ABTypedArg		arg= NULL;
	ISTRING			iname= istr_get_existing(name);

	if ((num_args= obj_get_num_args(obj)) > 0)
	{
		arg_list = obj->typed_args;
		for ( i = 0; i < num_args; i++)
		{
			if (istr_equal(arg_list[i].name, iname))
			{
				arg= &(arg_list[i]);
				break;
			}
		}
	}

	istr_destroy(iname);
	return arg;
}


/*
 * This routine finds the named arg from the given obj's args and 
 * returns an index to it. If unable to find it returns ERROR(-1).
 */
ABArg
obj_get_arg(ABObj obj, char *name)
{
	int	num_args;
	int	i;
	ABArg	arg_list;
	ABArg		arg= NULL;
	ISTRING		iname= istr_get_existing(name);

	if ((num_args= obj_get_num_args(obj)) > 0)
	{
		arg_list = obj->args;
		for ( i = 0; i < num_args; i++)
		{
			if (istr_equal(arg_list[i].name, iname))
			{
				arg= &(arg_list[i]);
				break;
			}
		}
	}

	istr_destroy(iname);
	return arg;
}



int 
obj_remove_typed_arg(ABObj obj, STRING targ_name)
{
	fprintf(stderr, "WARNING: obj_remove_typed_arg unimplemented!\n");
	return 0;
}


/*
 * This routine merges two argss - allocates dynamic memory.
 */
static ABArgList
MergeArgLists(ABArgList args1, int num_args1, ABArgList args2, int num_args2)
{
	ABArgList result, args;

	result= (ABArg)calloc((unsigned) (num_args1 + num_args2 + 1), 
				sizeof(AB_ARG));
	for (args = result; num_args1 != 0; num_args1--)
		*args++ = *args1++;
	for ( ; num_args2 != 0; num_args2--)
		*args++ = *args2++;
	return result;
}


/*
 * This routine merges two typed(vararg) argss - allocated dynamic 
 * memory. 
 */
static ABTypedArg 
MergeTypedArgLists(ABTypedArg args1, int num_args1,
		   ABTypedArg args2, int num_args2)
{
	ABTypedArg result, args;

	result= (ABTypedArg )calloc((size_t) (num_args1 + num_args2 + 1),
					    (size_t) sizeof(AB_TYPED_ARG));

	for (args = result; num_args1 != 0; num_args1--)
		*args++ = *args1++;
	for ( ; num_args2 != 0; num_args2--)
		*args++ = *args2++;

	return result;
}

/*
 * Remove the specified arg from the args and re-arrange 
 */
int
obj_remove_arg(ABObj obj, STRING name)
{
	int 		i;
	ABArgList	args;
	int 		nargs;
	ISTRING			iname= istr_get_existing(name);

	if ((obj == NULL) || (obj->args == NULL) )
	{
		return 0;
	}
	nargs= obj_get_num_args(obj);	
	if (nargs > 0)
	{
		args = obj->args;
		obj->args = (ABArgList)NULL;
		for (i = 0; i < nargs ; i++)
		{
			if (args[i].name && (istr_equal(args[i].name, iname)))
				continue;
			obj_merge_arglist(obj, &args[i]);
		}
/** LOA		(void)free((STRING )args); **/
	}

	istr_destroy(iname);
	return 0;
}


/* 
 * To this routine just pass in 'Arg *' and the obj of type ABObj,
 * it will merge with existing args for obj and stick it back in obj->args
 *
 */
int
obj_merge_arglist(ABObj obj, ABArg arg)
{
	ABArgList result;
	int nargs= 0;

	if ((nargs= obj_get_num_args(obj)) == 0)
	{			/* first time ever */
		result = MergeArgLists(arg, 1, (ABArgList)NULL, 0);
		obj->args= result;
	}
	else
	{			/* merge with existing args */
		result = MergeArgLists(arg, 1, obj->args, nargs);
		(void) free((STRING ) obj->args);
		obj->args = result;
	}
	return 0;
}

/* 
 * To this routine just pass in 'XtTypedArg *' and the obj of type ABObj,
 * it will merge with existing args for obj and stick it back in 
 * obj->typed_args
 *
 */
int
abo_merge_typed_args(ABObj obj, ABTypedArg arg)
{
	ABTypedArg result ;
	int nargs= 0;


	if ((nargs= obj_get_num_typed_args(obj)) == 0)
	{			/* first time ever */
		result = MergeTypedArgLists(arg, 1, (ABTypedArgList) NULL, 0);
		obj->typed_args = result;
	}
	else
	{			/* merge with existing args */
		result = MergeTypedArgLists(arg, 1, 
				obj->typed_args, nargs);
		(void) free((STRING )obj->typed_args); 
		obj->typed_args = result;
	}
	return 0;
}


/*
 * Returns the number of args in the given ABObj
 */
int
obj_get_num_args(ABObj obj)
{
	ABArgList args= obj->args;
	int n = 0;

        for ( ; (args != (ABArgList)NULL) && (args->name != NULL);
             args++, n++)
		;

        return n;
}


/*
 *    Count number of args in a typed arg list
 */
int
obj_get_num_typed_args(ABObj obj)
{
	ABTypedArg args = obj->typed_args;
	int n = 0;

        for ( ; (args != NULL) && (args->name != NULL); args++, n++)
	{
	}

        return (n);
}
 
#endif /* BOGUS */
