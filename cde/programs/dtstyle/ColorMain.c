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
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        ColorMain.c
 **
 **   Project:     DT 3.0 
 **
 **   Description: Controls the Dtstyle Color dialog
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
/* $TOG: ColorMain.c /main/8 1998/01/12 10:41:53 cshi $ */
/*+++++++++++++++++++++++++++++++++++++++*/
/* include files                         */
/*+++++++++++++++++++++++++++++++++++++++*/
#ifdef __apollo
#include  "/sys5/usr/include/limits.h"
#else  /* common default */
#include <limits.h>
#endif /* __apollo */

#include <locale.h>
#include <stdlib.h>
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <Xm/MwmUtil.h>

#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/MessageB.h>
#include <Xm/Protocols.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleBG.h>
#include <Xm/RowColumn.h>
#include <Xm/SelectioB.h>
#include <Xm/TextF.h>
#include <Xm/VendorSEP.h>

#include <Dt/DialogBox.h>
#include <Dt/Icon.h>
#include <Dt/TitleBox.h>

#include <Dt/HourGlass.h>

#include "Help.h"
#include "Main.h"
#include "SaveRestore.h"
#include "ColorMain.h"
#include "ColorFile.h"
#include "ColorEdit.h"

#ifdef sun
#include "OWsync.h"
#endif

/*+++++++++++++++++++++++++++++++++++++++*/
/* include extern functions              */
/*+++++++++++++++++++++++++++++++++++++++*/


/*+++++++++++++++++++++++++++++++++++++++*/
/* Local #defines                        */
/*+++++++++++++++++++++++++++++++++++++++*/
#define DEFAULT_PALETTE    "Default"
#define TYPE_OF_MONITOR  "Type Of Monitor" /* also in dtsession/SrvPalette.c */

#define BORDER_WIDTH                3
#define COLOR_BUTTON_WIDTH         35
#define COLOR_BUTTON_HEIGHT        35
#define ADD_PALETTE_TOP_OFFSET     20
#define ADD_PALETTE_LEFT_POSITION  65
#define PALETTE_RC_RIGHT_POSITION  60

#define DEFAULT_COLOR  (XmCO_HIGH_COLOR + 1)

#define B_W_STR            "B_W"
#define LOW_COLOR_STR      "LOW_COLOR"
#define MEDIUM_COLOR_STR   "MEDIUM_COLOR"
#define HIGH_COLOR_STR     "HIGH_COLOR"
#define DEFAULT_COLOR_STR  "DEFAULT"

#define STR1 ((char *)GETMESSAGE(14, 29, "A palette named '%s' already exists.\nThis new palette will overwrite the old one.\nIs this what you want to do?"))
#define STR2 ((char *)GETMESSAGE(14, 23, "Delete palette '%s'?\n"))
#define NEXT_SESSION ((char *)GETMESSAGE(14, 28, "The selected palette will take effect\nat your next session."))
#define CANT_DELETE ((char *)GETMESSAGE(14, 18, "Can't delete the last palette.\n"))
#define COLORUSE_WHEN ((char *)GETMESSAGE(14, 27, "The new Color Use value will take effect\nat your next session."))

/*+++++++++++++++++++++++++++++++++++++++*/
/* Local typedefs                        */
/*+++++++++++++++++++++++++++++++++++++++*/

typedef struct {
    Widget           colorForm;
    Widget           paletteTB;
    Widget	     palettesForm;
    Widget	     addPaletteButton;
    Widget	     deletePaletteButton;
    Widget           buttonsTB;
    Widget           colorlabel;
    Widget           highColorTG;
    Widget           mediumColorTG;
    Widget           lowColorTG;
    Widget           blackWhiteTG;
    Widget           defaultTG;
    Widget	     dlg;
    int              origColorUse;
    char             *currentColorUseStr;
    int              currentColorUse;
} colorWidgets;

/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Functions                    */
/*+++++++++++++++++++++++++++++++++++++++*/

static int CreateMainWindow( Widget parent) ;
static void selectPaletteCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void selectColorCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void timeoutCB( 
                        XtPointer client_data,
                        XtIntervalId *id) ;
static void addPaletteCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void addCancelCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void addOkCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void setDlgOkCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void modifyColorCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void dialogBoxCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void AddName(    
                        palette *newPalette) ;
static void deletePaletteCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void deleteOkCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void deleteCancelCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void resourcesCB(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void colorUseCB(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void colorUseExitCB(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void activateCBexitColor( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void _DtmapCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void _DtmapCB_colorUse( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static Boolean ValidName( char *name) ;
void loadDatabase();



/*+++++++++++++++++++++++++++++++++++++++*/
/* Global Variables                      */
/*+++++++++++++++++++++++++++++++++++++++*/

Atom     XA_CUSTOMIZE;
Atom     XA_TYPE_MONITOR;

/*  Palettes exist in a linked list. */
palette *pHeadPalette;
palette *pCurrentPalette;
palette *pOldPalette;

Widget  modifyColorButton;
int     TypeOfMonitor;
Bool    UsePixmaps;
int	FgColor;
Widget  paletteList;
Widget  deleteButton;
char    *defaultName;
Bool    WaitSelection;
int     NumOfPalettes;

/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Variables                    */
/*+++++++++++++++++++++++++++++++++++++++*/

/* palette names without the .dp */
static char *WHITE_BLACK = "WhiteBlack";
static char *BLACK_WHITE = "BlackWhite";
static char *WHITE_ONLY = "White";
static char *BLACK_ONLY = "Black";

static char *PALETTEDLG = "paletteDlg";
static saveRestore save = {FALSE, 0, };

static Widget           colorButton[XmCO_MAX_NUM_COLORS];
static Widget           addDialog;
static Widget           deleteDialog;
static Widget           colorUseDialog;
static palette          OrgPalette;
static XtIntervalId     timeID;
static int              dclick_time;
static int              selected_button;
static int              selected_position;
static Atom             XA_WM_DELETE_WINDOW;
static char             defaultName_restore[50];
static palette         *loop_palette, *loop_palette2;
static int              loopcount = 0;
static colorWidgets     colorDialog; 
static Widget           gParent;


/*
 *   copy of the system palette description file for the 
 *   current locale in xrm form
 */
XrmDatabase sys_pl_DB = NULL;

/*
 *   copy of the admin palette description file for the 
 *   current locale in xrm form
 */
XrmDatabase adm_pl_DB = NULL;

/*
 *   final combination of the admin & system dta bases 
 */
XrmDatabase pl_DB = NULL;

/*
 *   copy of the palette description file from the user's home 
 *   directory. This database is locale independent and will always
 *   be loaded.
 */
XrmDatabase hm_pl_DB = NULL;


void 
loadDatabase(void)
{
  
  char   *lang;
  char   *pl_desc;

  /* load the palette description data base for the given locale*/
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

  pl_desc = (char *)XtMalloc(strlen("/usr/dt/palettes/desc.") + strlen(lang) + 1);
  strcpy (pl_desc,"/usr/dt/palettes/desc.");
  strcat (pl_desc, lang);
  if(sys_pl_DB = XrmGetFileDatabase (pl_desc))
    XrmMergeDatabases(sys_pl_DB, &pl_DB);
  XtFree(pl_desc);
  
  /* load the palette description data base for the given locale*/
  /* from that locale's description file from the admin location */
  pl_desc = (char *)XtMalloc(strlen("/etc/dt/palettes/desc.") + strlen(lang) + 1);
  strcpy (pl_desc,"/etc/dt/palettes/desc.");
  strcat (pl_desc, lang);
  if (adm_pl_DB = XrmGetFileDatabase (pl_desc))
    XrmMergeDatabases(adm_pl_DB, &pl_DB);
  XtFree(pl_desc);

  /* load the palette description data base regardless of locale*/
  pl_desc = (char *) XtMalloc(strlen(style.home) +(strlen("/.dt/palettes/desc.palettes") + 1));
  strcpy (pl_desc, style.home);
  strcat (pl_desc, "/.dt/palettes/desc.palettes");
  if (hm_pl_DB = XrmGetFileDatabase (pl_desc))
    XrmMergeDatabases(hm_pl_DB, &pl_DB);
  XtFree(pl_desc);

  /* load the palette description data base regardless of locale for later use*/
  pl_desc = (char *) XtMalloc(strlen(style.home) +(strlen("/.dt/palettes/desc.palettes") + 1));
  strcpy (pl_desc, style.home);
  strcat (pl_desc, "/.dt/palettes/desc.palettes");
  if (XrmCombineFileDatabase (pl_desc, &pl_DB, True))
    /* load a separate home data base to be later saved as a file  */
    /* if any palettes are added */
    hm_pl_DB = XrmGetFileDatabase (pl_desc);
  XtFree(pl_desc);
}



void 
Customize(
        Widget shell )
{
    register int     i;

    /*  
    **  Main routine does the following:
    **   1. Creates the color dialog shell
    **   2. Checks which monitor the customizer is running on
    **   3. Initialize color palettes
    **
    */
  
    /* can the user access the Color portion of Style? */

    /* Not if Color Server is not running */
    if (style.colorSrv == FALSE)
    {
       ErrDialog(((char *)GETMESSAGE(14, 25, "The color portion of the Style Manager\nwill not operate because the color server\nis not running.  Check $HOME/.dt/errorlog.")), style.shell);
       return;
    }

    /* Not if useColorObj resource is False.  XmeUseColorObj will return false
       if color server is not running or if the resource UseColorObj is False.
       If the color server is not running, that case is caught above so if
       we get here, the resource UseColorObj must be False.  This is an
       undocumented resource. */
       
    if (XmeUseColorObj() == FALSE)
    {
       ErrDialog(((char *)GETMESSAGE(14, 26, "The color portion of the Style Manager\nwill not operate because the resource\n'useColorObj' is set to False.")), style.shell);
       return;
    }

    if (style.colorDialog == NULL || style.count < 12)
    {
        _DtTurnOnHourGlass(shell);

	/* Create Main Color Dialog */
	if(CreateMainWindow(shell) == -1)
        {
           _DtTurnOffHourGlass(shell);
           return; 
        }
        XtManageChild(style.colorDialog);  

        _DtTurnOffHourGlass(shell);
    }
    else 
    {
        XtManageChild(style.colorDialog);

        raiseWindow(XtWindow(XtParent(style.colorDialog)));
    }

   /* also determine if system uses long filenames, used by add palette only */
    CheckFileType();

    SaveOrgPalette();
}


/*
**  This routine creates the Color Customizer Main Window.   It is passed
**  the top level shell.
*/
static int 
CreateMainWindow(
        Widget parent )
{

    /* Create the DialogBox dialog */
    CreateDialogBoxD(parent);
  
    /* add some more to the dialog box */
    AddToDialogBox();

    /* Create the top part of the color dialog */
    CreateTopColor1();
    CreateTopColor2();

    /* read in palettes */
    ReadInPalettes(style.xrdb.paletteDir);

    if (NumOfPalettes == 0)
    {
        /* error dialog - no palettes */
        ErrDialog(((char *)GETMESSAGE(14, 38, "The color portion of the Style Manager\n\
will not operate because there are no palette\n\
files available.  Check $HOME/.dt/errorlog.")), style.shell);
        return(-1);        
    }

    /* go get the list of palettes of the color dialog */
    InitializePaletteList(parent,paletteList, False);

    /* Allocate the pixels for the Current palette. 
     * Will get the pixel values from the color server. 
     */
    AllocatePaletteCells(parent);

    /* go create the bottom portion of the color dialog */
    CreateBottomColor();

/*
**  Create the color buttons.  Have to do it after
**  initialize palettes so the correct pixels would be used.
*/
    CreatePaletteButtons(style.buttonsForm);
    
    return(0);

}


void 
CreatePaletteButtons(
        Widget parent )
{
    register int     i,n;
    Arg              args[16];
    XmString         string;
    Pixmap           pixmap100;
    Widget           paletteRc;

    if(style.count > 11)
       return;

    if (TypeOfMonitor == XmCO_BLACK_WHITE)      /* create pixmaps for top/bottom shadow */
    {
	edit.pixmap25 = XmGetPixmap (style.screen, "25_foreground",
	               BlackPixelOfScreen(style.screen),
		       WhitePixelOfScreen(style.screen));
	edit.pixmap75 = XmGetPixmap (style.screen, "75_foreground",
	               BlackPixelOfScreen(style.screen),
		       WhitePixelOfScreen(style.screen));
    }
    pixmap100 = XmGetPixmap (style.screen, "background",
             BlackPixelOfScreen(style.screen),
             pCurrentPalette->color[pCurrentPalette->active].bg.pixel); 
    n = 0;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);  n++;
    XtSetArg (args[n], XmNtopOffset, 0);  n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
    XtSetArg (args[n], XmNleftOffset, 0);  n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);  n++;
    XtSetArg (args[n], XmNrightPosition, PALETTE_RC_RIGHT_POSITION);  n++;
    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);  n++;
    XtSetArg (args[n], XmNbottomOffset, 0);  n++;
    XtSetArg (args[n], XmNspacing, 0);  n++;
    XtSetArg (args[n], XmNmarginWidth, style.horizontalSpacing);  n++;
    XtSetArg (args[n], XmNmarginHeight, style.verticalSpacing);  n++;
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL);  n++;
    XtSetArg (args[n], XmNpacking, XmPACK_COLUMN);  n++;
    XtSetArg (args[n], XmNadjustLast, False);  n++;
    if(TypeOfMonitor == XmCO_HIGH_COLOR)
    {
       XtSetArg (args[n], XmNnumColumns, 2);  n++;
    }
    else
    {
       XtSetArg (args[n], XmNnumColumns, 1);  n++;
    }
    paletteRc = XmCreateRowColumn(parent, "paletteRc", args, n);
    XtManageChild(paletteRc);

    for (i=0; i<XmCO_MAX_NUM_COLORS; i++)
    {
       n=0;
       XtSetArg (args[n], XmNrecomputeSize, False); n++; 
       XtSetArg (args[n], XmNwidth,  COLOR_BUTTON_WIDTH); n++; 
       XtSetArg (args[n], XmNheight, COLOR_BUTTON_HEIGHT); n++; 
       /* allow traversal only if dynamicColor is on */
       if (!style.dynamicColor)
       {
           XtSetArg (args[n], XmNtraversalOn, False); n++; 
       }
       XtSetArg (args[n], XmNborderWidth, BORDER_WIDTH); n++; 
       XtSetArg (args[n], XmNborderColor, 
           pCurrentPalette->color[pCurrentPalette->secondary].bg.pixel); n++;
       XtSetArg (args[n], XmNforeground, 
                     pCurrentPalette->color[i].fg.pixel); n++;
       XtSetArg (args[n], XmNbackground,
                     pCurrentPalette->color[i].bg.pixel); n++;
       XtSetArg (args[n], XmNarmColor, pCurrentPalette->color[i].sc.pixel); n++;
       XtSetArg (args[n], XmNmultiClick, XmMULTICLICK_KEEP); n++;
       if (TypeOfMonitor == XmCO_LOW_COLOR)
       {
           XtSetArg (args[n], XmNhighlightColor, 
            pCurrentPalette->color[pCurrentPalette->secondary].fg.pixel); n++;
       }
       else
       {
           XtSetArg (args[n], XmNhighlightPixmap, pixmap100); n++;
       }
       if(UsePixmaps == FALSE && TypeOfMonitor != XmCO_BLACK_WHITE)
       {
          XtSetArg (args[n], XmNtopShadowColor, 
                            pCurrentPalette->color[i].ts.pixel); n++;
          XtSetArg (args[n], XmNbottomShadowColor, 
                            pCurrentPalette->color[i].bs.pixel); n++;
       }
       else if (TypeOfMonitor == XmCO_BLACK_WHITE)
       {
          XtSetArg (args[n], XmNtopShadowPixmap, edit.pixmap25);        n++;
          XtSetArg (args[n], XmNbottomShadowPixmap, edit.pixmap75);     n++;
       }
       string = CMPSTR("     ");
       XtSetArg (args[n], XmNlabelString, string); n++;
       colorButton[i] = XmCreatePushButton(paletteRc, "colorButton", args, n);
       /* allow access to modify functionality only if dynamicColor is on */
       if (style.dynamicColor)
           XtAddCallback(colorButton[i], XmNactivateCallback, selectColorCB, 
                         (XtPointer)i);  
       XmStringFree(string);
    }
    XtManageChildren(colorButton,pCurrentPalette->num_of_colors);

    if(!save.restoreFlag)
       selected_button = 0;

    /* draw selection border only if dynamicColor is on */
    if (style.dynamicColor)
    {
        n=0;
        XtSetArg (args[n], XmNborderColor, BlackPixelOfScreen(style.screen)); n++;
        XtSetValues(colorButton[selected_button],args,n);
    }
    
    style.count++;
}

Boolean 
InitializePaletteList(
        Widget shell,
        Widget list,
#if NeedWidePrototypes
        int startup )
#else
        Boolean startup )
