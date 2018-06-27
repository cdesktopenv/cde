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
/* $XConsortium: widget.c /main/4 1995/11/01 15:57:40 rswiston $ */

/*	Copyright (c) 1991, 1992 UNIX System Laboratories, Inc. */
/*	All Rights Reserved     */

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF          */
/*	UNIX System Laboratories, Inc.			        */
/*	The copyright notice above does not evidence any        */
/*	actual or intended publication of such source code.     */

#include        "name.h"
#include        "shell.h"
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/XmStrDefs.h>
#include <Xm/DialogS.h>
#include "hash.h"
#include "stdio.h"
#define NO_AST
#include "dtksh.h"
#undef NO_AST
#include "xmksh.h"
#include "dtkcmds.h"
#include "xmcvt.h"
#include "widget.h"
#include "extra.h"
#include "xmwidgets.h"
#include "msgs.h"

/* values for the flags field of the W array */

#define RESOURCE_HASHSIZE	64
#define CLASS_HASHSIZE		32
#define WIDGETALLOC		16	/* Will realloc the widget array in this increment */

wtab_t **W = NULL;
int NumW = 0;
int MaxW = 0;
int Wtab_free = 0;	/* A count of how many table entries have been freed */


/* CONSTANTS */
char str_0123456789[] = "0123456789";

Hashtab_t *Wclasses;




static void fixupresources( 
                        char *name,
                        Hash_table_t *res,
                        resfixup_t *fixups) ;
static int rescompare( 
                        XtResource **r1,
                        XtResource **r2) ;
static void _pr_class( 
                        classtab_t *c) ;
static void _pr_resource_list( 
                        XtResource *res) ;
static void sort_and_print_res( void ) ;
static void gather_resource_list( 
                        char *name,
                        char *r,
                        void *notUsed) ;
static void _pr_resource( 
                        classtab_t *c,
                        wtab_t *w) ;
static void pr_resource( 
                        char *name,
                        char *r,
                        void *notUsed) ;
static void pr_class( 
                        char *name,
                        char *c,
                        void *notUsed) ;
static char * getname( 
                        wtab_t *w,
                        char *buf,
                        int max) ;
static void pr_widinfo( 
                        wtab_t *w) ;
static void pr_widheader( void ) ;



void
init_widgets( void )
{
	int i, n = 0;
	char *nam;
	wtab_t *wentries;
        char * errmsg;
        char * errhdr;

	if (C[0].class != NULL)
		return;

	toolkit_init_widgets();

        Wclasses = hashalloc(NULL, 0);

	for (i = 0; C[i].cname != NULL; i++) {
                if ((nam = hashput(Wclasses, C[i].cname, (char *)(&C[i]))) == NULL) 
                {
                         errhdr = strdup(GetSharedMsg(DT_ERROR));
                         errmsg = strdup(GETMESSAGE(14,1, 
                                 "Internal hash table failure during widget class initialization; exiting"));
                         printerr(errhdr, errmsg, NULL);
                         free(errhdr);
                         free(errmsg);
			 exit(1);
		}
	}

	/*
	 * The array of widget records starts out big enough to
	 * hold WIDGETALLOC widgets, and will grow in increments
	 * of WIDGETALLOC as it overflows.
	 */
	W = (wtab_t **)XtMalloc(WIDGETALLOC*sizeof(wtab_t *));
	wentries = (wtab_t *)XtCalloc(WIDGETALLOC, sizeof(wtab_t));
	for (i = 0; i < WIDGETALLOC; i++)
        {
		W[i] = &wentries[i];
                wentries[i].type = TAB_EMPTY;
        }
	MaxW = WIDGETALLOC;
}

/*
 * string to widgetclass
 */

