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
/*
 * $XConsortium: Restraint.c /main/4 1996/09/27 19:02:59 drk $
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

#ifdef DEBUG
#define ON_DEBUG(X) X
#else
#define ON_DEBUG(X)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "RestraintP.h"

/*  Static routine definitions  */

static void Initialize();
static void ChangeManaged();
static void Resize();
static void Realize();
static XtGeometryResult GeometryManager();

/*  The Restraint class record definition  */
externaldef(restraintclassrec) RestraintClassRec restraintClassRec =
{
   {
      (WidgetClass) &xmManagerClassRec, /* superclass	 	 */
      "Restraint",			/* class_name	         */	
      sizeof(RestraintRec),		/* widget_size           */	
      NULL,				/* class_initialize      */    
      NULL, 			        /* class_part_initialize */
      False,                            /* class_inited          */	
      (XtInitProc) Initialize,          /* initialize	         */	
      NULL,                             /* initialize_hook       */
      Realize,				/* realize	         */	
      NULL,				/* actions               */	
      0,				/* num_actions    	 */	
      NULL, 	                        /* resources	         */	
      0,				/* num_resources         */	
      NULLQUARK,                        /* xrm_class	         */	
      True,                             /* compress_motion       */	
      XtExposeCompressMaximal,	       	/* compress_exposure     */	
      True,                             /* compress_enterleave   */
      False,                            /* visible_interest      */	
      0,				/* destroy               */	
      Resize,			        /* resize                */	
      NULL,			        /* expose                */	
      NULL,				/* set_values	         */	
      NULL,                             /* set_values_hook       */
      XtInheritSetValuesAlmost,         /* set_values_almost     */
      NULL,                             /* get_values_hook       */
      NULL,                             /* accept_focus	         */	
      XtVersion,                        /* version               */
      NULL,                             /* callback private      */
      NULL,				/* tm_table              */
      NULL,                             /* query_geometry        */
      NULL,                             /* display_accelerator   */
      NULL,                             /* extension             */
   },

   {		/* composite_class fields */
      (XtGeometryHandler) GeometryManager,	/* geometry_manager   */
      (XtWidgetProc) ChangeManaged,		/* change_managed     */
      XtInheritInsertChild,			/* insert_child       */
      XtInheritDeleteChild,			/* delete_child       */
      NULL,                                     /* extension          */
   },

   {		/* constraint_class fields */
      NULL,					/* resource list        */   
      0,					/* num resources        */   
      0,					/* constraint size      */   
      NULL,					/* init proc            */   
      NULL,             			/* destroy proc         */   
      NULL,					/* set values proc      */   
      NULL,                                     /* extension            */
   },

   {		/* manager_class fields */
      XtInheritTranslations,			/* default translations   */
      NULL,					/* syn_resources      	  */
      0,					/* num_syn_resources 	  */
      NULL,					/* syn_cont_resources     */
      0,					/* num_syn_cont_resources */
      XmInheritParentProcess,			/* parent_process         */
      NULL,					/* extension		  */
   },

   {		/* restraint class fields */     
      NULL					/* extension		  */
   }
};

externaldef(restraintwidgetclass) WidgetClass restraintWidgetClass =
				   (WidgetClass) &restraintClassRec;


/* **************************************************************
 * Initialize
 * ************************************************************** */

static void
Initialize (RestraintWidget original, RestraintWidget w,
	    ArgList args, Cardinal *num_args)
{
  /* No size preferences until child is managed. */
  w->restraint.had_child = False;
}


/* **************************************************************
 * Realize
 * ************************************************************** */

static void
Realize (Widget w, XtValueMask *value_mask, XSetWindowAttributes *attrs)
{
  /* Might eventually need to try set up a reasonable size before realizing. */
  ON_DEBUG(printf ("Restraint::Realize\n"));
  (*restraintWidgetClass->core_class.superclass->core_class.realize)
    (w, value_mask, attrs);
}


/* **************************************************************
 * compute_height - compute my desired height
 * ************************************************************** */

/* Try to take on the height of the child. */

