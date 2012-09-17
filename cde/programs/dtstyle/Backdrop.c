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
/* $TOG: Backdrop.c /main/7 1998/11/25 14:48:36 samborn $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Backdrop.c
 **
 **   Project:     DT 3.0 
 **
 **   Description: Controls the Dtstyle Backdrop dialog
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990, 1993.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

/*+++++++++++++++++++++++++++++++++++++++*/
/* include files                         */
/*+++++++++++++++++++++++++++++++++++++++*/
#include <stdio.h>
#include <sys/types.h>

#if defined(_AIX) || defined(__apollo)
#include <sys/dir.h>
#else
#include <dirent.h>             /* opendir(), directory(3C) */
#endif /* _AIX */

#ifdef __apollo
#include <X11/apollosys.h>      /* needed for S_ISDIR macro */
#endif


#include <locale.h>
#include <X11/Xlib.h>
#include <Xm/MwmUtil.h>

#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/DrawnB.h>
#include <Xm/Form.h>
#include <Xm/List.h>
#include <Xm/VendorSEP.h>
#include <Xm/AtomMgr.h>

#include <Dt/DialogBox.h>

#include <Dt/Message.h>
#include <Dt/SessionM.h>
#include <Dt/HourGlass.h>
#include <Dt/Wsm.h>
#include <Dt/UserMsg.h>

#include "Help.h"
#include "Main.h"
#include "SaveRestore.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* include extern functions              */
/*+++++++++++++++++++++++++++++++++++++++*/
#include "Backdrop.h"


/*+++++++++++++++++++++++++++++++++++++++*/
/* Local #defines                        */
/*+++++++++++++++++++++++++++++++++++++++*/
#define MAX_STR_LEN         128
#define B_OK_BUTTON           1
#define B_APPLY_BUTTON        2
#define B_CANCEL_BUTTON       3
#define B_HELP_BUTTON         4

#define ERR2   ((char *)GETMESSAGE(11, 2, "The backdrop portion of the Style Manager\n\
will not operate because there are no backdrop\nfiles available. Check $HOME/.dt/errorlog."))

/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Functions                    */
/*+++++++++++++++++++++++++++++++++++++++*/

