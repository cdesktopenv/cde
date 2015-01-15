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
/* $XConsortium: WorkArea.c /main/6 1995/11/20 09:58:49 rswiston $ */
#include "WorkAreaP.h"
#include <Xm/PrimitiveP.h>
#include <Xm/ScrolledW.h>
#include "Icon.h"
#include <Xm/RepType.h>

/* Resource Strings */

const char gui_workarea_strings[] =
{
'i','s','L','i','s','t',0,
'a','t','t','a','c','h','m','e','n','t',0,
'A','t','t','a','c','h','m','e','n','t',0,
'A','t','t','a','c','h','m','e','n','t',0,
'i','s','W','o','r','k','A','r','e','a',0,
'a','u','t','o','R','e','s','i','z','e','W','i','d','t','h',0,
'i','s','O','p','e','n','e','d',0,
'i','s','T','r','a','n','s','i','e','n','t',0,
'l','i','n','e','T','h','i','c','k','n','e','s','s',0,
'L','i','n','e','T','h','i','c','k','n','e','s','s',0,
'l','i','n','e','O','f','f','s','e','t',0,
'L','i','n','e','O','f','f','s','e','t',0,
'n','o','d','e','L','i','n','e','L','e','n','g','t','h',0,
'N','o','d','e','L','i','n','e','L','e','n','g','t','h',0,
'n','u','m','b','e','r','S','u','b','N','o','d','e','s',0,
'N','u','m','b','e','r','S','u','b','N','o','d','e','s',0,
'n','u','m','b','e','r','C','o','l','u','m','n','s',0,
'N','u','m','b','e','r','C','o','l','u','m','n','s',0,
's','u','b','N','o','d','e','s',0,
'S','u','b','N','o','d','e','s',0,
'h','o','r','i','z','o','n','t','a','l','S','p','a','c','e',0,
'v','e','r','t','i','c','a','l','S','p','a','c','e',0,
'S','p','a','c','e',0,
's','u','p','e','r','N','o','d','e',0,
'S','u','p','e','r','N','o','d','e',0,
};

#define GuiStackAlloc(size, stack_cache_array)    \
   ((size) <= sizeof(stack_cache_array)        \
   ?  (XtPointer)(stack_cache_array)          \
   :  XtMalloc((unsigned)(size)))

#define GuiStackFree(pointer, stack_cache_array) \
   if ((pointer) != ((XtPointer)(stack_cache_array))) XtFree(pointer);

#define DEFAULT_HEIGHT 100
#define DEFAULT_WIDTH 100
#define superclass (&xmManagerClassRec)

static void ClassInitialize(void);
static void Initialize(WorkAreaWidget, WorkAreaWidget);
static void Realize(Widget w, XtValueMask *, XSetWindowAttributes *);
static void ConstraintInitialize(Widget, Widget);
static Boolean SetValues(WorkAreaWidget, WorkAreaWidget, WorkAreaWidget);
static Boolean ConstraintSetValues(Widget, Widget, Widget);
static void InsertNewNode(Widget, Widget);
static void DeleteNode(Widget, Widget);
static void ConstraintDestroy(Widget);
static XtGeometryResult GeometryManager(Widget, XtWidgetGeometry *,
                                        XtWidgetGeometry *);
static void ChangeManaged(WorkAreaWidget);
static void Redisplay(WorkAreaWidget, XEvent *, Region);
static void CreateGC(WorkAreaWidget);
static void NewLayout(WorkAreaWidget);
static void ComputePositions(WorkAreaWidget, Widget, Dimension *, Dimension *,
                            Dimension, Dimension, int *, int *, int, int *);
static void SetPositions(WorkAreaWidget, Widget);
static int OpenedSubnodes(Widget);
static void Resize(WorkAreaWidget);
static void MakeChildVisible(Widget, XtPointer, XtPointer);
static void ResizeTimeOut(Widget w, XtIntervalId *id);
static void ResizeSW(Widget, XtPointer, XtPointer);
static void DestroyParent(Widget, XtPointer, XtPointer);

static XmRepTypeId GuiRID_CHILDREN_ATTACHMENT;
static char *ChildrenAttachment[] =
{
   "attach_top", "attach_bottom", "attach_left", "attach_right"
};


/*  WorkArea Resources */

static XtResource resources[] =
{
   { GuiNlineOffset, GuiCLineOffset, XtRDimension, sizeof(Dimension),
     XtOffsetOf(struct _WorkAreaRec, workArea.line_offset),
     XtRImmediate, (XtPointer) 25
   },
   { GuiNnodeLineLength, GuiCNodeLineLength, XtRDimension, sizeof(Dimension),
     XtOffsetOf(struct _WorkAreaRec, workArea.node_line_length),
     XtRImmediate, (XtPointer) 15
   },
   { GuiNlineThickness, GuiCLineThickness, XtRDimension, sizeof(Dimension),
     XtOffsetOf(struct _WorkAreaRec, workArea.line_thickness),
     XtRImmediate, (XtPointer) 2
   },
   { GuiNhorizontalSpace, GuiCSpace, XtRDimension, sizeof(Dimension),
     XtOffsetOf(struct _WorkAreaRec, workArea.horizontal_spacing), 
     XtRImmediate, (XtPointer) 5
   },
   { GuiNverticalSpace, GuiCSpace, XtRDimension, sizeof(Dimension),
     XtOffsetOf(struct _WorkAreaRec, workArea.vertical_spacing), 
     XtRImmediate, (XtPointer) 5
   },
   { XmNalignment, XmCAlignment, XmRAlignment, sizeof(unsigned char),
     XtOffsetOf(struct _WorkAreaRec, workArea.alignment),
     XtRImmediate, (XtPointer) XmALIGNMENT_CENTER
   },
   { XmNentryVerticalAlignment, XmCVerticalAlignment, XmRVerticalAlignment,
     sizeof(unsigned char),
     XtOffsetOf(struct _WorkAreaRec, workArea.vertical_alignment),
     XtRImmediate, (XtPointer) XmALIGNMENT_CENTER
   },
   { XmNpacking, XmCPacking, XmRPacking, sizeof(unsigned char), 
     XtOffsetOf(struct _WorkAreaRec, workArea.packing),
     XtRImmediate, (XtPointer) XmPACK_COLUMN
   },
   { GuiNautoResizeWidth, XtCBoolean, XtRBoolean, sizeof(Boolean),
     XtOffsetOf(struct _WorkAreaRec, workArea.auto_resize_width),
     XtRImmediate, (XtPointer) TRUE
   },
   { GuiNisList, XtCBoolean, XtRBoolean, sizeof(Boolean),
     XtOffsetOf(struct _WorkAreaRec, workArea.is_list),
     XtRImmediate, (XtPointer) FALSE
   }
};


