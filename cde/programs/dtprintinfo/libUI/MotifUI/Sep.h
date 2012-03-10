/* $XConsortium: Sep.h /main/3 1995/11/06 09:44:53 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef SEP_H
#define SEP_H

#include "MotifUI.h"

class Sep : public MotifUI {

 private:

   SeparatorStyle _style;

 public:

   Sep(MotifUI * parent,
       SeparatorStyle style = SHADOW_ETCHED_IN);
   Sep(char *category,
       MotifUI * parent,
       SeparatorStyle style = SHADOW_ETCHED_IN);

   const UI_Class UIClass()           { return SEPARATOR; }
   const int UISubClass()             { return _style; }
   const char *const UIClassName()    { return "Sep"; }

};

#endif /* SEP_H */