static int CreateBackdropDialog( Widget parent) ;
static void MoreBitmaps( void ) ;
static ReadBitmaps( void ) ;
static Boolean CreatePixmaps( void ) ;
static ReadBitmapDirectory( char *dir ) ;
static void DrawBitmap( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void SizeBitmap( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static XmString * MakeListStrings( void ) ;
static void FreeListStrings( XmString *listPtr) ;
static void ListCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void ButtonCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void GetColors( void ) ;
static void FreeAll( void ) ;
static void _DtMapCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;


/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Variables                    */
/*+++++++++++++++++++++++++++++++++++++++*/

typedef struct {
    Widget   drawnButton;
    char   **dirList;
    int      dirCount;
    char   **tmpBitmapNames;
    int      tmpNumBitmaps;
    int      tmpMaxNumBitmaps;
    char   **bitmapNames;
    char   **bitmapDescs;
    Pixmap  *bitmaps;
    int      numBitmaps;
    int      maxNumBitmaps;
    int      selected;
    GC       gc;
    int      width, height;
    int      shadow;
    Pixel    fg, bg;
    char    *errStr;
    char     noBitmaps;
    Boolean  newColors;
} Backdrops, *BackdropsPtr;
static Backdrops backdrops; 

static saveRestore save = {FALSE, 0, };
char *BACKDROPSDLG = "backdropsDialog";

/*
 *   copy of the system backdrop description file for the 
 *   current locale in xrm form
 */
static XrmDatabase sys_bd_DB = NULL;

/*
 *   copy of the admin backdrop description file for the 
 *   current locale in xrm form
 */
static XrmDatabase adm_bd_DB = NULL;

/*
 *   copy of the user's home backdrop description file for the 
 *   current locale in xrm form
 */
static XrmDatabase hm_bd_DB = NULL;

/*
 *   final combination of the admin & system data bases 
 */
static XrmDatabase bd_DB = NULL;



/*+++++++++++++++++++++++++++++++++++++++*/
/* build_dirList                         */
/*+++++++++++++++++++++++++++++++++++++++*/

char ** 
build_dirList(char * dirStr, 
		 int * count)
{
   char tokenSep[] = ":";
   char * token;
   char ** dirList = NULL;
   register int i = 0;
   char * tmpStr;
   int len = strlen(dirStr);
   *count = 0;

   tmpStr = (char *)XtCalloc(1, len + 1);
   strcpy(tmpStr, dirStr);
   token = strtok(tmpStr, tokenSep);
   while(token != NULL)
     {
       ++(i);
       token = strtok(NULL, tokenSep);
     }

   if (i == 0)
       return (NULL);

   dirList = (char **) XtCalloc(1, i * sizeof(char *));
   if( dirList )
     {
       strcpy(tmpStr, dirStr); 
       token = strtok(tmpStr, tokenSep);
       *count=0;
       while(token != NULL)
	 {
	   dirList[*count] = (char *) XtCalloc(1, strlen( token ) + 1);
	   strcpy(dirList[*count], token);
	   token = strtok(NULL, tokenSep);
	   ++(*count);
	 }
     }

   XtFree ((char *) tmpStr);
   return(dirList);
 }



/*+++++++++++++++++++++++++++++++++++++++*/
/* free_dirList                          */
/*+++++++++++++++++++++++++++++++++++++++*/

void
free_dirList(char ** dirList, 
		    int count)

{
  register int   i;

  if (dirList == NULL)
    return;
  for (i=0; i<count; i++)
    XtFree((char *) dirList[i]);
  
  XtFree ((char *) dirList);
}


/************************************************************************
 *  SelectCurrentBackdrop() - Selects current backdrop in list
 *
 ************************************************************************/
void SelectCurrentBackdrop(callback)
{
    DtWsmWorkspaceInfo   *wInfo=NULL;
    Atom	     aWS;
    Widget list;
    char *backdropName;
    int i;

    if ((DtWsmGetCurrentWorkspace (style.display, style.root, &aWS) 
	 	!= Success) ||
	(DtWsmGetWorkspaceInfo (style.display, style.root, aWS, &wInfo)
	 	!= Success))
    {
      return;
    }
      
    list = XtNameToWidget(style.backdropDialog, "*bitmapList");

    backdropName = XmGetAtomName(style.display, wInfo->backdropName);

    for (i = 0; i < backdrops.numBitmaps; i++) {
      if (strcmp(backdrops.bitmapNames[i], backdropName) == 0) {
	XmListSelectPos (list, i + 1, callback);
	XmListSetPos(list, i + 1);
	backdrops.selected = i;
      }
    }

    XtFree((char *) backdropName);
    XtFree((char *) wInfo);
}

/************************************************************************
 *  BackdropDialog() - Create backdrop selection dialog first time up.
 *           If it has already been created, map it.
 ************************************************************************/
void 
BackdropDialog(
        Widget parent )
{
    int i;
    
    if (style.backdropDialog == NULL) 
    {
        _DtTurnOnHourGlass(parent);  
        if (!CreateBackdropDialog(parent)) { 
           _DtTurnOffHourGlass(parent);  
           return;
        }
	SelectCurrentBackdrop(False);
        XtManageChild(style.backdropDialog);
        XSync(style.display, 0);
        XmUpdateDisplay(style.backdropDialog);
        _DtTurnOffHourGlass(parent);  

    }
    else
    {
	SelectCurrentBackdrop(True);
        XtManageChild(style.backdropDialog);
        raiseWindow(XtWindow(XtParent(style.backdropDialog)));
        XmUpdateDisplay(style.backdropDialog);
    }
}


/************************************************************************
 *   CreateBackdropDialog()
 *           Create the Backdrop Dialog
 ************************************************************************/
static int 
CreateBackdropDialog(
        Widget parent )
{
    register int     i, n;
    Arg              args[20];
    Widget           mainForm;
    Widget           list;
    XmString         strings[NUM_LABELS+1];
    XmString        *listStrings;
    char            *bd_desc;
    char            *lang;


    if (backdrops.noBitmaps) 
    {
        ErrDialog (backdrops.errStr, style.shell);
        return 0;
    }

    /* initialize backdrop data */
    backdrops.bitmapNames = NULL;
    backdrops.bitmaps = NULL;
    backdrops.numBitmaps = 0;
    backdrops.maxNumBitmaps = 100;
    backdrops.selected = -1;
    backdrops.gc = NULL;
    backdrops.errStr = NULL;
    backdrops.shadow = 2;
    backdrops.width = 200 - 2*backdrops.shadow;
    backdrops.height = 200 - 2*backdrops.shadow;
    backdrops.newColors = True;

    
    /* load the backdrop description data base for the given locale*/
    /* from that locale's description file from the system location */
    lang = setlocale (LC_CTYPE,NULL);

#ifdef hpux      /* hpux-specific parsing of the locale string */
                 /* The following code is identical to the
                    ExtractLocaleName function in WmResParse.c
                    from dtwm
                 */
#define MAXLOCALE       64      /* buffer size of locale name */

{   char           *start;
    char           *end;
    int             len;
    static char     buf[MAXLOCALE];

    /*  If lang has a substring ":<category>;", extract <category>
     *  from the first such occurrence as the locale name.
     */

    start = lang;
    if (start = strchr (lang, ':')) {
        start++;
        if (end = strchr (start, ';')) {
            len = end - start;
            strncpy(buf, start, len);
            *(buf + len) = '\0';
            lang = buf;
      }
    }
}
#endif  /* hpux */

    bd_desc = (char *)XtMalloc(strlen("/usr/dt/backdrops/desc.") + strlen(lang) + 1);
    strcpy (bd_desc,"/usr/dt/backdrops/desc.");
    strcat (bd_desc, lang);
    if(sys_bd_DB = XrmGetFileDatabase (bd_desc))
      	XrmMergeDatabases(sys_bd_DB, &bd_DB);
    XtFree(bd_desc);
    
    /* load the backdrop description data base for the given locale*/
    /* from that locale's description file from the admin location */
    bd_desc = (char *)XtMalloc(strlen("/etc/dt/backdrops/desc.") + strlen(lang) + 1);
    strcpy (bd_desc,"/etc/dt/backdrops/desc.");
    strcat (bd_desc, lang);
    if (adm_bd_DB = XrmGetFileDatabase (bd_desc))
	XrmMergeDatabases(adm_bd_DB, &bd_DB);
    XtFree(bd_desc);

    /* load the backdrop description from the user's .dt/backdrops directory */
    /* regardless of locale */
    bd_desc = (char *)XtMalloc(strlen(style.home) + strlen("/.dt/backdrops/desc.backdrops") + 1);
    strcpy (bd_desc, style.home);
    strcat (bd_desc, "/.dt/backdrops/desc.backdrops");
    if (hm_bd_DB = XrmGetFileDatabase (bd_desc))
	XrmMergeDatabases(hm_bd_DB, &bd_DB);
    XtFree(bd_desc);

    /* Set up DialogBox button labels. */
    strings[0] = XmStringCreateLocalized (_DtOkString);
    strings[1] = XmStringCreateLocalized (_DtApplyString);
    strings[2] = XmStringCreateLocalized (_DtCloseString);
    strings[3] = XmStringCreateLocalized (_DtHelpString);

    /* saveRestore
     * Note that save.poscnt has been initialized elsewhere.
     * save.posArgs may contain information from restoreBackdrop().*/

    /* create the dialog box with shell */

    XtSetArg (save.posArgs[save.poscnt], XmNbuttonCount, NUM_LABELS+1);
                                                                 save.poscnt++;
    XtSetArg (save.posArgs[save.poscnt], XmNbuttonLabelStrings, strings);
                                                                 save.poscnt++;
    XtSetArg (save.posArgs[save.poscnt], XmNdefaultPosition, False);
                                                                 save.poscnt++;
    XtSetArg (save.posArgs[save.poscnt], XmNallowOverlap, False);
                                                                 save.poscnt++;
    style.backdropDialog = __DtCreateDialogBoxDialog (parent, BACKDROPSDLG,
                                                     save.posArgs, save.poscnt);
    XtAddCallback(style.backdropDialog, XmNcallback, ButtonCB, NULL);
    XtAddCallback(style.backdropDialog, XmNmapCallback, _DtMapCB, parent);
    XtAddCallback(style.backdropDialog, XmNhelpCallback,
            (XtCallbackProc)HelpRequestCB, (XtPointer)HELP_BACKDROP_DIALOG);

    /* free compound strings now */
    XmStringFree (strings[0]);
    XmStringFree (strings[1]);
    XmStringFree (strings[2]);
    XmStringFree (strings[3]);

    n = 0;
    XtSetArg (args[n], XmNtitle, ((char *)GETMESSAGE(11, 12, "Style Manager - Backdrop"))); n++;
    XtSetArg (args[n], XmNuseAsyncGeometry, True); n++;
    XtSetValues (XtParent(style.backdropDialog), args, n);

    /*  get bitmap data */
    if (!ReadBitmaps())  return 0;         /* uses style.backdropDialog */

    /* create the form to go in to dialog box as the work area */
    n = 0;
    XtSetArg(args[n], XmNhorizontalSpacing, style.horizontalSpacing); n++;
    XtSetArg(args[n], XmNverticalSpacing, style.verticalSpacing); n++;
    XtSetArg (args[n], XmNchildType, XmWORK_AREA);  n++;

    XtSetArg (args[n], XmNallowOverlap, False);  n++;
    mainForm = XmCreateForm (style.backdropDialog, "backdropsForm", args, n);

    /* create the scrolled list of bitmap names... first create XmStrings */
    listStrings = MakeListStrings ();
    n = 0;
    XtSetArg (args[n], XmNautomaticSelection, True);              n++;
    XtSetArg (args[n], XmNselectionPolicy, XmBROWSE_SELECT);      n++;
    XtSetArg (args[n], XmNitems, listStrings);                    n++;
    XtSetArg (args[n], XmNitemCount, backdrops.numBitmaps);       n++;
    list = XmCreateScrolledList (mainForm, "bitmapList", args, n);
    XtAddCallback (list, XmNbrowseSelectionCallback, ListCB, (XtPointer)NULL);
    FreeListStrings (listStrings);            /* after list has copied */

    /* set up attachments for scrolled list itself */
    n = 0;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);          n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);        n++;
    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);       n++;
    XtSetValues (XtParent(list), args, n);

    /*   Create drawing area for the bitmap  */
    n = 0;
    XtSetArg (args[n], XmNshadowType, XmSHADOW_IN);                     n++;
    XtSetArg (args[n], XmNshadowThickness, backdrops.shadow);           n++;
    XtSetArg (args[n], XmNhighlightThickness, 0);                       n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);            n++;
    XtSetArg (args[n], XmNrightWidget, XtParent(list));                 n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);                n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);               n++;
    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);             n++;
    XtSetArg (args[n], XmNborderWidth, 0);                              n++;
    XtSetArg (args[n], XmNwidth, backdrops.width+2*backdrops.shadow);   n++;
    XtSetArg (args[n], XmNheight, backdrops.height+2*backdrops.shadow); n++;
    XtSetArg (args[n], XmNtraversalOn, False);                          n++;  
    backdrops.drawnButton = XmCreateDrawnButton (mainForm, "bitmap", args, n);
    XtAddCallback (backdrops.drawnButton, XmNexposeCallback, DrawBitmap, NULL);
    XtAddCallback (backdrops.drawnButton, XmNresizeCallback, SizeBitmap, NULL);

    /* manage all of the widgets */
    XtManageChild (mainForm);
    XtManageChild (backdrops.drawnButton);
    XtManageChild (list);

    return 1;
}


