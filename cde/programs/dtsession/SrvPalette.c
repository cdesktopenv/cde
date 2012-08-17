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
static char rcsid[] =
  "$TOG: SrvPalette.c /main/13 1998/07/23 18:08:39 mgreess $";
*/
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

/**********************************<+>*************************************
***************************************************************************
**
**  File:        SrvPalette.c
**
**  Project:     HP DT Style Manager , integrated into dtsession 
**
**  Description:
**  -----------
**  This is the main program for the color server portion of the dt session
**  manager.  It:
**             1. Determines the number of color cells for each screen 
**                attached to the server this session manager is running on.
**             2. Reads in resouces for the colorserver on a per screen
**                basis. 
**             3. Allocates pixels either Read/Write or Read Only depending
**                on the resource DynamicColor.
**             4. Handles query's about those allocated pixels through
**                Selections. 
**
*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1990.  All rights are  
**  reserved.  Copying or other reproduction of this program      
**  except for archival purposes is prohibited without prior      
**  written consent of Hewlett-Packard Company.		     
********************************************************************
**
**
**************************************************************************
**********************************<+>*************************************/

#include <ctype.h>

#include <X11/Xatom.h>
#include "Srv.h"
#include "SrvFile_io.h"
#include "SrvPalette.h"

#ifdef sun
#include "OWsync.h"
#else
#include <Dt/DtP.h>
#endif

#define DEFAULT   4
#define SRVBUFSIZE              1024
#define DEFAULT_COLOR_PALETTE "Default.dp"
#define DEFAULT_GRAYSCALE_PALETTE "GrayScale.dp"

#define DtRColorUse         "ColorUse"
#define DtRForegroundColor  "ForegroundColor"
#define DtRShadowPixmaps    "ShadowPixmaps"

#define TYPE_OF_MONITOR  "Type Of Monitor" /* also in dtstyle/ColorMain.c */

/* global color server struct */
ColorSrv colorSrv;


/*************************
 * Color Server Resources
 *************************/
typedef struct {
   int     ColorUse;
   int     ShadowPixmaps;
   int     ForegroundColor;
   Boolean DynamicColor;
   Boolean WriteXrdbColors;
   char    *ColorPalette;
   char    *MonochromePalette;
} Appdata, *AppdataPtr;

static XtResource resources[] = {

    {   "colorUse",
        DtRColorUse,
        DtRColorUse,
        sizeof(int),
        XtOffset(AppdataPtr, ColorUse),
        XmRString,
        "DEFAULT"},

    {   "shadowPixmaps",
        DtRShadowPixmaps,
        DtRShadowPixmaps,
        sizeof(int),
        XtOffset(AppdataPtr, ShadowPixmaps),
        XmRString, 
        "DEFAULT"},

    {   "foregroundColor",
        DtRForegroundColor,
        DtRForegroundColor,
        sizeof(int),
        XtOffset(AppdataPtr, ForegroundColor),
        XmRString, 
        "DYNAMIC"},

    {   "dynamicColor",
        "DynamicColor",
        XmRBoolean,
        sizeof(Boolean),
        XtOffset(AppdataPtr, DynamicColor),
        XmRImmediate, 
        (XtPointer) True},

    {   "writeXrdbColors",
        "WriteXrdbColors",
        XmRBoolean,
        sizeof(Boolean),
        XtOffset(AppdataPtr, WriteXrdbColors),
        XmRImmediate, 
        (XtPointer) True},

    {   "colorPalette",
        "ColorPalette",
        XmRString,
        sizeof(char *),
        XtOffset(AppdataPtr, ColorPalette),
        XmRImmediate,
        (XtPointer)"DEFAULT"},

    {   "monochromePalette",
        "MonochromePalette",
        XmRString,
        sizeof(char *),
        XtOffset(AppdataPtr, MonochromePalette),
        XmRImmediate,
        (XtPointer)"Black.dp"},
};

Appdata pColorSrvRsrc;

/************************************
 * External Interface
 ***********************************/
/* variables */
Widget shell[MAX_NUM_SCREENS];

/********    Static Function Declarations    ********/

static Boolean AllocateColors( 
                        Display *dpy) ;
static char *convert_pixel_set(
                        int typeOfMonitor,
                        ColorSet *color );
static Boolean convert_selection( 
                        Widget w,
                        Atom *selection,
                        Atom *target,
                        Atom *type,
                        XtPointer *value,
                        unsigned long *length,
                        int *format) ;
static void lose_selection( 
                        Widget w,
                        Atom *selection) ;
static int FindMaximumDefault( 
                        Display *dpy,
                        int screen_number) ;
static int FindNumOfPixels( 
                        Display *dpy,
                        int screen_number) ;
static int GetNumOfPixels( 
                        int screen_number) ;
static void MatchAndStore( 
                        Display *dpy,
                        int screen_number,
                        unsigned long *pixels) ;
static Boolean AllocReadWrite( 
                        Display *dpy,
                        int screen_number,
                        int numOfPixels) ;
static void AllocReadOnly( 
                        Display *dpy,
                        int screen_number) ;
static void CvtStringToColorUse( 
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from_val,
                        XrmValue *to_val) ;
static void CvtStringToForegroundColor( 
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from_val,
                        XrmValue *to_val) ;
static void CvtStringToShadowPixmaps( 
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from_val,
                        XrmValue *to_val) ;
static Boolean _DtWmStringsAreEqual( 
                        register char *in_str,
                        register char *test_str) ;
static void SetDefaults( 
                        Display *dpy,
                        int screen_number) ;

static void _DtCacheProperties(	
			Display *dpy,
			Window Win) ;
/********    End Static Function Declarations    ********/

#define MSG1  ((char *)GETMESSAGE(26, 1, "Color Server Warning: couldn't get ownership of the selection: "))
#define MSG2  ((char *)GETMESSAGE(26, 2, "Color Server Warning, losing ownership of the selection: "))
#define MSG2a ((char *)GETMESSAGE(26, 3, "Should never lose the selection."))
#define MSG3  ((char *)GETMESSAGE(26, 4, "Warning, found more pixels then are available."))

/***********************************************************************
 *
 * InitializeDtcolor - calls all the routines which do the initialization
 *      for the color server.
 *
 **********************************************************************/
