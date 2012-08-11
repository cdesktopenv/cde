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
/* $TOG: BaseObj.h /main/5 1998/04/06 13:34:28 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef BASEOBJ_H
#define BASEOBJ_H

#include <stdio.h>
#include "Invoke.h"

#ifndef _BOOLEAN_
#define _BOOLEAN_
#if (defined(sun) && OSMAJORVERSION <= 5 && OSMINORVERSION <= 3)|| defined(USL) || defined(__uxp__)
  #include <sys/types.h>
  #define boolean boolean_t
  #define true B_TRUE
  #define false B_FALSE
#elif defined(linux) || defined(CSRG_BASED)
  #define false 0
  #define true 0
  #define boolean int
#else
  typedef enum
  {
     false = 0,
     true = 1
  } boolean;
#endif
#endif


#ifndef STRDUP
#define STRDUP(string) (string ? strdup(string) : NULL)
#define STRCMP(s1, s2) (s1 && s2 ? strcmp(s1, s2) : (s1 ? 1 : -1))
#define STRLEN(string) (string ? strlen(string) : 0)
#endif

typedef enum
{
   STRING,
   MULT_LINE_STRING,
   INTEGER,
   REAL,
   FILE_NAME,
   DIRECTORY_NAME,
   HOST_NAME,
   USER_NAME,
   GROUP_NAME,
   DATE,
   TIME,
   MONEY,
   HOUR,
   MINUTE,
   SECOND,
   DAY,
   MONTH,
   YEAR
} AttributeType;

typedef enum
{
   OPTIONAL = 0,
   ALLOW_DIRECT_ENTRY = 1,
   REQUIRED = 2,
   EDITABLE_AFTER_CREATE = 4, 
   EDITABLE_DURING_CREATE = 8, 
   ECHO_INPUT = 16
} Characteristics;

typedef enum
{
   INFORMATION_LINE,
   NO_LIST,
   SINGLE_SELECT_LIST,
   MULTI_SELECT_LIST,
   RANGE,
   MULTI_SELECT_MIN_RANGE,
   MULTI_SELECT_MAX_RANGE,
   MULTI_SELECT_RANGE
} ValueList;

#define AllowDirectEntry(attr) (attr->Mask & ALLOW_DIRECT_ENTRY)
#define IsRequired(attr) (attr->Mask & REQUIRED)
#define EditableAfterCreate(attr) (attr->Mask & EDITABLE_AFTER_CREATE)
#define EditableDuringCreate(attr) (attr->Mask & EDITABLE_DURING_CREATE)
#define EchoInput(attr) (attr->Mask & ECHO_INPUT)

typedef struct
{
   char *ReferenceName;
   AttributeType Type;
   Characteristics Mask;
   char *DisplayName;
   char *Value;
   char *DisplayValue;
   char *DefaultValue;
   char *DisplayDefaultValue;
   char *Help;
   char *ContextualHelp;
   ValueList ValueListType;
   char *Listing;
   char *Dependancies;
   // The base class initializes the next 4 variables
   int  n_values;
   char **CompiledValueList;
   char **CompiledDisplayValueList;
   char **CompiledHelpValueList;
} Attribute;

class BaseObj;

typedef int (*ActionHandler) (BaseObj *, char **output, BaseObj *requestor);

typedef struct
{
   ActionHandler Handler;
   char *ReferenceName;
   char *DisplayName;
   char *Nmemonic;
   char *Help;
   char *ContextualHelp;
   char *AcceleratorText;
   char *Accelerator;
   boolean InputRequired;
   char *Dependancies;
} Action;

extern const char *ACTION_NOT_FOUND;

class BaseObj {

 protected:
   char *_name;
   char *_displayName;
   char *_details;
   BaseObj *_parent;
   BaseObj **_children;
   int _numChildren;
   Attribute **_attributes;
   int _numAttributes;
   Action **_actions;
   int _numActions;
   boolean _init_children;
   boolean _init_attributes;
   char *_lastActionName;
   char *_lastActionOutput;
   int _lastActionStatus;

   BaseObj(BaseObj *parent,
	  const char *name);

   void AddToParent();
   void DeleteFromParent();
   void DeleteAttribute(const char *ReferenceName);
   void AddAttribute(const char *ReferenceName,
		     const char *DisplayName = NULL,
                     const char *Help = NULL,
                     const char *ContextualHelp = NULL,
                     Characteristics Mask = OPTIONAL,
                     ValueList ValueListType = NO_LIST,
		     const char *Listing = NULL,
                     const char *Dependancies = NULL,
		     const char *DefaultValue = NULL,
		     const char *DisplayDefaultValue = NULL);
   void DeleteAction(const char *ReferenceName);
   void AddAction(ActionHandler Handler,
		  const char *ReferenceName,
		  const char *DisplayName = NULL,
                  const char *Nmemonic = NULL,
                  const char *Help = NULL,
                  const char *ContextualHelp = NULL,
                  boolean InputRequired = false,
                  const char *AcceleratorText = NULL,
                  const char *Accelerator = NULL,
                  const char *Dependancies = NULL);

   // Derived classes should redefine these functions
   virtual void InitChildren() { _children = NULL; _numChildren = 0; }
   virtual void InitDetails() { }
   virtual void LoadAttributes(int /*n_attrs*/, Attribute ** /*attrs*/) { }
   virtual void InitDisplayName() { }

   // These messages are sent to all parents
   virtual void NotifyCreate(BaseObj *) { } ;
   virtual void NotifyDelete(BaseObj *) { } ;

 public:

   virtual ~BaseObj();             // destructor

   boolean HasAttribute(Attribute *action);
   boolean HasAttribute(const char *ReferenceName);
   boolean HasAttribute(const char *ReferenceName, Attribute **action);
   boolean HasAction(Action *action);
   boolean HasAction(const char *ReferenceName);
   boolean HasAction(const char *ReferenceName, Action **action);
   boolean SendAction(Action *action,
		      BaseObj *requestor = NULL);
   boolean SendAction(const char *ReferenceName, 
		      BaseObj *requestor = NULL);

   char * LastActionName()        { return _lastActionName; }
   char * LastActionOutput()      { return _lastActionOutput; }
   int LastActionStatus()         { return _lastActionStatus; }

   const char * Name()            { return _name; }
   char * DisplayName();
   char * Details();
   char * AttributeValue(char *ReferenceName);
   BaseObj * Parent()             { return _parent; }
   const int NumActions()         { return _numActions; }
   const int NumAttributes()      { return _numAttributes; }
   Action **Actions()             { return _actions; }
   Attribute **Attributes()       { return _attributes; }
   BaseObj ** Children();
   int NumChildren();
   void ReadAttributes();
   void UpdateChildren();
   void UpdateDetails();
   void DeleteChildren();
   void SetInitChildren()         { _init_children = true; }

   int RunCommand(const char *command,
	          char **std_out = NULL,
	          char **std_err = NULL);

   // These are for children
   BaseObj ** Siblings();
   int NumSiblings();

   // Log Error message
   void Error(const char *message);

   // Dumps object to stdout
   void Dump(boolean verbose = false,
	     int level = 0);
   // Dumps object heirarchy to stdout
   void DumpHierarchy(boolean verbose = false,
		      int level = 0);

   virtual const char *const ObjectClassName() { return "BaseObj"; }

};

#endif // BASEOBJ_H