classtab_t *
str_to_class(
        char *arg0,
        char *s )
{
   int i, j, n;
   Widget w;
   char *nam;
   classtab_t *ret;
   char * errhdr;
   char * errmsg;
   Cardinal numBlocks;
   XmSecondaryResourceData * blockArray;

   /*
    * If it looks like a handle, look it up and return the
    * class of the widget associated with the handle.
    */
   if (s[0] == 'W' && strspn(&s[1], str_0123456789) == strlen(&s[1])) 
   {
      wtab_t *w = str_to_wtab(arg0, s);
      if (w != NULL)
         return(w->wclass);
   }

   if ((nam = hashget(Wclasses, s)) != NULL) 
   {
      ret = (classtab_t *)nam;
      if (ret->res == NULL) 
      {
         XtResourceList resources;
         Cardinal numresources;

         /* First reference of a given widget class
          * Automatically causes that widget class to
          * be initialized, it's resources read and
          * hashed.
          */
         ret->res = (char *)hashalloc(NULL, 0);

         /*
          * Have to force the class init
          * of this widget to execute, else we won't
          * get a complete list of resources, and any
          * converters added by this widget won't be
          * available.
          */
         XtInitializeWidgetClass(ret->class);

         XtGetResourceList(ret->class, &resources, &numresources);
         for (i = 0; i < numresources; i++) 
         {
            if ((nam = (char *)hashput((Hash_table_t*)ret->res, 
                resources[i].resource_name, (char *)&resources[i])) == NULL) 
            {
               errhdr = strdup(GetSharedMsg(DT_ERROR));
               errmsg=strdup(GetSharedMsg( DT_HASHING_FAILURE));
               printerrf(errhdr, errmsg, resources[i].resource_name,
                         ret->cname, NULL, NULL, NULL, NULL, NULL, NULL);
               free(errhdr);
               free(errmsg);
            } 
         }

         /* Load any of Motif's secondary resources */
         /*
          * The following is a special hack to work around a Motif bug
          * (which has been reported).  DialogShell is a grandchild class of
          * VendorShell, which defines secondary resources.  DialogShell is 
          * also a child class of TransientShell.  Secondary resource
          * information is contained within extension records, which the
          * TransientShell class knows nothing about, thus preventing
          * XmGetSecondaryResourceData from getting any secondary resources
          * further up the inheritance chain.
          */
         if (ret->class == xmDialogShellWidgetClass)
         {
            numBlocks = XmGetSecondaryResourceData(vendorShellWidgetClass, 
                                                   &blockArray);
         }
         else
            numBlocks = XmGetSecondaryResourceData(ret->class, &blockArray);

         if (numBlocks > 0)
         {
            for (i = 0; i < numBlocks; i++)
            {
               for (j = 0; j < blockArray[i]->num_resources; j++)
               {
                  if ((nam = (char *)hashput((Hash_table_t*)ret->res, 
                             blockArray[i]->resources[j].resource_name, 
                             (char *)&(blockArray[i]->resources[j]))) == NULL)
                  {
                     errhdr = strdup(GetSharedMsg(DT_ERROR));
                     errmsg=strdup(GetSharedMsg( DT_HASHING_FAILURE));
                     printerrf(errhdr, errmsg, 
                               blockArray[i]->resources[j].resource_name,
                               ret->cname, NULL, NULL, NULL, NULL, NULL, NULL);
                     free(errhdr);
                     free(errmsg);
                  } 
               }
               /*
                * We can't free up the actual array of resources, since we
                * hashed a reference to the entries into our resource hash
                * table:
                * XtFree((char *)blockArray[i]->resources); 
                */
               XtFree((char *)blockArray[i]);
            }
         }

         /* Perform any special resource fixing up */
         fixupresources(s, (Hash_table_t *)ret->res, 
                        (resfixup_t *)&ret->resfix[0]);

         /*
	  * Get constraint resources, if there are any
          */
         XtGetConstraintResourceList(ret->class, &resources, &numresources);
         if (resources != NULL) 
         {
            ret->con = (char *)hashalloc(NULL, 0);
            for (i = 0; i < numresources; i++) 
            {
               if ((nam = (char *)hashput((Hash_table_t*)ret->con, 
                    resources[i].resource_name, &resources[i])) == NULL) 
               {
                  errhdr =strdup(GetSharedMsg(DT_ERROR));
                  errmsg=strdup(GetSharedMsg(DT_HASHING_FAILURE));
                  printerrf(errhdr, errmsg, resources[i].resource_name,
                            ret->cname, NULL, NULL, NULL, NULL, NULL, NULL);
                  free(errhdr);
                  free(errmsg);
               } 
            }
            fixupresources(s, (Hash_table_t *)ret->con, 
                           (resfixup_t *)&ret->confix[0]);
         } 
         else 
            ret->con = NULL;
      }
      return(ret);
   }

   errmsg = strdup(GETMESSAGE(14,2, 
                   "Could not find a widget class named '%s'"));
   printerrf(arg0, errmsg, s, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
   free(errmsg);
   return(NULL);
}

Widget
DtkshNameToWidget(
        String s )
{
	Widget w;
	char *p;
	int len;
        char * errhdr;
        char * errmsg;

	if (s == NULL || *s == '\0')
		return(NULL);

	if (W == NULL || W[0] == NULL) {
		errhdr = strdup(GetSharedMsg(DT_WARNING));
                errmsg = strdup(GetSharedMsg(DT_TK_NOT_INIT));
		printerr(errhdr, errmsg, NULL);
                free(errhdr);
                free(errmsg);
		return(NULL);
	}
	len = strlen(W[0]->wname);
	if (strncmp(s, W[0]->wname, len) == 0) {
		if (s[len] == '\0')
			return(Toplevel);
		if (s[len] == '.')
			return(XtNameToWidget(Toplevel, &s[len+1]));
	}
	return(NULL);
}

/*
 * Take a character string and translate it into a wtab_t.
 * The string should be of the form: W<num>.  The <num> must
 * point to a valid index in the W array.
 *
 * If the name is not of the correct form, we use XtNameToWidget
 * to try to convert the name to a widget id.
 */

wtab_t *
str_to_wtab(
        char *arg0,
        char *v )
{
	int index, len;
        char * errmsg;

	if (v == NULL || strcmp(v, "NULL") == 0)
		return(NULL);

	if (v[0] != 'W' || (len = strlen(v)) < 2 ||
		strspn(&v[1], str_0123456789) != len-1) {
		Widget wid;

		if ((wid = DtkshNameToWidget(v)) == NULL) {
			if (arg0)
                        {
                                errmsg = strdup(GETMESSAGE(14,3, 
                                  "The identifier '%s' is not a valid widget handle"));
				printerrf(arg0, errmsg, v,
                                          NULL, NULL, NULL, NULL, NULL,
                                          NULL, NULL);
                                free(errmsg);
                        }
			return(NULL);
		}

		return(widget_to_wtab(wid));
	}
	index = atoi(&v[1]);
	if (index < 0 || index >= NumW) {
		if (arg0)
                {
                        errmsg = strdup(GetSharedMsg(DT_BAD_WIDGET_HANDLE));
			printerrf(arg0, errmsg, v, NULL,
                                  NULL, NULL, NULL, NULL, NULL, NULL);
                        free(errmsg);
                }
		return(NULL);
	}
	if (W[index]->type == TAB_EMPTY && W[index]->w == NULL) {
		if (arg0)
                {
                        errmsg = strdup(GetSharedMsg(DT_BAD_WIDGET_HANDLE));
			printerrf(arg0, errmsg, v, NULL, NULL,
                                  NULL, NULL, NULL, NULL, NULL);
                        free(errmsg);
                }
		return(NULL);
	}
	return(W[index]);
}

Widget
handle_to_widget(
        char *arg0,
        char *handle )
{
	wtab_t *w = str_to_wtab(arg0, handle);

	if (w)
		return(w->w);
	else
		return(NULL);
}

/*
 * This function takes a widget and finds the wtab associated with it.
 * This operation is performed infrequently, for example if the user
 * gets a resource that is a widget.  So, we're just using a linear
 * search right now.  If profiling reveals this to be too slow we'll
 * have to introduce another hash table or something.
 */

wtab_t *
widget_to_wtab(
        Widget w )
{
	int i;
        char * errmsg;

	if (w == NULL)
		return(NULL);
	for (i = 0; i < NumW; i++) {
		if ((W[i]->type != TAB_EMPTY) && (W[i]->w == w))
			return(W[i]);
	}
	/*
	 * If we failed to find the widget id in the
	 * internal table then this was probably a widget that
	 * was created as a side effect of another widget's creation,
	 * or perhaps was created by a user-defined builtin or something.
	 * So, we'll create a new table entry for it using set_up_w().
	 * Of course, set_up_w() needs to know the widget's parent's
	 * wtab_t, which we get by recursively calling ourself.  Also,
	 * we need the widget's class.
	 */
	{
		wtab_t *pwtab;		/* parent wtab */
		WidgetClass wclass;	/* class record */
		classtab_t *class;	/* widget's class */

		if ((pwtab = widget_to_wtab(XtParent(w))) == NULL) {
                        errmsg = strdup(GetSharedMsg(DT_NO_PARENT));
			printerr("widget_to_wtab", errmsg, NULL);
                        free(errmsg);
			return(NULL);
		}
		wclass = XtClass(w);
		/*
		 * Again, we have to go linear searching for this
		 * right now.
		 */
		class = NULL;
		for (i = 0; C[i].cname != NULL; i++) {
			if (C[i].class == wclass) {
				class = &C[i];
				break;
			}
		}
		if (class == NULL) {
                        errmsg = strdup(GETMESSAGE(14,4, 
                              "Unable to find the widget class"));
			printerr("widget_to_wtab", errmsg, NULL);
                        free(errmsg);
			return(NULL);
		}
		/*
		 * If this class has not been initialized, we
		 * better force it to be set up by calling
		 * str_to_class();
		 */
		if (class->res == NULL)
			str_to_class("widget_to_wtab", class->cname);
		return(set_up_w(w, pwtab, NULL, NULL, class));
	}
}

void
get_new_wtab(
        wtab_t **w,
        char *name )

{
	int i;

	/*
	 * If there has been a destroywidget call, then one or more
	 * table entries may have been freed.  We might want to make
	 * a free list for this stuff someday, but for now we do a
	 * linear search for the free slot.  Most applications don't
	 * do much widget destroying anyway, so this should rarely
	 * execute and thus I'm not too dismayed by the linear search.
	 */
	i = NumW;
	if (Wtab_free > 0) {
		for (i = 0; i < NumW; i++) {
			if (W[i]->type == TAB_EMPTY) {
				Wtab_free--;
				break;
			}
		}
	}
	if (i == NumW) {
		if (NumW < MaxW) {
			i = NumW++;
		} else {
			int j;
			int oldmax = MaxW;
			wtab_t *wentries;

			MaxW += WIDGETALLOC;
			W = (wtab_t **)XtRealloc((char *)W, sizeof(wtab_t *)*MaxW);
			wentries = (wtab_t *)XtMalloc(sizeof(wtab_t)*WIDGETALLOC);
			for (j = 0; j < WIDGETALLOC; j++)
                        {
				W[oldmax+j] = &wentries[j];
			        wentries[j].type = TAB_EMPTY;
                        }
			i = NumW++;
		}
	}
	sprintf(name, "W%d", i);
	*w = W[i];
	return;
}

static void
fixupresources(
        char *name,
        Hash_table_t *res,
        resfixup_t *fixups )
{
	XtResource *resource;
	int i;
	char *nam;

	if (fixups == NULL)
		return;

        for (i = 0; fixups[i].name != NULL; i++) {
                resource = (XtResource *)hashget(res, fixups[i].name);
                /*
                 * We could be either adding a new resource or
                 * modifying an old one.
                 */
                if (resource == NULL)
                        resource = (XtResource *)XtMalloc(sizeof(XtResource));
                /*
                 * The only fields dtksh uses are the name, class, type and
                 * size, so that's all we attempt to fix up.
                 *
                 * NOTE: THE CLASS NAME IS REQUIRED, BECAUSE IF WE ARE ADDING
                 *       A NEW RESOURCE (VERSUS REPLACING AN EXISTING ONE),
                 *       THEN THE RESOURCE ENDS UP HAVING A 'NULL' CLASS
                 *       NAME.  SINCE DtWidgetInfo DISPLAYS THE RESOURCE CLASS 
                 *       NAME, THIS TIDBIT OF USEFUL INFORMATION WOULD BE 
                 *       MISSING.
                 */
                resource->resource_name = (String)fixups[i].name;
                resource->resource_class = (String)fixups[i].class;
                resource->resource_type = (String)fixups[i].type;
                resource->resource_size = fixups[i].size;
                hashput(res, fixups[i].name, (char *)resource);
        }
}

int
do_DtLoadWidget(
        int argc,
        char *argv[] )
{
   classtab_t *classtab;
   void *address;
   char *nam;
   char * errhdr;
   char * errmsg;

   init_widgets();

   if (argc != 3) 
   {
      errmsg = strdup(GETMESSAGE(14,5, 
               "Usage: DtLoadWidget widgetClassName widgetClassRecordName"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   if ((address = (void *)fsym(argv[2], -1)) == NULL) 
   {
      errmsg = strdup(GETMESSAGE(14,6, 
                    "Unable to locate a widget class record named '%s'"));
      printerrf(argv[0], errmsg,
                argv[2], NULL, NULL, NULL, NULL, NULL, NULL, NULL);
      free(errmsg);
      return(1);
   } 
   else 
   {
      classtab = (classtab_t *)XtMalloc(sizeof(classtab_t));
      memset(classtab, '\0', sizeof(classtab_t));
      classtab->cname = (char *)strdup(argv[1]);
      classtab->class = ((WidgetClass *)address)[0];
      if ((nam = hashput(Wclasses, classtab->cname, (char *)classtab)) == NULL)
      {
         errhdr = strdup(GetSharedMsg(DT_ERROR));
         errmsg= strdup(GETMESSAGE(14,7, 
            "Internal hash table failure during initialization of widget class '%s'"));
         printerrf(errhdr, errmsg, classtab->cname,
                   NULL, NULL, NULL, NULL, NULL, NULL, NULL);
         free(errhdr);
         free(errmsg);
         return(1);
      }
   }
   return(0);
}

static XtResource *Res[1024];
static int Nres;

static int
rescompare(
        XtResource **r1,
        XtResource **r2 )
{
	return(strcmp(r1[0]->resource_name, r2[0]->resource_name));
}

static void
_pr_class(
        classtab_t *c )
{
	printf("%s\n", c->cname);
}

static void
_pr_resource_list(
        XtResource *res )
{
	printf("\t%-24.24s %-24.24s %s\n", res->resource_name, res->resource_class, res->resource_type);
}

static void
sort_and_print_res( void )
{
	int i;
	qsort(Res, Nres, sizeof(XtResource *), (int (*)())rescompare);
	for (i = 0; i < Nres; i++) {
		_pr_resource_list(Res[i]);
	}
}

static void
gather_resource_list(
        char *name,
        char *r,
        void *notUsed )
{
        XtResource *res = (XtResource *)r;

	Res[Nres++] = res;
}

static int Show_constraint;

static void
_pr_resource(
        classtab_t *c,
        wtab_t *w )
{
        char * errmsg;
        char * errmsg2;

	if (c->res == NULL)
		str_to_class("DtWidgetInfo", c->cname);

	if (Show_constraint && c->con == NULL)	/* No constraint resources */
		return;

	errmsg = strdup(GETMESSAGE(14,8, "\n%sRESOURCES FOR %s%s%s:\n"));
	errmsg2 = strdup(Show_constraint ? GETMESSAGE(14,9, "CONSTRAINT ") : 
                         str_nill);
	printf(errmsg, errmsg2,
                c->cname,
		w ? " " : str_nill, 
                w ? w->widid : str_nill);
        free(errmsg);
        free(errmsg2);

	Nres = 0;

        hashwalk((Hash_table_t*)(Show_constraint ? c->con : c->res), 0, (int (*)())gather_resource_list, NULL);
	if (!Show_constraint && w && w->parent != NULL && 
		XtIsConstraint(w->parent->w)) {
                hashwalk((Hash_table_t *)w->parent->wclass->con, 0, (int (*)())gather_resource_list, NULL);

	}

	sort_and_print_res();
}

static void
pr_resource(
        char *name,
        char *r,
        void *notUsed )
{
        classtab_t *c = (classtab_t *)r;

	_pr_resource(c, NULL);
}

static void
pr_class(
        char *name,
        char *c,
        void *notUsed )
{
        classtab_t *class = (classtab_t *)c;

	_pr_class(class);
}

static char *
getname(
        wtab_t *w,
        char *buf,
        int max )
{
	char *p;
	int len;

	/* calculate a widget's name.  Goes backwards through the
	 * list of parents, filling in the names backwards in the
	 * buffer, then returns a pointer to the start of the name
	 */
	p = &buf[max];	/* that's right, buf[max] not buf[max-1] */
	for ( ; w; w = w->parent) {
		if (p - (len = strlen(w->wname)) < buf+3) {	/* overflow! */
			p--;
			*p = '*';
			return(p);
		}
		p -= len+1;
		strcpy(p, w->wname);
		if (p + len != buf + max - 1)
			p[len] = '.';
	}
	return(p);
}

static void
pr_widinfo(
        wtab_t *w )
{
	char namebuf[256];
	char *name;
	char statbuf[8];
        static char * errmsg = NULL;
        static char * realizedStr = NULL;
        static char * managedStr = NULL;
        static char * sensitiveStr = NULL;
        char * realized, * managed, * sensitive;

        if (errmsg == NULL)
        {
	   errmsg = strdup(GETMESSAGE(14,13, 
                           "%-15s %-6s %-6s %-18s %-6s %s\n"));
	   realizedStr = strdup(GETMESSAGE(14,10, "R"));
	   managedStr = strdup(GETMESSAGE(14,11, "M"));
	   sensitiveStr = strdup(GETMESSAGE(14,12, "S"));
        }

	name = getname(w, namebuf, sizeof(namebuf));
	realized = XtIsRealized(w->w) ? realizedStr : str_nill;
	managed = XtIsManaged(w->w) ? managedStr : str_nill;
	sensitive = XtIsSensitive(w->w) ? sensitiveStr : str_nill;
	sprintf(statbuf, "%s%s%s", realized, managed, sensitive);

	printf(errmsg,
		w->envar,
		w->widid, 
		w->parent ? w->parent->widid : "none", 
		w->wclass->cname,
		statbuf,
		name);
}

static void
pr_widheader( void )
{
   char * errmsg;

   errmsg = strdup(GETMESSAGE(14,14, 
            "ENV VARIABLE    HANDLE PARENT CLASS              STATUS NAME\n"));
   printf(errmsg);
   free(errmsg);
}

/*
 * DtWidgetInfo -r [widget|class]   print resources and their types for widgets
 * DtWidgetInfo -R [widget|class]   print constraint resources for widgets
 * DtWidgetInfo -c [class]	    print info about a class
 * DtWidgetInfo -h [handle]	    print widget handles [or widget name]
 * DtWidgetInfo			    print summary info about all widgets 
 */


int
do_DtWidgetInfo(
        int argc,
        char *argv[] )
{
	int i, j;
	char buf[1024];
	wtab_t *w;
	classtab_t *c;
	int errs = 0;
        char * errmsg;

	if (C[0].class == NULL) {
                errmsg = strdup(GetSharedMsg(DT_TK_NOT_INIT));
		printerr(argv[0], errmsg, NULL);
                free(errmsg);
		return(1);
	}
	if (argc == 1 || argv[1][0] != '-') {
		/* Print long listing of each widget */
		pr_widheader();
		if (argc == 1) {
			for (i = 0; i < NumW; i++) {
				if (W[i]->type == TAB_EMPTY)
					continue;
				pr_widinfo(W[i]);
			}
		} else {
			for (i = 1; i < argc; i++) {
				if ((w = str_to_wtab(argv[0], argv[i])) != NULL)
					pr_widinfo(w);
			}
		}
	} else if (argv[1][0] == '-') {
		if ((Show_constraint = strcmp(argv[1], "-r")) == 0 || 
			strcmp(argv[1], "-R") == 0) {
			/* print all the resources in each widget or class */
			if (argc == 2) {
                                hashwalk((Hash_table_t *)Wclasses, 0, 
                                           (int (*)())pr_resource, 0);
				return(0);
			} else {
				for (i = 2; i < argc; i++) {
					if ((c = str_to_class(argv[0], argv[i])) != NULL) {
						if (Show_constraint && c->con == NULL)
							return(0);

						if (!Show_constraint && c-> res == NULL)
							return(0);

						w = str_to_wtab(NULL, argv[i]);
						_pr_resource(c, w);
					}
				}
			}
			return(0);
		} else if (strcmp(argv[1], "-c") == 0) {
			/*
			 * print all the available classes, or check if a
			 * class is available
			 */
			if (argc == 2) {
                                hashwalk((Hash_table_t *)Wclasses, 0, (int (*)())pr_class, 0);
			} else {
				for (i = 2; i < argc; i++) {
					if ((c = str_to_class(argv[0], argv[i])) != NULL)
						_pr_class(c);
				}
			}
			return(0);
		} else if (strcmp(argv[1], "-h") == 0) {
			/* print active widget handles */
			if (argc == 2) {
				for (i = 0; i < NumW; i++) {
					if (W[i]->type == TAB_EMPTY)
						continue;
					printf("%s\n", W[i]->widid);
				}
			} else {
				for (i = 2; i < argc; i++) {
					if ((w = str_to_wtab(argv[0], argv[i])) == NULL) {
						errs++;
						continue;
					}
					printf("%s\n", w->wname);
				}
			}
		} else {
                        errmsg = strdup(GetSharedMsg(DT_UNKNOWN_OPTION));
			printerrf(argv[0], errmsg, argv[1], NULL,
                                  NULL, NULL, NULL, NULL, NULL, NULL);
                        free(errmsg);

                        errmsg = strdup(GETMESSAGE(14,15, "\nUsage:\tDtWidgetInfo [widgetHandle]\n\tDtWidgetInfo -r <widgetHandle|className>\n\tDtWidgetInfo -R <widgetHandle|className>\n\tDtWidgetInfo -c [className]\n\tDtWidgetInfo -h [widgetHandle]"));
		        printerr(str_nill, errmsg, NULL);
                        free(errmsg);
			return(255);
		}
	}
	return(errs);
}


/*
 * If the incoming environment variable is "-", then print the value out,
 * instead of placing it into the environment.
 */

void
alt_env_set_var(
        char *variable,
        char *value )
{
   if (strcmp(variable, "-") == 0)
      puts(value);
   else
      env_set_var(variable, value);
}
