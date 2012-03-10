/*******************************************************************************
**
**  dnd.h
**
**  static char sccsid[] = "@(#)dnd.h 1.9 94/11/07 Copyr 1993 Sun Microsystems, Inc.";
**
**  $XConsortium: dnd.h /main/4 1996/04/15 13:24:16 rswiston $
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
**  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/

/*									*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#ifndef _DND_H
#define _DND_H

#include "ansi_c.h"

typedef enum {SingleEditorList, 
	      SingleEditorIcon, 
	      GroupEditorList, 
	      GroupEditorIcon, 
	      TodoEditorList, 
	      TodoEditorIcon, 
	      StandAloneEditor } EditorType;

typedef struct {
	Calendar	*calendar;
	EditorType	editor_type;
	caddr_t		editor;
	char		*data;
} DragContext;

#define DRAG_THRESHOLD 4


extern void cm_register_drop_site	P((Calendar *c, Widget w));
extern void ApptDragStart		P((Widget, XEvent *, Calendar *,
					   EditorType));
extern void TranslationDragStart	P((Widget, XEvent *, String *,
					   Cardinal *));
extern void DtcmProcessPress		P((Widget, XEvent *, String *,
					   Cardinal *));

#endif