#endif
{
  register int     n;
  Arg              args[4];
  XmString         string;
  XmStringTable    string_table;
  char            *name_str;
  char            *class_str;
  char            *str_type_return;
  XrmValue         value_return;
  
  if(style.count > 8)
    return(True);
  
  /* 
   **  Add the palettes read in from ReadInPalettes
   */
  if(loopcount == 0) {
    if (TypeOfMonitor != XmCO_BLACK_WHITE) {
      loop_palette = pHeadPalette;
      loop_palette2 = pHeadPalette;
    }
    else {
      loop_palette = pHeadPalette;
      loop_palette2 = NULL;
      while(loop_palette != NULL)
	if(strcmp(loop_palette->name, "Black") == 0) {
	  loop_palette2 = loop_palette;
	  loop_palette = NULL;
	}
	else
	  loop_palette = loop_palette->next;
      loop_palette = pHeadPalette;
      if(loop_palette2 == NULL)
	loop_palette2 = pHeadPalette;
    }
  }
  while( loop_palette != NULL)
    {
      if (pl_DB !=NULL)
	{
	  name_str = (char *) XtMalloc(strlen("palettes.") + 
				       strlen(loop_palette->name) +
				       strlen(".desc") + 1);
	  
	  class_str = (char *) XtMalloc(strlen("Palettes.") + 
					strlen(loop_palette->name) + 
					strlen(".Desc") + 1);
	  strcpy(name_str, "palettes.");
	  strcpy(class_str, "Palettes.");
	  strcat(name_str, loop_palette->name);
	  strcat(class_str, loop_palette->name);
	  strcat(name_str, ".desc");
	  strcat(class_str, ".Desc");
	  
	  if (XrmGetResource (pl_DB, name_str, class_str, &str_type_return, &value_return))
	    {
	      loop_palette->desc = (char *)XtMalloc(strlen (value_return.addr) + 1 );
	      strcpy(loop_palette->desc, value_return.addr);
	    }
	  else
	    {
	      loop_palette->desc = (char *)XtMalloc(strlen (loop_palette->name) + 1 );
	      strcpy(loop_palette->desc, loop_palette->name);
	    }
	  XtFree(name_str);
	  XtFree(class_str);
	}
      else
	{
	  loop_palette->desc = (char *)XtMalloc(strlen (loop_palette->name) + 1 );
	  strcpy(loop_palette->desc, loop_palette->name);
	}
      
      XmListAddItem(list, CMPSTR(loop_palette->desc), loop_palette->item_position);
      
      /* if the item is the same as the default name provided by the
	 color Server, save it */
      if(!save.restoreFlag || defaultName_restore == NULL) {
	if (!(strcmp(loop_palette->name, defaultName)))
	  loop_palette2 = loop_palette;
      }
      else {
	if (!(strcmp(loop_palette->name, defaultName_restore)))
	  loop_palette2 = loop_palette;
      }
      loop_palette = loop_palette->next;
      loopcount++;
      if((loopcount % 5 == 0) && startup)
	{

	return(False);}
    }
  
  /*
   **  Make the palette named by the color Server the selected palette, if the
   **  palette returned by the color server doesn't match make it the head
   **  palette.
   */
  pOldPalette = NULL;
  
  /* the default name is the name to be selected */
  if (loop_palette2->name == NULL)
    string = CMPSTR("");
  else
    string = CMPSTR(loop_palette2->desc);
  string_table = &string;
  
  n=0;
  XtSetArg (args[n], XmNselectedItemCount, 1);  n++;
  XtSetArg (args[n], XmNselectedItems, string_table);  n++;
  XtSetValues (list, args, n);
  XmStringFree(string); 
  
  XmListSetPos(list, loop_palette2->item_position);
  XtManageChild(list);
  
  pCurrentPalette = loop_palette2;
  selected_position = pCurrentPalette->item_position;
  
  style.count++;
  return(True);
}
    
