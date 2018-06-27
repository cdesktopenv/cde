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
 * $XConsortium: pal.c /main/4 1996/04/21 19:27:07 drk $
 *
 *      RESTRICTED CONFIDENTIAL INFORMATION:
 *
 *      The information in this document is subject to special
 *      restrictions in a confidential disclosure agreement between
 *      HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *      document outside HP, IBM, Sun, USL, SCO, or Univel without
 *      Sun's specific written approval.  This document and all copies
 *      and derivative works thereof must be returned or destroyed at
 *      Sun's request.
 *
 *      Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */


/*
 * pal.c - Implements Palette Item functionality
 */
#include <stdio.h>
#include <string.h>
#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <ab_private/trav.h>
#include <ab_private/pal.h>

#define    MAX_PALETTE_ITEMS    30

/*************************************************************************
**                                                                      **
**       Global  Data                                                   **
**                                                                      **
**************************************************************************/
PalItemInfo*  	palette_item[MAX_PALETTE_ITEMS];
int   	      	palette_item_cnt = 0;

/* This should be getting its prototype from /usr/include/string.h
 * but it is not for some reason.
 */
#ifdef __cplusplus
extern "C" {
    extern int strcasecmp(const char *, const char *);
}
#endif /* __cplusplus */


/*************************************************************************
**                                                                      **
**       Private Data                                                   **
**                                                                      **
**************************************************************************/
static PalEditableObjInfo	*pal_ed_obj_head = NULL;

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

#ifdef NEED_STRCASECMP
/*
 * In case strcasecmp is not provided by the system here is one
 * which does the trick.
 */
static int
strcasecmp(const char *s1,
	   const char *s2)
{
    int c1, c2;

    while (*s1 && *s2) {
	c1 = isupper(*s1) ? tolower(*s1) : *s1;
	c2 = isupper(*s2) ? tolower(*s2) : *s2;
	if (c1 != c2)
	    return (c1 - c2);
	s1++;
	s2++;
    }
    return (int) (*s1 - *s2);
}
#endif

/*
 * Add a palette item into the palette item table
 * and register the widget for palette drag events
 */
void
pal_register_item_info(
    Widget        pwidget,
    PalItemInfo*  palitem,
    int		  subtype,
    String	  subname,
    Pixmap	  subpixmap
)
{
    int        i;
    Boolean    exists = FALSE;

    for (i = 0; i < palette_item_cnt; i++)
        if (palette_item[i] == palitem)
        {
            exists = TRUE;
            break;
        }

    if (!exists)
    {
        if (palette_item_cnt < MAX_PALETTE_ITEMS)
	{
	    palitem->subinfo = (PalSubtypeInfo*)XtMalloc(sizeof(PalSubtypeInfo));
	    palitem->num_subinfo             = 1;
	    palitem->subinfo[0].subtype      = subtype;
	    palitem->subinfo[0].pixmap       = subpixmap;
            palette_item[palette_item_cnt++] = palitem;

            if (subtype != AB_NO_SUBTYPE)
                palitem->subinfo[0].subname = XtNewString(subname);
            else
                palitem->subinfo[0].subname = palitem->name;
	}	
        else
        {
            if (util_get_verbosity() > 0)
                fprintf(stderr,"pal_register_item_info: palette item table full\n");
            return;
        }

	/* Register High Level Object Type */
	pal_register_editable_obj(palitem->type, palitem->subinfo[0].subtype,
		palitem->name, palitem); 

    }
    else /* Make room for additional Subtype information */
    {
  	int 	     i;
	BOOL 	     found = FALSE;
	PalSubtypeInfo *sub = palitem->subinfo;

 	for (i=0; i < palitem->num_subinfo; i++)
	    if (sub[i].subtype == subtype)
	    {
		found = TRUE;
		break;
	    }

	if (!found)
	{
	    PalSubtypeInfo *newsub;
	
	    (palitem->num_subinfo)++;
	    newsub = (PalSubtypeInfo*)XtMalloc(sizeof(PalSubtypeInfo)*palitem->num_subinfo);

	    for (i=0; i < palitem->num_subinfo- 1; i++)
		newsub[i] = sub[i];

	    newsub[i].subtype = subtype;
	    newsub[i].pixmap  = 0;
	    newsub[i].subname = XtNewString(subname);
	    XtFree((char *)(palitem->subinfo));
	    palitem->subinfo = newsub;
	}
    }

    if (pwidget != NULL)
    {
    	XtVaSetValues(pwidget, XmNuserData, palitem, NULL);
    	pal_enable_item_drag(pwidget, subtype);
    }

}

/*
 * return the palette item info corresponding to the object
 */
PalItemInfo*
pal_get_item_info(
    ABObj    obj
)
{
    int    	    i;
    PalItemInfo*    palitem = NULL;
    ABObj     	    vobj;

    if (obj_is_item(obj))
	vobj = obj_get_root(obj_get_parent(obj));
    else
        vobj = obj_get_root(obj);

    for (i=0; i < palette_item_cnt; i++)
        if ((*palette_item[i]->is_a_test)(vobj))
	{
            palitem = palette_item[i];
	    break;
	}

    return(palitem);

}

/*
 * return the palette item info corresponding to the type/subtype
 */
PalItemInfo*
pal_get_type_item_info(
    AB_OBJECT_TYPE	type,
    int			subtype
)
{
    int    	    i;
    PalItemInfo*    palitem = NULL;
    ABObj     	    vobj;

    for (i=0; i < palette_item_cnt; i++)
        if ((palette_item[i]->type == type) && 
		(palette_item[i]->subinfo->subtype == subtype))
	{
            palitem = palette_item[i];
	    break;
	}

    return(palitem);
}

