/* $XConsortium: Group.h /main/3 1995/11/06 09:40:43 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef GROUP_H
#define GROUP_H

#include "MotifUI.h"

class Group : public MotifUI {

 private:
   
   Widget _label;
   Widget _rc;
   GroupType _group_type;

 public:

   Group(MotifUI *parent,
	 char *name,
	 GroupType group_type = RADIO_GROUP);

   const Widget InnerWidget()         { return _rc; }
   const UI_Class UIClass()           { return GROUP; }
   const int UISubClass()             { return _group_type; }
   const char *const UIClassName()    { return "Group"; }

   // Override SetName, need to set label name, not BaseWidget
   boolean SetName(char *);

};

#endif /* GROUP_H */
