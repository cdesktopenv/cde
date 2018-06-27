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
/* $TOG: vglang.c /main/7 1998/03/04 19:28:18 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

 /****************************************************************************
 **
 **   File:        ui.c
 **
 **   Project:     Common Desktop Environment
 **
 **   Description: common ui code for login manager
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/



/***************************************************************************
 *
 *  Includes
 *
 ***************************************************************************/

#include	<stdio.h>
#include	"vg.h"
#include	"vgmsg.h"
#include	<Xm/CascadeBG.h>
#include	<Xm/RowColumn.h>
#include	<Xm/ToggleBG.h>

/***************************************************************************
 *
 *  GetLangName
 *
 *  Convert an actual locale name to a meaningful language name which will
 *  be shown in the language menu. This function may be platform dependent.
 ***************************************************************************/

static char *
GetLangName( char *label )
{
/*
 * Default implementation is to use langName resource. Language names can be
 * set in Dtlogin file as follows. (In this case, en_US is a locale name which
 * can be set to LANG environment variable.
 *
 *    Dtlogin*en_US*languageName: English (ISO8859-1)
 */
    char rmname[50];
    char rmclass[50];
    char *rmtype;
    XrmValue rmvalue;

    sprintf(rmname, "Dtlogin*%s*languageName", label);
    sprintf(rmclass, "Dtlogin*%s*LanguageName", label);
    if(XrmGetResource(XtDatabase(dpyinfo.dpy), rmname, rmclass, &rmtype,
                      &rmvalue)) {
        return(rmvalue.addr);
    }
    else
        return(NULL);
}

#if defined (ENABLE_DYNAMIC_LANGLIST)
/***************************************************************************
 *
 *  Methods for dynamic language list
 *
 ***************************************************************************/

/*
 * _enumLangCmdStart() - start enumeration of languages and descriptions
 *
 *  The command specified by Dtlogin*languageListCmd command returns
 *  the list of locales and a translated description of each.
 */
static void *
_enumLangCmdStart(void)
{
  if (appInfo.languageListCmd)
  {
    return((void *)popen(appInfo.languageListCmd, "r"));
  }
  return(NULL);
}

/*
 * _enumLangCmdNext() - get next language description pair
 *                   note: *lang and *desc must be freed by free()
 *
 * Read the next locale and description from pipe. Default description
 * comes from system, but user may override with Xresources description.
 */
static Boolean
_enumLangCmdNext(
  void *state,
  char **lang,
  char **desc)
{
  char buf[200];
  Boolean rc = False;

  if (fgets(buf, sizeof(buf), (FILE *)state) != NULL)
  {
   /*
    * The buf format is "locale desc ...\n". For example:
    *
    * pl_PL Polish ISO8859-2 
    * pt_BR Portuguese (Brazil) ISO8859-1
    */
    char *loclang = strtok(buf, " ");  /* lang name from system */
    char *locdesc = strtok(NULL, "\n"); /* description from system */
    char *userdesc; /* user provided description from Xresources */

    if ((userdesc = GetLangName(loclang)) != NULL)
    {
      locdesc = userdesc; /* use user provided description */
    }

    *lang = strdup(loclang);
    *desc = strdup(locdesc);
    rc = True;
  }
  return(rc);
}

/*
 * _enumLangCmdEnd() - end enumeration of language names and descriptions
 *
 * Close pipe.
 */
static void
_enumLangCmdEnd(
  void *state)
{
  pclose((FILE *)state);
}
#endif /* ENABLE_DYNAMIC_LANGLIST */

/***************************************************************************
 *
 *  Methods for language list of type 'LANGLIST'
 *
 ***************************************************************************/

#define DELIM		" \t"   /* delimiters in language list		   */

struct _enumState 
{
  char *dupstr;
  char *tokstr;
};

/*
 * _enumLanglistStart() - start enumeration of languages and descriptions
 * 
 * Dtlogin sets up the LANGLIST env var which contains the list
 * of locale names to display in the language menus. 
 */
static void *
_enumLanglistStart(void)
{
  char *p;
  struct _enumState *state = malloc(sizeof(struct _enumState));
 
  if (state)
  { 
    if ((p = (char *)getenv(LANGLIST)) == NULL )
    {
      free(state);
      state = NULL;
    }
    else
    {
      state->dupstr = strdup(p);
      state->tokstr = state->dupstr;
    }
  }

  return((void *)state);
}

/*
 * _enumLanglistNext() - get next language description pair
 *                   note: *lang and *desc must be freed by free()
 *
 * Get next locale from LANGLIST and get possible description from
 * Xresources.
 */