/************************************************************************
 *   MoreBitmaps()
 *           Create space for more bitmap entries
 ************************************************************************/
static void 
MoreBitmaps( void )
{
    int   newSize;

    /* allocate space for icon names */
    newSize =  (backdrops.maxNumBitmaps + 100) * sizeof(char *);
    backdrops.bitmapNames = (char **) XtRealloc((char *)backdrops.bitmapNames, 
                                                newSize);

    /* now allocate new bitmap space */
    newSize =  (backdrops.maxNumBitmaps + 100) * sizeof(Pixmap);
    backdrops.bitmaps = (Pixmap *)XtRealloc((char *)backdrops.bitmaps, newSize);

    backdrops.maxNumBitmaps += 100;
}

/************************************************************************
 *   ReadBitmaps()
 *   Create an array of bitmaps by reading backdrop directories in the
 *   following order overriding any duplicates:
 *   1) Read the system location /usr/dt/backdrops
 *   2) Read the admin location /etc/dt/backdrops
 *   3) Read the directories specified by the backdropDirectories
 *      resource.
 *   4) Read the user's home directory /$HOME/.dt/backdrops.
 ************************************************************************/
static 
     ReadBitmaps( void )
{
  int            status;
  Pixmap         tmpPix = 0;
  int            width, height, x, y;
  Window         win;
  int            num;
  register int   i;   
  char          *string;
  /* allocate space for temporary bitmap info */
  backdrops.tmpBitmapNames = (char **)XtCalloc(100, sizeof(char *));
  backdrops.tmpMaxNumBitmaps = 100;
  backdrops.tmpNumBitmaps = 0;
  
  /* read system backdrop directory */
    ReadBitmapDirectory("/usr/dt/backdrops");
  /* read sys admin backdrop directory */
  ReadBitmapDirectory("/etc/dt/backdrops");
  
  /* Parse the backdropDirectories resource to get the individual directories */
  if (style.xrdb.backdropDir)
    {
      backdrops.dirList = build_dirList(style.xrdb.backdropDir, &backdrops.dirCount);
      
      /* compile the list of bitmaps */
      for (i=0; i<backdrops.dirCount; i++)
	ReadBitmapDirectory(backdrops.dirList[i]);
    }
  
  /* read the directory $HOME/.dt/backdrops */
  string = (char *)XtMalloc(strlen(style.home) + strlen("/.dt/backdrops") + 1);
  sprintf(string, "%s/.dt/backdrops", style.home);
  ReadBitmapDirectory(string);
  if (string != NULL)
    XtFree(string);

  if (backdrops.tmpNumBitmaps == 0)
    {
      /* give error dialog, free space, and return */
      backdrops.errStr = (char *)XtMalloc(strlen(ERR2) + 1);
      sprintf(backdrops.errStr, "%s", ERR2);
      ErrDialog (backdrops.errStr, style.shell); 
      FreeAll();      
      free_dirList(backdrops.dirList, backdrops.dirCount);
      return 0;
    }
  
  /* get the fg/bg colors from Dtwm */
  if (backdrops.newColors)
    {
      GetColors();
      backdrops.newColors = False;
    }   
  
    /* create all the pixmaps */
  if (!CreatePixmaps())
    {
      /* give error dialog, free space, and return */
      backdrops.errStr = (char *)XtMalloc(strlen(ERR2) + 1);
      sprintf(backdrops.errStr, "%s", ERR2);
      ErrDialog (backdrops.errStr, style.shell); 
      FreeAll();  
      free_dirList(backdrops.dirList, backdrops.dirCount);
      return 0;
    }
  
  
  if (backdrops.selected == -1) backdrops.selected = 0;   
  
  return 1;
}