int 
InitializeDtcolor(
        Display *dpy,
        short sessionType )
{
    int status, screen_number;
    char xrdb_string[100];

  /* find out what type of monitor(s?) is being used */
    status = CheckMonitor(dpy);

    if(status == 0)
    {
      /* Allocate colors for the default palette */
       AllocateColors(dpy);
    }
    else {
       for(screen_number=0;
           screen_number != colorSrv.NumOfScreens;
           screen_number++)
       {
         /* Set disown selections of the pixel set atoms */
          XtDisownSelection(shell[screen_number],
                      colorSrv.XA_CUSTOMIZE[screen_number],
                      CurrentTime);
       }
       return(-1);
    }

    /* don't set resources if writeXrdbColors == false */
    if (!pColorSrvRsrc.WriteXrdbColors)
	return(0);

#ifdef sun
    OWsyncColorResources(dpy, colorSrv.TypeOfMonitor[0], 
			colorSrv.pCurrentPalette[0]->color);
    OWsyncLocaleResources(dpy);
#else
   /* Set the *background: resource 
       What gets set depends on what type of monitor */
     /* For XmCO_HIGH_COLOR the default primary colorset is 4, else 1 */
    if(colorSrv.TypeOfMonitor[0] != XmCO_BLACK_WHITE) 
    {
       int chipnum = colorSrv.TypeOfMonitor[0] == XmCO_HIGH_COLOR ? 4 : 1;
       sprintf(xrdb_string,
                   "*background: #%04X%04X%04X\n*foreground: #%04X%04X%04X\n",
                      colorSrv.pCurrentPalette[0]->color[chipnum].bg.red,
                      colorSrv.pCurrentPalette[0]->color[chipnum].bg.green,
                      colorSrv.pCurrentPalette[0]->color[chipnum].bg.blue,
                      colorSrv.pCurrentPalette[0]->color[chipnum].fg.red,
                      colorSrv.pCurrentPalette[0]->color[chipnum].fg.green,
                      colorSrv.pCurrentPalette[0]->color[chipnum].fg.blue);

    }
     /* For XmCO_BLACK_WHITE the resources depended on whether the default 
        palette is White on Black or Black on White */
    else /* XmCO_BLACK_WHITE */
    {
       if(strcmp(colorSrv.pCurrentPalette[0]->name, W_O_B) == 0 ||
          strcmp(colorSrv.pCurrentPalette[0]->name, B_ONLY) == 0)
       {
          sprintf(xrdb_string,
                   "*background: #000000000000\n*foreground: #FFFFFFFFFFFF\n");
       }
       else
       {
          sprintf(xrdb_string,
                   "*background: #FFFFFFFFFFFF\n*foreground: #000000000000\n");
       }
    }
    /* go merge the xrdb_string into the xrdb */
    _DtAddToResource(dpy, xrdb_string);
#endif    

    _DtCacheProperties(dpy, XtWindow(shell[0])) ;

    return(0);
}

static void
_DtCacheProperties(
        Display *dpy,
        Window win)
{
  Atom pixel_set_atom ;
  struct _palette *palette ;
  int typeOfMonitor;

  int screen_number = 0 ; /*assuming simple case i.e. screen 0 */

  pixel_set_atom = XInternAtom(dpy, XmSPIXEL_SET_PROP, FALSE) ;

  palette = colorSrv.pCurrentPalette[screen_number];
  typeOfMonitor = colorSrv.TypeOfMonitor[screen_number];
     if (palette->converted == NULL)
     {
       palette->converted = convert_pixel_set(typeOfMonitor, palette->color);
       palette->converted_len = strlen(palette->converted);
     }
  
  *(palette->converted + palette->converted_len) = XmPIXEL_SET_PROP_VERSION ;
  palette->converted_len++ ;
  palette->converted[palette->converted_len] = 0 ;
  XChangeProperty(dpy, win, pixel_set_atom, XA_STRING, 8, PropModeAppend, 
		 (unsigned char *) XtNewString(palette->converted), 
		 palette->converted_len) ;
}

