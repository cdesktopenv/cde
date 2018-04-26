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
/* $TOG: CustomListUiItem.C /main/6 1997/09/03 17:34:40 mgreess $ */
/*
 *+SNOTICE
 *
 *      $TOG: CustomListUiItem.C /main/6 1997/09/03 17:34:40 mgreess $
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
#include <DtMail/CustomListUiItem.hh>

extern void handleCustSelection(Widget, XtPointer, XtPointer );
extern Boolean props_changed;

// CustomListUiItem::CustomListUiItem
// CustomListUiItem ctor
////////////////////////////////////////////////////////////////////

CustomListUiItem::CustomListUiItem(Widget w, 
		       int source, 
		       char *search_key,
		       Widget key_entry_widget,
		       Widget value_entry_widget):ListUiItem(w, source, search_key, NULL)
{
  source = source; search_key = search_key;

  key_widget = key_entry_widget;
  value_widget = value_entry_widget;

  list_items = NULL;
  deleted_items = NULL;

  XtVaSetValues(w,
	XmNuserData, this,
        XmNautomaticSelection, True,
        XmNselectionPolicy, XmBROWSE_SELECT,
	NULL);

    XtAddCallback(w,
		  XmNbrowseSelectionCallback, 
		  (XtCallbackProc)handleCustSelection,
		  (XtPointer)this);



}

#ifdef DEAD_WOOD
//-----------------======================-----------------
void handleDoubleSelection(Widget w, XtPointer clientdata, XtPointer calldata)
{
  CustomListUiItem *item;
  XmListCallbackStruct *list_info = (XmListCallbackStruct *)calldata;
  char *selection_string = NULL;
  DtVirtArray<PropStringPair *> *list_items;


  XtVaGetValues(w, 
	XmNuserData, &item,
	NULL);  

  list_items = item->getItemList();

  if(list_items != NULL)
    {                                         // motif index is 1 based
                                              //virtarry is 0 based
      PropStringPair *pair = (*list_items)[list_info->item_position - 1];

      if(pair != NULL)
	{
	  XtVaSetValues(item->getKeyWidget(),
			XmNvalue,pair->label,
			NULL);
	  
	  XtVaSetValues(item->getValueWidget(),
			XmNvalue,pair->value,
			NULL);
	}
    }	
}
#endif /* DEAD_WOOD */

//-----------------======================-----------------
void handleCustSelection(Widget w, XtPointer, XtPointer calldata)
{
  CustomListUiItem *item;
  XmListCallbackStruct *list_info = (XmListCallbackStruct *)calldata;
  char *selection_string = NULL;
  DtVirtArray<PropStringPair *> *list_items;


  XtVaGetValues(w, 
	XmNuserData, &item,
	NULL);  

  list_items = item->getItemList();

  if(list_items != NULL)
    {                                         // motif index is 1 based
                                              //virtarry is 0 based
      PropStringPair *pair = (*list_items)[list_info->item_position - 1];

      if(pair != NULL)
	{
	  XtVaSetValues(item->getKeyWidget(),
			XmNvalue,pair->label,
			NULL);
	  
	  XtVaSetValues(item->getValueWidget(),
			XmNvalue,pair->value,
			NULL);
	}
    }	
}
// CustomListUiItem::writeFromUiToSource()
// Takes values in the UI and puts them into the source DB
///////////////////////////////////////////////////////////////////
void CustomListUiItem::writeFromUiToSource()
{
  Widget w = this->getWidget();
  DtMailEnv error;
  DtMail::Session * d_session = theRoamApp.session()->session();
  DtMail::MailRc * mail_rc = d_session->mailRc(error);
  int i, num_items;
  char *cf_str = NULL;
 
  if(list_items != NULL) {
      if(list_items->length() > 0)
        {
          num_items = list_items->length();
          // calc mailrc strlen...
          int total_len = 1; // add space for the terminator...
          for(i = 0; i < num_items; i++) {
  	      int num_blanks=0;
              for (char *ptr = (*list_items)[i]->value; 
			ptr && *ptr; ptr++)
                        if (*ptr == ' ') num_blanks++;
              // strlen(label) strlen(value) + space + colon +
              // potential \'s that precede the blank character
              if((*list_items)[i]->label != NULL)
                total_len = total_len + strlen(":") + 
			(strlen("\\") * num_blanks) + 
			strlen((*list_items)[i]->label) + 2;
 
              if((*list_items)[i]->value != NULL)
                total_len = total_len + strlen((*list_items)[i]->value);
          }
          cf_str = (char *)malloc(total_len);
          cf_str[0] = '\0';

          for(i = 0; i < num_items; i++) {
              if((*list_items)[i]->label != NULL) 
		   strcat(cf_str, (*list_items)[i]->label);
	      strcat(cf_str, ":");
              if((*list_items)[i]->value != NULL) {
                if (strchr((char*)(*list_items)[i]->value, ' ')) {
                        char *token, *tmpbuf;
                        tmpbuf = strdup((*list_items)[i]->value);
                        if (token = (char *)strtok(tmpbuf, " ")) {
                                strcat(cf_str, token);
                                while(token = (char *)strtok(NULL, " ")) {
                                        strcat(cf_str, "\\ ");
                                        strcat(cf_str, token);
                                }
                        }
                        free(tmpbuf);
                }
                else
                        strcat(cf_str, (*list_items)[i]->value);
	      }
	      strcat(cf_str, " ");
          }
          mail_rc->setValue(error, "additionalfields", cf_str);
        }
      else
          mail_rc->removeValue(error, "additionalfields");
    }
  else
       mail_rc->removeValue(error, "additionalfields");

}