/************************************************************************
 *   CreatePixmaps()
 *           Create the pixmpas in the backdrop list
             with workprocs 10 at a time
 ************************************************************************/
static Boolean
CreatePixmaps( void )

{
    static int     pixmapsCreated=0;
    int            i;
    Pixmap         tmpPixmap;
    
    backdrops.numBitmaps = 0;

    /* allocate space for real bitmap info */
    backdrops.bitmapNames = (char **)XtCalloc(100, sizeof(char *));
    backdrops.bitmaps = (Pixmap *)XtCalloc(100, sizeof(Pixmap));

    for (i=0; i<backdrops.tmpNumBitmaps; i++)
    {
        tmpPixmap = XmGetPixmap (style.screen, 
                                 backdrops.tmpBitmapNames[i], 
                                 backdrops.fg, backdrops.bg); 
        if (tmpPixmap != XmUNSPECIFIED_PIXMAP)
        {
            if (backdrops.numBitmaps == backdrops.maxNumBitmaps)
                MoreBitmaps();

            backdrops.bitmapNames[backdrops.numBitmaps] = 
                    backdrops.tmpBitmapNames[i];
            backdrops.bitmaps[backdrops.numBitmaps] = tmpPixmap;
 
            backdrops.numBitmaps++;
        }

    }
    if (backdrops.numBitmaps)
        return(True);
    else
        return(False);

}