/*****************************************************************************
**
**  Allocates color cells to be used by clients.  The global varible 
**  DynamicColor[screen_number] determines if the cells are to be allocated
**  read/write or read only.  Right now this routine allocates
**  all cells needed for a palette up front.  For performance tuning we will
**  want to look at allocating on demand.  The allocation scheme looks like
**  the following:  (AT - Alway True)
**  #  TypeOfMonior  UsePixmaps     FgColor     # of Cells allocated per palette
**  -  ------------  ----------     ------      -------------------------------
**  1   HIGH_COLOR    FALSE         DYNAMIC      (fg,bg,ts,bs,sc) 5*8 = 40
**  2   HIGH_COLOR    FALSE      BLACK or WHITE  (bg,ts,bs,sc)    4*8 = 32
**  3   HIGH_COLOR    TRUE          DYNAMIC      (fg,bg,sc)       3*8 = 24
**  4   HIGH_COLOR    TRUE       BLACK or WHITE  (bg,sc)          2*8 = 16
**
**  5   MEDIUM_COLOR  FALSE         DYNAMIC      (fg,bg,ts,bs,sc) 5*4 = 20
**  6   MEDIUM_COLOR  FALSE      BLACK or WHITE  (bg,ts,bs,sc)    4*4 = 16
**  7   MEDIUM_COLOR  TRUE          DYNAMIC      (fg,bg,sc)       3*4 = 12
**  8   MEDIUM_COLOR  TRUE       BLACK or WHITE  (bg,sc)          2*4 = 8
**
**  9   LOW_COLOR     FALSE         DYNAMIC      (fg,bg,ts,bs,sc) 5*2 = 10
** 10   LOW_COLOR     FALSE      BLACK or WHITE  (bg,ts,bs,sc)    4*2 = 8
** 11   LOW_COLOR     TRUE          DYNAMIC      (fg,bg,sc)       3*2 = 6
** 12   LOW_COLOR     TRUE       BLACK or WHITE  (bg,sc)          2*2 = 4
**
** 13   BLACK_WHITE    AT        Aways opposite                         0
**                                 of Bg
**
***************************************************************************/
static Boolean 
AllocateColors(
        Display *dpy )
{
    int             screen_number;
    int             numOfPixels;

    /* Determine how many pixels to allocate (numOfPixels) */
    for(screen_number=0;screen_number != colorSrv.NumOfScreens;screen_number++)
    {
       numOfPixels = GetNumOfPixels(screen_number);
 
   /* Now allocate the correct number of pixels using numOfPixels */
       if(numOfPixels != 0)  /* Not XmCO_BLACK_WHITE */
       {
          if(colorSrv.DynamicColor[screen_number] == True)
          {
            /* go allocate Read/Write cells for the color server */
             if(!AllocReadWrite(dpy, screen_number, numOfPixels))
                return(False);
          }
          else
            /* go allocate Read Only cells for the color server */
             AllocReadOnly(dpy, screen_number);
       } 

       if(colorSrv.TypeOfMonitor[screen_number] == XmCO_BLACK_WHITE)
       {
#define BlackColorSet(dpy,scr,xcolor) \
	(xcolor).pixel = BlackPixel((dpy),(scr)); \
	(xcolor).red = (xcolor).green = (xcolor).blue = 0;
#define WhiteColorSet(dpy,scr,xcolor) \
	(xcolor).pixel = WhitePixel((dpy),(scr)); \
	(xcolor).red = (xcolor).green = (xcolor).blue = 65535;

	 palettes *palette = colorSrv.pCurrentPalette[screen_number];

         /* Check to see what black and white palette it is */
	 /* note: color[0] = secondary, color[1] = primary (as of 8/8/90) */
          if(!(strcmp(colorSrv.pCurrentPalette[screen_number]->name, W_O_B)))
          {
	     WhiteColorSet(dpy,screen_number,palette->color[0].bg);
	     BlackColorSet(dpy,screen_number,palette->color[0].fg);
	     BlackColorSet(dpy,screen_number,palette->color[0].ts);
	     BlackColorSet(dpy,screen_number,palette->color[0].bs);
	     WhiteColorSet(dpy,screen_number,palette->color[0].sc);

	     BlackColorSet(dpy,screen_number,palette->color[1].bg);
	     WhiteColorSet(dpy,screen_number,palette->color[1].fg);
	     WhiteColorSet(dpy,screen_number,palette->color[1].ts);
	     WhiteColorSet(dpy,screen_number,palette->color[1].bs);
	     BlackColorSet(dpy,screen_number,palette->color[1].sc);
          }
          else 
          if(!(strcmp(colorSrv.pCurrentPalette[screen_number]->name, B_O_W)))
          {
	     BlackColorSet(dpy,screen_number,palette->color[0].bg);
	     WhiteColorSet(dpy,screen_number,palette->color[0].fg);
	     WhiteColorSet(dpy,screen_number,palette->color[0].ts);
	     WhiteColorSet(dpy,screen_number,palette->color[0].bs);
	     BlackColorSet(dpy,screen_number,palette->color[0].sc);

	     WhiteColorSet(dpy,screen_number,palette->color[1].bg);
	     BlackColorSet(dpy,screen_number,palette->color[1].fg);
	     BlackColorSet(dpy,screen_number,palette->color[1].ts);
	     BlackColorSet(dpy,screen_number,palette->color[1].bs);
	     WhiteColorSet(dpy,screen_number,palette->color[1].sc);
          }
          else 
          if(!(strcmp(colorSrv.pCurrentPalette[screen_number]->name, W_ONLY)))
          {
	     WhiteColorSet(dpy,screen_number,palette->color[0].bg);
	     BlackColorSet(dpy,screen_number,palette->color[0].fg);
	     BlackColorSet(dpy,screen_number,palette->color[0].ts);
	     BlackColorSet(dpy,screen_number,palette->color[0].bs);
	     WhiteColorSet(dpy,screen_number,palette->color[0].sc);

	     WhiteColorSet(dpy,screen_number,palette->color[1].bg);
	     BlackColorSet(dpy,screen_number,palette->color[1].fg);
	     BlackColorSet(dpy,screen_number,palette->color[1].ts);
	     BlackColorSet(dpy,screen_number,palette->color[1].bs);
	     WhiteColorSet(dpy,screen_number,palette->color[1].sc);
          }
          else  /* black only */
          {
	     BlackColorSet(dpy,screen_number,palette->color[0].bg);
	     WhiteColorSet(dpy,screen_number,palette->color[0].fg);
	     WhiteColorSet(dpy,screen_number,palette->color[0].ts);
	     WhiteColorSet(dpy,screen_number,palette->color[0].bs);
	     BlackColorSet(dpy,screen_number,palette->color[0].sc);

	     BlackColorSet(dpy,screen_number,palette->color[1].bg);
	     WhiteColorSet(dpy,screen_number,palette->color[1].fg);
	     WhiteColorSet(dpy,screen_number,palette->color[1].ts);
	     WhiteColorSet(dpy,screen_number,palette->color[1].bs);
	     BlackColorSet(dpy,screen_number,palette->color[1].sc);
          }
#undef BlackColorSet
#undef WhiteColorSet
       }
     XSync(dpy, 0);
   } /* for screen_number=0 ; screen_number < NumOfScreens; screen_number++ */
  
   return(True);
}