// CustomListUiItem::writeFromSourceToUi()
// Takes values in the UI and puts them into the source DB
///////////////////////////////////////////////////////////////////
void CustomListUiItem::writeFromSourceToUi()
{
  DtMailEnv error;
  DtMail::Session * d_session = theRoamApp.session()->session();
  DtMail::MailRc * mail_rc = d_session->mailRc(error);
  Widget w = this->getWidget();
  const char *list_str = NULL;
  DtVirtArray<char *> list_str_list(10);
  char *buf = NULL;
  int list_len, i;
  const char *value = NULL;

  XmListDeleteAllItems(w);

  mail_rc->getValue(error, "additionalfields", &value);

  if(list_items != NULL)
    delete list_items;

  list_items = new DtVirtArray<PropStringPair *>(10);

  parsePropString(value, *list_items);

  list_len = list_items->length();
  for(i = 0; i < list_len; i++)
    list_str_list.append(formatPropPair((*list_items)[i]->label, 
					(*list_items)[i]->value));

  options_list_init(w, &list_str_list);

  if (value != NULL)
    free((void *)value);
}

///////////////////////////////////////////////////////////
void CustomListUiItem::handleAddButtonPress()
{
  char *key_str = NULL;
  char *value_str = NULL;
  PropStringPair *new_pair = NULL;

  XtVaGetValues(key_widget,
		XmNvalue, &key_str,
		NULL);

  XtVaGetValues(value_widget,
		XmNvalue, &value_str,
		NULL);

  if(key_str != NULL)
    if(strlen(key_str) > 0)
      {
	new_pair = new PropStringPair;
	int *pos_list, num_pos;

	new_pair->label = strdup(key_str);
	
	if(value_str != NULL)
	  new_pair->value = strdup(value_str);
	else
	  new_pair->value = NULL;
	
	if(XmListGetSelectedPos(this->getWidget(),
				&pos_list,
				&num_pos))
	  {
            if(list_items == NULL)
              list_items = new DtVirtArray<PropStringPair *>(10);

	    list_items->insert(new_pair,pos_list[0] - 1); 
	
	    XmListAddItem(this->getWidget(),
			  XmStringCreateLocalized(
				       formatPropPair(
						      new_pair->label,
						      new_pair->value)),
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
			  XmStringCreateLocalized(
				       formatPropPair(
						      new_pair->label,
						      new_pair->value)),
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
void CustomListUiItem::handleChangeButtonPress()
{
  char *key_str = NULL;
  char *value_str = NULL;  
  PropStringPair *new_pair = NULL;
  XmString replace_string;
  int *pos_list, num_pos;

  // if nothing selected nothing to change...
  if(XmListGetSelectedPos(this->getWidget(),
			  &pos_list,
			  &num_pos))
    {
      XtVaGetValues(key_widget,
		    XmNvalue, &key_str,
		    NULL);

      XtVaGetValues(value_widget,
		    XmNvalue, &value_str,
		    NULL);

      if(key_str != NULL)
	if(strlen(key_str) > 0)
	  {

	    new_pair = (*list_items)[pos_list[0] - 1];

	    free(new_pair->label);
	    new_pair->label = strdup(key_str);
  
	    if(new_pair->value != NULL)
	      {
		free(new_pair->value);
		if(value_str != NULL)
		  new_pair->value = strdup(value_str);
		else
		  new_pair->value = NULL;
	      }

	    replace_string = XmStringCreateLocalized(
				     formatPropPair(new_pair->label,
						    new_pair->value));

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
void CustomListUiItem::handleDeleteButtonPress()
{
  Widget list_widget = this->getWidget();
  int *p_list, p_count;

  // get the selected position
  if(XmListGetSelectedPos(list_widget,
			  &p_list,
			  &p_count))
    {
      
      // delete the item from our list 
      this->list_items->remove(p_list[0] - 1); // remove only first

      // delete the item from the widget
      XmListDeletePos(list_widget, p_list[0]);

      XtVaSetValues(this->getKeyWidget(),
                        XmNvalue,"",
                        NULL);

      XtVaSetValues(this->getValueWidget(),
                        XmNvalue,"",
                        NULL);

      XmListSelectPos(list_widget,
		      p_list[0],
		      TRUE);
      
      props_changed = TRUE;
    }

}