/*  WorkArea Children Resources */

static XtResource workAreaConstraintResources[] =
{
   { GuiNsuperNode, GuiCSuperNode, XtRPointer, sizeof(Widget),
     XtOffset(WorkAreaConstraints, workArea.super_node), XtRPointer, NULL
   },
   { XmNorientation, XmCOrientation, XmROrientation, sizeof(unsigned char),
     XtOffset(WorkAreaConstraints, workArea.orientation),
     XtRImmediate, (XtPointer) XmVERTICAL
   },
   { GuiNisTransient, XtCBoolean, XtRBoolean, sizeof(Boolean),
     XtOffset(WorkAreaConstraints, workArea.is_transient),
     XtRImmediate, (XtPointer) FALSE
   },
   { GuiNisWorkArea, XtCBoolean, XtRBoolean, sizeof(Boolean),
     XtOffset(WorkAreaConstraints, workArea.is_workArea),
     XtRImmediate, (XtPointer) FALSE
   },
   { GuiNisOpened, XtCBoolean, XtRBoolean, sizeof(Boolean),
     XtOffset(WorkAreaConstraints, workArea.is_opened),
     XtRImmediate, (XtPointer) FALSE
   },
   { GuiNsubNodes, GuiCSubNodes, XtRPointer, sizeof(WidgetList),
     XtOffset(WorkAreaConstraints, workArea.sub_nodes), XtRPointer, NULL
   },
   { GuiNnumberSubNodes, GuiCNumberSubNodes, XtRInt, sizeof(int),
     XtOffset(WorkAreaConstraints, workArea.n_sub_nodes),
     XtRImmediate, (XtPointer) 0
   },
   { GuiNnumberColumns, GuiCNumberColumns, XtRInt, sizeof(int),
     XtOffset(WorkAreaConstraints, workArea.n_columns),
     XtRImmediate, (XtPointer) 0
   },
   { GuiNattachment, GuiCAttachment, GuiRAttachment, sizeof(unsigned char),
     XtOffset(WorkAreaConstraints, workArea.attachment),
     XtRImmediate, (XtPointer) GuiATTACH_LEFT
   }
};


WorkAreaClassRec workAreaClassRec = 
{
  {
   /* core_class fields  */
   (WidgetClass) superclass,              /* superclass              */
   "WorkArea",                            /* class_name              */
   sizeof(WorkAreaRec),                   /* widget_size             */
   ClassInitialize,                       /* class_init              */
   NULL,                                  /* class_part_init         */
   FALSE,                                 /* class_inited            */
   (XtInitProc)Initialize,                /* initialize              */
   NULL,                                  /* initialize_hook         */
   Realize,                               /* realize                 */
   NULL,                                  /* actions                 */
   0,                                     /* num_actions             */
   resources,                             /* resources               */
   XtNumber(resources),                   /* num_resources           */
   NULLQUARK,                             /* xrm_class               */
   TRUE,                                  /* compress_motion         */
   XtExposeCompressMaximal,               /* compress_exposure       */
   TRUE,                                  /* compress_enterleave     */
   FALSE,                                 /* visible_interest        */
   NULL,                                  /* destroy                 */
   (XtWidgetProc)Resize,                  /* resize                  */
   (XtExposeProc)Redisplay,               /* expose                  */
   (XtSetValuesFunc)SetValues,            /* set_values              */
   NULL,                                  /* set_values_hook         */
   XtInheritSetValuesAlmost,              /* set_values_almost       */
   NULL,                                  /* get_values_hook         */
   XtInheritAcceptFocus,                  /* accept_focus            */
   XtVersion,                             /* version                 */
   NULL,                                  /* callback_private        */
   XtInheritTranslations,                 /* tm_table                */
   XtInheritQueryGeometry,                /* query_geometry          */
   NULL,                                  /* display_accelerator     */
   NULL,                                  /* extension               */
  },
  {
   /* composite_class fields */
   GeometryManager,                       /* geometry_manager        */
   (XtWidgetProc)ChangeManaged,           /* change_managed          */
   XtInheritInsertChild,                  /* insert_child            */
   XtInheritDeleteChild,                  /* delete_child            */
   NULL,                                  /* extension               */
  },
  { 
   /* constraint_class fields */
   workAreaConstraintResources,           /* subresources            */
   XtNumber(workAreaConstraintResources), /* subresource_count       */
   sizeof(WorkAreaConstraintsRec),        /* constraint_size         */
   (XtInitProc)ConstraintInitialize,      /* initialize              */
   ConstraintDestroy,                     /* destroy                 */
   (XtSetValuesFunc)ConstraintSetValues,  /* set_values              */
   NULL,                                  /* extension               */
  },
  {
   /* manager_class fields */
   XtInheritTranslations,                 /* default translations    */
   NULL,                                  /* syn_resources           */
   0,                                     /* num_syn_resources       */
   NULL,                                  /* syn_cont_resources      */
   0,                                     /* num_syn_cont_resources  */
   NULL,                                  /* parent_process          */
   NULL,                                  /* extension               */
  },
  {
   /* WorkArea class fields */
   0,                                     /* ignore                  */
  }
};

WidgetClass workAreaWidgetClass = (WidgetClass) &workAreaClassRec;

/******************************************************************************
 *
 * Public Functions
 *
 ******************************************************************************/

/*
 *   NAME:    GuiCreateWorkArea
 *   FUNCTION: 
 *   RETURNS:  
 */
Widget
GuiCreateWorkArea(
   Widget parent, 
   char *name, 
   ArgList arglist, 
   Cardinal argcount)
{
   return XtCreateWidget(name, workAreaWidgetClass, parent, arglist, argcount);
}