/************************************************************************
**
** CheckMonitor - check to determine which type of monitor each of the
**                screens on the server is running on.
**
************************************************************************/
int 
CheckMonitor(
        Display *dpy )
{
    int n, screen_number, result;
    Arg args[4];
    char screenStr[5], cust_msg[24];
    char *tmpStr;
    char            tmpPalette[SRVBUFSIZE];
    char            *token1;
    char 	    *xrdb_string;

    Widget mainShell;
    XtAppContext app_context;
    
    /* Determine the number of screens attached to this server */
    colorSrv.NumOfScreens = ScreenCount(dpy);

   /* Initialize the Atoms used to pass information */
    colorSrv.XA_PIXEL_SET = XInternAtom(dpy, XmSPIXEL_SET, FALSE);
    colorSrv.XA_TYPE_MONITOR = XInternAtom(dpy, TYPE_OF_MONITOR, FALSE);

   /* create a top level shell to retrieve subresources from */
    n = 0;
    XtSetArg(args[n], XmNbackground, 
        BlackPixelOfScreen(DefaultScreenOfDisplay(dpy))); n++;
    XtSetArg(args[n], XmNmappedWhenManaged, False); n++;
    XtSetArg (args[n], XmNwidth, 1); n++;
    XtSetArg (args[n], XmNheight, 1); n++;
    mainShell = XtAppCreateShell("dtsession", XmSCOLOR_SRV_NAME,
                                  applicationShellWidgetClass, 
                                  dpy, args, n);

   /* create an application context */
    app_context = XtWidgetToApplicationContext(mainShell);
    
   /* Register the resource converters */
    XtAppAddConverter(app_context, XmRString, "ColorUse", 
            CvtStringToColorUse, NULL, 0);
    XtAppAddConverter(app_context, XmRString, "ForegroundColor", 
            CvtStringToForegroundColor, NULL, 0);
    XtAppAddConverter(app_context, XmRString, "ShadowPixmaps", 
            CvtStringToShadowPixmaps, NULL, 0);

   /* cycle through each screen */
    for(screen_number=0;screen_number != colorSrv.NumOfScreens;screen_number++)
    {
       sprintf(screenStr,"%d",screen_number);
       n = 0;
       XtSetArg(args[n], XmNbackground, 
           BlackPixelOfScreen(DefaultScreenOfDisplay(dpy))); n++;
       XtSetArg(args[n], XmNmappedWhenManaged, False); n++;
       XtSetArg (args[n], XmNwidth, 1); n++;
       XtSetArg (args[n], XmNheight, 1); n++;
       shell[screen_number] = XtAppCreateShell(screenStr, XmSCOLOR_SRV_NAME, 
                                               applicationShellWidgetClass, 
                                               dpy, args, n);

       /* 
	* widget needs to be realized for the window ID for 
	* selections to work 
	*/
       
       XtRealizeWidget(shell[screen_number]);
       
       sprintf(cust_msg,"%s%d", XmSCUSTOMIZE_DATA, screen_number);
       colorSrv.XA_CUSTOMIZE[screen_number] = 
	   XInternAtom(dpy, cust_msg, FALSE);
       
       /* go set ownership of the pixel set atoms */
       result = XtOwnSelection(shell[screen_number],
			       colorSrv.XA_CUSTOMIZE[screen_number],
			       CurrentTime, convert_selection, 
			       lose_selection, NULL);
  
       if(result == False)
       {
	   /*
	    * Don't forget to add length for the extra characters.
	    */
	   tmpStr = (char *)SRV_MALLOC(strlen(MSG1) + 25 + 5 + 1 + 1);
	   sprintf(tmpStr,"%s colorSrv.XA_CUSTOMIZE[%d].\n", 
		   MSG1, screen_number);
	   _DtSimpleError(XmSCOLOR_SRV_NAME, DtWarning, NULL, tmpStr, NULL);
	   SRV_FREE(tmpStr);
	   return(-1);
       }

      /* Get the colorserver resources for this screen */

       XtGetSubresources(mainShell, &pColorSrvRsrc, screenStr, screenStr,
                          resources, XtNumber(resources), NULL, 0);

      /* 
       * Set TypeOfMonitor, UsePixmaps FgColor and 
       * DynamicColor for this screen
       */

       SetDefaults(dpy, screen_number);
       
       if (colorSrv.TypeOfMonitor[screen_number] != XmCO_BLACK_WHITE)
       {
          colorSrv.pCurrentPalette[screen_number] = 
                (struct _palette *) GetPaletteDefinition(dpy, 
                                     screen_number, 
                                     pColorSrvRsrc.ColorPalette);
       }
       else
       {
           /* Allocate space for new palette. */
           colorSrv.pCurrentPalette[screen_number] =
               (struct _palette *) SRV_MALLOC( sizeof(struct _palette) + 1 );

           /*  allocate enough space for the name */
           strcpy(tmpPalette, pColorSrvRsrc.MonochromePalette); 
           for (token1=tmpPalette; *token1; token1++);
           while (token1!=tmpPalette && *token1!='.') token1--;
	   if (!strcmp(token1,PALETTE_SUFFIX)) *token1 = '\0';
           colorSrv.pCurrentPalette[screen_number]->name = 
               (char *)SRV_MALLOC(strlen(tmpPalette) + 1);
           strcpy(colorSrv.pCurrentPalette[screen_number]->name,
                  (char *) tmpPalette);
           colorSrv.pCurrentPalette[screen_number]->converted=NULL;
       }

       if (colorSrv.pCurrentPalette[screen_number] == (struct _palette *) NULL)
       {
           return(-1);
       }

      /* write out the color or monochrome palette resource for the screen */

       xrdb_string = XtMalloc(BUFSIZ);

       if (colorSrv.TypeOfMonitor[0] == XmCO_HIGH_COLOR || 
           colorSrv.TypeOfMonitor[0] == XmCO_MEDIUM_COLOR ||
           colorSrv.TypeOfMonitor[0] == XmCO_LOW_COLOR)
       {
           sprintf(xrdb_string, "*%d*ColorPalette: %s%s\n",
                   screen_number,
		   colorSrv.pCurrentPalette[screen_number]->name,
		   PALETTE_SUFFIX);
       }
       else /* XmCO_BLACK_WHITE */
       {
           sprintf(xrdb_string, "*%d*MonochromePalette: %s%s\n",
                   screen_number,
		   colorSrv.pCurrentPalette[screen_number]->name,
		   PALETTE_SUFFIX);
       }
       _DtAddToResource(dpy, xrdb_string);

       XtFree(xrdb_string);
    
   } /* for each screen */
   return(0);
}

/************************************************************************
**
** convert_pixel_set - converts palette pixel set to selection format
**
************************************************************************/
static char *
convert_pixel_set(
        int typeOfMonitor,
        ColorSet *color )
{
  int i;
  char *converted;
  char *p;
  int colormappingindex;

  const int colormapping [4][XmCO_MAX_NUM_COLORS] = {
    {0, 1, 2, 3, 4, 5, 6, 7}, /* XmCO_HIGH_COLOR */
    {0, 1, 2, 3, 1, 1, 2, 1}, /* XmCO_MEDIUM_COLOR */
    {0, 1, 1, 1, 1, 1, 1, 1}, /* XmCO_LOW_COLOR */
    {0, 1, 1, 1, 1, 1, 1, 1}  /* XmCO_BLACK_WHITE */
  };

#if 0 /* map when hi-color was the default */
  const int colormapping [4][XmCO_MAX_NUM_COLORS] = {
    {0, 1, 2, 3, 4, 5, 6, 7}, /* XmCO_HIGH_COLOR */
    {0, 1, 2, 3, 3, 3, 3, 3}, /* XmCO_MEDIUM_COLOR */
    {0, 1, 1, 0, 0, 0, 0, 0}, /* XmCO_LOW_COLOR */
    {0, 1, 1, 0, 0, 0, 0, 0}  /* XmCO_BLACK_WHITE */
  };
#endif

  switch(typeOfMonitor)
  {
    case XmCO_HIGH_COLOR:   colormappingindex = 0; break;
    case XmCO_MEDIUM_COLOR: colormappingindex = 1; break;
    case XmCO_LOW_COLOR:    colormappingindex = 2; break;
    case XmCO_BLACK_WHITE:  colormappingindex = 3; break;
  } 

  p = converted = (char *)SRV_MALLOC(400);
 
  /* lead the string with the type of monitor */
  p += sprintf(p, "%x_", typeOfMonitor);

  for (i = 0; i < NUM_OF_COLORS; i++)
  {
    p += sprintf (p, "%lx_%lx_%lx_%lx_%lx_", 
                  color[colormapping[colormappingindex][i]].bg.pixel,
                  color[colormapping[colormappingindex][i]].fg.pixel,
                  color[colormapping[colormappingindex][i]].ts.pixel,
                  color[colormapping[colormappingindex][i]].bs.pixel,
                  color[colormapping[colormappingindex][i]].sc.pixel);
  }
 
  return(converted);
}

/************************************************************************
**
** convert_selection - Callback, called when some other client wishes
**        to get information from the dtcolor (color server)
**
************************************************************************/
static Boolean 
convert_selection(
        Widget w,
        Atom *selection,
        Atom *target,
        Atom *type,
        XtPointer *value,
        unsigned long *length,
        int *format )
{
  char pixels[50];
  int i, screen_number;
  char *temp;
  char *str_type_return;
  XrmValue value_return;
  XrmValue    cvt_value;
  XrmDatabase db;
  char instanceName[30], instanceClass[30];
  Boolean status;
  struct _palette *palette;
  int typeOfMonitor;
                    

  /* Determine for which screen the selection came from */
  for(i=0; i < MAX_NUM_SCREENS; i++)
  {
     if(colorSrv.XA_CUSTOMIZE[i] == *selection)
     {
         screen_number = i;
         break;
     }
  }

  palette = colorSrv.pCurrentPalette[screen_number];
  typeOfMonitor = colorSrv.TypeOfMonitor[screen_number];

  if(*target == colorSrv.XA_PIXEL_SET)
  {
    /* wants to know the pixels allocated for the palette */

     if (palette->converted == NULL)
     {
       palette->converted = convert_pixel_set(typeOfMonitor, palette->color);
       palette->converted_len = strlen(palette->converted);
     }

     *type   = XA_STRING;
     *length = palette->converted_len;
     *value = XtNewString(palette->converted);
     *format = 8;
     return TRUE;
  }
  else if(*target == colorSrv.XA_TYPE_MONITOR)
  {
    /* wants to know ColorUse, ShadowPixmaps, ForegroundColor,
       and DynamicColor */
     *type   = XA_STRING;
     temp = (char *)SRV_MALLOC(20);
     sprintf (temp, "%x_%x_%x_%x", colorSrv.TypeOfMonitor[screen_number],
                                colorSrv.UsePixmaps[screen_number],
                                colorSrv.FgColor[screen_number],
                                colorSrv.DynamicColor[screen_number]);
     *length = strlen(temp);
     *value = temp;
     *format = 8;
     return TRUE;
  }
  else
     return FALSE;
}