static void
compute_height (RestraintWidget w, Widget child)
{
  XtGeometryResult result;
  XtWidgetGeometry reply, intended;
  XtWidgetGeometry my_request, my_reply;

  /* -------- See how big the kid wants to be. -------- */
  /* Let it know we plan to change his width. */
  intended.request_mode = CWWidth;
  intended.width = w->core.width;
  XtQueryGeometry (child, &intended, &reply);
  ON_DEBUG(printf ("In Restraint::compute_height, child wants to be:\n"));
  ON_DEBUG(printf ("  Width: %d,  Height: %d\n", reply.width, reply.height));

  /* -------- Check with my parent for the change. -------- */
  if (reply.height != 0)
    {
      my_request.request_mode = CWHeight;
      my_request.height = reply.height;
      ON_DEBUG(printf ("Restraint: Asking parent for height %d\n", reply.height));
      result = XtMakeGeometryRequest ((Widget) w, &my_request, &my_reply);

      if (result == XtGeometryAlmost)
	{
	  ON_DEBUG(printf ("  Parent said Almost\n"));
	  /* Go with suggestion if we can get taller. */
	  if (my_reply.height > w->core.height)
	    {
	      ON_DEBUG(printf ("  Asking again for %d\n", my_reply.height));
	      my_request.height = my_reply.height;
	      /* Parent must say Yes. */
	      XtMakeGeometryRequest ((Widget) w, &my_request, NULL);
	    }
	}
      else if (result == XtGeometryYes)
	ON_DEBUG(printf ("  Parent said Yes.\n"));
      else if (result == XtGeometryNo)
	ON_DEBUG(printf ("  Parent said No.\n"));
      else
	ON_DEBUG(printf ("  *** Impossible Reply = %d ***\n", result));
    }

  ON_DEBUG(printf ("Restraint: compute_height = %d\n", w->core.height));
}


/* **************************************************************
 * compute_width - compute my desired width based on kid
 * ************************************************************** */

static void
compute_width (RestraintWidget w, Widget child)
{
  XtGeometryResult result;
  XtWidgetGeometry reply;
  XtWidgetGeometry my_request, my_reply;

  /* Let's see how big the kid wants to be. */
  XtQueryGeometry (child, NULL, &reply);

  ON_DEBUG(printf ("In Restraint::compute_width, child wants to be:\n"));
  ON_DEBUG(printf ("  Width: %d,  Height: %d\n", reply.width, reply.height));
  
  /* Try to grow if the child wants to be wider. */
  if (reply.width > w->core.width)
    {
      my_request.request_mode = CWWidth;
      my_request.width = reply.width;

      ON_DEBUG(printf ("Restraint: Asking parent width = %d\n", reply.width));
      result = XtMakeGeometryRequest ((Widget) w, &my_request, &my_reply);

      /* Go with parent suggestion on almost. */
      if (result == XtGeometryAlmost)
	{
	  ON_DEBUG(printf ("  Parent said Almost.\n"));
	  ON_DEBUG(printf ("  Using suggested width of %d\n", my_reply.width));
	  my_request.width = my_reply.width;

	  /* Parent must say Yes. */
	  XtMakeGeometryRequest ((Widget) w, &my_request, NULL);
	}
      else if (result == XtGeometryYes)
	ON_DEBUG(printf ("  Parent said Yes\n"));
      else if (result == XtGeometryNo)
	ON_DEBUG(printf ("  Parent said No\n"));
      else
	ON_DEBUG(printf ("  *** Impossible Reply = %d ***\n", result));
    }

  ON_DEBUG(printf ("Restraint: compute_width = %d\n", w->core.width));
}


/* **************************************************************
 * ChangedManaged - do Motif traversal stuff
 * ************************************************************** */

static void
ChangeManaged (RestraintWidget w)
{
  Widget child = NULL;
  int i;
  /* Find the child, if any. */
  for (i = 0; i < w->composite.num_children; i++)
    if (XtIsManaged (w->composite.children[i]))
      {
	child = w->composite.children[i];
	/* There can only be one! */
	break;
      }

  ON_DEBUG(printf ("Restraint::ChangeManaged: my size = %d x %d\n",
		   w->core.width, w->core.height));
  ON_DEBUG(printf ("  Num children = %d, child = 0x%p\n",
		   w->composite.num_children, (void*)child));

  /* If this is our first child, compute initial sizing. */
  /* Tried using !XtIsRealized (w) to do this stuff when were
     are being realized, but this doesn't handle the case of this
     widget being realized before it has a child.  This code will
     handle both cases correctly. */
  ON_DEBUG(printf ("  Checking for width calc\n    child = 0x%p, first = %d\n",
		   (void*)child, w->restraint.had_child));
  if (child != NULL && w->restraint.had_child == False)
    {
      compute_width (w, child);
      /* Q: Should this be reset when last child is deleted, so next new
	 child causes another width computation? 14:54 02/03/93 DJB */
      w->restraint.had_child = True;
    }

  /* Force child to take on my width. */
  if (child != NULL)
    {
      compute_height (w, child);
      ON_DEBUG(printf ("Restraint::ChangeManaged: sizing child to %d x %d\n",
		       w->core.width, w->core.height));
      XtResizeWidget (child, w->core.width, w->core.height, 0);
    }

  ON_DEBUG(printf ("----\n"));
  /* Do Motif bookkeeping. */
  _XmNavigChangeManaged ((Widget) w);
}