/*
 *   NAME:    GuiCreateScrolledWorkArea
 *   FUNCTION: 
 *   RETURNS:  
 */
Widget
GuiCreateScrolledWorkArea(
   Widget parent, 
   char *name, 
   ArgList arglist, 
   Cardinal argcount)
{
   Widget swindow;
   Widget sworkArea;
   Arg args_cache[30];
   ArgList merged_args;
   int n;
   char s_cache[30];
   char *s;
   Cardinal s_size;
   Cardinal arg_size = argcount + 4;

   s_size = ((name) ? strlen(name) : 0) + 3;

   s = (char *) GuiStackAlloc(s_size, s_cache);  /* Name + NULL + "SW" */
   if (name)
    {
      strcpy(s, name);
      strcat(s, "SW");
    }
   else
      strcpy(s, "SW");

   /*
    * merge the application arglist with the required preset arglist, for
    * creating the scrolled window portion of the scroll workArea.
    */
   merged_args = (ArgList)GuiStackAlloc(arg_size, args_cache);
   for (n=0; n < argcount; n++)
    {
      merged_args[n].name = arglist[n].name;
      merged_args[n].value = arglist[n].value;
    }
   XtSetArg(merged_args[n], XmNscrollingPolicy, (XtArgVal) XmAUTOMATIC);
   n++;
   XtSetArg(merged_args[n], XmNscrollBarDisplayPolicy, (XtArgVal) XmAS_NEEDED);
   n++;

   swindow = XtCreateManagedWidget(s, xmScrolledWindowWidgetClass, parent,
                                   merged_args, n);
   XtAddCallback(swindow, XmNtraverseObscuredCallback, MakeChildVisible, NULL);

   GuiStackFree(s, s_cache);
   GuiStackFree((char *)merged_args, args_cache);

   /* Create WorkArea widget.  */
   sworkArea = XtCreateWidget(name, workAreaWidgetClass, swindow, arglist,
                              argcount);

   /* Add callback to destroy ScrolledWindow parent. */
   XtAddCallback(sworkArea, XmNdestroyCallback, DestroyParent, NULL);
   /* Add Resize and destroy callbacks to clip widget */
   XtAddCallback(XtParent(sworkArea), XmNresizeCallback, ResizeSW, sworkArea);

   /* Return WorkArea.*/
   return sworkArea;
}

/*
 *   NAME:    GuiWorkAreaReorderChildren
 *   FUNCTION: 
 *   RETURNS:  
 */
void
GuiWorkAreaReorderChildren(
   Widget parent_subnode, 
   WidgetList subnode_list, 
   int n_subnodes, 
   Widget position)
{
   WorkAreaConstraints workArea_const;

   if (GuiIsWorkArea(parent_subnode))
      parent_subnode = ((WorkAreaWidget)parent_subnode)->workArea.workArea_root;
   if (GuiIsWorkArea(XtParent(parent_subnode)))
    {
      workArea_const = WORKAREA_CONSTRAINT(parent_subnode);
      XtFree((char *)workArea_const->workArea.sub_nodes);
      workArea_const->workArea.sub_nodes =
         (WidgetList) XtMalloc(sizeof(Widget) * n_subnodes);
      memcpy(workArea_const->workArea.sub_nodes, subnode_list,
         sizeof(Widget) * n_subnodes);
      workArea_const->workArea.n_sub_nodes = n_subnodes;
      NewLayout((WorkAreaWidget)XtParent(parent_subnode));
      Redisplay((WorkAreaWidget)XtParent(parent_subnode), NULL, NULL);
    }
   else
    {
      XmeWarning(parent_subnode,
         "Cannot ReorderChildren of parent widget of non-subclass of WorkArea");
    }
}

/*
 *   NAME:    GuiWorkAreaDisableRedisplay
 *   FUNCTION: 
 *   RETURNS:  
 */
void
GuiWorkAreaDisableRedisplay(
   Widget w)
{
   WorkAreaWidget workArea = (WorkAreaWidget) w;

   if (GuiIsWorkArea(w))
      workArea->workArea.delay_layout = TRUE;
   else
      XmeWarning(w, "Cannot set DisableRedisplay of non-subclass of WorkArea");
}

/*
 *   NAME:    GuiWorkAreaEnableRedisplay
 *   FUNCTION: 
 *   RETURNS:  
 */
void
GuiWorkAreaEnableRedisplay(
   Widget w)
{
   WorkAreaWidget workArea = (WorkAreaWidget) w;

   if (GuiIsWorkArea(w))
    {
      workArea->workArea.delay_layout = FALSE;
      NewLayout(workArea);
      Redisplay(workArea, NULL, NULL);
    }
   else
      XmeWarning(w, "Cannot set EnableRedisplay of non-subclass of WorkArea");
}

/******************************************************************************
 *
 * Class methods
 *
 ******************************************************************************/

/*
 *   NAME:    ClassInitialize
 *   FUNCTION: 
 *   RETURNS:  
 */
static void
ClassInitialize(
   void)
{
   GuiRID_CHILDREN_ATTACHMENT = XmRepTypeRegister(GuiRAttachment, 
      ChildrenAttachment, NULL, sizeof(ChildrenAttachment) / sizeof(char *));
}

/*
 *   NAME:    Initialize
 *   FUNCTION: 
 *   RETURNS:  
 */