/************************************************************************
 *   ReadBitmapDirectory()
 *           Create an array of bitmap names overriding duplicates
 ************************************************************************/
static
ReadBitmapDirectory( 
    char *dir )

{
    DIR            *dirp;
    struct dirent  *filep;
    int             i;
    Boolean         duplicate;
    char           *name;
    int             stat_result;
    struct stat     stat_buf;
    char           *statPath, *pStatPath;
    int             newSize;

    /* open the backdrops directory */
    if ((dirp = opendir(dir)) == NULL)
    {
        /* print message to errorlog, free space, and return */
      return 0;
    }
    
    /* create string to contain complete path */
    statPath = (char *) XtMalloc(strlen(dir) + MAX_STR_LEN + 2);
    strcpy (statPath, dir);
    strcat (statPath, "/");
    pStatPath = statPath + strlen(statPath);

    filep = readdir(dirp);

    while (filep != NULL)
    {
        /* append filename to stat path */
        strcpy (pStatPath, filep->d_name);

        /* stat the file */
        if ((stat_result = stat (statPath, &stat_buf)) != 0)
        {
            filep = readdir(dirp);
            continue;
        }
            
        /* skip directories */
        if ((stat_buf.st_mode & S_IFMT) == S_IFDIR)
        {
            filep = readdir(dirp);
            continue; 
        }

	
        /* strip suffix off filename */
        name = (char *) XtMalloc(strlen(filep->d_name) + 1);
        strcpy (name, filep->d_name);
        (void)strtok(name, ".");

        /* check for duplicates */
        duplicate = 0;
        for (i=0; i<backdrops.tmpNumBitmaps; i++)
        {
            if (!strcmp(backdrops.tmpBitmapNames[i], name))
            {
                duplicate = 1;
                break;                
            }
        }
        
        if (!duplicate)
        {
            /* add to the temporary bitmap list */

            if (backdrops.tmpNumBitmaps == backdrops.tmpMaxNumBitmaps)
            {
                /* allocate space for more temporary bitmap info */
                newSize =  (backdrops.tmpMaxNumBitmaps + 100) * sizeof(char *);
                backdrops.tmpBitmapNames = 
                    (char **)XtRealloc((char *)backdrops.tmpBitmapNames, newSize);
                backdrops.tmpMaxNumBitmaps += 100;
            }

            backdrops.tmpBitmapNames[backdrops.tmpNumBitmaps] = 
                (char *) XtMalloc(strlen(name)+1);
            strcpy (backdrops.tmpBitmapNames[backdrops.tmpNumBitmaps], name);

            backdrops.tmpNumBitmaps++;
        }

        filep = readdir(dirp);
	XtFree(name);
    }

    XtFree(statPath);


    closedir (dirp);
    return 1;
}


