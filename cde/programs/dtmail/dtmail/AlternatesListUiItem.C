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
/* $XConsortium: AlternatesListUiItem.C /main/3 1995/11/06 16:03:45 rswiston $ */
/*
 *+SNOTICE
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */
#include "RoamApp.h"
#include <Xm/List.h>
#include <DtMail/options_util.h>
#include "options_ui.h"
#include <DtMail/PropUi.hh>
#include <DtMail/ListUiItem.hh>
#include <DtMail/AlternatesListUiItem.hh>

extern Boolean props_changed;
void handleAlternateSelection(Widget w, XtPointer clientdata, XtPointer calldata);

// AlternatesListUiItem::AlternatesListUiItem
// AlternatesListUiItem ctor
////////////////////////////////////////////////////////////////////

AlternatesListUiItem::AlternatesListUiItem(Widget w, 
		       int source, 
		       char *search_key,
		       Widget text_entry_widget):ListUiItem(w, source, search_key, NULL)
{
  source = source; search_key = search_key;

  entry_field_widget = text_entry_widget;

  list_items = NULL;
  deleted_items = NULL;

  XtVaSetValues(w,
	XmNuserData, this,
        XmNautomaticSelection, True,
        XmNselectionPolicy, XmBROWSE_SELECT,
	NULL);

    XtAddCallback(w,
		XmNbrowseSelectionCallback, 
		(XtCallbackProc)handleAlternateSelection,
		  (XtPointer)this);
}

//---------------------------------------------------------------
void handleAlternateSelection(Widget w, XtPointer clientdata, XtPointer calldata)
{
  AlternatesListUiItem *item;
  XmListCallbackStruct *list_info = (XmListCallbackStruct *)calldata;
  char *selection_string = NULL;
  DtVirtArray<PropStringPair *> *list_items;
  clientdata = clientdata;

  XtVaGetValues(w, 
	XmNuserData, &item,
	NULL);  

  list_items = item->getItemList();

  if(list_items != NULL)
    {
      // motif index is 1 based
      //virtarry is 0 based
      PropStringPair *pair = (*list_items)[list_info->item_position - 1];

      if(pair != NULL)
        {
          XtVaSetValues(item->getEntryFieldWidget(),
                        XmNvalue,pair->label,
                        NULL);

	}	

    }
}


// AlternatesListUiItem::writeFromUiToSource()
// Takes values in the UI and puts them into the source DB
///////////////////////////////////////////////////////////////////
void AlternatesListUiItem::writeFromUiToSource()
{
  Widget w = this->getWidget();
  DtMailEnv error;
  DtMail::Session * d_session = theRoamApp.session()->session();
  DtMail::MailRc * mail_rc = d_session->mailRc(error);
  int i, num_items;

  if(deleted_items != NULL)
    {
      num_items = deleted_items->length();

      for(i = 0; i < num_items; i++)
	{
	  mail_rc->removeAlternate(error,(*deleted_items)[i]);
	}
    }

  if(list_items != NULL)
    {
      num_items = list_items->length();
      for(i = 0; i < num_items; i++)
	{
	  mail_rc->setAlternate(error,(*list_items)[i]->label);
	}
    }
  
  if(deleted_items != NULL)
    {
      delete deleted_items;
      deleted_items = NULL;
    }
}

// AlternatesListUiItem::writeFromSourceToUi()
// Takes values in the UI and puts them into the source DB
///////////////////////////////////////////////////////////////////
void AlternatesListUiItem::writeFromSourceToUi()
{
  DtMailEnv error;
  DtMail::Session * d_session = theRoamApp.session()->session();
  DtMail::MailRc * mail_rc = d_session->mailRc(error);
  Widget w = this->getWidget();
  const char *list_str = NULL;
  DtVirtArray<char *> *char_list = NULL;
  PropStringPair *new_pair;
  char *token, *buf = NULL;

  XmListDeleteAllItems(w);

  if(deleted_items != NULL)
    {
      delete deleted_items;
      deleted_items = NULL;
    }

  if(list_items != NULL)
  {
    delete list_items;
    list_items = NULL;
  }

  if ((list_str = mail_rc->getAlternates(error)) == NULL) {
	list_str = strdup("");
  }
  
  if ((buf = (char *) malloc(strlen(list_str) + 1)) == NULL) {
    free((void *) list_str);
    return;
  }
  strcpy(buf, (char *)list_str);
  
  if((token = (char *) strtok(buf, " "))) 
    {
      list_items = new DtVirtArray<PropStringPair *>(10);
      char_list = new DtVirtArray<char *>(10);

      new_pair = new PropStringPair;
      new_pair->label = strdup(token);
      new_pair->value = NULL;
      list_items->append(new_pair);

      char_list->append(strdup(token));
 
      while(token = (char *)strtok(NULL, " "))
	{
	  new_pair = new PropStringPair;
	  new_pair->label = strdup(token);
	  new_pair->value = NULL;
	  list_items->append(new_pair);

	  char_list->append(strdup(token)); 
	}
    }

  options_list_init(w, char_list);

  delete char_list;

  free((void *)list_str);

  if(buf != NULL)
    free((void *)buf);

}