static void
Initialize(
   WorkAreaWidget request,
   WorkAreaWidget new)
{
   Arg             wargs[3];
   WorkAreaConstraints workArea_const;

   /* Make sure the widget's width and height are greater than zero.  */
   if (request->core.width <= 0)
      new->core.width = DEFAULT_WIDTH;
   if (request->core.height <= 0)
      new->core.height = DEFAULT_HEIGHT;
   new->workArea.old_width = new->core.width;
   new->workArea.old_height = new->core.height;

   /* Postpone geometry calculations until the workArea has been realized */
   GuiWorkAreaDisableRedisplay((Widget)new);

   if (!XmRepTypeValidValue(XmRepTypeGetId(XmRAlignment),
                            new->workArea.alignment, (Widget)new))
      new->workArea.alignment = XmALIGNMENT_CENTER;
   if (!XmRepTypeValidValue(XmRepTypeGetId(XmRVerticalAlignment),
                            new->workArea.vertical_alignment, (Widget) new))
      new->workArea.alignment = XmALIGNMENT_CENTER;
   if (!XmRepTypeValidValue(XmRepTypeGetId(XmRPacking), new->workArea.packing,
                            (Widget)new))
      new->workArea.packing = XmPACK_COLUMN;

   /* Create a graphics context for the connecting lines.  */
   CreateGC(new);

   /* Create the hidden root widget.  */
   XtSetArg(wargs[0], XmNwidth, 1);
   XtSetArg(wargs[1], XmNheight, 1);
   XtSetArg(wargs[2], XmNuserData, NULL);
   new->workArea.workArea_root =  (Widget) NULL;
   new->workArea.workArea_root = XtCreateWidget("root", xmPrimitiveWidgetClass,
                                                (Widget)new, wargs, 3);
}

/*
 *   NAME:    Realize
 *   FUNCTION: 
 *   RETURNS:  
 */
static void
Realize(
   Widget w, 
   XtValueMask *valueMask, 
   XSetWindowAttributes *attr)
{
   Boolean sw_child;
   Widget sw;
   Widget vbar;
   Dimension width, vbar_width, shadow_thickness, highlight_thickness;
   Dimension spacing, marginHeight, marginWidth;
   Arg args[10];
   int n;

   /*
    * Check to see if the workArea is a child of a scrolled window.
    * If it is and the workArea's width is less than the scrolled window's
    * width, resize the workArea's width to the scroll window.
    */
   sw = XtParent(w);
   if (((WorkAreaWidget)w)->workArea.auto_resize_width)
    {
      if (XmIsScrolledWindow(sw))
         sw_child = TRUE;
      else
       {
         /* Check the parent of the parent of the workArea, because the
            workArea could be a child of the scrolled window's clip widget */
         if ((sw = XtParent(sw)) != NULL)
            if (XmIsScrolledWindow(sw))
               sw_child = TRUE;
        }
    }
   else
      sw_child = FALSE;

   if (sw_child == TRUE)
    {
      n = 0;
      XtSetArg(args[n], XmNverticalScrollBar, &vbar); n++;
      XtSetArg(args[n], XmNwidth, &width); n++;
      XtSetArg(args[n], XmNspacing, &spacing); n++;
      XtSetArg(args[n], XmNshadowThickness, &shadow_thickness); n++;
      XtSetArg(args[n], XmNscrolledWindowMarginHeight, &marginHeight); n++;
      XtSetArg(args[n], XmNscrolledWindowMarginWidth, &marginWidth); n++;
      XtGetValues(sw, args, n);

      if (vbar != NULL)
       {
         n = 0;
         XtSetArg(args[n], XmNwidth, &vbar_width); n++;
         XtSetArg(args[n], XmNhighlightThickness, &highlight_thickness); n++;
         XtGetValues(vbar, args, n);
       }
      else
         vbar_width = highlight_thickness = 0;

      w->core.width = width - vbar_width - 
                      (2 * (highlight_thickness + shadow_thickness + spacing +
                      marginHeight + marginWidth));
    }

   /* Call the superclass's realize method to realize the workArea */
   (*superclass->core_class.realize)(w, valueMask, attr);

   /* Now perform the geometry calculations */
   GuiWorkAreaEnableRedisplay(w);
}

/*
 *   NAME:    ConstraintInitialize
 *   FUNCTION: 
 *   RETURNS:  
 */
static void
ConstraintInitialize(
   Widget request,
   Widget new)
{
   WorkAreaConstraints workArea_const = WORKAREA_CONSTRAINT(new);
   WorkAreaWidget workArea = (WorkAreaWidget) new->core.parent;

   /* Initialize the widget to have no sub-nodes. */
   workArea_const->workArea.n_sub_nodes = 0;
   workArea_const->workArea.sub_nodes = (WidgetList) XtMalloc(sizeof(Widget));
   workArea_const->workArea.x = workArea_const->workArea.y = 0;
   if (!XmRepTypeValidValue(GuiRID_CHILDREN_ATTACHMENT,
                            workArea_const->workArea.attachment, (Widget) new))
      workArea_const->workArea.attachment = GuiATTACH_LEFT;
   if (!XmRepTypeValidValue(XmRepTypeGetId(XmROrientation),
                            workArea_const->workArea.orientation, (Widget) new))
      workArea_const->workArea.orientation = XmVERTICAL;
   /*
    * If this widget has a super-node, add it to that 
    * widget' sub-nodes list. Otherwise make it a sub-node of 
    * the workArea_root widget.
    */
   if (workArea_const->workArea.super_node)
      InsertNewNode(workArea_const->workArea.super_node, new);
   else if (workArea->workArea.workArea_root)
      InsertNewNode(workArea->workArea.workArea_root, new);
}

/*
 *   NAME:    SetValues
 *   FUNCTION: 
 *   RETURNS:  
 */
static Boolean
SetValues(
   WorkAreaWidget current,
   WorkAreaWidget request,
   WorkAreaWidget new)
{
   int redraw = FALSE;

   /* If the fg color has changed, redo the GC's and indicate a redraw.  */
   if (new->manager.foreground != current->manager.foreground ||
       new->core.background_pixel != current->core.background_pixel ||
       new->workArea.line_thickness != current->workArea.line_thickness)
    {
      CreateGC(new);
      redraw = TRUE;
    }

   if (new->workArea.alignment != current->workArea.alignment)
    {
      if (!XmRepTypeValidValue(XmRepTypeGetId(XmRAlignment),
                               new->workArea.alignment, (Widget) new))
         new->workArea.alignment = current->workArea.alignment;
    }

   if (new->workArea.packing != current->workArea.packing)
    {
      if (!XmRepTypeValidValue(XmRepTypeGetId(XmRPacking),
                               new->workArea.packing, (Widget) new))
         new->workArea.packing = current->workArea.packing;
    }

   if (new->workArea.vertical_alignment != current->workArea.vertical_alignment)
    {
      if (!XmRepTypeValidValue(XmRepTypeGetId(XmRVerticalAlignment),
                               new->workArea.vertical_alignment, (Widget) new))
         new->workArea.vertical_alignment =current->workArea.vertical_alignment;
    }

   /* If any display characteristics have changed, recalculate the layout.  */
   if (new->workArea.vertical_spacing != current->workArea.vertical_spacing ||
      new->workArea.horizontal_spacing != current->workArea.horizontal_spacing||
      new->workArea.line_offset != current->workArea.line_offset ||
      new->workArea.node_line_length != current->workArea.node_line_length ||
      new->workArea.vertical_alignment != current->workArea.vertical_alignment||
      new->workArea.packing != current->workArea.packing ||
      new->workArea.alignment != current->workArea.alignment)
    {
      NewLayout(new);
      redraw = TRUE;
    }

   return redraw;
}

