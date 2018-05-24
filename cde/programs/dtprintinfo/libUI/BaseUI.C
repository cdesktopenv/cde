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
/* $TOG: BaseUI.C /main/10 1998/07/24 16:14:23 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "BaseUI.h"

#ifdef NO_REGCOMP
#include <ctype.h>
#if defined(SVR4) || defined(SYSV)
#include <libgen.h>
#endif
#else
#include <regex.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef hpux
  #include <time.h>
#elif defined(__linux__)
  #include <sys/time.h>
#else
  #include <sys/select.h>
#endif /* hpux */
#ifdef _AIX
#include <strings.h>            /* need to get bzero defined */
#endif /* _AIX */

#if defined(aix)
extern "C"
{
extern int strcasecmp(const char *,  const char *);
}
#endif

#define IsContainer(p) (p->UIClass() == CONTAINER)

#ifdef DEBUG
#define PrintObject() if (trace_calls) fprintf(stderr, " %s\n", _name);
#else
#define PrintObject()
#endif

int BaseUI::_UniqueID = 1;

BaseUI::BaseUI(BaseUI *parent,
	       const char *name,
	       const char *category)
{
   EnterFunction("BaseUI::BaseUI");
   _id = _UniqueID++;
   _name = STRDUP(name);
   PrintObject();

   _category = STRDUP(category);
   if (parent)
    {
      _viewStyle = parent->_viewStyle;
      _iconStyle = parent->_iconStyle;
      _visible = parent->_visible;
      _active = parent->_active;
    }
   else
    {
      _viewStyle = GRID;
      _iconStyle = LARGE_ICON;
      _visible = true;
      _active = true;
    }
   _update_message = NULL;
   _selected = false;
   _parent = parent;
   _children = NULL;
   _numChildren = 0;
   _allChildren = NULL;
   _numAllChildren = 0;
   _has_been_opened = false;
   _opened = false;
   AddToParent();
   AddToParentContainer();
   BaseUI *p = _parent;
   while (p && !IsContainer(p))
    {
      p->NotifyCreate(this);
      p = p->_parent;
    }
   if (p && IsContainer(p))
      p->NotifyCreate(this);

   LeaveFunction("BaseUI::BaseUI");
   PrintObject();
}

BaseUI::~BaseUI()
{
   EnterFunction("BaseUI::~BaseUI()");
   PrintObject();

   BaseUI *p = _parent;

   DeleteChildren();
   DeleteFromParent();
   DeleteFromParentContainer();

   while (p)
    {
      if (IsContainer(p))
       {
         if (p->UISubClass() == ICON_LIST ||
             p->UISubClass() == SCROLLED_ICON_LIST)
            break;
         if (!IsContainer(p->_parent) && p->_parent->UIClass() != ICON)
            break;
       }
      p->NotifyDelete(this);
      p = p->_parent;
    }
   if (p && IsContainer(p))
      p->NotifyDelete(this);

   delete(_update_message);
   delete []_children;
   delete []_allChildren;

   LeaveFunction("BaseUI::~BaseUI()");
   PrintObject();

   free(_category);
   free(_name);
}

void BaseUI::ContextualHelp()
{
   DoContextualHelp();
}

void BaseUI::Refresh()
{
   DoRefresh();
}

void BaseUI::ToFront()
{
   DoToFront();
}

void BaseUI::Parent(BaseUI *new_parent)
{
   if (SetParent(new_parent))
    {
      // need to write this 
      ;
    }
}

void BaseUI::Open(boolean flag)
{
   if (SetOpen(flag))
    {
      _has_been_opened = true;
      _opened = flag;
      BaseUI *p = _parent;
      while (p && !IsContainer(p))
       {
         p->NotifyOpen(this);
         p = p->_parent;
       }
      if (p && IsContainer(p))
         p->NotifyOpen(this);
    }
}

void BaseUI::AddToParent()
{
   if (!_parent)
      return;

   BaseUI **new_children;
   int i;

   new_children = new BaseUI*[_parent->_numChildren + 1];
   for (i = 0; i < _parent->_numChildren; i++)
      new_children[i] = _parent->_children[i];

   delete []_parent->_children;
   _parent->_children = new_children;
   _parent->_children[_parent->_numChildren] = this;

   _parent->_numChildren++;
}