/*
**  This is the selection callback for the Scrolled list.
**  The routine finds the item the user selected and changes 
**  the already allocated pixels to the colors of the selected palette.
*/
static void 
selectPaletteCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    register int     n,i;
    Arg              args[10];
    XmListCallbackStruct *cb = (XmListCallbackStruct *)call_data;
    palette         *tmp_palette;
    XmString         string;
    Pixel            white, black;
    static unsigned long   pixels[XmCO_MAX_NUM_COLORS*5];
    static int       count;
    static Boolean   First = True;


    white = WhitePixelOfScreen(style.screen);
    black = BlackPixelOfScreen(style.screen);

    if (((edit.DialogShell == NULL) || (!XtIsManaged(edit.DialogShell))) &&
                                      selected_position != cb->item_position)
    {
	selected_position = cb->item_position;

	tmp_palette = pHeadPalette;
	while( tmp_palette->item_position != selected_position &&
						  tmp_palette != NULL)
	     tmp_palette = tmp_palette->next;

	if(tmp_palette->item_position == selected_position)
	{
	    pOldPalette = pCurrentPalette;
	    pCurrentPalette = tmp_palette;

	    n=0;
            string = CMPSTR(pCurrentPalette->desc);
	    XtSetArg (args[n], XmNtitleString, string); n++;
	    XtSetValues (colorDialog.buttonsTB, args, n);
            XmStringFree(string);

            if (style.dynamicColor)
	        ReColorPalette();
	    else 
            {     
               /* PUT DIALOG saying can't dynamically change */
               if(First)
               {
                  InfoDialog(NEXT_SESSION, style.colorDialog, False);
                  First = False;
               }
               else
               {
                  if (TypeOfMonitor != XmCO_BLACK_WHITE)

                     /* free the cells from last selection */
                     XFreeColors(style.display, style.colormap, pixels, 
                                 count, 0);
               }

               if (TypeOfMonitor != XmCO_BLACK_WHITE)
               {
                  /* allocate new colors */
                  count = 0;

                  for (i=0; i<pCurrentPalette->num_of_colors; i++)
                  {
                     n=0;  
                     if (XAllocColor(style.display, style.colormap,
                                 &(pCurrentPalette->color[i].bg)) == 0)
                        break;
                     pixels[count++] = pCurrentPalette->color[i].bg.pixel;
                     XtSetArg (args[n], XmNbackground,
                                 pCurrentPalette->color[i].bg.pixel); n++;

                     if (XAllocColor(style.display, style.colormap,
                                 &(pCurrentPalette->color[i].sc)) == 0)
                        break;
                     pixels[count++] = pCurrentPalette->color[i].sc.pixel;
                     XtSetArg (args[n], XmNarmColor, 
                                 pCurrentPalette->color[i].sc.pixel); n++;

                     if (UsePixmaps == FALSE)
                     {
                        if (XAllocColor(style.display, style.colormap,
                                 &(pCurrentPalette->color[i].ts)) == 0)
                           break;
                        pixels[count++] = pCurrentPalette->color[i].ts.pixel;
                        XtSetArg (args[n], XmNtopShadowColor, 
                                 pCurrentPalette->color[i].ts.pixel); n++;

                        if (XAllocColor(style.display, style.colormap,
                                 &(pCurrentPalette->color[i].bs)) == 0)
                           break;
                        pixels[count++] = pCurrentPalette->color[i].bs.pixel;
                        XtSetArg (args[n], XmNbottomShadowColor, 
                            pCurrentPalette->color[i].bs.pixel); n++;
                     }
                     else     /* create pixmaps for top/bottom shadow */
                     {
                         XmDestroyPixmap(style.screen, edit.pixmap25);
                         XmDestroyPixmap(style.screen, edit.pixmap75);

                         edit.pixmap25 = XmGetPixmap (style.screen, 
                                         "50_foreground",
                                         pCurrentPalette->color[i].bg.pixel,
                                         WhitePixelOfScreen(style.screen));

                         edit.pixmap75 = XmGetPixmap (style.screen, 
                                         "50_foreground",
                                         pCurrentPalette->color[i].bg.pixel,
                                         BlackPixelOfScreen(style.screen));

                        XtSetArg (args[n], XmNtopShadowPixmap, edit.pixmap25);
                            n++;
                        XtSetArg (args[n], XmNbottomShadowPixmap, edit.pixmap75);     
                            n++;
                     }

                     XtSetValues(colorButton[i], args, n);
                  }
               }
               else  /* XmCO_BLACK_WHITE */
               {
                  /* set color buttons for new palette - read only cells */
                  /* primary=color[1] secondary=color[0] */
                  if (strcmp(pCurrentPalette->name, WHITE_BLACK) == 0)
                  {
                      n=0;      /* secondary color white */
                      XtSetArg (args[n], XmNforeground, black); n++;
                      XtSetArg (args[n], XmNbackground, white); n++;
                      XtSetArg (args[n], XmNarmColor, white); n++;
                      XtSetValues(colorButton[0], args, n);
                      pCurrentPalette->color[0].fg.pixel = black;
                      pCurrentPalette->color[0].bg.pixel = white;
                      pCurrentPalette->color[0].sc.pixel = white;
                      pCurrentPalette->color[0].ts.pixel = black;
                      pCurrentPalette->color[0].bs.pixel = black;

                      n=0;      /* primary color black */
                      XtSetArg (args[n], XmNforeground, white); n++;
                      XtSetArg (args[n], XmNbackground, black); n++;
                      XtSetArg (args[n], XmNarmColor, black); n++;
                      XtSetValues(colorButton[1], args, n);
                      pCurrentPalette->color[1].fg.pixel = white;
                      pCurrentPalette->color[1].bg.pixel = black;
                      pCurrentPalette->color[1].sc.pixel = black;
                      pCurrentPalette->color[1].ts.pixel = white;
                      pCurrentPalette->color[1].bs.pixel = white;
                  }
                  else if (strcmp(pCurrentPalette->name, BLACK_WHITE) == 0)
                  {
                      n=0;      /* secondary color black */
                      XtSetArg (args[n], XmNforeground, white); n++;
                      XtSetArg (args[n], XmNbackground, black); n++;
                      XtSetArg (args[n], XmNarmColor, black); n++;
                      XtSetValues(colorButton[0], args, n);
                      pCurrentPalette->color[0].fg.pixel = white;
                      pCurrentPalette->color[0].bg.pixel = black;
                      pCurrentPalette->color[0].sc.pixel = black;
                      pCurrentPalette->color[0].ts.pixel = white;
                      pCurrentPalette->color[0].bs.pixel = white;

                      n=0;      /* primary color white */
                      XtSetArg (args[n], XmNforeground, black); n++;
                      XtSetArg (args[n], XmNbackground, white); n++;
                      XtSetArg (args[n], XmNarmColor, white); n++;
                      XtSetValues(colorButton[1], args, n);
                      pCurrentPalette->color[1].fg.pixel = black;
                      pCurrentPalette->color[1].bg.pixel = white;
                      pCurrentPalette->color[1].sc.pixel = white;
                      pCurrentPalette->color[1].ts.pixel = black;
                      pCurrentPalette->color[1].bs.pixel = black;
                  }
                  else if (strcmp(pCurrentPalette->name, BLACK_ONLY) == 0)
                  {
                      n=0;      /* primary and secondary color black */
                      XtSetArg (args[n], XmNforeground, white); n++;
                      XtSetArg (args[n], XmNbackground, black); n++;
                      XtSetArg (args[n], XmNarmColor, black); n++;
                      XtSetValues(colorButton[0], args, n);
                      pCurrentPalette->color[0].fg.pixel = white;
                      pCurrentPalette->color[0].bg.pixel = black;
                      pCurrentPalette->color[0].sc.pixel = black;
                      pCurrentPalette->color[0].ts.pixel = white;
                      pCurrentPalette->color[0].bs.pixel = white;
                      XtSetValues(colorButton[1], args, n);
                      pCurrentPalette->color[1].fg.pixel = white;
                      pCurrentPalette->color[1].bg.pixel = black;
                      pCurrentPalette->color[1].sc.pixel = black;
                      pCurrentPalette->color[1].ts.pixel = white;
                      pCurrentPalette->color[1].bs.pixel = white;
                  }
                  else     /* WHITE_ONLY */
                  {
                      n=0;      /* primary and secondary color white */
                      XtSetArg (args[n], XmNforeground, black); n++;
                      XtSetArg (args[n], XmNbackground, white); n++;
                      XtSetArg (args[n], XmNarmColor, white); n++;
                      XtSetValues(colorButton[0], args, n);
                      pCurrentPalette->color[0].fg.pixel = black;
                      pCurrentPalette->color[0].bg.pixel = white;
                      pCurrentPalette->color[0].sc.pixel = white;
                      pCurrentPalette->color[0].ts.pixel = black;
                      pCurrentPalette->color[0].bs.pixel = black;
                      XtSetValues(colorButton[1], args, n);
                      pCurrentPalette->color[1].fg.pixel = black;
                      pCurrentPalette->color[1].bg.pixel = white;
                      pCurrentPalette->color[1].sc.pixel = white;
                      pCurrentPalette->color[1].ts.pixel = black;
                      pCurrentPalette->color[1].bs.pixel = black;
                  }
              }                  
           }
        }
    }
}


static void 
selectColorCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    int              i,n;
    Arg              args[4];
    ColorSet *color_set;
    XmPushButtonCallbackStruct *cb = (XmPushButtonCallbackStruct *)call_data;

    i = (int) client_data;

    /* if click_count == 1 .. first button press, set time out */
    if(cb->click_count == 1)
    {
       timeID = XtAppAddTimeOut(XtWidgetToApplicationContext(gParent), 
				(unsigned long) dclick_time, timeoutCB, 
				(XtPointer) i);
       return;
    }

    /* else .. second button press, remove the time out */
    XtRemoveTimeOut(timeID);

    if ((edit.DialogShell == NULL) || (!XtIsManaged(edit.DialogShell)))
    {
        /* make the new selected button have a border color */
        n=0;
        XtSetArg (args[n], XmNborderColor, 
                  pCurrentPalette->color[pCurrentPalette->secondary].bg.pixel);
        n++;
        XtSetValues(colorButton[selected_button],args,n);

        n=0;
        XtSetArg (args[n], XmNborderColor, BlackPixelOfScreen(style.screen));
        n++;
        XtSetValues(colorButton[i],args,n);

        selected_button = i;

        color_set = (ColorSet *) &pCurrentPalette->color[selected_button];
        ColorEditor(style.colorDialog,color_set);
    }
}


/*
**  This is the double click timeout callback.  If this routine is called
**  then there was only a single click on a colorbutton
*/
static void 
timeoutCB(
        XtPointer client_data,
        XtIntervalId *id )
{
    register int     n;
    int              i;
    Arg              args[2];

    if (TypeOfMonitor == XmCO_BLACK_WHITE)
        return;

    i = (int)client_data;

    if ((edit.DialogShell == NULL) || (!XtIsManaged(edit.DialogShell)))
    {
        /* make the new selected button have a border color */
        n=0;
        XtSetArg (args[n], XmNborderColor, 
                  pCurrentPalette->color[pCurrentPalette->secondary].bg.pixel);
        n++;
        XtSetValues(colorButton[selected_button],args,n);

        n=0;
        XtSetArg (args[n], XmNborderColor, BlackPixelOfScreen(style.screen));
        n++;
        XtSetValues(colorButton[i],args,n);
        selected_button = i;
    }
}

static void 
addPaletteCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{

    register int     n;
    Arg              args[10];
    XmString         string;
    XmString         string1;

    if (addDialog == NULL)
    {
        n = 0;

        XtSetArg(args[n], XmNokLabelString, CMPSTR((String) _DtOkString)); n++;
        XtSetArg(args[n], XmNcancelLabelString, CMPSTR((String) _DtCancelString)); n++;
        XtSetArg(args[n], XmNhelpLabelString, CMPSTR((String) _DtHelpString)); n++;
	string =  CMPSTR(((char *)GETMESSAGE(14, 10, "New palette name:")));
        XtSetArg(args[n], XmNselectionLabelString, string); n++;
        string1 =  CMPSTR("");
        XtSetArg(args[n], XmNtextString, string1); n++;
        XtSetArg(args[n], XmNborderWidth, 3); n++;
        XtSetArg(args[n], XmNautoUnmanage, False); n++;
        addDialog = XmCreatePromptDialog(style.colorDialog,"AddDialog",
                                                                      args, n);
        XmStringFree(string);
        XmStringFree(string1);

        XtAddCallback(addDialog, XmNokCallback, addOkCB, 
                                 (XtPointer) NULL);
        XtAddCallback(addDialog, XmNcancelCallback, addCancelCB, 
                                 (XtPointer) NULL);
        XtAddCallback(addDialog, XmNhelpCallback,
            (XtCallbackProc)HelpRequestCB, (XtPointer)HELP_ADD_PALETTE_DIALOG);

        n = 0;
        XtSetArg (args[n], XmNmwmInputMode,
                        MWM_INPUT_PRIMARY_APPLICATION_MODAL); n++;
        XtSetArg (args[n], XmNuseAsyncGeometry, True); n++;
        XtSetArg (args[n], XmNtitle, ((char *)GETMESSAGE(14, 11, "Add Palette"))); n++;
        XtSetArg (args[n], XmNmwmFunctions, DIALOG_MWM_FUNC); n++;
        XtSetValues (XtParent (addDialog), args, n);
    }
    n=0;
    string =  CMPSTR("");
    XtSetArg(args[n], XmNtextString, string); n++;
    XtSetValues(addDialog, args, n);
    XmStringFree(string);

    XtManageChild(addDialog);

}

static void 
addCancelCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
        XtUnmanageChild(addDialog);
}