/************************************************************************
 *   DrawBitmap()
 *           This is the exposeCallback for the bitmap drawing area.
 ************************************************************************/
static void 
DrawBitmap(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    XGCValues     gcValues;
    Arg           args[3];
        
    if (backdrops.selected == -1)
        return;

    if (backdrops.newColors)
    {
        GetColors();

        /* we could keep track of which tile pixmaps need to be updated
           since the last workspace change, but for now simply regenerate 
           each pixmap as it is selected after a workspace change has 
           occurred */

        /* backdrops.newColors = False; */
    }

    if (backdrops.gc == NULL)
    {
        gcValues.background = backdrops.bg;
        gcValues.foreground = backdrops.fg;
        gcValues.fill_style = FillTiled;
        gcValues.tile = backdrops.bitmaps[backdrops.selected];

        backdrops.gc = XCreateGC (style.display, XtWindow(w), 
                                GCForeground | GCBackground | 
                                GCTile | GCFillStyle, &gcValues);
    }

    XFillRectangle (style.display, XtWindow(w), backdrops.gc, backdrops.shadow,
                    backdrops.shadow, backdrops.width, backdrops.height);
}


/************************************************************************
 *   SizeBitmap()
 *           This is the resizeCallback for the bitmap drawing area.
 ************************************************************************/
static void 
SizeBitmap(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    backdrops.width = XtWidth(w) - 2*backdrops.shadow;
    backdrops.height = XtHeight(w) - 2*backdrops.shadow;
}


/************************************************************************
 *   MakeListStrings()
 *           Make XmStrings from the bitmap descriptions, to pass into list.
 *  
 ************************************************************************/
static XmString * 
MakeListStrings( void )
{
    int         i;
    XmString   *list;
    char       *name_str;
    char       *class_str;
    char       *str_type_return;
    XrmValue    value_return;
    
    /* allocate space for bitmap descriptions */
    backdrops.bitmapDescs = (char **)XtCalloc(backdrops.numBitmaps, sizeof(char *));
    
    for (i=0; i<backdrops.numBitmaps; i++)
      {
	if (bd_DB !=NULL)
	  {
	    name_str = (char *) XtMalloc(strlen("backdrops.") + 
					 strlen(backdrops.bitmapNames[i]) +
					 strlen(".desc") + 1);
	    
	    class_str = (char *) XtMalloc(strlen("Backdrops.") + 
					  strlen(backdrops.bitmapNames[i]) + 
					  strlen(".Desc") + 1);
	    strcpy(name_str, "backdrops.");
	    strcpy(class_str, "Backdrops.");
	    strcat(name_str, backdrops.bitmapNames[i]);
	    strcat(class_str, backdrops.bitmapNames[i]);
	    strcat(name_str, ".desc");
	    strcat(class_str, ".Desc");

	    if (XrmGetResource (bd_DB, name_str, class_str, &str_type_return, &value_return))
	      {
		/* make copy of resource value */
		backdrops.bitmapDescs[i] = (char *) XtMalloc(value_return.size + 1);
		strcpy (backdrops.bitmapDescs[i], value_return.addr);
	      }    
	    else
	      {	  
		backdrops.bitmapDescs[i] = (char *) XtMalloc(strlen(backdrops.bitmapNames[i]) + 1);
		strcpy(backdrops.bitmapDescs[i], backdrops.bitmapNames[i]);
	      }
	  }
	else
	  {	  
	    backdrops.bitmapDescs[i] = (char *) XtMalloc(strlen(backdrops.bitmapNames[i]) + 1);
	    strcpy(backdrops.bitmapDescs[i], backdrops.bitmapNames[i]);
	  }
      }

    list = (XmString *) XtCalloc(backdrops.numBitmaps, sizeof(XmString));
    
    for (i = 0; i < backdrops.numBitmaps; i++)
    {
        list[i] = XmStringCreateLocalized (backdrops.bitmapDescs[i]);
    }

    return (list);
}


/************************************************************************
 *   FreeListStrings()
 *           Free XmStrings from the bitmap names, passed into list.
 ************************************************************************/