void BaseUI::DeleteFromParent()
{
   if (!_parent)
      return;

   BaseUI **new_children;
   int i, index;

   index = 0;
   new_children = new BaseUI*[_parent->_numChildren - 1];
   for (i = 0; i < _parent->_numChildren; i++)
      if (_parent->_children[i] != this)
         new_children[index++] = _parent->_children[i];

   delete []_parent->_children;
   _parent->_children = new_children;

   _parent->_numChildren--;
}

void BaseUI::DeleteChildren()
{
   EnterFunction("BaseUI::DeleteChildren()");
   PrintObject();

   if (_numChildren)
    {
      BaseUI **kids = new BaseUI*[_numChildren];
      int i, j, n = _numChildren;
      // Save children first before deleting a child since the destructor
      // updates the _children variable
      for (i = 0; i < n; i++)
         kids[i] = _children[i];
      for (i = 0; i < n; i++)
       {
         for (j = 0; j < _numChildren; j++)
           if (_children[j] == kids[i])
            {
              delete kids[i];
              break;
            }
       }
      delete []kids;
    }
   LeaveFunction("BaseUI::DeleteChildren()");
   PrintObject();
}

BaseUI * BaseUI::ContainerParent()
{
   BaseUI *parent = this;

   while (parent && !IsContainer(parent))
      parent = parent->_parent;
      
   return parent;
}

void BaseUI::AddToParentContainer()
{
   BaseUI *parent;

   if (!(parent = ContainerParent()))
      return;

   BaseUI **new_children;
   int i;

   new_children = new BaseUI*[parent->_numAllChildren + 1];
   for (i = 0; i < parent->_numAllChildren; i++)
      new_children[i] = parent->_allChildren[i];

   delete []parent->_allChildren;
   parent->_allChildren = new_children;
   parent->_allChildren[parent->_numAllChildren] = this;

   parent->_numAllChildren++;
}

void BaseUI::DeleteFromParentContainer()
{
   BaseUI *parent;

   if (!(parent = ContainerParent()))
      return;

   BaseUI **new_children;
   int i, index;

   index = 0;
   new_children = new BaseUI*[parent->_numAllChildren - 1];
   for (i = 0; i < parent->_numAllChildren; i++)
      if (parent->_allChildren[i] != this)
         new_children[index++] = parent->_allChildren[i];

   delete []parent->_allChildren;
   parent->_allChildren = new_children;

   parent->_numAllChildren--;
}

boolean BaseUI::HandleHelpRequest()
{
   BaseUI *parent = _parent;
   while (parent && parent->HandleHelpRequest() == false)
      parent = parent->_parent;
   return true;
}

void BaseUI::SetVisible(boolean flag)
{
   if (SetVisiblity(flag))
    {
      _visible = flag;
      BaseUI *p = _parent;
      while (p)
       {
         if (IsContainer(p))
	  {
	    if (p->UISubClass() == ICON_LIST ||
	        p->UISubClass() == SCROLLED_ICON_LIST)
	       break;
	    if (!IsContainer(p->_parent) && p->_parent->UIClass() != ICON)
	       break;
	  }
         p->NotifyVisiblity(this);
         p = p->_parent;
       }
      if (p && IsContainer(p))
         p->NotifyVisiblity(this);
    }
}

// Set visiblity, calls derived function SetVisiblity
void BaseUI::Visible(boolean flag)
{
   if (UIClass() != CONTAINER &&
       UIClass() != MAIN_WINDOW &&
       UIClass() != DIALOG &&
       UIClass() != APPLICATION)
    {
      int i;
      SetVisible(flag);
      for (i = 0; i < _numChildren; i++)
         if (!IsContainer(_children[i]))
            _children[i]->Visible(flag);
    }
   else
      SetVisible(flag);
}

// Set sensitivity, calls derived function SetActivity
void BaseUI::Active(boolean flag)
{
   if (UIClass() != CONTAINER)
    {
      int i;
      if (flag == true)
       {
	if (SetActivity(flag))
	    _active = flag;
	 for (i = 0; i < _numChildren; i++)
	    if (!IsContainer(_children[i]))
               _children[i]->Active(flag);
       }
      else
       {
	 for (i = 0; i < _numChildren; i++)
	    if (!IsContainer(_children[i]))
               _children[i]->Active(flag);
	if (SetActivity(flag))
	    _active = flag;
       }
    }
   else if (SetActivity(flag))
      _active = flag;
}

