/*
 * $XConsortium: RestraintP.h /main/3 1996/06/11 16:45:58 cde-hal $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */

#ifndef _RestraintP_h
#define _RestraintP_h

#include "Restraint.h"
#include <Xm/XmP.h>
#if XmVersion >= 1002
#include <Xm/PrimitiveP.h>
#endif

/*  Restraint class structure.  */
#include <Xm/ManagerP.h>

typedef struct _RestraintClassPart
{
  XtPointer	extension;
} RestraintClassPart;


/*  Full class record declaration for Restraint class.  */

typedef struct _RestraintClassRec
{
   CoreClassPart         core_class;
   CompositeClassPart    composite_class;
   ConstraintClassPart   constraint_class;
   XmManagerClassPart    manager_class;
   RestraintClassPart    restraint_class;
} RestraintClassRec;

externalref RestraintClassRec restraintClassRec;

/*  The Restraint instance part record  */

typedef struct _RestraintPart
{
  /* Resource data */
  Boolean had_child;
} RestraintPart;


/* **************************************************************
 * Restraint class instance record
 * ************************************************************** */

typedef struct _RestraintRec
{
   CorePart	    core;
   CompositePart    composite;
   ConstraintPart   constraint;
   XmManagerPart    manager;
   RestraintPart    restraint;
} RestraintRec;

#endif /* _RestraintP_h */
/* DON'T ADD STUFF AFTER THIS #endif */
