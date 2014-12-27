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
/* $TOG: BaseObj.C /main/4 1998/08/03 16:30:23 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "BaseObj.h"

#include <string.h>
#include <stdlib.h>

const char *ACTION_NOT_FOUND = "ActionNotFound";

BaseObj::BaseObj(BaseObj *parent,
	         const char *name)
{
   _name = STRDUP(name);
   _displayName = NULL;
   _details = NULL;
   _parent = parent;
   _init_children = false;
   _init_attributes = false;
   _children = NULL;
   _numChildren = 0;
   _numActions = 0;
   _numAttributes = 0;
   _actions = NULL;
   _attributes = NULL;
   _lastActionName = NULL;
   _lastActionOutput = NULL;
   _lastActionStatus = 0;
   AddToParent();
   BaseObj *p = _parent;
   while (p)
    {
      p->NotifyCreate(this);
      p = p->_parent;
    }
}

BaseObj::~BaseObj()
{
   int i;

   DeleteChildren();
   DeleteFromParent();

   BaseObj *parent = _parent;
   while (parent)
    {
      parent->NotifyDelete(this);
      parent = parent->_parent;
    }

   for (i = 0; i < _numAttributes; i++)
    {
      delete _attributes[i]->ReferenceName;
      delete _attributes[i]->Value;
      delete _attributes[i]->DisplayName;
      delete _attributes[i]->DisplayValue;
      delete _attributes[i]->DefaultValue;
      delete _attributes[i]->DisplayDefaultValue;
      delete _attributes[i]->Help;
      delete _attributes[i]->ContextualHelp;
      delete _attributes[i]->Listing;
      delete _attributes[i]->Dependancies;
      delete _attributes[i];
    }
   delete []_attributes;
   for (i = 0; i < _numActions; i++)
    {
      delete _actions[i]->ReferenceName;
      delete _actions[i]->DisplayName;
      delete _actions[i]->Nmemonic;
      delete _actions[i]->AcceleratorText;
      delete _actions[i]->Accelerator;
      delete _actions[i]->Help;
      delete _actions[i]->ContextualHelp;
      delete _actions[i]->Dependancies;
      delete _actions[i];
    }
   delete []_actions;
   delete []_children;
   free(_lastActionName);
   delete [] _lastActionOutput;
   free(_details);
   free(_displayName);
   free(_name);
}

char *BaseObj::DisplayName()
{
   if (!_displayName)
    {
      InitDisplayName();
      if (!_displayName)
	 _displayName = STRDUP(_name);
    }
   return _displayName; 
}

void BaseObj::ReadAttributes()
{
   if (_init_attributes == false)
    {
      LoadAttributes(_numAttributes, _attributes);
      _init_attributes = true;
    }
}

char *BaseObj::Details()
{
   if (!_details)
    {
      InitDetails();
      if (!_details)
	 _details = STRDUP("");
    }
   return _details; 
}

void BaseObj::UpdateDetails()
{
   free(_details);
   _details = NULL;
   (void) Details();
}

void BaseObj::UpdateChildren()
{
   DeleteChildren();
   InitChildren();
   _init_children = true;
}

BaseObj **BaseObj::Children()
{
   if (_init_children == false)
    {
      InitChildren();
      _init_children = true;
    }
   return _children; 
}

int BaseObj::NumChildren()
{
   if (_init_children == false)
    {
      InitChildren();
      _init_children = true;
    }
   return _numChildren; 
}

void BaseObj::DeleteAction(const char *name)
{
   Action *action;

   if (!HasAction(name, &action))
      return;

   Action **new_actions;
   int i, index;

   index = 0;
   new_actions = new Action*[_numActions - 1];
   for (i = 0; i < _numActions; i++)
      if (_actions[i] != action)
         new_actions[index++] = _actions[i];
   delete action->ReferenceName;
   delete action->DisplayName;
   delete action->Nmemonic;
   delete action->AcceleratorText;
   delete action->Accelerator;
   delete action->Help;
   delete action->ContextualHelp;
   delete action->Dependancies;
   delete action;

   delete []_actions;
   _actions = new_actions;

   _numActions--;
}

void BaseObj::DeleteAttribute(const char *name)
{
   Attribute *attribute;

   if (!HasAttribute(name, &attribute))
      return;

   Attribute **new_attributes;
   int i, index;

   index = 0;
   new_attributes = new Attribute*[_numAttributes - 1];
   for (i = 0; i < _numAttributes; i++)
      if (_attributes[i] != attribute)
         new_attributes[index++] = _attributes[i];
   delete attribute->ReferenceName;
   delete attribute->Value;
   delete attribute->DisplayName;
   delete attribute->DisplayValue;
   delete attribute->DefaultValue;
   delete attribute->DisplayDefaultValue;
   delete attribute->Help;
   delete attribute->ContextualHelp;
   delete attribute->Listing;
   delete attribute->Dependancies;
   delete attribute;

   delete []_attributes;
   _attributes = new_attributes;

   _numAttributes--;
}

void BaseObj::AddAttribute(const char *ReferenceName,
                           const char *DisplayName,
                           const char *Help,
                           const char *ContextualHelp,
                           Characteristics Mask,
                           ValueList ValueListType,
                           const char *Listing,
                           const char *Dependancies,
                           const char *DefaultValue,
                           const char *DisplayDefaultValue)
{
   Attribute **new_attributes;
   int i;

   new_attributes = new Attribute*[_numAttributes + 1];
   for (i = 0; i < _numAttributes; i++)
      new_attributes[i] = _attributes[i];

   delete []_attributes;
   _attributes = new_attributes;
   Attribute *attribute = new Attribute;
   attribute->ReferenceName = STRDUP(ReferenceName);
   attribute->Value = NULL;
   attribute->DisplayName = STRDUP(DisplayName);
   attribute->DisplayValue = NULL;
   attribute->DefaultValue = STRDUP(DefaultValue);
   attribute->DisplayDefaultValue = STRDUP(DisplayDefaultValue);
   attribute->Mask = Mask;
   attribute->Help = STRDUP(Help);
   attribute->ContextualHelp = STRDUP(ContextualHelp);
   attribute->Listing = STRDUP(Listing);
   attribute->ValueListType = ValueListType;
   attribute->Dependancies = STRDUP(Dependancies);
   _attributes[_numAttributes] = attribute;

   _numAttributes++;
}

void BaseObj::AddAction(ActionHandler Handler, 
			const char *ReferenceName,
			const char *DisplayName,
			const char *Nmemonic,
                        const char *Help, 
                        const char *ContextualHelp,
			boolean InputRequired,
                        const char *AcceleratorText,
                        const char *Accelerator,
                        const char *Dependancies)
{
   Action **new_actions;
   int i;

   new_actions = new Action*[_numActions + 1];
   for (i = 0; i < _numActions; i++)
      new_actions[i] = _actions[i];

   delete []_actions;
   _actions = new_actions;
   Action *action = new Action;
   action->Handler = Handler;
   action->ReferenceName = STRDUP(ReferenceName);
   action->DisplayName = STRDUP(DisplayName);
   action->Nmemonic = STRDUP(Nmemonic);
   action->AcceleratorText = STRDUP(AcceleratorText);
   action->Accelerator = STRDUP(Accelerator);
   action->InputRequired = InputRequired;
   action->Help = STRDUP(Help);
   action->ContextualHelp = STRDUP(ContextualHelp);
   action->Dependancies = STRDUP(Dependancies);
   _actions[_numActions] = action;

   _numActions++;
}

void BaseObj::AddToParent()
{
   if (!_parent)
      return;

   BaseObj **new_children;
   int i;

   new_children = new BaseObj*[_parent->_numChildren + 1];
   for (i = 0; i < _parent->_numChildren; i++)
      new_children[i] = _parent->_children[i];

   delete []_parent->_children;
   _parent->_children = new_children;
   _parent->_children[_parent->_numChildren] = this;

   _parent->_numChildren++;
}

void BaseObj::DeleteFromParent()
{
   if (!_parent)
      return;

   BaseObj **new_children;
   int i, index;

   index = 0;
   new_children = new BaseObj*[_parent->_numChildren - 1];
   for (i = 0; i < _parent->_numChildren; i++)
      if (_parent->_children[i] != this)
         new_children[index++] = _parent->_children[i];

   delete []_parent->_children;
   _parent->_children = new_children;

   _parent->_numChildren--;
}

int BaseObj::NumSiblings()
{
   if (_parent)
      return _parent->_numChildren;
   else
      return 0;
}

BaseObj ** BaseObj::Siblings()
{
   if (_parent)
      return _parent->_children;
   else
      return NULL;
}

boolean BaseObj::HasAction(const char *actionName,
	                   Action **action)
{
   int i;

   for (i = 0; i < _numActions; i++)
      if (!strcmp(_actions[i]->ReferenceName, actionName))
	{
	  *action = _actions[i];
	  return true;
	}
   return false;
}

boolean BaseObj::HasAction(Action *action)
{
   Action *dummy;

   if (action)
      return HasAction(action->ReferenceName, &dummy);
   else
      return false;
}

boolean BaseObj::HasAction(const char *actionName)
{
   Action *dummy;

   return HasAction(actionName, &dummy);
}

boolean BaseObj::SendAction(Action *action,
		            BaseObj *requestor)
{
   boolean status;

   delete [] _lastActionOutput;
   free(_lastActionName);
   _lastActionOutput = NULL;
   _lastActionName = strdup(action->ReferenceName);
   if ((status = HasAction(action)) == true)
      _lastActionStatus = (*action->Handler)(this, &_lastActionOutput,
					     requestor);
   else
    {
      _lastActionStatus = -1;
      int len = strlen("'%s' is not an action of %s") +
		strlen(ObjectClassName()) + strlen(action->ReferenceName) + 1;
      _lastActionOutput = new char [len];
      sprintf(_lastActionOutput, "'%s' is not an action of %s", 
	      ObjectClassName(), action->ReferenceName);
    }
   return status;
}

boolean BaseObj::SendAction(const char *actionName,
		            BaseObj *requestor)
{
   Action *action;

   (void) HasAction(actionName, &action);
   return SendAction(action, requestor);
}

char * BaseObj::AttributeValue(char *referenceName)
{
   Attribute *dummy;

   ReadAttributes();
   if (HasAttribute(referenceName, &dummy))
      return dummy->Value;
   else
      return NULL;
}

boolean BaseObj::HasAttribute(const char *attributeName,
	                      Attribute **attribute)
{
   int i;

   for (i = 0; i < _numAttributes; i++)
      if (!strcmp(_attributes[i]->ReferenceName, attributeName))
	{
	  *attribute = _attributes[i];
	  return true;
	}
   return false;
}

boolean BaseObj::HasAttribute(Attribute *attribute)
{
   Attribute *dummy;

   return HasAttribute(attribute->ReferenceName, &dummy);
}

boolean BaseObj::HasAttribute(const char *attributeName)
{
   Attribute *dummy;

   return HasAttribute(attributeName, &dummy);
}

void BaseObj::DeleteChildren()
{
   if (_numChildren)
    {
      BaseObj **kids = new BaseObj*[_numChildren];
      int i, n = _numChildren;
      // Save children first before deleting a child since the destructor
      // updates the _children variable
      for (i = 0; i < n; i++)
         kids[i] = _children[i];
      for (i = 0; i < n; i++)
         delete kids[i];
      delete []kids;
    }
}

// Dump object
void BaseObj::Dump(boolean verbose, int level)
{
   int i, j;

   for (i = 0; i < level; i++) printf("   ");
   printf("%s : %s\n", Name(), ObjectClassName());
   if (verbose)
    {
      for (i = -1; i <= level; i++) printf("   ");
      printf("Display Name = '%s'\n", _displayName);
      for (i = -1; i <= level; i++) printf("   ");
      printf("Details = '%s'\n", _details);
      for (i = -1; i <= level; i++) printf("   ");
      printf("Number Actions = %d\n", _numActions);
      for (j = 0; j < _numActions; j++)
       {
         for (i = -2; i <= level; i++) printf("   ");
	 printf("Action.%s = '%s'\n", _actions[j]->ReferenceName,
		_actions[j]->DisplayName);
       }
      for (i = -1; i <= level; i++) printf("   ");
      printf("Number Attributes = %d\n", _numAttributes);
      for (j = 0; j < _numAttributes; j++)
       {
         for (i = -2; i <= level; i++) printf("   ");
	 printf("Attribute.%s = '%s'\n", _attributes[j]->ReferenceName,
		_attributes[j]->DisplayValue);
       }
      for (i = -1; i <= level; i++) printf("   ");
      printf("Number Children = %d\n", _numChildren);
    }
}

// Dump object hierarchy
void BaseObj::DumpHierarchy(boolean verbose, int level)
{
   int i;

   Dump(verbose, level);
   for (i = 0; i < _numChildren; i++)
      _children[i]->DumpHierarchy(verbose, level + 1);
}

void BaseObj::Error(const char *message) 
{
   // Log Error
   printf("%s: (%s) %s\n", ObjectClassName(), Name(), message);
}

int BaseObj::RunCommand(const char *command, 
			char **std_out,
			char **std_err)
{
   int status;

   Invoke *_thread = new Invoke(command, std_out, std_err);

   status = _thread->status;
   delete _thread;

   return status;
}