// Set view, calls derived function SetIcon
void BaseUI::ContainerView(ViewStyle viewStyle)
{
   if (UIClass() == CONTAINER)
    {
      int i;
      for (i = 0; i < _numAllChildren; i++)
	 if (!IsContainer(_allChildren[i]))
            _allChildren[i]->ContainerView(viewStyle);
    }
   _viewStyle = viewStyle;
}

// Set view, calls derived function SetIcon
void BaseUI::IconView(IconStyle iconStyle)
{
   if (UIClass() == CONTAINER)
    {
      int i;
      for (i = 0; i < _numAllChildren; i++)
	 if (IsContainer(_allChildren[i]))
	  {
	    if (UISubClass() == SCROLLED_ICON_LIST || UISubClass() == ICON_LIST)
               _allChildren[i]->IconView(iconStyle);
	  }
	 else
            _allChildren[i]->IconView(iconStyle);
    }
   if (SetIcon(iconStyle))
      _iconStyle = iconStyle;
}

// Set selected status, calls derived function SetSelected
void BaseUI::Selected(boolean flag)
{
   if (SetSelected(flag))
    {
      _selected = flag;
      BaseUI *p = _parent;
      while (p)
       {
         if (IsContainer(p))
	  {
	    if (p->UISubClass() == ICON_LIST ||
	        p->UISubClass() == SCROLLED_ICON_LIST)
	       break;
	    if (!IsContainer(p->_parent) && p->_parent->UIClass() != ICON)
	       break;
	  }
         p->NotifySelected(this);
         p = p->_parent;
       }
      if (p && IsContainer(p))
         p->NotifySelected(this);
    }
}

// Set name, calls derived function SetCategory
void BaseUI::Category(char *category)
{
   if (SetCategory(category))
    {
      free(_category);
      _category = STRDUP(category);
    }
}

// Set name, calls derived function SetName
void BaseUI::Name(char *name)
{
   if (SetName(name))
    {
      free(_name);
      _name = STRDUP(name);
    }
}

// Select/UnSelected all immediate children
void BaseUI::SelectAll(boolean select_status)
{
   int i;

   for (i = 0; i < _numChildren; i++)
    {
      _children[i]->Selected(select_status);
      if (UISubClass() == SCROLLED_ICON_LIST || UISubClass() == ICON_LIST)
         _children[i]->SelectAll(select_status);
    }

}

// Select/UnSelected all children, and their children, etc...
void BaseUI::SelectAllDescendants(boolean select_status)
{
   int i;

   if (UIClass() == CONTAINER)
      for (i = 0; i < _numChildren; i++)
         _children[i]->Selected(select_status);
   else
      for (i = 0; i < _numChildren; i++)
       {
         _children[i]->Selected(select_status);
	 if (!IsContainer(_children[i]))
            _children[i]->SelectAllDescendants(select_status);
       }
}

// Open/Close all children, and their children, etc...
void BaseUI::OpenAllDescendants(boolean opened)
{
   int i;

   if (UIClass() == CONTAINER)
      for (i = 0; i < _numChildren; i++)
         _children[i]->Open(opened);
   else
    {
      if (opened)
         for (i = 0; i < _numChildren; i++)
          {
            _children[i]->Open(opened);
	    if (!IsContainer(_children[i]))
               _children[i]->OpenAllDescendants(opened);
          }
      else
         for (i = 0; i < _numChildren; i++)
          {
	    if (!IsContainer(_children[i]))
               _children[i]->OpenAllDescendants(opened);
            _children[i]->Open(opened);
          }
    }
}

void BaseUI::Selection(int *n_items,
                       BaseUI ***items)
{
   if (UIClass() != CONTAINER)
    {
      if (items)
         *items = NULL;
      *n_items = 0;
      return;
    }

   boolean isScrolledList;
   if (UISubClass() == SCROLLED_ICON_LIST || UISubClass() == ICON_LIST)
      isScrolledList = true;
   else
      isScrolledList = false;

   int i;
   *n_items = 0;
   for (i = 0; i < _numAllChildren; i++)
      if (!IsContainer(_allChildren[i]) && _allChildren[i]->Selected())
         (*n_items)++;
      else if (IsContainer(_allChildren[i]) && isScrolledList)
       {
	 int tmp;
	 _allChildren[i]->Selection(&tmp);
	 (*n_items) += tmp;
       }

   if (!items)
      return;

   if (*n_items == 0)
      *items = NULL;
   else
    {
      BaseUI **list = new BaseUI*[*n_items];
      *n_items = 0;
      for (i = 0; i < _numAllChildren; i++)
         if (!IsContainer(_allChildren[i]) && _allChildren[i]->Selected())
	    list[(*n_items)++] = _allChildren[i];
         else if (IsContainer(_allChildren[i]) && isScrolledList)
          {
	    int tmp;
	    BaseUI **tmp1;
	    _allChildren[i]->Selection(&tmp, &tmp1);
	    int j, k = *n_items + tmp;
	    for (j = *n_items; j < k; j++)
	       list[(*n_items)++] = tmp1[j];
	    delete []tmp1;
          }

      *items = list;
    }
}