/*
 *   NAME:    ConstraintSetValues
 *   FUNCTION: 
 *   RETURNS:  
 */
static Boolean
ConstraintSetValues(
   Widget current,
   Widget request,
   Widget new)
{
   WorkAreaConstraints newconst = WORKAREA_CONSTRAINT(new);
   WorkAreaConstraints current_const = WORKAREA_CONSTRAINT(current);
   WorkAreaWidget workArea = (WorkAreaWidget) new->core.parent;
   Boolean CallNewLayout = FALSE;

   /* Can not set sub_nodes or n_sub_nodes */
   /* sub_nodes can be change with the GuiWorkAreaReorderChildren function. */
   if (current_const->workArea.sub_nodes != newconst->workArea.sub_nodes)
      newconst->workArea.sub_nodes = current_const->workArea.sub_nodes;
   if (current_const->workArea.n_sub_nodes != newconst->workArea.n_sub_nodes)
      newconst->workArea.n_sub_nodes = current_const->workArea.n_sub_nodes;

   if (newconst->workArea.is_workArea == TRUE &&
       current_const->workArea.is_opened != newconst->workArea.is_opened)
      CallNewLayout = TRUE;

   if (current_const->workArea.attachment != newconst->workArea.attachment)
    {
      if (!XmRepTypeValidValue(GuiRID_CHILDREN_ATTACHMENT,
                               newconst->workArea.attachment, (Widget) new))
         newconst->workArea.attachment = current_const->workArea.attachment;
      else
         CallNewLayout = TRUE;
    }

   if (current_const->workArea.orientation != newconst->workArea.orientation)
    {
      if (!XmRepTypeValidValue(XmRepTypeGetId(XmROrientation),
                               newconst->workArea.orientation, (Widget) new))
         newconst->workArea.orientation = current_const->workArea.orientation;
      else
         CallNewLayout = TRUE;
    }

   if (current_const->workArea.is_workArea != newconst->workArea.is_workArea)
      CallNewLayout = TRUE;
   if (current_const->workArea.is_transient == TRUE &&
       current_const->workArea.is_transient != newconst->workArea.is_transient)
      CallNewLayout = TRUE;
   if (current_const->workArea.n_columns != newconst->workArea.n_columns &&
       current_const->workArea.orientation == XmHORIZONTAL)
      CallNewLayout = TRUE;
   if (current_const->workArea.super_node != newconst->workArea.super_node)
    {
      if (current_const->workArea.super_node)
         DeleteNode(current_const->workArea.super_node, new);
      if (newconst->workArea.super_node)
         InsertNewNode(newconst->workArea.super_node, new);
      else
         InsertNewNode(workArea->workArea.workArea_root, new);
      CallNewLayout = TRUE;
    }
   if (CallNewLayout == TRUE)
      NewLayout(workArea);

   return CallNewLayout;
}

/*
 *   NAME:    InsertNewNode
 *   FUNCTION: 
 *   RETURNS:  
 */
static void
InsertNewNode(
   Widget super_node,
   Widget node)
{
   WorkAreaConstraints super_const = WORKAREA_CONSTRAINT(super_node);
   WorkAreaConstraints node_const = WORKAREA_CONSTRAINT(node);
   int index = super_const->workArea.n_sub_nodes;

   node_const->workArea.super_node = super_node;
   super_const->workArea.sub_nodes = 
      (WidgetList) XtRealloc((char *)super_const->workArea.sub_nodes, 
      (index + 1) * sizeof(Widget));
   super_const->workArea.sub_nodes[index] = node;
   super_const->workArea.n_sub_nodes++;
}

/*
 *   NAME:    DeleteNode
 *   FUNCTION: 
 *   RETURNS:  
 */
static void
DeleteNode(
   Widget super_node,
   Widget node)
{
   WorkAreaConstraints node_const = WORKAREA_CONSTRAINT(node);
   WorkAreaConstraints super_const;
   int pos, i;

   /* Make sure the super_node exists.  */
   if (!super_node) return;

   super_const = WORKAREA_CONSTRAINT(super_node);
   /* Find the sub_node on its super_node's list.  */
   for (pos = 0; pos < super_const->workArea.n_sub_nodes; pos++)
      if (super_const->workArea.sub_nodes[pos] == node)
         break;

   if (pos == super_const->workArea.n_sub_nodes)
      return;

   /* Decrement the number of sub_nodes */
   super_const->workArea.n_sub_nodes--;

   /* Fill in the gap left by the sub_node.  */
   for (i = pos; i < super_const->workArea.n_sub_nodes; i++)
      super_const->workArea.sub_nodes[i] = super_const->workArea.sub_nodes[i+1];

   super_const->workArea.sub_nodes[super_const->workArea.n_sub_nodes]=0;
}

/*
 *   NAME:    ConstraintDestroy
 *   FUNCTION: 
 *   RETURNS:  
 */
static void
ConstraintDestroy(
   Widget w)
{
   WorkAreaConstraints workArea_const = WORKAREA_CONSTRAINT(w);
   int i;

   if (w->core.parent->core.being_destroyed)
    {
     XtFree((char *)workArea_const->workArea.sub_nodes);
     return;
    }

   /* 
    * Remove the widget from its parent's sub-nodes list and
    * make all this widget's sub-nodes sub-nodes of the parent.
    */
   if (workArea_const->workArea.super_node)
    {
      DeleteNode(workArea_const->workArea.super_node, w);
      for (i=0;i< workArea_const->workArea.n_sub_nodes; i++)
         InsertNewNode(workArea_const->workArea.super_node, 
            workArea_const->workArea.sub_nodes[i]);
    }
   XtFree((char *)workArea_const->workArea.sub_nodes);
   NewLayout((WorkAreaWidget)w->core.parent);
}