static Boolean
_enumLanglistNext(
  void *state,
  char **lang,
  char **desc)
{
  Boolean rc = False;
  struct _enumState *enumstate = (struct _enumState *)state;
  char *loclang, *locdesc;

  loclang = strtok(enumstate->tokstr, DELIM);
  if (enumstate->tokstr)
  {
    enumstate->tokstr = NULL;
  }

  if (loclang != NULL)
  {
    if ((locdesc = GetLangName(loclang)) == NULL)
    {
      locdesc = loclang;
    }
    
    *lang = strdup(loclang);
    *desc = strdup(locdesc);

    rc = True;
  }
  return(rc);
}

/*
 * _enumLanglistEnd() - end enumeration of language names and descriptions
 *
 * Free memory.
 */
static void
_enumLanglistEnd(
  void *state)
{
  struct _enumState *enumstate = (struct _enumState *)state;

  free(enumstate->dupstr);
  free((char *)state); 
}

/***************************************************************************
 *
 *  Methods for language list
 *
 ***************************************************************************/

struct _enumObject
{
  Boolean (*methodNext)();
  void (*methodEnd)();
  void *enumstate;
};

/*
 * _enumLangStart() - start enumeration of languages and descriptions
 * 
 * ENABLE_DYNAMIC_LANGLIST defined
 *   Enumerate LANGLIST. If unsucessful, try 'LangCmd'. LANGLIST will only
 *   be set if user specified Dtlogin*languageList.
 *
 * ENABLE_DYNAMIC_LANGLIST undefined
 *   Enumerate LANGLIST.
 * 
 */
static void *
_enumLangStart(void)
{
  char *p;
  struct _enumObject *state = malloc(sizeof(struct _enumObject));
 
  if (state)
  { 
    state->enumstate = _enumLanglistStart();
    if (state->enumstate != NULL)
    {
      state->methodNext = _enumLanglistNext;
      state->methodEnd = _enumLanglistEnd;
    }

    #if defined (ENABLE_DYNAMIC_LANGLIST)
    if (state->enumstate == NULL)
    {
      state->enumstate = _enumLangCmdStart(); 
      if (state->enumstate != NULL)
      {
        state->methodNext = _enumLangCmdNext;
        state->methodEnd = _enumLangCmdEnd;
      }
    }
    #endif /* ENABLE_DYNAMIC_LANGLIST */

    if (state->enumstate == NULL)
    {
      free(state);
      state = NULL; 
    }
  }

  return((void *)state);
}

/*
 * _enumLangNext() - get next language description pair
 *                   note: *lang and *desc must be freed by free()
 */
static Boolean
_enumLangNext(
  void *state,
  char **lang,
  char **desc)
{
  Boolean rc;
  struct _enumObject *object = (struct _enumObject *)state;

  rc = (*object->methodNext)(object->enumstate, lang, desc);

  return(rc);
}

/*
 * _enumLangEnd() - end enumeration of language names and descriptions
 */
static void
_enumLangEnd(
  void *state)
{
  struct _enumObject *object = (struct _enumObject *)state;

  (*object->methodEnd)(object->enumstate);

  free((char *)state); 
}

/***************************************************************************
 *
 *  MakeLangMenu
 *
 *  Widgets: lang_menu
 *
 *  The language menu contains the list of locales available to the 
 *  the desktop session. This may be a subset of the actual installed
 *  locales. The list of locales to display in the language menu can
 *  be provided by the sysadmin, or determined by the login manager.
 *
 *  * Sysadmin provided language list
 *
 *    A sysadmin can set the Dtlogin.languageList resource to set the list
 *    of languages. The dtlogin process provides this list to dtgreet
 *    in the LANGLIST environment variable. This has priority.
 *
 *  * Login manager determined language list
 * 
 *    If the sysadm does not set Dtlogin.languageList, ie. LANGLIST unset, 
 *    the login manager will generate the list. There are two methods for
 *    doing this, one of which is selected at compile time with the
 *    ENABLE_DYNAMIC_LANGLIST define. 
 *
 *    * dynamic list (ENABLE_DYNAMIC_LANGLIST defined)
 *   
 *      This method executes the command specified by the 
 *      Dtlogin*languageListCmd resource. The default is
 *      /usr/dt/bin/dtlslocale. The languageListCmd command is expected
 *      to write to stdout a series of language names and descriptions:
 *        
 *        lang_name description
 *        lang_name description
 *        ...
 *        
 *        Example:
 * 
 *        En_US English (United States) - IBM-850
 *        Fr_BE French (Belgium) - IBM-850
 *
 *      Also, since languageListCmd is run under dtgreet's locale, a
 *      localized description can be returned.
 *
 *    * static list (ENABLE_DYNAMIC_LANGLIST undefined)
 *
 *      This method has dtlogin querying the system and generating
 *      the language list to be provided to dtgreet via the LANGLIST
 *      environment variable. In this case dtlogin takes care to use the
 *      sysadmin provided list if necessary. 
 *
 * * Language descriptions
 * 
 *   The sysadmin can set the Dtlogin*<lang>.languageName resource to 
 *   provide a descriptive name for a particular language. If languageName 
 *   unset, the value used depends on ENABLE_DYNAMIC_LANGLIST. If 
 *   ENABLE_DYNAMIC_LANGLIST set, the value used is the descriptive text
 *   provided by languageListCmd. If ENABLE_DYNAMIC_LANGLIST unset, the value
 *   used is simply the locale name.
 *
 * * Default language
 *  
 *   The sysadmin can set the Dtlogin*language resource to specify the
 *   default language in the language menu.
 *
 ***************************************************************************/

