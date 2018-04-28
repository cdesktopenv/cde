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
 *	$XConsortium: test.c /main/4 1996/10/02 15:40:27 drk $
 *
 *	@(#)test.c	3.30 13 Feb 1994	
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
 *  test.c - test driver for libABobj
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ab_private/obj.h>
#include <ab_private/trav.h>

static ABObj	*objArray= NULL;
static int	objArraySize= 0;

static int	print_sizeof(void);
static ABObj	build_test_project(void);
static ABObj	build_test_module(ABObj project);
static int	add_to_array(ABObj obj);
static ABObj	create_one(ABObj parent, 
				AB_OBJECT_TYPE type, ABObj compositeRoot);
static int	set_name(ABObj obj);
static int	show_salient_ui(ABObj root);
static int	show_salient_ui_tree(ABObj root);
static int	check_obj_verify(void);
static int	check_scoped_searches(ABObj root);
static int	check_parents_first(ABObj root);
static int	obj_print_salient_tree(ABObj root);
static int	obj_print_salient_tree_indented(ABObj obj, 
				int spaces, int verbosity, int numPrinted);

static unsigned	travMod= 0;


int 
main (int argc, STRING argv[])
{
    ABObj	tree= NULL;
    ABObj	module1= NULL;
    ABObj	module2= NULL;
    ABObj	module3= NULL;
    ABObj	obj= NULL;
    int		objCount= 0;

   util_init(&argc, &argv);
   travMod= AB_TRAV_MOD_SAFE;		/* use for all traversals */

   /*
   {
       STRING h1= NULL, h2= NULL, h3= NULL;
       h1= "h1"; h2= "h2"; h3= "h3";
       obj= obj_create(AB_TYPE_UNDEF, NULL);
       obj_set_help_data(obj, h1, h2, h3);
       h1= h2= h3= NULL;
       obj_get_help_data(obj, &h1, &h2, &h3);
       printf("'%s', '%s', '%s'\n", h1, h2, h3);
       exit(0);
   }
   */


    /* print_sizeof(); */
    if (util_get_verbosity() < 3)
    {
        util_set_verbosity(3);
    }

    /*
     * Test the obj_verify() method
     */
    check_obj_verify();

    tree= build_test_project();
    if (obj_tree_verify(tree) < 0)
    {
	printf("tree corrupt! %s:%d\n", __FILE__, __LINE__);
	exit(1);
    }

    /*
     * Check scoped searches
     */
    check_scoped_searches(tree);

    /* 
     * Check object counts...
     */
    objCount= trav_count(tree, AB_TRAV_ALL);
    if (objCount != objArraySize)
    {
	printf("object count incorrect! %s:%d\n", __FILE__, __LINE__);
	exit(1);
    }
    printf("Objects in tree: %d\n", objArraySize);

    /*
     * Make sure parents first works
     */
    check_parents_first(tree);

    /*
     * get modules - check find_by_name
     */
    module1= obj_find_module_by_name(tree, "module1");
    module2= obj_find_module_by_name(tree, "module2");
    module3= obj_find_module_by_name(tree, "module3");
    if (   ((module1 == NULL) || (!util_streq(obj_get_name(module1), "module1")))
	|| ((module2 == NULL) || (!util_streq(obj_get_name(module2), "module2")))
	|| ((module3 == NULL) || (!util_streq(obj_get_name(module3), "module3")))
	)
    {
	printf("error %s:%d\n", __FILE__, __LINE__);
	exit(1);
    }

    obj_tree_print(tree);

    /** test salients */
    show_salient_ui(tree);
    show_salient_ui_tree(tree);

    if (obj_tree_verify(tree) >= 0)
    {
       util_printf("Tree is OK\n");
    }
    else
    {
       util_printf("*** TREE IS CORRUPT ***\n");
    }
    return 0;
}

static int
check_obj_verify(void)
{
#define bg_color bg_color_ABOBJPRIVDDDD7609528164
#define name name_ABOBJPRIVDDDD6281986324

    int		rc = 0;		/* return code */
    ABObj	badObjRef = (ABObj)0x01;
    ABObj	obj = NULL;

    rc = obj_verify(badObjRef);
    printf("obj_verify(badObjRef):%d\n", rc);

    obj = obj_create(AB_TYPE_UNDEF, NULL);
    rc = obj_verify(obj);
    printf ("obj_verify(obj): %d\n", rc);

    obj->next_sibling = (ABObj)-1;
    rc = obj_verify(obj);
    printf("obj_verify(badSib):%d\n", rc);

    obj->name = (ISTRING)1000000;
    rc = obj_verify(obj);
    printf("obj_verify(badSibName):%d\n", rc);

    obj->name = NULL;
    obj->next_sibling = NULL;
    obj_set_name(obj, "TEST OBJECT");
    obj->bg_color = (ISTRING)1000000;
    rc = obj_verify(obj);
    printf("obj_verify(objBadBgColor):%d\n", rc);
    obj->bg_color = NULL;

    obj_destroy(obj); obj = NULL;

    return 0;
#undef bg_color
#undef name
}


static int
check_scoped_searches(ABObj root)
{
    ABObj	module1= NULL;
    ABObj	module2= NULL;
    ABObj	module3= NULL;
    ABObj	slider2_parent= NULL;
    ABObj	obj= NULL;
    char	nameBuf[1024] = "";

    module1= obj_find_module_by_name(root, "module1");
    module2= obj_find_module_by_name(root, "module2");
    module3= obj_find_module_by_name(root, "module3");

    /* we know there is one slider in each module. */
    obj= obj_scoped_find_by_name(module1, "slider1");
    printf("(module1, slider1): "); obj_print(obj);

    obj= obj_scoped_find_by_name(module1, "module2::slider2");
    printf("(module1, module2::slider2): "); obj_print(obj);
    if (obj == NULL)
    {
	fprintf(stderr, "SEARCH FAILED %s::%d\n", __FILE__, __LINE__);
	return -1;
    }
    slider2_parent = obj->parent;
    obj= obj_scoped_find_by_name(module3, "module2 :: slider2");
    printf("(module3, module2 :: slider2): "); obj_print(obj);

    obj= obj_scoped_find_by_name(module3, "module2  ::slider2");
    printf("(module3, module2  ::slider2): "); obj_print(obj);

    obj= obj_scoped_find_by_name(module3, "module2.slider2");
    printf("(module3, module2.slider2): "); obj_print(obj);

    obj= obj_scoped_find_by_name(slider2_parent, "slider2");
    printf("(%s, slider2): ", 
	obj_get_safe_name(slider2_parent, nameBuf, 1024));
    obj_print(obj);

    return 0;
}

static int
check_parents_first(ABObj root)
{
    AB_TRAVERSAL	trav;
    AB_TRAVERSAL	parentTrav;
    int			i;
    ABObj		obj= NULL;
    ABObj		ancestor= NULL;
    int			iRet= 0;
    BOOL		*visited= NULL;

    visited= (BOOL *)util_malloc(objArraySize * sizeof(BOOL));
    for (i= 0; i < objArraySize; ++i)
    {
	visited[i]= FALSE;
    }

    for (trav_open(&trav, root, travMod|AB_TRAV_MOD_PARENTS_FIRST);
	(obj= trav_next(&trav)) != NULL; )
    {
	/*
	 * mark object as visited
	 */
	for (i= 0; i < objArraySize; ++i)
	{
	    if (objArray[i] == obj)
	    {
		visited[i]= TRUE;
		break;
	    }
	}

	/*
	 * Make sure all ancestors have been visited
	 */
	for (trav_open(&parentTrav, obj, travMod|AB_TRAV_PARENTS);
		(ancestor= trav_next(&parentTrav)) != NULL; )
	{
	    for (i= 0; i < objArraySize; ++i)
	    {
		if (objArray[i] == ancestor)
		{
		    if (!visited[i])
		    {
			/* this ancestor has not been visited!! */
			fprintf(stderr, "Not all ancestors visited %s:%d\n",
				__FILE__, __LINE__);
			exit(1);
		    }
		}
	    }
	}
	trav_close(&parentTrav);
    }
    trav_close(&trav);
  

    for (i= 0; i < objArraySize; ++i)
    {
	if (!visited[i])
	{
	    fprintf(stderr, "PARENTS_FIRST didn't visit all nodes! %s:%d\n",
		__FILE__, __LINE__);
	    exit(1);
	}
    }
epilogue:
    util_free(visited);
    return iRet;
}

static int
show_salient_ui(ABObj root)
{
    AB_TRAVERSAL	trav;
    ABObj		obj= NULL;
    int			salientCount= 0;
    int			printCount= 0;
    int			i;

    for (i= 0; i < objArraySize; ++i)
    {
	if (obj_is_salient_ui(objArray[i]))
	{
	    ++salientCount;
	}
    }

    printf("\n***** AB_TRAV_SALIENT_UI *****\n");
    printCount= 0;
    for (trav_open(&trav, root, travMod|AB_TRAV_SALIENT_UI);
	(obj= trav_next(&trav)) != NULL; )
    {
	++printCount;
	util_dprintf(0, "% 3d ", printCount);
	obj_print(obj);
    }
    trav_close(&trav);

    if (printCount != salientCount)
    {
	fprintf(stderr, "Incorrect # salient objs returned. %s:%d\n",
		__FILE__, __LINE__);
	exit(1);
    }

    return 0;
}

static int
show_salient_ui_tree(ABObj tree)
{
    int		salientCount= 0;
    int		printCount= 0;
    int		i= 0;

    printf("\n***** AB_TRAV_SALIENT_UI_CHILDREN *****\n");

    salientCount= 0;
    for (i= 0; i < objArraySize; ++i)
    {
	if (obj_is_salient_ui(objArray[i]))
	{
	    ++salientCount;
	}
    }

    printCount= obj_print_salient_tree(tree);

    if (printCount != salientCount)
    {
	fprintf(stderr, "salient counts don't match! %s:%d\n", 
		__FILE__, __LINE__);
	exit(1);
    }

    return printCount;
}

static ABObj
build_test_project()
{
    ABObj	project= NULL;
    
    util_free(objArray); objArraySize= 0;
    project= obj_create(AB_TYPE_PROJECT, NULL);
    add_to_array(project);
    set_name(project);

    build_test_module(project);
    build_test_module(project);
    build_test_module(project);

    return project;
}

static ABObj 
build_test_module(ABObj project)
{
    ABObj	root= NULL;
    ABObj	compRoot= NULL;
    ABObj	obj1= NULL;
    ABObj	obj2= NULL;
    ABObj	obj3= NULL;
    ABObj	obj4= NULL;
    ABObj	obj5= NULL;
    ABObj	obj6= NULL;
    ABObj	obj7= NULL;

    root= project;
    /*
     * Interface
     */
    obj1= create_one(root, AB_TYPE_MODULE, NULL);
    obj1->part_of= obj1;
    compRoot= obj1;

    obj2= create_one(obj1, AB_TYPE_TEXT_FIELD, compRoot);
    obj3= create_one(obj2, AB_TYPE_CONTAINER, NULL);

    obj2= create_one(obj1, AB_TYPE_LIST, compRoot);
    obj3= create_one(obj2, AB_TYPE_CONTAINER, compRoot);
    obj4= create_one(obj3, AB_TYPE_CONTAINER, compRoot);
    obj5= create_one(obj4, AB_TYPE_TEXT_FIELD, compRoot);

    obj3= create_one(obj2, AB_TYPE_CONTAINER, compRoot);
    obj4= create_one(obj3, AB_TYPE_LIST, NULL);

    obj4= create_one(obj3, AB_TYPE_CONTAINER, NULL);
    obj5= create_one(obj4, AB_TYPE_SLIDER, NULL);
    obj5->part_of= obj5;
    compRoot= obj5;

    obj6= create_one(obj5, AB_TYPE_LIST, compRoot);
    obj7= create_one(obj6, AB_TYPE_ITEM, NULL);

    obj6= create_one(obj5, AB_TYPE_ITEM, compRoot);

    compRoot= obj1;

    obj3= create_one(obj2, AB_TYPE_TEXT_FIELD, NULL);

    obj2= create_one(obj1, AB_TYPE_LIST, compRoot);
    obj3= create_one(obj2, AB_TYPE_ITEM, NULL);

    obj3= create_one(obj2, AB_TYPE_LIST, NULL);
    obj3->part_of= obj3;
    compRoot= obj3;

    obj4= create_one(obj3, AB_TYPE_CONTAINER, compRoot);
    obj5= create_one(obj4, AB_TYPE_BUTTON, NULL);

    if (obj_tree_verify(root) < 0)
    {
	fprintf(stderr, "Tree invalid in create_tree!\n");
	exit(1);
    }
    return root;
}

static int
set_name(ABObj obj)
{
    static int	typeCounts[AB_OBJECT_TYPE_NUM_VALUES];
    static BOOL	typeCountsInited= FALSE;
    char	name[256];

    if (!typeCountsInited)
    {
    	int	i;
	typeCountsInited= TRUE;
	for (i= 0; i < AB_OBJECT_TYPE_NUM_VALUES; ++i)
	{
	    typeCounts[i]= 0;
	}
    }
    
    ++(typeCounts[obj->type]);
    sprintf(name, "%s%d", 
	util_object_type_to_string(obj->type)+1, typeCounts[obj->type]);
    obj_set_name(obj, name);
    return 0;
}

static ABObj
create_one(ABObj parent, AB_OBJECT_TYPE type, ABObj compositeRoot)
{
    ABObj	obj= NULL;
    obj= obj_create(type, NULL);
    add_to_array(obj);
    set_name(obj);
    obj_append_child(parent, obj);
    obj->part_of= compositeRoot;
    return obj;
}


/*
 * Returns # of objects printed
 */
static int
obj_print_salient_tree(ABObj obj)
{
	int	iRet= 0;
	int	printCount= 0;

	printf("***** Object tree *****\n");

	printCount= obj_print_salient_tree_indented(obj, 
			0, util_get_verbosity(), printCount);
	iRet= obj_tree_verify(obj);
	if (iRet < 0)
	{
		printf("\n***\n***  TREE IS CORRUPT!\n***");
	}
	else
	{
	    iRet= printCount;
	}
	printf("******* Tree End ******\n");
	printf("\n");

	return iRet;
}


/*
 * Returns total number printed
 */
static int
obj_print_salient_tree_indented(ABObj obj, 
		int spaces, int verbosity, int numPrinted)
{
	AB_TRAVERSAL	trav;
	ABObj		child= NULL;
	int		child_spaces = spaces;

	if (obj == NULL)
	{
		printf("NULL Tree\n");
		return numPrinted;
	}
	util_dprintf(0, "% 3d ", numPrinted);
	if (obj_is_salient_ui(obj))
	{
	    ++numPrinted;
	    obj_print_indented(obj, spaces, verbosity);
	    child_spaces = spaces + 4;
	}
	for (trav_open(&trav, obj, travMod|AB_TRAV_SALIENT_CHILDREN);
		(child= trav_next(&trav)) != NULL; )
	{
		numPrinted= obj_print_salient_tree_indented(child, 
				child_spaces, verbosity, numPrinted);
	}
	trav_close(&trav);

	return numPrinted;
}

static int
print_sizeof(void)
{
	AB_OBJ			obj;
	AB_OBJ_EXTRA_INFO	*info= &(obj.info);

	printf("sizeof obj:%ld\n", (long)sizeof(obj));
	printf("    info:%ld\n", (long)sizeof(obj.info));
	printf("    action:%ld\n", (long)sizeof(info->action));
	printf("    action_list:%ld\n", (long)sizeof(info->action_list));
	printf("    button:%ld\n", (long)sizeof(info->button));
	printf("    drawing_area:%ld\n", (long)sizeof(info->drawing_area));
	printf("    choice:%ld\n", (long)sizeof(info->choice));
	printf("    container:%ld\n", (long)sizeof(info->container));
        printf("    file_chooser:%ld\n", (long)sizeof(info->file_chooser));
        printf("    message_box:%ld\n", (long)sizeof(info->msg_box));    
	printf("    module:%ld\n", (long)sizeof(info->module));
	printf("    item:%ld\n", (long)sizeof(info->item));
	printf("    label:%ld\n", (long)sizeof(info->label));
	printf("    list:%ld\n", (long)sizeof(info->list));
	printf("    menu:%ld\n", (long)sizeof(info->menu));
	printf("    project:%ld\n", (long)sizeof(info->project));
	printf("    text:%ld\n", (long)sizeof(info->text));
	printf("    window:%ld\n", (long)sizeof(info->window));
	printf("\n");

	return 0;
}

static int
add_to_array(ABObj obj)
{
    ++objArraySize;
    objArray= (ABObj *)realloc(objArray, objArraySize * sizeof(ABObj));
    objArray[objArraySize-1]= obj;
    return 0;
}