/*
 *   NAME:    GeometryManager
 *   FUNCTION: 
 *   RETURNS:  
 */
static XtGeometryResult
GeometryManager(
   Widget w,
   XtWidgetGeometry *request,
   XtWidgetGeometry *reply)
{
   WorkAreaConstraints workArea_const = WORKAREA_CONSTRAINT(w);
   WorkAreaWidget workArea = (WorkAreaWidget) w->core.parent;

   /* No position changes are allowed when is_transient is set to FALSE.  */
   if ((request->request_mode & CWX && request->x!=w->core.x) ||
       (request->request_mode & CWY && request->y!=w->core.y))
    {
     if (workArea_const->workArea.is_transient)
     {
         w->core.x = request->x;
         w->core.y = request->y;
     }
     else
         return XtGeometryNo;
    }
   /* Allow all other resize requests. */
   if (request->request_mode & CWWidth)
      w->core.width = request->width;
   if (request->request_mode & CWHeight)
      w->core.height = request->height;
   if (request->request_mode & CWBorderWidth)
      w->core.border_width = request->border_width;
   /*  Compute the new layout based on the new widget sizes */
   NewLayout(workArea);
   return XtGeometryYes;
}

/*
 *   NAME:    ChangeManaged
 *   FUNCTION: 
 *   RETURNS:  
 */
static void
ChangeManaged(
   WorkAreaWidget workArea)
{
   NewLayout(workArea);
}

/*
 *   NAME:    Redisplay
 *   FUNCTION: 
 *   RETURNS:  
 */
static void
Redisplay(
   WorkAreaWidget w, 
   XEvent *event, 
   Region region)
{
   int i, j;
   WorkAreaConstraints workArea_const;
   Widget child;

   /*
    * If the WorkArea widget is realized, visible, and we're not delaying
    * redisplay, visit each managed child.
    */
   if (!w->workArea.is_list && XtIsRealized((Widget)w) &&
     w->core.visible && w->workArea.delay_layout == FALSE)
    {
      for (i = 0; i < w->composite.num_children; i++)
       {
         child = w->composite.children[i];
         workArea_const = WORKAREA_CONSTRAINT(child);
         /*
          * Draw a line between the right edge of each widget
          * and the left edge of each of its sub_nodes. Don't
          * draw lines from the fake workArea_root.
          */
         if (child != w->workArea.workArea_root &&
             workArea_const->workArea.n_sub_nodes > 0)
          {
            if (XtIsManaged(child))
             {
               int x0, y0, x1, y1, y_max, x_max;
               x0 = child->core.x + w->workArea.line_offset;
               y0 = child->core.y + child->core.height;
               x_max = y_max = -1;

               for (j = 0; j < workArea_const->workArea.n_sub_nodes; j++)
                {
                  if (!XtIsManaged(workArea_const->workArea.sub_nodes[j]))
                     continue;

                  x1 = workArea_const->workArea.sub_nodes[j]->core.x;
                  if (WORKAREA_CONSTRAINT(workArea_const->workArea.sub_nodes[j])->workArea.is_opened ||
                     !WORKAREA_CONSTRAINT(workArea_const->workArea.sub_nodes[j])->workArea.is_workArea)
                   {
                     y1 = workArea_const->workArea.sub_nodes[j]->core.y + 
                          workArea_const->workArea.sub_nodes[j]->core.height/2;
                     XDrawLine(XtDisplay(w), XtWindow(w), w->workArea.gc,
                               x0, y1, x1, y1);
                     if (y1 > y_max)
                        y_max = y1;
                    }
                  else
                   {
                     if (x1 > x_max)
                        x_max = x1;
                    }
                 }
               if (y_max != -1)
                {
                  XDrawLine(XtDisplay(w), XtWindow(w), w->workArea.gc,
                            x0, y0, x0, y_max);
                 }
              }
           }
        }
    }
}

/*
 *   NAME:    CreateGC
 *   FUNCTION: 
 *   RETURNS:  
 */
static void
CreateGC(
   WorkAreaWidget w)
{
   XGCValues values;
   XtGCMask valueMask;

   valueMask = GCForeground | GCBackground | GCLineWidth;
   values.foreground = w->manager.foreground;
   values.line_width = w->workArea.line_thickness;
   values.background = w->core.background_pixel;
   w->workArea.gc = XtGetGC((Widget)w, valueMask, &values);
}

/*
 *   NAME:    NewLayout
 *   FUNCTION: 
 *   RETURNS:  
 */
static void
NewLayout(
   WorkAreaWidget workArea)
{
   WorkAreaConstraints workArea_const;
   Dimension node_width, node_height;
   int cur_x, cur_y;
   Dimension new_width, new_height;
   Dimension replyWidth, replyHeight;
   int i, j;
   int seen_one = 0;
   XtGeometryResult result;

   if (workArea->workArea.delay_layout == TRUE)
      return;

   /* Initialize variables. */

   new_width = workArea->core.width;
   cur_x = workArea->workArea.horizontal_spacing;
   cur_y = workArea->workArea.vertical_spacing;
   node_width = node_height = 0;
   workArea_const = WORKAREA_CONSTRAINT(workArea->workArea.workArea_root);
   for (i = 0; i < workArea_const->workArea.n_sub_nodes; i++)
    {
      if (XtIsManaged(workArea_const->workArea.sub_nodes[i]) &&
         workArea_const->workArea.is_transient == FALSE)
       {
         if (workArea_const->workArea.sub_nodes[i]->core.width > node_width)
            node_width = workArea_const->workArea.sub_nodes[i]->core.width;
         if (workArea_const->workArea.sub_nodes[i]->core.height > node_height)
            node_height = workArea_const->workArea.sub_nodes[i]->core.height;
       }
    }
   if (node_width == 0 && node_height == 0)
      return;

   new_height = 0;

   /* Compute each widget's x,y position */
   ComputePositions(workArea, workArea->workArea.workArea_root, &new_width,
                    &new_height, node_width, node_height, &cur_x, &cur_y, 0,
		    &seen_one);

   replyHeight = 2 * workArea->workArea.vertical_spacing + node_height;
   if (replyHeight == 0)
      replyHeight = DEFAULT_HEIGHT;
      
   if (new_height == 0)
      new_height = replyHeight;
   /*
    * Resize the widget if new_width and new_height are different from the
    * Current width and height.
    */
   if (workArea->core.width != new_width || workArea->core.height != new_height)
    {
      result = XtMakeResizeRequest((Widget)workArea, new_width, new_height,
                                   &replyWidth, &replyHeight);

      /* Accept any compromise. */
      if (result == XtGeometryAlmost)
         XtMakeResizeRequest((Widget)workArea, replyWidth, replyHeight, NULL,
                             NULL);
    }

   /* Move each widget into place. */
   SetPositions(workArea, workArea->workArea.workArea_root);
   if (XtIsRealized((Widget)workArea))
      XClearArea(XtDisplay(workArea), XtWindow(workArea), 0, 0, 0, 0, TRUE);
}