/* **************************************************************
 * GeometryManager
 * ************************************************************** */

/* X, Y, and BorderWidth changes not allowed.
     -- Reply with current values.
   Height changes allowed if Restraint parent agrees.
     T- Reply with requested value.
     F- Reply with current values.
   Width changes never allowed.
     -- Reply with restraint's width.
*/

/* Never respond No to requests.  Always respond Yes or Almost */
/* NOTE: If request and reply are the same, there could be problems! */

static XtGeometryResult
GeometryManager (Widget w, XtWidgetGeometry *request, XtWidgetGeometry *reply)
{
  Widget restraint = w->core.parent;
  Boolean almost = False, yes = False;

  /* Always give some kind of reply to each request. */
  reply->request_mode = request->request_mode;

  ON_DEBUG(printf ("Restraint: Child '%s' requesting:\n", XtName (w)));
  if (request->request_mode & CWX)
    {
      ON_DEBUG(printf ("  X: %d\n", request->x));
      reply->x = w->core.x;
    }
  if (request->request_mode & CWY)
    {
      ON_DEBUG(printf ("  Y: %d\n", request->y));
      reply->y = w->core.y;
    }
#ifdef DEBUG
  if (request->request_mode & CWWidth)
    {
      printf ("  Width: %d\n", request->width);
      /* Detect conditions under which a Form bug occurs.  If the Form
	 request a width increase and a height change, this widget refuses
	 the width change, and the bogus Form bails on the height change.
	 The result is a totally messed up Form layout.  Therefore, the
	 query editor must coerce the Form into never requesting a width
	 increase. */
      if (request->width > w->core.width)
	{
	  fprintf (stderr,
		   "Severe warning: query term requesting width increase\n");
	  fprintf (stderr, "  File a DTS bug immediately!!!\n");
	  if (request->request_mode & CWHeight)
	    abort();
	}
    }
  if (request->request_mode & CWHeight)
    printf ("  Height: %d\n", request->height);
#endif
  if (request->request_mode & CWBorderWidth)
    {
      ON_DEBUG(printf ("  Border: %d\n", request->border_width));
      reply->border_width = w->core.border_width;
    }

  if (request->request_mode & (CWBorderWidth | CWX | CWY))
    almost = True;

  /* Only allow width changes to the width of the restraint. */
  if (request->request_mode & CWWidth)
    {
      if (request->width == restraint->core.width)
        yes = True;
      else
        almost = True;
    }

  /* Allow any height changes */ 
  if (request->request_mode & CWHeight)
    yes = True;

  if (yes && !almost)
    {
      /* Change is acceptable.  See if parent will accommodate. */
      XtWidgetGeometry my_request, my_reply;
      XtGeometryResult result;

      /* Only need to check width and height below, because they are the
	 only requests that can generate a yes. */
      my_request.request_mode = 0;
      if (request->request_mode & CWHeight)
	{
	  my_request.request_mode |= CWHeight;
	  my_request.height = request->height;
	  ON_DEBUG(printf ("Restraint: requesting height of %d\n", my_request.height));
	}
      if (request->request_mode & CWWidth)
	{
	  my_request.request_mode |= CWWidth;
	  my_request.width = request->width;
	  ON_DEBUG(printf ("Restraint: requesting width of %d\n", my_request.width));
	}

      result = XtMakeGeometryRequest (restraint, &my_request, &my_reply);

      /* Stick with current values if it said No. */
      if (result == XtGeometryNo)
	{
	  ON_DEBUG(puts ("Restraint: Parent said no"));
	}
      /* Need to go with parent's width and height if it said Almost. */
      else if (result == XtGeometryAlmost)
	{
	  ON_DEBUG(puts ("Restraint: Parent said almost -->"));
	  ON_DEBUG(puts ("Restraint: responding XtGeometryAlmost:"));
	  /* reply->request_mode = 0; */
	  if (request->request_mode & CWHeight)
	    {
	      ON_DEBUG(printf ("  Height: %d\n", reply->height));
	      /* reply->request_mode |= CWHeight; */
	      /* Allow height change if parent didn't mind. */
	      if (my_reply.request_mode & CWHeight)
		reply->height = request->height;
	      else
		reply->height = w->core.height;
	    }
	  if (request->request_mode & CWWidth)
	    {
	      ON_DEBUG(printf ("  Width: %d\n", reply->width));
	      /* reply->request_mode |= CWWidth; */
	      /* Allowable width is always the restraint width. */
	      reply->width = restraint->core.width;
	    }
	}
      /* Everything's cool if it said Yes. */
      else if (result == XtGeometryYes)
	{
	  ON_DEBUG(puts ("Restraint: Parent said yes"));
	  ON_DEBUG(puts ("Restraint: responding XtGeometryYes."));
	  if (request->request_mode & CWHeight)
	    {
	      w->core.height = request->height;
	    }
	  if (request->request_mode & CWWidth)
	    {
	      /* Sanity Check: Yes on width is only allowed if this is true. */
	      /* This means that width changes are essentially not allowed. */
	      if (w->core.width != restraint->core.width)
		abort();
	    }
	}

      /* If parent said No, stick around and return Almost later on. */
      if (result != XtGeometryNo)
	return (result);
    }

  /* Not a simple Yes, so it'll be an Almost reply. */
  /* Either 1) widget tried to change some things I don't like,
         or 2) my parent didn't allow the changes requested. */
  if (yes || almost)
    {
      ON_DEBUG(puts ("Restraint: responding XtGeometryAlmost:"));
      if (request->request_mode & CWX)
	ON_DEBUG(printf ("  X: %d\n", reply->x));
      if (request->request_mode & CWY)
	ON_DEBUG(printf ("  Y: %d\n", reply->y));
      if (request->request_mode & CWBorderWidth)
	ON_DEBUG(printf ("  BorderWidth: %d\n", reply->border_width));
      if (request->request_mode & CWWidth)
	{
	  ON_DEBUG(printf ("  Width: %d\n", w->core.width));
	  reply->request_mode |= CWWidth;
	  /* Always reply with restraint's width for child width. */
	  reply->width = restraint->core.width;
	}
      if (request->request_mode & CWHeight)
	{
	  /* Check with parent before allowing a height change. */
	  XtWidgetGeometry my_request, my_reply;
	  XtGeometryResult result;

	  ON_DEBUG(printf ("  Checking with parent for Almost height value:\n"));

	  my_request.request_mode = CWHeight & XtCWQueryOnly;
	  my_request.height = request->height;
	  ON_DEBUG(printf ("    Asking for height = %d\n", my_request.height));
	  result = XtMakeGeometryRequest (restraint, &my_request, &my_reply);

	  if (result == XtGeometryNo)               /* unable to comply */
	    {
	      ON_DEBUG(printf ("    Parent said No.\n"));
	      reply->height = w->core.height;
	  }
	  else if (result == XtGeometryAlmost)      /* partial compliance */
	    {
	      ON_DEBUG(printf ("    Parent said Almost.\n"));
	      reply->height = my_reply.height;
	    }
	  else if (result == XtGeometryYes)         /* total compliance */
	    {
	      ON_DEBUG(printf ("    Parent said Yes.\n"));
	      reply->height = request->height;
	    }

	  ON_DEBUG(printf ("  Height: %d\n", reply->height));
	  reply->request_mode |= CWHeight;
	}
      
      return (XtGeometryAlmost);
    }

  /* If we got here something is wrong, so bomb out. */
  abort();
}


/* **********************************************************************
 * Resize - resize child as well
 * **********************************************************************/

static void
Resize (RestraintWidget w)
{
  Widget child;

  /* This widget should only have one child. */
  if (w->composite.num_children == 0)
    return;

  child = w->composite.children[0];

  ON_DEBUG(printf ("Restraint::Resize resizing child to %d x %d\n",
		   w->core.width, w->core.height));

  XtResizeWidget(child,
		 w->core.width - child->core.border_width,
		 w->core.height - child->core.border_width,
		 child->core.border_width);
}