/************************************************************************
**
** lose_selection - Callback, called when some other client wishes
**        to take ownership of one of the servers selections ... 
**        should never happen.
**
************************************************************************/
static void 
lose_selection(
        Widget w,
        Atom *selection )
{
     char *tmpStr, *tmpStr2, *tmpStr3;

     Atom pixel_set_atom ;
     pixel_set_atom = XInternAtom(XtDisplay(w), XmSPIXEL_SET_PROP, FALSE) ;
     XDeleteProperty(XtDisplay(w), XtWindow(shell[0]), pixel_set_atom) ;

     tmpStr = (char *)SRV_MALLOC(strlen(MSG2) + strlen(MSG2a) +  6);
     tmpStr2 = (char *)SRV_MALLOC(strlen(MSG2) + 1);
     if (selection != NULL) {
          tmpStr3 = XGetAtomName(XtDisplay(w), *selection);
     } else {
          tmpStr3 = NULL;
     }
     printf(tmpStr2,"%s", MSG2);
     sprintf(tmpStr,"%s%s%s", tmpStr2, (tmpStr3 == NULL) ? "(null)" : tmpStr3, MSG2a);
     _DtSimpleError(XmSCOLOR_SRV_NAME, DtWarning, NULL, tmpStr, NULL);
     SRV_FREE(tmpStr);
     SRV_FREE(tmpStr2);
}

/*****************************************************************************
**
** FindMaximumDefault .. used when the actual or user defaults for
** TypeOfMonitor, UsePixmaps, and FgColor try to allocate more cells than
** are available .. this finds and allocates the maximum that are available
** It also adjusts TypeOfMonitor, UsePixmaps, and FgColor accordingly.
**
******************************************************************************/ 
static int 
FindMaximumDefault(
        Display *dpy,
        int screen_number )
{
   int numOfPixelsLeft;

   /* go find the Number of pixels left to allocate */
   numOfPixelsLeft = FindNumOfPixels(dpy, screen_number);

   if(numOfPixelsLeft < 4)
   {
     /* Use Black and White */
      colorSrv.TypeOfMonitor[screen_number] = XmCO_BLACK_WHITE;
      return(1);
   }
   if((colorSrv.TypeOfMonitor[screen_number] ==
       XmCO_HIGH_COLOR && numOfPixelsLeft >= 40) ||
      (colorSrv.TypeOfMonitor[screen_number] ==
       XmCO_MEDIUM_COLOR && numOfPixelsLeft >= 20) ||
      (colorSrv.TypeOfMonitor[screen_number] ==
       XmCO_LOW_COLOR && numOfPixelsLeft >= 10))
   {
     /* should never get here */
       return(0);
   }
   else if(colorSrv.TypeOfMonitor[screen_number] == XmCO_HIGH_COLOR)
   {
       if(numOfPixelsLeft >= 32) /* was asking for 40 */ 
       {
          colorSrv.UsePixmaps[screen_number] = FALSE;
          colorSrv.FgColor[screen_number] = WHITE;
          return(32);
       }
       else if(numOfPixelsLeft >= 24)
       {
          colorSrv.UsePixmaps[screen_number] = TRUE;
          colorSrv.FgColor[screen_number] = DYNAMIC;
          return(24);
       } 
       else if(numOfPixelsLeft >= 16)
       {
          colorSrv.UsePixmaps[screen_number] = TRUE;
          colorSrv.FgColor[screen_number] = WHITE;
          return(16);
       } 
       else  /* can't use XmCO_HIGH_COLOR anymore so set to
		next highest XmCO_MEDIUM_COLOR */
       {
          colorSrv.TypeOfMonitor[screen_number] = XmCO_MEDIUM_COLOR;
          colorSrv.pCurrentPalette[screen_number]->num_of_colors = 4;
       }
    }

   /* need to do an if instead of an else because TypeOfMonitor can be reset
      in the else if above */     
    if(colorSrv.TypeOfMonitor[screen_number] == XmCO_MEDIUM_COLOR)
    {
       if(numOfPixelsLeft >= 16)
       {
          colorSrv.UsePixmaps[screen_number] = FALSE;
          colorSrv.FgColor[screen_number] = WHITE;
          return(16);
       }
       if(numOfPixelsLeft >= 12)
       {
          colorSrv.UsePixmaps[screen_number] = TRUE;
          colorSrv.FgColor[screen_number] = DYNAMIC;
          return(12);
       }
       else if(numOfPixelsLeft >= 8)
       {
          colorSrv.UsePixmaps[screen_number] = TRUE;
          colorSrv.FgColor[screen_number] = WHITE;
          return(8);
       }
       else /* can't use XmCO_MEDIUM_COLOR anymore so set to next */
	    /* highest XmCO_LOW_COLOR*/ 
       {
          colorSrv.TypeOfMonitor[screen_number] = XmCO_LOW_COLOR;
          colorSrv.pCurrentPalette[screen_number]->num_of_colors = 2;
          SwitchAItoPS(colorSrv.pCurrentPalette[screen_number]);
       }
    }

   /* need to do an if instead of an else because TypeOfMonitor can be reset
      in the else if above */     
    if(colorSrv.TypeOfMonitor[screen_number] == XmCO_LOW_COLOR)
    {
       if(numOfPixelsLeft >= 10)
       {
          colorSrv.UsePixmaps[screen_number] = FALSE;
          colorSrv.FgColor[screen_number] = DYNAMIC;
          return(10);
       }
       else if(numOfPixelsLeft >= 8)
       {
          colorSrv.UsePixmaps[screen_number] = FALSE;
          colorSrv.FgColor[screen_number] = WHITE;
          return(8);
       }
       else if(numOfPixelsLeft >= 6)
       {
          colorSrv.UsePixmaps[screen_number] = TRUE;
          colorSrv.FgColor[screen_number] = DYNAMIC;
          return(6);
       }
       else if(numOfPixelsLeft >= 4)
       {
          colorSrv.UsePixmaps[screen_number] = TRUE;
          colorSrv.FgColor[screen_number] = WHITE;
          return(4);
       }
   }
   /* should never get here */
   return(0);
}