///////////////////////////////////////////////////////////
void AlternatesListUiItem::handleAddButtonPress()
{
  Widget entry_field = this->getEntryFieldWidget();
  char *test_str = NULL;
  PropStringPair *new_pair = NULL;

  XtVaGetValues(entry_field,
		XmNvalue, &test_str,
		NULL);

  if(test_str != NULL)
    if(strlen(test_str) > 1)
      {
	new_pair = new PropStringPair;
	int *pos_list, num_pos;

	new_pair->label = strdup(test_str);
	new_pair->value = NULL;
      
	if(XmListGetSelectedPos(this->getWidget(),
				&pos_list,
				&num_pos))
	  {
            if(list_items == NULL)
              list_items = new DtVirtArray<PropStringPair *>(10);

	    list_items->insert(new_pair,pos_list[0] - 1); 
	    XmListAddItem(this->getWidget(),
			  XmStringCreateLocalized(new_pair->label),
			  pos_list[0]);
	    
	    XmListSelectPos(this->getWidget(),
			    pos_list[0],
			    TRUE);

	    XmListSetPos(this->getWidget(),
			 pos_list[0]);
	  }	
	else
	  {
            if(list_items == NULL)
              list_items = new DtVirtArray<PropStringPair *>(10);

	    list_items->insert(new_pair,0); 

	    XmListAddItem(this->getWidget(),
			  XmStringCreateLocalized(new_pair->label),
			  1);
	    XmListSelectPos(this->getWidget(),
			    1,
			    TRUE);
	    XmListSetPos(this->getWidget(),
			 1);
	  }
	props_changed = TRUE;
      }
}
///////////////////////////////////////////////////////////
void AlternatesListUiItem::handleChangeButtonPress()
{
  Widget entry_field = this->getEntryFieldWidget();
  char *test_str = NULL;
  PropStringPair *new_pair = NULL;
  XmString replace_string;
  int *pos_list, num_pos;

  // if nothing selected nothing to change...
  if(XmListGetSelectedPos(this->getWidget(),
			  &pos_list,
			  &num_pos))
    {
      XtVaGetValues(entry_field,
		    XmNvalue, &test_str,
		    NULL);

      if(test_str != NULL)
	if(strlen(test_str) > 1)
	  {

	    new_pair = (*list_items)[pos_list[0] - 1];

	    if(deleted_items == NULL)
	      {
		deleted_items = new DtVirtArray<char *>(10);
	      }	

	    deleted_items->append(strdup((*list_items)[pos_list[0] -1]->label));

	    free(new_pair->label);
	    new_pair->label = strdup(test_str);

	    replace_string = XmStringCreateLocalized(new_pair->label);

	    XmListReplaceItemsPos(this->getWidget(),
				  &replace_string,
				  1,
				  pos_list[0]);

	    XmListSelectPos(this->getWidget(),
			    pos_list[0],
			    TRUE);
	  }
	props_changed = TRUE;
    }
}
///////////////////////////////////////////////////////////
void AlternatesListUiItem::handleDeleteButtonPress()
{
  Widget list_widget = this->getWidget();
  int *p_list, p_count;

  // get the selected position
  if(XmListGetSelectedPos(list_widget,
			  &p_list,
			  &p_count))
    {
      
      if(deleted_items == NULL)
	{
	  deleted_items = new DtVirtArray<char *>(10);
	}	

      deleted_items->append(strdup((*list_items)[p_list[0] -1]->label));

      // delete the item from our list 
      this->list_items->remove(p_list[0] - 1); // remove only first

      // delete the item from the widget
      XmListDeletePos(list_widget, p_list[0]);

      XtVaSetValues(this->getEntryFieldWidget(),
                        XmNvalue,"",
                        NULL);
      XmListSelectPos(list_widget,
		      p_list[0],
		      TRUE);
      
      props_changed = TRUE;
    }



}