int BaseUI::NumContainerChildren()
{
   BaseUI *container = ContainerParent();

   if (container)
      return container->_numAllChildren;
   else
      return 0;
}

BaseUI ** BaseUI::ContainerChildren()
{
   BaseUI *container = ContainerParent();

   if (container)
      return container->_allChildren;
   else
      return NULL;
}

int BaseUI::NumSiblings()
{
   if (_parent)
      return _parent->_numChildren;
   else
      return 0;
}

BaseUI ** BaseUI::Siblings()
{
   if (_parent)
      return _parent->_children;
   else
      return NULL;
}

void BaseUI::_Find(void *pattern, int depth, int *n_matches,
		   BaseUI ***matches, SelectProc select_proc,
		   boolean regular_expression, boolean case_sensitive,
		   boolean find_by_name, int cur_depth)
{
   if (depth != 0 && cur_depth == depth)
      return;

   BaseUI **children = _children;
   int n_children = _numChildren;
   int i;
   for (i = 0; i < n_children; i++)
    {
      int tmp = 0;
      BaseUI **tmp1 = NULL;
      children[i]->_Find(pattern, depth, &tmp, &tmp1, select_proc,
                             regular_expression, case_sensitive,
                             find_by_name, cur_depth + 1);
      if (tmp)
       {
         int j;
	 if (*n_matches == 0)
	    *matches = (BaseUI **)malloc(sizeof(BaseUI **) * tmp);
	 else
	    *matches = (BaseUI **)realloc(*matches, sizeof(BaseUI **) *
					  (*n_matches + tmp));
         for (j = 0; j < tmp; j++)
            (*matches)[(*n_matches)++] = tmp1[j];
         free(tmp1);
	 tmp = 0;
       }
      char *value;
      int is_match;
      if (find_by_name)
         value = children[i]->_name;
      else
         value = children[i]->_category;
      if (value)
       {
         if (case_sensitive)
          {
            if (regular_expression)
#ifdef NO_REGCOMP
               is_match = (int)regex((char *)pattern, value);
#else
               is_match = !regexec((regex_t *)pattern, value, (size_t)0,NULL,0);
#endif
            else
	       is_match = !strcmp(value, (char *)pattern);
          }
         else
          {
            if (regular_expression)
#ifdef NO_REGCOMP
               is_match = (int)regex((char *)pattern, value);
#else
               is_match = !regexec((regex_t *)pattern, value, (size_t)0,NULL,0);
#endif
            else 
	       is_match = !strcasecmp(value, (char *)pattern);
          }
        }
       else
          is_match = 0;

      if (is_match && select_proc)
         is_match = (*select_proc)(children[i]);
      if (is_match)
       {
	 if (*n_matches == 0)
	    *matches = (BaseUI **)malloc(sizeof(BaseUI **));
	 else
	    *matches = (BaseUI **)realloc(*matches, sizeof(BaseUI **) *
					  (*n_matches + 1));
         (*matches)[(*n_matches)++] = children[i];
       }
    }
}