static void 
addOkCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  register int     n, i;
  Arg              args[6];
  XmString         string;
  char             *name, *filename, *tmpstr;
  palette         *tmpPalette, *newPalette;
  int              count;
  int              ii, length;
  int              len;

  /* Get the text from the promp dialog */
  name = XmTextFieldGetString( XmSelectionBoxGetChild(addDialog, XmDIALOG_TEXT));
  
  /* see if the user typed in a valid palette name */
  if(!ValidName(name)) {
    ErrDialog(((char *)GETMESSAGE(14, 12, "The palette name cannot contain\nthese characters:\n\n * : ( ) [ ] { } < > ! | \" / \\")), 
	      style.colorDialog); 
    XtFree(name);
    return;
  }
  
  /* check the number of characters in name in a locale independent way */
  for ( ii = 0, count = 0; name[ii] != '\0'; ii += length ) {
    length = mblen( &(name[ii]), MB_CUR_MAX );
    if ( length <=0 )
      break;
    count++;
  }
  
  /* make sure the length of name is ok, short file names can only be 11 chars */
  if(count > 10 && !style.longfilename) {
    ErrDialog(((char *)GETMESSAGE(14, 13, "The palette name must be\n10 characters or less.\n")),style.colorDialog);
    XtFree(name);
    return;
  }
  
  /* Unmanage the promptDialog */
  XtUnmanageChild(addDialog);
  
  /* first search through palette descriptions and make sure the name to */
  /* add is not already in the list and go to the end of the palette list */
  for(tmpPalette = pHeadPalette; tmpPalette->next != NULL;
      tmpPalette = tmpPalette->next)
    {
      if((strcmp(tmpPalette->desc, name) == 0)) {
	SameName(w, tmpPalette, name);
	XtFree(name);
	return;
      }
    }
  
  /* Check the last palette */
  if((strcmp(tmpPalette->desc, name) == 0)) {
    SameName(w, tmpPalette, name);
    XtFree(name);
    return;
  }
  
  /* allocate space for a new palette */
  newPalette = (palette *)XtMalloc(sizeof(palette) + 1 );
  
  /* set the previous last palatte to this new one, it is now the last one*/
  tmpPalette->next = newPalette;
  newPalette->next = NULL;
  
  /* malloc space for the new palette desc */
  newPalette->desc = (char *)XtMalloc(strlen(name) + 1);
  for(i = 0; i < strlen(name); i++)
    newPalette->desc[i] = name[i];
  newPalette->desc[i] = '\0';
  
  /* malloc space for the new palette name directory */
  newPalette->directory = (char *)XtMalloc(strlen(style.home) +
					   strlen(DT_PAL_DIR) + 1);
  strcpy(newPalette->directory, style.home);
  strcat(newPalette->directory, DT_PAL_DIR);
  
  /* makeup a new name for the palette */
  tmpstr = (char *)XtMalloc(strlen(style.home) + strlen(DT_PAL_DIR) + 
			    strlen("dtXXXXXX") + 1);
  strcpy(tmpstr, newPalette->directory);
  len = strlen(tmpstr);
  strcat(tmpstr, "dtXXXXXX");
  mktemp(tmpstr);

  newPalette->name = (char *) XtMalloc(15 * sizeof(char));
  strcpy(newPalette->name, tmpstr + len);
  XtFree(tmpstr);

  /* the new palette is the next palette .. increase NumOfPalettes by one */
  newPalette->item_position = NumOfPalettes + 1;
  NumOfPalettes++;
  
  /* set all the new palette's color parameters to the current palette */
  newPalette->num_of_colors = pCurrentPalette->num_of_colors;
  for(i = 0; i < XmCO_MAX_NUM_COLORS; i++)
    {
      newPalette->color[i].fg.pixel = pCurrentPalette->color[i].fg.pixel;
      newPalette->color[i].fg.red = pCurrentPalette->color[i].fg.red;
      newPalette->color[i].fg.green = pCurrentPalette->color[i].fg.green;
      newPalette->color[i].fg.blue = pCurrentPalette->color[i].fg.blue;
      newPalette->color[i].fg.flags = pCurrentPalette->color[i].fg.flags;
      
      newPalette->color[i].bg.pixel = pCurrentPalette->color[i].bg.pixel;
      newPalette->color[i].bg.red = pCurrentPalette->color[i].bg.red;
      newPalette->color[i].bg.green = pCurrentPalette->color[i].bg.green;
      newPalette->color[i].bg.blue = pCurrentPalette->color[i].bg.blue;
      newPalette->color[i].bg.flags = pCurrentPalette->color[i].bg.flags;
      
      newPalette->color[i].ts.pixel = pCurrentPalette->color[i].ts.pixel;
      newPalette->color[i].ts.red = pCurrentPalette->color[i].ts.red;
      newPalette->color[i].ts.green = pCurrentPalette->color[i].ts.green;
      newPalette->color[i].ts.blue = pCurrentPalette->color[i].ts.blue;
      newPalette->color[i].ts.flags = pCurrentPalette->color[i].ts.flags;
      
      newPalette->color[i].bs.pixel = pCurrentPalette->color[i].bs.pixel;
      newPalette->color[i].bs.red = pCurrentPalette->color[i].bs.red;
      newPalette->color[i].bs.green = pCurrentPalette->color[i].bs.green;
      newPalette->color[i].bs.blue = pCurrentPalette->color[i].bs.blue;
      newPalette->color[i].bs.flags = pCurrentPalette->color[i].bs.flags;
      
      newPalette->color[i].sc.pixel = pCurrentPalette->color[i].sc.pixel;
      newPalette->color[i].sc.red = pCurrentPalette->color[i].sc.red;
      newPalette->color[i].sc.green = pCurrentPalette->color[i].sc.green;
      newPalette->color[i].sc.blue = pCurrentPalette->color[i].sc.blue;
      newPalette->color[i].sc.flags = pCurrentPalette->color[i].sc.flags;
    }
  
  /* Write out the palette */
  if ((WriteOutPalette(newPalette->name)) == -1)
    {
      XtFree(name);
      
      /*  remove palette from list */
      tmpPalette->next = NULL;
      XtFree ((char *)newPalette);
      
      return;
    }
  else
    WriteOutDesc(newPalette);
      
    
  /* add the name to the scrolled window list and select it */
  AddName(newPalette);
  
  /* now check to see if there is a ~filename .. if there is delete it */
  /* use the $HOME environment varible then constuct the full file name */
  filename = (char *)XtMalloc(strlen(style.home) + strlen(DT_PAL_DIR) +
			      strlen(newPalette->name) + strlen(PALETTE_SUFFIX) + 2);
  
  /* create the full path name plus file name */
  strcpy(filename, style.home);
  strcat(filename, DT_PAL_DIR);
  strcat(filename, "~");
  strcat(filename, newPalette->name);
  strcat(filename, PALETTE_SUFFIX);
  
  unlink(filename);
  
  XtFree(filename);
  XtFree(name);
  
  /* Go write out the palette */
  pCurrentPalette = newPalette;
  
}

static void 
setDlgOkCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   palette *tmpPalette = (palette *)client_data;
   int i;

  /* free the directory */
   XtFree(tmpPalette->directory);

  /* put the new (users) directory there */
   tmpPalette->directory = (char *)XtMalloc(strlen(style.home) +
                                          strlen(DT_PAL_DIR) + 1);
   strcpy(tmpPalette->directory, style.home);
   strcat(tmpPalette->directory, DT_PAL_DIR);

   for(i = 0; i < XmCO_MAX_NUM_COLORS; i++)
   {
       tmpPalette->color[i].fg.pixel = pCurrentPalette->color[i].fg.pixel;
       tmpPalette->color[i].fg.red = pCurrentPalette->color[i].fg.red;
       tmpPalette->color[i].fg.green = pCurrentPalette->color[i].fg.green;
       tmpPalette->color[i].fg.blue = pCurrentPalette->color[i].fg.blue;
       tmpPalette->color[i].fg.flags = pCurrentPalette->color[i].fg.flags;

       tmpPalette->color[i].bg.pixel = pCurrentPalette->color[i].bg.pixel;
       tmpPalette->color[i].bg.red = pCurrentPalette->color[i].bg.red;
       tmpPalette->color[i].bg.green = pCurrentPalette->color[i].bg.green;
       tmpPalette->color[i].bg.blue = pCurrentPalette->color[i].bg.blue;
       tmpPalette->color[i].bg.flags = pCurrentPalette->color[i].bg.flags;

       tmpPalette->color[i].ts.pixel = pCurrentPalette->color[i].ts.pixel;
       tmpPalette->color[i].ts.red = pCurrentPalette->color[i].ts.red;
       tmpPalette->color[i].ts.green = pCurrentPalette->color[i].ts.green;
       tmpPalette->color[i].ts.blue = pCurrentPalette->color[i].ts.blue;
       tmpPalette->color[i].ts.flags = pCurrentPalette->color[i].ts.flags;

       tmpPalette->color[i].bs.pixel = pCurrentPalette->color[i].bs.pixel;
       tmpPalette->color[i].bs.red = pCurrentPalette->color[i].bs.red;
       tmpPalette->color[i].bs.green = pCurrentPalette->color[i].bs.green;
       tmpPalette->color[i].bs.blue = pCurrentPalette->color[i].bs.blue;
       tmpPalette->color[i].bs.flags = pCurrentPalette->color[i].bs.flags;

       tmpPalette->color[i].sc.pixel = pCurrentPalette->color[i].sc.pixel;
       tmpPalette->color[i].sc.red = pCurrentPalette->color[i].sc.red;
       tmpPalette->color[i].sc.green = pCurrentPalette->color[i].sc.green;
       tmpPalette->color[i].sc.blue = pCurrentPalette->color[i].sc.blue;
       tmpPalette->color[i].sc.flags = pCurrentPalette->color[i].sc.flags;
   }

  /* Write out the palette */
   if ((WriteOutPalette(tmpPalette->name)) == -1)
      return;
   else
     WriteOutDesc(tmpPalette);

   pCurrentPalette = tmpPalette;

  /* select item in list as if user had selected it */
   XmListSelectPos (paletteList, tmpPalette->item_position, TRUE);
   XmListSetBottomPos(paletteList, tmpPalette->item_position);
   selected_position = tmpPalette->item_position;

}

static void 
modifyColorCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    ColorSet *color_set;

    if(TypeOfMonitor == XmCO_BLACK_WHITE)
       return;
        
    color_set = (ColorSet *) &pCurrentPalette->color[selected_button];
    ColorEditor(style.colorDialog,color_set);

}


/*
**  dialogBoxCB
**      Process callback from the Ok, Cancel and Help pushButtons in the 
**      DialogBox.
*/
static void 
dialogBoxCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  palette *tmp_palette;
  Bool match = FALSE;
  DtDialogBoxCallbackStruct *cb = (DtDialogBoxCallbackStruct *) call_data;
  
  switch (cb->button_position)
    {
    case OK_BUTTON:
      if ((edit.DialogShell != NULL) && (XtIsManaged(edit.DialogShell)))
	XtUnmanageChild(edit.DialogShell);
      
      XtUnmanageChild(style.colorDialog);
      UpdateDefaultPalette();
      break;
      
    case CANCEL_BUTTON:
      if ((edit.DialogShell != NULL) && (XtIsManaged(edit.DialogShell)))
	XtUnmanageChild(edit.DialogShell);
      
      if ((addDialog != NULL) && (XtIsManaged(addDialog)))
	XtCallCallbacks(addDialog, XmNcancelCallback, (XtPointer)NULL);
      
      if ((deleteDialog != NULL) && (XtIsManaged(deleteDialog)))
	XtCallCallbacks(deleteDialog, XmNcancelCallback, (XtPointer)NULL);
      
      if ((colorUseDialog != NULL) && (XtIsManaged(colorUseDialog)))
	XtCallCallbacks(colorUseDialog, XmNcallback, (XtPointer)NULL);
      
      XtUnmanageChild(style.colorDialog);
      
      tmp_palette = pHeadPalette;
      for(tmp_palette = pHeadPalette; tmp_palette != NULL; tmp_palette = tmp_palette->next)
	if(!(strcmp(tmp_palette->name, defaultName)))
	  {
	    match = TRUE;
	    break;
	  }
      
      if(match == FALSE)  /* the default palette is no longer valid */
	UpdateDefaultPalette();
      else 
	RestoreOrgPalette();
      
      break;
      
    case HELP_BUTTON:
      XtCallCallbacks(style.colorDialog, XmNhelpCallback, (XtPointer)NULL);
      break;
      
    default:
      break;
    }
}


static void 
AddName(
        palette *newPalette )
{
    XmString         string;

/*
**  Add the palette name to the list
*/
    string = CMPSTR(newPalette->desc);
    XmListAddItem(paletteList, string, newPalette->item_position);
    XmListSelectPos(paletteList, newPalette->item_position, TRUE);
    XmListSetBottomPos(paletteList, newPalette->item_position);
    selected_position = newPalette->item_position;
    XSync(style.display, 0);
    XmStringFree(string);
}

static void 
deletePaletteCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    register int     n;
    Arg              args[10];
    char            *tmpStr;
    palette         *tmp_palette;
    char            *string;
    char            *class_str;
    char            *str_type_return;
    XrmValue         value_return;

    tmp_palette = pHeadPalette;

    while( tmp_palette->item_position != selected_position &&
					      tmp_palette != NULL)
	 tmp_palette = tmp_palette->next;
    
    if (deleteDialog == NULL)
    {
	n=0;
        XtSetArg(args[n], XmNokLabelString, CMPSTR((String) _DtOkString)); n++;
        XtSetArg(args[n], XmNcancelLabelString, CMPSTR((String) _DtCancelString)); n++;
        XtSetArg(args[n], XmNhelpLabelString, CMPSTR((String) _DtHelpString)); n++;
	XtSetArg(args[n], XmNdialogType, XmDIALOG_INFORMATION);             n++;
	XtSetArg(args[n], XmNborderWidth, 3);                               n++;
        XtSetArg(args[n], XmNdefaultPosition, False);                      n++;
	deleteDialog = XmCreateQuestionDialog(style.colorDialog,
                                              "deleteDialog", args, n);
        XtAddCallback(deleteDialog, XmNmapCallback, CenterMsgCB,
                      style.colorDialog);
	XtAddCallback(deleteDialog, XmNcancelCallback, deleteCancelCB, NULL);
	XtAddCallback(deleteDialog, XmNokCallback, deleteOkCB, 
                      (XtPointer)(paletteList));
        XtAddCallback(deleteDialog, XmNhelpCallback,
                      (XtCallbackProc)HelpRequestCB, 
                      (XtPointer)HELP_DELETE_PALETTE_WARNING_DIALOG);


        n = 0;
        XtSetArg (args[n], XmNmwmInputMode,
                        MWM_INPUT_PRIMARY_APPLICATION_MODAL); n++;
        XtSetArg (args[n], XmNuseAsyncGeometry, True); n++;
        XtSetArg (args[n], XmNtitle, ((char *)GETMESSAGE(14, 16, "Delete Palette"))); n++;
        XtSetArg (args[n], XmNmwmFunctions, DIALOG_MWM_FUNC); n++;
        XtSetValues (XtParent (deleteDialog), args, n);

    }

    n = 0;
    tmpStr = XtMalloc(strlen(STR2) + strlen(tmp_palette->desc) + 1);
    sprintf(tmpStr, STR2, tmp_palette->desc);
    XtSetArg(args[n], XmNmessageString, CMPSTR(tmpStr)); n++;
    XtSetValues(deleteDialog, args, n);
    XtFree (tmpStr);

    XtManageChild(deleteDialog);
}