/****************************************************************************
**
** FindNumOfPixels ... routine used to determine the num of allocable cells
** left in the default colormap.  With this number we can determine the
** Maximum default for the user
**
******************************************************************************/
static int 
FindNumOfPixels(
        Display *dpy,
        int screen_number )
{
    unsigned long   *pixels;
    unsigned long   plane_mask;
    int  i, iterations, status;
    int  num_of_pixels, count, base, countdown;
    Colormap colormap;

    colormap = DefaultColormap(dpy, screen_number);

   /* get the total number of cells in this screen */
    num_of_pixels = XDisplayCells(dpy, screen_number);

   /* get the number of iterations to be used .. the number of plane in this
      screen */
    iterations = XDisplayPlanes(dpy, screen_number);

   /* Allocate enough space to store the pixels.  */
    pixels = (unsigned long *)SRV_MALLOC (num_of_pixels * sizeof (unsigned long));

  /* now iterate through until the we know how many cells are available */
    count = num_of_pixels;
    countdown = count;
    base = 0;
    for(i = 0; i < iterations; i++)
    {
       status = XAllocColorCells (dpy, colormap, (Boolean)0, &plane_mask,
                                                   0, pixels, count);

       countdown = countdown / 2;
       if(status == False)
       {
          count = base + countdown;
       }
       else
       {
          XFreeColors(dpy, colormap, pixels, count, (unsigned long)0);
          if(count != num_of_pixels)
          {
             base = count;
             count = base + countdown;
          }
       }
    }
    status = XAllocColorCells (dpy, colormap, (Boolean)0, &plane_mask,
                                                   0, pixels, count);
    if(status == False)
       count--;
    else
       XFreeColors(dpy, colormap, pixels, count, (unsigned long)0);
    SRV_FREE((char *) pixels);
    return(count);
}

/************************************************************************
**
** GetNumOfPixels - returns the number of pixels to allocate based on the
**        resources ColorUse(TypeOfMonitor), ShadowPixmaps(UsePixmaps),
**        and ForegroundColor(FgColor).
**
************************************************************************/
static int 
GetNumOfPixels(
        int screen_number )
{

   if(colorSrv.TypeOfMonitor[screen_number] == XmCO_BLACK_WHITE)
   {
      return(0);
   }
   else  /* non Black and White monitor */
   {
      if(colorSrv.UsePixmaps[screen_number] == FALSE)
      {
         if(colorSrv.FgColor[screen_number] == DYNAMIC) 
         {
            return(colorSrv.pCurrentPalette[screen_number]->num_of_colors * 5);
         }
         else  /*FgColor == BLACK or WHITE ... bg, ts, bs, & sc used */
         {
            return(colorSrv.pCurrentPalette[screen_number]->num_of_colors * 4);
         }
      }
      else  /* UsePixmaps == True */
      {
         if(colorSrv.FgColor[screen_number] == DYNAMIC) /* fg, bg, & sc used */
         {
            return(colorSrv.pCurrentPalette[screen_number]->num_of_colors * 3);
         }
         else  /*FgColor == BLACK or WHITE ... bg & sc used */
         {
            return(colorSrv.pCurrentPalette[screen_number]->num_of_colors * 2);
         }
      }
   }
}

/************************************************************************
**
** MatchAndStore - match the pixels already allocated with the current
**        palettes storage .. then do a Store Colors to set the colors
**        correctly at the X server.
**
************************************************************************/
static void 
MatchAndStore(
        Display *dpy,
        int screen_number,
        unsigned long *pixels )
{
   int i, count = 0;
   struct _palette    *p;
   XColor *xcolor;

   p = colorSrv.pCurrentPalette[screen_number];
   xcolor = (XColor *)SRV_MALLOC (p->num_of_colors * 5 * sizeof (XColor));

   for(i = 0; i < p->num_of_colors; i++) 
   {
      /* Background Pixel */
      p->color[i].bg.pixel = pixels[count];
      p->color[i].bg.flags = DoRed | DoGreen | DoBlue;
      xcolor[count] = p->color[i].bg;
      count++;

      /* SelectColor (ArmColor) Pixel */
      p->color[i].sc.pixel = pixels[count];
      p->color[i].sc.flags = DoRed | DoGreen | DoBlue;
      xcolor[count] = p->color[i].sc;
      count++;

      if(colorSrv.UsePixmaps[screen_number] == FALSE)
      {
         /* TopShadow Pixel */
         p->color[i].ts.pixel = pixels[count];
         p->color[i].ts.flags = DoRed | DoGreen | DoBlue;
         xcolor[count] = p->color[i].ts;
         count++;

         /* BottomShadow Pixel */
         p->color[i].bs.pixel = pixels[count];
         p->color[i].bs.flags = DoRed | DoGreen | DoBlue;
         xcolor[count] = p->color[i].bs;
         count++;
      }
      else  /* colorSrv.UsePixmaps = True */
      {
         /* TopShadow Pixel set to white */
         p->color[i].ts.pixel = WhitePixel(dpy,screen_number);
         p->color[i].ts.flags = 0;

         /* BottomShadow Pixel set to black */
         p->color[i].bs.pixel = BlackPixel(dpy,screen_number);
         p->color[i].bs.flags = 0;
      }
      if(colorSrv.FgColor[screen_number] == DYNAMIC)
      {
         /* Foreground Pixel */
         p->color[i].fg.pixel = pixels[count];
         p->color[i].fg.flags = DoRed | DoGreen | DoBlue;
         xcolor[count] = p->color[i].fg;
         count++;
      }
      else if(colorSrv.FgColor[screen_number] == BLACK)
      {
         /* Foreground Pixel set to BLACK */
         p->color[i].fg.pixel = BlackPixel(dpy,screen_number);
         p->color[i].fg.flags = 0;
      }
      else
      {
         /* Foreground Pixel set to WHITE */
         p->color[i].fg.pixel = WhitePixel(dpy,screen_number);
         p->color[i].fg.flags = 0;
      }
   } /* for */

   if (count > 0)
   {
     XStoreColors(dpy, DefaultColormap(dpy, screen_number), xcolor, count);
   }

   SRV_FREE((char *) xcolor);
}