#define MAX_LANG_ITEMS	16	/* maximum number of items in one lang menu */
#define MAX_NAME_LEN	128	/* maximum length of a language name	   */

struct Langlist {
  char *lang;                       /* lang name ie En_US, Ja_JP       */
  char *desc;                       /* lang description ie English     */
};

/*
 * compareLangDesc() - compare language descriptions in qsort()
 */
static int
compareLangDesc(
  const void *first, 
  const void *second)
{
  return(strcmp(((struct Langlist *)first)->desc, 
                ((struct Langlist *)second)->desc));
}


void 
MakeLangMenu( void )
{
    int i, k;
    char	cblab[MAX_NAME_LEN];	/* pushbutton label		   */
    int		nlang;			/* total number of languages	   */
    int		nlangMenus;		/* number of language sub-menus	   */
    int		maxitems;		/* max no. of items in sub-lang menu*/
    Widget	item_menu;
    Widget	button;
    char	*tostr;

    struct Langlist {
      char *lang;                       /* lang name ie En_US, Ja_JP       */
      char *desc;                       /* lang description ie English     */
    };

    struct Langlist list[500];
    void *state;

   /*
    * Generate list of langname/description pairs.
    */
    nlang = 0;
    state = _enumLangStart();
    if (state)
    {
      while (_enumLangNext(state, &list[nlang].lang, &list[nlang].desc))
      {
        nlang++;
      }
      _enumLangEnd(state);
    } 

    if (nlang > 0) {

       /*
        * Sort by description
        */
        qsort((char *)list, nlang, sizeof(list[0]), compareLangDesc);

	/*
	 *  determine number of language sub-menus ...
	 *  (MAX_LANG_ITEMS per menu)
	 */
	 
	nlangMenus = 0;
	do {
	    nlangMenus++;
	    maxitems = nlang/nlangMenus;
	} while ( maxitems > MAX_LANG_ITEMS );
	if (nlang%nlangMenus != 0) maxitems++;	/* allow for stragglers	   */


	/*
	 *  build language menu(s)...
	 */
	i = 0;
	lang_menu = XmCreatePulldownMenu(options_menu, "lang_menu", argt, i);
	item_menu = lang_menu;

	for (k = 0; k < nlang; k++) {

	    if ( nlangMenus > 1 && k%maxitems == 0) {
		i = 0;
		item_menu = XmCreatePulldownMenu(lang_menu, "item_menu",
						 argt, i);
	    }

	    /*
	     *  build toggle buttons...
	     */
	    i = InitArg(ToggleBG);
	    XtSetArg(argt[i], XmNrecomputeSize,		True		); i++;
            if (langenv && (strcmp(langenv, list[k].lang) == 0)) {
              XtSetArg(argt[i], XmNset,		True		); i++;
            }
    
            xmstr = XmStringCreateLocalized(list[k].desc);
            XtSetArg(argt[i], XmNlabelString,	xmstr		); i++;

            button = XmCreateToggleButtonGadget(
              item_menu, list[k].lang, argt, i);
            XtAddCallback (button, XmNvalueChangedCallback,
              RespondLangCB, (XtPointer) list[k].lang);
            XmStringFree(xmstr);

	    XtManageChild(button);
            tostr =
	      (char*) ReadCatalog(MC_LABEL_SET, MC_TO_LABEL, MC_DEF_TO_LABEL);

	    if ( nlangMenus > 1 && k%maxitems == 0 ) {
                int first = k;
                int last = k+maxitems >= nlang ? nlang-1 : k+maxitems-1;

		i = InitArg(CascadeBG);

		sprintf(cblab, "%s %s %s", 
			list[first].desc, tostr, list[last].desc);

		xmstr = XmStringCreateLocalized(cblab);
		XtSetArg(argt[i], XmNlabelString,	xmstr		); i++;
		XtSetArg(argt[i], XmNsubMenuId,		item_menu	); i++;
		XtSetArg(argt[i], XmNrecomputeSize,	True		); i++;
		button = XmCreateCascadeButtonGadget(lang_menu, cblab, argt, i);
		XtManageChild(button);
		XmStringFree(xmstr);
	    }
	}
       /*
        * Free language list
        */
        for (k = 0; k < nlang; k++)
        {
          /* free(list[k].lang); -- used as callback data, don't free */
          free(list[k].desc);
        }
    }
}