static void 
FreeListStrings(
        XmString *listPtr )
{
    int         i;
    int         n;
    XmString   *list = listPtr;

    for (i = 0; i < backdrops.numBitmaps; i++)
    {
        if (list[i]) XmStringFree(list[i]);
    }
    XtFree ((char *)list);
}


/************************************************************************
 *   ListCB()
 *           Get the bitmap selected from the list
 ************************************************************************/
static void 
ListCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    XmListCallbackStruct  *cb = (XmListCallbackStruct *)call_data;

    backdrops.selected = cb->item_position - 1;

    XSetTile (style.display, backdrops.gc, 
	      backdrops.bitmaps[backdrops.selected]);

    DrawBitmap (backdrops.drawnButton, NULL, NULL);
}



/************************************************************************
 *   ButtonCB()
 *          
 ************************************************************************/
static void 
ButtonCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    int      n, num;
    Arg      args[MAX_ARGS];

    DtDialogBoxCallbackStruct *cb = (DtDialogBoxCallbackStruct *) call_data;

    switch (cb->button_position)
    {
      case B_APPLY_BUTTON:
          /* send message to update backdrop */

          num = backdrops.selected;

          _DtWsmChangeBackdrop(style.display, style.root, 
                             backdrops.bitmapNames[num], 
                             backdrops.bitmaps[num]);
          break;

      case B_OK_BUTTON:  
          /* send message to update backdrop */

          num = backdrops.selected;

          _DtWsmChangeBackdrop(style.display, style.root,
                             backdrops.bitmapNames[num],
                             backdrops.bitmaps[num]);
          XtUnmanageChild(w);
          break;

      case B_CANCEL_BUTTON:            /* close */
          XtUnmanageChild(w);
          break;

      case B_HELP_BUTTON:
          XtCallCallbacks(style.backdropDialog, XmNhelpCallback, (XtPointer)NULL);
          break;

      default:
          break;
    }
}


/************************************************************************
 *   CheckWorkspace()
 *           Workspace may have changed, so get current workspace
 *           colors and draw the backdrop bitmap
 *          
 ************************************************************************/
void 
CheckWorkspace( void )
{
    backdrops.newColors = True;         /* need to get new colors */
    if (style.backdropDialog && XtIsManaged(style.backdropDialog))
    {
        DrawBitmap (backdrops.drawnButton, NULL, NULL);
    }
}


/************************************************************************
 *   GetColors()
 *           Get current workspace colors, and update GC if needed
 *          
 ************************************************************************/
static void 
GetColors( void )
{
    DtWsmWorkspaceInfo   *wInfo=NULL;
    unsigned long    num=0;
    Pixel            fg, bg;
    XGCValues        gcValues;
    Atom	     aWS;

    if ((DtWsmGetCurrentWorkspace (style.display, style.root, &aWS) 
	 	== Success) &&
	(DtWsmGetWorkspaceInfo (style.display, style.root, aWS, &wInfo)
	 	== Success))
    {
        backdrops.bg = wInfo->bg;
        backdrops.fg = wInfo->fg;
        DtWsmFreeWorkspaceInfo (wInfo);
    }
    else 
    {
        backdrops.bg = 0;
        backdrops.fg = 1;
    }

    if (backdrops.gc)      /* update the gc if there is one */
    {
        gcValues.background = backdrops.bg;
        gcValues.foreground = backdrops.fg;

        /* free old pixmap */
        XmDestroyPixmap(style.screen, 
                        backdrops.bitmaps[backdrops.selected]);

        /* allocate new pixmap */
        backdrops.bitmaps[backdrops.selected] = 
            XmGetPixmap (style.screen, 
                         backdrops.bitmapNames[backdrops.selected], 
                         backdrops.fg, backdrops.bg); 

        gcValues.tile = backdrops.bitmaps[backdrops.selected];

        XChangeGC (style.display, backdrops.gc, 
                   GCForeground | GCBackground | GCTile, &gcValues);
    }
}


/************************************************************************
 * FreeAll()
 *        Free some space that was allocated for backdrops
 ************************************************************************/
static void 
FreeAll( void )
{
    int i;

    /* set no bitmaps flag, so we won't try to get them next time */ 
    backdrops.noBitmaps = 1;

    /* free temporary list of backdrop names */
    for (i = 0; i < backdrops.tmpNumBitmaps; i++)
        if (backdrops.tmpBitmapNames[i]) 
            XtFree(backdrops.tmpBitmapNames[i]);
    XtFree ((char *)backdrops.tmpBitmapNames);
    XtFree ((char *)backdrops.bitmapNames);

    /* free backdrop bitmaps */
    for (i = 0; i < backdrops.numBitmaps; i++) {
        if (backdrops.bitmaps[i]) 
            XFreePixmap (style.display, backdrops.bitmaps[i]);
    if (backdrops.numBitmaps)
        XtFree((char *)backdrops.bitmaps);  
    }

    /* destory widgets (via first parent) */
    XtDestroyWidget (XtParent(style.backdropDialog));
    style.backdropDialog = NULL;
}


