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
 *	$XConsortium: test_notify.c /main/3 1995/11/06 18:41:12 rswiston $
 *
 *	@(#)test_notify.c	1.16 14 Feb 1994	cde_app_builder/src/libABobj
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
 *  template.c - template c file.
 */

#ifdef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ab_private/obj_notify.h>
#include <ab_private/trav.h>

typedef struct
{
    int 	free_count;
    STRING	text;
} UPDATE_DATA, *UpdateData;

/*************************************************************************
**                                                                      **
**       Constants (#define and const)					**
**                                                                      **
**************************************************************************/

#define MAX_OBJECTS 1000
#define UPDATE_MSG1 (1)
#define UPDATE_MSG2 (2)

/*************************************************************************
**                                                                      **
**       Private Functions (C declarations and macros)			**
**                                                                      **
**************************************************************************/

#define check_find(obj) (check_find_impl(obj, __FILE__, __LINE__))
#define verify_tree(tree) (verify_tree_impl(tree, __FILE__, __LINE__))
#define verify_count(tree, count) \
	    (verify_count_impl(tree, count, __FILE__, __LINE__))

#define err_exit(str) (err_exit_impl((str), __FILE__, __LINE__))

static int	err_exit_impl(STRING msg, STRING file, int line);
static int 	set_name(ABObj obj);
static STRING	safe_obj_name(ABObj obj);
static int	set_next_name_number(int number);
static ABObj	create_test_project(void);
static int	create_test_module(ABObj project);
static int 	composite_create_module(ABObj module);
static int 	composite_create_list(ABObj module);
static int	composite_create_slider(ABObj slider);
static int	check_find_impl(ABObj obj, STRING file, int line);
static int	verify_count_impl(ABObj obj, int count, STRING file, int line);
static int	verify_tree_impl(ABObj obj, STRING file, int line);
static int	test_destroy(ABObj *treePtr, int *objCountPtr);
static int	test_geometry(ABObj tree);
static int	test_selection(ABObj tree);
static int	test_update(ABObj tree);
static int 	print_selected(ABObj tree);
static int	count_selected(ABObj tree);
static int	free_update_data(int update_code, void *update_data);

/*************************************************************************
**                                                                      **
**       Data     	                                                **
**                                                                      **
**************************************************************************/

static char	msg[1024];
static int 	allObjectsArrayCount= 0;
static ABObj	allObjectsArray[MAX_OBJECTS];
static int	renameCount= 0;
static int	renameEventCount= 0;
static int	reparentCount= 0;
static int	reparentEventCount= 0;
static int	reparentCountsDiff= 0;	/* reparentCount - reparentEventCount */
static int	selectCount= 0;
static int	selectEventCount= 0;
static int	unselectCount= 0;
static int	unselectEventCount= 0;
static int	destroyCount= 0;
static int	destroyEventCount= 0;
static int	updateCount= 0;
static int	updateEventCount= 0;
static int	updateTreeCount= 0;
static int	updateTreeEventCount= 0;
static UPDATE_DATA	update_data1;
static UPDATE_DATA	update_data2;

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/


/*
 * "mod1" module callbacks
 */
static int rename_obj_create_callback(ObjEvCreateInfo info);
static int composite_obj_create_callback(ObjEvCreateInfo info);
static int mod2_obj_create_callback(ObjEvCreateInfo info);
static int rename_callback1(ObjEvAttChangeInfo info);
static int reparent_callback1(ObjEvReparentInfo info);
static int reparent_count_callback(ObjEvReparentInfo info);
static int allow_reparent_callback1(ObjEvAllowReparentInfo info);
static int count_select_callback(ObjEvAttChangeInfo info);
static int composite_select_callback(ObjEvAttChangeInfo info);
static int exclusive_select_callback(ObjEvAttChangeInfo info);
static int grid_geometry_callback(ObjEvAttChangeInfo info);
static int symmetric_geometry_allow_callback(ObjEvAllowGeometryChangeInfo info);
static int composite_geometry_allow_callback(ObjEvAllowGeometryChangeInfo info);
static int bogus_geometry_allow_callback(ObjEvAllowGeometryChangeInfo info);
static int composite_destroy_callback(ObjEvDestroyInfo info);
static int count_destroy_callback(ObjEvDestroyInfo info);
static int composite_update_callback(ObjEvUpdateInfo info);
static int count_update_callback(ObjEvUpdateInfo info);
static int count_update_with_data_callback(ObjEvUpdateWithDataInfo info);
static int composite_update_with_data_callback(ObjEvUpdateWithDataInfo info);


int
main (int argc, char *argv[])
{
    int		iRC= 0;		/* return code */
    ABObj	project= NULL;
    ABObj	module1= NULL;
    int		objCount= 0;
    int		fileMode= 0;

    util_init(argv[0]);

    /*
     * Make all output unbuffered
     */

    obj_add_allow_reparent_callback(
		allow_reparent_callback1, "allow_reparent_callback1");
    obj_add_reparent_callback(
		reparent_callback1, "reparent_callback1");
    obj_add_reparent_callback(
		reparent_count_callback, "reparent_count_callback");
    obj_add_create_callback(
		rename_obj_create_callback, "rename_obj_create_callback");
    obj_add_create_callback(
		composite_obj_create_callback, 
		"composite_obj_create_callback");
    obj_add_create_callback(
		mod2_obj_create_callback, "mod2_obj_create_callback");
    obj_add_rename_callback(
		rename_callback1, "rename_callback1");
    obj_add_selected_change_callback(
		exclusive_select_callback, "obj_add_selected_change_callback");
    obj_add_selected_change_callback(
		count_select_callback, "count_select_callback");
    obj_add_selected_change_callback(
		composite_select_callback, "composite_select_callback");
    obj_add_geometry_change_callback(
		grid_geometry_callback, "grid_geometry_callback");
    obj_add_allow_geometry_change_callback(
		symmetric_geometry_allow_callback, 
		"symmetric_geometry_allow_callback");
    obj_add_allow_geometry_change_callback(
		composite_geometry_allow_callback,
		"composite_geometry_allow_callback");
    obj_add_allow_geometry_change_callback(
		bogus_geometry_allow_callback,
		"bogus_geometry_allow_callback");
    obj_add_destroy_callback(
		composite_destroy_callback, 
		"composite_destroy_callback");
    obj_add_destroy_callback(
		count_destroy_callback, 
		"count_destroy_callback");
    obj_add_update_callback(
		count_update_callback, "count_update_callback");
    obj_add_update_callback(
		composite_update_callback, "composite_update_callback");
    obj_add_update_with_data_callback(
		composite_update_with_data_callback, 
		"composite_update_with_data_callback");
    obj_add_update_with_data_callback(
		count_update_with_data_callback,
		"count_update_with_data_callback");

    project= create_test_project();
    /* obj_print_tree(project); */
    module1= obj_find_module_by_name(project, "module1");
    check_find(module1);

    objCount= trav_count(project, AB_TRAV_ALL);
    if (objCount != allObjectsArrayCount)
    {
	fprintf(stderr, "Count mismatch (%d,%d) %s:%d\n",
		objCount, allObjectsArrayCount, __FILE__, __LINE__);
	exit(1);
    }
    printf("Objects in tree: %d\n", objCount);
    verify_count(project, objCount);

    printf("Rename events processed: %d\n", renameEventCount);
    if (renameEventCount != renameCount)
    {
	fprintf(stderr, "ERROR: %d rename events received!\n",
		renameEventCount);
    }

    /*
     * Test reparent allow
     */
    iRC= obj_unparent(module1); ++reparentCount;
    printf("unparent module: %d\n", iRC);
    if (iRC >= 0)
    {
	fprintf(stderr, "ERROR. Reparent allowed %s:%d\n", __FILE__, __LINE__);
    }
    reparentCountsDiff+= 1;	/* this reparent ended in allow_reparent */
    verify_count(project, objCount);

    /*
     * Test reparenting
     */
    {
	ABObj	slider1= obj_find_by_name(project, "slider101");
	ABObj	container3= obj_find_by_name(project, "container103");
	ABObj	container5= obj_find_by_name(project, "container105");
	ABObj	list2= obj_find_by_name(project, "list102");
	ABObj	list3= obj_find_by_name(project, "list103");
	check_find(slider1);
	check_find(container3);
	check_find(container5);
	check_find(list2);
	check_find(list3);

	verify_count(project, objCount);

	printf("test reparent\n");
	obj_reparent(slider1, container3); ++reparentCount;
	verify_count(project, objCount);

	printf("test move_children\n");
	reparentCount+= obj_get_num_children(container3);
	obj_move_children(list2, container3);
	verify_count(project, objCount);


	/*
	 * Put it back the way it was!
	 */
	obj_reparent(slider1, container5); ++reparentCount;
	obj_reparent(list3, container3); ++reparentCount;
	obj_reparent(container5, container3); ++reparentCount;
	verify_count(project, objCount);
    }

    printf("reparents - performed:%d   events processed: %d\n", 
	reparentCount, reparentEventCount);
    if ((reparentCount - reparentEventCount) != reparentCountsDiff)
    {
	fprintf(stderr, "ERROR: reparent counts don't match! %s:%d\n", 
		__FILE__, __LINE__);
	exit(1);
    }
    verify_count(project, objCount);

    test_selection(project);
    verify_count(project, objCount);

    test_geometry(project);
    verify_count(project, objCount);

    test_update(project);
    verify_count(project, objCount);

    test_destroy(&project, &objCount);
    verify_count(project, objCount);

    fflush(stdout);
    fflush(stderr);
    printf("\n*** TESTS COMPLETED SUCCESSFULLY ***\n\n");
    exit(0);
}


static int
err_exit_impl(STRING msg, STRING file, int line)
{
    if (msg != NULL)
    {
	fprintf(stderr, "ERROR. %s  %s:%d\n", msg, file, line);
    }
    exit(1);
    return 0;
}


static int
test_selection(ABObj tree)
{
    ABObj	button1= obj_find_by_name(tree, "button101");
    ABObj	module1= obj_find_module_by_name(tree, "module1");
    ABObj	list5= obj_find_by_name(tree, "list105");
    ABObj	container5= obj_find_by_name(tree, "container105");
    check_find(button1);
    check_find(module1);
    check_find(list5);
    check_find(container5);

    obj_select(button1); selectCount+= 1;
    print_selected(tree);
    if (count_selected(tree) != 1)
    {
	fprintf(stderr, "ERROR. incorrect select count %s:%d\n",
			__FILE__, __LINE__);
	exit(1); 
    }

    obj_select(module1); selectCount += 8;
    print_selected(tree);
    if (count_selected(tree) != 8)
    {
	fprintf(stderr, "ERROR. incorrect select count %s:%d\n",
			__FILE__, __LINE__);
	exit(1); 
    }

    obj_unselect(module1);
    print_selected(tree);
    if (count_selected(tree) != 0)
    {
	fprintf(stderr, "ERROR: unselect failed. %s:%d\n",
		__FILE__, __LINE__);
	exit(1);
    }

    printf("selections - performed:%d   events:%d\n", 
		selectCount, selectEventCount);
    if (selectCount != selectEventCount)
    {
       fprintf(stderr, "ERROR: Select event count mismatch %s:%d\n",
		__FILE__, __LINE__);
	exit(1);
    }

    return OK;
}


static int
test_update(ABObj tree)
{
    ABObj	module1= obj_find_module_by_name(tree, "module1");
    ABObj	slider1= obj_find_by_name(tree, "slider101");
    ABObj	item2= obj_find_by_name(tree, "item102");
    int		numObjects= 0;
    int		old_verbosity= util_get_verbosity();
    int		data1_free_count= 0;
    int		data2_free_count= 0;
    check_find(module1);
    check_find(slider1);
    check_find(item2);

    printf("testing update messages\n");
    update_data1.free_count= 0;
    update_data1.text= "Data packet 1";
    data1_free_count= 0;
    update_data2.free_count= 0;
    update_data2.text= "Data packet 2";
    data2_free_count= 0;

    updateCount= 0;
    updateEventCount= 0;
    updateTreeCount= 0;
    updateTreeEventCount= 0;

    /*
     * Composite
     */
    printf("updating an internal composite\n");
    obj_update_clients(module1); updateCount+= 8;
    if (   (updateCount != updateEventCount)
	|| (updateTreeCount != updateTreeEventCount) )
    {
	err_exit("Update count mismatch");
    }


    /*
     * subtree - should get one update_tree method, which will cause
     * more update object messages.
     */
    util_set_verbosity(4);
    numObjects= trav_count(module1, AB_TRAV_ALL);
    printf("updating module subtree\n");
    obj_tree_update_clients(module1); 
    updateTreeCount+= 1; updateCount+= 7;
    if (   (updateCount != updateEventCount)
	|| (updateTreeCount != updateTreeEventCount) )
    {
	err_exit("Update count mismatch");
    }

    /*
     * Update a composite w/data
     */
    obj_update_clients_with_data(slider1, 
			UPDATE_MSG1, (void *)&update_data1, free_update_data);
    updateCount+= 3;
    ++data1_free_count;
    if (updateCount != updateEventCount)
    {
	err_exit("Bad update event count");
    }


    /*
     * obj_tree_update_clients_with_data on large subtree
     */
    printf("testing tree update clients on complex subtree\n");
    numObjects= trav_count(module1, AB_TRAV_ALL);
    obj_tree_update_clients_with_data(module1, UPDATE_MSG2,
		(void *)&update_data2, free_update_data);
    updateCount+= numObjects;
    ++data2_free_count;

    /*
     * obj_tree_update_clients on single object
     */
    printf("update tree on single object\n");
    obj_tree_update_clients_with_data(item2, UPDATE_MSG2,
		(void *)&update_data2, free_update_data);
    ++updateCount;
    ++data2_free_count;

    if (update_data1.free_count != data1_free_count)
    {
	err_exit("bad free count");
    }
    printf("data1 free count: %d (OK)\n", update_data1.free_count);
    if (update_data2.free_count != data2_free_count)
    {
	err_exit("bad free count");
    }
    printf("data2 free count: %d (OK)\n", update_data2.free_count);


    util_set_verbosity(old_verbosity);
    return 0;
}


static int
test_geometry(ABObj tree)
{
    int		iRC= 0;		/* return code */
    ABObj	list1= obj_find_by_name(tree, "list101");
    ABObj	container4= obj_find_by_name(tree, "container104");
    ABObj	slider1= obj_find_by_name(tree, "slider101");
    int		x, y, w, h;
    check_find(list1);
    check_find(container4);
    check_find(slider1);

    printf("Testing geometry callbacks.\n");

    /* 
     * should be disallowed because list1 is a comp. subobj 
     */
    if ((iRC= obj_test_move(list1, 10, 10)) >= 0)
    {
	err_exit("illegal move allowed");
    }
    if (iRC != ERR_NOT_ALLOWED)
    {
	err_exit("bad error code");
    }

    if (obj_move(list1, 10, 10) != ERR_NOT_ALLOWED)
    {
	err_exit("illegal move allowed");
    }

    /* 
     * legal! (no adjustment necessary)
     */
    if (obj_move(slider1, 10, 10) < 0)
    {
	err_exit("legal move DISallowed");
    }

    /* 
     * legal, but should get moved to 10, 10 
     */
    if ((iRC= obj_test_set_geometry(container4, 5, 5, 100, 100)) < 0)
    {
	err_exit("legal set_geo DISallowed");
    }
    if ((iRC= obj_set_geometry(container4, 5, 5, 100, 100)) < 0)
    {
	err_exit("legal set_geo DISallowed");
    }
    obj_get_geometry(container4, &x, &y, &w, &h);
    if (! ((x==10) && (y==10) && (w==100) && (h==100)) )
    {
	err_exit("move not handled properly");
    }
    if (iRC < 0)
    {
	err_exit("bad error code!\n");
    }

    /*
     * illegal - width != height
     */
    if (obj_resize(container4, 55, 60) >= 0)
    {
	err_exit("illegal resize allowed!");
    }
    if (obj_resize(container4, 60, 60) < 0)
    {
	err_exit("legal resize DISallowed");
    }
    obj_get_size(container4, &w, &h);
    if (! ((w == 60) && (h == 60)) )
    {
	err_exit("resize not handled properly!");
    }

    /*
     * The bogus allow callback should try to move an object and 
     * should generate an error in the callback itself.
     */
    obj_move(container4, 1000, 1000);

    return 0;
}


static int
test_destroy(ABObj *treePtr, int *objCountPtr)
{
#define tree (*treePtr)
#define objCount (*objCountPtr)
    int		iRC= 0;		/* return code */
    ABObj	module1= obj_find_module_by_name(tree, "module1");
    ABObj	button1= obj_find_by_name(tree, "button101");
    ABObj	slider1= obj_find_by_name(tree, "slider101");
    ABObj	container5= obj_find_by_name(tree, "container105");
    check_find(module1);
    check_find(button1);
    check_find(slider1);
    check_find(container5);

    printf("Testing destroy callbacks\n");

    /*
     * atomic leaf node - should be OK
     */
    printf("deleting atomic leaf node\n");
    iRC= obj_destroy(button1); --objCount; ++destroyCount;
    if (iRC < 0)
    {
	err_exit("legal destroy DISallowed");
    }

    printf("destroying interior atomic\n");
    iRC= obj_destroy_one(container5); --objCount; ++destroyCount;
    if (iRC < 0)
    {
	err_exit("legal destroy failed");
    }
    verify_count(tree, objCount);

    printf("destroying interior composite\n");
    iRC= obj_destroy_one(module1); objCount-= 8; destroyCount+= 8;
    verify_count(tree, objCount);

    printf("destroying complex subtree\n");
    iRC= obj_destroy(slider1); objCount-= 4; destroyCount+= 4;
    verify_count(tree, objCount);

    verify_count(tree, objCount);	/* be sure objCount is up-to-date */
    iRC= obj_destroy(tree); destroyCount+= objCount; objCount= 0;
    tree= NULL;
    verify_count(tree, objCount);

    if (destroyCount != destroyEventCount)
    {
	err_exit("mismatched destroy counts");
    }

    return 0;
#undef tree
#undef objCount
}


static int
check_find_impl(ABObj obj, STRING file, int line)
{
    int		iReturn= 0;
    if (obj == NULL)
    {
	fprintf(stderr, "Bad find %s:%d\n", file, line);
	exit(1);
    }
    return iReturn;
}


static int
verify_count_impl(ABObj tree, int count, STRING file, int line)
{
    int 	localCount= 0;

    if (tree != NULL)
    {
        if (obj_tree_verify(tree) < 0) 
        {
	    fprintf(stderr, "ERROR: corrup tree %s:%d\n", file, line);
	    exit(1);
        }
	localCount= trav_count(tree, AB_TRAV_ALL);
    }
    
    if (count != localCount)
    {
        fprintf(stderr, "ERROR: bad count %s:%d\n", file, line);
	exit(1);
    }

    return 0;
}


static int
verify_tree_impl(ABObj obj, STRING file, int line)
{
    if (obj_tree_verify(obj) < 0)
    {
	fprintf(stderr, "ERROR. corrupt tree %s:%d\n", file, line);
	exit(1);
    }
    return 0;
}

static int
print_selected(ABObj tree)
{
    AB_TRAVERSAL	trav;
    ABObj		obj= NULL;
    int			selCount= 0;

    printf("Selected: ");
    for (trav_open(&trav, tree, AB_TRAV_ALL | AB_TRAV_MOD_PARENTS_FIRST);
	(obj= trav_next(&trav)) != NULL; )
    {
	if (obj_is_selected(obj))
	{
	    ++selCount;
	    if (selCount == 1)
	    {
		/* printf("\n"); */
	    }
	    printf("%s ", safe_obj_name(obj));
	}
    }
    trav_close(&trav);

    if (selCount == 0)
    {
	printf("[NONE]");
    }
    printf("\n");
    return 0;
}


static int
count_selected(ABObj tree)
{
    int		selectCount= 0;
    AB_TRAVERSAL	trav;
    ABObj		obj= NULL;

    for (trav_open(&trav, tree, AB_TRAV_ALL);
	(obj= trav_next(&trav)) != NULL; )
    {
	if (obj_is_selected(obj))
	{
	    ++selectCount;
	}
    }
    trav_close(&trav);

    return selectCount;
}


static ABObj
create_test_project(void)
{
    ABObj	project= NULL;
    ABObj	module1= NULL;
    ABObj	module2= NULL;
    ABObj	module3= NULL;
    ABObj	obj= NULL;
    ABObj	button= NULL;
    ABObj	parent= NULL;

    project= obj_create(AB_TYPE_PROJECT, NULL);
    create_test_module(project);
    create_test_module(project);
    create_test_module(project);

    module1= obj_find_module_by_name(project, "module101");
    check_find(module1);
    module2= obj_find_module_by_name(project, "module201");
    check_find(module2);
    module3= obj_find_module_by_name(project, "module301");
    check_find(module3);

    if (module1 != NULL)
    {
	obj_set_name(module1, "module1"); ++renameCount;
    }
    if (module2 != NULL)
    {
	obj_set_name(module2, "module2"); ++renameCount;
    }
    if (module3 != NULL)
    {
	obj_set_name(module3, "module3"); ++renameCount;
    }

    return project;
}


static int
create_test_module(ABObj project)
{
    static	int module_number= 0;
    int		baseNumber = 0;
    ABObj	button1= NULL;
    ABObj	module= NULL;
    ABObj	tf1= NULL;
    char	tf1Name[256] = "";
    ABObj	container3= NULL;
    char	container3Name[256] = "";
    ABObj	container5= NULL;
    char	container5Name[256] = "";
    ABObj	container6= NULL;
    char	container6Name[256] = "";
    ABObj	item3= NULL;
    char	item3Name[256] = "";
    ABObj	slider1= NULL;
    char	slider1Name[256] = "";
    ABObj	list1= NULL;
    char	list1Name[256] = "";
    ABObj	list2= NULL;
    char	list2Name[256] = "";
    ABObj	list4= NULL;
    char	list4Name[256] = "";
    ABObj	list5= NULL;
    char	list5Name[256] = "";

    ++module_number;
    baseNumber = module_number * 100;
    set_next_name_number(baseNumber + 1);

    sprintf(tf1Name, "text-field%d", baseNumber + 1);
    sprintf(container3Name, "container%d", baseNumber + 3);
    sprintf(container5Name, "container%d", baseNumber + 5);
    sprintf(container6Name, "container%d", baseNumber + 6);
    sprintf(item3Name, "item%d", baseNumber + 3);
    sprintf(slider1Name, "slider%d", baseNumber + 1);
    sprintf(list1Name, "list%d", baseNumber + 1);
    sprintf(list2Name, "list%d", baseNumber + 2);
    sprintf(list4Name, "list%d", baseNumber + 4);
    sprintf(list5Name, "list%d", baseNumber + 5);

    module= obj_create(AB_TYPE_MODULE, project);
        tf1= obj_find_by_name(module, tf1Name);
	check_find(tf1);
        obj_create(AB_TYPE_CONTAINER, tf1);

	container3= obj_find_by_name(module, container3Name);
	check_find(container3);
        obj_create(AB_TYPE_LIST, container3);

            container5= obj_create(AB_TYPE_CONTAINER, container3);

                slider1= obj_create(AB_TYPE_SLIDER, container5);

                    list4= obj_find_by_name(slider1, list4Name);
    		    check_find(list4);
    		    obj_create(AB_TYPE_ITEM, list4);

	list1= obj_find_by_name(module, list1Name);
	check_find(list1);
	obj_create(AB_TYPE_TEXT_FIELD, list1);

	list2= obj_find_by_name(module, list2Name);
	check_find(list2);
	list5= obj_create(AB_TYPE_LIST, list2);

	    item3= obj_create(AB_TYPE_ITEM, list2);

	    container6= obj_find_by_name(list5, container6Name);
	    check_find(container6);
	    button1= obj_create(AB_TYPE_BUTTON, container6);

    return 0;
}


static int	typeCounts[AB_OBJECT_TYPE_NUM_VALUES];
static BOOL	typeCountsInited= FALSE;

static int
set_next_name_number(int number)
{
    int i;
    typeCountsInited= TRUE;
    for (i= 0; i < AB_OBJECT_TYPE_NUM_VALUES; ++i)
    {
	typeCounts[i]=  number-1;
    }
    return 0;
}


static STRING
safe_obj_name(ABObj obj)
{
    if (obj == NULL)
    {
	return "NULL";
    }
    return util_strsafe(obj_get_name(obj));
}


static int
set_name(ABObj obj)
{
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
    obj_set_name(obj, name); ++renameCount;
    return 0;
}

/*************************************************************************
**									**
**		CREATE CALLBACKS					**
**									**
*************************************************************************/

static int 
rename_obj_create_callback(ObjEvCreateInfo info)
{
    static int objNum= 0;

    /* printf("rename_obj_create_callback (objNum:%d)\n", objNum); */

    set_name(info->obj);

    /* printf("rename_obj_create_callback (EXIT)\n"); */
    return OK;
}


static int
composite_obj_create_callback(ObjEvCreateInfo info)
{
    ABObj	obj= info->obj;

    if (obj_has_flag(obj, XmConfiguredFlag))
    {
	return OK;
    }

    if (obj_is_module(obj))
    {
	composite_create_module(obj);
    }
    else if (obj_is_list(obj))
    {
	composite_create_list(obj);
    }
    else if (obj_is_slider(obj))
    {
	composite_create_slider(obj);
    }

    obj_set_flag(obj, XmConfiguredFlag);

    return OK;
}

static int
composite_create_module(ABObj module)
{
    static int	module_number= 0;
    int		iReturn= 0;
    ABObj	tf1= NULL;
    ABObj	tf2= NULL;
    ABObj	list1= NULL;
    ABObj	list2= NULL;
    ABObj	cont1= NULL;
    ABObj	cont2= NULL;
    ABObj	cont3= NULL;

    module->part_of= module;
    obj_set_flag(module, XmConfiguredFlag);

        tf1= obj_create(AB_TYPE_TEXT_FIELD, module);
	tf1->part_of= module;
	obj_set_flag(tf1, XmConfiguredFlag);

        list1= obj_create(AB_TYPE_LIST, module);
	list1->part_of= module;
	obj_set_flag(list1, XmConfiguredFlag);

            cont1= obj_create(AB_TYPE_CONTAINER, list1);
	    cont1->part_of= module;
	    obj_set_flag(cont1, XmConfiguredFlag);

                cont2= obj_create(AB_TYPE_CONTAINER, cont1);
		cont2->part_of= module;
		obj_set_flag(cont2, XmConfiguredFlag);

                    tf2= obj_create(AB_TYPE_TEXT_FIELD, cont2);
		    tf2->part_of= module;
	            obj_set_flag(tf2, XmConfiguredFlag);

            cont3= obj_create(AB_TYPE_CONTAINER, list1);
	    cont3->part_of= module;
	    obj_set_flag(cont3, XmConfiguredFlag);

        list2= obj_create(AB_TYPE_LIST, module);
	list2->part_of= module;
	obj_set_flag(list2, XmConfiguredFlag);

    
    return iReturn;
}

static int
composite_create_list(ABObj list)
{
    ABObj	parent= obj_get_parent(list);

    if ((parent != NULL) && obj_is_list(parent))
    {
	ABObj	obj= NULL;
	list->part_of= list;		/* composite root */
        obj= obj_create(AB_TYPE_CONTAINER, list);
        obj->part_of= list;
    }
    else
    {
       /* atomic */
    }

    obj_set_flag(list, XmConfiguredFlag);

    return 0;
}

static int
composite_create_slider(ABObj slider)
{
    ABObj	obj= NULL;

    slider->part_of= slider;

    obj= obj_create(AB_TYPE_LIST, slider);
    obj->part_of= slider;
    obj_set_flag(obj, XmConfiguredFlag);

    obj= obj_create(AB_TYPE_ITEM, slider);
    obj->part_of= slider;
    obj_set_flag(obj, XmConfiguredFlag);

    return 0;
}



/*
 * "module 2" watches object creations and deletions, and keeps a list
 * of all extant objects
 */
static int
mod2_obj_create_callback(ObjEvCreateInfo info)
{
    /* printf("mod2_obj_create_callback\n"); */

    allObjectsArray[allObjectsArrayCount]= info->obj;
    ++allObjectsArrayCount;

    /* printf("mod2_obj_create_callback (EXIT)\n"); */
    return OK;
}

/*************************************************************************
**									**
**		RENAME CALLBACKS					**
**									**
*************************************************************************/

static int
rename_callback1(ObjEvAttChangeInfo info)
{
    ABObj	obj= info->obj;

    if (info->atts != OBJEV_ATT_NAME)
    {
	err_exit("bad att flag");
    }

    ++renameEventCount;
    return 0;
}


/*************************************************************************
**									**
**		REPARENT CALLBACKS					**
**									**
*************************************************************************/

/*
 * Won't allow modules to be reparented
 */
static int
allow_reparent_callback1(ObjEvAllowReparentInfo info)
{
    int iReturn= OK;
    ABObj	obj= info->obj;

    if (obj_is_module(info->obj))
    {
	iReturn= ERR_NOT_ALLOWED;
    }

    /* printf("allow_reparent_callback1(%s: %s -> %s) -> %d\n", 
	safe_obj_name(obj),
	safe_obj_name(obj->parent),
	safe_obj_name(info->parent),
	iReturn); */
    return iReturn;
}

static int
reparent_count_callback(ObjEvReparentInfo info)
{
    info->obj= info->obj;
    ++reparentEventCount;
    return OK;
}

static int
reparent_callback1(ObjEvReparentInfo info)
{
    int		iReturn= 0;
    ABObj	obj= info->obj;

    /* printf("reparent_callback1(%s: %s -> %s) -> %d\n", 
	safe_obj_name(obj),
	safe_obj_name(info->parent),
	safe_obj_name(obj->parent),
	iReturn); */
    return iReturn;
}


/*************************************************************************
**									**
**		SELECT CALLBACKS					**
**									**
*************************************************************************/

static int
count_select_callback(ObjEvAttChangeInfo info)
{
    /* printf("count_select_callback(%s,%d)\n",
		safe_obj_name(info->obj), info->obj->selected); */

    if (info->atts != OBJEV_ATT_SELECTED)
    {
	err_exit("bad att flag");
    }

    ++renameEventCount;

    if (obj_is_selected(info->obj))
    {
        ++selectEventCount;
    }
    else
    {
	++unselectEventCount;
    }
    return 0;
}


/*
 * Selects all the subobjects of an object
 */
static int
composite_select_callback(ObjEvAttChangeInfo info)
{
    AB_TRAVERSAL	trav;
    ABObj		curObj= NULL;
    ABObj		obj= info->obj;
    BOOL		selected= obj_is_selected(obj);

    /* printf("composite_select_callback(%s,%d)\n", 
		safe_obj_name(info->obj), info->obj->selected); */

    if (info->atts != OBJEV_ATT_SELECTED)
    {
	err_exit("bad att flag");
    }

    for (trav_open(&trav, obj, AB_TRAV_ALL);
	(curObj= trav_next(&trav)) != NULL; )
    {
	if (curObj->part_of == obj)
	{
	    if (selected)
	    {
	        obj_select(curObj);
	    }
	    else
	    {
		obj_unselect(curObj);
	    }
	}
    }
    trav_close(&trav);

    return 0;
}

/*
 * Clears the previous selection.
 * SHOULD BE THE FIRST CALLBACK!
 */
static int
exclusive_select_callback(ObjEvAttChangeInfo info)
{
    static ABObj	currentSelection= NULL;
    AB_TRAVERSAL	trav;
    ABObj		curObj= NULL;
    ABObj		obj= info->obj;
    ABObj		project= obj_get_project(obj);

    /* printf("exclusive_select_callback(%s, %d)\n", 
		safe_obj_name(info->obj), info->obj->selected); */

    if (info->atts != OBJEV_ATT_SELECTED)
    {
	err_exit("bad att flag");
    }

    if (!obj_is_selected(obj))
    {
	return OK;
    }

    if ((currentSelection == obj) || (currentSelection == obj->part_of))
    {
	return OK;
    }

    if (obj_is_root(obj))
    {
	currentSelection= obj;
    }
    else
    {
	currentSelection= obj->part_of;
    }

    /*
     * Kill the old selection
     */
    for (trav_open(&trav, project, AB_TRAV_ALL | AB_TRAV_MOD_PARENTS_FIRST);
	(curObj= trav_next(&trav)) != NULL; )
    {
	if ((curObj != currentSelection) && 
	     (curObj->part_of != currentSelection))
	{
	    obj_unselect(curObj);
	}
    }
    trav_next(&trav);

    return 0;
}


/*************************************************************************
**									**
**		GEOMETRY CALLBACKS					**
**									**
*************************************************************************/

/*
 * tries to reparent an object from within an allow callback.  Not allowed!
 */ 
static int
bogus_geometry_allow_callback(ObjEvAllowGeometryChangeInfo info)
{
    if (info->new_x == 1000)
    {
	ABObj	project= obj_get_project(info->obj);
	ABObj	module = obj_get_module(info->obj);
	ABObj	item3= obj_find_by_name(module, "item103");
	ABObj	module1= obj_find_module_by_name(project, "module1");
	check_find(item3);
	check_find(module1);

	/*
	 * Test should be OK, actual move should cause an error
	 */
	if (obj_test_reparent(item3, module1) < 0)
	{
	    err_exit("Legal reparent query DISallowed.");
	}
	if (obj_reparent(item3, module1) >= 0)
	{
	    err_exit("Illegal reparent allowed");
	}
	printf("bogus move disallowed in allow callback (correct).\n");
    }

    return OK;
}

/*
 * doesn't allow gemetry change for composite subobjects
 */
static int
composite_geometry_allow_callback(ObjEvAllowGeometryChangeInfo info)
{
    if (obj_is_sub(info->obj))
    {
	return ERR_NOT_ALLOWED;
    }
    return OK;
}


/*
 * only allows resizes where width = height
 */
static int
symmetric_geometry_allow_callback(ObjEvAllowGeometryChangeInfo info)
{
    if (info->new_width != info->new_height)
    {
	return ERR_NOT_ALLOWED;
    }
    return OK;
}


/*
 * Moves x,y to be multiples of 10
 */
static int
grid_geometry_callback(ObjEvAttChangeInfo info)
{
    ABObj	obj= info->obj;
    int		gridX= 0;
    int		gridY= 0;

    if (info->atts != OBJEV_ATT_GEOMETRY)
    {
	err_exit("bad att flag");
    }

    gridX= obj->x + (obj->x % 10);
    gridY= obj->x + (obj->y % 10);

    return obj_move(obj, gridX, gridY);
}

/*************************************************************************
**									**
**		DESTROY CALLBACKS					**
**									**
*************************************************************************/

/*
 * If the root of a composite object is destroyed, all of its composite
 * subobjects are destroyed.
 */
static int
composite_destroy_callback(ObjEvDestroyInfo info)
{
    ABObj	obj= info->obj;
    AB_TRAVERSAL	trav;
    ABObj		subobj= NULL;

    if (!obj_is_root(obj))
    {
	return OK;
    }

    for (trav_open(&trav, obj, AB_TRAV_ALL | AB_TRAV_MOD_SAFE);
	(subobj= trav_next(&trav)) != NULL; )
    {
	if (subobj->part_of == obj)
	{
	    obj_destroy_one(subobj);
	}
    }
    trav_close(&trav);

    return 0;
}


/*
 * Just counts events...
 */
static int
count_destroy_callback(ObjEvDestroyInfo info)
{
    ++destroyEventCount;
    return OK;
}


/*************************************************************************
**									**
**		UPDATE CALLBACKS					**
**									**
*************************************************************************/

static int
count_update_callback(ObjEvUpdateInfo info)
{
    if (info->update_subtree)
    {
	++updateTreeEventCount;
    }
    else
    {
        ++updateEventCount;
    }
    return OK;
}


static int
composite_update_callback(ObjEvUpdateInfo info)
{
    AB_TRAVERSAL	trav;
    ABObj		obj= info->obj;
    ABObj		subobj= NULL;

    if (!obj_is_root(obj))
    {
	return OK;
    }

    for (trav_open(&trav, obj, AB_TRAV_ALL);
	(subobj= trav_next(&trav)) != NULL; )
    {
       if ((subobj != obj) && (subobj->part_of == obj))
       {
	   obj_update_clients(subobj);
       }
    }
    trav_close(&trav);
    return OK;
}


static int
count_update_with_data_callback(ObjEvUpdateWithDataInfo info)
{
    ++updateEventCount;
    return OK;
}


static int
composite_update_with_data_callback(ObjEvUpdateWithDataInfo info)
{
    ABObj		obj= info->obj;
    AB_TRAVERSAL	trav;
    ABObj		subobj= NULL;

    if (!obj_is_root(obj))
    {
	return OK;
    }

    for (trav_open(&trav, obj, AB_TRAV_ALL);
	(subobj= trav_next(&trav)) != NULL; )
    {
	if ((subobj != obj) && (subobj->part_of == obj))
	{
	    obj_update_clients_with_data(subobj, 
			info->update_code,
			info->update_data, 
			info->update_data_free_func);
	}
    }
    trav_close(&trav);
    return 0;
}


static int
free_update_data(int update_code, void *void_update_data)
{
    UPDATE_DATA	*update_data= (UpdateData)void_update_data;

    printf("freeing %s\n", update_data->text);
    ++(update_data->free_count);
    return 0;
}