/************************************************************************
**
** AllocReadWrite - Allocates Read/Write cells for use by the color
**        server.  If the X server can't allocate enough cells (numOfPixels)
**        this routine finds the number of pixels available, and sets
**        the varibles TypeOfMonitor, UsePixmaps, and FgColor accordingly.
**
************************************************************************/
static Boolean 
AllocReadWrite(
        Display *dpy,
        int screen_number,
        int numOfPixels )
{
   char *tmpStr;
   unsigned long *pixels;
   unsigned long plane_mask;
   int status;

  /* Allocate enough space to store the pixels. */
   pixels = (unsigned long *)SRV_MALLOC (numOfPixels * sizeof (unsigned long));

  /* Now actually allocate R/W pixels */
   status = XAllocColorCells (dpy, DefaultColormap(dpy, screen_number),
                              (Boolean)0, &plane_mask, 0, pixels, numOfPixels);

  /* When status is false means the alloc couldn't get all the pixels 
    the user wanted or what the default is .. so lets go find the
    minumum and set up and use that */
   if(status == False)
   {
       SRV_FREE((char *) pixels);
       numOfPixels = FindMaximumDefault(dpy, screen_number);
       if(numOfPixels == 0)
       {
            tmpStr = (char *)SRV_MALLOC(strlen(MSG3) + 6);
            sprintf(tmpStr,"%s%d", MSG3, screen_number);
            _DtSimpleError(XmSCOLOR_SRV_NAME, DtWarning, NULL, tmpStr, NULL);
            SRV_FREE(tmpStr);
            return(False);
       }
       else
       {
           if(colorSrv.TypeOfMonitor[screen_number] != XmCO_BLACK_WHITE)
           {
           /* Allocate enough space to store the pixels. */
              pixels = (unsigned long *)SRV_MALLOC (numOfPixels *
                                            sizeof (unsigned long));

        /* Now actually allocate R/W pixels */
              status = XAllocColorCells(dpy, DefaultColormap(dpy,screen_number),
                            (Boolean)0, &plane_mask, 0, pixels, numOfPixels);

              if(status == False)
              {
                 SRV_FREE((char *) pixels);
                 _DtSimpleError(XmSCOLOR_SRV_NAME, DtWarning, NULL, ((char *)GETMESSAGE(26, 5, "Warning, can't allocate enough pixels.\n")), NULL);
                 return(False);
              }
           }
       }
   }

   if(colorSrv.TypeOfMonitor[screen_number] != XmCO_BLACK_WHITE)
   {
     /* Go match pixels allocated with the colorsets then use store
        XStoreColors to set the RGB values of them */
      MatchAndStore(dpy, screen_number, pixels);

   } 

  /* free the allocated space for pixels */
   SRV_FREE((char *) pixels);
   return(True);
} 

/************************************************************************
**
** AllocReadOnly - Allocates Read Only cells for use by the color
**        server.  If the X server can't allocate the cell it finds the
**        closest approximation to the color of a cell already allocated.
**        Therefore there is no error recorded.  
**
************************************************************************/
static void 
AllocReadOnly(
        Display *dpy,
        int screen_number )
{
   int i;

   for(i=0; i < colorSrv.pCurrentPalette[screen_number]->num_of_colors; i++)
   {
      XAllocColor(dpy, DefaultColormap(dpy, screen_number),
                &(colorSrv.pCurrentPalette[screen_number]->color[i].bg));
      XAllocColor(dpy, DefaultColormap(dpy, screen_number),
                &(colorSrv.pCurrentPalette[screen_number]->color[i].sc));

    /* Check UsePixmaps varible */
      if(colorSrv.UsePixmaps[screen_number] == FALSE)
      {
         XAllocColor(dpy, DefaultColormap(dpy, screen_number),
                     &(colorSrv.pCurrentPalette[screen_number]->color[i].ts));
         XAllocColor(dpy, DefaultColormap(dpy, screen_number),
                     &(colorSrv.pCurrentPalette[screen_number]->color[i].bs));
      }
      else /* colorSrv.UsePixmaps[screen_number] == True */
      {
         colorSrv.pCurrentPalette[screen_number]->color[i].ts.pixel =
                                          WhitePixel(dpy,screen_number);
         colorSrv.pCurrentPalette[screen_number]->color[i].bs.pixel =
                                          BlackPixel(dpy,screen_number);
      }

    /* Check FgColor varible */
      if(colorSrv.FgColor[screen_number] == DYNAMIC)
         XAllocColor(dpy, DefaultColormap(dpy, screen_number),
                     &(colorSrv.pCurrentPalette[screen_number]->color[i].fg));

      else if(colorSrv.FgColor[screen_number] == BLACK)
         colorSrv.pCurrentPalette[screen_number]->color[i].fg.pixel =
                                          BlackPixel(dpy,screen_number);

      else
         colorSrv.pCurrentPalette[screen_number]->color[i].fg.pixel =
                                          WhitePixel(dpy,screen_number);
   }
} 

/*********************************************************************
**
** Converter which converts a string to the ColorUse value 
**
**********************************************************************/
static void 
CvtStringToColorUse(
        XrmValue *args,
        Cardinal *num_args,
        XrmValue *from_val,
        XrmValue *to_val )
{
   char * in_str = (char *) (from_val->addr);
   static int i;

   to_val->size = sizeof (int);
   to_val->addr = (XtPointer) &i;

   if (_DtWmStringsAreEqual (in_str, "high_color"))
      i = XmCO_HIGH_COLOR;
   else if (_DtWmStringsAreEqual (in_str, "medium_color"))
      i = XmCO_MEDIUM_COLOR;
   else if (_DtWmStringsAreEqual (in_str, "low_color"))
      i = XmCO_LOW_COLOR;
   else if (_DtWmStringsAreEqual (in_str, "b_w"))
      i = XmCO_BLACK_WHITE;
   else if (_DtWmStringsAreEqual (in_str, "default"))
      i = DEFAULT;
   else
   {
      to_val->size = 0;
      to_val->addr = NULL;
      XtStringConversionWarning ((char *)from_val->addr, DtRColorUse);
   }
}
/**********************************************************************
**
** Converter which converts a string to the ForegroundColor value 
**
**********************************************************************/
static void 
CvtStringToForegroundColor(
        XrmValue *args,
        Cardinal *num_args,
        XrmValue *from_val,
        XrmValue *to_val )
{
   char * in_str = (char *) (from_val->addr);
   static int i;

   to_val->size = sizeof (int);
   to_val->addr = (XtPointer) &i;

   if (_DtWmStringsAreEqual (in_str, "dynamic"))
      i = DYNAMIC;
   else if (_DtWmStringsAreEqual (in_str, "black"))
      i = BLACK;
   else if (_DtWmStringsAreEqual (in_str, "white"))
      i = WHITE;
   else
   {
      to_val->size = 0;
      to_val->addr = NULL;
      XtStringConversionWarning ((char *)from_val->addr, DtRForegroundColor);
   }
}

/***********************************************************************
**
** Converter which converts a string to the ShadowPixmaps value 
**
***********************************************************************/
static void 
CvtStringToShadowPixmaps(
        XrmValue *args,
        Cardinal *num_args,
        XrmValue *from_val,
        XrmValue *to_val )
{
   char * in_str = (char *) (from_val->addr);
   static int i;

   to_val->size = sizeof (int);
   to_val->addr = (XtPointer) &i;

   if (_DtWmStringsAreEqual (in_str, "true"))
      i = 1;
   else if (_DtWmStringsAreEqual (in_str, "false"))
      i = 0;
   else if (_DtWmStringsAreEqual (in_str, "default"))
      i = -1;
   else
   {
      to_val->size = 0;
      to_val->addr = NULL;
      XtStringConversionWarning ((char *)from_val->addr, DtRShadowPixmaps);
   }
}