/************************************************************************
 * _DtMapCB
 *
 ************************************************************************/
static void 
_DtMapCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{

    DtWsmRemoveWorkspaceFunctions(style.display, XtWindow(XtParent(w)));

    if (!save.restoreFlag)
        putDialog((Widget)client_data, w);

    XtRemoveCallback(style.backdropDialog, XmNmapCallback, _DtMapCB, NULL);
}


/************************************************************************
 * restoreBackdrop()
 *
 * restore any state information saved with saveBackdrop.
 * This is called from restoreSession with the application
 * shell and the special xrm database retrieved for restore.
 ************************************************************************/
void 
restoreBackdrop(
        Widget shell,
        XrmDatabase db )
{
    XrmName xrm_name[5];
    XrmRepresentation rep_type;
    XrmValue value;

    xrm_name [0] = XrmStringToQuark (BACKDROPSDLG);
    xrm_name [2] = 0;

    /* get x position */
    xrm_name [1] = XrmStringToQuark ("x");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value)) {
        XtSetArg (save.posArgs[save.poscnt], XmNx, atoi((char *)value.addr)); 
        save.poscnt++;
        save.restoreFlag = True;
    }

    /* get y position */
    xrm_name [1] = XrmStringToQuark ("y");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value)) {
        XtSetArg (save.posArgs[save.poscnt], XmNy, atoi((char *)value.addr)); 
        save.poscnt++;
    }

    /* get width */
    xrm_name [1] = XrmStringToQuark ("width");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value)) {
        XtSetArg(save.posArgs[save.poscnt], XmNwidth, atoi((char *)value.addr));
        save.poscnt++;
    }

    /* get height */
    xrm_name [1] = XrmStringToQuark ("height");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value)) {
        XtSetArg(save.posArgs[save.poscnt],XmNheight, atoi((char *)value.addr));
        save.poscnt++;
    }

    xrm_name [1] = XrmStringToQuark ("ismapped");
    XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
    /* Are we supposed to be mapped? */
    if (strcmp(value.addr, "True") == 0)
        BackdropDialog(shell);
}


/************************************************************************
 * saveBackdrop()
 *
 * This routine will write out to the passed file descriptor any state
 * information this dialog needs.  It is called from saveSessionCB with the
 * file already opened.
 * All information is saved in xrm format.  There is no restriction
 * on what can be saved.  It doesn't have to be defined or be part of any
 * widget or Xt definition.  Just name and save it here and recover it in
 * restoreBackdrop.  The suggested minimum is whether you are mapped, and your
 * location.
 ************************************************************************/
void 
saveBackdrop(
        int fd )
{
    Position x,y;
    Dimension width, height;
    char *bufr = style.tmpBigStr;     /* size=[1024], make bigger if needed */
    XmVendorShellExtObject  vendorExt;
    XmWidgetExtData         extData;

    if (style.backdropDialog != NULL) 
    {
        if (XtIsManaged(style.backdropDialog))
            sprintf(bufr, "*backdropsDialog.ismapped: True\n");
        else
            sprintf(bufr, "*backdropsDialog.ismapped: False\n");

        /* Get and write out the geometry info for our Window */
        x = XtX (XtParent(style.backdropDialog));
        y = XtY (XtParent(style.backdropDialog));
        width = XtWidth (style.backdropDialog);
        height = XtHeight (style.backdropDialog);

        /* Modify x & y to take into account window mgr frames
         * This is pretty bogus, but I don't know a better way to do it.
         */
        extData = _XmGetWidgetExtData(style.shell, XmSHELL_EXTENSION);
        vendorExt = (XmVendorShellExtObject)extData->widget;
        x -= vendorExt->vendor.xOffset;
        y -= vendorExt->vendor.yOffset;

        sprintf(bufr, "%s*backdropsDialog.x: %d\n", bufr, x);
        sprintf(bufr, "%s*backdropsDialog.y: %d\n", bufr, y);
        sprintf(bufr, "%s*backdropsDialog.width: %d\n", bufr, width);
        sprintf(bufr, "%s*backdropsDialog.height: %d\n", bufr, height);
        sprintf(bufr, "%s*backdropsDialog.selectedItemNum: %d\n", bufr, 
                backdrops.selected);
        sprintf(bufr, "%s*backdropsDialog.selectedItem: %s\n", bufr, 
                backdrops.bitmapNames[backdrops.selected]);
        write (fd, bufr, strlen(bufr));
    }
}