static void 
deleteOkCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{

    XtUnmanageChild(deleteDialog);

    if(NumOfPalettes == 1)
    {
       InfoDialog(CANT_DELETE, style.colorDialog, False);
    } 
    else
    {
       if (RemovePalette() == True)
           DeletePaletteFromLinkList((Widget)client_data);
    }
}

static void 
deleteCancelCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
        XtUnmanageChild(deleteDialog);
}

static void 
resourcesCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{

    register int     n;
    Arg              args[12];
    XmString         button_string[NUM_LABELS]; 
    XmString	     string;
    Widget           parent = (Widget) client_data;
    Widget           colorUseTB;
    Widget           form;
    Widget           widget_list[10];
    int              count=0;
    Widget           pictLabel;
    Widget           colorUseRC;

    if (colorUseDialog == NULL)
    {
        n = 0;

        /* Set up DialogBox button labels. */
        button_string[0] = CMPSTR((String) _DtOkString);
        button_string[1] = CMPSTR((String) _DtCancelString);
        button_string[2] = CMPSTR((String) _DtHelpString);

        XtSetArg (args[n], XmNchildType, XmWORK_AREA);  n++;
        XtSetArg (args[n], XmNbuttonCount, NUM_LABELS);  n++;
        XtSetArg (args[n], XmNbuttonLabelStrings, button_string);  n++;
        XtSetArg (args[n], XmNdefaultPosition, False);  n++;
        colorUseDialog = __DtCreateDialogBoxDialog(parent, "colorUseDialog", 
                          args, n);
        XtAddCallback(colorUseDialog, XmNcallback, colorUseExitCB, NULL);
        XtAddCallback(colorUseDialog, XmNmapCallback, _DtmapCB_colorUse, NULL);
        XtAddCallback(colorUseDialog, XmNhelpCallback,
            (XtCallbackProc)HelpRequestCB, (XtPointer)HELP_COLOR_USE_DIALOG);

        XmStringFree(button_string[0]);
        XmStringFree(button_string[1]);
        XmStringFree(button_string[2]);

        widget_list[0] = _DtDialogBoxGetButton(colorUseDialog,2);
        n=0;
        XtSetArg(args[n], XmNautoUnmanage, False); n++;
        XtSetArg(args[n], XmNcancelButton, widget_list[0]); n++;
        XtSetValues (colorUseDialog, args, n);

        n = 0;
        XtSetArg (args[n], XmNuseAsyncGeometry, True); n++;
        XtSetArg (args[n], XmNtitle, 
            ((char *)GETMESSAGE(14, 39, "Number Of Colors To Use:"))); n++;
        XtSetArg (args[n], XmNmwmFunctions, DIALOG_MWM_FUNC); n++;
        XtSetValues (XtParent(colorUseDialog), args, n);

        n = 0;
        XtSetArg(args[n], XmNhorizontalSpacing, style.horizontalSpacing); n++;
        XtSetArg(args[n], XmNverticalSpacing, style.verticalSpacing); n++;
        XtSetArg(args[n], XmNallowOverlap, False); n++;
        XtSetArg(args[n], XmNchildType, XmWORK_AREA);  n++;
        form = XmCreateForm(colorUseDialog, "colorUseForm", args, n);
        XtManageChild(form);

        n = 0;
        XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);  n++;
        XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
        XtSetArg (args[n], XmNrightAttachment, XmATTACH_NONE);  n++;
        XtSetArg (args[n], XmNbottomAttachment, XmATTACH_NONE);  n++;
        XtSetArg (args[n], XmNbehavior, XmICON_LABEL); n++;
        XtSetArg (args[n], XmNshadowThickness, 0); n++;  
        XtSetArg (args[n], XmNstring, NULL); n++;  
        XtSetArg (args[n], XmNpixmapForeground, style.primBSCol); n++;
        XtSetArg (args[n], XmNpixmapBackground, style.primTSCol); n++;
        XtSetArg (args[n], XmNimageName, COLOR_ICON); n++;  
        XtSetArg (args[n], XmNtraversalOn, False); n++;  
        pictLabel = _DtCreateIcon(form, "pictLabel", args, n);
        XtManageChild(pictLabel);

        n = 0;
        XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);  n++;
        XtSetArg (args[n], XmNtopWidget, pictLabel);  n++;
        XtSetArg (args[n], XmNtopOffset, style.verticalSpacing);  n++;
        XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
        XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);  n++;
        XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);  n++;
        colorUseTB 
            = XmCreateFrame(form, "colorUseTB", args, n);
        XtManageChild(colorUseTB);

        /* create a rowColumn for ColorUse selections */
        n = 0;
        colorUseRC = XmCreateRadioBox(colorUseTB, "colorUseRC", args, n);
        XtManageChild(colorUseRC);

        n = 0;
	string = CMPSTR(((char *)GETMESSAGE(14, 41, "More Colors for Desktop")));
        XtSetArg(args[n], XmNlabelString, string); n++;
	widget_list[count++] = colorDialog.highColorTG 
            =  XmCreateToggleButtonGadget(colorUseRC,"highColorTG", args, n);
        XtAddCallback(colorDialog.highColorTG, XmNvalueChangedCallback, 
                        colorUseCB, (XtPointer)XmCO_HIGH_COLOR);  
        XmStringFree(string);

        n = 0;
	string = CMPSTR(((char *)GETMESSAGE(14, 42, "More Colors for Applications")));
        XtSetArg(args[n], XmNlabelString, string); n++;
        widget_list[count++] = colorDialog.mediumColorTG 
            = XmCreateToggleButtonGadget(colorUseRC,"mediumColorTG", args, n);
        XmStringFree(string);
        XtAddCallback(colorDialog.mediumColorTG, XmNvalueChangedCallback, 
                        colorUseCB, (XtPointer)XmCO_MEDIUM_COLOR);  

        n = 0;
	string = CMPSTR(((char *)GETMESSAGE(14, 43, "Most Colors for Applications")));
        XtSetArg(args[n], XmNlabelString, string); n++;
        widget_list[count++] = colorDialog.lowColorTG 
            = XmCreateToggleButtonGadget(colorUseRC,"lowColorTG", args, n);
        XmStringFree(string);
        XtAddCallback(colorDialog.lowColorTG, XmNvalueChangedCallback, 
                        colorUseCB, (XtPointer)XmCO_LOW_COLOR);  

        n = 0;
	string = CMPSTR(((char *)GETMESSAGE(14, 34, "Black and White")));
        XtSetArg(args[n], XmNlabelString, string); n++;
        widget_list[count++] = colorDialog.blackWhiteTG 
            = XmCreateToggleButtonGadget(colorUseRC,"blackWhiteTG", args, n);
        XmStringFree(string);
        XtAddCallback(colorDialog.blackWhiteTG, XmNvalueChangedCallback, 
                        colorUseCB, (XtPointer)XmCO_BLACK_WHITE);  

        n = 0;
	string = CMPSTR(((char *)GETMESSAGE(14, 35, "Default")));
        XtSetArg(args[n], XmNlabelString, string); n++;
        widget_list[count++] = colorDialog.defaultTG 
            = XmCreateToggleButtonGadget(colorUseRC,"defaultTG", args, n);
        XmStringFree(string);
        XtAddCallback(colorDialog.defaultTG, XmNvalueChangedCallback, 
                        colorUseCB, (XtPointer)DEFAULT_COLOR);  

        XtManageChildren(widget_list,count);
        putDialog (XtParent(style.colorDialog), colorUseDialog); 
    }

    XtManageChild(colorUseDialog);
}

/*
**  colorUseCB
**      Process new ColorUse selection
*/
static void 
colorUseCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    Arg              args[4];
    XmToggleButtonCallbackStruct *cb = 
            (XmToggleButtonCallbackStruct *)call_data;

    colorDialog.currentColorUse = (int) client_data;
    switch (colorDialog.currentColorUse)
    {
        case XmCO_HIGH_COLOR:
            colorDialog.currentColorUseStr = HIGH_COLOR_STR;
            break;

        case XmCO_MEDIUM_COLOR:
            colorDialog.currentColorUseStr = MEDIUM_COLOR_STR;
            break;

        case XmCO_LOW_COLOR:
            colorDialog.currentColorUseStr = LOW_COLOR_STR;
            break;

        case XmCO_BLACK_WHITE:
            colorDialog.currentColorUseStr = B_W_STR;
            break;

        case DEFAULT_COLOR:
            colorDialog.currentColorUseStr = DEFAULT_COLOR_STR;
            break;
    }


}


/*
**  colorUseExitCB
**      Process callback from the Ok, Cancel and Help pushButtons in the 
**      Configure DT Colors DialogBox.
*/
static void 
colorUseExitCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{

    char        colorUseRes[64];
    DtDialogBoxCallbackStruct *cb = (DtDialogBoxCallbackStruct *) call_data;

    switch (cb->button_position)
    {
      case HELP_BUTTON:
          XtCallCallbacks(colorUseDialog, XmNhelpCallback, (XtPointer)NULL);
          break;

      case OK_BUTTON:

         XtUnmanageChild(colorUseDialog);

         if (colorDialog.origColorUse != colorDialog.currentColorUse)
         {
             InfoDialog(COLORUSE_WHEN, style.colorDialog, False); 

             /* create the ColorUse resource spec for xrdb */
             /* remove ColorUse specification from database for DEFAULT_COLOR */

             sprintf(colorUseRes, "*%d*ColorUse: %s\n", 
                 style.screenNum,colorDialog.currentColorUseStr);

             switch (colorDialog.currentColorUse)
             {
                case XmCO_MEDIUM_COLOR:
                    sprintf(colorUseRes+strlen(colorUseRes), 
                             "*HelpColorUse: GRAY_SCALE\n");

                    break;

                case XmCO_LOW_COLOR:
                case XmCO_BLACK_WHITE:
                    sprintf(colorUseRes+strlen(colorUseRes), 
                             "*HelpColorUse: B_W\n");

                    break;

                case XmCO_HIGH_COLOR:
                case DEFAULT_COLOR:
                default:
                    sprintf(colorUseRes+strlen(colorUseRes), 
                             "*HelpColorUse: COLOR\n");
                    break;
             }

             _DtAddToResource(style.display, colorUseRes);

             colorDialog.origColorUse = colorDialog.currentColorUse;
         }

         break;

      case CANCEL_BUTTON:
      default:

         XtUnmanageChild(colorUseDialog);
         
         switch (colorDialog.origColorUse)
         {
           case XmCO_HIGH_COLOR:
             XmToggleButtonGadgetSetState(colorDialog.highColorTG, True, True);
             break;

           case XmCO_MEDIUM_COLOR:
             XmToggleButtonGadgetSetState(colorDialog.mediumColorTG, True, True);
             break;

            case XmCO_LOW_COLOR:
             XmToggleButtonGadgetSetState(colorDialog.lowColorTG, True, True);
             break;

            case XmCO_BLACK_WHITE:
             XmToggleButtonGadgetSetState(colorDialog.blackWhiteTG, True, True);
             break;

            case DEFAULT_COLOR:
             XmToggleButtonGadgetSetState(colorDialog.defaultTG, True, True);
             break;
          }
          break;

    }
}


static void 
activateCBexitColor(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  DtDialogBoxCallbackStruct CancelBut;

  if(style.colorDialog != NULL && XtIsManaged(style.colorDialog)) {
     CancelBut.button_position = CANCEL_BUTTON;
     XtCallCallbacks(style.colorDialog, XmNcallback, &CancelBut);
  }
}

static void 
_DtmapCB(
        Widget w,
        XtPointer client_data,

        XtPointer call_data )
{

    DtWsmRemoveWorkspaceFunctions(style.display, XtWindow(XtParent(w)));

    if (!save.restoreFlag)
        putDialog ((Widget)client_data, XtParent(w));

    XtRemoveCallback(style.colorDialog, XmNmapCallback, _DtmapCB, NULL);
}