/************************************************************************
 *
 *  _DtWmStringsAreEqual
 *      Compare two strings and return true if equal.
 *      The comparison is on lower cased strings.  It is the callers
 *      responsibility to ensure that test_str is already lower cased.
 *
 ************************************************************************/
static Boolean 
_DtWmStringsAreEqual(
        register char *in_str,
        register char *test_str )

{
   register int i;
   register int j;
   i = *in_str;

   for (;;)
   {
      i = *in_str;
      j = *test_str;

#ifdef MULTIBYTE
      if ((mblen(in_str, MB_CUR_MAX) == 1))
          if (isupper (i)) i = tolower (i);
#else
          if (isupper (i)) i = tolower (i);
#endif
      if (i != j) return (False);
      if (i == 0) return (True);

      in_str++;
      test_str++;
   }
}
  
/************************************************************************
 *
 * SetDefaults - set the TypeOfMonitor, UsePixmaps, FgColor, and DynamicColor
 *       for the screen passed in.  Use the resource values, the number of
 *       colors for this screen, and the visual type of the screen to 
 *       determine which values best fit.
 *
 *************************************************************************/
static void 
SetDefaults(
        Display *dpy,
        int screen_number )
{
   int numPlanes;
   Visual *visual;

   /* Initialize colorSrv data for this screen with specified resource values */
   colorSrv.UsePixmaps[screen_number] = pColorSrvRsrc.ShadowPixmaps;
   colorSrv.DynamicColor[screen_number] = pColorSrvRsrc.DynamicColor;

   /* If this is a static color visual class, set DynamicColor to False. */
   visual = XDefaultVisual(dpy, screen_number);


   /* GrayScale and PseudoColor are the only visual types that make */
   /* sense to change dynamically with the current implementation   */
   if ((visual->class != GrayScale) && (visual->class != PseudoColor))
      colorSrv.DynamicColor[screen_number] = False;

   /* if not specified, set ColorPalette default */
   if (strcmp(pColorSrvRsrc.ColorPalette,"DEFAULT") == 0)
   {
      if ((visual->class == GrayScale) || (visual->class == StaticGray))
      {
          pColorSrvRsrc.ColorPalette = 
                XtMalloc(strlen(DEFAULT_GRAYSCALE_PALETTE)+1);
          strcpy(pColorSrvRsrc.ColorPalette, DEFAULT_GRAYSCALE_PALETTE);
      }
      else
      {
          pColorSrvRsrc.ColorPalette = 
                XtMalloc(strlen(DEFAULT_COLOR_PALETTE)+1);
          strcpy(pColorSrvRsrc.ColorPalette, DEFAULT_COLOR_PALETTE);
      }
   }

   numPlanes = XDisplayPlanes(dpy, screen_number);

   if( numPlanes < 3) /* 1 or 2 planes */
   { 
      colorSrv.TypeOfMonitor[screen_number] = XmCO_BLACK_WHITE;
      colorSrv.DynamicColor[screen_number] = False;
   }
   else if( numPlanes == 3 ) /* 3 planes */
   {
      switch(pColorSrvRsrc.ColorUse) {
         case XmCO_LOW_COLOR:
            colorSrv.TypeOfMonitor[screen_number] = XmCO_LOW_COLOR;
            break;
         default:
            colorSrv.TypeOfMonitor[screen_number] = XmCO_BLACK_WHITE;
            colorSrv.DynamicColor[screen_number] = False;
      }
      /* for 3 planes UsePixmaps (ShadowPixmaps) have to be on (TRUE) */
      colorSrv.UsePixmaps[screen_number] = 1;
   }
   else if( numPlanes == 4 ) /* 4 planes */
   {
      switch(pColorSrvRsrc.ColorUse) {
         case XmCO_MEDIUM_COLOR:
           /* for 4 planes ColorUse = Med_color shadowPixmaps have to be True */
            pColorSrvRsrc.ShadowPixmaps = -1;
            colorSrv.TypeOfMonitor[screen_number] = XmCO_MEDIUM_COLOR;
            break;
         case XmCO_BLACK_WHITE:
            colorSrv.TypeOfMonitor[screen_number] = XmCO_BLACK_WHITE;
            colorSrv.DynamicColor[screen_number] = False;
            break;
         default:
            colorSrv.TypeOfMonitor[screen_number] = XmCO_LOW_COLOR;
      }

     /* check to see what type of shadow pixmap to use */
      if(pColorSrvRsrc.ShadowPixmaps == -1)
         colorSrv.UsePixmaps[screen_number] = 1;
   }
   else if( numPlanes == 5 ) /* 5 planes */
   {
      switch(pColorSrvRsrc.ColorUse) {
         case XmCO_HIGH_COLOR:
           /* for 5 planes ColorUse = hi_color shadowPixmaps have to be True */
            pColorSrvRsrc.ShadowPixmaps = -1;
            colorSrv.TypeOfMonitor[screen_number] = XmCO_HIGH_COLOR; 
            break; 
         case XmCO_MEDIUM_COLOR:
            colorSrv.TypeOfMonitor[screen_number] = XmCO_MEDIUM_COLOR;
            break;
         case XmCO_BLACK_WHITE:
            colorSrv.TypeOfMonitor[screen_number] = XmCO_BLACK_WHITE;
            colorSrv.DynamicColor[screen_number] = False;
            break;
         default:
            colorSrv.TypeOfMonitor[screen_number] = XmCO_LOW_COLOR;
      }
   }
   else /* 6 and above planes */
   {
      switch(pColorSrvRsrc.ColorUse) {
         case XmCO_HIGH_COLOR:
            colorSrv.TypeOfMonitor[screen_number] = XmCO_HIGH_COLOR;
            break;
         case XmCO_LOW_COLOR:
            colorSrv.TypeOfMonitor[screen_number] = XmCO_LOW_COLOR;
            break;
         case XmCO_BLACK_WHITE:
            colorSrv.TypeOfMonitor[screen_number] = XmCO_BLACK_WHITE;
            colorSrv.DynamicColor[screen_number] = False;
            break;
         default:
            colorSrv.TypeOfMonitor[screen_number] = XmCO_MEDIUM_COLOR;
      }

     /* check to see what type of shadow pixmap to use */
      if(pColorSrvRsrc.ShadowPixmaps == -1)
         colorSrv.UsePixmaps[screen_number] = 0;
   }

 /* Determine the correct foreground color useage */
   switch(pColorSrvRsrc.ForegroundColor) {
      case BLACK:
         colorSrv.FgColor[screen_number] = BLACK;
         break;
      case WHITE:
         colorSrv.FgColor[screen_number] = WHITE;
         break;
      default:
         colorSrv.FgColor[screen_number] = DYNAMIC;
  }
}