/*
 *   NAME:    ComputePositions
 *   FUNCTION: 
 *   RETURNS:  
 */
static void
ComputePositions(
   WorkAreaWidget workArea,
   Widget w,
   Dimension *workArea_width,
   Dimension *workArea_height,
   Dimension node_width,
   Dimension node_height,
   int *cur_x,
   int *cur_y,
   int level,
   int *seen_one)
{
   int i;
   WorkAreaConstraints node_const = WORKAREA_CONSTRAINT(w);
   Widget node, super_node;
   Dimension offset;
   Dimension line_offset = workArea->workArea.line_offset;
   Dimension node_line_length = workArea->workArea.node_line_length;
   Dimension h_spacing = workArea->workArea.horizontal_spacing;
   Dimension v_spacing = workArea->workArea.vertical_spacing;
   Dimension level_offset = line_offset + node_line_length;

   if (XtIsManaged(w) && node_const->workArea.is_transient == FALSE)
    {
      if (workArea->workArea.packing == XmPACK_TIGHT)
         node_width = w->core.width;
      if (level == 1)
       {
         if (node_const->workArea.is_opened == FALSE)
          {
            if ((Dimension)(*cur_x + h_spacing + node_width) > *workArea_width)
             {
	       if (*seen_one)
		{
                  *cur_x = h_spacing + (level - 1) * level_offset;
                  *cur_y = *cur_y + node_height + v_spacing;
		}
             }
            node_const->workArea.x = *cur_x;
            node_const->workArea.y = *cur_y;
            *cur_x = *cur_x + h_spacing + node_width;
            if (workArea->workArea.alignment == XmALIGNMENT_CENTER)
             {
               offset = (Dimension)(node_width - w->core.width) / 2;
               node_const->workArea.x += offset;
             }
            switch (workArea->workArea.vertical_alignment)
            {
            case XmALIGNMENT_CONTENTS_BOTTOM:
               offset = node_height - w->core.height;
               node_const->workArea.y += offset;
               break;
            case XmALIGNMENT_CENTER:
               offset = (Dimension)(node_height - w->core.height) / 2;
               offset = offset / 2;
               break;
            default:
               break;
            }
          }
         /* end level is 1 and is_opened is FALSE */
         else /* is_opened is TRUE, so it has workAreas */
          {
            if (*cur_x != (h_spacing + (level - 1) * level_offset))
             {
               *cur_x = h_spacing + (level - 1) * level_offset;
               if (!workArea->workArea.is_list)
                  *cur_y = *cur_y + node_height + v_spacing;
             }
            node_const->workArea.y = *cur_y;
            node_const->workArea.x = *cur_x;
            *cur_y = *cur_y + node_height + v_spacing;
            *cur_x = node_const->workArea.x + level_offset;
          }
         /* level == 1 and is_opened is TRUE */
       }
      else /* Level != 1 */
       {
         *cur_x = h_spacing + (level - 1) * level_offset;
         node_const->workArea.x = *cur_x;
         node_const->workArea.y = *cur_y;
         *cur_y = *cur_y + w->core.height + v_spacing;
       }
      *seen_one = 1;
    }
   if (!workArea->workArea.is_list || level == 0)
    {
      for (i = 0; i < node_const->workArea.n_sub_nodes; i++)
       {
         ComputePositions(workArea, node_const->workArea.sub_nodes[i],
                          workArea_width, workArea_height, node_width,
			  node_height, cur_x, cur_y, level + 1, seen_one);

	 if (!workArea->workArea.is_list)
	  {
            WorkAreaConstraints tmp_const;
            int h;
	    tmp_const = WORKAREA_CONSTRAINT(node_const->workArea.sub_nodes[i]);
	    h = node_const->workArea.sub_nodes[i]->core.height + v_spacing +
	        tmp_const->workArea.y;
	    if ((int)*workArea_height < h)
	       *workArea_height = h;
	  }
       }
      if (level == 1 && node_const->workArea.is_opened)
         *cur_x = h_spacing + (level - 1) * level_offset;
    }
   else if (node_const->workArea.n_sub_nodes > 0)
    {
      WorkAreaConstraints super_const;
      Widget widget;
      Boolean has_managed_children = False;
      super_const = node_const;
      for (i = 0; i < super_const->workArea.n_sub_nodes; i++)
       {
         widget = super_const->workArea.sub_nodes[i];
         if (XtIsManaged(widget))
          {
           has_managed_children = True; 
           if (widget->core.height > node_height)
              node_height = widget->core.height;
          }
       }
      if (has_managed_children)
       {
         int cur_level1_x;
         widget = super_const->workArea.sub_nodes[0];
         node_const = WORKAREA_CONSTRAINT(widget);
         if (XmIsPushButton(widget) && XtIsManaged(widget))
          {
            node_const->workArea.x = h_spacing;
            cur_level1_x = (2 * h_spacing) + widget->core.width;
            super_const->workArea.x = cur_level1_x;
            cur_level1_x += h_spacing + node_width;
          }
         else
            cur_level1_x = (2 * h_spacing) + node_width;
          
         offset = (Dimension)(w->core.height - node_height) / 2;
         if ((Dimension)(super_const->workArea.y + offset) < v_spacing)
          {
            *cur_y = node_height + 2 * v_spacing;
            super_const->workArea.y = v_spacing + (Position)
                                       (node_height - w->core.height) / 2;
          }
         else if (node_height > w->core.height)
          {
            *cur_y = *cur_y - w->core.height + node_height ;
            offset = (Dimension)(node_height - w->core.height) / 2;
            super_const->workArea.y += offset;
          }
         for (i = 0; i < super_const->workArea.n_sub_nodes; i++)
          {
            widget = super_const->workArea.sub_nodes[i];
            if (!XtIsManaged(widget))
               continue;

            node_const = WORKAREA_CONSTRAINT(widget);
            if (i != 0 || !XmIsPushButton(widget))
             {
               node_const->workArea.x = cur_level1_x;
               cur_level1_x += h_spacing + widget->core.width;
             }
            if (i == 0 && XmIsPushButton(widget) && GuiIsIcon(w))
             {
               XRectangle pixmap_rect, label_rect;
               int height = widget->core.height +
                  ((XmPrimitiveWidget)widget)->primitive.highlight_thickness;
               GuiIconGetRects(w, &pixmap_rect, &label_rect);
               pixmap_rect.height += pixmap_rect.y;
               if ((int)pixmap_rect.height > height)
                {
                  node_const->workArea.y = super_const->workArea.y +
                                           pixmap_rect.height - height - 2;
                  continue;
                }
             }
            offset = (Dimension)(node_height - widget->core.height) / 2;
            node_const->workArea.y = *cur_y - node_height -
                                     v_spacing + offset;
          }
         *workArea_height = *cur_y;
         i = super_const->workArea.n_sub_nodes - 2;
         widget = super_const->workArea.sub_nodes[i];
	 if (XtIsManaged(widget))
	   {
	     int y;
             widget = super_const->workArea.sub_nodes[i + 1];
             node_const = WORKAREA_CONSTRAINT(widget);
	     y = node_const->workArea.y;
             widget = super_const->workArea.sub_nodes[i];
             node_const = WORKAREA_CONSTRAINT(widget);
	     node_const->workArea.y = y + 5;
	   }
       }
    }
}