/*
 * call the palette item initialize method for the object
 */
int
pal_initialize_obj(
    ABObj    obj
)
{
    PalItemInfo*    palitem;

    palitem = pal_get_item_info(obj);

    if (palitem != NULL && palitem->initialize != NULL)
        return((*palitem->initialize)(obj_get_root(obj)));
    else
        return ERROR;

}

Pixmap
pal_get_item_pixmap(
    ABObj 	obj,
    int		subtype,
    int		*p_width,
    int		*p_height
)
{
    PalItemInfo*   palitem;
    PalSubtypeInfo *sub = NULL;
    int            i;

    palitem = pal_get_item_info(obj);

    if (palitem != NULL)
    {
	for (i=0; i < palitem->num_subinfo; i++)
	    if (palitem->subinfo[i].subtype == subtype)
	    {
		sub = &(palitem->subinfo[i]);
		break;
	    }

	if (sub != NULL)
	{
	    *p_width  = (int)sub->pmwidth;
            *p_height = (int)sub->pmheight;
	    return(sub->pixmap);
	}
    }

    *p_width  = *p_height = 0;
    return 0;

}
STRING
pal_get_item_subname(
    ABObj       obj,
    int         subtype
)
{
    PalItemInfo*   palitem;
    PalSubtypeInfo *sub = NULL;
    int            i;

    palitem = pal_get_item_info(obj);

    if (palitem != NULL)
    {
        for (i=0; i < palitem->num_subinfo; i++)
            if (palitem->subinfo[i].subtype == subtype)
		return(palitem->subinfo[i].subname);
    }    
    return NULL;
}

void
pal_register_editable_obj(
    AB_OBJECT_TYPE	type,
    int			subtype,
    STRING		name,
    PalItemInfo		*palitem
)
{
    PalEditableObjInfo *ed_obj_info;
    PalEditableObjInfo *prev, *cur;

    ed_obj_info = (PalEditableObjInfo*)util_malloc(sizeof(PalEditableObjInfo));
    ed_obj_info->type = type;
    ed_obj_info->subtype = subtype;
    ed_obj_info->name = istr_create(name);
    ed_obj_info->palitem = palitem; 
    ed_obj_info->next = NULL;

    if (pal_ed_obj_head == NULL) /* first one */
	pal_ed_obj_head = ed_obj_info;
    /* 
     * Need to insert into list alphabetically...
     */
    else if (strcasecmp(name, istr_string(pal_ed_obj_head->name)) < 0)
    {
	/* Put at head of list */
	ed_obj_info->next = pal_ed_obj_head;
	pal_ed_obj_head = ed_obj_info;
    }
    else
    { 
	for(cur = pal_ed_obj_head->next, prev = pal_ed_obj_head; 
	    cur != NULL; cur = cur->next)
	{
	    if (strcasecmp(name, istr_string(cur->name)) < 0)
	    {
		ed_obj_info->next = cur;
		prev->next = ed_obj_info;
		break;
	    }
	    prev = cur;
	}
	if (cur == NULL) /* hit end of list */
	    prev->next = ed_obj_info;
    }
}

PalEditableObjInfo*
pal_get_editable_obj_info(
    ABObj	obj
)
{
    PalEditableObjInfo *peo;
    int			subtype;

    if (obj == NULL)
    {
	util_dprintf(1, "pal_get_editable_obj_info: NULL object\n");
	return NULL;
    }

    if (obj->type == AB_TYPE_SCALE)
    {
	/*
	 * Special case for Scales/Gauges.
	 * Their subtype info is actually their read-only
	 * state i.e. True/False
	 */
        if (obj_get_read_only(obj) == False)
            subtype = (int)False;
        else /* Gauge */
            subtype = (int)True;
    }
    else
	subtype = obj_get_subtype(obj);

    for (peo = pal_ed_obj_head; peo != NULL; peo = peo->next)
    {
	/* If a palitem structure is registered, use the "is_a" test func
	 * in that structure to determine if the obj is of that type of
	 *  "editable" object, else directly compare the type & subtype.
	 *
	 * Note: the palitem structure test has precedence in order to
	 *       accommodate the high-level types which map to multiple
	 *	 subtypes. i.e. If we have a "Radio Box", we want to map to
	 * 	 the "Choice" editable object 'super' type.
	 */
	if (peo->palitem) 
	{
	    if ((*(peo->palitem)->is_a_test)(obj))
		return(peo);
	}
        else if ((obj->type == peo->type) &&
            (peo->subtype == AB_NO_SUBTYPE ||
             (subtype == peo->subtype)))
	    return(peo);
    }
    return NULL;
}

void
pal_add_editable_obj_menu_items(
    Widget		menu_pane,
    XtCallbackProc	menuitem_cb,
    PalEditableObjTestFunc test_func
)
{
    PalEditableObjInfo	*eop;
    XmString 		xmlabel;
    Widget   		button;

    for(eop = pal_ed_obj_head; eop != NULL; eop = eop->next)
    {
	if ((*test_func)(eop) == True)
	{
    	    xmlabel = XmStringCreateLocalized(istr_string(eop->name));
    	    button = XtVaCreateManagedWidget("object_menu_item",
            	xmPushButtonWidgetClass,
            	menu_pane,
            	XmNlabelString,     xmlabel,
            	XmNuserData,        (XtPointer)eop->palitem,
            	NULL);
    	    XmStringFree(xmlabel);

    	    XtAddCallback(button, XmNactivateCallback, menuitem_cb,
            	(XtPointer)eop);
	}
    }
}