BaseUI *BaseUI::_FindBy(char *pattern, int depth, int *n_matches,
		        BaseUI ***matches, SelectProc select_proc,
			boolean regular_expression, boolean case_sensitive, 
			boolean find_by_name)
{
   void *reg_expr = pattern;
#ifdef NO_REGCOMP
   if (regular_expression)
    {
      if (case_sensitive == false)
       {
         char *new_pattern = new char[(strlen(pattern) * 4) + 1];
         int i = 0;
         char *s;
         for (s = pattern; *s; s++)
          {
	     if (isalpha((int)*s))
	      {
	        new_pattern[i++] = '[';
	        new_pattern[i++] = *s;
	        if (islower(*s))
	           new_pattern[i++] = (char)toupper((int)*s);
	        else
	           new_pattern[i++] = (char)tolower((int)*s);
	        new_pattern[i++] = ']';
	      }
	     else
	        new_pattern[i++] = *s;
          }
         new_pattern[i] = '\0';
         reg_expr = regcmp(new_pattern, (char *)NULL);
	 delete new_pattern;
       }
      else
	 reg_expr = regcmp(pattern, (char *)NULL);
    }
#else
   regex_t re;
   if (regular_expression)
    {
      int compile_flags;
      if (case_sensitive)
         compile_flags = REG_NOSUB;
      else
         compile_flags = REG_ICASE|REG_NOSUB;
      if (regcomp(&re, pattern, compile_flags) != 0)
         reg_expr = NULL;
      else
         reg_expr = &re;
    }
#endif
   if (!reg_expr)
    {
      if (n_matches)
         *n_matches = 0;
      if (matches)
         matches = NULL;
      return NULL;
    }
   BaseUI **_matches = NULL;
   int _n_matches = 0;
   _Find(reg_expr, depth, &_n_matches, &_matches, select_proc,
	 regular_expression, case_sensitive, find_by_name, 0);
  
#ifndef NO_REGCOMP
   if (regular_expression)
      regfree(&re);
#endif
   if (n_matches)
      *n_matches = _n_matches;
   if (matches)
    {
      *matches = _matches;
      if (_n_matches)
         return _matches[0];
      else
         return NULL;
    }
   else if (_n_matches)
    {
      BaseUI *a_match = _matches[0];
      free(_matches);
      return a_match;
    }
   return NULL;
}

BaseUI *BaseUI::FindByName(char *pattern, int depth, int *n_matches,
		           BaseUI ***matches, SelectProc select_proc,
			   boolean regular_expression, boolean case_sensitive)
{
   return _FindBy(pattern, depth, n_matches, matches, select_proc,
                  regular_expression, case_sensitive, true);
}

BaseUI *BaseUI::FindByCategory(char *pattern, int depth, int *n_matches,
			       BaseUI ***matches, SelectProc select_proc,
			       boolean regular_expression,
			       boolean case_sensitive)
{
   return _FindBy(pattern, depth, n_matches, matches, select_proc,
                  regular_expression, case_sensitive, false);
}

void BaseUI::OrderByName(boolean /*flag*/)
{
}

void BaseUI::GroupByCategory(boolean /*flag*/)
{
}

// Calls derived class's SetOrder function
void BaseUI::Order(int new_position)
{
  if (!_parent || new_position < 0 || new_position > _parent->_numChildren)
     return;

  int current_position = Order();
  if (new_position == current_position)
    return;

  BaseUI **children = _parent->_children;
  int i;
  if (current_position < new_position)
     for (i = current_position; i < new_position; i++)
	children[i] = children[i + 1];
  else
     for (i = current_position; i > new_position; i--)
	children[i] = children[i - 1];
  children[new_position] = this;
  (void) SetOrder(new_position);
}

int BaseUI::Order()
{
   int i = 0;
   if (_parent)
    {
      for (i = 0; i < _parent->_numChildren; i++)
         if (_parent->_children[i] == this)
	    break;
    }
   return i;
}

boolean BaseUI::IsVisible()
{
   return DoIsVisible();
}

void BaseUI::MakeVisible()
{
   if (_visible == false)
      return;
   BaseUI *parent = Parent();
   if (parent)
     parent->MakeVisible();
   if (_visible && parent)
      DoMakeVisible();
}

void BaseUI::BeginUpdate()
{
   _update = false;
   DoBeginUpdate();
}

void BaseUI::EndUpdate()
{
   _update = true;
   DoEndUpdate();
}

void BaseUI::UpdateMessage(const char *message)
{
   free(_update_message);
   _update_message = STRDUP(message);
   DoUpdateMessage(_update_message);
}

boolean BaseUI::ObjectExists(int unique_id)
{
   int i;
   boolean found = false;
   for (i = 0; i < _numChildren; i++)
    {
      BaseUI *child = _children[i];
      if (unique_id == _children[i]->_id)
       {
         found = true;
         break;
       }
      else if (found = _children[i]->ObjectExists(unique_id))
	 break;
    }
   return found;
}

#define PrintBoolean(flag) flag ? "True" : "False"