static void 
_DtmapCB_colorUse(
        Widget w,
        XtPointer client_data,

        XtPointer call_data )
{

    char *str_type_return;
    XrmValue value_return;
    XrmValue    cvt_value;
    XrmDatabase db;
    Boolean status;
    char *string;
    char instanceString[24], nameString[24];

    DtWsmRemoveWorkspaceFunctions(style.display, XtWindow(XtParent(w)));

    db = XtDatabase(style.display);

    /* Get ColorUse value */
    sprintf (instanceString, "dtsession*%d*colorUse",style.screenNum);
    sprintf (nameString, "Dtsession*%d*ColorUse",style.screenNum);

    if (status = XrmGetResource (db, instanceString,
                                 nameString,
                                 &str_type_return, &value_return))
    {
        /* make local copy of string */
        string = (char *) XtMalloc( value_return.size );
        strcpy (string, value_return.addr);

        if (strcmp(string, HIGH_COLOR_STR) == 0)
        {
            XmToggleButtonGadgetSetState (colorDialog.highColorTG, True, True); 
            colorDialog.origColorUse = XmCO_HIGH_COLOR;
        }
        else if (strcmp(string, MEDIUM_COLOR_STR) == 0)
        {
            XmToggleButtonGadgetSetState (colorDialog.mediumColorTG, True, True); 
            colorDialog.origColorUse = XmCO_MEDIUM_COLOR;
        }
        else if (strcmp(string, LOW_COLOR_STR) == 0)
        {
            XmToggleButtonGadgetSetState (colorDialog.lowColorTG, True, True); 
            colorDialog.origColorUse = XmCO_LOW_COLOR;
        }
        else if (strcmp(string, B_W_STR) == 0)
        {
            XmToggleButtonGadgetSetState (colorDialog.blackWhiteTG, True, True); 
            colorDialog.origColorUse = XmCO_BLACK_WHITE;
        }
        else 
        {
            XmToggleButtonGadgetSetState (colorDialog.defaultTG, True, True); 
            colorDialog.origColorUse = DEFAULT_COLOR;
        }

        XtFree (string);
    }
    else /* ColorUse not specified */
    {
        XmToggleButtonGadgetSetState (colorDialog.defaultTG, True, True); 
        colorDialog.origColorUse = DEFAULT_COLOR;
    } 

    XtRemoveCallback(colorUseDialog, XmNmapCallback, _DtmapCB_colorUse, NULL);
}

/************************************************************************
 *
 *  DeletePaletteFromLinkList - routine used to delete a palette from
 *       the link list of palettes.  The palette which is at the current
 *       selected_position is the palette that is going to be deleted.
 *       Special things have to happen if the selected palette is at the
 *       head of the list.
 *
 ************************************************************************/ 
void
DeletePaletteFromLinkList(
        Widget list )
{
    register int n;
    Arg args[2];
    int i;
    XmString        string; 
    palette        *tmp_palette, *tmp2_palette;
    palette        *selected_palette;


    selected_palette = pHeadPalette;
    while( selected_palette->item_position != selected_position &&
					      selected_palette != NULL)
	 selected_palette = selected_palette->next;

    XmListDeletePos (list, selected_palette->item_position);

    /* delete item from palette list structure */

    /* If the palette is at the head .. remove the head and the next
         palette becomes the new selected palette */
    if (selected_palette->item_position == 1)
    {
        pHeadPalette = selected_palette->next;
        tmp_palette = pHeadPalette;
        tmp_palette->item_position--;
       /* new current palette */
        pCurrentPalette = tmp_palette;
    }
    else  /* find the palette just above the palette to be deleted .. it
             will become the new selected palette */
    {
        tmp_palette = pHeadPalette;
        for (i=1; i < selected_palette->item_position-1; i++)
            tmp_palette = tmp_palette->next;

        tmp_palette->next = selected_palette->next;

       /* what is CurrentPalette now? prev or next item?
           special case empty list or NULL entry */
       if (tmp_palette->next != NULL)
           pCurrentPalette = tmp_palette->next;
       else
           pCurrentPalette = tmp_palette;
    }

    /* decrement item_positions values in remaining palette entries */
    tmp2_palette = tmp_palette;
    while ((tmp2_palette = tmp2_palette->next) != NULL)
    {
        tmp2_palette->item_position--;
    }

   /* go copy the pixel numbers to the new palette */
    CopyPixel(selected_palette->color, pCurrentPalette->color,
                                      selected_palette->num_of_colors);

    /* select item in list as if user had selected it */
    XmListSelectPos (list, tmp_palette->item_position, TRUE);
    /* Need to check to see if the first palette is being deleted if it is
       need to change colors and update title box */
    if(selected_position == tmp_palette->item_position) {
       pOldPalette = selected_palette;

       n=0;
       string = CMPSTR(pCurrentPalette->desc);
       XtSetArg (args[n], XmNtitleString, string); n++;
       XtSetValues (colorDialog.buttonsTB, args, n);
       XmStringFree(string);

       ReColorPalette();
    }

    XmListSetBottomPos(paletteList, tmp_palette->item_position);
    selected_position = tmp_palette->item_position;

    NumOfPalettes--;

   /* deallocate the palette structure */
    XtFree(selected_palette->name);
    XtFree(selected_palette->desc);
    XtFree(selected_palette->directory);
    XtFree((char *)selected_palette);
}

void
CopyPixel(
        ColorSet srcPixels[XmCO_MAX_NUM_COLORS],
        ColorSet dstPixels[XmCO_MAX_NUM_COLORS],
	int numOfColors )
{
   int i;

   for(i=0; i < numOfColors; i++)
   {
      dstPixels[i].bg.pixel = srcPixels[i].bg.pixel;
      dstPixels[i].fg.pixel = srcPixels[i].fg.pixel;
      dstPixels[i].ts.pixel = srcPixels[i].ts.pixel;
      dstPixels[i].bs.pixel = srcPixels[i].bs.pixel;
      dstPixels[i].sc.pixel = srcPixels[i].sc.pixel;
   }
}

void 
SaveOrgPalette( void )
{
   int i;
   palette  *tmp_palette, *tmp2_palette;

   if(save.restoreFlag && defaultName_restore[0] != 0) {
      tmp_palette = pHeadPalette;
      while(tmp_palette->next != NULL )
         if(strcmp(tmp_palette->name, defaultName))
         {
            tmp_palette = tmp_palette->next;
         }
         else
         {
             break;
         }
      if(!strcmp(tmp_palette->name, defaultName)) {
         tmp2_palette = pCurrentPalette;
         pCurrentPalette = tmp_palette;
      }
      else
         tmp_palette = NULL;
   }

   OrgPalette.item_position = pCurrentPalette->item_position;
   OrgPalette.num_of_colors = pCurrentPalette->num_of_colors;
   for(i = 0; i < XmCO_MAX_NUM_COLORS; i++)
   {
       OrgPalette.primary = pCurrentPalette->primary;
       OrgPalette.secondary = pCurrentPalette->secondary;
       OrgPalette.active = pCurrentPalette->active;
       OrgPalette.inactive = pCurrentPalette->inactive;

       if(save.restoreFlag && defaultName_restore[0] != 0)
          OrgPalette.color[i].bg.pixel = tmp2_palette->color[i].bg.pixel;
       else
          OrgPalette.color[i].bg.pixel = pCurrentPalette->color[i].bg.pixel;
       OrgPalette.color[i].bg.red = pCurrentPalette->color[i].bg.red;
       OrgPalette.color[i].bg.green = pCurrentPalette->color[i].bg.green;
       OrgPalette.color[i].bg.blue = pCurrentPalette->color[i].bg.blue;

       if(save.restoreFlag && defaultName_restore[0] != 0)
          OrgPalette.color[i].fg.pixel = tmp2_palette->color[i].fg.pixel;
       else
          OrgPalette.color[i].fg.pixel = pCurrentPalette->color[i].fg.pixel;
       OrgPalette.color[i].fg.red = pCurrentPalette->color[i].fg.red;
       OrgPalette.color[i].fg.green = pCurrentPalette->color[i].fg.green;
       OrgPalette.color[i].fg.blue = pCurrentPalette->color[i].fg.blue;

       if(save.restoreFlag && defaultName_restore[0] != 0)
          OrgPalette.color[i].ts.pixel = tmp2_palette->color[i].ts.pixel;
       else
          OrgPalette.color[i].ts.pixel = pCurrentPalette->color[i].ts.pixel;
       OrgPalette.color[i].ts.red = pCurrentPalette->color[i].ts.red;
       OrgPalette.color[i].ts.green = pCurrentPalette->color[i].ts.green;
       OrgPalette.color[i].ts.blue = pCurrentPalette->color[i].ts.blue;

       if(save.restoreFlag && defaultName_restore[0] != 0)
          OrgPalette.color[i].bs.pixel = tmp2_palette->color[i].bs.pixel;
       else
          OrgPalette.color[i].bs.pixel = pCurrentPalette->color[i].bs.pixel;
       OrgPalette.color[i].bs.red = pCurrentPalette->color[i].bs.red;
       OrgPalette.color[i].bs.green = pCurrentPalette->color[i].bs.green;
       OrgPalette.color[i].bs.blue = pCurrentPalette->color[i].bs.blue;

       if(save.restoreFlag && defaultName_restore[0] != 0)
          OrgPalette.color[i].sc.pixel = tmp2_palette->color[i].sc.pixel;
       else
          OrgPalette.color[i].sc.pixel = pCurrentPalette->color[i].sc.pixel;
       OrgPalette.color[i].sc.red = pCurrentPalette->color[i].sc.red;
       OrgPalette.color[i].sc.green = pCurrentPalette->color[i].sc.green;
       OrgPalette.color[i].sc.blue = pCurrentPalette->color[i].sc.blue;
   }

   if(save.restoreFlag && defaultName_restore[0] != 0) 
      if(tmp_palette != NULL)
         pCurrentPalette = tmp2_palette;

}

void
RestoreOrgPalette( void )
{
   int i;
    palette  *tmp_palette;
   int         j=0;
   XColor      colors[XmCO_MAX_NUM_COLORS * 5];

   tmp_palette = pHeadPalette;
   while ( tmp_palette != NULL &&
           tmp_palette->item_position != OrgPalette.item_position )
      tmp_palette = tmp_palette->next;

   if ( tmp_palette != NULL &&
	tmp_palette->item_position == OrgPalette.item_position)
   {
      pCurrentPalette = tmp_palette;
      OrgPalette.num_of_colors = pCurrentPalette->num_of_colors;
      for(i = 0; i < XmCO_MAX_NUM_COLORS; i++)
      {
          pCurrentPalette->primary = OrgPalette.primary;
          pCurrentPalette->secondary = OrgPalette.secondary;
          pCurrentPalette->inactive = OrgPalette.inactive;
          pCurrentPalette->active = OrgPalette.active;

          pCurrentPalette->color[i].bg.pixel = OrgPalette.color[i].bg.pixel;
          pCurrentPalette->color[i].bg.red = OrgPalette.color[i].bg.red;
          pCurrentPalette->color[i].bg.green = OrgPalette.color[i].bg.green;
          pCurrentPalette->color[i].bg.blue = OrgPalette.color[i].bg.blue;
          if(i < OrgPalette.num_of_colors && TypeOfMonitor != XmCO_BLACK_WHITE)
             colors[j++] =  pCurrentPalette->color[i].bg;

          pCurrentPalette->color[i].sc.pixel = OrgPalette.color[i].sc.pixel;
          pCurrentPalette->color[i].sc.red = OrgPalette.color[i].sc.red;
          pCurrentPalette->color[i].sc.green = OrgPalette.color[i].sc.green;
          pCurrentPalette->color[i].sc.blue = OrgPalette.color[i].sc.blue;
          if(i < OrgPalette.num_of_colors && TypeOfMonitor != XmCO_BLACK_WHITE)
             colors[j++] =  pCurrentPalette->color[i].sc;

          pCurrentPalette->color[i].fg.pixel = OrgPalette.color[i].fg.pixel;
          pCurrentPalette->color[i].fg.red = OrgPalette.color[i].fg.red;
          pCurrentPalette->color[i].fg.green = OrgPalette.color[i].fg.green;
          pCurrentPalette->color[i].fg.blue = OrgPalette.color[i].fg.blue;
          if(i < OrgPalette.num_of_colors && TypeOfMonitor != XmCO_BLACK_WHITE)
             if(FgColor == DYNAMIC)
                colors[j++] =  pCurrentPalette->color[i].fg;

          pCurrentPalette->color[i].ts.pixel = OrgPalette.color[i].ts.pixel;
          pCurrentPalette->color[i].ts.red = OrgPalette.color[i].ts.red;
          pCurrentPalette->color[i].ts.green = OrgPalette.color[i].ts.green;
          pCurrentPalette->color[i].ts.blue = OrgPalette.color[i].ts.blue;
          if(i < OrgPalette.num_of_colors && TypeOfMonitor != XmCO_BLACK_WHITE)
             if(UsePixmaps == FALSE)
                colors[j++] =  pCurrentPalette->color[i].ts;

          pCurrentPalette->color[i].bs.pixel = OrgPalette.color[i].bs.pixel;
          pCurrentPalette->color[i].bs.red = OrgPalette.color[i].bs.red;
          pCurrentPalette->color[i].bs.green = OrgPalette.color[i].bs.green;
          pCurrentPalette->color[i].bs.blue = OrgPalette.color[i].bs.blue;
          if(i < OrgPalette.num_of_colors && TypeOfMonitor != XmCO_BLACK_WHITE)
             if(UsePixmaps == FALSE)
                colors[j++] =  pCurrentPalette->color[i].bs;

      }

      if (style.dynamicColor)
          XStoreColors (style.display, style.colormap, colors, j);

      XmListSelectPos (paletteList, OrgPalette.item_position, TRUE);
      XmListSetBottomPos(paletteList, OrgPalette.item_position);

   }
}

