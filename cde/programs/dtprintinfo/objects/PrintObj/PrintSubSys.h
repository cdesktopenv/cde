/* $XConsortium: PrintSubSys.h /main/3 1995/11/06 09:47:33 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef PRINTSUBSYS_H
#define PRINTSUBSYS_H

#include "BaseObj.h"
#include "dtprintinfomsg.h"

// Object Class Name
extern const char *PRINTSUBSYSTEM;

// List Children command;
extern const char *LIST_QUEUES;

class PrintSubSystem : public BaseObj {

 protected:

   void InitChildren();

 public:

   PrintSubSystem(BaseObj *parent);
   virtual ~PrintSubSystem();

   virtual const char *const ObjectClassName() { return PRINTSUBSYSTEM; }

};

#endif // PRINTSUBSYS_H