// Dump object to stdout
void BaseUI::Dump(boolean verbose, int level)
{
   int i;

   for (i = 0; i < level; i++)
      printf("   ");
   printf("%s : %s\n", _name, UIClassName());
   if (verbose)
    {
      for (i = -1; i <= level; i++) printf("   ");
      printf("Category : %s\n", _category ? _category : "");
      for (i = -1; i <= level; i++) printf("   ");
      printf("HasBeenOpened : %s\n", PrintBoolean(_has_been_opened));
      for (i = -1; i <= level; i++) printf("   ");
      printf("Opened : %s\n", PrintBoolean(_opened));
      for (i = -1; i <= level; i++) printf("   ");
      printf("Visible : %s\n", PrintBoolean(_visible));
      for (i = -1; i <= level; i++) printf("   ");
      printf("Active : %s\n", PrintBoolean(_active));
      for (i = -1; i <= level; i++) printf("   ");
      printf("Selected : %s\n", PrintBoolean(_selected));
      for (i = -1; i <= level; i++) printf("   ");
      switch (_viewStyle)
      {
      case AS_PLACED: printf("ViewStyle : AS_PLACED\n"); break;
      case GRID: printf("ViewStyle : GRID\n"); break;
      case BROWSER: printf("ViewStyle : BROWSER\n"); break;
      case TREE: printf("ViewStyle : TREE\n"); break;
      case PROPERTIES: printf("ViewStyle : PROPERTIES\n"); break;
      }
      for (i = -1; i <= level; i++) printf("   ");
      switch (_iconStyle)
      {
      case NAME_ONLY: printf("IconStyle : NAME_ONLY\n"); break;
      case LARGE_ICON: printf("IconStyle : LARGE_ICON\n"); break;
      case SMALL_ICON: printf("IconStyle : SMALL_ICON\n"); break;
      case DETAILS: printf("IconStyle : DETAILS\n"); break;
      }
      for (i = -1; i <= level; i++) printf("   ");
      printf("Number Children = %d\n",  _numChildren);
    }
}

// Dump object hierarchy to stdout
void BaseUI::DumpHierarchy(boolean verbose, int level)
{
   Dump(verbose, level);

   int i;
   for (i = 0; i < _numChildren; i++)
      _children[i]->DumpHierarchy(verbose, level + 1);
}

void BaseUI::AddTimeOut(TimeOutCallback timeoutCB, void *data, long interval)
{
   if (timeoutCB)
    {
      if (data)
         SetAddTimeOut(timeoutCB, data, interval);
      else
         SetAddTimeOut(timeoutCB, this, interval);
    }
}

int BaseUI::MicroSleep(long usecs)
{
   struct timeval timeoutVal;
   timeoutVal.tv_sec = usecs / 1000000;
   timeoutVal.tv_usec = usecs & 1000000;

   fd_set rdmask;
   fd_set wrmask;
   fd_set exmask;
   FD_ZERO(&rdmask);
   FD_ZERO(&wrmask);
   FD_ZERO(&exmask);

   return select(0,&rdmask,&wrmask,&exmask,&timeoutVal);
}

// DEBUG STUFF

#ifdef DEBUG

int BaseUI::indent = 0;
boolean BaseUI::trace_calls = false;

static char * make_header_buffer(int indent)
{
   static char header_buffer[1024];
   static char *leading_chars="1234567890abcdefghijklmnop";
   int indx;
   char c;

   header_buffer[0] = '\0';

   for (indx = 0 ; indx < indent; indx++)
    {
      c = leading_chars[indx];
      header_buffer[2*indx] = leading_chars[indx];
      header_buffer[2*indx +1] = ' ';
    }

   header_buffer[2*indx] = '\0';

   return(header_buffer);
}

void BaseUI::EnterFunction(char *FunctionName)
{
   char *format_buffer = new char[1024];
   char *header_buffer;

   if (trace_calls)
    {
      header_buffer = make_header_buffer(indent);
      sprintf(format_buffer,"%s>: %%s  %d ",header_buffer,indent);
      fprintf(stderr,format_buffer,FunctionName);
      indent++;
    }
   delete [] format_buffer;
}

void BaseUI::LeaveFunction(char *FunctionName)
{
   char *format_buffer = new char[1024];
   char *header_buffer;

   if (trace_calls)
    {
      indent--;
      header_buffer = make_header_buffer(indent);
      sprintf(format_buffer,"%s<: %%s  %d ",header_buffer,indent);
      fprintf(stderr,format_buffer,FunctionName);
    }
   delete [] format_buffer;
}
#endif // DEBUG