void 
     UpdateDefaultPalette( void )
{
  int      i;
   char     temp[XmCO_MAX_NUM_COLORS][60];
   char     xrdb_string[100];
   XColor   saved_color[2];
   
   for(i = 0; i < XmCO_MAX_NUM_COLORS; i++)
     {
     /* put the colors of the palette in the form #RRRRGGGGBBBB */
     if(TypeOfMonitor == XmCO_LOW_COLOR && (i == 0 || i == 1))
	sprintf(temp[i],"#%04x%04x%04x\n", saved_color[i].red,
		 saved_color[i].green,
		 saved_color[i].blue);
      else if(TypeOfMonitor == XmCO_LOW_COLOR && i == 2)
	sprintf(temp[i],"#%04x%04x%04x\n",pCurrentPalette->color[1].bg.red,
		  pCurrentPalette->color[1].bg.green,
		  pCurrentPalette->color[1].bg.blue);
      else if(TypeOfMonitor == XmCO_LOW_COLOR && i == 3)
	sprintf(temp[i],"#%04x%04x%04x\n",pCurrentPalette->color[0].bg.red,
		  pCurrentPalette->color[0].bg.green,
		  pCurrentPalette->color[0].bg.blue);
      else
	sprintf(temp[i],"#%04x%04x%04x\n",pCurrentPalette->color[i].bg.red,
		  pCurrentPalette->color[i].bg.green,
		  pCurrentPalette->color[i].bg.blue);
   }
   
   /* update the resource manager property with the palette resource */
   if (TypeOfMonitor == XmCO_BLACK_WHITE)
      {
      sprintf(xrdb_string, "*%d*MonochromePalette: %s%s\n",
                style.screenNum, pCurrentPalette->name, PALETTE_SUFFIX);
      }
    else
      {
      sprintf(xrdb_string, "*%d*ColorPalette: %s%s\n",
                style.screenNum, pCurrentPalette->name, PALETTE_SUFFIX);
      }
   _DtAddToResource(style.display, xrdb_string);
   
   /* update the defaultName */
   XtFree(defaultName);
 defaultName = (char *)XtMalloc(strlen(pCurrentPalette->name)+1);
   strcpy(defaultName, pCurrentPalette->name);
   

   /* update Xrdb for non Motif1.1 clients */
   if (style.xrdb.writeXrdbColors)
     {
#ifdef sun
       OWsyncColorResources(style.display, TypeOfMonitor, 
		pCurrentPalette->color);
#else
       int chipnum = TypeOfMonitor == XmCO_HIGH_COLOR ? 4 : 1;
       sprintf(xrdb_string, 
	       "*background: #%04X%04X%04X\n*foreground: #%04X%04X%04X\n",
	       pCurrentPalette->color[chipnum].bg.red, 
	       pCurrentPalette->color[chipnum].bg.green, 
	       pCurrentPalette->color[chipnum].bg.blue,
	       pCurrentPalette->color[chipnum].fg.red, 
	       pCurrentPalette->color[chipnum].fg.green, 
	       pCurrentPalette->color[chipnum].fg.blue);
       _DtAddToResource(style.display, xrdb_string);
#endif
   }
}
 


void 
show_selection(
        Widget w,
        XtPointer client_data,
        Atom *selection,
        Atom *type,
        XtPointer value,
        unsigned long *length,
        int *format )
{
    int      dynamic_color;

    style.colorSrv = True;
    if(value != NULL)
    {
       if((int)client_data == GET_TYPE_MONITOR)
       {
          sscanf ((char *)value, "%x_%x_%x_%x", (unsigned int *) &(TypeOfMonitor),
                                     (unsigned int *) &(UsePixmaps), (unsigned int *) &(FgColor), (unsigned int *) &dynamic_color);
          if(dynamic_color == FALSE)
             style.dynamicColor = False;
          else
             style.dynamicColor = True;
       }

       WaitSelection = FALSE;
       free(value);
    }
    else /* no response from Color Server - it must not be there */
    {
       style.colorSrv = False;
       WaitSelection = FALSE;
    }
}

/************************************************************************
 * restoreColor()
 *
 * restore any state information saved with saveBackdrop.
 * This is called from restoreSession with the application
 * shell and the special xrm database retrieved for restore.
 ************************************************************************/
void 
restoreColor(
        Widget shell,
        XrmDatabase db )
{
    XrmName xrm_name[5];
    XrmRepresentation rep_type;
    XrmValue value;
    palette *tmp_palette;

    /*"paletteDlg" is the resource name of the dialog shell we are saving for.*/
    xrm_name [0] = XrmStringToQuark (PALETTEDLG);
    xrm_name [2] = 0;

    /* get x position */
    xrm_name [1] = XrmStringToQuark ("x");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value)){
      XtSetArg (save.posArgs[save.poscnt], XmNx, atoi((char *)value.addr)); save.poscnt++;
    }

    /* get y position */
    xrm_name [1] = XrmStringToQuark ("y");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value)){
      XtSetArg (save.posArgs[save.poscnt], XmNy, atoi((char *)value.addr)); save.poscnt++;
    }

    /* get selected palette */
    xrm_name [1] = XrmStringToQuark ("selected_palette");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value)){
       strcpy(defaultName_restore, value.addr);
    }
    else
       defaultName_restore[0] = 0;

    /* get selected button */
    xrm_name [1] = XrmStringToQuark ("selected_button");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value)){
      selected_button = atoi((char *)value.addr);
    }

   /* need to have some variables initialized before creating the
        Color's dialog ... */
    InitializeAtoms();
    CheckMonitor(shell);
    GetDefaultPal(shell);

    xrm_name [1] = XrmStringToQuark ("ismapped");
    XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
    /* Are we supposed to be mapped? */
    if (strcmp(value.addr, "True") == 0) {
      save.restoreFlag = True;
      Customize(shell);
    }
}


/************************************************************************
 * saveColor()
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
saveColor(
        int fd )
{
    Position x,y;
    char *bufr = style.tmpBigStr;     /* size=[1024], make bigger if needed */
    XmVendorShellExtObject  vendorExt;
    XmWidgetExtData         extData;

    if (style.colorDialog != NULL) 
    {
	if (XtIsManaged(style.colorDialog))
	  sprintf(bufr, "*paletteDlg.ismapped: True\n");
	else
	  sprintf(bufr, "*paletteDlg.ismapped: False\n");

	/* Get and write out the geometry info for our Window */
	x = XtX(XtParent(style.colorDialog));
	y = XtY(XtParent(style.colorDialog));

        /* Modify x & y to take into account window mgr frames
         * This is pretty bogus, but I don't know a better way to do it.
         */
        extData = _XmGetWidgetExtData(style.shell, XmSHELL_EXTENSION);
        vendorExt = (XmVendorShellExtObject)extData->widget;
        x -= vendorExt->vendor.xOffset;
        y -= vendorExt->vendor.yOffset;

	sprintf(bufr, "%s*paletteDlg.x: %d\n", bufr, x);
	sprintf(bufr, "%s*paletteDlg.y: %d\n", bufr, y);
	sprintf(bufr, "%s*paletteDlg.selected_palette: %s\n", bufr, 
		pCurrentPalette->name);
	sprintf(bufr, "%s*paletteDlg.selected_button: %d\n", bufr, 
		selected_button);
	write (fd, bufr, strlen(bufr));
    }
}

/**************************************************************************
 *
 * SameName - procedure used by the Add palette .. if the palette desc the 
 *            user selects is the same name as a palette already in the
 *            linked list this procedure gets called.  It set up a
 *            Warning dialog asking the user if they really want to add
 *            a palette with the same name as an existing palette.
 *
 **************************************************************************/
void
SameName(
        Widget w,
        palette *tmpPalette,
        char *name )
{
   char    *tmpStr;
   int n=0;
   Arg args[10];

   if (colorDialog.dlg == NULL) {
     tmpStr = (char *)XtMalloc(strlen(STR1) + strlen(name) + 1);
     sprintf(tmpStr, STR1, name);
     XtSetArg(args[n], XmNmessageString, CMPSTR(tmpStr)); n++;
     XtSetArg(args[n], XmNokLabelString, CMPSTR((String) _DtOkString)); n++;
     XtSetArg(args[n], XmNcancelLabelString, CMPSTR((String) _DtCancelString)); n++;
     XtSetArg(args[n], XmNhelpLabelString, CMPSTR((String) _DtHelpString)); n++;
     XtSetArg(args[n], XmNmwmFunctions, DIALOG_MWM_FUNC ); n++;
     XtSetArg(args[n], XmNdialogTitle, CMPSTR(((char *)GETMESSAGE(14, 21, "Warning")))); n++;
     colorDialog.dlg = XmCreateWarningDialog(style.colorDialog, "QNotice", args, n);
     XtAddCallback(colorDialog.dlg, XmNokCallback, setDlgOkCB, (XtPointer)tmpPalette);
     XtAddCallback(colorDialog.dlg, XmNhelpCallback,
              (XtCallbackProc)HelpRequestCB, (XtPointer)HELP_ADD_PALETTE_WARNING_DIALOG);

      n=0;
      XtSetArg (args[n], XmNmwmInputMode,
                        MWM_INPUT_PRIMARY_APPLICATION_MODAL); n++;
      XtSetValues (XtParent(colorDialog.dlg), args, n);
      XtFree(tmpStr);
   }
   XtManageChild(colorDialog.dlg);
}

/****************************************************************************
 *
 * ValidName - procedure which checks to make sure the name being passed
 *             in is a valid filename.  Weeds out many of the non 
 *             alphabit characters.
 *
 ***************************************************************************/
static Boolean 
ValidName(
        char *name )
{
   int i;
   Boolean valid = True;
#ifdef NLS16
   int chlen;
#endif  /* NLS16 */

   if (!name || strlen(name) == 0)
   {
      valid = False;
   }

#ifdef NLS16
   chlen = mblen (&name[0], MB_CUR_MAX);
   for (i = 0; (i < strlen(name)) && chlen && valid; i += chlen)
   {
      chlen = mblen (&name[i], MB_CUR_MAX);
      if ((chlen == 1) &&
          ((name[i] == '*') ||
           (name[i] == '"') ||
           (name[i] == '\\') ||
           (name[i] == '[') ||
           (name[i] == ']') ||
           (name[i] == '{') ||
           (name[i] == '}') ||
           (name[i] == '>') ||
           (name[i] == '<') ||
           (name[i] == '(') ||
           (name[i] == ')') ||
           (name[i] == '!') ||
           (name[i] == '|') ||
           (name[i] == ':') ||
           (name[i] == '/'))) 
      {
           valid = False;
      }
   }
#else /* NLS16 */
   for (i = 0; (i < strlen(name)) && valid; i++)
   {
      if (strchr ("|!(){}[]<>*:\"\\", (int) name[i]))
      {
          valid = False;
      }
   }
#endif /* NLS16 */
   return (valid);
}