/*
 *   NAME:    SetPositions
 *   FUNCTION: 
 *   RETURNS:  
 */
static void
SetPositions(
   WorkAreaWidget workArea, 
   Widget w)
{
   int i;
   WorkAreaConstraints node_const;
   Dimension new_width;

   node_const = WORKAREA_CONSTRAINT(w);

   if (XtIsManaged(w) && node_const->workArea.is_transient == FALSE)
    {
      XtMoveWidget(w, node_const->workArea.x, node_const->workArea.y);
      /* Resize a workArea if it's opened */
      if (node_const->workArea.is_workArea)
       {
         new_width = workArea->core.width - node_const->workArea.x;
         XtResizeWidget(w, new_width, w->core.height, w->core.border_width);
       }
    }
   /* Set the positions of all sub_nodes. */
   for (i=0; i< node_const->workArea.n_sub_nodes;i++)
      SetPositions(workArea, node_const->workArea.sub_nodes[i]);
}

/*
 *   NAME:    OpenedSubnodes
 *   FUNCTION: 
 *   RETURNS:  
 */
static int
OpenedSubnodes(
   Widget w)
{
   int i, n_subnodes = 0;
   WorkAreaConstraints node_const;
   WorkAreaConstraints workArea_const;

   workArea_const = WORKAREA_CONSTRAINT(w);

   for (i=0; i< workArea_const->workArea.n_sub_nodes;i++)
    {
      node_const = WORKAREA_CONSTRAINT(workArea_const->workArea.sub_nodes[i]);
      if (node_const->workArea.is_opened == TRUE)
         n_subnodes++;
    }
   return n_subnodes;
}

/*
 *   NAME:    Resize
 *   FUNCTION: 
 *   RETURNS:  
 */
static void
Resize(
   WorkAreaWidget w)
{
   NewLayout(w);
   w->workArea.old_width = w->core.width;
   w->workArea.old_height = w->core.height;
}

/*
 *   NAME:    MakeChildVisible
 *   FUNCTION: 
 *   RETURNS:  
 */
static void
MakeChildVisible(
   Widget widget, 
   XtPointer client_data, 
   XtPointer callback_data)
{
   XmTraverseObscuredCallbackStruct *cb;
   Widget w;

   cb = (XmTraverseObscuredCallbackStruct *) callback_data;
   if (cb->direction != XmTRAVERSE_CURRENT)
    {
      w = cb->traversal_destination;
      XmScrollVisible(widget, w, 0, 0);
    }
}

/*
 *   NAME:    ResizeSW
 *   FUNCTION: 
 *   RETURNS:  
 */
static void
ResizeSW(
   Widget w, 
   XtPointer client_data, 
   XtPointer callback_data)
{
   /* Don't calculate resize here, since the scroll bar hasn't been
      updated.  Instead, add a sufficient timeout to calculate the resize
      after the scroll bar has updated it's XmNvalue resource */
   XtAppAddTimeOut(XtWidgetToApplicationContext(w), 300,
                   (XtTimerCallbackProc) ResizeTimeOut, client_data);
}

/*
 *   NAME:    DestroyParent
 *   FUNCTION: 
 *   RETURNS:  
 */
static void
DestroyParent(
   Widget w, 
   XtPointer client_data, 
   XtPointer callback_data)
{
    XtDestroyWidget (XtParent (w));
}

/*
 *   NAME:    ResizeTimeOut
 *   FUNCTION: 
 *   RETURNS:  
 */
static void
ResizeTimeOut(
   Widget workArea, 
   XtIntervalId *id)
{
   Arg args[1];
   int width = workArea->core.parent->core.width;
   WorkAreaWidget w = (WorkAreaWidget) workArea;
   if (w->workArea.horizontal_spacing > 2)
     width -= w->workArea.horizontal_spacing;
   else
     width -= 2;
   XtSetArg(args[0], XmNwidth, width);
   XtSetValues(workArea, args, 1);
}