void 
InitializeAtoms( void )
{
    char             cust_str[24];

    sprintf(cust_str,"%s%d", XmSCUSTOMIZE_DATA, style.screenNum);
    XA_CUSTOMIZE = XInternAtom(style.display, cust_str, FALSE);
    XA_TYPE_MONITOR = XInternAtom(style.display, TYPE_OF_MONITOR, FALSE);
    XA_WM_DELETE_WINDOW = XInternAtom(style.display, "WM_DELETE_WINDOW", False);
}

void 
GetDefaultPal(
        Widget shell )
{
    char *str_type_return;
    XrmValue value_return;
    XrmDatabase db;
    Boolean status;
    char *p;
    char *string;
    char instanceName[30], instanceClass[30];

    /* get the current default palette from the Reource Manager Property */

    db = XtDatabase(style.display);

    if (TypeOfMonitor == XmCO_BLACK_WHITE)   
    {
        sprintf(instanceName,"dtsession.%d.monochromePalette", style.screenNum);
        sprintf(instanceClass,"Dtsession.%d.MonochromePalette", style.screenNum);
    }
    else
    {
        sprintf(instanceName,"dtsession.%d.colorPalette", style.screenNum);
        sprintf(instanceClass,"Dtsession.%d.ColorPalette", style.screenNum);
    }

    if (status = XrmGetResource (db, instanceName, instanceClass,
                                     &str_type_return, &value_return))
    {
        /* copy string to defaultName */
        defaultName = (char *) XtMalloc( value_return.size );
        strcpy (defaultName, value_return.addr);
        p = strstr (defaultName, PALETTE_SUFFIX);
        if (p) *p = '\0';
    }
    else
    {
        /* set defaultName to default palette */
        defaultName = (char *) XtMalloc( strlen(DEFAULT_PALETTE));
        strcpy (defaultName, DEFAULT_PALETTE);
    }
}

void 
CreateDialogBoxD(
        Widget parent )
{
    XmString         button_string[NUM_LABELS]; 
    Arg              args[2];
    int		     n;
    Widget           w;

   if(style.count > 0)
      return;

    /* Get the default value of multiclick */
    dclick_time = XtGetMultiClickTime(style.display);

    gParent = parent;
    if(!XtIsRealized(parent))
       XtRealizeWidget(parent);

   /* Set up DialogBox button labels. */
    button_string[0] = CMPSTR((String) _DtOkString);
    button_string[1] = CMPSTR((String) _DtCancelString);
    button_string[2] = CMPSTR((String) _DtHelpString);

   /* saveRestore
    * Note that save.poscnt has been initialized elsewhere.  
    * save.posArgs may contain information from restoreColor().*/

    XtSetArg (save.posArgs[save.poscnt], XmNchildType, XmWORK_AREA);  save.poscnt++;
    XtSetArg (save.posArgs[save.poscnt], XmNbuttonCount, NUM_LABELS);  save.poscnt++;
    XtSetArg (save.posArgs[save.poscnt], XmNbuttonLabelStrings, button_string);  save.poscnt++;
    XtSetArg (save.posArgs[save.poscnt], XmNdefaultPosition, False); save.poscnt++;
    style.colorDialog = __DtCreateDialogBoxDialog(parent,PALETTEDLG, save.posArgs, save.poscnt);
    XtAddCallback(style.colorDialog, XmNcallback, dialogBoxCB, NULL);
    XtAddCallback(style.colorDialog, XmNmapCallback, _DtmapCB, parent);
    XtAddCallback(style.colorDialog, XmNhelpCallback,
            (XtCallbackProc)HelpRequestCB, (XtPointer)HELP_COLOR_DIALOG);

    XmStringFree(button_string[0]);
    XmStringFree(button_string[1]);
    XmStringFree(button_string[2]);

    w = _DtDialogBoxGetButton(style.colorDialog,2);
    n=0;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNcancelButton, w); n++;
    XtSetValues (style.colorDialog, args, n);

    style.count++;
}

void 
AddToDialogBox( void )
{
    register int     n;
    Arg              args[3];

   if(style.count > 1)
      return;

    n = 0;
    XtSetArg (args[n], XmNtitle, ((char *)GETMESSAGE(14, 30, "Style Manager - Color"))); n++;
    XtSetArg (args[n], XmNuseAsyncGeometry, True); n++;
    XtSetArg (args[n], XmNmwmFunctions, DIALOG_MWM_FUNC); n++;
    XtSetValues (XtParent(style.colorDialog), args, n);
    
    /* Add save session property to the main window */
    XmAddWMProtocolCallback(XtParent(style.colorDialog), 
                           XA_WM_DELETE_WINDOW, activateCBexitColor, NULL);

/*
**  Create a main form for color dialog
*/
    n = 0;
    XtSetArg(args[n], XmNhorizontalSpacing, style.horizontalSpacing); n++;
    XtSetArg(args[n], XmNverticalSpacing, style.verticalSpacing); n++;
    colorDialog.colorForm = XmCreateForm(style.colorDialog, "colorForm", args, n);
    XtManageChild(colorDialog.colorForm);
    
    style.count++;
}

void 
CreateTopColor1( void )
{
    register int     n;
    Arg              args[6];
    XmString         string; 

    if(style.count > 2)
       return;
/*
**  titlebox as child of the main form
*/
    n = 0;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);  n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);  n++;
    XtSetArg (args[n], XmNmarginWidth, 0);  n++;
    XtSetArg (args[n], XmNmarginHeight, 0);  n++;
    string = CMPSTR(((char *)GETMESSAGE(14, 5, "Palettes")));
    XtSetArg (args[n], XmNtitleString, string); n++;
    colorDialog.paletteTB = _DtCreateTitleBox(colorDialog.colorForm, "paletteTB", args, n);
    XtManageChild(colorDialog.paletteTB);
    XmStringFree(string);

/* 
**  Create a form inside palette titlebox 
*/
    n = 0;
    XtSetArg(args[n], XmNhorizontalSpacing, style.horizontalSpacing); n++;
    XtSetArg(args[n], XmNverticalSpacing, style.verticalSpacing); n++;
    colorDialog.palettesForm = XmCreateForm(colorDialog.paletteTB, "palettesForm", args, n);
    XtManageChild(colorDialog.palettesForm);

    style.count++;
}

void 
CreateTopColor2( void )
{
    register int     n;
    Arg              args[8];

    if(style.count > 3)
       return;
/*
**  Create a scrolled list widget.  This widget will contain the list of
**  palettes currently loaded (by ReadPalettes) in the customizer.
*/
    n=0;
    XtSetArg (args[n], XmNselectionPolicy, XmBROWSE_SELECT); n++;
    XtSetArg (args[n], XmNautomaticSelection, True); n++;
    XtSetArg (args[n], XmNvisibleItemCount, 6); n++;
    paletteList = XmCreateScrolledList(colorDialog.palettesForm,"paletteList",args,n);
    XtAddCallback(paletteList, XmNbrowseSelectionCallback,
                                              selectPaletteCB, NULL);

    n=0;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);  n++;
    XtSetArg (args[n], XmNtopOffset, style.horizontalSpacing);  n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
    XtSetArg (args[n], XmNleftOffset, style.horizontalSpacing);  n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);  n++;
    XtSetArg (args[n], XmNrightPosition, 60);  n++;
    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);  n++;
    XtSetValues (XtParent(paletteList), args, n);

    style.count++;
}

void 
CreateBottomColor( void )
{
    register int     n;
    Arg              args[12];
    XmString         string; 
    Widget           addDeleteForm;
    Widget           resourcesPB;

    colorDialog.dlg = NULL;

    if(style.count > 10)
       return;


    if(style.dynamicColor)
    {
        /* Create form for Add and Delete buttons */
        n = 0;
        XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);  n++;
        XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);  n++;
        XtSetArg(args[n], XmNleftWidget, paletteList);  n++;
        XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);  n++;
        XtSetArg(args[n], XmNrightOffset, 0);  n++;
        XtSetArg(args[n], XmNhorizontalSpacing, style.horizontalSpacing); n++;
        XtSetArg(args[n], XmNverticalSpacing, style.verticalSpacing); n++;
        XtSetArg(args[n], XmNallowOverlap, False); n++;
        XtSetArg(args[n], XmNchildType, XmWORK_AREA);  n++;
        addDeleteForm = 
            XmCreateForm(colorDialog.palettesForm, "addDeleteForm", args, n);
        XtManageChild(addDeleteForm);

        /* Create Add button */
        n = 0;
        XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);  n++;
        XtSetArg (args[n], XmNtopOffset, ADD_PALETTE_TOP_OFFSET);  n++;
        XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
        XtSetArg (args[n], XmNleftOffset, style.horizontalSpacing);  n++;
        XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);  n++;
        string = CMPSTR(((char *)GETMESSAGE(14, 6, "Add...")));
        XtSetArg (args[n], XmNlabelString, string); n++;
        colorDialog.addPaletteButton =
                XmCreatePushButtonGadget(addDeleteForm, "addPalette", args, n);
        XmStringFree(string);
        XtManageChild(colorDialog.addPaletteButton);
        XtAddCallback(colorDialog.addPaletteButton, XmNactivateCallback, addPaletteCB, 
                            (XtPointer) NULL);

       /* Create Delete button */
        n = 0;
        XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);  n++;
        XtSetArg (args[n], XmNtopWidget, colorDialog.addPaletteButton);  n++;
        XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
        XtSetArg (args[n], XmNleftOffset, style.horizontalSpacing);  n++;
        XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);  n++;
        string = CMPSTR(((char *)GETMESSAGE(14, 7, "Delete...")));
        XtSetArg (args[n], XmNlabelString, string); n++;
        colorDialog.deletePaletteButton =
        XmCreatePushButtonGadget(addDeleteForm,"deletePalette",args,n);
        XmStringFree(string);
        XtManageChild(colorDialog.deletePaletteButton);
        XtAddCallback(colorDialog.deletePaletteButton, XmNactivateCallback, deletePaletteCB,
                               (XtPointer) NULL);
    }
    
/*
**  Create a title box for palette color buttons
*/
    n=0;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg (args[n], XmNtopWidget, colorDialog.paletteTB);  n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);  n++;
    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_NONE);  n++;
    XtSetArg (args[n], XmNmarginWidth, 0);  n++;
    XtSetArg (args[n], XmNmarginHeight, 0);  n++;
    string = CMPSTR(pCurrentPalette->desc);
    XtSetArg (args[n], XmNtitleString, string); n++;
    colorDialog.buttonsTB = _DtCreateTitleBox(colorDialog.colorForm, "ButtonsTB", args, n);
    XtManageChild(colorDialog.buttonsTB);
    XmStringFree(string);

    /* Create a form inside palette buttons titlebox */
    n = 0;
    XtSetArg(args[n], XmNhorizontalSpacing, style.horizontalSpacing); n++;
    XtSetArg(args[n], XmNverticalSpacing, style.verticalSpacing); n++;
    style.buttonsForm = XmCreateForm(colorDialog.buttonsTB, "buttonsForm", args, n);
    XtManageChild(style.buttonsForm);
    
    /* Create Modify... button */
    if(style.dynamicColor)
    {
        n = 0;
        XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);  n++;
        XtSetArg (args[n], XmNtopOffset, style.horizontalSpacing+BORDER_WIDTH);  n++;
        XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);  n++;
        XtSetArg (args[n], XmNleftPosition, ADD_PALETTE_LEFT_POSITION);  n++;
        XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);  n++;
        string = CMPSTR(((char *)GETMESSAGE(14, 8, "Modify...")));
        XtSetArg (args[n], XmNlabelString, string); n++;
        modifyColorButton = 
        XmCreatePushButtonGadget(style.buttonsForm, "modifyColorButton", args, n);
        XmStringFree(string);
        XtManageChild(modifyColorButton);
        XtAddCallback(modifyColorButton, XmNactivateCallback, modifyColorCB, 
                            (XtPointer) NULL);
    }
/*
**  Create a pushbutton for configuring DT colors
*/
    n=0;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg (args[n], XmNtopWidget, colorDialog.buttonsTB);  n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);  n++;
    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);  n++;
    XtSetArg (args[n], XmNnavigationType, XmTAB_GROUP);  n++;
    string = CMPSTR(((char *)GETMESSAGE(14, 40, "Number Of Colors...")));
    XtSetArg (args[n], XmNlabelString, string); n++;
    resourcesPB = XmCreatePushButtonGadget(colorDialog.colorForm, "resourcesPB", args, n);
    XtManageChild(resourcesPB);
    XtAddCallback(resourcesPB, XmNactivateCallback, resourcesCB,
                               (XtPointer) style.colorDialog);
    XmStringFree(string);

    style.count++;
}
 

